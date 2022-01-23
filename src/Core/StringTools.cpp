////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <K/Core/StringTools.h>

#include <limits>
#include <sstream>
#include <typeinfo>
#include <K/Core/Interface.h>
#include <K/Core/ItemReadInterface.h>
#include <K/Core/ItemWriteInterface.h>
#include <K/Core/NumberTools.h>

using std::string;
using std::istringstream;
using std::ostringstream;
using std::vector;
using std::invalid_argument;
using std::out_of_range;
using std::numeric_limits;
using K::Core::NumberTools;

namespace K {
namespace Core {

const char * const StringTools::base64EncodeTable = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const char * const StringTools::hexEncodeTable    = "0123456789abcdef";

vector<string> StringTools::Tokenize(const string &text, char separator) {
    vector<string> tokens;
    istringstream  stream(text);
    string         token;
    while (getline(stream, token, separator)) {
        tokens.push_back(token);
    }
    return tokens;
}

vector<string> StringTools::Tokenize(const std::string &text, const std::string &separators, bool suppressEmptyTokens) {
    vector<string> tokens;
    string         remainder = text;
    while (true) {
        size_t position = remainder.find_first_of(separators);
        if (position == string::npos) {
            if (!suppressEmptyTokens || remainder.length()) {
                tokens.push_back(remainder);
            }
            return tokens;
        }
        else {
            if (!suppressEmptyTokens || position) {
                tokens.push_back(remainder.substr(0, position));
            }
            remainder.erase(0, position + 1u);
        }
    }
}

string StringTools::Concatenate(const std::vector<std::string> &tokens, const std::string &separatorString) {
    ostringstream stream;
    bool first = true;
    for (const string &token : tokens) {
        if (!first) {
            stream << separatorString;
        }
        stream << token;
        first = false;
    }

    return stream.str();
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

string StringTools::ToLower(const std::string &text) {
    string result;
    for (char character : text) {
        result.push_back(tolower(character));
    }
    return result;
}

string StringTools::ToBase64(const std::string &text) {
    string result;

    uint8_t in[3];
    char    out[4];

    int cursor  = 0;
    int numLeft = static_cast<int>(text.length());
    while (numLeft) {
        // Get next 3-byte block of binary input data...
        int numBytes = 0;
        for (int i = 0; i < 3; ++i) {
            if (numLeft) {
                in[i] = static_cast<uint8_t>(text[cursor++]);
                --numLeft;
                ++numBytes;
            } else {
                in[i] = 0;
            }
        }

        // Generate corresponding 4-byte base64 block...
        out[0] = base64EncodeTable[in[0] >> 2];
        out[1] = base64EncodeTable[((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4)];
        out[2] = (numBytes > 1) ? base64EncodeTable[((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6)] : '=';
        out[3] = (numBytes > 2) ? base64EncodeTable[in[2] & 0x3f] : '=';
        for (int i = 0; i < 4; ++i) {
            result.push_back(static_cast<char>(out[i]));
        }
    }

    return result;
}

string StringTools::ToHex(const void *data, int dataSize) {
    string result;

    const uint8_t *dataPtr = static_cast<const uint8_t *>(data);
    for (int i = 0; i < dataSize; ++i) {
        uint8_t byte = *dataPtr++;
        result.push_back(hexEncodeTable[byte >> 4]);
        result.push_back(hexEncodeTable[byte & 0xfu]);
    }

    return result;
}

string StringTools::GetCleanClassName(const Interface *instance) {
    return GetCleanClassName(instance, numeric_limits<int>::max());
}

string StringTools::GetCleanClassName(const Interface *instance, int maxNumSegments) {
    NumberTools::ClampMin(&maxNumSegments, 1);

    string className = instance ? typeid(*instance).name() : "";
    if (className.length() && (className[0] == 'N')) {
        className.erase(0, 1);
    }
    if (className.length() && (className[className.length() - 1u] == 'E')) {
        className.erase(className.length() - 1u, 1);
    }
    vector<string> tokens = StringTools::Tokenize(className, "1234567890", true);
    int numTokens = static_cast<int>(tokens.size());
    if (numTokens > maxNumSegments) {
        vector<string> newTokens;
        for (int i = numTokens - maxNumSegments; i < numTokens; ++i) {
            newTokens.push_back(tokens[i]);
        }
        tokens = newTokens;
    }

    return StringTools::Concatenate(tokens, "::");
}

void StringTools::Serialize(const string &text, ItemWriteInterface *stream) {
    uint32_t size = static_cast<uint32_t>(text.size());
    stream->WriteItem(&size, sizeof(size));
    if (size) {
        stream->WriteItem(&text[0], size * sizeof(string::value_type));
    }
}

void StringTools::Deserialize(string *text, ItemReadInterface *stream) {
    uint32_t size;
    stream->ReadItem(&size, sizeof(size));
    if (stream->Good()) {
        text->clear();
        if (size) {
            text->resize(size);
            stream->ReadItem(&(*text)[0], size * sizeof(string::value_type));
        }
    }
}

}    // Namespace Core.
}    // namespace K.
