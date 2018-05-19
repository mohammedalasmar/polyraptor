
#ifndef __INET_RAPTORQECHOAPP_H
#define __INET_RAPTORQECHOAPP_H

#include "inet/common/INETDefs.h"
#include "inet/common/INETMath.h"
#include "inet/common/lifecycle/ILifecycle.h"
#include "inet/common/lifecycle/NodeStatus.h"
#include "inet/raptorqimplementation/rqtransportlayer/contract/raptorq/RQSocket.h"

namespace inet {

/**
 * Accepts any number of incoming connections, and sends back whatever
 * arrives on them.
 */
class INET_API RaptorQEchoApp : public cSimpleModule, public ILifecycle
{
  protected:
    simtime_t delay;
    double echoFactor = NaN;

    RQSocket socket;
    NodeStatus *nodeStatus = nullptr;

    long bytesRcvd = 0;
    long bytesSent = 0;

    static simsignal_t rcvdPkSignal;
    static simsignal_t sentPkSignal;

  protected:
    virtual bool isNodeUp();
    virtual void sendDown(cMessage *msg);
    virtual void startListening();
    virtual void stopListening();

    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
    virtual bool handleOperationStage(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback) override;

  public:
    RaptorQEchoApp() {}
};

} // namespace inet

#endif // ifndef __INET_RAPTORQECHOAPP_H

