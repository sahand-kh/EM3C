#ifndef CLOPERATIONS_H_
#define CLOPERATIONS_H_
#include <map>
#include <vector>

using namespace std;

class ClOperations {
public:

    struct GeneratedContentDetails{
        double intervalBetweenContentGenerations;
        int t1ContentSize;
        int t2ContentSize;
        int t3ContentSize;
    };

    struct clusterMembers{
        int nodeType;
        int powerLevel;
        int storageLevel;
        int hopCount; //distance from the clusterHead
    };

    struct flowTableDeatils{
        string instructions;
        int priority; //Relative priority of table entry - default 0
        double timeout; //Expiry date
        int counter; //received packets
        string source;
        string destination; //nextNode gate
    };

    struct cachedContentDetails{
        double productionTime;
        int totalContentSize;
        string source;
        map<string,int> hostNodes; //list of <nodes, number of chunks that host by node>
    };

    struct numberOfeachTypeDetails{
        int numberOfTypeOneNodes;
        int numberOfTypeTwoNodes;
        int numberOfTypeThreeNodes;
    };

    ClOperations();
    virtual ~ClOperations();
    int nextGeneratedContentID;
    void setContentGenerationList();
    void setNodesHopCountList();
    int investigateContentForCaching(string clusterHead, int contentId, string sourceCluster, int totalContentSize, double productionTime, double currentSimTime, map<string, int> popularityOfEachCluster, bool useAlpha1 = true);
    void removeOutdateCachedContents(string clusterHead, double currentSimTime);
    int calculateFreeStorageInTheCluster(string clusterHead);
    vector<string> nodeSecelction(string clusterHead, bool useAlpha1 = true);
    double howManyCachedCopyExist();
    map<string, map<int,cachedContentDetails>> cachedContents; //clusterHead, content name
    map<int,GeneratedContentDetails> contentGenerationList;
    map<string, map<string,clusterMembers>> clusterMembersStatus; //clusterHead, name of node in the cluster , nodes details
    map<string, map<string, map<string, map<int,flowTableDeatils>>>> flowTable; //clusterHead, message type: ContentMsg & requestMsg , prevNode(nodes which we recieved message from it), contentName, flow's details
    vector<int> tempHopCountList; //temporary list to hold random hop counts
    map<string,numberOfeachTypeDetails> numberOfSimilarNodes; //clusterHead, number of each type
    bool powerConsumedForContentGeneration;
    bool powerConsumedForContentCaching;
    bool ConsiderHopCountForPowerConsumedForContentGeneration;
    int type1PowerThreshold = 100; 
    int type2PowerThreshold = 500;
    int type3PowerThreshold = 2500;
    int type1StorageThreshold = 2;
    int type2StorageThreshold = 2;
    int type3StorageThreshold = 3;
    double contentValidAgeThreshold = 170;

    //weights related to determining candidate content process
    double weightPopularity = 0.260498;
    double weightContentSize = 0.106156;
    double weightFreshness = 0.633346;

    //weights related to node selection process
    double weightStorage = 0.207096;
    double weightPowerLevel = 0.549616;
    double weightHopCount = 0.08786;
    double weightNumberOfSimilarNodes = 0.155428;
    double alpha1 = 0.7; //node which their suitability value is more than alpha*bestNodeSuitability are choosed as candidate nodes
    double alpha2 = 0.2; //if candidate nodes does not have adequate storage to cache the content, we redefine candidate nodes with alpha2

    int type1Storage;
    int type2Storage;
    int type3Storage;
};

#endif /* CLOPERATIONS_H_ */
