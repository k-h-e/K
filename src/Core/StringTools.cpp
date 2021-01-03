#include <K/Core/StringTools.h>

#include <sstream>
#include <typeinfo>
#include <K/Core/Interface.h>

using std::string;
using std::istringstream;
using std::ostringstream;
using std::vector;
using std::invalid_argument;
using std::out_of_range;

namespace K {
namespace Core {

void StringTools::Tokenize(const string &text, char separator, vector<string> *outTokens) {
    outTokens->clear();
    istringstream stream(text);
    string token;
    while (getline(stream, token, separator)) {
        outTokens->push_back(token);
    }
}

void StringTools::Tokenize(const std::string &text, const std::string &separators, bool suppressEmptyTokens,
                           std::vector<std::string> *outTokens) {
    outTokens->clear();
    string remainder = text;
    while (true) {
        size_t position = remainder.find_first_of(separators);
        if (position == string::npos) {
            if (!suppressEmptyTokens || remainder.length()) {
                outTokens->push_back(remainder);
            }
            return;
        }
        else {
            if (!suppressEmptyTokens || position) {
                outTokens->push_back(remainder.substr(0, position));
            }
            remainder.erase(0, position + 1u);
        }
    }
}

void StringTools::Concatenate(const std::vector<std::string> &tokens, const std::string &separatorString,
                              std::string *outResult) {
    ostringstream stream;
    bool first = true;
    for (const string &token : tokens) {
        if (!first) {
            stream << separatorString;
        }
        stream << token;
        first = false;
    }

    *outResult = stream.str();
}

void StringTools::Trim(std::string *inOutText, const std::unordered_set<char> &invalidCharacters) {
    if (inOutText->length() > 0u) {
        int left  = 0;
        int right = static_cast<int>(inOutText->length()) - 1;
        while ((left <= right) && (invalidCharacters.find((*inOutText)[left]) != invalidCharacters.end())) {
            ++left;
        }
        while ((left <= right) && (invalidCharacters.find((*inOutText)[right]) != invalidCharacters.end())) {
            --right;
        }

        string result;
        for (int i = left; i <= right; ++i) {
            result.push_back((*inOutText)[i]);
        }
        *inOutText = result;
    }
}

bool StringTools::Parse(const std::string &text, int *outValue) {
    try {
        *outValue = stoi(text);
        return true;
    }
    catch (const invalid_argument &) {}
    catch (const out_of_range &) {}

    return false;
}

bool StringTools::Parse(const std::string &text, float *outValue) {
    try {
        *outValue = stof(text);
        return true;
    }
    catch (const invalid_argument &) {}
    catch (const out_of_range &) {}

    return false;
}

bool StringTools::Parse(const std::string &text, double *outValue) {
    try {
        *outValue = stod(text);
        return true;
    }
    catch (const invalid_argument &) {}
    catch (const out_of_range &) {}

    return false;
}

string StringTools::GetCleanClassName(Interface *instance) {
    string className = instance ? typeid(*instance).name() : "";
    if (className.length() && (className[0] == 'N')) {
        className.erase(0, 1);
    }
    if (className.length() && (className[className.length() - 1u] == 'E')) {
        className.erase(className.length() - 1u, 1);
    }
    vector<string> tokens;
    StringTools::Tokenize(className, "1234567890", true, &tokens);
    StringTools::Concatenate(tokens, "::", &className);

    return className;
}

}    // Namespace Core.
}    // namespace K.
