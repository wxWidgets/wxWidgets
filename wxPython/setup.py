#!/usr/bin/env python
#----------------------------------------------------------------------

import sys, os, string, glob
from distutils.core      import setup, Extension
from distutils.file_util import copy_file
from distutils.dir_util  import mkpath
from distutils.dep_util  import newer

from my_distutils import run_swig, contrib_copy_tree

#----------------------------------------------------------------------
# flags and values that affect this script
#----------------------------------------------------------------------

VERSION          = "2.3.3pre8"
DESCRIPTION      = "Cross platform GUI toolkit for Python"
AUTHOR           = "Robin Dunn"
AUTHOR_EMAIL     = "Robin Dunn <robin@alldunn.com>"
URL              = "http://wxPython.org/"
LICENSE          = "wxWindows (LGPL derivative)"
LONG_DESCRIPTION = """\
wxPython is a GUI toolkit for Python that is a wrapper around the
wxWindows C++ GUI library.  wxPython provides a large variety of
window types and controls, all implemented with a native look and
feel (and native runtime speed) on the platforms it is supported
on.
"""


BUILD_GLCANVAS = 1 # If true, build the contrib/glcanvas extension module
BUILD_OGL = 1      # If true, build the contrib/ogl extension module
BUILD_STC = 1      # If true, build the contrib/stc extension module
BUILD_XRC = 1      # XML based resource system
BUILD_GIZMOS = 1   # Build a module for the gizmos contrib library
BUILD_DLLWIDGET = 1# Build a module that enables unknown wx widgets
                   # to be loaded from a DLL and to be used from Python.

                   # Internet Explorer wrapper (experimental)
BUILD_IEWIN = (os.name == 'nt')

CORE_ONLY = 0      # if true, don't build any of the above

GL_ONLY = 0        # Only used when making the -gl RPM.  See the "b" script
                   # for the ugly details

USE_SWIG = 0       # Should we actually execute SWIG, or just use the
                   # files already in the distribution?

UNICODE = 0        # This will pass the 'wxUSE_UNICODE' flag to SWIG and
                   # will ensure that the right headers are found and the
                   # right libs are linked.

IN_CVS_TREE = 0    # Set to true if building in a full wxWindows CVS
                   # tree, otherwise will assume all needed files are
                   # available in the wxPython source distribution

UNDEF_NDEBUG = 1   # Python 2.2 on Unix/Linux by default defines NDEBUG,
                   # and distutils will pick this up and use it on the
                   # compile command-line for the extensions.  This could
                   # conflict with how wxWindows was built.  If NDEBUG is
                   # set then wxWindows' __WXDEBUG__ setting will be turned
                   # off.  If wxWindows was actually built with it turned
                   # on then you end up with mismatched class structures,
                   # and wxPython will crash.

NO_SCRIPTS = 0     # Don't install the tool scripts


WX_CONFIG = "wx-config"    # Usually you shouldn't need to touch this,
                           # but you can set it to pass an alternate
                           # version of wx-config or alternate flags,
                           # eg. as required by the .deb in-tree build.

BUILD_BASE = "build"

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

WXDLLVER = '233'   # Version part of wxWindows DLL name


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

force = '--force' in sys.argv or '-f' in sys.argv
debug = '--debug' in sys.argv or '-g' in sys.argv

bcpp_compiling = '-c' in sys.argv and 'my_bcpp' in sys.argv # Bad heuristic

if bcpp_compiling:
    msg("Compiling wxPython by Borland C/C++ Compiler")
    HYBRID=0
    WXBCPPLIBVER = string.replace(WXDLLVER,"_","")
    # Version part of BCPP build LIBRARY name
    WXDLLVER="" # no dll ver path avaible


#----------------------------------------------------------------------
# Check for build flags on the command line
#----------------------------------------------------------------------

# Boolean (int) flags
for flag in ['BUILD_GLCANVAS', 'BUILD_OGL', 'BUILD_STC', 'BUILD_XRC',
             'BUILD_GIZMOS', 'BUILD_DLLWIDGET', 'BUILD_IEWIN',
             'CORE_ONLY', 'USE_SWIG', 'IN_CVS_TREE', 'UNICODE',
             'UNDEF_NDEBUG', 'NO_SCRIPTS',
             'FINAL', 'HYBRID', ]:
    for x in range(len(sys.argv)):
        if string.find(sys.argv[x], flag) == 0:
            pos = string.find(sys.argv[x], '=') + 1
            if pos > 0:
                vars()[flag] = eval(sys.argv[x][pos:])
                sys.argv[x] = ''

