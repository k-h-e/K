#include <K/IO/ListenSocket.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <K/Core/Log.h>
#include <K/IO/Socket.h>
#include <K/IO/TcpConnection.h>

using std::shared_ptr;
using std::make_shared;
using std::mutex;
using std::unique_lock;
using std::to_string;
using K::Core::Log;
using K::IO::ConnectionIO;

namespace K {
namespace IO {

ListenSocket::ListenSocket(int port, const shared_ptr<ConnectionIO> &connectionIO)
        : connectionIO_(connectionIO),
          socketDown_(false) {
    bool success = false;

    fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_ != -1) {
        struct sockaddr_in address = {};
        address.sin_family      = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port        = htons(static_cast<uint16_t>(port));
        if (!bind(fd_, (struct sockaddr *)&address, sizeof(address))) {
            if (!listen(fd_, 4)) {
                Log::Print(Log::Level::Debug, this, [=]{ return "socket " + to_string(fd_) + " listening on port "
                    + to_string(port); });
                success = true;
            }
        }
    }

    if (!success) {
        // Don't have to take lock, constructor.
        Close();
        Log::Print(Log::Level::Warning, this, [=]{ return "failed to create listen socket on port "
            + to_string(port); });
    }
}

ListenSocket::~ListenSocket() {
    // Don't have to take lock, destructor.
    Close();
}

shared_ptr<Socket> ListenSocket::Accept() {
    int fd = DoAccept();
    return (fd >= 0) ? make_shared<Socket>(fd) : nullptr;
}

shared_ptr<TcpConnection> ListenSocket::AcceptConnection() {
    int fd = DoAccept();
    return (fd >= 0) ? make_shared<TcpConnection>(fd, connectionIO_) : nullptr;
}

void ListenSocket::ShutDown() {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    if ((fd_ != -1) && !socketDown_) {
        shutdown(fd_, SHUT_RDWR);
        Log::Print(Log::Level::Debug, this, [=]{ return "listen socket " + to_string(fd_) + " shut down"; });
        socketDown_ = true;
    }
}    // ......................................................................................... critical section, end.

bool ListenSocket::ErrorState() {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    return (fd_ == -1) || socketDown_;
}    // ......................................................................................... critical section, end.

int ListenSocket::DoAccept() {
    int  fd;
    bool socketDown;
    {
        unique_lock<mutex> critical(lock_);    // Critical section .....................................................
        fd         = fd_;
        socketDown = socketDown_;
    }    // ..................................................................................... critical section, end.

    if ((fd != -1) && !socketDown) {
        struct sockaddr_in clientAddress;
        socklen_t clientAddressSize = sizeof(clientAddress);
        int connectionFD = accept(fd, (struct sockaddr *)&clientAddress, &clientAddressSize);
        if (connectionFD != -1) {
            Log::Print(Log::Level::Debug, this, [=]{ return "socket " + to_string(connectionFD)
                + " accepted connection"; });
            return connectionFD;
        }
    }

    return -1;
}

// Lock expected to be held.
void ListenSocket::Close() {
    if (fd_ != -1) {
        close(fd_);
        Log::Print(Log::Level::Debug, this, [=]{ return "listen socket " + to_string(fd_) + " closed"; });
        fd_         = -1;
        socketDown_ = true;
    }
}

}    // Namesapce IO.
}    // Namespace K.
