//
// Generated file, do not edit! Created by nedtool 5.2 from inet/raptorqimplementation/application/raptorqapp/GenericAppMsgRq.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include "GenericAppMsgRq_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

namespace inet {

// forward
template<typename T, typename A>
std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec);

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// operator<< for std::vector<T>
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');
    
    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

Register_Class(GenericAppMsgRq)

GenericAppMsgRq::GenericAppMsgRq(const char *name, short kind) : ::omnetpp::cPacket(name,kind)
{
    this->ESI = 0;
    this->SBN = 0;
    this->fileId = 0;
    this->opcode = 0;
    this->numSymbolsToGet = 0;
    this->isClient = false;
    this->numSymbolsToSend = 0;
    this->isReader = false;
    this->isWriter = false;
    this->expectedReplyLength = 0;
    this->replyDelay = 0;
    this->serverClose = false;
}

GenericAppMsgRq::GenericAppMsgRq(const GenericAppMsgRq& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

GenericAppMsgRq::~GenericAppMsgRq()
{
}

GenericAppMsgRq& GenericAppMsgRq::operator=(const GenericAppMsgRq& other)
{
    if (this==&other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void GenericAppMsgRq::copy(const GenericAppMsgRq& other)
{
    this->ESI = other.ESI;
    this->SBN = other.SBN;
    this->fileId = other.fileId;
    this->opcode = other.opcode;
    this->numSymbolsToGet = other.numSymbolsToGet;
    this->isClient = other.isClient;
    this->numSymbolsToSend = other.numSymbolsToSend;
    this->isReader = other.isReader;
    this->isWriter = other.isWriter;
    this->expectedReplyLength = other.expectedReplyLength;
    this->replyDelay = other.replyDelay;
    this->serverClose = other.serverClose;
}

void GenericAppMsgRq::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->ESI);
    doParsimPacking(b,this->SBN);
    doParsimPacking(b,this->fileId);
    doParsimPacking(b,this->opcode);
    doParsimPacking(b,this->numSymbolsToGet);
    doParsimPacking(b,this->isClient);
    doParsimPacking(b,this->numSymbolsToSend);
    doParsimPacking(b,this->isReader);
    doParsimPacking(b,this->isWriter);
    doParsimPacking(b,this->expectedReplyLength);
    doParsimPacking(b,this->replyDelay);
    doParsimPacking(b,this->serverClose);
}

void GenericAppMsgRq::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->ESI);
    doParsimUnpacking(b,this->SBN);
    doParsimUnpacking(b,this->fileId);
    doParsimUnpacking(b,this->opcode);
    doParsimUnpacking(b,this->numSymbolsToGet);
    doParsimUnpacking(b,this->isClient);
    doParsimUnpacking(b,this->numSymbolsToSend);
    doParsimUnpacking(b,this->isReader);
    doParsimUnpacking(b,this->isWriter);
    doParsimUnpacking(b,this->expectedReplyLength);
    doParsimUnpacking(b,this->replyDelay);
    doParsimUnpacking(b,this->serverClose);
}

int GenericAppMsgRq::getESI() const
{
    return this->ESI;
}

void GenericAppMsgRq::setESI(int ESI)
{
    this->ESI = ESI;
}

int GenericAppMsgRq::getSBN() const
{
    return this->SBN;
}

void GenericAppMsgRq::setSBN(int SBN)
{
    this->SBN = SBN;
}

int GenericAppMsgRq::getFileId() const
{
    return this->fileId;
}

void GenericAppMsgRq::setFileId(int fileId)
{
    this->fileId = fileId;
}

int GenericAppMsgRq::getOpcode() const
{
    return this->opcode;
}

void GenericAppMsgRq::setOpcode(int opcode)
{
    this->opcode = opcode;
}

int GenericAppMsgRq::getNumSymbolsToGet() const
{
    return this->numSymbolsToGet;
}

void GenericAppMsgRq::setNumSymbolsToGet(int numSymbolsToGet)
{
    this->numSymbolsToGet = numSymbolsToGet;
}

bool GenericAppMsgRq::getIsClient() const
{
    return this->isClient;
}

void GenericAppMsgRq::setIsClient(bool isClient)
{
    this->isClient = isClient;
}

int GenericAppMsgRq::getNumSymbolsToSend() const
{
    return this->numSymbolsToSend;
}

void GenericAppMsgRq::setNumSymbolsToSend(int numSymbolsToSend)
{
    this->numSymbolsToSend = numSymbolsToSend;
}

bool GenericAppMsgRq::getIsReader() const
{
    return this->isReader;
}

void GenericAppMsgRq::setIsReader(bool isReader)
{
    this->isReader = isReader;
}

bool GenericAppMsgRq::getIsWriter() const
{
    return this->isWriter;
}

void GenericAppMsgRq::setIsWriter(bool isWriter)
{
    this->isWriter = isWriter;
}

int GenericAppMsgRq::getExpectedReplyLength() const
{
    return this->expectedReplyLength;
}

void GenericAppMsgRq::setExpectedReplyLength(int expectedReplyLength)
{
    this->expectedReplyLength = expectedReplyLength;
}

double GenericAppMsgRq::getReplyDelay() const
{
    return this->replyDelay;
}

