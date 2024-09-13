#include "src/server.h"
#include <assert.h>

dx::Logger::ptr g_logger = SERVER_LOG_ROOT();


void test_assert() {
    // assert(0);
    // SERVER_LOG_INFO(g_logger);
    std::cout << dx::BacktraceToString(10, 2, "     ");
    // assert(0 == 1);
    SERVER_ASSERT_ARG(false, "abcdef xx");
}

int main(int argc, char** argv) {
    test_assert();
    return 0;
}
