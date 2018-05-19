#include "RQSocket.h"

namespace inet {

RQSocket::RQSocket() {
//    std::cout << "  |||||| STEP 0 Create a socket (RQSocket::RQSocket()) ||||||  " << std::endl;
    // don't allow user-specified connIds because they may conflict with
    // automatically assigned ones.
    connId = getEnvir()->getUniqueNumber();
    sockstate = NOT_BOUND;

    localPrt = remotePrt = -1;
    cb = nullptr;
    yourPtr = nullptr;

    gateToRq = nullptr;
    dataTransferMode = RQ_TRANSFER_UNDEFINED;
}

RQSocket::RQSocket(cMessage *msg) {
    RQCommand *ind = dynamic_cast<RQCommand *>(msg->getControlInfo());

    if (!ind)
        throw cRuntimeError(
                "RQSocket::RQSocket(cMessage *): no RQCommand control info in message (not from RQ?)");

    connId = ind->getConnId();
    sockstate = CONNECTED;

    localPrt = remotePrt = -1;
    cb = nullptr;
    yourPtr = nullptr;
    dataTransferMode = RQ_TRANSFER_UNDEFINED;    // FIXME set dataTransferMode
    gateToRq = nullptr;

    if (msg->getKind() == RQ_I_ESTABLISHED) {
        // management of stockstate is left to processMessage() so we always
        // set it to CONNECTED in the ctor, whatever RQ_I_xxx arrives.
        // However, for convenience we extract RQConnectInfo already here, so that
        // remote address/port can be read already after the ctor call.

        RQConnectInfo *connectInfo = check_and_cast<RQConnectInfo *>(
                msg->getControlInfo());
        localAddr = connectInfo->getLocalAddr();
        remoteAddr = connectInfo->getRemoteAddr();
        localPrt = connectInfo->getLocalPort();
        remotePrt = connectInfo->getRemotePort();
    }
}

RQSocket::~RQSocket() {
    if (cb)
        cb->socketDeleted(connId, yourPtr);
}

const char *RQSocket::stateName(int state) {
#define CASE(x)    case x: \
        s = #x; break
    const char *s = "unknown";
    switch (state) {
    CASE(NOT_BOUND)
;        CASE(BOUND);
        CASE(LISTENING);
        CASE(CONNECTING);
        CASE(CONNECTED);
        CASE(PEER_CLOSED);
        CASE(LOCALLY_CLOSED);
        CASE(CLOSED);
        CASE(SOCKERROR);
    }
    return s;
#undef CASE
}

void RQSocket::sendToRQ(cMessage *msg) {
    if (!gateToRq)
        throw cRuntimeError(
                "RQSocket: setOutputGate() must be invoked before socket can be used");

    check_and_cast<cSimpleModule *>(gateToRq->getOwnerModule())->send(msg,  gateToRq);
}

void RQSocket::bind(int lPort) {
    if (sockstate != NOT_BOUND)
        throw cRuntimeError("RQSocket::bind(): socket already bound");

    if (lPort < 0 || lPort > 65535)
        throw cRuntimeError("RQSocket::bind(): invalid port number %d", lPort);

    localPrt = lPort;
    sockstate = BOUND;
}

////////////////////////////////////  Step (1) ///////////////////////////////////////
void RQSocket::bind(L3Address lAddr, int lPort) {
    if (sockstate != NOT_BOUND)
        throw cRuntimeError("RQSocket::bind(): socket already bound");

    // allow -1 here, to make it possible to specify address only
    if ((lPort < 0 || lPort > 65535) && lPort != -1)
        throw cRuntimeError("RQSocket::bind(): invalid port number %d", lPort);
//std::cout << " bind localAddr " << lAddr << "\n";
    localAddr = lAddr;
    localPrt = lPort;
    sockstate = BOUND;
}

////////////////////////////////////  Step (2).Server ///////////////////////////////////////
void RQSocket::listen(bool fork) {

    if (sockstate != BOUND)
        throw cRuntimeError(
                sockstate == NOT_BOUND ?
                        "RQSocket: must call bind() before listen()" :
                        "RQSocket::listen(): connect() or listen() already called");
//    std::cout << "  |||||| STEP (2).Server ||||||    RQSocket::listen() ,, sendToRq cMessage(PassiveOPEN, RQ_C_OPEN_PASSIVE) "<< std::endl;

    cMessage *msg = new cMessage("PassiveOPEN", RQ_C_OPEN_PASSIVE);

    RQOpenCommand *openCmd = new RQOpenCommand();
    openCmd->setLocalAddr(localAddr);
    openCmd->setLocalPort(localPrt);
    openCmd->setConnId(connId);
    openCmd->setFork(fork);
    openCmd->setDataTransferMode(dataTransferMode);
    openCmd->setRqAlgorithmClass(rqAlgorithmClass.c_str());
    openCmd->setIsSender(false);
    msg->setControlInfo(openCmd);
    sendToRQ(msg);
    sockstate = LISTENING;
}


void RQSocket::connect(L3Address remoteAddress, int remotePort, int fileID , unsigned int numSymbolsToSend , bool isLongFlow , bool isMulticastConn ,  unsigned int multicastGroupIndex ,  bool isMultiSourcingConn, unsigned int multiSourcingGroupIndex) {
    if (sockstate != NOT_BOUND && sockstate != BOUND)
        throw cRuntimeError( "RQSocket::connect(): connect() or listen() already called (need renewSocket()?)");

    if (remotePort < 0 || remotePort > 65535)
        throw cRuntimeError( "RQSocket::connect(): invalid remote port number %d", remotePort);


    cMessage *msg = new cMessage("ActiveOPEN", RQ_C_OPEN_ACTIVE);

    remoteAddr = remoteAddress;
    remotePrt = remotePort;

    RQOpenCommand *openCmd = new RQOpenCommand();
    openCmd->setConnId(connId);
    openCmd->setLocalAddr(localAddr);
    openCmd->setLocalPort(localPrt);
    openCmd->setRemoteAddr(remoteAddr);
    openCmd->setRemotePort(remotePrt);
    openCmd->setDataTransferMode(dataTransferMode);
    openCmd->setRqAlgorithmClass(rqAlgorithmClass.c_str());
    // number of symbols to send
    openCmd->setNumSymbolsToSend(numSymbolsToSend);
    openCmd->setFileIdToGet(fileID);
    openCmd->setIsLongFlow(isLongFlow);
    openCmd->setIsMulticastConn(isMulticastConn);
    openCmd->setMulticastGroupIndex(multicastGroupIndex);

    openCmd->setMultiSourcingGroupIndex(multiSourcingGroupIndex);
    openCmd->setIsMultiSourcingConn(isMultiSourcingConn);


    openCmd->setIsSender(true);
//    std::cout << " RQSocket::connect localAddr localAddr " << localAddr << "\n";

//  std::cout << " RQSocket::connect " << msg << "\n";
    msg->setControlInfo(openCmd);
    sendToRQ(msg);
    sockstate = CONNECTING;
}

void RQSocket::send(cMessage *msg) {
    if (sockstate != CONNECTED && sockstate != CONNECTING
            && sockstate != PEER_CLOSED)
        throw cRuntimeError(
                "RQSocket::send(): socket not connected or connecting, state is %s",
                stateName(sockstate));

    msg->setKind(RQ_C_SEND);
    RQSendCommand *cmd = new RQSendCommand();
//    std::cout << "connId ddd " << connId <<  " " << std::endl;

    cmd->setConnId(connId);
    msg->setControlInfo(cmd);
    sendToRQ(msg);
}

void RQSocket::sendCommand(cMessage *msg) {
    sendToRQ(msg);
}

void RQSocket::close() {
    std::cout << "RQSocket::close() " << std::endl;
    if (sockstate != CONNECTED && sockstate != PEER_CLOSED
            && sockstate != CONNECTING && sockstate != LISTENING)
        throw cRuntimeError(
                "RQSocket::close(): not connected or close() already called (sockstate=%s)",
                stateName(sockstate));

    cMessage *msg = new cMessage("CLOSE", RQ_C_CLOSE);
    RQCommand *cmd = new RQCommand();
    cmd->setConnId(connId);
    msg->setControlInfo(cmd);
    sendToRQ(msg);
    sockstate = (sockstate == CONNECTED) ? LOCALLY_CLOSED : CLOSED;
}

void RQSocket::abort() {
    if (sockstate != NOT_BOUND && sockstate != BOUND && sockstate != CLOSED
            && sockstate != SOCKERROR) {
        cMessage *msg = new cMessage("ABORT", RQ_C_ABORT);
        RQCommand *cmd = new RQCommand();
        cmd->setConnId(connId);
        msg->setControlInfo(cmd);
        sendToRQ(msg);
    }
    sockstate = CLOSED;
}

void RQSocket::requestStatus() {
    cMessage *msg = new cMessage("STATUS", RQ_C_STATUS);
    RQCommand *cmd = new RQCommand();
    cmd->setConnId(connId);
    msg->setControlInfo(cmd);
    sendToRQ(msg);
}

void RQSocket::renewSocket() {
    connId = getEnvir()->getUniqueNumber();
    remoteAddr = localAddr = L3Address();
    remotePrt = localPrt = -1;
    sockstate = NOT_BOUND;
}

bool RQSocket::belongsToSocket(cMessage *msg) {
    return dynamic_cast<RQCommand *>(msg->getControlInfo())
            && ((RQCommand *) (msg->getControlInfo()))->getConnId() == connId;
}

bool RQSocket::belongsToAnyRQSocket(cMessage *msg) {
    return dynamic_cast<RQCommand *>(msg->getControlInfo());
}

void RQSocket::setCallbackObject(CallbackInterface *callback,
        void *yourPointer) {
    cb = callback;
    yourPtr = yourPointer;
}

void RQSocket::processMessage(cMessage *msg) {

     ASSERT(belongsToSocket(msg));

    RQStatusInfo *status;
    RQConnectInfo *connectInfo;

    switch (msg->getKind()) {
    case RQ_I_DATA:
        if (cb)
            cb->socketDataArrived(connId, yourPtr, PK(msg), false); // see RaptorQBasicClientApp::socketDataArrived
        else
            delete msg;

        break;

    case RQ_I_URGENT_DATA:
        if (cb)
            cb->socketDataArrived(connId, yourPtr, PK(msg), true);
        else
            delete msg;

        break;

    case RQ_I_ESTABLISHED:
        // Note: this code is only for sockets doing active open, and nonforking
        // listening sockets. For a forking listening sockets, RQ_I_ESTABLISHED
        // carries a new connId which won't match the connId of this RQSocket,
        // so you won't get here. Rather, when you see RQ_I_ESTABLISHED, you'll
        // want to create a new RQSocket object via new RQSocket(msg).
        sockstate = CONNECTED;
        connectInfo = check_and_cast<RQConnectInfo *>(msg->getControlInfo());
        localAddr = connectInfo->getLocalAddr();
        remoteAddr = connectInfo->getRemoteAddr();
        localPrt = connectInfo->getLocalPort();
        remotePrt = connectInfo->getRemotePort();
        delete msg;

        if (cb)
            cb->socketEstablished(connId, yourPtr); // MOH: this calls  RaptorQBasicClientApp::socketEstablished(int connId, void *ptr) to send the first request

        break;

    case RQ_I_PEER_CLOSED:
        sockstate = PEER_CLOSED;

        delete msg;

        if (cb)
            cb->socketPeerClosed(connId, yourPtr);

        break;

    case RQ_I_CLOSED:
        std::cout << " socket closed \n";

        sockstate = CLOSED;
        delete msg;

        if (cb) {
            cb->socketClosed(connId, yourPtr);
        }

        break;

    case RQ_I_CONNECTION_REFUSED:
    case RQ_I_CONNECTION_RESET:
    case RQ_I_TIMED_OUT:
        sockstate = SOCKERROR;

        if (cb)
            cb->socketFailure(connId, yourPtr, msg->getKind());

        delete msg;
        break;

    case RQ_I_STATUS:
        status = check_and_cast<RQStatusInfo *>(msg->removeControlInfo());
        delete msg;

        if (cb)
            cb->socketStatusArrived(connId, yourPtr, status);

        break;

    default:
        throw cRuntimeError(
                "RQSocket: invalid msg kind %d, one of the RQ_I_xxx constants expected",
                msg->getKind());
    }
}

RQDataTransferMode RQSocket::convertStringToDataTransferMode(
        const char *transferMode) {
    if (nullptr == transferMode || 0 == transferMode[0])
        return RQ_TRANSFER_UNDEFINED;

    if (0 == strcmp(transferMode, "bytecount"))
        return RQ_TRANSFER_BYTECOUNT;

    if (0 == strcmp(transferMode, "object"))
        return RQ_TRANSFER_OBJECT;

    if (0 == strcmp(transferMode, "bytestream"))
        return RQ_TRANSFER_BYTESTREAM;

    if (0 == strcmp(transferMode, "rqrequests"))
        return RQ_REQUESTS;

    return RQ_TRANSFER_UNDEFINED;
}

void RQSocket::readDataTransferModePar(cComponent& component) {
    const char *transferMode = component.par("dataTransferMode");

    if (nullptr == transferMode)
        throw cRuntimeError("Missing dataTransferMode parameter at %s.",
                component.getFullPath().c_str());

    RQDataTransferMode x = convertStringToDataTransferMode(transferMode);

    if (x == RQ_TRANSFER_UNDEFINED)
        throw cRuntimeError("Invalid '%s' dataTransferMode parameter at %s.",
                transferMode, component.getFullPath().c_str());

    dataTransferMode = x;
}

} // namespace inet

