
#include "inet/common/INETDefs.h"

#include "RQByteStreamRcvQueue.h"

#include "inet/common/RawPacket.h"
#include "../../contract/raptorq/RQCommand_m.h"
#include "../raptorq_common/RQSegment.h"

namespace inet {

namespace raptorq {

Register_Class(RQByteStreamRcvQueue);

bool RQByteStreamRcvQueue::Region::merge(const RQVirtualDataRcvQueue::Region *_other)
{
    const Region *other = dynamic_cast<const Region *>(_other);

    if (!other)
        throw cRuntimeError("merge(): cannot cast (RQVirtualDataRcvQueue::Region *) to type 'RQDataStreamRcvQueue::Region *'");

    if (seqLess(end, other->begin) || seqLess(other->end, begin))
        return false;

    uint32 nbegin = seqMin(begin, other->begin);
    uint32 nend = seqMax(end, other->end);

    if (nbegin != begin || nend != end) {
        char *buff = new char[nend - nbegin];

        if (nbegin != begin)
            other->data.copyDataToBuffer(buff, begin - nbegin);

        data.copyDataToBuffer(buff + begin - nbegin, end - begin);

        if (nend != end)
            other->data.copyDataToBuffer(buff + end - nbegin, nend - end);

        begin = nbegin;
        end = nend;
        data.assignBuffer(buff, end - begin);
    }

    return true;
}

RQByteStreamRcvQueue::Region *RQByteStreamRcvQueue::Region::split(uint32 seq)
{
    ASSERT(seqGreater(seq, begin) && seqLess(seq, end));

    Region *reg = new Region(begin, seq);
    reg->data.setDataFromByteArray(data, 0, seq - begin);
    data.truncateData(seq - begin, 0);
    begin = seq;
    return reg;
}

void RQByteStreamRcvQueue::Region::copyTo(cPacket *msg_) const
{
    ASSERT(getLength() == data.getDataArraySize());

    RawPacket *msg = check_and_cast<RawPacket *>(msg_);
    RQVirtualDataRcvQueue::Region::copyTo(msg);
    msg->setByteArray(data);
}

////////////////////////////////////////////////////////////////////

RQByteStreamRcvQueue::~RQByteStreamRcvQueue()
{
}

std::string RQByteStreamRcvQueue::info() const
{
    std::stringstream os;

    os << "rcv_nxt=" << rcv_nxt;

    for (const auto & elem : regionList) {
        os << " [" << (elem)->getBegin() << ".." << (elem)->getEnd() << ")";
    }

    os << " " << regionList.size() << "msgs";

    return os.str();
}

cPacket *RQByteStreamRcvQueue::extractBytesUpTo(uint32 seq)
{
    cPacket *msg = nullptr;
    RQVirtualDataRcvQueue::Region *reg = extractTo(seq);
    if (reg) {
        msg = new RawPacket("data");
        reg->copyTo(msg);
        delete reg;
    }
    return msg;
}

RQVirtualDataRcvQueue::Region *RQByteStreamRcvQueue::createRegionFromSegment(RQSegment *rqseg)
{
    ASSERT(rqseg->getPayloadLength() == rqseg->getByteArray().getDataArraySize());

    Region *region = new Region(rqseg->getSequenceNo(),
                rqseg->getSequenceNo() + rqseg->getPayloadLength(), rqseg->getByteArray());

    return region;
}

} // namespace rq

} // namespace inet

