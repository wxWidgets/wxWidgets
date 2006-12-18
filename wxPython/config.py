#----------------------------------------------------------------------
# Name:        wx.build.config
# Purpose:     Most of the contents of this module used to be located 
#              in wxPython's setup.py script.  It was moved here so
#              it would be installed with the rest of wxPython and
#              could therefore be used by the setup.py for other
#              projects that needed this same info and functionality
#              (most likely in order to be compatible with wxPython.)
#
#              This split from setup.py is still fairly rough, and
#              some things may still get shuffled back and forth,
#              refactored, etc.  Please send me any comments and
#              suggestions about this.
#
# Author:      Robin Dunn
#
# Created:     23-March-2004
# RCS-ID:      $Id$
# Copyright:   (c) 2004 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------

import sys, os, glob, fnmatch, tempfile
from distutils.core      import setup, Extension
from distutils.file_util import copy_file
from distutils.dir_util  import mkpath
from distutils.dep_util  import newer
from distutils.spawn     import spawn

import distutils.command.install
import distutils.command.install_data
import distutils.command.install_headers
import distutils.command.clean

#----------------------------------------------------------------------
# flags and values that affect this script
#----------------------------------------------------------------------

VER_MAJOR        = 2      # The first three must match wxWidgets
VER_MINOR        = 8
VER_RELEASE      = 0
VER_SUBREL       = 2      # wxPython release num for x.y.z release of wxWidgets
VER_FLAGS        = ""     # release flags, such as prerelease or RC num, etc.

DESCRIPTION      = "Cross platform GUI toolkit for Python"
AUTHOR           = "Robin Dunn"
AUTHOR_EMAIL     = "Robin Dunn <robin@alldunn.com>"
URL              = "http://wxPython.org/"
DOWNLOAD_URL     = "http://wxPython.org/download.php"
LICENSE          = "wxWidgets Library License (LGPL derivative)"
PLATFORMS        = "WIN32,OSX,POSIX"
KEYWORDS         = "GUI,wx,wxWindows,wxWidgets,cross-platform"

LONG_DESCRIPTION = """\
wxPython is a GUI toolkit for Python that is a wrapper around the
wxWidgets C++ GUI library.  wxPython provides a large variety of
window types and controls, all implemented with a native look and
feel (by using the native widgets) on the platforms upon which it is
supported.
"""

CLASSIFIERS      = """\
Development Status :: 6 - Mature
Environment :: MacOS X :: Carbon
Environment :: Win32 (MS Windows)
Environment :: X11 Applications :: GTK
Intended Audience :: Developers
License :: OSI Approved
Operating System :: MacOS :: MacOS X
Operating System :: Microsoft :: Windows :: Windows 95/98/2000
Operating System :: POSIX
Programming Language :: Python
Topic :: Software Development :: User Interfaces
"""

## License :: OSI Approved :: wxWidgets Library Licence


# Config values below this point can be reset on the setup.py command line.

BUILD_GLCANVAS = 1 # If true, build the contrib/glcanvas extension module
BUILD_OGL = 0      # If true, build the contrib/ogl extension module
BUILD_STC = 1      # If true, build the contrib/stc extension module
BUILD_GIZMOS = 1   # Build a module for the gizmos contrib library
BUILD_DLLWIDGET = 0# Build a module that enables unknown wx widgets
                   # to be loaded from a DLL and to be used from Python.

                   # Internet Explorer wrapper (experimental)
BUILD_ACTIVEX = (os.name == 'nt')  # new version of IEWIN and more


CORE_ONLY = 0      # if true, don't build any of the above

PREP_ONLY = 0      # Only run the prepatory steps, not the actual build.

USE_SWIG = 0       # Should we actually execute SWIG, or just use the
                   # files already in the distribution?

SWIG = "swig"      # The swig executable to use.

BUILD_RENAMERS = 0 # Should we build the renamer modules too?

FULL_DOCS = 0      # Some docstrings are split into a basic docstring and a
                   # details string.  Setting this flag to 1 will
                   # cause the two strings to be combined and output
                   # as the full docstring.

UNICODE = 0        # This will pass the 'wxUSE_UNICODE' flag to SWIG and
                   # will ensure that the right headers are found and the
                   # right libs are linked.

UNDEF_NDEBUG = 1   # Python 2.2 on Unix/Linux by default defines NDEBUG,
                   # and distutils will pick this up and use it on the
                   # compile command-line for the extensions.  This could
                   # conflict with how wxWidgets was built.  If NDEBUG is
                   # set then wxWidgets' __WXDEBUG__ setting will be turned
                   # off.  If wxWidgets was actually built with it turned
                   # on then you end up with mismatched class structures,
                   # and wxPython will crash.

NO_SCRIPTS = 0     # Don't install the tool scripts
NO_HEADERS = 0     # Don't install the wxPython *.h and *.i files

