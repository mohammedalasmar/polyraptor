#include "inet/raptorqimplementation/application/raptorqapp/GenericAppMsgRq_m.h"
#include "inet/raptorqimplementation/application/raptorqapp/RaptorQGenericSrvThread.h"
#include <iostream>
#include <fstream>
using namespace std;
namespace inet {

Register_Class(RaptorQGenericSrvThread);

void RaptorQGenericSrvThread::established() {
    // no initialization needed
}

void RaptorQGenericSrvThread::dataArrived(cMessage *msg, bool) {
    std::cout << "data-arrived-from-client::::   " << msg->getFullPath()  << std::endl;

    GenericAppMsgRq *appmsg = dynamic_cast<GenericAppMsgRq *>(msg);
    int opcode = appmsg->getOpcode();
    int fileId = appmsg->getFileId();
    bool isClient= appmsg->getIsClient();
    std::cout << "data-arrived-from-client:OPCODE=   " << opcode << std::endl;

    if (!appmsg)
        throw cRuntimeError("Message (%s)%s is not a GenericAppMsgRq -- "
                "probably wrong client app, or wrong setting of RQ's "
                "dataTransferMode parameters "
                "(try \"object\")", msg->getClassName(), msg->getName());

//    if (appmsg->getReplyDelay() > 0)
//        throw cRuntimeError(
//                "Cannot process (%s)%s: %s class doesn't support replyDelay field"
//                        " of GenericAppMsgRq, try to use RQGenericSrvApp instead",
//                msg->getClassName(), msg->getName(), getClassName());

    bool doClose = appmsg->getServerClose();

    if (isClient == true) {
        // process message: send back requested number of bytes, then close
        // connection if that was requested too
        long requestedBytes = appmsg->getExpectedReplyLength();


        if (requestedBytes == 0) {
            delete appmsg;
        } else {
//            char msgname[16];
//            sprintf(msgname, "SYMBOL-%d", ++ESI);
//            GenericAppMsgRq *datamsg = new GenericAppMsgRq(msgname);
//            datamsg->setByteLength(requestedBytes);
//
//            datamsg->setESI(ESI);
//            datamsg->setSBN(SBN); // SBN=0  for now: one Source Block
//            getSocket()->send(datamsg);
//        appmsg->setByteLength(requestedBytes);
//        delete appmsg->removeControlInfo();
//        getSocket()->send(appmsg);
        }
    }

    if (doClose)
           getSocket()->close();
//    unsigned char data[1000];

    //ofstream file("mohammedfile.txt",std::ofstream::binary);
    // char* buffer = new char[10000 ];

    //  file.write(buffer,10000);
    //  file << "helloooooo " << std::endl;
    //  file << "hiiiiiiii " << std::endl;
    //  file.close();

}

void RaptorQGenericSrvThread::timerExpired(cMessage *timer) {
    // no timers in this serverThread
}

} // namespace inet

