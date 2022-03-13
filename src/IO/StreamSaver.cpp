#include <K/IO/StreamSaver.h>

#include <K/Core/BinaryWriter.h>
#include <K/IO/File.h>
#include <K/IO/StreamBuffer.h>

using std::make_shared;
using std::make_unique;
using std::string;
using K::Core::BinaryWriter;

namespace K {
namespace IO {

StreamSaver::StreamSaver(const string &fileName)
        : writer_(make_unique<BinaryWriter>(
                      make_shared<StreamBuffer>(make_shared<File>(fileName, File::AccessMode::WriteOnly, true),
                                                File::AccessMode::WriteOnly, 4 * 1024))) {
    // Nop.
}

void StreamSaver::HandleStreamData(const void *data, int dataSize) {
    if (writer_) {
        writer_->WriteItem(data, dataSize);
        if (writer_->WriteFailed()) {
            writer_.reset();
        }
    }
}

void StreamSaver::HandleEof() {
    writer_.reset();
}

void StreamSaver::HandleError() {
    writer_.reset();
}

}    // Namespace IO.
}    // Namespace K.
