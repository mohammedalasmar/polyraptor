
#include "RaptorQ.h"

#include "inet/networklayer/common/IPSocket.h"
#include "inet/networklayer/contract/INetworkProtocolControlInfo.h"
#include "inet/networklayer/common/IPProtocolId_m.h"
#include "inet/common/lifecycle/LifecycleOperation.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/lifecycle/NodeOperations.h"
#include "inet/common/lifecycle/NodeStatus.h"
#include "RQConnection.h"
#include "raptorq_common/RQSegment.h"
#include "../contract/raptorq/RQCommand_m.h"

#ifdef WITH_IPv4
#include "inet/networklayer/ipv4/ICMPMessage_m.h"
#endif // ifdef WITH_IPv4

#ifdef WITH_IPv6
#include "inet/networklayer/icmpv6/ICMPv6Message_m.h"
#endif // ifdef WITH_IPv6

#include "queues/RQByteStreamRcvQueue.h"
#include "queues/RQByteStreamSendQueue.h"
#include "queues/RQMsgBasedRcvQueue.h"
#include "queues/RQMsgBasedSendQueue.h"
#include "queues/RQVirtualDataRcvQueue.h"
#include "queues/RQVirtualDataSendQueue.h"
#include "queues/RQRequestsSendQueue.h"
#include "queues/RQSymbolsRcvQueue.h"
#include "inet/raptorqimplementation/FatTreeRQ/CentralScheduler.h"
#include "inet/raptorqimplementation/application/raptorqapp/RQSinkApp.h"

#include "inet/networklayer/ipv4/IPv4RoutingTable.h"
#include "inet/networklayer/common/L3AddressResolver.h"

