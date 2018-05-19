//
// Copyright (C) 2004 Andras Varga
// Copyright (C) 2010 Zoltan Bojthe
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

#ifndef __INET_RQBYTESTREAMRCVQUEUE_H
#define __INET_RQBYTESTREAMRCVQUEUE_H

#include <map>
#include <string>

#include "inet/common/INETDefs.h"

#include "../raptorq_common/RQSegment.h"
#include "RQVirtualDataRcvQueue.h"
#include "inet/common/ByteArray.h"

namespace inet {

namespace raptorq {

/**
 * RQ send queue that stores actual bytes.
 *
 * @see RQSendQueue
 */
class INET_API RQByteStreamRcvQueue : public RQVirtualDataRcvQueue
{
  protected:
    class Region : public RQVirtualDataRcvQueue::Region
    {
      protected:
        ByteArray data;

      public:
        Region(uint32 _begin, uint32 _end) : RQVirtualDataRcvQueue::Region(_begin, _end) {};
        Region(uint32 _begin, uint32 _end, ByteArray _data)
            : RQVirtualDataRcvQueue::Region(_begin, _end), data(_data) {};

        virtual ~Region() {};

        /// Merge other to self
        virtual bool merge(const RQVirtualDataRcvQueue::Region *other) override;

        /// Copy self to msg
        virtual void copyTo(cPacket *msg) const override;

        /**
         * Returns an allocated new Region object with filled with begin..seq and set self to seq..end
         */
        virtual RQByteStreamRcvQueue::Region *split(uint32 seq) override;
    };

  public:
    /**
     * Ctor.
     */
    RQByteStreamRcvQueue() : RQVirtualDataRcvQueue() {};

    /**
     * Virtual dtor.
     */
    virtual ~RQByteStreamRcvQueue();

    /**
     * Returns a string with region stored.
     */
    virtual std::string info() const override;

    cPacket *extractBytesUpTo(uint32 seq) override;

    /**
     * Create a new Region from rqseg.
     * Called from insertBytesFromSegment()
     */
    virtual RQVirtualDataRcvQueue::Region *createRegionFromSegment(RQSegment *rqseg) override;
};

} // namespace rq

} // namespace inet

#endif // ifndef __INET_RQBYTESTREAMRCVQUEUE_H

