#ifndef K_CORE_VALIDCHARACTERS_H_
#define K_CORE_VALIDCHARACTERS_H_

#include <string>
#include <unordered_set>

namespace K {
namespace Core {

//! Specifies a set of characters deemed valid for use in strings for a specific purpose.
class ValidCharacters {
  public:
    //! Sets up the set of valid characters as the set of characters contained in the specified string.
    ValidCharacters(const std::string &validCharacters);
    ValidCharacters(const ValidCharacters &other)            = delete;
    ValidCharacters &operator=(const ValidCharacters &other) = delete;
    ValidCharacters(ValidCharacters &&other)                 = delete;
    ValidCharacters &operator=(ValidCharacters &&other)      = delete;

    //! Tells whether or not the specified string is valid with respect to the set of valid characters.
    bool IsValid(const std::string &text);
    //! Returns a valid version of the specified string, where all invalid characters have been replaced by the
    //! specified default valid character.
    /*!
     *  If the specified default valid character is not itself in the set of valid characters, the returned string will
     *  be empty.
     */
    std::string MakeValid(const std::string &text, char defaultValidCharacter);


  private:
    std::unordered_set<char> validCharacters_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_VALIDCHARACTERS_H_

