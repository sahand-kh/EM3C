#include "SdnOperations.h"
#include <string.h>
#include <algorithm>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <iostream>

using namespace std;

SdnOperations::SdnOperations() {
    // TODO Auto-generated constructor stub
    lastPopularityRecordsCleaningTime = 0;
}

SdnOperations::~SdnOperations() {
    // TODO Auto-generated destructor stub
}

void SdnOperations::dijkstra() {

     for(int i=0 ; i<clusterList.size() ; i++){

        map<string,int> dist;
        map<string,string> prevNode;

        for(int m=0 ; m<clusterList.size() ; m++){
            dist[clusterList[m]] = INT_MAX;
        }
        for(int m=0 ; m<gatewayList.size() ; m++){
            dist[gatewayList[m]] = INT_MAX;
        }

        int numberOfClusters = clusterList.size();
        numberOfClusters += gatewayList.size();
        vector<string> selectedNodes;
        vector<string> tempNeighbors;
        selectedNodes.push_back(clusterList[i]);
        tempNeighbors = neighbors[clusterList[i]];
        dist[clusterList[i]] = 0;

        for(int n=0 ; n<tempNeighbors.size() ; n++){
            dist[tempNeighbors[n]] = 1;
        }

        //when shortest distance to all destinations found we can stop algorithm for current node
        while(selectedNodes.size() < numberOfClusters){

            int min = INT_MAX;
            string minNode;
            string tempPrevNode;

            //for each selected nodes
            for(int j=0 ; j<selectedNodes.size() ; j++){

                vector<string> tempNeighbors = neighbors[selectedNodes[j]];

                //search its neighbors
                for(int k=0 ; k<tempNeighbors.size() ;k++){

                    if(find(selectedNodes.begin(), selectedNodes.end(), tempNeighbors[k]) != selectedNodes.end()){

                        continue;

                    } else if(dist[tempNeighbors[k]] < min) {

                        min = dist[tempNeighbors[k]];
                        minNode = tempNeighbors[k];
                        tempPrevNode = selectedNodes[j];

                    }

                } //search its neighbors

            } //for each selected nodes

            selectedNodes.push_back(minNode);
            nodesConnections[clusterList[i]][minNode][tempPrevNode] = min;
            vector<string> tempNeighbors2 = neighbors[minNode];

            for(int j=0 ; j<tempNeighbors2.size() ; j++){

                if(find(selectedNodes.begin(), selectedNodes.end(), tempNeighbors2[j]) == selectedNodes.end()){

                    int tempDist = dist[minNode] + 1;
                    if(tempDist < dist[tempNeighbors2[j]]){
                        dist[tempNeighbors2[j]] = tempDist;
                    }

                }

            }//updating dist values after selecting new node

        } //dijestra for current source to all destinations

        calculateHopCountToCloud(clusterList[i]);

    }  //for each cluster

}

