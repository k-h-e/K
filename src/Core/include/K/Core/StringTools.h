#ifndef K_CORE_STRINGTOOLS_H_
#define K_CORE_STRINGTOOLS_H_

#include <string>
#include <vector>
#include <unordered_set>

namespace K {
namespace Core {

class StringTools {
  public:
    static void Tokenize(const std::string &text, char separator, std::vector<std::string> *outTokens);
    static void Trim(std::string *inOutText, const std::unordered_set<char> &invalidCharacters);
    static bool Parse(const std::string &text, int *outValue);
    static bool Parse(const std::string &text, float *outValue);
    static bool Parse(const std::string &text, double *outValue);
};

}    // Namespace Core.
}    // namespace K.

#endif    // K_CORE_STRINGTOOLS_H_
