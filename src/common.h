#ifndef __COMMON_H__

#include <unistd.h>
#include <inttypes.h>
#include <string>
#include <vector>

#define DEBUG_DUMP_RAW

std::vector<std::string> split(const std::string& s, char delimiter);

#endif
