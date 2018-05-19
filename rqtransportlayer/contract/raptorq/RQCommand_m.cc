//
// Generated file, do not edit! Created by nedtool 5.2 from inet/raptorqimplementation/rqtransportlayer/RaptorQ/queues/../../contract/raptorq/RQCommand.msg.
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
#include "RQCommand_m.h"

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

EXECUTE_ON_STARTUP(
    omnetpp::cEnum *e = omnetpp::cEnum::find("inet::RQCommandCode");
    if (!e) omnetpp::enums.getInstance()->add(e = new omnetpp::cEnum("inet::RQCommandCode"));
    e->insert(RQ_C_OPEN_ACTIVE, "RQ_C_OPEN_ACTIVE");
    e->insert(RQ_C_OPEN_PASSIVE, "RQ_C_OPEN_PASSIVE");
    e->insert(RQ_C_SEND, "RQ_C_SEND");
    e->insert(RQ_C_CLOSE, "RQ_C_CLOSE");
    e->insert(RQ_C_ABORT, "RQ_C_ABORT");
    e->insert(RQ_C_STATUS, "RQ_C_STATUS");
    e->insert(RQ_C_QUEUE_BYTES_LIMIT, "RQ_C_QUEUE_BYTES_LIMIT");
    e->insert(RQ_C_READ, "RQ_C_READ");
)

EXECUTE_ON_STARTUP(
    omnetpp::cEnum *e = omnetpp::cEnum::find("inet::RQStatusInd");
    if (!e) omnetpp::enums.getInstance()->add(e = new omnetpp::cEnum("inet::RQStatusInd"));
    e->insert(RQ_I_DATA, "RQ_I_DATA");
    e->insert(RQ_I_URGENT_DATA, "RQ_I_URGENT_DATA");
    e->insert(RQ_I_ESTABLISHED, "RQ_I_ESTABLISHED");
    e->insert(RQ_I_PEER_CLOSED, "RQ_I_PEER_CLOSED");
    e->insert(RQ_I_CLOSED, "RQ_I_CLOSED");
    e->insert(RQ_I_CONNECTION_REFUSED, "RQ_I_CONNECTION_REFUSED");
    e->insert(RQ_I_CONNECTION_RESET, "RQ_I_CONNECTION_RESET");
    e->insert(RQ_I_TIMED_OUT, "RQ_I_TIMED_OUT");
    e->insert(RQ_I_STATUS, "RQ_I_STATUS");
    e->insert(RQ_I_SEND_MSG, "RQ_I_SEND_MSG");
    e->insert(RQ_I_DATA_NOTIFICATION, "RQ_I_DATA_NOTIFICATION");
)

EXECUTE_ON_STARTUP(
    omnetpp::cEnum *e = omnetpp::cEnum::find("inet::RQErrorCode");
    if (!e) omnetpp::enums.getInstance()->add(e = new omnetpp::cEnum("inet::RQErrorCode"));
)

Register_Class(RQCommand)

RQCommand::RQCommand() : ::omnetpp::cObject()
{
    this->connId = -1;
    this->userId = -1;
    this->forcMultiSrcConnClose = false;
    this->multiSourcingGrpId = -1;
}

RQCommand::RQCommand(const RQCommand& other) : ::omnetpp::cObject(other)
{
    copy(other);
}

RQCommand::~RQCommand()
{
}

RQCommand& RQCommand::operator=(const RQCommand& other)
{
    if (this==&other) return *this;
    ::omnetpp::cObject::operator=(other);
    copy(other);
    return *this;
}

void RQCommand::copy(const RQCommand& other)
{
    this->connId = other.connId;
    this->userId = other.userId;
    this->forcMultiSrcConnClose = other.forcMultiSrcConnClose;
    this->multiSourcingGrpId = other.multiSourcingGrpId;
}

void RQCommand::parsimPack(omnetpp::cCommBuffer *b) const
{
    doParsimPacking(b,this->connId);
    doParsimPacking(b,this->userId);
    doParsimPacking(b,this->forcMultiSrcConnClose);
    doParsimPacking(b,this->multiSourcingGrpId);
}

void RQCommand::parsimUnpack(omnetpp::cCommBuffer *b)
{
    doParsimUnpacking(b,this->connId);
    doParsimUnpacking(b,this->userId);
    doParsimUnpacking(b,this->forcMultiSrcConnClose);
    doParsimUnpacking(b,this->multiSourcingGrpId);
}

int RQCommand::getConnId() const
{
    return this->connId;
}

void RQCommand::setConnId(int connId)
{
    this->connId = connId;
}

int RQCommand::getUserId() const
{
    return this->userId;
}

void RQCommand::setUserId(int userId)
{
    this->userId = userId;
}

bool RQCommand::getForcMultiSrcConnClose() const
{
    return this->forcMultiSrcConnClose;
}

void RQCommand::setForcMultiSrcConnClose(bool forcMultiSrcConnClose)
{
    this->forcMultiSrcConnClose = forcMultiSrcConnClose;
}

int RQCommand::getMultiSourcingGrpId() const
{
    return this->multiSourcingGrpId;
}

void RQCommand::setMultiSourcingGrpId(int multiSourcingGrpId)
{
    this->multiSourcingGrpId = multiSourcingGrpId;
}

class RQCommandDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    RQCommandDescriptor();
    virtual ~RQCommandDescriptor();

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

Register_ClassDescriptor(RQCommandDescriptor)

RQCommandDescriptor::RQCommandDescriptor() : omnetpp::cClassDescriptor("inet::RQCommand", "omnetpp::cObject")
{
    propertynames = nullptr;
}

RQCommandDescriptor::~RQCommandDescriptor()
{
    delete[] propertynames;
}

bool RQCommandDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<RQCommand *>(obj)!=nullptr;
}

const char **RQCommandDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *RQCommandDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int RQCommandDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 4+basedesc->getFieldCount() : 4;
}

unsigned int RQCommandDescriptor::getFieldTypeFlags(int field) const
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
    };
    return (field>=0 && field<4) ? fieldTypeFlags[field] : 0;
}

const char *RQCommandDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "connId",
        "userId",
        "forcMultiSrcConnClose",
        "multiSourcingGrpId",
    };
    return (field>=0 && field<4) ? fieldNames[field] : nullptr;
}

int RQCommandDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='c' && strcmp(fieldName, "connId")==0) return base+0;
    if (fieldName[0]=='u' && strcmp(fieldName, "userId")==0) return base+1;
    if (fieldName[0]=='f' && strcmp(fieldName, "forcMultiSrcConnClose")==0) return base+2;
    if (fieldName[0]=='m' && strcmp(fieldName, "multiSourcingGrpId")==0) return base+3;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *RQCommandDescriptor::getFieldTypeString(int field) const
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
        "bool",
        "int",
    };
    return (field>=0 && field<4) ? fieldTypeStrings[field] : nullptr;
}

const char **RQCommandDescriptor::getFieldPropertyNames(int field) const
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

const char *RQCommandDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int RQCommandDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    RQCommand *pp = (RQCommand *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *RQCommandDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RQCommand *pp = (RQCommand *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string RQCommandDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RQCommand *pp = (RQCommand *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getConnId());
        case 1: return long2string(pp->getUserId());
        case 2: return bool2string(pp->getForcMultiSrcConnClose());
        case 3: return long2string(pp->getMultiSourcingGrpId());
        default: return "";
    }
}

bool RQCommandDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    RQCommand *pp = (RQCommand *)object; (void)pp;
    switch (field) {
        case 0: pp->setConnId(string2long(value)); return true;
        case 1: pp->setUserId(string2long(value)); return true;
        case 2: pp->setForcMultiSrcConnClose(string2bool(value)); return true;
        case 3: pp->setMultiSourcingGrpId(string2long(value)); return true;
        default: return false;
    }
}

const char *RQCommandDescriptor::getFieldStructName(int field) const
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

void *RQCommandDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    RQCommand *pp = (RQCommand *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

Register_Class(RQErrorInfo)

RQErrorInfo::RQErrorInfo() : ::inet::RQCommand()
{
    this->errorCode = 0;
}

RQErrorInfo::RQErrorInfo(const RQErrorInfo& other) : ::inet::RQCommand(other)
{
    copy(other);
}

RQErrorInfo::~RQErrorInfo()
{
}

RQErrorInfo& RQErrorInfo::operator=(const RQErrorInfo& other)
{
    if (this==&other) return *this;
    ::inet::RQCommand::operator=(other);
    copy(other);
    return *this;
}

void RQErrorInfo::copy(const RQErrorInfo& other)
{
    this->errorCode = other.errorCode;
    this->messageText = other.messageText;
}

void RQErrorInfo::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::RQCommand::parsimPack(b);
    doParsimPacking(b,this->errorCode);
    doParsimPacking(b,this->messageText);
}

void RQErrorInfo::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::RQCommand::parsimUnpack(b);
    doParsimUnpacking(b,this->errorCode);
    doParsimUnpacking(b,this->messageText);
}

int RQErrorInfo::getErrorCode() const
{
    return this->errorCode;
}

void RQErrorInfo::setErrorCode(int errorCode)
{
    this->errorCode = errorCode;
}

const char * RQErrorInfo::getMessageText() const
{
    return this->messageText.c_str();
}

void RQErrorInfo::setMessageText(const char * messageText)
{
    this->messageText = messageText;
}

class RQErrorInfoDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    RQErrorInfoDescriptor();
    virtual ~RQErrorInfoDescriptor();

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

Register_ClassDescriptor(RQErrorInfoDescriptor)

RQErrorInfoDescriptor::RQErrorInfoDescriptor() : omnetpp::cClassDescriptor("inet::RQErrorInfo", "inet::RQCommand")
{
    propertynames = nullptr;
}

RQErrorInfoDescriptor::~RQErrorInfoDescriptor()
{
    delete[] propertynames;
}

bool RQErrorInfoDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<RQErrorInfo *>(obj)!=nullptr;
}

const char **RQErrorInfoDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *RQErrorInfoDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int RQErrorInfoDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 2+basedesc->getFieldCount() : 2;
}

unsigned int RQErrorInfoDescriptor::getFieldTypeFlags(int field) const
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
    };
    return (field>=0 && field<2) ? fieldTypeFlags[field] : 0;
}

const char *RQErrorInfoDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "errorCode",
        "messageText",
    };
    return (field>=0 && field<2) ? fieldNames[field] : nullptr;
}

int RQErrorInfoDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='e' && strcmp(fieldName, "errorCode")==0) return base+0;
    if (fieldName[0]=='m' && strcmp(fieldName, "messageText")==0) return base+1;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *RQErrorInfoDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "string",
    };
    return (field>=0 && field<2) ? fieldTypeStrings[field] : nullptr;
}

const char **RQErrorInfoDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 0: {
            static const char *names[] = { "enum",  nullptr };
            return names;
        }
        default: return nullptr;
    }
}

const char *RQErrorInfoDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 0:
            if (!strcmp(propertyname,"enum")) return "inet::RQErrorCode";
            return nullptr;
        default: return nullptr;
    }
}

int RQErrorInfoDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    RQErrorInfo *pp = (RQErrorInfo *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *RQErrorInfoDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RQErrorInfo *pp = (RQErrorInfo *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string RQErrorInfoDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RQErrorInfo *pp = (RQErrorInfo *)object; (void)pp;
    switch (field) {
        case 0: return enum2string(pp->getErrorCode(), "inet::RQErrorCode");
        case 1: return oppstring2string(pp->getMessageText());
        default: return "";
    }
}

bool RQErrorInfoDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    RQErrorInfo *pp = (RQErrorInfo *)object; (void)pp;
    switch (field) {
        case 0: pp->setErrorCode((inet::RQErrorCode)string2enum(value, "inet::RQErrorCode")); return true;
        case 1: pp->setMessageText((value)); return true;
        default: return false;
    }
}

const char *RQErrorInfoDescriptor::getFieldStructName(int field) const
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

