import distutils.command.install_lib
import distutils.command.install
import os
from distutils.core import setup

class wxaddon_install_lib(distutils.command.install_lib.install_lib):
    """need to change self.install_dir to the actual library dir"""
    def run(self):
        install_cmd = self.get_finalized_command('install')
        self.install_dir = os.path.join(getattr(install_cmd, 'install_purelib'), "wxaddons")
        return distutils.command.install_lib.install_lib.run(self)
        
class wxaddon_install(distutils.command.install.install):
    def run(self):
        result = distutils.command.install.install.run(self)
                
        metadata_file = 'addon.info'
        if os.path.exists(metadata_file):
            import wx
            import email
            fields = email.message_from_string(open(metadata_file).read())
            config = wx.Config("wxaddons-receipts")
            config.SetPath(fields['name'])
            for field in fields._headers:
                config.Write(field[0], field[1])
        return result     
        
def wxaddon(**kwargs):
    kwargs['cmdclass'] =  {'install_lib' :    wxaddon_install_lib, 
                           'install' :  wxaddon_install }
    setup(**kwargs)