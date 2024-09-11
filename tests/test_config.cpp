#include "src/config.h"
#include "src/log.h"
#include <yaml-cpp/yaml.h>
#include <iostream>

// dx::ConfigVar<int>::ptr g_int_value_config = 
//     dx::Config::Lookup("system.port", (int)8080, "system port");


// dx::ConfigVar<float>::ptr g_float_value_config = 
//     dx::Config::Lookup("system.value", (float)10.2f, "system value");

// dx::ConfigVar<std::vector<int>>::ptr g_int_vec_config = 
//     dx::Config::Lookup("system.int_vec", std::vector<int>{1, 2}, "system in vec");
    
// dx::ConfigVar<std::list<int>>::ptr g_int_list_config = 
//     dx::Config::Lookup("system.int_list", std::list<int>{1, 2}, "system in list");

// dx::ConfigVar<std::set<int>>::ptr g_int_set_config = 
//     dx::Config::Lookup("system.int_set", std::set<int>{1, 2}, "system in set");

// dx::ConfigVar<std::unordered_set<int>>::ptr g_int_unordered_set_config = 
//     dx::Config::Lookup("system.int_unordered_set", std::unordered_set<int>{1, 2}, "system in unordered_set");

// dx::ConfigVar<std::map<std::string, int>>::ptr g_str_int_map_config = 
//     dx::Config::Lookup("system.str_int_map", std::map<std::string, int> {{"k", 2}, }, "system in map");

// dx::ConfigVar<std::unordered_map<std::string, int>>::ptr g_str_int_umap_config = 
//     dx::Config::Lookup("system.str_int_umap", std::unordered_map<std::string, int> {{"k", 2}, }, "system in umap");

/** 
void print_yaml(const YAML::Node& node, int level) {
    if(node.IsScalar()) {
        SERVER_LOG_INFO(SERVER_LOG_ROOT()) << std::string(level * 4, ' ') << node.Scalar() << " - " << node.Type() << " - " << level;
    } else if(node.IsNull()) {
        SERVER_LOG_INFO(SERVER_LOG_ROOT()) << std::string(level * 4, ' ') << " NULL - " << node.Type() << " - " << level;
    } else if(node.IsMap()) {
        for(auto it = node.begin(); it != node.end(); it++) {
            SERVER_LOG_INFO(SERVER_LOG_ROOT()) << std::string(level * 4, ' ') << it->first << " - " << it->second.Type() << " - " << level;
            print_yaml(it->second, level + 1);
        }
    } else if(node.IsSequence()) {
        for(size_t i = 0; i < node.size(); i++) {
            SERVER_LOG_INFO(SERVER_LOG_ROOT()) << std::string(level * 4, ' ') << i << " - " << node[i].Type() << " - " << level;
            print_yaml(node[i], level + 1);
        }
    }

}

void test_yaml() {
    YAML::Node node = YAML::LoadFile("/home/dx/workspace/server/conf/test.yaml");
    // SERVER_LOG_INFO(SERVER_LOG_ROOT()) << root;
    print_yaml(node, 0);
}

void test_config() {
    SERVER_LOG_INFO(SERVER_LOG_ROOT()) << "before: " << g_int_value_config->ToString();
    SERVER_LOG_INFO(SERVER_LOG_ROOT()) << "before: " << g_float_value_config->ToString();

#define XX(g_val, name, prefix) { \
        auto& v = g_val->GetValue(); \
        for(auto& i : v) { \
            SERVER_LOG_INFO(SERVER_LOG_ROOT()) <<  #prefix " " #name << ": " << i; \
        } \
        SERVER_LOG_INFO(SERVER_LOG_ROOT()) <<  #prefix " " #name << " yaml: " << g_val->ToString(); \
    }

#define XX_M(g_val, name, prefix) { \
        auto& v = g_val->GetValue(); \
        for(auto& i : v) { \
            SERVER_LOG_INFO(SERVER_LOG_ROOT()) <<  #prefix " " #name << ": {" << i.first << " - " << i.second << "}"; \
        } \
        SERVER_LOG_INFO(SERVER_LOG_ROOT()) <<  #prefix " " #name << " yaml: " << g_val->ToString(); \
    }

    XX(g_int_vec_config, int_vec, before);
    XX(g_int_list_config, list_vec, before);
    XX(g_int_set_config, set_vec, before);
    XX(g_int_unordered_set_config, unordered_set, before);
    XX_M(g_str_int_map_config, str_int_map, before);
    XX_M(g_str_int_umap_config, str_int_umap, before);

    YAML::Node node = YAML::LoadFile("/home/dx/workspace/server/conf/test.yaml");

    dx::Config::LoarFromYaml(node);

    SERVER_LOG_INFO(SERVER_LOG_ROOT()) << "after: " << g_int_value_config->ToString();
    SERVER_LOG_INFO(SERVER_LOG_ROOT()) << "after: " << g_float_value_config->ToString();

    XX(g_int_vec_config, int_vec, after);
    XX(g_int_list_config, list_vec, after);
    XX(g_int_set_config, set_vec, after);
    XX(g_int_unordered_set_config, unordered_set, after);
    XX_M(g_str_int_map_config, str_int_map, after);
    XX_M(g_str_int_umap_config, str_int_umap, after);
#undef XX_M
#undef XX

}
*/