void *RQErrorInfoDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    RQErrorInfo *pp = (RQErrorInfo *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

EXECUTE_ON_STARTUP(
    omnetpp::cEnum *e = omnetpp::cEnum::find("inet::RQDataTransferMode");
    if (!e) omnetpp::enums.getInstance()->add(e = new omnetpp::cEnum("inet::RQDataTransferMode"));
    e->insert(RQ_TRANSFER_UNDEFINED, "RQ_TRANSFER_UNDEFINED");
    e->insert(RQ_TRANSFER_BYTECOUNT, "RQ_TRANSFER_BYTECOUNT");
    e->insert(RQ_TRANSFER_OBJECT, "RQ_TRANSFER_OBJECT");
    e->insert(RQ_TRANSFER_BYTESTREAM, "RQ_TRANSFER_BYTESTREAM");
    e->insert(RQ_REQUESTS, "RQ_REQUESTS");
    e->insert(RQ_RECEIVE_SYMBOLS, "RQ_RECEIVE_SYMBOLS");
)

Register_Class(RQOpenCommand)

RQOpenCommand::RQOpenCommand() : ::inet::RQCommand()
{
    this->localPort = -1;
    this->remotePort = -1;
    this->fork = false;
    this->dataTransferMode = 0;
    this->fileIdToGet = 0;
    this->numSymbolsToSend = 0;
    this->isLongFlow = false;
    this->isMulticastConn = false;
    this->multicastGroupIndex = 0;
    this->isSender = false;
    this->isMultiSourcingConn = false;
    this->multiSourcingGroupIndex = 0;
}

RQOpenCommand::RQOpenCommand(const RQOpenCommand& other) : ::inet::RQCommand(other)
{
    copy(other);
}

RQOpenCommand::~RQOpenCommand()
{
}

RQOpenCommand& RQOpenCommand::operator=(const RQOpenCommand& other)
{
    if (this==&other) return *this;
    ::inet::RQCommand::operator=(other);
    copy(other);
    return *this;
}

void RQOpenCommand::copy(const RQOpenCommand& other)
{
    this->localAddr = other.localAddr;
    this->remoteAddr = other.remoteAddr;
    this->localPort = other.localPort;
    this->remotePort = other.remotePort;
    this->fork = other.fork;
    this->dataTransferMode = other.dataTransferMode;
    this->rqAlgorithmClass = other.rqAlgorithmClass;
    this->fileIdToGet = other.fileIdToGet;
    this->numSymbolsToSend = other.numSymbolsToSend;
    this->isLongFlow = other.isLongFlow;
    this->isMulticastConn = other.isMulticastConn;
    this->multicastGroupIndex = other.multicastGroupIndex;
    this->isSender = other.isSender;
    this->isMultiSourcingConn = other.isMultiSourcingConn;
    this->multiSourcingGroupIndex = other.multiSourcingGroupIndex;
}

void RQOpenCommand::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::RQCommand::parsimPack(b);
    doParsimPacking(b,this->localAddr);
    doParsimPacking(b,this->remoteAddr);
    doParsimPacking(b,this->localPort);
    doParsimPacking(b,this->remotePort);
    doParsimPacking(b,this->fork);
    doParsimPacking(b,this->dataTransferMode);
    doParsimPacking(b,this->rqAlgorithmClass);
    doParsimPacking(b,this->fileIdToGet);
    doParsimPacking(b,this->numSymbolsToSend);
    doParsimPacking(b,this->isLongFlow);
    doParsimPacking(b,this->isMulticastConn);
    doParsimPacking(b,this->multicastGroupIndex);
    doParsimPacking(b,this->isSender);
    doParsimPacking(b,this->isMultiSourcingConn);
    doParsimPacking(b,this->multiSourcingGroupIndex);
}

void RQOpenCommand::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::RQCommand::parsimUnpack(b);
    doParsimUnpacking(b,this->localAddr);
    doParsimUnpacking(b,this->remoteAddr);
    doParsimUnpacking(b,this->localPort);
    doParsimUnpacking(b,this->remotePort);
    doParsimUnpacking(b,this->fork);
    doParsimUnpacking(b,this->dataTransferMode);
    doParsimUnpacking(b,this->rqAlgorithmClass);
    doParsimUnpacking(b,this->fileIdToGet);
    doParsimUnpacking(b,this->numSymbolsToSend);
    doParsimUnpacking(b,this->isLongFlow);
    doParsimUnpacking(b,this->isMulticastConn);
    doParsimUnpacking(b,this->multicastGroupIndex);
    doParsimUnpacking(b,this->isSender);
    doParsimUnpacking(b,this->isMultiSourcingConn);
    doParsimUnpacking(b,this->multiSourcingGroupIndex);
}

L3Address& RQOpenCommand::getLocalAddr()
{
    return this->localAddr;
}

void RQOpenCommand::setLocalAddr(const L3Address& localAddr)
{
    this->localAddr = localAddr;
}

L3Address& RQOpenCommand::getRemoteAddr()
{
    return this->remoteAddr;
}

void RQOpenCommand::setRemoteAddr(const L3Address& remoteAddr)
{
    this->remoteAddr = remoteAddr;
}

int RQOpenCommand::getLocalPort() const
{
    return this->localPort;
}

void RQOpenCommand::setLocalPort(int localPort)
{
    this->localPort = localPort;
}

int RQOpenCommand::getRemotePort() const
{
    return this->remotePort;
}

void RQOpenCommand::setRemotePort(int remotePort)
{
    this->remotePort = remotePort;
}

bool RQOpenCommand::getFork() const
{
    return this->fork;
}

void RQOpenCommand::setFork(bool fork)
{
    this->fork = fork;
}

int RQOpenCommand::getDataTransferMode() const
{
    return this->dataTransferMode;
}

void RQOpenCommand::setDataTransferMode(int dataTransferMode)
{
    this->dataTransferMode = dataTransferMode;
}

const char * RQOpenCommand::getRqAlgorithmClass() const
{
    return this->rqAlgorithmClass.c_str();
}

void RQOpenCommand::setRqAlgorithmClass(const char * rqAlgorithmClass)
{
    this->rqAlgorithmClass = rqAlgorithmClass;
}

int RQOpenCommand::getFileIdToGet() const
{
    return this->fileIdToGet;
}

void RQOpenCommand::setFileIdToGet(int fileIdToGet)
{
    this->fileIdToGet = fileIdToGet;
}

unsigned int RQOpenCommand::getNumSymbolsToSend() const
{
    return this->numSymbolsToSend;
}

void RQOpenCommand::setNumSymbolsToSend(unsigned int numSymbolsToSend)
{
    this->numSymbolsToSend = numSymbolsToSend;
}

bool RQOpenCommand::getIsLongFlow() const
{
    return this->isLongFlow;
}

void RQOpenCommand::setIsLongFlow(bool isLongFlow)
{
    this->isLongFlow = isLongFlow;
}

bool RQOpenCommand::getIsMulticastConn() const
{
    return this->isMulticastConn;
}

