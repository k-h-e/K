#include <K/IO/ConfigurationFile.h>

#include <K/Core/StringTools.h>
#include <K/Core/Result.h>
#include <K/IO/File.h>
#include <K/IO/StreamBuffer.h>
#include <K/IO/IOTools.h>

using std::string;
using std::to_string;
using std::unordered_set;
using std::vector;
using std::make_shared;
using K::Core::StringTools;
using K::Core::Result;
using K::IO::File;
using K::IO::StreamBuffer;

namespace K {
namespace IO {

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
    auto result = make_shared<Result>();
    {
        StreamBuffer stream(make_shared<File>(fileName, File::AccessMode::WriteOnly, true),
                            File::AccessMode::WriteOnly, 4 * 1024, result);
        bool firstSection = true;
        for (auto &pair : sections_) {
            if (!firstSection) {
                stream << "\n";
            }
            stream << "[" << pair.first << "]\n";
            for (auto &keyValuePair : pair.second) {
                stream << keyValuePair.first << " = " << keyValuePair.second << "\n";
            }
            firstSection = false;
        }
    }

    return result->Success();
}

bool ConfigurationFile::Load(const std::string &fileName) {
    StreamBuffer stream(make_shared<File>(fileName, File::AccessMode::ReadOnly, false),
                        File::AccessMode::ReadOnly, 4 * 1024);
    sections_.clear();

    unordered_set<char> whiteSpace{ ' ', '\t' };
    string currentSection;
    string line;
    while (Read(&stream, '\n', &line)) {
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

    if (stream.ErrorState()) {
        sections_.clear();
        return false;
    }
    else {
        return true;
    }
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

}    // Namespace IO.
}    // Namespace K.
