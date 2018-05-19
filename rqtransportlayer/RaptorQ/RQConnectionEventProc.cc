
#include <string.h>
#include "RaptorQ.h"
#include "RQConnection.h"
#include "raptorq_common/RQSegment.h"
#include "../contract/raptorq/RQCommand_m.h"
#include "RQSendQueue.h"
#include "RQReceiveQueue.h"
#include "RQAlgorithm.h"


namespace inet {

namespace raptorq {

//
// Event processing code
//
void RQConnection::process_OPEN_ACTIVE(RQEventCode& event, RQCommand *rqCommand, cMessage *msg)
{
    RQOpenCommand *openCmd = check_and_cast<RQOpenCommand *>(rqCommand);
    L3Address localAddr, remoteAddr;
    int localPort, remotePort;

    switch (fsm.getState()) {
        case RQ_S_INIT:
            initConnection(openCmd);
//            std::cout << "\n\n\n RQConnection::process_OPEN_ACTIVE \n";

            // store local/remote socket
            state->active = true;
            localAddr = openCmd->getLocalAddr();
            remoteAddr = openCmd->getRemoteAddr();
            localPort = openCmd->getLocalPort();
            remotePort = openCmd->getRemotePort();
            state->fileId= openCmd->getFileIdToGet();
            state->numSymbolsToSend = openCmd->getNumSymbolsToSend();

//            std::cout << "RQConnection::process_OPEN_ACTIVE localAddr " << localAddr << " \n";

            state->isMulticastConn = openCmd->getIsMulticastConn();
            state->multicastGroupIndex = openCmd->getMulticastGroupIndex();
            state->isSender =  openCmd->getIsSender();  // true

            state->isMultiSourcingConn = openCmd->getIsMultiSourcingConn() ;
            state->multiSourcingGroupIndex = openCmd->getMultiSourcingGroupIndex();



//            std::cout<< "   state->fileId   "    << state->fileId <<std::endl;

            if (remoteAddr.isUnspecified() || remotePort == -1)
                throw cRuntimeError(rqMain, "Error processing command OPEN_ACTIVE: remote address and port must be specified");

            if (localPort == -1) {
                localPort = rqMain->getEphemeralPort();
                EV_DETAIL << "Assigned ephemeral port " << localPort << "\n";
            }

            EV_DETAIL << "OPEN: " << localAddr << ":" << localPort << " --> " << remoteAddr << ":" << remotePort << "\n";
//            std::cout << "OPEN, localAddr: " << localAddr << " ,localPort:" << localPort << " -->remoteAddr: " << remoteAddr << ", remotePort:" << remotePort << "\n";

//            std::cout  << "RQConnection::process_OPEN_ACTIVE  OPEN: " << localAddr << ":" << localPort << " --> " << remoteAddr << ":" << remotePort << "\n";

             rqMain->addSockPair(this, localAddr, remoteAddr, localPort, remotePort);
//            if (state->isMulticastConn == false ) rqMain->addSockPair(this, localAddr, remoteAddr, localPort, remotePort);
//            if (state->isMulticastConn == true )  rqMain->addMulticastSockPair(this,  localAddr,   localPort, state->multicastGroupIndex);

            // send initial SYN
            selectInitialSeqNum();
            sendSyn();
//            if (state->isMulticastConn == false)  sendSyn();
//            if (state->isMulticastConn == true) sendToIPMulticast();


//            startSynRexmitTimer(); // timer for SYN mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//            scheduleTimeout(connEstabTimer, RQ_TIMEOUT_CONN_ESTAB);  // connection establishing timer
            break;

        default:
            throw cRuntimeError(rqMain, "Error processing command OPEN_ACTIVE: connection already exists");
    }

    delete openCmd;
    delete msg;
}

void RQConnection::process_OPEN_PASSIVE(RQEventCode& event, RQCommand *rqCommand, cMessage *msg)
{
    RQOpenCommand *openCmd = check_and_cast<RQOpenCommand *>(rqCommand);
    L3Address localAddr;
    int localPort;

    switch (fsm.getState()) {
        case RQ_S_INIT:
            initConnection(openCmd);
//            std::cout << "\n\n\n RQConnection::process_OPEN_PASSIVE \n" ;

            // store local/remote socket
            state->active = false;
            state->fork = openCmd->getFork();
            localAddr = openCmd->getLocalAddr();
            localPort = openCmd->getLocalPort();
            state->isSender =  openCmd->getIsSender(); // false

//            state->isMulticastConn = openCmd->getIsMulticastConn();
//            state->multicastGroupIndex = openCmd->getMulticastGroupIndex();


            if (localPort == -1)
                throw cRuntimeError(rqMain, "Error processing command OPEN_PASSIVE: local port must be specified");

            EV_DETAIL << "Starting to listen on: " << localAddr << ":" << localPort << "\n";
//             std::cout << "RQConnection::process_OPEN_PASSIVE Starting to listen on: " << localAddr << ":" << localPort << "\n";

             rqMain->addSockPair(this, localAddr, L3Address(), localPort, -1);
//            rqMain->addMulticastSockPair(this,  localAddr,   localPort, -1);

            break;

        default:
            throw cRuntimeError(rqMain, "Error processing command OPEN_PASSIVE: connection already exists");
    }

    delete openCmd;
    delete msg;
}

void RQConnection::process_SEND(RQEventCode& event, RQCommand *rqCommand, cMessage *msg)
{
    RQSendCommand *sendCommand = check_and_cast<RQSendCommand *>(rqCommand);

    // FIXME how to support PUSH? One option is to treat each SEND as a unit of data,
    // and set PSH at SEND boundaries
    switch (fsm.getState()) {
        case RQ_S_INIT:
            throw cRuntimeError(rqMain, "Error processing command SEND: connection not open");

        case RQ_S_LISTEN:
            EV_DETAIL << "SEND command turns passive open into active open, sending initial SYN\n";
            state->active = true;
            selectInitialSeqNum();
            sendSyn();
            startSynRexmitTimer();
            scheduleTimeout(connEstabTimer, RQ_TIMEOUT_CONN_ESTAB);
            sendQueue->enqueueAppData(PK(msg));    // queue up for later
            EV_DETAIL << sendQueue->getBytesAvailable(state->snd_una) << " bytes in queue\n";
            break;

        case RQ_S_SYN_RCVD:
        case RQ_S_SYN_SENT:
            EV_DETAIL << "Queueing up data for sending later.\n";
            sendQueue->enqueueAppData(PK(msg));    // queue up for later
            EV_DETAIL << sendQueue->getBytesAvailable(state->snd_una) << " bytes in queue\n";
            break;

        case RQ_S_ESTABLISHED:
        case RQ_S_CLOSE_WAIT:

//            if (msg->getNamePooling() == false){ // we don't want to queue the initial request message that we get from the app
////                std::cout << "msg->getKind() =" <<msg->getKind() << "\n";
//                sendQueue->enqueueAppData(PK(msg));
//            }
//            EV_DETAIL << "fsm.getState():  " << fsm.getState() << "  mmmm "   << sendQueue->getBytesAvailable(state->snd_una) << " bytes in queue, plus "
//                      << (state->snd_max - state->snd_una) << " bytes unacknowledged\n";
//        std::cout << " we are here \n";
        rqAlgorithm->sendCommandInvoked(msg); // MOH: added the argument msg to be used for checking if it's a server or a client side by cehcking the opcode value (see RQBaseAlg.cc)

         break;

        case RQ_S_LAST_ACK:
        case RQ_S_FIN_WAIT_1:
        case RQ_S_FIN_WAIT_2:
        case RQ_S_CLOSING:
        case RQ_S_TIME_WAIT:
            throw cRuntimeError(rqMain, "Error processing command SEND: connection closing");
    }

    if ((state->sendQueueLimit > 0) && (sendQueue->getBytesAvailable(state->snd_una) > state->sendQueueLimit))
        state->queueUpdate = false;

    delete sendCommand;    // msg itself has been taken by the sendQueue
}

void RQConnection::process_READ_REQUEST(RQEventCode& event, RQCommand *rqCommand, cMessage *msg)
{
    delete msg;
    cMessage *dataMsg;
    while ((dataMsg = receiveQueue->extractBytesUpTo(state->rcv_nxt)) != nullptr)
    {
        dataMsg->setKind(RQ_I_DATA);
        RQCommand *cmd = new RQCommand();
        cmd->setConnId(connId);
        dataMsg->setControlInfo(cmd);
        sendToApp(dataMsg);
    }
}

void RQConnection::process_CLOSE(RQEventCode& event, RQCommand *rqCommand, cMessage *msg)
{
    delete rqCommand;
    delete msg;

    std::cout << " RQConnection::process_CLOSE \n" ;
    std::cout << " state->snd_max:  " << state->snd_max << " ,sendQueue->getBufferEndSeq(): " << sendQueue->getBufferEndSeq() <<" \n"  ;


    switch (fsm.getState()) {
        case RQ_S_INIT:
            throw cRuntimeError(rqMain, "Error processing command CLOSE: connection not open");

        case RQ_S_LISTEN:
            // Nothing to do here
            break;

        case RQ_S_SYN_SENT:
            // Delete the TCB and return "error:  closing" responses to any
            // queued SENDs, or RECEIVEs.
            break;

        case RQ_S_SYN_RCVD:
        case RQ_S_ESTABLISHED:
        case RQ_S_CLOSE_WAIT:
            //
            // SYN_RCVD processing (ESTABLISHED and CLOSE_WAIT are similar):
            //"
            // If no SENDs have been issued and there is no pending data to send,
            // then form a FIN segment and send it, and enter FIN-WAIT-1 state;
            // otherwise queue for processing after entering ESTABLISHED state.
            //"


            // added MOH, TODO check the if condition below if needed
            sendFin();
            rqAlgorithm->restartRexmitTimer();

            if (state->snd_max == sendQueue->getBufferEndSeq()) {
                EV_DETAIL << "No outstanding SENDs, sending FIN right away, advancing snd_nxt over the FIN\n";
                state->snd_nxt = state->snd_max;

                sendFin();
                rqAlgorithm->restartRexmitTimer();
                state->snd_max = ++state->snd_nxt;

                if (unackedVector)
                    unackedVector->record(state->snd_max - state->snd_una);

                // state transition will automatically take us to FIN_WAIT_1 (or LAST_ACK)
            }
            else {
                EV_DETAIL << "SEND of " << (sendQueue->getBufferEndSeq() - state->snd_max)
                          << " bytes pending, deferring sending of FIN\n";
                event = RQ_E_IGNORE;
            }
            state->send_fin = true;
            state->snd_fin_seq = sendQueue->getBufferEndSeq();
            break;

        case RQ_S_FIN_WAIT_1:
        case RQ_S_FIN_WAIT_2:
        case RQ_S_CLOSING:
        case RQ_S_LAST_ACK:
        case RQ_S_TIME_WAIT:
            // RFC 793 is not entirely clear on how to handle a duplicate close request.
            // Here we treat it as an error.
            throw cRuntimeError(rqMain, "Duplicate CLOSE command: connection already closing");
    }
}

void RQConnection::process_ABORT(RQEventCode& event, RQCommand *rqCommand, cMessage *msg)
{
    delete rqCommand;
    delete msg;

    //
    // The ABORT event will automatically take the connection to the CLOSED
    // state, flush queues etc -- no need to do it here. Also, we don't need to
    // send notification to the user, they know what's going on.
    //
    switch (fsm.getState()) {
        case RQ_S_INIT:
            throw cRuntimeError("Error processing command ABORT: connection not open");

        case RQ_S_SYN_RCVD:
        case RQ_S_ESTABLISHED:
        case RQ_S_FIN_WAIT_1:
        case RQ_S_FIN_WAIT_2:
        case RQ_S_CLOSE_WAIT:
            //"
            // Send a reset segment:
            //
            //   <SEQ=SND.NXT><CTL=RST>
            //"
            sendRst(state->snd_nxt);
            break;
    }
}

void RQConnection::process_STATUS(RQEventCode& event, RQCommand *rqCommand, cMessage *msg)
{
    delete rqCommand;    // but reuse msg for reply

    if (fsm.getState() == RQ_S_INIT)
        throw cRuntimeError("Error processing command STATUS: connection not open");

    RQStatusInfo *statusInfo = new RQStatusInfo();

    statusInfo->setState(fsm.getState());
    statusInfo->setStateName(stateName(fsm.getState()));

    statusInfo->setLocalAddr(localAddr);
    statusInfo->setRemoteAddr(remoteAddr);
    statusInfo->setLocalPort(localPort);
    statusInfo->setRemotePort(remotePort);

    statusInfo->setSnd_mss(state->snd_mss);
    statusInfo->setSnd_una(state->snd_una);
    statusInfo->setSnd_nxt(state->snd_nxt);
    statusInfo->setSnd_max(state->snd_max);
    statusInfo->setSnd_wnd(state->snd_wnd);
    statusInfo->setSnd_up(state->snd_up);
    statusInfo->setSnd_wl1(state->snd_wl1);
    statusInfo->setSnd_wl2(state->snd_wl2);
    statusInfo->setIss(state->iss);
    statusInfo->setRcv_nxt(state->rcv_nxt);
    statusInfo->setRcv_wnd(state->rcv_wnd);
    statusInfo->setRcv_up(state->rcv_up);
    statusInfo->setIrs(state->irs);
    statusInfo->setFin_ack_rcvd(state->fin_ack_rcvd);

    msg->setControlInfo(statusInfo);
    msg->setKind(RQ_I_STATUS);
    sendToApp(msg);
}

void RQConnection::process_QUEUE_BYTES_LIMIT(RQEventCode& event, RQCommand *rqCommand, cMessage *msg)
{
    if (state == nullptr)
        throw cRuntimeError("Called process_QUEUE_BYTES_LIMIT on uninitialized RQConnection!");

    state->sendQueueLimit = rqCommand->getUserId();    // Set queue size limit
    EV << "state->sendQueueLimit set to " << state->sendQueueLimit << "\n";
    delete msg;
    delete rqCommand;
}

} // namespace raptorq

} // namespace inet

