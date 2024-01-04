/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/IO/Directory.h>

#include <sys/stat.h>
#include <cerrno>
#include <K/Core/Log.h>

using std::string;
using std::shared_ptr;
using std::make_shared;
using std::unordered_map;
using K::IO::Path;
using K::Core::Log;

namespace K {
namespace IO {

Directory::Directory(const Path &path)
        : path_ { path },
          directory_ { nullptr} {
    if (!path.ErrorState()) {
        directory_ = opendir(path.ToOsPath().c_str());
    }

    error_ = (directory_ == nullptr);
    atEnd_ = error_;
}

Directory::~Directory() {
    if (directory_) {
        (void)closedir(directory_);
    }
}

bool Directory::GetNextEntry(std::string *outName, bool *outIsDirectory, off_t *outSize) {
    if (!error_ && !atEnd_) {
        errno = 0;
        struct dirent *entry { readdir(directory_) };
        if (entry) {
            *outName        = entry->d_name;
            *outIsDirectory = (entry->d_type == DT_DIR);

            if (!*outIsDirectory) {
                struct stat fileStats;
                if (!stat((path_ + *outName).ToOsPath().c_str(), &fileStats)) {
                    *outSize = fileStats.st_size;
                    return true;
                }
            } else {
                *outSize = 0u;
                return true;
            }
        }

        atEnd_ = true;
        if (errno != 0) {
            error_ = true;
        }
    }

    return false;
}

bool Directory::ErrorState() const {
    return error_;
}

bool Directory::Create(const Path &directory) {
    if (!directory.ErrorState()) {
        return (mkdir(directory.ToOsPath().c_str(), 0700) == 0);
    } else {
        return false;
    }
}

shared_ptr<unordered_map<std::string, off_t>> Directory::GetFiles(const Path &directory) {
    if (!directory.ErrorState()) {
        auto result { make_shared<unordered_map<std::string, off_t>>() };

        string name;
        bool   isDirectory;
        off_t  size;
        Directory reader { directory };
        while (reader.GetNextEntry(&name, &isDirectory, &size)) {
            if (!isDirectory) {
                (*result)[name] = size;
            }
        }

        if (!reader.ErrorState()) {
            return result;
        }
    }

    Log::Print(Log::Level::Error, nullptr, [&]{
        return "failed to read directory \"" + directory.ToShortString() + "\" !";
    });
    return nullptr;
}

}    // Namesapce IO.
}    // Namespace K.