INSTALL_MULTIVERSION = 1 # Install the packages such that multiple versions
                   # can co-exist.  When turned on the wx and wxPython
                   # pacakges will be installed in a versioned subdir
                   # of site-packages, and a *.pth file will be
                   # created that adds that dir to the sys.path.  In
                   # addition, a wxselect.py module will be installed
                   # to site-pacakges that will allow applications to
                   # choose a specific version if more than one is
                   # installed.
                   
FLAVOUR = ""       # Optional flavour string to be appended to VERSION
                   # in MULTIVERSION installs

EP_ADD_OPTS = 1    # When doing MULTIVERSION installs the wx port and
                   # ansi/unicode settings can optionally be added to the
                   # subdir path used in site-packages

EP_FULL_VER = 0    # When doing MULTIVERSION installs the default is to
                   # put only 2 or 3 (depending on stable/unstable) of
                   # the version compnonents into the "extra path"
                   # subdir of site-packages.  Setting this option to
                   # 1 will cause the full 4 components of the version
                   # number to be used instead.
                                      
WX_CONFIG = None   # Usually you shouldn't need to touch this, but you can set
                   # it to pass an alternate version of wx-config or alternate
                   # flags, eg. as required by the .deb in-tree build.  By
                   # default a wx-config command will be assembled based on
                   # version, port, etc. and it will be looked for on the
                   # default $PATH.

SYS_WX_CONFIG = None # When installing an in tree build, setup.py uses wx-config
                     # for two different purposes.  First, to determine the prefix
                     # where files will be installed, and secondly, to initialise
                     # build_options.py with the correct options for it.
                     # WX_CONFIG is used for the first task.  SYS_WX_CONFIG may
                     # be set independently, to the value that should appear in
                     # build_options.py, if it is different to that.  The default
                     # is to use the value of WX_CONFIG.

WXPORT = 'gtk2'    # On Linux/Unix there are several ports of wxWidgets available.
                   # Setting this value lets you select which will be used for
                   # the wxPython build.  Possibilites are 'gtk', 'gtk2' and
                   # 'x11'.  Curently only gtk and gtk2 works.

BUILD_BASE = "build" # Directory to use for temporary build files.
                     # This name will be appended to if the WXPORT or
                     # the UNICODE flags are set to non-standard
                     # values.  See below.


CONTRIBS_INC = ""  # A dir to add as an -I flag when compiling the contribs


# Some MSW build settings

MONOLITHIC = 0     # The core wxWidgets lib can be built as either a
                   # single monolithic DLL or as a collection of DLLs.
                   # This flag controls which set of libs will be used
                   # on Windows.  (For other platforms it is automatic
                   # via using wx-config.)

FINAL = 0          # Will use the release version of the wxWidgets libs on MSW.

HYBRID = 1         # Will use the "hybrid" version of the wxWidgets
                   # libs on MSW.  A "hybrid" build is one that is
                   # basically a release build, but that also defines
                   # __WXDEBUG__ to activate the runtime checks and
                   # assertions in the library.  When any of these is
                   # triggered it is turned into a Python exception so
                   # this is a very useful feature to have turned on.


                   # Version part of wxWidgets LIB/DLL names
WXDLLVER = '%d%d' % (VER_MAJOR, VER_MINOR)

WXPY_SRC = '.'  # Assume we're in the source tree already, but allow the
                # user to change it, particularly for extension building.


#----------------------------------------------------------------------

def msg(text):
    if hasattr(sys, 'setup_is_main') and sys.setup_is_main:
        print text


def opj(*args):
    path = os.path.join(*args)
    return os.path.normpath(path)


def libFlag():
    if FINAL:
        rv = ''
    elif HYBRID:
        rv = 'h'
    else:
        rv = 'd'
    if UNICODE:
        rv = 'u' + rv
    return rv


#----------------------------------------------------------------------
# Some other globals
#----------------------------------------------------------------------

PKGDIR = 'wx'
wxpExtensions = []
DATA_FILES = []
CLEANUP = []

force = '--force' in sys.argv or '-f' in sys.argv
debug = '--debug' in sys.argv or '-g' in sys.argv
cleaning = 'clean' in sys.argv


# change the PORT default for wxMac
if sys.platform[:6] == "darwin":
    WXPORT = 'mac'

# and do the same for wxMSW, just for consistency
if os.name == 'nt':
    WXPORT = 'msw'

WXPYTHON_TYPE_TABLE = '_wxPython_table'

#----------------------------------------------------------------------
# Check for build flags on the command line
#----------------------------------------------------------------------

# Boolean (int) flags
for flag in [ 'BUILD_ACTIVEX', 'BUILD_DLLWIDGET',
              'BUILD_GIZMOS', 'BUILD_GLCANVAS', 
              'BUILD_OGL', 'BUILD_STC',     
             'CORE_ONLY', 'PREP_ONLY', 'USE_SWIG', 'UNICODE',
             'UNDEF_NDEBUG', 'NO_SCRIPTS', 'NO_HEADERS', 'BUILD_RENAMERS',
             'FULL_DOCS', 'INSTALL_MULTIVERSION', 'EP_ADD_OPTS', 'EP_FULL_VER',
             'MONOLITHIC', 'FINAL', 'HYBRID', ]:
    for x in range(len(sys.argv)):
        if sys.argv[x].find(flag) == 0:
            pos = sys.argv[x].find('=') + 1
            if pos > 0:
                vars()[flag] = eval(sys.argv[x][pos:])
                sys.argv[x] = ''

