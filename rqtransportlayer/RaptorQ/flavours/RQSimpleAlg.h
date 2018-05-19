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

#ifndef __INET_TCPSimpleAlg_H
#define __INET_TCPSimpleAlg_H

#include "inet/common/INETDefs.h"

#include "RQTahoeRenoFamily.h"

namespace inet {

namespace raptorq {

/**
 * State variables for RQSimpleAlg.
 */

typedef RQBaseAlgStateVariables RQSimpleAlgStateVariables;

//class INET_API TCPSimpleAlgStateVariables : public TCPBaseAlgStateVariables
//{
//  public:
//    TCPSimpleAlgStateVariables();
//    virtual std::string info() const override;
//    virtual std::string detailedInfo() const override;
//    virtual void setSendQueueLimit(uint32 newLimit);
//};


class INET_API RQSimpleAlg : public RQBaseAlg
{
  protected:
    RQSimpleAlgStateVariables *& state;    // alias to TCPAlgorithm's 'state'

    /** Create and return a TCPTahoeStateVariables object. */
      virtual RQStateVariables *createStateVariables() override
      {
          return new RQSimpleAlgStateVariables();
      }

//    virtual void processRexmitTimer(RQEventCode& event) override;

  public:
//    /** Ctor */
     RQSimpleAlg();
//
    /** Redefine what should happen when data got acked, to add congestion window management */
        virtual void receivedDataAck(uint32 firstSeqAcked) override;

        /** Redefine what should happen when dupAck was received, to add congestion window management */
        virtual void receivedDuplicateAck() override;


};

} // namespace raptorq

} // namespace inet

#endif // ifndef __INET_TCPSimpleAlg_H

