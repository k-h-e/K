#ifndef K_CORE_STRINGTOOLS_H_
#define K_CORE_STRINGTOOLS_H_

#include <string>
#include <vector>
#include <unordered_set>

namespace K {
namespace Core {

class Interface;
class ItemReadInterface;
class ItemWriteInterface;

class StringTools {
  public:
    static std::vector<std::string> Tokenize(const std::string &text, char separator);
    static std::vector<std::string> Tokenize(const std::string &text, const std::string &separators,
                                             bool suppressEmptyTokens);
    static std::string Concatenate(const std::vector<std::string> &tokens, const std::string &separatorString);
    static void Trim(std::string *inOutText, const std::unordered_set<char> &invalidCharacters);
    static bool Parse(const std::string &text, int *outValue);
    static bool Parse(const std::string &text, float *outValue);
    static bool Parse(const std::string &text, double *outValue);
    static std::string GetCleanClassName(Interface *instance);
    static std::string GetCleanClassName(Interface *instance, int maxNumSegments);
    static void Serialize(const std::string &text, ItemWriteInterface *stream);
    static void Deserialize(std::string *text, ItemReadInterface *stream);
};

}    // Namespace Core.
}    // namespace K.

#endif    // K_CORE_STRINGTOOLS_H_
