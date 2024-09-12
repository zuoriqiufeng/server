/**
 * *****************************************************************************
 * @file log.cpp
 * @brief 日志模块实现
 * @author zuoriqiufeng (507684323@qq.com)
 * @date 2024-02-23
 * @copyright 
 * *****************************************************************************
 */
#include "log.h"
#include <memory>
#include <iostream>
#include <map>
#include <functional>
#include <time.h>
#include <string>
#include <stdarg.h>
#include "config.h"
#include "thread.h"

namespace dx {


const char* LogLevel::ToString(LogLevel::Level level)
{
    switch (level)
    {
#define XX(name) \
    case LogLevel::name: \
        return #name; \
        break;

    XX(DEBUG);
    XX(INFO)
    XX(WARN);
    XX(ERROR);
    XX(FATAL);
#undef XX
    default:
        return "UNKNOW";
    }
    return "UNKNOW";
}

LogLevel::Level LogLevel::FromString(const std::string& str) {
#define XX(level, name) \
    if(str == #name) \
        return LogLevel::level;
    XX(DEBUG, debug);
    XX(INFO, info)
    XX(WARN, warn);
    XX(ERROR, error);
    XX(FATAL, fatal);
    XX(DEBUG, DEBUG);
    XX(INFO, INFO)
    XX(WARN, WARN);
    XX(ERROR, ERROR);
    XX(FATAL, FATAL);
    return LogLevel::UNKNOW;
#undef XX
}  

class MessageFormatItem : public LogFormatter::FormatItem {
public:
    MessageFormatItem(const std::string &fmt = ""){}
    void Format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->GetContent();
    }
};

class LevelFormatItem : public LogFormatter::FormatItem {
public:
    LevelFormatItem(const std::string &fmt = ""){}
    void Format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << LogLevel::ToString(level);
    }
};

class ElapseFormatItem : public LogFormatter::FormatItem {
public:
    ElapseFormatItem(const std::string &fmt = ""){}
    void Format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->GetElapse();
    }
};

class NameFormatItem : public LogFormatter::FormatItem {
public:
    NameFormatItem(const std::string &fmt = ""){}
    void Format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->GetLogger()->GetName();
    }
};


class ThreadIdFormatItem : public LogFormatter::FormatItem {
public:
    ThreadIdFormatItem(const std::string &fmt = ""){}
    void Format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->GetThreadId();
    }
};

class FiberIdFormatItem : public LogFormatter::FormatItem {
public:
    FiberIdFormatItem(const std::string &fmt = ""){}
    void Format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->GetFiberId();
    }
};

class DateTimeFormatItem : public LogFormatter::FormatItem {
public:
    DateTimeFormatItem(std::string format = "%Y-%m-%d %H:%M:%S") : m_format(format) {
        if(m_format.empty()) {
            m_format = "%Y-%m-%d %H:%M:%S";
        }
    }
    void Format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        struct tm tm;
        time_t time = event->GetTime();
        localtime_r(&time, &tm);
        char buf[64];
        strftime(buf, sizeof(buf), m_format.c_str(), &tm);
        os << buf;
    }
private:
    std::string m_format;
};

class FilenameFormatItem : public LogFormatter::FormatItem {
public:
    FilenameFormatItem(const std::string &fmt = ""){}
    void Format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->GetFile();
    }
};

class LineFormatItem : public LogFormatter::FormatItem {
public:
    LineFormatItem(const std::string &fmt = ""){}
    void Format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->GetLine();
    }
};

class NewLineFormatItem : public LogFormatter::FormatItem {
public:
    NewLineFormatItem(const std::string &fmt = ""){}
    void Format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << std::endl;
    }
};


class StringFormatItem : public LogFormatter::FormatItem {
public:
    StringFormatItem(const std::string &fmt) : m_string(fmt){}
    void Format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << m_string;
    }
private:
    std::string m_string;
};

