import sys

"""
The wxtest module is used for common code across the wx Test Suite.

Its major use is to provide a standard means of determining the current
platform, for platform-specific testing.  There are currently three choices
of platform: Windows, Linux, and Mac.

The two types of use cases are as follows:
    
    # Method 1:
    if wxtest.CURRENT_PLATFORM == wxtest.MAC:
        # mac-specific test
    if wxtest.CURRENT_PLATFORM != wxtest.WINDOWS:
        # test not to be run on windows
    
    # Method 2 (for convenience)
    if wxtest.PlatformIsMac():
        # mac-specific test
    if wxtest.PlatformIsNotWindows():
        # test not to be run on windows
"""

# A poor attempt at enums
WINDOWS = 100
LINUX   = 200
MAC     = 300

# Determine current platform
CURRENT_PLATFORM = None
if sys.platform.find('win32') != -1:
    CURRENT_PLATFORM = WINDOWS
elif sys.platform.find('linux') != -1:
    CURRENT_PLATFORM = LINUX
elif sys.platform.find('mac') != -1:
    CURRENT_PLATFORM = MAc
else:
    raise EnvironmentError("Unknown platform!")


# Convenience methods

def PlatformIsWindows():
    return CURRENT_PLATFORM == WINDOWS

def PlatformIsLinux():
    return CURRENT_PLATFORM == LINUX

def PlatformIsMac():
    return CURRENT_PLATFORM == MAC

def PlatformIsNotWindows():
    return CURRENT_PLATFORM != WINDOWS

def PlatformIsNotLinux():
    return CURRENT_PLATFORM != LINUX

def PlatformIsNotMac():
    return CURRENT_PLATFORM != MAC
