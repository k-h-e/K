#include <K/IO/ConfigurationFile.h>

#include <K/Core/StreamOperations.h>
#include <K/Core/StringTools.h>
#include <K/Core/Result.h>
#include <K/IO/File.h>
#include <K/IO/StreamBuffer.h>

using std::sprintf;
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

ConfigurationFile::ConfigurationFile() :
        validCharacters_("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,-_:/ "),
        errorState_(false) {
    // Nop.
}

bool ConfigurationFile::ErrorState() const {
    return errorState_;
}

void ConfigurationFile::SetValue(const string &section, const string &key, const string &value) {
    if (!errorState_ && IsSectionName(section) && IsKey(key) && IsValue(value)) {
        sections_[section][key] = value;
    } else {
        errorState_ = true;
    }
}

void ConfigurationFile::SetValue(const string &section, const string &key, float value) {
    char buffer[100];
    sprintf(buffer, "%f", value);
    SetValue(section, key, string(buffer));
}

void ConfigurationFile::SetValue(const string &section, const string &key, double value) {
    char buffer[100];
    sprintf(buffer, "%.8f", value);
    SetValue(section, key, string(buffer));
}

void ConfigurationFile::SetValue(const string &section, const string &key, bool value) {
    SetValue(section, key, string(value ? "true" : "false"));
}

void ConfigurationFile::GetValue(const string &section, const string &key, string *outValue) {
    if (!errorState_) {
        auto sectionIter = sections_.find(section);
        if (sectionIter != sections_.end()) {
            auto keyIter = sectionIter->second.find(key);
            if (keyIter != sectionIter->second.end()) {
                *outValue = keyIter->second;
                return;
            }
        }
    }

    errorState_ = true;
}

void ConfigurationFile::GetValue(const string &section, const string &key, float *outValue) {
    if (!errorState_) {
        string value;
        GetValue(section, key, &value);
        if (!errorState_) {
            if (StringTools::Parse(value, outValue)) {
                return;
            }
        }
    }

    errorState_ = true;
}

void ConfigurationFile::GetValue(const string &section, const string &key, double *outValue) {
    if (!errorState_) {
        string value;
        GetValue(section, key, &value);
        if (!errorState_) {
            if (StringTools::Parse(value, outValue)) {
                return;
            }
        }
    }

    errorState_ = true;
}

void ConfigurationFile::GetValue(const string &section, const string &key, bool *outValue) {
    if (!errorState_) {
        string value;
        GetValue(section, key, &value);
        if (!errorState_) {
            if (value == "true") {
                *outValue = true;
                return;
            } else if (value == "false") {
                *outValue = false;
                return;
            }
        }
    }

    errorState_ = true;
}

void ConfigurationFile::Save(const string &fileName) {
    if (!errorState_) {
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

        if (!result->Success()) {
            errorState_ = true;
        }
    }
}

void ConfigurationFile::Load(const string &fileName) {
    sections_.clear();
    errorState_ = false;

    StreamBuffer stream(make_shared<File>(fileName, File::AccessMode::ReadOnly, false),
                        File::AccessMode::ReadOnly, 4 * 1024);
    unordered_set<char> whiteSpace{ ' ', '\t' };
    string currentSection;
    string line;
    bool bad = false;
    while (!stream.ReadFailed() && !stream.Eof() && !bad) {
        Read(&stream, '\n', &line);
        if (!stream.ReadFailed()) {
            if ((line.length() > 0u) && (line[0] == '[')) {
                StringTools::Trim(&line, unordered_set<char>{ ' ', '\t', '[', ']' });
                currentSection = line;
            }
            else {
                vector<string> tokens = StringTools::Tokenize(line, '=');
                if (tokens.size() == 2u) {
                    StringTools::Trim(&tokens[0], whiteSpace);
                    StringTools::Trim(&tokens[1], whiteSpace);
                    SetValue(currentSection, tokens[0], tokens[1]);
                }
                else {
                    StringTools::Trim(&line, whiteSpace);
                    if (!line.empty()) {
                        bad = true;
                    }
                }
            }
        }
    }

    if (stream.ReadFailed() || bad) {
        sections_.clear();
        errorState_ = true;
    }
}

bool ConfigurationFile::IsSectionName(const string &text) {
    return IsValue(text);
}

bool ConfigurationFile::IsKey(const string &text) {
    return IsValue(text);
}

bool ConfigurationFile::IsValue(const string &text) {
    return validCharacters_.IsValid(text);
}

}    // Namespace IO.
}    // Namespace K.
