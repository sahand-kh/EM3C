#include "ClOperations.h"

#include <string.h>
#include <fstream>
#include <algorithm>
#include <float.h>
#include <iostream>

using namespace std;

ClOperations::ClOperations() {
    // TODO Auto-generated constructor stub

    nextGeneratedContentID = 1;
    powerConsumedForContentGeneration = true;
    ConsiderHopCountForPowerConsumedForContentGeneration = true;
    powerConsumedForContentCaching = false;

}

ClOperations::~ClOperations() {
    // TODO Auto-generated destructor stub
}

void ClOperations::setContentGenerationList() {

    ifstream inputFile1;
    inputFile1.open("D:/Journal/contentGenerationDetail.txt",ios::in);
    inputFile1.ignore(10000, '\n');

    string temp1,temp2,temp3,temp4,temp5;

    while(inputFile1>>temp1){

        inputFile1>>temp2;
        inputFile1>>temp3;
        inputFile1>>temp4;
        inputFile1>>temp5;

        contentGenerationList[stoi(temp1)].intervalBetweenContentGenerations = stod(temp2);
        contentGenerationList[stoi(temp1)].t1ContentSize = stoi(temp3);
        contentGenerationList[stoi(temp1)].t2ContentSize = stoi(temp4);
        contentGenerationList[stoi(temp1)].t3ContentSize = stoi(temp5);
    }

}

void ClOperations::setNodesHopCountList() {

    ifstream inputFile1;
    inputFile1.open("D:/Journal/nodesHopCountDetail.txt",ios::in);

    string temp1;
    while(inputFile1>>temp1){
        tempHopCountList.push_back( stoi(temp1) );
    }

}

