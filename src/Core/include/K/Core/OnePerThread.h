#ifndef K_CORE_ONEPERTHREAD_H_
#define K_CORE_ONEPERTHREAD_H_

#include <memory>
#include <unordered_map>
#include <thread>
#include <K/Core/Interface.h>
#include <K/Core/Log.h>

namespace K {
namespace Core {

//! Manages an object resource where each thread must have its own copy.
/*!
 *  \ingroup Core
 *
 *  The class is thread-safe, i.e. all its public methods.
 */
template<class T>
class OnePerThread : public virtual K::Core::Interface {
  public:
    //! The lock must outlive the <c>OnePerThread</c> object.
    OnePerThread(const std::string &name, std::mutex &lock);
    OnePerThread(const OnePerThread &other)            = delete;
    OnePerThread &operator=(const OnePerThread &other) = delete;
    OnePerThread(OnePerThread &&other)                 = delete;
    OnePerThread &operator=(OnePerThread &&other)      = delete;
    //! Clears all resource copies and registers the specified one for the current thread.
    void Set(const std::shared_ptr<T> &object);
    //! Returns the resource copy for the current thread.
    /*!
     *  If none is present, it is automatically cloned from an existing copy.
     *
     *  \return
     *  <c>nullptr</c> is no copy is present for the resource.
     */
    std::shared_ptr<T> Get();

  private:
    std::mutex                                              &lock_;
    std::string                                             name_;
    std::unordered_map<std::thread::id, std::shared_ptr<T>> instances_;
};

template<class T>
OnePerThread<T>::OnePerThread(const std::string &name, std::mutex &lock)
        : lock_(lock),
          name_(name) {
    // Nop.
}

template<class T>
void OnePerThread<T>::Set(const std::shared_ptr<T> &object) {
    std::unique_lock<std::mutex> critical(lock_);    // Critical section................................................
    std::thread::id id = std::this_thread::get_id();
    instances_.clear();
    instances_[id] = object;
    K::Core::Log::Print(K::Core::Log::Level::Debug, this, [&]{
        return "resource " + name_ + " updated, num=" + std::to_string(instances_.size()); });
}    // ......................................................................................... critical section, end.

template<class T>
std::shared_ptr<T> OnePerThread<T>::Get() {
    std::unique_lock<std::mutex> critical(lock_);    // Critical section................................................
    std::thread::id id = std::this_thread::get_id();
    if (auto iter = instances_.find(id); iter != instances_.end()) {
        return iter->second;
    }
    else if (auto iter = instances_.begin(); iter != instances_.end()) {
        instances_[id] = std::make_shared<T>(*(iter->second));
        K::Core::Log::Print(K::Core::Log::Level::Debug, this, [&]{
            return "resource " + name_ + " cloned, num=" + std::to_string(instances_.size()); });
        return instances_[id];
    }
    else {
        return nullptr;
    }
}    // ......................................................................................... critical section, end.

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_ONEPERTHREAD_H_

