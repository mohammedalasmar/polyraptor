#include "inet/raptorqimplementation/application/raptorqapp/GenericAppMsgRq_m.h"
#include "inet/raptorqimplementation/application/raptorqapp/RaptorQBasicClientApp.h"
#include "inet/common/lifecycle/NodeOperations.h"
#include "inet/common/ModuleAccess.h"
#include <iostream>
#include <random>

namespace inet {

#define MSGKIND_CONNECT    0
#define MSGKIND_SEND       1

Define_Module(RaptorQBasicClientApp);
simsignal_t RaptorQBasicClientApp::rcvdSymbolSignal =  registerSignal("rcvdSymbolSignal");

RaptorQBasicClientApp::~RaptorQBasicClientApp() {
    cancelAndDelete(timeoutMsg);

    // Added
//    if (requestRexmitTimer)
//        delete cancelEvent(requestRexmitTimer);
}

void RaptorQBasicClientApp::initialize(int stage) {

    RaptorQAppBase::initialize(stage);

    // Added
//  requestRexmitTimer = new cMessage("REQUEST-REXMIT");
//    requestRexmitTimer->setContextPointer(this); // check if needed

    if (stage == INITSTAGE_LOCAL) {

        //  earlySend = false;    // TBD make it parameter
        //  WATCH(numRequestsToSend);
        bytesRcvd = 0;
        packetsRcvd=0;
        WATCH(bytesRcvd);
        opcode = par("opcode");
       fileId = par("fileId");
//       numRequestsToSend = par("numRequestsToSend").longValue();


        startTime = par("startTime");
        stopTime = par("stopTime");
        if (stopTime >= SIMTIME_ZERO&& stopTime < startTime)
        throw cRuntimeError("Invalid startTime/stopTime parameters");
    }
    else if (stage == INITSTAGE_APPLICATION_LAYER) {
        timeoutMsg = new cMessage("timer");
        nodeStatus = dynamic_cast<NodeStatus *>(findContainingNode(this)->getSubmodule("status"));
        if (isNodeUp()) {
            timeoutMsg->setKind(MSGKIND_CONNECT);
            scheduleAt(startTime, timeoutMsg);
        }
    }
}

bool RaptorQBasicClientApp::isNodeUp() {
    return !nodeStatus || nodeStatus->getState() == NodeStatus::UP;
}


void RaptorQBasicClientApp::handleTimer(cMessage *msg) {
    /////// Added MOH send requests based on a timer
    switch (msg->getKind()) {

    case MSGKIND_CONNECT:
//        std::cout << " RaptorQBasicClientApp::handleTime  MSGKIND_CONNECT \n";
        connect();    // active OPEN
        // significance of earlySend: if true, data will be sent already
        // in the ACK of SYN, otherwise only in a separate packet (but still
        // immediately)
//                    if (earlySend)
//                     sendRequest();
        break;

    case MSGKIND_SEND:   // not used now MHO see  RaptorQBasicClientApp::socketEstablished
//        sendRequest();
        break;

    default:
        throw cRuntimeError("Invalid timer msg: kind=%d", msg->getKind());
    }
}


void RaptorQBasicClientApp::socketEstablished(int connId, void *ptr) {
    RaptorQAppBase::socketEstablished(connId, ptr);

    // determine number of requests in this session
//    numRequestsToSend = par("numRequestsPerSession").longValue();
//    if (numRequestsToSend < 1)
//        numRequestsToSend = 1;

    // perform first request if not already done (next one will be sent when reply arrives)
//    if (!earlySend)
//      if (opcode == 1)  sendReadRequest();   // 1 read, 2 write
      if (opcode == 2)  sendWriteRequest();   // 1 read, 2 write

    //startRequestRexmitTimer(); //added
 //   numRequestsToSend--;
}


void RaptorQBasicClientApp::sendWriteRequest() {

    long requestLength = par("requestLength");
    long replyLength = par("replyLength");

    if (requestLength < 1)
        requestLength = 1;
    if (replyLength < 1)
        replyLength = 1;

    char msgname[16];
//    sprintf(msgname, "REQUEST-%d",++requestIndex);
    sprintf(msgname, "REQUEST-WRITE-INIT");

    GenericAppMsgRq *msg = new GenericAppMsgRq(msgname);
    msg->setByteLength(requestLength);
    msg->setExpectedReplyLength(replyLength);
    msg->setServerClose(false);
//    msg->setIsClient(true);  // client 1, server 0 ///////dasdasdsadasdasdasdasdad


    if (opcode == 1) {    // opcode=1 ==> Reader,  opcode=2 ==> Writer
        msg->setIsReader(true);
        msg->setIsWriter(false);

    } else if (opcode == 2) {
        msg->setIsReader(false);
        msg->setIsWriter(true);
    }


    msg->setOpcode(opcode); // 1 read, 2 write
    msg->setFileId(fileId);
//    msg->setNumSymbolsToGet(numRequestsToSend);
//    msg->setNumSymbolsToSend(numSymbolsToSend);

//    msg->setNamePooling(true);

    sendPacket(msg);

 //    numRequestsToSend--;
    // schedule self-message with kind MSGKIND_SEND to send the next request after a thinkTime (see handleTimer)
//    if (numRequestsToSend >0 ){
//    if (timeoutMsg) {
//       simtime_t d = simTime() + (simtime_t) par("thinkTime");                     // time gap between requests
//        rescheduleOrDeleteTimer(d, MSGKIND_SEND); // this sends new request
//     }
//    }

}







bool RaptorQBasicClientApp::handleOperationStage(LifecycleOperation *operation,
        int stage, IDoneCallback *doneCallback) {
    Enter_Method_Silent();
    if (dynamic_cast<NodeStartOperation *>(operation)) {
        if ((NodeStartOperation::Stage)stage == NodeStartOperation::STAGE_APPLICATION_LAYER) {
            simtime_t now = simTime();
            simtime_t start = std::max(startTime, now);
            if (timeoutMsg && ((stopTime < SIMTIME_ZERO) || (start < stopTime) || (start == stopTime && startTime == stopTime))) {
                timeoutMsg->setKind(MSGKIND_CONNECT);
                scheduleAt(start, timeoutMsg);
            }
        }
    }
    else if (dynamic_cast<NodeShutdownOperation *>(operation)) {
        if ((NodeShutdownOperation::Stage)stage == NodeShutdownOperation::STAGE_APPLICATION_LAYER) {
            cancelEvent(timeoutMsg);
            if (socket.getState() == RQSocket::CONNECTED || socket.getState() == RQSocket::CONNECTING || socket.getState() == RQSocket::PEER_CLOSED)
            close();
            // TODO: wait until socket is closed
        }
    }
    else if (dynamic_cast<NodeCrashOperation *>(operation)) {
        if ((NodeCrashOperation::Stage)stage == NodeCrashOperation::STAGE_CRASH)
        cancelEvent(timeoutMsg);
    }
    else
    throw cRuntimeError("Unsupported lifecycle operation '%s'", operation->getClassName());
    return true;
}






// NOT used now (see RQSinkApp)
void RaptorQBasicClientApp::sendReadRequest() {

    long requestLength = par("requestLength");
    long replyLength = par("replyLength");

    if (requestLength < 1)
        requestLength = 1;
    if (replyLength < 1)
        replyLength = 1;

    char msgname[16];
//    sprintf(msgname, "REQUEST-%d",++requestIndex);
    sprintf(msgname, "REQUEST-READ-INIT");

    GenericAppMsgRq *msg = new GenericAppMsgRq(msgname);
    msg->setByteLength(requestLength);
    msg->setExpectedReplyLength(replyLength);
    msg->setServerClose(false);
//    msg->setIsClient(true);  // client 1, server 0 ///////dasdasdsadasdasdasdasdad

    if (opcode == 1) {    // opcode=1 ==> Reader,  opcode=2 ==> Writer
        msg->setIsReader(true);
        msg->setIsWriter(false);

    } else if (opcode == 2) {
        msg->setIsReader(false);
        msg->setIsWriter(true);
    }


    msg->setOpcode(opcode);
    msg->setFileId(fileId);
//    msg->setNumSymbolsToGet(numRequestsToSend);
//    msg->setNamePooling(true);


    sendPacket(msg);
 //    numRequestsToSend--;
    // schedule self-message with kind MSGKIND_SEND to send the next request after a thinkTime (see handleTimer)
//    if (numRequestsToSend >0 ){
//    if (timeoutMsg) {
//       simtime_t d = simTime() + (simtime_t) par("thinkTime");                     // time gap between requests
//        rescheduleOrDeleteTimer(d, MSGKIND_SEND); // this sends new request
//     }
//    }

}







void RaptorQBasicClientApp::rescheduleOrDeleteTimer(simtime_t d,
        short int msgKind) {
    cancelEvent(timeoutMsg);

    if (stopTime < SIMTIME_ZERO|| d < stopTime) {
        timeoutMsg->setKind(msgKind);
        scheduleAt(d, timeoutMsg);
    }
    else {
        delete timeoutMsg;
        timeoutMsg = nullptr;
    }
}

void RaptorQBasicClientApp::socketDataArrived(int connId, void *ptr, cPacket *msg, bool urgent) {
    std::cout << "\n\n yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy  " << std::endl;

    if (msg->getKind() == RQ_I_DATA) {
        std::cout << "reply arrived to the client  " << std::endl;
        packetsRcvd++;


        cPacket *pk = PK(msg);
        long packetLength = pk->getByteLength();
        bytesRcvd += packetLength;


        emit(rcvdSymbolSignal, pk);

//       RaptorQAppBase::socketDataArrived(connId, ptr, msg, urgent); // this line deletes the arrived msg (keep it below here) MOH

        std::cout << "packetsRcvd  " << packetsRcvd << std::endl;
//        delete msg;
    }






    // Added: modifying the up lines codes mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm importantttttttttt modeling the decoding time :::
//    if (numRequestsToSend==0){
//            double const exp_dist_mean   = 10;
//            double const exp_dist_lambda = 1 / exp_dist_mean;
//            std::random_device rd;  //Will be used to obtain a seed for the random number engine
//            std::mt19937 generator(rd());
//            std::exponential_distribution<double> distribution(exp_dist_lambda);
//
//           double number = distribution(generator);
//           std::cout << number << std::endl;
//
//           if (number >=1000) {  // if decoding fails send new request
//               ++numRequestsToSend;
//           }
//    }
//    --numRequestsToSend;
//    if (numRequestsToSend > 0) {
//        EV_INFO << "reply arrived\n";

        // don't close the connection until receiving all the requested symbols
//    if (packetsRcvd == numRequestsToSend  && socket.getState() != RQSocket::LOCALLY_CLOSED) {
//        EV_INFO << "reply to last request arrived, closing session\n";
//        close();
//    }

    // MOH my notes: in the current codes, when the last requested data arrived and the numRequestsToSend=0 then close the connection
    //  BUT we want to use a black box to decide whether the decoding fails or successes,
    // if success then close the connection otherwise increase ++numRequestsToSend to get a new symbol from the server
}

void RaptorQBasicClientApp::socketClosed(int connId, void *ptr) {
    RaptorQAppBase::socketClosed(connId, ptr);

    // start another session after a delay
    if (timeoutMsg) {
        simtime_t d = simTime() + (simtime_t) par("idleInterval");
        // rescheduleOrDeleteTimer(d, MSGKIND_CONNECT);  // MOH commented
    }
}

void RaptorQBasicClientApp::socketFailure(int connId, void *ptr, int code) {
    RaptorQAppBase::socketFailure(connId, ptr, code);

    // reconnect after a delay
    if (timeoutMsg) {
        simtime_t d = simTime() + (simtime_t) par("reconnectInterval");
        rescheduleOrDeleteTimer(d, MSGKIND_CONNECT);
    }
}

// Added by MOH
//void RaptorQBasicClientApp::startRequestRexmitTimer()
//{
//    request_rexmit_count = 0;
//    request_rexmit_timeout = RQ_TIMEOUT_REQUEST_REXMIT;
//
//    if (requestRexmitTimer->isScheduled())
//        cancelEvent(requestRexmitTimer);
//
//    scheduleAt(simTime() + request_rexmit_timeout, requestRexmitTimer); // start the timer for request segment
//}

}// namespace inet

