#include "Configuration.h"

namespace fs = std::filesystem;
std::shared_ptr<Configuration>
Configuration::getByName(const std::string &name,
                         const ini::parsed_data &defaults) {
  fs::create_directory("config");
  auto path = fs::path{"config/" + name};

  if (!fs::exists(path)) {
    auto outstream = std::ofstream(path);
    outstream << "default=true\n";
    outstream.flush();
  }

  auto parser =
      std::make_unique<ini::parser<ini::comment_char::hash_tag>>(path.string());
  for (auto const &[section, values] : defaults) {
    if (parser->get_parsed_data().contains(section)) {
      for (auto const &[entry, value] : values) {
        if (!parser->get_parsed_data()[section].contains(entry)) {
          parser->get_parsed_data()[section][entry] = value;
        }
      }
    } else {
      parser->get_parsed_data()[section] = values;
    }
  }

  parser->write_to_file(path.string());

  return std::make_shared<Configuration>(std::move(parser));
}

std::string Configuration::getEntry(const std::string &section,
                                    const std::string &name) {
  return this->parser->get_parsed_data().at(section).at(name);
}

void Configuration::addSection(const std::string &section) {
  this->parser->get_parsed_data()[section] = {};
}

void Configuration::addEntry(const std::string &section,
                             const std::string &entry,
                             const std::string &value) {
  this->parser->get_parsed_data()[section][entry] = value;
}
