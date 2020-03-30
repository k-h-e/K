#include <K/Core/StringTools.h>

#include <sstream>

using std::string;
using std::istringstream;
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

}    // Namespace Core.
}    // namespace K.
