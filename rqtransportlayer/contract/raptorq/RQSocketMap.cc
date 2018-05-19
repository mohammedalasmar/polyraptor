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

#include "RQSocketMap.h"
#include "inet/common/INETDefs.h"


namespace inet {

RQSocket *RQSocketMap::findSocketFor(cMessage *msg)
{
    RQCommand *ind = dynamic_cast<RQCommand *>(msg->getControlInfo());
    if (!ind)
        throw cRuntimeError("RQSocketMap: findSocketFor(): no RQCommand control info in message (not from RQ?)");

    int connId = ind->getConnId();
    auto i = socketMap.find(connId);
    ASSERT(i == socketMap.end() || i->first == i->second->getConnectionId());
    return (i == socketMap.end()) ? nullptr : i->second;
}

void RQSocketMap::addSocket(RQSocket *socket)
{
    ASSERT(socketMap.find(socket->getConnectionId()) == socketMap.end());
    socketMap[socket->getConnectionId()] = socket;
}

RQSocket *RQSocketMap::removeSocket(RQSocket *socket)
{
    auto i = socketMap.find(socket->getConnectionId());
    if (i != socketMap.end())
        socketMap.erase(i);
    return socket;
}

void RQSocketMap::deleteSockets()
{
    for (auto & elem : socketMap)
        delete elem.second;
    socketMap.clear();
}

} // namespace inet

