/**
 * @file config.cpp
 * @author Dingx (dingx@info2soft.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "src/config.h"

namespace dx
{

/**
 * @brief
 *
 * @param node
 * @version 0.1
 * @date 2024-08-22
 * @copyright Copyright (c) 2024
 */
static void ListAllMember(const std::string &prefix, const YAML::Node &node,
                            std::list<std::pair<std::string, const YAML::Node>> &output)
{
    if (prefix.find_first_not_of("abcdefghijklmnopqrstuvwxyz._0123456789") != std::string::npos)
    {
        SERVER_LOG_ERROR(SERVER_LOG_ROOT()) << "Config invalid name: " << prefix << " : " << node;
        return;
    }
    output.push_back(std::make_pair(prefix, node));
    if (node.IsMap())
    {
        for (auto it = node.begin(); it != node.end(); it++)
        {
            ListAllMember(prefix.empty() ? it->first.Scalar() : (prefix + "." + it->first.Scalar()), it->second, output);
        }
    }
}

void Config::LoarFromYaml(const YAML::Node &node)
{
    std::list<std::pair<std::string, const YAML::Node>> all_nodes;
    ListAllMember("", node, all_nodes);

    for (auto &i : all_nodes)
    {
        std::string key = i.first;
        if (key.empty())
            continue;

        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        ConfigVarBase::ptr var = LookupBase(key);

        if (var)
        {
            if (i.second.IsScalar())
            {
                var->FromString(i.second.Scalar());
            }
            else
            {
                std::stringstream ss;
                ss << i.second;
                var->FromString(ss.str());
            }
        }
    }
}

ConfigVarBase::ptr Config::LookupBase(const std::string &name)
{
    auto it = GetDatas().find(name);
    return it == GetDatas().end() ? nullptr : it->second;
}



}
