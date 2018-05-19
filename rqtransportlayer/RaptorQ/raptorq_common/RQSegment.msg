 
cplusplus {{
#include <iostream>
#include "inet/common/INETDefs.h"
#include "inet/common/ByteArray.h"

namespace inet {
namespace raptorq {
class Sack;
class RQOption;
typedef RQOption *RQOptionPtr;
}
}

}}

namespace inet::raptorq;

cplusplus {{
    // default RQ header length: 20 bytes
    #define RQ_HEADER_OCTETS  20    // without options

    // maximum RQ header length (base + options): 60 = 4 * 15 bytes
    const unsigned int RQ_MAX_HEADER_OCTETS = 60;

    // maximum allowed sack entry number, if no other options are used
    const unsigned int MAX_SACK_ENTRIES = 4;

    typedef cPacket *cPacketPtr;

    inline std::ostream& operator<<(std::ostream& os, cPacketPtr msg)
    {
        return os << "(" << msg->getClassName() << ") " << msg->getName() << ": " << msg->info();
    }

    inline void doPacking(cCommBuffer *b, cPacketPtr& msg) {msg->parsimPack(b);}
    inline void doUnpacking(cCommBuffer *b, cPacketPtr& msg) {msg->parsimUnpack(b);}
}}

struct cPacketPtr;

class noncobject ByteArray;

struct RQPayloadMessage
{
    unsigned int endSequenceNo;
    cPacketPtr msg = nullptr;      // pointer to payload msg
}

//
// RQ Option Numbers
// Reference: http://www.iana.org/assignments/tcp-parameters/
// Date: 2011-07-02
//
// Note: Options not yet implemented should stay commented out
//
enum RQOptionNumbers
{
    RQOPTION_END_OF_OPTION_LIST = 0;                   // RFC 793, LENGTH: 1 Byte
    RQOPTION_NO_OPERATION = 1;                         // RFC 793, LENGTH: 1 Byte
    RQOPTION_MAXIMUM_SEGMENT_SIZE = 2;                 // RFC 793, LENGTH: 4 Bytes
    RQOPTION_WINDOW_SCALE = 3;                         // RFC 1323, LENGTH: 3 Bytes
    RQOPTION_SACK_PERMITTED = 4;                       // RFC 2018, LENGTH: 2 Bytes
    RQOPTION_SACK = 5;                                 // RFC 2018, LENGTH: N (max. N = 4) 8 * n + 2 Bytes  => 32 + 2 + 2 * NOP = 36 Bytes; If TIMESTAMP option is used with SACK: max. n = 3 => 12 Bytes (for Timestamp) + 28 Bytes (for SACK) = 40 Bytes
//    RQOPTION_ECHO = 6;                               // (obsoleted by option 8) RFC 1072 & RFC 6247, LENGTH: 6 Bytes
//    RQOPTION_ECHO_REPLY = 7;                         // (obsoleted by option 8) RFC 1072 & RFC 6247, LENGTH: 6 Bytes
    RQOPTION_TIMESTAMP = 8;                            // RFC 1323, LENGTH: 10 Bytes
//    RQOPTION_PARTIAL_ORDER_CONNECTION_PERMITTED = 9; // (obsolete) RFC 1693 & RFC 6247, LENGTH: 2 Bytes
//    RQOPTION_PARTIAL_ORDER_SERVICE_PROFILE = 10;     // (obsolete) RFC 1693 & RFC 6247, LENGTH: 3 Bytes
//    RQOPTION_CC = 11;                                // (obsolete) RFC 1644 & RFC 6247, LENGTH: -
//    RQOPTION_CC_NEW = 12;                            // (obsolete) RFC 1644 & RFC 6247, LENGTH: -
//    RQOPTION_CC_ECHO = 13;                           // (obsolete) RFC 1644 & RFC 6247, LENGTH: -
//    RQOPTION_RQ_ALTERNATE_CHECKSUM_REQUEST = 14;    // (obsolete) RFC 1146 & RFC 6247, LENGTH: 3 Bytes
//    RQOPTION_RQ_ALTERNATE_CHECKSUM_DATA = 15;       // (obsolete) RFC 1146 & RFC 6247, LENGTH: N
//  RQOPTION_SKEETER = 16;                             // [Knowles], LENGTH: -
//  RQOPTION_BUBBA = 17;                               // [Knowles], LENGTH: -
//  RQOPTION_TRAILER_CHECKSUM_OPTION = 18;             // [Subbu & Monroe], LENGTH: 3Bytes
//    RQOPTION_MD5_SIGNATURE_OPTION = 19;              // (obsoleted by option 29) RFC 2385, LENGTH: 18 Bytes
//  RQOPTION_SCPS_CAPABILITIES = 20;                   // [Scott], LENGTH: -
//  RQOPTION_SELECTIVE_NEGATIVE_ACKNOWLEDGEMENTS = 21; // [Scott], LENGTH: -
//  RQOPTION_RECORD_BOUNDARIES = 22;                   // [Scott], LENGTH: -
//  RQOPTION_CORRUPTION_EXPERIENCED = 23;              // [Scott], LENGTH: -
//  RQOPTION_SNAP = 24;                                // [Sukonnik], LENGTH: -
//  RQOPTION_UNASSIGNED = 25;                          // released 2000-12-18 [-], LENGTH: -
//  RQOPTION_RQ_COMPRESSION_FILTER = 26;              // [Bellovin], LENGTH: -
//  RQOPTION_QUICK_START_RESPONSE = 27;                // RFC 4782, LENGTH: 8 Bytes
//  RQOPTION_USER_TIMEOUT_OPTION = 28;                 // RFC 5482, LENGTH: 4 Bytes
//  RQOPTION_AUTHENTICATION_OPTION = 29;               // RFC 5925, LENGTH: -
//    RQOPTION kinds 30-252 Unassigned
//  RQOPTION_RFC3692_STYLE_EXPERIMENT_1 = 253;         // RFC 4727, LENGTH: N
//  RQOPTION_RFC3692_STYLE_EXPERIMENT_2 = 254;         // RFC 4727, LENGTH: N
};

