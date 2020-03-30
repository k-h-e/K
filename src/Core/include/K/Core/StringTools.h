#ifndef K_CORE_STRINGTOOLS_H_
#define K_CORE_STRINGTOOLS_H_

#include <string>
#include <vector>

namespace K {
namespace Core {

class StringTools {
  public:
    static void Tokenize(const std::string &text, char separator, std::vector<std::string> *outTokens);
    static bool Parse(const std::string &text, int *outValue);
    static bool Parse(const std::string &text, float *outValue);
    static bool Parse(const std::string &text, double *outValue);
};

}    // Namespace Core.
}    // namespace K.

#endif    // K_CORE_STRINGTOOLS_H_
