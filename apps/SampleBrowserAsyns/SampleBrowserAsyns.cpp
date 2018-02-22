
#include <iostream>

#include <chrono> // std::chrono::seconds
#include <thread> // std::this_thread::sleep_for

#include <servus/listener.h>
#include <servus/servus.h>
#include <servus/uint128_t.h>
//#include <servus/version.h>

namespace app {
static const std::string TEST_DAQRI_SERVICE = "_daqri-test._tcp";

class ServiceModel : public servus::Listener {
public:
    explicit ServiceModel(servus::Servus& service)
        : service_(service)
    {
        std::cout << "Service type: " << service_.getName() << std::endl;

        service_.addListener(this);
        service_.beginBrowsing(servus::Servus::IF_ALL);
    }

    ~ServiceModel() final
    {
        std::cout << "ServiceModel: ~ServiceModel!" << std::endl;

        service_.removeListener(this);
        service_.endBrowsing();
    }

    void instanceAdded(const std::string& instance) final
    {
        std::cout << "Add" << std::endl;
        std::cout << "  Service name: " << instance << std::endl;
        std::cout << "  Address: " << service_.getHost(instance) << std::endl;

        const std::vector<std::string>& keys = service_.getKeys(instance);
        for (const std::string& key : keys) {
            std::cout << "  TXT: " << key << "=" << service_.get(instance, key)
                      << std::endl;
        }
    }

    void instanceRemoved(const std::string& instance) final
    {
        std::cout << "Remove  \n  " << instance << std::endl;
    }

    void printHosts()
    {
        const servus::Servus& service = service_;
        servus::Strings instances = service.getInstances();

        std::cout << "Service type: " << service.getName() << std::endl;
        for (std::string instance : instances) {
            std::cout << "  Service name: " << instance << std::endl;
            std::cout << "  Address: " << service.getHost(instance)
                      << std::endl;

            for (std::string key : service.getKeys(instance)) {
                std::cout << "  TXT: " << key << "="
                          << service.get(instance, key) << std::endl;
            }
        }
    }

private:
    servus::Servus& service_;
};
}

int main(int argc, char** argv)
{
    std::cout << "Hello, this is sample browser async application! \n"
              << std::endl;

    std::unique_ptr<servus::Servus> service;
    std::unique_ptr<app::ServiceModel> model;

    //    const std::string searchService = "_zeroeq_pub._tcp";
    const std::string searchService = app::TEST_DAQRI_SERVICE;

    const auto onServiceChanged = [&]() {
        const std::string& serviceName = searchService;
        if (service && service->getName() == serviceName) {
            return;
        }

        std::cout << "Starting browsing service \n"
                  << std::endl;
        service.reset(new servus::Servus(serviceName));
        model.reset(new app::ServiceModel(*service));

    };

    onServiceChanged();

    // beginBrowsing() and endBrowsing() is here called only ones.
    // browse() will detect if new instance was added or removed.
    // warning: in this case browse() doesn't detect if service instance changed description
    // TXT for example. This should not be needed.

    //You can use in Linux to test same results.
    //avahi-browse -d local _daqri-test._tcp --resolve

    // This should run in thread.
    int nLoops = 100;
    while (--nLoops) {
        const auto& res = service->browse(-1);

        //        model->printHosts();

        //        if (res == servus::Servus::Result::SUCCESS)
        //        {
        //            std::cout << "SUCCESS" << std::endl;
        //        }
        //        else if (res == servus::Servus::Result::PENDING)
        //        {
        //            std::cout << "PENDING" << std::endl;
        //        }
        //        else if (res == servus::Servus::Result::NOT_SUPPORTED)
        //        {
        //            std::cout << "NOT_SUPPORTED" << std::endl;
        //        }
        //        else if (res == servus::Servus::Result::POLL_ERROR)
        //        {
        //            std::cout << "POLL_ERROR" << std::endl;
        //        }

        //        std::this_thread::sleep_for(std::chrono::seconds(500));
    }

    return 0;
}