# String options
for option in ['WX_CONFIG', 'SYS_WX_CONFIG', 'WXDLLVER', 'BUILD_BASE',
               'WXPORT', 'SWIG', 'CONTRIBS_INC', 'WXPY_SRC', 'FLAVOUR',
               'VER_FLAGS',
               ]:
    for x in range(len(sys.argv)):
        if sys.argv[x].find(option) == 0:
            pos = sys.argv[x].find('=') + 1
            if pos > 0:
                vars()[option] = sys.argv[x][pos:]
                sys.argv[x] = ''

sys.argv = filter(None, sys.argv)


#----------------------------------------------------------------------
# some helper functions
#----------------------------------------------------------------------

def Verify_WX_CONFIG():
    """ Called below for the builds that need wx-config, if WX_CONFIG
        is not set then determines the flags needed based on build
        options and searches for wx-config on the PATH.  
    """
    # if WX_CONFIG hasn't been set to an explicit value then construct one.
    global WX_CONFIG
    if WX_CONFIG is None:
        WX_CONFIG='wx-config'
        port = WXPORT
        if port == "x11":
            port = "x11univ"
        flags =  ' --toolkit=%s' % port
        flags += ' --unicode=%s' % (UNICODE and 'yes' or 'no')
        flags += ' --version=%s.%s' % (VER_MAJOR, VER_MINOR)

        searchpath = os.environ["PATH"]
        for p in searchpath.split(':'):
            fp = os.path.join(p, 'wx-config')
            if os.path.exists(fp) and os.access(fp, os.X_OK):
                # success
                msg("Found wx-config: " + fp)
                msg("    Using flags: " + flags)
                WX_CONFIG = fp + flags
                if hasattr(sys, 'setup_is_main') and not sys.setup_is_main:
                    WX_CONFIG += " 2>/dev/null "
                break
        else:
            msg("ERROR: WX_CONFIG not specified and wx-config not found on the $PATH")
            # should we exit?

        # TODO:  execute WX_CONFIG --list and verify a matching config is found
        

def run_swig(files, dir, gendir, package, USE_SWIG, force, swig_args,
             swig_deps=[], add_under=False):
    """Run SWIG the way I want it done"""

    if USE_SWIG and not os.path.exists(os.path.join(dir, gendir)):
        os.mkdir(os.path.join(dir, gendir))

    sources = []

    if add_under:  pre = '_'
    else:          pre = ''
        
    for file in files:
        basefile = os.path.splitext(file)[0]
        i_file   = os.path.join(dir, file)
        py_file  = os.path.join(dir, gendir, pre+basefile+'.py')
        cpp_file = os.path.join(dir, gendir, pre+basefile+'_wrap.cpp')

        if add_under:
            interface = ['-interface', '_'+basefile+'_']
        else:
            interface = []
            
        sources.append(cpp_file)

        if not cleaning and USE_SWIG:
            for dep in swig_deps:
                # this may fail for external builds, but it's not 
                # a fatal error, so keep going.
                try:
                    if newer(dep, py_file) or newer(dep, cpp_file):
                        force = 1
                        break
                except:
                    pass

            if force or newer(i_file, py_file) or newer(i_file, cpp_file):
                ## we need forward slashes here, even on win32
                #cpp_file = opj(cpp_file) #'/'.join(cpp_file.split('\\'))
                #i_file = opj(i_file)     #'/'.join(i_file.split('\\'))

                if BUILD_RENAMERS:
                    xmltemp = tempfile.mktemp('.xml')

                    # First run swig to produce the XML file, adding
                    # an extra -D that prevents the old rename
                    # directives from being used
                    cmd = [ swig_cmd ] + swig_args + \
                          [ '-DBUILDING_RENAMERS', '-xmlout', xmltemp ] + \
                          ['-I'+dir, '-o', cpp_file, i_file]
                    msg(' '.join(cmd))
                    spawn(cmd)

                    # Next run build_renamers to process the XML
                    myRenamer = BuildRenamers()
                    myRenamer.run(dir, pre+basefile, xmltemp)
                    os.remove(xmltemp)

                # Then run swig for real
                cmd = [ swig_cmd ] + swig_args + interface + \
                      ['-I'+dir, '-o', cpp_file, i_file]
                msg(' '.join(cmd))
                spawn(cmd)


        # copy the generated python file to the package directory
        copy_file(py_file, package, update=not force, verbose=0)
        CLEANUP.append(opj(package, os.path.basename(py_file)))

    return sources