# String options
for option in ['WX_CONFIG', 'WXDLLVER', 'BUILD_BASE']:
    for x in range(len(sys.argv)):
        if string.find(sys.argv[x], option) == 0:
            pos = string.find(sys.argv[x], '=') + 1
            if pos > 0:
                vars()[option] = sys.argv[x][pos:]
                sys.argv[x] = ''

sys.argv = filter(None, sys.argv)



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


if UNICODE and os.name != 'nt':
    print "UNICODE is currently only supported on Win32"
    sys.exit()


if UNICODE:
    BUILD_BASE = BUILD_BASE + '.unicode'
    VERSION = VERSION + 'u'


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

    if debug:
        FINAL  = 0
        HYBRID = 0

    if HYBRID:
        FINAL = 0

    includes = ['src',
                opj(WXDIR, 'lib', 'mswdll' + libFlag()),
                opj(WXDIR, 'include'),
                ]

    defines = [ ('WIN32', None),        # Some of these are no longer
                ('__WIN32__', None),    # necessary.  Anybody know which?
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

    if bcpp_compiling:  # overwrite it
        defines = [
            ('_WINDOWS', None),
            ('WINVER', '0x0400'),
            ('STRICT', None),

            ('WXUSINGDLL', '1'),

            ('SWIG_GLOBAL', None),
            ('HAVE_CONFIG_H', None),
            ('WXP_USE_THREAD', '1'),

            ('WXUSE_DEFINE','1'),
            ('_RTLDLL',None),
            ]


    if not FINAL or HYBRID:
        defines.append( ('__WXDEBUG__', None) )

    libdirs = [ opj(WXDIR, 'lib') ]
    wxdll = 'wxmsw' + WXDLLVER + libFlag()
    libs = [ wxdll ]

    if bcpp_compiling:
        libs = [ 'wx'+WXBCPPLIBVER ]

    libs = libs + ['kernel32', 'user32', 'gdi32', 'comdlg32',
            'winspool', 'winmm', 'shell32', 'oldnames', 'comctl32',
            'ctl3d32', 'odbc32', 'ole32', 'oleaut32', 'uuid', 'rpcrt4',
            'advapi32', 'wsock32']


    cflags = [ '/Gy',
             # '/GX-'  # workaround for internal compiler error in MSVC on some machines
             ]
    lflags = None


    if bcpp_compiling:  # BCC flags
        cflags = ['-5', '-VF',  ### To support MSVC spurious semicolons in the class scope
                  ### else, all semicolons at the end of all DECLARE_...CALLBACK... macros must be eliminated
                  '-Hc', '-H=' + opj(WXDIR, '\src\msw\wx32.csm'),
                  '@' + opj(WXDIR, '\src\msw\wxwin32.cfg')
                  ]
        if not FINAL:
            cflags = cflags + ['/Od', '/v', '/y']
            lflags = lflags + ['/v', ]

    else:  # MSVC flags
        if FINAL:
            pass #cflags = cflags + ['/O1']
        elif HYBRID :
            pass #cflags = cflags + ['/Ox']
        else:
            pass # cflags = cflags + ['/Od', '/Z7']
                 # lflags = ['/DEBUG', ]




elif os.name == 'posix' and sys.platform[:6] == "darwin":
    # Flags and such for a Darwin (Max OS X) build of Python

    WXDIR = '..'              # assumes IN_CVS_TREE
    WXPLAT = '__WXMAC__'
    GENDIR = 'mac'

    includes = ['src']
    defines = [('SWIG_GLOBAL', None),
               ('HAVE_CONFIG_H', None),
               ('WXP_USE_THREAD', '1'),
               ]
    libdirs = []
    libs = []

    cflags = os.popen(WX_CONFIG + ' --cxxflags', 'r').read()[:-1]
    cflags = string.split(cflags)
    if UNDEF_NDEBUG:
        cflags.append('-UNDEBUG')
    if debug:
        cflags.append('-g')
        cflags.append('-O0')

    lflags = os.popen(WX_CONFIG + ' --libs', 'r').read()[:-1]
    lflags = string.split(lflags)

    NO_SCRIPTS = 1


elif os.name == 'posix':
    # Set flags for Unix type platforms

    WXDIR = '..'              # assumes IN_CVS_TREE
    WXPLAT = '__WXGTK__'      # and assumes GTK...
    GENDIR = 'gtk'            # Need to allow for Motif eventually too

    includes = ['src']
    defines = [('SWIG_GLOBAL', None),
               ('HAVE_CONFIG_H', None),
               ('WXP_USE_THREAD', '1'),
               ]
    libdirs = []
    libs = []

    cflags = os.popen(WX_CONFIG + ' --cxxflags', 'r').read()[:-1] + ' ' + \
             os.popen('gtk-config --cflags', 'r').read()[:-1]
    cflags = string.split(cflags)
    if UNDEF_NDEBUG:
        cflags.append('-UNDEBUG')
    if debug:
        cflags.append('-g')
        cflags.append('-O0')

    lflags = os.popen(WX_CONFIG + ' --libs', 'r').read()[:-1]
    lflags = string.split(lflags)


else:
    raise 'Sorry Charlie...'


#----------------------------------------------------------------------
# Check if the version file needs updated
#----------------------------------------------------------------------

#if IN_CVS_TREE and newer('setup.py', 'src/__version__.py'):
open('src/__version__.py', 'w').write("ver = '%s'\n" % VERSION)



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

if not GL_ONLY:
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
    copy_file('src/wxc.pyd.manifest', PKGDIR, update=1, verbose=0)

    if IN_CVS_TREE:   # update the license files
        mkpath('licence')
        for file in ['preamble.txt', 'licence.txt', 'licendoc.txt', 'lgpl.txt']:
            copy_file(opj(WXDIR, 'docs', file), opj('licence',file), update=1, verbose=0)


    if os.name == 'nt':
        rc_file = ['src/wxc.rc']
    else:
        rc_file = []


    ext = Extension('wxc', ['src/helpers.cpp',
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

CTRB_SRC =  opj(WXDIR, 'contrib/src')
CTRB_INC =  opj(WXDIR, 'contrib/include/wx')

if BUILD_GLCANVAS or GL_ONLY:
    msg('Preparing GLCANVAS...')
    location = 'contrib/glcanvas'
    swig_files = ['glcanvas.i']
    other_sources = []

    swig_sources = run_swig(swig_files, location, GENDIR, PKGDIR,
                            USE_SWIG, swig_force, swig_args, swig_deps)

    gl_libs = []
    if os.name == 'posix':
        gl_config = os.popen(WX_CONFIG + ' --gl-libs', 'r').read()[:-1]
        gl_lflags = string.split(gl_config) + lflags
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

if not GL_ONLY and BUILD_OGL:
    msg('Preparing OGL...')
    location = 'contrib/ogl'
    OGLLOC = opj(location, 'contrib/src/ogl')
    OGLINC = opj(location, 'contrib/include')

    swig_files = ['ogl.i', 'oglbasic.i', 'oglshapes.i', 'oglshapes2.i',
                  'oglcanvas.i']

    swig_sources = run_swig(swig_files, location, '', PKGDIR,
                            USE_SWIG, swig_force, swig_args, swig_deps)

    if IN_CVS_TREE:
        # make sure local copy of contrib files are up to date
        contrib_copy_tree(opj(CTRB_INC, 'ogl'), opj(OGLINC, 'wx/ogl'))
        contrib_copy_tree(opj(CTRB_SRC, 'ogl'), OGLLOC)

    ext = Extension('oglc', ['%s/basic.cpp' % OGLLOC,
                             '%s/bmpshape.cpp' % OGLLOC,
                             '%s/composit.cpp' % OGLLOC,
                             '%s/divided.cpp' % OGLLOC,
                             '%s/lines.cpp' % OGLLOC,
                             '%s/misc.cpp' % OGLLOC,
                             '%s/basic2.cpp' % OGLLOC,
                             '%s/canvas.cpp' % OGLLOC,
                             '%s/constrnt.cpp' % OGLLOC,
                             '%s/drawn.cpp' % OGLLOC,
                             '%s/mfutils.cpp' % OGLLOC,
                             '%s/ogldiag.cpp' % OGLLOC,
                             ] + swig_sources,

                    include_dirs =  [OGLINC] + includes,
                    define_macros = defines,

                    library_dirs = libdirs,
                    libraries = libs,

                    extra_compile_args = cflags,
                    extra_link_args = lflags,
                    )

    wxpExtensions.append(ext)



#----------------------------------------------------------------------
# Define the STC extension module
#----------------------------------------------------------------------

if not GL_ONLY and BUILD_STC:
    msg('Preparing STC...')
    location = 'contrib/stc'
    STCLOC = opj(location, 'contrib/src/stc')
    STCINC = opj(location, 'contrib/include')
    STC_H =  opj(location, 'contrib/include/wx/stc')

    if IN_CVS_TREE:
        # Check if gen_iface needs to be run for the wxSTC sources
        if (newer(opj(CTRB_SRC, 'stc/stc.h.in'),     opj(CTRB_INC, 'stc/stc.h'  )) or
            newer(opj(CTRB_SRC, 'stc/stc.cpp.in'),   opj(CTRB_SRC, 'stc/stc.cpp')) or
            newer(opj(CTRB_SRC, 'stc/gen_iface.py'), opj(CTRB_SRC, 'stc/stc.cpp'))):

            msg('Running gen_iface.py, regenerating stc.h and stc.cpp...')
            cwd = os.getcwd()
            os.chdir(opj(CTRB_SRC, 'stc'))
            import gen_iface
            gen_iface.main([])
            os.chdir(cwd)


        # make sure local copy of contrib files are up to date
        contrib_copy_tree(opj(CTRB_INC, 'stc'), opj(STCINC, 'wx/stc'))
        contrib_copy_tree(opj(CTRB_SRC, 'stc'), STCLOC)



    swig_files = ['stc_.i']
    swig_sources = run_swig(swig_files, location, GENDIR, PKGDIR,
                            USE_SWIG, swig_force,
                            swig_args + ['-I'+STC_H, '-I'+location],
                            [opj(STC_H, 'stc.h')] + swig_deps)

    # copy a contrib project specific py module to the main package dir
    copy_file(opj(location, 'stc.py'), PKGDIR, update=1, verbose=0)

    # add some include dirs to the standard set
    stc_includes = includes[:]
    stc_includes.append('%s/scintilla/include' % STCLOC)
    stc_includes.append('%s/scintilla/src' % STCLOC)
    stc_includes.append(STCINC)

    # and some macro definitions
    stc_defines = defines[:]
    stc_defines.append( ('__WX__', None) )
    stc_defines.append( ('SCI_LEXER', None) )
    stc_defines.append( ('LINK_LEXERS', None) )


    ext = Extension('stc_c',
                    ['%s/scintilla/src/AutoComplete.cxx' % STCLOC,
                     '%s/scintilla/src/CallTip.cxx' % STCLOC,
                     '%s/scintilla/src/CellBuffer.cxx' % STCLOC,
                     '%s/scintilla/src/ContractionState.cxx' % STCLOC,
                     '%s/scintilla/src/Document.cxx' % STCLOC,
                     '%s/scintilla/src/DocumentAccessor.cxx' % STCLOC,
                     '%s/scintilla/src/Editor.cxx' % STCLOC,
                     '%s/scintilla/src/Indicator.cxx' % STCLOC,
                     '%s/scintilla/src/KeyMap.cxx' % STCLOC,
                     '%s/scintilla/src/KeyWords.cxx' % STCLOC,
                     '%s/scintilla/src/LineMarker.cxx' % STCLOC,
                     '%s/scintilla/src/PropSet.cxx' % STCLOC,
                     '%s/scintilla/src/RESearch.cxx' % STCLOC,
                     '%s/scintilla/src/ScintillaBase.cxx' % STCLOC,
                     '%s/scintilla/src/Style.cxx' % STCLOC,
                     '%s/scintilla/src/StyleContext.cxx' % STCLOC,
                     '%s/scintilla/src/UniConversion.cxx' % STCLOC,
                     '%s/scintilla/src/ViewStyle.cxx' % STCLOC,
                     '%s/scintilla/src/WindowAccessor.cxx' % STCLOC,

                     '%s/scintilla/src/LexAda.cxx' % STCLOC,
                     '%s/scintilla/src/LexAVE.cxx' % STCLOC,
                     '%s/scintilla/src/LexBaan.cxx' % STCLOC,
                     '%s/scintilla/src/LexBullant.cxx' % STCLOC,
                     '%s/scintilla/src/LexCPP.cxx' % STCLOC,
                     '%s/scintilla/src/LexConf.cxx' % STCLOC,
                     '%s/scintilla/src/LexCrontab.cxx' % STCLOC,
                     '%s/scintilla/src/LexEiffel.cxx' % STCLOC,
                     '%s/scintilla/src/LexHTML.cxx' % STCLOC,
                     '%s/scintilla/src/LexLisp.cxx' % STCLOC,
                     '%s/scintilla/src/LexLua.cxx' % STCLOC,
                     '%s/scintilla/src/LexMatlab.cxx' % STCLOC,
                     '%s/scintilla/src/LexOthers.cxx' % STCLOC,
                     '%s/scintilla/src/LexPascal.cxx' % STCLOC,
                     '%s/scintilla/src/LexPerl.cxx' % STCLOC,
                     '%s/scintilla/src/LexPython.cxx' % STCLOC,
                     '%s/scintilla/src/LexRuby.cxx' % STCLOC,
                     '%s/scintilla/src/LexSQL.cxx' % STCLOC,
                     '%s/scintilla/src/LexVB.cxx' % STCLOC,

                     '%s/PlatWX.cpp' % STCLOC,
                     '%s/ScintillaWX.cpp' % STCLOC,
                     '%s/stc.cpp' % STCLOC,
                     ] + swig_sources,

                    include_dirs = stc_includes,
                    define_macros = stc_defines,

                    library_dirs = libdirs,
                    libraries = libs,

                    extra_compile_args = cflags,
                    extra_link_args = lflags,
                    )

    wxpExtensions.append(ext)



#----------------------------------------------------------------------
# Define the IEWIN extension module (experimental)
#----------------------------------------------------------------------

if not GL_ONLY and BUILD_IEWIN:
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

if not GL_ONLY and BUILD_XRC:
    msg('Preparing XRC...')
    location = 'contrib/xrc'
    XMLLOC = opj(location, 'contrib/src/xrc')
    XMLINC = opj(location, 'contrib/include')

    swig_files = ['xrc.i']

    swig_sources = run_swig(swig_files, location, '', PKGDIR,
                            USE_SWIG, swig_force, swig_args, swig_deps)

    xmlres_includes = includes[:]
    xmlres_includes.append('%s/expat/xmlparse' % XMLLOC)
    xmlres_includes.append('%s/expat/xmltok' % XMLLOC)
    xmlres_includes.append(XMLINC)


    # make sure local copy of contrib files are up to date
    if IN_CVS_TREE:
        contrib_copy_tree(opj(CTRB_INC, 'xrc'), opj(XMLINC, 'wx/xrc'))
        contrib_copy_tree(opj(CTRB_SRC, 'xrc'), XMLLOC)

    ext = Extension('xrcc', ['%s/expat/xmlparse/xmlparse.c' % XMLLOC,
                                '%s/expat/xmltok/xmlrole.c' % XMLLOC,
                                '%s/expat/xmltok/xmltok.c' % XMLLOC,

                                '%s/xh_bmp.cpp' % XMLLOC,
                                '%s/xh_bmpbt.cpp' % XMLLOC,
                                '%s/xh_bttn.cpp' % XMLLOC,
                                '%s/xh_cald.cpp' % XMLLOC,
                                '%s/xh_chckb.cpp' % XMLLOC,

                                '%s/xh_chckl.cpp' % XMLLOC,
                                '%s/xh_choic.cpp' % XMLLOC,
                                '%s/xh_combo.cpp' % XMLLOC,
                                '%s/xh_dlg.cpp' % XMLLOC,
                                '%s/xh_frame.cpp' % XMLLOC,

                                '%s/xh_gauge.cpp' % XMLLOC,
                                '%s/xh_gdctl.cpp' % XMLLOC,
                                '%s/xh_html.cpp' % XMLLOC,
                                '%s/xh_listb.cpp' % XMLLOC,
                                '%s/xh_listc.cpp' % XMLLOC,
                                '%s/xh_menu.cpp' % XMLLOC,

                                '%s/xh_notbk.cpp' % XMLLOC,
                                '%s/xh_panel.cpp' % XMLLOC,
                                '%s/xh_radbt.cpp' % XMLLOC,
                                '%s/xh_radbx.cpp' % XMLLOC,
                                '%s/xh_scrol.cpp' % XMLLOC,

                                '%s/xh_sizer.cpp' % XMLLOC,
                                '%s/xh_slidr.cpp' % XMLLOC,
                                '%s/xh_spin.cpp' % XMLLOC,
                                '%s/xh_stbmp.cpp' % XMLLOC,
                                '%s/xh_stbox.cpp' % XMLLOC,

                                '%s/xh_stlin.cpp' % XMLLOC,
                                '%s/xh_sttxt.cpp' % XMLLOC,
                                '%s/xh_text.cpp' % XMLLOC,
                                '%s/xh_toolb.cpp' % XMLLOC,
                                '%s/xh_tree.cpp' % XMLLOC,

                                '%s/xh_unkwn.cpp' % XMLLOC,
                                '%s/xml.cpp' % XMLLOC,
                                '%s/xmlres.cpp' % XMLLOC,
                                '%s/xmlrsall.cpp' % XMLLOC,

                             ] + swig_sources,

                    include_dirs =  xmlres_includes,
                    define_macros = defines,

                    library_dirs = libdirs,
                    libraries = libs,

                    extra_compile_args = cflags,
                    extra_link_args = lflags,
                    )

    wxpExtensions.append(ext)



#----------------------------------------------------------------------
# Define the GIZMOS  extension module
#----------------------------------------------------------------------

if not GL_ONLY and BUILD_GIZMOS:
    msg('Preparing GIZMOS...')
    location = 'contrib/gizmos'
    GIZMOLOC = opj(location, 'contrib/src/gizmos')
    GIZMOINC = opj(location, 'contrib/include')

    swig_files = ['gizmos.i']

    swig_sources = run_swig(swig_files, location, '', PKGDIR,
                            USE_SWIG, swig_force, swig_args, swig_deps)

    gizmos_includes = includes[:]
    gizmos_includes.append(GIZMOINC)


    # make sure local copy of contrib files are up to date
    if IN_CVS_TREE:
        contrib_copy_tree(opj(CTRB_INC, 'gizmos'), opj(GIZMOINC, 'wx/gizmos'))
        contrib_copy_tree(opj(CTRB_SRC, 'gizmos'), GIZMOLOC)

    ext = Extension('gizmosc', [
                                '%s/dynamicsash.cpp' % GIZMOLOC,
                                '%s/editlbox.cpp' % GIZMOLOC,
                                #'%s/multicell.cpp' % GIZMOLOC,
                                '%s/splittree.cpp' % GIZMOLOC,
                                '%s/ledctrl.cpp'   % GIZMOLOC,
                             ] + swig_sources,

                    include_dirs =  gizmos_includes,
                    define_macros = defines,

                    library_dirs = libdirs,
                    libraries = libs,

                    extra_compile_args = cflags,
                    extra_link_args = lflags,
                    )

    wxpExtensions.append(ext)



#----------------------------------------------------------------------
# Define the DLLWIDGET  extension module
#----------------------------------------------------------------------

if not GL_ONLY and BUILD_DLLWIDGET:
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

## TOOLS = [("wxPython/tools",        glob.glob("tools/*.py")),
##          ("wxPython/tools/XRCed",  glob.glob("tools/XRCed/*.py") +
##                                    glob.glob("tools/XRCed/*.xrc") +
##                                    ["tools/XRCed/CHANGES",
##                                     "tools/XRCed/TODO",
##                                     "tools/XRCed/README"]),
##          ]


if NO_SCRIPTS:
    SCRIPTS = None
else:
    SCRIPTS = [opj('scripts/img2png'),
               opj('scripts/img2xpm'),
               opj('scripts/img2py'),
               opj('scripts/xrced'),
               opj('scripts/pyshell'),
               opj('scripts/pycrust'),
               ]


#----------------------------------------------------------------------
# Do the Setup/Build/Install/Whatever
#----------------------------------------------------------------------

if __name__ == "__main__":
    if not GL_ONLY:
        setup(name             = PKGDIR,
              version          = VERSION,
              description      = DESCRIPTION,
              long_description = LONG_DESCRIPTION,
              author           = AUTHOR,
              author_email     = AUTHOR_EMAIL,
              url              = URL,
              license          = LICENSE,

              packages = [PKGDIR,
                          PKGDIR+'.lib',
                          PKGDIR+'.lib.editor',
                          PKGDIR+'.lib.mixins',
                          PKGDIR+'.lib.PyCrust',
                          PKGDIR+'.tools',
                          PKGDIR+'.tools.XRCed',
                          ],

              ext_package = PKGDIR,
              ext_modules = wxpExtensions,

              options = { 'build' : { 'build_base' : BUILD_BASE }},

              ##data_files = TOOLS,
              scripts = SCRIPTS,
              )

    else:

        setup(name             = "wxPython-gl",
              version          = VERSION,
              description      = "wxGLCanvas class for wxPython",
              author           = AUTHOR,
              author_email     = AUTHOR_EMAIL,
              url              = URL,
              license          = LICENSE,

              py_modules = [ "wxPython.glcanvas" ],

              ext_package = PKGDIR,
              ext_modules = wxpExtensions,

              )




#----------------------------------------------------------------------
#----------------------------------------------------------------------
