
#ifndef __INET_RAPTORQGENERICSRVTHREAD_H
#define __INET_RAPTORQGENERICSRVTHREAD_H

#include "inet/common/INETDefs.h"

#include "inet/raptorqimplementation/application/raptorqapp/RaptorQSrvHostApp.h"

namespace inet {

/**
 * Example server thread, to be used with RQSrvHostApp.
 */
class INET_API RaptorQGenericSrvThread : public RaptorQServerThreadBase
{
protected:
//     int ESI = 0;
//     int SBN = 0;

  public:
    RaptorQGenericSrvThread() {}

    virtual void established() override;
    virtual void dataArrived(cMessage *msg, bool urgent) override;
    virtual void timerExpired(cMessage *timer) override;
};

} // namespace inet

#endif // ifndef __INET_RAPTORQGENERICSRVTHREAD_H

