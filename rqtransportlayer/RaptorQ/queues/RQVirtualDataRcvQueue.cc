
#include "RQVirtualDataRcvQueue.h"

namespace inet {

namespace raptorq {

Register_Class(RQVirtualDataRcvQueue);

bool RQVirtualDataRcvQueue::Region::merge(const RQVirtualDataRcvQueue::Region *other)
{
    if (seqLess(end, other->begin) || seqLess(other->end, begin))
        return false;
    if (seqLess(other->begin, begin))
        begin = other->begin;
    if (seqLess(end, other->end))
        end = other->end;
    return true;
}

RQVirtualDataRcvQueue::Region *RQVirtualDataRcvQueue::Region::split(uint32 seq)
{
    ASSERT(seqGreater(seq, begin) && seqLess(seq, end));

    Region *reg = new Region(begin, seq);
    begin = seq;
    return reg;
}

RQVirtualDataRcvQueue::Region::CompareStatus RQVirtualDataRcvQueue::Region::compare(const RQVirtualDataRcvQueue::Region& other) const
{
    if (end == other.begin)
        return BEFORE_TOUCH;
    if (begin == other.end)
        return AFTER_TOUCH;
    if (seqLess(end, other.begin))
        return BEFORE;
    if (seqLess(other.end, begin))
        return AFTER;
    return OVERLAP;
}

void RQVirtualDataRcvQueue::Region::copyTo(cPacket *msg) const
{
    msg->setByteLength(getLength());
}

ulong RQVirtualDataRcvQueue::Region::getLengthTo(uint32 seq) const
{
    // seq below 1st region
    if (seqLE(seq, begin))
        return 0;

    if (seqLess(seq, end)) // part of 1st region
        return (ulong)(seq - begin);

    return (ulong)(end - begin);
}

////////////////////////////////////////////////////////////////////

RQVirtualDataRcvQueue::RQVirtualDataRcvQueue() :
    RQReceiveQueue(),
    rcv_nxt(-1)
{
}

RQVirtualDataRcvQueue::~RQVirtualDataRcvQueue()
{
    while (!regionList.empty()) {
        delete regionList.front();
        regionList.pop_front();
    }
}

void RQVirtualDataRcvQueue::init(uint32 startSeq)
{
    rcv_nxt = startSeq;

    while (!regionList.empty()) {
        delete regionList.front();
        regionList.pop_front();
    }
}

std::string RQVirtualDataRcvQueue::info() const
{
    std::string res;
    char buf[32];
    sprintf(buf, "rcv_nxt=%u", rcv_nxt);
    res = buf;

    for (const auto & elem : regionList) {
        sprintf(buf, " [%u..%u)", (elem)->getBegin(), (elem)->getEnd());
        res += buf;
    }
    return res;
}

RQVirtualDataRcvQueue::Region *RQVirtualDataRcvQueue::createRegionFromSegment(RQSegment *rqseg)
{
    Region *region = new Region(rqseg->getSequenceNo(), rqseg->getSequenceNo() + rqseg->getPayloadLength());
    return region;
}

uint32 RQVirtualDataRcvQueue::insertBytesFromSegment(RQSegment *rqseg)
{
    Region *region = createRegionFromSegment(rqseg);

#ifndef NDEBUG
    if (!regionList.empty()) {
        uint32 ob = regionList.front()->getBegin();
        uint32 oe = regionList.back()->getEnd();
        uint32 nb = region->getBegin();
        uint32 ne = region->getEnd();
//        std::cout << "ob= " << ob << "  oe= " << oe <<  " nb= " << nb << "  ne= " << ne << "\n";
        uint32 minb = seqMin(ob, nb);
        uint32 maxe = seqMax(oe, ne);
        if (seqGE(minb, oe) || seqGE(minb, ne) || seqGE(ob, maxe) || seqGE(nb, maxe))
            throw cRuntimeError("The new segment is [%u, %u) out of the acceptable range at the queue %s",
                    region->getBegin(), region->getEnd(), info().c_str());
    }
#endif // ifndef NDEBUG

    merge(region);

    if (seqGE(rcv_nxt, regionList.front()->getBegin()))
        rcv_nxt = regionList.front()->getEnd();

    return rcv_nxt;
}

void RQVirtualDataRcvQueue::merge(RQVirtualDataRcvQueue::Region *seg)
{
    // Here we have to update our existing regions with the octet range
    // rqseg represents. We either have to insert rqseg as a separate region
    // somewhere, or (if it overlaps with an existing region) extend
    // existing regions; we also may have to merge existing regions if
    // they become overlapping (or touching) after adding rqseg.

    RegionList::reverse_iterator i = regionList.rbegin();
    Region::CompareStatus cmp;

    while (i != regionList.rend() && Region::BEFORE != (cmp = (*i)->compare(*seg))) {
        if (cmp != Region::AFTER) {
            if (seg->merge(*i)) {
                delete *i;
                i = (RegionList::reverse_iterator)(regionList.erase((++i).base()));
                continue;
            }
            else
                throw cRuntimeError("Model error: merge of region [%u,%u) with [%u,%u) unsuccessful", (*i)->getBegin(), (*i)->getEnd(), seg->getBegin(), seg->getEnd());
        }
        ++i;
    }

    regionList.insert(i.base(), seg);
}

cPacket *RQVirtualDataRcvQueue::extractBytesUpTo(uint32 seq)
{
    cPacket *msg = nullptr;
    Region *reg = extractTo(seq);
    std::cout << " 444444444444444444444 \n" ;

    if (reg) {
        msg = new cPacket("data");
        reg->copyTo(msg);
        std::cout << " 55555555555555555555 \n" ;

        delete reg;
    }
    return msg;
}

RQVirtualDataRcvQueue::Region *RQVirtualDataRcvQueue::extractTo(uint32 seq)
{
//    std::cout<< " hhhhhhhhhhhhhhhhhhhh seq " << seq << std::endl;
//    std::cout<< " hhhhhhhhhhhhhhhhhhhh rcv_nxt " << rcv_nxt << std::endl;

    // ASSERT(seqLE(seq, rcv_nxt));

    if (regionList.empty()){
//        std::cout << " 2222222222222222222222222 \n" ;
        return nullptr;
    }
    Region *reg = regionList.front();
    uint32 beg = reg->getBegin();

    if (seqLE(seq, beg))
    {
//        std::cout << " seqseqseq  " << seq << std::endl ;
//        std::cout << " begbegbeg  " << beg << std::endl ;
//        std::cout << " 33333333333333333333333333 \n" ;
        return nullptr;
    }

    if (seqGE(seq, reg->getEnd())) {
        regionList.pop_front();
//        std::cout << " seqseqseq  " << seq << std::endl ;
//        std::cout << "  reg->getEnd()  " <<  reg->getEnd()  << std::endl ;
//        std::cout << " 1111111111111111111111111111111 \n" ;
        return reg;
    }
    return reg->split(seq);
}

uint32 RQVirtualDataRcvQueue::getAmountOfBufferedBytes()
{
    uint32 bytes = 0;

    for (auto & elem : regionList)
        bytes += (elem)->getLength();

    return bytes;
}

uint32 RQVirtualDataRcvQueue::getAmountOfFreeBytes(uint32 maxRcvBuffer)
{
    uint32 usedRcvBuffer = getAmountOfBufferedBytes();
    uint32 freeRcvBuffer = maxRcvBuffer - usedRcvBuffer;
    return (maxRcvBuffer > usedRcvBuffer) ? freeRcvBuffer : 0;
}

uint32 RQVirtualDataRcvQueue::getQueueLength()
{
    return regionList.size();
}

void RQVirtualDataRcvQueue::getQueueStatus()
{
    EV_DEBUG << "receiveQLength=" << regionList.size() << " " << info() << "\n";
}

uint32 RQVirtualDataRcvQueue::getLE(uint32 fromSeqNum)
{
    auto i = regionList.begin();

    while (i != regionList.end()) {
        if (seqLE((*i)->getBegin(), fromSeqNum) && seqLess(fromSeqNum, (*i)->getEnd())) {
//            rqEV << "Enqueued region: [" << i->begin << ".." << i->end << ")\n";
            return (*i)->getBegin();
        }

        i++;
    }

    return fromSeqNum;
}

uint32 RQVirtualDataRcvQueue::getRE(uint32 toSeqNum)
{
    auto i = regionList.begin();

    while (i != regionList.end()) {
        if (seqLess((*i)->getBegin(), toSeqNum) && seqLE(toSeqNum, (*i)->getEnd())) {
//            rqEV << "Enqueued region: [" << i->begin << ".." << i->end << ")\n";
            return (*i)->getEnd();
        }

        i++;
    }

    return toSeqNum;
}

uint32 RQVirtualDataRcvQueue::getFirstSeqNo()
{
    if (regionList.empty())
        return rcv_nxt;
    return seqMin(regionList.front()->getBegin(), rcv_nxt);
}

} // namespace raptorq

} // namespace inet