def swig_version():
    # It may come on either stdout or stderr, depending on the
    # version, so read both.
    i, o, e = os.popen3(SWIG + ' -version', 't')
    stext = o.read() + e.read()
    import re
    match = re.search(r'[0-9]+\.[0-9]+\.[0-9]+$', stext, re.MULTILINE)
    if not match:
        raise 'NotFound'
    SVER = match.group(0)
    return SVER


# Specializations of some distutils command classes
class wx_smart_install_data(distutils.command.install_data.install_data):
    """need to change self.install_dir to the actual library dir"""
    def run(self):
        install_cmd = self.get_finalized_command('install')
        self.install_dir = getattr(install_cmd, 'install_lib')
        return distutils.command.install_data.install_data.run(self)


class wx_extra_clean(distutils.command.clean.clean):
    """
    Also cleans stuff that this setup.py copies itself.  If the
    --all flag was used also searches for .pyc, .pyd, .so files
    """
    def run(self):
        from distutils import log
        from distutils.filelist import FileList
        global CLEANUP

        distutils.command.clean.clean.run(self)

        if self.all:
            fl = FileList()
            fl.include_pattern("*.pyc", 0)
            fl.include_pattern("*.pyd", 0)
            fl.include_pattern("*.so", 0)
            CLEANUP += fl.files

        for f in CLEANUP:
            if os.path.isdir(f):
                try:
                    if not self.dry_run and os.path.exists(f):
                        os.rmdir(f)
                    log.info("removing '%s'", f)
                except IOError:
                    log.warning("unable to remove '%s'", f)

            else:
                try:
                    if not self.dry_run and os.path.exists(f):
                        os.remove(f)
                    log.info("removing '%s'", f)
                except IOError:
                    log.warning("unable to remove '%s'", f)



class wx_install(distutils.command.install.install):
    """
    Turns off install_path_file
    """
    def initialize_options(self):
        distutils.command.install.install.initialize_options(self)
        self.install_path_file = 0
        

class wx_install_headers(distutils.command.install_headers.install_headers):
    """
    Install the header files to the WXPREFIX, with an extra dir per
    filename too
    """
    def initialize_options(self):
        self.root = None
        distutils.command.install_headers.install_headers.initialize_options(self)

    def finalize_options(self):
        self.set_undefined_options('install', ('root', 'root'))
        distutils.command.install_headers.install_headers.finalize_options(self)

    def run(self):
        if os.name == 'nt':
            return
        headers = self.distribution.headers
        if not headers:
            return

        root = self.root
        if root is None or WXPREFIX.startswith(root):
            root = ''
        for header, location in headers:
            install_dir = os.path.normpath(root +
                                           WXPREFIX +
                                           '/include/wx-%d.%d/wx' % (VER_MAJOR, VER_MINOR) +
                                           location)
            self.mkpath(install_dir)
            (out, _) = self.copy_file(header, install_dir)
            self.outfiles.append(out)




def build_locale_dir(destdir, verbose=1):
    """Build a locale dir under the wxPython package for MSW"""
    moFiles = glob.glob(opj(WXDIR, 'locale', '*.mo'))
    for src in moFiles:
        lang = os.path.splitext(os.path.basename(src))[0]
        #dest = opj(destdir, lang)
        dest = opj(destdir, lang, 'LC_MESSAGES')
        mkpath(dest, verbose=verbose)
        copy_file(src, opj(dest, 'wxstd.mo'), update=1, verbose=verbose)
        CLEANUP.append(opj(dest, 'wxstd.mo'))
        CLEANUP.append(dest)


def build_locale_list(srcdir):
    # get a list of all files under the srcdir, to be used for install_data
    def walk_helper(lst, dirname, files):
        for f in files:
            filename = opj(dirname, f)
            if not os.path.isdir(filename):
                lst.append( (dirname, [filename]) )
    file_list = []
    os.path.walk(srcdir, walk_helper, file_list)
    return file_list


def find_data_files(srcdir, *wildcards):
    # get a list of all files under the srcdir matching wildcards,
    # returned in a format to be used for install_data

    def walk_helper(arg, dirname, files):
        names = []
        lst, wildcards = arg
        for wc in wildcards:
            for f in files:
                filename = opj(dirname, f)
                if fnmatch.fnmatch(filename, wc) and not os.path.isdir(filename):
                    names.append(filename)
        if names:
            lst.append( (dirname, names ) )

    file_list = []
    os.path.walk(srcdir, walk_helper, (file_list, wildcards))
    return file_list


def makeLibName(name):
    if os.name == 'posix':
        libname = '%s_%s-%s' % (WXBASENAME, name, WXRELEASE)
    elif name:
        libname = 'wxmsw%s%s_%s' % (WXDLLVER, libFlag(), name)
    else:
        libname = 'wxmsw%s%s' % (WXDLLVER, libFlag())
    return [libname]


def findLib(name, libdirs):
    name = makeLibName(name)[0]
    if os.name == 'posix':
        dirs = libdirs + ['/usr/lib', '/usr/local/lib']
        name = 'lib'+name
    else:
        dirs = libdirs[:]
    for d in dirs:
        p = os.path.join(d, name)
        if glob.glob(p+'*') != []:
            return True
    return False


