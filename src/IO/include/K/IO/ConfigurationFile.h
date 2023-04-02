#ifndef K_IO_CONFIGURATIONFILE_H_
#define K_IO_CONFIGURATIONFILE_H_

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <K/Core/ValidCharacters.h>
#include <K/Core/ErrorStateInterface.h>

namespace K {
namespace IO {

class Path;

//! In-RAM representation of a configuration file.
class ConfigurationFile : public virtual Core::ErrorStateInterface {
  public:
    ConfigurationFile();
    ConfigurationFile(const ConfigurationFile &other)            = delete;
    ConfigurationFile &operator=(const ConfigurationFile &other) = delete;
    ConfigurationFile(ConfigurationFile &&other)                 = delete;
    ConfigurationFile &operator=(ConfigurationFile &&other)      = delete;

    bool ErrorState() const override;

    //! Tells whether the specified key is present.
    bool KeyPresent(const std::string &section, const std::string &key) const;
    //! Sets the value for the specified key.
    void SetValue(const std::string &section, const std::string &key, const std::string &value);
    //! Sets the value for the specified key.
    void SetValue(const std::string &section, const std::string &key, float value);
    //! Sets the value for the specified key.
    void SetValue(const std::string &section, const std::string &key, double value);
    //! Sets the value for the specified key.
    void SetValue(const std::string &section, const std::string &key, bool value);
    //! Gets the value of the specified key.
    void GetValue(const std::string &section, const std::string &key, std::string *outValue);
    //! Gets the value of the specified key.
    void GetValue(const std::string &section, const std::string &key, int *outValue);
    //! Gets the value of the specified key.
    void GetValue(const std::string &section, const std::string &key, float *outValue);
    //! Gets the value of the specified key.
    void GetValue(const std::string &section, const std::string &key, double *outValue);
    //! Gets the value of the specified key.
    void GetValue(const std::string &section, const std::string &key, bool *outValue);
    //! Saves the configuration file to the file system.
    void Save(const Path &fileName);
    //! Loads the configuration file from the file system.
    /*!
     *  Can recall the object from error state.
     */
    void Load(const Path &fileName);

  private:
    bool IsSectionName(const std::string &text);
    bool IsKey(const std::string &text);
    bool IsValue(const std::string &text);

    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> sections_;
    Core::ValidCharacters                                                         validCharacters_;
    bool                                                                          errorState_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_CONFIGURATIONFILE_H_