void RQOpenCommand::setIsMulticastConn(bool isMulticastConn)
{
    this->isMulticastConn = isMulticastConn;
}

unsigned int RQOpenCommand::getMulticastGroupIndex() const
{
    return this->multicastGroupIndex;
}

void RQOpenCommand::setMulticastGroupIndex(unsigned int multicastGroupIndex)
{
    this->multicastGroupIndex = multicastGroupIndex;
}

bool RQOpenCommand::getIsSender() const
{
    return this->isSender;
}

void RQOpenCommand::setIsSender(bool isSender)
{
    this->isSender = isSender;
}

bool RQOpenCommand::getIsMultiSourcingConn() const
{
    return this->isMultiSourcingConn;
}

void RQOpenCommand::setIsMultiSourcingConn(bool isMultiSourcingConn)
{
    this->isMultiSourcingConn = isMultiSourcingConn;
}

unsigned int RQOpenCommand::getMultiSourcingGroupIndex() const
{
    return this->multiSourcingGroupIndex;
}

void RQOpenCommand::setMultiSourcingGroupIndex(unsigned int multiSourcingGroupIndex)
{
    this->multiSourcingGroupIndex = multiSourcingGroupIndex;
}

class RQOpenCommandDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    RQOpenCommandDescriptor();
    virtual ~RQOpenCommandDescriptor();

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

Register_ClassDescriptor(RQOpenCommandDescriptor)

RQOpenCommandDescriptor::RQOpenCommandDescriptor() : omnetpp::cClassDescriptor("inet::RQOpenCommand", "inet::RQCommand")
{
    propertynames = nullptr;
}

RQOpenCommandDescriptor::~RQOpenCommandDescriptor()
{
    delete[] propertynames;
}

bool RQOpenCommandDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<RQOpenCommand *>(obj)!=nullptr;
}

const char **RQOpenCommandDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *RQOpenCommandDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int RQOpenCommandDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 15+basedesc->getFieldCount() : 15;
}

unsigned int RQOpenCommandDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISCOMPOUND,
        FD_ISCOMPOUND,
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
        FD_ISEDITABLE,
    };
    return (field>=0 && field<15) ? fieldTypeFlags[field] : 0;
}

const char *RQOpenCommandDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "localAddr",
        "remoteAddr",
        "localPort",
        "remotePort",
        "fork",
        "dataTransferMode",
        "rqAlgorithmClass",
        "fileIdToGet",
        "numSymbolsToSend",
        "isLongFlow",
        "isMulticastConn",
        "multicastGroupIndex",
        "isSender",
        "isMultiSourcingConn",
        "multiSourcingGroupIndex",
    };
    return (field>=0 && field<15) ? fieldNames[field] : nullptr;
}

int RQOpenCommandDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='l' && strcmp(fieldName, "localAddr")==0) return base+0;
    if (fieldName[0]=='r' && strcmp(fieldName, "remoteAddr")==0) return base+1;
    if (fieldName[0]=='l' && strcmp(fieldName, "localPort")==0) return base+2;
    if (fieldName[0]=='r' && strcmp(fieldName, "remotePort")==0) return base+3;
    if (fieldName[0]=='f' && strcmp(fieldName, "fork")==0) return base+4;
    if (fieldName[0]=='d' && strcmp(fieldName, "dataTransferMode")==0) return base+5;
    if (fieldName[0]=='r' && strcmp(fieldName, "rqAlgorithmClass")==0) return base+6;
    if (fieldName[0]=='f' && strcmp(fieldName, "fileIdToGet")==0) return base+7;
    if (fieldName[0]=='n' && strcmp(fieldName, "numSymbolsToSend")==0) return base+8;
    if (fieldName[0]=='i' && strcmp(fieldName, "isLongFlow")==0) return base+9;
    if (fieldName[0]=='i' && strcmp(fieldName, "isMulticastConn")==0) return base+10;
    if (fieldName[0]=='m' && strcmp(fieldName, "multicastGroupIndex")==0) return base+11;
    if (fieldName[0]=='i' && strcmp(fieldName, "isSender")==0) return base+12;
    if (fieldName[0]=='i' && strcmp(fieldName, "isMultiSourcingConn")==0) return base+13;
    if (fieldName[0]=='m' && strcmp(fieldName, "multiSourcingGroupIndex")==0) return base+14;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *RQOpenCommandDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "L3Address",
        "L3Address",
        "int",
        "int",
        "bool",
        "int",
        "string",
        "int",
        "unsigned int",
        "bool",
        "bool",
        "unsigned int",
        "bool",
        "bool",
        "unsigned int",
    };
    return (field>=0 && field<15) ? fieldTypeStrings[field] : nullptr;
}

const char **RQOpenCommandDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 5: {
            static const char *names[] = { "enum",  nullptr };
            return names;
        }
        default: return nullptr;
    }
}

const char *RQOpenCommandDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 5:
            if (!strcmp(propertyname,"enum")) return "inet::RQDataTransferMode";
            return nullptr;
        default: return nullptr;
    }
}

int RQOpenCommandDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    RQOpenCommand *pp = (RQOpenCommand *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *RQOpenCommandDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RQOpenCommand *pp = (RQOpenCommand *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string RQOpenCommandDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RQOpenCommand *pp = (RQOpenCommand *)object; (void)pp;
    switch (field) {
        case 0: {std::stringstream out; out << pp->getLocalAddr(); return out.str();}
        case 1: {std::stringstream out; out << pp->getRemoteAddr(); return out.str();}
        case 2: return long2string(pp->getLocalPort());
        case 3: return long2string(pp->getRemotePort());
        case 4: return bool2string(pp->getFork());
        case 5: return enum2string(pp->getDataTransferMode(), "inet::RQDataTransferMode");
        case 6: return oppstring2string(pp->getRqAlgorithmClass());
        case 7: return long2string(pp->getFileIdToGet());
        case 8: return ulong2string(pp->getNumSymbolsToSend());
        case 9: return bool2string(pp->getIsLongFlow());
        case 10: return bool2string(pp->getIsMulticastConn());
        case 11: return ulong2string(pp->getMulticastGroupIndex());
        case 12: return bool2string(pp->getIsSender());
        case 13: return bool2string(pp->getIsMultiSourcingConn());
        case 14: return ulong2string(pp->getMultiSourcingGroupIndex());
        default: return "";
    }
}

bool RQOpenCommandDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    RQOpenCommand *pp = (RQOpenCommand *)object; (void)pp;
    switch (field) {
        case 2: pp->setLocalPort(string2long(value)); return true;
        case 3: pp->setRemotePort(string2long(value)); return true;
        case 4: pp->setFork(string2bool(value)); return true;
        case 5: pp->setDataTransferMode((inet::RQDataTransferMode)string2enum(value, "inet::RQDataTransferMode")); return true;
        case 6: pp->setRqAlgorithmClass((value)); return true;
        case 7: pp->setFileIdToGet(string2long(value)); return true;
        case 8: pp->setNumSymbolsToSend(string2ulong(value)); return true;
        case 9: pp->setIsLongFlow(string2bool(value)); return true;
        case 10: pp->setIsMulticastConn(string2bool(value)); return true;
        case 11: pp->setMulticastGroupIndex(string2ulong(value)); return true;
        case 12: pp->setIsSender(string2bool(value)); return true;
        case 13: pp->setIsMultiSourcingConn(string2bool(value)); return true;
        case 14: pp->setMultiSourcingGroupIndex(string2ulong(value)); return true;
        default: return false;
    }
}

