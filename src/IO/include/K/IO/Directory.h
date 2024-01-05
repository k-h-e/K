/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_IO_DIRECTORY_H_
#define K_IO_DIRECTORY_H_

#include <dirent.h>
#include <memory>
#include <unordered_map>
#include <string>
#include <K/Core/ErrorStateInterface.h>
#include <K/IO/Path.h>

namespace K {
namespace IO {

//! Reads a directory.
class Directory : public virtual K::Core::ErrorStateInterface {
  public:
    Directory(const K::IO::Path &path);
    Directory(const Directory &other)            = delete;
    Directory &operator=(const Directory &other) = delete;
    Directory(Directory &&other)                 = delete;
    Directory &operator=(Directory &&other)      = delete;
    ~Directory();

    //! Retrieves the next directory entry.
    /*!
     *  \return
     *  <c>false</c> in case there are no more directory entries. In this case, the output parameters are undefined and
     *  ErrorState() should be inspected to check whether or not directory reading finished because of an encountered
     *  error.
     */
    bool GetNextEntry(std::string *outName, bool *outIsDirectory, off_t *outSize);
    bool ErrorState() const;

    //! Creates the specified directory.
    /*!
     *  \return
     *  <c>false</c> in case of failure.
     */
    static bool Create(const K::IO::Path &directory);
    //! Reads the specified directory and returns information on all file-type entries.
    /*!
     * \return
     * Map mapping file name to file size, <c>nullptr</c> in case of failure.
     */
    static std::shared_ptr<std::unordered_map<std::string, off_t>> GetFiles(const K::IO::Path &directory);

  private:
    K::IO::Path path_;
    DIR         *directory_;
    bool        atEnd_;
    bool        error_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_DIRECTORY_H_
