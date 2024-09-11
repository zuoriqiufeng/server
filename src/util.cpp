#include "util.h"
#include <syscall.h>
#include <csignal>
#include <iostream>
#include <zconf.h>
namespace dx {
pid_t GetThreadId() {
    return (pid_t)syscall(SYS_gettid);
}

uint32_t GetFiberId() {

    return 0;
}

}
