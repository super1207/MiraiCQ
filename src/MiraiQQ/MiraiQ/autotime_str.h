#ifndef __AUTOTIME_STR_H__QRQMAGMLJXKTEEQV
#define __AUTOTIME_STR_H__QRQMAGMLJXKTEEQV

#include <vector>
#include <ctime>
#include <boost/thread/mutex.hpp>
class autotime_str
{
public:
	autotime_str();
	char * insert(char * new_str);
	~autotime_str();
private:
	std::vector<std::pair<char *,time_t>> data_vec;
	time_t ts;
	boost::mutex mx;
};

#endif