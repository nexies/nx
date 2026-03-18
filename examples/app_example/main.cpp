//
// Created by nexie on 19.02.2026.
//

#include <iostream>
#include <../../modules/core/include/nx/core/App.hpp>

#include "nx/core/Timer.hpp"

#include <nx/core/app/Application.hpp>

struct TimeoutReceiver : public nx::Object
{
    void onTimeout()
    {
        nxInfo("Timeout!");
    }

    NX_SIGNAL(receivedTimeout);
};


namespace run_before_main
{
    class BeforeMainRunner {
    public:
        constexpr
        BeforeMainRunner() noexcept {
            // fprintf(stdout, "BeforeMainRunner: ctor\n");
            // delete this;
        }
    };

    struct property_descriptor {
        static inline std::string_view name = "123";
    };

    template <class ObjectType>
    struct MetaPropertyManager {
        std::vector<int> vec;

        constexpr void
        registerProperty (int i) { vec.push_back(i); }

    };

    template <class ObjectType>
    using MetaProperty = nx::Singleton<MetaPropertyManager<ObjectType>>;



    template<typename ObjectType>
    [[nodiscard]] constexpr auto
    registerProperty (int id) -> decltype(auto) {
        MetaProperty<ObjectType>::Instance().registerProperty(id);
        return id;
    }

    // auto _ = BeforeMainRunner();

    class TestObject {
    public:

        static inline auto m_propertydescriptor = registerProperty<TestObject>(1);
        static inline auto m_propertydescriptor1 = registerProperty<TestObject>(2);
        static inline auto m_propertydescriptor2 = registerProperty<TestObject>(3);
        static inline auto m_propertydescriptor3 = registerProperty<TestObject>(4);
        static inline auto m_propertydescriptor4 = registerProperty<TestObject>(5);
        static inline auto m_propertydescriptor5 = registerProperty<TestObject>(6);
    };
}


int main(int argc, char * argv[]) {
    using namespace nx;
    fprintf(stdout, "main: begin\n");

    using namespace run_before_main;

    std::cerr << TestObject::m_propertydescriptor << std::endl;
    std::cerr << MetaProperty<TestObject>::Instance().vec.size() << std::endl;

    // std::cerr << run_before_main::TestObject::m_propertydescriptor::name;
    // App::Init(argc, argv);

    Timer timer;
    TimeoutReceiver receiver;

    nx::connect(&timer, &Timer::timeout,
        &receiver, &TimeoutReceiver::receivedTimeout, Connection::Queued);
    nx::connect(&timer, &Timer::timeout,
        &receiver, &TimeoutReceiver::receivedTimeout, Connection::Queued);

    nx::connect(&receiver, &TimeoutReceiver::receivedTimeout,
                &receiver, &TimeoutReceiver::onTimeout);

    timer.setDuration(Milliseconds(500));
    timer.setType(Timer::Type::Periodic);
    timer.startNow();

    // auto res = App::Exec();
    // return res;
    return 0;
}