const char *RQOpenCommandDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 0: return omnetpp::opp_typename(typeid(L3Address));
        case 1: return omnetpp::opp_typename(typeid(L3Address));
        default: return nullptr;
    };
}

void *RQOpenCommandDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    RQOpenCommand *pp = (RQOpenCommand *)object; (void)pp;
    switch (field) {
        case 0: return (void *)(&pp->getLocalAddr()); break;
        case 1: return (void *)(&pp->getRemoteAddr()); break;
        default: return nullptr;
    }
}

Register_Class(RQSendCommand)

RQSendCommand::RQSendCommand() : ::inet::RQCommand()
{
}

RQSendCommand::RQSendCommand(const RQSendCommand& other) : ::inet::RQCommand(other)
{
    copy(other);
}

RQSendCommand::~RQSendCommand()
{
}

RQSendCommand& RQSendCommand::operator=(const RQSendCommand& other)
{
    if (this==&other) return *this;
    ::inet::RQCommand::operator=(other);
    copy(other);
    return *this;
}

void RQSendCommand::copy(const RQSendCommand& other)
{
}

void RQSendCommand::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::RQCommand::parsimPack(b);
}

void RQSendCommand::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::RQCommand::parsimUnpack(b);
}

class RQSendCommandDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    RQSendCommandDescriptor();
    virtual ~RQSendCommandDescriptor();

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

Register_ClassDescriptor(RQSendCommandDescriptor)

RQSendCommandDescriptor::RQSendCommandDescriptor() : omnetpp::cClassDescriptor("inet::RQSendCommand", "inet::RQCommand")
{
    propertynames = nullptr;
}

RQSendCommandDescriptor::~RQSendCommandDescriptor()
{
    delete[] propertynames;
}

bool RQSendCommandDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<RQSendCommand *>(obj)!=nullptr;
}

const char **RQSendCommandDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *RQSendCommandDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int RQSendCommandDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 0+basedesc->getFieldCount() : 0;
}

unsigned int RQSendCommandDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    return 0;
}

const char *RQSendCommandDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

int RQSendCommandDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *RQSendCommandDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

const char **RQSendCommandDescriptor::getFieldPropertyNames(int field) const
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

const char *RQSendCommandDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int RQSendCommandDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    RQSendCommand *pp = (RQSendCommand *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *RQSendCommandDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RQSendCommand *pp = (RQSendCommand *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string RQSendCommandDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RQSendCommand *pp = (RQSendCommand *)object; (void)pp;
    switch (field) {
        default: return "";
    }
}

bool RQSendCommandDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    RQSendCommand *pp = (RQSendCommand *)object; (void)pp;
    switch (field) {
        default: return false;
    }
}

const char *RQSendCommandDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

void *RQSendCommandDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    RQSendCommand *pp = (RQSendCommand *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

Register_Class(RQConnectInfo)

RQConnectInfo::RQConnectInfo() : ::inet::RQCommand()
{
    this->localPort = 0;
    this->remotePort = 0;
}

RQConnectInfo::RQConnectInfo(const RQConnectInfo& other) : ::inet::RQCommand(other)
{
    copy(other);
}

RQConnectInfo::~RQConnectInfo()
{
}

RQConnectInfo& RQConnectInfo::operator=(const RQConnectInfo& other)
{
    if (this==&other) return *this;
    ::inet::RQCommand::operator=(other);
    copy(other);
    return *this;
}

void RQConnectInfo::copy(const RQConnectInfo& other)
{
    this->localAddr = other.localAddr;
    this->remoteAddr = other.remoteAddr;
    this->localPort = other.localPort;
    this->remotePort = other.remotePort;
}

void RQConnectInfo::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::RQCommand::parsimPack(b);
    doParsimPacking(b,this->localAddr);
    doParsimPacking(b,this->remoteAddr);
    doParsimPacking(b,this->localPort);
    doParsimPacking(b,this->remotePort);
}

void RQConnectInfo::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::RQCommand::parsimUnpack(b);
    doParsimUnpacking(b,this->localAddr);
    doParsimUnpacking(b,this->remoteAddr);
    doParsimUnpacking(b,this->localPort);
    doParsimUnpacking(b,this->remotePort);
}

L3Address& RQConnectInfo::getLocalAddr()
{
    return this->localAddr;
}

void RQConnectInfo::setLocalAddr(const L3Address& localAddr)
{
    this->localAddr = localAddr;
}

L3Address& RQConnectInfo::getRemoteAddr()
{
    return this->remoteAddr;
}

void RQConnectInfo::setRemoteAddr(const L3Address& remoteAddr)
{
    this->remoteAddr = remoteAddr;
}

int RQConnectInfo::getLocalPort() const
{
    return this->localPort;
}

void RQConnectInfo::setLocalPort(int localPort)
{
    this->localPort = localPort;
}

int RQConnectInfo::getRemotePort() const
{
    return this->remotePort;
}

void RQConnectInfo::setRemotePort(int remotePort)
{
    this->remotePort = remotePort;
}

class RQConnectInfoDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    RQConnectInfoDescriptor();
    virtual ~RQConnectInfoDescriptor();

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

Register_ClassDescriptor(RQConnectInfoDescriptor)

