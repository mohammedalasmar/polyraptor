
#include "RQVirtualDataSendQueue.h"

namespace inet {

namespace raptorq {

Register_Class(RQVirtualDataSendQueue);

RQVirtualDataSendQueue::RQVirtualDataSendQueue() : RQSendQueue()
{
    begin = end = 0;
}

RQVirtualDataSendQueue::~RQVirtualDataSendQueue()
{
}

void RQVirtualDataSendQueue::init(uint32 startSeq)
{
    begin = startSeq;
    end = startSeq;
}

std::string RQVirtualDataSendQueue::info() const
{
    std::stringstream out;
    out << "[" << begin << ".." << end << ")";
    return out.str();
}

void RQVirtualDataSendQueue::enqueueAppData(cPacket *msg)
{
    //rqEV << "sendQ: " << info() << " enqueueAppData(bytes=" << msg->getByteLength() << ")\n";
    end += msg->getByteLength();
    if (seqLess(end, begin))
        throw cRuntimeError("Send queue is full");
    delete msg;
}

uint32 RQVirtualDataSendQueue::getBufferStartSeq()
{
    return begin;
}

uint32 RQVirtualDataSendQueue::getBufferEndSeq()
{
    return end;
}

RQSegment *RQVirtualDataSendQueue::createSegmentWithBytes(uint32 fromSeq, ulong numBytes)
{
    //rqEV << "sendQ: " << info() << " createSeg(seq=" << fromSeq << " len=" << numBytes << ")\n";

    ASSERT(seqLE(begin, fromSeq) && seqLE(fromSeq + numBytes, end));

    char msgname[32];
    sprintf(msgname, "rqseg(l=%lu)", numBytes);

    RQSegment *rqseg = conn->createRQSegment(msgname);
    rqseg->setSequenceNo(fromSeq);
    rqseg->setPayloadLength(numBytes);
    return rqseg;
}

void RQVirtualDataSendQueue::discardUpTo(uint32 seqNum)
{
    //rqEV << "sendQ: " << info() << " discardUpTo(seq=" << seqNum << ")\n";

    ASSERT(seqLE(begin, seqNum) && seqLE(seqNum, end));

    begin = seqNum;
}

} // namespace raptorq

} // namespace inet

