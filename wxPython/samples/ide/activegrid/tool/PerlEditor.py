#----------------------------------------------------------------------------
# Name:         PerlEditor.py
# Purpose:      Perl Script Editor for pydocview tbat uses the Styled Text Control
#
# Author:       Morgan Hua
#
# Created:      1/5/04
# CVS-ID:       $Id$
# Copyright:    (c) 2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------

import wx
import string
import STCTextEditor
import CodeEditor


class PerlDocument(CodeEditor.CodeDocument):

    pass


class PerlView(CodeEditor.CodeView):


    def GetCtrlClass(self):
        """ Used in split window to instantiate new instances """
        return PerlCtrl


    def GetAutoCompleteHint(self):
        pos = self.GetCtrl().GetCurrentPos()
        if pos == 0:
            return None, None
            
        validLetters = string.letters + string.digits + '_/'
        word = ''
        while (True):
            pos = pos - 1
            if pos < 0:
                break
            char = chr(self.GetCtrl().GetCharAt(pos))
            if char not in validLetters:
                break
            word = char + word
            
        return None, word


    def GetAutoCompleteDefaultKeywords(self):
        return PERLKEYWORDS


class PerlService(CodeEditor.CodeService):


    def __init__(self):
        CodeEditor.CodeService.__init__(self)