RQConnectInfoDescriptor::RQConnectInfoDescriptor() : omnetpp::cClassDescriptor("inet::RQConnectInfo", "inet::RQCommand")
{
    propertynames = nullptr;
}

RQConnectInfoDescriptor::~RQConnectInfoDescriptor()
{
    delete[] propertynames;
}

bool RQConnectInfoDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<RQConnectInfo *>(obj)!=nullptr;
}

const char **RQConnectInfoDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *RQConnectInfoDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int RQConnectInfoDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 4+basedesc->getFieldCount() : 4;
}

unsigned int RQConnectInfoDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISCOMPOUND,
        FD_ISCOMPOUND,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<4) ? fieldTypeFlags[field] : 0;
}

const char *RQConnectInfoDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "localAddr",
        "remoteAddr",
        "localPort",
        "remotePort",
    };
    return (field>=0 && field<4) ? fieldNames[field] : nullptr;
}

int RQConnectInfoDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='l' && strcmp(fieldName, "localAddr")==0) return base+0;
    if (fieldName[0]=='r' && strcmp(fieldName, "remoteAddr")==0) return base+1;
    if (fieldName[0]=='l' && strcmp(fieldName, "localPort")==0) return base+2;
    if (fieldName[0]=='r' && strcmp(fieldName, "remotePort")==0) return base+3;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *RQConnectInfoDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "L3Address",
        "L3Address",
        "int",
        "int",
    };
    return (field>=0 && field<4) ? fieldTypeStrings[field] : nullptr;
}

const char **RQConnectInfoDescriptor::getFieldPropertyNames(int field) const
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

const char *RQConnectInfoDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int RQConnectInfoDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    RQConnectInfo *pp = (RQConnectInfo *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *RQConnectInfoDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RQConnectInfo *pp = (RQConnectInfo *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string RQConnectInfoDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RQConnectInfo *pp = (RQConnectInfo *)object; (void)pp;
    switch (field) {
        case 0: {std::stringstream out; out << pp->getLocalAddr(); return out.str();}
        case 1: {std::stringstream out; out << pp->getRemoteAddr(); return out.str();}
        case 2: return long2string(pp->getLocalPort());
        case 3: return long2string(pp->getRemotePort());
        default: return "";
    }
}

bool RQConnectInfoDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    RQConnectInfo *pp = (RQConnectInfo *)object; (void)pp;
    switch (field) {
        case 2: pp->setLocalPort(string2long(value)); return true;
        case 3: pp->setRemotePort(string2long(value)); return true;
        default: return false;
    }
}

const char *RQConnectInfoDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 0: return omnetpp::opp_typename(typeid(L3Address));
        case 1: return omnetpp::opp_typename(typeid(L3Address));
        default: return nullptr;
    };
}

void *RQConnectInfoDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    RQConnectInfo *pp = (RQConnectInfo *)object; (void)pp;
    switch (field) {
        case 0: return (void *)(&pp->getLocalAddr()); break;
        case 1: return (void *)(&pp->getRemoteAddr()); break;
        default: return nullptr;
    }
}

Register_Class(RQStatusInfo)

RQStatusInfo::RQStatusInfo() : ::inet::RQCommand()
{
    this->state = 0;
    this->localPort = 0;
    this->remotePort = 0;
    this->snd_mss = 0;
    this->snd_una = 0;
    this->snd_nxt = 0;
    this->snd_max = 0;
    this->snd_wnd = 0;
    this->snd_up = 0;
    this->snd_wl1 = 0;
    this->snd_wl2 = 0;
    this->iss = 0;
    this->rcv_nxt = 0;
    this->rcv_wnd = 0;
    this->rcv_up = 0;
    this->irs = 0;
    this->fin_ack_rcvd = false;
}

RQStatusInfo::RQStatusInfo(const RQStatusInfo& other) : ::inet::RQCommand(other)
{
    copy(other);
}

RQStatusInfo::~RQStatusInfo()
{
}

RQStatusInfo& RQStatusInfo::operator=(const RQStatusInfo& other)
{
    if (this==&other) return *this;
    ::inet::RQCommand::operator=(other);
    copy(other);
    return *this;
}

void RQStatusInfo::copy(const RQStatusInfo& other)
{
    this->state = other.state;
    this->stateName = other.stateName;
    this->localAddr = other.localAddr;
    this->remoteAddr = other.remoteAddr;
    this->localPort = other.localPort;
    this->remotePort = other.remotePort;
    this->snd_mss = other.snd_mss;
    this->snd_una = other.snd_una;
    this->snd_nxt = other.snd_nxt;
    this->snd_max = other.snd_max;
    this->snd_wnd = other.snd_wnd;
    this->snd_up = other.snd_up;
    this->snd_wl1 = other.snd_wl1;
    this->snd_wl2 = other.snd_wl2;
    this->iss = other.iss;
    this->rcv_nxt = other.rcv_nxt;
    this->rcv_wnd = other.rcv_wnd;
    this->rcv_up = other.rcv_up;
    this->irs = other.irs;
    this->fin_ack_rcvd = other.fin_ack_rcvd;
}

void RQStatusInfo::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::RQCommand::parsimPack(b);
    doParsimPacking(b,this->state);
    doParsimPacking(b,this->stateName);
    doParsimPacking(b,this->localAddr);
    doParsimPacking(b,this->remoteAddr);
    doParsimPacking(b,this->localPort);
    doParsimPacking(b,this->remotePort);
    doParsimPacking(b,this->snd_mss);
    doParsimPacking(b,this->snd_una);
    doParsimPacking(b,this->snd_nxt);
    doParsimPacking(b,this->snd_max);
    doParsimPacking(b,this->snd_wnd);
    doParsimPacking(b,this->snd_up);
    doParsimPacking(b,this->snd_wl1);
    doParsimPacking(b,this->snd_wl2);
    doParsimPacking(b,this->iss);
    doParsimPacking(b,this->rcv_nxt);
    doParsimPacking(b,this->rcv_wnd);
    doParsimPacking(b,this->rcv_up);
    doParsimPacking(b,this->irs);
    doParsimPacking(b,this->fin_ack_rcvd);
}

