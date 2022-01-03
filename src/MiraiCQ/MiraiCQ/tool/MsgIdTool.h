#include <string>
#include <mutex>
#include <list>
#include <jsoncpp/json.h>
class MsgIdTool
{
public:
	int to_cqid(const Json::Value& webid);
	Json::Value to_webid(int cqid);
	static MsgIdTool * getInstance();
private:
	MsgIdTool();
	std::mutex mx;
	std::list<std::pair<int, Json::Value>> idlist;
	int curcqid = 1;
	size_t maxidnum = 512;
};