#include "config.h"
#include <fstream>
#include <iostream>
#include <toml11/toml.hpp>

Config loadConfig(const std::string& configFilePath) {
    Config config;

    try {
        auto configData = toml::parse_file(configFilePath);

        config.clangFlags = toml::find<std::string>(configData, "clang_flags");
        config.generateHtmlReport = toml::find<bool>(configData, "generate_html_report");
        config.outputDirectory = toml::find<std::string>(configData, "output_directory");
    } catch (const toml::parse_error& err) {
        std::cerr << "Error parsing config file: " << err.description() << std::endl;
        std::cerr << "Error location: " << err.source().begin << std::endl;
    }

    return config;
}