void RQStatusInfo::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::RQCommand::parsimUnpack(b);
    doParsimUnpacking(b,this->state);
    doParsimUnpacking(b,this->stateName);
    doParsimUnpacking(b,this->localAddr);
    doParsimUnpacking(b,this->remoteAddr);
    doParsimUnpacking(b,this->localPort);
    doParsimUnpacking(b,this->remotePort);
    doParsimUnpacking(b,this->snd_mss);
    doParsimUnpacking(b,this->snd_una);
    doParsimUnpacking(b,this->snd_nxt);
    doParsimUnpacking(b,this->snd_max);
    doParsimUnpacking(b,this->snd_wnd);
    doParsimUnpacking(b,this->snd_up);
    doParsimUnpacking(b,this->snd_wl1);
    doParsimUnpacking(b,this->snd_wl2);
    doParsimUnpacking(b,this->iss);
    doParsimUnpacking(b,this->rcv_nxt);
    doParsimUnpacking(b,this->rcv_wnd);
    doParsimUnpacking(b,this->rcv_up);
    doParsimUnpacking(b,this->irs);
    doParsimUnpacking(b,this->fin_ack_rcvd);
}

int RQStatusInfo::getState() const
{
    return this->state;
}

void RQStatusInfo::setState(int state)
{
    this->state = state;
}

const char * RQStatusInfo::getStateName() const
{
    return this->stateName.c_str();
}

void RQStatusInfo::setStateName(const char * stateName)
{
    this->stateName = stateName;
}

L3Address& RQStatusInfo::getLocalAddr()
{
    return this->localAddr;
}

void RQStatusInfo::setLocalAddr(const L3Address& localAddr)
{
    this->localAddr = localAddr;
}

L3Address& RQStatusInfo::getRemoteAddr()
{
    return this->remoteAddr;
}

void RQStatusInfo::setRemoteAddr(const L3Address& remoteAddr)
{
    this->remoteAddr = remoteAddr;
}

int RQStatusInfo::getLocalPort() const
{
    return this->localPort;
}

void RQStatusInfo::setLocalPort(int localPort)
{
    this->localPort = localPort;
}

int RQStatusInfo::getRemotePort() const
{
    return this->remotePort;
}

void RQStatusInfo::setRemotePort(int remotePort)
{
    this->remotePort = remotePort;
}

unsigned int RQStatusInfo::getSnd_mss() const
{
    return this->snd_mss;
}

void RQStatusInfo::setSnd_mss(unsigned int snd_mss)
{
    this->snd_mss = snd_mss;
}

unsigned int RQStatusInfo::getSnd_una() const
{
    return this->snd_una;
}

void RQStatusInfo::setSnd_una(unsigned int snd_una)
{
    this->snd_una = snd_una;
}

unsigned int RQStatusInfo::getSnd_nxt() const
{
    return this->snd_nxt;
}

void RQStatusInfo::setSnd_nxt(unsigned int snd_nxt)
{
    this->snd_nxt = snd_nxt;
}

unsigned int RQStatusInfo::getSnd_max() const
{
    return this->snd_max;
}

void RQStatusInfo::setSnd_max(unsigned int snd_max)
{
    this->snd_max = snd_max;
}

unsigned int RQStatusInfo::getSnd_wnd() const
{
    return this->snd_wnd;
}

void RQStatusInfo::setSnd_wnd(unsigned int snd_wnd)
{
    this->snd_wnd = snd_wnd;
}

unsigned int RQStatusInfo::getSnd_up() const
{
    return this->snd_up;
}

void RQStatusInfo::setSnd_up(unsigned int snd_up)
{
    this->snd_up = snd_up;
}

unsigned int RQStatusInfo::getSnd_wl1() const
{
    return this->snd_wl1;
}

void RQStatusInfo::setSnd_wl1(unsigned int snd_wl1)
{
    this->snd_wl1 = snd_wl1;
}

unsigned int RQStatusInfo::getSnd_wl2() const
{
    return this->snd_wl2;
}

void RQStatusInfo::setSnd_wl2(unsigned int snd_wl2)
{
    this->snd_wl2 = snd_wl2;
}

unsigned int RQStatusInfo::getIss() const
{
    return this->iss;
}

void RQStatusInfo::setIss(unsigned int iss)
{
    this->iss = iss;
}

unsigned int RQStatusInfo::getRcv_nxt() const
{
    return this->rcv_nxt;
}

void RQStatusInfo::setRcv_nxt(unsigned int rcv_nxt)
{
    this->rcv_nxt = rcv_nxt;
}

unsigned int RQStatusInfo::getRcv_wnd() const
{
    return this->rcv_wnd;
}

void RQStatusInfo::setRcv_wnd(unsigned int rcv_wnd)
{
    this->rcv_wnd = rcv_wnd;
}

unsigned int RQStatusInfo::getRcv_up() const
{
    return this->rcv_up;
}

void RQStatusInfo::setRcv_up(unsigned int rcv_up)
{
    this->rcv_up = rcv_up;
}

unsigned int RQStatusInfo::getIrs() const
{
    return this->irs;
}

void RQStatusInfo::setIrs(unsigned int irs)
{
    this->irs = irs;
}

bool RQStatusInfo::getFin_ack_rcvd() const
{
    return this->fin_ack_rcvd;
}

void RQStatusInfo::setFin_ack_rcvd(bool fin_ack_rcvd)
{
    this->fin_ack_rcvd = fin_ack_rcvd;
}

class RQStatusInfoDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    RQStatusInfoDescriptor();
    virtual ~RQStatusInfoDescriptor();

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

Register_ClassDescriptor(RQStatusInfoDescriptor)

RQStatusInfoDescriptor::RQStatusInfoDescriptor() : omnetpp::cClassDescriptor("inet::RQStatusInfo", "inet::RQCommand")
{
    propertynames = nullptr;
}

RQStatusInfoDescriptor::~RQStatusInfoDescriptor()
{
    delete[] propertynames;
}

bool RQStatusInfoDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<RQStatusInfo *>(obj)!=nullptr;
}

const char **RQStatusInfoDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *RQStatusInfoDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int RQStatusInfoDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 20+basedesc->getFieldCount() : 20;
}

unsigned int RQStatusInfoDescriptor::getFieldTypeFlags(int field) const
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
        FD_ISCOMPOUND,
        FD_ISCOMPOUND,
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
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<20) ? fieldTypeFlags[field] : 0;
}