int ClOperations::investigateContentForCaching(string clusterHead, int contentId, string sourceCluster, int totalContentSize, double productionTime, double currentSimTime, map<string, int> popularityOfEachCluster, bool useAlpha1){

    removeOutdateCachedContents(clusterHead, currentSimTime);

    if(cachedContents[clusterHead].find(contentId) != cachedContents[clusterHead].end()){
        //content has been cached before
    } else if( (currentSimTime - productionTime) > contentValidAgeThreshold){
        //we can not cache the content because it is invalid
    } else {

        vector<string> candidateNodes = nodeSecelction(clusterHead, useAlpha1);
        if(candidateNodes.size() == 0){
            //there is no capable node for caching the content
        } else {

            int freeStorage = 0;
            int storageCapacitySum = 0;
            bool thereAreEnoughFreeStorage = false;
            int index = -1;

            for(int i=0 ; i<candidateNodes.size() ; i++){

                int type = clusterMembersStatus[clusterHead][candidateNodes[i]].nodeType;
                switch(type){
                case 1:
                    storageCapacitySum += type1Storage;
                    break;
                case 2:
                    storageCapacitySum += type2Storage;
                    break;
                case 3:
                    storageCapacitySum += type3Storage;
                    break;
                }

                freeStorage += clusterMembersStatus[clusterHead][candidateNodes[i]].storageLevel;
                if(freeStorage >= totalContentSize){
                    thereAreEnoughFreeStorage = true;
                    index = i;
                    break;
                }
            }

            if(storageCapacitySum < totalContentSize){

                if(useAlpha1){

                    investigateContentForCaching(clusterHead, contentId, sourceCluster, totalContentSize, productionTime, currentSimTime, popularityOfEachCluster, false);

                }

            }else if(thereAreEnoughFreeStorage){

                cachedContents[clusterHead][contentId].productionTime = productionTime;
                cachedContents[clusterHead][contentId].source = sourceCluster;
                cachedContents[clusterHead][contentId].totalContentSize = totalContentSize;
                int tempTotalContentSize = totalContentSize;

                for(int i=0 ; i<=index ;i++){

                    freeStorage = clusterMembersStatus[clusterHead][candidateNodes[i]].storageLevel;
                    if(tempTotalContentSize >= freeStorage){

                        clusterMembersStatus[clusterHead][candidateNodes[i]].storageLevel = 0;
                        if(strcmp(clusterHead.c_str(), sourceCluster.c_str()) != 0 && powerConsumedForContentCaching){
                            clusterMembersStatus[clusterHead][candidateNodes[i]].powerLevel -= freeStorage;
                        }
                        cachedContents[clusterHead][contentId].hostNodes[candidateNodes[i]] = freeStorage;
                        tempTotalContentSize -= freeStorage;

                    } else {
                        //in this case only a portion of storage is used for caching the content

                        clusterMembersStatus[clusterHead][candidateNodes[i]].storageLevel -= tempTotalContentSize;
                        if(strcmp(clusterHead.c_str(), sourceCluster.c_str()) != 0 && powerConsumedForContentCaching){
                            clusterMembersStatus[clusterHead][candidateNodes[i]].powerLevel -= tempTotalContentSize;
                        }
                        cachedContents[clusterHead][contentId].hostNodes[candidateNodes[i]] = tempTotalContentSize;
                        break;

                    }
                    if(powerConsumedForContentCaching && clusterMembersStatus[clusterHead][candidateNodes[i]].powerLevel < 0){
                        clusterMembersStatus[clusterHead][candidateNodes[i]].powerLevel = 0;
                    }
                }

            } else {
                //in this case some of previously cached contents should be removed if we want to cache the content

                map<int,cachedContentDetails> tempCachedContents = cachedContents[clusterHead];
                map<int,double> contentValue; //contentId, its value
                double currentContentValue;

                int popularitySum = popularityOfEachCluster[sourceCluster];
                double contentSizeSum = (1.0 / totalContentSize);
                double tempAge = (currentSimTime - productionTime);
                double freshnessSum = (contentValidAgeThreshold - tempAge);

                for( map<int,cachedContentDetails>::iterator it1 = tempCachedContents.begin() ; it1 != tempCachedContents.end() ; it1++){

                    popularitySum += popularityOfEachCluster[it1->second.source];
                    contentSizeSum += (1.0 / it1->second.totalContentSize);
                    tempAge = currentSimTime - it1->second.productionTime;
                    freshnessSum += (contentValidAgeThreshold - tempAge);

                }

                currentContentValue = ( (popularityOfEachCluster[sourceCluster] / popularitySum) * weightPopularity );
                tempAge = (currentSimTime - productionTime);
                tempAge = (contentValidAgeThreshold - tempAge);
                currentContentValue += ( (tempAge / freshnessSum) * weightFreshness );
                currentContentValue += ( ((1.0 /totalContentSize) / contentSizeSum) * weightContentSize );

                bool locatedOnCandidateNodes;
                double percentageLocatedOnCandidateNodes;
                for( map<int,cachedContentDetails>::iterator it1 = tempCachedContents.begin() ; it1 != tempCachedContents.end() ; it1++){

                    locatedOnCandidateNodes = false;
                    percentageLocatedOnCandidateNodes = 0;
                    map<string,int> hostNodes = it1->second.hostNodes;
                    for(map<string,int>::iterator it2 = hostNodes.begin() ; it2 != hostNodes.end() ; it2++){

                        if(find(candidateNodes.begin() , candidateNodes.end(), it2->first) != candidateNodes.end()){
                            //if this portion of cached content is located on candidate nodes

                            locatedOnCandidateNodes = true;
                            percentageLocatedOnCandidateNodes += it2->second;
                        }
                    }

                    if(locatedOnCandidateNodes){

                        percentageLocatedOnCandidateNodes = percentageLocatedOnCandidateNodes / it1->second.totalContentSize;

                        contentValue[it1->first] = ( (popularityOfEachCluster[it1->second.source] / popularitySum) * weightPopularity );
                        tempAge = currentSimTime - it1->second.productionTime;
                        tempAge = (contentValidAgeThreshold - tempAge);
                        contentValue[it1->first] += ( (tempAge / freshnessSum) * weightFreshness );
                        contentValue[it1->first] += ( ((1.0 /it1->second.totalContentSize) / contentSizeSum) * weightContentSize );

                        contentValue[it1->first] *= percentageLocatedOnCandidateNodes;

                    }
                }

                double min;
                int minValueContentId;
                vector<int> sortetContents;
                int tempSize = contentValue.size();
                map<int,double> tempContentValue = contentValue;
                for(int i=0 ; i<tempSize; i++){

                    min = DBL_MAX;
                    for(map<int,double>::iterator it1 = tempContentValue.begin() ; it1 != tempContentValue.end() ; it1++){

                        if(it1->second <= min){

                            min = it1->second;
                            minValueContentId = it1->first;
                        }
                    }

                    sortetContents.push_back(minValueContentId);
                    tempContentValue.erase(minValueContentId);

                }

                tempSize = sortetContents.size();
                int freeStorageRequired = totalContentSize - freeStorage; //size of content - available free space on candidate nodes
                contentSizeSum = 0;
                double contentValueSum = 0;

                for(int i=0 ; i<tempSize ; i++){

                    contentValueSum += contentValue[sortetContents[i]];
                    contentSizeSum += cachedContents[clusterHead][sortetContents[i]].totalContentSize;
                    index = i;

                    if(contentSizeSum >= freeStorageRequired){
                        break;
                    }
                }

                if(contentValueSum < currentContentValue){

                    cachedContents[clusterHead][contentId].productionTime = productionTime;
                    cachedContents[clusterHead][contentId].source = sourceCluster;
                    cachedContents[clusterHead][contentId].totalContentSize = totalContentSize;
                    int tempTotalContentSize = totalContentSize;

                    //remove contents which are not valuable enough
                    for(int i=0 ; i<index ; i++){

                        map<string,int> hostNodes = cachedContents[clusterHead][sortetContents[i]].hostNodes;
                        for(map<string,int>::iterator it1 = hostNodes.begin() ; it1 != hostNodes.end() ; it1++){

                            clusterMembersStatus[clusterHead][it1->first].storageLevel += it1->second;
                            freeStorage = clusterMembersStatus[clusterHead][it1->first].storageLevel;

                            if(tempTotalContentSize == 0){
                                continue;
                            } else if(tempTotalContentSize >=  freeStorage){

                                clusterMembersStatus[clusterHead][it1->first].storageLevel = 0;
                                if(strcmp(clusterHead.c_str(), sourceCluster.c_str()) != 0 && powerConsumedForContentCaching){
                                    clusterMembersStatus[clusterHead][it1->first].powerLevel -= freeStorage;
                                }
                                cachedContents[clusterHead][contentId].hostNodes[it1->first] = freeStorage;
                                tempTotalContentSize -= freeStorage;

                            } else {
                                clusterMembersStatus[clusterHead][it1->first].storageLevel -= tempTotalContentSize;
                                if(strcmp(clusterHead.c_str(), sourceCluster.c_str()) != 0 && powerConsumedForContentCaching){
                                    clusterMembersStatus[clusterHead][it1->first].powerLevel -= tempTotalContentSize;
                                }
                                cachedContents[clusterHead][contentId].hostNodes[it1->first] = tempTotalContentSize;
                                tempTotalContentSize = 0;
                            }

                            if(powerConsumedForContentCaching && clusterMembersStatus[clusterHead][it1->first].powerLevel < 0){
                                clusterMembersStatus[clusterHead][it1->first].powerLevel = 0;
                            }
                        }
                    }

                } else {
                    //dont cache the content
                }

            }
        }

    }

    int availableFreeStorage = calculateFreeStorageInTheCluster(clusterHead);
    return availableFreeStorage;
}


