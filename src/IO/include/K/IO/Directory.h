#ifndef K_IO_DIRECTORY_H_
#define K_IO_DIRECTORY_H_

#include <dirent.h>
#include <string>
#include <K/Core/ErrorStateInterface.h>

namespace K {
namespace IO {

//! Reads a directory.
class Directory : public virtual K::Core::ErrorStateInterface {
  public:
    Directory(const std::string &path);
    Directory(const Directory &other)             = delete;
    Directory &operator=(const Directory &other)  = delete;
    Directory(const Directory &&other)            = delete;
    Directory &operator=(const Directory &&other) = delete;
    ~Directory();

    //! Retrieves the next directory entry.
    /*!
     *  \return
     *  <c>false</c> in case there are no more directory entries. In this case, the output parameters are undefined and
     *  ErrorState() should be inspected to check whether or not directory reading finished because of an encountered
     *  error.
     */
    bool GetNextEntry(std::string *outName, bool *outIsDirectory);
    bool ErrorState();

  private:
    DIR  *directory_;
    bool error_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_DIRECTORY_H_
