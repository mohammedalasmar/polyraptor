
#ifndef __INET_RQBASEALG_H
#define __INET_RQBASEALG_H

#include "inet/common/INETDefs.h"

#include "../RQAlgorithm.h"

namespace inet {

namespace raptorq {

/**
 * State variables for RQBaseAlg.
 */
class INET_API RQBaseAlgStateVariables : public RQStateVariables
{
  public:
    RQBaseAlgStateVariables();
    virtual std::string info() const override;
    virtual std::string detailedInfo() const override;

    /// retransmit count
    //@{
    int rexmit_count;    ///< number of retransmissions (=1 after first rexmit)
    simtime_t rexmit_timeout;    ///< current retransmission timeout (aka RTO)
    //@}

    /// persist factor
    //@{
    uint persist_factor;    ///< factor needed for simplified PERSIST timer calculation
    simtime_t persist_timeout;    ///< current persist timeout
    //@}

    /// congestion window
    //@{
    uint32 snd_cwnd;    ///< congestion window
    //@}

    /// round-trip time measurements
    //@{
    uint32 rtseq;    ///< starting sequence number of timed data
    simtime_t rtseq_sendtime;    ///< time when rtseq was sent (0 if RTT measurement is not running)
    //@}

    /// round-trip time estimation (Jacobson's algorithm)
    //@{
    simtime_t srtt;    ///< smoothed round-trip time
    simtime_t rttvar;    ///< variance of round-trip time
    //@}

    /// number of RTOs
    //@{
    uint32 numRtos;    ///< total number of RTOs
    //@}

    /// RFC 3782 variables
    //@{
    uint32 recover;    ///< recover (RFC 3782)
    bool firstPartialACK;    ///< first partial acknowledgement (RFC 3782)
    //@}
};

/**
 * Includes basic RQ algorithms: adaptive retransmission, PERSIST timer,
 * keep-alive, delayed acks -- EXCLUDING congestion control. Congestion
 * control is implemented in subclasses such as RQTahoeAlg or RQRenoAlg.
 *
 * Implements:
 *   - delayed ACK algorithm (RFC 1122)
 *   - Jacobson's and Karn's algorithms for adaptive retransmission
 *   - Nagle's algorithm (RFC 896) to prevent silly window syndrome
 *   - Increased Initial Window (RFC 3390)
 *   - PERSIST timer
 *
 * To be done:
 *   - KEEP-ALIVE timer
 *
 * Note: currently the timers and time calculations are done in double
 * and NOT in Unix (200ms or 500ms) ticks. It's possible to write another
 * RQAlgorithm which uses ticks (or rather, factor out timer handling to
 * separate methods, and redefine only those).
 *
 * Congestion window is set to SMSS when the connection is established,
 * and not touched after that. Subclasses may redefine any of the virtual
 * functions here to add their congestion control code.
 */
class INET_API RQBaseAlg : public RQAlgorithm
{
  protected:
    RQBaseAlgStateVariables *& state;    // alias to RQAlgorithm's 'state'
    bool  isClient;

    bool isReader;
    bool isWriter;

    unsigned int opcode;
//    int numSymbolsToGet;
    int numSymbolsToSend;

    cMessage *rexmitTimer;
    cMessage *persistTimer;
    cMessage *delayedAckTimer;
    cMessage *keepAliveTimer;

    cOutVector *cwndVector;    // will record changes to snd_cwnd
    cOutVector *ssthreshVector;    // will record changes to ssthresh
    cOutVector *rttVector;    // will record measured RTT
    cOutVector *srttVector;    // will record smoothed RTT
    cOutVector *rttvarVector;    // will record RTT variance (rttvar)
    cOutVector *rtoVector;    // will record retransmission timeout
    cOutVector *numRtosVector;    // will record total number of RTOs

  protected:
    /** @name Process REXMIT, PERSIST, DELAYED-ACK and KEEP-ALIVE timers */
    //@{
    virtual void processRexmitTimer(RQEventCode& event);
    virtual void processPersistTimer(RQEventCode& event);
    virtual void processDelayedAckTimer(RQEventCode& event);
    virtual void processKeepAliveTimer(RQEventCode& event);
    //@}

    /**
     * Start REXMIT timer and initialize retransmission variables
     */
    virtual void startRexmitTimer();

    /**
     * Update state vars with new measured RTT value. Passing two simtime_t's
     * will allow rttMeasurementComplete() to do calculations in double or
     * in 200ms/500ms ticks, as needed)
     */
    virtual void rttMeasurementComplete(simtime_t tSent, simtime_t tAcked);

    /**
     * Converting uint32 echoedTS to simtime_t and calling rttMeasurementComplete()
     * to update state vars with new measured RTT value.
     */
    virtual void rttMeasurementCompleteUsingTS(uint32 echoedTS) override;

    /**
     * Send data, observing Nagle's algorithm and congestion window
     */
    virtual bool sendData(bool sendCommandInvoked);

    /** Utility function */
    cMessage *cancelEvent(cMessage *msg) { return conn->getRQMain()->cancelEvent(msg); }

  public:
    /**
     * Ctor.
     */
    RQBaseAlg();

    /**
     * Virtual dtor.
     */
    virtual ~RQBaseAlg();

    /**
     * Create timers, etc.
     */
    virtual void initialize() override;

    virtual void established(bool active) override;

    virtual void connectionClosed() override;

    /**
     * Process REXMIT, PERSIST, DELAYED-ACK and KEEP-ALIVE timers.
     */
    virtual void processTimer(cMessage *timer, RQEventCode& event) override;

    virtual void sendCommandInvoked(cMessage *msg) override;

    virtual void receivedOutOfOrderSegment() override;

    virtual void receiveSeqChanged() override;

    virtual void receivedDataAck(uint32 firstSeqAcked) override;

    virtual void receivedDuplicateAck() override;

    virtual void receivedAckForDataNotYetSent(uint32 seq) override;

    virtual void ackSent() override;

    virtual void dataSent(uint32 fromseq) override;

    virtual void segmentRetransmitted(uint32 fromseq, uint32 toseq) override;

    virtual void restartRexmitTimer() override;

    // MOH added
    virtual void  cancelRexmitTimer()  override;

};

} // namespace RQ

} // namespace inet

#endif // ifndef __INET_RQBASEALG_H

