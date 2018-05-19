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


package inet.raptorqimplementation.application.raptorqapp;

import inet.raptorqimplementation.application.contract.IRaptorQApp;


//
// Accepts any number of incoming RQ connections, and discards whatever
// arrives on them.
// Compatible with both ~IPv4 and ~IPv6.
//
// <b>Configuring App</b>
//
// The module parameter dataTransferMode should be set the transfer mode in RQ layer.
// Currently you have three choices:
//
//   -# set them to "bytecount".
//      This mode manages "virtual bytes", that is, only byte counts are
//      transmitted over the RQ connection and no actual data. cMessage
//      contents, and even message boundaries are not preserved with these
//      classes: for example, if the client sends a single cMessage with
//      length = 1 megabyte over RQ, the receiver-side client will see a
//      sequence of MSS-sized messages.
//
//   -# use "object", which transmits
//      cMessage objects (and subclasses) over a RQ connection. The same
//      message object sequence that was sent by the client to the
//      sender-side Rq entity will be reproduced on the receiver side.
//      If a client sends a cMessage with length = 1 megabyte, the
//      receiver-side client will receive the same message object (or a clone)
//      after the RQ entities have completed simulating the transmission
//      of 1 megabyte over the connection. This is a different behaviour
//      from RQVirtualDataSendQueue/RcvQueue.
//      This mode is not implemented in ~RQ_NSC yet.
//
//   -# use "bytestream", which transmits real bytes of messages.
//
simple RQSinkApp like IRaptorQApp
{
    parameters:
        volatile string localAddress = default(""); // may be left empty ("")
        volatile int localPort = default(1000); // port number to listen on
        string dataTransferMode @enum("bytecount","object","bytestream") = default("bytecount");
        @display("i=block/sink");



        int multiSrcGoupId = default(-1);
        bool isMultiSourcingSink = default(false);

        // moh
        bool isLongFlow= default(false); 
        int opcode;  // opcode=1 read  , opcode=2 write  
//        volatile int numSymbolsToGet = default(1);  // if this is a writer(sender)
        
        // MOH: method1 goodput using my signals 
//        @signal[throughputRecordv1](type=double);
//        @statistic[throughputRecordv1](title="RQ Sesssion's overall throughput"; source=throughputRecordv1;unit=bps; record=last); 
        
         // MOH: FCT using my signals 
        
        
        @signal[fctRecordv2](type=double);
        @statistic[fctRecordv2](title="flow completion time"; source=fctRecordv2; unit=s; record=last);
        
        @signal[rcvdPk](type=cPacket);
        @statistic[rcvdPk](title="packets (symbols) received"; source=rcvdPk; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        
         // MOH: FCT using ResultFilter.cc (messageAge)
//       @statistic[endToEndDelay](title="end-to-end delay"; source="messageAge(rcvdPk)"; unit=s; record=histogram,vector; interpolationmode=none);
       
        //MOH: method2 instantaneous throughputs, see ResultFilter.cc
         @statistic[mohThroughputRQ](title="mohThroughputRQ"; source="throughput(rcvdPk)"; unit=bps; record=mean);
        
        
    gates:
        input raptorqIn @labels(RQCommand/up);
        output raptorqOut @labels(RQCommand/down);
}

