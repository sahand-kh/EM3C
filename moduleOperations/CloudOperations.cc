#include "CloudOperations.h"

using namespace std;

CloudOperations::CloudOperations() {
    // TODO Auto-generated constructor stub

}

CloudOperations::~CloudOperations() {
    // TODO Auto-generated destructor stub
}

void CloudOperations::saveContents(int contentID, int totalContentSize , bool t1ContentExists, bool t2ContentExists, bool t3ContentExists){

    contentsDatabase[contentID].totalContentSize = totalContentSize;
    contentsDatabase[contentID].t1ContentExists = t1ContentExists;
    contentsDatabase[contentID].t2ContentExists = t2ContentExists;
    contentsDatabase[contentID].t3ContentExists = t3ContentExists;

}
