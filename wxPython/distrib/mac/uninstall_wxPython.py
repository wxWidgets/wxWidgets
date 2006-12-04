#!/usr/bin/env python
"""
This script will search for installed versions of wxPython on OSX and
allow the user to choose one to uninstall.  It then will use the
metadata stored about the installed package to remove all the files
associated with that install.

Only the files installed by the main Installer Package will be
removed.  This includes the Python modules and the wxWidgets shared
libraries. If you also installed the demo or docs by dragging them out
of the disk image, then you will need to drag them to the Trash
yourself.
"""

import sys, os, glob
from fnmatch import fnmatchcase
import cPickle, urllib

RCPTDIR = "/Library/Receipts"
RSRCDIR = "Contents/Resources"

# Only completly clean out dirs that have one of these as a prefix.
# We do this because the file list returned from lsbom will include /,
# /usr, /usr/local, etc.
PREFIXES = [ '/Library/Python/2.3/',
             '/Library/Python/2.4/',
             '/Library/Frameworks/Python.framework/Versions/2.3/lib/python2.3/site-packages/',
             '/Library/Frameworks/Python.framework/Versions/2.4/lib/python2.4/site-packages/',
             '/usr/local/lib/',
             ]

# The files that match one of the items in this list will only be
# removed if the last installation of wxPython on the system is being
# uninstalled.
COMMON_FILES = [ '/usr/local/bin/*',
                 'wx.pth',
                 'wxversion.py',
                 ]
    


class AccessError(Exception):
    pass

class ReceiptError(Exception):
    pass


class InstalledReceipt(object):
    def __init__(self, rcptPath):
        self.rcptPath = rcptPath
        self.rsrcPath = os.path.join(rcptPath, RSRCDIR)
        bf = glob.glob(os.path.join(self.rsrcPath, "*.bom"))
        if bf:
            self.bomFile = bf[0]
        else:
            print "WARNING: Unable to find %s/*.bom" % self.rsrcPath
            raise ReceiptError
        self.findMetaData()


    def findMetaData(self):
        # TODO: Make this be able to also look at Info.plist files
        infoFiles = glob.glob(os.path.join(self.rsrcPath, "*.info"))
        if infoFiles:
            # there should be only one
            infoFile = infoFiles[0]
            self.mdata = {}
            for line in open(infoFile, "r").readlines():
                line = line.strip()
                if line and line[0] != '#':
                    ls = line.split()
                    self.mdata[ls[0]] = line[len(ls[0])+1:]
        else:
            print "WARNING: Unable to find %s/*.info" % self.rsrcPath
            raise ReceiptError
            

    def getFileList(self):
        p = os.popen("lsbom -s %s" % self.bomFile, "r")
        data = p.read()
        data.strip()
        data = filter(lambda s: s!='' and s!='.', data.split('\n'))
        loc = self.mdata['DefaultLocation']
        return [loc+item for item in data]


    def walkFiles(self, handleFile, handleDir):
        dirs = []
        names = self.getFileList()

        # the plain files
        for name in names:
            name = os.path.abspath(name)
            if os.path.isdir(name):
                dirs.append(name)
            else:
                handleFile(name)

        # the directories
        dirs.reverse()
        for dir in dirs:
            for prefix in PREFIXES:
                if dir.startswith(prefix):
                    handleDir(dir)
                    break

        # Finally, remove the Receipts package, bottom-up
        for dirpath, dirname, filenames in os.walk(self.rcptPath, False):
            for name in filenames:
                name = os.path.join(dirpath, name)
                handleFile(name)
            handleDir(dirpath)

    # wxaddons should be always kept as the user may have installed
    # third-party modules seperate from wxpython.
    def testWxaddons(self, name):
        for prefix in PREFIXES:
            if name.startswith(prefix + "wxaddons"):
                return True
        return False

    def testCommon(self, name):
        for cmn in COMMON_FILES:
            if fnmatchcase(name, cmn) or fnmatchcase(os.path.basename(name), cmn):
                return True
        return False


    def showFiles(self):
        def show(name):
            if os.path.exists(name):
                if not self.lastInstall and self.testCommon(name):
                    return
                if self.testWxaddons(name):
                    return
                print "Will remove:", name
        self.walkFiles(show, show)


    def testUninstallAccess(self):
        def testFile(name):
            if os.path.exists(name):
                if not self.lastInstall and self.testCommon(name):
                    return
                if not os.access(name, os.W_OK):
                    raise AccessError(name)
        self.walkFiles(testFile, testFile)


    def doUninstall(self):
        def removeFile(name):
            if os.path.exists(name):
                if not self.lastInstall and self.testCommon(name):
                    return
                if self.testWxaddons(name):
                    return
                print "Removing:", name
                os.unlink(name)
        def removeDir(name):
            print "Removing:", name
            if os.path.exists(name) and not self.testWxaddons(name):
                hasFiles = os.listdir(name)
                if hasFiles:  # perhaps some stale symlinks, or .pyc files
                    for file in hasFiles:
                        os.unlink(os.path.join(name, file))
                os.rmdir(name)

        try:
            self.testUninstallAccess()
        except AccessError, e:
            print "UNABLE TO UNINSTALL!\nNo permission to remove: ", e.args[0]
            sys.exit()
            
        self.walkFiles(removeFile, removeDir)




def findInstalled():
    installed = []
    for name in glob.glob(os.path.join(RCPTDIR, "wxPython*")):
        try:
            ir = InstalledReceipt(name)
            installed.append(ir)
        except ReceiptError:
            pass  # just skip it...

    return installed


# Just in case a Python < 2.3 is used to run this
try:
    enumerate
except NameError:
    def enumerate(sequence):
        return zip(range(len(sequence)), sequence)


def main():
    if len(sys.argv) > 1 and sys.argv[1] == "-doit":
        inst = cPickle.loads(urllib.unquote(sys.argv[2]))
        inst.doUninstall()
        sys.exit()
        
    print __doc__
    installed = findInstalled()

    if not installed:
        print "*** No wxPython installations found!  ***"
        raw_input("Press RETURN...")
        sys.exit()
        
    for i, inst in enumerate(installed):
        print " %2d.  %-40s    %s" % (i+1, inst.mdata["Title"], inst.mdata["Version"])
        
    print 
    ans = raw_input("Enter the number of the install to examine or 'Q' to quit: ")
    if ans in ['Q', 'q']:
        sys.exit()
    inst = installed[int(ans) - 1]
    inst.lastInstall = len(installed) == 1

    while True:
        print
        print """
        Title:       %(Title)s
        Version:     %(Version)s
        Description: %(Description)s
        """ % inst.mdata

        ans = raw_input("(U)ninstall, (S)how what will be removed, or (Q)uit? [u,s,q] ")
        if ans in ['Q', 'q']:
            sys.exit()

        elif ans in ['S', 's']:
            inst.showFiles()

        elif ans in ['U', 'u']:
            print
            print "Launching uninstaller with sudo, please enter your password if prompted:"
            os.system("sudo %s -doit %s" %
                      (sys.argv[0],
                       urllib.quote(cPickle.dumps(inst))))
            sys.exit()
        
    
if __name__ == '__main__':
    main()
