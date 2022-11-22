#ifndef K_GEOPOSITIONING_RTCMMESSAGE_H_
#define K_GEOPOSITIONING_RTCMMESSAGE_H_

#include <cstdint>
#include <vector>
#include <K/Core/SerializableInterface.h>

namespace K {
    namespace Core {
        class ItemWriteInterface;
    }
}

namespace K {
namespace GeoPositioning {

//! RTCM message.
class RtcmMessage : public virtual K::Core::SerializableInterface {
  public:
    RtcmMessage();
    RtcmMessage(const RtcmMessage &other)            = default;
    RtcmMessage &operator=(const RtcmMessage &other) = default;
    RtcmMessage(RtcmMessage &&other)                 = default;
    RtcmMessage &operator=(RtcmMessage &&other)      = default;

    //! Resets the message.
    void Reset();
    //! Appends the specified data to the message image.
    /*!
     *  The data gets copied.
     */
    void AppendToImage(const void *data, int dataSize);
    //! Tells the current size of the message image.
    int ImageSize() const;
    //! Tells the message type.
    /*!
     *  \return <c>false</c> in case the message type could not be deduced. The output parameter will then be
     *          undefined.
     */
    bool GetType(int *outType) const;
    //! Tells the payload size.
    int PayloadSize() const;
    //! Writes the RTCM message to the specified stream.
    void WriteTo(Core::ItemWriteInterface *stream) const;

    void Serialize(K::Core::BinaryWriterInterface *writer) const override;
    void Deserialize(K::Core::BinaryReaderInterface *reader) override;

  private:
    std::vector<uint8_t> image_;
};

}    // Namespace GeoPositioning.
}    // Namespace K.

#endif    // K_GEOPOSITIONING_RTCMMESSAGE_H_
