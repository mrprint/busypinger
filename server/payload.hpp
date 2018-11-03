#ifndef PAYLOAD_HPP
#define PAYLOAD_HPP

#include <string>
#include <vector>

using VectorStr = std::vector<std::string>;

bool isbusy();
VectorStr ip_get();
VectorStr users_get();

#endif // PAYLOAD_HPP
