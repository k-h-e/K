/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_PROGRESSTRACKER_H_
#define K_CORE_PROGRESSTRACKER_H_

#include <memory>
#include <string>

#include <K/Core/Interface.h>

namespace K {
namespace Core {

//! Keeps track of progress in percent.
class ProgressTracker : public virtual Interface {
  public:
    //! Interface to progress tracker handlers.
    class HandlerInterface : public virtual Interface {
      public:
        //! Informs the handler that the respective activity's progress has been updated. 
        virtual void OnProgressUpdate(const std::string &activity, int progressPercent) = 0;
    };
    //! Interface to parent progress trackers.
    class ParentInterface : public virtual Interface {
      public:
        //! Informs the parent that the respective activity's progress has been updated. 
        virtual void OnProgressUpdate(int id, int progressPercent) = 0;
    };

    ProgressTracker(const std::shared_ptr<HandlerInterface> &handler, const std::string &activity);
    ProgressTracker()                                                    = delete;
    ProgressTracker(const ProgressTracker &other)                        = delete;
    ProgressTracker &operator=(const ProgressTracker &other)             = delete;
    ProgressTracker(ProgressTracker &&other)                             = delete;
    ProgressTracker &operator=(ProgressTracker &&other)                  = delete;
    ~ProgressTracker();

    //! Sets a parent for the tracker.
    /*!
     *  Pass <c>nullptr</c> to unset a previously set parent. The passed ID is then ignored.
     */
    void SetParent(ParentInterface *parent, int idForParent);

  protected:
    void SetProgress(int progressPercent);
    void NotifyParent();
    void NotifyHandler();

  private:
    std::shared_ptr<HandlerInterface> handler_;
    std::string                       activity_;
    ParentInterface                   *parent_;
    int                               idForParent_;
    int                               progress_;
    int                               oldProgress_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_PROGRESSTRACKER_H_
