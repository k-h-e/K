/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_IDS_H_
#define K_CORE_IDS_H_

#include <unordered_set>

#include <K/Core/Interface.h>

namespace K {
namespace Core {

//! Manages integer-valued IDs.
class Ids : public virtual Interface {
  public:
    Ids();
    Ids(const Ids &other)            = delete;
    Ids &operator=(const Ids &other) = delete;
    Ids(Ids &&other)                 = delete;
    Ids &operator=(Ids &&other)      = delete;
    ~Ids()                           = default;

    //! Gets a free ID and returns it.
    /*!
     *  The ID is subsequently considered in-use until it is put back via <c>Put()</c>;
     *
     *  Valid IDs are always non-negative.
     */
    int Get();
    //! Puts back the specified in-use ID into the pool of free IDs.
    void Put(int id);

  private:
    int                     highestId_;
    std::unordered_set<int> unusedIds_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_IDS_H_
