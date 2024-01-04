/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_INTERFACE_H_
#define K_CORE_INTERFACE_H_

//! Kai's C++ cross-platform assets.
namespace K {
//! Core classes.
namespace Core {

//! Base class to all hierarchy classes, basically providing the virtual destructor to allow for polymorphic object
//! deletion.
class Interface {
  public:
    virtual ~Interface();
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_INTERFACE_H_
