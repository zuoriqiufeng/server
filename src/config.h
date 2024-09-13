/**
 * @file config.h
 * @author Dingx (dingx@info2soft.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-23
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef __SERVER_CONFIG_H__
#define __SERVER_CONFIG_H__

#include <memory>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include "log.h"
#include "util.h"
#include <yaml-cpp/yaml.h>
#include <set>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <functional>
#include "thread.h"


namespace dx {

/**
 * @brief 
 * 
 * 
 */
class ConfigVarBase {
public:
    typedef std::shared_ptr<ConfigVarBase> ptr;

    ConfigVarBase(const std::string& name, const std::string& description = "") 
    : m_name(name)
    , m_description(description) {
        std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
    }

    virtual ~ConfigVarBase() {}

    const std::string& GetName() const { return m_name; }
    const std::string& GetDescription() const { return m_description; }

    virtual std::string ToString() = 0;
    virtual bool FromString(const std::string& val) = 0;
    virtual std::string GetTypeName() const = 0; 
private:
    std::string m_name;
    std::string m_description;

};

/**
 * @brief 
 * 
 * @tparam F from type
 * @tparam T to type
 */
template<class F, class T>
class LexicalCast {
public:
    T operator()(const F& v) {
        return boost::lexical_cast<T>(v);
    }
};

