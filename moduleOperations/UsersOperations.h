#ifndef USERSOPERATIONS_H_
#define USERSOPERATIONS_H_
#include <map>

using namespace std;

class UsersOperations {
public:

    struct RequestedContentDetails{
        double intervalBetweenRequests;
        string srcClusterNum;
        string dstClusterNum;
        double requestedContentAge;
    };

    UsersOperations();
    virtual ~UsersOperations();
    void setContentsRequestList();
    map<string,map<int,RequestedContentDetails>> requestList; //userID, content ID, request details
    map<string,map<int,double>> pendingRequests; //userID, content ID (this is not content name only a method to trace request) , simTime
    map<string,int> gateToNextNode;
};

#endif /* USERSOPERATIONS_H_ */
