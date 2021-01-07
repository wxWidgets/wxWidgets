// cotire example project

#include "example.h"

#ifndef NDEBUG
#include <algorithm>
#include <iterator>
#endif

namespace example {

std::string get_message() {
	char msg_chrs[] = { 'C', 'o', 't', 'i', 'r', 'e', 'd', '!' };
#ifdef NDEBUG
	return std::string(&msg_chrs[0], &msg_chrs[sizeof(msg_chrs)]);
#else
	std::string msg;
	msg.reserve(sizeof(msg_chrs));
	std::copy(msg_chrs, msg_chrs + sizeof(msg_chrs), std::back_inserter(msg));
	return msg;
#endif
}

}
