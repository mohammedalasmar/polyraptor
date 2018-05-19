//
// Generated file, do not edit! Created by nedtool 5.2 from inet/raptorqimplementation/rqtransportlayer/RaptorQ/raptorq_common/RQSegment.msg.
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
#include "RQSegment_m.h"

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
namespace raptorq {

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

RQPayloadMessage::RQPayloadMessage()
{
    this->endSequenceNo = 0;
    this->msg = nullptr;
}

void __doPacking(omnetpp::cCommBuffer *b, const RQPayloadMessage& a)
{
    doParsimPacking(b,a.endSequenceNo);
    doParsimPacking(b,a.msg);
}

void __doUnpacking(omnetpp::cCommBuffer *b, RQPayloadMessage& a)
{
    doParsimUnpacking(b,a.endSequenceNo);
    doParsimUnpacking(b,a.msg);
}

class RQPayloadMessageDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    RQPayloadMessageDescriptor();
    virtual ~RQPayloadMessageDescriptor();

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

Register_ClassDescriptor(RQPayloadMessageDescriptor)

RQPayloadMessageDescriptor::RQPayloadMessageDescriptor() : omnetpp::cClassDescriptor("inet::raptorq::RQPayloadMessage", "")
{
    propertynames = nullptr;
}

RQPayloadMessageDescriptor::~RQPayloadMessageDescriptor()
{
    delete[] propertynames;
}

bool RQPayloadMessageDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<RQPayloadMessage *>(obj)!=nullptr;
}

const char **RQPayloadMessageDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *RQPayloadMessageDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int RQPayloadMessageDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 2+basedesc->getFieldCount() : 2;
}

unsigned int RQPayloadMessageDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISCOMPOUND,
    };
    return (field>=0 && field<2) ? fieldTypeFlags[field] : 0;
}

const char *RQPayloadMessageDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "endSequenceNo",
        "msg",
    };
    return (field>=0 && field<2) ? fieldNames[field] : nullptr;
}

int RQPayloadMessageDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='e' && strcmp(fieldName, "endSequenceNo")==0) return base+0;
    if (fieldName[0]=='m' && strcmp(fieldName, "msg")==0) return base+1;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *RQPayloadMessageDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "unsigned int",
        "cPacketPtr",
    };
    return (field>=0 && field<2) ? fieldTypeStrings[field] : nullptr;
}

const char **RQPayloadMessageDescriptor::getFieldPropertyNames(int field) const
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

const char *RQPayloadMessageDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int RQPayloadMessageDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    RQPayloadMessage *pp = (RQPayloadMessage *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *RQPayloadMessageDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RQPayloadMessage *pp = (RQPayloadMessage *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string RQPayloadMessageDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RQPayloadMessage *pp = (RQPayloadMessage *)object; (void)pp;
    switch (field) {
        case 0: return ulong2string(pp->endSequenceNo);
        case 1: {std::stringstream out; out << pp->msg; return out.str();}
        default: return "";
    }
}

bool RQPayloadMessageDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    RQPayloadMessage *pp = (RQPayloadMessage *)object; (void)pp;
    switch (field) {
        case 0: pp->endSequenceNo = string2ulong(value); return true;
        default: return false;
    }
}

const char *RQPayloadMessageDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 1: return omnetpp::opp_typename(typeid(cPacketPtr));
        default: return nullptr;
    };
}

void *RQPayloadMessageDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    RQPayloadMessage *pp = (RQPayloadMessage *)object; (void)pp;
    switch (field) {
        case 1: return (void *)(&pp->msg); break;
        default: return nullptr;
    }
}

EXECUTE_ON_STARTUP(
    omnetpp::cEnum *e = omnetpp::cEnum::find("inet::raptorq::RQOptionNumbers");
    if (!e) omnetpp::enums.getInstance()->add(e = new omnetpp::cEnum("inet::raptorq::RQOptionNumbers"));
    e->insert(RQOPTION_END_OF_OPTION_LIST, "RQOPTION_END_OF_OPTION_LIST");
    e->insert(RQOPTION_NO_OPERATION, "RQOPTION_NO_OPERATION");
    e->insert(RQOPTION_MAXIMUM_SEGMENT_SIZE, "RQOPTION_MAXIMUM_SEGMENT_SIZE");
    e->insert(RQOPTION_WINDOW_SCALE, "RQOPTION_WINDOW_SCALE");
    e->insert(RQOPTION_SACK_PERMITTED, "RQOPTION_SACK_PERMITTED");
    e->insert(RQOPTION_SACK, "RQOPTION_SACK");
    e->insert(RQOPTION_TIMESTAMP, "RQOPTION_TIMESTAMP");
)

Register_Class(SackItem)

SackItem::SackItem() : ::omnetpp::cObject()
{
    this->start_var = 0;
    this->end_var = 0;
}

SackItem::SackItem(const SackItem& other) : ::omnetpp::cObject(other)
{
    copy(other);
}

SackItem::~SackItem()
{
}

SackItem& SackItem::operator=(const SackItem& other)
{
    if (this==&other) return *this;
    ::omnetpp::cObject::operator=(other);
    copy(other);
    return *this;
}

void SackItem::copy(const SackItem& other)
{
    this->start_var = other.start_var;
    this->end_var = other.end_var;
}

void SackItem::parsimPack(omnetpp::cCommBuffer *b) const
{
    doParsimPacking(b,this->start_var);
    doParsimPacking(b,this->end_var);
}

void SackItem::parsimUnpack(omnetpp::cCommBuffer *b)
{
    doParsimUnpacking(b,this->start_var);
    doParsimUnpacking(b,this->end_var);
}

unsigned int SackItem::getStart() const
{
    return this->start_var;
}

void SackItem::setStart(unsigned int start)
{
    this->start_var = start;
}

unsigned int SackItem::getEnd() const
{
    return this->end_var;
}

void SackItem::setEnd(unsigned int end)
{
    this->end_var = end;
}

class SackItemDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    SackItemDescriptor();
    virtual ~SackItemDescriptor();

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

Register_ClassDescriptor(SackItemDescriptor)

SackItemDescriptor::SackItemDescriptor() : omnetpp::cClassDescriptor("inet::raptorq::SackItem", "omnetpp::cObject")
{
    propertynames = nullptr;
}

SackItemDescriptor::~SackItemDescriptor()
{
    delete[] propertynames;
}

bool SackItemDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<SackItem *>(obj)!=nullptr;
}

const char **SackItemDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = { "fieldNameSuffix",  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *SackItemDescriptor::getProperty(const char *propertyname) const
{
    if (!strcmp(propertyname,"fieldNameSuffix")) return "_var";
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int SackItemDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 2+basedesc->getFieldCount() : 2;
}

unsigned int SackItemDescriptor::getFieldTypeFlags(int field) const
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

const char *SackItemDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "start",
        "end",
    };
    return (field>=0 && field<2) ? fieldNames[field] : nullptr;
}

int SackItemDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='s' && strcmp(fieldName, "start")==0) return base+0;
    if (fieldName[0]=='e' && strcmp(fieldName, "end")==0) return base+1;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *SackItemDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "unsigned int",
        "unsigned int",
    };
    return (field>=0 && field<2) ? fieldTypeStrings[field] : nullptr;
}

const char **SackItemDescriptor::getFieldPropertyNames(int field) const
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

const char *SackItemDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int SackItemDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    SackItem *pp = (SackItem *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *SackItemDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    SackItem *pp = (SackItem *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string SackItemDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    SackItem *pp = (SackItem *)object; (void)pp;
    switch (field) {
        case 0: return ulong2string(pp->getStart());
        case 1: return ulong2string(pp->getEnd());
        default: return "";
    }
}

bool SackItemDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    SackItem *pp = (SackItem *)object; (void)pp;
    switch (field) {
        case 0: pp->setStart(string2ulong(value)); return true;
        case 1: pp->setEnd(string2ulong(value)); return true;
        default: return false;
    }
}

const char *SackItemDescriptor::getFieldStructName(int field) const
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

void *SackItemDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    SackItem *pp = (SackItem *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

Sack_Base::Sack_Base() : ::inet::raptorq::SackItem()
{
}

Sack_Base::Sack_Base(const Sack_Base& other) : ::inet::raptorq::SackItem(other)
{
    copy(other);
}

Sack_Base::~Sack_Base()
{
}

Sack_Base& Sack_Base::operator=(const Sack_Base& other)
{
    if (this==&other) return *this;
    ::inet::raptorq::SackItem::operator=(other);
    copy(other);
    return *this;
}

void Sack_Base::copy(const Sack_Base& other)
{
}

void Sack_Base::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::raptorq::SackItem::parsimPack(b);
}

void Sack_Base::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::raptorq::SackItem::parsimUnpack(b);
}

class SackDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    SackDescriptor();
    virtual ~SackDescriptor();

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

Register_ClassDescriptor(SackDescriptor)

SackDescriptor::SackDescriptor() : omnetpp::cClassDescriptor("inet::raptorq::Sack", "inet::raptorq::SackItem")
{
    propertynames = nullptr;
}

SackDescriptor::~SackDescriptor()
{
    delete[] propertynames;
}

bool SackDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<Sack_Base *>(obj)!=nullptr;
}

