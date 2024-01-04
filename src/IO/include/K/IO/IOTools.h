/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_IO_IOTOOLS_H_
#define K_IO_IOTOOLS_H_

namespace K {
    namespace Core {
        class Interface;
    }
}

namespace K {
//! I/O.
namespace IO {

//! I/O tool functions.
class IOTools {
  public:
    static const char *whiteSpace;

    //! Closes the specified UNIX file descriptor.
    /*!
     *  \return
     *  <c>false</c> in case of failure.
     */
    static bool CloseFileDescriptor(int fd, K::Core::Interface *loggingObject);
};

}    // Namespace IO.
}    // namespace K.

#endif    // K_IO_IOTOOLS_H_
