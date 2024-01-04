/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_GEOPOSITIONING_DEPRECATED_NMEADEVICE_H_
#define K_GEOPOSITIONING_DEPRECATED_NMEADEVICE_H_

#include <memory>
#include <K/Core/ErrorStateInterface.h>
#include <K/GeoPositioning/NmeaParser.h>

namespace K {
    namespace Core {
        namespace Deprecated {
            class ConnectionStreamInterface;
        }
    }
    namespace GeoPositioning {
        class NmeaMessage;
    }
}

namespace K {
namespace GeoPositioning {
namespace Deprecated {

//! Provides access to an NMEA (protocol defined by the National Marine Electronics Association) device.
class NmeaDevice : public virtual K::Core::ErrorStateInterface {
  public:
    NmeaDevice(const std::shared_ptr<K::Core::Deprecated::ConnectionStreamInterface> &connection);
    ~NmeaDevice();
    //! Registers the specified sentence receiver.
    /*!
     *  The receiver methods will we called from an arbitrary thread and must not call back into the NMEA device.
     */
    bool Register(const std::shared_ptr<NmeaMessageHandlerInterface> &handler);
    //! Unregisters the specified sentence receiver if it was registered.
    /*!
     *  When the method returns, it will be guaranteed that the previous receiver will not be called again.
     */
    void Unregister(const std::shared_ptr<NmeaMessageHandlerInterface> &handler);
    //! Writes the specified NMEA message.
    /*!
     * \return
     *  <c>false</c> in case of failure.
     */
    bool Write(const NmeaMessage &message);
    bool ErrorState() const override;

  private:
    std::shared_ptr<NmeaMessageHandlerInterface> handler_;    // Given to connection thread.
    std::shared_ptr<NmeaParser>                  parser_;     // Given to connection thread.

    std::shared_ptr<K::Core::Deprecated::ConnectionStreamInterface> connection_;
    mutable bool                                                    error_;
};

}    // Namespace Deprecated.
}    // Namespace GeoPositioning.
}    // Namespace K.

#endif    // K_GEOPOSITIONING_DEPRECATED_NMEADEVICE_H_