bool SdnOperations::availablePathFinder(string source, double currentSimTime, int msgLength, bool cachingMatters, string dst, bool continueToGateways){

    availablePathes.clear();

    bool thereIsCapableCluster;
    if(cachingMatters){
        thereIsCapableCluster = ContentCanCached(msgLength);
    } else {
        thereIsCapableCluster = true;
    }

    if(strcmp(source.c_str(), dst.c_str()) == 0){
        availablePathes[dst].delayTime = 0;
        return thereIsCapableCluster;
    }

    bool clusterCanCacheTheContent;

    //if we want to find a path to gateways
    if(strcmp(dst.c_str(),"none") == 0){

        //for each gateway
        for(int i=0 ; i<gatewayList.size() ; i++){

            if(cachingMatters){
                clusterCanCacheTheContent = false;
            } else {
                clusterCanCacheTheContent = true;
            }

            double delay = gatewayQueue[gatewayList[i]];

            if(delay != 0){
                delay -= currentSimTime;
            }

            //this is happened when queue is empty
            if(delay < 0){
                delay = 0;
            }

            //calculate overall delay to reach the specified gateway
            delay = delay + (nodesConnections[source][gatewayList[i]].begin()->second * interClusterChannelDelay);
            availablePathes[gatewayList[i]].delayTime = delay;
            string tempDst = gatewayList[i];

            //we don't have direct access to specified gateway
            if(strcmp(nodesConnections[source][tempDst].begin()->first.c_str(),source.c_str()) != 0){

                while(strcmp(nodesConnections[source][tempDst].begin()->first.c_str(),source.c_str()) != 0){

                    tempDst = nodesConnections[source][tempDst].begin()->first;
                    availablePathes[gatewayList[i]].path.push_back(tempDst);

                    if( !clusterCanCacheTheContent && clustersStatus[tempDst].powerLevelSum >= msgLength ){
                        clusterCanCacheTheContent = true;
                    }

                }

            }

            tempDst = source;
            availablePathes[gatewayList[i]].path.push_back(tempDst);
            if( !clusterCanCacheTheContent && clustersStatus[tempDst].powerLevelSum >= msgLength ){
                clusterCanCacheTheContent = true;
            }

            if(thereIsCapableCluster && !clusterCanCacheTheContent && cachingMatters){
                availablePathes.erase(gatewayList[i]);
            }

        }
        return thereIsCapableCluster;

    } else { 

        if(continueToGateways){

            string tempDst;

            for(int i=0 ; i<gatewayList.size() ; i++){

                if(cachingMatters){
                    clusterCanCacheTheContent = false;
                } else {
                    clusterCanCacheTheContent = true;
                }

                double delay = gatewayQueue[gatewayList[i]];

                if(delay != 0){
                    delay -= currentSimTime;
                }

                if(delay < 0){
                    delay = 0;
                }

                delay = delay + (nodesConnections[dst][gatewayList[i]].begin()->second * interClusterChannelDelay);
                availablePathes[gatewayList[i]].delayTime = delay;
                tempDst = gatewayList[i];

                if(strcmp(nodesConnections[dst][tempDst].begin()->first.c_str(),dst.c_str()) != 0){

                    while(strcmp(nodesConnections[dst][tempDst].begin()->first.c_str(),dst.c_str()) != 0){

                        tempDst = nodesConnections[dst][tempDst].begin()->first;
                        availablePathes[gatewayList[i]].path.push_back(tempDst);

                        if( !clusterCanCacheTheContent && clustersStatus[tempDst].powerLevelSum >= msgLength ){
                            clusterCanCacheTheContent = true;
                        }

                    }

                }

                   tempDst = dst;
                   availablePathes[gatewayList[i]].path.push_back(tempDst);
                   if( !clusterCanCacheTheContent && clustersStatus[tempDst].powerLevelSum >= msgLength ){
                       clusterCanCacheTheContent = true;
                   }

                   if(thereIsCapableCluster && !clusterCanCacheTheContent && cachingMatters){
                       availablePathes.erase(gatewayList[i]);
                   }

            }

            tempDst = dst;

            if(strcmp(nodesConnections[source][tempDst].begin()->first.c_str(),source.c_str()) != 0){

                while(strcmp(nodesConnections[source][tempDst].begin()->first.c_str(),source.c_str()) != 0){

                    tempDst = nodesConnections[source][tempDst].begin()->first;
                    for(int i=0 ; i<gatewayList.size() ; i++){
                        availablePathes[gatewayList[i]].path.push_back(tempDst);
                    }

                    if( !clusterCanCacheTheContent && clustersStatus[tempDst].powerLevelSum >= msgLength ){
                        clusterCanCacheTheContent = true;
                    }

                }

            }

            tempDst = source;
            for(map<string, availablePathDetails>::iterator it1=availablePathes.begin() ; it1 != availablePathes.end() ; it1++ ){
                it1->second.path.push_back(tempDst);
            }

            if( !clusterCanCacheTheContent && clustersStatus[tempDst].powerLevelSum >= msgLength ){
                clusterCanCacheTheContent = true;
            }


        } else { //we only want to reach destination for instance when we want to access cached content

            if(cachingMatters){
                clusterCanCacheTheContent = false;
            } else {
                clusterCanCacheTheContent = true;
            }

            string tempDst = dst;

            if(strcmp(nodesConnections[source][tempDst].begin()->first.c_str(),source.c_str()) != 0){

                while(strcmp(nodesConnections[source][tempDst].begin()->first.c_str(),source.c_str()) != 0){

                    tempDst = nodesConnections[source][tempDst].begin()->first;
                    availablePathes[dst].path.push_back(tempDst);

                    if( !clusterCanCacheTheContent && clustersStatus[tempDst].powerLevelSum >= msgLength ){
                        clusterCanCacheTheContent = true;
                    }

                }

            }

            tempDst = source;
            availablePathes[dst].path.push_back(tempDst);
            if( !clusterCanCacheTheContent && clustersStatus[tempDst].powerLevelSum >= msgLength ){
                clusterCanCacheTheContent = true;
            }

            thereIsCapableCluster = clusterCanCacheTheContent;

        }

        return thereIsCapableCluster;

    }
}

