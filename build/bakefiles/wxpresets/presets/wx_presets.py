
# We use 'COMPILER_PREFIX' option in places where bakefile doesn't like it, so
# we must register a substitution function for it that provides additional
# knowledge about the option (in this case that it does not contain dir
# separators and so utils.nativePaths() doesn't have to do anything with it):

from utils import addSubstituteCallback

def __noopSubst(name, func, caller):
    return '$(%s)' % name
addSubstituteCallback('COMPILER_PREFIX', __noopSubst)