const char **SackDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = { "customize",  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *SackDescriptor::getProperty(const char *propertyname) const
{
    if (!strcmp(propertyname,"customize")) return "true";
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int SackDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 0+basedesc->getFieldCount() : 0;
}

unsigned int SackDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    return 0;
}

const char *SackDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

int SackDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *SackDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

const char **SackDescriptor::getFieldPropertyNames(int field) const
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

const char *SackDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int SackDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    Sack_Base *pp = (Sack_Base *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *SackDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    Sack_Base *pp = (Sack_Base *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string SackDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    Sack_Base *pp = (Sack_Base *)object; (void)pp;
    switch (field) {
        default: return "";
    }
}

bool SackDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    Sack_Base *pp = (Sack_Base *)object; (void)pp;
    switch (field) {
        default: return false;
    }
}

const char *SackDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

void *SackDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    Sack_Base *pp = (Sack_Base *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

Register_Class(RQOption)

RQOption::RQOption() : ::omnetpp::cObject()
{
    this->kind = -1;
    this->length = 1;
}

RQOption::RQOption(const RQOption& other) : ::omnetpp::cObject(other)
{
    copy(other);
}

RQOption::~RQOption()
{
}

RQOption& RQOption::operator=(const RQOption& other)
{
    if (this==&other) return *this;
    ::omnetpp::cObject::operator=(other);
    copy(other);
    return *this;
}

void RQOption::copy(const RQOption& other)
{
    this->kind = other.kind;
    this->length = other.length;
}

void RQOption::parsimPack(omnetpp::cCommBuffer *b) const
{
    doParsimPacking(b,this->kind);
    doParsimPacking(b,this->length);
}

void RQOption::parsimUnpack(omnetpp::cCommBuffer *b)
{
    doParsimUnpacking(b,this->kind);
    doParsimUnpacking(b,this->length);
}

unsigned short RQOption::getKind() const
{
    return this->kind;
}

void RQOption::setKind(unsigned short kind)
{
    this->kind = kind;
}

unsigned short RQOption::getLength() const
{
    return this->length;
}

void RQOption::setLength(unsigned short length)
{
    this->length = length;
}

class RQOptionDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    RQOptionDescriptor();
    virtual ~RQOptionDescriptor();

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

Register_ClassDescriptor(RQOptionDescriptor)

RQOptionDescriptor::RQOptionDescriptor() : omnetpp::cClassDescriptor("inet::raptorq::RQOption", "omnetpp::cObject")
{
    propertynames = nullptr;
}

RQOptionDescriptor::~RQOptionDescriptor()
{
    delete[] propertynames;
}

bool RQOptionDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<RQOption *>(obj)!=nullptr;
}

const char **RQOptionDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *RQOptionDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int RQOptionDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 2+basedesc->getFieldCount() : 2;
}

unsigned int RQOptionDescriptor::getFieldTypeFlags(int field) const
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

const char *RQOptionDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "kind",
        "length",
    };
    return (field>=0 && field<2) ? fieldNames[field] : nullptr;
}

int RQOptionDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='k' && strcmp(fieldName, "kind")==0) return base+0;
    if (fieldName[0]=='l' && strcmp(fieldName, "length")==0) return base+1;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *RQOptionDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "unsigned short",
        "unsigned short",
    };
    return (field>=0 && field<2) ? fieldTypeStrings[field] : nullptr;
}

const char **RQOptionDescriptor::getFieldPropertyNames(int field) const
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

const char *RQOptionDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 0:
            if (!strcmp(propertyname,"enum")) return "inet::raptorq::RQOptionNumbers";
            return nullptr;
        default: return nullptr;
    }
}

int RQOptionDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    RQOption *pp = (RQOption *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *RQOptionDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RQOption *pp = (RQOption *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string RQOptionDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RQOption *pp = (RQOption *)object; (void)pp;
    switch (field) {
        case 0: return enum2string(pp->getKind(), "inet::raptorq::RQOptionNumbers");
        case 1: return ulong2string(pp->getLength());
        default: return "";
    }
}

bool RQOptionDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    RQOption *pp = (RQOption *)object; (void)pp;
    switch (field) {
        case 0: pp->setKind((inet::raptorq::RQOptionNumbers)string2enum(value, "inet::raptorq::RQOptionNumbers")); return true;
        case 1: pp->setLength(string2ulong(value)); return true;
        default: return false;
    }
}

const char *RQOptionDescriptor::getFieldStructName(int field) const
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

void *RQOptionDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    RQOption *pp = (RQOption *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

Register_Class(RQOptionEnd)

RQOptionEnd::RQOptionEnd() : ::inet::raptorq::RQOption()
{
    this->setKind(RQOPTION_END_OF_OPTION_LIST);
    this->setLength(1);
}

RQOptionEnd::RQOptionEnd(const RQOptionEnd& other) : ::inet::raptorq::RQOption(other)
{
    copy(other);
}

RQOptionEnd::~RQOptionEnd()
{
}

RQOptionEnd& RQOptionEnd::operator=(const RQOptionEnd& other)
{
    if (this==&other) return *this;
    ::inet::raptorq::RQOption::operator=(other);
    copy(other);
    return *this;
}

void RQOptionEnd::copy(const RQOptionEnd& other)
{
}

void RQOptionEnd::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::raptorq::RQOption::parsimPack(b);
}

void RQOptionEnd::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::raptorq::RQOption::parsimUnpack(b);
}

class RQOptionEndDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    RQOptionEndDescriptor();
    virtual ~RQOptionEndDescriptor();

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

Register_ClassDescriptor(RQOptionEndDescriptor)

RQOptionEndDescriptor::RQOptionEndDescriptor() : omnetpp::cClassDescriptor("inet::raptorq::RQOptionEnd", "inet::raptorq::RQOption")
{
    propertynames = nullptr;
}

RQOptionEndDescriptor::~RQOptionEndDescriptor()
{
    delete[] propertynames;
}

bool RQOptionEndDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<RQOptionEnd *>(obj)!=nullptr;
}

const char **RQOptionEndDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *RQOptionEndDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int RQOptionEndDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 0+basedesc->getFieldCount() : 0;
}

unsigned int RQOptionEndDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    return 0;
}

const char *RQOptionEndDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

int RQOptionEndDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *RQOptionEndDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

const char **RQOptionEndDescriptor::getFieldPropertyNames(int field) const
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

const char *RQOptionEndDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int RQOptionEndDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    RQOptionEnd *pp = (RQOptionEnd *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *RQOptionEndDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RQOptionEnd *pp = (RQOptionEnd *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string RQOptionEndDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RQOptionEnd *pp = (RQOptionEnd *)object; (void)pp;
    switch (field) {
        default: return "";
    }
}

bool RQOptionEndDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    RQOptionEnd *pp = (RQOptionEnd *)object; (void)pp;
    switch (field) {
        default: return false;
    }
}

const char *RQOptionEndDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

void *RQOptionEndDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    RQOptionEnd *pp = (RQOptionEnd *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

Register_Class(RQOptionNop)

RQOptionNop::RQOptionNop() : ::inet::raptorq::RQOption()
{
    this->setKind(RQOPTION_NO_OPERATION);
    this->setLength(1);
}

RQOptionNop::RQOptionNop(const RQOptionNop& other) : ::inet::raptorq::RQOption(other)
{
    copy(other);
}

RQOptionNop::~RQOptionNop()
{
}

RQOptionNop& RQOptionNop::operator=(const RQOptionNop& other)
{
    if (this==&other) return *this;
    ::inet::raptorq::RQOption::operator=(other);
    copy(other);
    return *this;
}

void RQOptionNop::copy(const RQOptionNop& other)
{
}

void RQOptionNop::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::raptorq::RQOption::parsimPack(b);
}

void RQOptionNop::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::raptorq::RQOption::parsimUnpack(b);
}

class RQOptionNopDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    RQOptionNopDescriptor();
    virtual ~RQOptionNopDescriptor();

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

Register_ClassDescriptor(RQOptionNopDescriptor)

RQOptionNopDescriptor::RQOptionNopDescriptor() : omnetpp::cClassDescriptor("inet::raptorq::RQOptionNop", "inet::raptorq::RQOption")
{
    propertynames = nullptr;
}

RQOptionNopDescriptor::~RQOptionNopDescriptor()
{
    delete[] propertynames;
}

bool RQOptionNopDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<RQOptionNop *>(obj)!=nullptr;
}

const char **RQOptionNopDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *RQOptionNopDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int RQOptionNopDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 0+basedesc->getFieldCount() : 0;
}

unsigned int RQOptionNopDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    return 0;
}

const char *RQOptionNopDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

int RQOptionNopDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *RQOptionNopDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

const char **RQOptionNopDescriptor::getFieldPropertyNames(int field) const
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

const char *RQOptionNopDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int RQOptionNopDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    RQOptionNop *pp = (RQOptionNop *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *RQOptionNopDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RQOptionNop *pp = (RQOptionNop *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string RQOptionNopDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RQOptionNop *pp = (RQOptionNop *)object; (void)pp;
    switch (field) {
        default: return "";
    }
}

bool RQOptionNopDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    RQOptionNop *pp = (RQOptionNop *)object; (void)pp;
    switch (field) {
        default: return false;
    }
}

const char *RQOptionNopDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

