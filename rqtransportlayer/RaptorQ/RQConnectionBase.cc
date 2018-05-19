#include <string.h>
#include <assert.h>
#include "RaptorQ.h"
#include "RQConnection.h"
#include "raptorq_common/RQSegment.h"
#include "../contract/raptorq/RQCommand_m.h"
#include "RQSendQueue.h"
#include "RQReceiveQueue.h"
#include "RQAlgorithm.h"
//#include "RQSACKRexmitQueue.h"

using namespace std;
namespace inet {

namespace raptorq {

RQStateVariables::RQStateVariables() {



    numSymbolsToGet = 0;
    numSymbolsToSend = 0;
    esi = 0;   // encoding symbol id
    sbn = 0;  // source block number (8-bit unsigned integer)
    request_id = 0;  // source block number (8-bit unsigned integer)
    numberReceivedSymbols = 0;
    numberSentSymbols=0;
    IW=8; // send the initial window (12 symbols as in NDP) IWWWWWWWWWWWW
    connFinished= false;

    isfinalReceivedPrintedOut = false;

    isMulticastConn= false;
    multicastGroupIndex = -1;

    isMultiSourcingConn = false;
    multiSourcingGroupIndex = -1;
    forcMultiSrcConnClose = false;


    numReplica = 0;
    isSender =false;


    numRcvdPkt=0;



    request_rexmit_count = 0;
    request_rexmit_timeout=0;
    opcodeValue=0;
    numPullsTimeOuts=0;
    isHeader=false;
    connNotAddedYet= true;
    // set everything to 0 -- real init values will be set manually
    active = false;
    fork = false;
    snd_mss = 0; // will initially be set from configureStateVariables() and probably reset during connection setup
    snd_una = 0;
    snd_nxt = 0;
    snd_max = 0;
    snd_wnd = 0;
    snd_up = 0;
    snd_wl1 = 0;
    snd_wl2 = 0;
    iss = 0;

    rcv_nxt = 0;
    rcv_wnd = 0;    // will be set from configureStateVariables()
    rcv_up = 0;
    irs = 0;
    rcv_adv = 0;    // will be set from configureStateVariables()


    syn_rexmit_count = 0;
    syn_rexmit_timeout = 0;

    fin_ack_rcvd = false;
    send_fin = false;
    snd_fin_seq = 0;
    fin_rcvd = false;
    rcv_fin_seq = 0;
    sentBytes = 0;

    nagle_enabled = false;    // will be set from configureStateVariables()
    delayed_acks_enabled = false;  // will be set from configureStateVariables()
    limited_transmit_enabled = false; // will be set from configureStateVariables()
    increased_IW_enabled = false;  // will be set from configureStateVariables()
    full_sized_segment_counter = 0;
    ack_now = false;

    afterRto = false;

    time_last_data_sent = 0;

    ws_support = false;    // will be set from configureStateVariables()
    ws_enabled = false;
    snd_ws = false;
    rcv_ws = false;
    rcv_wnd_scale = 0;    // will be set from configureStateVariables()
    snd_wnd_scale = 0;

    ts_support = false;    // will be set from configureStateVariables()
    ts_enabled = false;
    snd_initial_ts = false;
    rcv_initial_ts = false;
    ts_recent = 0;
    last_ack_sent = 0;

//    sack_support = false;    // will be set from configureStateVariables()
//    sack_enabled = false;
//    snd_sack_perm = false;
//    rcv_sack_perm = false;

//    snd_sack = false;
//    snd_dsack = false;
//    start_seqno = 0;
//    end_seqno = 0;
//    highRxt = 0;
//    pipe = 0;
//    recoveryPoint = 0;
//    sackedBytes = 0;
//    sackedBytes_old = 0;
//    lossRecovery = false;

    dupacks = 0;
//    snd_sacks = 0;
//    rcv_sacks = 0;
    rcv_oooseg = 0;
    rcv_naseg = 0;

    maxRcvBuffer = 0;    // will be set from configureStateVariables()
    usedRcvBuffer = 0;
    freeRcvBuffer = 0;
    rqRcvQueueDrops = 0;
    sendQueueLimit = 0;
    queueUpdate = true;
}

std::string RQStateVariables::info() const {
    std::stringstream out;
    out << "snd_una=" << snd_una;
    out << " snd_nxt=" << snd_nxt;
    out << " snd_max=" << snd_max;
    out << " snd_wnd=" << snd_wnd;
    out << " rcv_nxt=" << rcv_nxt;
    out << " rcv_wnd=" << rcv_wnd;
    return out.str();
}

std::string RQStateVariables::detailedInfo() const {
    std::stringstream out;
    out << "active=" << active << "\n";
    out << "snd_mss=" << snd_mss << "\n";
    out << "snd_una=" << snd_una << "\n";
    out << "snd_nxt=" << snd_nxt << "\n";
    out << "snd_max=" << snd_max << "\n";
    out << "snd_wnd=" << snd_wnd << "\n";
    out << "snd_up=" << snd_up << "\n";
    out << "snd_wl1=" << snd_wl1 << "\n";
    out << "snd_wl2=" << snd_wl2 << "\n";
    out << "iss=" << iss << "\n";
    out << "rcv_nxt=" << rcv_nxt << "\n";
    out << "rcv_wnd=" << rcv_wnd << "\n";
    out << "rcv_up=" << rcv_up << "\n";
    out << "irs=" << irs << "\n";
    out << "rcv_adv=" << rcv_adv << "\n";
    out << "fin_ack_rcvd=" << fin_ack_rcvd << "\n";
    out << "nagle_enabled=" << nagle_enabled << "\n";
    out << "limited_transmit_enabled=" << limited_transmit_enabled << "\n";
    out << "increased_IW_enabled=" << increased_IW_enabled << "\n";
    out << "delayed_acks_enabled=" << delayed_acks_enabled << "\n";
    out << "ws_support=" << ws_support << "\n";
    out << "ws_enabled=" << ws_enabled << "\n";
    out << "ts_support=" << ts_support << "\n";
    out << "ts_enabled=" << ts_enabled << "\n";
//    out << "sack_support=" << sack_support << "\n";
//    out << "sack_enabled=" << sack_enabled << "\n";
//    out << "snd_sack_perm=" << snd_sack_perm << "\n";
//    out << "snd_sacks=" << snd_sacks << "\n";
//    out << "rcv_sacks=" << rcv_sacks << "\n";
    out << "dupacks=" << dupacks << "\n";
    out << "rcv_oooseg=" << rcv_oooseg << "\n";
    out << "rcv_naseg=" << rcv_naseg << "\n";
    return out.str();
}

RQConnection::RQConnection() {
    // Note: this ctor is NOT used to create live connections, only
    // temporary ones to invoke segmentArrivalWhileClosed() on
    transferMode = RQ_TRANSFER_BYTECOUNT;
}

//
// FSM framework, RQ FSM
//

RQConnection::RQConnection(RaptorQ *_mod, int _appGateIndex, int _connId) {
    rqMain = _mod;
    appGateIndex = _appGateIndex;
    connId = _connId;

    char fsmname[24];
    sprintf(fsmname, "fsm-%d", connId);
    fsm.setName(fsmname);
    fsm.setState(RQ_S_INIT);

    // queues and algorithm will be created on active or passive open

    requestInternalTimer = new cMessage("requestInternalTimer");
    the2MSLTimer = new cMessage("2MSL");
    connEstabTimer = new cMessage("CONN-ESTAB");
    finWait2Timer = new cMessage("FIN-WAIT-2");
    synRexmitTimer = new cMessage("SYN-REXMIT");

    requestInternalTimer->setContextPointer(this);
    the2MSLTimer->setContextPointer(this);
    connEstabTimer->setContextPointer(this);
    finWait2Timer->setContextPointer(this);
    synRexmitTimer->setContextPointer(this);

    // statistics
    if (getRQMain()->recordStatistics) {
        sndWndVector = new cOutVector("send window");
        rcvWndVector = new cOutVector("receive window");
        rcvAdvVector = new cOutVector("advertised window");
        sndNxtVector = new cOutVector("sent seq");
        sndAckVector = new cOutVector("sent ack");
        rcvSeqVector = new cOutVector("rcvd seq");
        rcvAckVector = new cOutVector("rcvd ack");
        unackedVector = new cOutVector("unacked bytes");
        dupAcksVector = new cOutVector("rcvd dupAcks");
        pipeVector = new cOutVector("pipe");
//        sndSacksVector = new cOutVector("sent sacks");
//        rcvSacksVector = new cOutVector("rcvd sacks");
//        rcvOooSegVector = new cOutVector("rcvd oooseg");
//        rcvNASegVector = new cOutVector("rcvd naseg");
        //sackedBytesVector = new cOutVector("rcvd sackedBytes");
        //rqRcvQueueBytesVector = new cOutVector("RQRcvQueueBytes");
        //rqRcvQueueDropsVector = new cOutVector("RQRcvQueueDrops");
    }


}

RQConnection::~RQConnection() {

    std::list<EncodingSymbols>::iterator iter;  // received iterator
    iter = encodingSymbolsList.begin();
    while (!encodingSymbolsList.empty()) {
        //      std::cout << " ESI " << iter->EnSyId << " msgName "  << iter->msg->getFullName() << "\n";
        delete encodingSymbolsList.front().msg;
        iter++;
        encodingSymbolsList.pop_front();
    }

    while (!receivedSymbolsList.empty()) {
        delete receivedSymbolsList.front().msg;
        receivedSymbolsList.pop_front();
    }

    delete sendQueue;
//    delete rexmitQueue;
    delete receiveQueue;
    delete rqAlgorithm;
    delete state;

    if (the2MSLTimer)
        delete cancelEvent(the2MSLTimer);
    if (connEstabTimer)
        delete cancelEvent(connEstabTimer);
    if (finWait2Timer)
        delete cancelEvent(finWait2Timer);
    if (synRexmitTimer)
        delete cancelEvent(synRexmitTimer);
    if (requestInternalTimer)
        delete cancelEvent(requestInternalTimer);

    // statistics
    delete sndWndVector;
    delete rcvWndVector;
    delete rcvAdvVector;
    delete sndNxtVector;
    delete sndAckVector;
    delete rcvSeqVector;
    delete rcvAckVector;
    delete unackedVector;
    delete dupAcksVector;
//    delete sndSacksVector;
//    delete rcvSacksVector;
    delete rcvOooSegVector;
    delete rcvNASegVector;
    delete rqRcvQueueBytesVector;
    delete rqRcvQueueDropsVector;
    delete pipeVector;
    //  delete sackedBytesVector;
}

bool RQConnection::processTimer(cMessage *msg) {
    printConnBrief();
    EV_DETAIL << msg->getName() << " timer expired\n";
    std::cout << msg->getFullPath() << " timer expired\n";

    // first do actions
    RQEventCode event;

    if (msg == requestInternalTimer) {
        event = RQ_E_IGNORE;
        process_TIMEOUT_REQUEST_REXMIT();
//            sendRequest();   // based on my congestion control, I send requests after receiving  a symbol not based on a timer
    }

    else if (msg == the2MSLTimer) {
        event = RQ_E_TIMEOUT_2MSL;
        process_TIMEOUT_2MSL();
    } else if (msg == connEstabTimer) {
        event = RQ_E_TIMEOUT_CONN_ESTAB;
        process_TIMEOUT_CONN_ESTAB();
    } else if (msg == finWait2Timer) {
        event = RQ_E_TIMEOUT_FIN_WAIT_2;
        process_TIMEOUT_FIN_WAIT_2();
    } else if (msg == synRexmitTimer) {
        event = RQ_E_IGNORE;
        process_TIMEOUT_SYN_REXMIT(event);
    } else {
        std::cout << " elseeeeeeee\n";
        event = RQ_E_IGNORE;
        rqAlgorithm->processTimer(msg, event); // seeeee processTimer method in RQBaseAlg.cc
    }

    // then state transitions
    return performStateTransition(event);
}

////////////////////////////////////  Step (10) ///////////////////////////////////////
bool RQConnection::processRQSegment(RQSegment *RQseg, L3Address segSrcAddr,
        L3Address segDestAddr) {

    printConnBrief();
//    if (!localAddr.isUnspecified()) {
//        ASSERT(localAddr == segDestAddr);
//        ASSERT(localPort == RQseg->getDestPort());
//    }
//
//    if (!remoteAddr.isUnspecified()) {
//        ASSERT(remoteAddr == segSrcAddr);
//        ASSERT(remotePort == RQseg->getSrcPort());
//    }

    if (tryFastRoute(RQseg))
        return true;

    // first do actions
    RQEventCode event = process_RCV_SEGMENT(RQseg, segSrcAddr, segDestAddr);

    // then state transitions
    return performStateTransition(event);
}

