

#ifndef __INET_RAPTORQAPPBASE_H
#define __INET_RAPTORQAPPBASE_H

#include "inet/common/INETDefs.h"
#include "inet/raptorqimplementation/rqtransportlayer/contract/raptorq/RQSocket.h"

namespace inet {

/**
 * Base class for clients app for RQ-based request-reply protocols or apps.
 * Handles a single session (and RQ connection) at a time.
 *
 * It needs the following NED parameters: localAddress, localPort, connectAddress, connectPort.
 */
class INET_API RaptorQAppBase : public cSimpleModule, public RQSocket::CallbackInterface
{
  protected:
    RQSocket socket;

    // statistics
    int numSessions;
    int numBroken;
    int packetsSent;
    int packetsRcvd;
    int bytesSent;
    long bytesRcvd;




  protected:
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;

    /* Utility functions */
    virtual void connect();
    virtual void close();
    virtual void sendPacket(cPacket *pkt);
    virtual void setStatusString(const char *s);

    /* RQSocket::CallbackInterface callback methods */
    virtual void handleTimer(cMessage *msg) = 0;
    virtual void socketEstablished(int connId, void *yourPtr) override;
    virtual void socketDataArrived(int connId, void *yourPtr, cPacket *msg, bool urgent) override;
    virtual void socketPeerClosed(int connId, void *yourPtr) override;
    virtual void socketClosed(int connId, void *yourPtr) override;
    virtual void socketFailure(int connId, void *yourPtr, int code) override;
    virtual void socketStatusArrived(int connId, void *yourPtr, RQStatusInfo *status) override { delete status; }
};

} // namespace inet

#endif // ifndef __INET_RAPTORQAPPBASE_H

