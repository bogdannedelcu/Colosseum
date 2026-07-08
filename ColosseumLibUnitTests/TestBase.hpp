#ifndef msr_ColosseumLibUnitTests_TestBase_hpp
#define msr_ColosseumLibUnitTests_TestBase_hpp

#include <string>
#include <exception>
#include "common/common_utils/Utils.hpp"

namespace colosseum
{

    class TestBase
    {
    public:
        virtual ~TestBase() = default;
        virtual void run() = 0;

        void testAssert(double lhs, double rhs, const std::string& message)
        {
            testAssert(lhs == rhs, message);
        }

        void testAssert(bool condition, const std::string& message)
        {
            if (!condition) {
                common_utils::Utils::DebugBreak();
                throw std::runtime_error(message.c_str());
            }
        }
    };
}
#endif