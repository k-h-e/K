/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_STRINGTOOLS_H_
#define K_CORE_STRINGTOOLS_H_

#include <string>
#include <vector>
#include <unordered_set>

namespace K {
namespace Core {

class Interface;
class BlockingInStreamInterface;
class BlockingOutStreamInterface;

//! Tool functions dealing with strings.
class StringTools {
  public:
    static std::vector<std::string> Tokenize(const std::string &text, const std::string &separators,
                                             bool suppressEmptyTokens);
    static std::string Concatenate(const std::vector<std::string> &tokens, const std::string &separatorString);
    static void Trim(std::string *inOutText, const std::unordered_set<char> &invalidCharacters);
    static bool Parse(const std::string &text, int *outValue);
    static bool Parse(const std::string &text, float *outValue);
    static bool Parse(const std::string &text, double *outValue);
    static bool Parse(const char *text, unsigned int *outValue);
    static bool ParseHex(const char *text, unsigned int *outValue);
    static std::string ToLower(const std::string &text);
    static std::string ToBase64(const std::string &text);
    static std::string ToHex(const void *data, int dataSize);
    static std::string GetCleanClassName(const Interface *instance);
    static std::string GetCleanClassName(const Interface *instance, int maxNumSegments);
    static void Serialize(const std::string &text, BlockingOutStreamInterface *stream);
    static void Deserialize(std::string *text, BlockingInStreamInterface *stream);
    static void Serialize(const std::string &text, std::vector<uint8_t> *outBinary);
    static bool Deserialize(std::string *text, const void *binary, int binarySize);

  private:
    static const char * const base64EncodeTable;
    static const char * const hexEncodeTable;
};

}    // Namespace Core.
}    // namespace K.

#endif    // K_CORE_STRINGTOOLS_H_
