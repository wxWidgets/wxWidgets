#!/usr/bin/env python

###################################
# Author: Kevin Ollivier
# Licence: wxWindows licence
###################################

import os
import re
import sys
import builder
import glob
import optparse
import platform
import shutil
import types
import subprocess

# builder object
wxBuilder = None

# other globals
scriptDir = None
wxRootDir = None
contribDir = None
options = None
configure_opts = None
exitWithException = True
nmakeCommand = 'nmake.exe'

verbose = False


def numCPUs():
    """
    Detects the number of CPUs on a system.
    This approach is from detectCPUs here: http://www.artima.com/weblogs/viewpost.jsp?thread=230001
    """
    # Linux, Unix and MacOS:
    if hasattr(os, "sysconf"):
        if "SC_NPROCESSORS_ONLN" in os.sysconf_names:
            # Linux & Unix:
            ncpus = os.sysconf("SC_NPROCESSORS_ONLN")
            if isinstance(ncpus, int) and ncpus > 0:
                return ncpus
        else: # OSX:
            p = subprocess.Popen("sysctl -n hw.ncpu", shell=True, stdout=subprocess.PIPE)
            return p.stdout.read()
            
    # Windows:
    if "NUMBER_OF_PROCESSORS" in os.environ:
            ncpus = int(os.environ["NUMBER_OF_PROCESSORS"]);
            if ncpus > 0:
                return ncpus
    return 1 # Default


def getXcodePaths():
    base = getoutput("xcode-select -print-path")
    return [base, base+"/Platforms/MacOSX.platform/Developer"]


def getVisCVersion():
    text = getoutput("cl.exe")
    if 'Version 13' in text:
        return '71'
    if 'Version 15' in text:
        return '90'
    if 'Version 16' in text:
        return '100'
    # TODO: Add more tests to get the other versions...
    else:
        return 'FIXME'
    

def exitIfError(code, msg):
    if code != 0:
        print(msg)
        if exitWithException:
            raise builder.BuildError(msg)
        else:
            sys.exit(1)
     
            
def getWxRelease(wxRoot=None):
    if not wxRoot:
        global wxRootDir
        wxRoot = wxRootDir
        
    configureText = open(os.path.join(wxRoot, "configure.in"), "r").read()
    majorVersion = re.search("wx_major_version_number=(\d+)", configureText).group(1)
    minorVersion = re.search("wx_minor_version_number=(\d+)", configureText).group(1)
    
    versionText = "%s.%s" % (majorVersion, minorVersion)
    
    if int(minorVersion) % 2:
        releaseVersion = re.search("wx_release_number=(\d+)", configureText).group(1)
        versionText += ".%s" % (releaseVersion)
    
    return versionText


def getFrameworkName(options):
    # the name of the framework is based on the wx port being built
    name = "wxOSX"
    if options.osx_cocoa:
        name += "Cocoa"
    else:
        name += "Carbon"
    return name            


def getPrefixInFramework(options, wxRoot=None):
    # the path inside the framework that is the wx --prefix
    fwPrefix = os.path.join(
        os.path.abspath(options.mac_framework_prefix),
        "%s.framework/Versions/%s" % (getFrameworkName(options), getWxRelease(wxRoot)))
    return fwPrefix


def macFixupInstallNames(destdir, prefix, buildDir=None):
    # When an installdir is used then the install_names embedded in
    # the dylibs are not correct.  Reset the IDs and the dependencies
    # to use just the prefix.
    print("**** macFixupInstallNames(%s, %s, %s)" % (destdir, prefix, buildDir))
    pwd = os.getcwd()
    os.chdir(destdir+prefix+'/lib')
    dylibs = glob.glob('*.dylib')     # ('*[0-9].[0-9].[0-9].[0-9]*.dylib')
    for lib in dylibs:
        cmd = 'install_name_tool -id %s/lib/%s %s/lib/%s' % \
              (prefix,lib,  destdir+prefix,lib)
        print(cmd)
        run(cmd)
        for dep in dylibs:
            if buildDir is not None:
                cmd = 'install_name_tool -change %s/lib/%s %s/lib/%s %s/lib/%s' % \
                      (buildDir,dep,  prefix,dep,  destdir+prefix,lib)
            else:
                cmd = 'install_name_tool -change %s/lib/%s %s/lib/%s %s/lib/%s' % \
                      (destdir+prefix,dep,  prefix,dep,  destdir+prefix,lib)
            print(cmd)
            run(cmd)        
    os.chdir(pwd)


