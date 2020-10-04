#include "msg_id_convert.h"

#include <boost/circular_buffer.hpp>
#include <exception>

static __int32 gs_current_cq_msg_id;

static boost::circular_buffer<std::pair<__int32,__int32>> gs_cb(1024);

MsgIdConvert::MsgIdConvert()
{
	gs_current_cq_msg_id = 0;
}

__int32 MsgIdConvert::to_cq(__int32 web_msg_id)
{
	gs_cb.push_back(std::pair<__int32,__int32>(++gs_current_cq_msg_id,web_msg_id));
	return gs_current_cq_msg_id;
}

__int32 MsgIdConvert::to_web(__int32 cq_msg_id)
{
	for(size_t i = 0;i < gs_cb.size();++i)
	{
		if(gs_cb[i].first == cq_msg_id)
		{
			return gs_cb[i].second;
		}
	}
	throw std::exception("the message_id is not exist!");
}

MsgIdConvert * MsgIdConvert::getInstance()
{
	// MsgIdConvert::MsgIdConvert * MsgIdConvert::getInstance()
	// {
	static MsgIdConvert sm;
	return &sm;
	// }
}

