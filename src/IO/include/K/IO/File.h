#ifndef K_IO_FILE_H_
#define K_IO_FILE_H_

#include <string>
#include <K/IO/BlockingStreamCore.h>
#include <K/IO/SeekableBlockingStreamInterface.h>

namespace K {
namespace IO {

//! File.
class File : public BlockingStreamCore,
             public virtual SeekableBlockingStreamInterface {
  public:
    enum class AccessMode { ReadOnly,
                            WriteOnly,
                            ReadWrite };

    File(const std::string &fileName, AccessMode accessMode, bool truncate);
    File(const std::string &fileName, AccessMode accessMode, bool truncate,
         const std::shared_ptr<Core::Result> &resultAcceptor);
    File(const File &other)             = delete;
    File &operator=(const File &other)  = delete;
    File(const File &&other)            = delete;
    File &operator=(const File &&other) = delete;
    ~File();

    int Read(void *outBuffer, int bufferSize) override;
    int Write(const void *data, int dataSize) override;
    bool Seek(int64_t position) override;
    int64_t StreamPosition() override;
    bool Eof() override;
    bool ErrorState() override;
    void SetFinalResultAcceptor(const std::shared_ptr<Core::Result> &resultAcceptor) override;

    static void AccessModeToFlags(AccessMode accessMode, bool *outReadable, bool *outWritable);

  private:
    int                           fd_;
    bool                          readable_;
    bool                          writable_;
    int64_t                       position_;
    bool                          eof_;
    bool                          error_;
    std::shared_ptr<Core::Result> finalResultAcceptor_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_FILE_H_
