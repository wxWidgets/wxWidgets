#----------------------------------------------------------------------
# Name:        __init__.py
# Purpose:     Import logic and common functions for wxaddons module
#
# Author:      Kevin Ollivier
#
# Created:     15-Nov-2006
# RCS-ID:      $Id$
# Copyright:   (c) 2006 Kevin Ollivier
# Licence:     wxWindows license
#----------------------------------------------------------------------

import sys, os, string
import xmlrpclib
import __builtin__
import wx

# NB: For some reason that I haven't been able to track down, on Mac (at least)
# calling xmlrpc methods no longer works after the wx.App is started. Therefore, 
# we grab the package URL even before prompting the user if they want to install
# the package in order for us to have the info we need before the wx.App is started.

domain = 'http://wxaddons.wxcommunity.com'
builtin_import = __builtin__.__import__

debug = False
use_gui = True
checkImports = True
config = wx.Config("wxaddons")
if config.Read("PerformChecks", "true") != "true":
    checkImports = False

if use_gui and not wx.App.IsDisplayAvailable():
    use_gui = False

s = xmlrpclib.Server('%s/xmlrpc-server.php' % domain, verbose=(debug == True)) 

def check_imports(check):
    if check:
        config.Write("PerformChecks", "true")
    else:
        config.Write("PerformChecks", "false")

def version_greater_than_or_equal(version1, version2):
    """
    Checks if version1 >= version2, returning true if so,
    false if otherwise.
    """
    greater_than = True
    
    for index in range(0, len(version1)-1):
        if version1[index] > version2[index]:
            greater_than = True
            break
        elif version[index] < current_version[index]:
            greater_than = False
            break
            
    return greater_than
        
def prompt_install(name, version):
    should_install = False
    message = "The wxaddon %s is not installed, but was found on the wxaddons site. Would you like to download and install it?" % (name + " " + version)
    if use_gui:
        app = wx.PySimpleApp()
        app.MainLoop()
        result = wx.MessageBox(message, "Install Dependency?", style=wx.YES_NO)
        if result == wx.YES:
            should_install = True
    else:
        result = raw_input(message + " [y/n]")
        if result[0].lower() == "y":
            should_install = True
            
    return should_install
                        
def require_addon_version(name, version=[], canBeNewer=True):
    # Check the install receipt to see if we've got an appropriate version 
    config = wx.Config("wxaddons-receipts")
    needs_update = True
    if config.HasGroup(name):
        config.SetPath(name)
        current_version = config.Read("version", "0.0").split(".")
        
        needs_update = version_greater_than_or_equal(version, current_version)
        
    if needs_update:
        comp_xml = s.getComponent(name)
        if not comp_xml:
            raise
        
        comp = xmlrpclib.loads(comp_xml)[0][0]
        comp_version = comp["version"].split(".")
        
        update_comp = False
        if canBeNewer:
            update_comp = version_greater_than_or_equal(comp_version, version)
        else:
            update_comp = (version == comp_version)
            
        if update_comp:
            url = get_url(name, version)
            should_install = prompt_install(name, comp_version)
            
            if should_install:
                dl_and_install_addon(name, comp_version, url)

def get_url(name, version):
    url = ""
    release_xml = s.getReleases(name)
    if not release_xml:
        return ""
        
    releases = xmlrpclib.loads(release_xml)[0][0]
    for release in releases:
        if release['version'] == version:
            url = release['url']
            
    return url
    
def dl_and_install_addon(name, version, url):
    installed = True
    tempdir = None
    cwd = os.getcwd()
    
    if use_gui:
        progress = wx.ProgressDialog("Installing Dependency", 
            "Preparing to install the %s addon module." % name, 
            4,
            style=wx.PD_APP_MODAL|wx.PD_SMOOTH)
                                
    message = "Downloading tarball..."
    print message
    if use_gui: progress.Update(1, message)
    import urllib
    temp_archive, headers = urllib.urlretrieve(url)
    
    message = "Extracting files..."
    print message
    if use_gui: progress.Update(2, message)
    import tempfile
    tempdir = tempfile.mkdtemp()
    
    os.chdir(tempdir)
    import tarfile
    tar = tarfile.open(temp_archive, "r:gz")
    for tarinfo in tar:
        tar.extract(tarinfo)
    tar.close()
    
    os.chdir(name)
    
    message = "Installing %s" % name
    if use_gui: progress.Update(3, message)
    # TODO: Add support for modified PYTHONPATH?
    # Also, do we need admin install support here?
    retval = os.system(sys.executable + " " + string.join(("setup.py", "install"), " "))
    if use_gui: progress.Update(4)
    
    if retval == 0:
        message = "The %s addon was successfully installed." % name
        print message
        if use_gui:
            wx.MessageBox(message, "Installation Successful")
    else:
        installed = False
        
    # cleanup
    if use_gui: progress.Destroy()
    os.chdir(cwd)
    import shutil
    shutil.rmtree(tempdir)
    os.remove(temp_archive)
    
    return installed
                        
def import_hook(name, globals=None, locals=None, fromlist=None):
    # Fast path: see if the module has already been imported.
    try:
        return builtin_import(name, globals, locals, fromlist)
    except:
        if name.startswith("wxaddons"):
            print "Querying %s for module." % domain
            try:
                modname = name.split(".")[1]
                comp = None
                comp_xml = s.getComponent(modname)
                if not comp_xml:
                    raise
                
                comp = xmlrpclib.loads(comp_xml)[0][0]                
                url = get_url(comp["name"], comp["version"])
                should_install = prompt_install(comp["name"], comp["version"])

                if should_install:
                    try:
                        success = dl_and_install_addon(comp["name"], comp["version"], url)
                        if not success:
                            raise
                    except:
                        raise
                    
                
            except:
                raise
        else:
            raise

def runTests():
    import wxaddons.persistence
    import wxaddons.foo_bar
    import googly

if checkImports:
    __builtin__.__import__ = import_hook

