import wx
from cStringIO import StringIO


class MyFileSystemHandler(wx.FileSystemHandler):    
    def CanOpen(self, location):
        print 'CanOpen:    ', self.this
        print '   location:', repr(location)
        print '   protocol:', repr(self.GetProtocol(location))
        print '   left:    ', repr(self.GetLeftLocation(location))
        print '   anchor:  ', repr(self.GetAnchor(location))
        print '   right:   ', repr(self.GetRightLocation(location))
        print '   mimetype:', repr(self.GetMimeTypeFromExt(location))
        return False

    def OpenFile(self, fs, location):
        print 'OpenFile:', self, fs, location
        fsfile = wx.FSFile(StringIO('the file data'),
                           location, 'text/plain', '', wx.DateTime.Now())
        return fsfile

wx.FileSystem.AddHandler(MyFileSystemHandler())

app = wx.App(False)

fs = wx.FileSystem()
fsf = fs.OpenFile('myprot://myfilename.txt')