def adjustCFLAGS(cflags, defines, includes):
    '''Extract the raw -I, -D, and -U flags and put them into
       defines and includes as needed.'''
    newCFLAGS = []
    for flag in cflags:
        if flag[:2] == '-I':
            includes.append(flag[2:])
        elif flag[:2] == '-D':
            flag = flag[2:]
            if flag.find('=') == -1:
                defines.append( (flag, None) )
            else:
                defines.append( tuple(flag.split('=')) )
        elif flag[:2] == '-U':
            defines.append( (flag[2:], ) )
        else:
            newCFLAGS.append(flag)
    return newCFLAGS



def adjustLFLAGS(lflags, libdirs, libs):
    '''Extract the -L and -l flags and put them in libdirs and libs as needed'''
    newLFLAGS = []
    for flag in lflags:
        if flag[:2] == '-L':
            libdirs.append(flag[2:])
        elif flag[:2] == '-l':
            libs.append(flag[2:])
        else:
            newLFLAGS.append(flag)

    # remove any flags for universal binaries, we'll get those from
    # distutils instead
    return [flag for flag in newLFLAGS
            if flag not in ['-isysroot', '-arch', 'ppc', 'i386'] and
            not flag.startswith('/Developer') ]



def getExtraPath(shortVer=True, addOpts=False):
    """Get the dirname that wxPython will be installed under."""

    if shortVer:
        # short version, just Major.Minor
        ep = "wx-%d.%d" % (VER_MAJOR, VER_MINOR)
         
        # plus release if minor is odd
        if VER_MINOR % 2 == 1:
            ep += ".%d" % VER_RELEASE
            
        ##ep = "wx-%d.%d.%d" % (VER_MAJOR, VER_MINOR, VER_RELEASE)
        
    else:
        # long version, full version 
        ep = "wx-%d.%d.%d.%d" % (VER_MAJOR, VER_MINOR, VER_RELEASE, VER_SUBREL)

    if addOpts:
        port = WXPORT
        if port == "msw": port = "win32"
        ep += "-%s-%s" % (WXPORT, (UNICODE and 'unicode' or 'ansi'))
        
    if FLAVOUR:
        ep += "-" + FLAVOUR

    return ep

#----------------------------------------------------------------------
# sanity checks

if CORE_ONLY:
    BUILD_GLCANVAS = 0
    BUILD_OGL = 0
    BUILD_STC = 0
    BUILD_GIZMOS = 0
    BUILD_DLLWIDGET = 0
    BUILD_ACTIVEX = 0

if debug:
    FINAL  = 0
    HYBRID = 0

if FINAL:
    HYBRID = 0

if UNICODE and WXPORT not in ['msw', 'gtk2', 'mac']:
    raise SystemExit, "UNICODE mode not currently supported on this WXPORT: "+WXPORT


if CONTRIBS_INC:
    CONTRIBS_INC = [ CONTRIBS_INC ]
else:
    CONTRIBS_INC = []


#----------------------------------------------------------------------
# Setup some platform specific stuff
#----------------------------------------------------------------------

if os.name == 'nt':
    # Set compile flags and such for MSVC.  These values are derived
    # from the wxWidgets makefiles for MSVC, other compilers settings
    # will probably vary...
    if os.environ.has_key('WXWIN'):
        WXDIR = os.environ['WXWIN']
    else:
        msg("WARNING: WXWIN not set in environment.")
        WXDIR = '..'  # assumes in CVS tree
    WXPLAT = '__WXMSW__'
    GENDIR = 'msw'

    includes = ['include', 'src',
                opj(WXDIR, 'lib', 'vc_dll', 'msw'  + libFlag()),
                opj(WXDIR, 'include'),
                opj(WXDIR, 'contrib', 'include'),
                ]

    defines = [ ('WIN32', None),
                ('_WINDOWS', None),

                (WXPLAT, None),
                ('WXUSINGDLL', '1'),

                ('SWIG_TYPE_TABLE', WXPYTHON_TYPE_TABLE),
                ('SWIG_PYTHON_OUTPUT_TUPLE', None),
                ('WXP_USE_THREAD', '1'),
                ]

    if UNDEF_NDEBUG:
        defines.append( ('NDEBUG',) )  # using a 1-tuple makes it do an undef

    if HYBRID:
        defines.append( ('__NO_VC_CRTDBG__', None) )

    if not FINAL or HYBRID:
        defines.append( ('__WXDEBUG__', None) )

    if UNICODE:
        defines.append( ('wxUSE_UNICODE', 1) )

    libdirs = [ opj(WXDIR, 'lib', 'vc_dll') ]
    if MONOLITHIC:
        libs = makeLibName('')
    else:
        libs = [ 'wxbase' + WXDLLVER + libFlag(), 
                 'wxbase' + WXDLLVER + libFlag() + '_net',
                 'wxbase' + WXDLLVER + libFlag() + '_xml',
                 makeLibName('core')[0],
                 makeLibName('adv')[0],
                 makeLibName('html')[0],
                 ]

    libs = libs + ['kernel32', 'user32', 'gdi32', 'comdlg32',
            'winspool', 'winmm', 'shell32', 'oldnames', 'comctl32',
            'odbc32', 'ole32', 'oleaut32', 'uuid', 'rpcrt4',
            'advapi32', 'wsock32']


    cflags = [ '/Gy',
             # '/GX-'  # workaround for internal compiler error in MSVC on some machines
             ]
    lflags = None

    # Other MSVC flags...
    # Too bad I don't remember why I was playing with these, can they be removed?
    if FINAL:
        pass #cflags = cflags + ['/O1']
    elif HYBRID :
        pass #cflags = cflags + ['/Ox']
    else:
        pass # cflags = cflags + ['/Od', '/Z7']
             # lflags = ['/DEBUG', ]



