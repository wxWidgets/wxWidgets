#!/usr/bin/env python
#----------------------------------------------------------------------------
# Name:         build.py
# Purpose:	This script is used to build wxPython.  It reads a build
#               configuration file in the requested project directory and
#               based on the contents of the file can build Makefiles for
#               unix or win32, and can execute make with various options
#               potentially automating the entire build/install/clean process
#               from a single command.
#
# Author:       Robin Dunn
#
# Created:      18-Aug-1999
# RCS-ID:       $Id$
# Copyright:    (c) 1999 by Total Control Software
# Licence:      wxWindows license
#----------------------------------------------------------------------------
"""
build.py

    This script is used to build wxPython.  It reads a build configuration
    file in the requested project directory and based on the contents of
    the file can build Makefiles for unix or win32, and can execute make
    with various options potentially automating the entire
    build/install/clean process from a single command.

    The default action is to build the Makefile and exit.

Options
    -C dir        CD to dir before doing anything
    -B file       Use file as the build configuration (default ./build.cfg)
    -M file       Use file as the name of the makefile to create
                  (default Makefile)

    -b            Build the module (runs make)
    -i            Install the module (runs make install)
    -c            Cleanup (runs make clean)
    -u            Uninstall (runs make uninstall)

    -h            Show help and exit


Configuration Files

    The build configuration file lists targets, source files and options
    for the the build process.  The contents of the build.cfg are used to
    dynamically generate the Makefile.

    To prevent you from getting screwed when the default build.cfg is
    updated, you can override the values in build.cfg by putting your
    custom definitions in a file named build.local.  You can also place a
    build.local file in the parent directory, or even in the grandparent
    directory for project-wide overrides.  Finally, command-line arguments
    of the form NAME=VALUE can also be used to override simple configuration
    values.  The order of evaluation is:

        0. comman-line flags (-M, -b, etc.)
        1. ./build.cfg
        2. ../../build.local   (if present)
        3. ../build.local      (if present)
        4. ./build.local       (if present)
        5. command-line NAME=VALUEs

    The config files are actually just Python files that get exec'ed in a
    separate namespace which is then used later as a configuration object.
    This keeps the build script simple in that it doesn't have to parse
    anything, and the config files can be much more than just names and
    values as pretty much any python code can be executed.  The global
    variables set in the config namespace are what are used later as
    configuation values.


Configuration Options

    The following variables can be set in the config files.  Only a few are
    required, the rest will either have suitable defaults or will be
    calculated from your current Python runtime environment.

    MODULE           The name of the extension module to produce
    SWIGFILES        A list of files that should be run through SWIG
    SWIGFLAGS        Flags for SWIG
    SOURCES          Other C/C++ sources that should be part of the module
    PYFILES          Other Python files that should be installed with the module
    CFLAGS           Flags to be used by the compiler
    LFLAGS           Flags to be used at the link step
    LIBS             Libraries to be linked with

    OTHERCFLAGS      Extra flags to append to CFLAGS
    OTHERLFLAGS      Extra flags to append to LFLAGS
    OTHERSWIGFLAGS   Extra flags to append to SWIGFLAGS
    OTHERLIBS        Other libraries to be linked with, in addition to LIBS
    OTHERTARGETS     Other targets to be placed on the default rule line
    OTHERINSTALLTARGETS
                     Other targets to be placed on the install rule line
    OTHERDEFS        Text to place near the begining of the Makefile
    OTHERRULES       This text is placed at the end of the makefile and
                     will typically be used for adding rules and such
    DEFAULTRULE      Text to be used for the default rule in the makefile

    TARGETDIR        Destination for the install step

    MAKE             The make program to use
    MAKEFILE         The name of the makefile

    runBuild         Setting this to 1 is eqivalent to the -b flag
    runInstall       Setting this to 1 is eqivalent to the -i flag
    runClean         Setting this to 1 is eqivalent to the -c flag
    runUninstall     Setting this to 1 is eqivalent to the -u flag

    PYVERSION        Version number of Python used in pathnames
    PYPREFIX         The root of the Python install
    EXECPREFIX       The root of the Python install for binary files
    PYTHONLIB        The Python link library

"""