string SdnOperations::bestPathFinder(){

    double minDelay = DBL_MAX;
    string bestDst;

    for(map<string, availablePathDetails>::iterator it1 = availablePathes.begin() ; it1 != availablePathes.end() ; it1++){

        if(it1->second.delayTime < minDelay){
            minDelay = it1->second.delayTime;
            bestDst = it1->first;
        }

    }

    return bestDst;
}

void SdnOperations::findMostSuitableClusterToCacheTheContent(int contentId, int msgLength, string src, string dst){

    map<string, map<string,double>> matrix;

    string tempCLusterHeadName;
    int tempHopCount;
    double powerUpperBound;
    double storageUpperBound;
    double hopCountUpperBound;
    double palsiUpperBound;
    double powerLowerBound;
    double storageLowerBound;
    double hopCountLowerBound;
    double palsiLowerBound;
    double maxPowerLevel = 0;
    double minPowerLevel = DBL_MAX;
    double maxStorageLevel = 0;
    double minStorageLevel = DBL_MAX;
    double maxHopCount = 0;
    double minHopCount = DBL_MAX;
    double maxPalsi = 0;
    double minPalsi = DBL_MAX;
    map<string, double> relativeCloseness; //clusterhead, value

    double powerLevelSum = 0;
    double storageLevelSum = 0;
    double hopCountSum = 0;
    double palsiSum = 0;
    double kEntropy;
    map<string, map<string,double>> entropyMatrix;
    map<string, double> convergenceEntropy;
    map<string, double> weightEntropy;
    vector<string> tempPath;

    if(strcmp(dst.c_str(),"none") == 0){ //we want to find most suitable cluster in the network

        tempPath = clusterList;

    } else { //we want to find suitable cluster in a specific path

        tempPath = availablePathes[dst].path;

    }

    for(int i = 0 ; i<tempPath.size() ; i++){

        tempCLusterHeadName = tempPath[i];

        //it is possible that some of the clusters on the path does not have adequate power to cache the content and thus should not participate in calculations
        if(clustersStatus[tempCLusterHeadName].powerLevelSum < msgLength){
            continue;
        } else { 

            matrix[tempCLusterHeadName]["powerLevel"] = clustersStatus[tempCLusterHeadName].powerLevelSum;
            powerLevelSum += matrix[tempCLusterHeadName]["powerLevel"];
            //it is possible that both if's are satisfied
            if(matrix[tempCLusterHeadName]["powerLevel"] > maxPowerLevel){
                maxPowerLevel = matrix[tempCLusterHeadName]["powerLevel"];
            }
            if(matrix[tempCLusterHeadName]["powerLevel"] < minPowerLevel){
                minPowerLevel = matrix[tempCLusterHeadName]["powerLevel"];
            }

            matrix[tempCLusterHeadName]["storageLevel"] = clustersStatus[tempCLusterHeadName].storageLevelSum;
            storageLevelSum += matrix[tempCLusterHeadName]["storageLevel"];
            if(matrix[tempCLusterHeadName]["storageLevel"] > maxStorageLevel){
                maxStorageLevel = matrix[tempCLusterHeadName]["storageLevel"];
            }
            if(matrix[tempCLusterHeadName]["storageLevel"] < minStorageLevel){
                minStorageLevel = matrix[tempCLusterHeadName]["storageLevel"];
            }

            if(strcmp(tempCLusterHeadName.c_str(),src.c_str()) == 0){
                tempHopCount = 0;
            } else {
                tempHopCount = nodesConnections[src][tempCLusterHeadName].begin()->second;
            }
            matrix[tempCLusterHeadName]["hopCount"] = tempHopCount;
            hopCountSum += matrix[tempCLusterHeadName]["hopCount"];
            if(matrix[tempCLusterHeadName]["hopCount"] > maxHopCount){
                maxHopCount = matrix[tempCLusterHeadName]["hopCount"];
            }
            if( matrix[tempCLusterHeadName]["hopCount"] < minHopCount ){
                minHopCount = matrix[tempCLusterHeadName]["hopCount"];
            }

            // N - |p - b| / N , PALSI is a positive criteria
            matrix[tempCLusterHeadName]["palsi"] = (double)(clusterList.size() - abs(popularityOfEachCluster[tempCLusterHeadName] - betweennessRank[tempCLusterHeadName])) / clusterList.size();
            palsiSum += matrix[tempCLusterHeadName]["palsi"];
            if(matrix[tempCLusterHeadName]["palsi"] > maxPalsi){
                maxPalsi = matrix[tempCLusterHeadName]["palsi"];
            }
            if(matrix[tempCLusterHeadName]["palsi"] <minPalsi){
                minPalsi = matrix[tempCLusterHeadName]["palsi"];
            }

        }
    }

    //fill Entropy matrix and also normalize the values
    for(map<string, map<string,double>>::iterator it1 = matrix.begin() ; it1 != matrix.end() ; it1++){
        entropyMatrix[it1->first]["powerLevel"] = it1->second["powerLevel"] / powerLevelSum;
        entropyMatrix[it1->first]["storageLevel"] = it1->second["storageLevel"] / storageLevelSum;
        entropyMatrix[it1->first]["hopCount"] = it1->second["hopCount"] / hopCountSum;
        entropyMatrix[it1->first]["palsi"] = it1->second["palsi"] / palsiSum;
    }


    convergenceEntropy["powerLevel"] = 0;
    convergenceEntropy["storageLevel"] = 0;
    convergenceEntropy["hopCount"] = 0;
    convergenceEntropy["palsi"] = 0;


    for(map<string, map<string,double>>::iterator it1 = entropyMatrix.begin() ; it1 != entropyMatrix.end() ; it1++){
        //Entropy calculations
        if(it1->second["powerLevel"] != 0){
            convergenceEntropy["powerLevel"] = convergenceEntropy["powerLevel"] + (it1->second["powerLevel"] * log(it1->second["powerLevel"]));
        }
        if(it1->second["storageLevel"] != 0){
            convergenceEntropy["storageLevel"] = convergenceEntropy["storageLevel"] + (it1->second["storageLevel"] * log(it1->second["storageLevel"]));
        }
        if(it1->second["hopCount"] != 0){
            convergenceEntropy["hopCount"] = convergenceEntropy["hopCount"] + (it1->second["hopCount"] * log(it1->second["hopCount"]));
        }
        if(it1->second["palsi"] != 0){
            convergenceEntropy["palsi"] = convergenceEntropy["palsi"] + (it1->second["palsi"] * log(it1->second["palsi"]));
        }
    }

    if(matrix.size() != 1){
        kEntropy = 1 / log(entropyMatrix.size());
    } else {
        kEntropy = DBL_MAX;
    }
    kEntropy = kEntropy * (-1);

    convergenceEntropy["powerLevel"] *= kEntropy;
    convergenceEntropy["storageLevel"] *= kEntropy;
    convergenceEntropy["hopCount"] *= kEntropy;
    convergenceEntropy["palsi"] *= kEntropy;

    convergenceEntropy["powerLevel"] = 1 - convergenceEntropy["powerLevel"];
    convergenceEntropy["storageLevel"] = 1 - convergenceEntropy["storageLevel"];
    convergenceEntropy["hopCount"] = 1 - convergenceEntropy["hopCount"];
    convergenceEntropy["palsi"] = 1 - convergenceEntropy["palsi"];

    double tempSum = convergenceEntropy["powerLevel"] + convergenceEntropy["storageLevel"] + convergenceEntropy["hopCount"] + convergenceEntropy["palsi"];

    weightEntropy["powerLevel"] = convergenceEntropy["powerLevel"] / tempSum;
    weightEntropy["storageLevel"] = convergenceEntropy["storageLevel"] / tempSum;
    weightEntropy["hopCount"] = convergenceEntropy["hopCount"] / tempSum;
    weightEntropy["palsi"] = convergenceEntropy["palsi"] / tempSum;




    //normalization procedure for TOPSIS matrix
    if(maxPowerLevel != minPowerLevel){
        powerUpperBound = (maxPowerLevel - minPowerLevel) / maxPowerLevel;
        powerLowerBound = minPowerLevel / maxPowerLevel;
    } else {
        powerUpperBound = 1;
        powerLowerBound = 0;
    }

    if(maxStorageLevel != minStorageLevel){
        storageUpperBound = (maxStorageLevel - minStorageLevel) / maxStorageLevel;
        storageLowerBound = minStorageLevel / maxStorageLevel;
    } else {
        storageUpperBound = 1;
        storageLowerBound = 0;
    }

    if(maxPalsi != minPalsi){
        palsiUpperBound = (maxPalsi - minPalsi) / maxPalsi;
        palsiLowerBound = minPalsi / maxPalsi;
    } else {
        palsiUpperBound = 1;
        palsiLowerBound = 0;
    }

    if(maxHopCount != minHopCount){
        hopCountUpperBound = (maxHopCount - minHopCount) / maxHopCount;
        hopCountLowerBound = minHopCount / maxHopCount;
    } else {
        hopCountUpperBound = 1;
        hopCountLowerBound = 0;
    }

    for(map<string, map<string,double>>::iterator it1 = matrix.begin() ; it1 != matrix.end() ; it1++){

        if(maxPowerLevel != minPowerLevel){
            it1->second["powerLevel"] = powerLowerBound + ( ((it1->second["powerLevel"] - minPowerLevel) * (powerUpperBound - powerLowerBound)) /(maxPowerLevel - minPowerLevel));
        } else { //in this case all should have the same value
            it1->second["powerLevel"] = 1.0;
        }

        if(maxStorageLevel != minStorageLevel){
            it1->second["storageLevel"] = storageLowerBound + ( ((it1->second["storageLevel"] - minStorageLevel) * (storageUpperBound - storageLowerBound)) /(maxStorageLevel - minStorageLevel));
        } else {
            it1->second["storageLevel"] = 1.0;
        }

        if(maxHopCount != minHopCount){
            it1->second["hopCount"] = hopCountLowerBound + ( ((maxHopCount - it1->second["hopCount"]) * (hopCountUpperBound - hopCountLowerBound)) /(maxHopCount - minHopCount));
        } else {
            it1->second["hopCount"] = 1.0;
        }

        if(maxPalsi != minPalsi){
            it1->second["palsi"] = palsiLowerBound + ( ((it1->second["palsi"] - minPalsi) * (palsiUpperBound - palsiLowerBound)) /(maxPalsi - minPalsi));
        } else {
            it1->second["palsi"] = 1.0;
        }

        //create weighted matrix
        it1->second["powerLevel"] *= weightEntropy["powerLevel"];
        it1->second["storageLevel"] *= weightEntropy["storageLevel"];
        it1->second["hopCount"] *= weightEntropy["hopCount"];
        it1->second["palsi"] *= weightEntropy["palsi"];

    }

    maxPowerLevel = 0;
    minPowerLevel = DBL_MAX;
    maxStorageLevel = 0;
    minStorageLevel = DBL_MAX;
    maxHopCount = 0;
    minHopCount = DBL_MAX;
    maxPalsi = 0;
    minPalsi = DBL_MAX;

    //re-calculate min and max
    for(map<string, map<string,double>>::iterator it1 = matrix.begin() ; it1 != matrix.end() ; it1++){

        if( it1->second["powerLevel"] > maxPowerLevel ){
            maxPowerLevel = it1->second["powerLevel"];
        }
        if( it1->second["powerLevel"] < minPowerLevel ){
            minPowerLevel = it1->second["powerLevel"];
        }
        if( it1->second["storageLevel"] > maxStorageLevel ){
            maxStorageLevel = it1->second["storageLevel"];
        }
        if( it1->second["storageLevel"] < minStorageLevel ){
            minStorageLevel = it1->second["storageLevel"];
        }
        if( it1->second["hopCount"] > maxHopCount ){
            maxHopCount = it1->second["hopCount"];
        }
        if( it1->second["hopCount"] < minHopCount ){
            minHopCount = it1->second["hopCount"];
        }
        if( it1->second["palsi"] > maxPalsi ){
            maxPalsi = it1->second["palsi"];
        }
        if( it1->second["palsi"] < minPalsi ){
            minPalsi = it1->second["palsi"];
        }

    }

    double dplus,dminus;
    for(map<string, map<string,double>>::iterator it1 = matrix.begin() ; it1 != matrix.end() ; it1++){

        dplus =0;
        dminus = 0;

        dplus += pow((it1->second["powerLevel"] - maxPowerLevel), 2);
        dminus += pow((it1->second["powerLevel"] - minPowerLevel), 2);

        dplus += pow((it1->second["storageLevel"] - maxStorageLevel), 2);
        dminus += pow((it1->second["storageLevel"] - minStorageLevel), 2);

        dplus += pow((it1->second["hopCount"] - maxHopCount), 2);
        dminus += pow((it1->second["hopCount"] - minHopCount), 2);

        dplus += pow((it1->second["palsi"] - maxPalsi), 2);
        dminus += pow((it1->second["palsi"] - minPalsi), 2);

        dplus = sqrt(dplus);
        dminus = sqrt(dminus);

        relativeCloseness[it1->first] = dminus / (dminus + dplus);

    }

    double clusterSuitabilityValue;
    int tempNumber = relativeCloseness.size();

    //sort clusters based on their suitability to cache the content
    for(int i=0 ; i<tempNumber ; i++){

        clusterSuitabilityValue = DBL_MAX;

        for( map<string, double>::iterator it1 = relativeCloseness.begin() ; it1 != relativeCloseness.end() ; it1++){
            if(it1->second < clusterSuitabilityValue){
                clusterSuitabilityValue = it1->second;
                tempCLusterHeadName = it1->first;
            }
        }

        bestCacheOptions[contentId].push_back(tempCLusterHeadName);
        relativeCloseness.erase(tempCLusterHeadName);

    }

}

