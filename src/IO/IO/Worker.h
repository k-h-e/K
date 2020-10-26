#ifndef K_EVENTS_IO_WORKER_H_
#define K_EVENTS_IO_WORKER_H_

#include <unordered_map>
#include <K/Core/ActionInterface.h>
#include <K/IO/IO.h>

namespace K {
namespace IO {

//! Worker for the central I/O mechanism.
class IO::Worker : public virtual K::Core::ActionInterface {
  public:
    Worker(int pipe, std::shared_ptr<SharedState> sharedState);
    Worker(const Worker &other)            = delete;
    Worker &operator=(const Worker &other) = delete;
    Worker(Worker &&other)                 = delete;
    Worker &operator=(Worker &&other)      = delete;
    ~Worker();
    void ExecuteAction();

  private:
    static const int bufferSize = 8192;

    struct FileDescriptorInfo {
        IO::ReadHandlerInterface  *reader;
        IO::WriteHandlerInterface *writer;
        bool canRead;
        bool canWrite;
        bool clientCanRead;
        bool clientCanWrite;

        FileDescriptorInfo()
            : reader(nullptr),
              writer(nullptr),
              canRead(false),
              canWrite(false),
              clientCanRead(false),
              clientCanWrite(false) {
            // Nop.
        }

        FileDescriptorInfo(IO::ReadHandlerInterface *aReader, IO::WriteHandlerInterface *aWriter)
            : reader(aReader),
              writer(aWriter),
              canRead(false),
              canWrite(false),
              clientCanRead(false),
              clientCanWrite(false) {
            // Nop.
        }
        // Defaut copy/move, okay.
    };

    void UpdateHighestFileDescriptor(int fileDescriptor);
    bool ProcessClientRequests();
    bool Read(int fileDescriptor, bool *outEof);

    std::shared_ptr<SharedState> sharedState_;

    int                                         pipe_;
    int                                         highestFileDescriptor_;
    uint8_t                                     buffer_[bufferSize];
    std::unordered_map<int, FileDescriptorInfo> fileDescriptors_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_EVENTS_IO_WORKER_H_