import sys, os, string, getopt

#----------------------------------------------------------------------------
# This is really the wxPython version number, and will be placed in the
# Makefiles for use with the distribution related targets.

major_version = '2.2'
build_version = '1'

__version__ = major_version + '.' + build_version

#----------------------------------------------------------------------------

def main(args):
    try:
        opts, args = getopt.getopt(args[1:], 'C:B:M:bicu')
    except getopt.error:
        usage()
        sys.exit(1)

    if not os.environ.has_key('WXWIN'):
        print "WARNING: WXWIN is not set in the environment.  WXDIR may not\n"\
              "         be set properly in the makefile, you will have to \n"\
              "         set the environment variable or override in build.local."

    bldCfg       = 'build.cfg'
    bldCfgLocal  = 'build.local'
    MAKEFILE     = 'Makefile'
    runBuild     = 0
    runInstall   = 0
    runClean     = 0
    runUninstall = 0

    for flag, value in opts:
        if   flag == '-C':   os.chdir(value)
        elif flag == '-B':   bldCfgFile = value
        elif flag == '-M':   makefile = value
        elif flag == '-b':   runBuild = 1
        elif flag == '-c':   runClean = 1
        elif flag == '-i':   runInstall = 1
        elif flag == '-u':   runUninstall = 1

        elif flag == '-h':   usage(); sys.exit(1)
        else:  usage(); sys.exit(1)

    config = BuildConfig(bldCfg = bldCfg,
                         bldCfgLocal = bldCfgLocal,
                         MAKEFILE = MAKEFILE,
                         runBuild = runBuild,
                         runInstall =  runInstall,
                         runClean = runClean,
                         runUninstall = runUninstall)

    err = 0
    if config.readConfigFiles(args):
        config.doFixups()
        config.makeMakefile()

        if config.runBuild:
            cmd = "%s -f %s" % (config.MAKE, config.MAKEFILE)
            print "Running:", cmd
            err = os.system(cmd)

        if not err and config.runInstall:
            cmd = "%s -f %s install" % (config.MAKE, config.MAKEFILE)
            print "Running:", cmd
            err = os.system(cmd)


        if not err and config.runClean:
            cmd = "%s -f %s clean" % (config.MAKE, config.MAKEFILE)
            print "Running:", cmd
            err = os.system(cmd)

        if not err and config.runUninstall:
            cmd = "%s -f %s uninstall" % (config.MAKE, config.MAKEFILE)
            print "Running:", cmd
            err = os.system(cmd)

    return err/256


#----------------------------------------------------------------------------

def usage():
    print __doc__

#----------------------------------------------------------------------------

def swapslash(st):
    if sys.platform != 'win32':
        st = string.join(string.split(st, '\\'), '/')
    return st

#----------------------------------------------------------------------------

def splitlines(st):
    return string.join(string.split(string.strip(st), ' '), ' \\\n\t')

#----------------------------------------------------------------------------

