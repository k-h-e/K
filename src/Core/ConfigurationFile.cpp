#include <K/Core/ConfigurationFile.h>

#include <fstream>
#include <K/Core/StringTools.h>

using std::endl;
using std::getline;
using std::ifstream;
using std::ios;
using std::ofstream;
using std::string;
using std::to_string;
using std::unordered_set;
using std::vector;
using K::Core::StringTools;

namespace K {
namespace Core {

ConfigurationFile::ConfigurationFile() {
    for (char character : string("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,-_")) {
        validCharacters_.insert(character);
    }
}

bool ConfigurationFile::SetValue(const string &section, const string &key, const string &value) {
    if (isSectionName(section) && isKey(key) && isValue(value)) {
        sections_[section][key] = value;
        return true;
    }
    return false;
}

bool ConfigurationFile::SetValue(const string &section, const string &key, float value) {
    return SetValue(section, key, to_string(value));
}

bool ConfigurationFile::GetValue(const std::string &section, const std::string &key, std::string *outValue) {
    auto sectionIter = sections_.find(section);
    if (sectionIter != sections_.end()) {
        auto keyIter = sectionIter->second.find(key);
        if (keyIter != sectionIter->second.end()) {
            *outValue = keyIter->second;
            return true;
        }
    }

    return false;
}

bool ConfigurationFile::GetValue(const std::string &section, const std::string &key, float *outValue) {
    string value;
    if (GetValue(section, key, &value)) {
        return StringTools::Parse(value, outValue);
    }

    return false;
}

bool ConfigurationFile::Save(const std::string &fileName) {
    ofstream stream(fileName, ios::out | ios::binary | ios::trunc);
    bool firstSection = true;
    for (auto &pair : sections_) {
        if (!firstSection) {
            stream << endl;
        }
        stream << "[" << pair.first << "]" << endl;
        for (auto &keyValuePair : pair.second) {
            stream << keyValuePair.first << "=" << keyValuePair.second << endl;
        }
        firstSection = false;
    }
    stream.close();

    return stream.good();
}

bool ConfigurationFile::Load(const std::string &fileName) {
    unordered_set<char> whiteSpace{ ' ', '\t' };

    string currentSection;
    sections_.clear();
    ifstream stream(fileName, ios::in | ios::binary);
    string line;
    while (getline(stream, line)) {
        if ((line.length() > 0u) && (line[0] == '[')) {
            StringTools::Trim(&line, unordered_set<char>{ ' ', '\t', '[', ']' });
            currentSection = line;
        }
        else {
            vector<string> tokens = StringTools::Tokenize(line, '=');
            if (tokens.size() == 2u) {
                StringTools::Trim(&tokens[0], whiteSpace);
                StringTools::Trim(&tokens[1], whiteSpace);
                if (!SetValue(currentSection, tokens[0], tokens[1])) {
                    sections_.clear();
                    return false;
                }
            }
            else {
                StringTools::Trim(&line, whiteSpace);
                if (!line.empty()) {
                    sections_.clear();
                    return false;
                }
            }
        }
    }

    if (stream.eof()) {
        return true;
    }

    sections_.clear();
    return false;
}

bool ConfigurationFile::isSectionName(const std::string &text) {
    return isValue(text);
}

bool ConfigurationFile::isKey(const std::string &text) {
    return isValue(text);
}

bool ConfigurationFile::isValue(const std::string &text) {
    if (text.length() > 80u) {
        return false;
    }
    for (char character : text) {
        if (validCharacters_.find(character) == validCharacters_.end()) {
            return false;
        }
    }
    return true;
}

}    // Namespace Core.
}    // Namespace K.
