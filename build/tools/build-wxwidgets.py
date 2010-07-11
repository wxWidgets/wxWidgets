#!/usr/bin/env python

###################################
# Author: Kevin Ollivier
# License: wxWidgets License
###################################

import os
import re
import sys
import builder
import commands
import glob
import optparse
import platform
import shutil
import types

# builder object
wxBuilder = None

# other globals
scriptDir = None
wxRootDir = None
contribDir = None
options = None
configure_opts = None
exitWithException = True


def exitIfError(code, msg):
    if code != 0:
        print msg
        if exitWithException:
            raise builder.BuildError, msg
        else:
            sys.exit(1)
     
            
def getWxRelease():
    global wxRootDir
    configureText = open(os.path.join(wxRootDir, "configure.in"), "r").read()
    
    majorVersion = re.search("wx_major_version_number=(\d+)", configureText).group(1)
    minorVersion = re.search("wx_minor_version_number=(\d+)", configureText).group(1)
    
    return "%s.%s" % (majorVersion, minorVersion)
   


def doMacLipoBuild(arch, buildDir, installDir,
                   cxxcompiler="g++-4.0", cccompiler="gcc-4.0", target="10.4", flags=""):
    archInstallDir = installDir + "/" + arch
    old_env = dict(CXX = os.environ.get('CXX'),
                   CC = os.environ.get('CC'),
                   MACOSX_DEPLOYMENT_TARGET = os.environ.get('MACOSX_DEPLOYMENT_TARGET'),
                   )
    
    os.environ["CXX"] = "%s -arch %s %s" % (cxxcompiler, arch, flags)
    os.environ["CC"] = "%s -arch %s %s" % (cccompiler, arch, flags)
    os.environ["MACOSX_DEPLOYMENT_TARGET"] = target
    archArgs = ["DESTDIR=" + archInstallDir]
    buildRoot = "bld-" + arch
    if buildDir:
        buildRoot = buildDir + "/" + buildRoot
    
    if not os.path.exists(buildRoot):
        os.makedirs(buildRoot)
    
    olddir = os.getcwd()
    os.chdir(buildRoot)
    
    if not options.no_config:
        exitIfError(wxBuilder.configure(dir=wxRootDir, options=configure_opts), "Error running configure for "+arch)
    exitIfError(wxBuilder.build(options=archArgs), "Error building for "+arch)
    exitIfError(wxBuilder.install(options=["DESTDIR=" + archInstallDir]), "Error Installing for "+arch)
    
    if options.wxpython and os.path.exists(os.path.join(wxRootDir, contribDir)):
        exitIfError(wxBuilder.build(dir=os.path.join(contribDir, "gizmos"), options=archArgs), 
                    "Error building gizmos for "+arch)
        exitIfError(wxBuilder.install(os.path.join(contribDir, "gizmos"), options=["DESTDIR=" + archInstallDir]), 
                    "Error Installing gizmos for "+arch)
        
        exitIfError(wxBuilder.build(dir=os.path.join(contribDir, "stc"),options=archArgs), 
                    "Error building stc for "+arch)
        exitIfError(wxBuilder.install(os.path.join(contribDir, "stc"),options=["DESTDIR=" + archInstallDir]), 
                    "Error installing stc for "+arch)

    os.chdir(olddir)
    for key, val in old_env.items():
        if val:
            os.environ[key] = val
        else:
            del os.environ[key]


def macFixupInstallNames(destdir, prefix):
    # When an installdir is used then the install_names embedded in
    # the dylibs are not correct.  Reset the IDs and the dependencies
    # to use just the prefix.
    pwd = os.getcwd()
    os.chdir(destdir+prefix+'/lib')
    dylibs = glob.glob('*.dylib')     # ('*[0-9].[0-9].[0-9].[0-9]*.dylib')
    for lib in dylibs:
        cmd = 'install_name_tool -id %s/lib/%s %s/lib/%s' % \
              (prefix,lib,  destdir+prefix,lib)
        print cmd
        os.system(cmd)
        for dep in dylibs:
            cmd = 'install_name_tool -change %s/lib/%s %s/lib/%s %s/lib/%s' % \
                  (destdir+prefix,dep,  prefix,dep,  destdir+prefix,lib)
            print cmd
            os.system(cmd)        
    os.chdir(pwd)