class BuildConfig:
    def __init__(self, **kw):
        self.__dict__.update(kw)
        self.setDefaults()

    #------------------------------------------------------------
    def setDefaults(self):
        base = os.path.split(sys.argv[0])[0]
        base = os.path.join(base, '..')
        self.WXPYDIR = os.path.abspath(base)
        self.VERSION = __version__
        self.MAJVER = major_version
        self.BLDVER = build_version
        self.MODULE = ''
        self.SWIGFILES = []
        self.SWIGFLAGS = '-c++ -shadow -python -keyword -dnone -I$(WXPSRCDIR)'
        self.SOURCES = []
        self.PYFILES = []
        self.LFLAGS = ''
        self.OTHERCFLAGS = ''
        self.OTHERLFLAGS = ''
        self.OTHERSWIGFLAGS = ''
        self.OTHERLIBS = ''
        self.OTHERTARGETS = ''
        self.OTHERINSTALLTARGETS = ''
        self.OTHERUNINSTALLTARGETS = ''
        self.OTHERRULES = ''
        self.DEFAULTRULE = 'default: $(GENCODEDIR) $(TARGET) $(BUILDDIR)/$(TARGET) bldpycfiles'
        self.PYVERSION = sys.version[:3]
        self.PYPREFIX  = sys.prefix
        self.EXECPREFIX = sys.exec_prefix
        self.WXDIR = '$(WXWIN)'
        self.FINAL = '0'
        self.WXP_USE_THREAD = '1'
        self.WXUSINGDLL = '1'
        self.OTHERDEP = ''
        self.WXPSRCDIR = '$(WXPYDIR)/src'
        self.SWIGDEPS = ''
        self.OTHERDEPS = ''
        self.OTHERDEFS = ''


        if sys.platform == 'win32':
            self.MAKE = 'nmake'
            compactver = string.join(string.split(self.PYVERSION, '.'), '')
            self.PYTHONLIB = '$(PYPREFIX)\\libs\\python%s.lib' % compactver
            self.TARGETDIR = '$(PYPREFIX)\\wxPython'
            self.LIBS = '$(PYTHONLIB) $(WXPSRCDIR)\wxc.lib'
            self.GENCODEDIR = 'msw'
            self.SWIGTOOLKITFLAG = '-D__WXMSW__'
            self.OBJEXT = '.obj'
            self.TARGET = '$(MODULE).pyd'
            self.CFLAGS = '-I$(PYPREFIX)\include -I$(WXPSRCDIR) -I. /Fp$(MODULE).pch /YXhelpers.h -DSWIG_GLOBAL -DHAVE_CONFIG_H $(THREAD) '
            self.LFLAGS = '$(DEBUGLFLAGS) /DLL /subsystem:windows,3.50 /machine:I386 /nologo'
            self.RESFILE = ''
            self.RESRULE = ''
            self.OVERRIDEFLAGS = '/GX-'
            self.RMCMD  = '-erase '
            self.WXPSRCDIR = os.path.normpath(self.WXPSRCDIR)
            self.CRTFLAG = ''


        else:
            self.MAKE      = 'make'
            self.PYLIB     = '$(EXECPREFIX)/lib/python$(PYVERSION)'
            self.LIBPL     = '$(PYLIB)/config'
            self.PYTHONLIB = '$(LIBPL)/libpython$(PYVERSION).a'
            self.TARGETDIR = '$(EXECPREFIX)/lib/python$(PYVERSION)/site-packages/wxPython'
            self.TARGET = '$(MODULE)module$(SO)'
            self.OBJEXT = '.o'
            self.HELPERLIB = 'wxPyHelpers'
            self.HELPERLIBDIR = '$(EXECPREFIX)/lib'
            self.CFLAGS = '-DSWIG_GLOBAL -DHAVE_CONFIG_H $(THREAD) -I. '\
                          '`$(WXCONFIG) --cflags` -I$(PYINCLUDE) -I$(EXECINCLUDE) '\
                          '-I$(WXPSRCDIR)'
            self.LFLAGS = '-L$(WXPSRCDIR) `$(WXCONFIG) --libs`'
            self.RMCMD  = '-rm -f '
            self.WXCONFIG = 'wx-config'
            self.USE_SONAME = '1'

            # **** What to do when I start supporting Motif, etc.???
            self.GENCODEDIR = 'gtk'
            self.SWIGTOOLKITFLAG = '-D__WXGTK__'

            # Extract a few things from Python's Makefile...
            try:
                filename = os.path.join(self.EXECPREFIX,
                                        'lib/python'+self.PYVERSION,
                                        'config/Makefile')
                mfText = string.split(open(filename, 'r').read(), '\n')
            except IOError:
                raise SystemExit, "Python development files not found"

            self.CCC = self.findMFValue(mfText, 'CCC')
            self.CC = self.findMFValue(mfText, 'CC')
            self.OPT = self.findMFValue(mfText, 'OPT')
            self.SO = self.findMFValue(mfText, 'SO')
            self.LDSHARED = self.findMFValue(mfText, 'LDSHARED')
            self.CCSHARED = self.findMFValue(mfText, 'CCSHARED')
            #self.LINKFORSHARED = self.findMFValue(mfText, 'LINKFORSHARED')
            #self. = self.findMFValue(mfText, '')
            #self. = self.findMFValue(mfText, '')


            # The majority of cases will require LDSHARED to be
            # modified to use the C++ driver instead of the C driver
            # for linking.  We'll try to do it here and if we goof up
            # then the user can correct it in their build.local file.
            self.LDSHARED = string.join(['$(CCC)'] +
                                        string.split(self.LDSHARED, ' ')[1:],
                                        ' ')


    #------------------------------------------------------------
    def doFixups(self):
        # This is called after the config files have been evaluated
        # so we can do some sanity checking...
        if sys.platform != 'win32':
	    if not self.CCC:
                self.CCC = os.popen('%(WXCONFIG)s --cxx' % self.__dict__, 'r').read()[:-1]
                if not self.CCC:
                    print "Warning: C++ compiler not specified (CCC). Assuming c++"
                    self.CCC = 'c++'
            if not self.CC:
                self.CCC = os.popen('%(WXCONFIG)s --cc' % self.__dict__, 'r').read()[:-1]
                if not self.CC:
                    print "Warning: C compiler not specified (CC). Assuming cc"
                    self.CC = 'cc'

    #------------------------------------------------------------
    def findMFValue(self, mfText, st):
        # Find line begining with st= and return the value
        # Regex would probably be to cooler way to do this, but
        # I think this is the most understandable.
        for line in mfText:
            if string.find(line, st+'=') == 0:
                st = string.strip(line[len(st)+1:])
                return st
        return None

    #------------------------------------------------------------
    def makeMakefile(self):

        # make a list of object file names
        objects = ""
        for name in self.SWIGFILES:
            objects = objects + os.path.splitext(name)[0] + self.OBJEXT + ' '
        for name in self.SOURCES:
	    obj = os.path.basename(name)
            objects = objects + os.path.splitext(obj)[0] + self.OBJEXT + ' '
        self.OBJECTS = splitlines(objects)


        # now build the text for the dependencies
        depends = ""
        for name in self.SWIGFILES:
            rootname = os.path.splitext(name)[0]
            text = '$(GENCODEDIR)/%s.cpp $(GENCODEDIR)/%s.py : %s.i %s\n' \
                   '$(TARGETDIR)\\%s.py : $(GENCODEDIR)\\%s.py\n' \
                   '$(BUILDDIR)\\%s.py : $(GENCODEDIR)\\%s.py\n' % \
                   (rootname, rootname, rootname, self.SWIGDEPS,
                    rootname, rootname, rootname, rootname)
            depends = depends + text
            if self.OTHERDEPS:
                text = '%s%s : %s\n' % \
                       (os.path.splitext(name)[0], self.OBJEXT, self.OTHERDEPS)
                depends = depends + text
        for name in self.PYFILES:
            text = '$(TARGETDIR)\\%s.py : %s.py\n' % \
                   tuple([os.path.splitext(name)[0]] * 2)
            depends = depends + text
        if self.OTHERDEPS:
            for name in self.SOURCES:
                name = os.path.basename(name)
                text = '%s%s : %s\n' % \
                       (os.path.splitext(name)[0], self.OBJEXT, self.OTHERDEPS)
                depends = depends + text

        self.DEPENDS = swapslash(depends)


        # and the list of .py files
        pymodules = ""
        bldpymodules = ""
        for name in self.SWIGFILES:
            pymodules = pymodules + '$(TARGETDIR)\\%s.py ' % os.path.splitext(name)[0]
            bldpymodules = bldpymodules + '$(BUILDDIR)\\%s.py ' % os.path.splitext(name)[0]
        for name in self.PYFILES:
            pymodules = pymodules + '$(TARGETDIR)\\%s.py ' % os.path.splitext(name)[0]
            bldpymodules = bldpymodules + '$(BUILDDIR)\\%s.py ' % os.path.splitext(name)[0]
        self.PYMODULES = splitlines(swapslash(pymodules))
        self.BLDPYMODULES = splitlines(swapslash(bldpymodules))


        # now make a list of the python files that would need cleaned up
        pycleanup = ""
        for name in self.SWIGFILES:
            pycleanup = pycleanup + self.makeCleanupList(name)
        for name in self.PYFILES:
            pycleanup = pycleanup + self.makeCleanupList(name)
        self.PYCLEANUP = swapslash(pycleanup)


        # now make a list of the python files that would need uninstalled
        pyUninstall = ""
        for name in self.SWIGFILES:
            pyUninstall = pyUninstall + self.makeUninstallList(name)
        for name in self.PYFILES:
            pyUninstall = pyUninstall + self.makeUninstallList(name)
        self.PYUNINSTALL = swapslash(pyUninstall)


        # finally, build the makefile
        if sys.platform == 'win32':
            if self.RESFILE:
                self.RESFILE = '$(MODULE).res'
                self.RESRULE = '$(MODULE).res : $(MODULE).rc $(WXDIR)\\include\\wx\\msw\\wx.rc\n\t'\
                               '$(rc) -r /i$(WXDIR)\\include -fo$@ $(MODULE).rc'
            text = win32Template % self.__dict__
        else:
            text = unixTemplate % self.__dict__
        f = open(self.MAKEFILE, 'w')
        f.write(text)
        f.close()

        print "Makefile created: ", self.MAKEFILE



    #------------------------------------------------------------
    def makeUninstallList(self, name):
        st = ""
        st = st + '\t%s$(TARGETDIR)\\%s.py\n' % (self.RMCMD, os.path.splitext(name)[0])
        st = st + '\t%s$(TARGETDIR)\\%s.pyc\n' % (self.RMCMD, os.path.splitext(name)[0])
        st = st + '\t%s$(TARGETDIR)\\%s.pyo\n' % (self.RMCMD, os.path.splitext(name)[0])
        return st


    #------------------------------------------------------------
    def makeCleanupList(self, name):
        st = ""
        st = st + '\t%s$(BUILDDIR)\\%s.py\n' % (self.RMCMD, os.path.splitext(name)[0])
        st = st + '\t%s$(BUILDDIR)\\%s.pyc\n' % (self.RMCMD, os.path.splitext(name)[0])
        st = st + '\t%s$(BUILDDIR)\\%s.pyo\n' % (self.RMCMD, os.path.splitext(name)[0])
        return st


    #------------------------------------------------------------
    def readConfigFiles(self, args):
        return self.processFile(self.bldCfg, 1) and \
               self.processFile(os.path.join('../..', self.bldCfgLocal)) and \
               self.processFile(os.path.join('..', self.bldCfgLocal)) and \
               self.processFile(os.path.join('.', self.bldCfgLocal)) and \
               self.processArgs(args)

    #------------------------------------------------------------
    def processFile(self, filename, required=0):
        try:
            text = open(filename, 'r').read()
        except IOError:
            if required:
                print "Unable to open %s" % filename
                return 0
            else:
                return 1

        try:
            exec(text, self.__dict__)
        except:
            print "Error evaluating %s" % filename
            import traceback
            traceback.print_exc()
            return 0
        return 1


    #------------------------------------------------------------
    def processArgs(self, args):
        try:
            for st in args:
                pair = string.split(st, '=')
                name = pair[0]
                value = string.join(pair[1:], '=')
                self.__dict__[name] = value
        except:
            print "Error parsing command-line: %s" % st
            return 0

        return 1


    #------------------------------------------------------------