//
// This structure represents a single SACK (selective acknowledgment):
//
class SackItem
{
    @fieldNameSuffix("_var"); //TODO remove when OMNeT++ 4.x compatibility is no longer required
    unsigned int start;     // start seq no. of sack block
    unsigned int end;       // end seq no. of sack block
}

class Sack extends SackItem
{
    @customize(true);
}

// Header Options (optional):
class noncobject RQOptionPtr;
class RQOption
{
    unsigned short kind @enum(RQOptionNumbers) = -1;  // option kind
    unsigned short length = 1;                    // option length
}

class RQOptionEnd extends RQOption
{
    kind = RQOPTION_END_OF_OPTION_LIST;
    length = 1;
}

class RQOptionNop extends RQOption
{
    kind = RQOPTION_NO_OPERATION;
    length = 1;
}

class RQOptionMaxSegmentSize extends RQOption
{
    kind = RQOPTION_MAXIMUM_SEGMENT_SIZE;
    length = 4;
    uint16_t maxSegmentSize;   // uint16_t
}

class RQOptionWindowScale extends RQOption
{
    kind = RQOPTION_WINDOW_SCALE;
    length = 3;
    unsigned short windowScale;   // uint8_t
}

class RQOptionSackPermitted extends RQOption
{
    kind = RQOPTION_SACK_PERMITTED;
    length = 2;
}

class RQOptionSack extends RQOption
{
    kind = RQOPTION_SACK;
    length = 2;     // 2 + getSackArraySize() * 8
    SackItem sackItem[];
}

class RQOptionTimestamp extends RQOption
{
    kind = RQOPTION_TIMESTAMP;
    length = 10;
    uint32_t   senderTimestamp;
    uint32_t   echoedTimestamp;
}

class RQOptionUnknown extends RQOption
{
    kind = -1;
    uint8_t bytes[];
}