void *RQOptionNopDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    RQOptionNop *pp = (RQOptionNop *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

Register_Class(RQOptionMaxSegmentSize)

RQOptionMaxSegmentSize::RQOptionMaxSegmentSize() : ::inet::raptorq::RQOption()
{
    this->setKind(RQOPTION_MAXIMUM_SEGMENT_SIZE);
    this->setLength(4);

    this->maxSegmentSize = 0;
}

RQOptionMaxSegmentSize::RQOptionMaxSegmentSize(const RQOptionMaxSegmentSize& other) : ::inet::raptorq::RQOption(other)
{
    copy(other);
}

RQOptionMaxSegmentSize::~RQOptionMaxSegmentSize()
{
}

RQOptionMaxSegmentSize& RQOptionMaxSegmentSize::operator=(const RQOptionMaxSegmentSize& other)
{
    if (this==&other) return *this;
    ::inet::raptorq::RQOption::operator=(other);
    copy(other);
    return *this;
}

void RQOptionMaxSegmentSize::copy(const RQOptionMaxSegmentSize& other)
{
    this->maxSegmentSize = other.maxSegmentSize;
}

void RQOptionMaxSegmentSize::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::raptorq::RQOption::parsimPack(b);
    doParsimPacking(b,this->maxSegmentSize);
}

void RQOptionMaxSegmentSize::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::raptorq::RQOption::parsimUnpack(b);
    doParsimUnpacking(b,this->maxSegmentSize);
}

uint16_t RQOptionMaxSegmentSize::getMaxSegmentSize() const
{
    return this->maxSegmentSize;
}

void RQOptionMaxSegmentSize::setMaxSegmentSize(uint16_t maxSegmentSize)
{
    this->maxSegmentSize = maxSegmentSize;
}

class RQOptionMaxSegmentSizeDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    RQOptionMaxSegmentSizeDescriptor();
    virtual ~RQOptionMaxSegmentSizeDescriptor();

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

Register_ClassDescriptor(RQOptionMaxSegmentSizeDescriptor)

RQOptionMaxSegmentSizeDescriptor::RQOptionMaxSegmentSizeDescriptor() : omnetpp::cClassDescriptor("inet::raptorq::RQOptionMaxSegmentSize", "inet::raptorq::RQOption")
{
    propertynames = nullptr;
}

RQOptionMaxSegmentSizeDescriptor::~RQOptionMaxSegmentSizeDescriptor()
{
    delete[] propertynames;
}

bool RQOptionMaxSegmentSizeDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<RQOptionMaxSegmentSize *>(obj)!=nullptr;
}

const char **RQOptionMaxSegmentSizeDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *RQOptionMaxSegmentSizeDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int RQOptionMaxSegmentSizeDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 1+basedesc->getFieldCount() : 1;
}

unsigned int RQOptionMaxSegmentSizeDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
    };
    return (field>=0 && field<1) ? fieldTypeFlags[field] : 0;
}

const char *RQOptionMaxSegmentSizeDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "maxSegmentSize",
    };
    return (field>=0 && field<1) ? fieldNames[field] : nullptr;
}

int RQOptionMaxSegmentSizeDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='m' && strcmp(fieldName, "maxSegmentSize")==0) return base+0;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *RQOptionMaxSegmentSizeDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "uint16_t",
    };
    return (field>=0 && field<1) ? fieldTypeStrings[field] : nullptr;
}

const char **RQOptionMaxSegmentSizeDescriptor::getFieldPropertyNames(int field) const
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

const char *RQOptionMaxSegmentSizeDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int RQOptionMaxSegmentSizeDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    RQOptionMaxSegmentSize *pp = (RQOptionMaxSegmentSize *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *RQOptionMaxSegmentSizeDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RQOptionMaxSegmentSize *pp = (RQOptionMaxSegmentSize *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string RQOptionMaxSegmentSizeDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RQOptionMaxSegmentSize *pp = (RQOptionMaxSegmentSize *)object; (void)pp;
    switch (field) {
        case 0: return ulong2string(pp->getMaxSegmentSize());
        default: return "";
    }
}

bool RQOptionMaxSegmentSizeDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    RQOptionMaxSegmentSize *pp = (RQOptionMaxSegmentSize *)object; (void)pp;
    switch (field) {
        case 0: pp->setMaxSegmentSize(string2ulong(value)); return true;
        default: return false;
    }
}

const char *RQOptionMaxSegmentSizeDescriptor::getFieldStructName(int field) const
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

void *RQOptionMaxSegmentSizeDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    RQOptionMaxSegmentSize *pp = (RQOptionMaxSegmentSize *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

Register_Class(RQOptionWindowScale)

RQOptionWindowScale::RQOptionWindowScale() : ::inet::raptorq::RQOption()
{
    this->setKind(RQOPTION_WINDOW_SCALE);
    this->setLength(3);

    this->windowScale = 0;
}

RQOptionWindowScale::RQOptionWindowScale(const RQOptionWindowScale& other) : ::inet::raptorq::RQOption(other)
{
    copy(other);
}

RQOptionWindowScale::~RQOptionWindowScale()
{
}

RQOptionWindowScale& RQOptionWindowScale::operator=(const RQOptionWindowScale& other)
{
    if (this==&other) return *this;
    ::inet::raptorq::RQOption::operator=(other);
    copy(other);
    return *this;
}

void RQOptionWindowScale::copy(const RQOptionWindowScale& other)
{
    this->windowScale = other.windowScale;
}

void RQOptionWindowScale::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::raptorq::RQOption::parsimPack(b);
    doParsimPacking(b,this->windowScale);
}

void RQOptionWindowScale::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::raptorq::RQOption::parsimUnpack(b);
    doParsimUnpacking(b,this->windowScale);
}

unsigned short RQOptionWindowScale::getWindowScale() const
{
    return this->windowScale;
}

void RQOptionWindowScale::setWindowScale(unsigned short windowScale)
{
    this->windowScale = windowScale;
}

class RQOptionWindowScaleDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    RQOptionWindowScaleDescriptor();
    virtual ~RQOptionWindowScaleDescriptor();

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

Register_ClassDescriptor(RQOptionWindowScaleDescriptor)

RQOptionWindowScaleDescriptor::RQOptionWindowScaleDescriptor() : omnetpp::cClassDescriptor("inet::raptorq::RQOptionWindowScale", "inet::raptorq::RQOption")
{
    propertynames = nullptr;
}

RQOptionWindowScaleDescriptor::~RQOptionWindowScaleDescriptor()
{
    delete[] propertynames;
}

bool RQOptionWindowScaleDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<RQOptionWindowScale *>(obj)!=nullptr;
}

const char **RQOptionWindowScaleDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *RQOptionWindowScaleDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int RQOptionWindowScaleDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 1+basedesc->getFieldCount() : 1;
}

unsigned int RQOptionWindowScaleDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
    };
    return (field>=0 && field<1) ? fieldTypeFlags[field] : 0;
}

const char *RQOptionWindowScaleDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "windowScale",
    };
    return (field>=0 && field<1) ? fieldNames[field] : nullptr;
}

int RQOptionWindowScaleDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='w' && strcmp(fieldName, "windowScale")==0) return base+0;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *RQOptionWindowScaleDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "unsigned short",
    };
    return (field>=0 && field<1) ? fieldTypeStrings[field] : nullptr;
}

const char **RQOptionWindowScaleDescriptor::getFieldPropertyNames(int field) const
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

const char *RQOptionWindowScaleDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int RQOptionWindowScaleDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    RQOptionWindowScale *pp = (RQOptionWindowScale *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *RQOptionWindowScaleDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RQOptionWindowScale *pp = (RQOptionWindowScale *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string RQOptionWindowScaleDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RQOptionWindowScale *pp = (RQOptionWindowScale *)object; (void)pp;
    switch (field) {
        case 0: return ulong2string(pp->getWindowScale());
        default: return "";
    }
}

bool RQOptionWindowScaleDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    RQOptionWindowScale *pp = (RQOptionWindowScale *)object; (void)pp;
    switch (field) {
        case 0: pp->setWindowScale(string2ulong(value)); return true;
        default: return false;
    }
}

const char *RQOptionWindowScaleDescriptor::getFieldStructName(int field) const
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

void *RQOptionWindowScaleDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    RQOptionWindowScale *pp = (RQOptionWindowScale *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

Register_Class(RQOptionSackPermitted)

RQOptionSackPermitted::RQOptionSackPermitted() : ::inet::raptorq::RQOption()
{
    this->setKind(RQOPTION_SACK_PERMITTED);
    this->setLength(2);
}

RQOptionSackPermitted::RQOptionSackPermitted(const RQOptionSackPermitted& other) : ::inet::raptorq::RQOption(other)
{
    copy(other);
}

RQOptionSackPermitted::~RQOptionSackPermitted()
{
}

RQOptionSackPermitted& RQOptionSackPermitted::operator=(const RQOptionSackPermitted& other)
{
    if (this==&other) return *this;
    ::inet::raptorq::RQOption::operator=(other);
    copy(other);
    return *this;
}

void RQOptionSackPermitted::copy(const RQOptionSackPermitted& other)
{
}

void RQOptionSackPermitted::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::raptorq::RQOption::parsimPack(b);
}

void RQOptionSackPermitted::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::raptorq::RQOption::parsimUnpack(b);
}

class RQOptionSackPermittedDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    RQOptionSackPermittedDescriptor();
    virtual ~RQOptionSackPermittedDescriptor();

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

Register_ClassDescriptor(RQOptionSackPermittedDescriptor)

RQOptionSackPermittedDescriptor::RQOptionSackPermittedDescriptor() : omnetpp::cClassDescriptor("inet::raptorq::RQOptionSackPermitted", "inet::raptorq::RQOption")
{
    propertynames = nullptr;
}

RQOptionSackPermittedDescriptor::~RQOptionSackPermittedDescriptor()
{
    delete[] propertynames;
}

bool RQOptionSackPermittedDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<RQOptionSackPermitted *>(obj)!=nullptr;
}

const char **RQOptionSackPermittedDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *RQOptionSackPermittedDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int RQOptionSackPermittedDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 0+basedesc->getFieldCount() : 0;
}

unsigned int RQOptionSackPermittedDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    return 0;
}

const char *RQOptionSackPermittedDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

int RQOptionSackPermittedDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *RQOptionSackPermittedDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

const char **RQOptionSackPermittedDescriptor::getFieldPropertyNames(int field) const
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

const char *RQOptionSackPermittedDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int RQOptionSackPermittedDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    RQOptionSackPermitted *pp = (RQOptionSackPermitted *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *RQOptionSackPermittedDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RQOptionSackPermitted *pp = (RQOptionSackPermitted *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string RQOptionSackPermittedDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RQOptionSackPermitted *pp = (RQOptionSackPermitted *)object; (void)pp;
    switch (field) {
        default: return "";
    }
}

bool RQOptionSackPermittedDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    RQOptionSackPermitted *pp = (RQOptionSackPermitted *)object; (void)pp;
    switch (field) {
        default: return false;
    }
}

const char *RQOptionSackPermittedDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

void *RQOptionSackPermittedDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    RQOptionSackPermitted *pp = (RQOptionSackPermitted *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

Register_Class(RQOptionSack)

RQOptionSack::RQOptionSack() : ::inet::raptorq::RQOption()
{
    this->setKind(RQOPTION_SACK);
    this->setLength(2);

    sackItem_arraysize = 0;
    this->sackItem = 0;
}

RQOptionSack::RQOptionSack(const RQOptionSack& other) : ::inet::raptorq::RQOption(other)
{
    sackItem_arraysize = 0;
    this->sackItem = 0;
    copy(other);
}

RQOptionSack::~RQOptionSack()
{
    delete [] this->sackItem;
}

RQOptionSack& RQOptionSack::operator=(const RQOptionSack& other)
{
    if (this==&other) return *this;
    ::inet::raptorq::RQOption::operator=(other);
    copy(other);
    return *this;
}

void RQOptionSack::copy(const RQOptionSack& other)
{
    delete [] this->sackItem;
    this->sackItem = (other.sackItem_arraysize==0) ? nullptr : new SackItem[other.sackItem_arraysize];
    sackItem_arraysize = other.sackItem_arraysize;
    for (unsigned int i=0; i<sackItem_arraysize; i++)
        this->sackItem[i] = other.sackItem[i];
}

void RQOptionSack::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::raptorq::RQOption::parsimPack(b);
    b->pack(sackItem_arraysize);
    doParsimArrayPacking(b,this->sackItem,sackItem_arraysize);
}

void RQOptionSack::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::raptorq::RQOption::parsimUnpack(b);
    delete [] this->sackItem;
    b->unpack(sackItem_arraysize);
    if (sackItem_arraysize==0) {
        this->sackItem = 0;
    } else {
        this->sackItem = new SackItem[sackItem_arraysize];
        doParsimArrayUnpacking(b,this->sackItem,sackItem_arraysize);
    }
}

void RQOptionSack::setSackItemArraySize(unsigned int size)
{
    SackItem *sackItem2 = (size==0) ? nullptr : new SackItem[size];
    unsigned int sz = sackItem_arraysize < size ? sackItem_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        sackItem2[i] = this->sackItem[i];
    sackItem_arraysize = size;
    delete [] this->sackItem;
    this->sackItem = sackItem2;
}

unsigned int RQOptionSack::getSackItemArraySize() const
{
    return sackItem_arraysize;
}

SackItem& RQOptionSack::getSackItem(unsigned int k)
{
    if (k>=sackItem_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", sackItem_arraysize, k);
    return this->sackItem[k];
}

void RQOptionSack::setSackItem(unsigned int k, const SackItem& sackItem)
{
    if (k>=sackItem_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", sackItem_arraysize, k);
    this->sackItem[k] = sackItem;
}

class RQOptionSackDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    RQOptionSackDescriptor();
    virtual ~RQOptionSackDescriptor();

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

Register_ClassDescriptor(RQOptionSackDescriptor)

RQOptionSackDescriptor::RQOptionSackDescriptor() : omnetpp::cClassDescriptor("inet::raptorq::RQOptionSack", "inet::raptorq::RQOption")
{
    propertynames = nullptr;
}

RQOptionSackDescriptor::~RQOptionSackDescriptor()
{
    delete[] propertynames;
}

bool RQOptionSackDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<RQOptionSack *>(obj)!=nullptr;
}

const char **RQOptionSackDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *RQOptionSackDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int RQOptionSackDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 1+basedesc->getFieldCount() : 1;
}

unsigned int RQOptionSackDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISARRAY | FD_ISCOMPOUND | FD_ISCOBJECT,
    };
    return (field>=0 && field<1) ? fieldTypeFlags[field] : 0;
}

const char *RQOptionSackDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "sackItem",
    };
    return (field>=0 && field<1) ? fieldNames[field] : nullptr;
}

int RQOptionSackDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='s' && strcmp(fieldName, "sackItem")==0) return base+0;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *RQOptionSackDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "SackItem",
    };
    return (field>=0 && field<1) ? fieldTypeStrings[field] : nullptr;
}

const char **RQOptionSackDescriptor::getFieldPropertyNames(int field) const
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

const char *RQOptionSackDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int RQOptionSackDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    RQOptionSack *pp = (RQOptionSack *)object; (void)pp;
    switch (field) {
        case 0: return pp->getSackItemArraySize();
        default: return 0;
    }
}

const char *RQOptionSackDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RQOptionSack *pp = (RQOptionSack *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string RQOptionSackDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RQOptionSack *pp = (RQOptionSack *)object; (void)pp;
    switch (field) {
        case 0: {std::stringstream out; out << pp->getSackItem(i); return out.str();}
        default: return "";
    }
}

bool RQOptionSackDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    RQOptionSack *pp = (RQOptionSack *)object; (void)pp;
    switch (field) {
        default: return false;
    }
}

const char *RQOptionSackDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 0: return omnetpp::opp_typename(typeid(SackItem));
        default: return nullptr;
    };
}

void *RQOptionSackDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    RQOptionSack *pp = (RQOptionSack *)object; (void)pp;
    switch (field) {
        case 0: return (void *)static_cast<omnetpp::cObject *>(&pp->getSackItem(i)); break;
        default: return nullptr;
    }
}

Register_Class(RQOptionTimestamp)

RQOptionTimestamp::RQOptionTimestamp() : ::inet::raptorq::RQOption()
{
    this->setKind(RQOPTION_TIMESTAMP);
    this->setLength(10);

    this->senderTimestamp = 0;
    this->echoedTimestamp = 0;
}

RQOptionTimestamp::RQOptionTimestamp(const RQOptionTimestamp& other) : ::inet::raptorq::RQOption(other)
{
    copy(other);
}

RQOptionTimestamp::~RQOptionTimestamp()
{
}

RQOptionTimestamp& RQOptionTimestamp::operator=(const RQOptionTimestamp& other)
{
    if (this==&other) return *this;
    ::inet::raptorq::RQOption::operator=(other);
    copy(other);
    return *this;
}

void RQOptionTimestamp::copy(const RQOptionTimestamp& other)
{
    this->senderTimestamp = other.senderTimestamp;
    this->echoedTimestamp = other.echoedTimestamp;
}

void RQOptionTimestamp::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::raptorq::RQOption::parsimPack(b);
    doParsimPacking(b,this->senderTimestamp);
    doParsimPacking(b,this->echoedTimestamp);
}

void RQOptionTimestamp::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::raptorq::RQOption::parsimUnpack(b);
    doParsimUnpacking(b,this->senderTimestamp);
    doParsimUnpacking(b,this->echoedTimestamp);
}

uint32_t RQOptionTimestamp::getSenderTimestamp() const
{
    return this->senderTimestamp;
}

void RQOptionTimestamp::setSenderTimestamp(uint32_t senderTimestamp)
{
    this->senderTimestamp = senderTimestamp;
}

uint32_t RQOptionTimestamp::getEchoedTimestamp() const
{
    return this->echoedTimestamp;
}

void RQOptionTimestamp::setEchoedTimestamp(uint32_t echoedTimestamp)
{
    this->echoedTimestamp = echoedTimestamp;
}

class RQOptionTimestampDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    RQOptionTimestampDescriptor();
    virtual ~RQOptionTimestampDescriptor();

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

Register_ClassDescriptor(RQOptionTimestampDescriptor)

RQOptionTimestampDescriptor::RQOptionTimestampDescriptor() : omnetpp::cClassDescriptor("inet::raptorq::RQOptionTimestamp", "inet::raptorq::RQOption")
{
    propertynames = nullptr;
}

