//
// Generated file, do not edit! Created by nedtool 5.4 from msg/clNodes.msg.
//

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#ifndef __CLNODES_M_H
#define __CLNODES_M_H

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0504
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



/**
 * Class generated from <tt>msg/clNodes.msg:2</tt> by nedtool.
 * <pre>
 * message clNodesMsg
 * {
 *     string msgType;
 *     int contentID;
 *     double productionTime;
 * }
 * </pre>
 */
class clNodesMsg : public ::omnetpp::cMessage
{
  protected:
    ::omnetpp::opp_string msgType;
    int contentID;
    double productionTime;

  private:
    void copy(const clNodesMsg& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const clNodesMsg&);

  public:
    clNodesMsg(const char *name=nullptr, short kind=0);
    clNodesMsg(const clNodesMsg& other);
    virtual ~clNodesMsg();
    clNodesMsg& operator=(const clNodesMsg& other);
    virtual clNodesMsg *dup() const override {return new clNodesMsg(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual const char * getMsgType() const;
    virtual void setMsgType(const char * msgType);
    virtual int getContentID() const;
    virtual void setContentID(int contentID);
    virtual double getProductionTime() const;
    virtual void setProductionTime(double productionTime);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const clNodesMsg& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, clNodesMsg& obj) {obj.parsimUnpack(b);}


#endif // ifndef __CLNODES_M_H