const char *RQStatusInfoDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "state",
        "stateName",
        "localAddr",
        "remoteAddr",
        "localPort",
        "remotePort",
        "snd_mss",
        "snd_una",
        "snd_nxt",
        "snd_max",
        "snd_wnd",
        "snd_up",
        "snd_wl1",
        "snd_wl2",
        "iss",
        "rcv_nxt",
        "rcv_wnd",
        "rcv_up",
        "irs",
        "fin_ack_rcvd",
    };
    return (field>=0 && field<20) ? fieldNames[field] : nullptr;
}

int RQStatusInfoDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='s' && strcmp(fieldName, "state")==0) return base+0;
    if (fieldName[0]=='s' && strcmp(fieldName, "stateName")==0) return base+1;
    if (fieldName[0]=='l' && strcmp(fieldName, "localAddr")==0) return base+2;
    if (fieldName[0]=='r' && strcmp(fieldName, "remoteAddr")==0) return base+3;
    if (fieldName[0]=='l' && strcmp(fieldName, "localPort")==0) return base+4;
    if (fieldName[0]=='r' && strcmp(fieldName, "remotePort")==0) return base+5;
    if (fieldName[0]=='s' && strcmp(fieldName, "snd_mss")==0) return base+6;
    if (fieldName[0]=='s' && strcmp(fieldName, "snd_una")==0) return base+7;
    if (fieldName[0]=='s' && strcmp(fieldName, "snd_nxt")==0) return base+8;
    if (fieldName[0]=='s' && strcmp(fieldName, "snd_max")==0) return base+9;
    if (fieldName[0]=='s' && strcmp(fieldName, "snd_wnd")==0) return base+10;
    if (fieldName[0]=='s' && strcmp(fieldName, "snd_up")==0) return base+11;
    if (fieldName[0]=='s' && strcmp(fieldName, "snd_wl1")==0) return base+12;
    if (fieldName[0]=='s' && strcmp(fieldName, "snd_wl2")==0) return base+13;
    if (fieldName[0]=='i' && strcmp(fieldName, "iss")==0) return base+14;
    if (fieldName[0]=='r' && strcmp(fieldName, "rcv_nxt")==0) return base+15;
    if (fieldName[0]=='r' && strcmp(fieldName, "rcv_wnd")==0) return base+16;
    if (fieldName[0]=='r' && strcmp(fieldName, "rcv_up")==0) return base+17;
    if (fieldName[0]=='i' && strcmp(fieldName, "irs")==0) return base+18;
    if (fieldName[0]=='f' && strcmp(fieldName, "fin_ack_rcvd")==0) return base+19;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *RQStatusInfoDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "string",
        "L3Address",
        "L3Address",
        "int",
        "int",
        "unsigned int",
        "unsigned int",
        "unsigned int",
        "unsigned int",
        "unsigned int",
        "unsigned int",
        "unsigned int",
        "unsigned int",
        "unsigned int",
        "unsigned int",
        "unsigned int",
        "unsigned int",
        "unsigned int",
        "bool",
    };
    return (field>=0 && field<20) ? fieldTypeStrings[field] : nullptr;
}

const char **RQStatusInfoDescriptor::getFieldPropertyNames(int field) const
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

const char *RQStatusInfoDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int RQStatusInfoDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    RQStatusInfo *pp = (RQStatusInfo *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *RQStatusInfoDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RQStatusInfo *pp = (RQStatusInfo *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string RQStatusInfoDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RQStatusInfo *pp = (RQStatusInfo *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getState());
        case 1: return oppstring2string(pp->getStateName());
        case 2: {std::stringstream out; out << pp->getLocalAddr(); return out.str();}
        case 3: {std::stringstream out; out << pp->getRemoteAddr(); return out.str();}
        case 4: return long2string(pp->getLocalPort());
        case 5: return long2string(pp->getRemotePort());
        case 6: return ulong2string(pp->getSnd_mss());
        case 7: return ulong2string(pp->getSnd_una());
        case 8: return ulong2string(pp->getSnd_nxt());
        case 9: return ulong2string(pp->getSnd_max());
        case 10: return ulong2string(pp->getSnd_wnd());
        case 11: return ulong2string(pp->getSnd_up());
        case 12: return ulong2string(pp->getSnd_wl1());
        case 13: return ulong2string(pp->getSnd_wl2());
        case 14: return ulong2string(pp->getIss());
        case 15: return ulong2string(pp->getRcv_nxt());
        case 16: return ulong2string(pp->getRcv_wnd());
        case 17: return ulong2string(pp->getRcv_up());
        case 18: return ulong2string(pp->getIrs());
        case 19: return bool2string(pp->getFin_ack_rcvd());
        default: return "";
    }
}

bool RQStatusInfoDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    RQStatusInfo *pp = (RQStatusInfo *)object; (void)pp;
    switch (field) {
        case 0: pp->setState(string2long(value)); return true;
        case 1: pp->setStateName((value)); return true;
        case 4: pp->setLocalPort(string2long(value)); return true;
        case 5: pp->setRemotePort(string2long(value)); return true;
        case 6: pp->setSnd_mss(string2ulong(value)); return true;
        case 7: pp->setSnd_una(string2ulong(value)); return true;
        case 8: pp->setSnd_nxt(string2ulong(value)); return true;
        case 9: pp->setSnd_max(string2ulong(value)); return true;
        case 10: pp->setSnd_wnd(string2ulong(value)); return true;
        case 11: pp->setSnd_up(string2ulong(value)); return true;
        case 12: pp->setSnd_wl1(string2ulong(value)); return true;
        case 13: pp->setSnd_wl2(string2ulong(value)); return true;
        case 14: pp->setIss(string2ulong(value)); return true;
        case 15: pp->setRcv_nxt(string2ulong(value)); return true;
        case 16: pp->setRcv_wnd(string2ulong(value)); return true;
        case 17: pp->setRcv_up(string2ulong(value)); return true;
        case 18: pp->setIrs(string2ulong(value)); return true;
        case 19: pp->setFin_ack_rcvd(string2bool(value)); return true;
        default: return false;
    }
}

const char *RQStatusInfoDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 2: return omnetpp::opp_typename(typeid(L3Address));
        case 3: return omnetpp::opp_typename(typeid(L3Address));
        default: return nullptr;
    };
}

void *RQStatusInfoDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    RQStatusInfo *pp = (RQStatusInfo *)object; (void)pp;
    switch (field) {
        case 2: return (void *)(&pp->getLocalAddr()); break;
        case 3: return (void *)(&pp->getRemoteAddr()); break;
        default: return nullptr;
    }
}

} // namespace inet