class TabFormatItem : public LogFormatter::FormatItem {
public:
    TabFormatItem(const std::string &fmt) {}
    void Format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << "\t";
    }
};

LogEventWrap::LogEventWrap(LogEvent::ptr ptr)
    :m_event(ptr) {


}
LogEventWrap::~LogEventWrap() {
    m_event->GetLogger()->Log(m_event->GetLevel(), m_event);
}

std::stringstream& LogEventWrap::GetSS() {
    return m_event->GetSS();
}


LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char* file, int32_t line, uint32_t elapse, uint32_t thread_id, uint32_t fiber_id, uint64_t time, const std::string& thread_name) 
    : m_file(file),
    m_line(line),
    m_elapse(elapse),
    m_threadId(thread_id), 
    m_fiberId(fiber_id),
    m_time(time),
    m_logger(logger),
    m_level(level) {

}

LogEvent::~LogEvent() {

}


void LogEvent::Format(const char* fmt, ...) {
    va_list al;
    va_start(al, fmt);
    Format(fmt, al);
    va_end(al);
}

void LogEvent::Format(const char* fmt, va_list al) {
    char* buf = nullptr;
    int len = vasprintf(&buf, fmt, al);
    if(len != -1)
    {
        m_ss << std::string(buf, len);
        free(buf);
    }
}


/**
 * @brief 日志生成器
 * 
 * @param name 
 */
Logger::Logger(const std::string name)
    : m_name(name),
    m_level(LogLevel::DEBUG) {
    m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));

}

void Logger::Log(LogLevel::Level level, const LogEvent::ptr event) {
    if(level >= m_level) {
        auto self = shared_from_this();
        
        MutexType::MutexGuard g(m_lock);
        if(!m_appenders.empty()) {
            for(auto& i : m_appenders) {
                i->Log(self, level, event);
            }

        } else if(m_root) {
            m_root->Log(level, event);
        }
    }
}

/**
 * @brief 按照yaml格式 输出字符串
 * 
 * @return std::string 
 */
std::string Logger::ToYamlString() {
    MutexType::MutexGuard g(m_lock);

    YAML::Node node;
    std::stringstream ss;

    node["name"] = m_name;
    if(m_level != LogLevel::UNKNOW)
        node["level"] = LogLevel::ToString(m_level);
    if(m_formatter)
        node["formatter"] = m_formatter->GetPattern();
    
    for(auto& i : m_appenders) {
        node["appenders"].push_back(YAML::Load(i->ToYamlString()));
    }
    
    ss << node;
    return ss.str();
}

void Logger::Debug(LogEvent::ptr event) {
    Log(LogLevel::DEBUG, event);
} 

void Logger::Info(LogEvent::ptr event) {
    Log(LogLevel::INFO, event);
}   

void Logger::Warn(LogEvent::ptr event) {
    Log(LogLevel::WARN, event);
}

void Logger::Error(LogEvent::ptr event) {
    Log(LogLevel::ERROR, event);
}

void Logger::Fatal(LogEvent::ptr event) {
    Log(LogLevel::FATAL, event);
}

void Logger::AddAppender(LogAppender::ptr appender) {
    MutexType::MutexGuard g(m_lock);
    
    if(!appender->HasForamtter()) {
        MutexType::MutexGuard apg(appender->m_lock);
        appender->m_formatter = m_formatter;
    }
    m_appenders.push_back(appender);
}

/**
 * @brief 
 * 
 * @param  appender         
 */
void Logger::DelAppender(LogAppender::ptr appender) {
    MutexType::MutexGuard g(m_lock);
    
    for(auto it = m_appenders.begin();
        it != m_appenders.end(); it++) {
        if(*it == appender) {
            m_appenders.erase(it);
            break;
        }
    }
}

void Logger::ClearAppenders() {
    MutexType::MutexGuard g(m_lock);
    m_appenders.clear();
}