class PerlCtrl(CodeEditor.CodeCtrl):


    def __init__(self, parent, ID = -1, style = wx.NO_FULL_REPAINT_ON_RESIZE):
        CodeEditor.CodeCtrl.__init__(self, parent, ID, style)
        self.SetLexer(wx.stc.STC_LEX_PERL)
        self.SetKeyWords(0, string.join(PERLKEYWORDS))


    def CanWordWrap(self):
        return True


    def SetViewDefaults(self):
        CodeEditor.CodeCtrl.SetViewDefaults(self, configPrefix = "Perl", hasWordWrap = True, hasTabs = True)


    def GetFontAndColorFromConfig(self):
        return CodeEditor.CodeCtrl.GetFontAndColorFromConfig(self, configPrefix = "Perl")


    def UpdateStyles(self):
        CodeEditor.CodeCtrl.UpdateStyles(self)
        
        if not self.GetFont():
            return

        faces = { 'font' : self.GetFont().GetFaceName(),
                  'size' : self.GetFont().GetPointSize(),
                  'size2': self.GetFont().GetPointSize() - 2,
                  'color' : "%02x%02x%02x" % (self.GetFontColor().Red(), self.GetFontColor().Green(), self.GetFontColor().Blue())
                  }

        # Perl Styles
        self.StyleSetSpec(wx.stc.STC_PL_DEFAULT, "face:%(font)s,fore:#000000,face:%(font)s,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_PL_COMMENTLINE, "face:%(font)s,fore:#007F00,italic,face:%(font)s,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_PL_NUMBER, "face:%(font)s,fore:#007F7F,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_PL_CHARACTER, "face:%(font)s,fore:#7F007F,face:%(font)s,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_PL_STRING, "face:%(font)s,fore:#7F007F,face:%(font)s,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_PL_STRING_Q, "face:%(font)s,fore:#7F007F,face:%(font)s,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_PL_STRING_QQ, "face:%(font)s,fore:#7F007F,face:%(font)s,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_PL_STRING_QX, "face:%(font)s,fore:#7F007F,face:%(font)s,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_PL_STRING_QR, "face:%(font)s,fore:#7F007F,face:%(font)s,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_PL_STRING_QW, "face:%(font)s,fore:#7F007F,face:%(font)s,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_PL_BACKTICKS, "face:%(font)s,fore:#7F007F,face:%(font)s,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_PL_WORD, "face:%(font)s,fore:#00007F,bold,size:%(size)d" % faces) # keyword
        self.StyleSetSpec(wx.stc.STC_PL_IDENTIFIER, "face:%(font)s,fore:#%(color)s,face:%(font)s,size:%(size)d" % faces)

        # Default
        self.StyleSetSpec(wx.stc.STC_PL_ARRAY, "face:%(font)s,fore:#000000,face:%(font)s,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_PL_DATASECTION, "face:%(font)s,fore:#000000,face:%(font)s,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_PL_ERROR, "face:%(font)s,fore:#000000,face:%(font)s,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_PL_HASH, "face:%(font)s,fore:#000000,face:%(font)s,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_PL_HERE_DELIM, "face:%(font)s,fore:#000000,face:%(font)s,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_PL_HERE_Q, "face:%(font)s,fore:#000000,face:%(font)s,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_PL_HERE_QQ, "face:%(font)s,fore:#000000,face:%(font)s,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_PL_HERE_QX, "face:%(font)s,fore:#000000,face:%(font)s,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_PL_LONGQUOTE, "face:%(font)s,fore:#000000,face:%(font)s,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_PL_OPERATOR, "face:%(font)s,fore:#000000,face:%(font)s,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_PL_POD, "face:%(font)s,fore:#000000,face:%(font)s,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_PL_PREPROCESSOR, "face:%(font)s,fore:#000000,face:%(font)s,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_PL_PUNCTUATION, "face:%(font)s,fore:#000000,face:%(font)s,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_PL_REGEX, "face:%(font)s,fore:#000000,face:%(font)s,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_PL_REGSUBST, "face:%(font)s,fore:#000000,face:%(font)s,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_PL_SCALAR, "face:%(font)s,fore:#000000,face:%(font)s,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_PL_SYMBOLTABLE, "face:%(font)s,fore:#000000,face:%(font)s,size:%(size)d" % faces)


class PerlOptionsPanel(STCTextEditor.TextOptionsPanel):

    def __init__(self, parent, id):
        STCTextEditor.TextOptionsPanel.__init__(self, parent, id, configPrefix = "Perl", label = "Perl", hasWordWrap = True, hasTabs = True)


PERLKEYWORDS = [
        "abs",
        "accept",
        "alarm",
        "atan2",
        "bind",
        "binmode",
        "bless",
        "caller",
        "chdir",
        "chmod",
        "chomp",
        "chop",
        "chown",
        "chr",
        "chroot",
        "close",
        "closedir",
        "connect",
        "continue",
        "cos",
        "crypt",
        "dbmclose",
        "dbmopen",
        "defined",
        "delete",
        "die",
        "do",
        "dump",
        "each",
        "endgrent",
        "endhostent",
        "endnetent",
        "endprotoent",
        "endpwent",
        "endservent",
        "eof",
        "eval",
        "exec",
        "exists",
        "exit",
        "exp",
        "fcntl",
        "fileno",
        "flock",
        "fork",
        "format",
        "formline",
        "getc",
        "getgrent",
        "getgrgid",
        "getgrnam",
        "gethostbyaddr",
        "gethostbyname",
        "gethostent",
        "getlogin",
        "getnetbyaddr",
        "getnetbyname",
        "getnetent",
        "getpeername",
        "getpgrp",
        "getppid",
        "getpriority",
        "getprotobyname",
        "getprotobynumber",
        "getprotoent",
        "getpwent",
        "getpwnam",
        "getpwuid",
        "getservbyname",
        "getservbyport",
        "getservent",
        "getsockname",
        "getsockopt",
        "glob",
        "gmtime",
        "goto",
        "grep",
        "hex",
        "import",
        "index",
        "int",
        "ioctl",
        "join",
        "keys",
        "kill",
        "last",
        "lc",
        "lcfirst",
        "length",
        "link",
        "listen",
        "local",
        "localtime",
        "log",
        "lstat",
        "m//",
        "map",
        "mkdir",
        "msgctl",
        "msgget",
        "msgrcv",
        "msgsnd",
        "my",
        "next",
        "no",
        "oct",
        "open",
        "opendir",
        "ord",
        "pack",
        "package",
        "pipe",
        "pop",
        "pos",
        "print",
        "printf",
        "prototype",
        "push",
        "q/STRING/",
        "qq/STRING/",
        "quotemeta",
        "qw",
        "qw/STRING/",
        "qx",
        "qx/STRING/",
        "rand",
        "read",
        "readdir",
        "readline",
        "readlink",
        "readpipe",
        "recv",
        "redo",
        "ref",
        "rename",
        "require",
        "reset",
        "return",
        "reverse",
        "rewinddir",
        "rindex",
        "rmdir",
        "s///",
        "scalar",
        "seek",
        "seekdir",
        "select",
        "semctl",
        "semget",
        "semop",
        "send",
        "setgrent",
        "sethostent",
        "setnetent",
        "setpgrp",
        "setpriority",
        "setprotoent",
        "setpwent",
        "setservent",
        "setsockopt",
        "shift",
        "shmctl",
        "shmget",
        "shmread",
        "shmwrite",
        "shutdown",
        "sin",
        "sleep",
        "socket",
        "socketpair",
        "sort",
        "splice",
        "split",
        "sprintf",
        "sqrt",
        "srand",
        "stat",
        "study",
        "sub",
        "substr",
        "symlink",
        "syscall",
        "sysopen",
        "sysread",
        "sysseek",
        "system",
        "syswrite",
        "tell",
        "telldir",
        "tie",
        "tied",
        "times",
        "tr///",
        "truncate",
        "uc",
        "ucfirst",
        "umask",
        "undef",
        "unlink",
        "unpack",
        "unshift",
        "untie",
        "use",
        "utime",
        "values",
        "vec",
        "wait",
        "waitpid",
        "wantarray",
        "warn",
        "write",
        "y///",
        "eq",
        "ne",
        "lt",
        "le",
        "gt",
        "ge",
        "cmp",
        "if",
        "else"
        "not",
        "and",
        "xor",
        "or",
        "if",
        "while",
        "until",
        "for",
        "foreach",
        "last",
        "next",
        "redo",
        "goto",
        "STDIN",
        "STDOUT",
        "STDERR",
        "WHEncE",
        "BEGIN",
        "END",
        "require",
        "integer",
        "less",
        "sigtrap",
        "strict",
        "subs"
    ]


#----------------------------------------------------------------------------
# Icon Bitmaps - generated by encode_bitmaps.py
#----------------------------------------------------------------------------
from wx import ImageFromStream, BitmapFromImage
from wx import EmptyIcon
import cStringIO


def getPerlData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x0e\x00\x00\x00\x10\x08\x06\
\x00\x00\x00&\x94N:\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\x00\
\x01mIDAT(\x91\x9d\x93/\x8e\xf30\x10\xc5\x7f[\xed\x05\xca\x82,Ef\xc6e\x01Vx{\
\x89\x04\x16\x94\x97\x045\xa8\xa0\xa44G\x08\xc9\x01\xa2\x80\x1e $ld\xc9\xa8w\
\x08\xf0\xa2x\x93~\x1f\xda\x91F\xb2\xfd\xe6=\xcd?\x7f\xcd\xf3\x1c\xf8\x83}/\
\x87i\x9a\x000\xc6D\xb0\xeb:\x86a MS\xce\xe7\xf3\x968M\x13}\xdf\xe3\x9cCD\
\xb8\xddn\x18c\xe8\xba\x8e\xb2,\xc9\xb2\x0c\x11\x01\xd8\x90w\xc6\x18\x94R\
\xf1\xa1\xef{\xba\xae\xa3i\x1a\xb4\xd6h\xad)\x8a\x02\xe7\\\xccj\x93\xea\xa2\
\nP\xd75\xd7\xeb\x15\x00\xef\xfdFt)e\xb7\x80\x8b\xbas\x8e$I\xe2}\xc1\x8b\xa2\
\xd8\xf4b\x07\xa0\x94\xe2\xf5za\xad\xc5Z\xcb\xfb\xfdFD\xe8\xfb\x9e\x05\x17\
\x11\x9cs4M\xf3K<\x9dNdY\xc60\x0cx\xef\x11\x11\xea\xbaF)\x85s\x8e\xba\xae)\
\xcb\x12\x11!M\xd3_"\xc0\xfd~\xc7Z\x8bs\x0e\x80$I\xa2:@UU1u\x00\xe6y\x0ek\
\x1f\xc71\x1c\x0e\x87\xd0\xb6m\xd8\xef\xf7\xe1\xf1x\x84\xcb\xe5\x12\xe6y\x0e\
\xc7\xe31\xc6\xed\xf80\x11!\xcb2\xbc\xf7TUE\x9e\xe71=\xadul\xce\xf7\'Qk\x8d\
\xf7\x9e<\xcf\x81\xed&Yk\xb7\xe3\xf84\xa5\x14\xc6\x18D\x84\xe7\xf3\x19\x83\
\xd75\xfe\x97\xb8\x0eXo\xcc2\x9e\x7f\x9a3\x8ech\xdb6|6l\xf15\xf6\xf5\xd7o\
\xf5\x03\xaf\x9f\xfa@\x02\xe4\xdc\xf9\x00\x00\x00\x00IEND\xaeB`\x82' 


def getPerlBitmap():
    return BitmapFromImage(getPerlImage())

def getPerlImage():
    stream = cStringIO.StringIO(getPerlData())
    return ImageFromStream(stream)

def getPerlIcon():
    icon = EmptyIcon()
    icon.CopyFromBitmap(getPerlBitmap())
    return icon
