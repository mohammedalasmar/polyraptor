
#include "RQMsgBasedRcvQueue.h"
#include "../../contract/raptorq/RQCommand_m.h"
#include "../raptorq_common/RQSegment.h"

namespace inet {

namespace raptorq {

Register_Class(RQMsgBasedRcvQueue);

RQMsgBasedRcvQueue::RQMsgBasedRcvQueue() : RQVirtualDataRcvQueue()
{
}

RQMsgBasedRcvQueue::~RQMsgBasedRcvQueue()
{
    while (!payloadList.empty()) {
        EV_DEBUG << "SendQueue Destructor: Drop msg from " << this->getFullPath()
                 << " Queue: offset=" << payloadList.front().seqNo
                 << ", length=" << payloadList.front().packet->getByteLength() << endl;
        delete payloadList.front().packet;
        payloadList.pop_front();
    }
}

void RQMsgBasedRcvQueue::init(uint32 startSeq)
{
    RQVirtualDataRcvQueue::init(startSeq);
}

std::string RQMsgBasedRcvQueue::info() const
{
    std::stringstream os;

    os << "rcv_nxt=" << rcv_nxt;

    for (const auto & elem : regionList) {
        os << " [" << (elem)->getBegin() << ".." << (elem)->getEnd() << ")";
    }

    os << " " << payloadList.size() << " msgs";

    return os.str();
}

uint32 RQMsgBasedRcvQueue::insertBytesFromSegment(RQSegment *rqseg)
{
    RQVirtualDataRcvQueue::insertBytesFromSegment(rqseg);

    cPacket *msg;
    uint32 endSeqNo;
    auto i = payloadList.begin();
    while (nullptr != (msg = rqseg->removeFirstPayloadMessage(endSeqNo))) {
        while (i != payloadList.end() && seqLess(i->seqNo, endSeqNo))
            ++i;

        // insert, avoiding duplicates
        if (i != payloadList.end() && i->seqNo == endSeqNo)
            delete msg;
        else {
            i = payloadList.insert(i, PayloadItem(endSeqNo, msg));
            ASSERT(seqLE(payloadList.front().seqNo, payloadList.back().seqNo));
        }
    }

    return rcv_nxt;
}

cPacket *RQMsgBasedRcvQueue::extractBytesUpTo(uint32 seq)
{
    cPacket *msg = nullptr;

    if (!payloadList.empty() && seqLess(payloadList.begin()->seqNo, seq))
        seq = payloadList.begin()->seqNo;

    payloadList.begin()->seqNo = seq;  // MOH: added to satisfy the 'if' condition below


    Region *reg = extractTo(seq);

    if (reg) {
        if (!payloadList.empty() && payloadList.begin()->seqNo == reg->getEnd()) {
//        std::cout << " payloadList.begin()->seqNo " << payloadList.begin()->seqNo   << "\n";
//        std::cout << "  reg->getEnd() " <<  reg->getEnd()   << "\n";
//        std::cout << "  payloadList.empty() " <<  payloadList.empty()   << "\n";
            msg = payloadList.begin()->packet;
            payloadList.erase(payloadList.begin());
         }
        delete reg;
    }
    return msg;
}

} // namespace raptorq

} // namespace inet