void GenericAppMsgRq::setReplyDelay(double replyDelay)
{
    this->replyDelay = replyDelay;
}

bool GenericAppMsgRq::getServerClose() const
{
    return this->serverClose;
}

void GenericAppMsgRq::setServerClose(bool serverClose)
{
    this->serverClose = serverClose;
}

class GenericAppMsgRqDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    GenericAppMsgRqDescriptor();
    virtual ~GenericAppMsgRqDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(GenericAppMsgRqDescriptor)

GenericAppMsgRqDescriptor::GenericAppMsgRqDescriptor() : omnetpp::cClassDescriptor("inet::GenericAppMsgRq", "omnetpp::cPacket")
{
    propertynames = nullptr;
}

GenericAppMsgRqDescriptor::~GenericAppMsgRqDescriptor()
{
    delete[] propertynames;
}

bool GenericAppMsgRqDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<GenericAppMsgRq *>(obj)!=nullptr;
}

const char **GenericAppMsgRqDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *GenericAppMsgRqDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int GenericAppMsgRqDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 12+basedesc->getFieldCount() : 12;
}

unsigned int GenericAppMsgRqDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<12) ? fieldTypeFlags[field] : 0;
}

const char *GenericAppMsgRqDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "ESI",
        "SBN",
        "fileId",
        "opcode",
        "numSymbolsToGet",
        "isClient",
        "numSymbolsToSend",
        "isReader",
        "isWriter",
        "expectedReplyLength",
        "replyDelay",
        "serverClose",
    };
    return (field>=0 && field<12) ? fieldNames[field] : nullptr;
}

int GenericAppMsgRqDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='E' && strcmp(fieldName, "ESI")==0) return base+0;
    if (fieldName[0]=='S' && strcmp(fieldName, "SBN")==0) return base+1;
    if (fieldName[0]=='f' && strcmp(fieldName, "fileId")==0) return base+2;
    if (fieldName[0]=='o' && strcmp(fieldName, "opcode")==0) return base+3;
    if (fieldName[0]=='n' && strcmp(fieldName, "numSymbolsToGet")==0) return base+4;
    if (fieldName[0]=='i' && strcmp(fieldName, "isClient")==0) return base+5;
    if (fieldName[0]=='n' && strcmp(fieldName, "numSymbolsToSend")==0) return base+6;
    if (fieldName[0]=='i' && strcmp(fieldName, "isReader")==0) return base+7;
    if (fieldName[0]=='i' && strcmp(fieldName, "isWriter")==0) return base+8;
    if (fieldName[0]=='e' && strcmp(fieldName, "expectedReplyLength")==0) return base+9;
    if (fieldName[0]=='r' && strcmp(fieldName, "replyDelay")==0) return base+10;
    if (fieldName[0]=='s' && strcmp(fieldName, "serverClose")==0) return base+11;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *GenericAppMsgRqDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
        "int",
        "int",
        "int",
        "bool",
        "int",
        "bool",
        "bool",
        "int",
        "double",
        "bool",
    };
    return (field>=0 && field<12) ? fieldTypeStrings[field] : nullptr;
}

const char **GenericAppMsgRqDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *GenericAppMsgRqDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int GenericAppMsgRqDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    GenericAppMsgRq *pp = (GenericAppMsgRq *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *GenericAppMsgRqDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    GenericAppMsgRq *pp = (GenericAppMsgRq *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string GenericAppMsgRqDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    GenericAppMsgRq *pp = (GenericAppMsgRq *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getESI());
        case 1: return long2string(pp->getSBN());
        case 2: return long2string(pp->getFileId());
        case 3: return long2string(pp->getOpcode());
        case 4: return long2string(pp->getNumSymbolsToGet());
        case 5: return bool2string(pp->getIsClient());
        case 6: return long2string(pp->getNumSymbolsToSend());
        case 7: return bool2string(pp->getIsReader());
        case 8: return bool2string(pp->getIsWriter());
        case 9: return long2string(pp->getExpectedReplyLength());
        case 10: return double2string(pp->getReplyDelay());
        case 11: return bool2string(pp->getServerClose());
        default: return "";
    }
}

bool GenericAppMsgRqDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    GenericAppMsgRq *pp = (GenericAppMsgRq *)object; (void)pp;
    switch (field) {
        case 0: pp->setESI(string2long(value)); return true;
        case 1: pp->setSBN(string2long(value)); return true;
        case 2: pp->setFileId(string2long(value)); return true;
        case 3: pp->setOpcode(string2long(value)); return true;
        case 4: pp->setNumSymbolsToGet(string2long(value)); return true;
        case 5: pp->setIsClient(string2bool(value)); return true;
        case 6: pp->setNumSymbolsToSend(string2long(value)); return true;
        case 7: pp->setIsReader(string2bool(value)); return true;
        case 8: pp->setIsWriter(string2bool(value)); return true;
        case 9: pp->setExpectedReplyLength(string2long(value)); return true;
        case 10: pp->setReplyDelay(string2double(value)); return true;
        case 11: pp->setServerClose(string2bool(value)); return true;
        default: return false;
    }
}

const char *GenericAppMsgRqDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *GenericAppMsgRqDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    GenericAppMsgRq *pp = (GenericAppMsgRq *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

} // namespace inet

