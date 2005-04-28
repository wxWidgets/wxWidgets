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
import cStringIO


def getPerlData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x01\x81IDAT8\x8d\xa5S1n\xe30\x10\x1cJ\xf9\x80;U\x04\x04~\xc06\xdc\xa9\
\x10\xd4\xb3K\xf2\x01\xf9\x01F\x92\xce\x8d:vI\x84\xdc\xd5\xaa\xdc%\xd1\x17\
\x0c\x17ns\xa7\x07,\x16\xe0G6E@B\x91\x9c*\x0b\x10 \xb9\xe4\xecpg\xa8T\x92\
\xe27q5^\xbc\xbf\xbd\n\x00\\\xdf\xdc\xaa\xb0\xf7\xfc\xf4(\xcc\x8c<\xcfqw\xff\
\xa0\xa6\x00PI\x8aa\x18\xa4m[\xd9\xedvb\xad\x95a\x18D%)\xfa\xbe\x97\xc5b!\
\xd6Z\xb1\xd6J\xdb\xb6\xa2\x92\x14\xe3\x91\x00\xc0r\xb5VZ\xeb\x08z<\x1e\xf1\
\xfe\xf6*]\xd7\xc1\x18\x03c\x0c\xea\xba\x063\xe3\xff\xbf\x0f\xf9\xf1\tD\x14\
\xe7\xce9\xec\xf7{\x00\x80\xf7\xfe\x1b\xf88\x920\xf1\xde\xc7j\xcc\x8c,\xcb\
\xe2:\xe4\xeb\xba\x06\x80o,"\x03\xad5\x0e\x87C\xacz>\x9fAD\xb1\xba\xd6\x1aD\
\x04f\x063\xcf\x19\\\xdf\xdc\xaa\xa2(p:\x9d\xe0\xbd\x07\x11\xc19\x07\xad5\
\x98\x19\xce9l\xb7[\x10\x11\xf2<\x9f\x03\x00\xc0\xcb\x9f\xbf\xaa,\xcbX!\xcb2\
t]\x17\xf3M\xd3\xc4\'\xc5\x98\xca\x12d\xddl6\x12d\x0c\x12\xab$\x85\xb5Vf2N\
\x83\x88P\x14\x05\xbc\xf7h\x9a\x06UUE\xda\xc6\x98\xcbM\x1c\x871\x06\xde{TU\
\x05\xe0\xcb\'\xe1RY\x96X\xae\xd6\xd1\x91\x17\x19\x00_]_\xae\xd6\x8a\x88\xf0\
\xfc\xf4(\xe1\xd2\xb4\x07?\x02\x8c\x0f\x8e\x1d85\xd2\xc5\x06\xf6}?\xf3|\x18\
\xb3\xdco\xbf\xf3\'`\xa6\xbc1\xa7\xd6\xcb\xbf\x00\x00\x00\x00IEND\xaeB`\x82'\


def getPerlBitmap():
    return BitmapFromImage(getPerlImage())

def getPerlImage():
    stream = cStringIO.StringIO(getPerlData())
    return ImageFromStream(stream)

def getPerlIcon():
    return wx.IconFromBitmap(getPerlBitmap())
