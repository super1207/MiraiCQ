#include "MsgIdTool.h"

inline MsgIdTool::MsgIdTool()
{

}

int MsgIdTool::to_cqid(const Json::Value & webid)
{
	if (!(webid.isInt64() || webid.isString()))
	{
		throw std::runtime_error("wrong msg_id type");
	}
	std::lock_guard<std::mutex> lock(mx);
	++curcqid;
	if (curcqid == INT_MAX)
	{
		curcqid = 1;
	}
	idlist.push_back({ curcqid ,webid });
	if (idlist.size() > maxidnum)
	{
		idlist.pop_front();
	}
	return curcqid;
}

Json::Value MsgIdTool::to_webid(int cqid)
{
	std::lock_guard<std::mutex> lock(mx);
	for (const auto& it : idlist)
	{
		if (it.first == cqid)
		{
			return it.second;
		}
	}
	throw std::runtime_error("msg_id not found");
}

MsgIdTool* MsgIdTool::getInstance()
{
	static MsgIdTool msg_id_tool;;
	return &msg_id_tool;
}

