//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package inet.raptorqimplementation;

// import StandardHostRq;
@namespace(inet);
@namespace(raptorq);

import inet.networklayer.configurator.ipv4.FlatNetworkConfigurator;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;
import ned.DatarateChannel;
import ned.IBidirectionalChannel;


//
// TODO auto-generated type
//
network myexample
{
    @display("bgb=512,395");
    submodules:
        client: StandardHostRq {
            @display("p=44,213");
        }
        flatNetworkConfigurator: FlatNetworkConfigurator {
            @display("p=295,91");
        }
        server: StandardHostRq {
            @display("p=431,190");
        }
        router: Router {
            @display("p=252,256");
        }
    connections:
        router.pppg++ <--> DatarateChannel {  datarate = 1000Mbps;per=0;delay=0s; }<--> server.pppg++;
        client.pppg++ <--> DatarateChannel {  datarate = 1000Mbps;per=0;delay=0s; }<--> router.pppg++;
        
         
}