int SdnOperations::whichContentProduced(double requestedContentAge, string srcCluster, double currentSimTime){

    map<int, GeneratedContentDetails> generatedContentList = generatedContentDatabase[srcCluster];

    double minTimeDif = DBL_MAX;
    double requestedContentTimeOfProduction = abs(currentSimTime - requestedContentAge);
    int contentName;

    for(map<int, GeneratedContentDetails>::iterator it1 = generatedContentList.begin() ; it1 != generatedContentList.end() ; it1++){

        double timeDif = abs(requestedContentTimeOfProduction - it1->second.productionTime);

        if( timeDif < minTimeDif){

            minTimeDif = timeDif;
            contentName = it1->first;

        } else {
            break;
        }
    }

    if(minTimeDif < acceptableRangeForProductionTimeDif){
        return contentName;
    } else {
        return 0;
    }

}

void SdnOperations::betweennessCalculator(){

    int numberOfShortestPath = 0;
    string tempDst;
    map<string, double> betweenness;

    for(int k=0 ; k<clusterList.size() ; k++){
        betweenness[clusterList[k]] = 0;
    }

    for(int i=0 ; i<clusterList.size() ; i++){
        for(int j=i+1 ; j<clusterList.size() ; j++){

            tempDst = clusterList[j];
            numberOfShortestPath++;

            while(strcmp(nodesConnections[clusterList[i]][tempDst].begin()->first.c_str(),clusterList[i].c_str()) != 0){
                tempDst = nodesConnections[clusterList[i]][tempDst].begin()->first;
                betweenness[tempDst]++;
            }

        }
    }

    for(int i=0 ; i<clusterList.size() ; i++){
        for(int j=0 ; j<gatewayList.size() ; j++){

            tempDst = gatewayList[j];
            numberOfShortestPath++;

            while(strcmp(nodesConnections[clusterList[i]][tempDst].begin()->first.c_str(),clusterList[i].c_str()) != 0){
                tempDst = nodesConnections[clusterList[i]][tempDst].begin()->first;
                betweenness[tempDst]++;
            }

        }
    }

    //make betweenness value normalized
    for(map<string, double>::iterator it1=betweenness.begin() ; it1 != betweenness.end() ; it1++ ){
        it1->second /= numberOfShortestPath;
    }

    string tempClName, prevCl;
    double prevClBetweenness;
    double tempMinValue;
    bool firstRun = true;

    //rank clusters based on their betweenness
    for(int i=0 ; i<clusterList.size() ; i++){
        tempMinValue = DBL_MAX;

        for(map<string, double>::iterator it1=betweenness.begin() ; it1 != betweenness.end() ; it1++ ){
            if(it1->second < tempMinValue){
                tempMinValue = it1->second;
                tempClName = it1->first;
            }
        }


        if(firstRun){
            firstRun = false;
            prevClBetweenness = betweenness[tempClName];
            betweennessRank[tempClName] = 1;
            prevCl = tempClName;
        } else {

            if(betweenness[tempClName] > prevClBetweenness){
                betweennessRank[tempClName] = i+1;
            } else {
                betweennessRank[tempClName] = betweennessRank[prevCl];
            }

            prevClBetweenness = betweenness[tempClName];
            prevCl = tempClName;

        }

        betweenness.erase(tempClName);

    }

}

