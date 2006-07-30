

from wxPython.wx import *
import glob

class File2(wxFileSystemHandler):

    def CanOpen(self,location):
        return self.GetProtocol(location) == "file2"

    def OpenFile(self,fs,location):
        return wxFSFile(wxInputStream(open(self.GetRightLocation(location),"rb")),
                                      location,"text/plain","",wxDateTime())

    def FindFirst(self,location,flags):
        # the flags are ignored
        self.files = glob.glob(self.GetRightLocation(location))
        if len(self.files) == 0:
            return ""
        else:
            return self.files[0]

    def FindNext(self):
        self.files = self.files[1:]
        if len(self.files) == 0:
            return ""
        else:
            return self.files[0]

# register new handler
wxFileSystem_AddHandler(File2())
fs = wxFileSystem()

# cat /etc/passwd
print fs.OpenFile("file2:/projects/files.lst").GetStream().read()

# ls /etc/*
fn = fs.FindFirst("file2:/projects/*")
while fn:
    print fn
    fn = fs.FindNext()