//
// Represents a RQ segment, to be used with the ~RQ module.
//
// RQ header fields not explicitly modelled: work on going
// - Data Offset (number of 32 bit words in the header): represented
//   by cMessage::length().
// - Reserved (reserved for future use)
// - Checksum (header checksum): modelled by cMessage::hasBitError()
// - Header Options: Currently only EOL, NOP, MSS, WS, SACK_PERMITTED, SACK and TS are implemented
// - Padding
//
// cMessage::getKind() may be set to an arbitrary value: RQ entities will
// ignore it and use only the header fields (synBit, ackBit, rstBit).
//
packet RQSegment
{
    @customize(true);
    @fieldNameSuffix("_var"); //TODO remove when OMNeT++ 4.x compatibility is no longer required

    // Source Port
    unsigned short srcPort;

    // Destination Port
    unsigned short destPort;

    // Sequence Number: first sequence number of the first data octet
    // in the respective segment (except if SYN is set; then the the
    // seq. number is the initial seq. number (ISS) and the first data
    // octet is ISS + 1)
    unsigned int sequenceNo;

    // Acknowledgement Number: if ACK flag is set, this field contains
    // the next sequence number the sender of this segment is expecting
    // to receive
    unsigned int ackNo;

  // Added MOH
   bool isClient;
   bool isWriter;
   bool isReader;
   bool isHeader;
   bool isRequestPacket;
   
   bool isMulticastConnSeg;
   unsigned int  multicastGroupIndex;
   bool isMulticastSender; 
   bool isMulticastReceiver; 
   
    bool isMultiSourcingConn  ;
    unsigned int multiSourcingGroupIndex ;
   bool isMultiSourcingSender; 
   bool isMultiSourcingReceiver; 
   
   
   bool isLastSymbolToSend;
   
   
   unsigned int numSymbolsToSend;
   unsigned int numSymbolsToGet;
   
   unsigned int fileId;
   unsigned int requestSequenceNumber;
   unsigned int symbolSequenceNumber;
    unsigned int opcode;
 

    // RQ Header Length - default: 20 bytes
    // if header options are used the headerLength is greater than 20 bytes (default)
    unsigned short headerLength = RQ_HEADER_OCTETS; // RQ_HEADER_OCTETS = 20

    bool urgBit; // URG: urgent pointer field significant if set
    bool ackBit; // ACK: ackNo significant if set
    bool pshBit; // PSH: push function
    bool rstBit; // RST: reset the connection
    bool synBit; // SYN: synchronize seq. numbers
    bool finBit; // FIN: finish - no more data from sender

    // Window Size: the number of data octets beginning with the one indicated
    // in the acknowledgement field which the sender of this segment is
    // willing to accept
    unsigned short window;

    // Urgent Pointer: communicates the current value of the urgent pointer
    // as a positive offset from the sequence number in this segment. The
    // urgent pointer points to the sequence number of the octet following
    // the urgent data. This field is only be interpreted in segments with
    // the URG control bit set.
    unsigned short urgentPointer;

    // Header options (optional)
    // Currently only EOL, NOP, MSS, WS, SACK_PERMITTED, SACK and TS are implemented
    abstract RQOptionPtr headerOption[];

    // Payload length in octets (not an actual RQ header field).
    // This may not always be the same as encapsulatedPacket()->getByteLength();
    // e.g. when simulating a virtual data stream there's no encapsulated
    // packet at all.
    unsigned long payloadLength;

    // Message objects (cMessages) that travel in this segment as data.
    // This field is used only when the ~RQDataTransferMode is RQ_TRANSFER_OBJECT.
    // Every message object is put into the RQSegment that would (in real life)
    // carry its first octet. That is, if message object 'msg' with length=100 bytes
    // occupies stream offset number range 10000..10099, it will travel in the
    // RQSegment which carries the octet 10000. This way it is easily achieved
    // that the receiving RQ passes up the message object to its client
    // when the last byte of the message has arrived.
    abstract RQPayloadMessage payload[];

    // Message bytes that travel in this segment as data.
    // This field is used only when the ~RQDataTransferMode is RQ_TRANSFER_BYTESTREAM.
    ByteArray byteArray;
}
