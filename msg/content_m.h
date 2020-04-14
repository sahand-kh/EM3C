//
// Generated file, do not edit! Created by nedtool 5.4 from msg/content.msg.
//

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#ifndef __CONTENT_M_H
#define __CONTENT_M_H

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0504
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



/**
 * Class generated from <tt>msg/content.msg:2</tt> by nedtool.
 * <pre>
 * message contentMsg
 * {
 *     int contentID;
 *     bool t1ContentExists;
 *     bool t2ContentExists;
 *     bool t3ContentExists;
 *     int totalContentSize;
 * }
 * </pre>
 */
class contentMsg : public ::omnetpp::cMessage
{
  protected:
    int contentID;
    bool t1ContentExists;
    bool t2ContentExists;
    bool t3ContentExists;
    int totalContentSize;

  private:
    void copy(const contentMsg& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const contentMsg&);

  public:
    contentMsg(const char *name=nullptr, short kind=0);
    contentMsg(const contentMsg& other);
    virtual ~contentMsg();
    contentMsg& operator=(const contentMsg& other);
    virtual contentMsg *dup() const override {return new contentMsg(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual int getContentID() const;
    virtual void setContentID(int contentID);
    virtual bool getT1ContentExists() const;
    virtual void setT1ContentExists(bool t1ContentExists);
    virtual bool getT2ContentExists() const;
    virtual void setT2ContentExists(bool t2ContentExists);
    virtual bool getT3ContentExists() const;
    virtual void setT3ContentExists(bool t3ContentExists);
    virtual int getTotalContentSize() const;
    virtual void setTotalContentSize(int totalContentSize);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const contentMsg& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, contentMsg& obj) {obj.parsimUnpack(b);}


#endif // ifndef __CONTENT_M_H
