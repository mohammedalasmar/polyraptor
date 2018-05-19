#ifndef __INET_RQRequestsSendQueue_H
#define __INET_RQRequestsSendQueue_H

#include "../RQSendQueue.h"

namespace inet {

namespace raptorq {

/**
 * Send queue that manages "virtual bytes", that is, byte counts only.
 * It uses the length() field of the messages only, everything else is
 * ignored.
 *
 * @see RQVirtualDataRcvQueue
 */
class INET_API RQRequestsSendQueue: public RQSendQueue {

protected:
    struct Payload {
        unsigned int endSequenceNo;
        cPacket *msg;
    };
    typedef std::list<Payload> PayloadQueue;
    PayloadQueue payloadQueue;

    uint32 begin;    // 1st sequence number stored
    uint32 end;    // last sequence number stored +1

public:
    /**
     * Ctor
     */
    RQRequestsSendQueue();

    /**
     * Virtual dtor.
     */
    virtual ~RQRequestsSendQueue();

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
    virtual RQSegment *createSegmentWithBytes(uint32 fromSeq, ulong numBytes)
            override;

    /**
     *
     */
    virtual void discardUpTo(uint32 seqNum) override;
};

} // namespace raptorq

} // namespace inet

#endif // ifndef __INET_RQRequestsSendQueue_H

