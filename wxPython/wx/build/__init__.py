# A Python package
"""
This package provides the config module, which is used by wxPython's
setup.py distutils script.  It was moved here so it would be installed
with the rest of wxPython and could therefore be used by the setup.py
for other projects that needed this same info and functionality (most
likely in order to be compatible with wxPython.)

See config.py and wxPython's setup.py for more details.

"""


# Exclude config from the epydoc docs because it will currently cause
# a lot of noise.  Once it has been refactored then add "config" to
# the list below.

__all__ = []


