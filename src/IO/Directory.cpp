#include <K/IO/Directory.h>

#include <sys/stat.h>
#include <cerrno>

using std::string;
using std::shared_ptr;
using std::make_shared;
using std::unordered_map;

namespace K {
namespace IO {

Directory::Directory(const string &path) {
    path_      = path;
    directory_ = opendir(path.c_str());
    error_     = (directory_ == nullptr);
    atEnd_     = error_;
}

Directory::~Directory() {
    if (directory_) {
        (void)closedir(directory_);
    }
}

bool Directory::GetNextEntry(std::string *outName, bool *outIsDirectory, off_t *outSize) {
    if (!error_ && !atEnd_) {
        errno = 0;
        struct dirent *entry = readdir(directory_);
        if (entry) {
            *outName        = entry->d_name;
            *outIsDirectory = (entry->d_type == DT_DIR);

            if (!*outIsDirectory) {
                struct stat fileStats;
                if (!stat((path_ + "/" + *outName).c_str(), &fileStats)) {
                    *outSize = fileStats.st_size;
                    return true;
                }
            }
            else {
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

bool Directory::ErrorState() {
    return error_;
}

bool Directory::Create(const std::string &directory) {
    return (mkdir(directory.c_str(), 0700) == 0);
}

shared_ptr<unordered_map<std::string, off_t>> Directory::GetFiles(const std::string &directory) {
    auto result = make_shared<unordered_map<std::string, off_t>>();

    string name;
    bool   isDirectory;
    off_t  size;
    Directory reader(directory);
    while (reader.GetNextEntry(&name, &isDirectory, &size)) {
        if (!isDirectory) {
            (*result)[name] = size;
        }
    }

    if (reader.ErrorState()) {
        result.reset();
    }

    return result;
}

}    // Namesapce IO.
}    // Namespace K.