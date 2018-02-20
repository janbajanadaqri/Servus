/* Copyright (c) 2012-2017, Stefan.Eilemann@epfl.ch
 *
 * This file is part of Servus <https://github.com/HBPVIS/Servus>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <chrono>
#include <thread>

#define BOOST_TEST_MODULE servus_servus
#include <boost/test/unit_test.hpp>

#include <servus/servus.h>
#include <servus/uint128_t.h>

#include <random>

#ifdef SERVUS_USE_DNSSD
#include <dns_sd.h>
#endif

#ifdef _MSC_VER
#include <windows.h>
#define _sleep Sleep
#else
#define _sleep ::sleep
#endif

static const int _propagationTime = 1000;
static const int _propagationTries = 20;

namespace
{
void printHosts(const servus::Servus& service)
{
    servus::Strings hosts = service.getInstances();

    for (std::string host : hosts)
    {
        std::cout << "  discovered: " << host << std::endl;
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
    //    const uint32_t port = getRandomPort();
    const uint32_t port = inPort;
    const std::string& tmpServiceType = serviceType;
    const std::string& tmpInstanceName = instanceName;

    std::cout << "Starting announce: service type: " << tmpServiceType
              << ", service name: " << tmpInstanceName << ", port: " << port
              << std::endl;

    try
    {
        servus::Servus service(tmpServiceType);
    }
    catch (const std::runtime_error& e)
    {
        if (getenv("TRAVIS"))
        {
            std::cerr << "Bailing, no avahi on a Travis CI setup" << std::endl;
            BOOST_CHECK_EQUAL(e.what(),
                              "Can't setup avahi client: Daemon not running");
            return;
        }
        throw;
    }

    servus::Servus service(tmpServiceType);
    BOOST_CHECK(!service.isAnnounced());
    const servus::Servus::Result& result =
        service.announce(port, tmpInstanceName);

    BOOST_CHECK_EQUAL(service.getName(), tmpServiceType);

    if (!servus::Servus::isAvailable())
    {
        // BOOST_CHECK_EQUAL gives a link error for Result::NOT_SUPPORTED
        BOOST_CHECK(result == servus::Servus::Result::NOT_SUPPORTED);
        return;
    }

#ifdef SERVUS_USE_DNSSD
    BOOST_CHECK(servus::Result::SUCCESS == kDNSServiceErr_NoError);
#endif
    if (result != servus::Result::SUCCESS) // happens on CI VMs
    {
        std::cerr << "Bailing, got " << result
                  << ": looks like a broken zeroconf setup" << std::endl;
        return;
    }

    BOOST_CHECK_EQUAL(result, result);
    BOOST_CHECK_EQUAL(result.getString(), "success");
    BOOST_CHECK_EQUAL(result.getCode(), 0);
    std::cout << "  announce: " << result << std::endl; // for coverage

    service.withdraw();
    service.set("priority", "1");
    service.set("UUID", UUID);
    BOOST_CHECK_EQUAL(service.get("priority"), "1");
    BOOST_CHECK_EQUAL(service.get("UUID"), UUID);
    BOOST_CHECK_EQUAL(service.get(UUID), std::string());

    BOOST_CHECK(service.announce(port, tmpInstanceName));

    BOOST_CHECK(service.isAnnounced());

    std::this_thread::sleep_for(std::chrono::milliseconds(4000));

    std::cout << "Changing message ..." << std::endl;
    service.set("Test", "Daqri test");

    std::this_thread::sleep_for(std::chrono::milliseconds(4000));

    std::cout << "Browse results: " << std::endl;

    // continuous browse API to browse results.
    BOOST_CHECK(!service.isBrowsing());
    BOOST_CHECK(service.beginBrowsing(servus::Servus::IF_LOCAL));
    BOOST_CHECK(service.isBrowsing());
    // BOOST_CHECK_EQUAL gives a link error for Result::PENDING
    BOOST_CHECK(service.beginBrowsing(servus::Servus::IF_LOCAL) ==
                servus::Servus::Result::PENDING);
    BOOST_CHECK(service.isBrowsing());

    BOOST_CHECK_EQUAL(service.browse(_propagationTime), service.browse(0));
    servus::Strings hosts = service.getInstances();
    BOOST_REQUIRE_EQUAL(hosts.size(), 1);
    BOOST_CHECK_EQUAL(service.get(hosts.front(), "UUID"), UUID);
    BOOST_CHECK_EQUAL(service.getKeys().size(), 3);

    printHosts(service);

    int nLoops = _propagationTries;
    { // test updates during browsing

        servus::Servus service2(tmpServiceType);
        BOOST_CHECK(service2.announce(port + 1, std::to_string(port + 1)));

        nLoops = _propagationTries;
        while (--nLoops)
        {
            BOOST_CHECK(service.browse(_propagationTime));
            hosts = service.getInstances();
            if (hosts.size() < 2 && nLoops > 1)
                continue;

            BOOST_CHECK_EQUAL(hosts.size(), 2);
            break;
        }
    }

    nLoops = _propagationTries;
    while (--nLoops)
    {
        BOOST_CHECK(service.browse(_propagationTime));
        hosts = service.getInstances();
        if (hosts.size() > 1 && nLoops > 1)
            continue;

        BOOST_CHECK_EQUAL(hosts.size(), 1);
        break;
    }

    BOOST_CHECK(service.isBrowsing());
    service.endBrowsing();
    BOOST_CHECK(!service.isBrowsing());

    hosts = service.getInstances();
    BOOST_REQUIRE_EQUAL(hosts.size(), 1);
    BOOST_CHECK_EQUAL(service.get(hosts.front(), "UUID"), UUID);
    BOOST_CHECK_EQUAL(service.getKeys().size(), 3);

    std::cout << "End test" << std::endl;
}
}

BOOST_AUTO_TEST_CASE(test_daqri)
{
    std::cout << "Test Announce" << std::endl;

    std::string serviceType = "_daqri-service._tcp";
    std::string UUID = std::to_string(servus::make_UUID());
    std::string serviceName = "daqri.core." + UUID;

    test(serviceType, serviceName, UUID, 2366);
}
