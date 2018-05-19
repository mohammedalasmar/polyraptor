
#ifndef __INET_RQSRVHOSTAPP_H
#define __INET_RQSRVHOSTAPP_H

#include "inet/common/INETDefs.h"
#include "inet/raptorqimplementation/rqtransportlayer/contract/raptorq/RQSocket.h"
#include "inet/raptorqimplementation/rqtransportlayer/contract/raptorq/RQSocketMap.h"
#include "inet/common/lifecycle/ILifecycle.h"
#include "inet/common/lifecycle/LifecycleOperation.h"

namespace inet {

//forward declaration:
class RaptorQServerThreadBase;

/**
 * Hosts a server application, to be subclassed from RQServerProcess (which
 * is a sSimpleModule). Creates one instance (using dynamic module creation)
 * for each incoming connection. More info in the corresponding NED file.
 */
class INET_API RaptorQSrvHostApp : public cSimpleModule, public ILifecycle
{
  protected:
    RQSocket serverSocket;
    RQSocketMap socketMap;

    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
    virtual void updateDisplay();

    virtual bool handleOperationStage(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback) override
    { Enter_Method_Silent(); throw cRuntimeError("Unsupported lifecycle operation '%s'", operation->getClassName()); return true; }

  public:
    virtual void removeThread(RaptorQServerThreadBase *thread);
};

/**
 * Abstract base class for server processes to be used with RQSrvHostApp.
 * Subclasses need to be registered using the Register_Class() macro.
 *
 * @see RQSrvHostApp
 */
class INET_API RaptorQServerThreadBase : public cObject, public RQSocket::CallbackInterface
{
  protected:
    RaptorQSrvHostApp *hostmod;
    RQSocket *sock;    // ptr into socketMap managed by RQSrvHostApp

    // internal: RQSocket::CallbackInterface methods
    virtual void socketDataArrived(int, void *, cPacket *msg, bool urgent) override { dataArrived(msg, urgent); }
    virtual void socketEstablished(int, void *) override { established(); }
    virtual void socketPeerClosed(int, void *) override { peerClosed(); }
    virtual void socketClosed(int, void *) override { closed(); }
    virtual void socketFailure(int, void *, int code) override { failure(code); }
    virtual void socketStatusArrived(int, void *, RQStatusInfo *status) override { statusArrived(status); }

  public:

    RaptorQServerThreadBase() { sock = nullptr; hostmod = nullptr; }
    virtual ~RaptorQServerThreadBase() {}

    // internal: called by RQSrvHostApp after creating this module
    virtual void init(RaptorQSrvHostApp *hostmodule, RQSocket *socket) { hostmod = hostmodule; sock = socket; }

    /*
     * Returns the socket object
     */
    virtual RQSocket *getSocket() { return sock; }

    /*
     * Returns pointer to the host module
     */
    virtual RaptorQSrvHostApp *getHostModule() { return hostmod; }

    /**
     * Schedule an event. Do not use getContextPointer() of cMessage, because
     * RQServerThreadBase uses it for its own purposes.
     */
    virtual void scheduleAt(simtime_t t, cMessage *msg) { msg->setContextPointer(this); hostmod->scheduleAt(t, msg); }

    /*
     *  Cancel an event
     */
    virtual void cancelEvent(cMessage *msg) { hostmod->cancelEvent(msg); }

    /**
     * Called when connection is established. To be redefined.
     */
    virtual void established() = 0;

    /*
     * Called when a data packet arrives. To be redefined.
     */
    virtual void dataArrived(cMessage *msg, bool urgent) = 0;

    /*
     * Called when a timer (scheduled via scheduleAt()) expires. To be redefined.
     */
    virtual void timerExpired(cMessage *timer) = 0;

    /*
     * Called when the client closes the connection. By default it closes
     * our side too, but it can be redefined to do something different.
     */
    virtual void peerClosed() { getSocket()->close(); }

    /*
     * Called when the connection closes (successful RQ teardown). By default
     * it deletes this thread, but it can be redefined to do something different.
     */
    virtual void closed() { hostmod->removeThread(this); }

    /*
     * Called when the connection breaks (RQ error). By default it deletes
     * this thread, but it can be redefined to do something different.
     */
    virtual void failure(int code) { hostmod->removeThread(this); }

    /*
     * Called when a status arrives in response to getSocket()->getStatus().
     * By default it deletes the status object, redefine it to add code
     * to examine the status.
     */
    virtual void statusArrived(RQStatusInfo *status) { delete status; }
};

} // namespace inet

#endif // ifndef __INET_RQSRVHOSTAPP_H

