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

#include "DumbRQ.h"
#include "../RaptorQ.h"

namespace inet {

namespace raptorq {

Register_Class(DumbRQ);

// just a dummy value
#define REXMIT_TIMEOUT    2

DumbRQ::DumbRQ() : RQAlgorithm(),
    state((DumbRQStateVariables *&)RQAlgorithm::state)
{
    rexmitTimer = nullptr;
}

DumbRQ::~DumbRQ()
{
    // cancel and delete timers
    if (rexmitTimer)
        delete conn->getRQMain()->cancelEvent(rexmitTimer);
}

void DumbRQ::initialize()
{
    RQAlgorithm::initialize();

    rexmitTimer = new cMessage("DumbRQ-REXMIT");
    rexmitTimer->setContextPointer(conn);
}

void DumbRQ::established(bool active)
{
    if (active) {
        // finish connection setup with ACK (possibly piggybacked on data)
        EV_INFO << "Completing connection setup by sending ACK (possibly piggybacked on data)\n";

        if (!conn->sendData(false, 65535,true, 1, 1 )) // modified moh
            conn->sendAck();
    }
}

void DumbRQ::connectionClosed()
{
    conn->getRQMain()->cancelEvent(rexmitTimer);
}

void DumbRQ::processTimer(cMessage *timer, RQEventCode& event)
{
    if (timer != rexmitTimer)
        throw cRuntimeError(timer, "unrecognized timer");

    conn->retransmitData();
    conn->scheduleTimeout(rexmitTimer, REXMIT_TIMEOUT);
}

void DumbRQ::sendCommandInvoked(cMessage *msg)
{
    // start sending
    conn->sendData(false, 65535 ,true, 1 ,1); // modified moh
}

void DumbRQ::receivedOutOfOrderSegment()
{
    EV_INFO << "Out-of-order segment, sending immediate ACK\n";
    conn->sendAck();
}

void DumbRQ::receiveSeqChanged()
{
    // new data received, ACK immediately (more sophisticated algs should
    // wait a little to see if piggybacking is possible)
    EV_INFO << "rcv_nxt changed to " << state->rcv_nxt << ", sending immediate ACK\n";
    conn->sendAck();
}

void DumbRQ::receivedDataAck(uint32)
{
    // ack may have freed up some room in the window, try sending.
    // small segments also OK (Nagle off)
    conn->sendData(false, 65535,true, 1 ,1 );// modified moh
}

void DumbRQ::receivedDuplicateAck()
{
    EV_INFO << "Duplicate ACK #" << state->dupacks << "\n";
}

void DumbRQ::receivedAckForDataNotYetSent(uint32 seq)
{
    EV_INFO << "ACK acks something not yet sent, sending immediate ACK\n";
    conn->sendAck();
}

void DumbRQ::ackSent()
{
}

void DumbRQ::dataSent(uint32 fromseq)
{
    if (rexmitTimer->isScheduled())
     conn->getRQMain()->cancelEvent(rexmitTimer);
     conn->scheduleTimeout(rexmitTimer, REXMIT_TIMEOUT);
}

void DumbRQ::segmentRetransmitted(uint32 fromseq, uint32 toseq)
{
}

void DumbRQ::restartRexmitTimer()
{
}

void DumbRQ::rttMeasurementCompleteUsingTS(uint32 echoedTS)
{
}

void DumbRQ::cancelRexmitTimer()
{
}

} // namespace RQ

} // namespace inet

