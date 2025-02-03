/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Core/StringTools.h>

#include <cstring>
#include <limits>
#include <sstream>
#include <typeinfo>

#include <K/Core/BlockingInStreamInterface.h>
#include <K/Core/BlockingOutStreamInterface.h>
#include <K/Core/NumberTools.h>

using std::invalid_argument;
using std::istringstream;
using std::memcpy;
using std::numeric_limits;
using std::ostringstream;
using std::out_of_range;
using std::string;
using std::unordered_set;
using std::vector;
using K::Core::NumberTools;

namespace K {
namespace Core {

const char * const StringTools::base64EncodeTable { "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
                                                        "0123456789+/" };
const char * const StringTools::hexEncodeTable    { "0123456789abcdef" };

vector<string> StringTools::Tokenize(const string &text, const string &separators, bool suppressEmptyTokens) {
    vector<string> tokens;
    string         remainder { text };
    while (true) {
        size_t position { remainder.find_first_of(separators) };
        if (position == string::npos) {
            if (!suppressEmptyTokens || remainder.length()) {
                tokens.push_back(remainder);
            }
            return tokens;
        } else {
            if (!suppressEmptyTokens || position) {
                tokens.push_back(remainder.substr(0, position));
            }
            remainder.erase(0, position + 1u);
        }
    }
}

string StringTools::Concatenate(const vector<string> &tokens, const string &separatorString) {
    ostringstream stream;
    bool          first { true };
    for (const string &token : tokens) {
        if (!first) {
            stream << separatorString;
        }
        stream << token;
        first = false;
    }

    return stream.str();
}

void StringTools::Trim(string &inOutText, const unordered_set<char> &invalidCharacters, bool trimLeft, bool trimRight) {
    if (inOutText.length() > 0u) {
        int left  { 0 };
        int right { static_cast<int>(inOutText.length()) - 1 };
        if (trimLeft) {
            while ((left <= right) && (invalidCharacters.find(inOutText[left]) != invalidCharacters.end())) {
                ++left;
            }
        }
        if (trimRight) {
            while ((left <= right) && (invalidCharacters.find(inOutText[right]) != invalidCharacters.end())) {
                --right;
            }
        }

        string result;
        for (int i = left; i <= right; ++i) {
            result.push_back(inOutText[i]);
        }
        inOutText = result;
    }
}

void StringTools::Trim(string &inOutText, const unordered_set<char> &invalidCharacters) {
    Trim(inOutText, invalidCharacters, true, true);
}

bool StringTools::Parse(const string &text, int &outValue) {
    try {
        outValue = stoi(text);
        return true;
    }
    catch (const invalid_argument &) {}
    catch (const out_of_range &) {}

    return false;
}

bool StringTools::Parse(const string &text, float &outValue) {
    try {
        outValue = stof(text);
        return true;
    }
    catch (const invalid_argument &) {}
    catch (const out_of_range &) {}

    return false;
}

bool StringTools::Parse(const string &text, double &outValue) {
    try {
        outValue = stod(text);
        return true;
    }
    catch (const invalid_argument &) {}
    catch (const out_of_range &) {}

    return false;
}

bool StringTools::Parse(const char *text, unsigned int &outValue) {
    unsigned int value { 0u };
    while (true) {
        char character { *text };

        unsigned int digitValue { 0u };
        switch (character) {
            case '\0':
                outValue = value;
                return true;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                digitValue = static_cast<unsigned int>(character - '0');
                break;
            default:
                return false;
        }

        value *= 10u;
        value += digitValue;

        ++text;
    }

    return false;
}

bool StringTools::ParseHex(const char *text, unsigned int &outValue) {
    unsigned int value { 0u };
    while (true) {
        char character { *text };

        unsigned int digitValue { 0u };
        switch (character) {
            case '\0':
                outValue = value;
                return true;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                digitValue = static_cast<unsigned int>(character - '0');
                break;
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
                digitValue = static_cast<unsigned int>(character - 'a') + 10u;
                break;
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
                digitValue = static_cast<unsigned int>(character - 'A') + 10u;
                break;
            default:
                return false;
        }

        value *= 16u;
        value += digitValue;

        ++text;
    }

    return false;
}

string StringTools::ToLower(const string &text) {
    string result;
    for (char character : text) {
        result.push_back(tolower(character));
    }
    return result;
}

string StringTools::ToBase64(const string &text) {
    string result;

    uint8_t in[3];
    char    out[4];

    int cursor  { 0 };
    int numLeft { static_cast<int>(text.length()) };
    while (numLeft) {
        // Get next 3-byte block of binary input data...
        int numBytes { 0 };
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

    const uint8_t *dataPtr { static_cast<const uint8_t *>(data) };
    for (int i = 0; i < dataSize; ++i) {
        uint8_t byte { *dataPtr++ };
        result.push_back(hexEncodeTable[byte >> 4]);
        result.push_back(hexEncodeTable[byte & 0xfu]);
    }

    return result;
}

string StringTools::AddressToHex(const void *data) {
    auto address = reinterpret_cast<uintptr_t>(data);
    NumberTools::Reverse(&address, sizeof(address));
    string hexText { ToHex(&address, sizeof(address)) };
    unordered_set<char> invalid;
    invalid.insert('0');
    Trim(hexText, invalid, true, false);
    return hexText;
}

string StringTools::GetCleanClassName(const Interface &instance) {
    return GetCleanClassName(instance, numeric_limits<int>::max());
}

string StringTools::GetCleanClassName(const Interface &instance, int maxNumSegments) {
    NumberTools::ClampMin(maxNumSegments, 1);

    string className { typeid(instance).name() };
    if (className.length() && (className[0] == 'N')) {
        className.erase(0, 1);
    }
    if (className.length() && (className[className.length() - 1u] == 'E')) {
        className.erase(className.length() - 1u, 1);
    }
    vector<string> tokens    { StringTools::Tokenize(className, "1234567890", true) };
    int            numTokens { static_cast<int>(tokens.size()) };
    if (numTokens > maxNumSegments) {
        vector<string> newTokens;
        for (int i = numTokens - maxNumSegments; i < numTokens; ++i) {
            newTokens.push_back(tokens[i]);
        }
        tokens = newTokens;
    }

    return StringTools::Concatenate(tokens, "::");
}

void StringTools::Serialize(const string &text, BlockingOutStreamInterface &stream) {
    uint32_t size { static_cast<uint32_t>(text.size()) };
    stream << size;
    if (size) {
        WriteItem(stream, &text[0], size * sizeof(string::value_type));
    }
}

void StringTools::Deserialize(string &text, BlockingInStreamInterface &stream) {
    uint32_t size;
    stream >> size;
    if (!stream.ErrorState()) {
        text.clear();
        if (size) {
            text.resize(size);
            ReadItem(stream, &text[0], size * sizeof(string::value_type));
        }
    }
}

void StringTools::Serialize(const string &text, vector<uint8_t> &outBinary) {
    uint32_t size { static_cast<uint32_t>(text.size()) };
    outBinary.resize(sizeof(size) + size*sizeof(string::value_type));
    memcpy(&outBinary[0], &size, sizeof(size));
    if (size) {
        memcpy(&outBinary[sizeof(size)], &text[0], size*sizeof(string::value_type));
    }
}

bool StringTools::Deserialize(string &text, const void *binary, int binarySize) {
    uint32_t size;
    if (binarySize >= static_cast<int>(sizeof(size))) {
        memcpy(&size, binary, sizeof(size));
        if (static_cast<int>(sizeof(size) + size*sizeof(string::value_type)) >= binarySize) {
            text.resize(size);
            if (size) {
                const uint8_t *binaryBytes { static_cast<const uint8_t *>(binary) };
                memcpy(&text[0], &binaryBytes[sizeof(size)], size*sizeof(string::value_type));
            }
            return true;
        }
    }

    text.clear();
    return false;
}

}    // Namespace Core.
}    // namespace K.