#----------------------------------------------------------------------------
#----------------------------------------------------------------------------
#----------------------------------------------------------------------------

win32Template = '''
#----------------------------------------------------------------------
# This makefile was autogenerated from build.py.  Your changes will be
# lost if the generator is run again.  You have been warned.
#----------------------------------------------------------------------

WXDIR = %(WXDIR)s
VERSION = %(VERSION)s
MAJVER = %(MAJVER)s
BLDVER = %(BLDVER)s
MODULE = %(MODULE)s
SWIGFLAGS = %(SWIGFLAGS)s %(SWIGTOOLKITFLAG)s %(OTHERSWIGFLAGS)s
CFLAGS = %(CFLAGS)s
LFLAGS = %(LFLAGS)s
PYVERSION = %(PYVERSION)s
PYPREFIX = %(PYPREFIX)s
EXECPREFIX = %(EXECPREFIX)s
PYTHONLIB = %(PYTHONLIB)s
FINAL = %(FINAL)s
WXP_USE_THREAD = %(WXP_USE_THREAD)s
WXUSINGDLL = %(WXUSINGDLL)s
GENCODEDIR = %(GENCODEDIR)s
RESFILE = %(RESFILE)s
WXPSRCDIR = %(WXPSRCDIR)s
%(OTHERDEFS)s

WXPYDIR   = %(WXPYDIR)s
BUILDDIR  = $(WXPYDIR)\\wxPython
TARGETDIR = %(TARGETDIR)s

OBJECTS = %(OBJECTS)s
PYMODULES = %(PYMODULES)s
BLDPYMODULES = %(BLDPYMODULES)s
TARGET = %(TARGET)s




!if "$(FINAL)" == "0"
DEBUGLFLAGS = /DEBUG /INCREMENTAL:YES
!else
DEBUGLFLAGS = /INCREMENTAL:NO
!endif
!if "$(WXP_USE_THREAD)" == "1"
THREAD=-DWXP_USE_THREAD=1
!endif




NOPCH=1
OVERRIDEFLAGS=%(OVERRIDEFLAGS)s
EXTRAFLAGS = $(CFLAGS) %(OTHERCFLAGS)s

LFLAGS = %(LFLAGS)s %(OTHERLFLAGS)s
EXTRALIBS = %(LIBS)s %(OTHERLIBS)s

CRTFLAG=%(CRTFLAG)s

#----------------------------------------------------------------------

!include $(WXDIR)\\src\\makevc.env

#----------------------------------------------------------------------

%(DEFAULTRULE)s %(OTHERTARGETS)s



install: default $(TARGETDIR) $(TARGETDIR)\\$(TARGET) pycfiles %(OTHERINSTALLTARGETS)s

clean:
	-erase *.obj
	-erase *.exe
	-erase *.res
	-erase *.map
	-erase *.sbr
	-erase *.pdb
	-erase *.pch
	-erase $(MODULE).exp
	-erase $(MODULE).lib
	-erase $(MODULE).ilk
	-erase $(TARGET)
	-erase $(BUILDDIR)\$(TARGET)
%(PYCLEANUP)s


uninstall: %(OTHERUNINSTALLTARGETS)s
	-erase $(TARGETDIR)\\$(TARGET)
%(PYUNINSTALL)s


#----------------------------------------------------------------------
# implicit rule for compiling .cpp and .c files
{}.cpp{}.obj:
	$(cc) @<<
$(CPPFLAGS) /c /Tp $<
<<

{$(GENCODEDIR)}.cpp{}.obj:
	$(cc) @<<
$(CPPFLAGS) /c /Tp $<
<<

{}.c{}.obj:
	$(cc) @<<
$(CPPFLAGS) /c $<
<<

.SUFFIXES : .i .py

# Implicit rules to run SWIG
{}.i{$(GENCODEDIR)}.cpp:
	swig $(SWIGFLAGS) -c -o $@ $<

{}.i{$(GENCODEDIR)}.py:
        swig $(SWIGFLAGS) -c -o $(GENCODEDIR)\\tmp_wrap.cpp $<
        -erase $(GENCODEDIR)\\tmp_wrap.cpp


{$(GENCODEDIR)}.py{$(TARGETDIR)}.py:
	copy $< $@

{}.py{$(TARGETDIR)}.py:
	copy $< $@


{$(GENCODEDIR)}.py{$(BUILDDIR)}.py:
	copy $< $@

{}.py{$(BUILDDIR)}.py:
	copy $< $@

#----------------------------------------------------------------------

$(TARGET) : $(DUMMYOBJ) $(WXLIB) $(OBJECTS) $(RESFILE)
	$(link) @<<
/out:$@
$(LFLAGS) /export:init$(MODULE) /implib:./$(MODULE).lib
$(DUMMYOBJ) $(OBJECTS) $(RESFILE)
$(LIBS)
<<


%(RESRULE)s


$(TARGETDIR)\\$(TARGET) : $(TARGET)
	copy $(TARGET) $@

$(BUILDDIR)\\$(TARGET) : $(TARGET)
	copy $(TARGET) $@


pycfiles : $(PYMODULES)
	$(EXECPREFIX)\\python $(PYPREFIX)\\Lib\\compileall.py -l $(TARGETDIR)
	$(EXECPREFIX)\\python -O $(PYPREFIX)\Lib\\compileall.py -l $(TARGETDIR)

bldpycfiles : $(BLDPYMODULES)


$(TARGETDIR) :
	mkdir $(TARGETDIR)

$(GENCODEDIR):
	mkdir $(GENCODEDIR)

#----------------------------------------------------------------------

%(DEPENDS)s

#----------------------------------------------------------------------

showflags:
	@echo CPPFLAGS:
	@echo $(CPPFLAGS)
	@echo LFLAGS:
	@echo $(LFLAGS)
        @echo BUILDDIR:
        @echo $(BUILDDIR)


%(OTHERRULES)s
'''

