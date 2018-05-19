
#ifndef __INET_RQSymbolsRcvQueue_H
#define __INET_RQSymbolsRcvQueue_H

#include <map>
#include <string>

#include "inet/common/INETDefs.h"

#include "../raptorq_common/RQSegment.h"
#include "RQVirtualDataRcvQueue.h"

namespace inet {

namespace raptorq {

/**
 * @see RQMsgBasedSendQueue
 */
class INET_API RQSymbolsRcvQueue : public RQVirtualDataRcvQueue
{
  protected:
    struct PayloadItem
    {
        uint32 seqNo;
        cPacket *packet;
        PayloadItem(uint32 _seqNo, cPacket *_packet) : seqNo(_seqNo), packet(_packet) {}
    };
    typedef std::list<PayloadItem> PayloadList;
    PayloadList payloadList;    // sorted list, used the sequence number comparators

  public:
    /**
     * Ctor.
     */
    RQSymbolsRcvQueue();

    /**
     * Virtual dtor.
     */
    virtual ~RQSymbolsRcvQueue();

    /**
     * Set initial receive sequence number.
     */
    virtual void init(uint32 startSeq) override;

    /**
     * Returns a string with region stored.
     */
    virtual std::string info() const override;

    /**
     * Called when a RQ segment arrives. Returns sequence number for ACK.
     */
    virtual uint32 insertBytesFromSegment(RQSegment *rqseg) override;

    /**
     *
     */
    virtual cPacket *extractBytesUpTo(uint32 seq) override;
};

} // namespace raptorq

} // namespace inet

#endif // ifndef __INET_RQSymbolsRcvQueue_H