#----------------------------------------------------------------------

elif os.name == 'posix':
    WXDIR = '..'
    includes = ['include', 'src']
    defines = [('SWIG_TYPE_TABLE', WXPYTHON_TYPE_TABLE),
               ('SWIG_PYTHON_OUTPUT_TUPLE', None),
               ('WXP_USE_THREAD', '1'),
               ]
    if UNDEF_NDEBUG:
        defines.append( ('NDEBUG',) )  # using a 1-tuple makes it do an undef

    Verify_WX_CONFIG()

    libdirs = []
    libs = []

    # If you get unresolved symbol errors on Solaris and are using gcc, then
    # uncomment this block to add the right flags to the link step and build
    # again.
    ## if os.uname()[0] == 'SunOS':
    ##     import commands
    ##     libs.append('gcc')
    ##     libdirs.append(commands.getoutput("gcc -print-search-dirs | grep '^install' | awk '{print $2}'")[:-1])

    cflags = os.popen(WX_CONFIG + ' --cxxflags', 'r').read()[:-1]
    cflags = cflags.split()
    if debug:
        cflags.append('-g')
        cflags.append('-O0')
    else:
        cflags.append('-O3')

    lflags = os.popen(WX_CONFIG + ' --libs', 'r').read()[:-1]
    MONOLITHIC = (lflags.find("_xrc") == -1)
    lflags = lflags.split()

    WXBASENAME = os.popen(WX_CONFIG + ' --basename').read()[:-1]
    WXRELEASE  = os.popen(WX_CONFIG + ' --release').read()[:-1]
    WXPREFIX   = os.popen(WX_CONFIG + ' --prefix').read()[:-1]


    if sys.platform[:6] == "darwin" and WXPORT == 'mac':
        # Flags and such for a Darwin (Max OS X) build of Python
        WXPLAT = '__WXMAC__'
        GENDIR = 'mac'
        libs = ['stdc++']
        NO_SCRIPTS = 1


    else:
        # Set flags for other Unix type platforms
        GENDIR = WXPORT

        if WXPORT == 'gtk':
            WXPLAT = '__WXGTK__'
            portcfg = os.popen('gtk-config --cflags', 'r').read()[:-1]
        elif WXPORT == 'gtk2':
            WXPLAT = '__WXGTK__'
            GENDIR = 'gtk' # no code differences so use the same generated sources
            portcfg = os.popen('pkg-config gtk+-2.0 --cflags', 'r').read()[:-1]
            BUILD_BASE = BUILD_BASE + '-' + WXPORT
        elif WXPORT == 'x11':
            WXPLAT = '__WXX11__'
            portcfg = ''
            BUILD_BASE = BUILD_BASE + '-' + WXPORT
        else:
            raise SystemExit, "Unknown WXPORT value: " + WXPORT

        cflags += portcfg.split()

        # Some distros (e.g. Mandrake) put libGLU in /usr/X11R6/lib, but
        # wx-config doesn't output that for some reason.  For now, just
        # add it unconditionally but we should really check if the lib is
        # really found there or wx-config should be fixed.
        libdirs.append("/usr/X11R6/lib")


    # Move the various -I, -D, etc. flags we got from the *config scripts
    # into the distutils lists.
    cflags = adjustCFLAGS(cflags, defines, includes)
    lflags = adjustLFLAGS(lflags, libdirs, libs)


#----------------------------------------------------------------------
else:
    raise 'Sorry, platform not supported...'


#----------------------------------------------------------------------
# build options file
#----------------------------------------------------------------------

if SYS_WX_CONFIG is None:
    SYS_WX_CONFIG = WX_CONFIG

build_options_template = """
UNICODE=%d
UNDEF_NDEBUG=%d
INSTALL_MULTIVERSION=%d
FLAVOUR="%s"
EP_ADD_OPTS=%d
EP_FULL_VER=%d
WX_CONFIG="%s"
WXPORT="%s"
MONOLITHIC=%d
FINAL=%d
HYBRID=%d
""" % (UNICODE, UNDEF_NDEBUG, INSTALL_MULTIVERSION, FLAVOUR, EP_ADD_OPTS,
       EP_FULL_VER, SYS_WX_CONFIG, WXPORT, MONOLITHIC, FINAL, HYBRID)

