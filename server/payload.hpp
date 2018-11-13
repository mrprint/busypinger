#ifndef PAYLOAD_HPP
#define PAYLOAD_HPP

#include <string>
#include <vector>

template <typename En, typename Ex>
class RAIInplace
{
    En enter;
    Ex exit;
public:
    RAIInplace(En enter_, Ex exit_) : enter(enter_), exit(exit_) { enter();  }
    ~RAIInplace() { exit(); }
};

using VectorStr = std::vector<std::string>;

bool isbusy();
VectorStr ip_get();
VectorStr users_get();

#endif // PAYLOAD_HPP
