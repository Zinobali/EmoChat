#include "ConfigMgr.h"

ConfigMgr::ConfigMgr() {
    auto current_path = boost::filesystem::current_path();
    auto config_path = current_path / "config.ini";
    std::cout << "Config path: " << config_path << std::endl;
    LoadConfig(config_path.string());
}

void ConfigMgr::LoadConfig(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex_);
    try {
        boost::property_tree::ini_parser::read_ini(filename, config_);
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Failed to load INI file: " + std::string(e.what()));
    }
}

ConfigMgr::SectionProxy ConfigMgr::operator[](const std::string& section_name) {
    return SectionProxy(config_, section_name);
}

ConfigMgr::SectionProxy::SectionProxy(ptree& section, const std::string& section_name)
    : section_(section), section_name_(section_name) {}

std::string ConfigMgr::SectionProxy::operator[](const std::string& key) const {
    try {
        return section_.get<std::string>(section_name_ + "." + key);
    }
    catch (boost::property_tree::ptree_error& e) {
        throw std::runtime_error("Key not found: " + section_name_ + "." + key);
    }
}
