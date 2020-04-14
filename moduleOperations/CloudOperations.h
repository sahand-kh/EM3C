#ifndef MODULEOPERATIONS_CLOUDOPERATIONS_H_
#define MODULEOPERATIONS_CLOUDOPERATIONS_H_
#include <map>

using namespace std;

class CloudOperations {
public:
    struct contentDetails{
        bool t1ContentExists;
        bool t2ContentExists;
        bool t3ContentExists;
        int totalContentSize;
    };

    CloudOperations();
    virtual ~CloudOperations();
    void saveContents(int contentID, int totalContentSize , bool t1ContentExists, bool t2ContentExists, bool t3ContentExists);
    map<int,contentDetails> contentsDatabase;
};

#endif /* MODULEOPERATIONS_CLOUDOPERATIONS_H_ */
