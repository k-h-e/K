#ifndef K_CORE_BUFFER_H_
#define K_CORE_BUFFER_H_

#include <stdint.h>
#include <vector>
#include <K/Core/ItemReadInterface.h>
#include <K/Core/ItemWriteInterface.h>

namespace K {
namespace Core {

//! Binary buffer, allowing iterative composition and readout.
class Buffer : public virtual ItemWriteInterface {
  public:
    //! Allows for iterative buffer readout.
	/*!
     *  A reader may only be used as long as the associated buffer is alive. Reading from a buffer via a reader may be
     *  interleaved with adding data via \ref Append().
	 */

    class Reader : public virtual ItemReadInterface {
	  public:
        Reader(const Reader &other)            = default;
        Reader &operator=(const Reader &other) = default;
        Reader(Reader &&other)                 = default;
        Reader &operator=(Reader &&other)      = default;
        ~Reader();

        //! Reads at most <c>targetBufferSize</c> bytes from the buffer and transfers them to the specified memory
        //! location.
		/*!
         *  \return
         *  The number of bytes actually transferred. A short read (including the return value of <c>0</c>) means there
         *  are currently no more bytes in the buffer to read.
		 */
	    int Read(void *targetBuffer, int targetBufferSize);
        //! Convenience method reading an entire block of specified size.
        /*!
         *  \return
         *  <c>false</c> in case the block could not be read entirely. This means there are currently no more bytes in
         *  the buffer to read. The contents of the target buffer are undefined.
         */
        bool ReadBlock(void *targetBuffer, int targetBufferSize);

        void ReadItem(void *outItem, int itemSize) override;
        bool Good() const override;
        bool ErrorState() override;
        bool Eof() override;
        void ClearEof() override;
        void SetFinalResultAcceptor(const std::shared_ptr<Core::Result> &resultAcceptor) override;

	  private:
		friend class Buffer;

		Reader(const Buffer *buffer);

        const Buffer                  *buffer_;
        int                           cursor_;
        bool                          eof_;
        std::shared_ptr<Core::Result> finalResultAcceptor_;
	};

    //! Creates an empty buffer.
    Buffer();
	//! Creates a buffer of the given initial size, but does not initialize its contents.
	Buffer(int initialSize);
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
	void Append(const void *data, int dataSize);
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

    void WriteItem(const void *item, int itemSize) override;
    bool Good() const override;
    bool ErrorState() override;
    void SetFinalResultAcceptor(const std::shared_ptr<Core::Result> &resultAcceptor) override;

  private:
    std::vector<uint8_t>          buffer_;
    int                           bufferFill_;
    std::shared_ptr<Core::Result> finalResultAcceptor_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_BUFFER_H_
