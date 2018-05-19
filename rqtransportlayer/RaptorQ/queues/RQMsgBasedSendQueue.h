
#ifndef __INET_RQMSGBASEDSENDQUEUE_H
#define __INET_RQMSGBASEDSENDQUEUE_H

#include <list>
#include "../RQSendQueue.h"

namespace inet {

namespace raptorq {

/**
 * Send queue that manages messages.
 *
 * @see RQMsgBasedRcvQueue
 */
class INET_API RQMsgBasedSendQueue : public RQSendQueue
{
  protected:
    struct Payload
    {
        unsigned int endSequenceNo;
        cPacket *msg;
    };
    typedef std::list<Payload> PayloadQueue;
    PayloadQueue payloadQueue;

    uint32 begin;    // 1st sequence number stored
    uint32 end;    // last sequence number stored + 1

  public:
    /**
     * Ctor
     */
    RQMsgBasedSendQueue();

    /**
     * Virtual dtor.
     */
    virtual ~RQMsgBasedSendQueue();

    /**
     *
     */
    virtual void init(uint32 startSeq) override;

    /**
     * Returns a string with the region stored.
     */
    virtual std::string info() const override;

    /**
     *
     */
    virtual void enqueueAppData(cPacket *msg) override;

    /**
     *
     */
    virtual uint32 getBufferStartSeq() override;

    /**
     *
     */
    virtual uint32 getBufferEndSeq() override;

    /**
     *
     */
    virtual RQSegment *createSegmentWithBytes(uint32 fromSeq, ulong numBytes) override;

    /**
     *
     */
    virtual void discardUpTo(uint32 seqNum) override;
};

} // namespace raptorq

} // namespace inet

#endif // ifndef __INET_RQMSGBASEDSENDQUEUE_H

