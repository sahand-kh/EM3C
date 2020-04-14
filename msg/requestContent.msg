message requestContentMsg
{
    string userName;
    string srcCluster;
    string dstCluster;
    double requestedContentAge;
    int requestID; //this is a number to trace the request
    int contentID = 0;
    bool satisfied = false; //does any content produced in that time?
    bool returnPath = false; //this request is on the 
    int hopCount = 0;
    int hopCountUntilCacheHit = 0;
    bool cacheHitHappened = false; //if requested content is returned from cache storage
    int distanceToCloud;
}