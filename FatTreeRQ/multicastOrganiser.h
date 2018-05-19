#ifndef __INET_RQ_H
#define __INET_RQ_H

#include <map>
#include <set>
#include <algorithm>    // std::random_shuffle
#include <vector>       // std::vector
#include <random>
#include <cmath>
#include <time.h>       /* time */

#include "inet/common/INETDefs.h"

#include "inet/common/lifecycle/ILifecycle.h"
#include "inet/applications/tcpapp/TCPSessionApp.h"
#include "../application/raptorqapp/RaptorQBasicClientApp.h"
#include "../application/raptorqapp/RQSinkApp.h"
#include "../rqtransportlayer/RaptorQ/RaptorQ.h"

namespace inet {
class multicastOrganiser : public cSimpleModule
{


public:
    multicastOrganiser()
    {
    }
    virtual ~multicastOrganiser();
protected:
    virtual void initialize(int stage) override;




};

} // namespace inet

#endif // ifndef __INET_RQ_H


