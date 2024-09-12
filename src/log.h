#ifndef __DX_LOG_H__
#define __DX_LOG_H__

#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <sstream>
#include <fstream>
#include <vector>
#include <iostream>
#include "util.h"
#include <map>
#include "singleton.h"
#include "thread.h"

#define SERVER_LOG_LEVEL(logger, level) \
    if(logger->GetLevel() <= level) \
        dx::LogEventWrap(dx::LogEvent::ptr(new dx::LogEvent(logger, level, __FILE__, __LINE__, 0, dx::GetThreadId(), \
        dx::GetFiberId(), time(0), ""))).GetSS()

#define SERVER_LOG_DEBUG(logger) SERVER_LOG_LEVEL(logger, dx::LogLevel::DEBUG)
#define SERVER_LOG_INFO(logger) SERVER_LOG_LEVEL(logger, dx::LogLevel::INFO)
#define SERVER_LOG_WARN(logger) SERVER_LOG_LEVEL(logger, dx::LogLevel::WARN)
#define SERVER_LOG_ERROR(logger) SERVER_LOG_LEVEL(logger, dx::LogLevel::ERROR)
#define SERVER_LOG_FATAL(logger) SERVER_LOG_LEVEL(logger, dx::LogLevel::FATAL)

#define SERVER_LOG_FMT_LEVEL(logger, level, fmt, ...) \
    if(logger->GetLevel() <= level) \
        dx::LogEventWrap(dx::LogEvent::ptr(new dx::LogEvent(logger, level, __FILE__, __LINE__, 0, dx::GetThreadId(), \
        dx::GetFiberId(), time(0), ""))).GetEvent()->Format(fmt, __VA_ARGS__)

#define SERVER_LOG_FMT_DEBUG(logger, fmt, ...) SERVER_LOG_FMT_LEVEL(logger, dx::LogLevel::DEBUG, fmt,  __VA_ARGS__)
#define SERVER_LOG_FMT_INFO(logger, fmt, ...) SERVER_LOG_FMT_LEVEL(logger, dx::LogLevel::INFO, fmt, __VA_ARGS__)
#define SERVER_LOG_FMT_WARN(logger, fmt, ...) SERVER_LOG_FMT_LEVEL(logger, dx::LogLevel::WARN, fmt, __VA_ARGS__)
#define SERVER_LOG_FMT_ERROR(logger, fmt, ...) SERVER_LOG_FMT_LEVEL(logger, dx::LogLevel::ERROR, fmt, __VA_ARGS__)
#define SERVER_LOG_FMT_FATAL(logger, fmt, ...) SERVER_LOG_FMT_LEVEL(logger, dx::LogLevel::FATAL, fmt, __VA_ARGS__)

#define SERVER_LOG_ROOT() dx::LoggerMgr::GetInstance()->GetRoot()

#define SERVER_LOG_NAME(name) dx::LoggerMgr::GetInstance()->GetLogger(name);


namespace dx {

class Logger;
class LogFormatter;
class LogAppender;

class LoggerManager;
/**
 * @brief 日志级别
 * 
 */
class LogLevel 
{
public:
    enum Level
    {
        UNKNOW = 0,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL
    };

    static const char* ToString(LogLevel::Level level);
    static LogLevel::Level FromString(const std::string& str);
};

/**
 * @brief 日志事件
 * 
 */
class LogEvent 
{
public:
    typedef std::shared_ptr<LogEvent> ptr;

    LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char* file, int32_t line, uint32_t elapace, uint32_t thread_id, uint32_t fiber_id, uint64_t time, const std::string& thread_name);
    ~LogEvent();
    
    const char* GetFile() const {return m_file; }
    int32_t GetLine() const { return m_line; }
    uint32_t GetElapse() const { return m_elapse; }
    int32_t GetThreadId() const { return m_threadId; }
    uint32_t GetFiberId() const { return m_fiberId;}
    uint64_t GetTime() const { return m_time; }
    std::string GetContent() const { return m_ss.str();}
    std::shared_ptr<Logger> GetLogger() const { return m_logger; }
    LogLevel::Level GetLevel() const { return m_level; }

    std::stringstream& GetSS() { return m_ss;}
    void Format(const char* fmt, ...);
    void Format(const char* fmt, va_list al);
private:
    const char* m_file = nullptr; // 文件名
    int32_t m_line = 0;           // 行号
    uint32_t m_elapse = 0;        // 程序启动开始到现在的毫秒数
    uint32_t m_threadId = 0;       // 线程id
    uint32_t m_fiberId = 0;       // 协程id
    uint64_t m_time = 0;          // 时间戳
    std::stringstream m_ss;

