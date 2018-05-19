//
// Copyright (C) 2013 Maria Fernandez, Carlos Calafate, Juan-Carlos Cano and
// Pietro Manzoni
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#ifndef __INET_RQVEGAS_H
#define __INET_RQVEGAS_H

#include "inet/common/INETDefs.h"

#include "../flavours/RQBaseAlg.h"
#include "../flavours/RQSegmentTransmitInfoList.h"

namespace inet {

namespace raptorq {

/**
 * State variables for RQVegas.
 */
class INET_API RQVegasStateVariables : public RQBaseAlgStateVariables
{
  public:
    RQVegasStateVariables();
    ~RQVegasStateVariables();
    virtual std::string info() const override;
    virtual std::string detailedInfo() const override;

    uint32 v_recoverypoint;
    simtime_t v_cwnd_changed;    // last time cwnd changes because of a rtx.

    simtime_t v_baseRTT;
    simtime_t v_sumRTT;    // sum of rtt's measured within one RTT
    int v_cntRTT;    // # of rtt's measured within one RTT
    uint32 v_begseq;    // register next pkt to be sent,for rtt calculation in receivedDataAck
    simtime_t v_begtime;    // register time for rtt calculation

    simtime_t v_rtt_timeout;    // vegas fine-grained timeout
    simtime_t v_sa;    // average for vegas fine-grained timeout
    simtime_t v_sd;    // deviation for vegas fine-grained timeout

    RQSegmentTransmitInfoList regions;

    uint32 ssthresh;    ///< slow start threshold

    bool v_inc_flag;    // for slow start: "exponential growth only every other RTT"
    bool v_incr_ss;    // to control no incr. cwnd if during slowstart ssthresh has been exceeded before the rtt is over
    int32 v_incr;    // incr/decr
    uint32 v_worried;    // pkts a to retransmit due to vegas fine-grained timeout
};

class INET_API RQVegas : public RQBaseAlg
{
  protected:
    RQVegasStateVariables *& state;    // alias to RQAlgorithm's 'state'

    /** Create and return a RQvegasStateVariables object. */
    virtual RQStateVariables *createStateVariables() override
    {
        return new RQVegasStateVariables();
    }

    /** Utility function to recalculate ssthresh */
    virtual void recalculateSlowStartThreshold();

    /** Redefine what should happen on retransmission */
    virtual void processRexmitTimer(RQEventCode& event) override;

  public:
    /** Ctor */
    RQVegas();

    /** Redefine what should happen when data got acked, to add congestion window management */
    virtual void receivedDataAck(uint32 firstSeqAcked) override;

    /** Redefine what should happen when dupAck was received, to add congestion window management */
    virtual void receivedDuplicateAck() override;

    /** Called after we send data */
    virtual void dataSent(uint32 fromseq) override;

    virtual void segmentRetransmitted(uint32 fromseq, uint32 toseq) override;
};

} // namespace raptorq

} // namespace inet

#endif // ifndef __INET_RQVEGAS_H

