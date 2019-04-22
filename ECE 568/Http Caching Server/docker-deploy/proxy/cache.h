#include <string>



using namespace std;
class CacheNode{
public:
    string url;
    time_t create_time;
    time_t expire_time;
    bool revalid_required;
    string Etag;
    string response_content;


    CacheNode(){

    }

};