
#ifndef __INET_RQVIRTUALDATARCVQUEUE_H
#define __INET_RQVIRTUALDATARCVQUEUE_H

#include <list>
#include <string>

#include "../raptorq_common/RQSegment.h"
#include "../RQReceiveQueue.h"

namespace inet {

namespace raptorq {

/**
 * Receive queue that manages "virtual bytes", that is, byte counts only.
 *
 * @see RQVirtualDataSendQueue
 */
class INET_API RQVirtualDataRcvQueue : public RQReceiveQueue
{
  protected:
    uint32 rcv_nxt = 0;

    class Region
    {
      protected:
        uint32 begin;
        uint32 end;

      public:
        enum CompareStatus { BEFORE = 1, BEFORE_TOUCH, OVERLAP, AFTER_TOUCH, AFTER };
        Region(uint32 _begin, uint32 _end) : begin(_begin), end(_end) {};
        virtual ~Region() {};
        uint32 getBegin() const { return begin; }
        uint32 getEnd() const { return end; }
        unsigned long getLength() const { return (ulong)(end - begin); }
        unsigned long getLengthTo(uint32 seq) const;

        /** Compare self and other */
        CompareStatus compare(const RQVirtualDataRcvQueue::Region& other) const;

        // Virtual functions:

        /** Merge other region to self */
        virtual bool merge(const RQVirtualDataRcvQueue::Region *other);

        /** Copy self to msg */
        virtual void copyTo(cPacket *msg) const;

        /**
         * Returns an allocated new Region object with filled with [begin..seq) and set self to [seq..end)
         */
        virtual RQVirtualDataRcvQueue::Region *split(uint32 seq);
    };

    typedef std::list<Region *> RegionList;

    RegionList regionList;

    /** Merge segment byte range into regionList, the parameter region must created by 'new' operator. */
    void merge(RQVirtualDataRcvQueue::Region *region);

    // Returns number of bytes extracted
    RQVirtualDataRcvQueue::Region *extractTo(uint32 toSeq);

    /**
     * Create a new Region from rqseg.
     * Called from insertBytesFromSegment()
     */
    virtual RQVirtualDataRcvQueue::Region *createRegionFromSegment(RQSegment *rqseg);

  public:
    /**
     * Ctor.
     */
    RQVirtualDataRcvQueue();

    /**
     * Virtual dtor.
     */
    virtual ~RQVirtualDataRcvQueue();

    /** Method inherited from RQReceiveQueue */
    virtual void init(uint32 startSeq) override;

    /** Method inherited from RQReceiveQueue */
    virtual std::string info() const override;

    /** Method inherited from RQReceiveQueue */
    virtual uint32 insertBytesFromSegment(RQSegment *rqseg) override;

    /** Method inherited from RQReceiveQueue */
    virtual cPacket *extractBytesUpTo(uint32 seq) override;

    /** Method inherited from RQReceiveQueue */
    virtual uint32 getAmountOfBufferedBytes() override;

    /** Method inherited from RQReceiveQueue */
    virtual uint32 getAmountOfFreeBytes(uint32 maxRcvBuffer) override;

    /** Method inherited from RQReceiveQueue */
    virtual uint32 getQueueLength() override;

    /** Method inherited from RQReceiveQueue */
    virtual void getQueueStatus() override;

    /** Method inherited from RQReceiveQueue */
    virtual uint32 getLE(uint32 fromSeqNum) override;

    /** Method inherited from RQReceiveQueue */
    virtual uint32 getRE(uint32 toSeqNum) override;

    virtual uint32 getFirstSeqNo() override;
};

} // namespace raptorq

} // namespace inet

#endif // ifndef __INET_RQVIRTUALDATARCVQUEUE_H