try: 
    from build_options import *
except:
    build_options_file = os.path.join(os.path.dirname(__file__), "build_options.py")
    if not os.path.exists(build_options_file):
        try:
            myfile = open(build_options_file, "w")
            myfile.write(build_options_template)
            myfile.close()
        except:
            print "WARNING: Unable to create build_options.py."
    

#----------------------------------------------------------------------
# post platform setup checks and tweaks, create the full version string
#----------------------------------------------------------------------

if UNICODE:
    BUILD_BASE = BUILD_BASE + '.unicode'

if os.path.exists('DAILY_BUILD'):
    VER_FLAGS += '.' + open('DAILY_BUILD').read().strip()

VERSION = "%s.%s.%s.%s%s" % (VER_MAJOR, VER_MINOR, VER_RELEASE,
                             VER_SUBREL, VER_FLAGS)


#----------------------------------------------------------------------
# SWIG defaults
#----------------------------------------------------------------------

# *.i files could live in the wxWidgets/wxPython/src dir, or in 
# a subdirectory of the devel package. Let's specify both 
# dirs as includes so we don't have to guess which is correct.
 
wxfilesdir = ""
i_subdir = opj("include", getExtraPath(), "wx", "wxPython", "i_files")
if os.name != "nt":
    wxfilesdir = opj(WXPREFIX, i_subdir)
else:
    wxfilesdir = opj(WXPY_SRC, i_subdir)

i_files_includes = [ '-I' + opj(WXPY_SRC, 'src'),
                     '-I' + wxfilesdir ]

swig_cmd = SWIG
swig_force = force
swig_args = ['-c++',
             #'-Wall',
             '-python',
             '-new_repr',
             '-modern',
             '-D'+WXPLAT,
             ] + i_files_includes

if USE_SWIG:
    SVER = swig_version()
    if int(SVER[-2:]) >= 29:
        swig_args += [ '-fastdispatch',
                       '-fvirtual',
                       '-fastinit',
                       '-fastunpack',
                       #'-outputtuple',  Currently setting this with a -D define above
                       ]
             
if UNICODE:
    swig_args.append('-DwxUSE_UNICODE')

if FULL_DOCS:
    swig_args.append('-D_DO_FULL_DOCS')
    

swig_deps = [ opj(WXPY_SRC, 'src/my_typemaps.i'),
              opj(WXPY_SRC, 'src/pyfragments.swg'),
              ]

depends = [ #'include/wx/wxPython/wxPython.h',
            #'include/wx/wxPython/wxPython_int.h',
            #'src/pyclasses.h',
            ]

#----------------------------------------------------------------------

####################################
# BuildRenamers
####################################

import pprint, shutil
try:
    import libxml2
    FOUND_LIBXML2 = True
except ImportError:
    FOUND_LIBXML2 = False

#---------------------------------------------------------------------------

renamerTemplateStart = """\
// A bunch of %rename directives generated by BuildRenamers in config.py
// in order to remove the wx prefix from all global scope names.

#ifndef BUILDING_RENAMERS

"""

renamerTemplateEnd = """
#endif
"""

wxPythonTemplateStart = """\
## This file reverse renames symbols in the wx package to give
## them their wx prefix again, for backwards compatibility.
##
## Generated by BuildRenamers in config.py

# This silly stuff here is so the wxPython.wx module doesn't conflict
# with the wx package.  We need to import modules from the wx package
# here, then we'll put the wxPython.wx entry back in sys.modules.
import sys
_wx = None
if sys.modules.has_key('wxPython.wx'):
    _wx = sys.modules['wxPython.wx']
    del sys.modules['wxPython.wx']

import wx.%s

sys.modules['wxPython.wx'] = _wx
del sys, _wx


# Now assign all the reverse-renamed names:
"""

wxPythonTemplateEnd = """

"""



