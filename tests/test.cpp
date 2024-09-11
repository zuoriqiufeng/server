#include <iostream>
#include "src/log.h"


int main(int argc, char** argv)
{
    dx::Logger::ptr logger(new dx::Logger);
    logger->AddAppender(dx::LogAppender::ptr(new dx::StdoutLogAppender));
    // dx::LogEvent::ptr event(new dx::LogEvent(logger,dx::LogLevel::DEBUG, __FILE__, __LINE__, 0, 1, 2, time(0)));
    // event->GetSS() << "hello dx log " << std::endl;
    // logger->Log(dx::LogLevel::DEBUG, event);

    SERVER_LOG_INFO(logger) << "test macro";
    SERVER_LOG_DEBUG(logger) << "test macro";
    SERVER_LOG_WARN(logger) << "test macro";
    SERVER_LOG_ERROR(logger) << "test macro";
    SERVER_LOG_FATAL(logger) << "test macro";
    std::string file = "./log.txt";
    dx::FileLogAppender::ptr file_appender(new dx::FileLogAppender(file));
    dx::LogFormatter::ptr fmt(new dx::LogFormatter("%d%T%m%n"));
    file_appender->SetFormatter(fmt);
    file_appender->SetLevel(dx::LogLevel::ERROR);

    logger->AddAppender(file_appender);

    SERVER_LOG_FMT_ERROR(logger, "test fmt error %s", "aa");
    SERVER_LOG_FMT_DEBUG(logger, "test fmt error %s", "aa");

    auto l = dx::LoggerMgr::GetInstance()->GetLogger("xx");

    SERVER_LOG_INFO(l) << "xxx";
    return 0;
}