class Person {
public: 
    Person() {
        m_name = "";
    }
    std::string ToString() const{
        std::stringstream ss;
        ss << "[Person name=" << m_name << " age=" << m_age << " sex=" << m_sex << "]";
        return ss.str();
    }

    bool operator==(const Person& a) const { 
        return m_name == a.m_name && m_age == a.m_age && m_sex == a.m_sex;
    }

public:
    std::string m_name;
    int m_age = 0;
    bool m_sex = 0;
};

dx::ConfigVar<Person>::ptr g_person = dx::Config::Lookup("class.person", Person(), "system person");

dx::ConfigVar<std::map<std::string, Person> >::ptr g_person_map = dx::Config::Lookup("class.map", std::map<std::string, Person>(), "system person");

dx::ConfigVar<std::map<std::string, std::vector<Person> > >::ptr 
                    g_person_vec_map = dx::Config::Lookup("class.vec_map", std::map<std::string, std::vector<Person> >(), "system person");


namespace dx {

template<>
class LexicalCast<std::string, Person> {
public:    
    Person operator() (const std::string& v) {
        YAML::Node node = YAML::Load(v);
        Person p;
        p.m_name = node["name"].as<std::string>();
        p.m_age = node["age"].as<int>();
        p.m_sex = node["sex"].as<bool>();
        return p;
    }
};

template<>
class LexicalCast<Person, std::string> {
public:
    std::string operator() (const Person& v) {
        YAML::Node node;
        node["name"] = v.m_name;
        node["age"] = v.m_age;
        node["sex"] = v.m_sex;
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

}

void test_class() {
    SERVER_LOG_INFO(SERVER_LOG_ROOT()) << "before: "<< g_person->GetValue().ToString() << " - " << g_person->ToString();

#define XX_PM(g_val, prefix) \
    {\
        auto m = g_val->GetValue();\
        for(auto& i : m) {\
            SERVER_LOG_INFO(SERVER_LOG_ROOT()) << prefix  << ": "<< i.first << " - " << i.second.ToString();\
        }\
        SERVER_LOG_INFO(SERVER_LOG_ROOT()) <<  prefix  << ": size="<< m.size();\
    }

    g_person->AddListener(10, [](const Person& old_val, const Person& new_val) {
        SERVER_LOG_INFO(SERVER_LOG_ROOT()) << "old_val=" << old_val.ToString() << " new_val=" << new_val.ToString();
    });

    XX_PM(g_person_map, "class.map before");
    SERVER_LOG_INFO(SERVER_LOG_ROOT()) << "before: " << g_person_vec_map->ToString();

    YAML::Node node = YAML::LoadFile("/home/dx/workspace/server/conf/test.yaml");
    dx::Config::LoarFromYaml(node);

    SERVER_LOG_INFO(SERVER_LOG_ROOT()) << "after: " << g_person_vec_map->ToString();
    SERVER_LOG_INFO(SERVER_LOG_ROOT()) << "after: " << g_person->GetValue().ToString() << " - " << g_person->ToString();

    XX_PM(g_person_map, "class.map after");
#undef XX_PM
}

void test_log() {
    static dx::Logger::ptr system_log = SERVER_LOG_NAME("system");
    SERVER_LOG_INFO(system_log) << "hello system";
    std::cout << dx::LoggerMgr::GetInstance()->ToYamlString() << std::endl;
    YAML::Node node = YAML::LoadFile("/home/dx/workspace/server/conf/log.yaml");
    dx::Config::LoarFromYaml(node);
    std::cout << "=========================" << std::endl;
    std::cout << dx::LoggerMgr::GetInstance()->ToYamlString() << std::endl;
    SERVER_LOG_INFO(system_log) << "hello system";

}

int main(int argc, char** argv) {

    // SERVER_LOG_INFO(SERVER_LOG_ROOT()) << g_int_value_config->GetValue();
    // SERVER_LOG_INFO(SERVER_LOG_ROOT()) << g_int_value_config->ToString();
    // SERVER_LOG_INFO(SERVER_LOG_ROOT()) << g_float_value_config->GetValue();
    // SERVER_LOG_INFO(SERVER_LOG_ROOT()) << g_float_value_config->ToString();
    // test_config();
    // test_yaml();
    // test_class();
    // printf("dsadas\n");
    test_log();
    return 0;
}

