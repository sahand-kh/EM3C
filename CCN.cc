#include <string.h>
#include <omnetpp.h>
#include "msg/requestContent_m.h"
#include "msg/clNodes_m.h"
#include "msg/packetIn_m.h"
#include "msg/packetOut_m.h"
#include "msg/content_m.h"
#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include "moduleOperations/ClOperations.h"
#include "moduleOperations/SdnOperations.h"
#include "moduleOperations/UsersOperations.h"
#include "moduleOperations/CloudOperations.h"

using namespace omnetpp;
using namespace std;

class SDNController : public cSimpleModule
{
private:
    map<string,int> gateToNextNode; //name, gateNumber
public:
    static SdnOperations sdnClass;
protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(SDNController);
SdnOperations SDNController::sdnClass;





class nodes : public cSimpleModule
{
private:
    string clusterHeadName;
protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    static bool firstHandleMessageRun;
public:
    static map<int,double> relativeEnergyConsumptionBasedOnHopCount; //hopCount, relative energy consumption relative to normal situation
};

Define_Module(nodes);
bool nodes::firstHandleMessageRun = true;
map<int,double> nodes::relativeEnergyConsumptionBasedOnHopCount;





class users : public cSimpleModule
{
private:
    static UsersOperations userClass;
    int nextRequestedContentID;
    cMessage *event;
    ofstream output1;
protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
public:
    static vector<string> failedNodesList;
    static ofstream output2;
};

Define_Module(users);
UsersOperations users::userClass;
ofstream users::output2;
vector<string> users::failedNodesList;





class clusterHeads : public cSimpleModule
{
public:
    static ClOperations clClass;
    static int numberOfPowerLessClusters; //illustrates number of clusters which run out of energy and could not produce any content.
private:
    map<string,map<string,int>> gateToNextNode; //type, name, gateNumber
    bool clusterCanProduceContent;
protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    void setGateNumbers();
};

Define_Module(clusterHeads);
ClOperations clusterHeads::clClass;
int clusterHeads::numberOfPowerLessClusters = 0;

void clusterHeads::initialize()
{

    if(clusterHeads::clClass.contentGenerationList.empty()){
        clusterHeads::clClass.setContentGenerationList();
        clusterHeads::clClass.setNodesHopCountList();
    }

    clusterHeads::clClass.numberOfSimilarNodes[getName()].numberOfTypeOneNodes = 0;
    clusterHeads::clClass.numberOfSimilarNodes[getName()].numberOfTypeTwoNodes = 0;
    clusterHeads::clClass.numberOfSimilarNodes[getName()].numberOfTypeThreeNodes = 0;

    setGateNumbers();
    clusterCanProduceContent = true;

    double nextContentGenerationInterval = clusterHeads::clClass.contentGenerationList[clusterHeads::clClass.nextGeneratedContentID].intervalBetweenContentGenerations;
    cMessage *event= new cMessage(to_string(clusterHeads::clClass.nextGeneratedContentID).c_str());
    clusterHeads::clClass.nextGeneratedContentID++;
    scheduleAt(simTime()+nextContentGenerationInterval, event);

}

void clusterHeads::handleMessage(cMessage *msg)
{

    if(msg->isSelfMessage()){

        if(clusterCanProduceContent){

            if( simTime() < SDNController::sdnClass.maxSimulatonTime ){

                map<string,int> tempMap = gateToNextNode["nodes"];
                double productionTime = simTime().dbl();

                //notifying nodes about producing the content
                for(map<string,int>::iterator it1=tempMap.begin() ; it1 != tempMap.end() ; it1++ ){
                    clNodesMsg *ContentGenerationsEvent = new clNodesMsg("generateContentEvent");
                    ContentGenerationsEvent->setMsgType("ContentGeneration");
                    ContentGenerationsEvent->setContentID(stoi(msg->getName()));
                    ContentGenerationsEvent->setProductionTime(productionTime);
                    send(ContentGenerationsEvent, "gate$o", it1->second);
                }

                clusterCanProduceContent = false;

                //scheduling next content production
                double nextContentGenerationInterval = clusterHeads::clClass.contentGenerationList[clusterHeads::clClass.nextGeneratedContentID].intervalBetweenContentGenerations;
                cMessage *event= new cMessage(to_string(clusterHeads::clClass.nextGeneratedContentID).c_str());
                clusterHeads::clClass.nextGeneratedContentID++;
                scheduleAt(simTime()+nextContentGenerationInterval, event);

            }

        } else {
            clusterHeads::numberOfPowerLessClusters++;
        }

    } else {

        string moduleType = msg->getSenderModule()->getClassName();

        if(strcmp(moduleType.c_str(),"nodes") == 0){

            if(!clusterCanProduceContent){

                //asking SDN to specify a path for new content
                int totalContentSize = 0;
                if(SDNController::sdnClass.generatedContentDatabase[getName()][stoi(msg->getName())].t1ContentExists){
                    totalContentSize += clusterHeads::clClass.contentGenerationList[stoi(msg->getName())].t1ContentSize;
                }
                if(SDNController::sdnClass.generatedContentDatabase[getName()][stoi(msg->getName())].t2ContentExists){
                    totalContentSize += clusterHeads::clClass.contentGenerationList[stoi(msg->getName())].t2ContentSize;
                }
                if(SDNController::sdnClass.generatedContentDatabase[getName()][stoi(msg->getName())].t3ContentExists){
                    totalContentSize += clusterHeads::clClass.contentGenerationList[stoi(msg->getName())].t3ContentSize;
                }

                packetInMsg * sdnQueryMsg = new packetInMsg("sdnQuery");
                sdnQueryMsg->setBuffer_id(stoi(msg->getName()));
                sdnQueryMsg->setReason("newContentGeneration");
                sdnQueryMsg->setHeader("sdnQuery");
                sdnQueryMsg->setIn_port(getName()); //clusterHead name
                sdnQueryMsg->setTotal_len(totalContentSize);
                send(sdnQueryMsg,"gate$o",gateToNextNode["SDNController"]["sdn"]);

                clusterCanProduceContent = true;
            }

        } else if(strcmp(moduleType.c_str(),"SDNController") == 0){

            if(strcmp(msg->getClassName(),"requestContentMsg") == 0){

                requestContentMsg * requestMsg = check_and_cast<requestContentMsg *>(msg);
                //if a content produced in that period of time (request is valid)
                if(requestMsg->getSatisfied()){

                    int requestId = requestMsg->getRequestID();
                    int contentID = requestMsg->getContentID();
                    bool returnPath = requestMsg->getReturnPath();

                    if( !returnPath && (clusterHeads::clClass.cachedContents[getName()].find(contentID) != clusterHeads::clClass.cachedContents[getName()].end()) ){

                        int totalContentSize = clusterHeads::clClass.cachedContents[getName()][contentID].totalContentSize;
                        packetInMsg * sdnQueryMsg = new packetInMsg(requestMsg->getUserName());
                        sdnQueryMsg->setBuffer_id(requestMsg->getContentID());
                        sdnQueryMsg->setReason("forwardCachedContent");
                        sdnQueryMsg->setHeader(requestMsg->getSrcCluster());
                        sdnQueryMsg->setIn_port(getName()); //clusterHead name
                        sdnQueryMsg->setTotal_len(totalContentSize);
                        send(sdnQueryMsg,"gate$o",gateToNextNode["SDNController"]["sdn"]);

                        requestMsg->setReturnPath(true);
                        requestMsg->setCacheHitHappened(true);
                        requestMsg->setHopCountUntilCacheHit(requestMsg->getHopCount());
                        send(requestMsg,"gate$o",gateToNextNode["SDNController"]["sdn"]);

                    } else {

                        string type;
                        if(gateToNextNode["clusterHeads"].find(clusterHeads::clClass.flowTable[getName()]["requestMsg"][getName()][requestId].destination) != gateToNextNode["clusterHeads"].end()){
                            type = "clusterHeads";
                        }else if(gateToNextNode["gateways"].find(clusterHeads::clClass.flowTable[getName()]["requestMsg"][getName()][requestId].destination) != gateToNextNode["gateways"].end()){
                            type = "gateways";
                        } else {
                            type = "users";
                        }

                        send(requestMsg, "gate$o", gateToNextNode[type][clusterHeads::clClass.flowTable[getName()]["requestMsg"][getName()][requestId].destination]);
                        clusterHeads::clClass.flowTable[getName()]["requestMsg"][getName()].erase(requestId);
                    }

                } else {
                    send(requestMsg, "gate$o", gateToNextNode["users"][requestMsg->getUserName()]);
                }
            } else if(strcmp(msg->getClassName(),"packetOutMsg") == 0){

                packetOutMsg * sdnResponseMsg = check_and_cast<packetOutMsg *>(msg);
                int contentId = sdnResponseMsg->getBuffer_id();
                string header = sdnResponseMsg->getHeader();
                string preveNode = sdnResponseMsg->getIn_port();
                string producerCluster = sdnResponseMsg->getName();
                map <string,string> tempFlowTableMap = sdnResponseMsg->getAction();
                clusterHeads::clClass.flowTable[getName()][header][preveNode][contentId].destination = tempFlowTableMap["Output"]; //nextNode

                int totalContentSize;
                //if SDN Controller order the clusterHead to cache the content
                if( (strcmp(header.c_str(), "ContentMsg")  == 0) && (strcmp(tempFlowTableMap["Experimenter"].c_str(),"cache") == 0) ){

                    totalContentSize = SDNController::sdnClass.generatedContentDatabase[producerCluster][contentId].totalContentSize;
                    double productionTime = SDNController::sdnClass.generatedContentDatabase[producerCluster][contentId].productionTime;
                    int availableStorageOnCluster = clusterHeads::clClass.investigateContentForCaching(getName(), contentId, producerCluster, totalContentSize, productionTime, simTime().dbl(), SDNController::sdnClass.popularityOfEachCluster);
                    SDNController::sdnClass.clustersStatus[getName()].storageLevelSum = availableStorageOnCluster;

                }

                //if this is the source cluster
                if( (strcmp(sdnResponseMsg->getHeader(), "ContentMsg")  == 0) && (strcmp(getName(), sdnResponseMsg->getIn_port()) == 0)){

                    totalContentSize = SDNController::sdnClass.generatedContentDatabase[producerCluster][contentId].totalContentSize;

                    if(strcmp(tempFlowTableMap["Experimenter"].c_str(),"forward") == 0){

                        if(clusterHeads::clClass.cachedContents[getName()].find(contentId) != clusterHeads::clClass.cachedContents[getName()].end()){

                            double consumedPowerAmount;
                            map<string,int> hostNodes = clusterHeads::clClass.cachedContents[getName()][contentId].hostNodes;
                            for(map<string,int>::iterator it1 = hostNodes.begin() ; it1 != hostNodes.end() ; it1++){

                                consumedPowerAmount = it1->second * nodes::relativeEnergyConsumptionBasedOnHopCount[clusterHeads::clClass.clusterMembersStatus[getName()][it1->first].hopCount];

                                //power level can not be negative
                                if(consumedPowerAmount > clusterHeads::clClass.clusterMembersStatus[getName()][it1->first].powerLevel){
                                    consumedPowerAmount = clusterHeads::clClass.clusterMembersStatus[getName()][it1->first].powerLevel;

                                    if(find(users::failedNodesList.begin(), users::failedNodesList.end(), it1->first) == users::failedNodesList.end()) {
                                        users::failedNodesList.push_back(it1->first);
                                        users::output2<<setw(20)<<left<<it1->first<<setw(20)<<simTime()<<endl;
                                    }

                                }

                                clusterHeads::clClass.clusterMembersStatus[getName()][it1->first].powerLevel -= consumedPowerAmount;
                                SDNController::sdnClass.clustersStatus[getName()].powerLevelSum -= consumedPowerAmount;
                            }
                        }
                    }

                    contentMsg *content = new contentMsg("content");

                    content->setContentID(sdnResponseMsg->getBuffer_id());
                    content->setT1ContentExists(SDNController::sdnClass.generatedContentDatabase[getName()][contentId].t1ContentExists);
                    content->setT2ContentExists(SDNController::sdnClass.generatedContentDatabase[getName()][contentId].t2ContentExists);
                    content->setT3ContentExists(SDNController::sdnClass.generatedContentDatabase[getName()][contentId].t3ContentExists);
                    content->setTotalContentSize(totalContentSize);

                    string type;
                    if(gateToNextNode["clusterHeads"].find(clusterHeads::clClass.flowTable[getName()]["ContentMsg"][preveNode][contentId].destination) != gateToNextNode["clusterHeads"].end()){
                        type = "clusterHeads";
                    } else if(gateToNextNode["gateways"].find(clusterHeads::clClass.flowTable[getName()]["ContentMsg"][preveNode][contentId].destination) != gateToNextNode["gateways"].end()) {
                        type = "gateways";
                    } else {
                        type = "users";
                    }

                    send(content, "gate$o", gateToNextNode[type][clusterHeads::clClass.flowTable[getName()][header][preveNode][contentId].destination]);
                    clusterHeads::clClass.flowTable[getName()][header][preveNode].erase(contentId);

                }

            }

        } else if(strcmp(moduleType.c_str(),"users") == 0){

            requestContentMsg * requestMsg = check_and_cast<requestContentMsg *>(msg);
            requestMsg->setHopCount(requestMsg->getHopCount()+1);
            send(requestMsg,"gate$o",gateToNextNode["SDNController"]["sdn"]);

        } else if(strcmp(moduleType.c_str(),"gateways") == 0){

            if(strcmp(msg->getClassName(),"contentMsg") == 0){

                contentMsg * content = check_and_cast<contentMsg *>(msg);

                int contentId = content->getContentID();
                string preveNode = "blabla";
                string type;
                if(gateToNextNode["clusterHeads"].find(clusterHeads::clClass.flowTable[getName()]["ContentMsg"][preveNode][contentId].destination) != gateToNextNode["clusterHeads"].end()){
                    type = "clusterHeads";
                } else if(gateToNextNode["gateways"].find(clusterHeads::clClass.flowTable[getName()]["ContentMsg"][preveNode][contentId].destination) != gateToNextNode["gateways"].end()) {
                    type = "gateways";
                } else {
                    type = "users";
                }

                send(content, "gate$o", gateToNextNode[type][clusterHeads::clClass.flowTable[getName()]["ContentMsg"][preveNode][contentId].destination]);
                clusterHeads::clClass.flowTable[getName()]["ContentMsg"][preveNode].erase(contentId);

            } else if(strcmp(msg->getClassName(),"requestContentMsg") == 0) {

                requestContentMsg * requestMsg = check_and_cast<requestContentMsg *>(msg);
                requestMsg->setHopCount(requestMsg->getHopCount()+1);

                int totalContentSize = SDNController::sdnClass.generatedContentDatabase[requestMsg->getDstCluster()][requestMsg->getContentID()].totalContentSize;
                packetInMsg * sdnQueryMsg = new packetInMsg(requestMsg->getUserName());
                sdnQueryMsg->setBuffer_id(requestMsg->getContentID());
                sdnQueryMsg->setReason("forwardCloudContent");
                sdnQueryMsg->setHeader(requestMsg->getSrcCluster());
                sdnQueryMsg->setIn_port(getName()); //clusterHead name
                sdnQueryMsg->setTotal_len(totalContentSize);

                send(requestMsg,"gate$o",gateToNextNode["SDNController"]["sdn"]);
                send(sdnQueryMsg,"gate$o",gateToNextNode["SDNController"]["sdn"]);

            }

        } else if(strcmp(moduleType.c_str(),"clusterHeads") == 0){

            if(strcmp(msg->getClassName(),"contentMsg") == 0){
                contentMsg * content = check_and_cast<contentMsg *>(msg);

                int contentId = content->getContentID();
                string preveNode = msg->getSenderModule()->getName();
                string type;
                if(gateToNextNode["clusterHeads"].find(clusterHeads::clClass.flowTable[getName()]["ContentMsg"][preveNode][contentId].destination) != gateToNextNode["clusterHeads"].end()){
                    type = "clusterHeads";
                } else if(gateToNextNode["gateways"].find(clusterHeads::clClass.flowTable[getName()]["ContentMsg"][preveNode][contentId].destination) != gateToNextNode["gateways"].end()) {
                    type = "gateways";
                } else {
                    type = "users";
                }

                send(content, "gate$o", gateToNextNode[type][clusterHeads::clClass.flowTable[getName()]["ContentMsg"][preveNode][contentId].destination]);
                clusterHeads::clClass.flowTable[getName()]["ContentMsg"][preveNode].erase(contentId);

            } else if(strcmp(msg->getClassName(),"requestContentMsg") == 0) {

                requestContentMsg * requestMsg = check_and_cast<requestContentMsg *>(msg);
                requestMsg->setHopCount(requestMsg->getHopCount()+1);

                int requestId = requestMsg->getRequestID();
                int contentID = requestMsg->getContentID();
                string preveNode = msg->getSenderModule()->getName();
                bool returnPath = requestMsg->getReturnPath();

                if( !returnPath && (clusterHeads::clClass.cachedContents[getName()].find(contentID) != clusterHeads::clClass.cachedContents[getName()].end()) ){

                    int totalContentSize = clusterHeads::clClass.cachedContents[getName()][contentID].totalContentSize;
                    packetInMsg * sdnQueryMsg = new packetInMsg(requestMsg->getUserName());
                    sdnQueryMsg->setBuffer_id(requestMsg->getContentID());
                    sdnQueryMsg->setReason("forwardCachedContent");
                    sdnQueryMsg->setHeader(requestMsg->getSrcCluster());
                    sdnQueryMsg->setIn_port(getName()); //clusterHead name
                    sdnQueryMsg->setTotal_len(totalContentSize);
                    send(sdnQueryMsg,"gate$o",gateToNextNode["SDNController"]["sdn"]);

                    requestMsg->setReturnPath(true);
                    requestMsg->setCacheHitHappened(true);
                    requestMsg->setHopCountUntilCacheHit(requestMsg->getHopCount());
                    send(requestMsg,"gate$o",gateToNextNode["SDNController"]["sdn"]);

                } else {

                    string type;
                    if(gateToNextNode["clusterHeads"].find(clusterHeads::clClass.flowTable[getName()]["requestMsg"][preveNode][requestId].destination) != gateToNextNode["clusterHeads"].end()){
                        type = "clusterHeads";
                    }else if(gateToNextNode["gateways"].find(clusterHeads::clClass.flowTable[getName()]["requestMsg"][preveNode][requestId].destination) != gateToNextNode["gateways"].end()){
                        type = "gateways";
                    } else {
                        type = "users";
                    }
                    send(requestMsg, "gate$o", gateToNextNode[type][clusterHeads::clClass.flowTable[getName()]["requestMsg"][preveNode][requestId].destination]);
                    clusterHeads::clClass.flowTable[getName()]["requestMsg"][preveNode].erase(requestId);
                }

            }
        }

    }
}

void clusterHeads::setGateNumbers(){

    SDNController::sdnClass.clusterList.push_back(getName());

    for(int i=0; i<gateSize("gate$o"); i++){

        string className = gate("gate$o", i)->getNextGate()->getOwnerModule()->getClassName();
        string moduleName = gate("gate$o", i)->getNextGate()->getOwnerModule()->getFullName();

        gateToNextNode[className][moduleName] = i;

        if( strcmp(className.c_str(),"gateways") == 0){
            SDNController::sdnClass.neighbors[getName()].push_back(moduleName);
        } else if(strcmp(className.c_str(), "clusterHeads") == 0){
            SDNController::sdnClass.neighbors[getName()].push_back(moduleName);
        }
    }

}







void SDNController::initialize()
{

    for(int i=0; i<gateSize("gate$o"); i++){
           string moduleName = gate("gate$o", i)->getNextGate()->getOwnerModule()->getFullName();
           gateToNextNode[moduleName] = i;
    }

}

void SDNController::handleMessage(cMessage *msg)
{

    if(SDNController::sdnClass.nodesConnections.empty()){
        SDNController::sdnClass.dijkstra();
        SDNController::sdnClass.betweennessCalculator();
        SDNController::sdnClass.popularityCalculator(0);
    }

    if(strcmp(msg->getClassName(),"requestContentMsg") == 0){

        requestContentMsg * requestMsg = check_and_cast<requestContentMsg *>(msg);
        bool returnPath = requestMsg->getReturnPath();

        if(returnPath){

            double currentSimTime = simTime().dbl();
            string senderCluster = msg->getSenderModule()->getName();

            SDNController::sdnClass.availablePathFinder(senderCluster, currentSimTime, 0, false, requestMsg->getSrcCluster());
            string bestDst = SDNController::sdnClass.bestPathFinder();

            vector<string> tempPath = SDNController::sdnClass.availablePathes[bestDst].path;
            for(int i=0 ; i<tempPath.size() ; i++){
                packetOutMsg * sdnResponseMsg = new packetOutMsg("sdnResponse");
                sdnResponseMsg->setBuffer_id(requestMsg->getRequestID());
                sdnResponseMsg->setHeader("requestMsg");

                if(i == (tempPath.size()-1)){
                    sdnResponseMsg->setIn_port(tempPath[i].c_str());
                } else {
                    sdnResponseMsg->setIn_port(tempPath[i+1].c_str());
                }

                sdnResponseMsg->setActions_len(1);

                map<string,string> action;
                if(i == 0){
                    action["Output"] = bestDst;
                } else {
                    action["Output"] = tempPath[i-1];
                }

                sdnResponseMsg->setAction(action);
                send(sdnResponseMsg,"gate$o",gateToNextNode[tempPath[i]]);

            }

            packetOutMsg * sdnResponseMsg = new packetOutMsg("sdnResponse");
            sdnResponseMsg->setBuffer_id(requestMsg->getRequestID());
            sdnResponseMsg->setHeader("requestMsg");

            if(tempPath.size() > 0){
                sdnResponseMsg->setIn_port(tempPath[0].c_str());
            } else {
                sdnResponseMsg->setIn_port(bestDst.c_str());
            }

            sdnResponseMsg->setActions_len(2);

            map<string,string> action;
            action["Output"] = requestMsg->getUserName(); //user which request the content
            action["Experimenter"] = "none";
            sdnResponseMsg->setAction(action);
            send(sdnResponseMsg,"gate$o",gateToNextNode[bestDst]);
            send(requestMsg,"gate$o",gateToNextNode[senderCluster]);

        } else {
            //this is a request for a content that should send toward a node that possess the content

            //calculate the popularity of the content
            if( (simTime().dbl() - SDNController::sdnClass.lastPopularityRecordsCleaningTime) > SDNController::sdnClass.popularityCaclulationPeriod){
                SDNController::sdnClass.popularityCalculator(simTime().dbl());
            }
            SDNController::sdnClass.popularityRecords[requestMsg->getDstCluster()].push_back(simTime().dbl());

            int contentID = SDNController::sdnClass.whichContentProduced(requestMsg->getRequestedContentAge(), requestMsg->getDstCluster(), simTime().dbl());

            if(contentID == 0){
                requestMsg->setSatisfied(false);
                send(requestMsg,"gate$o",gateToNextNode[requestMsg->getSrcCluster()]);
            } else {
                requestMsg->setContentID(contentID);
                requestMsg->setSatisfied(true);
                bool cacheIsAvailable = false;
                string clusterPossessCachedContent;
                string tempCurrentCLuster;

                for(int i=0 ; i<SDNController::sdnClass.clusterList.size() ; i++){

                    tempCurrentCLuster = SDNController::sdnClass.clusterList[i];

                    //if cached content is available in the cluster with clusterHead: clusterList[i]
                    if(clusterHeads::clClass.cachedContents[tempCurrentCLuster].find(contentID) != clusterHeads::clClass.cachedContents[tempCurrentCLuster].end()){

                        cacheIsAvailable = true;
                        clusterPossessCachedContent = SDNController::sdnClass.clusterList[i];
                        break;
                    }

                }

                double currentSimTime = simTime().dbl();
                string bestDst;

                if(cacheIsAvailable){

                    SDNController::sdnClass.availablePathFinder(requestMsg->getSrcCluster(), currentSimTime, 0, false, clusterPossessCachedContent,true);
                    bestDst = SDNController::sdnClass.bestPathFinder();

                } else {
                    //there is no cached version of the content, we should send request to the server

                    SDNController::sdnClass.availablePathFinder(requestMsg->getSrcCluster(), currentSimTime, 0, false)
                    bestDst = SDNController::sdnClass.bestPathFinder(); 

                }

                vector<string> tempPath = SDNController::sdnClass.availablePathes[bestDst].path;
                for(int i=0 ; i<tempPath.size() ; i++){
                    packetOutMsg * sdnResponseMsg = new packetOutMsg("sdnResponse");
                    sdnResponseMsg->setBuffer_id(requestMsg->getRequestID());
                    sdnResponseMsg->setHeader("requestMsg");

                    if(i == (tempPath.size()-1)){
                        sdnResponseMsg->setIn_port(tempPath[i].c_str());
                    } else {
                        sdnResponseMsg->setIn_port(tempPath[i+1].c_str());
                    }

                    sdnResponseMsg->setActions_len(1);

                    map<string,string> action;
                   if(i == 0){
                        action["Output"] = bestDst;
                    } else {
                        action["Output"] = tempPath[i-1];
                    }

                   sdnResponseMsg->setAction(action);
                   send(sdnResponseMsg,"gate$o",gateToNextNode[tempPath[i]]);

                }

                requestMsg->setDistanceToCloud(SDNController::sdnClass.hopCountFromSrcToCloud[requestMsg->getSrcCluster()] +1);
                send(requestMsg,"gate$o",gateToNextNode[requestMsg->getSrcCluster()]);

            }
        }

    } else if(strcmp(msg->getClassName(),"packetInMsg") == 0){

        packetInMsg * sdnQueryMsg = check_and_cast<packetInMsg *>(msg);
        string reason = sdnQueryMsg->getReason();

        if(strcmp(reason.c_str(),"newContentGeneration") == 0){

            string source = sdnQueryMsg->getIn_port();
            int contentId = sdnQueryMsg->getBuffer_id();
            int msgLength = sdnQueryMsg->getTotal_len();
            double currentSimTime = simTime().dbl();
            SDNController::sdnClass.bestCacheOptions.clear();

            SDNController::sdnClass.generatedContentDatabase[source][contentId].totalContentSize = msgLength;

            bool thereIsCapableCluster = SDNController::sdnClass.availablePathFinder(source, currentSimTime, msgLength);
            string bestDst;

            if(SDNController::sdnClass.availablePathes.empty()){

                if(thereIsCapableCluster){

                    /*
                     * none of the clusters on the shortest path can not cache the  content but somt other cluster can
                     * thus we we find most suitable cluster to cache the content and forward contnet to the gateway throw this cluster
                     */
                    SDNController::sdnClass.findMostSuitableClusterToCacheTheContent(contentId, msgLength, source, "none");
                    string tempdst = SDNController::sdnClass.bestCacheOptions[contentId].back();
                    SDNController::sdnClass.availablePathFinder(source, currentSimTime, msgLength, true, tempdst, true);

                } else {

                    SDNController::sdnClass.availablePathFinder(source, currentSimTime, msgLength, false);
                    SDNController::sdnClass.bestCacheOptions[contentId].push_back("none");

                }

                bestDst = SDNController::sdnClass.bestPathFinder();

            } else { //a path determined for the content

                bestDst = SDNController::sdnClass.bestPathFinder();

                if(thereIsCapableCluster){

                    if(SDNController::sdnClass.availablePathes[bestDst].path.size() != 1){
                        SDNController::sdnClass.findMostSuitableClusterToCacheTheContent(contentId, msgLength, source, bestDst);
                    } else {
                        SDNController::sdnClass.bestCacheOptions[contentId].push_back(SDNController::sdnClass.availablePathes[bestDst].path[0]);
                    }

                } else {
                    SDNController::sdnClass.bestCacheOptions[contentId].push_back("none");
                }
            }

            vector<string> tempPath = SDNController::sdnClass.availablePathes[bestDst].path;
            for(int i=0 ; i<tempPath.size() ; i++){

                packetOutMsg * sdnResponseMsg = new packetOutMsg(source.c_str());
                sdnResponseMsg->setBuffer_id(contentId);
                sdnResponseMsg->setHeader("ContentMsg");

                if(i == (tempPath.size()-1)){
                    sdnResponseMsg->setIn_port(tempPath[i].c_str());
                } else {
                    sdnResponseMsg->setIn_port(tempPath[i+1].c_str());
                }

                sdnResponseMsg->setActions_len(2);

                map<string,string> action;
                if(i == 0){
                    action["Output"] = bestDst;
                } else {
                    action["Output"] = tempPath[i-1];
                }

                string tempBestCacheOption = SDNController::sdnClass.bestCacheOptions[contentId].back();
                if(strcmp(tempBestCacheOption.c_str(), tempPath[i].c_str()) == 0){
                    action["Experimenter"] = "cache";
                } else {
                    action["Experimenter"] = "none";
                }

                sdnResponseMsg->setAction(action);
                send(sdnResponseMsg,"gate$o",gateToNextNode[tempPath[i]]);

            }

        } else if(strcmp(reason.c_str(),"forwardCachedContent") == 0) {

            string source = sdnQueryMsg->getIn_port();
            string dst = sdnQueryMsg->getHeader();
            int contentId = sdnQueryMsg->getBuffer_id();
            double currentSimTime = simTime().dbl();

            SDNController::sdnClass.availablePathFinder(source, currentSimTime, 0, false, dst);
            string bestDst = SDNController::sdnClass.bestPathFinder();

            vector<string> tempPath = SDNController::sdnClass.availablePathes[bestDst].path;
            for(int i=0 ; i<tempPath.size() ; i++){

                packetOutMsg * sdnResponseMsg = new packetOutMsg("sdnResponse");
                sdnResponseMsg->setBuffer_id(contentId);
                sdnResponseMsg->setHeader("ContentMsg");

                if(i == (tempPath.size()-1)){
                    sdnResponseMsg->setIn_port(tempPath[i].c_str());
                } else {
                    sdnResponseMsg->setIn_port(tempPath[i+1].c_str());
                }

                sdnResponseMsg->setActions_len(2);

                map<string,string> action;
               if(i == 0){
                    action["Output"] = bestDst;
                } else {
                    action["Output"] = tempPath[i-1];
                }

               action["Experimenter"] = "forward";
               sdnResponseMsg->setAction(action);

               send(sdnResponseMsg,"gate$o",gateToNextNode[tempPath[i]]);
            }

            packetOutMsg * sdnResponseMsg = new packetOutMsg("sdnResponse");
            sdnResponseMsg->setBuffer_id(contentId);
            sdnResponseMsg->setHeader("ContentMsg");

            if(tempPath.size() > 0){
                sdnResponseMsg->setIn_port(tempPath[0].c_str());
            } else {
                sdnResponseMsg->setIn_port(bestDst.c_str());
            }

            sdnResponseMsg->setActions_len(2);

            map<string,string> action;
            action["Output"] = sdnQueryMsg->getName();
            action["Experimenter"] = "none";
            sdnResponseMsg->setAction(action);
            send(sdnResponseMsg,"gate$o",gateToNextNode[bestDst]);

        } else if(strcmp(reason.c_str(),"forwardCloudContent") == 0) {

            SDNController::sdnClass.bestCacheOptions.clear();
            string source = sdnQueryMsg->getIn_port();
            int contentId = sdnQueryMsg->getBuffer_id();
            int msgLength = sdnQueryMsg->getTotal_len();
            string dst = sdnQueryMsg->getHeader();
            double currentSimTime = simTime().dbl();
            string producerCluster = SDNController::sdnClass.whichClusterProduceTheContent(contentId);
            double productionTime = SDNController::sdnClass.generatedContentDatabase[producerCluster][contentId].productionTime;

            bool cachingMatters = true;
            if( (currentSimTime - productionTime) > clusterHeads::clClass.contentValidAgeThreshold){
                cachingMatters = false;
            }

            bool thereIsCapableCluster = SDNController::sdnClass.availablePathFinder(source, currentSimTime, msgLength, cachingMatters, dst);

            string bestDst = SDNController::sdnClass.bestPathFinder();

            if(cachingMatters && thereIsCapableCluster){

                if(SDNController::sdnClass.availablePathes[bestDst].path.size() != 1){
                    SDNController::sdnClass.findMostSuitableClusterToCacheTheContent(contentId, msgLength, source, bestDst);
                } else {
                    SDNController::sdnClass.bestCacheOptions[contentId].push_back(SDNController::sdnClass.availablePathes[bestDst].path[0]);
                }

            } else {
                SDNController::sdnClass.bestCacheOptions[contentId].push_back("none");
            }

            vector<string> tempPath = SDNController::sdnClass.availablePathes[bestDst].path;
            for(int i=0 ; i<tempPath.size() ; i++){

                packetOutMsg * sdnResponseMsg = new packetOutMsg("sdnResponse");
                sdnResponseMsg->setBuffer_id(contentId);
                sdnResponseMsg->setHeader("ContentMsg");

                if(i == (tempPath.size()-1)){

                    sdnResponseMsg->setIn_port("blabla");
                } else {
                    sdnResponseMsg->setIn_port(tempPath[i+1].c_str());
                }

                sdnResponseMsg->setActions_len(2);

                map<string,string> action;
                if(i == 0){
                    action["Output"] = bestDst; //final destination
                } else {
                    action["Output"] = tempPath[i-1];
                }

                string tempBestCacheOption = SDNController::sdnClass.bestCacheOptions[contentId].back();
                if(strcmp(tempBestCacheOption.c_str(), tempPath[i].c_str()) == 0){
                    action["Experimenter"] = "cache";
                } else {
                    action["Experimenter"] = "none";
                }

                sdnResponseMsg->setAction(action);

                send(sdnResponseMsg,"gate$o",gateToNextNode[tempPath[i]]);

            }

            packetOutMsg * sdnResponseMsg = new packetOutMsg("sdnResponse");
            sdnResponseMsg->setBuffer_id(contentId);
            sdnResponseMsg->setHeader("ContentMsg");

            if(tempPath.size() > 0){
                sdnResponseMsg->setIn_port(tempPath[0].c_str());
            } else {
                sdnResponseMsg->setIn_port("blabla");;
            }

            sdnResponseMsg->setActions_len(2);

            map<string,string> action;
            action["Output"] = sdnQueryMsg->getName(); 
            action["Experimenter"] = "none";
            sdnResponseMsg->setAction(action);

            send(sdnResponseMsg,"gate$o",gateToNextNode[bestDst]);

        }
    } //if messsage is packetInMsg

}





class gateways : public cSimpleModule
{
private:
    double gatewayDelay = 0.5;
    map<int,string> queue; //msgId, destination , determines this messageId should send to where
    double serverQueueTimer; //messages from server should also put in queue but they have different queue from cluster heads
    map<string,int> gateToNextNode;
protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(gateways);

void gateways::initialize()
{
    SDNController::sdnClass.gatewayList.push_back(getName());
    SDNController::sdnClass.gatewayQueue[getName()] = 0;
    serverQueueTimer = 0;

    for(int i=0; i<gateSize("gate$o"); i++){

        string className = gate("gate$o", i)->getNextGate()->getOwnerModule()->getClassName();
        gateToNextNode[className] = i;

    }

}

void gateways::handleMessage(cMessage *msg)
{

    int messageId;
    requestContentMsg * request;
    contentMsg * content;
    bool isRequestMsg;

    if(strcmp(msg->getClassName(),"requestContentMsg") == 0){

        request = check_and_cast<requestContentMsg *>(msg);
        messageId = request->getRequestID();
        isRequestMsg = true;

    } else if(strcmp(msg->getClassName(),"contentMsg") == 0){

        content = check_and_cast<contentMsg *>(msg);
        messageId = content->getContentID();
        isRequestMsg = false;

    }

    if(msg->isSelfMessage()){

        if(strcmp(msg->getClassName(),"requestContentMsg") == 0){
            request->setHopCount(request->getHopCount()+1);
            send(request,"gate$o",gateToNextNode[queue[messageId]]);
        } else {
            send(content,"gate$o",gateToNextNode[queue[messageId]]);
        }

    } else {
        string moduleType = msg->getSenderModule()->getClassName();

        if(strcmp(moduleType.c_str(),"clusterHeads") == 0){

            queue[messageId] = "cloudServer";

            //if queue is empty
            if(SDNController::sdnClass.gatewayQueue[getName()] < simTime().dbl()){
                SDNController::sdnClass.gatewayQueue[getName()] = simTime().dbl();
            }

            SDNController::sdnClass.gatewayQueue[getName()] += gatewayDelay; 
            if(isRequestMsg){
                scheduleAt(SDNController::sdnClass.gatewayQueue[getName()], request);
            } else {
                scheduleAt(SDNController::sdnClass.gatewayQueue[getName()], content);
            }

        } else if(strcmp(moduleType.c_str(),"cloudServer") == 0){

            queue[messageId] = "clusterHeads";

            if(serverQueueTimer < simTime().dbl()){
                serverQueueTimer = simTime().dbl();
            }

            serverQueueTimer += gatewayDelay;
            if(isRequestMsg){

                scheduleAt(serverQueueTimer, request);
            } else {

                scheduleAt(serverQueueTimer, content);
            }

        }

    }

}





class cloudServer : public cSimpleModule
{
private:
    CloudOperations cloudClass;
    map<string,int> gateToNextNode;
    ofstream output1;
protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(cloudServer);

void cloudServer::initialize()
{
    output1.open("d:/Journal/diversity.txt",ios::out);
    output1<<setw(20)<<left<<"Diversity"<<setw(20)<<"Current Time";
    for(int i=0; i<gateSize("gate$o"); i++){

        string moduleName = gate("gate$o", i)->getNextGate()->getOwnerModule()->getName();
        gateToNextNode[moduleName] = i;

    }
}

void cloudServer::handleMessage(cMessage *msg)
{

    if(strcmp(msg->getClassName(),"requestContentMsg") == 0){

        requestContentMsg * requestMsg = check_and_cast<requestContentMsg *>(msg);
        requestMsg->setHopCount(requestMsg->getHopCount()+1);
        int contentId = requestMsg->getContentID();
        string moduleName = msg->getSenderModule()->getName();


        if(cloudClass.contentsDatabase.find(contentId) != cloudClass.contentsDatabase.end()){
            //content exist on the cloud

            requestMsg->setSatisfied(true);
            requestMsg->setReturnPath(true);

            send(requestMsg,"gate$o",gateToNextNode[moduleName]);

            contentMsg *content = new contentMsg("content");
            content->setContentID(contentId);
            content->setT1ContentExists(cloudClass.contentsDatabase[contentId].t1ContentExists);
            content->setT2ContentExists(cloudClass.contentsDatabase[contentId].t2ContentExists);
            content->setT3ContentExists(cloudClass.contentsDatabase[contentId].t3ContentExists);
            content->setTotalContentSize(cloudClass.contentsDatabase[contentId].totalContentSize);

            send(content,"gate$o",gateToNextNode[moduleName]);

        } else {
            requestMsg->setSatisfied(false);
            requestMsg->setReturnPath(true);
            send(requestMsg,"gate$o",gateToNextNode[moduleName]);

        }

    } else if(strcmp(msg->getClassName(),"contentMsg") == 0){

        contentMsg * content = check_and_cast<contentMsg *>(msg);
        cloudClass.saveContents(content->getContentID(), content->getTotalContentSize(), content->getT1ContentExists(), content->getT1ContentExists(), content->getT1ContentExists());

        double diversity = clusterHeads::clClass.howManyCachedCopyExist();
        output1<<endl<<setw(20)<<left<<diversity<<setw(20)<<simTime();

    }

}







void users::initialize()
{

    output1.open("d:/Journal/results.txt",ios::out);
    output1<<setw(20)<<left<<"Content ID"<<setw(20)<<"Cache exist"<<setw(20)<<"Stretch"<<setw(20)<<"Hop count"<<setw(20)<<"Retrieval Time"<<setw(20)<<"Current Time";

    if(!users::output2.is_open()){
        users::output2.open("d:/Journal/failover.txt",ios::out);
        users::output2<<setw(20)<<left<<"Node name"<<setw(20)<<"Time"<<endl;
    }

    nextRequestedContentID = 1;

    if(users::userClass.requestList.empty()){
        users::userClass.setContentsRequestList();
    }

    double tempTimer = par("warmpUpTime");
    tempTimer += users::userClass.requestList[getName()][nextRequestedContentID].intervalBetweenRequests;

    event = new cMessage("nextRequestTimer");
    scheduleAt(tempTimer, event);
}

void users::handleMessage(cMessage *msg)
{

    if(msg->isSelfMessage()){

        if(clusterHeads::numberOfPowerLessClusters < SDNController::sdnClass.clusterList.size() && ( simTime() < SDNController::sdnClass.maxSimulatonTime ) ){

            requestContentMsg * requestMsg = new requestContentMsg("request for content");
            requestMsg->setUserName(getName());
            requestMsg->setRequestID(nextRequestedContentID);
            requestMsg->setSrcCluster(users::userClass.requestList[getName()][nextRequestedContentID].srcClusterNum.c_str());
            requestMsg->setDstCluster(users::userClass.requestList[getName()][nextRequestedContentID].dstClusterNum.c_str());
            requestMsg->setRequestedContentAge(users::userClass.requestList[getName()][nextRequestedContentID].requestedContentAge);
            send(requestMsg,"gate$o",users::userClass.gateToNextNode[users::userClass.requestList[getName()][nextRequestedContentID].srcClusterNum]);

            users::userClass.pendingRequests[getName()][nextRequestedContentID] = simTime().dbl();
            users::userClass.requestList[getName()].erase(nextRequestedContentID);

            nextRequestedContentID++;
            double tempTimer = users::userClass.requestList[getName()][nextRequestedContentID].intervalBetweenRequests;

            scheduleAt(simTime()+tempTimer, event);

        }

    } else {

        if(strcmp(msg->getClassName(),"requestContentMsg") == 0){

            requestContentMsg * requestMsg = check_and_cast<requestContentMsg *>(msg);
            if(requestMsg->getSatisfied()){

                double tempRetrievalTime = simTime().dbl() - users::userClass.pendingRequests[getName()][requestMsg->getRequestID()];

                output1<<endl<<setw(20)<<left<<requestMsg->getContentID();
                if(requestMsg->getCacheHitHappened()){
                    double stretch = (double)requestMsg->getHopCountUntilCacheHit()/requestMsg->getDistanceToCloud();
                    stretch = (stretch > 1) ? 1:stretch;
                    output1<<setw(20)<<left<<"yes"<<setw(20)<<stretch;
                } else {
                    output1<<setw(20)<<left<<"no"<<setw(20)<<'1';
                }
                output1<<setw(20)<<left<<requestMsg->getHopCount()<<setw(20)<<tempRetrievalTime<<setw(20)<<simTime().dbl();
                users::userClass.pendingRequests[getName()].erase(requestMsg->getRequestID());

            } else {
                users::userClass.pendingRequests[getName()].erase(requestMsg->getRequestID());
            }

        }

    }
}







void nodes::initialize()
{
    int type = par(type);
    int battry = par("battery");
    int storage = par("storage");
    int clusterNumber = par("clusterNumber");
    clusterHeadName = "ch" + to_string(clusterNumber);

    clusterHeads::clClass.clusterMembersStatus[clusterHeadName][getFullName()].nodeType = type;
    clusterHeads::clClass.clusterMembersStatus[clusterHeadName][getFullName()].powerLevel = battry;
    clusterHeads::clClass.clusterMembersStatus[clusterHeadName][getFullName()].storageLevel = storage;
    clusterHeads::clClass.clusterMembersStatus[clusterHeadName][getFullName()].hopCount = clusterHeads::clClass.tempHopCountList.back();

    clusterHeads::clClass.tempHopCountList.pop_back();

    switch(type){
    case 1:
        clusterHeads::clClass.numberOfSimilarNodes[clusterHeadName].numberOfTypeOneNodes++;
        clusterHeads::clClass.type1Storage = storage;
        break;
    case 2:
        clusterHeads::clClass.numberOfSimilarNodes[clusterHeadName].numberOfTypeTwoNodes++;
        clusterHeads::clClass.type2Storage = storage;
        break;
    case 3:
        clusterHeads::clClass.numberOfSimilarNodes[clusterHeadName].numberOfTypeThreeNodes++;
        clusterHeads::clClass.type3Storage = storage;
        break;
    }


    if(SDNController::sdnClass.clustersStatus.find(clusterHeadName) == SDNController::sdnClass.clustersStatus.end()){
        SDNController::sdnClass.clustersStatus[clusterHeadName].powerLevelSum = battry;
        SDNController::sdnClass.clustersStatus[clusterHeadName].storageLevelSum = storage;
    } else {
        SDNController::sdnClass.clustersStatus[clusterHeadName].powerLevelSum += battry;
        SDNController::sdnClass.clustersStatus[clusterHeadName].storageLevelSum += storage;
    }

}

void nodes::handleMessage(cMessage *msg)
{
    if(nodes::firstHandleMessageRun){

        nodes::firstHandleMessageRun = false;
        clusterHeads::clClass.tempHopCountList.clear();

        nodes::relativeEnergyConsumptionBasedOnHopCount[1] = 1;
        nodes::relativeEnergyConsumptionBasedOnHopCount[2] = 1.1;
        nodes::relativeEnergyConsumptionBasedOnHopCount[3] = 1.2;
        nodes::relativeEnergyConsumptionBasedOnHopCount[4] = 1.3;

    }

    clNodesMsg *clusterHeadMsg = check_and_cast<clNodesMsg *>(msg);

    if(strcmp(clusterHeadMsg->getName(),"ContentGeneration")){

        bool generateContent = true;
        int type = par("type");

        if(clusterHeads::clClass.powerConsumedForContentGeneration){

            int producedContentSize;

            switch(type){
            case 1:
                producedContentSize = clusterHeads::clClass.contentGenerationList[clusterHeadMsg->getContentID()].t1ContentSize;
                break;
            case 2:
                producedContentSize = clusterHeads::clClass.contentGenerationList[clusterHeadMsg->getContentID()].t2ContentSize;
                break;
            case 3:
                producedContentSize = clusterHeads::clClass.contentGenerationList[clusterHeadMsg->getContentID()].t3ContentSize;
                break;
            }


            if(clusterHeads::clClass.ConsiderHopCountForPowerConsumedForContentGeneration){

                producedContentSize = producedContentSize * nodes::relativeEnergyConsumptionBasedOnHopCount[clusterHeads::clClass.clusterMembersStatus[clusterHeadName][getFullName()].hopCount];

            }

            if(producedContentSize > clusterHeads::clClass.clusterMembersStatus[clusterHeadName][getFullName()].powerLevel){
                producedContentSize = clusterHeads::clClass.clusterMembersStatus[clusterHeadName][getFullName()].powerLevel;
                generateContent = false; 

                if(find(users::failedNodesList.begin(), users::failedNodesList.end(), getFullName()) == users::failedNodesList.end()) {
                    users::failedNodesList.push_back(getFullName());
                    users::output2<<setw(20)<<left<<getFullName()<<setw(20)<<simTime()<<endl;
                }
            }

            clusterHeads::clClass.clusterMembersStatus[clusterHeadName][getFullName()].powerLevel -= producedContentSize;
            SDNController::sdnClass.clustersStatus[clusterHeadName].powerLevelSum -= producedContentSize;
        }

        if(generateContent){
            //record information related to produced contents

            if(SDNController::sdnClass.generatedContentDatabase[clusterHeadName].find(clusterHeadMsg->getContentID()) == SDNController::sdnClass.generatedContentDatabase[clusterHeadName].end()){

                SDNController::sdnClass.tempStruct.productionTime = clusterHeadMsg->getProductionTime();
                SDNController::sdnClass.tempStruct.t1ContentExists = false;
                SDNController::sdnClass.tempStruct.t2ContentExists = false;
                SDNController::sdnClass.tempStruct.t3ContentExists = false;
                SDNController::sdnClass.generatedContentDatabase[clusterHeadName][clusterHeadMsg->getContentID()] = SDNController::sdnClass.tempStruct;

            }

            switch(type){
            case 1:
                SDNController::sdnClass.generatedContentDatabase[clusterHeadName][clusterHeadMsg->getContentID()].t1ContentExists = true;
                break;
            case 2:
                SDNController::sdnClass.generatedContentDatabase[clusterHeadName][clusterHeadMsg->getContentID()].t2ContentExists = true;
                break;
            case 3:
                SDNController::sdnClass.generatedContentDatabase[clusterHeadName][clusterHeadMsg->getContentID()].t3ContentExists = true;
                break;
            }

            cMessage *msg2 = new cMessage(to_string(clusterHeadMsg->getContentID()).c_str());
            send(msg2,"gate$o",0);
        }

    } //end of content production procedure

}
