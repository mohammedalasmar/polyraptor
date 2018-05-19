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

#include <algorithm>
#include "RQSimpleAlg.h"
#include "inet/raptorqimplementation/rqtransportlayer/RaptorQ/RaptorQ.h"

namespace inet {

namespace raptorq {

Register_Class(RQSimpleAlg);

RQSimpleAlg::RQSimpleAlg() : RQBaseAlg(),
    state((RQSimpleAlgStateVariables *&)RQAlgorithm::state)
{
}



//std::string TCPSimpleAlgStateVariables::info() const
//{
//    std::stringstream out;
//    out << TCPBaseAlgStateVariables::info();
//   //Print our variables
//    return out.str();
//}
//
//std::string TCPSimpleAlgStateVariables::detailedInfo() const
//{
//    std::stringstream out;
//    out << TCPBaseAlgStateVariables::detailedInfo();
//    //Print our variables
//    return out.str();
//}

//---
//void processRexmitTimer(TCPEventCode& event){
//
//}




void RQSimpleAlg::receivedDataAck(uint32 firstSeqAcked){
    RQBaseAlg::receivedDataAck(firstSeqAcked);
  //  state->snd_cwnd += 2*state->snd_mss;
    state->snd_cwnd=4*state->snd_mss; // Note: MOH: the server's cwnd remains 1 mss to send one symbol upon receiveing a request

    if (cwndVector)
        cwndVector->record(state->snd_cwnd);

    sendData(false);
}

void RQSimpleAlg::receivedDuplicateAck(){
    RQBaseAlg::receivedDuplicateAck();
}

} // namespace raptorq

} // namespace inet

