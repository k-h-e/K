#ifndef K_GEOPOSITIONING_NMEAMESSAGE_H_
#define K_GEOPOSITIONING_NMEAMESSAGE_H_

#include <string>
#include <vector>

namespace K {

namespace Core {
    class ItemWriteInterface;
}

namespace GeoPositioning {

//! NMEA message (also called "sentence").
class NmeaMessage {
  public:
    NmeaMessage(const std::string &type);
    NmeaMessage(const NmeaMessage &other)            = default;
    NmeaMessage &operator=(const NmeaMessage &other) = default;
    NmeaMessage(NmeaMessage &&other)                 = default;
    NmeaMessage &operator=(NmeaMessage &&other)      = default;

    //! Resets the sentence.
    void Reset(const std::string &type);
    //! Tells the message type.
    std::string Type() const;
    //! Tells whether or not the message is of the specified type.
    bool IsOfType(const std::string &type) const;
    //! Adds the specified field.
    void AddField(const std::string &field);
    //! Grants access to the specified field.
    const std::string &Field(int index) const;
    //! Tells the number of message fields.
    int FieldCount() const;
    //! Computes and returns the current checksum (hex in lower case).
    std::string CheckSum() const;
    //! Returns a string representation of the message.
    std::string ToString() const;
    //! Writes the NMEA message to the specified stream.
    void WriteTo(Core::ItemWriteInterface *stream) const;

  private:
    std::string              type_;
    std::vector<std::string> fields_;
};

}    // Namespace GeoPositioning.
}    // Namespace K.

#endif    // K_GEOPOSITIONING_NMEAMESSAGE_H_
