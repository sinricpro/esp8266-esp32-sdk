#include <Arduino.h>

struct Version {
    int major;
    int minor;
    int patch;

    String toString() const {
        return String(major) + "." + String(minor) + "." + String(patch);
    }
};

Version parseVersion(const String& versionStr) {
    Version v;
    int firstDot = versionStr.indexOf('.');
    int secondDot = versionStr.lastIndexOf('.');    
    v.major = versionStr.substring(0, firstDot).toInt();
    v.minor = versionStr.substring(firstDot + 1, secondDot).toInt();
    v.patch = versionStr.substring(secondDot + 1).toInt();    
    return v;
}

bool isNewerVersion(const Version& currentVersion, const Version& newVersion) {
    if (newVersion.major > currentVersion.major) return true;
    if (newVersion.major < currentVersion.major) return false;
    
    if (newVersion.minor > currentVersion.minor) return true;
    if (newVersion.minor < currentVersion.minor) return false;
    
    return newVersion.patch > currentVersion.patch;
}