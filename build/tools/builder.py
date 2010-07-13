import os
import string
import subprocess
import sys
import time

class BuildError(Exception):
    def __init__(self, value):
        self.value = value

    def __repr__(self):
        return repr(self.value)

def runInDir(command, dir=None, verbose=True):
    if dir:
        olddir = os.getcwd()
        os.chdir(dir)

    commandStr = " ".join(command)
    if verbose:
        print commandStr
    result = os.system(commandStr)

    if dir:
        os.chdir(olddir)

    return result

class Builder:
    """
    Base class exposing the Builder interface.
    """

    def __init__(self, formatName="", commandName="", programDir=None):
        """
        formatName = human readable name for project format (should correspond with Bakefile names)
        commandName = name of command line program used to invoke builder
        programDir = directory program is located in, if not on the path
        """

        self.dir = dir
        self.name = commandName
        self.formatName = formatName
        self.programDir = programDir
        self.doSetup()

    def doSetup(self):
        """
        Do anything special needed to configure the environment to build with this builder.
        """

        pass

    def isAvailable(self):
        """
        Run sanity checks before attempting to build with this format
        """
        # Make sure the builder program exists
        programPath = self.getProgramPath()
        if os.path.exists(programPath):
            return True
        else:
            # check the PATH for the program
            # TODO: How do we check if we're in Cygwin?
            if sys.platform.startswith("win"):
                result = os.system(self.name)
                if result == 0:
                    return True
                dirs = os.environ["PATH"].split(":")

                for dir in dirs:
                    if os.path.isfile(os.path.join(dir, self.name)):
                        return True  

            else:
                result = os.system("which %s" % self.name)

                if result == 0:
                    return True

        return False

    def getProgramPath(self):
        if self.programDir:
            path = os.path.join(self.programDir, self.name)
            if sys.platform.startswith("win"):
                path = '"%s"' % path
            return path

        return self.name

    def clean(self, dir=None, projectFile=None):
        """
        dir = the directory containing the project file
        projectFile = Some formats need to explicitly specify the project file's name
        """
        
        args = [self.getProgramPath(), "clean"]
        if dir:
            args.append(dir)
        if self.isAvailable():
            result = runInDir(args)
            return result

        return False

    def configure(self, options=None):
        # if we don't have configure, just report success
        return True

    def build(self, dir=None, projectFile=None, targets=None, options=None):
        if self.isAvailable():
            if options:
                optionList = list(options)
            else:
                optionList = []

            optionList.insert(0, self.getProgramPath())

            result = runInDir(optionList, dir)

            return result

        return 1

    def install(self, dir=None, options=None):
        if self.isAvailable():

            args = ["make", "install"]
            if options:
                args.extend(options)
            result = runInDir(args, dir)
            return result

        return 1

# Concrete subclasses of abstract Builder interface

class GNUMakeBuilder(Builder):
    def __init__(self, commandName="make", formatName="GNUMake"):
        Builder.__init__(self, commandName=commandName, formatName=formatName)


class XcodeBuilder(Builder):
    def __init__(self, commandName="xcodebuild", formatName="Xcode"):
        Builder.__init__(self, commandName=commandName, formatName=formatName)


class AutoconfBuilder(GNUMakeBuilder):
    def __init__(self, formatName="autoconf"):
        GNUMakeBuilder.__init__(self, formatName=formatName)

    def configure(self, dir=None, options=None):
        #olddir = os.getcwd()
        #os.chdir(dir)

        configdir = dir
        if not dir:
            configdir = os.getcwd()

        configure_cmd = ""
        while os.path.exists(configdir):
            config_cmd = os.path.join(configdir, "configure")
            if not os.path.exists(config_cmd):
                parentdir = os.path.abspath(os.path.join(configdir, ".."))
                if configdir == parentdir:
                    break

                configdir = parentdir 
            else:
                configure_cmd = config_cmd
                break

        if not configure_cmd:
            sys.stderr.write("Could not find configure script at %r. Have you run autoconf?\n" % dir)
            return 1

        optionsStr = string.join(options, " ") if options else ""
        command = "%s %s" % (configure_cmd, optionsStr)
        print command
        result = os.system(command)
        #os.chdir(olddir)
        return result


class MSVCBuilder(Builder):
    def __init__(self):
        Builder.__init__(self, commandName="nmake.exe", formatName="msvc")

    def isAvailable(self):
        PATH = os.environ['PATH'].split(os.path.pathsep)
        for p in PATH:
            if os.path.exists(os.path.join(p, self.name)):
                return True
        return False

        
class MSVCProjectBuilder(Builder):
    def __init__(self):
        Builder.__init__(self, commandName="VCExpress.exe", formatName="msvcProject")
        for key in ["VS90COMNTOOLS", "VC80COMNTOOLS", "VC71COMNTOOLS"]:
            if os.environ.has_key(key):
                self.prgoramDir = os.path.join(os.environ[key], "..", "IDE")

        if self.programDir == None:
            for version in ["9.0", "8", ".NET 2003"]:
                msvcDir = "C:\\Program Files\\Microsoft Visual Studio %s\\Common7\\IDE" % version
                if os.path.exists(msvcDir):
                    self.programDir = msvcDir

    def isAvailable(self):
        if self.programDir:
            path = os.path.join(self.programDir, self.name)
            if os.path.exists(path):
                return True
            else:
                # I don't have commercial versions of MSVC so I can't test this
                name = "devenv.com"
                path = os.path.join(self.programDir, name)
                if os.path.exists(path):
                    self.name = "devenv.com"
                    return True

        return False

builders = [GNUMakeBuilder, XcodeBuilder, AutoconfBuilder, MSVCBuilder, MSVCProjectBuilder]

def getAvailableBuilders():
    availableBuilders = {}
    for symbol in builders:
        thisBuilder = symbol()
        if thisBuilder.isAvailable():
            availableBuilders[thisBuilder.formatName] = symbol

    return availableBuilders
