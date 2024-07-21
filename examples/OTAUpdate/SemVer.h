#pragma once

#include <Arduino.h>

class Version {
  public:
    Version(const String& versionStr);
    String toString() const;

    bool operator > (const Version& other) const;
    bool operator < (const Version& other) const;
    bool operator == (const Version& other) const;
    bool operator != (const Version& other) const;

  protected:
    int major;
    int minor;
    int patch;
};

Version::Version(const String& versionStr) {
    int     firstDot  = versionStr.indexOf('.');
    int     secondDot = versionStr.lastIndexOf('.');
    major           = versionStr.substring(0, firstDot).toInt();
    minor           = versionStr.substring(firstDot + 1, secondDot).toInt();
    patch           = versionStr.substring(secondDot + 1).toInt();
}

String Version::toString() const {
    return String(major) + "." + String(minor) + "." + String(patch);
}

bool Version::operator>(const Version& other) const {
    if (major > other.major) return true;
    if (minor > other.minor) return true;
    if (patch > other.patch) return true;
    return false;
}

bool Version::operator<(const Version& other) const {
    if (major < other.major) return true;
    if (minor < other.minor) return true;
    if (patch < other.patch) return true;
    return false;
}

bool Version::operator==(const Version& other) const {
    if (major != other.major) return false;
    if (minor != other.minor) return false;
    if (patch != other.patch) return false;
    return true;
}

bool Version::operator!=(const Version& other) const {
    return !operator==(other);
}