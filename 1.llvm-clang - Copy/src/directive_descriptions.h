#ifndef DIRECTIVE_DESCRIPTIONS_H
#define DIRECTIVE_DESCRIPTIONS_H

#include <string>
#include <unordered_map>

class DirectiveDescriptions {
public:
    static std::string getDescription(const std::string& directive);
    static std::string getIRTransformation(const std::string& directive);

private:
    static const std::unordered_map<std::string, std::string> directiveDescriptions;
    static const std::unordered_map<std::string, std::string> irTransformations;
};

#endif // DIRECTIVE_DESCRIPTIONS_H