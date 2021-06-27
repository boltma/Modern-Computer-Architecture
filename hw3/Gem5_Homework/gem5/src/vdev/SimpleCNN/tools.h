#pragma once

#define SWAP32(x)  ((( (x) & 0x000000FF) << 24) | \
		(((x) & 0x0000FF00) << 8) | \
		(((x) & 0x00FF0000) >> 8) | \
		(((x) & 0xFF000000) >> 24))

class simulator_error : public std::runtime_error
{
public:
	simulator_error(std::string const& msg) : std::runtime_error(msg) {}
};

struct Range
{
	Range(int beg, int end) :begin(beg), end(end) {}
	int begin = 0;
	int end = 0;
};