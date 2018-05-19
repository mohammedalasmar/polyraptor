

#include "inet/raptorqimplementation/application/raptorqapp/RaptorQAppBase.h"

#include "inet/networklayer/common/L3AddressResolver.h"

namespace inet {

void RaptorQAppBase::initialize(int stage) {
    cSimpleModule::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        numSessions = numBroken = packetsSent = packetsRcvd = bytesSent =
                bytesRcvd = 0;
    } else if (stage == INITSTAGE_APPLICATION_LAYER) {
        // parameters
        const char *localAddress = par("localAddress");
        int localPort = par("localPort");
        socket.readDataTransferModePar(*this);
        socket.bind( *localAddress ? L3AddressResolver().resolve(localAddress) : L3Address(), localPort);
        socket.setCallbackObject(this);
        socket.setOutputGate(gate("raptorqOut"));
        setStatusString("waiting");
    }
}

void RaptorQAppBase::handleMessage(cMessage *msg) {
    if (msg->isSelfMessage()) {
        handleTimer(msg);
    }
    else
        socket.processMessage(msg);
}

void RaptorQAppBase::connect() {
    // we need a new connId if this is not the first connection
    socket.renewSocket();
    int fileId = par("fileId");  // MOH added
    unsigned int numSymbolsToSend = par("numSymbolsToSend").longValue();
    bool isLongFlow = par("isLongFlow").boolValue();


    bool isMulticastConn =  par("isMulticastConn").boolValue();
    bool isMultiSourcingConn =  par("isMultiSourcingConn").boolValue();

    unsigned int  multicastGroupIndex =  par("multicastGroupIndex").longValue();
    unsigned int  multiSourcingGroupIndex =  par("multicastGroupIndex").longValue();
    // connect
    const char *connectAddress = par("connectAddress");
    int connectPort = par("connectPort");

    L3Address destination;
    L3AddressResolver().tryResolve(connectAddress, destination);
    if (destination.isUnspecified()) {
        EV_ERROR << "Connecting to " << connectAddress << " port=" << connectPort  << ": cannot resolve destination address\n";
//        std::cout << "RaptorQAppBase::connect() Connecting to " << connectAddress << " port=" << connectPort  << ": cannot resolve destination address\n";

    } else {
        EV_INFO << "Connecting to " << connectAddress << "(" << destination << ") port=" << connectPort << endl;

//        std::string dd= "10.0.0.86";
//        L3Address destination2 =L3Address(dd.c_str());
        setStatusString("connecting");
        socket.connect(destination  , connectPort, fileId, numSymbolsToSend, isLongFlow, isMulticastConn ,   multicastGroupIndex , isMultiSourcingConn, multiSourcingGroupIndex );
        EV_INFO << "Connecting to mmmmm" << connectAddress << "(" << destination  << ") port=" << connectPort << endl;

        numSessions++;
    }
}

void RaptorQAppBase::close() {
    setStatusString("closing");
    EV_INFO << "issuing CLOSE command\n";
    socket.close();
}

void RaptorQAppBase::sendPacket(cPacket *msg) {
    int numBytes = msg->getByteLength();
    socket.send(msg);

    packetsSent++;
    bytesSent += numBytes;
}

void RaptorQAppBase::setStatusString(const char *s) {
    if (hasGUI())
        getDisplayString().setTagArg("t", 0, s);
}

void RaptorQAppBase::socketEstablished(int, void *) {
    // *redefine* to perform or schedule first sending
    EV_INFO << "connected\n";
    setStatusString("connected");
}

void RaptorQAppBase::socketDataArrived(int, void *, cPacket *msg, bool) {
    // *redefine* to perform or schedule next sending
    packetsRcvd++;
    bytesRcvd += msg->getByteLength();
    delete msg;
}

void RaptorQAppBase::socketPeerClosed(int, void *) {
    // close the connection (if not already closed)
    if (socket.getState() == RQSocket::PEER_CLOSED) {
        EV_INFO << "remote RQ closed, closing here as well\n";
        close();
    }
}

void RaptorQAppBase::socketClosed(int, void *) {
    // *redefine* to start another session etc.
    EV_INFO << "connection closed\n";
    setStatusString("closed");
}

void RaptorQAppBase::socketFailure(int, void *, int code) {
    // subclasses may override this function, and add code try to reconnect after a delay.
    EV_WARN << "connection broken\n";
    setStatusString("broken");

    numBroken++;
}

void RaptorQAppBase::finish() {
    std::string modulePath = getFullPath();

    EV_INFO << modulePath << ": opened " << numSessions << " sessions\n";
    EV_INFO << modulePath << ": sent " << bytesSent << " bytes in "
                   << packetsSent << " packets\n";
    EV_INFO << modulePath << ": received " << bytesRcvd << " bytes in "
                   << packetsRcvd << " packets\n";

//    std::cout << modulePath << ": opened " << numSessions << " sessions\n";
//    std::cout << modulePath << ": sent " << bytesSent << " bytes in " << packetsSent << " packets\n";
//    std::cout << modulePath << ": received " << bytesRcvd << " bytes in " << packetsRcvd << " packets\n";


}

} // namespace inet