def run(cmd):
    global verbose
    if verbose:
        print("Running %s" % cmd)
    return exitIfError(os.system(cmd), "Error running %s" % cmd)


def getoutput(cmd):
    sp = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    output = None
    output = sp.stdout.read()
    if sys.version_info > (3,):
        output = output.decode('utf-8')  # TODO: is utf-8 okay here?
    output = output.rstrip()
    rval = sp.wait()
    if rval:
        # Failed!
        print("Command '%s' failed with exit code %d." % (cmd, rval))
        sys.exit(rval)
    return output
    

def main(scriptName, args):
    global scriptDir
    global wxRootDir
    global contribDir
    global options
    global configure_opts
    global wxBuilder
    global nmakeCommand
    
    scriptDir = os.path.dirname(os.path.abspath(scriptName))
    wxRootDir = os.path.abspath(os.path.join(scriptDir, "..", ".."))
    
    contribDir = os.path.join("contrib", "src")
    installDir = None

    VERSION = tuple([int(i) for i in getWxRelease().split('.')[:2]])
    
    if sys.platform.startswith("win"):
        contribDir = os.path.join(wxRootDir, "contrib", "build")
    
    if sys.platform.startswith("win"):
        toolkit = "msvc"
    else:
        toolkit = "autoconf"
    
    defJobs = str(numCPUs())
    defFwPrefix = '/Library/Frameworks'
    
    option_dict = { 
        "clean"         : (False, "Clean all files from the build directory"),
        "debug"         : (False, "Build the library in debug symbols"),
        "builddir"      : ("", "Directory where the build will be performed for autoconf builds."),
        "prefix"        : ("", "Configured prefix to use for autoconf builds. Defaults to installdir if set. Ignored for framework builds."),
        "jobs"          : (defJobs, "Number of jobs to run at one time in make. Default: %s" % defJobs),
        "install"       : (False, "Install the toolkit to the installdir directory, or the default dir."),
        "installdir"    : ("", "Directory where built wxWidgets will be installed"),
        "mac_distdir"   : (None, "If set on Mac, will create an installer package in the specified dir."),
        "mac_universal_binary" 
                        : ("", "Comma separated list of architectures to include in the Mac universal binary"),
        "mac_framework" : (False, "Install the Mac build as a framework"),
        "mac_framework_prefix" 
                        : (defFwPrefix, "Prefix where the framework should be installed. Default: %s" % defFwPrefix),
        "cairo"         : (False, "Enable dynamically loading the Cairo lib for wxGraphicsContext on MSW"),
        "no_config"     : (False, "Turn off configure step on autoconf builds"),
        "config_only"   : (False, "Only run the configure step and then exit"),
        "rebake"        : (False, "Regenerate Bakefile and autoconf files"),
        "unicode"       : (False, "Build the library with unicode support"),
        "wxpython"      : (False, "Build the wxWidgets library with all options needed by wxPython"),
        "cocoa"         : (False, "Build the old Mac Cocoa port."),
        "osx_cocoa"     : (False, "Build the new Cocoa port"),
        "shared"        : (False, "Build wx as a dynamic library"),
        "extra_make"    : ("", "Extra args to pass on [n]make's command line."),
        "features"      : ("", "A comma-separated list of wxUSE_XYZ defines on Win, or a list of configure flags on unix."),
        "verbose"       : (False, "Print commands as they are run, (to aid with debugging this script)"),
        "jom"           : (False, "Use jom.exe instead of nmake for MSW builds."),
    }
        
    parser = optparse.OptionParser(usage="usage: %prog [options]", version="%prog 1.0")

    keys = option_dict.keys()
    for opt in sorted(keys):
        default = option_dict[opt][0]
        action = "store"
        if type(default) == bool:
            action = "store_true"
        parser.add_option("--" + opt, default=default, action=action, dest=opt, 
                          help=option_dict[opt][1])
    
    options, arguments = parser.parse_args(args=args)

    global verbose
    if options.verbose:
        verbose = True
        
    # compiler / build system specific args
    buildDir = options.builddir
    args = []
    installDir = options.installdir
    prefixDir = options.prefix
    
    if toolkit == "autoconf":
        if not buildDir:
            buildDir = os.getcwd()
        configure_opts = []
        if options.features != "":
            configure_opts.extend(options.features.split(" "))
        
        if options.unicode:
            configure_opts.append("--enable-unicode")
            
        if options.debug:
            configure_opts.append("--enable-debug")
            
        if options.cocoa:
            configure_opts.append("--with-old_cocoa")
            
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

        # Try to use use lowest available SDK back to 10.5. Both Carbon and
        # Cocoa builds require at least the 10.5 SDK now. We only add it to
        # the wxpy options because this is a hard-requirement for wxPython,
        # but other cases it is optional and is left up to the developer.
        # TODO: there should be a command line option to set the SDK...
        if sys.platform.startswith("darwin"):
            for xcodePath in getXcodePaths():
                sdks = [
                    xcodePath+"/SDKs/MacOSX10.5.sdk",
                    xcodePath+"/SDKs/MacOSX10.6.sdk",
                    xcodePath+"/SDKs/MacOSX10.7.sdk",
                    xcodePath+"/SDKs/MacOSX10.8.sdk",
                    ]
            
                # use the lowest available sdk
                for sdk in sdks:
                    if os.path.exists(sdk):
                        wxpy_configure_opts.append(
                            "--with-macosx-sdk=%s" % sdk)
                        break

        if not options.mac_framework:
            if installDir and not prefixDir:
                prefixDir = installDir
            if prefixDir:
                prefixDir = os.path.abspath(prefixDir)
                configure_opts.append("--prefix=" + prefixDir)
                             
        
        if options.wxpython:
            configure_opts.extend(wxpy_configure_opts)
            if options.debug:
                # wxPython likes adding these debug options too
                configure_opts.append("--enable-debug_gdb")
                configure_opts.append("--disable-optimise")
                configure_opts.remove("--enable-optimise")
                
    
        if options.rebake:
            retval = run("make -f autogen.mk")
            exitIfError(retval, "Error running autogen.mk")
            
        if options.mac_framework:
            # TODO: Should options.install be automatically turned on if the
            # mac_framework flag is given?
            
            # framework builds always need to be monolithic
            if not "--enable-monolithic" in configure_opts:
                configure_opts.append("--enable-monolithic")

            # The --prefix given to configure will be the framework prefix
            # plus the framework specific dir structure.
            prefixDir = getPrefixInFramework(options)
            configure_opts.append("--prefix=" + prefixDir)
            
            # the framework build adds symlinks above the installDir + prefixDir folder
            # so we need to wipe from the framework root instead of inside the prefixDir.
            frameworkRootDir = os.path.abspath(os.path.join(installDir + prefixDir, "..", ".."))
            if os.path.exists(frameworkRootDir):
                if os.path.exists(frameworkRootDir):
                    shutil.rmtree(frameworkRootDir)

        if options.mac_universal_binary: 
            if options.mac_universal_binary == 'default':
                if options.osx_cocoa:
                    configure_opts.append("--enable-universal_binary=i386,x86_64")                
                else:
                    configure_opts.append("--enable-universal_binary")                
            else:
                configure_opts.append("--enable-universal_binary=%s" % options.mac_universal_binary)

            
        print("Configure options: " + repr(configure_opts))
        wxBuilder = builder.AutoconfBuilder()
        if not options.no_config and not options.clean:
            olddir = os.getcwd()
            if buildDir:
                os.chdir(buildDir)
            exitIfError(wxBuilder.configure(dir=wxRootDir, options=configure_opts), 
                        "Error running configure")
            os.chdir(olddir)

        if options.config_only:
            print("Exiting after configure")
            return
    
    elif toolkit in ["msvc", "msvcProject"]:
        flags = {}
        buildDir = os.path.abspath(os.path.join(scriptDir, "..", "msw"))

        print("creating wx/msw/setup.h")
        if options.unicode:
            flags["wxUSE_UNICODE"] = "1"
    
        if options.cairo:
            if not os.environ.get("CAIRO_ROOT"):
                print("WARNING: Expected CAIRO_ROOT set in the environment!")
            flags["wxUSE_CAIRO"] = "1"
    
        if options.wxpython:
            flags["wxDIALOG_UNIT_COMPATIBILITY "] = "0"
            flags["wxUSE_DEBUGREPORT"] = "0"
            flags["wxUSE_DIALUP_MANAGER"] = "0"
            flags["wxUSE_GRAPHICS_CONTEXT"] = "1"
            flags["wxUSE_DISPLAY"] = "1"
            flags["wxUSE_GLCANVAS"] = "1"
            flags["wxUSE_POSTSCRIPT"] = "1"
            flags["wxUSE_AFM_FOR_POSTSCRIPT"] = "0"
            flags["wxUSE_DATEPICKCTRL_GENERIC"] = "1"

            # Remove this when Windows XP finally dies, or when there is a
            # solution for ticket #13116...
            flags["wxUSE_COMPILER_TLS"] = "0"
            
            if VERSION < (2,9):
                flags["wxUSE_DIB_FOR_BITMAP"] = "1"

            if VERSION >= (2,9):
                flags["wxUSE_UIACTIONSIMULATOR"] = "1"

    
        mswIncludeDir = os.path.join(wxRootDir, "include", "wx", "msw")
        setupFile = os.path.join(mswIncludeDir, "setup.h")
        setupText = open(setupFile, "rb").read()
        
        for flag in flags:
            setupText, subsMade = re.subn(flag + "\s+?\d", "%s %s" % (flag, flags[flag]), setupText)
            if subsMade == 0:
                print("Flag %s wasn't found in setup.h!" % flag)
                sys.exit(1)
    
        setupFile = open(os.path.join(mswIncludeDir, "setup.h"), "wb")
        setupFile.write(setupText)
        setupFile.close()
        args = []
        if toolkit == "msvc":
            print("setting build options...")
            args.append("-f makefile.vc")
            if options.unicode:
                args.append("UNICODE=1")
    
            if options.wxpython:
                args.append("OFFICIAL_BUILD=1")
                args.append("COMPILER_VERSION=%s" % getVisCVersion())
                args.append("SHARED=1")
                args.append("MONOLITHIC=0")
                args.append("USE_OPENGL=1")
                args.append("USE_GDIPLUS=1")
                
                if not options.debug:
                    args.append("BUILD=release")
                else:
                    args.append("BUILD=debug")
                   
            if options.shared:
                args.append("SHARED=1")

            if options.cairo:
                args.append(
                    "CPPFLAGS=/I%s" %
                     os.path.join(os.environ.get("CAIRO_ROOT", ""), 'include\\cairo'))
                
            if options.jom:
                nmakeCommand = 'jom.exe'
    
            wxBuilder = builder.MSVCBuilder(commandName=nmakeCommand)
            
        if toolkit == "msvcProject":
            args = []
            if options.shared or options.wxpython:
                args.append("wx_dll.dsw")
            else:
                args.append("wx.dsw")
                
            # TODO:
            wxBuilder = builder.MSVCProjectBuilder()
        
            
    if not wxBuilder:
        print("Builder not available for your specified platform/compiler.")
        sys.exit(1)
    
    if options.clean:
        print("Performing cleanup.")
        wxBuilder.clean(dir=buildDir, options=args)
        
        sys.exit(0)

    if options.extra_make:
        args.append(options.extra_make)
     
    if not sys.platform.startswith("win"):
        args.append("--jobs=" + options.jobs)
    exitIfError(wxBuilder.build(dir=buildDir, options=args), "Error building")
        
    if options.install:
        extra=None
        if installDir:
            extra = ['DESTDIR='+installDir]
        wxBuilder.install(dir=buildDir, options=extra)             
            
    if options.install and options.mac_framework:
    
        def renameLibrary(libname, frameworkname):
            reallib = libname
            links = []
            while os.path.islink(reallib):
                links.append(reallib)
                reallib = "lib/" + os.readlink(reallib)
                
            #print("reallib is %s" % reallib)
            run("mv -f %s lib/%s.dylib" % (reallib, frameworkname))
            
            for link in links:
                run("ln -s -f %s.dylib %s" % (frameworkname, link))
    
        frameworkRootDir = prefixDir
        if installDir:
            print("installDir = %s" % installDir)
            frameworkRootDir = installDir + prefixDir
        os.chdir(frameworkRootDir)
        build_string = ""
        if options.debug:
            build_string = "d"

        fwname = getFrameworkName(options)
        version = getoutput("bin/wx-config --release")
        version_full = getoutput("bin/wx-config --version")
        basename = getoutput("bin/wx-config --basename")
        configname = getoutput("bin/wx-config --selected-config")
        
        os.makedirs("Resources")
        wxplist = dict(
            CFBundleDevelopmentRegion="English",
            CFBundleIdentifier='org.wxwidgets.wxosxcocoa',
            CFBundleName=fwname,
            CFBundleVersion=version_full,
            CFBundleExecutable=fwname,
            CFBundleGetInfoString="%s %s" % (fwname, version_full),
            CFBundlePackageType="FMWK",
            CFBundleSignature="WXCO",
            CFBundleShortVersionString=version_full,
            CFBundleInfoDictionaryVersion="6.0",
        )
        
        import plistlib
        plistlib.writePlist(wxplist, os.path.join(frameworkRootDir, "Resources", "Info.plist"))
        
        # we make wx the "actual" library file and link to it from libwhatever.dylib
        # so that things can link to wx and survive minor version changes
        renameLibrary("lib/lib%s-%s.dylib" % (basename, version), fwname)
        run("ln -s -f lib/%s.dylib %s" % (fwname, fwname))
        
        run("ln -s -f include Headers")
        
        for lib in ["GL", "STC", "Gizmos", "Gizmos_xrc"]:  
            libfile = "lib/lib%s_%s-%s.dylib" % (basename, lib.lower(), version)
            if os.path.exists(libfile):
                frameworkDir = "framework/wx%s/%s" % (lib, version)
                if not os.path.exists(frameworkDir):
                    os.makedirs(frameworkDir)
                renameLibrary(libfile, "wx" + lib)
                run("ln -s -f ../../../%s %s/wx%s" % (libfile, frameworkDir, lib))        
        
        for lib in glob.glob("lib/*.dylib"):
            if not os.path.islink(lib):
                corelibname = "lib/lib%s-%s.0.dylib" % (basename, version)
                run("install_name_tool -id %s %s" % (os.path.join(prefixDir, lib), lib))
                run("install_name_tool -change %s %s %s" % (os.path.join(frameworkRootDir, corelibname), os.path.join(prefixDir, corelibname), lib))

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
            headers += "#include <wx/" + os.path.basename(include) + ">\n"
            
        framework_header = open("%s.h" % fwname, "w")
        framework_header.write(header_template % headers)
        framework_header.close()
        
        run("ln -s -f %s wx" % header_dir)
        os.chdir("wx-%s/wx" % version)
        run("ln -s -f ../../../lib/wx/include/%s/wx/setup.h setup.h" % configname)
        
        os.chdir(os.path.join(frameworkRootDir, ".."))
        run("ln -s -f %s Current" % getWxRelease())
        os.chdir("..")
        run("ln -s -f Versions/Current/Headers Headers")
        run("ln -s -f Versions/Current/Resources Resources")
        run("ln -s -f Versions/Current/%s %s" % (fwname, fwname))
        
        # sanity check to ensure the symlink works
        os.chdir("Versions/Current")
    
        # put info about the framework into wx-config
        os.chdir(frameworkRootDir)
        text = file('lib/wx/config/%s' % configname).read()
        text = text.replace("MAC_FRAMEWORK=", "MAC_FRAMEWORK=%s" % getFrameworkName(options))
        if options.mac_framework_prefix not in ['/Library/Frameworks',
                                                '/System/Library/Frameworks']:
            text = text.replace("MAC_FRAMEWORK_PREFIX=", 
                         "MAC_FRAMEWORK_PREFIX=%s" % options.mac_framework_prefix)
        file('lib/wx/config/%s' % configname, 'w').write(text)
        
        # The framework is finished!
        print("wxWidgets framework created at: " + 
              os.path.join( installDir, 
                            options.mac_framework_prefix,
                            '%s.framework' % fwname))
        
        
    # adjust the install_name if needed
    if sys.platform.startswith("darwin") and \
           options.install and \
           options.installdir and \
           not options.mac_framework and \
           not options.wxpython:  # wxPython's build will do this later if needed
        if not prefixDir:
            prefixDir = '/usr/local'
        macFixupInstallNames(options.installdir, prefixDir)#, buildDir)

    # make a package if a destdir was set.
    if options.mac_framework and \
            options.install and \
            options.installdir and \
            options.mac_distdir:

        if os.path.exists(options.mac_distdir):
            shutil.rmtree(options.mac_distdir)
        
        packagedir = os.path.join(options.mac_distdir, "packages")
        os.makedirs(packagedir)
        basename = os.path.basename(prefixDir.split(".")[0])
        packageName = basename + "-" + getWxRelease()
        packageMakerPath = getXcodePaths()[0]+"/usr/bin/packagemaker "
        args = []
        args.append("--root %s" % options.installdir)
        args.append("--id org.wxwidgets.%s" % basename.lower())
        args.append("--title %s" % packageName)
        args.append("--version %s" % getWxRelease())
        args.append("--out %s" % os.path.join(packagedir, packageName + ".pkg"))
        cmd = packageMakerPath + ' '.join(args)
        print("cmd = %s" % cmd)
        run(cmd)
        
        os.chdir(options.mac_distdir)
        
        run('hdiutil create -srcfolder %s -volname "%s" -imagekey zlib-level=9 %s.dmg' % (packagedir, packageName, packageName))
        
        shutil.rmtree(packagedir)
        
if __name__ == '__main__':
    exitWithException = False  # use sys.exit instead
    main(sys.argv[0], sys.argv[1:])
    
