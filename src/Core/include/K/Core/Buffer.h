////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_BUFFER_H_
#define K_CORE_BUFFER_H_

#include <stdint.h>
#include <vector>
#include <K/Core/BlockingOutStreamInterface.h>
#include <K/Core/SeekableBlockingInStreamInterface.h>

namespace K {
namespace Core {

class NonBlockingInStreamInterface;

//! Binary buffer, allowing iterative composition and readout.
class Buffer : public virtual BlockingOutStreamInterface {
  public:
    //! Allows for iterative buffer readout.
	/*!
     *  A reader may only be used as long as the associated buffer is alive. Reading from a buffer via a reader may be
     *  interleaved with adding data via \ref Append().
	 */

    class Reader : public virtual SeekableBlockingInStreamInterface {
	  public:
        Reader()                               = default;
        Reader(const Reader &other)            = default;
        Reader &operator=(const Reader &other) = default;
        Reader(Reader &&other)                 = default;
        Reader &operator=(Reader &&other)      = default;
        ~Reader()                              = default;

        bool ErrorState() const override;
        Error StreamError() const override;
        int ReadBlocking(void *buffer, int bufferSize) override;
        void Seek(int64_t position) override;
        void RecoverAndSeek(int64_t position) override;
        int64_t StreamPosition() const override;

	  private:
		friend class Buffer;

		Reader(const Buffer *buffer);

        const Buffer                          *buffer_;
        int                                   cursor_;
        Error                                 error_;
	};

	//! Creates a buffer of the given initial size, but does not initialize its contents.
	Buffer(int initialSize);
    //! Creates an empty buffer.
    Buffer();
    Buffer(const Buffer &other)            = delete;
    Buffer &operator=(const Buffer &other) = delete;
    Buffer(Buffer &&other)                 = delete;
    Buffer &operator=(Buffer &&other)      = delete;
    ~Buffer();

    //! Grants access to the buffer's data. Note that the handed-out memory location gets invalidated by subsequent
    //! calls to \ref Append().
	void *Data();
    //! Grants read access to the buffer's data. Note that the handed-out memory location gets invalidated by subsequent
    //! calls to \ref Append().
    const void *Data() const;
	//! Tells the current data size, in bytes.
    int DataSize() const;
	//! Clears the buffer, which allows you to begin a new composition via \ref Append().
	/*!
	 *  All readers get invalidated.
	 *
	 *  This operation is fast and does not shrink the buffer's underlying capacity.
	 */
	void Clear();
    //! If \ref DataSize() is larger, the buffer gets shrinked to the specified size.
    /*!
     *  All readers get invalidated.
     *
     *  This operation is fast and does not shrink the buffer's underlying capacity.
     */
    void Shrink(int size);
    //! Appends a copy of the specified memory region to the buffer, potentially invalidating the memory location handed
    //! out earlier via \ref Data().
    /*!
     *  If <c>data</c> is <c>0</c>, the buffer will still be grown, but no data will actually get filled into the new
     *  space. Instead, the contents of that buffer region will be undefined.
     *
     *  If <c>dataSize</c> is <c>0</c>, nothing will happen.
     */
    void AppendFromMemory(const void *data, int dataSize);
    //! Appends data to the buffer by reading from the specified stream, potentially invalidating the memory location
    //! handed out earlier via \ref Data().
    /*!
     *  \param maxNumBytes
     *  Maximum number of bytes to read and append.
     *
     *  \return
     *  The number of bytes actually read and appended.
     */
    int AppendFromStream(NonBlockingInStreamInterface *stream, int maxNumBytes);
    //! Makes the current content (as reported by \ref DataSize()) cover the whole of the buffer's underlying capacity.
	/*!
	 *  The bytes potentially "appended" are undefined.
	 *
	 *  Fast operation.
	 */
	void RestoreToCurrentCapacity();
	//! Provides a reader allowing for iterative buffer readout.
	/*!
	 *  The reader may only be used as long as the associated buffer is alive.
	 */
	Reader GetReader() const;

    bool ErrorState() const override;
    Error StreamError() const override;
    void SetCloseResultAcceptor(const std::shared_ptr<Core::ResultAcceptor> &resultAcceptor) override;
    int WriteBlocking(const void *data, int dataSize) override;

  private:
    void Grow();

    std::vector<uint8_t>                  buffer_;
    int                                   bufferFill_;
    std::shared_ptr<Core::ResultAcceptor> closeResultAcceptor_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_BUFFER_H_
