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


    def __init__(self, parent, id=-1, style=wx.NO_FULL_REPAINT_ON_RESIZE):
        CodeEditor.CodeCtrl.__init__(self, parent, id, style)
        self.SetLexer(wx.stc.STC_LEX_PERL)
        self.SetKeyWords(0, string.join(PERLKEYWORDS))


    def CanWordWrap(self):
        return True


    def SetViewDefaults(self):
        CodeEditor.CodeCtrl.SetViewDefaults(self, configPrefix = "Perl", hasWordWrap = True, hasTabs = True, hasFolding=True)


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
        STCTextEditor.TextOptionsPanel.__init__(self, parent, id, configPrefix = "Perl", label = "Perl", hasWordWrap = True, hasTabs = True, hasFolding=True)


    def GetIcon(self):
        return getPerlIcon()


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
import cStringIO


def getPerlData():
    return \
"\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x01CIDAT8\x8d\x95\x93\xbfN\x02A\x10\x87\xbf[\xef\x05\xach|\x02m}\x008\
\x881Z\xd2\x1a\x13\x0bKx\x02{\xa3\xa13\xc6\xc6\xc6\xd2DZ+s\\\x83\x95\x1d\xa2\
&V\x104\x01/p\x01\t\xa0\xe8X\xc0\x9d\x87\xdc^\xf0\x97Lfw3\xdf\xec\xcc\xfe1\
\xca\x95\xba0\x95\xeb\xf5\x01h\xb4\xda\x00x\xde\x90\x83\xfd\r\x838\x95+u\xa9\
\rDj\x03\x91\xe7\xbe\xc8}wb\xa7\xc5\xb2\xdc\xbe\x89\xe4\x8f\xae\xc4PK\xe8L\
\xc5%\xef\x8da{7K\xee\xf0Rt1\xa6\xeb\xf5Y\x01>\xbea\xf45Y\xec\x8e&\xe5\xdf]\
\xdb4\xdd\x0e\xaf/-\xed&&\x110\xc0\xfa\x96\xc5\xf1N\x06\x80\xe5\xb5\xac6\x81\
\x82y\xb87\x9e\r\xeaT\x8b3s+\x95\x14+\x95\x14\x00\xd5h\xb5\xe9\x8e\xe6a\x7f\
\xf70\x14\xf6\xfeXy\xde0\x08\xf2\xe1\xf6gt\xb9V*)v\xc9\t\xae\xd5.9\x86\x19\
\x05\x9e\xefe\xe6i\x8dT\xd3\xed\xfc\x0b\x0e\xb7\x00\xd3C\xd4\x95\x1c'\xbf\
\x15\x15\xbe\xe3\xc6;l\x9e\xdc\x00\xbf\xfe/`\x97\x1c#|\x0e&@\xb1p\x16\x04>U\
\x1fI\x00\x17\xb9<\t\xa0\xc9*\t\x1e\xf4\xa5D\xbd\xeft:-q~\xe1\xbf\xb0\x88b\
\x13\x84{\x8d\x9a\x03\xfc\x00\xea\x7f\xa9A\xa7\xc3Vo\x00\x00\x00\x00IEND\xae\
B`\x82" 


def getPerlBitmap():
    return BitmapFromImage(getPerlImage())

def getPerlImage():
    stream = cStringIO.StringIO(getPerlData())
    return ImageFromStream(stream)

def getPerlIcon():
    return wx.IconFromBitmap(getPerlBitmap())
