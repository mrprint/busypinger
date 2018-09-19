#ifndef SERVICE_HPP
#define SERVICE_HPP

#include <string>
#include <boost/program_options.hpp>
#include <crow.h>
#include "service_base.hpp"

class Service: public ServiceBase, public crow::SimpleApp
{
public:
    using ServiceBase::run;
    using ProcPlace = enum {
        IN_ENTRY,
        IN_SERVICE
    };

    Service(const Service& other) = delete;
    Service& operator=(const Service& other) = delete;

    Service(Service&& other) = delete;
    Service& operator=(Service&& other) = delete;

    Service();
    void handle_args(int argc, WCHAR* argv[], ProcPlace place);
private:
    std::string addr;
    std::uint16_t port;
    boost::program_options::options_description desc;
    boost::program_options::variables_map vm;
    std::thread th_worker;

    void onStart(DWORD argc, WCHAR* argv[]) override;
    void onStop() override;
};

#endif // SERVICE_HPP
