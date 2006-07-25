import distutils.command.install_lib
import distutils.command.install_data
import distutils.command.install
import os, string
from distutils.core import setup
from user import home # this gives us the user's home dir on all platforms

class wxaddon_install_lib(distutils.command.install_lib.install_lib):
    """need to change self.install_dir to the actual library dir"""
    def run(self):
        install_cmd = self.get_finalized_command('install')
        self.install_dir = os.path.join(getattr(install_cmd, 'install_purelib'), "wxaddons")
        return distutils.command.install_lib.install_lib.run(self)
        
class wxaddon_install_data(distutils.command.install_data.install_data):
    def run(self):
        self.install_dir = os.path.join(home, ".wxaddons_data", self.distribution.get_name())
        if not os.path.exists(self.install_dir):
            os.makedirs(self.install_dir)
        return distutils.command.install_data.install_data.run(self)

class wxaddon_install(distutils.command.install.install):
    def run(self):
        result = distutils.command.install.install.run(self)
                
        import wx
        config = wx.Config("wxaddons-receipts")
        config.SetPath(self.distribution.get_name())
        config.Write("Version", str(self.distribution.get_version()))
        config.Write("Summary", self.distribution.get_description())
        config.Write("Home-page", self.distribution.get_url())
        
        # NB: Although self.distribution has get_author() and get_author_email()
        # methods, get_contact* returns either author or maintainer, and I think
        # either is suitable for our purposes.
        config.Write("Author", self.distribution.get_contact())
        config.Write("Author-email", self.distribution.get_contact_email())
        config.Write("License", self.distribution.get_license())
        
        keywords = string.join( self.distribution.get_keywords(), ';')
        if keywords:
            config.Write('Keywords', keywords)

        platforms = string.join( self.distribution.get_platforms(), ';')
        if platforms:
            config.Write('Platforms', platforms )

        classifiers = string.join( self.distribution.get_classifiers(), ';')
        if classifiers:
            config.Write('Classifiers', classifiers )

        return result     
        
def wxaddon(**kwargs):
    kwargs['cmdclass'] =  {'install_lib' :    wxaddon_install_lib, 
                           'install' :  wxaddon_install,
                           'install_data' : wxaddon_install_data }
    setup(**kwargs)