#----------------------------------------------------------------------
# Name:        wxversion
# Purpose:     Allows a wxPython program to search for alternate 
#              installations of the wxPython packages and modify sys.path
#              so they will be found when "import wx" is done.
#
# Author:      Robin Dunn
#
# Created:     24-Sept-2004
# RCS-ID:      $Id$
# Copyright:   (c) 2004 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------

"""
If you have more than one version of wxPython installed this module
allows your application to choose which version of wxPython will be
imported when it does 'import wx'.  The main function of this module
is `select` and you use it like this::

    import wxversion
    wxversion.select('2.4')
    import wx

Or additional build options can also be selected, although they will
not be required if they are not installed, like this::

    import wxversion
    wxversion.select('2.5.3-unicode')
    import wx

Or you can require an exact match on the build options like this::

    import wxversion
    wxversion.select('2.5.3-unicode', optionsRequired=True)
    import wx

Finally you can also specify a collection of versions that are allowed
by your application, like this::

    import wxversion
    wxversion.select(['2.5.4', '2.5.5', '2.6'])
    import wx


Of course the default wxPython version can also be controlled by
setting PYTHONPATH or by editing the wx.pth path configuration file,
but using wxversion will allow an application to manage the version
selection itself rather than depend on the user to setup the
environment correctly.

It works by searching the sys.path for directories matching wx-* and
then comparing them to what was passed to the select function.  If a
match is found then that path is inserted into sys.path.

NOTE: If you are making a 'bundle' of your application with a tool
like py2exe then you should *not* use the wxversion module since it
looks at the filesystem for the directories on sys.path, it will fail
in a bundled environment.  Instead you should simply ensure that the
version of wxPython that you want is found by default on the sys.path
when making the bundled version by setting PYTHONPATH.  Then that
version will be included in your bundle and your app will work as
expected.  Py2exe and the others usually have a way to tell at runtime
if they are running from a bundle or running raw, so you can check
that and only use wxversion if needed.  For example, for py2exe::

    if not hasattr(sys, 'frozen'):
        import wxversion
        wxversion.select('2.5')
    import wx

More documentation on wxversion and multi-version installs can be
found at: http://wiki.wxpython.org/index.cgi/MultiVersionInstalls

"""

import sys, os, glob, fnmatch


_selected = None
class VersionError(Exception):
    pass

#----------------------------------------------------------------------

def select(versions, optionsRequired=False):
    """
    Search for a wxPython installation that matches version.  If one
    is found then sys.path is modified so that version will be
    imported with a 'import wx', otherwise a VersionError exception is
    raised.  This funciton should only be caled once at the begining
    of the application before wxPython is imported.

        :param versions: Specifies the version to look for, it can
            either be a string or a list of strings.  Each string is
            compared to the installed wxPythons and the best match is
            inserted into the sys.path, allowing an 'import wx' to
            find that version.

            The version string is composed of the dotted version
            number (at least 2 of the 4 components) optionally
            followed by hyphen ('-') separated options (wx port,
            unicode/ansi, flavour, etc.)  A match is determined by how
            much of the installed version matches what is given in the
            version parameter.  If the version number components don't
            match then the score is zero, otherwise the score is
            increased for every specified optional component that is
            specified and that matches.

            Please note, however, that it is possible for a match to
            be selected that doesn't exactly match the versions
            requested.  The only component that is required to be
            matched is the version number.  If you need to require a
            match on the other components as well, then please use the
            optional ``optionsRequired`` parameter described next.

        :param optionsRequired: Allows you to specify that the other
             components of the version string (such as the port name
             or character type) are also required to be present for an
             installed version to be considered a match.  Using this
             parameter allows you to change the selection from a soft,
             as close as possible match to a hard, exact match.
        
    """
    if type(versions) == str:
        versions = [versions]

    global _selected
    if _selected is not None:
        # A version was previously selected, ensure that it matches
        # this new request
        for ver in versions:
            if _selected.Score(_wxPackageInfo(ver), optionsRequired) > 0:
                return
        # otherwise, raise an exception
        raise VersionError("A previously selected wx version does not match the new request.")

    # If we get here then this is the first time wxversion is used, 
    # ensure that wxPython hasn't been imported yet.
    if sys.modules.has_key('wx') or sys.modules.has_key('wxPython'):
        raise VersionError("wxversion.select() must be called before wxPython is imported")
    
    # Look for a matching version and manipulate the sys.path as
    # needed to allow it to be imported.
    installed = _find_installed(True)
    bestMatch = _get_best_match(installed, versions, optionsRequired)
    
    if bestMatch is None:
        raise VersionError("Requested version of wxPython not found")

    sys.path.insert(0, bestMatch.pathname)
    _selected = bestMatch
        
#----------------------------------------------------------------------

UPDATE_URL = "http://wxPython.org/"
#UPDATE_URL = "http://sourceforge.net/project/showfiles.php?group_id=10718"