    std::shared_ptr<Logger> m_logger;
    LogLevel::Level m_level;
};

class LogEventWrap {
public:
    LogEventWrap(LogEvent::ptr ptr);
    ~LogEventWrap();

    std::stringstream& GetSS();
    LogEvent::ptr GetEvent() {return m_event;}
private:
    LogEvent::ptr m_event;

};

/**
 * @brief 日志格式器
 * 
 */
class LogFormatter 
{
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    LogFormatter(const std::string& pattern);

    std::string Format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
public:
    class FormatItem 
    {
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        FormatItem(const std::string &fmt = ""){}
        virtual ~FormatItem() {};
        virtual void Format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    };

    void Init();
    bool IsError() const { return m_err; }
    const std::string GetPattern() const { return m_pattern; }
private:
    bool m_err = false;
    std::string m_pattern;
    std::vector<FormatItem::ptr> m_items;
};

/**
 * @brief 日志输出地
 * 
 */
class LogAppender 
{
friend class Logger;
public:
    typedef std::shared_ptr<LogAppender> ptr;
    typedef SMutex MutexType;

    LogAppender();
    virtual ~LogAppender(){};
    virtual void Log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;

    void SetFormatter(LogFormatter::ptr val);
    void SetFormatter(const std::string& val);
    LogFormatter::ptr GetFormatter();

    LogLevel::Level GetLevel() const { return m_level; }
    void SetLevel(LogLevel::Level level) { m_level = level; }
    
    bool HasForamtter() const { return m_hasFormatter; }

    virtual std::string ToYamlString() = 0;
protected:
    MutexType m_lock;
    bool m_hasFormatter = false;
    LogLevel::Level m_level = LogLevel::DEBUG;
    LogFormatter::ptr m_formatter;
};


/**
 * @brief 日志输出器
 * 
 */ 
class Logger : public std::enable_shared_from_this<Logger> {
friend class LoggerManager;
public:
    typedef std::shared_ptr<Logger> ptr;
    typedef SMutex MutexType;

    void Log(LogLevel::Level level, const LogEvent::ptr event);
    Logger(const std::string name = "root");
    
    void Debug(LogEvent::ptr event);
    void Info(LogEvent::ptr event);
    void Warn(LogEvent::ptr event);
    void Error(LogEvent::ptr event);
    void Fatal(LogEvent::ptr event);
    void AddAppender(LogAppender::ptr appender);
    void DelAppender(LogAppender::ptr appender);
    void ClearAppenders();
    void SetFormatter(LogFormatter::ptr val);
    void SetFormatter(const std::string& val);
    LogFormatter::ptr GetFormatter();

    LogLevel::Level GetLevel() const { return m_level; }
    void SetLevel(LogLevel::Level val) { m_level = val; }
    const std::string GetName() { return m_name; }

    std::string ToYamlString();
public:
    std::string m_name; // 日志名称
    Logger::ptr m_root;

private:
    MutexType m_lock;
    LogLevel::Level m_level; // 日志级别
    LogFormatter::ptr m_formatter;
    std::list<LogAppender::ptr> m_appenders;        // Appender 集合
};


/**
 * @brief 输出到控制台的Appender
 * 
 */
class StdoutLogAppender : public LogAppender
{
public:
    StdoutLogAppender();
    ~StdoutLogAppender();
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    void Log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;

    std::string ToYamlString() override;
private:
};

/**
 * @brief 输出到文件的Appender
 * 
 */
class FileLogAppender : public LogAppender 
{
public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string& filename);
    ~FileLogAppender(){}

    void Log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
    bool Reopen();

    std::string ToYamlString() override;

private:

    std::string     m_name; // 文件名
    std::ofstream   m_filestream; // 
};

class LogManager {
public:
    typedef SMutex MutexType;

    LogManager();
    Logger::ptr GetLogger(const std::string& name);

    void Init();
    Logger::ptr GetRoot() const { return m_root; }

    std::string ToYamlString();
private:
    MutexType m_lock;
    Logger::ptr m_root;    
    std::map<std::string, Logger::ptr> m_loggers;
};

typedef dx::Singleton<LogManager> LoggerMgr;

}

#endif