#---------------------------------------------------------------------------
class BuildRenamers:
    def run(self, destdir, modname, xmlfile, wxPythonDir="wxPython"):    

        assert FOUND_LIBXML2, "The libxml2 module is required to use the BuildRenamers functionality."
            
        if not os.path.exists(wxPythonDir):
            os.mkdir(wxPythonDir)

        swigDest = os.path.join(destdir, "_"+modname+"_rename.i")
        pyDest = os.path.join(wxPythonDir, modname + '.py')
    
        swigDestTemp = tempfile.mktemp('.tmp')
        swigFile = open(swigDestTemp, "w")
        swigFile.write(renamerTemplateStart)
    
        pyDestTemp = tempfile.mktemp('.tmp')
        pyFile = open(pyDestTemp, "w")
        pyFile.write(wxPythonTemplateStart % modname)
    
        print "Parsing XML and building renamers..."
        self.processXML(xmlfile, modname, swigFile, pyFile)
        
        self.checkOtherNames(pyFile, modname,
                        os.path.join(destdir, '_'+modname+'_reverse.txt'))
        pyFile.write(wxPythonTemplateEnd)        
        pyFile.close()

        swigFile.write(renamerTemplateEnd)
        swigFile.close()

        # Compare the files just created with the existing one and
        # blow away the old one if they are different.
        for dest, temp in [(swigDest, swigDestTemp),
                           (pyDest, pyDestTemp)]:
            # NOTE: we don't use shutil.move() because it was introduced
            # in Python 2.3. Eventually we can switch to it when people
            # stop building using 2.2.
            if not os.path.exists(dest):
                shutil.copyfile(temp, dest)
            elif open(dest).read() != open(temp).read():
                os.unlink(dest)
                shutil.copyfile(temp, dest)
            else:
                print dest + " not changed."
                os.unlink(temp)
    
    #---------------------------------------------------------------------------
    
    
    def GetAttr(self, node, name):
        path = "./attributelist/attribute[@name='%s']/@value" % name
        n = node.xpathEval2(path)
        if len(n):
            return n[0].content
        else:
            return None
        
    
    def processXML(self, xmlfile, modname, swigFile, pyFile):
   
        topnode = libxml2.parseFile(xmlfile).children
    
        # remove any import nodes as we don't need to do renamers for symbols found therein
        imports = topnode.xpathEval2("*/import")
        for n in imports:
            n.unlinkNode()
            n.freeNode()
    
        # do a depth first iteration over what's left
        for node in topnode:
            doRename = False
            addWX = False
            revOnly = False
    
    
            if node.name == "class":
                lastClassName = name = self.GetAttr(node, "name")
                lastClassSymName = sym_name = self.GetAttr(node, "sym_name")
                doRename = True
                if sym_name != name:
                    name = sym_name
                    addWX = True
    
            # renamed constructors
            elif node.name == "constructor":
                name     = self.GetAttr(node, "name")
                sym_name = self.GetAttr(node, "sym_name")
                if sym_name != name:
                    name = sym_name
                    addWX = True
                    doRename = True
    
            # only enumitems at the top level
            elif node.name == "enumitem" and node.parent.parent.name == "include":
                name     = self.GetAttr(node, "name")
                sym_name = self.GetAttr(node, "sym_name")
                doRename = True
    
    
            elif node.name in ["cdecl", "constant"]:
                name      = self.GetAttr(node, "name")
                sym_name  = self.GetAttr(node, "sym_name")
                toplevel  = node.parent.name == "include"
    
                # top-level functions
                if toplevel and self.GetAttr(node, "view") == "globalfunctionHandler":
                    doRename = True
    
                # top-level global vars
                elif toplevel and self.GetAttr(node, "feature_immutable") == "1":
                    doRename = True
    
                # static methods
                elif self.GetAttr(node, "view") == "staticmemberfunctionHandler":
                    name     = lastClassName + '_' + name
                    sym_name = lastClassSymName + '_' + sym_name
                    # only output the reverse renamer in this case
                    doRename = revOnly = True
           
                if doRename and name != sym_name:
                    name = sym_name
                    addWX = True
                   
    
            if doRename and name:
                old = new = name
                if old.startswith('wx') and not old.startswith('wxEVT_'):
                    # remove all wx prefixes except wxEVT_ and write a %rename directive for it
                    new = old[2:]
                    if not revOnly:
                        swigFile.write("%%rename(%s)  %35s;\n" % (new, old))
    
                # Write assignments to import into the old wxPython namespace
                if addWX and not old.startswith('wx'):
                    old = 'wx'+old
                pyFile.write("%s = wx.%s.%s\n" % (old, modname, new))
                
    
    #---------------------------------------------------------------------------
    
    def checkOtherNames(self, pyFile, moduleName, filename):
        if os.path.exists(filename):
            prefixes = []
            for line in file(filename):
                if line.endswith('\n'):
                    line = line[:-1]
                if line and not line.startswith('#'):
                    if line.endswith('*'):
                        prefixes.append(line[:-1])
                    elif line.find('=') != -1:
                        pyFile.write("%s\n" % line)
                    else:
                        wxname = 'wx' + line
                        if line.startswith('wx') or line.startswith('WX') or line.startswith('EVT'):
                            wxname = line
                        pyFile.write("%s = wx.%s.%s\n" % (wxname, moduleName, line))
    
            if prefixes:
                pyFile.write(
                    "\n\nd = globals()\nfor k, v in wx.%s.__dict__.iteritems():"
                    % moduleName)
                first = True
                for p in prefixes:
                    if first:
                        pyFile.write("\n    if ")
                        first = False
                    else:
                        pyFile.write("\n    elif ")
                    pyFile.write("k.startswith('%s'):\n        d[k] = v" % p)
                pyFile.write("\ndel d, k, v\n\n")

                        
#---------------------------------------------------------------------------
