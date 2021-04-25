#define WINVER _WIN32_WINNT_VISTA
#define _WIN32_WINNT _WIN32_WINNT_VISTA

#define CROW_MAIN
#include <crow.h>

#include "service.hpp"
#include "payload.hpp"

namespace po=boost::program_options;

Service::Service()
    : ServiceBase(L"BusyBeacon",
                  L"Occupation Beacon Service",
                  SERVICE_DEMAND_START,
                  SERVICE_ERROR_NORMAL,
                  SERVICE_ACCEPT_STOP),
      desc("General options"),
      th_worker()
{
    using namespace std;

    desc.add_options()
        ("port,p", po::value<uint16_t>(&port)->default_value(18080), "Assign port")
        ("addr,a", po::value<string>(&addr)->default_value("0.0.0.0"), "Specify address to bind to")
        ;
}

void Service::onStart(DWORD argc, WCHAR* argv[])
{
    using namespace std;
    using namespace crow;

    handle_args(static_cast<int>(argc), argv, IN_SERVICE);

    th_worker = thread([this]{
        SimpleApp app;

        CROW_ROUTE(app, "/")([](){
            try{
                return crow::response(isbusy() ? "1" : "0");
            }
            catch(system_error& ex)
            {
                return crow::response(ex.what());
            }
            catch(...)
            {
                cerr << "Exception during \"isbusy\" execution" << endl;
            }
            return crow::response("");
        });

        CROW_ROUTE(app, "/detailed")([](){
            json::wvalue resp;
            try{
                vector<json::wvalue> vresp;
                for (auto& i: users_get())
                {
                    json::wvalue val;
                    val["ip"] = i;
                    vresp.emplace_back(std::move(val));
                }
                resp = std::move(vresp);
            }
            catch(system_error& ex)
            {
                return crow::response(ex.what());
            }
            catch(...)
            {
                cerr << "Exception during \"users_get\" execution" << endl;
                return crow::response("");
            }
            return crow::response(resp);
        });

        app.bindaddr(addr).port(port).multithreaded().run();
    });
}

void Service::onStop()
{
    raise(SIGINT);
    th_worker.join();
}

void Service::handle_args(int argc, WCHAR* argv[], ProcPlace place)
{
    using namespace std;

    auto print_desc = [this](){ cout << desc << endl; };

    try {
        po::wparsed_options parsed =
                po::wcommand_line_parser(argc, argv)
                .options(desc)
                .allow_unregistered()
                .run();
        po::store(parsed, vm);
        po::notify(vm);
        if(place == IN_ENTRY) {
            print_desc();
            return;
        }
    }
    catch(exception&) {
        print_desc();
        return;
    }
}

int wmain(int argc, wchar_t* argv[])
{
    Service service;
    service.handle_args(argc, argv, Service::IN_ENTRY);
    service.run();
    return 0;
}
