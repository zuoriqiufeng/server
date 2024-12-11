
#include "src/server.h"

dx::Logger::ptr g_logger = SERVER_LOG_ROOT();

int main(int argc, char** argv) {
    dx::Scheduler sc;
    sc.Start();
    sc.Stop();
    return 0;
}
