
#include "inet/raptorqimplementation/application/raptorqapp/RaptorQEchoApp.h"
#include "inet/common/RawPacket.h"
#include "inet/raptorqimplementation/rqtransportlayer/contract/raptorq/RQCommand_m.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/lifecycle/NodeOperations.h"

namespace inet {

Define_Module(RaptorQEchoApp);

simsignal_t RaptorQEchoApp::rcvdPkSignal = registerSignal("rcvdPk");
simsignal_t RaptorQEchoApp::sentPkSignal = registerSignal("sentPk");

void RaptorQEchoApp::initialize(int stage)
{
    cSimpleModule::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        delay = par("echoDelay");
        echoFactor = par("echoFactor");

        bytesRcvd = bytesSent = 0;
        WATCH(bytesRcvd);
        WATCH(bytesSent);

        socket.setOutputGate(gate("raptorqOut"));
        socket.readDataTransferModePar(*this);

        nodeStatus = dynamic_cast<NodeStatus *>(findContainingNode(this)->getSubmodule("status"));
    }
    else if (stage == INITSTAGE_APPLICATION_LAYER) {
        if (isNodeUp())
            startListening();
    }
}

bool RaptorQEchoApp::isNodeUp()
{
    return !nodeStatus || nodeStatus->getState() == NodeStatus::UP;
}

void RaptorQEchoApp::startListening()
{
    const char *localAddress = par("localAddress");
    int localPort = par("localPort");
    socket.renewSocket();
    socket.bind(localAddress[0] ? L3Address(localAddress) : L3Address(), localPort);
    socket.listen();
}

void RaptorQEchoApp::stopListening()
{
    socket.close();
}

void RaptorQEchoApp::sendDown(cMessage *msg)
{
    if (msg->isPacket()) {
        bytesSent += ((cPacket *)msg)->getByteLength();
        emit(sentPkSignal, (cPacket *)msg);
    }

    send(msg, "raptorqOut");
}

void RaptorQEchoApp::handleMessage(cMessage *msg)
{
    if (!isNodeUp())
        throw cRuntimeError("Application is not running");
    if (msg->isSelfMessage()) {
        sendDown(msg);
    }
    else if (msg->getKind() == RQ_I_PEER_CLOSED) {
        // we'll close too
        msg->setName("close");
        msg->setKind(RQ_C_CLOSE);

        if (delay == 0)
            sendDown(msg);
        else
            scheduleAt(simTime() + delay, msg); // send after a delay
    }
    else if (msg->getKind() == RQ_I_DATA || msg->getKind() == RQ_I_URGENT_DATA) {
        cPacket *pkt = check_and_cast<cPacket *>(msg);
        emit(rcvdPkSignal, pkt);
        bytesRcvd += pkt->getByteLength();

        if (echoFactor == 0) {
            delete pkt;
        }
        else {
            // reverse direction, modify length, and send it back
            pkt->setKind(RQ_C_SEND);
            RQCommand *ind = check_and_cast<RQCommand *>(pkt->removeControlInfo());
            RQSendCommand *cmd = new RQSendCommand();
            cmd->setConnId(ind->getConnId());
            pkt->setControlInfo(cmd);
            delete ind;

            long byteLen = pkt->getByteLength() * echoFactor;

            if (byteLen < 1)
                byteLen = 1;

            pkt->setByteLength(byteLen);

            RawPacket *baMsg = dynamic_cast<RawPacket *>(pkt);

            // if (dataTransferMode == RQ_TRANSFER_BYTESTREAM)
            if (baMsg) {
                ByteArray& outdata = baMsg->getByteArray();
                ByteArray indata = outdata;
                outdata.setDataArraySize(byteLen);

                for (long i = 0; i < byteLen; i++)
                    outdata.setData(i, indata.getData(i / echoFactor));
            }

            if (delay == 0)
                sendDown(pkt);
            else
                scheduleAt(simTime() + delay, pkt); // send after a delay
        }
    }
    else {
        // some indication -- ignore
        delete msg;
    }

    if (hasGUI()) {
        char buf[80];
        sprintf(buf, "rcvd: %ld bytes\nsent: %ld bytes", bytesRcvd, bytesSent);
        getDisplayString().setTagArg("t", 0, buf);
    }
}

bool RaptorQEchoApp::handleOperationStage(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback)
{
    Enter_Method_Silent();
    if (dynamic_cast<NodeStartOperation *>(operation)) {
        if ((NodeStartOperation::Stage)stage == NodeStartOperation::STAGE_APPLICATION_LAYER)
            startListening();
    }
    else if (dynamic_cast<NodeShutdownOperation *>(operation)) {
        if ((NodeShutdownOperation::Stage)stage == NodeShutdownOperation::STAGE_APPLICATION_LAYER)
            // TODO: wait until socket is closed
            stopListening();
    }
    else if (dynamic_cast<NodeCrashOperation *>(operation))
        ;
    else
        throw cRuntimeError("Unsupported lifecycle operation '%s'", operation->getClassName());
    return true;
}

void RaptorQEchoApp::finish()
{
    recordScalar("bytesRcvd", bytesRcvd);
    recordScalar("bytesSent", bytesSent);
}

} // namespace inet