RQOptionTimestampDescriptor::~RQOptionTimestampDescriptor()
{
    delete[] propertynames;
}

bool RQOptionTimestampDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<RQOptionTimestamp *>(obj)!=nullptr;
}

const char **RQOptionTimestampDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *RQOptionTimestampDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int RQOptionTimestampDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 2+basedesc->getFieldCount() : 2;
}

unsigned int RQOptionTimestampDescriptor::getFieldTypeFlags(int field) const
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

const char *RQOptionTimestampDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "senderTimestamp",
        "echoedTimestamp",
    };
    return (field>=0 && field<2) ? fieldNames[field] : nullptr;
}

int RQOptionTimestampDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='s' && strcmp(fieldName, "senderTimestamp")==0) return base+0;
    if (fieldName[0]=='e' && strcmp(fieldName, "echoedTimestamp")==0) return base+1;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *RQOptionTimestampDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "uint32_t",
        "uint32_t",
    };
    return (field>=0 && field<2) ? fieldTypeStrings[field] : nullptr;
}

const char **RQOptionTimestampDescriptor::getFieldPropertyNames(int field) const
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

const char *RQOptionTimestampDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int RQOptionTimestampDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    RQOptionTimestamp *pp = (RQOptionTimestamp *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *RQOptionTimestampDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RQOptionTimestamp *pp = (RQOptionTimestamp *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string RQOptionTimestampDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RQOptionTimestamp *pp = (RQOptionTimestamp *)object; (void)pp;
    switch (field) {
        case 0: return ulong2string(pp->getSenderTimestamp());
        case 1: return ulong2string(pp->getEchoedTimestamp());
        default: return "";
    }
}

bool RQOptionTimestampDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    RQOptionTimestamp *pp = (RQOptionTimestamp *)object; (void)pp;
    switch (field) {
        case 0: pp->setSenderTimestamp(string2ulong(value)); return true;
        case 1: pp->setEchoedTimestamp(string2ulong(value)); return true;
        default: return false;
    }
}

const char *RQOptionTimestampDescriptor::getFieldStructName(int field) const
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

void *RQOptionTimestampDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    RQOptionTimestamp *pp = (RQOptionTimestamp *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

Register_Class(RQOptionUnknown)

RQOptionUnknown::RQOptionUnknown() : ::inet::raptorq::RQOption()
{
    this->setKind(-1);

    bytes_arraysize = 0;
    this->bytes = 0;
}

RQOptionUnknown::RQOptionUnknown(const RQOptionUnknown& other) : ::inet::raptorq::RQOption(other)
{
    bytes_arraysize = 0;
    this->bytes = 0;
    copy(other);
}

RQOptionUnknown::~RQOptionUnknown()
{
    delete [] this->bytes;
}

RQOptionUnknown& RQOptionUnknown::operator=(const RQOptionUnknown& other)
{
    if (this==&other) return *this;
    ::inet::raptorq::RQOption::operator=(other);
    copy(other);
    return *this;
}

void RQOptionUnknown::copy(const RQOptionUnknown& other)
{
    delete [] this->bytes;
    this->bytes = (other.bytes_arraysize==0) ? nullptr : new uint8_t[other.bytes_arraysize];
    bytes_arraysize = other.bytes_arraysize;
    for (unsigned int i=0; i<bytes_arraysize; i++)
        this->bytes[i] = other.bytes[i];
}

void RQOptionUnknown::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::raptorq::RQOption::parsimPack(b);
    b->pack(bytes_arraysize);
    doParsimArrayPacking(b,this->bytes,bytes_arraysize);
}

void RQOptionUnknown::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::raptorq::RQOption::parsimUnpack(b);
    delete [] this->bytes;
    b->unpack(bytes_arraysize);
    if (bytes_arraysize==0) {
        this->bytes = 0;
    } else {
        this->bytes = new uint8_t[bytes_arraysize];
        doParsimArrayUnpacking(b,this->bytes,bytes_arraysize);
    }
}

void RQOptionUnknown::setBytesArraySize(unsigned int size)
{
    uint8_t *bytes2 = (size==0) ? nullptr : new uint8_t[size];
    unsigned int sz = bytes_arraysize < size ? bytes_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        bytes2[i] = this->bytes[i];
    for (unsigned int i=sz; i<size; i++)
        bytes2[i] = 0;
    bytes_arraysize = size;
    delete [] this->bytes;
    this->bytes = bytes2;
}

unsigned int RQOptionUnknown::getBytesArraySize() const
{
    return bytes_arraysize;
}

uint8_t RQOptionUnknown::getBytes(unsigned int k) const
{
    if (k>=bytes_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", bytes_arraysize, k);
    return this->bytes[k];
}

void RQOptionUnknown::setBytes(unsigned int k, uint8_t bytes)
{
    if (k>=bytes_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", bytes_arraysize, k);
    this->bytes[k] = bytes;
}

class RQOptionUnknownDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    RQOptionUnknownDescriptor();
    virtual ~RQOptionUnknownDescriptor();

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

Register_ClassDescriptor(RQOptionUnknownDescriptor)

RQOptionUnknownDescriptor::RQOptionUnknownDescriptor() : omnetpp::cClassDescriptor("inet::raptorq::RQOptionUnknown", "inet::raptorq::RQOption")
{
    propertynames = nullptr;
}

RQOptionUnknownDescriptor::~RQOptionUnknownDescriptor()
{
    delete[] propertynames;
}

bool RQOptionUnknownDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<RQOptionUnknown *>(obj)!=nullptr;
}

const char **RQOptionUnknownDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *RQOptionUnknownDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int RQOptionUnknownDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 1+basedesc->getFieldCount() : 1;
}

unsigned int RQOptionUnknownDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISARRAY | FD_ISEDITABLE,
    };
    return (field>=0 && field<1) ? fieldTypeFlags[field] : 0;
}

const char *RQOptionUnknownDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "bytes",
    };
    return (field>=0 && field<1) ? fieldNames[field] : nullptr;
}

int RQOptionUnknownDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='b' && strcmp(fieldName, "bytes")==0) return base+0;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *RQOptionUnknownDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "uint8_t",
    };
    return (field>=0 && field<1) ? fieldTypeStrings[field] : nullptr;
}

const char **RQOptionUnknownDescriptor::getFieldPropertyNames(int field) const
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

const char *RQOptionUnknownDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int RQOptionUnknownDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    RQOptionUnknown *pp = (RQOptionUnknown *)object; (void)pp;
    switch (field) {
        case 0: return pp->getBytesArraySize();
        default: return 0;
    }
}

const char *RQOptionUnknownDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RQOptionUnknown *pp = (RQOptionUnknown *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string RQOptionUnknownDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RQOptionUnknown *pp = (RQOptionUnknown *)object; (void)pp;
    switch (field) {
        case 0: return ulong2string(pp->getBytes(i));
        default: return "";
    }
}

bool RQOptionUnknownDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    RQOptionUnknown *pp = (RQOptionUnknown *)object; (void)pp;
    switch (field) {
        case 0: pp->setBytes(i,string2ulong(value)); return true;
        default: return false;
    }
}

const char *RQOptionUnknownDescriptor::getFieldStructName(int field) const
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

