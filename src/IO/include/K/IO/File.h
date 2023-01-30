#ifndef K_IO_FILE_H_
#define K_IO_FILE_H_

#include <K/Core/SeekableBlockingIOStreamInterface.h>
#include <K/IO/Path.h>

namespace K {
namespace IO {

//! File.
class File : public virtual Core::SeekableBlockingIOStreamInterface {
  public:
    enum class AccessMode { ReadOnly,
                            WriteOnly,
                            ReadWrite };

    File(const K::IO::Path &fileName, AccessMode accessMode, bool truncate);
    File(const File &other)             = delete;
    File &operator=(const File &other)  = delete;
    File(File &&other)                  = delete;
    File &operator=(File &&other)       = delete;
    ~File();

    int ReadBlocking(void *buffer, int bufferSize) override;
    int WriteBlocking(const void *data, int dataSize) override;
    void Seek(int64_t position) override;
    void RecoverAndSeek(int64_t position) override;
    int64_t StreamPosition() const override;
    bool ErrorState() const override;
    Error StreamError() const override;
    void SetCloseResultAcceptor(const std::shared_ptr<Core::ResultAcceptor> &resultAcceptor) override;

    static void AccessModeToFlags(AccessMode accessMode, bool *outReadable, bool *outWritable);
    //! Renames the specified file, moving it between directories if required.
    /*!
     *  Can also be used on directories.
     */
    static bool Rename(const K::IO::Path &oldFileName, const K::IO::Path &newFileName);
    //! Deletes the specified file.
    static bool Delete(const K::IO::Path &fileName);

  private:
    K::IO::Path                           fileName_;
    int                                   fd_;
    bool                                  readable_;
    bool                                  writable_;
    int64_t                               position_;
    Error                                 error_;
    std::shared_ptr<Core::ResultAcceptor> closeResultAcceptor_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_FILE_H_