using namespace std;
namespace inet {

namespace raptorq {

Define_Module(raptorq::RaptorQ);
simsignal_t numRequestsRTOs = raptorq::RaptorQ::registerSignal("numRequestsRTOs");

#define EPHEMERAL_PORTRANGE_START    1024
#define EPHEMERAL_PORTRANGE_END      5000 // mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm multicast index group  ???

static std::ostream& operator<<(std::ostream& os, const RaptorQ::SockPair& sp) {
    os << "loc=" << sp.localAddr << ":" << sp.localPort << " " << "rem="
            << sp.remoteAddr << ":" << sp.remotePort;
    return os;
}

static std::ostream& operator<<(std::ostream& os, const RaptorQ::AppConnKey& app) {
    os << "connId=" << app.connId << " appGateIndex=" << app.appGateIndex;
    return os;
}

static std::ostream& operator<<(std::ostream& os, const RQConnection& conn) {
    os << "connId=" << conn.connId << " "
            << RQConnection::stateName(conn.getFsmState()) << " state={"
            << const_cast<RQConnection&>(conn).getState()->info() << "}";
    return os;
}

void RaptorQ::initialize(int stage) {


    cSimpleModule::initialize(stage);

    // MOH: adding this message here is a big mistake, as it will be initialised 13 times (which is the number of stages, see  InitStages.h)
    // // requestTimerMsg = new cMessage("requestTimerMsg");
   // //  requestTimerMsg->setContextPointer(this);




    if (stage == INITSTAGE_LOCAL) {
        const char *q;
        q = par("sendQueueClass");
        if (*q != '\0')
            throw cRuntimeError(
                    "Don't use obsolete sendQueueClass = \"%s\" parameter", q);

        q = par("receiveQueueClass");
        if (*q != '\0')
            throw cRuntimeError(
                    "Don't use obsolete receiveQueueClass = \"%s\" parameter",
                    q);

        lastEphemeralPort = EPHEMERAL_PORTRANGE_START;
        WATCH(lastEphemeralPort);

        WATCH_PTRMAP(rqConnMap);
        WATCH_PTRMAP(rqAppConnMap);

        requestTimerStamps.setName("requestTimerStamps");

        recordStatistics = par("recordStats");
        useDataNotification = par("useDataNotification");
    }
    else if (stage == INITSTAGE_TRANSPORT_LAYER) {


        requestTimerMsg = new cMessage("requestTimerMsg");
         requestTimerMsg->setContextPointer(this);

        cModule *host = findContainingNode(this);
//        std::cout << "RaptorQ::initialize getFullPath: " << host->getFullPath() << std::endl;

        NodeStatus *nodeStatus = check_and_cast_nullable<NodeStatus *>(host ? host->getSubmodule("status") : nullptr);

        isOperational = (!nodeStatus) || nodeStatus->getState() == NodeStatus::UP;

//        std::cout << "RaptorQ::initialize IPSocket ipSocket(gate(ipOut))" << std::endl;
        IPSocket ipSocket(gate("ipOut"));
        ipSocket.registerProtocol(IP_PROT_TCP);
    }
}

// destructor
RaptorQ::~RaptorQ() {
    while (!rqAppConnMap.empty()) {
        auto i = rqAppConnMap.begin();
        delete i->second;
        rqAppConnMap.erase(i);
    }

}

void RaptorQ::handleMessage(cMessage *msg) {

    if (!isOperational) {
        if (msg->isSelfMessage())
            throw cRuntimeError(
                    "Model error: self msg '%s' received when isOperational is false",
                    msg->getName());
        EV_ERROR << "RQ is turned off, dropping '" << msg->getName()
                        << "' message\n";
        delete msg;
    }

    else if (msg->isSelfMessage()) {
//        std::cout << "isSelfMessage " << msg->getFullPath()  << std::endl;
        if (msg == requestTimerMsg) {
            process_REQUEST_TIMER();
         }
        else{
        RQConnection *conn = (RQConnection *) msg->getContextPointer();
        bool ret = conn->processTimer(msg);
      //  if (!ret)
          //  removeConnection(conn);
        }



    }

    else if (msg->arrivedOn("ipIn")) {
        if (false
#ifdef WITH_IPv4
                || dynamic_cast<ICMPMessage *>(msg)
#endif // ifdef WITH_IPv4
#ifdef WITH_IPv6
                || dynamic_cast<ICMPv6Message *>(msg)
#endif // ifdef WITH_IPv6
                ) {
            EV_DETAIL << "ICMP error received -- discarding\n"; // FIXME can ICMP packets really make it up to RQ???
            delete msg;
        } else {


            // must be a RQSegment
            RQSegment *rqseg = check_and_cast<RQSegment *>(msg);
//            std::cout << "\n\n\n newwww " << rqseg << std::endl;

            // get src/dest addresses
            L3Address srcAddr, destAddr;
            cObject *ctrl = rqseg->removeControlInfo();
            if (!ctrl)  throw cRuntimeError("(%s)%s arrived without control info",  rqseg->getClassName(), rqseg->getName());

            INetworkProtocolControlInfo *controlInfo = check_and_cast<INetworkProtocolControlInfo *>(ctrl);
            srcAddr = controlInfo->getSourceAddress();
            destAddr = controlInfo->getDestinationAddress();
            //interfaceId = controlInfo->getInterfaceId();
            delete ctrl;

            // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
            // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
            // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
            // FOR MULTICAST
            bool  isMultiC = rqseg->getIsMulticastConnSeg() ;
            unsigned int   multiGid = rqseg->getMulticastGroupIndex() ;
            bool  isMulticastSender = rqseg->getIsMulticastSender() ; // rqseg is the received seg, so if this is the receiver node then isSender=true, otherwise false
            bool  isMulticastReceiver = rqseg->getIsMulticastReceiver() ;

            if (isMultiC == true) {
                std::string mod = "centralScheduler";
                cModule *centralMod = getModuleByPath(mod.c_str());
                CentralScheduler *centralSch = check_and_cast<CentralScheduler *>(centralMod);

                std::string multicastSenderName;
                centralSch->getMulticastGrSender(multiGid, multicastSenderName);
//                std::cout << "    ???? multicastSenderName " << multicastSenderName << std::endl;
//                std::cout << "    ???? multicastSenderName multiGid" << multiGid << std::endl;

                if (isMulticastSender == true) { // this is a receiver (as isMulticastSender ios coming from sender), so make src address of the incoming packet as the group sender ip

                    std::string nodeRoutingTable = multicastSenderName + ".routingTable";
                    cModule *routingTableMod = getModuleByPath( nodeRoutingTable.c_str());
                    IPv4RoutingTable *obj = check_and_cast<IPv4RoutingTable *>(  routingTableMod);
                    IPv4Address multicastSenderAdrs = obj->getRouterId();
                    srcAddr = multicastSenderAdrs;
                }

                std::string firstEdge;
                if (isMulticastReceiver == true) { // this is a sender, so make src address of the incoming packet as the sender edge router
                    centralSch->getMulticastGrFirstEdgeDest( multiGid, multicastSenderName ,   firstEdge);

//                    std::string nodeRoutingTable = firstEdge + ".routingTable";
//                    cModule *routingTableMod = getModuleByPath( nodeRoutingTable.c_str());
//                    IPv4RoutingTable *obj = check_and_cast<IPv4RoutingTable *>(  routingTableMod);
//                    IPv4Address multicastSenderAdrs = obj->getRouterId();
//                    srcAddr = multicastSenderAdrs;


                    L3Address ssrr;
                    L3AddressResolver().tryResolve(firstEdge.c_str(), ssrr );
                    srcAddr = ssrr;

//                    std::cout << " MULTICAST  conn ???? srcAddr " << srcAddr << std::endl;
//                    std::cout << " MULTICAST conn ???? src port" << rqseg->getSrcPort() << std::endl;

//                    std::cout << " MULTICAST conn ???? destAddr " << destAddr << std::endl;
//                    std::cout << " MULTICAST conn ???? dest port" << rqseg->getDestPort() << std::endl;

                }
            }

            // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
            // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
            // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
            // FOR MULTI SOURCING
            bool isMultiSrc = rqseg->getIsMultiSourcingConn();
            unsigned int multiGrpId = rqseg->getMultiSourcingGroupIndex();
            bool isMultiSrcSender = rqseg->getIsMultiSourcingSender(); // rqseg is the received seg, so if this is the receiver node then isSender=true, otherwise false
            bool isMultiSrcReceiver = rqseg->getIsMultiSourcingReceiver();

            if (isMultiSrc == true) {
//                std::cout << " MULTI SOURCING 11 srcAddr " << srcAddr   << std::endl;
//                std::cout << " MULTI SOURCING 11 destAddr " << destAddr   << std::endl;


                std::string modCen = "centralScheduler";
                cModule *centralModule = getModuleByPath(modCen.c_str());
                CentralScheduler *centralSchObj = check_and_cast<CentralScheduler *>(centralModule);

                 std::string multiSrcReciverName;
                centralSchObj->getMultiSourcingGrReceiver(multiGrpId, multiSrcReciverName)  ;
//                 std::cout << "    ???? multiSrcReciverName " << multiSrcReciverName << std::endl;
//                std::cout << "    ???? multiSrcReciverName multiGid" << multiGid << std::endl;

//                multiSrcReciverName = multiSrcReciverName

//                multiSrcReciverName = toSt
                if (isMulticastSender == true) { // this is a receiver (as isMulticastSender ios coming from sender), so make src address of the incoming packet as the group sender ip
//                    L3Address ssrr;
//                    L3AddressResolver().tryResolve(multiSrcReciverName.c_str(), ssrr);
//                    srcAddr = ssrr;

//                    std::cout << " MULTI SOURCING 22 srcAddr " << srcAddr   << std::endl;
//                    std::cout << " MULTI SOURCING 22 destAddr " << destAddr   << std::endl;

//                    std::cout << " MULTI SOURCING 22 dest port " << rqseg->getDestPort()   << std::endl;
//                    std::cout << " MULTI SOURCING 22 src port " << rqseg->getSrcPort()    << std::endl;

//                    std::string nodeRoutingTable = multicastSenderName + ".routingTable";
//                    cModule *routingTableMod = getModuleByPath(  nodeRoutingTable.c_str());
//                    IPv4RoutingTable *obj = check_and_cast<IPv4RoutingTable *>( routingTableMod);
//                    IPv4Address multicastSenderAdrs = obj->getRouterId();
//                    srcAddr = multicastSenderAdrs;
                }
//
                  std::string firstEdge;
                if (isMulticastReceiver == true) { // this is a sender, so make src address of the incoming packet as the sender edge router
//                    centralSch->getMulticastGrFirstEdgeDest(multiGid,  multiSrcReciverName, firstEdge);
//
//                    L3Address ssrr;
//                    L3AddressResolver().tryResolve(firstEdge.c_str(), ssrr);
//                    srcAddr = ssrr;
//                    std::cout << " MULTI SOURCING 22 srcAddr " << srcAddr   << std::endl;
//                    std::cout << " MULTI SOURCING 22 destAddr " << destAddr   << std::endl;

//                    std::cout << " MULTI SOURCING 22 dest port " << rqseg->getDestPort()   << std::endl;
//                    std::cout << " MULTI SOURCING 22 src port " << rqseg->getSrcPort()    << std::endl;
//                    std::cout << " MULTI SOURCING conn ???? srcAddr " << srcAddr << std::endl;
//                    std::cout << " MULTI SOURCING conn ???? src port" << rqseg->getSrcPort()  << std::endl;
//
//                    std::cout << " MULTI SOURCING conn ???? destAddr " << destAddr   << std::endl;
//                    std::cout << " MULTI SOURCING conn ???? dest port" << rqseg->getDestPort() << std::endl;
//
                }
            }





            RQConnection *conn = nullptr;
            // process segment
             conn = findConnForSegment(rqseg, srcAddr, destAddr); // this is a receiver

//            if (isMultiC == false) conn = findConnForSegment(rqseg, srcAddr, destAddr);
//             if (isMultiC == true) conn = findConnForSegmentMulticast(rqseg,  destAddr ,multiGid );

//              if (isSender == true) conn = findConnForSegment(rqseg, srcAddr, destAddr); // this is a receiver
//              if (isSender == false) conn = findConnForSegmentMulticast(rqseg,  destAddr ,multiGid ); // this is a sender


            if (conn) {
                bool ret = conn->processRQSegment(rqseg, srcAddr, destAddr);
                 //////////////////////////////

//                requestCONNMap[connIndex]=conn;
//                 ++connIndex;

                if (!ret)
                    removeConnection(conn);
            } else {
                segmentArrivalWhileClosed(rqseg, srcAddr, destAddr);
            }
        }
    }




////////////////must be from app /////////////////////////////////
    else {    // must be from app

//        RQCommand    *cntInfo = check_and_cast<RQCommand *>( msg->getControlInfo());
        RQCommand *controlInfo = check_and_cast<RQCommand *>( msg->getControlInfo());
        bool forcMultiSrcConnClose = controlInfo->getForcMultiSrcConnClose();

        //***************
        if (forcMultiSrcConnClose == false) {
        int appGateIndex = msg->getArrivalGate()->getIndex();
        int connId = controlInfo->getConnId();
        //        std::cout << " ( appGateIndex , connId ) = ( " << appGateIndex << " , " << connId << " ) " << std::endl;
        RQConnection *conn = findConnForApp(appGateIndex, connId);
        //std::cout << "  conn ???? " << conn << std::endl;

        if (!conn) {
            conn = createConnection(appGateIndex, connId);
            // add into appConnMap here; it'll be added to connMap during processing
            // the OPEN command in RQConnection's processAppCommand().
            AppConnKey key;
            key.appGateIndex = appGateIndex;
            key.connId = connId;
            rqAppConnMap[key] = conn;
            //             std::cout << "  RaptorQ::handleMessage RQ connection created for " << msg << std::endl  ;
            EV_INFO << "RQ connection created for " << msg << "\n";
        }
        bool ret = conn->processAppCommand(msg);
        if (!ret)
            removeConnection(conn);
        }
        //***************
        if (forcMultiSrcConnClose == true) {
           unsigned int  multiSourcingGrpId  = controlInfo->getMultiSourcingGrpId();
           std::cout << "allReceivedCloseAllMultiSourcingConns22222 \n\n\n\n" ;

           for (auto iter = connInfoMultiSrcList.begin(); iter!= connInfoMultiSrcList.end() ; ++iter){
              if( iter->multiSrcGoupId == multiSourcingGrpId) {
                  std::cout << "allReceivedCloseAllMultiSourcingConnsaaaa  " <<  iter->multiSrcGoupId << "\n" ;
                  iter->rConn->setConnFinished();
              }
           }





        }







    }// else end must be from app
////////////////end must be from app ////////////////////////////////


    if (hasGUI())
        updateDisplayString();

//    std::cout <<  std::endl<< std::endl<< std::endl;

}


void RaptorQ::fillMultiSrcConnectionsInfo(RQConnection *conn, unsigned  int multiSrcGoupId, unsigned int connectionIndex) {
    connInfoMultiSrc  connInfo ;
    connInfo.connectionIndex = connectionIndex;
    connInfo.multiSrcGoupId = multiSrcGoupId;
    connInfo.rConn=conn;
    connInfoMultiSrcList.push_back(connInfo);

}


RQConnection *RaptorQ::createConnection(int appGateIndex, int connId) {
    return new RQConnection(this, appGateIndex, connId);
}

void RaptorQ::segmentArrivalWhileClosed(RQSegment *rqseg, L3Address srcAddr,
        L3Address destAddr) {
    RQConnection *tmp = new RQConnection();
    tmp->segmentArrivalWhileClosed(rqseg, srcAddr, destAddr);
    delete tmp;
    delete rqseg;
}

void RaptorQ::updateDisplayString() {
#if OMNETPP_VERSION < 0x0500
    if (getEnvir()->isDisabled()) {
#else
    if (getEnvir()->isExpressMode()) {
#endif
        // in express mode, we don't bother to update the display
        // (std::map's iteration is not very fast if map is large)
        getDisplayString().setTagArg("t", 0, "");
        return;
    }

    //char buf[40];
    //sprintf(buf,"%d conns", rqAppConnMap.size());
    //getDisplayString().setTagArg("t",0,buf);
    if (isOperational)
        getDisplayString().removeTag("i2");
    else
        getDisplayString().setTagArg("i2", 0, "status/cross");

    int numINIT = 0, numCLOSED = 0, numLISTEN = 0, numSYN_SENT = 0,
            numSYN_RCVD = 0, numESTABLISHED = 0, numCLOSE_WAIT = 0,
            numLAST_ACK = 0, numFIN_WAIT_1 = 0, numFIN_WAIT_2 = 0, numCLOSING =
                    0, numTIME_WAIT = 0;

    for (auto & elem : rqAppConnMap) {
        int state = (elem).second->getFsmState();

        switch (state) {
        case RQ_S_INIT:
            numINIT++;
            break;

        case RQ_S_CLOSED:
            numCLOSED++;
            break;

        case RQ_S_LISTEN:
            numLISTEN++;
            break;

        case RQ_S_SYN_SENT:
            numSYN_SENT++;
            break;

        case RQ_S_SYN_RCVD:
            numSYN_RCVD++;
            break;

        case RQ_S_ESTABLISHED:
            numESTABLISHED++;
            break;

        case RQ_S_CLOSE_WAIT:
            numCLOSE_WAIT++;
            break;

        case RQ_S_LAST_ACK:
            numLAST_ACK++;
            break;

        case RQ_S_FIN_WAIT_1:
            numFIN_WAIT_1++;
            break;

        case RQ_S_FIN_WAIT_2:
            numFIN_WAIT_2++;
            break;

        case RQ_S_CLOSING:
            numCLOSING++;
            break;

        case RQ_S_TIME_WAIT:
            numTIME_WAIT++;
            break;
        }
    }

    char buf2[200];
    buf2[0] = '\0';

    if (numINIT > 0)
        sprintf(buf2 + strlen(buf2), "init:%d ", numINIT);
    if (numCLOSED > 0)
        sprintf(buf2 + strlen(buf2), "closed:%d ", numCLOSED);
    if (numLISTEN > 0)
        sprintf(buf2 + strlen(buf2), "listen:%d ", numLISTEN);
    if (numSYN_SENT > 0)
        sprintf(buf2 + strlen(buf2), "syn_sent:%d ", numSYN_SENT);
    if (numSYN_RCVD > 0)
        sprintf(buf2 + strlen(buf2), "syn_rcvd:%d ", numSYN_RCVD);
    if (numESTABLISHED > 0)
        sprintf(buf2 + strlen(buf2), "estab:%d ", numESTABLISHED);
    if (numCLOSE_WAIT > 0)
        sprintf(buf2 + strlen(buf2), "close_wait:%d ", numCLOSE_WAIT);
    if (numLAST_ACK > 0)
        sprintf(buf2 + strlen(buf2), "last_ack:%d ", numLAST_ACK);
    if (numFIN_WAIT_1 > 0)
        sprintf(buf2 + strlen(buf2), "fin_wait_1:%d ", numFIN_WAIT_1);
    if (numFIN_WAIT_2 > 0)
        sprintf(buf2 + strlen(buf2), "fin_wait_2:%d ", numFIN_WAIT_2);
    if (numCLOSING > 0)
        sprintf(buf2 + strlen(buf2), "closing:%d ", numCLOSING);
    if (numTIME_WAIT > 0)
        sprintf(buf2 + strlen(buf2), "time_wait:%d ", numTIME_WAIT);

    getDisplayString().setTagArg("t", 0, buf2);
}

RQConnection *RaptorQ::findConnForSegment(RQSegment *rqseg, L3Address srcAddr, L3Address destAddr) {
    SockPair key;
    key.localAddr = destAddr;
     key.remoteAddr = srcAddr;
    key.localPort = rqseg->getDestPort();
    key.remotePort = rqseg->getSrcPort();
    SockPair save = key;

//    std::cout << "   eeee localPort   " <<  key.localPort  << " \n";
//    std::cout << " eee localAddr  " <<  destAddr  << " \n";
//    std::cout << " eee destAddr " <<  srcAddr << " \n";
//    std::cout << " eee remotePort  " <<  key.remotePort  << " \n";

    // try with fully qualified SockPair
    auto i = rqConnMap.find(key);
    if (i != rqConnMap.end())
        return i->second;

    // try with localAddr missing (only localPort specified in passive/active open)
    key.localAddr = L3Address();
    i = rqConnMap.find(key);
    if (i != rqConnMap.end())
        return i->second;

    // try fully qualified local socket + blank remote socket (for incoming SYN)
    key = save;
    key.remoteAddr = L3Address();
    key.remotePort = -1;
    i = rqConnMap.find(key);
    if (i != rqConnMap.end())
        return i->second;

    // try with blank remote socket, and localAddr missing (for incoming SYN)
    key.localAddr = L3Address();
    i = rqConnMap.find(key);
    if (i != rqConnMap.end())
        return i->second;

    // given up
    return nullptr;
}

// ^^^^^^^^^^^^^^^^^^^^^^    Multicast
RQConnection *RaptorQ::findConnForSegmentMulticast(RQSegment *rqseg, L3Address destAddr, unsigned int multicastGid ) {

// std::cout << "  RaptorQ::findConnForSegmentMulticast \n";
//    SockPairMulticast key;
//    key.localAddr = destAddr;  // MOH: arrived packet dest address is the local address at this node
//    key.localPort = rqseg->getDestPort();
//    key.multicastGid = multicastGid ;
//
//    std::cout << " lll  multicastGid " <<  multicastGid << " \n";
//    std::cout << " lll multicakey.localPort   " <<  key.localPort  << " \n";
//    std::cout << " lll localAddr  " <<  destAddr  << " \n";
//
////    key.remoteAddr = srcAddr;
////    key.remotePort = rqseg->getSrcPort();
//    SockPairMulticast save = key;
//
//    // try with fully qualified SockPair
//    auto i = rqConnMapMulticast.find(key);
//    if (i != rqConnMapMulticast.end())
//        return i->second;
//    std::cout << "  RaptorQ::findConnForSegmentMulticast222 \n";
//
//    // try with localAddr missing (only localPort specified in passive/active open)
////    key.localAddr = L3Address();
////    i = rqConnMapMulticast.find(key);
////    if (i != rqConnMapMulticast.end())
////        return i->second;
//    std::cout << "  RaptorQ::findConnForSegmentMulticast333 \n";
//
//    // try fully qualified local socket + blank remote socket (for incoming SYN)
//    key = save;
//    key.localAddr = L3Address();
//    key.localPort = -1;
//    i = rqConnMapMulticast.find(key);
//    if (i != rqConnMapMulticast.end())
//        return i->second;
//
//    // try with blank remote socket, and localAddr missing (for incoming SYN)
////    key.localAddr = L3Address();
//    key.multicastGid = multicastGid;
//    i = rqConnMapMulticast.find(key);
//    if (i != rqConnMapMulticast.end())
//        return i->second;
//    std::cout << "  RaptorQ::findConnForSegmentMulticast44\n";
//
//    // given up
    return nullptr;
}





















RQConnection *RaptorQ::findConnForApp(int appGateIndex, int connId) {
    AppConnKey key;
    key.appGateIndex = appGateIndex;
    key.connId = connId;

    auto i = rqAppConnMap.find(key);
    return i == rqAppConnMap.end() ? nullptr : i->second;
}

ushort RaptorQ::getEphemeralPort() {
    // start at the last allocated port number + 1, and search for an unused one
    ushort searchUntil = lastEphemeralPort++;
    if (lastEphemeralPort == EPHEMERAL_PORTRANGE_END) // wrap
        lastEphemeralPort = EPHEMERAL_PORTRANGE_START;

    while (usedEphemeralPorts.find(lastEphemeralPort)
            != usedEphemeralPorts.end()) {
        if (lastEphemeralPort == searchUntil) // got back to starting point?
            throw cRuntimeError(
                    "Ephemeral port range %d..%d exhausted, all ports occupied",
                    EPHEMERAL_PORTRANGE_START, EPHEMERAL_PORTRANGE_END);

        lastEphemeralPort++;

        if (lastEphemeralPort == EPHEMERAL_PORTRANGE_END) // wrap
            lastEphemeralPort = EPHEMERAL_PORTRANGE_START;
    }

    // found a free one, return it
    return lastEphemeralPort;
}

void RaptorQ::addSockPair(RQConnection *conn, L3Address localAddr,
        L3Address remoteAddr, int localPort, int remotePort) {
    // update addresses/ports in RQConnection
    SockPair key;
    key.localAddr = conn->localAddr = localAddr;
    key.remoteAddr = conn->remoteAddr = remoteAddr; // mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm conn->remoteAddr hereeeeeeeeeee
    key.localPort = conn->localPort = localPort;
    key.remotePort = conn->remotePort = remotePort;

//    std::cout << "aaaa  localAddr " <<  localAddr << " \n";
//    std::cout << "aaa remoteAddr   " <<  remoteAddr  << " \n";
//    std::cout << "aaa localPort  " <<  localPort  << " \n";
//    std::cout << "aaa remotePort  " <<  remotePort  << " \n";


    // make sure connection is unique
    auto it = rqConnMap.find(key);
    if (it != rqConnMap.end()) {
        // throw "address already in use" error
        if (remoteAddr.isUnspecified() && remotePort == -1)
            throw cRuntimeError(
                    "Address already in use: there is already a connection listening on %s:%d",
                    localAddr.str().c_str(), localPort);
        else
            throw cRuntimeError(
                    "Address already in use: there is already a connection %s:%d to %s:%d",
                    localAddr.str().c_str(), localPort,
                    remoteAddr.str().c_str(), remotePort);
    }

    // then insert it into rqConnMap
    rqConnMap[key] = conn;

    // mark port as used
    if (localPort >= EPHEMERAL_PORTRANGE_START
            && localPort < EPHEMERAL_PORTRANGE_END)
        usedEphemeralPorts.insert(localPort);
}

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ Multicast
void RaptorQ::addMulticastSockPair(RQConnection *conn, L3Address localAddr,  int localPort,  int multicastGid) {

    std::cout << " RaptorQ::addMulticastSockPair \n" ;
    // update addresses/ports in RQConnection
    SockPairMulticast key;
    key.localAddr = conn->localAddr = localAddr;
    key.localPort = conn->localPort = localPort;
    key.multicastGid =   multicastGid;

    std::cout << "  multicastGid " <<  multicastGid << " \n";
    std::cout << "  multicakey.localPort   " <<  key.localPort  << " \n";
    std::cout << " localAddr  " <<  localAddr  << " \n";
//    key.remoteAddr = conn->remoteAddr = remoteAddr; // mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm conn->remoteAddr hereeeeeeeeeee
//    key.remotePort = conn->remotePort = remotePort;


    // make sure connection is unique
//    auto it = rqConnMapMulticast.find(key);
//    if (it != rqConnMapMulticast.end()) {
//        // throw "address already in use" error
//        if (remoteAddr.isUnspecified() && remotePort == -1)
//            throw cRuntimeError( "Address already in use: there is already a connection listening on %s:%d", localAddr.str().c_str(), localPort);
//        else
//            throw cRuntimeError(
//                    "Address already in use: there is already a connection %s:%d to %s:%d",
//                    localAddr.str().c_str(), localPort,
//                    remoteAddr.str().c_str(), remotePort);
//    }

    // then insert it into rqConnMap
    rqConnMapMulticast[key] = conn;

    // mark port as used
    if (localPort >= EPHEMERAL_PORTRANGE_START
            && localPort < EPHEMERAL_PORTRANGE_END)
        usedEphemeralPorts.insert(localPort);
}







void RaptorQ::updateSockPair(RQConnection *conn, L3Address localAddr,
        L3Address remoteAddr, int localPort, int remotePort) {
    // find with existing address/port pair...
    SockPair key;
    key.localAddr = conn->localAddr;
    key.remoteAddr = conn->remoteAddr;
    key.localPort = conn->localPort;
    key.remotePort = conn->remotePort;
    auto it = rqConnMap.find(key);

    ASSERT(it != rqConnMap.end() && it->second == conn);

    // ...and remove from the old place in rqConnMap
    rqConnMap.erase(it);

    // then update addresses/ports, and re-insert it with new key into rqConnMap
    key.localAddr = conn->localAddr = localAddr;
    key.remoteAddr = conn->remoteAddr = remoteAddr;
    ASSERT(conn->localPort == localPort);
    key.remotePort = conn->remotePort = remotePort;
    rqConnMap[key] = conn;

    // localPort doesn't change (see ASSERT above), so there's no need to update usedEphemeralPorts[].
}

void RaptorQ::addForkedConnection(RQConnection *conn, RQConnection *newConn, L3Address localAddr, L3Address remoteAddr, int localPort,
 int remotePort) {
    // update conn's socket pair, and register newConn (which'll keep LISTENing)
    updateSockPair(conn, localAddr, remoteAddr, localPort, remotePort);
    addSockPair(newConn, newConn->localAddr, newConn->remoteAddr, newConn->localPort, newConn->remotePort);
//  std::cout << " RaptorQ::addForkedConnection \n\n\n";
    // conn will get a new connId...
    AppConnKey key;
    key.appGateIndex = conn->appGateIndex;
    key.connId = conn->connId;
    rqAppConnMap.erase(key);
    key.connId = conn->connId = getEnvir()->getUniqueNumber();
    rqAppConnMap[key] = conn;

    // ...and newConn will live on with the old connId
    key.appGateIndex = newConn->appGateIndex;
    key.connId = newConn->connId;
    rqAppConnMap[key] = newConn;
}

void RaptorQ::removeConnection(RQConnection *conn) {
    EV_INFO << "Deleting RQ connectionnn \n";

    AppConnKey key;
    key.appGateIndex = conn->appGateIndex;
    key.connId = conn->connId;
    rqAppConnMap.erase(key);

    SockPair key2;
    key2.localAddr = conn->localAddr;
    key2.remoteAddr = conn->remoteAddr;
    key2.localPort = conn->localPort;
    key2.remotePort = conn->remotePort;
    rqConnMap.erase(key2);

    // IMPORTANT: usedEphemeralPorts.erase(conn->localPort) is NOT GOOD because it
    // deletes ALL occurrences of the port from the multiset.
    auto it = usedEphemeralPorts.find(conn->localPort);

    if (it != usedEphemeralPorts.end())
        usedEphemeralPorts.erase(it);

    delete conn;
}

void RaptorQ::finish() {
    if (requestTimerMsg->isScheduled())
        cancelEvent(requestTimerMsg);
      delete requestTimerMsg;
//    for (std::map<int, int>::iterator iter =  appGateIndexTimeOutMap.begin(); iter !=  appGateIndexTimeOutMap.end(); ++iter) {
//        std::cout << "  connId: " << iter->first << " ===> ";
//        std::cout << "  numRequestsRTOs:      " << iter->second << " \n";
//        emit(numRequestsRTOs, iter->second);
//    }


    EV_INFO << getFullPath() << ": finishing with " << rqConnMap.size()
                   << " connections open.\n";
}

RQSendQueue *RaptorQ::createSendQueue(RQDataTransferMode transferModeP) {

    switch (transferModeP) {
    case RQ_TRANSFER_BYTECOUNT:
        return new RQVirtualDataSendQueue();

    case RQ_TRANSFER_OBJECT:
        return new RQMsgBasedSendQueue();

    case RQ_TRANSFER_BYTESTREAM:
        return new RQByteStreamSendQueue();

    case RQ_REQUESTS:
            return new RQRequestsSendQueue();


    default:
        throw cRuntimeError("Invalid RQ data transfer mode: %d",
                transferModeP);
    }
}

RQReceiveQueue *RaptorQ::createReceiveQueue(RQDataTransferMode transferModeP) {
    switch (transferModeP) {
    case RQ_TRANSFER_BYTECOUNT:
        return new RQVirtualDataRcvQueue();

    case RQ_TRANSFER_OBJECT:
        return new RQMsgBasedRcvQueue();

    case RQ_TRANSFER_BYTESTREAM:
        return new RQByteStreamRcvQueue();

    case RQ_RECEIVE_SYMBOLS:
        return new RQSymbolsRcvQueue();

    default:
        throw cRuntimeError("Invalid RQ data transfer mode: %d",
                transferModeP);
    }
}

bool RaptorQ::handleOperationStage(LifecycleOperation *operation, int stage,
        IDoneCallback *doneCallback) {
    Enter_Method_Silent();

    if (dynamic_cast<NodeStartOperation *>(operation)) {
        if ((NodeStartOperation::Stage)stage == NodeStartOperation::STAGE_TRANSPORT_LAYER) {
            //FIXME implementation
            isOperational = true;
            updateDisplayString();
        }
    }
    else if (dynamic_cast<NodeShutdownOperation *>(operation)) {
        if ((NodeShutdownOperation::Stage)stage == NodeShutdownOperation::STAGE_TRANSPORT_LAYER) {
            //FIXME close connections???
            reset();
            isOperational = false;
            updateDisplayString();
        }
    }
    else if (dynamic_cast<NodeCrashOperation *>(operation)) {
        if ((NodeCrashOperation::Stage)stage == NodeCrashOperation::STAGE_CRASH) {
            //FIXME implementation
            reset();
            isOperational = false;
            updateDisplayString();
        }
    }
    else {
        throw cRuntimeError("Unsupported operation '%s'", operation->getClassName());
    }

    return true;
}

void RaptorQ::reset() {
    for (auto & elem : rqAppConnMap)
        delete elem.second;
    rqAppConnMap.clear();
    rqConnMap.clear();
    usedEphemeralPorts.clear();
    lastEphemeralPort = EPHEMERAL_PORTRANGE_START;
}





// MOH not used now ?????
void RaptorQ::getCentralSchedulerModule(unsigned int groupIndex,std::set<int>& multicastReceivers , std::set<std::string>& nodePodRackLoc , std::vector<int>& receiversPortNumbers ) {
//    std::cout << "aaaaaaa    RaptorQ::getCentralSchedulerModule()"      <<  "  \n";

    std::string mod = "centralScheduler";
    cModule *centralMod = getModuleByPath(mod.c_str());
    CentralScheduler *centralSch = check_and_cast<CentralScheduler *>(centralMod);
    centralSch->getMulticastGroupGivenIndex( groupIndex ,  multicastReceivers);

    centralSch->getPodRackNodeForEachReceiverInMulticastGroup( multicastReceivers ,  nodePodRackLoc);
    centralSch->getMulticastGroupReceiversPortNumbers(groupIndex ,receiversPortNumbers );
    if (centralMod) {
        std::cout  << "RQSinkApp::handleMessage    centralMod centralMod  \n\n\n";
    }

}

//void RaptorQ::getCorrespondingMulticastGroup() {
//
//
//
//}




void RaptorQ::sendFirstRequest() {

//       std::cout << "  sendFirstRequest  \n";

    bool allEmpty = allPullQueuesEmpty();
//    if (allEmpty == false && test == true) {   // ?????? why test is here
    if (allEmpty == false ) {
         requestTimer();

    //    process_REQUEST_TIMER();
//        test = false;
    }
}


// MOH added
void RaptorQ::printConnRequestMap() {
//    std::cout << "   printConnRequestMap   "     << "\n";
    auto iterrr = requestCONNMap.begin();
    int index = 0;
    while (iterrr != requestCONNMap.end()) {
//          EV << index << " printConnRequestMap \n " ;
//          EV <<  " connIndex = " <<  iterrr->first << " \n " ;
//          EV << "  connId=  " << iterrr->second->connId  << "\n" ;
//          EV << " getPullsQueueLength()=  " << iterrr->second->getPullsQueueLength() << "\n" ;
//          EV << " rcvdSymbols =  " << iterrr->second->getNumRcvdSymbols() <<  "\n\n\n" ;
//          EV << " requestCONNMap.size() =  " << requestCONNMap.size() <<  "\n\n\n" ;

//          std::cout << " requestCONNMap.size() =  " << requestCONNMap.size() <<  "\n\n\n" ;

//        if (iterrr->second->getNumRcvdSymbols() > 0 && test == true) {
//            requestTimer();
//            test = false;
//        }
        index++;
        iterrr++;
    }

}


// returns true if all Pull queues are empty; otherwise returns false
bool RaptorQ::allPullQueuesEmpty() {
//     std::cout << "  RaptorQ::allPullQueuesEmpty()    "  << this->getFullPath()   << "\n";
    int pullsQueueLength =0;
    auto iter = requestCONNMap.begin();
    while (iter !=  requestCONNMap.end()){
//        std::cout << "   aaaaallPullQueuesEmpty   "     << "\n";
         pullsQueueLength = iter->second->getPullsQueueLength();
//         std::cout <<    this->getFullPath()   << " pullsQueueLength=   "  <<   pullsQueueLength<< "\n";

        if (pullsQueueLength >0) return false;
        ++iter;
    }
      return true;
}

// returns true if all connections are finished; otherwise returns false
bool RaptorQ::allConnFinished() {
//     std::cout << "  allConnFinished ?   "     << "\n";
    bool connDone;

    auto iter = requestCONNMap.begin();
    int ii=0;
    while (iter != requestCONNMap.end()) {
        connDone = iter->second->isConnFinished();
//        std::cout << "  allConnFinished ssss  "    << requestCONNMap.size() << "\n";

      if (connDone == false) {
//            std::cout << "allConnFinished FALSE FFFFFFFFFFF, connIndex=  " << ii << " ,numConn = " << requestCONNMap.size() << "\n";
            return false;
        }
        ++iter;
        ++ii;
    }
//     std::cout << " allConnFinished TREUEEE TTTTTTTTTTTT: "  << "\n";
    cancelRequestTimer();
    return true;
}



void RaptorQ::updateConnMap() {
    std::cout << "  updateConnMap updateConnMap   "     << "\n";
    a:
    bool connDone;
    auto iter = requestCONNMap.begin();

    while (iter != requestCONNMap.end()) {
        connDone = iter->second->isConnFinished();
        if (connDone == true)  {
            requestCONNMap.erase(iter);
            goto a;
        }
         ++iter;
   }
}


void RaptorQ::requestTimer() {
//    std::cout << "  requestTimer  \n";

//  //  state->request_rexmit_count = 0;
//  //  state->request_rexmit_timeout = RQ_TIMEOUT_PULL_REXMIT;  // 3 sec
//    std::cout << " start requestTimer... \n ";
//    EV << " start requestTimer... \n ";

    cancelRequestTimer();
    simtime_t requestTime = (simTime() + SimTime(8, SIMTIME_US));
//    if (allConnFinished() == false)
     scheduleAt(requestTime, requestTimerMsg); // 0.000009
//     requestTimerStamps.record(requestTime);


//    if (allConnFinished() == true)
//        cancelRequestTimer();
}

void RaptorQ::cancelRequestTimer() {
//    std::cout << " cancelRequestTimer  "   << "\n";
    if (requestTimerMsg->isScheduled())
        cancelEvent(requestTimerMsg);
//    delete requestTimerMsg;
}

bool RaptorQ::getNapState() {
    return nap;
}


void RaptorQ::process_REQUEST_TIMER() {
//     std::cout << "  \n\n\n\n hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh process_REQUEST_TIMER " << this->getFullPath() << " \n";
    bool sendNewRequest = false;
    auto iter = requestCONNMap.begin();
    bool allEmpty = allPullQueuesEmpty();
    bool allDone = allConnFinished();


    if (allDone == true) {
        cancelRequestTimer();
//        std::cout << "allDone == true allDone == true " << " \n";
    } else if (allDone == false && allEmpty == true) {
//        std::cout << " allDone == false && allEmpty == true " << this->getFullPath() << " \n";
        ++times;
//         requestTimer(); // ?????????????????????
//        if (times == 50000) {
//            times=0;
//            cancelRequestTimer();
//        }

         nap=true;

    } else if (allDone == false && allEmpty == false) {
//        std::cout << " while " << "\n";
        times=0;
        nap=false;
        while (sendNewRequest != true) {

            if (counter == requestCONNMap.size())  counter = 0;
            auto iter = requestCONNMap.begin();
            std::advance(iter, counter);
            int pullsQueueLength = iter->second->getPullsQueueLength();
//            std::cout << "  pullsQueueLength = "<<pullsQueueLength<<  " \n";

            if (pullsQueueLength > 0) {
                iter->second->sendRequestFromPullsQueue();
                requestTimer();
                sendNewRequest = true;
            }
            ++counter;
        }
//        std::cout << "  end while  \n\n\n";

    }
}



////    if (counter == requestCONNMap.size()) counter = 0;
//      updateConnMap();
//    bool sendNewRequest = false;
//    while (sendNewRequest != true) {
//        auto iter = requestCONNMap.begin();
//        std::cout << " while " << "\n";
//
//        if (counter == requestCONNMap.size())        counter = 0;
//
//        bool allEmpty = allPullQueuesEmpty();
//        bool allDone = allConnFinished();
//
//        if (allDone == true) {
//            cancelRequestTimer();
//            sendNewRequest = true;
//            counter = 0; timeOut=0;
//            std::cout << "allDone == true allDone == true "     << " \n";
//
//        }
//        else if (allDone == false && allEmpty == true) {
//             requestTimer();
//            sendNewRequest = true;
//
//            std::cout << " allDone == false && allEmpty == true " <<  this->getFullPath() << " \n";
//            counter = requestCONNMap.size()-1;
//            std::advance(iter, counter);
//             std::cout << "  asdasdasdasdasdasdasdasdas timeOut" <<  timeOut << " \n";
//             std::cout << "  asdasdasdasdasdasdasdasdas requestCONNMap.size()" <<  requestCONNMap.size() << " \n";
//
//             bool thisDone = iter->second->isConnFinished();
//             std::cout << "  asdasdasdasdasdasdasdasdas thisDone" <<  thisDone  << " \n";
//
//             if (timeOut > 10 && thisDone == false){
//
//             iter->second->addRequestToPullsQueue();
//             int pullsQueueLength = iter->second->getPullsQueueLength();
//             if (pullsQueueLength > 0) {
//                             iter->second->sendRequestFromPullsQueue();
//                             requestTimer();
//                             std::cout << "  pullsQueueLength > 0  \n";
//                             sendNewRequest = true;
//             }
//             ++counter;
//             timeOut=0;
//
//             }
//             ++timeOut;
//
//            // counter = 0;
//        }
//
//        else if (allDone == false && allEmpty == false) {
//            timeOut = 0;
//            std::advance(iter, counter);
//            int pullsQueueLength = iter->second->getPullsQueueLength();
//            std::cout << "  pullsQueueLength aaaa> 0  \n";
//
//            if (pullsQueueLength > 0) {
//                iter->second->sendRequestFromPullsQueue();
//                requestTimer();
//                std::cout << "  pullsQueueLength > 0  \n";
//                sendNewRequest = true;
//            }
//            ++counter;
//
////            std::cout << "  allEmpty ==false  \n";
////            std::cout << "  process_REQUEST_TIMER  \n";
////            std::cout << " connIndex = " << iter->first << " \n ";
////            std::cout << "  connId=  " << iter->second->connId << "\n";
////            std::cout << "  getPullsQueueLength()=  " << this->getFullPath() << " " << iter->second->getPullsQueueLength() << "\n";
// //            std::cout << " rcvdSymbols =  " << iter->second->getNumRcvdSymbols()<< "\n\n\n";
////            std::cout << " requestCONNMap.size() =  " << requestCONNMap.size() << "\n\n\n";
////            std::cout << " requestCONNMap.size()aaaaa =  "  << requestCONNMap.size() << "\n\n\n";
////
////            EV << "  process_REQUEST_TIMER  \n";
////            EV << " connIndex = " << iter->first << " \n ";
////            EV << "  connId=  " << iter->second->connId << "\n";
////            EV << "  getPullsQueueLength()=  " << iter->second->getPullsQueueLength() << "\n";
////            EV << " getPullsQueueLength()=  " << iter->second->getPullsQueueLength() << "\n";
////            EV << " rcvdSymbols =  " << iter->second->getNumRcvdSymbols()  << "\n\n\n";
////            EV << " requestCONNMap.size() =  " << requestCONNMap.size() << "\n\n\n";
////            EV << " requestCONNMap.size()aaaaa =  " << requestCONNMap.size()  << "\n\n\n";
//        }
//
//    }
//}



} // namespace rq
} // namespace inet
