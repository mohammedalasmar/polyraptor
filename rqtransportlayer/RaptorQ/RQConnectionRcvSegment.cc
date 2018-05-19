
#include <string.h>
#include "RaptorQ.h"
#include "RQConnection.h"
#include "raptorq_common/RQSegment.h"
#include "inet/raptorqimplementation/rqtransportlayer/contract/raptorq/RQCommand_m.h"
#include "RQSendQueue.h"
//#include "RQSACKRexmitQueue.h"
#include "RQReceiveQueue.h"
#include "RQAlgorithm.h"
#include "inet/raptorqimplementation/application/raptorqapp/GenericAppMsgRq_m.h"

namespace inet {

namespace raptorq {



bool RQConnection::tryFastRoute(RQSegment *rqseg) {
    // fast route processing not yet implemented
    return false;
}


// MOH at the sender side
void RQConnection::generateSymbolsList() {
//         std::cout << " generateSymbolsList "     << "\n";

    char msgname[16];
//    int store = state->numSymbolsToSend;
//    if (store > 1000000000) store=state->numSymbolsToSend+ 10000000;




//    for (int i = 1; i < state->numSymbolsToSend*30; i++) { // mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm TODO
//        sprintf(msgname, "SYMBOL-%d", i);
//        GenericAppMsgRq *datamsg = new GenericAppMsgRq(msgname);
//
//        datamsg->setESI(i);
//        datamsg->setSBN(state->sbn); // SBN=0  for now: one Source Block
//
//        EncodingSymbols encodingSymbols;
//        encodingSymbols.EnSyId = i;
//        encodingSymbols.msg = PK(datamsg);
//        encodingSymbols.msg->setByteLength(1000);
//        encodingSymbolsList.push_back(encodingSymbols);
//    }




//    std::list<EncodingSymbols>::iterator it;
//    it = encodingSymbolsList.begin();
//    while (it != encodingSymbolsList.end()) {
//        std::cout << " ESI " << it->EnSyId << " msgName " << it->msg->getFullName() << "\n";
//        it++;
//    }
}


void RQConnection::getNewSymbolToSend() {
    char msgname[16];
    int ini =   state->esi +1 ;
    sprintf(msgname, "SYMBOL-%d",ini);
    GenericAppMsgRq *datamsg = new GenericAppMsgRq(msgname);

    datamsg->setESI(state->esi);
    datamsg->setSBN(state->sbn); // SBN=0  for now: one Source Block

    EncodingSymbols encodingSymbols;
    encodingSymbols.EnSyId =  ini;
    encodingSymbols.msg = PK(datamsg);
    encodingSymbols.msg->setByteLength(1000);
    encodingSymbolsList.push_back(encodingSymbols);

}


//MOH
void RQConnection::sendInitialWindow(){

    // TODO  we don't do any checking about the received request segment, e.g. check if it's  a request nothing else
    // fetch the next symbol from the encodingSymbols list
    std::list<EncodingSymbols>::iterator itt;

    if (state->esi == 0) {
//        std::cout << " ££££££££££££  Sender ££££££££££££££££££££ sendInitialWindow "<< std::endl;

        for (int i = 1; i <= state->IW; i++) { // send the initial window (12 symbols as in NDP) IWWWWWWWWWWWW
            getNewSymbolToSend();

            itt = encodingSymbolsList.begin();
            std::advance(itt, state->esi); // increment the iterator by esi

            // create a segment for the symbol that will be sent
            RQSegment *rqseg = new RQSegment(nullptr);
            const char *payloadName = nullptr;
            payloadName = itt->msg->getName();
            rqseg->setPayloadLength(1000);
            //    rqseg->addPayloadMessage(itt->msg->dup(), itt->EnSyId);
            rqseg->addPayloadMessage(itt->msg->dup(), 1);
            // why 1 ==> each segment has one symbol, we don't send segment with multiple packets as in TCP
            rqseg->setName(payloadName);
            rqseg->setIsClient(false); // we send symbol segment "(server=sender)



            rqseg->setIsWriter(true);
            rqseg->setIsReader(false);
            rqseg->setIsHeader(state->isHeader);
            rqseg->setIsRequestPacket(false);



            rqseg->setIsLastSymbolToSend(false);


            // for multicast
            rqseg->setIsMulticastConnSeg(state->isMulticastConn) ; // multicast
            rqseg->setMulticastGroupIndex(state->multicastGroupIndex);
            rqseg->setIsMulticastSender(state->isSender); // true
            rqseg->setIsMulticastReceiver(false);


            // for multiSourcing
            rqseg->setIsMultiSourcingConn(state->isMultiSourcingConn);
            rqseg->setMultiSourcingGroupIndex(state->multiSourcingGroupIndex) ;
            rqseg->setIsMultiSourcingSender(true);
            rqseg->setIsMultiSourcingReceiver(false);



//                std::cout << " rqseg->getPayloadArraySize() " << rqseg->getPayloadArraySize() << std::endl;
            state->esi++;
//            std::cout << " state->esi " << state->esi << std::endl;
            rqseg->setOpcode(state->opcodeValue);

            state->request_id = 0;
            rqseg->setSymbolSequenceNumber(state->esi);

//                if (!(  state->esi== 2 || state->esi== 3 || state->esi== 4 || state->esi== 5 || state->esi== 6 || state->esi== 7 || state->esi== 8 || state->esi== 9 || state->esi== 10  || state->esi== 11  ))   // don't send symbols 4&5 just for testing
                sendToIP(rqseg);
        }
}

}



RQEventCode RQConnection::process_RCV_SEGMENT(RQSegment *rqseg, L3Address src, L3Address dest) {
    EV_INFO << "Seg arrived: ";
    printSegmentBrief(rqseg);
    EV_DETAIL << "TCB: " << state->info() << "\n";

//    if (rcvSeqVector)
//        rcvSeqVector->record(rqseg->getSequenceNo());
//
//    if (rcvAckVector)
//        rcvAckVector->record(rqseg->getAckNo());

    //
    // Note: this code is organized exactly as RFC 793, section "3.9 Event
    // Processing", subsection "SEGMENT ARRIVES".
    //
    RQEventCode event;

    if (fsm.getState() == RQ_S_LISTEN && isFileExist(rqseg->getFileId())) {
        event = processSegmentInListen(rqseg, src, dest);
    } else if (fsm.getState() == RQ_S_SYN_SENT) {
        event = processSegmentInSynSent(rqseg, src, dest);
    } else {
        // RFC 793 steps "first check sequence number", "second check the RST bit", etc
        event = processSegment1stThru8th(rqseg);
//        std::cout << "Returned RQEventCode=  "  << event << std::endl;

    }

    delete rqseg;
    return event;
}

bool RQConnection::isFileExist(int fileId) {
//    std::cout << "fileId " << fileId << "\n";

    if (fileId == 2017) {
        return true;
    } else {
//        std::cout << "File does not exist \n";
        // TODO if file does not exist then either send a signal from the server to let the client knows,
        // or we need to stop sending the SYN  (syntimeout should be for few times not unlimited as now)
        return false;
    }

}


RQEventCode RQConnection::processSegment1stThru8th(RQSegment *rqseg) {

//    std::cout << "Current State=  "  << fsm.getState() << std::endl;
    RQEventCode event = RQ_E_IGNORE;

    // RFC 793: eighth, check the FIN bit,
    if (rqseg->getFinBit()) {
        state->ack_now = true;
        state->fin_rcvd = true;
        rqAlgorithm->receivedOutOfOrderSegment(); // just to send an ack for the received FIN
        event = RQ_E_RCV_FIN;

        switch (fsm.getState()) {
        case RQ_S_FIN_WAIT_1:
            if (state->fin_ack_rcvd) {
                event = RQ_E_RCV_FIN_ACK;

                // start the time-wait timer, turn off the other timers
                cancelEvent(finWait2Timer);
                scheduleTimeout(the2MSLTimer, RQ_TIMEOUT_2MSL);

                // we're entering TIME_WAIT, so we can signal CLOSED the user
                // (the only thing left to do is wait until the 2MSL timer expires)
            }
            break;

        case RQ_S_FIN_WAIT_2:

            event = RQ_E_RCV_FIN;
            rqAlgorithm->cancelRexmitTimer(); // TODO ADDEd check if needed MOH
            // Start the time-wait timer, turn off the other timers.
            cancelEvent(the2MSLTimer);
            // scheduleTimeout(the2MSLTimer, RQ_TIMEOUT_2MSL);

            // we're entering TIME_WAIT, so we can signal CLOSED the user
            // (the only thing left to do is wait until the 2MSL timer expires)
            break;

        case RQ_S_TIME_WAIT:
            sendAck();
            // rqAlgorithm->cancelRexmitTimer(); // TODO ADDEd check if needed MOH

            // Restart the 2 MSL time-wait timeout.
            cancelEvent(the2MSLTimer);
            scheduleTimeout(the2MSLTimer, RQ_TIMEOUT_2MSL);
            event = RQ_E_TIMEOUT_2MSL;
            break;

        default:
            break;
        }

        return event;
// TBD do PUSH stuff
    }

    if (fsm.getState() == RQ_S_CLOSE_WAIT) {
//        std::cout << " sssssss RQ_S_CLOSE_WAIT " << std::endl;
        return RQ_E_CLOSE;

    }
    if (fsm.getState() == RQ_S_LAST_ACK) {

//        std::cout << " sssssss  RQ_S_LAST_ACK" << std::endl;

        return RQ_E_RCV_ACK;
    }

    if (fsm.getState() == RQ_S_FIN_WAIT_1)
        return RQ_E_RCV_ACK;

    if (fsm.getState() == RQ_S_FIN_WAIT_2) {

    }

    if (fsm.getState() == RQ_S_CLOSING) {
//"
// In addition to the processing for the ESTABLISHED state, if
// the ACK acknowledges our FIN then enter the TIME-WAIT state,
// otherwise ignore the segment.
//"
        if (state->fin_ack_rcvd) {
            EV_INFO << "Our FIN acked -- can go to TIME_WAIT now\n";
            event = RQ_E_RCV_ACK;    // will trigger transition to TIME-WAIT
            // start the time-wait timer, turn off the other timers
            cancelEvent(finWait2Timer);
            scheduleTimeout(the2MSLTimer, RQ_TIMEOUT_2MSL);    // start timer

            // we're entering TIME_WAIT, so we can signal CLOSED the user
            // (the only thing left to do is wait until the 2MSL timer expires)
        }
    }

    if (fsm.getState() == RQ_S_TIME_WAIT) {
        sendAck();
        rqAlgorithm->cancelRexmitTimer();
        cancelEvent(the2MSLTimer);
        scheduleTimeout(the2MSLTimer, RQ_TIMEOUT_2MSL);
    }

    if (fsm.getState() == RQ_S_SYN_RCVD) {
        // notify rqAlgorithm and app layer
        rqAlgorithm->established(false);
        sendEstabIndicationToApp();


// This will trigger transition to ESTABLISHED. Timers and notifying
// app will be taken care of in stateEntered().
        event = RQ_E_RCV_ACK;
    }

// ££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££
// ££££££££££££££££££££££££ REQUEST Arrived £££££££££££££££££££££££££££££
// ££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££

// requests arrived from the receiver (client) (==1) to the sender(server) ==> send a symbol   {{ WRITER }}
    if (fsm.getState() == RQ_S_ESTABLISHED && rqseg->getIsWriter() == false && rqseg->getIsReader() == true) {

        // TODO  we don't do any checking about the received request segment, e.g. check if it's  a request nothing else
        // fetch the next symbol from the encodingSymbols list
        std::list<EncodingSymbols>::iterator itt;

//         std::cout << "  \n\n\n  ££££££££££££££££  Sender ££££££££££££££ new  REQUEST arrived: request_id = " << state->request_id + 1 << " RequestSequenceNumber= " << rqseg->getRequestSequenceNumber() << std::endl;
//         std::cout << " sender " << getRQMain()->getFullPath()  << "\n";


         //        std::cout << " "<< cSimulation::getActiveSimulation()->getContextSimpleModule()->getFullPath() << std::endl;

        int requestsGap;
        requestsGap = rqseg->getRequestSequenceNumber() - state->request_id;
//           std::cout <<  "  requestsGap=  " << requestsGap << std::endl;

        if (requestsGap >= 1) {
            //  we send symbols  based on requestsGap value
            // if the requestsGap is smaller than 1 that means we received a delayed request which we need to  ignore
            // as we have assumed it was lost and we send extra symbols previously
            for (int i = 1; i <= requestsGap; i++) {
                getNewSymbolToSend();
//                std::cout << "  " << this->getRQMain()->getFullPath()<< std::endl;

//                 std::cout << " qqqq state->esi " <<state->esi << std::endl;
//                std::cout << "encodingSymbolsList  " <<encodingSymbolsList.size()<< std::endl;
                state->esi = encodingSymbolsList.size()-1;

                itt = encodingSymbolsList.begin();
                std::advance(itt, state->esi); // increment the iterator by esi
//                std::cout << "dasdasdsadsadas 22222=  " <<itt->msg->getName() << std::endl;


                // create a segment for the symbol that will be sent
                RQSegment *rqseg = new RQSegment(nullptr);
                const char *payloadName = nullptr;
                payloadName = itt->msg->getName();
                rqseg->setPayloadLength(1000);
//                std::cout << "dasdasdsadsadas 3333=  " << requestsGap << std::endl;

                rqseg->addPayloadMessage(itt->msg->dup(), 1);
//                std::cout << "dasdasdsadsadas 4444444=  " << requestsGap << std::endl;

                rqseg->setName(payloadName);
                rqseg->setIsClient(false); // we send symbol segment "(server=sender)
                rqseg->setOpcode(state->opcodeValue); // we send symbol segment "(server=sender)
                rqseg->setIsWriter(true);
                rqseg->setIsReader(false);
                rqseg->setIsHeader(state->isHeader);

                state->esi++;
                state->request_id++;
                rqseg->setSymbolSequenceNumber(state->esi);

                rqseg->setIsRequestPacket(false);



                rqseg->setIsLastSymbolToSend(false); // ???



                // for multicast
                rqseg->setIsMulticastConnSeg(state->isMulticastConn) ; // multicast
                rqseg->setMulticastGroupIndex(state->multicastGroupIndex);
                rqseg->setIsMulticastSender(state->isSender); // true
                rqseg->setIsMulticastReceiver(false);



                // for multiSourcing
                rqseg->setIsMultiSourcingConn(state->isMultiSourcingConn);
                rqseg->setMultiSourcingGroupIndex(state->multiSourcingGroupIndex) ;
                rqseg->setIsMultiSourcingSender(true);
                rqseg->setIsMultiSourcingReceiver(false);


//                 std::cout << " SEND NEW SYMBOL  state->esi " << state->esi  <<"\n\n\n";

//                     if (!( state->esi== 14 || state->esi== 15 )){
//                std::cout << " state->esi ggggg" << state->esi << std::endl;
                sendToIP(rqseg);
//                      }
//
//
//                    if ( state->esi== 18  ){
//                        itt = encodingSymbolsList.begin();
//                        int delayedSymbolIndex=15;
//                        std::advance(itt, delayedSymbolIndex-1); // increment the iterator by esi
//                        // create a segment for the symbol that will be sent
//                        RQSegment *rqseg = new RQSegment(nullptr);
//                        const char *payloadName = nullptr;
//                        payloadName = itt->msg->getName();
//                        rqseg->setPayloadLength(1000);
//                        rqseg->addPayloadMessage(itt->msg->dup(), 1);
//                        rqseg->setName(payloadName);
//                        rqseg->setIsClient(false); // we send symbol segment "(server=sender)
//                        rqseg->setSymbolSequenceNumber(delayedSymbolIndex);
//                        sendToIP(rqseg);
//                    }

            }

        }
    }

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// $$$$$$$$$$$$$$$$$$$$$$  HEADER arrived   $$$$$$$$$$$$$$$$
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// header arrived at the receiver==> send new request with pacing (fixed pacing: MTU/1Gbps)
    if (fsm.getState() == RQ_S_ESTABLISHED && rqseg->getIsHeader()==true   ) { // 1 read, 2 write

        EV  << "  $$$$$$$$$$$$$$$  Receiver $$$$$$$$$$$$$$$$$  HEADER arrived.  " << std::endl;
//         std::cout  << " \n\n\n\n\n $$$$$$$$$$$$$$$  Receiver $$$$$$$$$$$$$$$$$  HEADER arrived.  " <<  getRQMain()->getFullPath() << std::endl;

       // sendRequest();
//        ++state->request_id;
        getRQMain()->appGateIndexTimeOutMap[connId] = state->numPullsTimeOuts;
//        getRQMain()->requestCONNMap[this] = pullQueue.getLength(); // moh added
//        if (state->numberReceivedSymbols == 0){
//        getRQMain()->requestCONNMap[getRQMain()->connIndex] =this; // moh added
//        ++getRQMain()->connIndex;
//        }

        addRequestToPullsQueue();

        if (state->numberReceivedSymbols ==0 && state->connNotAddedYet == true) {
            getRQMain()->requestCONNMap[getRQMain()->connIndex] = this; // moh added

            state->connNotAddedYet = false;
            EV << "sendFirstRequest sendFirstRequest sendFirstRequest sendFirstRequest .\n";
            if (state->isMultiSourcingConn ==  true){
                getRQMain()->fillMultiSrcConnectionsInfo(this, state->multiSourcingGroupIndex , getRQMain()->connIndex) ;
            }
            ++getRQMain()->connIndex;
            getRQMain()->sendFirstRequest();
        }


    }

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// $$$$$$$$$$$$$$$$$$$$$$  SYMBOL arrived   $$$$$$$$$$$$$$$$
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// symbols arrived to the receiver (client) ==> send a new request   {{ READER }}

    // data packet arrived
    if (fsm.getState() == RQ_S_ESTABLISHED && rqseg->getIsWriter() == true
            && rqseg->getIsReader() == false && rqseg->getIsHeader() == false) { // 1 read, 2 write


        EV  << " $$$$$$$$$$$$$$$  Receiver $$$$$$$$$$$$$$$$$  SYMBOL arrived.  " << std::endl;
//        std::cout << " Receiver " << getRQMain()->getFullPath()  << "\n";

//          std::cout  << " \n\n\n\n $$$$$$$$$$$$$$$  Receiver $$$$$$$$$$$$$$$$$  SYMBOL arrived. SSN= " << rqseg->getSymbolSequenceNumber() << std::endl;
//          std::cout  << "  rqMain->connIndex = " << rqMain->connIndex << std::endl;

//          std::cout  << " connId = " << connId << std::endl;

//        std::cout << " " << cSimulation::getActiveSimulation()->getContextSimpleModule()->getFullPath() << std::endl;
//        std::cout << "  connId:  " << connId << std::endl;
        getRQMain()->appGateIndexTimeOutMap[connId] = state->numPullsTimeOuts; // moh added  timeout for sent requests (NOT USED NOW)
//        getRQMain()->requestCONNMap[this] = pullQueue.getLength(); // moh added
//         if(state->request_id==0)  ++state->request_id ;
//         if (state->esi==1)   state->request_id= 0;
//        if (arrivedSymbolsActualIndex)

        state->esi++;
        state->numberReceivedSymbols++;
        int numberReceivedSymbols = state->numberReceivedSymbols;
        int numberSentRequests = state->request_id;
        int arrivedSymbolsInternalIndex = state->esi;
        int arrivedSymbolsActualIndex = rqseg->getSymbolSequenceNumber();
        int arrivedSymbolsGap = arrivedSymbolsActualIndex- arrivedSymbolsInternalIndex;

 //        std::cout << " mmmmmmmmmmmmmm  numberReceivedSymbols= " << numberReceivedSymbols << std::endl;
//        std::cout << " mmmmmmmmmmmmmm  numberSentRequests= " << numberSentRequests << std::endl;
//        std::cout << " mmmmmmmmmmmmmm  arrivedSymbolsInternalIndex= " << arrivedSymbolsInternalIndex << std::endl;
//        std::cout << " mmmmmmmmmmmmmm  arrivedSymbolsActualIndex= " << arrivedSymbolsActualIndex << std::endl;
//        std::cout << " mmmmmmmmmmmmmm arrivedSymbolsGap= " << arrivedSymbolsGap << std::endl;
//        std::cout << " aaaaaaaaaaaa\n ";

        int initialSentSymbols = state->IW;
        int wantedSymbols = state->numSymbolsToGet;

        if ( numberReceivedSymbols > wantedSymbols) {
            state->connFinished= true;
            getRQMain()->allConnFinished();
            goto ll;

        }


       // if (requestInternalTimer->isScheduled())   cancelEvent(requestInternalTimer); // not used now


//        addRequestToPullsQueue();

        if (numberReceivedSymbols < wantedSymbols  && state->connFinished == false) {


            if (arrivedSymbolsGap == 0 && numberReceivedSymbols  <= (wantedSymbols - initialSentSymbols)) { //
//                std::cout << " sendRequest stable scenario  \n ";
               addRequestToPullsQueue();  //   mmmmmmmmmmmmmmmmmmmmmmmmmmmm


            } else if (arrivedSymbolsGap > 0 && numberReceivedSymbols<= (wantedSymbols - initialSentSymbols)) { //
//                std::cout << " sendRequest for LOST SYMBOLS \n ";
                int h = 0;
//            while (arrivedSymbolsGap >= 0 && h < initialSentSymbols) {  // MOHHHHHHHHHHHHHHHHHH
//                sendRequest();
//                ++state->request_id;
                 addRequestToPullsQueue();  //   mmmmmmmmmmmmmmmmmmmmmmmmmmmm
//                getRQMain()->requestCONNMap[this] = pullQueue.getLength(); // moh added
//                  std::cout << " sendRequest for lost symbols >>>>>>>>> "  << std::endl;
                  arrivedSymbolsGap--;
                  h++;
//               } // while

//              startRequestRexmitTimer();
                state->esi = arrivedSymbolsActualIndex;

            } else if (arrivedSymbolsGap < 0) {
                --state->esi; // to be used in case of receiving a delayed symbol
//                std::cout << "don't send a new request as we sent more previously "<< std::endl;
            } else if (arrivedSymbolsGap > 0  && numberReceivedSymbols> (wantedSymbols - initialSentSymbols)) { //numberReceivedSymbols > numberSentRequests
                int h = 0;
//                std::cout << " last windowwwww1111 \n ";

//             while (arrivedSymbolsGap > 0 && h < initialSentSymbols ) {  // MOHHHHHHHHHHHHHHHHHH
//                sendRequest();
//               ++state->request_id;
                 addRequestToPullsQueue(); //   mmmmmmmmmmmmmmmmmmmmmmmmmmmm
//                getRQMain()->requestCONNMap[this] = pullQueue.getLength(); // moh added

//                 std::cout << " sendRequest for lost symbols >>>>>>>>> " << std::endl;
                 arrivedSymbolsGap--;
                 h++;
                if (numberReceivedSymbols == wantedSymbols - 1)
                    arrivedSymbolsGap = 0; // just send one request (this will ends the while loop, but we already sent a new reques) even the gap is larger than 1, as we received  this number of symbols: wantedSymbols-1

//             } // while
//            startRequestRexmitTimer();
                state->esi = arrivedSymbolsActualIndex;
            }
//        else if (arrivedSymbolsGap == 0 && numberReceivedSymbols > numberSentRequests &&numberReceivedSymbols==13 ){
//            sendRequest();
//            startRequestRexmitTimer();
//            state->esi++;
//            std::cout << " last windowwwww2222 \n ";
//        }
            else if (arrivedSymbolsGap == 0  && numberReceivedSymbols > (wantedSymbols - initialSentSymbols)) { //
//                std::cout << " last windowwwww333 TODO NONE \n ";

//                state->connFinished= true;
                // addRequestToPullsQueue();  // ?????? >>>


                if (requestInternalTimer->isScheduled())
                    cancelEvent(requestInternalTimer);
//            startRequestRexmitTimer();
            }
        }

        cPacket *msgRx;
        uint32 endSeqNo;
        msgRx = check_and_cast<cPacket *>( rqseg->removeFirstPayloadMessage(endSeqNo)); // each segment has just one packet
//        std::cout << " endSeqNo " << endSeqNo << "\n";



        if (numberReceivedSymbols == 1 && state->connNotAddedYet == true ) {
            getRQMain()->requestCONNMap[getRQMain()->connIndex] = this; // moh added

            if (state->isMultiSourcingConn ==  true){
                getRQMain()->fillMultiSrcConnectionsInfo(this, state->multiSourcingGroupIndex , getRQMain()->connIndex) ;
            }

            ++getRQMain()->connIndex;
            state->connNotAddedYet = false;
            EV << "sendFirstRequest sendFirstRequest sendFirstRequest sendFirstRequest .\n";

            getRQMain()->sendFirstRequest();
        }



        if (state->connFinished == false){

        // buffer the received symbol segment
        std::list<EncodingSymbols>::iterator itR;  // received iterator
        itR = receivedSymbolsList.begin();
        std::advance(itR, state->esi); // increment the iterator by esi
        EncodingSymbols receivedEncodingSymbols;
        receivedEncodingSymbols.EnSyId = state->esi;
        receivedEncodingSymbols.msg = msgRx;
        receivedSymbolsList.push_back(receivedEncodingSymbols);


        // MOH: Send any received symbol to the app, just for now to test the Incast example, this shouldn't be the normal case
        cMessage *msgRcvd = nullptr;
        msgRcvd = check_and_cast<cMessage *>(msgRx);
        msgRcvd->setKind(RQ_I_DATA); // TBD currently we never send TCP_I_URGENT_DATA
        RQCommand *cmd = new RQCommand();
        cmd->setConnId(connId);
        msgRcvd->setControlInfo(cmd);
        sendToApp(msgRcvd);
        }
//         std::cout << " \n\n rrrrrrrrr = " << state->multiSourcingGroupIndex << "\n";
        // All the symbols have been received
         if (state->isfinalReceivedPrintedOut == false) {
//             std::cout << "  multiSourcingGroupIndex = " << state->multiSourcingGroupIndex << "\n";

            if (numberReceivedSymbols == wantedSymbols || state->connFinished == true) {
//            std::cout << " \n\n all symbols received, no more request to be sent \n ";
                // checking that all the symbols have been stored in the receivedSymbolsList
//            std::cout << " aaaaaaaaaaaaa  All the symbols have been received All the symbols have been received All the symbols have been received aaaaaaaaaaaaa " << receivedSymbolsList.size()<< "\n";
                std::list<EncodingSymbols>::iterator iter;  // received iterator
                iter = receivedSymbolsList.begin();
                int index = 0;
                while (iter != receivedSymbolsList.end()) {
                    std::cout << ++index << "  ESI " << iter->EnSyId << " msgName " << iter->msg->getFullName() << "\n";
                    iter++;
                }
//             std::cout << " aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa sendIndicationToAppsendIndicationToApp\n\n\n";
                sendIndicationToApp(RQ_I_PEER_CLOSED); // this is ok if the sinkApp is used by one conn
                // getRQMain()->allConnFinished();
                state->isfinalReceivedPrintedOut=true;

            }

         }


    }
    ll:
    return event;
}


void RQConnection::addRequestToPullsQueue(){
    EV << "Add new request packet to pullsQueue.\n";

    ++state->request_id;

//     std::cout << "Add new request packet to pullsQueue.  ,   request_id = " << state->request_id << " \n";

    char msgname[16];
    sprintf(msgname, "REQUEST-%d", state->request_id);
    RQSegment *rqseg = createRQSegment(msgname);
    rqseg->setPayloadLength(10);
    rqseg->setIsClient(true);

    rqseg->setIsRequestPacket(true);

    rqseg->setIsMulticastConnSeg(state->isMulticastConn); // multicast
   rqseg->setMulticastGroupIndex(state->multicastGroupIndex);
   rqseg->setIsMulticastSender(false);
   rqseg->setIsMulticastReceiver(true);


   rqseg->setIsMultiSourcingConn(state->isMultiSourcingConn);
   rqseg->setMultiSourcingGroupIndex(state->multiSourcingGroupIndex) ;
   rqseg->setIsMultiSourcingSender(false);
   rqseg->setIsMultiSourcingReceiver(true);


    rqseg->setIsReader(true);
   rqseg->setIsWriter(false);
   state->opcodeValue=1;
    rqseg->setRequestSequenceNumber(state->request_id);
    rqseg->setOpcode(state->opcodeValue);
    //    rqseg->addPayloadMessage(msg->dup(),10);
    //sendToIP(rqseg);
    pullQueue.insert(rqseg);
    EV << "Adding new request to the pull queue -- pullsQueue length now = " << pullQueue.getLength()<< "\n\n\n\n";
//    std::cout << "pullsQueue length now = " << pullQueue.getLength()<< ".\n";
//    std::cout  << " mmmm  addRequestToPullsQueue  this->getFullPath() " << rqMain->getFullPath() << " , len= " << pullQueue.getLength()   << "\n";
    bool napState = getRQMain()->getNapState();
    if (napState == true) {
        getRQMain()->requestTimer();
    }


}

void RQConnection::sendRequestFromPullsQueue(){
//    std::cout  << " ssssssssssssss sendRequestFromPullsQueuemmmm  this->getFullPath() " << rqMain->getFullPath()     << "\n";

    if (pullQueue.getLength() > 0  ){
        RQSegment *rqseg =(RQSegment *)  pullQueue.pop();
        EV << "a request has been poped from the Pull queue, the new queue length  = " << pullQueue.getLength()<< " \n\n";
        sendToIP(rqseg);
    }
}

int RQConnection::getPullsQueueLength()  {
//    std::cout << "pullsQueue length now = " <<" " << pullQueue.getLength()<< ".\n";
    int len = pullQueue.getLength();
    return len;
}


// MOH added NOT used now
void RQConnection::startRequestRexmitTimer() {

    state->request_rexmit_count = 0;
    state->request_rexmit_timeout = RQ_TIMEOUT_PULL_REXMIT;  // 0.5 sec

    if (requestInternalTimer->isScheduled())
        cancelEvent(requestInternalTimer);

    scheduleTimeout(requestInternalTimer, state->request_rexmit_timeout); // start the timer for Request segment
}

// MOH
void RQConnection::process_TIMEOUT_REQUEST_REXMIT() {
//    std::cout << "process_TIMEOUT_REQUEST_REXMIT    \n";
//    ++state->numPullsTimeOuts;
//    getRQMain()->appGateIndexTimeOutMap[connId] = state->numPullsTimeOuts; // moh added
//
//    WATCH(state->numPullsTimeOuts);
//    std::cout << "connId: " <<  connId << "\n";
//
//    std::cout << "state->numPullsTimeOuts: " << state->numPullsTimeOuts << "\n";
//    if (++state->request_rexmit_count > 20) {
//        std::cout << "Retransmission count during connection setup exceeds "<< 12 << ", giving up\n";
//        return;
//    }
//
//
//    sendRequest();
//    if (requestInternalTimer->isScheduled())
//            cancelEvent(requestInternalTimer);
//    // reschedule timer
//       state->request_rexmit_timeout *= 2;
//
//       if (state->request_rexmit_timeout > RQ_TIMEOUT_SYN_REXMIT_MAX)
//           state->request_rexmit_timeout = RQ_TIMEOUT_SYN_REXMIT_MAX;
//
//       scheduleTimeout(requestInternalTimer, state->request_rexmit_timeout);

// //    startRequestRexmitTimer();
}



bool RQConnection::isConnFinished() {
    return state->connFinished;
}

int RQConnection::getNumRcvdSymbols() {
    return state->numberReceivedSymbols;
}



void RQConnection::setConnFinished() {
    std::cout << "allReceivedCloseAllMultiSourcingConnsmmmm \n\n\n\n" ;

      state->connFinished = true;
}














//----






























bool RQConnection::hasEnoughSpaceForSegmentInReceiveQueue(RQSegment *rqseg) {
    //TODO must rewrite it
    //return (state->freeRcvBuffer >= rqseg->getPayloadLength()); // enough freeRcvBuffer in rcvQueue for new segment?

    uint32 firstSeq = receiveQueue->getFirstSeqNo();
    return seqLE(firstSeq, rqseg->getSequenceNo())
            && seqLE(rqseg->getSequenceNo() + rqseg->getPayloadLength(),
                    firstSeq + state->maxRcvBuffer);
}

RQEventCode RQConnection::processSegmentInListen(RQSegment *rqseg,  L3Address srcAddr, L3Address destAddr) {
    EV_DETAIL << "Processing segment in LISTEN\n";
//    std::cout << "Processing segment in LISTEN\n";

    //"
    // first check for an RST
    //   An incoming RST should be ignored.  Return.
    //"
    if (rqseg->getRstBit()) {
        EV_INFO << "RST bit set: dropping segment\n";
        return RQ_E_IGNORE;
    }

    //"
    // second check for an ACK
    //    Any acknowledgment is bad if it arrives on a connection still in
    //    the LISTEN state.  An acceptable reset segment should be formed
    //    for any arriving ACK-bearing segment.  The RST should be
    //    formatted as follows:
    //
    //      <SEQ=SEG.ACK><CTL=RST>
    //
    //    Return.
    //"
    if (rqseg->getAckBit()) {
        EV_INFO << "ACK bit set: dropping segment and sending RST\n";
        sendRst(rqseg->getAckNo(), destAddr, srcAddr, rqseg->getDestPort(),
                rqseg->getSrcPort());
        return RQ_E_IGNORE;
    }

    //"
    // third check for a SYN
    //"
    if (rqseg->getSynBit()) {
        if (rqseg->getFinBit()) {
            // Looks like implementations vary on how to react to SYN+FIN.
            // Some treat it as plain SYN (and reply with SYN+ACK), some send RST+ACK.
            // Let's just do the former here.
            EV_INFO << "SYN+FIN received: ignoring FIN\n";
        }

        EV_DETAIL
                         << "SYN bit set: filling in foreign socket and sending SYN+ACK\n";
//         std::cout  << "SYN bit set: filling in foreign socket and sending SYN+ACK\n";

//"
// If the listen was not fully specified (i.e., the foreign socket was not
// fully specified), then the unspecified fields should be filled in now.
//"
//
// Also, we may need to fork, in order to leave another connection
// LISTENing on the port. Note: forking will change our connId.
//
        if (state->fork) {
            RQConnection *conn = cloneListeningConnection(); // "conn" is the clone which will stay LISTENing, while "this" gets updated with the remote address
            rqMain->addForkedConnection(this, conn, destAddr, srcAddr, rqseg->getDestPort(), rqseg->getSrcPort());
            EV_DETAIL << "Connection forked: this connection got new connId="   << connId << ", " "spinoff keeps LISTENing with connId="  << conn->connId << "\n";
//            std::cout << "Connection forked: this connection got new connId="   << connId << ", " "spinoff keeps LISTENing with connId="  << conn->connId << "\n";

        } else {
            rqMain->updateSockPair(this, destAddr, srcAddr,  rqseg->getDestPort(), rqseg->getSrcPort());
        }

//"
//  Set RCV.NXT to SEG.SEQ+1, IRS is set to SEG.SEQ and any other
//  control or text should be queued for processing later.  ISS
//  should be selected and a SYN segment sent of the form:
//
//    <SEQ=ISS><ACK=RCV.NXT><CTL=SYN,ACK>
//
//  SND.NXT is set to ISS+1 and SND.UNA to ISS.  The connection
//  state should be changed to SYN-RECEIVED.
//"
        state->rcv_nxt = rqseg->getSequenceNo() + 1;
        state->rcv_adv = state->rcv_nxt + state->rcv_wnd;

        if (rcvAdvVector)
            rcvAdvVector->record(state->rcv_adv);

        state->irs = rqseg->getSequenceNo();
        receiveQueue->init(state->rcv_nxt);           // FIXME may init twice...
        selectInitialSeqNum();

// although not mentioned in RFC 793, seems like we have to pick up
// initial snd_wnd from the segment here.
        updateWndInfo(rqseg, true);

        if (rqseg->getHeaderLength() > RQ_HEADER_OCTETS) // Header options present? RQ_HEADER_OCTETS = 20
            readHeaderOptions(rqseg);



        // this is a receiver
        state->numSymbolsToGet = rqseg->getNumSymbolsToSend();

        state->isMulticastConn =  rqseg->getIsMulticastConnSeg();
        state->multicastGroupIndex = rqseg->getMulticastGroupIndex();

        state->isMultiSourcingConn = rqseg->getIsMultiSourcingConn() ;
        state->multiSourcingGroupIndex = rqseg->getMultiSourcingGroupIndex();

        state->ack_now = true;
        sendSynAck();
//        startSynRexmitTimer(); // neededddddddd mmmmmmmmmm

        if (!connEstabTimer->isScheduled())
            scheduleTimeout(connEstabTimer, RQ_TIMEOUT_CONN_ESTAB);

//"
// Note that any other incoming control or data (combined with SYN)
// will be processed in the SYN-RECEIVED state, but processing of SYN
// and ACK should not be repeated.
//"
// We don't send text in SYN or SYN+ACK, but accept it. Otherwise
// there isn't much left to do: RST, SYN, ACK, FIN got processed already,
// so there's only URG and PSH left to handle.
//
        if (rqseg->getPayloadLength() > 0) {
            updateRcvQueueVars();

            if (hasEnoughSpaceForSegmentInReceiveQueue(rqseg)) { // enough freeRcvBuffer in rcvQueue for new segment?
                receiveQueue->insertBytesFromSegment(rqseg);
            } else {   // not enough freeRcvBuffer in rcvQueue for new segment
                state->rqRcvQueueDrops++; // update current number of rq receive queue drops

                if (rqRcvQueueDropsVector)
                    rqRcvQueueDropsVector->record(state->rqRcvQueueDrops);

                EV_WARN << "RcvQueueBuffer has run out, dropping segment\n";
                return RQ_E_IGNORE;
            }
        }

        if (rqseg->getUrgBit() || rqseg->getPshBit())
            EV_DETAIL << "Ignoring URG and PSH bits in SYN\n"; // TBD

        return RQ_E_RCV_SYN; // this will take us to SYN_RCVD
    }

    //"
    //  fourth other text or control
    //   So you are unlikely to get here, but if you do, drop the segment, and return.
    //"
    EV_WARN << "Unexpected segment: dropping it\n";
    return RQ_E_IGNORE;
}

RQEventCode RQConnection::processSegmentInSynSent(RQSegment *rqseg,
        L3Address srcAddr, L3Address destAddr) {
    EV_DETAIL << "Processing segment in SYN_SENT\n";

    //"
    // first check the ACK bit
    //
    //   If the ACK bit is set
    //
    //     If SEG.ACK =< ISS, or SEG.ACK > SND.NXT, send a reset (unless
    //     the RST bit is set, if so drop the segment and return)
    //
    //       <SEQ=SEG.ACK><CTL=RST>
    //
    //     and discard the segment.  Return.
    //
    //     If SND.UNA =< SEG.ACK =< SND.NXT then the ACK is acceptable.
    //"
    if (rqseg->getAckBit()) {
        if (seqLE(rqseg->getAckNo(),
                state->iss) /* || seqGreater(rqseg->getAckNo(), state->snd_nxt) */) {
            if (rqseg->getRstBit())
                EV_DETAIL << "ACK+RST bit set but wrong AckNo, ignored\n";
            else {
                EV_DETAIL << "ACK bit set but wrong AckNo, sending RST\n";
                sendRst(rqseg->getAckNo(), destAddr, srcAddr,
                        rqseg->getDestPort(), rqseg->getSrcPort());
            }
            return RQ_E_IGNORE;
        }

        EV_DETAIL << "ACK bit set, AckNo acceptable\n";
    }

    //"
    // second check the RST bit
    //
    //   If the RST bit is set
    //
    //     If the ACK was acceptable then signal the user "error:
    //     connection reset", drop the segment, enter CLOSED state,
    //     delete TCB, and return.  Otherwise (no ACK) drop the segment
    //     and return.
    //"
    if (rqseg->getRstBit()) {
        if (rqseg->getAckBit()) {
            EV_DETAIL << "RST+ACK: performing connection reset\n";
            sendIndicationToApp(RQ_I_CONNECTION_RESET);

            return RQ_E_RCV_RST;
        } else {
            EV_DETAIL << "RST without ACK: dropping segment\n";

            return RQ_E_IGNORE;
        }
    }

    //"
    // third check the security and precedence -- not done
    //
    // fourth check the SYN bit
    //
    //   This step should be reached only if the ACK is ok, or there is
    //   no ACK, and it the segment did not contain a RST.
    //
    //   If the SYN bit is on and the security/compartment and precedence
    //   are acceptable then,
    //"
    if (rqseg->getSynBit()) {


//
//   RCV.NXT is set to SEG.SEQ+1, IRS is set to
//   SEG.SEQ.  SND.UNA should be advanced to equal SEG.ACK (if there
//   is an ACK), and any segments on the retransmission queue which
//   are thereby acknowledged should be removed.
//
        state->rcv_nxt = rqseg->getSequenceNo() + 1;
        state->rcv_adv = state->rcv_nxt + state->rcv_wnd;

        if (rcvAdvVector)
            rcvAdvVector->record(state->rcv_adv);

        state->irs = rqseg->getSequenceNo();
        receiveQueue->init(state->rcv_nxt);

        if (rqseg->getAckBit()) {
            state->snd_una = rqseg->getAckNo();
            sendQueue->discardUpTo(state->snd_una);

            //  if (state->sack_enabled)
            //     rexmitQueue->discardUpTo(state->snd_una);

            // although not mentioned in RFC 793, seems like we have to pick up
            // initial snd_wnd from the segment here.
            updateWndInfo(rqseg, true);
        }

// this also seems to be a good time to learn our local IP address
// (was probably unspecified at connection open)
        rqMain->updateSockPair(this, destAddr, srcAddr, rqseg->getDestPort(),
                rqseg->getSrcPort());

//"
//   If SND.UNA > ISS (our SYN has been ACKed), change the connection
//   state to ESTABLISHED, form an ACK segment
//
//     <SEQ=SND.NXT><ACK=RCV.NXT><CTL=ACK>
//
//   and send it.  Data or controls which were queued for
//   transmission may be included.  If there are other controls or
//   text in the segment then continue processing at the sixth step
//   below where the URG bit is checked, otherwise return.
//"
        if (seqGreater(state->snd_una, state->iss)) {
            EV_INFO << "SYN+ACK bits set, connection established.\n";

            // RFC says "continue processing at the sixth step below where
            // the URG bit is checked". Those steps deal with: URG, segment text
            // (and PSH), and FIN.
            // Now: URG and PSH we don't support yet; in SYN+FIN we ignore FIN;
            // with segment text we just take it easy and put it in the receiveQueue
            // -- we'll forward it to the user when more data arrives.
            if (rqseg->getFinBit())
                EV_DETAIL << "SYN+ACK+FIN received: ignoring FIN\n";

            if (rqseg->getPayloadLength() > 0) {
                updateRcvQueueVars();

                if (hasEnoughSpaceForSegmentInReceiveQueue(rqseg)) { // enough freeRcvBuffer in rcvQueue for new segment?
                    receiveQueue->insertBytesFromSegment(rqseg); // TBD forward to app, etc.
                } else { // not enough freeRcvBuffer in rcvQueue for new segment
                    state->rqRcvQueueDrops++; // update current number of rq receive queue drops

                    if (rqRcvQueueDropsVector)
                        rqRcvQueueDropsVector->record(state->rqRcvQueueDrops);

                    EV_WARN << "RcvQueueBuffer has run out, dropping segment\n";
                    return RQ_E_IGNORE;
                }
            }

            if (rqseg->getUrgBit() || rqseg->getPshBit())
                EV_DETAIL << "Ignoring URG and PSH bits in SYN+ACK\n"; // TBD

            if (rqseg->getHeaderLength() > RQ_HEADER_OCTETS) // Header options present? RQ_HEADER_OCTETS = 20
                readHeaderOptions(rqseg);

            // notify rqAlgorithm (it has to send ACK of SYN) and app layer
            state->ack_now = true;

            rqAlgorithm->established(true); // to send sendFirstAck() RQBaseAlg::established
            sendEstabIndicationToApp();
//            if (state->isMulticastConn == false)  {
//                rqAlgorithm->established(true); // to send sendFirstAck() RQBaseAlg::established
//                sendEstabIndicationToApp();
//            }
            ++state->numRcvdPkt;

//            if (state->isMulticastConn == true && state->numRcvdPkt != state->numReplica) {
//                return RQ_E_IGNORE;
//            }

//            if (state->isMulticastConn == true && state->numRcvdPkt == state->numReplica)  {
//                sendToIPMulticastFirstACK();
//            }

            // This will trigger transition to ESTABLISHED. Timers and notifying
            // app will be taken care of in stateEntered().
            return RQ_E_RCV_SYN_ACK;
        }

//"
//   Otherwise enter SYN-RECEIVED, form a SYN,ACK segment
//
//     <SEQ=ISS><ACK=RCV.NXT><CTL=SYN,ACK>
//
//   and send it.  If there are other controls or text in the
//   segment, queue them for processing after the ESTABLISHED state
//   has been reached, return.
//"

        EV_INFO << "SYN bit set: sending SYN+ACK\n";
        state->snd_max = state->snd_nxt = state->iss;
        sendSynAck();
        startSynRexmitTimer();

// Note: code below is similar to processing SYN in LISTEN.

// For consistency with that code, we ignore SYN+FIN here
        if (rqseg->getFinBit())
            EV_DETAIL << "SYN+FIN received: ignoring FIN\n";

// We don't send text in SYN or SYN+ACK, but accept it. Otherwise
// there isn't much left to do: RST, SYN, ACK, FIN got processed already,
// so there's only URG and PSH left to handle.
        if (rqseg->getPayloadLength() > 0) {
            updateRcvQueueVars();

            if (hasEnoughSpaceForSegmentInReceiveQueue(rqseg)) { // enough freeRcvBuffer in rcvQueue for new segment?
                receiveQueue->insertBytesFromSegment(rqseg); // TBD forward to app, etc.
            } else {   // not enough freeRcvBuffer in rcvQueue for new segment
                state->rqRcvQueueDrops++; // update current number of rq receive queue drops

                if (rqRcvQueueDropsVector)
                    rqRcvQueueDropsVector->record(state->rqRcvQueueDrops);

                EV_WARN << "RcvQueueBuffer has run out, dropping segment\n";
                return RQ_E_IGNORE;
            }
        }

        if (rqseg->getUrgBit() || rqseg->getPshBit())
            EV_DETAIL << "Ignoring URG and PSH bits in SYN\n"; // TBD

        return RQ_E_RCV_SYN;
    }

    //"
    // fifth, if neither of the SYN or RST bits is set then drop the
    // segment and return.
    //"
    return RQ_E_IGNORE;
}

RQEventCode RQConnection::processRstInSynReceived(RQSegment * rqseg) {
    EV_DETAIL << "Processing RST in SYN_RCVD\n";

    //"
    // If this connection was initiated with a passive OPEN (i.e.,
    // came from the LISTEN state), then return this connection to
    // LISTEN state and return.  The user need not be informed.  If
    // this connection was initiated with an active OPEN (i.e., came
    // from SYN-SENT state) then the connection was refused, signal
    // the user "connection refused".  In either case, all segments
    // on the retransmission queue should be removed.  And in the
    // active OPEN case, enter the CLOSED state and delete the TCB,
    // and return.
    //"

    sendQueue->discardUpTo(sendQueue->getBufferEndSeq()); // flush send queue

    // if (state->sack_enabled)
    //     rexmitQueue->discardUpTo(rexmitQueue->getBufferEndSeq()); // flush rexmit queue

    if (state->active) {
// signal "connection refused"
        sendIndicationToApp(RQ_I_CONNECTION_REFUSED);
    }

    // on RCV_RST, FSM will go either to LISTEN or to CLOSED, depending on state->active
    // FIXME if this was a forked connection, it should rather close than go back to listening (otherwise we'd now have two listening connections with the original one!)
    return RQ_E_RCV_RST;
}

bool RQConnection::processAckInEstabEtc(RQSegment *rqseg) {
    EV_DETAIL << "Processing ACK in a data transfer state\n";

    //
    //"
    //  If SND.UNA < SEG.ACK =< SND.NXT then, set SND.UNA <- SEG.ACK.
    //  Any segments on the retransmission queue which are thereby
    //  entirely acknowledged are removed.  Users should receive
    //  positive acknowledgments for buffers which have been SENT and
    //  fully acknowledged (i.e., SEND buffer should be returned with
    //  "ok" response).  If the ACK is a duplicate
    //  (SEG.ACK < SND.UNA), it can be ignored.  If the ACK acks
    //  something not yet sent (SEG.ACK > SND.NXT) then send an ACK,
    //  drop the segment, and return.
    //
    //  If SND.UNA < SEG.ACK =< SND.NXT, the send window should be
    //  updated.  If (SND.WL1 < SEG.SEQ or (SND.WL1 = SEG.SEQ and
    //  SND.WL2 =< SEG.ACK)), set SND.WND <- SEG.WND, set
    //  SND.WL1 <- SEG.SEQ, and set SND.WL2 <- SEG.ACK.
    //
    //  Note that SND.WND is an offset from SND.UNA, that SND.WL1
    //  records the sequence number of the last segment used to update
    //  SND.WND, and that SND.WL2 records the acknowledgment number of
    //  the last segment used to update SND.WND.  The check here
    //  prevents using old segments to update the window.
    //"
    // Note: should use SND.MAX instead of SND.NXT in above checks
    //
    if (seqGE(state->snd_una, rqseg->getAckNo())) {
//
// duplicate ACK? A received RQ segment is a duplicate ACK if all of
// the following apply:
//    (1) snd_una == ackNo
//    (2) segment contains no data
//    (3) there's unacked data (snd_una != snd_max)
//
// Note: ssfnet uses additional constraint "window is the same as last
// received (not an update)" -- we don't do that because window updates
// are ignored anyway if neither seqNo nor ackNo has changed.
//
        if (state->snd_una == rqseg->getAckNo()
                && rqseg->getPayloadLength() == 0
                && state->snd_una != state->snd_max) {
            state->dupacks++;

            if (dupAcksVector)
                dupAcksVector->record(state->dupacks);

            // we need to update send window even if the ACK is a dupACK, because rcv win
            // could have been changed if faulty data receiver is not respecting the "do not shrink window" rule
            updateWndInfo(rqseg);

            rqAlgorithm->receivedDuplicateAck();
        } else {
            // if doesn't qualify as duplicate ACK, just ignore it.
            if (rqseg->getPayloadLength() == 0) {
                if (state->snd_una != rqseg->getAckNo())
                    EV_DETAIL << "Old ACK: ackNo < snd_una\n";
                else if (state->snd_una == state->snd_max)
                    EV_DETAIL
                                     << "ACK looks duplicate but we have currently no unacked data (snd_una == snd_max)\n";
            }

            // reset counter
            state->dupacks = 0;

            if (dupAcksVector)
                dupAcksVector->record(state->dupacks);
        }
    } else if (seqLE(rqseg->getAckNo(), state->snd_max)) {
// ack in window.
        uint32 old_snd_una = state->snd_una;
        state->snd_una = rqseg->getAckNo();

        if (unackedVector)
            unackedVector->record(state->snd_max - state->snd_una);

// after retransmitting a lost segment, we may get an ack well ahead of snd_nxt
        if (seqLess(state->snd_nxt, state->snd_una))
            state->snd_nxt = state->snd_una;

// RFC 1323, page 36:
// "If SND.UNA < SEG.ACK =< SND.NXT then, set SND.UNA <- SEG.ACK.
// Also compute a new estimate of round-trip time.  If Snd.TS.OK
// bit is on, use my.TSclock - SEG.TSecr; otherwise use the
// elapsed time since the first segment in the retransmission
// queue was sent.  Any segments on the retransmission queue
// which are thereby entirely acknowledged."
        if (state->ts_enabled)
            rqAlgorithm->rttMeasurementCompleteUsingTS(getTSecr(rqseg));
// Note: If TS is disabled the RTT measurement is completed in RQBaseAlg::receivedDataAck()

        uint32 discardUpToSeq = state->snd_una;

// our FIN acked?
        if (state->send_fin && rqseg->getAckNo() == state->snd_fin_seq + 1) {
            // set flag that our FIN has been acked
            EV_DETAIL << "ACK acks our FIN\n";
            state->fin_ack_rcvd = true;
            discardUpToSeq--;        // the FIN sequence number is not real data
        }

// acked data no longer needed in send queue
        sendQueue->discardUpTo(discardUpToSeq);

// acked data no longer needed in rexmit queue
//  if (state->sack_enabled)
//      rexmitQueue->discardUpTo(discardUpToSeq);

        updateWndInfo(rqseg);

// if segment contains data, wait until data has been forwarded to app before sending ACK,
// otherwise we would use an old ACKNo
        if (rqseg->getPayloadLength() == 0 && fsm.getState() != RQ_S_SYN_RCVD) {
            // notify
            rqAlgorithm->receivedDataAck(old_snd_una);

            // in the receivedDataAck we need the old value
            state->dupacks = 0;

            if (dupAcksVector)
                dupAcksVector->record(state->dupacks);
        }
    } else {
        ASSERT(seqGreater(rqseg->getAckNo(), state->snd_max)); // from if-ladder

// send an ACK, drop the segment, and return.
        rqAlgorithm->receivedAckForDataNotYetSent(rqseg->getAckNo());
        state->dupacks = 0;

        if (dupAcksVector)
            dupAcksVector->record(state->dupacks);

        return false; // means "drop"
    }

    return true;
}

//----

void RQConnection::process_TIMEOUT_CONN_ESTAB() {
    switch (fsm.getState()) {
    case RQ_S_SYN_RCVD:
    case RQ_S_SYN_SENT:
// Nothing to do here. TIMEOUT_CONN_ESTAB event will automatically
// take the connection to LISTEN or CLOSED, and cancel SYN-REXMIT timer.
        if (state->active) {
            // notify user if we're on the active side
            sendIndicationToApp(RQ_I_TIMED_OUT);
        }
        break;

    default:
// We should not receive this timeout in this state.
        throw cRuntimeError(rqMain,
                "Internal error: received CONN_ESTAB timeout in state %s",
                stateName(fsm.getState()));
    }
}

void RQConnection::process_TIMEOUT_2MSL() {
    //"
    // If the time-wait timeout expires on a connection delete the TCB,
    // enter the CLOSED state and return.
    //"
    switch (fsm.getState()) {
    case RQ_S_TIME_WAIT:
// Nothing to do here. The TIMEOUT_2MSL event will automatically take
// the connection to CLOSED. We already notified the user
// (RQ_I_CLOSED) when we entered the TIME_WAIT state from CLOSING,
// FIN_WAIT_1 or FIN_WAIT_2.
        break;

    default:
// We should not receive this timeout in this state.
        throw cRuntimeError(rqMain,
                "Internal error: received time-wait (2MSL) timeout in state %s",
                stateName(fsm.getState()));
    }
}

void RQConnection::process_TIMEOUT_FIN_WAIT_2() {
    switch (fsm.getState()) {
    case RQ_S_FIN_WAIT_2:
// Nothing to do here. The TIMEOUT_FIN_WAIT_2 event will automatically take
// the connection to CLOSED.
        break;

    default:
// We should not receive this timeout in this state.
        throw cRuntimeError(rqMain,
                "Internal error: received FIN_WAIT_2 timeout in state %s",
                stateName(fsm.getState()));
    }
}

void RQConnection::startSynRexmitTimer() {
//    std::cout
//            << "  |||||| STEP (9).bb  Client ||||||   note  RQStateVariables *state contains Contains state variables ( TCB ) for RQ  e.g.   "
//            << std::endl;
//    std::cout
//            << "  |||||| STEP (9).bb  Client ||||||   uint32 snd_nxt; uint32  snd_wnd; uint32 snd_mss; int syn_rexmit_count;   simtime_t syn_rexmit_timeout;   "
//            << std::endl;

    state->syn_rexmit_count = 0;
    state->syn_rexmit_timeout = RQ_TIMEOUT_SYN_REXMIT;

    if (synRexmitTimer->isScheduled())
        cancelEvent(synRexmitTimer);

    scheduleTimeout(synRexmitTimer, state->syn_rexmit_timeout); // start the timer for SYN segment
}



void RQConnection::process_TIMEOUT_SYN_REXMIT(RQEventCode& event) {
    if (++state->syn_rexmit_count > MAX_SYN_REXMIT_COUNT) {
        EV_INFO << "Retransmission count during connection setup exceeds "
                       << MAX_SYN_REXMIT_COUNT << ", giving up\n";
// Note ABORT will take the connection to closed, and cancel CONN-ESTAB timer as well
        event = RQ_E_ABORT;
        return;
    }

    EV_INFO << "Performing retransmission #" << state->syn_rexmit_count << "\n";

    // resend what's needed
    switch (fsm.getState()) {
    case RQ_S_SYN_SENT:
        sendSyn();
        break;

    case RQ_S_SYN_RCVD:
        sendSynAck();
        break;

    default:
        throw cRuntimeError(rqMain,
                "Internal error: SYN-REXMIT timer expired while in state %s",
                stateName(fsm.getState()));
    }

    // reschedule timer
    state->syn_rexmit_timeout *= 2;

    if (state->syn_rexmit_timeout > RQ_TIMEOUT_SYN_REXMIT_MAX)
        state->syn_rexmit_timeout = RQ_TIMEOUT_SYN_REXMIT_MAX;

    scheduleTimeout(synRexmitTimer, state->syn_rexmit_timeout);
}

void RQConnection::segmentArrivalWhileClosed(RQSegment *rqseg,
        L3Address srcAddr, L3Address destAddr) {
    EV_INFO << "Seg arrived: ";
    printSegmentBrief(rqseg);

    // This segment doesn't belong to any connection, so this object
    // must be a temp object created solely for the purpose of calling us

    ASSERT(state == nullptr);

    EV_INFO << "Segment doesn't belong to any existing connection\n";

    // RFC 793:
    //"
    // all data in the incoming segment is discarded.  An incoming
    // segment containing a RST is discarded.  An incoming segment not
    // containing a RST causes a RST to be sent in response.  The
    // acknowledgment and sequence field values are selected to make the
    // reset sequence acceptable to the RQ that sent the offending
    // segment.
    //
    // If the ACK bit is off, sequence number zero is used,
    //
    //    <SEQ=0><ACK=SEG.SEQ+SEG.LEN><CTL=RST,ACK>
    //
    // If the ACK bit is on,
    //
    //    <SEQ=SEG.ACK><CTL=RST>
    //
    // ...
    //
    //    SEG.LEN = the number of octets occupied by the data in the segment
    //              (counting SYN and FIN)
    //"
    if (rqseg->getRstBit()) {
        EV_DETAIL << "RST bit set: dropping segment\n";
        return;
    }

    if (!rqseg->getAckBit()) {
        EV_DETAIL << "ACK bit not set: sending RST+ACK\n";
        uint32 ackNo = rqseg->getSequenceNo() + rqseg->getSegLen();
        sendRstAck(0, ackNo, destAddr, srcAddr, rqseg->getDestPort(),
                rqseg->getSrcPort());
    } else {
        EV_DETAIL << "ACK bit set: sending RST\n";
        sendRst(rqseg->getAckNo(), destAddr, srcAddr, rqseg->getDestPort(),
                rqseg->getSrcPort());
    }
}
//
//TBD:
//"
// USER TIMEOUT
//
//    For any state if the user timeout expires, flush all queues, signal
//    the user "error:  connection aborted due to user timeout" in general
//    and for any outstanding calls, delete the TCB, enter the CLOSED
//    state and return.
//"

}// namespace rq

} // namespace inet

