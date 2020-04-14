//
// Generated file, do not edit! Created by nedtool 5.4 from msg/requestContent.msg.
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
#include "requestContent_m.h"

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

Register_Class(requestContentMsg)

requestContentMsg::requestContentMsg(const char *name, short kind) : ::omnetpp::cMessage(name,kind)
{
    this->requestedContentAge = 0;
    this->requestID = 0;
    this->contentID = 0;
    this->satisfied = false;
    this->returnPath = false;
    this->hopCount = 0;
    this->hopCountUntilCacheHit = 0;
    this->cacheHitHappened = false;
    this->distanceToCloud = 0;
}

requestContentMsg::requestContentMsg(const requestContentMsg& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

requestContentMsg::~requestContentMsg()
{
}

requestContentMsg& requestContentMsg::operator=(const requestContentMsg& other)
{
    if (this==&other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void requestContentMsg::copy(const requestContentMsg& other)
{
    this->userName = other.userName;
    this->srcCluster = other.srcCluster;
    this->dstCluster = other.dstCluster;
    this->requestedContentAge = other.requestedContentAge;
    this->requestID = other.requestID;
    this->contentID = other.contentID;
    this->satisfied = other.satisfied;
    this->returnPath = other.returnPath;
    this->hopCount = other.hopCount;
    this->hopCountUntilCacheHit = other.hopCountUntilCacheHit;
    this->cacheHitHappened = other.cacheHitHappened;
    this->distanceToCloud = other.distanceToCloud;
}

void requestContentMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->userName);
    doParsimPacking(b,this->srcCluster);
    doParsimPacking(b,this->dstCluster);
    doParsimPacking(b,this->requestedContentAge);
    doParsimPacking(b,this->requestID);
    doParsimPacking(b,this->contentID);
    doParsimPacking(b,this->satisfied);
    doParsimPacking(b,this->returnPath);
    doParsimPacking(b,this->hopCount);
    doParsimPacking(b,this->hopCountUntilCacheHit);
    doParsimPacking(b,this->cacheHitHappened);
    doParsimPacking(b,this->distanceToCloud);
}

void requestContentMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->userName);
    doParsimUnpacking(b,this->srcCluster);
    doParsimUnpacking(b,this->dstCluster);
    doParsimUnpacking(b,this->requestedContentAge);
    doParsimUnpacking(b,this->requestID);
    doParsimUnpacking(b,this->contentID);
    doParsimUnpacking(b,this->satisfied);
    doParsimUnpacking(b,this->returnPath);
    doParsimUnpacking(b,this->hopCount);
    doParsimUnpacking(b,this->hopCountUntilCacheHit);
    doParsimUnpacking(b,this->cacheHitHappened);
    doParsimUnpacking(b,this->distanceToCloud);
}

const char * requestContentMsg::getUserName() const
{
    return this->userName.c_str();
}

void requestContentMsg::setUserName(const char * userName)
{
    this->userName = userName;
}

const char * requestContentMsg::getSrcCluster() const
{
    return this->srcCluster.c_str();
}

void requestContentMsg::setSrcCluster(const char * srcCluster)
{
    this->srcCluster = srcCluster;
}

const char * requestContentMsg::getDstCluster() const
{
    return this->dstCluster.c_str();
}

void requestContentMsg::setDstCluster(const char * dstCluster)
{
    this->dstCluster = dstCluster;
}

double requestContentMsg::getRequestedContentAge() const
{
    return this->requestedContentAge;
}

void requestContentMsg::setRequestedContentAge(double requestedContentAge)
{
    this->requestedContentAge = requestedContentAge;
}

int requestContentMsg::getRequestID() const
{
    return this->requestID;
}

void requestContentMsg::setRequestID(int requestID)
{
    this->requestID = requestID;
}

int requestContentMsg::getContentID() const
{
    return this->contentID;
}

void requestContentMsg::setContentID(int contentID)
{
    this->contentID = contentID;
}

bool requestContentMsg::getSatisfied() const
{
    return this->satisfied;
}

void requestContentMsg::setSatisfied(bool satisfied)
{
    this->satisfied = satisfied;
}

bool requestContentMsg::getReturnPath() const
{
    return this->returnPath;
}

void requestContentMsg::setReturnPath(bool returnPath)
{
    this->returnPath = returnPath;
}

int requestContentMsg::getHopCount() const
{
    return this->hopCount;
}

void requestContentMsg::setHopCount(int hopCount)
{
    this->hopCount = hopCount;
}

int requestContentMsg::getHopCountUntilCacheHit() const
{
    return this->hopCountUntilCacheHit;
}

void requestContentMsg::setHopCountUntilCacheHit(int hopCountUntilCacheHit)
{
    this->hopCountUntilCacheHit = hopCountUntilCacheHit;
}

bool requestContentMsg::getCacheHitHappened() const
{
    return this->cacheHitHappened;
}

void requestContentMsg::setCacheHitHappened(bool cacheHitHappened)
{
    this->cacheHitHappened = cacheHitHappened;
}

int requestContentMsg::getDistanceToCloud() const
{
    return this->distanceToCloud;
}