#----------------------------------------------------------------------------
#----------------------------------------------------------------------------
#----------------------------------------------------------------------------

unixTemplate = '''
#----------------------------------------------------------------------
# This makefile was autogenerated from build.py.  Your changes will be
# lost if the generator is run again.  You have been warned.
#----------------------------------------------------------------------

WXDIR = %(WXDIR)s
VERSION = %(VERSION)s
MAJVER = %(MAJVER)s
BLDVER = %(BLDVER)s
MODULE = %(MODULE)s
SWIGFLAGS = %(SWIGFLAGS)s %(SWIGTOOLKITFLAG)s %(OTHERSWIGFLAGS)s
CFLAGS = %(CFLAGS)s $(OPT) %(OTHERCFLAGS)s
LFLAGS = %(LFLAGS)s %(OTHERLFLAGS)s

PYVERSION = %(PYVERSION)s
PYPREFIX = %(PYPREFIX)s
EXECPREFIX = %(EXECPREFIX)s
PYINCLUDE = $(PYPREFIX)/include/python$(PYVERSION)
EXECINCLUDE = $(EXECPREFIX)/include/python$(PYVERSION)
PYLIB = %(PYLIB)s
LIBPL = %(LIBPL)s
PYTHONLIB = %(PYTHONLIB)s
FINAL = %(FINAL)s
WXP_USE_THREAD = %(WXP_USE_THREAD)s
GENCODEDIR = %(GENCODEDIR)s
WXPSRCDIR = %(WXPSRCDIR)s
HELPERLIB = %(HELPERLIB)s
HELPERLIBDIR = %(HELPERLIBDIR)s
WXCONFIG=%(WXCONFIG)s

WXPYDIR   = %(WXPYDIR)s
BUILDDIR  = $(WXPYDIR)/wxPython
TARGETDIR = %(TARGETDIR)s
%(OTHERDEFS)s


CCC = %(CCC)s
CC = %(CC)s
OPT = %(OPT)s
SO = %(SO)s
LDSHARED = %(LDSHARED)s
CCSHARED = %(CCSHARED)s


OBJECTS = %(OBJECTS)s
PYMODULES = %(PYMODULES)s
BLDPYMODULES = %(BLDPYMODULES)s
TARGET = %(TARGET)s


ifeq ($(WXP_USE_THREAD), 1)
THREAD=-DWXP_USE_THREAD
endif

USE_SONAME = %(USE_SONAME)s
ifeq ($(USE_SONAME), 1)
LIBS = -l$(HELPERLIB) %(OTHERLIBS)s
else
LIBS = $(WXPSRCDIR)/lib$(HELPERLIB)$(SO) %(OTHERLIBS)s
endif

#----------------------------------------------------------------------

%(DEFAULTRULE)s %(OTHERTARGETS)s

install: default $(TARGETDIR) $(TARGETDIR)/$(TARGET) pycfiles %(OTHERINSTALLTARGETS)s

clean:
	-rm -f *.o *$(SO) *$(SO).* *~
	-rm -f $(TARGET)
	-rm -f $(BUILDDIR)/$(TARGET)
%(PYCLEANUP)s

uninstall: %(OTHERUNINSTALLTARGETS)s
	-rm -f $(TARGETDIR)/$(TARGET)
%(PYUNINSTALL)s


#----------------------------------------------------------------------

%%.o : %%.cpp
	$(CCC) $(CCSHARED) $(CFLAGS) $(OTHERCFLAGS) -c $<

%%.o : $(GENCODEDIR)/%%.cpp
	$(CCC) $(CCSHARED) $(CFLAGS) $(OTHERCFLAGS) -c $<

%%.o : %%.c
	$(CC) $(CCSHARED) $(CFLAGS) $(OTHERCFLAGS) -c $<

%%.o : $(GENCODEDIR)/%%.c
	$(CC) $(CCSHARED) $(CFLAGS) $(OTHERCFLAGS) -c $<

ifndef NOSWIG
$(GENCODEDIR)/%%.cpp : %%.i
	swig $(SWIGFLAGS) -c -o $@ $<

$(GENCODEDIR)/%%.py : %%.i
	swig $(SWIGFLAGS) -c -o $(GENCODEDIR)/tmp_wrap.cpp $<
	rm $(GENCODEDIR)/tmp_wrap.cpp
endif


$(TARGETDIR)/%% : %%
	cp -f $< $@

$(TARGETDIR)/$(TARGET) : $(TARGET)
	cp -f $< $@
	strip $@

$(TARGETDIR)/%% : $(GENCODEDIR)/%%
	cp -f $< $@

$(BUILDDIR)/%% : %%
	cp -f $< $@

$(BUILDDIR)/%% : $(GENCODEDIR)/%%
	cp -f $< $@

#----------------------------------------------------------------------

%(DEPENDS)s

#----------------------------------------------------------------------

$(TARGET) : $(OBJECTS)
	$(LDSHARED) $(OBJECTS) $(LFLAGS) $(LIBS) $(OTHERLIBS) -o $(TARGET)


pycfiles : $(PYMODULES)
	$(EXECPREFIX)/bin/python $(PYLIB)/compileall.py -l $(TARGETDIR)
	$(EXECPREFIX)/bin/python -O $(PYLIB)/compileall.py -l $(TARGETDIR)

bldpycfiles : $(BLDPYMODULES)


$(TARGETDIR) :
	mkdir -p $(TARGETDIR)

$(GENCODEDIR):
	mkdir $(GENCODEDIR)

#----------------------------------------------------------------------


%(OTHERRULES)s


'''


#----------------------------------------------------------------------------

if __name__ == '__main__':
    err = main(sys.argv)
    sys.exit(err)

#----------------------------------------------------------------------------