vector<string> ClOperations::nodeSecelction(string clusterHead, bool useAlpha1){

    double storageSum = 0;
    double powerLevelSum = 0;
    double hopCountSum = 0;
    double numberOfSimilarNodesSum = 0;
    int numberOfSimilarNodesInThisCluster;

    vector<string> capableNodes;

    map<string,clusterMembers> tempClusterMembers = clusterMembersStatus[clusterHead];
    for(map<string,clusterMembers>::iterator it1 = tempClusterMembers.begin() ; it1 != tempClusterMembers.end() ; it1++){

        int tempStorageThreshold, tempPowerThreshold;
        switch(it1->second.nodeType){
        case 1:
            tempStorageThreshold = type1StorageThreshold;
            tempPowerThreshold = type1PowerThreshold;
            numberOfSimilarNodesInThisCluster = numberOfSimilarNodes[clusterHead].numberOfTypeOneNodes;
            break;
        case 2:
            tempStorageThreshold = type2StorageThreshold;
            tempPowerThreshold = type2PowerThreshold;
            numberOfSimilarNodesInThisCluster = numberOfSimilarNodes[clusterHead].numberOfTypeTwoNodes;
            break;
        case 3:
            tempStorageThreshold = type3StorageThreshold;
            tempPowerThreshold = type3PowerThreshold;
            numberOfSimilarNodesInThisCluster = numberOfSimilarNodes[clusterHead].numberOfTypeThreeNodes;
            break;
        }


        if(it1->second.storageLevel < tempStorageThreshold){
            continue;
        } else if(it1->second.powerLevel < tempPowerThreshold){
            continue;
        }

        storageSum += it1->second.storageLevel;
        powerLevelSum += it1->second.powerLevel;
        hopCountSum += it1->second.hopCount;
        numberOfSimilarNodesSum += numberOfSimilarNodesInThisCluster;
        capableNodes.push_back(it1->first);

    }

    hopCountSum = 1.0 / hopCountSum;

    map<string,double> nodesSuitabilityValue;
    string tempNodeName;
    double max = 0;

    //calculating each node's suitability
    for(int i=0 ; i<capableNodes.size() ; i++){

        tempNodeName = capableNodes[i];

        nodesSuitabilityValue[tempNodeName] = ((clusterMembersStatus[clusterHead][tempNodeName].storageLevel /storageSum) * weightStorage );
        nodesSuitabilityValue[tempNodeName] += ((clusterMembersStatus[clusterHead][tempNodeName].powerLevel /powerLevelSum) * weightPowerLevel );

        switch(clusterMembersStatus[clusterHead][tempNodeName].nodeType){
        case 1:
            numberOfSimilarNodesInThisCluster = numberOfSimilarNodes[clusterHead].numberOfTypeOneNodes;
            break;
        case 2:
            numberOfSimilarNodesInThisCluster = numberOfSimilarNodes[clusterHead].numberOfTypeTwoNodes;
            break;
        case 3:
            numberOfSimilarNodesInThisCluster = numberOfSimilarNodes[clusterHead].numberOfTypeThreeNodes;
            break;
        }

        nodesSuitabilityValue[tempNodeName] += ((numberOfSimilarNodesInThisCluster / numberOfSimilarNodesSum) * weightNumberOfSimilarNodes );
        nodesSuitabilityValue[tempNodeName] += (((1.0 / clusterMembersStatus[clusterHead][tempNodeName].hopCount) /hopCountSum) * weightHopCount );
        if(nodesSuitabilityValue[tempNodeName] > max){
            max = nodesSuitabilityValue[tempNodeName];
        }

    }

    double alphaValue = (useAlpha1) ? alpha1 : alpha2;
    vector<string> candidateNodes;
    //selecting candidate nodes
    while(!capableNodes.empty()){

        tempNodeName = capableNodes.back();

        if(nodesSuitabilityValue[tempNodeName] >= (max * alphaValue)){
            candidateNodes.push_back(tempNodeName);
        }

        capableNodes.pop_back();
    }

    return candidateNodes;

}

