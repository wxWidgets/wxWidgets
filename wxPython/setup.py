#!/usr/bin/env python
#----------------------------------------------------------------------

import sys, os, glob, fnmatch
from distutils.core      import setup, Extension
from distutils.file_util import copy_file
from distutils.dir_util  import mkpath
from distutils.dep_util  import newer
from distutils.spawn     import spawn
from distutils.command.install_data import install_data

#----------------------------------------------------------------------
# flags and values that affect this script
#----------------------------------------------------------------------

VER_MAJOR        = 2      # The first three must match wxWindows
VER_MINOR        = 5
VER_RELEASE      = 0
VER_SUBREL       = 0      # wxPython release num for x.y.z release of wxWindows
VER_FLAGS        = "p1"   # release flags, such as prerelease num, unicode, etc.

DESCRIPTION      = "Cross platform GUI toolkit for Python"
AUTHOR           = "Robin Dunn"
AUTHOR_EMAIL     = "Robin Dunn <robin@alldunn.com>"
URL              = "http://wxPython.org/"
LICENSE          = "wxWindows (LGPL derivative)"
LONG_DESCRIPTION = """\
wxPython is a GUI toolkit for Python that is a wrapper around the
wxWindows C++ GUI library.  wxPython provides a large variety of
window types and controls, all implemented with a native look and
feel (by using the native widgets) on the platforms it is supported
on.
"""


# Config values below this point can be reset on the setup.py command line.

BUILD_GLCANVAS = 1 # If true, build the contrib/glcanvas extension module
BUILD_OGL = 1      # If true, build the contrib/ogl extension module
BUILD_STC = 1      # If true, build the contrib/stc extension module
BUILD_XRC = 1      # XML based resource system
BUILD_GIZMOS = 1   # Build a module for the gizmos contrib library
BUILD_DLLWIDGET = 0# Build a module that enables unknown wx widgets
                   # to be loaded from a DLL and to be used from Python.

                   # Internet Explorer wrapper (experimental)
BUILD_IEWIN = (os.name == 'nt')


CORE_ONLY = 0      # if true, don't build any of the above

PREP_ONLY = 0      # Only run the prepatory steps, not the actual build.

USE_SWIG = 0       # Should we actually execute SWIG, or just use the
                   # files already in the distribution?

UNICODE = 0        # This will pass the 'wxUSE_UNICODE' flag to SWIG and
                   # will ensure that the right headers are found and the
                   # right libs are linked.

IN_CVS_TREE = 1    # Set to true if building in a full wxWindows CVS
                   # tree, or the new style of a full wxPythonSrc tarball.
                   # wxPython used to be distributed as a separate source
                   # tarball without the wxWindows but with a copy of the
                   # needed contrib code.  That's no longer the case and so
                   # this setting is now defaulting to true.  Eventually it
                   # should be removed entirly.

UNDEF_NDEBUG = 1   # Python 2.2 on Unix/Linux by default defines NDEBUG,
                   # and distutils will pick this up and use it on the
                   # compile command-line for the extensions.  This could
                   # conflict with how wxWindows was built.  If NDEBUG is
                   # set then wxWindows' __WXDEBUG__ setting will be turned
                   # off.  If wxWindows was actually built with it turned
                   # on then you end up with mismatched class structures,
                   # and wxPython will crash.

NO_SCRIPTS = 0     # Don't install the tool scripts

WX_CONFIG = None   # Usually you shouldn't need to touch this, but you can set
                   # it to pass an alternate version of wx-config or alternate
                   # flags, eg. as required by the .deb in-tree build.  By
                   # default a wx-config command will be assembled based on
                   # version, port, etc. and it will be looked for on the
                   # default $PATH.

WXPORT = 'gtk'     # On Linux/Unix there are several ports of wxWindows available.
                   # Setting this value lets you select which will be used for
                   # the wxPython build.  Possibilites are 'gtk', 'gtk2' and
                   # 'x11'.  Curently only gtk and gtk2 works.

BUILD_BASE = "build"       # Directory to use for temporary build files.



# Some MSW build settings

FINAL = 0          # Mirrors use of same flag in wx makefiles,
                   # (0 or 1 only) should probably find a way to
                   # autodetect this...

