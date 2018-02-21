/* Copyright (c) 2015, Human Brain Project
 *                     Daniel.Nachbaur@epfl.ch
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

#include <iostream>

#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds

#include <servus/listener.h>
#include <servus/servus.h>
#include <servus/uint128_t.h>
//#include <servus/version.h>


class ServiceModel : public servus::Listener {

public:

    ServiceModel(servus::Servus& service): service_(service) {
        std::cout << "ServiceModel: ServiceModel: " <<  service_.getName() << std::endl;

        service_.addListener(this);
        service_.beginBrowsing(servus::Servus::IF_LOCAL);
    }

    ~ServiceModel() {
        std::cout << "ServiceModel: ~ServiceModel!" << std::endl;

        service_.removeListener(this);
        service_.endBrowsing();
    }

    void instanceAdded(const std::string& instance) final
    {
        std::cout << "ServiceModel: instanceAdded: " << instance << std::endl;

        const std::vector<std::string>& keys = service_.getKeys(instance);
        for (const std::string& key : keys)
        {
            std::cout << "key: " << key << ": " << service_.get(instance, key) << std::endl;
        }
    }

    void instanceRemoved(const std::string& instance) final
    {
        std::cout << "ServiceModel: instanceRemoved: " << instance << std::endl;

    }

private:
    servus::Servus& service_;

};

int main(int argc, char **argv) {
    std::cout << "Hello, World!" << std::endl;

    std::unique_ptr<servus::Servus> service;
    std::unique_ptr<ServiceModel> model;

//    const std::string searchService = "_zeroeq_pub._tcp";
    const std::string searchService = "_ssh._tcp";

    const auto onServiceChanged = [&]() {
        const std::string &serviceName = searchService;
        if (service && service->getName() == serviceName) {
            return;
        }

        std::cout << "Hello, reset" << std::endl;
        service.reset(new servus::Servus(serviceName));
        model.reset(new ServiceModel(*service));

    };

    onServiceChanged();

//    for (int i=10; i>0; --i) {
//        onServiceChanged();
//        std::this_thread::sleep_for (std::chrono::seconds(1));
//    }

    return 0;
}