void *RQOptionUnknownDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    RQOptionUnknown *pp = (RQOptionUnknown *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

RQSegment_Base::RQSegment_Base(const char *name, short kind) : ::omnetpp::cPacket(name,kind)
{
    this->srcPort_var = 0;
    this->destPort_var = 0;
    this->sequenceNo_var = 0;
    this->ackNo_var = 0;
    this->isClient_var = false;
    this->isWriter_var = false;
    this->isReader_var = false;
    this->isHeader_var = false;
    this->isRequestPacket_var = false;
    this->isMulticastConnSeg_var = false;
    this->multicastGroupIndex_var = 0;
    this->isMulticastSender_var = false;
    this->isMulticastReceiver_var = false;
    this->isMultiSourcingConn_var = false;
    this->multiSourcingGroupIndex_var = 0;
    this->isMultiSourcingSender_var = false;
    this->isMultiSourcingReceiver_var = false;
    this->isLastSymbolToSend_var = false;
    this->numSymbolsToSend_var = 0;
    this->numSymbolsToGet_var = 0;
    this->fileId_var = 0;
    this->requestSequenceNumber_var = 0;
    this->symbolSequenceNumber_var = 0;
    this->opcode_var = 0;
    this->headerLength_var = RQ_HEADER_OCTETS;
    this->urgBit_var = false;
    this->ackBit_var = false;
    this->pshBit_var = false;
    this->rstBit_var = false;
    this->synBit_var = false;
    this->finBit_var = false;
    this->window_var = 0;
    this->urgentPointer_var = 0;
    this->payloadLength_var = 0;
}

RQSegment_Base::RQSegment_Base(const RQSegment_Base& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

RQSegment_Base::~RQSegment_Base()
{
}

RQSegment_Base& RQSegment_Base::operator=(const RQSegment_Base& other)
{
    if (this==&other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void RQSegment_Base::copy(const RQSegment_Base& other)
{
    this->srcPort_var = other.srcPort_var;
    this->destPort_var = other.destPort_var;
    this->sequenceNo_var = other.sequenceNo_var;
    this->ackNo_var = other.ackNo_var;
    this->isClient_var = other.isClient_var;
    this->isWriter_var = other.isWriter_var;
    this->isReader_var = other.isReader_var;
    this->isHeader_var = other.isHeader_var;
    this->isRequestPacket_var = other.isRequestPacket_var;
    this->isMulticastConnSeg_var = other.isMulticastConnSeg_var;
    this->multicastGroupIndex_var = other.multicastGroupIndex_var;
    this->isMulticastSender_var = other.isMulticastSender_var;
    this->isMulticastReceiver_var = other.isMulticastReceiver_var;
    this->isMultiSourcingConn_var = other.isMultiSourcingConn_var;
    this->multiSourcingGroupIndex_var = other.multiSourcingGroupIndex_var;
    this->isMultiSourcingSender_var = other.isMultiSourcingSender_var;
    this->isMultiSourcingReceiver_var = other.isMultiSourcingReceiver_var;
    this->isLastSymbolToSend_var = other.isLastSymbolToSend_var;
    this->numSymbolsToSend_var = other.numSymbolsToSend_var;
    this->numSymbolsToGet_var = other.numSymbolsToGet_var;
    this->fileId_var = other.fileId_var;
    this->requestSequenceNumber_var = other.requestSequenceNumber_var;
    this->symbolSequenceNumber_var = other.symbolSequenceNumber_var;
    this->opcode_var = other.opcode_var;
    this->headerLength_var = other.headerLength_var;
    this->urgBit_var = other.urgBit_var;
    this->ackBit_var = other.ackBit_var;
    this->pshBit_var = other.pshBit_var;
    this->rstBit_var = other.rstBit_var;
    this->synBit_var = other.synBit_var;
    this->finBit_var = other.finBit_var;
    this->window_var = other.window_var;
    this->urgentPointer_var = other.urgentPointer_var;
    this->payloadLength_var = other.payloadLength_var;
    this->byteArray_var = other.byteArray_var;
}

void RQSegment_Base::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->srcPort_var);
    doParsimPacking(b,this->destPort_var);
    doParsimPacking(b,this->sequenceNo_var);
    doParsimPacking(b,this->ackNo_var);
    doParsimPacking(b,this->isClient_var);
    doParsimPacking(b,this->isWriter_var);
    doParsimPacking(b,this->isReader_var);
    doParsimPacking(b,this->isHeader_var);
    doParsimPacking(b,this->isRequestPacket_var);
    doParsimPacking(b,this->isMulticastConnSeg_var);
    doParsimPacking(b,this->multicastGroupIndex_var);
    doParsimPacking(b,this->isMulticastSender_var);
    doParsimPacking(b,this->isMulticastReceiver_var);
    doParsimPacking(b,this->isMultiSourcingConn_var);
    doParsimPacking(b,this->multiSourcingGroupIndex_var);
    doParsimPacking(b,this->isMultiSourcingSender_var);
    doParsimPacking(b,this->isMultiSourcingReceiver_var);
    doParsimPacking(b,this->isLastSymbolToSend_var);
    doParsimPacking(b,this->numSymbolsToSend_var);
    doParsimPacking(b,this->numSymbolsToGet_var);
    doParsimPacking(b,this->fileId_var);
    doParsimPacking(b,this->requestSequenceNumber_var);
    doParsimPacking(b,this->symbolSequenceNumber_var);
    doParsimPacking(b,this->opcode_var);
    doParsimPacking(b,this->headerLength_var);
    doParsimPacking(b,this->urgBit_var);
    doParsimPacking(b,this->ackBit_var);
    doParsimPacking(b,this->pshBit_var);
    doParsimPacking(b,this->rstBit_var);
    doParsimPacking(b,this->synBit_var);
    doParsimPacking(b,this->finBit_var);
    doParsimPacking(b,this->window_var);
    doParsimPacking(b,this->urgentPointer_var);
    // field headerOption is abstract -- please do packing in customized class
    doParsimPacking(b,this->payloadLength_var);
    // field payload is abstract -- please do packing in customized class
    doParsimPacking(b,this->byteArray_var);
}

void RQSegment_Base::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->srcPort_var);
    doParsimUnpacking(b,this->destPort_var);
    doParsimUnpacking(b,this->sequenceNo_var);
    doParsimUnpacking(b,this->ackNo_var);
    doParsimUnpacking(b,this->isClient_var);
    doParsimUnpacking(b,this->isWriter_var);
    doParsimUnpacking(b,this->isReader_var);
    doParsimUnpacking(b,this->isHeader_var);
    doParsimUnpacking(b,this->isRequestPacket_var);
    doParsimUnpacking(b,this->isMulticastConnSeg_var);
    doParsimUnpacking(b,this->multicastGroupIndex_var);
    doParsimUnpacking(b,this->isMulticastSender_var);
    doParsimUnpacking(b,this->isMulticastReceiver_var);
    doParsimUnpacking(b,this->isMultiSourcingConn_var);
    doParsimUnpacking(b,this->multiSourcingGroupIndex_var);
    doParsimUnpacking(b,this->isMultiSourcingSender_var);
    doParsimUnpacking(b,this->isMultiSourcingReceiver_var);
    doParsimUnpacking(b,this->isLastSymbolToSend_var);
    doParsimUnpacking(b,this->numSymbolsToSend_var);
    doParsimUnpacking(b,this->numSymbolsToGet_var);
    doParsimUnpacking(b,this->fileId_var);
    doParsimUnpacking(b,this->requestSequenceNumber_var);
    doParsimUnpacking(b,this->symbolSequenceNumber_var);
    doParsimUnpacking(b,this->opcode_var);
    doParsimUnpacking(b,this->headerLength_var);
    doParsimUnpacking(b,this->urgBit_var);
    doParsimUnpacking(b,this->ackBit_var);
    doParsimUnpacking(b,this->pshBit_var);
    doParsimUnpacking(b,this->rstBit_var);
    doParsimUnpacking(b,this->synBit_var);
    doParsimUnpacking(b,this->finBit_var);
    doParsimUnpacking(b,this->window_var);
    doParsimUnpacking(b,this->urgentPointer_var);
    // field headerOption is abstract -- please do unpacking in customized class
    doParsimUnpacking(b,this->payloadLength_var);
    // field payload is abstract -- please do unpacking in customized class
    doParsimUnpacking(b,this->byteArray_var);
}

unsigned short RQSegment_Base::getSrcPort() const
{
    return this->srcPort_var;
}

void RQSegment_Base::setSrcPort(unsigned short srcPort)
{
    this->srcPort_var = srcPort;
}

unsigned short RQSegment_Base::getDestPort() const
{
    return this->destPort_var;
}

void RQSegment_Base::setDestPort(unsigned short destPort)
{
    this->destPort_var = destPort;
}

unsigned int RQSegment_Base::getSequenceNo() const
{
    return this->sequenceNo_var;
}

void RQSegment_Base::setSequenceNo(unsigned int sequenceNo)
{
    this->sequenceNo_var = sequenceNo;
}

unsigned int RQSegment_Base::getAckNo() const
{
    return this->ackNo_var;
}

void RQSegment_Base::setAckNo(unsigned int ackNo)
{
    this->ackNo_var = ackNo;
}

bool RQSegment_Base::getIsClient() const
{
    return this->isClient_var;
}

void RQSegment_Base::setIsClient(bool isClient)
{
    this->isClient_var = isClient;
}

bool RQSegment_Base::getIsWriter() const
{
    return this->isWriter_var;
}

void RQSegment_Base::setIsWriter(bool isWriter)
{
    this->isWriter_var = isWriter;
}

bool RQSegment_Base::getIsReader() const
{
    return this->isReader_var;
}

void RQSegment_Base::setIsReader(bool isReader)
{
    this->isReader_var = isReader;
}

bool RQSegment_Base::getIsHeader() const
{
    return this->isHeader_var;
}

void RQSegment_Base::setIsHeader(bool isHeader)
{
    this->isHeader_var = isHeader;
}

bool RQSegment_Base::getIsRequestPacket() const
{
    return this->isRequestPacket_var;
}

void RQSegment_Base::setIsRequestPacket(bool isRequestPacket)
{
    this->isRequestPacket_var = isRequestPacket;
}

bool RQSegment_Base::getIsMulticastConnSeg() const
{
    return this->isMulticastConnSeg_var;
}

void RQSegment_Base::setIsMulticastConnSeg(bool isMulticastConnSeg)
{
    this->isMulticastConnSeg_var = isMulticastConnSeg;
}

unsigned int RQSegment_Base::getMulticastGroupIndex() const
{
    return this->multicastGroupIndex_var;
}

void RQSegment_Base::setMulticastGroupIndex(unsigned int multicastGroupIndex)
{
    this->multicastGroupIndex_var = multicastGroupIndex;
}

bool RQSegment_Base::getIsMulticastSender() const
{
    return this->isMulticastSender_var;
}

void RQSegment_Base::setIsMulticastSender(bool isMulticastSender)
{
    this->isMulticastSender_var = isMulticastSender;
}

bool RQSegment_Base::getIsMulticastReceiver() const
{
    return this->isMulticastReceiver_var;
}

void RQSegment_Base::setIsMulticastReceiver(bool isMulticastReceiver)
{
    this->isMulticastReceiver_var = isMulticastReceiver;
}

