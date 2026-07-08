#ifndef msr_ColosseumLibUnitTests_SettingsTest_hpp
#define msr_ColosseumLibUnitTests_SettingsTest_hpp

#include "TestBase.hpp"
#include "common/Settings.hpp"

namespace colosseum
{

    class SettingsTest : public TestBase
    {
    public:
        virtual void run() override
        {
            Settings& settings = Settings::loadJSonFile("settings.json");
            unused(settings);
        }
    };
}
#endif