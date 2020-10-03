#include "autotime_str.h"
#include "binpack.h"
autotime_str::autotime_str()
{
	ts = 10; //10s
}

char * autotime_str::insert( char * new_str )
{
	boost::mutex::scoped_lock lock(mx);

	time_t curr_time = time(0);
	std::vector<std::pair<char *,time_t>> new_vec;
	for(size_t i = 0;i < data_vec.size();++i)
	{
		if(curr_time - data_vec[i].second <= ts)
		{
			new_vec.push_back(data_vec[i]);
		}else
		{
			delete [] data_vec[i].first;
		}
	}
	data_vec = new_vec;
	data_vec.push_back(std::pair<char *,time_t>(new_str,curr_time));
	return new_str;
}

autotime_str::~autotime_str()
{

}
