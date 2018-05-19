//
// Copyright (C) 2004 Andras Varga
// Copyright (C) 2009-2010 Thomas Reschka
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

package inet.raptorqimplementation.rqtransportlayer.RaptorQ;

import  inet.raptorqimplementation.rqtransportlayer.contract.IRaptorQ;

//
// RQ protocol implementation.
// See the ~IRaptorQ for the RaptorQ layer general informations.
//
// This implementation supports:
//   - RFC  793 - Transmission Control Protocol
//   - RFC  896 - Congestion Control in IP/RQ Internetworks
//   - RFC 1122 - Requirements for Internet Hosts -- Communication Layers
//   - RFC 1323 - RQ Extensions for High Performance
//   - RFC 2018 - RQ Selective Acknowledgment Options
//   - RFC 2581 - RQ Congestion Control
//   - RFC 2883 - An Extension to the Selective Acknowledgement (SACK) Option for RQ
//   - RFC 3042 - Enhancing RQ's Loss Recovery Using Limited Transmit
//   - RFC 3390 - Increasing RQ's Initial Window
//   - RFC 3517 - A Conservative Selective Acknowledgment (SACK)-based Loss Recovery
//                Algorithm for RQ
//   - RFC 3782 - The NewReno Modification to RQ's Fast Recovery Algorithm
//
// This module is compatible with both ~IPv4 and ~IPv6.
//
// A RQ segment is represented by the class ~RQSegment.
//
// <b>Communication with clients</b>
//
// For communication between client applications and RQ, the ~RQCommandCode
// and ~RQStatusInd enums are used as message kinds, and ~RQCommand
// and its subclasses are used as control info.
//
// To open a connection from a client app, send a cMessage to RQ with
// RQ_C_OPEN_ACTIVE as message kind and a ~RQOpenCommand object filled in
// and attached to it as control info. (The peer RQ will have to be LISTENing;
// the server app can achieve this with a similar cMessage but RQ_C_OPEN_PASSIVE
// message kind.) With passive open, there's a possibility to cause the connection
// "fork" on an incoming connection, leaving the original connection LISTENing
// on the port (see the fork field in ~RQOpenCommand).
//
// The client app can send data by assigning the RQ_C_SEND message kind
// and attaching a ~RQSendCommand control info object to the data packet,
// and sending it to RQ. The server app will receive data as messages
// with the RQ_I_DATA message kind and ~RQSendCommand control info.
//
// To close, the client sends a cMessage to RQ with the RQ_C_CLOSE message kind
// and ~RQCommand control info.
//
// RQ sends notifications to the application whenever there's a significant
// change in the state of the connection: established, remote RQ closed,
// closed, timed out, connection refused, connection reset, etc. These
// notifications are also cMessages with message kind RQ_I_xxx
// (RQ_I_ESTABLISHED, etc.) and ~RQCommand as control info.
//
// One RQ module can serve several application modules, and several
// connections per application. The <i>k</i>th application connects to RQ's
// appIn[k] and appOut[k] ports. When talking to applications, a
// connection is identified by the (application port index, connId) pair,
// where connId is assigned by the application in the OPEN call.
//
// <b>Sockets</b>
//
// The RQSocket C++ class is provided to simplify managing RQ connections
// from applications. RQSocket handles the job of assembling and sending
// command messages (OPEN, CLOSE, etc) to RQ, and it also simplifies
// the task of dealing with packets and notification messages coming from RQ.
//
// <b>Communication with the IP layer</b>
//
// The RQ model relies on sending and receiving ~IPv4ControlInfo objects
// attached to RQ segment objects as control info
// (see cMessage::setControlInfo()).
//
// <b>Configuring RQ</b>
//
// The obsoleted sendQueueClass and receiveQueueClass parameters are removed!
// You must set the dataTransferMode in ~RQOpenCommand message.
//
//   -# use the module parameter (limitedTransmitEnabled) to enabled/disabled
//      Limited Transmit algorithm (RFC 3042) integrated to RQBaseAlg
//      (can be used for RQNewReno, RQReno, RQTahoe and RQNoCongestionControl but not
//      for DumbRQ).
//
//   -# use the module parameter (increasedIWEnabled) to change Initial Window
//      from one segment (RFC 2001) (based on MSS) to maximal four segments
//      (min(4*MSS, max (2*MSS, 4380 bytes))) (RFC 3390) integrated to
//      RQBaseAlg (can be used for RQNewReno, RQReno, RQTahoe and RQNoCongestionControl
//      but not for DumbRQ).
//
// The RQ flavour supported depends on the value of the RQAlgorithmClass
// module parameter, e.g. "RQTahoe" or "RQReno". In the future, other
// classes can be written which implement Vegas, LinuxRQ (which
// differs from others) or other variants.
//
// Note that ~RQOpenCommand allows RQAlgorithmClass to be chosen per-connection.
//
// Notes:
//  - if you do active OPEN, then send data and close before the connection
//    has reached ESTABLISHED, the connection will go from SYN_SENT to CLOSED
//    without actually sending the buffered data. This is consistent with
//    RFC 793 but may not be what you'd expect.
//  - handling segments with SYN+FIN bits set (esp. with data too) is
//    inconsistent across RQs, so check this one if it's of importance
//
// <b>Standards</b>
//
// The RQ module itself implements the following:
//  - all RFC 793 RQ states and state transitions
//  - connection setup and teardown as in RFC 793
//  - generally, RFC 793 compliant segment processing
//  - all socked commands (except RECEIVE) and indications
//  - receive buffer to cache above-sequence data and data not yet forwarded
//    to the user
//  - CONN-ESTAB timer, SYN-REXMIT timer, 2MSL timer, FIN-WAIT-2 timer
//  - The basic SACK implementation (RFC 2018 and RFC 2883) is located in
//    RQ main (and not in flavours).
//    This means that all existing RQ algorithm classes may be used with
//    SACK, although currently only RQReno makes sense.
//  - RFC 3517 - (SACK)-based Loss Recovery algorithm which is a conservative
//    replacement of the fast recovery algorithm (RFC2581) integrated to
//    RQReno but not to RQNewReno, RQTahoe, RQNoCongestionControl and DumbRQ.
//  - changes from RFC 2001 to RFC 2581:
//      - ACK generation (ack_now = true) RFC 2581, page 6: "(...) a RQ receiver SHOULD send an immediate ACK
//        when the incoming segment fills in all or part of a gap in the sequence space."
//  - RQ header options:
//      - EOL: End of option list.
//      - NOP: Padding bytes, currently needed for SACK_PERMITTED and SACK.
//      - MSS: The value of snd_mss (SMSS) is set to the minimum of snd_mss
//        (local parameter) and the value specified in the MSS option
//        received during connection startup. Based on [RFC 2581, page 1].
//      - WS: Window Scale option, based on RFC 1323.
//      - SACK_PERMITTED: SACK can only be used if both nodes sent SACK_-
//        PERMITTED during connection startup.
//      - SACK: SACK option, based on RFC 2018, RFC 2883 and RFC 3517.
//      - TS: Timestamps option, based on RFC 1323.
//  - flow control: finite receive buffer size (initiated by parameter
//    advertisedWindow). If receive buffer is exhausted (by out-of-order
//    segments) and the payload length of a new received segment
//    is higher than free receiver buffer, the new segment will be dropped.
//    Such drops are recorded in RQRcvQueueDropsVector.
//
// The RQNewReno, RQReno and RQTahoe algorithms implement:
//  - RFC 1122 - delayed ACK algorithm (optional) with 200ms timeout
//  - RFC 896 - Nagle's algorithm (optional)
//  - Jacobson's and Karn's algorithms for round-trip time measurement and
//    adaptive retransmission
//  - RQTahoe (Fast Retransmit), RQReno (Fast Retransmit and Fast Recovery), RQNewReno (Fast Retransmit and Fast Recovery)
//  - RFC 3390 - Increased Initial Window (optional) integrated to RQBaseAlg
//    (can be used for RQNewReno, RQReno, RQTahoe and RQNoCongestionControl but not
//    for DumbRQ).
//  - RFC 3042 - Limited Transmit algorithm (optional) integrated to RQBaseAlg
//    (can be used for RQNewReno, RQReno, RQTahoe and RQNoCongestionControl but not
//    for DumbRQ).
//
// Missing bits:
//  - URG and PSH bits not handled. Receiver always acts as if PSH was set
//    on all segments: always forwards data to the app as soon as possible.
//  - no RECEIVE command. Received data are always forwarded to the app as
//    soon as possible, as if the app issued a very large RECEIVE request
//    at the beginning. This means there's currently no flow control
//    between RQ and the app.
//  - all timeouts are precisely calculated: timer granularity (which is caused
//    by "slow" and "fast" i.e. 500ms and 200ms timers found in many *nix RQ
//    implementations) is not simulated
//  - new ECN flags (CWR and ECE). Need to be added to header by [RFC 3168].
//
// RQNewReno/RQReno/RQTahoe issues and missing features:
//  - KEEP-ALIVE not implemented (idle connections never time out)
//  - Nagle's algorithm (RFC 896) possibly not precisely implemented
//
// The above problems are relatively easy to fix, and will be resolved in the
// next iteration. Also, other RQAlgorithms will be added.
//
// <b>Tests</b>
//
// There are automated test cases (*.test files) for RQ -- see the <i>tests</i>
// directory in the source distribution.
//
// Please also see ChangeLog.
//
simple RaptorQ like IRaptorQ
{
    parameters:
        int advertisedWindow = default(14*this.mss); // in bytes, corresponds with the maximal receiver buffer capacity (Note: normally, NIC queues should be at least this size)
        bool delayedAcksEnabled = default(false); // delayed ACK algorithm (RFC 1122) enabled/disabled
        bool nagleEnabled = default(false); // Nagle's algorithm (RFC 896) enabled/disabled
        bool limitedTransmitEnabled = default(false); // Limited Transmit algorithm (RFC 3042) enabled/disabled (can be used for RQReno/RQTahoe/RQNewReno/RQNoCongestionControl)
        bool increasedIWEnabled = default(false); // Increased Initial Window (RFC 3390) enabled/disabled
        bool sackSupport = default(false); // Selective Acknowledgment (RFC 2018, 2883, 3517) support (header option) (SACK will be enabled for a connection if both endpoints support it)
        bool windowScalingSupport = default(false); // Window Scale (RFC 1323) support (header option) (WS will be enabled for a connection if both endpoints support it)
        int windowScalingFactor = default(-1); // Window Scaling Factor to the power of 2. -1 indicates that manual window scaling is turned off.
        bool timestampSupport = default(false); // Timestamps (RFC 1323) support (header option) (TS will be enabled for a connection if both endpoints support it)
        int mss = default(536); // Maximum Segment Size (RFC 793) (header option)
        string rqAlgorithmClass =default("RQSimpleAlg"); //   default("RQReno"); // RQSimpleAlg/RQVegas/DumbRQ/ RQReno/RQTahoe/RQNewReno/RQNoCongestionControl/DumbRQ
        bool recordStats = default(true); // recording of seqNum etc. into output vectors enabled/disabled
        string sendQueueClass = default("");    // Obsolete!!!
        string receiveQueueClass = default(""); // Obsolete!!!
        bool useDataNotification = default(false); // turn the notifications for arrived data on or off
        @display("i=block/wheelbarrow");
        
        @signal[numRequestsRTOs](type=double);
        @statistic[numRequestsRTOs](title="numRequestsRTOs "; source=numRequestsRTOs; record=vector); 
        
        
    gates:
        input appIn[] @labels(RQCommand/down);
        input ipIn @labels(RQSegment,IPv4ControlInfo/up,IPv6ControlInfo/up);
        output appOut[] @labels(RQCommand/up);
        output ipOut @labels(RQSegment,IPv4ControlInfo/down,IPv6ControlInfo/down);
}
