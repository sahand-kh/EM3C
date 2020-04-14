#ifndef SDNOPERATIONS_H_
#define SDNOPERATIONS_H_
#include <string.h>
#include <map>
#include <vector>

using namespace std;

class SdnOperations {
private:
    bool ContentCanCached(int msgLength);
    void calculateHopCountToCloud(string source);
public:
    struct GeneratedContentDetails{
        double productionTime;
        bool t1ContentExists;
        bool t2ContentExists;
        bool t3ContentExists;
        int totalContentSize;
    };

    struct clustersDetails{
        int powerLevelSum;
        int storageLevelSum;
    };

    struct availablePathDetails{
        vector<string> path; //different nodes among src and dst
        double delayTime;
    };

    struct popularityRecordsDetails{
        int popularityCount;
        double recordTime;
    };

    SdnOperations();
    virtual ~SdnOperations();
    void dijkstra();
    int whichContentProduced(double requestedContentAge, string srcCluster, double currentSimTime);
    bool availablePathFinder(string source, double currentSimTime, int msgLength, bool cachingMatters = true, string dst = "none", bool continueToGateways = false);
    string bestPathFinder();
    void findMostSuitableClusterToCacheTheContent(int contentId, int msgLength, string src, string dst = "none");
    void betweennessCalculator();
    void popularityCalculator(double currentSimTime);
    string whichClusterProduceTheContent(int contentId);
    GeneratedContentDetails tempStruct;
    map<string, map<int, GeneratedContentDetails>> generatedContentDatabase; //clusterHead, contentName, contents details
    map<string,vector<string>> neighbors; //node name, list of neighbors
    vector<string> clusterList; //cluster head name
    vector<string> gatewayList; //gateway name
    map<string, map<string, map<string,int>>> nodesConnections; //src, dst, prevNode, dist based on hop (show available connection between nodes)
    map<string, availablePathDetails> availablePathes; //dst, different available paths (this is temporary data structure that removed after each use)
    map<string,clustersDetails> clustersStatus; //clusterHead, cluster details
    map<string,double> gatewayQueue; //gateway name, time when queue finishes
    int acceptableRangeForProductionTimeDif = 50;
    double interClusterChannelDelay = 0.25;
    map<string, int> betweennessRank; //clusterHead, betweenness rank (higher rank means more central)
    double lastPopularityRecordsCleaningTime; //old records
    double popularityCaclulationPeriod = 30;
    double popularityRecordValidTime = 90;
    map<string, vector<double>> popularityRecords; //clusterHead, requests time to that cluster
    map<string, int> popularityOfEachCluster; //clusterHead, popularity rank (higher rank means more popular)
    map<int, vector<string>> bestCacheOptions; //content name, list of clusters
    int maxSimulatonTime = 187200; //130 days
    map<string, int> hopCountFromSrcToCloud; //cluster, distance
};

#endif /* SDNOPERATIONS_H_ */
