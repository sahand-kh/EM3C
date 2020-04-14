#include "UsersOperations.h"

#include <fstream>
#include <string.h>

using namespace std;

UsersOperations::UsersOperations() {
    // TODO Auto-generated constructor stub
    gateToNextNode["ch1"] = 0;
    gateToNextNode["ch2"] = 1;
    gateToNextNode["ch3"] = 2;
    gateToNextNode["ch4"] = 3;
    gateToNextNode["ch5"] = 4;
    gateToNextNode["ch6"] = 5;
    gateToNextNode["ch7"] = 6;
    gateToNextNode["ch8"] = 7;
}

UsersOperations::~UsersOperations() {
    // TODO Auto-generated destructor stub
}

void UsersOperations::setContentsRequestList(){

    ifstream inputFile1;
    inputFile1.open("D:/Journal/contentRequestDetails.txt",ios::in);
    inputFile1.ignore(10000, '\n');

    string temp1,temp2,temp3,temp4,temp5;
    int user1RequestID = 1, user2RequestID = 1;
    int tempContentID;

    while( inputFile1>>temp1 ){

        inputFile1>>temp1;
        inputFile1>>temp2;
        inputFile1>>temp3;
        inputFile1>>temp4;
        inputFile1>>temp5;

        if(strcmp(temp5.c_str(), "user1") == 0){
            tempContentID = user1RequestID;
            user1RequestID++;
        } else {
            tempContentID = user2RequestID;
            user2RequestID++;
        }

        requestList[temp5][tempContentID].intervalBetweenRequests = stod(temp1);
        requestList[temp5][tempContentID].srcClusterNum = "ch" + temp2;
        requestList[temp5][tempContentID].dstClusterNum = "ch" + temp3;
        requestList[temp5][tempContentID].requestedContentAge = stod(temp4);

    }

}
