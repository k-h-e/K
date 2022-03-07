#ifndef K_IO_FILE_H_
#define K_IO_FILE_H_

#include <string>
#include <K/Core/SeekableBlockingIOStreamInterface.h>
#include <K/IO/BlockingIOStreamCore.h>


namespace K {
namespace IO {

//! File.
class File : public BlockingIOStreamCore,
             public virtual Core::SeekableBlockingIOStreamInterface {
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

    int ReadBlocking(void *buffer, int bufferSize) override;
    bool ReadFailed() const override;
    void ClearReadFailed() override;
    int WriteBlocking(const void *data, int dataSize) override;
    bool WriteFailed() const override;
    void ClearWriteFailed() override;
    void Seek(int64_t position) override;
    int64_t StreamPosition() const override;
    bool Eof() const override;
    bool ErrorState() const override;
    void SetFinalResultAcceptor(const std::shared_ptr<Core::Result> &resultAcceptor) override;

    static void AccessModeToFlags(AccessMode accessMode, bool *outReadable, bool *outWritable);
    //! Renames the specified file, moving it between directories if required.
    /*!
     *  Can also be used on directories.
     */
    static bool Rename(const std::string &oldFileName, const std::string &newFileName);

  private:
    std::string                   fileName_;
    int                           fd_;
    bool                          readable_;
    bool                          writable_;
    int64_t                       position_;
    bool                          readFailed_;
    bool                          writeFailed_;
    bool                          eof_;
    bool                          error_;
    std::shared_ptr<Core::Result> finalResultAcceptor_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_FILE_H_