template<class T>
class LexicalCast<std::string, std::vector<T> > {
public:    
    std::vector<T> operator() (const std::string& v) {
        YAML::Node node = YAML::Load(v);
        std::stringstream ss;
        typename std::vector<T> vec;
        for(size_t i = 0; i < node.size(); i++) {
            ss.str("");
            ss << node[i];
            vec.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

template<class T>
class LexicalCast<std::vector<T>, std::string> {
public:
    std::string operator() (const std::vector<T>& v) {
        YAML::Node node;
        for(auto& i : v)
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};


/**
 * @brief 
 * 
 * @tparam T 
 */
template<class T>
class LexicalCast<std::string, std::list<T> > {
public:
    std::list<T> operator() (const std::string& v) {
        YAML::Node node = YAML::Load(v);
        std::stringstream ss;
        typename std::list<T> vec;
        for(size_t i = 0; i < node.size(); i++) {
            ss.str("");
            ss << node[i];
            vec.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

/**
 * @brief 
 * 
 * @tparam T 
 */
template<class T>
class LexicalCast<std::list<T>, std::string> {
public:
    std::string operator() (const std::list<T>& v) {
        YAML::Node node;
        for(auto& i : v)
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

/**
 * @brief 
 * 
 * @tparam T 
 */
template<class T>
class LexicalCast<std::string, std::set<T> > {
public:
    std::set<T> operator() (const std::string& v) {
        YAML::Node node = YAML::Load(v);
        std::stringstream ss;
        typename std::set<T> vec;
        for(size_t i = 0; i < node.size(); i++) {
            ss.str("");
            ss << node[i];
            vec.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

/**
 * @brief 
 * 
 * @tparam T 
 */
template<class T>
class LexicalCast<std::set<T>, std::string> {
public:
    std::string operator() (const std::set<T>& v) {
        YAML::Node node;
        for(auto& i : v)
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

/**
 * @brief 
 * 
 * @tparam T 
 */
template<class T>
class LexicalCast<std::string, std::unordered_set<T> > {
public:
    std::unordered_set<T> operator() (const std::string& v) {
        YAML::Node node = YAML::Load(v);
        std::stringstream ss;
        typename std::unordered_set<T> vec;
        for(size_t i = 0; i < node.size(); i++) {
            ss.str("");
            ss << node[i];
            vec.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

/**
 * @brief 
 * 
 * @tparam T 
 */
template<class T>
class LexicalCast<std::unordered_set<T>, std::string> {
public:
    std::string operator() (const std::unordered_set<T>& v) {
        YAML::Node node;
        for(auto& i : v)
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

/**
 * @brief 
 * 
 * @tparam T 
 */
template<class T>
class LexicalCast<std::string, std::map<std::string, T> > {
public:
    std::map<std::string, T> operator() (const std::string& v) {
        YAML::Node node = YAML::Load(v);
        std::stringstream ss;
        typename std::map<std::string, T> vec;
        for(auto it = node.begin(); it != node.end(); it++) {
            ss.str("");
            ss << it->second;
            vec.insert(std::make_pair(it->first.Scalar(), LexicalCast<std::string, T>()(ss.str())));
        }
        return vec;
    }
};

/**
 * @brief 
 * 
 * @tparam T 
 */
template<class T>
class LexicalCast<std::map<std::string, T>, std::string> {
public:
    std::string operator() (const std::map<std::string, T>& v) {
        YAML::Node node;
        for(auto& i : v) {
            node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
        }        
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

/**
 * @brief 
 * 
 * @tparam T 
 */
template<class T>
class LexicalCast<std::string, std::unordered_map<std::string, T> > {
public:
    std::unordered_map<std::string, T> operator() (const std::string& v) {
        YAML::Node node = YAML::Load(v);
        std::stringstream ss;
        typename std::unordered_map<std::string, T> vec;
        for(auto it = node.begin(); it != node.end(); it++) {
            ss.str("");
            ss << it->second;
            vec.insert(std::make_pair(it->first.Scalar(), LexicalCast<std::string, T>()(ss.str())));
        }
        return vec;
    }
};

/**
 * @brief 
 * 
 * @tparam T 
 */
template<class T>
class LexicalCast<std::unordered_map<std::string, T>, std::string> {
public:
    std::string operator() (const std::unordered_map<std::string, T>& v) {
        YAML::Node node;
        for(auto& i : v) {
            node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
        }        
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};


/**
 * @brief 
 * 
 * @tparam T 
 * @tparam FromStr T operator()(const std::string&) 
 * @tparam ToStr   std::string operator()(cosnt T& t)
 */
template<class T, class FromStr = LexicalCast<std::string, T>, class ToStr = LexicalCast<T, std::string> >
class ConfigVar : public ConfigVarBase {
public:
    typedef RWMutex MutexType;
    typedef std::shared_ptr<ConfigVar> ptr;
    typedef std::function<void (const T& old_val, const T& new_val)> on_change_cb;

    ConfigVar(const std::string& name, const T& default_value, const std::string& description = "")
    : ConfigVarBase(name, description)
    , m_val(default_value) {}

    /**
     * @brief 
     * 
     * @return std::string 
     * 
     */
    std::string ToString() override {
        try {
            MutexType::ReadLock g(m_mutex);
            return ToStr()(m_val);
        } catch(std::exception& e) {
            SERVER_LOG_ERROR(SERVER_LOG_ROOT()) << "ConfigVar::ToString Exception" << e.what()
            << " convert:" << typeid(m_val).name() << " to string";
        }
        return "";
    }


    virtual bool FromString(const std::string& val) override {
        try {
            // m_val = boost::lexical_cast<T>(val);
            SetValue(FromStr()(val));
        } catch(std::exception& e) {
            SERVER_LOG_ERROR(SERVER_LOG_ROOT()) << "ConfigVar::ToString Exception" << e.what()
            << " convert: string to " << typeid(m_val).name();
        }
        return "";
    }

    const T GetValue() { 
        MutexType::ReadLock g(m_mutex);
        return m_val; 
    }
    
    void SetValue(const T& v) { 
        {
            MutexType::ReadLock g(m_mutex);
            if(v == m_val)
                return;

            for(auto& i : m_cbs) {
                i.second(m_val, v);
            }
        }
        MutexType::WriteLock g(m_mutex);
        m_val = v;
    }
    std::string GetTypeName() const { return typeid(T).name(); }

    uint64_t AddListener(on_change_cb cb) {
        static uint64_t s_fun_id = 0;
        MutexType::WriteLock g(m_mutex);
        s_fun_id++;
        m_cbs[s_fun_id] = cb;
        return s_fun_id;
    }

    void DelListener(uint64_t key) {
        MutexType::WriteLock g(m_mutex);
        m_cbs.erase(key);
    }

    on_change_cb GetListener(uint64_t key) {
        MutexType::ReadLock g(m_mutex);
        auto& it = m_cbs.find(key);
        return it == m_cbs.end() ? nullptr : it->second;
    }

    void ClearListener() {
        MutexType::WriteLock g(m_mutex);
        m_cbs.clear();
    }

private:
    T m_val;
    MutexType m_mutex;
    // 变更回调函数数组, uint64_t key 唯一，
    std::map<uint64_t, on_change_cb> m_cbs;
};

/**
 * @brief 
 * 
 * @version 0.1
 * @date 2024-08-21
 * @copyright Copyright (c) 2024
 */
class Config {
public:
    typedef std::map<std::string, ConfigVarBase::ptr> ConfigVarMap;
    typedef RWMutex MutexType;

    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name, const T& default_value, const std::string& description = "") {
        
        MutexType::WriteLock g(GetMutex());
        auto it = GetDatas().find(name);
        if(it != GetDatas().end()) {
            auto tmp = std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
            if(tmp) {
                SERVER_LOG_INFO(SERVER_LOG_ROOT()) << "Lookup name=" << name << " exists";
                return tmp;
            } else {
                SERVER_LOG_ERROR(SERVER_LOG_ROOT()) << "Lookup name=" << name<< " exists but type not " << typeid(T).name() << "real_type=" << it->second->GetTypeName() << " " << it->second->ToString(); 
                return nullptr;
            }
        }
        
        if(name.find_first_not_of("abcdefghijklmnopqrstuvwxyz._0123456789") != std::string::npos) {
            SERVER_LOG_ERROR(SERVER_LOG_ROOT()) << "Lookup name invalid " << name; 
            throw std::invalid_argument(name);
        }

        typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, description));
        GetDatas()[name] = v;
        return v;
    }

    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name) {
        MutexType::ReadLock g(GetMutex());
        auto it = GetDatas().find(name);
        if(it == GetDatas().end()) {
            return nullptr;
        }
        return std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
    }

    static void LoarFromYaml (const YAML::Node& root);
    static ConfigVarBase::ptr LookupBase(const std::string& name);
    static void Visit(std::function<void(ConfigVarBase::ptr)> cb);

private:
    static ConfigVarMap& GetDatas() {
        static ConfigVarMap s_datas;
        return s_datas;
    }

    static Config::MutexType& GetMutex() {
        static Config::MutexType m_mutex;
        return m_mutex;
    }
};

}

#endif
