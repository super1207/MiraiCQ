#include "gobal_value.h"

#include <boost/circular_buffer.hpp>
#include <boost/thread/mutex.hpp>

static boost::circular_buffer<char> gs_cb(10000);
static boost::mutex  mx;

void SetLog( char ch )
{
	mx.lock();
	gs_cb.push_back(ch);
	mx.unlock();
}

std::string GetLog()
{
	std::string retStr;
	mx.lock();
	for(size_t i = 0;i < gs_cb.size();++i)
	{
		retStr += gs_cb[i];
		//retStr.append();
	}
	//gs_cb.clear();
	mx.unlock();
	return retStr;
}


//LogDlg g_logdlg;

