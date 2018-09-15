#include <boost/program_options.hpp>
#include "crow.h"
#include "payload.hpp"

namespace po=boost::program_options;

int main(int argc, char* argv[])
{
    using namespace std;

    string addr;
    uint16_t port;

    po::options_description desc("General options");
    desc.add_options()
        ("help,h", "Show help")
        ("port,p", po::value<uint16_t>(&port)->default_value(18080), "Assign port")
        ("addr,a", po::value<string>(&addr)->default_value("0.0.0.0"), "Specify address to bind to")
        ("verb,v", "Be verbose")
        ;

    auto print_desc = [&desc](){ cout << desc << endl; };

    po::variables_map vm;
    try {
        po::parsed_options parsed =
                po::command_line_parser(argc, argv)
                .options(desc)
                .allow_unregistered()
                .run();
        po::store(parsed, vm);
        po::notify(vm);
        if(vm.count("help")) {
            print_desc();
            return 0;
        }
    }
    catch(exception&) {
        print_desc();
        return 0;
    }

    crow::SimpleApp app;

    if(!vm.count("verb")) {
        app.loglevel(crow::LogLevel::Warning);
    }

    CROW_ROUTE(app, "/")([](){
        try{
            return isbusy() ? "1" : "0";
        }
        catch(system_error& ex)
        {
            return ex.what();
        }
    });

    app.bindaddr(addr).port(port).multithreaded().run();
    return 0;
}