 bool RQConnection::processAppCommand(cMessage *msg) {

    printConnBrief();

    // first do actions
    RQCommand *rqCommand = (RQCommand *) (msg->removeControlInfo());
    RQEventCode event = preanalyseAppCommandEvent(msg->getKind());
    EV_INFO << "App command eventName: " << eventName(event) << "\n";

//     std::cout << "App command: " << eventName(event) << std::endl;
    switch (event) {
    case RQ_E_OPEN_ACTIVE:
        process_OPEN_ACTIVE(event, rqCommand, msg);
        break;

    case RQ_E_OPEN_PASSIVE:
        process_OPEN_PASSIVE(event, rqCommand, msg);
        break;

    case RQ_E_SEND:
        process_SEND(event, rqCommand, msg);
        break;

    case RQ_E_CLOSE:
        process_CLOSE(event, rqCommand, msg);
        break;

    case RQ_E_ABORT:
        process_ABORT(event, rqCommand, msg);
        break;

    case RQ_E_STATUS:
        process_STATUS(event, rqCommand, msg);
        break;

    case RQ_E_QUEUE_BYTES_LIMIT:
        process_QUEUE_BYTES_LIMIT(event, rqCommand, msg);
        break;

    case RQ_E_READ:
        process_READ_REQUEST(event, rqCommand, msg);
        break;

    default:
        throw cRuntimeError(rqMain, "wrong event code");
    }

    // then state transitions
    return performStateTransition(event);
}

RQEventCode RQConnection::preanalyseAppCommandEvent(int commandCode) {
    switch (commandCode) {
    case RQ_C_OPEN_ACTIVE:
        return RQ_E_OPEN_ACTIVE;

    case RQ_C_OPEN_PASSIVE:
        return RQ_E_OPEN_PASSIVE;

    case RQ_C_SEND:
        return RQ_E_SEND;

    case RQ_C_CLOSE:
        return RQ_E_CLOSE;

    case RQ_C_ABORT:
        return RQ_E_ABORT;

    case RQ_C_STATUS:
        return RQ_E_STATUS;

    case RQ_C_QUEUE_BYTES_LIMIT:
        return RQ_E_QUEUE_BYTES_LIMIT;

    case RQ_C_READ:
        return RQ_E_READ;

    default:
        throw cRuntimeError(rqMain, "Unknown message kind in app command");
    }
}

bool RQConnection::performStateTransition(const RQEventCode& event) {
    ASSERT(fsm.getState() != RQ_S_CLOSED); // closed connections should be deleted immediately

    if (event == RQ_E_IGNORE) {    // e.g. discarded segment
        EV_DETAIL << "Staying in state: " << stateName(fsm.getState())
                         << " (no FSM event)\n";
        return true;
    }

    // state machine
    // TBD add handling of connection timeout event (KEEP-ALIVE), with transition to CLOSED
    // Note: empty "default:" lines are for gcc's benefit which would otherwise spit warnings
    int oldState = fsm.getState();

    switch (fsm.getState()) {
    case RQ_S_INIT:
        switch (event) {
        case RQ_E_OPEN_PASSIVE:
            FSM_Goto(fsm, RQ_S_LISTEN);
            break;

        case RQ_E_OPEN_ACTIVE:
            FSM_Goto(fsm, RQ_S_SYN_SENT);
            break;

        default:
            break;
        }
        break;

    case RQ_S_LISTEN:
        switch (event) {
        case RQ_E_OPEN_ACTIVE:
            FSM_Goto(fsm, RQ_S_SYN_SENT);
            break;

        case RQ_E_SEND:
            FSM_Goto(fsm, RQ_S_SYN_SENT);
            break;

        case RQ_E_CLOSE:
            FSM_Goto(fsm, RQ_S_CLOSED);
            break;

        case RQ_E_ABORT:
            FSM_Goto(fsm, RQ_S_CLOSED);
            break;

        case RQ_E_RCV_SYN:
            FSM_Goto(fsm, RQ_S_SYN_RCVD);
            break;

        default:
            break;
        }
        break;

    case RQ_S_SYN_RCVD:
        switch (event) {
        case RQ_E_CLOSE:
            FSM_Goto(fsm, RQ_S_FIN_WAIT_1);
            break;

        case RQ_E_ABORT:
            FSM_Goto(fsm, RQ_S_CLOSED);
            break;

        case RQ_E_TIMEOUT_CONN_ESTAB:
            FSM_Goto(fsm, state->active ? RQ_S_CLOSED : RQ_S_LISTEN);
            break;

        case RQ_E_RCV_RST:
            FSM_Goto(fsm, state->active ? RQ_S_CLOSED : RQ_S_LISTEN);
            break;

        case RQ_E_RCV_ACK:
            FSM_Goto(fsm, RQ_S_ESTABLISHED);
            break;

        case RQ_E_RCV_FIN:
            FSM_Goto(fsm, RQ_S_CLOSE_WAIT);
            break;

        case RQ_E_RCV_UNEXP_SYN:
            FSM_Goto(fsm, RQ_S_CLOSED);
            break;

        default:
            break;
        }
        break;

    case RQ_S_SYN_SENT:
        switch (event) {
        case RQ_E_CLOSE:
            FSM_Goto(fsm, RQ_S_CLOSED);
            break;

        case RQ_E_ABORT:
            FSM_Goto(fsm, RQ_S_CLOSED);
            break;

        case RQ_E_TIMEOUT_CONN_ESTAB:
            FSM_Goto(fsm, RQ_S_CLOSED);
            break;

        case RQ_E_RCV_RST:
            FSM_Goto(fsm, RQ_S_CLOSED);
            break;

        case RQ_E_RCV_SYN_ACK:
            FSM_Goto(fsm, RQ_S_ESTABLISHED);
            break;

        case RQ_E_RCV_SYN:
            FSM_Goto(fsm, RQ_S_SYN_RCVD);
            break;

        default:
            break;
        }
        break;

    case RQ_S_ESTABLISHED:
        switch (event) {
        case RQ_E_CLOSE:
            FSM_Goto(fsm, RQ_S_FIN_WAIT_1);
            break;

        case RQ_E_ABORT:
            FSM_Goto(fsm, RQ_S_CLOSED);
            break;

        case RQ_E_RCV_FIN:
            FSM_Goto(fsm, RQ_S_CLOSE_WAIT);
            break;

        case RQ_E_RCV_RST:
            FSM_Goto(fsm, RQ_S_CLOSED);
            break;

        case RQ_E_RCV_UNEXP_SYN:
            FSM_Goto(fsm, RQ_S_CLOSED);
            break;

        default:
            break;
        }
        break;

    case RQ_S_CLOSE_WAIT:
        switch (event) {
        case RQ_E_CLOSE:
            FSM_Goto(fsm, RQ_S_LAST_ACK);
            break;

        case RQ_E_ABORT:
            FSM_Goto(fsm, RQ_S_CLOSED);
            break;

        case RQ_E_RCV_RST:
            FSM_Goto(fsm, RQ_S_CLOSED);
            break;

        case RQ_E_RCV_UNEXP_SYN:
            FSM_Goto(fsm, RQ_S_CLOSED);
            break;

        default:
            break;
        }
        break;

    case RQ_S_LAST_ACK:
        switch (event) {
        case RQ_E_ABORT:
            FSM_Goto(fsm, RQ_S_CLOSED);
            break;

        case RQ_E_RCV_ACK:
            FSM_Goto(fsm, RQ_S_CLOSED);
            break;

        case RQ_E_RCV_RST:
            FSM_Goto(fsm, RQ_S_CLOSED);
            break;

        case RQ_E_RCV_UNEXP_SYN:
            FSM_Goto(fsm, RQ_S_CLOSED);
            break;

        default:
            break;
        }
        break;

    case RQ_S_FIN_WAIT_1:
        switch (event) {
        case RQ_E_ABORT:
            FSM_Goto(fsm, RQ_S_CLOSED);
            break;

        case RQ_E_RCV_FIN:
            FSM_Goto(fsm, RQ_S_CLOSING);
            break;

        case RQ_E_RCV_ACK:
            FSM_Goto(fsm, RQ_S_FIN_WAIT_2);
            break;

        case RQ_E_RCV_FIN_ACK:
            FSM_Goto(fsm, RQ_S_TIME_WAIT);
            break;

        case RQ_E_RCV_RST:
            FSM_Goto(fsm, RQ_S_CLOSED);
            break;

        case RQ_E_RCV_UNEXP_SYN:
            FSM_Goto(fsm, RQ_S_CLOSED);
            break;

        default:
            break;
        }
        break;

    case RQ_S_FIN_WAIT_2:
        switch (event) {
        case RQ_E_ABORT:
            FSM_Goto(fsm, RQ_S_CLOSED);
            break;

        case RQ_E_RCV_FIN:
            FSM_Goto(fsm, RQ_S_TIME_WAIT);
            break;

        case RQ_E_TIMEOUT_FIN_WAIT_2:
            FSM_Goto(fsm, RQ_S_CLOSED);
            break;

        case RQ_E_RCV_RST:
            FSM_Goto(fsm, RQ_S_CLOSED);
            break;

        case RQ_E_RCV_UNEXP_SYN:
            FSM_Goto(fsm, RQ_S_CLOSED);
            break;

        default:
            break;
        }
        break;

    case RQ_S_CLOSING:
        switch (event) {
        case RQ_E_ABORT:
            FSM_Goto(fsm, RQ_S_CLOSED);
            break;

        case RQ_E_RCV_ACK:
            FSM_Goto(fsm, RQ_S_TIME_WAIT);
            break;

        case RQ_E_RCV_RST:
            FSM_Goto(fsm, RQ_S_CLOSED);
            break;

        case RQ_E_RCV_UNEXP_SYN:
            FSM_Goto(fsm, RQ_S_CLOSED);
            break;

        default:
            break;
        }
        break;

    case RQ_S_TIME_WAIT:
        switch (event) {
        case RQ_E_ABORT:
            FSM_Goto(fsm, RQ_S_CLOSED);
            break;

        case RQ_E_TIMEOUT_2MSL:
            FSM_Goto(fsm, RQ_S_CLOSED);
            break;

        case RQ_E_RCV_RST:
            FSM_Goto(fsm, RQ_S_CLOSED);
            break;

        case RQ_E_RCV_UNEXP_SYN:
            FSM_Goto(fsm, RQ_S_CLOSED);
            break;

        default:
            break;
        }
        break;

    case RQ_S_CLOSED:
        break;
    }

    if (oldState != fsm.getState()) {
        EV_INFO << "Transition: " << stateName(oldState) << " --> "
                       << stateName(fsm.getState()) << "  (event was: "
                       << eventName(event) << ")\n";
        EV_DEBUG_C("testing") << rqMain->getName() << ": "
                                     << stateName(oldState) << " --> "
                                     << stateName(fsm.getState()) << "  (on "
                                     << eventName(event) << ")\n";

        // cancel timers, etc.
        stateEntered(fsm.getState(), oldState, event);
    } else {
        EV_DETAIL << "Staying in state: " << stateName(fsm.getState())
                         << " (event was: " << eventName(event) << ")\n";
    }

    return fsm.getState() != RQ_S_CLOSED;
}

void RQConnection::stateEntered(int state, int oldState, RQEventCode event) {
    // cancel timers
    switch (state) {
    case RQ_S_INIT:
        // we'll never get back to INIT
        break;

    case RQ_S_LISTEN:
        // we may get back to LISTEN from SYN_RCVD
        ASSERT(connEstabTimer && synRexmitTimer);
        cancelEvent(connEstabTimer);
        cancelEvent(synRexmitTimer);
        break;

    case RQ_S_SYN_RCVD:
    case RQ_S_SYN_SENT:
        break;

    case RQ_S_ESTABLISHED:
        // we're in ESTABLISHED, these timers are no longer needed
        delete cancelEvent(connEstabTimer);
        delete cancelEvent(synRexmitTimer);
        connEstabTimer = synRexmitTimer = nullptr;
        // RQ_I_ESTAB notification moved inside event processing
        break;

    case RQ_S_CLOSE_WAIT:
    case RQ_S_LAST_ACK:
    case RQ_S_FIN_WAIT_1:
    case RQ_S_FIN_WAIT_2:
    case RQ_S_CLOSING:
        if (state == RQ_S_CLOSE_WAIT)
            sendIndicationToApp(RQ_I_PEER_CLOSED);
        // whether connection setup succeeded (ESTABLISHED) or not (others),
        // cancel these timers
        if (connEstabTimer)
            cancelEvent(connEstabTimer);
        if (synRexmitTimer)
            cancelEvent(synRexmitTimer);
        break;

    case RQ_S_TIME_WAIT:
        sendIndicationToApp(RQ_I_CLOSED);
        break;

    case RQ_S_CLOSED:
        if (oldState != RQ_S_TIME_WAIT && event != RQ_E_ABORT)
            sendIndicationToApp(RQ_I_CLOSED);
        // all timers need to be cancelled
        if (the2MSLTimer)
            cancelEvent(the2MSLTimer);
        if (connEstabTimer)
            cancelEvent(connEstabTimer);
        if (finWait2Timer)
            cancelEvent(finWait2Timer);
        if (synRexmitTimer)
            cancelEvent(synRexmitTimer);
        rqAlgorithm->connectionClosed();
        break;
    }
}

} // namespace RQ

} // namespace inet

