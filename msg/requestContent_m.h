//
// Generated file, do not edit! Created by nedtool 5.4 from msg/requestContent.msg.
//

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#ifndef __REQUESTCONTENT_M_H
#define __REQUESTCONTENT_M_H

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0504
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



/**
 * Class generated from <tt>msg/requestContent.msg:2</tt> by nedtool.
 * <pre>
 * message requestContentMsg
 * {
 *     string userName;
 *     string srcCluster;
 *     string dstCluster;
 *     double requestedContentAge;
 *     int requestID; //this is a number to trace the request
 *     int contentID = 0;
 *     bool satisfied = false; //does any content produced in that time?
 *     bool returnPath = false; //this request is on the 
 *     int hopCount = 0;
 *     int hopCountUntilCacheHit = 0;
 *     bool cacheHitHappened = false; //if requested content is returned from cache storage
 *     int distanceToCloud;
 * }
 * </pre>
 */
class requestContentMsg : public ::omnetpp::cMessage
{
  protected:
    ::omnetpp::opp_string userName;
    ::omnetpp::opp_string srcCluster;
    ::omnetpp::opp_string dstCluster;
    double requestedContentAge;
    int requestID;
    int contentID;
    bool satisfied;
    bool returnPath;
    int hopCount;
    int hopCountUntilCacheHit;
    bool cacheHitHappened;
    int distanceToCloud;

  private:
    void copy(const requestContentMsg& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const requestContentMsg&);

  public:
    requestContentMsg(const char *name=nullptr, short kind=0);
    requestContentMsg(const requestContentMsg& other);
    virtual ~requestContentMsg();
    requestContentMsg& operator=(const requestContentMsg& other);
    virtual requestContentMsg *dup() const override {return new requestContentMsg(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual const char * getUserName() const;
    virtual void setUserName(const char * userName);
    virtual const char * getSrcCluster() const;
    virtual void setSrcCluster(const char * srcCluster);
    virtual const char * getDstCluster() const;
    virtual void setDstCluster(const char * dstCluster);
    virtual double getRequestedContentAge() const;
    virtual void setRequestedContentAge(double requestedContentAge);
    virtual int getRequestID() const;
    virtual void setRequestID(int requestID);
    virtual int getContentID() const;
    virtual void setContentID(int contentID);
    virtual bool getSatisfied() const;
    virtual void setSatisfied(bool satisfied);
    virtual bool getReturnPath() const;
    virtual void setReturnPath(bool returnPath);
    virtual int getHopCount() const;
    virtual void setHopCount(int hopCount);
    virtual int getHopCountUntilCacheHit() const;
    virtual void setHopCountUntilCacheHit(int hopCountUntilCacheHit);
    virtual bool getCacheHitHappened() const;
    virtual void setCacheHitHappened(bool cacheHitHappened);
    virtual int getDistanceToCloud() const;
    virtual void setDistanceToCloud(int distanceToCloud);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const requestContentMsg& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, requestContentMsg& obj) {obj.parsimUnpack(b);}


#endif // ifndef __REQUESTCONTENT_M_H