void Logger::SetFormatter(LogFormatter::ptr val) {
    MutexType::MutexGuard g(m_lock);

    m_formatter = val;
    for(auto& i : m_appenders) {
        MutexType::MutexGuard pg(i->m_lock);
        if(!i->HasForamtter())
            i->m_formatter = m_formatter;
    }
}

void Logger::SetFormatter(const std::string& val) {
    MutexType::MutexGuard g(m_lock);

    dx::LogFormatter::ptr new_val(new LogFormatter(val));
    if(new_val->IsError())
        std::cout << "Logger::SetFoammtter name= " << m_name << "value=" << val << "invalid formatter" << std::endl;

    SetFormatter(new_val);
}

LogFormatter::ptr Logger::GetFormatter() {
    MutexType::MutexGuard g(m_lock);
    return m_formatter;
}

LogAppender::LogAppender(){}

void LogAppender::SetFormatter(LogFormatter::ptr val) {
    MutexType::MutexGuard g(m_lock);
    m_formatter = val;
    if(m_formatter)
        m_hasFormatter = true;
    else 
        m_hasFormatter = false;
}

void LogAppender::SetFormatter(const std::string& val) {
    MutexType::MutexGuard g(m_lock);
    dx::LogFormatter::ptr new_val(new LogFormatter(val));
    if(new_val->IsError())
        std::cout << "LogAppender::SetForamtter value=" << val << "invalid formatter" << std::endl;

    m_formatter = new_val;
    m_hasFormatter = true;
}

LogFormatter::ptr LogAppender::GetFormatter() {
    MutexType::MutexGuard g(m_lock);
    if(m_hasFormatter)
        return m_formatter;
    return nullptr;
}

StdoutLogAppender::StdoutLogAppender(){}
StdoutLogAppender::~StdoutLogAppender(){}

/**
 * @brief 
 * 
 * @param  logger           
 * @param  level            
 * @param  event            
 */
void StdoutLogAppender::Log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
    if(level >= m_level) {
        MutexType::MutexGuard g(m_lock);
        std::cout << m_formatter->Format(logger, level, event);
    }
}

/**
 * @brief 
 * 
 * @return std::string 
 */
std::string StdoutLogAppender::ToYamlString() {
    MutexType::MutexGuard g(m_lock);
    
    YAML::Node node;
    node["type"] = "StdoutLogAppender";
    
    if(m_level != LogLevel::UNKNOW)
        node["level"] = LogLevel::ToString(m_level);
    if(m_hasFormatter && m_formatter)
        node["formatter"] = m_formatter->GetPattern();
    
    std::stringstream ss;
    ss << node;
    return ss.str();
}


FileLogAppender::FileLogAppender(const std::string& filename) 
    : m_name(filename) {
    Reopen();
}

/**
 * @brief 重新打开文件，文件打开成功 true, 失败false
 * 
 * @return true 
 * @return false 
 */
bool  FileLogAppender::Reopen() {
    MutexType::MutexGuard g(m_lock);
    
    if(m_filestream) {
        m_filestream.close();
    }
    m_filestream.open(m_name);
    return !!m_filestream;
}

void FileLogAppender::Log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
    if(level >= m_level) {
        MutexType::MutexGuard g(m_lock);
        m_filestream << m_formatter->Format(logger, level, event);
    }
    
}

std::string FileLogAppender::ToYamlString() {
    MutexType::MutexGuard g(m_lock);
    
    YAML::Node node;
    node["type"] = "FileLogAppender";
    node["file"] = m_name;
    node["level"] = LogLevel::ToString(m_level);
    if(m_hasFormatter && m_formatter)
        node["m_formatter"] = m_formatter->GetPattern();
    std::stringstream ss;
    ss << node;
    return ss.str();
}



LogFormatter::LogFormatter(const std::string& pattern) 
    :m_pattern(pattern) {
    Init();
}



