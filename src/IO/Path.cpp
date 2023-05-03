#include <K/IO/Path.h>

#include <algorithm>
#include <K/Core/Log.h>
#include <K/Core/StringTools.h>

using std::find;
using std::nullopt;
using std::optional;
using std::string;
using std::vector;
using K::Core::Log;
using K::Core::StringTools;

namespace K {
namespace IO {

Path::Path(const string &path)
        : error_ { false } {
    vector<string> components { StringTools::Tokenize(path, "/", false) };
    if ((components.size() >= 2u) && components[0].empty()) {
        root_ = "";
        for (unsigned int i = 1; i < components.size(); ++i) {
            if (!components[i].empty()) {
                components_.push_back(components[i]);
            }
        }
    } else {
        for (string &component : components) {
            if (!component.empty()) {
                components_.push_back(component);
            }
        }
    }
}

Path::Path()
        : error_ { false } {
    Clear();
}

Path Path::operator+(const std::string &component) const {
    Path result { *this };
    if (!error_) {
        if (IsComponentText(component)) {
            result.components_.push_back(component);
        } else {
            result.error_ = true;
        }
    }

    return result;
}

Path Path::Parent() const {
    Path result { *this };
    if (!error_ && !components_.empty()) {
        result.components_.pop_back();
    } else {
        result.Clear();
        result.error_ = true;
    }

    return result;
}

optional<string> Path::LastComponent() const {
    if (!error_ && !components_.empty()) {
        return components_.back();
    } else {
        return nullopt;
    }
}

void Path::SetLastComponent(const string &component) {
    if (!error_) {
        if (IsComponentText(component)) {
            if (components_.empty()) {
                components_.push_back(component);
            } else {
                components_.back() = component;
            }
        } else {
            error_ = true;
        }
    }
}

void Path::AppendToLastComponent(const std::string &text) {
    if (!error_) {
        if (IsComponentText(text) && !components_.empty()) {
            components_.back() += text;
        } else {
            error_ = true;
        }
    }
}

string Path::ToOsPath() const {
    if (error_) {
        Log::Print(Log::Level::Error, this, [&]{ return "bad path about to get passed to OS!"; });
        return "";
    } else {
        if (root_) {
            return *root_ + "/" + StringTools::Concatenate(components_, "/");
        } else {
            return components_.empty() ? "." : StringTools::Concatenate(components_, "/");
        }
    }
}

string Path::ToString() const {
    if (error_) {
        return "<bad>";
    } else {
        if (root_) {
            return *root_ + "/" + StringTools::Concatenate(components_, "/");
        } else {
            return components_.empty() ? "." : StringTools::Concatenate(components_, "/");
        }
    }
}

string Path::ToShortString() const {
    string result = ToString();
    
    const unsigned int maxLength = 40u;
    if (result.length() > maxLength) {
        result = result.substr(result.length() - maxLength, maxLength);
        result[0] = '.';
        result[1] = '.';
        result[2] = '.';
    }
    
    return result;
}

void Path::Clear() {
    components_.clear();
    root_.reset();
    error_ = false;
}

bool Path::ErrorState() const {
    return error_;
}

bool Path::IsComponentText(const string &component) const {
    return (find(component.begin(), component.end(), '/') == component.end());
}

}    // Namespace IO.
}    // Namespace K.