def main(scriptName, args):
    global scriptDir
    global wxRootDir
    global contribDir
    global options
    global configure_opts
    global wxBuilder
    
    scriptDir = os.path.dirname(os.path.abspath(scriptName))
    wxRootDir = os.path.abspath(os.path.join(scriptDir, "..", ".."))
    
    contribDir = os.path.join("contrib", "src")
    installDir = None

    VERSION = tuple([int(i) for i in getWxRelease().split('.')])
    
    if sys.platform.startswith("win"):
        contribDir = os.path.join(wxRootDir, "contrib", "build")
    
    if sys.platform.startswith("win"):
        toolkit = "msvc"
    else:
        toolkit = "autoconf"
    
    option_dict = { 
        "clean"      : (False, "Clean all files from the build directory"),
        "debug"      : (False, "Build the library in debug symbols"),
        "builddir"   : ("", "Directory where the build will be performed for autoconf builds."),
        "prefix"     : ("", "Configured prefix to use for autoconf builds. Defaults to installdir if set."),
        "install"    : (False, "Install the toolkit to the installdir directory, or the default dir."),
        "installdir" : ("", "Directory where built wxWidgets will be installed"),
        "mac_universal_binary" : (False, "Build Mac version as a universal binary"),
        "mac_lipo"   : (False, "EXPERIMENTAL: Create a universal binary by merging a PPC and Intel build together."),
        "mac_framework" : (False, "Install the Mac build as a framework"),
        "no_config"  : (False, "Turn off configure step on autoconf builds"),
        "rebake"     : (False, "Regenerate Bakefile and autoconf files"),
        "unicode"    : (False, "Build the library with unicode support"),
        "wxpython"   : (False, "Build the wxWidgets library with all options needed by wxPython"),
        "cocoa"      : (False, "Build the Cooca port (Mac only currently)."),
        "osx_cocoa"  : (False, "Build the new Cocoa port"),
        "shared"     : (False, "Build wx as a dynamic library"),
        "cairo"      : (False, "Build support for wxCairoContext (always true on GTK+)"),
        "extra_make" : ("", "Extra args to pass on [n]make's command line."),
        "features"   : ("", "A comma-separated list of wxUSE_XYZ defines on Win, or a list of configure flags on unix."),
    }
        
    parser = optparse.OptionParser(usage="usage: %prog [options]", version="%prog 1.0")
    
    for opt in option_dict:
        default = option_dict[opt][0]
        
        action = "store"
        if type(default) == types.BooleanType:
            action = "store_true"
        parser.add_option("--" + opt, default=default, action=action, dest=opt, help=option_dict[opt][1])
    
    options, arguments = parser.parse_args(args=args)
    
    # compiler / build system specific args
    buildDir = options.builddir
    args = None
    installDir = options.installdir
    prefixDir = options.prefix
    
    if toolkit == "autoconf":
        configure_opts = []
        if options.features != "":
            configure_opts.extend(options.features.split(" "))
        
        if options.unicode:
            configure_opts.append("--enable-unicode")
            
        if options.debug:
            configure_opts.append("--enable-debug")
            
        if options.mac_universal_binary: 
            configure_opts.append("--enable-universal_binary")
            
        if options.cocoa:
            configure_opts.append("--with-cocoa")
            
        if options.osx_cocoa:
            configure_opts.append("--with-osx_cocoa")
        
        wxpy_configure_opts = [
                            "--with-opengl",
                            "--enable-sound",
                            "--enable-graphics_ctx",
                            "--enable-mediactrl",
                            "--enable-display",
                            "--enable-geometry",
                            "--enable-debug_flag",
                            "--enable-optimise",
                            "--disable-debugreport",
                            "--enable-uiactionsim",
                            ]

        if sys.platform.startswith("darwin"):
            wxpy_configure_opts.append("--enable-monolithic")
        else:
            wxpy_configure_opts.append("--with-sdl")
            wxpy_configure_opts.append("--with-gnomeprint")
                                        
        if not options.mac_framework:
            if installDir and not prefixDir:
                prefixDir = installDir
            if prefixDir:
                configure_opts.append("--prefix=" + prefixDir)
    
        if options.wxpython:
            configure_opts.extend(wxpy_configure_opts)
            if options.debug:
                # wxPython likes adding these debug options too
                configure_opts.append("--enable-debug_gdb")
                configure_opts.append("--disable-optimise")
    
        if options.rebake:
            retval = os.system("make -f autogen.mk")
            exitIfError(retval, "Error running autogen.mk")
            
        if options.mac_framework:
            # Framework build is always a universal binary
            options.mac_lipo = True
            name = "wx"
            if options.osx_cocoa:
                name += "OSXCocoa"
            installDir = "/Library/Frameworks/%s.framework/Versions/%s" % (name, getWxRelease())
            configure_opts.append("--prefix=" + installDir)
            # framework builds always need to be monolithic
            if not "--enable-monolithic" in configure_opts:
                configure_opts.append("--enable-monolithic")
            
        print "Configure options: " + `configure_opts`
        wxBuilder = builder.AutoconfBuilder()
        if not options.no_config and not options.clean and not options.mac_lipo:
            olddir = os.getcwd()
            if buildDir:
                os.chdir(buildDir)
            exitIfError(wxBuilder.configure(dir=wxRootDir, options=configure_opts), 
                        "Error running configure")
            os.chdir(olddir)
    
    elif toolkit in ["msvc", "msvcProject"]:
        flags = {}
        buildDir = os.path.abspath(os.path.join(scriptDir, "..", "msw"))
    
        if options.unicode:
            flags["wxUSE_UNICODE"] = "1"
            if VERSION < (2,9):
                flags["wxUSE_UNICODE_MSLU"] = "1"
    
        if options.cairo:
            flags["wxUSE_CAIRO"] = "1"
    
        if options.wxpython:
            flags["wxDIALOG_UNIT_COMPATIBILITY "] = "0"
            flags["wxUSE_DEBUG_CONTEXT"] = "1"
            flags["wxUSE_MEMORY_TRACING"] = "1"
            flags["wxUSE_DIALUP_MANAGER"] = "0"
            flags["wxUSE_GLCANVAS"] = "1"
            flags["wxUSE_POSTSCRIPT"] = "1"
            flags["wxUSE_AFM_FOR_POSTSCRIPT"] = "0"
            flags["wxUSE_DISPLAY"] = "1"
            flags["wxUSE_DEBUGREPORT"] = "0"
            flags["wxUSE_GRAPHICS_CONTEXT"] = "1"
            flags["wxUSE_DATEPICKCTRL_GENERIC"] = "1"
            if VERSION < (2,9):
                flags["wxUSE_DIB_FOR_BITMAP"] = "1"

            if VERSION >= (2,9):
                flags["wxUSE_UIACTIONSIMULATOR"] = "1"
                
            # setup the wxPython 'hybrid' build
            if not options.debug:
                flags["wxUSE_MEMORY_TRACING"] = "0"
                flags["wxUSE_DEBUG_CONTEXT"] = "0"
    
        mswIncludeDir = os.path.join(wxRootDir, "include", "wx", "msw")
        setup0File = os.path.join(mswIncludeDir, "setup0.h")
        setupText = open(setup0File, "rb").read()
        
        for flag in flags:
            setupText, subsMade = re.subn(flag + "\s+?\d", "%s %s" % (flag, flags[flag]), setupText)
            if subsMade == 0:
                print "Flag %s wasn't found in setup0.h!" % flag
                sys.exit(1)
    
        setupFile = open(os.path.join(mswIncludeDir, "setup.h"), "wb")
        setupFile.write(setupText)
        setupFile.close()
        args = []
        if toolkit == "msvc":
            print "setting build options..."
            args.append("-f makefile.vc")
            if options.unicode:
                args.append("UNICODE=1")
                if VERSION < (2,9):
                    args.append("MSLU=1")
    
            if options.wxpython:
                args.append("OFFICIAL_BUILD=1")
                args.append("SHARED=1")
                args.append("MONOLITHIC=0")
                args.append("USE_OPENGL=1")
                args.append("USE_GDIPLUS=1")
                args.append("CXXFLAGS=/D__NO_VC_CRTDBG__")
                
                if not options.debug:
                    # "Hybrid" build, not really release or debug
                    args.append("DEBUG_FLAG=1")
                    args.append("WXDEBUGFLAG=h")
                    args.append("BUILD=release")
                else:
                    args.append("BUILD=debug")
    
            wxBuilder = builder.MSVCBuilder()
            
        if toolkit == "msvcProject":
            args = []
            if options.shared or options.wxpython:
                args.append("wx_dll.dsw")
            else:
                args.append("wx.dsw")
                
            # TODO:
            wxBuilder = builder.MSVCProjectBuilder()
        
    if not wxBuilder:
        print "Builder not available for your specified platform/compiler."
        sys.exit(1)
    
    if options.clean:
        print "Performing cleanup."
        wxBuilder.clean()
        
        if options.wxpython:
            exitIfError(wxBuilder.clean(os.path.join(contribDir, "gizmos")), "Error building gizmos")
            exitIfError(wxBuilder.clean(os.path.join(contribDir, "stc")), "Error building stc")
        
        sys.exit(0)
    
    isLipo = False    
    if options.mac_lipo:
        if options.mac_universal_binary:
            print "WARNING: Cannot specify both mac_lipo and mac_universal_binary, as they conflict."
            print "         Using mac_universal_binary..."
        else:
            isLipo = True
            # TODO: Add 64-bit when we're building OS X Cocoa
            
            # 2.8, use gcc 3.3 on PPC for 10.3 support, but only when building ...
            macVersion = platform.mac_ver()[0]
            isLeopard = macVersion.find("10.5") != -1
            
            if not isLeopard and os.path.exists(os.path.join(wxRootDir, contribDir)):
                # Building wx 2.8 so make the ppc build compatible with Panther
                doMacLipoBuild("ppc", buildDir, installDir, cxxcompiler="g++-3.3", cccompiler="gcc-3.3", 
                            target="10.3", flags="-DMAC_OS_X_VERSION_MAX_ALLOWED=1040")
            else:
                doMacLipoBuild("ppc", buildDir, installDir)
    
            doMacLipoBuild("i386", buildDir, installDir)
            
            # Use lipo to merge together all binaries in the install dirs, and it
            # also copies all other files and links it finds to the new destination.
            result = os.system("python %s/distrib/scripts/mac/lipo-dir.py %s %s %s" %
                               (wxRootDir, installDir+"/ppc", installDir+"/i386", installDir))

            # tweak the wx-config script
            fname = os.path.abspath(installDir + '/bin/wx-config') 
            data = open(fname).read()
            data = data.replace('ppc/', '')
            data = data.replace('i386/', '')
            open(fname, 'w').write(data)
            
            shutil.rmtree(installDir + "/ppc")
            shutil.rmtree(installDir + "/i386")

                                
      
    if not isLipo:
        if options.extra_make:
            args.append(options.extra_make)
        exitIfError(wxBuilder.build(dir=buildDir, options=args), "Error building")
        
        if options.wxpython and os.path.exists(contribDir):
            exitIfError(wxBuilder.build(os.path.join(contribDir, "gizmos"), options=args), "Error building gizmos")
            exitIfError(wxBuilder.build(os.path.join(contribDir, "stc"),options=args), "Error building stc")
            
        if options.install:
            extra=None
            if installDir:
                extra = ['DESTDIR='+installDir]
            wxBuilder.install(options=extra) 
            
            if options.wxpython and os.path.exists(contribDir):
                exitIfError(wxBuilder.install(os.path.join(contribDir, "gizmos"), options=extra), "Error building gizmos")
                exitIfError(wxBuilder.install(os.path.join(contribDir, "stc"), options=extra), "Error building stc")
            
    if options.mac_framework:
    
        def renameLibrary(libname, frameworkname):
            reallib = libname
            links = []
            while os.path.islink(reallib):
                links.append(reallib)
                reallib = "lib/" + os.readlink(reallib)
                
            print "reallib is %s" % reallib
            os.system("mv -f %s lib/%s.dylib" % (reallib, frameworkname))
            
            for link in links:
                os.system("ln -s -f %s.dylib %s" % (frameworkname, link))
    
        os.chdir(installDir)
        build_string = ""
        if options.debug:
            build_string = "d"
        version = commands.getoutput("bin/wx-config --release")
        basename = commands.getoutput("bin/wx-config --basename")
        configname = commands.getoutput("bin/wx-config --selected-config")
        
        os.system("ln -s -f bin Resources")
        
        # we make wx the "actual" library file and link to it from libwhatever.dylib
        # so that things can link to wx and survive minor version changes
        renameLibrary("lib/lib%s-%s.dylib" % (basename, version), "wx")
        os.system("ln -s -f lib/wx.dylib wx")
        
        os.system("ln -s -f include/wx Headers")
        
        for lib in ["GL", "STC", "Gizmos", "Gizmos_xrc"]:  
            libfile = "lib/lib%s_%s-%s.dylib" % (basename, lib.lower(), version)
            if os.path.exists(libfile):
                frameworkDir = "framework/wx%s/%s" % (lib, version)
                if not os.path.exists(frameworkDir):
                    os.makedirs(frameworkDir)
                renameLibrary(libfile, "wx" + lib)
                os.system("ln -s -f ../../../%s %s/wx%s" % (libfile, frameworkDir, lib))        
        
        for lib in glob.glob("lib/*.dylib"):
            if not os.path.islink(lib):
                corelibname = "lib/lib%s-%s.0.dylib" % (basename, version)
                os.system("install_name_tool -id %s %s" % (os.path.join(installDir, lib), lib))
                os.system("install_name_tool -change %s %s %s" % (os.path.join(installDir, "i386", corelibname), os.path.join(installDir, corelibname), lib))
                
        os.chdir("include")
        
        header_template = """
        
#ifndef __WX_FRAMEWORK_HEADER__
#define __WX_FRAMEWORK_HEADER__

%s

#endif // __WX_FRAMEWORK_HEADER__
"""
        headers = ""
        header_dir = "wx-%s/wx" % version
        for include in glob.glob(header_dir + "/*.h"):
            headers += "wx/" + os.path.basename(include) + "\n"
            
        framework_header = open("wx.h", "w")
        framework_header.write(header_template % headers)
        framework_header.close()
        
        os.system("ln -s -f %s wx" % header_dir)
        os.system("ln -s -f ../../../lib/wx/include/%s/wx/setup.h wx/setup.h" % configname)
        
        os.chdir(os.path.join(installDir, "..", ".."))
        os.system("ln -s -f %s Versions/Current" % os.path.basename(installDir))
        os.system("ln -s -f Versions/Current/Headers Headers")
        os.system("ln -s -f Versions/Current/Resources Resources")
        os.system("ln -s -f Versions/Current/wx wx")
        

    # adjust the install_name if needed  TODO: skip this for framework builds?
    if sys.platform.startswith("darwin") and \
           options.install and \
           options.installdir and \
           not options.wxpython:  # wxPython's build will do this later if needed
        prefix = options.prefix
        if not prefix:
            prefix = '/usr/local'
        macFixupInstallNames(options.installdir, prefix)

        
        
if __name__ == '__main__':
    exitWithException = False  # use sys.exit instead
    main(sys.argv[0], sys.argv[1:])
    