/**
 * @brief 
 * 日志格式器，执行日志格式化，负责日志格式的初始化。
 * 解析日志格式，将用户自定义的日志格式，解析为对应的FormatItem。
 * 日志格式举例：%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n
 * 格式解析：
    %d{%Y-%m-%d %H:%M:%S} : %d 标识输出的是时间 {%Y-%m-%d %H:%M:%S}为时间格式，可选 DateTimeFormatItem
    %T : Tab[\t]            TabFormatItem
    %t : 线程id             ThreadIdFormatItem
    %N : 线程名称           ThreadNameFormatItem
    %F : 协程id             FiberIdFormatItem
    %p : 日志级别           LevelFormatItem       
    %c : 日志名称           NameFormatItem
    %f : 文件名             FilenameFormatItem
    %l : 行号               LineFormatItem
    %m : 日志内容           MessageFormatItem
    %n : 换行符[\r\n]       NewLineFormatItem

    具体日志：
    2019-06-17 00:28:45     9368    main    6       [INFO]  [system]        sylar/tcp_server.cc:64  server bind success: [Socket sock=9 is_connected=0 family=2 type=1 protocol=0 local_address=0.0.0.0:8020]
 * @version 0.1
 * @date 2024-08-16
 * @copyright Copyright (c) 2024
 */