void requestContentMsg::setDistanceToCloud(int distanceToCloud)
{
    this->distanceToCloud = distanceToCloud;
}

class requestContentMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    requestContentMsgDescriptor();
    virtual ~requestContentMsgDescriptor();

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

Register_ClassDescriptor(requestContentMsgDescriptor)

requestContentMsgDescriptor::requestContentMsgDescriptor() : omnetpp::cClassDescriptor("requestContentMsg", "omnetpp::cMessage")
{
    propertynames = nullptr;
}

requestContentMsgDescriptor::~requestContentMsgDescriptor()
{
    delete[] propertynames;
}

bool requestContentMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<requestContentMsg *>(obj)!=nullptr;
}

const char **requestContentMsgDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *requestContentMsgDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int requestContentMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 12+basedesc->getFieldCount() : 12;
}

unsigned int requestContentMsgDescriptor::getFieldTypeFlags(int field) const
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

const char *requestContentMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "userName",
        "srcCluster",
        "dstCluster",
        "requestedContentAge",
        "requestID",
        "contentID",
        "satisfied",
        "returnPath",
        "hopCount",
        "hopCountUntilCacheHit",
        "cacheHitHappened",
        "distanceToCloud",
    };
    return (field>=0 && field<12) ? fieldNames[field] : nullptr;
}

int requestContentMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='u' && strcmp(fieldName, "userName")==0) return base+0;
    if (fieldName[0]=='s' && strcmp(fieldName, "srcCluster")==0) return base+1;
    if (fieldName[0]=='d' && strcmp(fieldName, "dstCluster")==0) return base+2;
    if (fieldName[0]=='r' && strcmp(fieldName, "requestedContentAge")==0) return base+3;
    if (fieldName[0]=='r' && strcmp(fieldName, "requestID")==0) return base+4;
    if (fieldName[0]=='c' && strcmp(fieldName, "contentID")==0) return base+5;
    if (fieldName[0]=='s' && strcmp(fieldName, "satisfied")==0) return base+6;
    if (fieldName[0]=='r' && strcmp(fieldName, "returnPath")==0) return base+7;
    if (fieldName[0]=='h' && strcmp(fieldName, "hopCount")==0) return base+8;
    if (fieldName[0]=='h' && strcmp(fieldName, "hopCountUntilCacheHit")==0) return base+9;
    if (fieldName[0]=='c' && strcmp(fieldName, "cacheHitHappened")==0) return base+10;
    if (fieldName[0]=='d' && strcmp(fieldName, "distanceToCloud")==0) return base+11;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *requestContentMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "string",
        "string",
        "string",
        "double",
        "int",
        "int",
        "bool",
        "bool",
        "int",
        "int",
        "bool",
        "int",
    };
    return (field>=0 && field<12) ? fieldTypeStrings[field] : nullptr;
}

const char **requestContentMsgDescriptor::getFieldPropertyNames(int field) const
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

const char *requestContentMsgDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int requestContentMsgDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    requestContentMsg *pp = (requestContentMsg *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *requestContentMsgDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    requestContentMsg *pp = (requestContentMsg *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string requestContentMsgDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    requestContentMsg *pp = (requestContentMsg *)object; (void)pp;
    switch (field) {
        case 0: return oppstring2string(pp->getUserName());
        case 1: return oppstring2string(pp->getSrcCluster());
        case 2: return oppstring2string(pp->getDstCluster());
        case 3: return double2string(pp->getRequestedContentAge());
        case 4: return long2string(pp->getRequestID());
        case 5: return long2string(pp->getContentID());
        case 6: return bool2string(pp->getSatisfied());
        case 7: return bool2string(pp->getReturnPath());
        case 8: return long2string(pp->getHopCount());
        case 9: return long2string(pp->getHopCountUntilCacheHit());
        case 10: return bool2string(pp->getCacheHitHappened());
        case 11: return long2string(pp->getDistanceToCloud());
        default: return "";
    }
}

bool requestContentMsgDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    requestContentMsg *pp = (requestContentMsg *)object; (void)pp;
    switch (field) {
        case 0: pp->setUserName((value)); return true;
        case 1: pp->setSrcCluster((value)); return true;
        case 2: pp->setDstCluster((value)); return true;
        case 3: pp->setRequestedContentAge(string2double(value)); return true;
        case 4: pp->setRequestID(string2long(value)); return true;
        case 5: pp->setContentID(string2long(value)); return true;
        case 6: pp->setSatisfied(string2bool(value)); return true;
        case 7: pp->setReturnPath(string2bool(value)); return true;
        case 8: pp->setHopCount(string2long(value)); return true;
        case 9: pp->setHopCountUntilCacheHit(string2long(value)); return true;
        case 10: pp->setCacheHitHappened(string2bool(value)); return true;
        case 11: pp->setDistanceToCloud(string2long(value)); return true;
        default: return false;
    }
}

const char *requestContentMsgDescriptor::getFieldStructName(int field) const
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

void *requestContentMsgDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    requestContentMsg *pp = (requestContentMsg *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}


