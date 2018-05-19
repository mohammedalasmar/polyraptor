//
// Copyright (C) 2004 Andras Varga
// Copyright (C) 2010-2011 Zoltan Bojthe
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

#ifndef __INET_RQ_H
#define __INET_RQ_H

#include <map>
#include <set>

#include "inet/common/INETDefs.h"

#include "inet/common/lifecycle/ILifecycle.h"
#include "inet/networklayer/common/L3Address.h"
#include "../contract/raptorq/RQCommand_m.h"
//#include "../../FatTreeRQ/CentralScheduler.h"

namespace inet {

namespace raptorq {

// Forward declarations:
class RQConnection;
class RQSegment;
class RQSendQueue;
class RQReceiveQueue;

/**
 * Implements the RQ protocol. This section describes the internal
 * architecture of the RQ model.
 *
 * Usage and compliance with various RFCs are discussed in the corresponding
 * NED documentation for RQ. Also, you may want to check the RQSocket
 * class which makes it easier to use RQ from applications.
 *
 * The RQ protocol implementation is composed of several classes (discussion
 * follows below):
 *  - RQ: the module class
 *  - RQConnection: manages a connection
 *  - RQSendQueue, RQReceiveQueue: abstract base classes for various types
 *    of send and receive queues
 *  - RQVirtualDataSendQueue and TCPVirtualDataRcvQueue which implement
 *    queues with "virtual" bytes (byte counts only)
 *  - RQAlgorithm: abstract base class for RQ algorithms, and subclasses:
 *    DumbRQ, RQBaseAlg, RQTahoeRenoFamily, RQTahoe, RQReno, RQNewReno.
 *
 * RQ subclassed from cSimpleModule. It manages socketpair-to-connection
 * mapping, and dispatches segments and user commands to the appropriate
 * RQConnection object.
 *
 * RQConnection manages the connection, with the help of other objects.
 * RQConnection itself implements the basic RQ "machinery": takes care
 * of the state machine, stores the state variables (TCB), sends/receives
 * SYN, FIN, RST, ACKs, etc.
 *
 * RQConnection internally relies on 3 objects. The first two are subclassed
 * from RQSendQueue and RQReceiveQueue. They manage the actual data stream,
 * so RQConnection itself only works with sequence number variables.
 * This makes it possible to easily accomodate need for various types of
 * simulated data transfer: real byte stream, "virtual" bytes (byte counts
 * only), and sequence of cMessage objects (where every message object is
 * mapped to a RQ sequence number range).
 *
 * Currently implemented send queue and receive queue classes are
 * RQVirtualDataSendQueue and RQVirtualDataRcvQueue which implement
 * queues with "virtual" bytes (byte counts only).
 *
 * The third object is subclassed from RQAlgorithm. Control over
 * retransmissions, congestion control and ACK sending are "outsourced"
 * from RQConnection into RQAlgorithm: delayed acks, slow start, fast rexmit,
 * etc. are all implemented in RQAlgorithm subclasses. This simplifies the
 * design of RQConnection and makes it a lot easier to implement new RQ
 * variations such as NewReno, Vegas or LinuxRQ as RQAlgorithm subclasses.
 *
 * Currently implemented RQAlgorithm classes are RQReno, RQTahoe, TCPNewReno,
 * RQNoCongestionControl and DumbRQ.
 *
 * The concrete RQAlgorithm class to use can be chosen per connection (in OPEN)
 * or in a module parameter.
 */
class INET_API RaptorQ : public cSimpleModule, public ILifecycle
{
  public:
    struct AppConnKey    // XXX this class is redundant since connId is already globally unique
    {
        int appGateIndex;
        int connId;

        inline bool operator<(const AppConnKey& b) const
        {
            if (appGateIndex != b.appGateIndex)
                return appGateIndex < b.appGateIndex;
            else
                return connId < b.connId;
        }
    };

    struct SockPair
    {
        L3Address localAddr;
        L3Address remoteAddr;
        int localPort;    // -1: unspec
        int remotePort;    // -1: unspec


        inline bool operator<(const SockPair& b) const
        {
            if (remoteAddr != b.remoteAddr)
                return remoteAddr < b.remoteAddr;
            else if (localAddr != b.localAddr)
                return localAddr < b.localAddr;
            else if (remotePort != b.remotePort)
                return remotePort < b.remotePort;
            else
                return localPort < b.localPort;
        }
    };

    // ^^^^^^^^^^^^^^^^^ Multicast
    struct SockPairMulticast
    {
        L3Address localAddr;
//        L3Address remoteAddr;
        int localPort;    // -1: unspec
//        int remotePort;    // -1: unspec

         int multicastGid;    // multicast group id

        inline bool operator<(const SockPairMulticast& b) const
        {
            if (localAddr != b.localAddr)
                return localAddr < b.localAddr;
//            else if (localAddr != b.localAddr)
//                return localAddr < b.localAddr;
//            else if (remotePort != b.remotePort)
//                return remotePort < b.remotePort;
            else
                return localPort < b.localPort;
        }
    };



