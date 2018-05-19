#include "RQRequestsSendQueue.h"
#include "../raptorq_common/RQSegment.h"

namespace inet {

namespace raptorq {

Register_Class(RQRequestsSendQueue);

RQRequestsSendQueue::RQRequestsSendQueue() :
        RQSendQueue() {
    begin = end = 0;
}

RQRequestsSendQueue::~RQRequestsSendQueue() {
    for (auto & elem : payloadQueue)
        delete elem.msg;
}

void RQRequestsSendQueue::init(uint32 startSeq) {
    begin = startSeq;
    end = startSeq;
}

std::string RQRequestsSendQueue::info() const {
    std::stringstream out;
    out << "[" << begin << ".." << end << "), " << payloadQueue.size() << " packets";
    return out.str();
}

void RQRequestsSendQueue::enqueueAppData(cPacket *msg) {
    //rqEV << "sendQ: " << info() << " enqueueAppData(bytes=" << msg->getByteLength() << ")\n";
//    std::cout<< "sendQ: " << info() << " enqueueAppData(bytes=" << msg->getByteLength() << ")\n";

    end += msg->getByteLength();
    if (seqLess(end, begin))
        throw cRuntimeError("Send queue is full");

    Payload payload;
    payload.endSequenceNo = end;
    payload.msg = msg;
    payloadQueue.push_back(payload);
//    std::cout<< "sendQ2: " << info() << " enqueueAppData(bytes=" << msg->getByteLength() <<" name: "<<  msg->getName() << ")\n";

}

uint32 RQRequestsSendQueue::getBufferStartSeq() {
    return begin;
}

uint32 RQRequestsSendQueue::getBufferEndSeq() {
    return end;
}

RQSegment *RQRequestsSendQueue::createSegmentWithBytes(uint32 fromSeq, ulong numBytes)
{
      std::cout << "begin: " << begin << " end: " << end << " fromSeq=" << fromSeq <<  " numBytes=" << numBytes << ")\n";

    //rqEV << "sendQ: " << info() << " createSeg(seq=" << fromSeq << " len=" << numBytes << ")\n";
        ASSERT(seqLE(begin, fromSeq) && seqLE(fromSeq + numBytes, end));

        RQSegment *rqseg = new RQSegment(nullptr);

        rqseg->setSequenceNo(fromSeq);
        rqseg->setPayloadLength(numBytes);

        // add payload messages whose endSequenceNo is between fromSeq and fromSeq + numBytes
        auto i = payloadQueue.begin();

        while (i != payloadQueue.end() && seqLE(i->endSequenceNo, fromSeq))
            ++i;

        uint32 toSeq = fromSeq + numBytes;
        const char *payloadName = nullptr;

        while (i != payloadQueue.end() && seqLE(i->endSequenceNo, toSeq)) {
            if (!payloadName)
                payloadName = i->msg->getName();

            rqseg->addPayloadMessage(i->msg->dup(), i->endSequenceNo);
            ++i;
        }

        // give segment a name
        char msgname[80];
        if (!payloadName)
            sprintf(msgname, "rqseg(l=%lu,%dmsg)", numBytes, rqseg->getPayloadArraySize());
        else
            sprintf(msgname, "%.10s(l=%lu,%dmsg)", payloadName, numBytes, rqseg->getPayloadArraySize());
        rqseg->setName(msgname);

        return rqseg;
}

void RQRequestsSendQueue::discardUpTo(uint32 seqNum) {
    //rqEV << "sendQ: " << info() << " discardUpTo(seq=" << seqNum << ")\n";
   // std::cout << "begin: " << begin << " end: " << end << " discardUpTo(seq=" << seqNum << ")\n";


    ASSERT(seqLE(begin, seqNum) && seqLE(seqNum, end));

    begin = seqNum;

    // remove payload messages whose endSequenceNo is below seqNum
    while (!payloadQueue.empty() && seqLE(payloadQueue.front().endSequenceNo, seqNum)) {
        delete payloadQueue.front().msg;
        payloadQueue.pop_front();
    }
}

} // namespace raptorq

} // namespace inet

