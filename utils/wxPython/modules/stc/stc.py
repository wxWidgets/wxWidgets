
# The name "stc.cpp" was cuasing the debugger to get confused with the
# same module name in the stc library, so I changed the name of this
# one to stc_.cpp and this little stub to make the "stc" Python module
# name still usesable for everything that matters.

from stc_ import *
