

cplusplus {{
#include "inet/common/INETDefs.h"
}}

namespace inet;

//
// Message class for generic request-reply style applications.
// The client sends a ~GenericAppMsg which contains the number of
// bytes the client wants the server to send back as reply.
//
// This way intelligence (behaviour specific to the modelled application,
// e.g. HTTP, SMB, database protocol) needs only to be present in
// the client, and the server model can be kept simple and dumb.
//
// @see ~TCPGenericSrvApp, TCPGenericCliAppBase (C++ only)
//
packet GenericAppMsgRq
{
    int ESI;
    int SBN;  
    int fileId;              // 
    int opcode;              // 1 Read request (RRQ), 2 Write request (WRQ)
    int numSymbolsToGet;  //  if this is a reader(receiver)
    bool isClient;  // receiver 
    
    int numSymbolsToSend;  //  if this is a writer(sender)
    
    bool isReader;
    bool isWriter;
    
    int expectedReplyLength; // in bytes
    double replyDelay;       // reply after this many seconds
    bool serverClose;        // if true, server should close the
                             // connection after sending the reply
}



