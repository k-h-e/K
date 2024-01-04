/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_TIMERS_WORKER_H_
#define K_CORE_TIMERS_WORKER_H_

#include <K/Core/ActionInterface.h>
#include <K/Core/Timers.h>

namespace K {
namespace Core {

//! Worker for the timers implementation.
class Timers::Worker : public virtual ActionInterface {
  public:
    Worker(const std::shared_ptr<SharedState> &shared);
    Worker(const Worker &other)            = delete;
    Worker &operator=(const Worker &other) = delete;
    Worker(Worker &&other)                 = delete;
    Worker &operator=(Worker &&other)      = delete;

    void ExecuteAction() override;

  private:
    std::shared_ptr<SharedState> shared_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_TIMERS_WORKER_H_
