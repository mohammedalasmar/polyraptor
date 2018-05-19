#include <string.h>
#include <algorithm>    // min,max

#include "RaptorQ.h"
#include "RQConnection.h"
#include "raptorq_common/RQSegment.h"
#include "../contract/raptorq/RQCommand_m.h"
#include "inet/networklayer/contract/IL3AddressType.h"
#include "inet/networklayer/contract/INetworkProtocolControlInfo.h"
#include "inet/networklayer/common/IPProtocolId_m.h"
#include "RQSendQueue.h"

//#include "inet/transportlayer/tcp/RQSACKRexmitQueue.h"
#include "RQReceiveQueue.h"
#include "RQAlgorithm.h"
#include "inet/common/INETUtils.h"

#include "inet/raptorqimplementation/application/raptorqapp/GenericAppMsgRq_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"

namespace inet {

namespace raptorq {

//
// helper functions
//

const char *RQConnection::stateName(int state) {
#define CASE(x)    case x: \
        s = #x + 5; break
    const char *s = "unknown";
    switch (state) {
    CASE(RQ_S_INIT)
;        CASE(RQ_S_CLOSED);
        CASE(RQ_S_LISTEN);
        CASE(RQ_S_SYN_SENT);
        CASE(RQ_S_SYN_RCVD);
        CASE(RQ_S_ESTABLISHED);
        CASE(RQ_S_CLOSE_WAIT);
        CASE(RQ_S_LAST_ACK);
        CASE(RQ_S_FIN_WAIT_1);
        CASE(RQ_S_FIN_WAIT_2);
        CASE(RQ_S_CLOSING);
        CASE(RQ_S_TIME_WAIT);
    }
    return s;
#undef CASE
}

const char *RQConnection::eventName(int event) {
#define CASE(x)    case x: \
        s = #x + 5; break
    const char *s = "unknown";
    switch (event) {
    CASE(RQ_E_IGNORE)
;        CASE(RQ_E_OPEN_ACTIVE);
        CASE(RQ_E_OPEN_PASSIVE);
        CASE(RQ_E_SEND);
        CASE(RQ_E_CLOSE);
        CASE(RQ_E_ABORT);
        CASE(RQ_E_STATUS);
        CASE(RQ_E_QUEUE_BYTES_LIMIT);
        CASE(RQ_E_RCV_DATA);
        CASE(RQ_E_RCV_ACK);
        CASE(RQ_E_RCV_SYN);
        CASE(RQ_E_RCV_SYN_ACK);
        CASE(RQ_E_RCV_FIN);
        CASE(RQ_E_RCV_FIN_ACK);
        CASE(RQ_E_RCV_RST);
        CASE(RQ_E_RCV_UNEXP_SYN);
        CASE(RQ_E_TIMEOUT_2MSL);
        CASE(RQ_E_TIMEOUT_CONN_ESTAB);
        CASE(RQ_E_TIMEOUT_FIN_WAIT_2);
    }
    return s;
#undef CASE
}

const char *RQConnection::indicationName(int code) {
#define CASE(x)    case x: \
        s = #x + 5; break
    const char *s = "unknown";
    switch (code) {
    CASE(RQ_I_DATA)
;        CASE(RQ_I_URGENT_DATA);
        CASE(RQ_I_ESTABLISHED);
        CASE(RQ_I_PEER_CLOSED);
        CASE(RQ_I_CLOSED);
        CASE(RQ_I_CONNECTION_REFUSED);
        CASE(RQ_I_CONNECTION_RESET);
        CASE(RQ_I_TIMED_OUT);
        CASE(RQ_I_STATUS);
        CASE(RQ_I_SEND_MSG);
    }
    return s;
#undef CASE
}





void RQConnection::sendToIP(RQSegment *rqseg, L3Address src, L3Address dest) {
    EV_INFO << "Sending: ";
    printSegmentBrief(rqseg);

    IL3AddressType *addressType = dest.getAddressType();
    INetworkProtocolControlInfo *controlInfo =
            addressType->createNetworkProtocolControlInfo();
    controlInfo->setTransportProtocol(IP_PROT_TCP);
    controlInfo->setSourceAddress(src);
    controlInfo->setDestinationAddress(dest);
    rqseg->setControlInfo(check_and_cast<cObject *>(controlInfo));
    rqseg->setByteLength(rqseg->getHeaderLength() + rqseg->getPayloadLength());
    check_and_cast<RaptorQ *>(getSimulation()->getContextModule())->send(rqseg,"ipOut");
}

RQSegment *RQConnection::createRQSegment(const char *name) {
    return new RQSegment(name);
}

void RQConnection::signalConnectionTimeout() {
    sendIndicationToApp(RQ_I_TIMED_OUT);
}

void RQConnection::sendIndicationToApp(int code, const int id) {
    EV_INFO << "Notifying app: " << indicationName(code) << "\n";
//    std::cout<< "Notifying app: CLOSEEEEEEEEEEEE " << indicationName(code) << "\n";

    cMessage *msg = new cMessage(indicationName(code));
    msg->setKind(code);
    RQCommand *ind = new RQCommand();
    ind->setConnId(connId);
    ind->setUserId(id);
    msg->setControlInfo(ind);
    sendToApp(msg);
//    std::cout<< "Notifying app: CLOSEEEEEEEEEEEE " << indicationName(code) << "\n";

}

void RQConnection::sendEstabIndicationToApp() {
    EV_INFO << "Notifying app: " << indicationName(RQ_I_ESTABLISHED) << "\n";
    cMessage *msg = new cMessage(indicationName(RQ_I_ESTABLISHED));
    msg->setKind(RQ_I_ESTABLISHED);

    RQConnectInfo *ind = new RQConnectInfo();
    ind->setConnId(connId);
    ind->setLocalAddr(localAddr);
    ind->setRemoteAddr(remoteAddr);
    ind->setLocalPort(localPort);
    ind->setRemotePort(remotePort);

    msg->setControlInfo(ind);
    sendToApp(msg);
}

void RQConnection::sendToApp(cMessage *msg) {
    rqMain->send(msg, "appOut", appGateIndex);

}

void RQConnection::initConnection(RQOpenCommand *openCmd) {

    transferMode = (RQDataTransferMode) (openCmd->getDataTransferMode());

    // MOH: MODIFIED  4 RQ_REQUESTS (couldn't be specified  in the .ini
    //file as rqrequests because we just have one transfermode identifier for both sendQueue and ReceiveQueue)
    // create send queue      // create receive queue
    if (transferMode == RQ_REQUESTS) {
        sendQueue = rqMain->createSendQueue(RQ_REQUESTS);
        receiveQueue = rqMain->createReceiveQueue(RQ_RECEIVE_SYMBOLS);
    } else {
        sendQueue = rqMain->createSendQueue(RQ_TRANSFER_OBJECT);
        receiveQueue = rqMain->createReceiveQueue(RQ_TRANSFER_OBJECT);
    }
    sendQueue->setConnection(this);
    receiveQueue->setConnection(this);
    // create SACK retransmit queue
    //  rexmitQueue = new RQSACKRexmitQueue();
    // rexmitQueue->setConnection(this);
    // create algorithm
    const char *rqAlgorithmClass = openCmd->getRqAlgorithmClass();
    if (!rqAlgorithmClass || !rqAlgorithmClass[0])
        rqAlgorithmClass = rqMain->par("rqAlgorithmClass");
    rqAlgorithm = check_and_cast<RQAlgorithm *>( inet::utils::createOne(rqAlgorithmClass));
    rqAlgorithm->setConnection(this);
    // create state block
    state = rqAlgorithm->getStateVariables();
    configureStateVariables();
    rqAlgorithm->initialize();
}

void RQConnection::configureStateVariables() {

    long advertisedWindowPar = rqMain->par("advertisedWindow").longValue();
    state->ws_support = rqMain->par("windowScalingSupport"); // if set, this means that current host supports WS (RFC 1323)
    state->ws_manual_scale = rqMain->par("windowScalingFactor"); // scaling factor (set manually) to help for RQ validation
    if (!state->ws_support
            && (advertisedWindowPar > RQ_MAX_WIN || advertisedWindowPar <= 0))
        throw cRuntimeError("Invalid advertisedWindow parameter: %ld",
                advertisedWindowPar);

    state->rcv_wnd = advertisedWindowPar;
    state->rcv_adv = advertisedWindowPar;
    state->snd_mss = rqMain->par("mss").longValue(); // Maximum Segment Size (RFC 793)

//    if (state->ws_support && advertisedWindowPar > RQ_MAX_WIN) {
//        state->rcv_wnd = RQ_MAX_WIN; // we cannot to guarantee that the other end is also supporting the Window Scale (header option) (RFC 1322)
//        state->rcv_adv = RQ_MAX_WIN; // therefore RQ_MAX_WIN is used as initial value for rcv_wnd and rcv_adv
//    }

    state->maxRcvBuffer = advertisedWindowPar;
    state->delayed_acks_enabled = rqMain->par("delayedAcksEnabled"); // delayed ACK algorithm (RFC 1122) enabled/disabled
    state->nagle_enabled = rqMain->par("nagleEnabled"); // Nagle's algorithm (RFC 896) enabled/disabled
    state->limited_transmit_enabled = rqMain->par("limitedTransmitEnabled"); // Limited Transmit algorithm (RFC 3042) enabled/disabled
    state->increased_IW_enabled = rqMain->par("increasedIWEnabled"); // Increased Initial Window (RFC 3390) enabled/disabled
    state->ts_support = rqMain->par("timestampSupport"); // if set, this means that current host supports TS (RFC 1323)
    state->sack_support = rqMain->par("sackSupport"); // if set, this means that current host supports SACK (RFC 2018, 2883, 3517)

//    if (state->sack_support) {
//        std::string algorithmName1 = "RQReno";
//        std::string algorithmName2 = rqMain->par("rqAlgorithmClass");
//
//        if (algorithmName1 != algorithmName2) { // TODO add additional checks for new SACK supporting algorithms here once they are implemented
//            EV_DEBUG
//                            << "If you want to use RQ SACK please set rqAlgorithmClass to RQReno\n";
//
//            ASSERT(false);
//        }
//    }
}

void RQConnection::selectInitialSeqNum() {
    // set the initial send sequence number
    state->iss = (unsigned long) fmod(SIMTIME_DBL(simTime()) * 250000.0,
            1.0 + (double) (unsigned) 0xffffffffUL) & 0xffffffffUL;

    state->snd_una = state->snd_nxt = state->snd_max = state->iss =
            state->request_id = state->esi = 0;
    sendQueue->init(state->iss + 1);    // + 1 is for SYN
    //  rexmitQueue->init(state->iss + 1);    // + 1 is for SYN

}


void RQConnection::sendSyn() {

    if (remoteAddr.isUnspecified() || remotePort == -1)
        throw cRuntimeError(rqMain,
                "Error processing command OPEN_ACTIVE: foreign socket unspecified");

    if (localPort == -1)
        throw cRuntimeError(rqMain,
                "Error processing command OPEN_ACTIVE: local port unspecified");

    // create segment
    RQSegment *rqseg = createRQSegment("SYN");
    rqseg->setSequenceNo(state->iss);
    rqseg->setSynBit(true);
    updateRcvWnd();
    rqseg->setWindow(state->rcv_wnd);
    rqseg->setFileId(state->fileId);
    rqseg->setNumSymbolsToSend(state->numSymbolsToSend);

     std::cout  << " RQConnection::sendSyn()  \n";
//     std::cout  << " RQConnection::sendSyn() state->isMulticastConn "  << state->isMulticastConn << "\n";
//     std::cout  << " RQConnection::sendSyn() state->multicastGroupIndex "  << state->multicastGroupIndex << "\n";


    rqseg->setIsMulticastConnSeg(state->isMulticastConn) ; // multicast
    rqseg->setMulticastGroupIndex(state->multicastGroupIndex);
    rqseg->setIsMulticastSender(state->isSender); // true
    rqseg->setIsMulticastReceiver(false);


    rqseg->setIsMultiSourcingConn(state->isMultiSourcingConn);
    rqseg->setMultiSourcingGroupIndex(state->multiSourcingGroupIndex) ;
    rqseg->setIsMultiSourcingSender(true);
    rqseg->setIsMultiSourcingReceiver(false);




//    std::cout << " sendSyn    state->multicastGroupIndex " <<state->multicastGroupIndex << std::endl << std::endl << std::endl << std::endl;

    state->snd_max = state->snd_nxt = state->iss + 1;

    // write header options
    writeHeaderOptions(rqseg);

    // send it
    sendToIP(rqseg);
//      if (state->isMulticastConn == false) sendToIP(rqseg);
//      if (state->isMulticastConn == true) sendToIPMulticast(rqseg);
}




// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// ^^^^^^^^^^^^^^^^^^     Multicast  send SYN                ^^^^^^^^^
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
void RQConnection::sendToIPMulticast() {
//
////    if (remoteAddr.isUnspecified() || remotePort == -1)
////        throw cRuntimeError(rqMain,"Error processing command OPEN_ACTIVE: foreign socket unspecified");
//    if (localPort == -1)
//        throw cRuntimeError(rqMain,"Error processing command OPEN_ACTIVE: local port unspecified");
//
//
//
//
//    std::cout << " syn sendToIPMulticast qqqqqqqqqqqqqqqqqqqqqqqqq  \n";
//    std::string mod = "centralScheduler";
//
//    std::set<int> multicastReceivers ;  // to store the receivers IDs  for the: multicastGroupIndex (the index value has been read from the App)
//    std::set<std::string>  nodePodRackLoc;// to store the receivers names'  for the: multicastGroupIndex
//    std::vector<int> receiversPortNumbers;
//    // get receivers names[Pod.Rack.Node]. which will be used to assign connectAddress
//    getRQMain()->getCentralSchedulerModule(state->multicastGroupIndex , multicastReceivers , nodePodRackLoc , receiversPortNumbers);
//
//    for (auto iter= multicastReceivers.begin(); iter!= multicastReceivers.end(); ++iter){
////        std::cout << " kkkkkkkkkk " << *iter  << "   \n";
//    }
//
//    for (auto iter= receiversPortNumbers.begin(); iter!= receiversPortNumbers.end(); ++iter){
////        std::cout << " kkkkkkkkkk pppppppp " << *iter  << "   \n";
//    }
//
//    int i = 0;
////    auto iterrr= receiversPortNumbers.begin();
//    for (auto iter = nodePodRackLoc.begin(); iter != nodePodRackLoc.end();++iter) {
//        std::cout << " kkkkkkkkkk dsdsd" << *iter << "   \n";
//
//
//
////        std::string connectAddress =  "FatTree.Pod[1].racks[0].servers[0]" ;
////                   L3Address destination;
////                   L3AddressResolver().tryResolve(connectAddress.c_str(), destination);
////          std::cout << "ccccccc Connecting to " << remoteAddr     << std::endl;
//         L3AddressResolver().tryResolve(iter->c_str(), remoteAddr);
//         remotePort = receiversPortNumbers.at(i);
//         ++i;
//
//
//         std::cout << "ccccccc remoteAddr " << remoteAddr     << std::endl;
//         std::cout << "ccccccc remotePort " << remotePort     << std::endl;
//
//
//
//
//        // create segment
//        RQSegment *rqseg = createRQSegment("SYN");
//        rqseg->setSequenceNo(state->iss);
//        rqseg->setSynBit(true);
//        updateRcvWnd();
//        rqseg->setWindow(state->rcv_wnd);
//        rqseg->setFileId(state->fileId);
//        rqseg->setNumSymbolsToSend(state->numSymbolsToSend);
//
//        rqseg->setIsMulticastConnSeg(state->isMulticastConn); // multicast
//        rqseg->setMulticastGroupIndex(state->multicastGroupIndex);
//        rqseg->setIsSender(true);
//
//        state->snd_max = state->snd_nxt = state->iss + 1;
//        writeHeaderOptions(rqseg);
//
//        rqseg->setSrcPort(localPort);
//        rqseg->setDestPort(remotePort);
//
//        ASSERT(rqseg->getHeaderLength() >= RQ_HEADER_OCTETS); // RQ_HEADER_OCTETS = 20 (without options)
//        ASSERT(rqseg->getHeaderLength() <= RQ_MAX_HEADER_OCTETS); // RQ_MAX_HEADER_OCTETS = 60
//        rqseg->setByteLength(
//                rqseg->getHeaderLength() + rqseg->getPayloadLength());
//        state->sentBytes = rqseg->getPayloadLength(); // resetting sentBytes to 0 if sending a segment without data (e.g. ACK)
//        EV_INFO << "Sending: ";
//        printSegmentBrief(rqseg);
//        // TBD reuse next function for sending
//        IL3AddressType *addressType = remoteAddr.getAddressType();
//        INetworkProtocolControlInfo *controlInfo =   addressType->createNetworkProtocolControlInfo();
//        controlInfo->setTransportProtocol(IP_PROT_TCP); // TODO RQ ???
//        controlInfo->setSourceAddress(localAddr);
//        controlInfo->setDestinationAddress(remoteAddr);
//        rqseg->setControlInfo(check_and_cast<cObject *>(controlInfo));
//
//        ASSERT(rqseg->getHeaderLength() >= RQ_HEADER_OCTETS); // RQ_HEADER_OCTETS = 20 (without options)
//        ASSERT(rqseg->getHeaderLength() <= RQ_MAX_HEADER_OCTETS); // RQ_MAX_HEADER_OCTETS = 60
//        rqseg->setByteLength(  rqseg->getHeaderLength() + rqseg->getPayloadLength());
//
//
//        //    std::cout << " ssendddddd to ippppppp \n";
//        rqMain->send(rqseg, "ipOut");
//    }
//
//    state->numReplica = receiversPortNumbers.size();
//    std::cout << " ssendddddd to ippppppp " << state->numReplica   << " \n\n\n";

}




void RQConnection::sendToIPMulticastFirstACK() {

//    if (remoteAddr.isUnspecified() || remotePort == -1)
//        throw cRuntimeError(rqMain,"Error processing command OPEN_ACTIVE: foreign socket unspecified");
//    if (localPort == -1)
//        throw cRuntimeError(rqMain,"Error processing command OPEN_ACTIVE: local port unspecified");
//
//
//
//    std::cout << " syn sendToIPMulticast qqqqqqqqqqqqqqqqqqqqqqqqq  \n";
//    std::string mod = "centralScheduler";
//
//    std::set<int> multicastReceivers ;  // to store the receivers IDs  for the: multicastGroupIndex (the index value has been read from the App)
//    std::set<std::string>  nodePodRackLoc;// to store the receivers names'  for the: multicastGroupIndex
//    std::vector<int> receiversPortNumbers;
//    // get receivers names[Pod.Rack.Node]. which will be used to assign connectAddress
//    getRQMain()->getCentralSchedulerModule(state->multicastGroupIndex , multicastReceivers , nodePodRackLoc , receiversPortNumbers);
//
//    for (auto iter= multicastReceivers.begin(); iter!= multicastReceivers.end(); ++iter){
////        std::cout << " kkkkkkkkkk " << *iter  << "   \n";
//    }
//
//    for (auto iter= receiversPortNumbers.begin(); iter!= receiversPortNumbers.end(); ++iter){
////        std::cout << " kkkkkkkkkk pppppppp " << *iter  << "   \n";
//    }
//
//    int i = 0;
////    auto iterrr= receiversPortNumbers.begin();
//    for (auto iter = nodePodRackLoc.begin(); iter != nodePodRackLoc.end();++iter) {
//        std::cout << " kkkkkkkkkk dsdsd" << *iter << "   \n";
//
//
//
////        std::string connectAddress =  "FatTree.Pod[1].racks[0].servers[0]" ;
////                   L3Address destination;
////                   L3AddressResolver().tryResolve(connectAddress.c_str(), destination);
////          std::cout << "ccccccc Connecting to " << remoteAddr     << std::endl;
//         L3AddressResolver().tryResolve(iter->c_str(), remoteAddr);
//         remotePort = receiversPortNumbers.at(i);
//         ++i;
//
//
//         std::cout << "ccccccc remoteAddr " << remoteAddr     << std::endl;
//         std::cout << "ccccccc remotePort " << remotePort     << std::endl;
//
//
//
//
//        // create segment
//        RQSegment *rqseg = createRQSegment("FIRST-ACK-MC");
//        rqseg->setSequenceNo(state->snd_nxt);
////        rqseg->setSynBit(true);
//        rqseg->setAckBit(true);
//        rqseg->setAckNo(state->rcv_nxt);
//
//        updateRcvWnd();
//        rqseg->setWindow(updateRcvWnd());
//        rqseg->setFileId(state->fileId);
//        rqseg->setNumSymbolsToSend(state->numSymbolsToSend);
//
//
//
//        rqseg->setIsMulticastConnSeg(state->isMulticastConn); // multicast
//        rqseg->setMulticastGroupIndex(state->multicastGroupIndex);
//
//        state->snd_max = state->snd_nxt = state->iss + 1;
//        writeHeaderOptions(rqseg);
//
//        rqseg->setSrcPort(localPort);
//        rqseg->setDestPort(remotePort);
//
//        ASSERT(rqseg->getHeaderLength() >= RQ_HEADER_OCTETS); // RQ_HEADER_OCTETS = 20 (without options)
//        ASSERT(rqseg->getHeaderLength() <= RQ_MAX_HEADER_OCTETS); // RQ_MAX_HEADER_OCTETS = 60
//        rqseg->setByteLength(
//                rqseg->getHeaderLength() + rqseg->getPayloadLength());
//        state->sentBytes = rqseg->getPayloadLength(); // resetting sentBytes to 0 if sending a segment without data (e.g. ACK)
//        EV_INFO << "Sending: ";
//        printSegmentBrief(rqseg);
//        // TBD reuse next function for sending
//        IL3AddressType *addressType = remoteAddr.getAddressType();
//        INetworkProtocolControlInfo *controlInfo =   addressType->createNetworkProtocolControlInfo();
//        controlInfo->setTransportProtocol(IP_PROT_TCP); // TODO RQ ???
//        controlInfo->setSourceAddress(localAddr);
//        controlInfo->setDestinationAddress(remoteAddr);
//        rqseg->setControlInfo(check_and_cast<cObject *>(controlInfo));
//
//        ASSERT(rqseg->getHeaderLength() >= RQ_HEADER_OCTETS); // RQ_HEADER_OCTETS = 20 (without options)
//        ASSERT(rqseg->getHeaderLength() <= RQ_MAX_HEADER_OCTETS); // RQ_MAX_HEADER_OCTETS = 60
//        rqseg->setByteLength(  rqseg->getHeaderLength() + rqseg->getPayloadLength());
//
//
//        //    std::cout << " ssendddddd to ippppppp \n";
//        rqMain->send(rqseg, "ipOut");
//    }
//
//    state->numReplica = receiversPortNumbers.size();
//    std::cout << " ssendddddd to ippppppp " << state->numReplica   << " \n\n\n";
//    rqAlgorithm->ackSent();

}




void RQConnection::sendToIP(RQSegment *rqseg) {
    // record seq (only if we do send data) and ackno
    if (sndNxtVector && rqseg->getPayloadLength() != 0)
        sndNxtVector->record(rqseg->getSequenceNo());

    if (sndAckVector)
        sndAckVector->record(rqseg->getAckNo());

    // final touches on the segment before sending
    rqseg->setSrcPort(localPort);
    rqseg->setDestPort(remotePort);
//     std::cout << " ssendddddd to ippppppp 1111\n";


    ASSERT(rqseg->getHeaderLength() >= RQ_HEADER_OCTETS); // RQ_HEADER_OCTETS = 20 (without options)
    ASSERT(rqseg->getHeaderLength() <= RQ_MAX_HEADER_OCTETS); // RQ_MAX_HEADER_OCTETS = 60
    rqseg->setByteLength(rqseg->getHeaderLength() + rqseg->getPayloadLength());
    state->sentBytes = rqseg->getPayloadLength(); // resetting sentBytes to 0 if sending a segment without data (e.g. ACK)

    EV_INFO << "Sending: ";
    printSegmentBrief(rqseg);

    // TBD reuse next function for sending
    IL3AddressType *addressType = remoteAddr.getAddressType();
    INetworkProtocolControlInfo *controlInfo =
            addressType->createNetworkProtocolControlInfo();
    controlInfo->setTransportProtocol(IP_PROT_TCP); // TODO RQ ???
    controlInfo->setSourceAddress(localAddr);
    controlInfo->setDestinationAddress(remoteAddr);
    rqseg->setControlInfo(check_and_cast<cObject *>(controlInfo));

//    std::cout << " ssendddddd to ippppppp \n";
    rqMain->send(rqseg, "ipOut");
}





void RQConnection::sendSynAck() {
    // create segment
    RQSegment *rqseg = createRQSegment("SYN+ACK");
    rqseg->setSequenceNo(state->iss);
    rqseg->setAckNo(state->rcv_nxt);
    rqseg->setSynBit(true);
    rqseg->setAckBit(true);
    updateRcvWnd();
    rqseg->setWindow(state->rcv_wnd);

    state->snd_max = state->snd_nxt = state->iss + 1;
//     std::cout << " sendSynAck    numSymbolsToGet numSymbolsToGet " << state->numSymbolsToGet << std::endl << std::endl << std::endl << std::endl;
    // write header options
    writeHeaderOptions(rqseg);

   // ^^^^^^^^^^^^^^^^^^^ Multicast
     rqseg->setIsMulticastConnSeg(state->isMulticastConn); // multicast
    rqseg->setMulticastGroupIndex(state->multicastGroupIndex);
    rqseg->setIsMulticastSender(false);
    rqseg->setIsMulticastReceiver(true);

    rqseg->setIsMultiSourcingConn(state->isMultiSourcingConn);
    rqseg->setMultiSourcingGroupIndex(state->multiSourcingGroupIndex) ;
    rqseg->setIsMultiSourcingSender(false);
    rqseg->setIsMultiSourcingReceiver(true);

    // send it
    sendToIP(rqseg);

    // notify
    rqAlgorithm->ackSent();
}

// not used
void RQConnection::sendAck() {
    RQSegment *rqseg = createRQSegment("ACK");

    rqseg->setAckBit(true);
    rqseg->setSequenceNo(state->snd_nxt);
    rqseg->setAckNo(state->rcv_nxt);
    rqseg->setWindow(updateRcvWnd());

    // write header options
    writeHeaderOptions(rqseg);

    // send it
    sendToIP(rqseg);  // MOH: HAS BEEN REMOVED

    // notify
    rqAlgorithm->ackSent();
}


// MOH: HAS BEEN ADDED
void RQConnection::sendFirstAck() {
//   std::cout << "sending the first ACK " << std::endl;
    RQSegment *rqseg = createRQSegment("first-ACK");

    rqseg->setAckBit(true);
    rqseg->setSequenceNo(state->snd_nxt);
    rqseg->setAckNo(state->rcv_nxt);
    rqseg->setWindow(updateRcvWnd());
    // MOH
    rqseg->setFileId(state->fileId);

    rqseg->setIsMulticastConnSeg(state->isMulticastConn) ; // multicast
    rqseg->setMulticastGroupIndex(state->multicastGroupIndex);
    rqseg->setIsMulticastSender(state->isSender); // true
    rqseg->setIsMulticastReceiver(false);

    rqseg->setIsMultiSourcingConn(state->isMultiSourcingConn);
    rqseg->setMultiSourcingGroupIndex(state->multiSourcingGroupIndex) ;
    rqseg->setIsMultiSourcingSender(true);
    rqseg->setIsMultiSourcingReceiver(false);

    // write header options
    writeHeaderOptions(rqseg);

    // send it
    sendToIP(rqseg);  // MOH: HAS BEEN REMOVED

    // notify
    rqAlgorithm->ackSent();
}


void RQConnection::sendRequestSegment() {
    char msgname[16];

//    if (state->request_id == 7) {
//        state->request_id = 3;
//        sprintf(msgname, "REQUEST-%d", state->request_id);
//        RQSegment *rqseg = createRQSegment(msgname);
//        rqseg->setPayloadLength(10);
//        rqseg->setIsClient(true);
//        rqseg->setRequestSequenceNumber(state->request_id);
//        //    rqseg->addPayloadMessage(msg->dup(),10);
//        sendToIP(rqseg);
//        state->request_id = 7;
//    }
// std::cout << "RQConnection::sendRequestSegment() \n";
    sprintf(msgname, "REQUEST-%d", state->request_id);
    RQSegment *rqseg = createRQSegment(msgname);
    rqseg->setPayloadLength(10);
    rqseg->setIsClient(true);

    rqseg->setIsReader(true);
   rqseg->setIsWriter(false);
   state->opcodeValue=1;
    rqseg->setRequestSequenceNumber(state->request_id);
    rqseg->setOpcode(state->opcodeValue);
    //    rqseg->addPayloadMessage(msg->dup(),10);
//    sendToIP(rqseg);  // i think not used now ??/
    //   state->snd_nxt=state->snd_nxt+10;
}

// Added MOH
void RQConnection::sendRequest() {
//    std::cout << "void RQConnection::sendRequest() ............... \n  ";
//     ++state->request_id;

EV << " sending a new request \n";

//    char msgname[16];
//    sprintf(msgname, "REQUEST-%d", ++state->request_id);
//    GenericAppMsgRq *msg = new GenericAppMsgRq(msgname);
//    int requestLength = 10;
//    int replyLength = state->snd_mss;
//    msg->setByteLength(requestLength);
//    msg->setExpectedReplyLength(replyLength);
//    msg->setServerClose(false);
//    msg->setIsClient(true);  // client 1, server 0
//    std::cout << " SENDREQUEST: state->request_id............... " << state->request_id   << "\n ";

    // losing requests 3 & 4 : just for testing
   // if (!(state->request_id == 3 || state->request_id == 4)) {
//        std::cout << "  state->request_id.............. " << state->request_id << "\n ";



//        if (state->numSymbolsToGet > 0) // TODO it was a bug
            sendRequestSegment();
  //  }

//    if (state->numSymbolsToGet > 0) {
//        simtime_t d = simTime() + 1;                // time gap between requests
//        rqMain->scheduleAt(d, requestInternalTimer);
//    }

}

bool RQConnection::sendData(bool fullSegmentsOnly, uint32 congestionWindow, bool isReader ,bool isWriter,  unsigned int opcode) {
// std::cout << " sendData opcode = " << opcode << std::endl;
// std::cout << " ggggg"<< cSimulation::getActiveSimulation()->getContextSimpleModule()->getFullPath() << std::endl;
    state->opcodeValue=opcode;
//    state->numSymbolsToSend = numSymbolsToSend;

//    std::cout << " RQConnection::sendData,,  isReader " << isReader << " , isWriter "  << isWriter << std::endl ;


//    if (isClient) { //  receiver
    if (opcode==1 && isWriter==false && isReader==true) { // 1 Read
//        sendRequest(); // send the first request to get the whole initial window (=12 symbols NDP)

        //        startRequestRexmitTimer();
//        std::cout << " opcode =  1   Reader Send initial request  \n" ;

        return true;
    } else if (opcode==2 && isWriter==true && isReader==false ) { //  sender 2 Write
//        std::cout << " opcode =  2   Writer \n" ;
        generateSymbolsList();
        sendInitialWindow();
////
////        RQSegment *rqseg = sendQueue->createSegmentWithBytes(state->snd_nxt, 1000);
////        sendToIP(rqseg);
////        state->snd_nxt=state->snd_nxt+1000;
//
//        ulong bytesToSend = state->snd_mss;
//        if (bytesToSend <= state->snd_mss)
//            sendSegment(bytesToSend);
//        //  state->snd_nxt=state->snd_nxt+1000;
//
         return true;
    }

}



























const char *RQConnection::optionName(int option) {
    switch (option) {
    case RQOPTION_END_OF_OPTION_LIST:
        return "EOL";

    case RQOPTION_NO_OPERATION:
        return "NOP";

    case RQOPTION_MAXIMUM_SEGMENT_SIZE:
        return "MSS";

    case RQOPTION_WINDOW_SCALE:
        return "WS";

    case RQOPTION_SACK_PERMITTED:
        return "SACK_PERMITTED";

    case RQOPTION_SACK:
        return "SACK";

    case RQOPTION_TIMESTAMP:
        return "TS";

    default:
        return "unknown";
    }
}

void RQConnection::printConnBrief() const {
    EV_DETAIL << "Connection " << localAddr << ":" << localPort << " to "
                     << remoteAddr << ":" << remotePort << "  on app["
                     << appGateIndex << "], connId=" << connId << "  in "
                     << stateName(fsm.getState()) << "\n";
}

void RQConnection::printSegmentBrief(RQSegment *rqseg) {
    EV_INFO << "." << rqseg->getSrcPort() << " > ";
    EV_INFO << "." << rqseg->getDestPort() << ": ";

    if (rqseg->getSynBit())
        EV_INFO << (rqseg->getAckBit() ? "SYN+ACK " : "SYN ");

    if (rqseg->getFinBit())
        EV_INFO << "FIN(+ACK) ";

    if (rqseg->getRstBit())
        EV_INFO << (rqseg->getAckBit() ? "RST+ACK " : "RST ");

    if (rqseg->getPshBit())
        EV_INFO << "PSH ";

    if (rqseg->getPayloadLength() > 0 || rqseg->getSynBit()) {
        EV_INFO << "[" << rqseg->getSequenceNo() << ".."
                       << (rqseg->getSequenceNo() + rqseg->getPayloadLength())
                       << ") ";
        EV_INFO << "(l=" << rqseg->getPayloadLength() << ") ";
    }

    if (rqseg->getAckBit())
        EV_INFO << "ack " << rqseg->getAckNo() << " ";

    EV_INFO << "win " << rqseg->getWindow() << " ";

    if (rqseg->getUrgBit())
        EV_INFO << "urg " << rqseg->getUrgentPointer() << " ";

    if (rqseg->getHeaderLength() > RQ_HEADER_OCTETS) { // Header options present? RQ_HEADER_OCTETS = 20
        EV_INFO << "options ";

        for (uint i = 0; i < rqseg->getHeaderOptionArraySize(); i++) {
            const RQOption *option = rqseg->getHeaderOption(i);
            short kind = option->getKind();
            EV_INFO << optionName(kind) << " ";
        }
    }
    EV_INFO << "\n";
}

RQConnection *RQConnection::cloneListeningConnection() {
    RQConnection *conn = new RQConnection(rqMain, appGateIndex, connId);

    conn->transferMode = transferMode;
    // following code to be kept consistent with initConnection()
    const char *sendQueueClass = sendQueue->getClassName();
    conn->sendQueue = check_and_cast<RQSendQueue *>(
            inet::utils::createOne(sendQueueClass));
    conn->sendQueue->setConnection(conn);

    const char *receiveQueueClass = receiveQueue->getClassName();
    conn->receiveQueue = check_and_cast<RQReceiveQueue *>(
            inet::utils::createOne(receiveQueueClass));
    conn->receiveQueue->setConnection(conn);

    // create SACK retransmit queue
//    conn->rexmitQueue = new RQSACKRexmitQueue();
//    conn->rexmitQueue->setConnection(this);

    const char *rqAlgorithmClass = rqAlgorithm->getClassName();
    conn->rqAlgorithm = check_and_cast<RQAlgorithm *>(
            inet::utils::createOne(rqAlgorithmClass));
    conn->rqAlgorithm->setConnection(conn);

    conn->state = conn->rqAlgorithm->getStateVariables();
    configureStateVariables();
    conn->rqAlgorithm->initialize();

    // put it into LISTEN, with our localAddr/localPort
    conn->state->active = false;
    conn->state->fork = true;
    conn->localAddr = localAddr;
    conn->localPort = localPort;
    FSM_Goto(conn->fsm, RQ_S_LISTEN);

    return conn;
}

void RQConnection::sendRst(uint32 seqNo) {
    sendRst(seqNo, localAddr, remoteAddr, localPort, remotePort);
}

void RQConnection::sendRst(uint32 seq, L3Address src, L3Address dest,
        int srcPort, int destPort) {
    RQSegment *rqseg = createRQSegment("RST");

    rqseg->setSrcPort(srcPort);
    rqseg->setDestPort(destPort);

    rqseg->setRstBit(true);
    rqseg->setSequenceNo(seq);

    // send it
    sendToIP(rqseg, src, dest);
}

void RQConnection::sendRstAck(uint32 seq, uint32 ack, L3Address src,
        L3Address dest, int srcPort, int destPort) {
    RQSegment *rqseg = createRQSegment("RST+ACK");

    rqseg->setSrcPort(srcPort);
    rqseg->setDestPort(destPort);

    rqseg->setRstBit(true);
    rqseg->setAckBit(true);
    rqseg->setSequenceNo(seq);
    rqseg->setAckNo(ack);

    // send it
    sendToIP(rqseg, src, dest);

    // notify
    if (rqAlgorithm)
        rqAlgorithm->ackSent();
}


void RQConnection::sendFin() {
    RQSegment *rqseg = createRQSegment("FIN");

    // Note: ACK bit *must* be set for both FIN and FIN+ACK. What makes
    // the difference for FIN+ACK is that its ackNo acks the remote RQ's FIN.
    rqseg->setFinBit(true);
    rqseg->setAckBit(true);
    rqseg->setAckNo(state->rcv_nxt);
    rqseg->setSequenceNo(state->snd_nxt);
    rqseg->setWindow(updateRcvWnd());

    // send it
    sendToIP(rqseg);

    // notify
    rqAlgorithm->ackSent();
}


void RQConnection::sendSegment(uint32 bytes) {
    if (state->sack_enabled && state->afterRto) {
        // check rexmitQ and try to forward snd_nxt before sending new data
        //   uint32 forward = rexmitQueue->checkRexmitQueueForSackedOrRexmittedSegments(state->snd_nxt);

        // if (forward > 0) {
        //     EV_INFO << "sendSegment(" << bytes << ") forwarded " << forward << " bytes of snd_nxt from " << state->snd_nxt;
        //     state->snd_nxt += forward;
        //    EV_INFO << " to " << state->snd_nxt << endl;
        //      rexmitQueue->info();
        //  }
    }

    ulong buffered = sendQueue->getBytesAvailable(state->snd_nxt);

    if (bytes > buffered) // last segment?
        bytes = buffered;

    // if header options will be added, this could reduce the number of data bytes allowed for this segment,
    // because following condition must to be respected:
    //     bytes + options_len <= snd_mss
    RQSegment *rqseg_temp = createRQSegment(nullptr);
    rqseg_temp->setAckBit(true); // needed for TS option, otherwise TSecr will be set to 0
    writeHeaderOptions(rqseg_temp);
    uint options_len = rqseg_temp->getHeaderLength() - RQ_HEADER_OCTETS; // RQ_HEADER_OCTETS = 20

    ASSERT(options_len < state->snd_mss);

    if (bytes + options_len > state->snd_mss)
        bytes = state->snd_mss - options_len;

    state->sentBytes = bytes;

    // send one segment of 'bytes' bytes from snd_nxt, and advance snd_nxt
    RQSegment *rqseg = sendQueue->createSegmentWithBytes(state->snd_nxt, bytes);

    // if sack_enabled copy region of rqseg to rexmitQueue
    //   if (state->sack_enabled)
    //       rexmitQueue->enqueueSentData(state->snd_nxt, state->snd_nxt + bytes);

    rqseg->setAckNo(state->rcv_nxt); //  hhhhhhhhhhhhh
    rqseg->setAckBit(true);         //
    rqseg->setWindow(updateRcvWnd());

//    rqseg->setIsClient(); // Added: to be used by the receiver

    // TBD when to set PSH bit?
    // TBD set URG bit if needed
    ASSERT(bytes == rqseg->getPayloadLength());

    state->snd_nxt += bytes;

    // check if afterRto bit can be reset
    if (state->afterRto && seqGE(state->snd_nxt, state->snd_max))
        state->afterRto = false;

    if (state->send_fin && state->snd_nxt == state->snd_fin_seq) {
        EV_DETAIL << "Setting FIN on segment\n";
        rqseg->setFinBit(true);
        state->snd_nxt = state->snd_fin_seq + 1;
    }

    // add header options and update header length (from rqseg_temp)
    for (uint i = 0; i < rqseg_temp->getHeaderOptionArraySize(); i++)
        rqseg->addHeaderOption(rqseg_temp->getHeaderOption(i)->dup());

    rqseg->setHeaderLength(rqseg_temp->getHeaderLength());
    delete rqseg_temp;

    // send it
    rqseg->setIsClient(false); // we send symbol segment "(server=sender)
    rqseg->setIsReader(false);
   rqseg->setIsWriter(true);


   sendToIP(rqseg);


    // let application fill queue again, if there is space
    const uint32 alreadyQueued = sendQueue->getBytesAvailable(
            sendQueue->getBufferStartSeq());
    const uint32 abated =
            (state->sendQueueLimit > alreadyQueued) ?
                    state->sendQueueLimit - alreadyQueued : 0;
    if ((state->sendQueueLimit > 0) && !state->queueUpdate
            && (abated >= state->snd_mss)) { // request more data if space >= 1 MSS
        // Tell upper layer readiness to accept more data
        sendIndicationToApp(RQ_I_SEND_MSG, abated);
        state->queueUpdate = true;
    }
}


//bool RQConnection::isSegmentAcceptable(RQSegment *rqseg) const {
//    // check that segment entirely falls in receive window
//    // RFC 793, page 69:
//    // "There are four cases for the acceptability test for an incoming segment:
//    //    Segment Receive  Test
//    //    Length  Window
//    //    ------- -------  -------------------------------------------
//    //       0       0     SEG.SEQ = RCV.NXT
//    //       0      >0     RCV.NXT =< SEG.SEQ < RCV.NXT+RCV.WND
//    //      >0       0     not acceptable
//    //      >0      >0     RCV.NXT =< SEG.SEQ < RCV.NXT+RCV.WND
//    //                  or RCV.NXT =< SEG.SEQ+SEG.LEN-1 < RCV.NXT+RCV.WND"
//    uint32 len = rqseg->getPayloadLength();
//    uint32 seqNo = rqseg->getSequenceNo();
//    uint32 ackNo = rqseg->getAckNo();
//    uint32 rcvWndEnd = state->rcv_nxt + state->rcv_wnd;
//    bool ret;
//
//    if (len == 0) {
//        if (state->rcv_wnd == 0)
//            ret = (seqNo == state->rcv_nxt);
//        else
//            // rcv_wnd > 0
////            ret = seqLE(state->rcv_nxt, seqNo) && seqLess(seqNo, rcvWndEnd);
//            ret = seqLE(state->rcv_nxt, seqNo) && seqLE(seqNo, rcvWndEnd); // Accept an ACK on end of window
//    } else {    // len > 0
//        if (state->rcv_wnd == 0)
//            ret = false;
//        else
//            // rcv_wnd > 0
//            ret = (seqLE(state->rcv_nxt, seqNo) && seqLess(seqNo, rcvWndEnd))
//                    || (seqLess(state->rcv_nxt, seqNo + len)
//                            && seqLE(seqNo + len, rcvWndEnd)); // Accept an ACK on end of window
//    }
//
//    // RFC 793, page 25:
//    // "A new acknowledgment (called an "acceptable ack"), is one for which
//    // the inequality below holds:
//    //    SND.UNA < SEG.ACK =< SND.NXT"
//    if (!ret && len == 0) {
//        if (!state->afterRto)
//            ret = (seqLess(state->snd_una, ackNo)
//                    && seqLE(ackNo, state->snd_nxt));
//        else
//            ret = (seqLess(state->snd_una, ackNo)
//                    && seqLE(ackNo, state->snd_max)); // after RTO snd_nxt is reduced therefore we need to use snd_max instead of snd_nxt here
//    }
//
//    if (!ret)
//        EV_WARN << "Not Acceptable segment. seqNo=" << seqNo << " ackNo="
//                       << ackNo << " len=" << len << " rcv_nxt="
//                       << state->rcv_nxt << " rcv_wnd=" << state->rcv_wnd
//                       << " afterRto=" << state->afterRto << "\n";
//
//    return ret;
//}

bool RQConnection::sendProbe() {
    // we'll start sending from snd_max
    state->snd_nxt = state->snd_max;

    // check we have 1 byte to send
    if (sendQueue->getBytesAvailable(state->snd_nxt) == 0) {
        EV_WARN << "Cannot send probe because send buffer is empty\n";
        return false;
    }

    uint32 old_snd_nxt = state->snd_nxt;

    EV_INFO << "Sending 1 byte as probe, with seq=" << state->snd_nxt << "\n";
    sendSegment(1);

    // remember highest seq sent (snd_nxt may be set back on retransmission,
    // but we'll need snd_max to check validity of ACKs -- they must ack
    // something we really sent)
    state->snd_max = state->snd_nxt;

    if (unackedVector)
        unackedVector->record(state->snd_max - state->snd_una);

    // notify
    rqAlgorithm->ackSent();
    rqAlgorithm->dataSent(old_snd_nxt);

    return true;
}

void RQConnection::retransmitOneSegment(bool called_at_rto) {
    uint32 old_snd_nxt = state->snd_nxt;

    // retransmit one segment at snd_una, and set snd_nxt accordingly (if not called at RTO)
    state->snd_nxt = state->snd_una;

    // When FIN sent the snd_max - snd_nxt larger than bytes available in queue
    ulong bytes = std::min(
            (ulong) std::min(state->snd_mss, state->snd_max - state->snd_nxt),
            sendQueue->getBytesAvailable(state->snd_nxt));

    // FIN (without user data) needs to be resent
    if (bytes == 0 && state->send_fin
            && state->snd_fin_seq == sendQueue->getBufferEndSeq()) {
        state->snd_max = sendQueue->getBufferEndSeq();
        EV_DETAIL
                         << "No outstanding DATA, resending FIN, advancing snd_nxt over the FIN\n";
        state->snd_nxt = state->snd_max;
        sendFin();
        rqAlgorithm->segmentRetransmitted(state->snd_nxt, state->snd_nxt + 1);
        state->snd_max = ++state->snd_nxt;

        if (unackedVector)
            unackedVector->record(state->snd_max - state->snd_una);
    } else {
        ASSERT(bytes != 0);

        sendSegment(bytes);
        rqAlgorithm->segmentRetransmitted(state->snd_una, state->snd_nxt);

        if (!called_at_rto) {
            if (seqGreater(old_snd_nxt, state->snd_nxt))
                state->snd_nxt = old_snd_nxt;
        }

        // notify
        rqAlgorithm->ackSent();

        //  if (state->sack_enabled) {
        // RFC 3517, page 7: "(3) Retransmit the first data segment presumed dropped -- the segment
        // starting with sequence number HighACK + 1.  To prevent repeated
        // retransmission of the same data, set HighRxt to the highest
        // sequence number in the retransmitted segment."
        //        state->highRxt = rexmitQueue->getHighestRexmittedSeqNum();
        // }
    }
}

void RQConnection::retransmitData() {
    // retransmit everything from snd_una
    state->snd_nxt = state->snd_una;

    uint32 bytesToSend = state->snd_max - state->snd_nxt;

    // FIN (without user data) needs to be resent
    if (bytesToSend == 0 && state->send_fin
            && state->snd_fin_seq == sendQueue->getBufferEndSeq()) {
        state->snd_max = sendQueue->getBufferEndSeq();
        EV_DETAIL
                         << "No outstanding DATA, resending FIN, advancing snd_nxt over the FIN\n";
        state->snd_nxt = state->snd_max;
        sendFin();
        state->snd_max = ++state->snd_nxt;

        if (unackedVector)
            unackedVector->record(state->snd_max - state->snd_una);
        return;
    }

    ASSERT(bytesToSend != 0);

    // TBD - avoid to send more than allowed - check cwnd and rwnd before retransmitting data!
    while (bytesToSend > 0) {
        uint32 bytes = std::min(bytesToSend, state->snd_mss);
        bytes = std::min(bytes,
                (uint32) (sendQueue->getBytesAvailable(state->snd_nxt)));
        sendSegment(bytes);

        // Do not send packets after the FIN.
        // fixes bug that occurs in examples/inet/bulktransfer at event #64043  T=13.861159213744
        if (state->send_fin && state->snd_nxt == state->snd_fin_seq + 1)
            break;

        bytesToSend -= state->sentBytes;
    }
    rqAlgorithm->segmentRetransmitted(state->snd_una, state->snd_nxt);
}

void RQConnection::readHeaderOptions(RQSegment *rqseg) {
    EV_INFO << "RQ Header Option(s) received:\n";

    for (uint i = 0; i < rqseg->getHeaderOptionArraySize(); i++) {
        const RQOption *option = rqseg->getHeaderOption(i);
        short kind = option->getKind();
        short length = option->getLength();
        bool ok = true;

        EV_DETAIL << "Option type " << kind << " (" << optionName(kind)
                         << "), length " << length << "\n";

        switch (kind) {
        case RQOPTION_END_OF_OPTION_LIST:    // EOL=0
        case RQOPTION_NO_OPERATION:    // NOP=1
            if (length != 1) {
                EV_ERROR << "ERROR: option length incorrect\n";
                ok = false;
            }
            break;

        case RQOPTION_MAXIMUM_SEGMENT_SIZE:    // MSS=2
            ok = processMSSOption(rqseg,
                    *check_and_cast<const RQOptionMaxSegmentSize *>(option));
            break;

        case RQOPTION_WINDOW_SCALE:    // WS=3
            ok = processWSOption(rqseg,
                    *check_and_cast<const RQOptionWindowScale *>(option));
            break;

        case RQOPTION_SACK_PERMITTED:    // SACK_PERMITTED=4
//            ok = processSACKPermittedOption(rqseg,
//                    *check_and_cast<const RQOptionSackPermitted *>(option));
            break;

        case RQOPTION_SACK:    // SACK=5
//            ok = processSACKOption(rqseg,
//                    *check_and_cast<const RQOptionSack *>(option));
            break;

        case RQOPTION_TIMESTAMP:    // TS=8
            ok = processTSOption(rqseg,
                    *check_and_cast<const RQOptionTimestamp *>(option));
            break;

            // TODO add new RQOptions here once they are implemented
            // TODO delegate to RQAlgorithm as well -- it may want to recognized additional options

        default:
            EV_ERROR << "ERROR: Unsupported RQ option kind " << kind << "\n";
            break;
        }
        (void) ok;    // unused
    }
}

bool RQConnection::processMSSOption(RQSegment *rqseg,
        const RQOptionMaxSegmentSize& option) {
    if (option.getLength() != 4) {
        EV_ERROR << "ERROR: MSS option length incorrect\n";
        return false;
    }

    if (fsm.getState() != RQ_S_LISTEN && fsm.getState() != RQ_S_SYN_SENT) {
        EV_ERROR
                        << "ERROR: RQ Header Option MSS received, but in unexpected state\n";
        return false;
    }

    // RFC 2581, page 1:
    // "The SMSS is the size of the largest segment that the sender can transmit.
    // This value can be based on the maximum transmission unit of the network,
    // the path MTU discovery [MD90] algorithm, RMSS (see next item), or other
    // factors.  The size does not include the RQ/IP headers and options."
    //
    // "The RMSS is the size of the largest segment the receiver is willing to accept.
    // This is the value specified in the MSS option sent by the receiver during
    // connection startup.  Or, if the MSS option is not used, 536 bytes [Bra89].
    // The size does not include the RQ/IP headers and options."
    //
    //
    // The value of snd_mss (SMSS) is set to the minimum of snd_mss (local parameter) and
    // the value specified in the MSS option received during connection startup.
    state->snd_mss = std::min(state->snd_mss,
            (uint32) option.getMaxSegmentSize());

    if (state->snd_mss == 0)
        state->snd_mss = 536;

    EV_INFO << "RQ Header Option MSS(=" << option.getMaxSegmentSize()
                   << ") received, SMSS is set to " << state->snd_mss << "\n";
    return true;
}

bool RQConnection::processWSOption(RQSegment *rqseg,
        const RQOptionWindowScale& option) {
    if (option.getLength() != 3) {
        EV_ERROR << "ERROR: length incorrect\n";
        return false;
    }

    if (fsm.getState() != RQ_S_LISTEN && fsm.getState() != RQ_S_SYN_SENT) {
        EV_ERROR
                        << "ERROR: RQ Header Option WS received, but in unexpected state\n";
        return false;
    }

    state->rcv_ws = true;
    state->ws_enabled = state->ws_support && state->snd_ws && state->rcv_ws;
    state->snd_wnd_scale = option.getWindowScale();
    EV_INFO << "RQ Header Option WS(=" << state->snd_wnd_scale
                   << ") received, WS (ws_enabled) is set to "
                   << state->ws_enabled << "\n";

    if (state->snd_wnd_scale > 14) { // RFC 1323, page 11: "the shift count must be limited to 14"
        EV_ERROR
                        << "ERROR: RQ Header Option WS received but shift count value is exceeding 14\n";
        state->snd_wnd_scale = 14;
    }

    return true;
}

bool RQConnection::processTSOption(RQSegment *rqseg,
        const RQOptionTimestamp& option) {
    if (option.getLength() != 10) {
        EV_ERROR << "ERROR: length incorrect\n";
        return false;
    }

    if ((!state->ts_enabled && fsm.getState() != RQ_S_LISTEN
            && fsm.getState() != RQ_S_SYN_SENT)
            || (state->ts_enabled && fsm.getState() != RQ_S_SYN_RCVD
                    && fsm.getState() != RQ_S_ESTABLISHED
                    && fsm.getState() != RQ_S_FIN_WAIT_1
                    && fsm.getState() != RQ_S_FIN_WAIT_2)) {
        EV_ERROR
                        << "ERROR: RQ Header Option TS received, but in unexpected state\n";
        return false;
    }

    if (!state->ts_enabled) {
        state->rcv_initial_ts = true;
        state->ts_enabled = state->ts_support && state->snd_initial_ts
                && state->rcv_initial_ts;
        EV_INFO << "RQ Header Option TS(TSval=" << option.getSenderTimestamp()
                       << ", TSecr=" << option.getEchoedTimestamp()
                       << ") received, TS (ts_enabled) is set to "
                       << state->ts_enabled << "\n";
    } else
        EV_INFO << "RQ Header Option TS(TSval=" << option.getSenderTimestamp()
                       << ", TSecr=" << option.getEchoedTimestamp()
                       << ") received\n";

    // RFC 1323, page 35:
    // "Check whether the segment contains a Timestamps option and bit
    // Snd.TS.OK is on.  If so:
    //   If SEG.TSval < TS.Recent, then test whether connection has
    //   been idle less than 24 days; if both are true, then the
    //   segment is not acceptable; follow steps below for an
    //   unacceptable segment.
    //   If SEG.SEQ is equal to Last.ACK.sent, then save SEG.[TSval] in
    //   variable TS.Recent."
    if (state->ts_enabled) {
        if (seqLess(option.getSenderTimestamp(), state->ts_recent)) {
            if ((simTime() - state->time_last_data_sent) > PAWS_IDLE_TIME_THRESH) { // PAWS_IDLE_TIME_THRESH = 24 days
                EV_DETAIL << "PAWS: Segment is not acceptable, TSval="
                                 << option.getSenderTimestamp() << " in "
                                 << stateName(fsm.getState())
                                 << " state received: dropping segment\n";
                return false;
            }
        } else if (seqLE(rqseg->getSequenceNo(), state->last_ack_sent)) { // Note: test is modified according to the latest proposal of the rqlw@cray.com list (Braden 1993/04/26)
            state->ts_recent = option.getSenderTimestamp();
            EV_DETAIL << "Updating ts_recent from segment: new ts_recent="
                             << state->ts_recent << "\n";
        }
    }

    return true;
}

//bool RQConnection::processSACKPermittedOption(RQSegment *rqseg,
//        const RQOptionSackPermitted& option) {
//    if (option.getLength() != 2) {
//        EV_ERROR << "ERROR: length incorrect\n";
//        return false;
//    }
//
//    if (fsm.getState() != RQ_S_LISTEN && fsm.getState() != RQ_S_SYN_SENT) {
//        EV_ERROR
//                        << "ERROR: RQ Header Option SACK_PERMITTED received, but in unexpected state\n";
//        return false;
//    }
//
//    state->rcv_sack_perm = true;
//    state->sack_enabled = state->sack_support && state->snd_sack_perm
//            && state->rcv_sack_perm;
//    EV_INFO
//                   << "RQ Header Option SACK_PERMITTED received, SACK (sack_enabled) is set to "
//                   << state->sack_enabled << "\n";
//    return true;
//}

RQSegment RQConnection::writeHeaderOptions(RQSegment *rqseg) {
    // SYN flag set and connetion in INIT or LISTEN state (or after synRexmit timeout)
    if (rqseg->getSynBit()  && (fsm.getState() == RQ_S_INIT || fsm.getState() == RQ_S_LISTEN
                    || ((fsm.getState() == RQ_S_SYN_SENT
                            || fsm.getState() == RQ_S_SYN_RCVD)
                            && state->syn_rexmit_count > 0))) {
        // MSS header option
        if (state->snd_mss > 0) {
            RQOptionMaxSegmentSize *option = new RQOptionMaxSegmentSize();
            option->setMaxSegmentSize(state->snd_mss);
            rqseg->addHeaderOption(option);
            EV_INFO << "RQ Header Option MSS(=" << state->snd_mss << ") sent\n";
        }

        // WS header option
        if (state->ws_support
                && (state->rcv_ws
                        || (fsm.getState() == RQ_S_INIT
                                || (fsm.getState() == RQ_S_SYN_SENT
                                        && state->syn_rexmit_count > 0)))) {
            // 1 padding byte
            rqseg->addHeaderOption(new RQOptionNop());    // NOP

            // Update WS variables
            if (state->ws_manual_scale > -1) {
                state->rcv_wnd_scale = state->ws_manual_scale;
            } else {
                ulong scaled_rcv_wnd = receiveQueue->getFirstSeqNo()
                        + state->maxRcvBuffer - state->rcv_nxt;
                state->rcv_wnd_scale = 0;

                while (scaled_rcv_wnd > RQ_MAX_WIN && state->rcv_wnd_scale < 14) { // RFC 1323, page 11: "the shift count must be limited to 14"
                    scaled_rcv_wnd = scaled_rcv_wnd >> 1;
                    state->rcv_wnd_scale++;
                }
            }

            RQOptionWindowScale *option = new RQOptionWindowScale();
            option->setWindowScale(state->rcv_wnd_scale); // rcv_wnd_scale is also set in scaleRcvWnd()
            state->snd_ws = true;
            state->ws_enabled = state->ws_support && state->snd_ws
                    && state->rcv_ws;
            EV_INFO << "RQ Header Option WS(=" << option->getWindowScale()
                           << ") sent, WS (ws_enabled) is set to "
                           << state->ws_enabled << "\n";
            rqseg->addHeaderOption(option);
        }

        // SACK_PERMITTED header option
//        if (state->sack_support
//                && (state->rcv_sack_perm
//                        || (fsm.getState() == RQ_S_INIT
//                                || (fsm.getState() == RQ_S_SYN_SENT
//                                        && state->syn_rexmit_count > 0)))) {
//            if (!state->ts_support) { // if TS is supported by host, do not add NOPs to this segment
//                // 2 padding bytes
//                rqseg->addHeaderOption(new RQOptionNop());    // NOP
//                rqseg->addHeaderOption(new RQOptionNop());    // NOP
//            }
//
//            rqseg->addHeaderOption(new RQOptionSackPermitted());
//
//            // Update SACK variables
//            state->snd_sack_perm = true;
//            state->sack_enabled = state->sack_support && state->snd_sack_perm
//                    && state->rcv_sack_perm;
//            EV_INFO
//                           << "RQ Header Option SACK_PERMITTED sent, SACK (sack_enabled) is set to "
//                           << state->sack_enabled << "\n";
//        }

        // TS header option
        if (state->ts_support
                && (state->rcv_initial_ts
                        || (fsm.getState() == RQ_S_INIT
                                || (fsm.getState() == RQ_S_SYN_SENT
                                        && state->syn_rexmit_count > 0)))) {
//            if (!state->sack_support) { // if SACK is supported by host, do not add NOPs to this segment
//                // 2 padding bytes
//                rqseg->addHeaderOption(new RQOptionNop());    // NOP
//                rqseg->addHeaderOption(new RQOptionNop());    // NOP
//            }

            RQOptionTimestamp *option = new RQOptionTimestamp();

            // Update TS variables
            // RFC 1323, page 13: "The Timestamp Value field (TSval) contains the current value of the timestamp clock of the RQ sending the option."
            option->setSenderTimestamp(convertSimtimeToTS(simTime()));

            // RFC 1323, page 16: "(3) When a TSopt is sent, its TSecr field is set to the current TS.Recent value."
            // RFC 1323, page 13:
            // "The Timestamp Echo Reply field (TSecr) is only valid if the ACK
            // bit is set in the RQ header; if it is valid, it echos a times-
            // tamp value that was sent by the remote RQ in the TSval field
            // of a Timestamps option.  When TSecr is not valid, its value
            // must be zero."
            option->setEchoedTimestamp(
                    rqseg->getAckBit() ? state->ts_recent : 0);

            state->snd_initial_ts = true;
            state->ts_enabled = state->ts_support && state->snd_initial_ts
                    && state->rcv_initial_ts;
            EV_INFO << "RQ Header Option TS(TSval="
                           << option->getSenderTimestamp() << ", TSecr="
                           << option->getEchoedTimestamp()
                           << ") sent, TS (ts_enabled) is set to "
                           << state->ts_enabled << "\n";
            rqseg->addHeaderOption(option);
        }

        // TODO add new RQOptions here once they are implemented
    } else if (fsm.getState() == RQ_S_SYN_SENT
            || fsm.getState() == RQ_S_SYN_RCVD
            || fsm.getState() == RQ_S_ESTABLISHED
            || fsm.getState() == RQ_S_FIN_WAIT_1
            || fsm.getState() == RQ_S_FIN_WAIT_2) // connetion is not in INIT or LISTEN state
                    {
        // TS header option
        if (state->ts_enabled) {    // Is TS enabled?
//            if (!(state->sack_enabled && (state->snd_sack || state->snd_dsack))) { // if SACK is enabled and SACKs need to be added, do not add NOPs to this segment
//                // 2 padding bytes
//                rqseg->addHeaderOption(new RQOptionNop());    // NOP
//                rqseg->addHeaderOption(new RQOptionNop());    // NOP
//            }

            RQOptionTimestamp *option = new RQOptionTimestamp();

            // Update TS variables
            // RFC 1323, page 13: "The Timestamp Value field (TSval) contains the current value of the timestamp clock of the RQ sending the option."
            option->setSenderTimestamp(convertSimtimeToTS(simTime()));

            // RFC 1323, page 16: "(3) When a TSopt is sent, its TSecr field is set to the current TS.Recent value."
            // RFC 1323, page 13:
            // "The Timestamp Echo Reply field (TSecr) is only valid if the ACK
            // bit is set in the RQ header; if it is valid, it echos a times-
            // tamp value that was sent by the remote RQ in the TSval field
            // of a Timestamps option.  When TSecr is not valid, its value
            // must be zero."
            option->setEchoedTimestamp(
                    rqseg->getAckBit() ? state->ts_recent : 0);

            EV_INFO << "RQ Header Option TS(TSval="
                           << option->getSenderTimestamp() << ", TSecr="
                           << option->getEchoedTimestamp() << ") sent\n";
            rqseg->addHeaderOption(option);
        }

        // SACK header option

        // RFC 2018, page 4:
        // "If sent at all, SACK options SHOULD be included in all ACKs which do
        // not ACK the highest sequence number in the data receiver's queue.  In
        // this situation the network has lost or mis-ordered data, such that
        // the receiver holds non-contiguous data in its queue.  RFC 1122,
        // Section 4.2.2.21, discusses the reasons for the receiver to send ACKs
        // in response to additional segments received in this state.  The
        // receiver SHOULD send an ACK for every valid segment that arrives
        // containing new data, and each of these "duplicate" ACKs SHOULD bear a
        // SACK option."
//        if (state->sack_enabled && (state->snd_sack || state->snd_dsack)) {
//            addSacks(rqseg);
//        }

        // TODO add new RQOptions here once they are implemented
        // TODO delegate to RQAlgorithm as well -- it may want to append additional options
    }

    if (rqseg->getHeaderOptionArraySize() != 0) {
        uint options_len = rqseg->getHeaderOptionArrayLength();

        if (options_len <= RQ_OPTIONS_MAX_SIZE) // Options length allowed? - maximum: 40 Bytes
            rqseg->setHeaderLength(RQ_HEADER_OCTETS + options_len); // RQ_HEADER_OCTETS = 20
        else {
            rqseg->setHeaderLength(RQ_HEADER_OCTETS);   // RQ_HEADER_OCTETS = 20
            rqseg->dropHeaderOptions();    // drop all options
            EV_ERROR
                            << "ERROR: Options length exceeded! Segment will be sent without options"
                            << "\n";
        }
    }

    return *rqseg;
}

uint32 RQConnection::getTSval(RQSegment *rqseg) const {
    for (uint i = 0; i < rqseg->getHeaderOptionArraySize(); i++) {
        const RQOption *option = rqseg->getHeaderOption(i);
        if (option->getKind() == RQOPTION_TIMESTAMP)
            return check_and_cast<const RQOptionTimestamp *>(option)->getSenderTimestamp();
    }

    return 0;
}

uint32 RQConnection::getTSecr(RQSegment *rqseg) const {
    for (uint i = 0; i < rqseg->getHeaderOptionArraySize(); i++) {
        const RQOption *option = rqseg->getHeaderOption(i);
        if (option->getKind() == RQOPTION_TIMESTAMP)
            return check_and_cast<const RQOptionTimestamp *>(option)->getEchoedTimestamp();
    }

    return 0;
}

void RQConnection::updateRcvQueueVars() {
    // update receive queue related state variables
    state->freeRcvBuffer = receiveQueue->getAmountOfFreeBytes(
            state->maxRcvBuffer);
    state->usedRcvBuffer = state->maxRcvBuffer - state->freeRcvBuffer;

    // update receive queue related statistics
    if (rqRcvQueueBytesVector)
        rqRcvQueueBytesVector->record(state->usedRcvBuffer);

//    rqEV << "receiveQ: receiveQLength=" << receiveQueue->getQueueLength() << " maxRcvBuffer=" << state->maxRcvBuffer << " usedRcvBuffer=" << state->usedRcvBuffer << " freeRcvBuffer=" << state->freeRcvBuffer << "\n";
}

unsigned short RQConnection::updateRcvWnd() {
    uint32 win = 0;

    // update receive queue related state variables and statistics
    updateRcvQueueVars();
    win = state->freeRcvBuffer;

    // Following lines are based on [Stevens, W.R.: RQ/IP Illustrated, Volume 2, chapter 26.7, pages 878-879]:
    // Don't advertise less than one full-sized segment to avoid SWS
    if (win < (state->maxRcvBuffer / 4) && win < state->snd_mss)
        win = 0;

    // Do not shrink window
    // (rcv_adv minus rcv_nxt) is the amount of space still available to the sender that was previously advertised
    if (win < state->rcv_adv - state->rcv_nxt)
        win = state->rcv_adv - state->rcv_nxt;

    // Observe upper limit for advertised window on this connection
    if (win > RQ_MAX_WIN && !state->ws_enabled) // RQ_MAX_WIN = 65535 (16 bit)
        win = RQ_MAX_WIN; // Note: The window size is limited to a 16 bit value in the RQ header if WINDOW SCALE option (RFC 1323) is not used

    // Note: The order of the "Do not shrink window" and "Observe upper limit" parts has been changed to the order used in FreeBSD Release 7.1

    // update rcv_adv if needed
    if (win > 0 && seqGE(state->rcv_nxt + win, state->rcv_adv)) {
        state->rcv_adv = state->rcv_nxt + win;

        if (rcvAdvVector)
            rcvAdvVector->record(state->rcv_adv);
    }

    state->rcv_wnd = win;

    if (rcvWndVector)
        rcvWndVector->record(state->rcv_wnd);

    // scale rcv_wnd:
    uint32 scaled_rcv_wnd = state->rcv_wnd;
    if (state->ws_enabled && state->rcv_wnd_scale) {
        ASSERT(state->rcv_wnd_scale <= 14); // RFC 1323, page 11: "the shift count must be limited to 14"
        scaled_rcv_wnd = scaled_rcv_wnd >> state->rcv_wnd_scale;
    }

    ASSERT(scaled_rcv_wnd == (unsigned short )scaled_rcv_wnd);

    return (unsigned short) scaled_rcv_wnd;
}

void RQConnection::updateWndInfo(RQSegment *rqseg, bool doAlways) {
    uint32 true_window = rqseg->getWindow();
    // RFC 1323, page 10:
    // "The window field (SEG.WND) in the header of every incoming
    // segment, with the exception of SYN segments, is left-shifted
    // by Snd.Wind.Scale bits before updating SND.WND:
    //    SND.WND = SEG.WND << Snd.Wind.Scale"
    if (state->ws_enabled && !rqseg->getSynBit())
        true_window = rqseg->getWindow() << state->snd_wnd_scale;

    // Following lines are based on [Stevens, W.R.: RQ/IP Illustrated, Volume 2, page 982]:
    if (doAlways
            || (rqseg->getAckBit()
                    && (seqLess(state->snd_wl1, rqseg->getSequenceNo())
                            || (state->snd_wl1 == rqseg->getSequenceNo()
                                    && seqLE(state->snd_wl2, rqseg->getAckNo()))
                            || (state->snd_wl2 == rqseg->getAckNo()
                                    && true_window > state->snd_wnd)))) {
        // send window should be updated
        state->snd_wnd = true_window;
        EV_INFO << "Updating send window from segment: new wnd="
                       << state->snd_wnd << "\n";
        state->snd_wl1 = rqseg->getSequenceNo();
        state->snd_wl2 = rqseg->getAckNo();

        if (sndWndVector)
            sndWndVector->record(state->snd_wnd);
    }
}

void RQConnection::sendOneNewSegment(bool fullSegmentsOnly,
        uint32 congestionWindow) {
    ASSERT(state->limited_transmit_enabled);

    // RFC 3042, page 3:
    // "When a RQ sender has previously unsent data queued for transmission
    // it SHOULD use the Limited Transmit algorithm, which calls for a RQ
    // sender to transmit new data upon the arrival of the first two
    // consecutive duplicate ACKs when the following conditions are
    // satisfied:
    //
    //  * The receiver's advertised window allows the transmission of the
    //  segment.
    //
    //  * The amount of outstanding data would remain less than or equal
    //  to the congestion window plus 2 segments.  In other words, the
    //  sender can only send two segments beyond the congestion window
    //  (cwnd).
    //
    // The congestion window (cwnd) MUST NOT be changed when these new
    // segments are transmitted.  Assuming that these new segments and the
    // corresponding ACKs are not dropped, this procedure allows the sender
    // to infer loss using the standard Fast Retransmit threshold of three
    // duplicate ACKs [RFC2581].  This is more robust to reordered packets
    // than if an old packet were retransmitted on the first or second
    // duplicate ACK.
    //
    // Note: If the connection is using selective acknowledgments [RFC2018],
    // the data sender MUST NOT send new segments in response to duplicate
    // ACKs that contain no new SACK information, as a misbehaving receiver
    // can generate such ACKs to trigger inappropriate transmission of data
    // segments.  See [SCWA99] for a discussion of attacks by misbehaving
    // receivers."
    if (!state->sack_enabled
            || (state->sack_enabled
                    && state->sackedBytes_old != state->sackedBytes)) {
        // check how many bytes we have
        ulong buffered = sendQueue->getBytesAvailable(state->snd_max);

        if (buffered >= state->snd_mss || (!fullSegmentsOnly && buffered > 0)) {
            ulong outstandingData = state->snd_max - state->snd_una;

            // check conditions from RFC 3042
            if (outstandingData + state->snd_mss <= state->snd_wnd
                    && outstandingData + state->snd_mss
                            <= congestionWindow + 2 * state->snd_mss) {
                // RFC 3042, page 3: "(...)the sender can only send two segments beyond the congestion window (cwnd)."
                uint32 effectiveWin = std::min(state->snd_wnd, congestionWindow)
                        - outstandingData + 2 * state->snd_mss;

                // bytes: number of bytes we're allowed to send now
                uint32 bytes = std::min(effectiveWin, state->snd_mss);

                if (bytes >= state->snd_mss
                        || (!fullSegmentsOnly && bytes > 0)) {
                    uint32 old_snd_nxt = state->snd_nxt;
                    // we'll start sending from snd_max
                    state->snd_nxt = state->snd_max;

                    EV_DETAIL
                                     << "Limited Transmit algorithm enabled. Sending one new segment.\n";
                    sendSegment(bytes);

                    if (seqGreater(state->snd_nxt, state->snd_max))
                        state->snd_max = state->snd_nxt;

                    if (unackedVector)
                        unackedVector->record(state->snd_max - state->snd_una);

                    // reset snd_nxt if needed
                    if (state->afterRto)
                        state->snd_nxt = old_snd_nxt + state->sentBytes;

                    // notify
                    rqAlgorithm->ackSent();
                    rqAlgorithm->dataSent(old_snd_nxt);
                }
            }
        }
    }
}

uint32 RQConnection::convertSimtimeToTS(simtime_t simtime) {
    ASSERT(SimTime::getScaleExp() <= -3);

    uint32 timestamp = (uint32) (simtime.inUnit(SIMTIME_MS));
    return timestamp;
}

simtime_t RQConnection::convertTSToSimtime(uint32 timestamp) {
    ASSERT(SimTime::getScaleExp() <= -3);

    simtime_t simtime(timestamp, SIMTIME_MS);
    return simtime;
}

bool RQConnection::isSendQueueEmpty() {
    return sendQueue->getBytesAvailable(state->snd_nxt) == 0;
}

} // namespace rq

} // namespace inet

