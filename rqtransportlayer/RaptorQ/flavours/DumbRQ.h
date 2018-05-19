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

#ifndef __INET_DUMBRQ_H
#define __INET_DUMBRQ_H

#include "inet/common/INETDefs.h"

#include "../RQAlgorithm.h"

namespace inet {

namespace raptorq {

/**
 * State variables for DumbRQ.
 */
class INET_API DumbRQStateVariables : public RQStateVariables
{
  public:
    //...
};

/**
 * A very-very basic RQAlgorithm implementation, with hardcoded
 * retransmission timeout and no other sophistication. It can be
 * used to demonstrate what happened if there was no adaptive
 * timeout calculation, delayed acks, silly window avoidance,
 * congestion control, etc.
 */
class INET_API DumbRQ : public RQAlgorithm
{
  protected:
    DumbRQStateVariables *& state;    // alias to TCLAlgorithm's 'state'

    cMessage *rexmitTimer;    // retransmission timer

  protected:
    /** Creates and returns a DumbRQStateVariables object. */
    virtual RQStateVariables *createStateVariables() override
    {
        return new DumbRQStateVariables();
    }

  public:
    /** Ctor */
    DumbRQ();

    virtual ~DumbRQ();

    virtual void initialize() override;

    virtual void established(bool active) override;

    virtual void connectionClosed() override;

    virtual void processTimer(cMessage *timer, RQEventCode& event) override;

    virtual void sendCommandInvoked(cMessage *msg) override;

    virtual void receivedOutOfOrderSegment() override;

    virtual void receiveSeqChanged() override;

    virtual void receivedDataAck(uint32 firstSeqAcked) override;

    virtual void receivedDuplicateAck() override;

    virtual void receivedAckForDataNotYetSent(uint32 seq) override;

    virtual void ackSent() override;

    virtual void dataSent(uint32 fromseq) override;

    virtual void segmentRetransmitted(uint32 fromseq, uint32 toseq) override;

    virtual void restartRexmitTimer() override;
    virtual void cancelRexmitTimer() override;



    virtual void rttMeasurementCompleteUsingTS(uint32 echoedTS) override;
};

} // namespace RQ

} // namespace inet

#endif // ifndef __INET_DUMBRQ_H

