/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_PROGRESSTRACKERCORE_H_
#define K_CORE_PROGRESSTRACKERCORE_H_

#include <memory>

#include <K/Core/Interface.h>


namespace K {
    namespace Core {
        class CompoundProgressTrackerInterface;
    }
}

namespace K {
namespace Core {

//! Base class to progress trackers.
class ProgressTrackerCore : public virtual Interface {
  public:
    //! Interface to entities handling tracked progress.
    class HandlerInterface : public virtual Interface {
      public:
        virtual void OnProgress(int percent) = 0;
    };

    ProgressTrackerCore(const std::shared_ptr<HandlerInterface> &handler,
                        const std::shared_ptr<CompoundProgressTrackerInterface> &superActivityProgressTracker);
    ProgressTrackerCore()                                            = delete;
    ProgressTrackerCore(const ProgressTrackerCore &other)            = delete;
    ProgressTrackerCore &operator=(const ProgressTrackerCore &other) = delete;
    ProgressTrackerCore(ProgressTrackerCore &&other)                 = delete;
    ProgressTrackerCore &operator=(ProgressTrackerCore &&other)      = delete;
    ~ProgressTrackerCore();

  protected:
    //! To be called by subclasses whenever they have recomputed the current progress percent.
    /*!
     *  Clamps percent value to <c>[0, 100]</c>.
     */
    void OnPercentComputed(int percent);

  private:
    std::shared_ptr<HandlerInterface>                 handler_;
    std::shared_ptr<CompoundProgressTrackerInterface> superActivityProgressTracker_;
    int                                               subActivityId_;    // Valid <=> super activity tracker present.
    int                                               percent_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_PROGRESSTRACKERCORE_H_