HYBRID = 1         # If set and not debug or FINAL, then build a
                   # hybrid extension that can be used by the
                   # non-debug version of python, but contains
                   # debugging symbols for wxWindows and wxPython.
                   # wxWindows must have been built with /MD, not /MDd
                   # (using FINAL=hybrid will do it.)

WXDLLVER = '250'   # Version part of wxWindows DLL name


#----------------------------------------------------------------------

def msg(text):
    if __name__ == "__main__":
        print text


def opj(*args):
    path = apply(os.path.join, args)
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

PKGDIR = 'wxPython'
wxpExtensions = []
DATA_FILES = []

force = '--force' in sys.argv or '-f' in sys.argv
debug = '--debug' in sys.argv or '-g' in sys.argv

# change the PORT default for wxMac
if sys.platform[:6] == "darwin":
    WXPORT = 'mac'

# and do the same for wxMSW, just for consistency
if os.name == 'nt':
    WXPORT = 'msw'


#----------------------------------------------------------------------
# Check for build flags on the command line
#----------------------------------------------------------------------

# Boolean (int) flags
for flag in ['BUILD_GLCANVAS', 'BUILD_OGL', 'BUILD_STC', 'BUILD_XRC',
             'BUILD_GIZMOS', 'BUILD_DLLWIDGET', 'BUILD_IEWIN',
             'CORE_ONLY', 'PREP_ONLY', 'USE_SWIG', 'IN_CVS_TREE', 'UNICODE',
             'UNDEF_NDEBUG', 'NO_SCRIPTS',
             'FINAL', 'HYBRID', ]:
    for x in range(len(sys.argv)):
        if sys.argv[x].find(flag) == 0:
            pos = sys.argv[x].find('=') + 1
            if pos > 0:
                vars()[flag] = eval(sys.argv[x][pos:])
                sys.argv[x] = ''

# String options
for option in ['WX_CONFIG', 'WXDLLVER', 'BUILD_BASE', 'WXPORT']:
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
    """ Called below for the builds that need wx-config,
        if WX_CONFIG is not set then tries to select the specific
        wx*-config script based on build options.  If not found
        then it defaults to 'wx-config'.
    """
    # if WX_CONFIG hasn't been set to an explicit value then construct one.
    global WX_CONFIG
    if WX_CONFIG is None:
        if debug:             # TODO: Fix this.  wxPython's --debug shouldn't be tied to wxWindows...
            df = 'd'
        else:
            df = ''
        if UNICODE:
            uf = 'u'
        else:
            uf = ''
        ver2 = "%s.%s" % (VER_MAJOR, VER_MINOR)
        WX_CONFIG = 'wx%s%s%s-%s-config' % (WXPORT, uf, df, ver2)

        searchpath = os.environ["PATH"]
        for p in searchpath.split(':'):
            fp = os.path.join(p, WX_CONFIG)
            if os.path.exists(fp) and os.access(fp, os.X_OK):
                # success
                msg("Found wx-config: " + fp)
                WX_CONFIG = fp
                break
        else:
            msg("WX_CONFIG not specified and %s not found on $PATH "
                  "defaulting to \"wx-config\"" % WX_CONFIG)
            WX_CONFIG = 'wx-config'



def run_swig(files, dir, gendir, package, USE_SWIG, force, swig_args, swig_deps=[]):
    """Run SWIG the way I want it done"""
    if not os.path.exists(os.path.join(dir, gendir)):
        os.mkdir(os.path.join(dir, gendir))

    sources = []

    for file in files:
        basefile = os.path.splitext(file)[0]
        i_file   = os.path.join(dir, file)
        py_file  = os.path.join(dir, gendir, basefile+'.py')
        cpp_file = os.path.join(dir, gendir, basefile+'.cpp')

        sources.append(cpp_file)

        if USE_SWIG:
            for dep in swig_deps:
                if newer(dep, py_file) or newer(dep, cpp_file):
                    force = 1
                    break

            if force or newer(i_file, py_file) or newer(i_file, cpp_file):
                # we need forward slashes here even on win32
                cpp_file = '/'.join(cpp_file.split('\\'))
                i_file = '/'.join(i_file.split('\\'))

                cmd = ['./wxSWIG/wxswig'] + swig_args + ['-I'+dir, '-c', '-o', cpp_file, i_file]
                msg(' '.join(cmd))
                spawn(cmd)

        # copy the generated python file to the package directory
        copy_file(py_file, package, update=not force, verbose=0)

    return sources



