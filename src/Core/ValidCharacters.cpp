/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Core/ValidCharacters.h>

using std::string;

namespace K {
namespace Core {

ValidCharacters::ValidCharacters(const string &validCharacters) {
    for (char character : validCharacters) {
        validCharacters_.insert(character);
    }
}

bool ValidCharacters::IsValid(const string &text) {
    for (char character : text) {
        if (validCharacters_.find(character) == validCharacters_.end()) {
            return false;
        }
    }

    return true;
}

string ValidCharacters::MakeValid(const string &text, char defaultValidCharacter) {
    string result;
    if (validCharacters_.find(defaultValidCharacter) != validCharacters_.end()) {
        for (char character : text) {
            if (validCharacters_.find(character) != validCharacters_.end()) {
                result.push_back(character);
            }
            else {
                result.push_back(defaultValidCharacter);
            }
        }
    }

    return result;
}

}    // Namespace Core.
}    // Namespace K.
