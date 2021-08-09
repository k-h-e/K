#include <K/IO/StreamSaver.h>

#include <K/IO/File.h>

using std::make_shared;
using std::make_unique;
using std::string;
using K::IO::File;

namespace K {
namespace IO {

StreamSaver::StreamSaver(const string &fileName)
        : stream_(make_unique<StreamBuffer>(make_shared<File>(fileName, File::AccessMode::WriteOnly, true),
                                            File::AccessMode::WriteOnly, 4 * 1024)) {
    // Nop.
}

void StreamSaver::HandleStreamData(const void *data, int dataSize) {
    if (stream_) {
        stream_->WriteItem(data, dataSize);
        if (!stream_->Good()) {
            stream_.reset();
        }
    }
}

void StreamSaver::HandleEof() {
    stream_.reset();
}

void StreamSaver::HandleError() {
    stream_.reset();
}

}    // Namespace IO.
}    // Namespace K.
