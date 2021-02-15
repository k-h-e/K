#include <K/IO/Directory.h>

#include <cerrno>

using std::string;

namespace K {
namespace IO {

Directory::Directory(const string &path) {
    directory_ = opendir(path.c_str());
    error_     = (directory_ == nullptr);
}

Directory::~Directory() {
    if (directory_) {
        (void)closedir(directory_);
    }
}

bool Directory::GetNextEntry(std::string *outName, bool *outIsDirectory) {
    if (!error_) {
        struct dirent *entry = readdir(directory_);
        if (entry) {
            *outName        = entry->d_name;
            *outIsDirectory = (entry->d_type == DT_DIR);
            return true;
        }
        else {
            if (errno != ENOENT) {
                error_ = true;
            }
        }
    }

    return false;
}

bool Directory::ErrorState() {
    return error_;
}

}    // Namesapce IO.
}    // Namespace K.