_EM_DEBUG=0

def ensureMinimal(minVersion, optionsRequired=False):
    """
    Checks to see if the default version of wxPython is greater-than
    or equal to `minVersion`.  If not then it will try to find an
    installed version that is >= minVersion.  If none are available
    then a message is displayed that will inform the user and will
    offer to open their web browser to the wxPython downloads page,
    and will then exit the application.
    """
    assert type(minVersion) == str

    # ensure that wxPython hasn't been imported yet.
    if sys.modules.has_key('wx') or sys.modules.has_key('wxPython'):
        raise VersionError("wxversion.ensureMinimal() must be called before wxPython is imported")

    bestMatch = None
    minv = _wxPackageInfo(minVersion)

    # check the default version first
    defaultPath = _find_default()
    if defaultPath:
        defv = _wxPackageInfo(defaultPath, True)
        if defv >= minv and minv.CheckOptions(defv, optionsRequired):
            bestMatch = defv

    # if still no match then check look at all installed versions
    if bestMatch is None:
        installed = _find_installed()
        # The list is in reverse sorted order, so find the first
        # one that is big enough and optionally matches the
        # options
        for inst in installed:
            if inst >= minv and minv.CheckOptions(inst, optionsRequired):
                bestMatch = inst
                break

    # if still no match then prompt the user
    if bestMatch is None:
        if _EM_DEBUG: # We'll do it this way just for the test code below
            raise VersionError("Requested version of wxPython not found")
        
        import wx, webbrowser
        versions = "\n".join(["      "+ver for ver in getInstalled()])
        app = wx.PySimpleApp()
        result = wx.MessageBox("This application requires a version of wxPython "
                               "greater than or equal to %s, but a matching version "
                               "was not found.\n\n"
                               "You currently have these version(s) installed:\n%s\n\n"
                               "Would you like to download a new version of wxPython?\n"
                               % (minVersion, versions),
                      "wxPython Upgrade Needed", style=wx.YES_NO)
        if result == wx.YES:
            webbrowser.open(UPDATE_URL)
        app.MainLoop()
        sys.exit()

    sys.path.insert(0, bestMatch.pathname)
    global _selected
    _selected = bestMatch
        

#----------------------------------------------------------------------

def checkInstalled(versions, optionsRequired=False):
    """
    Check if there is a version of wxPython installed that matches one
    of the versions given.  Returns True if so, False if not.  This
    can be used to determine if calling `select` will succeed or not.

        :param versions: Same as in `select`, either a string or a list
            of strings specifying the version(s) to check for.

        :param optionsRequired: Same as in `select`.
    """
    
    if type(versions) == str:
        versions = [versions]
    installed = _find_installed()
    bestMatch = _get_best_match(installed, versions, optionsRequired)
    return bestMatch is not None

#----------------------------------------------------------------------

def getInstalled():
    """
    Returns a list of strings representing the installed wxPython
    versions that are found on the system.
    """
    installed = _find_installed()
    return [os.path.basename(p.pathname)[3:] for p in installed]



#----------------------------------------------------------------------
# private helpers...

def _get_best_match(installed, versions, optionsRequired):
    bestMatch = None
    bestScore = 0
    for pkg in installed:
        for ver in versions:
            score = pkg.Score(_wxPackageInfo(ver), optionsRequired)
            if score > bestScore:
                bestMatch = pkg
                bestScore = score
    return bestMatch


_pattern = "wx-[0-9].*"
def _find_installed(removeExisting=False):
    installed = []
    toRemove = []
    for pth in sys.path:

        # empty means to look in the current dir
        if not pth:
            pth = '.'

        # skip it if it's not a package dir
        if not os.path.isdir(pth):
            continue
        
        base = os.path.basename(pth)

        # if it's a wx path that's already in the sys.path then mark
        # it for removal and then skip it
        if fnmatch.fnmatchcase(base, _pattern):
            toRemove.append(pth)
            continue

        # now look in the dir for matching subdirs
        for name in glob.glob(os.path.join(pth, _pattern)):
            # make sure it's a directory
            if not os.path.isdir(name):
                continue
            # and has a wx subdir
            if not os.path.exists(os.path.join(name, 'wx')):
                continue
            installed.append(_wxPackageInfo(name, True))

    if removeExisting:
        for rem in toRemove:
            del sys.path[sys.path.index(rem)]
        
    installed.sort()
    installed.reverse()
    return installed


# Scan the sys.path looking for either a directory matching _pattern,
# or a wx.pth file
def _find_default():
    for pth in sys.path:
        # empty means to look in the current dir
        if not pth:
            pth = '.'

        # skip it if it's not a package dir
        if not os.path.isdir(pth):
            continue
        
        # does it match the pattern?
        base = os.path.basename(pth)
        if fnmatch.fnmatchcase(base, _pattern):
            return pth

    for pth in sys.path:
        if not pth:
            pth = '.'
        if not os.path.isdir(pth):
            continue
        if os.path.exists(os.path.join(pth, 'wx.pth')):
            base = open(os.path.join(pth, 'wx.pth')).read()
            return os.path.join(pth, base)

    return None


