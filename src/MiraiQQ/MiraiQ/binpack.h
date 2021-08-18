#ifndef __BINPACK_H__JYDAYACBDBHPWTBR
#define  __BINPACK_H__JYDAYACBDBHPWTBR

#include <string>
#include <vector>

class BinPack {
public:
	void string_push(const std::string & str);
	void int16_push(__int16 num);
	void int64_push(__int64 num);
	void int32_push(__int32 num);
	void bool_push(bool bool_num);
	void byte_push(const char * bt,size_t len);
	void token_push(const std::vector<char> & token);
	BinPack(const std::vector<char> & content);
	BinPack();
	__int64 int64_pop();
	__int16 int16_pop();
	std::string string_pop();
	std::vector<char> content;
private:
	size_t curr;
};

#endif