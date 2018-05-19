//
// Copyright (C) 2004 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#include "RQByteStreamSendQueue.h"

#include "inet/common/RawPacket.h"
#include "../raptorq_common/RQSegment.h"

namespace inet {

namespace raptorq {

Register_Class(RQByteStreamSendQueue);

RQByteStreamSendQueue::RQByteStreamSendQueue() : RQSendQueue()
{
    begin = end = 0;
}

RQByteStreamSendQueue::~RQByteStreamSendQueue()
{
}

void RQByteStreamSendQueue::init(uint32 startSeq)
{
    begin = startSeq;
    end = startSeq;
    dataBuffer.clear();
}

std::string RQByteStreamSendQueue::info() const
{
    std::stringstream out;
    out << "[" << begin << ".." << end << "), " << dataBuffer.getLength() << " bytes";
    return out.str();
}

void RQByteStreamSendQueue::enqueueAppData(cPacket *msg)
{
    //rqEV << "sendQ: " << info() << " enqueueAppData(bytes=" << msg->getByteLength() << ")\n";
    RawPacket *bamsg = check_and_cast<RawPacket *>(msg);
    int64 bytes = bamsg->getByteLength();
    ASSERT(bytes == bamsg->getByteArray().getDataArraySize());
    dataBuffer.push(bamsg->getByteArray());
    end += bytes;
    delete msg;
}

uint32 RQByteStreamSendQueue::getBufferStartSeq()
{
    return begin;
}

uint32 RQByteStreamSendQueue::getBufferEndSeq()
{
    return end;
}

RQSegment *RQByteStreamSendQueue::createSegmentWithBytes(uint32 fromSeq, ulong numBytes)
{
    //rqEV << "sendQ: " << info() << " createSeg(seq=" << fromSeq << " len=" << numBytes << ")\n";
    ASSERT(seqLE(begin, fromSeq) && seqLE(fromSeq + numBytes, end));

    RQSegment *rqseg = new RQSegment();
    rqseg->setSequenceNo(fromSeq);
    rqseg->setPayloadLength(numBytes);

    // add payload messages whose endSequenceNo is between fromSeq and fromSeq+numBytes
    unsigned int fromOffs = (uint32)(fromSeq - begin);
    char *buffer = new char[numBytes];
    unsigned int bytes = dataBuffer.getBytesToBuffer(buffer, numBytes, fromOffs);
    ASSERT(bytes == numBytes);
    rqseg->getByteArray().assignBuffer(buffer, bytes);

    // give segment a name
    char msgname[80];
    sprintf(msgname, "rqseg(l=%lu)", numBytes);
    rqseg->setName(msgname);

    return rqseg;
}

void RQByteStreamSendQueue::discardUpTo(uint32 seqNum)
{
    //rqEV << "sendQ: " << info() << " discardUpTo(seq=" << seqNum << ")\n";
    ASSERT(seqLE(begin, seqNum) && seqLE(seqNum, end));
    dataBuffer.drop(seqNum - begin);
    begin = seqNum;
}

} // namespace raptorq

} // namespace inet

