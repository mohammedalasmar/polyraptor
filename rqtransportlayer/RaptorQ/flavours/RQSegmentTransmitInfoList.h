
#ifndef __INET_RQSEGMENTTRANSMITINFOLIST_H
#define __INET_RQSEGMENTTRANSMITINFOLIST_H

#include "inet/common/INETDefs.h"

#include "RQBaseAlg.h"

namespace inet {

namespace raptorq {

class INET_API RQSegmentTransmitInfoList
{
  public:
    class Item
    {
      protected:
        uint32_t beg;    // segment [begin, end)
        uint32_t end;
        simtime_t firstSentTime;    // time of first sending
        simtime_t lastSentTime;    // time of last sending
        int transmitCount;    // num of transmissions

      public:
        Item(uint32_t beg, uint32_t end, simtime_t firstTime, simtime_t lastTime, int transmits) : beg(beg), end(end), firstSentTime(firstTime), lastSentTime(lastTime), transmitCount(transmits) {}
        uint32_t getBeg() const { return beg; }
        uint32_t getEnd() const { return end; }
        simtime_t getFirstSentTime() const { return firstSentTime; }
        simtime_t getLastSentTime() const { return lastSentTime; }
        int getTransmitCount() const { return transmitCount; }

        friend class RQSegmentTransmitInfoList;
    };
    typedef std::list<Item> RQSegmentTransmitInfoItems;
    RQSegmentTransmitInfoItems regions;    // region[i].end == region[i+1].beg

  public:
    void set(uint32_t beg, uint32_t end, simtime_t sentTime);    // [beg,end)

    /// returns pointer to Item, or nullptr if not found
    const Item *get(uint32_t seq) const;

    void clearTo(uint32_t endseg);
};

} // namespace RQ

} // namespace inet

#endif // ifndef __INET_RQSEGMENTTRANSMITINFOLIST_H

