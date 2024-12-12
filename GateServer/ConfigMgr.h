#pragma once
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <map>
#include <iostream>
#include <mutex>
#include <memory>
#include <boost/filesystem.hpp>

using boost::property_tree::ptree;


class ConfigMgr
{
    // inner class
    class SectionProxy
    {
    public:
        SectionProxy(ptree& section, const std::string& section_name);
        std::string operator[](const std::string& key) const;

    private:
        ptree& section_;
        std::string section_name_;
    };
public:
    ConfigMgr();

    void LoadConfig(const std::string& filename);
    SectionProxy operator[](const std::string& section_name);

private:
    ptree config_;
    std::mutex mutex_;
};

