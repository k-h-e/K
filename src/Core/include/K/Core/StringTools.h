#ifndef K_CORE_STRINGTOOLS_H_
#define K_CORE_STRINGTOOLS_H_

#include <string>
#include <vector>
#include <unordered_set>

namespace K {
namespace Core {

class Interface;

class StringTools {
  public:
    static void Tokenize(const std::string &text, char separator, std::vector<std::string> *outTokens);
    static void Tokenize(const std::string &text, const std::string &separators, bool suppressEmptyTokens,
                         std::vector<std::string> *outTokens);
    static void Concatenate(const std::vector<std::string> &tokens, const std::string &separatorString,
                            std::string *outResult);
    static void Trim(std::string *inOutText, const std::unordered_set<char> &invalidCharacters);
    static bool Parse(const std::string &text, int *outValue);
    static bool Parse(const std::string &text, float *outValue);
    static bool Parse(const std::string &text, double *outValue);
    static std::string GetCleanClassName(Interface *instance);
};

}    // Namespace Core.
}    // namespace K.

#endif    // K_CORE_STRINGTOOLS_H_