    cMessage *requestTimerMsg = nullptr;


    // MOH
    std::map<int, int> appGateIndexTimeOutMap;  // moh: contains num of timeouts for each app
    bool test = true;

//    struct connRequests
//    {
////        int appGateIndex;
//        int connId;
//        int numQueuedRequests;
//    };
//    typedef std::map<connRequests,RQConnection *> ConnRequestMap;
//    ConnRequestMap conniRequestMap;

//    std::map<int,int> connRequestMap;

    std::map< int,  RQConnection * > requestCONNMap;
    int connIndex = 0;

   int counter=0;
   int timeOut = 0;
   int times=0;
   bool nap=false;



   struct connInfoMultiSrc{
       unsigned int multiSrcGoupId;
       unsigned int connectionIndex;
       RQConnection *rConn;

   };
typedef std::list<connInfoMultiSrc>  ConnInfoMultiSrcList;
ConnInfoMultiSrcList connInfoMultiSrcList;

  protected:
    typedef std::map<AppConnKey, RQConnection *> RqAppConnMap;
    typedef std::map<SockPair, RQConnection *> RqConnMap;

    RqAppConnMap rqAppConnMap;
    RqConnMap rqConnMap;
    cOutVector requestTimerStamps;

    ushort lastEphemeralPort = (ushort)-1;
    std::multiset<ushort> usedEphemeralPorts;

    // ^^^^^^^^^^^^^^^^^ Multicast
    typedef std::map<SockPairMulticast, RQConnection *> RqConnMapMulticast;
    RqConnMapMulticast rqConnMapMulticast;




  protected:
    /** Factory method; may be overriden for customizing rq */
    virtual RQConnection *createConnection(int appGateIndex, int connId);

    // utility methods
    virtual RQConnection *findConnForSegment(RQSegment *rqseg, L3Address srcAddr, L3Address destAddr);
    virtual RQConnection *findConnForSegmentMulticast(RQSegment *rqseg,   L3Address destAddr ,  unsigned int multicastGid);



    virtual RQConnection *findConnForApp(int appGateIndex, int connId);

    virtual void segmentArrivalWhileClosed(RQSegment *rqseg, L3Address src, L3Address dest);
    virtual void removeConnection(RQConnection *conn);
    virtual void updateDisplayString();

  public:
    static bool testing;    // switches between tcpEV and testingEV
    static bool logverbose;    // if !testing, turns on more verbose logging

    bool recordStatistics = false;    // output vectors on/off
    bool isOperational = false;    // lifecycle: node is up/down

    bool useDataNotification = false;

  public:
    RaptorQ() {}
    virtual ~RaptorQ();

  protected:
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;

  public:
    /**
     * To be called from RQConnection when a new connection gets created,
     * during processing of OPEN_ACTIVE or OPEN_PASSIVE.
     */
    virtual void addSockPair(RQConnection *conn, L3Address localAddr, L3Address remoteAddr, int localPort, int remotePort);
    virtual void  addMulticastSockPair(RQConnection *conn, L3Address localAddr,  int localPort,  int multicastGid) ;
    /**
     * To be called from RQConnection when socket pair (key for rqConnMap) changes
     * (e.g. becomes fully qualified).
     */
    virtual void updateSockPair(RQConnection *conn, L3Address localAddr, L3Address remoteAddr, int localPort, int remotePort);

    /**
     * Update conn's socket pair, and register newConn (which'll keep LISTENing).
     * Also, conn will get a new connId (and newConn will live on with its old connId).
     */
    virtual void addForkedConnection(RQConnection *conn, RQConnection *newConn, L3Address localAddr, L3Address remoteAddr, int localPort, int remotePort);

    /**
     * To be called from RQConnection: reserves an ephemeral port for the connection.
     */
    virtual ushort getEphemeralPort();

    /**
     * To be called from RQConnection: create a new send queue.
     */
    virtual RQSendQueue *createSendQueue(RQDataTransferMode transferModeP);

    /**
     * To be called from RQConnection: create a new receive queue.
     */
    virtual RQReceiveQueue *createReceiveQueue(RQDataTransferMode transferModeP);

    // ILifeCycle:
    virtual bool handleOperationStage(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback) override;

    // called at shutdown/crash
    virtual void reset();

    virtual void  requestTimer();

    virtual void  cancelRequestTimer();
    virtual bool   getNapState();
    virtual bool allPullQueuesEmpty();
    virtual bool allConnFinished();
    virtual void updateConnMap();
    virtual void  sendFirstRequest();
    virtual void  process_REQUEST_TIMER();
    virtual void   getCentralSchedulerModule(unsigned int groupIndex,std::set<int>& multicastReceivers , std::set<std::string>& nodePodRackLoc, std::vector<int>& receiversPortNumbers );
    virtual void  printConnRequestMap();

    virtual void fillMultiSrcConnectionsInfo(RQConnection *conn, unsigned  int multiSrcGoupId, unsigned int connectionIndex) ;


};

} // namespace raptorq

} // namespace inet

#endif // ifndef __INET_RQ_H

