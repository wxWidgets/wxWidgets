#
# Helper functions for wxWindows bakefiles
#
# $Id$
#


def mk_wxid(id):
    """Creates wxWindows library identifier from bakefile target ID that
       follows this convention: DLLs end with 'dll', static libraries
       end with 'lib'. If withPrefix=1, then _wxid is returned instead
       of wxid."""
    if id.endswith('dll') or id.endswith('lib'):
        wxid = id[:-3]
    else:
        wxid = id
    return wxid


# List of library names/ids for categories with different names:
LIBS_BASE = ['base']
LIBS_GUI  = ['core', 'html']

def mkLibName(wxid):
    """Returns string that can be used as library name, including name
       suffixes, prefixes, version tags etc. This must be kept in sync
       with variables defined in common.bkl!"""
    if wxid == 'mono':
        return '$(WXNAMEPREFIXGUI)$(WXNAMESUFFIX)$(WXVERSIONTAG)'
    if wxid in LIBS_BASE:
        return '$(WXNAMEPREFIX)$(WXNAMESUFFIX)_%s$(WXVERSIONTAG)' % wxid
    return '$(WXNAMEPREFIXGUI)$(WXNAMESUFFIX)_%s$(WXVERSIONTAG)' % wxid

def mkDllName(wxid):
    """Returns string that can be used as DLL name, including name
       suffixes, prefixes, version tags etc. This must be kept in sync
       with variables defined in common.bkl!"""
    if wxid == 'mono':
        return '$(WXNAMEPREFIXGUI)$(WXNAMESUFFIX)$(WXCOMPILER)$(WXVERSIONTAG)'
    if wxid in LIBS_BASE:
        return '$(WXNAMEPREFIX)$(WXNAMESUFFIX)_%s$(WXCOMPILER)$(WXVERSIONTAG)' % wxid
    return '$(WXNAMEPREFIXGUI)$(WXNAMESUFFIX)_%s$(WXCOMPILER)$(WXVERSIONTAG)' % wxid


def libToLink(wxlibname):
    """Returns string to pass to <sys-lib> when linking against 'wxlibname'.
       libToLink('foo') returns '$(WXLIB_FOO)' which must be defined in
       common.bkl as either nothing (in monolithic build) or mkLibName('foo')
       (otherwise)."""
    return '$(WXLIB_%s)' % wxlibname.upper()


wxVersion = None
VERSION_FILE = '../../include/wx/version.h'

def getVersion():
    """Returns wxWindows version as a tuple: (major,minor,release)."""
    global wxVersion
    if wxVersion == None:
        f = open(VERSION_FILE, 'rt')
        lines = f.readlines()
        f.close()
        major = minor = release = None
        for l in lines:
            if not l.startswith('#define'): continue
            splitted = l.strip().split()
            if splitted[0] != '#define': continue
            if len(splitted) < 3: continue
            name = splitted[1]
            value = splitted[2]
            if value == None: continue
            if name == 'wxMAJOR_VERSION': major = int(value)
            if name == 'wxMINOR_VERSION': minor = int(value)
            if name == 'wxRELEASE_NUMBER': release = int(value)
            if major != None and minor != None and release != None:
                break
        wxVersion = (major, minor, release)
    return wxVersion

def getVersionMajor():
    return getVersion()[0]
def getVersionMinor():
    return getVersion()[1]
def getVersionRelease():
    return getVersion()[2]
