
#include <iostream>

#include <cassert>
#include <chrono> // std::chrono::seconds
#include <thread> // std::this_thread::sleep_for

#include <servus/listener.h>
#include <servus/servus.h>
#include <servus/uint128_t.h>
#include <servus/version.h>

#ifdef SERVUS_USE_DNSSD
#include <dns_sd.h>
#endif

#ifdef _MSC_VER
#include <windows.h>
#define _sleep Sleep
#else
#define _sleep ::sleep
#endif

namespace
{
static const int _propagationTime = 1000;
static const int _propagationTries = 50;

void printHosts(const servus::Servus& service);
void test(const std::string& serviceType, const std::string& instanceName,
          const std::string& UUID, uint32_t inPort);

void printHosts(const servus::Servus& service)
{
    servus::Strings instances = service.getInstances();

    std::cout << "Service type: " << service.getName() << std::endl;
    for (std::string instance : instances)
    {
        std::cout << "  Service name: " << instance << std::endl;
        std::cout << "  Host: " << service.getHost(instance) << std::endl;

        for (std::string key : service.getKeys(instance))
        {
            std::cout << "  TXT: " << key << "=" << service.get(instance, key)
                      << std::endl;
        }
    }
}

void test(const std::string& serviceType)
{
    const std::string& tmpServiceType = serviceType;

    std::cout << "Starting browsing: service type: " << tmpServiceType
              << std::endl;
    std::cout.flush();

    try
    {
        servus::Servus service(tmpServiceType);
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << "Bailing, no avahi on a Travis CI setup" << std::endl;
        std::cerr << e.what() << std::endl;
        throw;
    }

    servus::Servus service(tmpServiceType);

    std::cout << "Discovering results: " << std::endl;
    int nLoops = _propagationTries;
    while (--nLoops)
    {
        const servus::Strings& hosts =
            service.discover(servus::Servus::IF_ALL, _propagationTime);

        const bool hasUUID =
            !hosts.empty() && service.containsKey(hosts.front(), "UUID");

        if ((hosts.empty() || !hasUUID))
        {
            std::cout << "... " << std::endl;
            continue;
        }

        assert(hosts.size() == 1);
        assert(service.containsKey(hosts.front(), "priority"));
        assert(service.containsKey(hosts.front(), "UUID"));
        assert(service.get(hosts.front(), "priority") == "1");
        assert(service.get("1", "priority").empty());

        printHosts(service);
        //        break;
    }

    // browse if data are in network.
    //        std::this_thread::sleep_for(std::chrono::milliseconds(4000));
    //        std::cout << "Browse results: " << std::endl;
    //
    //        // continuous browse API to browse results.
    //        assert(!service.isBrowsing());
    //        assert(service.beginBrowsing(servus::Servus::IF_LOCAL));
    //        assert(service.isBrowsing());
    //        // BOOST_CHECK_EQUAL gives a link error for Result::PENDING
    //        assert(service.beginBrowsing(servus::Servus::IF_LOCAL) ==
    //                    servus::Servus::Result::PENDING);
    //        assert(service.isBrowsing());
    //
    //        assert(service.browse(_propagationTime) == service.browse(0));
    //        servus::Strings hosts = service.getInstances();
    //        assert(hosts.size() == 1);
    //        assert(!service.get(hosts.front(), "UUID").empty());
    //        assert(service.getKeys().size() == 3);
    //
    //        printHosts(service);

    std::cout << "End test" << std::endl;
    std::cout.flush();
}
}

int main(int argc, char** argv)
{
    std::cout << "Hello, this is sample browser application! \n" << std::endl;

    std::string serviceType = "_daqri-service._tcp";

    test(serviceType);

#ifdef _WIN32
    getchar();
#endif

    return 0;
}