bool RQSegment_Base::getIsMultiSourcingConn() const
{
    return this->isMultiSourcingConn_var;
}

void RQSegment_Base::setIsMultiSourcingConn(bool isMultiSourcingConn)
{
    this->isMultiSourcingConn_var = isMultiSourcingConn;
}

unsigned int RQSegment_Base::getMultiSourcingGroupIndex() const
{
    return this->multiSourcingGroupIndex_var;
}

void RQSegment_Base::setMultiSourcingGroupIndex(unsigned int multiSourcingGroupIndex)
{
    this->multiSourcingGroupIndex_var = multiSourcingGroupIndex;
}

bool RQSegment_Base::getIsMultiSourcingSender() const
{
    return this->isMultiSourcingSender_var;
}

void RQSegment_Base::setIsMultiSourcingSender(bool isMultiSourcingSender)
{
    this->isMultiSourcingSender_var = isMultiSourcingSender;
}

bool RQSegment_Base::getIsMultiSourcingReceiver() const
{
    return this->isMultiSourcingReceiver_var;
}

void RQSegment_Base::setIsMultiSourcingReceiver(bool isMultiSourcingReceiver)
{
    this->isMultiSourcingReceiver_var = isMultiSourcingReceiver;
}

bool RQSegment_Base::getIsLastSymbolToSend() const
{
    return this->isLastSymbolToSend_var;
}

void RQSegment_Base::setIsLastSymbolToSend(bool isLastSymbolToSend)
{
    this->isLastSymbolToSend_var = isLastSymbolToSend;
}

unsigned int RQSegment_Base::getNumSymbolsToSend() const
{
    return this->numSymbolsToSend_var;
}

void RQSegment_Base::setNumSymbolsToSend(unsigned int numSymbolsToSend)
{
    this->numSymbolsToSend_var = numSymbolsToSend;
}

unsigned int RQSegment_Base::getNumSymbolsToGet() const
{
    return this->numSymbolsToGet_var;
}

void RQSegment_Base::setNumSymbolsToGet(unsigned int numSymbolsToGet)
{
    this->numSymbolsToGet_var = numSymbolsToGet;
}

unsigned int RQSegment_Base::getFileId() const
{
    return this->fileId_var;
}

void RQSegment_Base::setFileId(unsigned int fileId)
{
    this->fileId_var = fileId;
}

unsigned int RQSegment_Base::getRequestSequenceNumber() const
{
    return this->requestSequenceNumber_var;
}

void RQSegment_Base::setRequestSequenceNumber(unsigned int requestSequenceNumber)
{
    this->requestSequenceNumber_var = requestSequenceNumber;
}

unsigned int RQSegment_Base::getSymbolSequenceNumber() const
{
    return this->symbolSequenceNumber_var;
}

void RQSegment_Base::setSymbolSequenceNumber(unsigned int symbolSequenceNumber)
{
    this->symbolSequenceNumber_var = symbolSequenceNumber;
}

unsigned int RQSegment_Base::getOpcode() const
{
    return this->opcode_var;
}

void RQSegment_Base::setOpcode(unsigned int opcode)
{
    this->opcode_var = opcode;
}

unsigned short RQSegment_Base::getHeaderLength() const
{
    return this->headerLength_var;
}

void RQSegment_Base::setHeaderLength(unsigned short headerLength)
{
    this->headerLength_var = headerLength;
}

bool RQSegment_Base::getUrgBit() const
{
    return this->urgBit_var;
}

void RQSegment_Base::setUrgBit(bool urgBit)
{
    this->urgBit_var = urgBit;
}

bool RQSegment_Base::getAckBit() const
{
    return this->ackBit_var;
}

void RQSegment_Base::setAckBit(bool ackBit)
{
    this->ackBit_var = ackBit;
}

bool RQSegment_Base::getPshBit() const
{
    return this->pshBit_var;
}

void RQSegment_Base::setPshBit(bool pshBit)
{
    this->pshBit_var = pshBit;
}

bool RQSegment_Base::getRstBit() const
{
    return this->rstBit_var;
}

void RQSegment_Base::setRstBit(bool rstBit)
{
    this->rstBit_var = rstBit;
}

bool RQSegment_Base::getSynBit() const
{
    return this->synBit_var;
}

void RQSegment_Base::setSynBit(bool synBit)
{
    this->synBit_var = synBit;
}

bool RQSegment_Base::getFinBit() const
{
    return this->finBit_var;
}

void RQSegment_Base::setFinBit(bool finBit)
{
    this->finBit_var = finBit;
}

unsigned short RQSegment_Base::getWindow() const
{
    return this->window_var;
}

void RQSegment_Base::setWindow(unsigned short window)
{
    this->window_var = window;
}

unsigned short RQSegment_Base::getUrgentPointer() const
{
    return this->urgentPointer_var;
}

void RQSegment_Base::setUrgentPointer(unsigned short urgentPointer)
{
    this->urgentPointer_var = urgentPointer;
}

unsigned long RQSegment_Base::getPayloadLength() const
{
    return this->payloadLength_var;
}

void RQSegment_Base::setPayloadLength(unsigned long payloadLength)
{
    this->payloadLength_var = payloadLength;
}

ByteArray& RQSegment_Base::getByteArray()
{
    return this->byteArray_var;
}

void RQSegment_Base::setByteArray(const ByteArray& byteArray)
{
    this->byteArray_var = byteArray;
}

class RQSegmentDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    RQSegmentDescriptor();
    virtual ~RQSegmentDescriptor();

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

Register_ClassDescriptor(RQSegmentDescriptor)

RQSegmentDescriptor::RQSegmentDescriptor() : omnetpp::cClassDescriptor("inet::raptorq::RQSegment", "omnetpp::cPacket")
{
    propertynames = nullptr;
}

RQSegmentDescriptor::~RQSegmentDescriptor()
{
    delete[] propertynames;
}

bool RQSegmentDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<RQSegment_Base *>(obj)!=nullptr;
}

const char **RQSegmentDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = { "customize", "fieldNameSuffix",  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *RQSegmentDescriptor::getProperty(const char *propertyname) const
{
    if (!strcmp(propertyname,"customize")) return "true";
    if (!strcmp(propertyname,"fieldNameSuffix")) return "_var";
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int RQSegmentDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 37+basedesc->getFieldCount() : 37;
}

unsigned int RQSegmentDescriptor::getFieldTypeFlags(int field) const
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
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISARRAY | FD_ISCOMPOUND,
        FD_ISEDITABLE,
        FD_ISARRAY | FD_ISCOMPOUND,
        FD_ISCOMPOUND,
    };
    return (field>=0 && field<37) ? fieldTypeFlags[field] : 0;
}

const char *RQSegmentDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "srcPort",
        "destPort",
        "sequenceNo",
        "ackNo",
        "isClient",
        "isWriter",
        "isReader",
        "isHeader",
        "isRequestPacket",
        "isMulticastConnSeg",
        "multicastGroupIndex",
        "isMulticastSender",
        "isMulticastReceiver",
        "isMultiSourcingConn",
        "multiSourcingGroupIndex",
        "isMultiSourcingSender",
        "isMultiSourcingReceiver",
        "isLastSymbolToSend",
        "numSymbolsToSend",
        "numSymbolsToGet",
        "fileId",
        "requestSequenceNumber",
        "symbolSequenceNumber",
        "opcode",
        "headerLength",
        "urgBit",
        "ackBit",
        "pshBit",
        "rstBit",
        "synBit",
        "finBit",
        "window",
        "urgentPointer",
        "headerOption",
        "payloadLength",
        "payload",
        "byteArray",
    };
    return (field>=0 && field<37) ? fieldNames[field] : nullptr;
}

int RQSegmentDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='s' && strcmp(fieldName, "srcPort")==0) return base+0;
    if (fieldName[0]=='d' && strcmp(fieldName, "destPort")==0) return base+1;
    if (fieldName[0]=='s' && strcmp(fieldName, "sequenceNo")==0) return base+2;
    if (fieldName[0]=='a' && strcmp(fieldName, "ackNo")==0) return base+3;
    if (fieldName[0]=='i' && strcmp(fieldName, "isClient")==0) return base+4;
    if (fieldName[0]=='i' && strcmp(fieldName, "isWriter")==0) return base+5;
    if (fieldName[0]=='i' && strcmp(fieldName, "isReader")==0) return base+6;
    if (fieldName[0]=='i' && strcmp(fieldName, "isHeader")==0) return base+7;
    if (fieldName[0]=='i' && strcmp(fieldName, "isRequestPacket")==0) return base+8;
    if (fieldName[0]=='i' && strcmp(fieldName, "isMulticastConnSeg")==0) return base+9;
    if (fieldName[0]=='m' && strcmp(fieldName, "multicastGroupIndex")==0) return base+10;
    if (fieldName[0]=='i' && strcmp(fieldName, "isMulticastSender")==0) return base+11;
    if (fieldName[0]=='i' && strcmp(fieldName, "isMulticastReceiver")==0) return base+12;
    if (fieldName[0]=='i' && strcmp(fieldName, "isMultiSourcingConn")==0) return base+13;
    if (fieldName[0]=='m' && strcmp(fieldName, "multiSourcingGroupIndex")==0) return base+14;
    if (fieldName[0]=='i' && strcmp(fieldName, "isMultiSourcingSender")==0) return base+15;
    if (fieldName[0]=='i' && strcmp(fieldName, "isMultiSourcingReceiver")==0) return base+16;
    if (fieldName[0]=='i' && strcmp(fieldName, "isLastSymbolToSend")==0) return base+17;
    if (fieldName[0]=='n' && strcmp(fieldName, "numSymbolsToSend")==0) return base+18;
    if (fieldName[0]=='n' && strcmp(fieldName, "numSymbolsToGet")==0) return base+19;
    if (fieldName[0]=='f' && strcmp(fieldName, "fileId")==0) return base+20;
    if (fieldName[0]=='r' && strcmp(fieldName, "requestSequenceNumber")==0) return base+21;
    if (fieldName[0]=='s' && strcmp(fieldName, "symbolSequenceNumber")==0) return base+22;
    if (fieldName[0]=='o' && strcmp(fieldName, "opcode")==0) return base+23;
    if (fieldName[0]=='h' && strcmp(fieldName, "headerLength")==0) return base+24;
    if (fieldName[0]=='u' && strcmp(fieldName, "urgBit")==0) return base+25;
    if (fieldName[0]=='a' && strcmp(fieldName, "ackBit")==0) return base+26;
    if (fieldName[0]=='p' && strcmp(fieldName, "pshBit")==0) return base+27;
    if (fieldName[0]=='r' && strcmp(fieldName, "rstBit")==0) return base+28;
    if (fieldName[0]=='s' && strcmp(fieldName, "synBit")==0) return base+29;
    if (fieldName[0]=='f' && strcmp(fieldName, "finBit")==0) return base+30;
    if (fieldName[0]=='w' && strcmp(fieldName, "window")==0) return base+31;
    if (fieldName[0]=='u' && strcmp(fieldName, "urgentPointer")==0) return base+32;
    if (fieldName[0]=='h' && strcmp(fieldName, "headerOption")==0) return base+33;
    if (fieldName[0]=='p' && strcmp(fieldName, "payloadLength")==0) return base+34;
    if (fieldName[0]=='p' && strcmp(fieldName, "payload")==0) return base+35;
    if (fieldName[0]=='b' && strcmp(fieldName, "byteArray")==0) return base+36;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *RQSegmentDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "unsigned short",
        "unsigned short",
        "unsigned int",
        "unsigned int",
        "bool",
        "bool",
        "bool",
        "bool",
        "bool",
        "bool",
        "unsigned int",
        "bool",
        "bool",
        "bool",
        "unsigned int",
        "bool",
        "bool",
        "bool",
        "unsigned int",
        "unsigned int",
        "unsigned int",
        "unsigned int",
        "unsigned int",
        "unsigned int",
        "unsigned short",
        "bool",
        "bool",
        "bool",
        "bool",
        "bool",
        "bool",
        "unsigned short",
        "unsigned short",
        "RQOptionPtr",
        "unsigned long",
        "RQPayloadMessage",
        "ByteArray",
    };
    return (field>=0 && field<37) ? fieldTypeStrings[field] : nullptr;
}

const char **RQSegmentDescriptor::getFieldPropertyNames(int field) const
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

const char *RQSegmentDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int RQSegmentDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    RQSegment_Base *pp = (RQSegment_Base *)object; (void)pp;
    switch (field) {
        case 33: return pp->getHeaderOptionArraySize();
        case 35: return pp->getPayloadArraySize();
        default: return 0;
    }
}

const char *RQSegmentDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RQSegment_Base *pp = (RQSegment_Base *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string RQSegmentDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RQSegment_Base *pp = (RQSegment_Base *)object; (void)pp;
    switch (field) {
        case 0: return ulong2string(pp->getSrcPort());
        case 1: return ulong2string(pp->getDestPort());
        case 2: return ulong2string(pp->getSequenceNo());
        case 3: return ulong2string(pp->getAckNo());
        case 4: return bool2string(pp->getIsClient());
        case 5: return bool2string(pp->getIsWriter());
        case 6: return bool2string(pp->getIsReader());
        case 7: return bool2string(pp->getIsHeader());
        case 8: return bool2string(pp->getIsRequestPacket());
        case 9: return bool2string(pp->getIsMulticastConnSeg());
        case 10: return ulong2string(pp->getMulticastGroupIndex());
        case 11: return bool2string(pp->getIsMulticastSender());
        case 12: return bool2string(pp->getIsMulticastReceiver());
        case 13: return bool2string(pp->getIsMultiSourcingConn());
        case 14: return ulong2string(pp->getMultiSourcingGroupIndex());
        case 15: return bool2string(pp->getIsMultiSourcingSender());
        case 16: return bool2string(pp->getIsMultiSourcingReceiver());
        case 17: return bool2string(pp->getIsLastSymbolToSend());
        case 18: return ulong2string(pp->getNumSymbolsToSend());
        case 19: return ulong2string(pp->getNumSymbolsToGet());
        case 20: return ulong2string(pp->getFileId());
        case 21: return ulong2string(pp->getRequestSequenceNumber());
        case 22: return ulong2string(pp->getSymbolSequenceNumber());
        case 23: return ulong2string(pp->getOpcode());
        case 24: return ulong2string(pp->getHeaderLength());
        case 25: return bool2string(pp->getUrgBit());
        case 26: return bool2string(pp->getAckBit());
        case 27: return bool2string(pp->getPshBit());
        case 28: return bool2string(pp->getRstBit());
        case 29: return bool2string(pp->getSynBit());
        case 30: return bool2string(pp->getFinBit());
        case 31: return ulong2string(pp->getWindow());
        case 32: return ulong2string(pp->getUrgentPointer());
        case 33: {std::stringstream out; out << pp->getHeaderOption(i); return out.str();}
        case 34: return ulong2string(pp->getPayloadLength());
        case 35: {std::stringstream out; out << pp->getPayload(i); return out.str();}
        case 36: {std::stringstream out; out << pp->getByteArray(); return out.str();}
        default: return "";
    }
}

bool RQSegmentDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    RQSegment_Base *pp = (RQSegment_Base *)object; (void)pp;
    switch (field) {
        case 0: pp->setSrcPort(string2ulong(value)); return true;
        case 1: pp->setDestPort(string2ulong(value)); return true;
        case 2: pp->setSequenceNo(string2ulong(value)); return true;
        case 3: pp->setAckNo(string2ulong(value)); return true;
        case 4: pp->setIsClient(string2bool(value)); return true;
        case 5: pp->setIsWriter(string2bool(value)); return true;
        case 6: pp->setIsReader(string2bool(value)); return true;
        case 7: pp->setIsHeader(string2bool(value)); return true;
        case 8: pp->setIsRequestPacket(string2bool(value)); return true;
        case 9: pp->setIsMulticastConnSeg(string2bool(value)); return true;
        case 10: pp->setMulticastGroupIndex(string2ulong(value)); return true;
        case 11: pp->setIsMulticastSender(string2bool(value)); return true;
        case 12: pp->setIsMulticastReceiver(string2bool(value)); return true;
        case 13: pp->setIsMultiSourcingConn(string2bool(value)); return true;
        case 14: pp->setMultiSourcingGroupIndex(string2ulong(value)); return true;
        case 15: pp->setIsMultiSourcingSender(string2bool(value)); return true;
        case 16: pp->setIsMultiSourcingReceiver(string2bool(value)); return true;
        case 17: pp->setIsLastSymbolToSend(string2bool(value)); return true;
        case 18: pp->setNumSymbolsToSend(string2ulong(value)); return true;
        case 19: pp->setNumSymbolsToGet(string2ulong(value)); return true;
        case 20: pp->setFileId(string2ulong(value)); return true;
        case 21: pp->setRequestSequenceNumber(string2ulong(value)); return true;
        case 22: pp->setSymbolSequenceNumber(string2ulong(value)); return true;
        case 23: pp->setOpcode(string2ulong(value)); return true;
        case 24: pp->setHeaderLength(string2ulong(value)); return true;
        case 25: pp->setUrgBit(string2bool(value)); return true;
        case 26: pp->setAckBit(string2bool(value)); return true;
        case 27: pp->setPshBit(string2bool(value)); return true;
        case 28: pp->setRstBit(string2bool(value)); return true;
        case 29: pp->setSynBit(string2bool(value)); return true;
        case 30: pp->setFinBit(string2bool(value)); return true;
        case 31: pp->setWindow(string2ulong(value)); return true;
        case 32: pp->setUrgentPointer(string2ulong(value)); return true;
        case 34: pp->setPayloadLength(string2ulong(value)); return true;
        default: return false;
    }
}

const char *RQSegmentDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 33: return omnetpp::opp_typename(typeid(RQOptionPtr));
        case 35: return omnetpp::opp_typename(typeid(RQPayloadMessage));
        case 36: return omnetpp::opp_typename(typeid(ByteArray));
        default: return nullptr;
    };
}

void *RQSegmentDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    RQSegment_Base *pp = (RQSegment_Base *)object; (void)pp;
    switch (field) {
        case 33: return (void *)(&pp->getHeaderOption(i)); break;
        case 35: return (void *)(&pp->getPayload(i)); break;
        case 36: return (void *)(&pp->getByteArray()); break;
        default: return nullptr;
    }
}

} // namespace raptorq
} // namespace inet

