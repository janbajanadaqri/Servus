
#include <iostream>

#include <cassert>
#include <chrono>         // std::chrono::seconds
#include <thread>         // std::this_thread::sleep_for

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

namespace {

    static const int _propagationTime = 1000;
    static const int _propagationTries = 20;

    void printHosts(const servus::Servus& service);
    void test(const std::string& serviceType, const std::string& instanceName,
              const std::string& UUID, uint32_t inPort);

    void printHosts(const servus::Servus& service)
    {
        servus::Strings hosts = service.getInstances();

        std::cout << "Service type: " << service.getName() << std::endl;
        for (std::string host : hosts)
        {
            std::cout << "  Service name: " << host << std::endl;
            for (std::string key : service.getKeys())
            {
                std::cout << "  TXT: " << key << "=" << service.get(host, key)
                          << std::endl;
            }
        }
    }

    void test(const std::string& serviceType, const std::string& instanceName,
              const std::string& UUID, uint32_t inPort)
    {
        const uint32_t port = inPort;
        const std::string& tmpServiceType = serviceType;
        const std::string& tmpInstanceName = instanceName;

        std::cout << "Starting announce: service type: " << tmpServiceType
                  << ", service name: " << tmpInstanceName << ", port: " << port
                  << std::endl;
        std::cout.flush();

        try
        {
            servus::Servus service(tmpServiceType);
        }
        catch (const std::runtime_error& e)
        {
            std::cerr << "Bailing, no avahi on a Travis CI setup" << std::endl;
            std::cerr << e.what() << std::endl;\
            throw;
        }

        servus::Servus service(tmpServiceType);

        service.set("priority", "1");
        service.set("UUID", UUID);

        assert(!service.isAnnounced());
        assert(service.get("priority") == "1");
        assert(service.get("UUID") == UUID);
        assert(service.get(UUID) == std::string());

        const servus::Servus::Result& result =
                service.announce(port, tmpInstanceName);
        assert(service.isAnnounced());

        assert(service.getName() == tmpServiceType);

        if (!servus::Servus::isAvailable())
        {
            // BOOST_CHECK_EQUAL gives a link error for Result::NOT_SUPPORTED
            assert(result == servus::Servus::Result::NOT_SUPPORTED);
            std::cerr << "Service is not available" << std::endl;
            return;
        }

#ifdef SERVUS_USE_DNSSD
        assert(servus::Result::SUCCESS == kDNSServiceErr_NoError);
#endif
        if (result != servus::Result::SUCCESS) // happens on CI VMs
        {
            std::cerr << "Bailing, got " << result
                      << ": looks like a broken zeroconf setup" << std::endl;
            return;
        }

        assert(result == result);
        assert(result.getString() == "success");
        assert(result.getCode() == 0);
        std::cout << "  announce: " << result << std::endl; // for coverage

        //changing message
        std::this_thread::sleep_for(std::chrono::milliseconds(4000));
        std::cout << "Changing message ..." << std::endl;
        service.set("Test", "Daqri test");

        //browse if data are in network.
        std::this_thread::sleep_for(std::chrono::milliseconds(4000));
        std::cout << "Browse results: " << std::endl;

        // continuous browse API to browse results.
        assert(!service.isBrowsing());
        assert(service.beginBrowsing(servus::Servus::IF_LOCAL));
        assert(service.isBrowsing());
        // BOOST_CHECK_EQUAL gives a link error for Result::PENDING
        assert(service.beginBrowsing(servus::Servus::IF_LOCAL) ==
                    servus::Servus::Result::PENDING);
        assert(service.isBrowsing());

        assert(service.browse(_propagationTime) == service.browse(0));
        servus::Strings hosts = service.getInstances();
        assert(hosts.size() == 1);
        assert(service.get(hosts.front(), "UUID") == UUID);
        assert(service.getKeys().size() == 3);

        printHosts(service);

        service.withdraw();
        std::cout << "End test" << std::endl;
        std::cout.flush();
    }

}

int main(int argc, char **argv) {
    std::cout << "Hello, this is announce server sample application!" << std::endl;

    std::string serviceType = "_daqri-service._tcp";
    std::string UUID = std::to_string(servus::make_UUID());
    std::string serviceName = "daqri.core." + UUID;

    test(serviceType, serviceName, UUID, 2366);

#ifdef _WIN32
	getchar();
#endif

    return 0;
}
