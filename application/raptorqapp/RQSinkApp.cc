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

#include "inet/raptorqimplementation/application/raptorqapp/RQSinkApp.h"
#include "inet/raptorqimplementation/application/raptorqapp/GenericAppMsgRq_m.h"
#include "inet/common/lifecycle/NodeOperations.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/lifecycle/NodeStatus.h"
//#include "inet/transportlayer/contract/tcp/TCPCommand_m.h"
#include "inet/raptorqimplementation/rqtransportlayer/contract/raptorq/RQCommand_m.h"
//#include "/Volumes/LocalDataHD/m/ma/ma777/Desktop/omnetpp-5.2.1-hpc/omnetpp-5.2.1/samples/inet-myprojects/inet/src/inet/common/ResultFilters.h"
namespace inet {
#define SEND_INIT_REQUEST_TO_READ    0

//RQSinkApp::~RQSinkApp() {
//    cancelAndDelete(timeoutMsg);
//
//}
Define_Module(RQSinkApp);

simsignal_t RQSinkApp::rcvdPkSignalRQ = registerSignal("rcvdPk");

simsignal_t fctRecordv2 = NodeStatus::registerSignal("fctRecordv2");

// MOH ADDED
//simsignal_t throughputRecordv1eee = NodeStatus::registerSignal("throughputRecordv1eee");

void RQSinkApp::initialize(int stage)
{
    cSimpleModule::initialize(stage);

    isLongFlow = par("isLongFlow");  // moh
    opcode = par("opcode");

    multiSrcGoupId = par("multiSrcGoupId");
    isMultiSourcingSink= par("isMultiSourcingSink") ;
    if (stage == INITSTAGE_LOCAL) {
        bytesRcvd = 0;
        WATCH(bytesRcvd);
    }
    else if (stage == INITSTAGE_APPLICATION_LAYER) {
        timeoutMsg = new cMessage("timer");

        bool isOperational;
        NodeStatus *nodeStatus = dynamic_cast<NodeStatus *>(findContainingNode(this)->getSubmodule("status"));
        isOperational = (!nodeStatus) || nodeStatus->getState() == NodeStatus::UP;
        if (!isOperational)
            throw cRuntimeError("This module doesn't support starting in node DOWN state");

        const char *localAddress = par("localAddress");
        int localPort = par("localPort");
//        std::cout << "RQSinkApp initialize -- localPort =   " << localPort << "\n";
//        std::cout << "RQSinkApp initialize -- isLongFlow =   " << isLongFlow << "\n";
        gateToRaptorQ = gate("raptorqOut"); // MOH for multi sourcing
        socket.setOutputGate(gate("raptorqOut"));
        socket.readDataTransferModePar(*this);
        socket.bind(localAddress[0] ? L3AddressResolver().resolve(localAddress) : L3Address(), localPort);
        socket.listen();

         timeoutMsg->setKind(SEND_INIT_REQUEST_TO_READ);
        scheduleAt(simTime(), timeoutMsg);
    }

}

void RQSinkApp::handleMessage(cMessage *msg)
{
    if (msg->getKind() == RQ_I_PEER_CLOSED) {

        //Moh: added time stamp when closing the connection
        tEndAdded = simTime();
       std::cout << "RQSinkApp::handleMessage mohmohmmohmohmohmohmohmohm   PEER CLOSE "<<bytesRcvd <<" \n";
        //Moh: increment numCompletedShortFlows in the centralScheduler
        std::string mod = "FatTree.centralScheduler";
        cModule* centralMod = getModuleByPath(mod.c_str());
        
        
        if (centralMod) {
            int   numFinishedFlows = centralMod->par("numCompletedShortFlows");
            int newNumFinishedFlows = numFinishedFlows +1 ;
            centralMod->par("numCompletedShortFlows").setDoubleValue(newNumFinishedFlows);
//            std::cout << "RQSinkApp::handleMessage  numCompletedShortFlows " << newNumFinishedFlows <<  "\n\n\n";
        }
        delete msg;

        // we close too
//        msg->setName("close");
//        msg->setKind(RQ_C_CLOSE);
      //  send(msg, "raptorqOut");  // TODO    we need to use this one
    }
    else if (msg->getKind() == RQ_I_DATA
            || msg->getKind() == RQ_I_URGENT_DATA) {
        cPacket *pk = PK(msg);
        long packetLength = pk->getByteLength();
        bytesRcvd += packetLength;
        emit(rcvdPkSignalRQ, pk);
        // Moh added: time stamp when receiving the first data packet (not the SYN, as the app wouldn't get that packet)
        if (firstDataReceived == true) {
            tStartAdded = simTime();
            firstDataReceived = false;
        }
//        std::cout << "bytesRcvd  " << bytesRcvd <<  " " <<  this->getFullPath() << std::endl;

         if (bytesRcvd == 70000) {
             if (isMultiSourcingSink == true) allReceivedCloseAllMultiSourcingConns();
         }




//            tEndAdded = simTime(); // TODO remove this line temp now MOHH
        //        delete msg;



    } else if (msg->getKind() == RQ_I_ESTABLISHED ){
        delete msg;
//          sendRequest();

    }


    else {
        // must be data or some kind of indication -- can be dropped
        delete msg;
    }
}


void RQSinkApp::sendRequest() {
        char msgname[16];
    //    sprintf(msgname, "REQUEST-%d",++requestIndex);
        sprintf(msgname, "REQUEST-INIT_READ");

        GenericAppMsgRq *msg = new GenericAppMsgRq(msgname);
        msg->setByteLength(10);
//        msg->setExpectedReplyLength(replyLength);
        msg->setServerClose(false);
        msg->setIsClient(true);  // client 1, server 0 ///////dasdasdsadasdasdasdasdad
        msg->setOpcode(opcode);
//        msg->setFileId(fileId);
        msg->setNumSymbolsToGet(30);
    //    msg->setNamePooling(true);

//        EV_INFO << "sending request with " << requestLength
//                       << " bytes, expected reply length " << replyLength
//                       << " bytes," << "numRequestsToSend " << numRequestsToSend
//                       << " request\n";

             socket.send(msg);

}


void RQSinkApp::allReceivedCloseAllMultiSourcingConns() {

    std::cout << "allReceivedCloseAllMultiSourcingConns \n\n\n\n" ;
    char msgname[16];
    sprintf(msgname, "CloseMultiSourcingConns");

    GenericAppMsgRq *msg = new GenericAppMsgRq(msgname);
    msg->setByteLength(10);
//    msg->setServerClose(false);
//    msg->setIsClient(true);  // client 1, server 0 ///////dasdasdsadasdasdasdasdad
//    msg->setOpcode(opcode);
//        msg->setFileId(fileId);
//    msg->setNumSymbolsToGet(30);
//    msg->setNamePooling(true);

//        EV_INFO << "sending request with " << requestLength
//                       << " bytes, expected reply length " << replyLength
//                       << " bytes," << "numRequestsToSend " << numRequestsToSend
//                       << " request\n";

    RQCommand *cmd = new RQCommand();
    cmd->setForcMultiSrcConnClose(true);
    cmd->setMultiSourcingGrpId(multiSrcGoupId);

    msg->setControlInfo(cmd);
     send(msg,  gateToRaptorQ);


//        socket.send(msg);

}



void RQSinkApp::finish()
{
//     std::cout << " RQSinkApp::finish() fffffffffffffffffffffffffffffffffffffffffffffffffffff \n\n\n";

    // MOH Added
    double throughput = 8 * (double) bytesRcvd / (SIMTIME_DBL(tEndAdded - tStartAdded));
    double FCT = SIMTIME_DBL(tEndAdded - tStartAdded);
//    std::cout << FCT << "\n\n\n";

    // don't emit the FCT of the longFlows(no need), we just observe the shortFlows
    if (isLongFlow == false) {
//        if (FCT < 0)  throw cRuntimeError("FCT FCT negative ");
        if (FCT < 0)
//         std::cout << " negative !!! FCT   " << FCT << std::endl;
//        emit(throughputRecordv1eee, throughput); // we don't use this throughputRecordv1, we use the throughput calculations in ResultFilter.cc
         emit(fctRecordv2, FCT);
          std::cout << "\n\n\n RQSinkApp::finish() bytesRcvd ";
          std::cout << bytesRcvd << "  "   << this->getFullPath() <<  std::endl;
    }
}

void RQSinkApp::refreshDisplay() const
{
    std::ostringstream os;
//    os << RQSocket::stateName(socket.getState()) << "\nrcvd: " << bytesRcvd << " bytes";
    getDisplayString().setTagArg("t", 0, os.str().c_str());
}

} // namespace inet

