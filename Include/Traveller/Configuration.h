#pragma once

#include "ini-parser.hh"
#include <memory>

class Configuration {
public:
  static std::shared_ptr<Configuration>
  getByName(const std::string &name, const ini::parsed_data &defaults);

  std::string getEntry(const std::string &section, const std::string &name);

  void addSection(const std::string &section);

  void addEntry(const std::string &section, const std::string &entry,
                const std::string &value);

  Configuration(
      std::unique_ptr<ini::parser<ini::comment_char::hash_tag>> parser)
      : parser(std::move(parser)) {}

  std::unique_ptr<ini::parser<ini::comment_char::hash_tag>> parser;
};
