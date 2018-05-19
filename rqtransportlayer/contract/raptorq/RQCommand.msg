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


cplusplus {{
#include "inet/networklayer/common/L3Address.h"
}}

namespace inet;

class noncobject L3Address;

//
// RQR command codes, sent by the application to RQR. These constants
// should be set as message kind on a message sent to the RQR entity.
//
// @see ~RQRCommand, ~RQROpenCommand, ~IRaptorQ
//
enum RQCommandCode
{
    RQ_C_OPEN_ACTIVE = 1;   // active open (must carry ~RQOpenCommand)
    RQ_C_OPEN_PASSIVE = 2;  // passive open (must carry ~RQOpenCommand)
    RQ_C_SEND = 3;          // send data (set on data packet)
    RQ_C_CLOSE = 5;         // "I have no more data to send"
    RQ_C_ABORT = 6;         // abort connection
    RQ_C_STATUS = 7;        // request status info (RQ_I_STATUS) from RQ
    RQ_C_QUEUE_BYTES_LIMIT = 8; // set send queue limit (in bytes)
    RQ_C_READ = 9;       // send request to RQ to deliver data
}


//
// RQ indications, sent by RQ to the application. RQ will set these
// constants as message kind on messages it sends to the application.
//
// @see ~RQCommand, ~RQStatusInfo, ~IRQ
//
enum RQStatusInd
{
    RQ_I_DATA = 1;              // data packet (set on data packet)
    RQ_I_URGENT_DATA = 2;       // urgent data (set on data packet)
    RQ_I_ESTABLISHED = 3;       // connection established
    RQ_I_PEER_CLOSED = 4;       // FIN received from remote RQ
    RQ_I_CLOSED = 5;            // connection closed normally (via FIN exchange)
    RQ_I_CONNECTION_REFUSED = 6; // connection refused
    RQ_I_CONNECTION_RESET = 7;  // connection reset
    RQ_I_TIMED_OUT = 8;         // conn-estab timer went off, or max retransm. count reached
    RQ_I_STATUS = 9;            // status info (will carry ~RQStatusInfo)
    RQ_I_SEND_MSG = 10;         // send queue abated, send more messages
    RQ_I_DATA_NOTIFICATION = 11; // notify the upper layer that data has arrived
}


//
// Currently not in use.
//
enum RQErrorCode
{
}


//
// Control info for RQ connections. This class is to be set as control info
// (see cMessage::setControlInfo()) on all messages exchanged between RQ and
// application, in both directions. Some commands and indications
// (RQ_C_OPEN_xxx, RQ_I_STATUS) use subclasses.
//
// connId identifies the connection locally within the application (internally,
// RQ uses the (app gate index, connId) pair to identify the socket).
// connId is to be chosen by the application in the open command.
//
//# TODO explain userId
//
// @see ~RQCommandCode, ~RQStatusInd, ~RQOpenCommand, ~RQStatusInfo, ~IRaptorQ
//
class RQCommand
{
    int connId = -1;   // identifies the socket within the application
    int userId = -1;   // id than can be freely used by the app
    bool forcMultiSrcConnClose = false;
    int  multiSourcingGrpId = -1;
    
}


//
// Currently not in use.
//
class RQErrorInfo extends RQCommand
{
    int errorCode @enum(RQErrorCode);
    string messageText;
}

//
// Defines what to transmit as payload in RQ segments
//
// Currently you have the following choices:
//   - RQ_TRANSFER_BYTECOUNT:
//      RQ layer will transmit byte counts only.
//   - RQ_TRANSFER_OBJECT:
//      RQ layer will transmit the copy of application packet C++ objects
//   - RQ_TRANSFER_BYTESTREAM:
//      RQ layer will transmit bytes of the application packet.
//
// See ~IRQ (the RQ layer interface) for details.
//
enum RQDataTransferMode
{
    RQ_TRANSFER_UNDEFINED = 0;         // Invalid value
    RQ_TRANSFER_BYTECOUNT = 1;         // Transmit byte counts only
    RQ_TRANSFER_OBJECT = 2;            // Transmit the application packet C++ objects
    RQ_TRANSFER_BYTESTREAM = 3;        // Transmit raw bytes
    RQ_REQUESTS = 4;
    RQ_RECEIVE_SYMBOLS=5; 
}

//
// Control info to be used for active or passive RQ open.
//
// localAddr, remoteAddr, localPort, remotePort should be self-explanatory.
// localAddr is optional because RQ can learn it from IP when a packet
// is received from the peer; localPort is optional because RQ supports
// ephemeral ports.
//
// The fork parameter is used with passive open, and controls what happens
// when an incoming connection is received. With fork=true, it emulates
// the Unix accept(2) syscall semantics: a new connection structure
// is created for the connection (with a new connId, see in ~RQCommand),
// and the connection structure with the old connId remains listening.
// With fork=false, all the above does not happen: the first connection
// is accepted (with the original connId), and further incoming connections
// will be refused by RQ by sending an RST segment.
//
// The dataTransferMode and rqAlgorithmClass fields
// allow per-connection TCP configuration.
// The dataTransferMode field set the
// The tcpAlgorithmClass field may contain name of class subclassed from
// TCPAlgorithm, respectively.
// If not set, module parameters with similar names are used.
//
// @see ~RQCommandCode, ~IRaptorQ
//
class RQOpenCommand extends RQCommand
{
    L3Address localAddr; // may be left empty
    L3Address remoteAddr;// required for active open
    int localPort = -1;       // required for passive open
    int remotePort = -1;      // required for active open
    bool fork = false;        // used only for passive open
    int dataTransferMode @enum(RQDataTransferMode); // whether to transmit C++ objects, real bytes or just byte counts. See ~RQDataTransferMode.
    string rqAlgorithmClass; // TCP congestion control algorithm; leave empty for default
    
    int fileIdToGet;
    unsigned int numSymbolsToSend;
    bool    isLongFlow;
    bool isMulticastConn;
    unsigned int multicastGroupIndex;
     bool isSender;
     
    bool isMultiSourcingConn;
    unsigned int multiSourcingGroupIndex;
//    bool forcMultiSrcConnClose;
     
}


//
// Control info to be used with the SEND command.
//
// @see ~RQCommandCode, ~IRaptorQ
//
class RQSendCommand extends RQCommand
{
}


//
// Sent with message kind RQ_I_ESTABLISHED, to let the app know
// about the local and remote IP address and port.
//
// @see ~RQCommandCode, ~IRaptorQ
//
class RQConnectInfo extends RQCommand
{
    L3Address localAddr;
    L3Address remoteAddr;
    int localPort;
    int remotePort;
}


//
// Sent with message kind RQ_I_STATUS, in response to command RQ_C_STATUS.
// For explanation of variables, see RFC 793 or RQStateVariables in
// RQConnection.h.
//
// @see ~RQStatusInd, ~RQCommandCode, ~IRaptorQ
//
class RQStatusInfo extends RQCommand
{
    int state;
    string stateName;

    L3Address localAddr;
    L3Address remoteAddr;
    int localPort;
    int remotePort;

    unsigned int snd_mss;

    unsigned int snd_una;
    unsigned int snd_nxt;
    unsigned int snd_max;
    unsigned int snd_wnd;
    unsigned int snd_up;
    unsigned int snd_wl1;
    unsigned int snd_wl2;
    unsigned int iss;

    unsigned int rcv_nxt;
    unsigned int rcv_wnd;
    unsigned int rcv_up;
    unsigned int irs;

    bool fin_ack_rcvd;
}