void ClOperations::removeOutdateCachedContents(string clusterHead, double currentSimTime){

    map<int,cachedContentDetails> tempMap = cachedContents[clusterHead];

    for(map<int,cachedContentDetails>::iterator it1 = tempMap.begin() ; it1 != tempMap.end() ; it1++){
        if( (currentSimTime - it1->second.productionTime) > contentValidAgeThreshold ){

            map<string,int> hostNodes = it1->second.hostNodes;
            for(map<string,int>::iterator it2 = hostNodes.begin() ; it2 != hostNodes.end() ; it2++){
                clusterMembersStatus[clusterHead][it2->first].storageLevel += it2->second;
            }

            cachedContents[clusterHead].erase(it1->first);

        }
    }

}

int ClOperations::calculateFreeStorageInTheCluster(string clusterHead){

    int availableStorage = 0;
    map<string,clusterMembers> tempClusterMembers = clusterMembersStatus[clusterHead];

    for(map<string,clusterMembers>::iterator it1 = tempClusterMembers.begin() ; it1 != tempClusterMembers.end() ; it1++){

        availableStorage += it1->second.storageLevel;
    }

    return availableStorage;
}


double ClOperations::howManyCachedCopyExist(){

    map<int,double> numberOfCachedCopy;

    for( map<string, map<int,cachedContentDetails>>::iterator it1 = cachedContents.begin() ; it1 != cachedContents.end() ; it1++){
        map<int,cachedContentDetails> tempCachedContentOnCluster = it1->second;

        for( map<int,cachedContentDetails>::iterator it2 = tempCachedContentOnCluster.begin() ; it2 != tempCachedContentOnCluster.end() ; it2++){
            if(numberOfCachedCopy.find(it2->first) == numberOfCachedCopy.end()){
                numberOfCachedCopy[it2->first] = 1;
            } else {
                numberOfCachedCopy[it2->first]++;
            }
        }
    }

    double numberOfCopies = 0;
    for( map<int,double>::iterator it1 = numberOfCachedCopy.begin() ; it1 != numberOfCachedCopy.end() ; it1++){
        numberOfCopies += it1->second;
    }

    double diversity = numberOfCachedCopy.size() / numberOfCopies;
    return diversity;

}