class _wxPackageInfo(object):
    def __init__(self, pathname, stripFirst=False):
        self.pathname = pathname
        base = os.path.basename(pathname)
        segments = base.split('-')
        if stripFirst:
            segments = segments[1:]
        self.version = tuple([int(x) for x in segments[0].split('.')])
        self.options = segments[1:]


    def Score(self, other, optionsRequired):
        score = 0
        
        # whatever number of version components given in other must
        # match exactly
        minlen = min(len(self.version), len(other.version))
        if self.version[:minlen] != other.version[:minlen]:
            return 0        
        score += 1

        # check for matching options, if optionsRequired then the
        # options are not optional ;-)
        for opt in other.options:
            if opt in self.options:
                score += 1
            elif optionsRequired:
                return 0
            
        return score

    
    def CheckOptions(self, other, optionsRequired):
        # if options are not required then this always succeeds
        if not optionsRequired:
            return True
        # otherwise, if we have any option not present in other, then
        # the match fails.
        for opt in self.options:
            if opt not in other.options:
                return False
        return True

            
   
    def __lt__(self, other):
        return self.version < other.version or \
               (self.version == other.version and self.options < other.options)
    def __le__(self, other):
        return self.version <= other.version or \
               (self.version == other.version and self.options <= other.options)
    
    def __gt__(self, other):
        return self.version > other.version or \
               (self.version == other.version and self.options > other.options)
    def __ge__(self, other):
        return self.version >= other.version or \
               (self.version == other.version and self.options >= other.options)
    
    def __eq__(self, other):
        return self.version == other.version and self.options == other.options
        
    

#----------------------------------------------------------------------

if __name__ == '__main__':
    import pprint

    #ensureMinimal('2.5')
    #pprint.pprint(sys.path)
    #sys.exit()
    
    
    def test(version, optionsRequired=False):
        # setup
        savepath = sys.path[:]

        #test
        select(version, optionsRequired)
        print "Asked for %s, (%s):\t got: %s" % (version, optionsRequired, sys.path[0])

        # reset
        sys.path = savepath[:]
        global _selected
        _selected = None


    def testEM(version, optionsRequired=False):
        # setup
        savepath = sys.path[:]

        #test
        ensureMinimal(version, optionsRequired)
        print "EM: Asked for %s, (%s):\t got: %s" % (version, optionsRequired, sys.path[0])

        # reset
        sys.path = savepath[:]
        global _selected
        _selected = None
        
        
    # make some test dirs
    names = ['wx-2.4-gtk-ansi',
             'wx-2.5.2-gtk2-unicode',
             'wx-2.5.3-gtk-ansi',
             'wx-2.6-gtk2-unicode',
             'wx-2.6-gtk2-ansi',
             'wx-2.6-gtk-ansi',
             'wx-2.7.1-gtk2-ansi',
             ]
    for name in names:
        d = os.path.join('/tmp', name)
        os.mkdir(d)
        os.mkdir(os.path.join(d, 'wx'))

    # setup sys.path to see those dirs
    sys.path.append('/tmp')
    

    # now run some tests
    pprint.pprint( getInstalled())
    print checkInstalled("2.4")
    print checkInstalled("2.5-unicode")
    print checkInstalled("2.99-bogus")
    print "Current sys.path:"
    pprint.pprint(sys.path)
    print
    
    test("2.4")
    test("2.5")
    test("2.5-gtk2")
    test("2.5.2")
    test("2.5-ansi")
    test("2.5-unicode")
    test("2.6")
    test("2.6-ansi")
    test(["2.6-unicode", "2.7-unicode"])
    test(["2.6", "2.7"])
    test(["2.6-unicode", "2.7-unicode"], optionsRequired=True)
    
    
    
    # There isn't a unicode match for this one, but it will give the best
    # available 2.4.  Should it give an error instead?  I don't think so...
    test("2.4-unicode") 

    # Try asking for multiple versions
    test(["2.5.2", "2.5.3", "2.6"])

    try:
        # expecting an error on this one
        test("2.9")
    except VersionError, e:
        print "Asked for 2.9:\t got Exception:", e 

    # check for exception when incompatible versions are requested
    try:
        select("2.4")
        select("2.5")
    except VersionError, e:
        print "Asked for incompatible versions, got Exception:", e 

    _EM_DEBUG=1
    testEM("2.6")
    testEM("2.6-unicode")
    testEM("2.6-unicode", True)
    try:
        testEM("2.9")
    except VersionError, e:
        print "EM: Asked for 2.9:\t got Exception:", e 

    # cleanup
    for name in names:
        d = os.path.join('/tmp', name)
        os.rmdir(os.path.join(d, 'wx'))
        os.rmdir(d)

        