bool SdnOperations::ContentCanCached(int msgLength){

    bool thereIsCapableCluster = false;

    for(map<string,clustersDetails>::iterator it1 = clustersStatus.begin() ; it1 != clustersStatus.end() ; it1++){
        if(it1->second.powerLevelSum >= msgLength){
            thereIsCapableCluster = true;
            break;
        }
    }

    return thereIsCapableCluster;

}

void SdnOperations::popularityCalculator(double currentSimTime){

    if(currentSimTime == 0){
        for(int i=0 ; i<clusterList.size() ; i++){
                popularityOfEachCluster[clusterList[i]] = 1;
            }
    } else {

        lastPopularityRecordsCleaningTime = currentSimTime;
        int lastRemovedRecordIndex;

//        remove invalide records (old records)
        for(map<string, vector<double>>::iterator it1 = popularityRecords.begin() ; it1 != popularityRecords.end() ; it1++){
            lastRemovedRecordIndex = -1;

            for(int i=0 ; i<it1->second.size() ; i++){
                if((currentSimTime - it1->second[i]) > popularityRecordValidTime){
                    lastRemovedRecordIndex = i;
                }
            }

            if(lastRemovedRecordIndex != -1){
                lastRemovedRecordIndex++;
                it1->second.erase(it1->second.begin(), it1->second.begin()+lastRemovedRecordIndex);
            }

        }

        for(int i=0 ; i<clusterList.size() ; i++){
            popularityOfEachCluster[clusterList[i]] = popularityRecords[clusterList[i]].size();
        }

        map<string, int> tempPopularityRankingOfCLusters = popularityOfEachCluster;
        int tempMinValue;
        string tempClName, prevCl;
        int prevClPopularity;
        bool firstRun = true;


        for(int i=0 ; i<clusterList.size() ; i++){

            tempMinValue = INT_MAX;

            for(map<string, int>::iterator it1=tempPopularityRankingOfCLusters.begin() ; it1 != tempPopularityRankingOfCLusters.end() ; it1++){
                if(it1->second < tempMinValue){
                    tempMinValue = it1->second;
                    tempClName = it1->first;
                }
            }

            if(firstRun){
                firstRun = false;
                prevClPopularity = tempPopularityRankingOfCLusters[tempClName];
                popularityOfEachCluster[tempClName] = 1;
                prevCl = tempClName;
            } else {

                if(tempPopularityRankingOfCLusters[tempClName] > prevClPopularity){
                    popularityOfEachCluster[tempClName] = i+1;
                } else {
                    popularityOfEachCluster[tempClName] = popularityOfEachCluster[prevCl];
                }

                prevClPopularity = tempPopularityRankingOfCLusters[tempClName];
                prevCl = tempClName;

            }

            tempPopularityRankingOfCLusters.erase(tempClName);
         }
    }

}

string SdnOperations::whichClusterProduceTheContent(int contentId){

    string producerCluster = "none";

    for(map<string, map<int, GeneratedContentDetails>>::iterator it1 = generatedContentDatabase.begin() ; it1 != generatedContentDatabase.end() ; it1++){

        map<int, GeneratedContentDetails> tempContentList = it1->second;
        for(map<int, GeneratedContentDetails>::iterator it2 = tempContentList.begin() ; it2 != tempContentList.end() ; it2++){

            if(it2->first == contentId){
                producerCluster = it1->first;
                break;
            }
        }

        if(strcmp(producerCluster.c_str(), "none") != 0){
            break;
        }

    }

    return producerCluster;

}


void SdnOperations::calculateHopCountToCloud(string source){
    int minDist = INT_MAX;
    string closestGateway;

    for(int i=0 ; i<gatewayList.size() ; i++){

        int dist = nodesConnections[source][gatewayList[i]].begin()->second;
        if(dist < minDist){
            minDist = dist;
            closestGateway = gatewayList[i];
        }
    }
    hopCountFromSrcToCloud[source] = minDist;
}
