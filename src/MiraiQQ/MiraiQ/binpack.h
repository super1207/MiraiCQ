#pragma once
#include <string>
#include <vector>
class BinPack {
public:
	std::vector<char> content;
	bool string_push(const std::string & str)
	{
		size_t len = str.length();
		if(len > 32767)
		{
			throw std::exception("binpack string_push error");
		}
		int16_push((__int16)len);
		for(size_t i = 0;i < str.length();++i)
		{
			content.push_back(str[i]);
		}
		return true;
	}
	void int16_push(__int16 num)
	{
		content.push_back('\0');
		content.push_back('\0');
		std::memcpy(&content[content.size() - 2], &num, 2);
		std::reverse(content.end()-2,content.end());
	}
	void int64_push(__int64 num)
	{
		content.push_back('\0');
		content.push_back('\0');
		content.push_back('\0');
		content.push_back('\0');
		content.push_back('\0');
		content.push_back('\0');
		content.push_back('\0');
		content.push_back('\0');
		std::memcpy(&content[content.size() - 8], &num, 8);
		std::reverse(content.end()-8,content.end());
	}
	void int32_push(__int32 num)
	{
		content.push_back('\0');
		content.push_back('\0');
		content.push_back('\0');
		content.push_back('\0');
		std::memcpy(&content[content.size() - 4], &num, 4);
		std::reverse(content.end()-4,content.end());
	}

	void bool_push(bool bool_num)
	{
		__int32 num = bool_num;
		content.push_back('\0');
		content.push_back('\0');
		content.push_back('\0');
		content.push_back('\0');
		std::memcpy(&content[content.size() - 4], &num, 4);
	}

	void byte_push(const char * bt,size_t len)
	{
		for(size_t i = 0;i < len;++i)
		{
			content.push_back(bt[i]);
		}
	}

	bool token_push(const std::vector<char> & token)
	{
		size_t len = token.size();
		if(len > 32767)
		{
			return false;
		}
		int16_push((__int16)len);
		if(len == 0)
		{
			return true;
		}
		byte_push(&token[0],len);
		return true;

	}
	/*BinPack() : bytes_(""), curr_(0) {
	}

	explicit BinPack(const std::string &b) : bytes_(b), curr_(0) {
	}

	explicit BinPack(std::string &&b) : bytes_(std::move(b)), curr_(0) {
	}

	size_t size()   {
		return bytes_.size() - curr_;
	}

	bool empty()   {
		return size() == 0;
	}

	template <typename IntType>
	IntType pop_int() {
		constexpr auto size = sizeof(IntType);
		check_enough(size);

		auto s = bytes_.substr(curr_, size);
		curr_ += size;
		std::reverse(s.begin(), s.end());

		IntType result;
		std::memcpy(static_cast<void *>(&result), s.data(), size);
		return result;
	}

	std::string pop_string() {
		const auto len = pop_int<__int16>();
		if (len == 0) {
			return std::string();
		}
		check_enough(len);
		auto result = string_from_coolq(bytes_.substr(curr_, len));
		curr_ += len;
		return result;
	}

	std::string pop_bytes(const size_t len) {
		auto result = bytes_.substr(curr_, len);
		curr_ += len;
		return result;
	}

	std::string pop_token() {
		return pop_bytes(pop_int<__int16>());
	}

	bool pop_bool() {
		return static_cast<bool>(pop_int<__int32>());
	}

private:
	std::string bytes_;
	size_t curr_;

	void check_enough(const size_t needed) const noexcept(false) {
		if (size() < needed) {
			throw std::exception("there aren't enough bytes remained");
		}
	}*/
};