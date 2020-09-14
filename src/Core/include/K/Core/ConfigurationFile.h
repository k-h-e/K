#ifndef K_CORE_CONFIGURATIONFILE_H_
#define K_CORE_CONFIGURATIONFILE_H_

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <K/Core/Interface.h>

namespace K {
namespace Core {

//! In-RAM representation of a configuration file.
/*!
 *  \ingroup Core
 */
class ConfigurationFile : public virtual Interface {
  public:
    ConfigurationFile();
    ConfigurationFile(const ConfigurationFile &other)            = delete;
    ConfigurationFile &operator=(const ConfigurationFile &other) = delete;
    ConfigurationFile(ConfigurationFile &&other)                 = delete;
    ConfigurationFile &operator=(ConfigurationFile &&other)      = delete;

    //! Sets the value for the specified key.
    /*!
     * \return <c>false</c> in case of failure. The object then remains unchanged.
     */
    bool SetValue(const std::string &section, const std::string &key, const std::string &value);
    //! Sets the value for the specified key.
    /*!
     * \return <c>false</c> in case of failure. The object then remains unchanged.
     */
    bool SetValue(const std::string &section, const std::string &key, float value);
    //! Gets the value of the specified key.
    /*!
     * \return <c>false</c> in case of failure. The output parameter will then be undefined.
     */
    bool GetValue(const std::string &section, const std::string &key, std::string *outValue);
    //! Gets the value of the specified key.
    /*!
     * \return <c>false</c> in case of failure. The output parameter will then be undefined.
     */
    bool GetValue(const std::string &section, const std::string &key, float *outValue);
    //! Saves the configuration file to the file system.
    /*!
     * \return <c>false</c> in case of failure. The output file is then in undefined state.
     */
    bool Save(const std::string &fileName);
    //! Loads the configuration file from the file system.
    /*!
     * \return <c>false</c> in case of failure. The object is then cleared.
     */
    bool Load(const std::string &fileName);

  private:
    bool isSectionName(const std::string &text);
    bool isKey(const std::string &text);
    bool isValue(const std::string &text);

    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> sections_;
    std::unordered_set<char>                                                      validCharacters_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_CONFIGURATIONFILE_H_