def contrib_copy_tree(src, dest, verbose=0):
    """Update local copies of wxWindows contrib files"""
    from distutils.dir_util import mkpath, copy_tree

    mkpath(dest, verbose=verbose)
    copy_tree(src, dest, update=1, verbose=verbose)



class smart_install_data(install_data):
    def run(self):
        #need to change self.install_dir to the actual library dir
        install_cmd = self.get_finalized_command('install')
        self.install_dir = getattr(install_cmd, 'install_lib')
        return install_data.run(self)


def build_locale_dir(destdir, verbose=1):
    """Build a locale dir under the wxPython package for MSW"""
    moFiles = glob.glob(opj(WXDIR, 'locale', '*.mo'))
    for src in moFiles:
        lang = os.path.splitext(os.path.basename(src))[0]
        dest = opj(destdir, lang, 'LC_MESSAGES')
        mkpath(dest, verbose=verbose)
        copy_file(src, opj(dest, 'wxstd.mo'), update=1, verbose=verbose)


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
    else:
        raise NotImplementedError

    return [libname]



def adjustCFLAGS(cflags, defines, includes):
    '''Extrace the raw -I, -D, and -U flags and put them into
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



def adjustLFLAGS(lfags, libdirs, libs):
    '''Extrace the -L and -l flags and put them in libdirs and libs as needed'''
    newLFLAGS = []
    for flag in lflags:
        if flag[:2] == '-L':
            libdirs.append(flag[2:])
        elif flag[:2] == '-l':
            libs.append(flag[2:])
        else:
            newLFLAGS.append(flag)

    return newLFLAGS

#----------------------------------------------------------------------
# sanity checks

if CORE_ONLY:
    BUILD_GLCANVAS = 0
    BUILD_OGL = 0
    BUILD_STC = 0
    BUILD_XRC = 0
    BUILD_GIZMOS = 0
    BUILD_DLLWIDGET = 0
    BUILD_IEWIN = 0

if debug:
    FINAL  = 0
    HYBRID = 0

if FINAL:
    HYBRID = 0

if UNICODE and WXPORT not in ['msw', 'gtk2']:
    raise SystemExit, "UNICODE mode not currently supported on this WXPORT: "+WXPORT


#----------------------------------------------------------------------
# Setup some platform specific stuff
#----------------------------------------------------------------------

if os.name == 'nt':
    # Set compile flags and such for MSVC.  These values are derived
    # from the wxWindows makefiles for MSVC, other compilers settings
    # will probably vary...
    if os.environ.has_key('WXWIN'):
        WXDIR = os.environ['WXWIN']
    else:
        msg("WARNING: WXWIN not set in environment.")
        WXDIR = '..'  # assumes in CVS tree
    WXPLAT = '__WXMSW__'
    GENDIR = 'msw'

    includes = ['src',
                opj(WXDIR, 'lib', 'mswdll' + libFlag()),
                opj(WXDIR, 'include'),
                ]

    defines = [ ('WIN32', None),
                ('__WIN32__', None),
                ('_WINDOWS', None),
                ('__WINDOWS__', None),
                ('WINVER', '0x0400'),
                ('__WIN95__', None),
                ('STRICT', None),

                (WXPLAT, None),
                ('WXUSINGDLL', '1'),

                ('SWIG_GLOBAL', None),
                ('HAVE_CONFIG_H', None),
                ('WXP_USE_THREAD', '1'),
                ]

    if UNDEF_NDEBUG:
        defines.append( ('NDEBUG',) )  # using a 1-tuple makes it do an undef


    if not FINAL or HYBRID:
        defines.append( ('__WXDEBUG__', None) )

    libdirs = [ opj(WXDIR, 'lib') ]
    wxdll = 'wxmsw' + WXDLLVER + libFlag()
    libs = [ wxdll ]

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
    WXDIR = '..'              # assumes IN_CVS_TREE
    includes = ['src']
    defines = [('SWIG_GLOBAL', None),
               ('HAVE_CONFIG_H', None),
               ('WXP_USE_THREAD', '1'),
               ]
    if UNDEF_NDEBUG:
        defines.append( ('NDEBUG',) )  # using a 1-tuple makes it do an undef

    Verify_WX_CONFIG()

    libdirs = []
    libs = []

    cflags = os.popen(WX_CONFIG + ' --cxxflags', 'r').read()[:-1]
    cflags = cflags.split()
    if debug:
        cflags.append('-g')
        cflags.append('-O0')

    lflags = os.popen(WX_CONFIG + ' --libs', 'r').read()[:-1]
    lflags = lflags.split()

    WXBASENAME = os.popen(WX_CONFIG + ' --basename').read()[:-1]
    WXRELEASE  = os.popen(WX_CONFIG + ' --release').read()[:-1]
    WXPREFIX   = os.popen(WX_CONFIG + ' --prefix').read()[:-1]


    if sys.platform[:6] == "darwin":
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
    raise 'Sorry Charlie, platform not supported...'


#----------------------------------------------------------------------
# post platform setup checks and tweaks, create the full version string
#----------------------------------------------------------------------

if UNICODE:
    BUILD_BASE = BUILD_BASE + '.unicode'
    VER_FLAGS += 'u'


VERSION = "%s.%s.%s.%s%s" % (VER_MAJOR, VER_MINOR, VER_RELEASE,
                             VER_SUBREL, VER_FLAGS)

#----------------------------------------------------------------------
# Update the version file
#----------------------------------------------------------------------

# Unconditionally updated since the version string can change based
# on the UNICODE flag
open('src/__version__.py', 'w').write("""\
# This file was generated by setup.py...

wxVERSION_STRING  = '%(VERSION)s'
wxMAJOR_VERSION   = %(VER_MAJOR)s
wxMINOR_VERSION   = %(VER_MINOR)s
wxRELEASE_VERSION = %(VER_RELEASE)s
wxSUBREL_VERSION  = %(VER_SUBREL)s

wxVERSION = (wxMAJOR_VERSION, wxMINOR_VERSION, wxRELEASE_VERSION,
             wxSUBREL_VERSION, '%(VER_FLAGS)s')

wxRELEASE_NUMBER = wxRELEASE_VERSION  # for compatibility
""" % globals())




#----------------------------------------------------------------------
# SWIG defaults
#----------------------------------------------------------------------

swig_force = force
swig_args = ['-c++', '-shadow', '-python', '-keyword',
             '-dnone',
             #'-dascii',
             #'-docstring', '-Sbefore',
             '-I./src', '-D'+WXPLAT,
             ]
if UNICODE:
    swig_args.append('-DwxUSE_UNICODE')

swig_deps = ['src/my_typemaps.i']


#----------------------------------------------------------------------
# Define the CORE extension module
#----------------------------------------------------------------------

msg('Preparing CORE...')
swig_files = [ 'wx.i', 'windows.i', 'windows2.i', 'windows3.i', 'events.i',
               'misc.i', 'misc2.i', 'gdi.i', 'mdi.i', 'controls.i',
               'controls2.i', 'cmndlgs.i', 'stattool.i', 'frames.i', 'image.i',
               'printfw.i', 'sizers.i', 'clip_dnd.i',
               'filesys.i', 'streams.i', 'utils.i', 'fonts.i'
               ]

swig_sources = run_swig(swig_files, 'src', GENDIR, PKGDIR,
                        USE_SWIG, swig_force, swig_args, swig_deps)

copy_file('src/__init__.py', PKGDIR, update=1, verbose=0)
copy_file('src/__version__.py', PKGDIR, update=1, verbose=0)


if IN_CVS_TREE:   # update the license files
    mkpath('licence')
    for file in ['preamble.txt', 'licence.txt', 'licendoc.txt', 'lgpl.txt']:
        copy_file(opj(WXDIR, 'docs', file), opj('licence',file), update=1, verbose=0)


if os.name == 'nt':
    build_locale_dir(opj(PKGDIR, 'locale'))
    DATA_FILES += build_locale_list(opj(PKGDIR, 'locale'))


if os.name == 'nt':
    rc_file = ['src/wxc.rc']
else:
    rc_file = []


ext = Extension('wxc', ['src/helpers.cpp',
                        'src/drawlist.cpp',
                        'src/libpy.c',
                        ] + rc_file + swig_sources,

                include_dirs = includes,
                define_macros = defines,

                library_dirs = libdirs,
                libraries = libs,

                extra_compile_args = cflags,
                extra_link_args = lflags,
                )
wxpExtensions.append(ext)


# Extension for the grid module
swig_sources = run_swig(['grid.i'], 'src', GENDIR, PKGDIR,
                        USE_SWIG, swig_force, swig_args, swig_deps)
ext = Extension('gridc', swig_sources,
                include_dirs =  includes,
                define_macros = defines,
                library_dirs = libdirs,
                libraries = libs,
                extra_compile_args = cflags,
                extra_link_args = lflags,
                )
wxpExtensions.append(ext)


# Extension for the html modules
swig_sources = run_swig(['html.i', 'htmlhelp.i'], 'src', GENDIR, PKGDIR,
                        USE_SWIG, swig_force, swig_args, swig_deps)
ext = Extension('htmlc', swig_sources,
                include_dirs =  includes,
                define_macros = defines,
                library_dirs = libdirs,
                libraries = libs,
                extra_compile_args = cflags,
                extra_link_args = lflags,
                )
wxpExtensions.append(ext)


# Extension for the calendar module
swig_sources = run_swig(['calendar.i'], 'src', GENDIR, PKGDIR,
                        USE_SWIG, swig_force, swig_args, swig_deps)
ext = Extension('calendarc', swig_sources,
                include_dirs =  includes,
                define_macros = defines,
                library_dirs = libdirs,
                libraries = libs,
                extra_compile_args = cflags,
                extra_link_args = lflags,
                )
wxpExtensions.append(ext)


# Extension for the help module
swig_sources = run_swig(['help.i'], 'src', GENDIR, PKGDIR,
                        USE_SWIG, swig_force, swig_args, swig_deps)
ext = Extension('helpc', swig_sources,
                include_dirs =  includes,
                define_macros = defines,
                library_dirs = libdirs,
                libraries = libs,
                extra_compile_args = cflags,
                extra_link_args = lflags,
                )
wxpExtensions.append(ext)


# Extension for the wizard module
swig_sources = run_swig(['wizard.i'], 'src', GENDIR, PKGDIR,
                        USE_SWIG, swig_force, swig_args, swig_deps)
ext = Extension('wizardc', swig_sources,
                include_dirs =  includes,
                define_macros = defines,
                library_dirs = libdirs,
                libraries = libs,
                extra_compile_args = cflags,
                extra_link_args = lflags,
                )
wxpExtensions.append(ext)


#----------------------------------------------------------------------
# Define the GLCanvas extension module
#----------------------------------------------------------------------

if BUILD_GLCANVAS:
    msg('Preparing GLCANVAS...')
    location = 'contrib/glcanvas'
    swig_files = ['glcanvas.i']
    other_sources = []

    swig_sources = run_swig(swig_files, location, GENDIR, PKGDIR,
                            USE_SWIG, swig_force, swig_args, swig_deps)

    gl_libs = []
    if os.name == 'posix':
        gl_config = os.popen(WX_CONFIG + ' --gl-libs', 'r').read()[:-1]
        gl_lflags = gl_config.split() + lflags
        gl_libs = libs
    else:
        other_sources = [opj(location, 'msw/myglcanvas.cpp')]
        gl_libs = libs + ['opengl32', 'glu32']
        gl_lflags = lflags

    ext = Extension('glcanvasc',
                    swig_sources + other_sources,

                    include_dirs = includes,
                    define_macros = defines,

                    library_dirs = libdirs,
                    libraries = gl_libs,

                    extra_compile_args = cflags,
                    extra_link_args = gl_lflags,
                    )

    wxpExtensions.append(ext)


#----------------------------------------------------------------------
# Define the OGL extension module
#----------------------------------------------------------------------

if BUILD_OGL:
    msg('Preparing OGL...')
    location = 'contrib/ogl'

    swig_files = ['ogl.i', 'oglbasic.i', 'oglshapes.i', 'oglshapes2.i',
                  'oglcanvas.i']

    swig_sources = run_swig(swig_files, location, '', PKGDIR,
                            USE_SWIG, swig_force, swig_args, swig_deps)

    ext = Extension('oglc',
                    swig_sources,

                    include_dirs =  includes,
                    define_macros = defines + [('wxUSE_DEPRECATED', '0')],

                    library_dirs = libdirs,
                    libraries = libs + makeLibName('ogl'),

                    extra_compile_args = cflags,
                    extra_link_args = lflags,
                    )

    wxpExtensions.append(ext)



#----------------------------------------------------------------------
# Define the STC extension module
#----------------------------------------------------------------------

if BUILD_STC:
    msg('Preparing STC...')
    location = 'contrib/stc'
    STC_H = opj(WXPREFIX, 'include/wx/stc')

## NOTE: need to add this to the stc.bkl...

##         # Check if gen_iface needs to be run for the wxSTC sources
##         if (newer(opj(CTRB_SRC, 'stc/stc.h.in'),     opj(CTRB_INC, 'stc/stc.h'  )) or
##             newer(opj(CTRB_SRC, 'stc/stc.cpp.in'),   opj(CTRB_SRC, 'stc/stc.cpp')) or
##             newer(opj(CTRB_SRC, 'stc/gen_iface.py'), opj(CTRB_SRC, 'stc/stc.cpp'))):

##             msg('Running gen_iface.py, regenerating stc.h and stc.cpp...')
##             cwd = os.getcwd()
##             os.chdir(opj(CTRB_SRC, 'stc'))
##             sys.path.insert(0, os.curdir)
##             import gen_iface
##             gen_iface.main([])
##             os.chdir(cwd)


    swig_files = ['stc_.i']
    swig_sources = run_swig(swig_files, location, GENDIR, PKGDIR,
                            USE_SWIG, swig_force,
                            swig_args + ['-I'+STC_H, '-I'+location],
                            [opj(STC_H, 'stc.h')] + swig_deps)

    # copy a contrib project specific py module to the main package dir
    copy_file(opj(location, 'stc.py'), PKGDIR, update=1, verbose=0)

    ext = Extension('stc_c',
                    swig_sources,

                    include_dirs = includes,
                    define_macros = defines,

                    library_dirs = libdirs,
                    libraries = libs + makeLibName('stc'),

                    extra_compile_args = cflags,
                    extra_link_args = lflags,
                    )

    wxpExtensions.append(ext)



#----------------------------------------------------------------------
# Define the IEWIN extension module (experimental)
#----------------------------------------------------------------------

if BUILD_IEWIN:
    msg('Preparing IEWIN...')
    location = 'contrib/iewin'

    swig_files = ['iewin.i', ]

    swig_sources = run_swig(swig_files, location, '', PKGDIR,
                            USE_SWIG, swig_force, swig_args, swig_deps)


    ext = Extension('iewinc', ['%s/IEHtmlWin.cpp' % location,
                               '%s/wxactivex.cpp' % location,
                             ] + swig_sources,

                    include_dirs =  includes,
                    define_macros = defines,

                    library_dirs = libdirs,
                    libraries = libs,

                    extra_compile_args = cflags,
                    extra_link_args = lflags,
                    )

    wxpExtensions.append(ext)


#----------------------------------------------------------------------
# Define the XRC extension module
#----------------------------------------------------------------------

if BUILD_XRC:
    msg('Preparing XRC...')
    location = 'contrib/xrc'

    swig_files = ['xrc.i']
    swig_sources = run_swig(swig_files, location, '', PKGDIR,
                            USE_SWIG, swig_force, swig_args, swig_deps)

    ext = Extension('xrcc',
                    swig_sources,

                    include_dirs =  includes,
                    define_macros = defines,

                    library_dirs = libdirs,
                    libraries = libs + makeLibName('xrc'),

                    extra_compile_args = cflags,
                    extra_link_args = lflags,
                    )

    wxpExtensions.append(ext)



#----------------------------------------------------------------------
# Define the GIZMOS  extension module
#----------------------------------------------------------------------

if BUILD_GIZMOS:
    msg('Preparing GIZMOS...')
    location = 'contrib/gizmos'

    swig_files = ['gizmos.i']
    swig_sources = run_swig(swig_files, location, '', PKGDIR,
                            USE_SWIG, swig_force, swig_args, swig_deps)

    ext = Extension('gizmosc',
                    swig_sources,

                    include_dirs =  includes,
                    define_macros = defines,

                    library_dirs = libdirs,
                    libraries = libs + makeLibName('gizmos'),

                    extra_compile_args = cflags,
                    extra_link_args = lflags,
                    )

    wxpExtensions.append(ext)



#----------------------------------------------------------------------
# Define the DLLWIDGET  extension module
#----------------------------------------------------------------------

if BUILD_DLLWIDGET:
    msg('Preparing DLLWIDGET...')
    location = 'contrib/dllwidget'
    swig_files = ['dllwidget_.i']

    swig_sources = run_swig(swig_files, location, '', PKGDIR,
                            USE_SWIG, swig_force, swig_args, swig_deps)

    # copy a contrib project specific py module to the main package dir
    copy_file(opj(location, 'dllwidget.py'), PKGDIR, update=1, verbose=0)

    ext = Extension('dllwidget_c', [
                                '%s/dllwidget.cpp' % location,
                             ] + swig_sources,

                    include_dirs =  includes,
                    define_macros = defines,

                    library_dirs = libdirs,
                    libraries = libs,

                    extra_compile_args = cflags,
                    extra_link_args = lflags,
                    )

    wxpExtensions.append(ext)




#----------------------------------------------------------------------
# Tools and scripts
#----------------------------------------------------------------------

if NO_SCRIPTS:
    SCRIPTS = None
else:
    SCRIPTS = [opj('scripts/helpviewer'),
               opj('scripts/img2png'),
               opj('scripts/img2xpm'),
               opj('scripts/img2py'),
               opj('scripts/xrced'),
               opj('scripts/pyshell'),
               opj('scripts/pycrust'),
               opj('scripts/pywrap'),
               opj('scripts/pywrap'),
               opj('scripts/pyalacarte'),
               opj('scripts/pyalamode'),
               ]


DATA_FILES += find_data_files('wxPython/tools/XRCed', '*.txt', '*.xrc')
DATA_FILES += find_data_files('wxPython/py', '*.txt', '*.ico', '*.css', '*.html')
DATA_FILES += find_data_files('wx', '*.txt', '*.css', '*.html')


#----------------------------------------------------------------------
# Do the Setup/Build/Install/Whatever
#----------------------------------------------------------------------

if __name__ == "__main__":
    if not PREP_ONLY:
        setup(name             = PKGDIR,
              version          = VERSION,
              description      = DESCRIPTION,
              long_description = LONG_DESCRIPTION,
              author           = AUTHOR,
              author_email     = AUTHOR_EMAIL,
              url              = URL,
              license          = LICENSE,

              packages = ['wxPython',
                          'wxPython.lib',
                          'wxPython.lib.colourchooser',
                          'wxPython.lib.editor',
                          'wxPython.lib.mixins',
                          'wxPython.lib.PyCrust',
                          'wxPython.py',
                          'wxPython.py.wxd',
                          'wxPython.tools',
                          'wxPython.tools.XRCed',

                          'wx',
                          'wx.lib',
                          'wx.lib.colourchooser',
                          'wx.lib.editor',
                          'wx.lib.mixins',
                          'wx.py',
                          'wx.tools',
                          'wx.tools.XRCed',
                          ],

              ext_package = PKGDIR,
              ext_modules = wxpExtensions,

              options = { 'build' : { 'build_base' : BUILD_BASE }},

              scripts = SCRIPTS,

              cmdclass = { 'install_data': smart_install_data},
              data_files = DATA_FILES,

              )


#----------------------------------------------------------------------
#----------------------------------------------------------------------