void LogFormatter::Init() {
    // string, formatter, type
    std::vector<std::tuple<std::string, std::string, int> > vec;
    // size_t lasst_pos = 0;
    std::string nstr;

    for(size_t i = 0; i < m_pattern.size(); i++) 
    {
        if(m_pattern[i] != '%') 
        {
            nstr.append(1, m_pattern[i]);
            continue;
        }

        if((i + 1) < m_pattern.size()) 
        {
            if(m_pattern[i + 1] == '%') 
            {
                nstr.append(1, '%');
                continue;
            }
        }        
        size_t n = i + 1;
        int fmt_status = 0;
        size_t fmt_begin = 0;

        std::string str;
        std::string fmt;
        while(n < m_pattern.size()) 
        {
            if(!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{' && m_pattern[n] != '}')) 
            {
                str = m_pattern.substr(i + 1, n - i - 1);
                break;
            }    

            if(fmt_status == 0) 
            {
                if(m_pattern[n] == '{') 
                {
                    str = m_pattern.substr(i + 1, n - i - 1);
                    fmt_status = 1; // 解析格式
                    fmt_begin = n;
                    ++n;
                    continue;
                } 
            } 
            else if(fmt_status == 1) 
            {
                if(m_pattern[n] == '}')
                {
                    fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                    fmt_status = 0;
                    ++n;
                    break;
                }
            }
            ++n;

            if(n == m_pattern.size()) {
                if(str.empty())
                    str = m_pattern.substr(i + 1);
            }

        }

        if(fmt_status == 0) 
        {
            if(!nstr.empty()) {
                vec.push_back(std::make_tuple(nstr, std::string(), 0));
                nstr.clear();
            } 
            
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n - 1;
        } 
        else if(fmt_status == 1) 
        {
            std::cout << "pattern paser error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
            m_err = true;
        }
    }

    if(!nstr.empty()) 
        vec.push_back(std::make_tuple(nstr, "", 0));
    

    static std::map<std::string, std::function<FormatItem::ptr (const std::string& str)> > s_format_items = {
#define XX(str, C) \
        {#str, [] (const std::string& fmt) { return FormatItem::ptr(new C(fmt)); } }
        XX(m, MessageFormatItem),
        XX(p, LevelFormatItem),
        XX(r, ElapseFormatItem),
        XX(c, NameFormatItem),
        XX(t, ThreadIdFormatItem),
        XX(n, NewLineFormatItem),
        XX(d, DateTimeFormatItem),
        XX(f, FilenameFormatItem),
        XX(l, LineFormatItem),
        XX(T, TabFormatItem),
        XX(F, FiberIdFormatItem)
#undef XX
    };

    for(auto& i : vec) {
        if(std::get<2>(i) == 0) {
            m_items.push_back(FormatItem::ptr (new StringFormatItem(std::get<0>(i)) ));
        } else {
            auto it = s_format_items.find(std::get<0>(i));
            if(it == s_format_items.end()) {
                m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
                std::cout << "pattern paser error: " << std::endl;
                m_err = true;
            } else {
                m_items.push_back(it->second(std::get<1>(i)));
            }
        }
        // std::cout << "(" << std::get<0>(i) << ") - (" << std::get<1>(i) <<  ") - (" << std::get<2>(i) << ")" << std::endl;
    }
    // %m -- 消息体 
    // %p -- level
    // %r -- 启动后的时间
    // %c -- 日志名称
    // %t -- 线程id
    // %n -- 回车换行
    // %d -- 时间
    // %f -- 文件名
    // %l -- 行号
}

std::string LogFormatter::Format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
    std::stringstream ss;
    for(auto& it: m_items) {
        it->Format(ss, logger, level, event);
    }
    return ss.str();
}


LogManager::LogManager() {
    m_root.reset(new Logger());
    m_root->AddAppender(LogAppender::ptr(new StdoutLogAppender));

    m_loggers[m_root->m_name] = m_root;

    Init();
}

std::string LogManager::ToYamlString() {
    MutexType::MutexGuard g(m_lock);

    YAML::Node node;
    for(auto& i : m_loggers) {
        node.push_back(YAML::Load(i.second->ToYamlString()));
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
}


/**
 * @brief 
 * 
 * @param  name             
 * @return Logger::ptr 
 */
Logger::ptr LogManager::GetLogger(const std::string& name) {
    MutexType::MutexGuard g(m_lock);

    auto it = m_loggers.find(name);
    if(it != m_loggers.end()) {
        return it->second;
    }

    Logger::ptr logger(new Logger(name));
    logger->m_root = m_root;
    m_loggers[name] = logger;
    return logger;
}

/**
 * @brief 
 * 
 */
struct LogAppenderDefine {
    int type = 0;   // 1 file 2 stdout
    LogLevel::Level level = LogLevel::UNKNOW;
    std::string formatter;
    std::string file;

    bool operator==(const LogAppenderDefine& oth) const {
        return type == oth.type
            && level == oth.level
            && formatter == oth.formatter
            && file == oth.file;
    }
}; 

/**
 * @brief 
 * 
 */
struct LogDefine {
    std::string name;
    LogLevel::Level level = LogLevel::UNKNOW;
    std::string formatter;
    std::vector<LogAppenderDefine> appenders;

    bool operator==(const LogDefine& oth) const {
        return name == oth.name
            && level == oth.level
            && formatter == oth.formatter
            && appenders == appenders;
    }

    bool operator<(const LogDefine& oth) const {
        return name < oth.name;
    }
};

/**
 * @brief 从string 转成 LogDefine
 * 
 * @tparam T 
 */
template<>
class LexicalCast<std::string, LogDefine> {
public:
    LogDefine operator() (const std::string& v) {
        YAML::Node node = YAML::Load(v);
        
        LogDefine logdef;
        if(!node["name"].IsDefined()) {
            std::cout << "log config error: name is null" << std::endl;
            throw std::logic_error("log config name is null");
        }
        logdef.name = node["name"].as<std::string>();
        logdef.level = LogLevel::FromString(node["level"].IsDefined() ? node["level"].as<std::string>() : "");
        
        if(node["formatter"].IsDefined())
            logdef.formatter = node["formatter"].as<std::string>();

        if(node["appenders"].IsDefined()) {
            for(size_t i = 0; i < node["appenders"].size(); i++) {
                auto a = node["appenders"][i];
                if(!a["type"].IsDefined()) {
                    std::cout << "log config error: appender type is null " << i << std::endl;
                    continue;
                }
                std::string type = a["type"].as<std::string>();
                
                LogAppenderDefine lad;
                if(type == "FileLogAppender") {
                    lad.type = 1;
                    if(!a["file"].IsDefined()) {
                        std::cout << "log config error: fileappender file is null " << i << std::endl;
                        continue;
                    }
                    lad.file = a["file"].as<std::string>();
                
                } else if (type == "StdoutLogAppender") {
                    lad.type = 2;
                } else {
                    std::cout << "log config error: appender type is inValid " << i << std::endl;
                    continue;
                }
                
                    lad.level = LogLevel::FromString(a["level"].IsDefined() ? node["level"].as<std::string>() : "");

                if(a["formatter"].IsDefined()) {
                    lad.formatter = a["formatter"].as<std::string>();
                }

                logdef.appenders.push_back(lad);
            }
        }

        return logdef;
    }
};


/**
 * @brief 
 * 
 * @tparam T 
 */
template<>
class LexicalCast<LogDefine, std::string> {
public:
    std::string operator() (const LogDefine& v) {
        std::stringstream ss;
        YAML::Node node;
        node["name"] = v.name;
        
        if(v.level != LogLevel::UNKNOW)
            node["level"] = LogLevel::ToString(v.level);
        if(!v.formatter.empty())
            node["formatter"] = v.formatter;
        
        for(auto& i : v.appenders) {
            YAML::Node ap;
            if(i.type == 0) {
                ap["type"] = "FileLogAppender";
                ap["file"] = i.file;
            }
            else if(i.type == 1)
                ap["type"] = "StdoutLogAppender";
            if(i.level != LogLevel::UNKNOW)
                ap["level"] = LogLevel::ToString(i.level);
            
            if(!i.formatter.empty())
                ap["formatter"] = i.formatter;
            
            node["appenders"].push_back(ap);
        }
        ss << node;
        return ss.str();
    }
};

dx::ConfigVar<std::set<LogDefine> >::ptr g_log_defines = dx::Config::Lookup("logs", std::set<LogDefine>(), "log config");

struct LogIniter {
    LogIniter() {
        g_log_defines->AddListener(0xF1E231, [](const std::set<LogDefine>& old_val, const std::set<LogDefine>& new_val) {
            // 1 新增 & 修改
            SERVER_LOG_INFO(SERVER_LOG_ROOT()) << "on_logger_conf_changed";
            for(auto& i : new_val) {
                dx::Logger::ptr logger = SERVER_LOG_NAME(i.name);
                
                logger->SetLevel(i.level);
                if(!i.formatter.empty())
                    logger->SetFormatter(i.formatter);
                
                logger->ClearAppenders();
                for(auto& a : i.appenders) {
                    dx::LogAppender::ptr ap;
                    if(a.type == 1) 
                        ap.reset(new FileLogAppender(a.file));
                    else if(a.type == 2) 
                        ap.reset(new StdoutLogAppender);
                    ap->SetLevel(a.level);
                    if(!a.formatter.empty()) {
                        LogFormatter::ptr fmt(new LogFormatter(a.formatter));
                        if(!fmt->IsError())
                            ap->SetFormatter(fmt);
                        else
                            std::cout << "log.name=" << i.name << ", appender type=" << a.type <<" formatter=" << a.formatter << " is invalid" << std::endl;  
                    }   

                    logger->AddAppender(ap);
                }
            } 

            //  删除
            for(auto& i : old_val) {
                auto it = new_val.find(i);
                if(it == new_val.end()) {
                    // 删除 logger
                    auto logger = SERVER_LOG_NAME(i.name);
                    logger->SetLevel((LogLevel::Level)100);
                    logger->ClearAppenders();
                } 
            }
        });
    }
};

static LogIniter __log_init;


void LogManager::Init() {

}


}
