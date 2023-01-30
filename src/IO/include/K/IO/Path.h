#ifndef K_IO_PATH_H_
#define K_IO_PATH_H_

#include <optional>
#include <string>
#include <vector>

#include <K/Core/ErrorStateInterface.h>

namespace K {
namespace IO {

//! File system path.
class Path : public virtual Core::ErrorStateInterface {
  public:
    explicit Path(const std::string &path);
    //! Constructs an empty relative path.
    Path();
    Path(const Path &other)            = default;
    Path &operator=(const Path &other) = default;
    Path(Path &&other)                 = default;
    Path &operator=( Path &&other)     = default;
    ~Path()                            = default;

    //! Appends the specified path component.
    Path operator+(const std::string &component) const;

    //! Returns the parent path.
    Path Parent() const;
    //! Returns the last non-root path component, or <c>nullopt</c> in case there is none.
    std::optional<std::string> LastComponent() const;
    //! Sets the last path component as specified.
    void SetLastComponent(const std::string &component);
    //! Returns the path as operating-system-specific path string.
    std::string ToOsPath() const;
    //! Returns a string representation for the path.
    std::string ToString() const;
    //! Makes the path an empty relative path.
    /*!
     *  Recovers the path from error state if necessary.
     */
    void Clear();

    bool ErrorState() const override;

  private:
    bool IsComponent(const std::string &component) const;

    std::vector<std::string>   components_;
    std::optional<std::string> root_;          // Present <=> path is absolute.
    bool                       error_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_PATH_H_
