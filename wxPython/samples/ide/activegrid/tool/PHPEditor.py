#----------------------------------------------------------------------------
# Name:         PHPEditor.py
# Purpose:      PHP Script Editor for pydocview tbat uses the Styled Text Control
#
# Author:       Morgan Hua
#
# Created:      1/4/04
# CVS-ID:       $Id$
# Copyright:    (c) 2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------

import wx
import string
import STCTextEditor
import CodeEditor
import OutlineService
import os
import re


class PHPDocument(CodeEditor.CodeDocument):

    pass


class PHPView(CodeEditor.CodeView):


    def GetCtrlClass(self):
        """ Used in split window to instantiate new instances """
        return PHPCtrl


    def GetAutoCompleteHint(self):
        pos = self.GetCtrl().GetCurrentPos()
        if pos == 0:
            return None, None
            
        validLetters = string.letters + string.digits + '_$'
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
        return PHPKEYWORDS
        
    #----------------------------------------------------------------------------
    # Methods for OutlineService
    #----------------------------------------------------------------------------

    def DoLoadOutlineCallback(self, force=False):
        outlineService = wx.GetApp().GetService(OutlineService.OutlineService)
        if not outlineService:
            return False

        outlineView = outlineService.GetView()
        if not outlineView:
            return False

        treeCtrl = outlineView.GetTreeCtrl()
        if not treeCtrl:
            return False

        view = treeCtrl.GetCallbackView()
        newCheckSum = self.GenCheckSum()
        if not force:
            if view and view is self:
                if self._checkSum == newCheckSum:
                    return False
        self._checkSum = newCheckSum

        treeCtrl.DeleteAllItems()

        document = self.GetDocument()
        if not document:
            return True

        filename = document.GetFilename()
        if filename:
            rootItem = treeCtrl.AddRoot(os.path.basename(filename))
            treeCtrl.SetDoSelectCallback(rootItem, self, None)
        else:
            return True

        text = self.GetValue()
        if not text:
            return True

        INTERFACE_PATTERN = 'interface[ \t]+\w+'
        CLASS_PATTERN = '((final|abstract)[ \t]+)?((public|private|protected)[ \t]+)?(static[ \t]+)?class[ \t]+\w+((implements|extends)\w+)?'
        FUNCTION_PATTERN = '(abstract[ \t]+)?((public|private|protected)[ \t]+)?(static[ \t]+)?function[ \t]+?\w+\(.*?\)'
        interfacePat = re.compile(INTERFACE_PATTERN, re.M|re.S)
        classPat = re.compile(CLASS_PATTERN, re.M|re.S)
        funcPat= re.compile(FUNCTION_PATTERN, re.M|re.S)
        pattern = re.compile('^[ \t]*('+ CLASS_PATTERN + '.*?{|' + FUNCTION_PATTERN + '|' + INTERFACE_PATTERN +'\s*?{).*?$', re.M|re.S)

        iter = pattern.finditer(text)
        indentStack = [(0, rootItem)]
        for pattern in iter:
            line = pattern.string[pattern.start(0):pattern.end(0)]
            foundLine = classPat.search(line)
            if foundLine:
                indent = foundLine.start(0)
                itemStr = foundLine.string[foundLine.start(0):foundLine.end(0)]
            else:
                foundLine = funcPat.search(line)
                if foundLine:
                    indent = foundLine.start(0)
                    itemStr = foundLine.string[foundLine.start(0):foundLine.end(0)]
                else:
                    foundLine = interfacePat.search(line)
                    if foundLine:
                        indent = foundLine.start(0)
                        itemStr = foundLine.string[foundLine.start(0):foundLine.end(0)]

            if indent == 0:
                parentItem = rootItem
            else:
                lastItem = indentStack.pop()
                while lastItem[0] >= indent:
                    lastItem = indentStack.pop()
                indentStack.append(lastItem)
                parentItem = lastItem[1]

            item = treeCtrl.AppendItem(parentItem, itemStr)
            treeCtrl.SetDoSelectCallback(item, self, (pattern.end(0), pattern.start(0) + indent))  # select in reverse order because we want the cursor to be at the start of the line so it wouldn't scroll to the right
            indentStack.append((indent, item))

        treeCtrl.Expand(rootItem)

        return True


class PHPService(CodeEditor.CodeService):


    def __init__(self):
        CodeEditor.CodeService.__init__(self)


class PHPCtrl(CodeEditor.CodeCtrl):


    def __init__(self, parent, ID = -1, style = wx.NO_FULL_REPAINT_ON_RESIZE):
        CodeEditor.CodeCtrl.__init__(self, parent, ID, style)
        self.SetLexer(wx.stc.STC_LEX_HTML)
        self.SetStyleBits(7)
        self.SetKeyWords(4, string.join(PHPKEYWORDS))
        self.SetProperty("fold.html", "1")


    def CanWordWrap(self):
        return True


    def SetViewDefaults(self):
        CodeEditor.CodeCtrl.SetViewDefaults(self, configPrefix = "PHP", hasWordWrap = True, hasTabs = True)


    def GetFontAndColorFromConfig(self):
        return CodeEditor.CodeCtrl.GetFontAndColorFromConfig(self, configPrefix = "PHP")


    def UpdateStyles(self):
        CodeEditor.CodeCtrl.UpdateStyles(self)
        
        if not self.GetFont():
            return

        faces = { 'font' : self.GetFont().GetFaceName(),
                  'size' : self.GetFont().GetPointSize(),
                  'size2': self.GetFont().GetPointSize() - 2,
                  'color' : "%02x%02x%02x" % (self.GetFontColor().Red(), self.GetFontColor().Green(), self.GetFontColor().Blue())
                  }


        # HTML Styles
        # White space
        self.StyleSetSpec(wx.stc.STC_H_DEFAULT, "face:%(font)s,fore:#000000,face:%(font)s,size:%(size)d" % faces)
        # Comment
        self.StyleSetSpec(wx.stc.STC_H_COMMENT, "face:%(font)s,fore:#007F00,italic,face:%(font)s,size:%(size)d" % faces)
        # Number
        self.StyleSetSpec(wx.stc.STC_H_NUMBER, "face:%(font)s,fore:#007F7F,size:%(size)d" % faces)
        # String
        self.StyleSetSpec(wx.stc.STC_H_SINGLESTRING, "face:%(font)s,fore:#7F007F,face:%(font)s,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_H_DOUBLESTRING, "face:%(font)s,fore:#7F007F,face:%(font)s,size:%(size)d" % faces)
        # Tag
        self.StyleSetSpec(wx.stc.STC_H_TAG, "face:%(font)s,fore:#00007F,bold,size:%(size)d" % faces)
        # Attributes
        self.StyleSetSpec(wx.stc.STC_H_ATTRIBUTE, "face:%(font)s,fore:#00007F,bold,size:%(size)d" % faces)


        # PHP Styles
        self.StyleSetSpec(wx.stc.STC_HPHP_DEFAULT, "face:%(font)s,fore:#000000,face:%(font)s,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_HPHP_COMMENT, "face:%(font)s,fore:#007F00,italic,face:%(font)s,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_HPHP_COMMENTLINE, "face:%(font)s,fore:#007F00,italic,face:%(font)s,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_HPHP_NUMBER, "face:%(font)s,fore:#007F7F,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_HPHP_SIMPLESTRING, "face:%(font)s,fore:#7F007F,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_HPHP_HSTRING, "face:%(font)s,fore7F007F,face:%(font)s,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_HPHP_HSTRING_VARIABLE, "face:%(font)s,fore:#007F7F,italic,bold,face:%(font)s,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_HPHP_VARIABLE, "face:%(font)s,fore:#000000,face:%(font)s,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_HPHP_OPERATOR, "face:%(font)s,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_HPHP_WORD, "face:%(font)s,fore:#00007F,bold,size:%(size)d" % faces) # keyword
        

class PHPOptionsPanel(STCTextEditor.TextOptionsPanel):

    def __init__(self, parent, id):
        STCTextEditor.TextOptionsPanel.__init__(self, parent, id, configPrefix = "PHP", label = "PHP", hasWordWrap = True, hasTabs = True)


PHPKEYWORDS = [
        "and", "or", "xor", "__FILE__", "exception", "__LINE__", "array", "as", "break", "case",
        "class", "const", "continue", "declare", "default", "die", "do", "echo", "else", "elseif",
        "empty", "enddeclare", "endfor", "endforeach", "endif", "endswith", "endwhile", "eval",
        "exit", "extends", "for", "foreach", "function", "global", "if", "include", "include_once",
        "isset", "list", "new", "print", "require", "require_once", "return", "static", "switch",
        "unset", "use", "var", "while", "__FUNCTION__", "__CLASS__", "__METHOD__", "final", "php_user_filter",
        "interface", "implements", "extends", "public", "private", "protected", "abstract", "clone", "try", "catch",
        "throw", "cfunction", "old_function",
        "$_SERVER", "$_ENV", "$_COOKIE", "$_GET", "$_POST", "$_FILES", "$_REQUEST", "$_SESSION", "$GLOBALS", "$php_errormsg",
        "PHP_VERSION", "PHP_OS", "PHP_EOL", "DEFAULT_INCLUDE_PATH", "PEAR_INSTALL_DIR", "PEAR_EXTENSION_DIR",
        "PHP_EXTENSION_DIR", "PHP_BINDIR", "PHP_LIBDIR", "PHP_DATADIR", "PHP_SYSCONFDIR", "PHP_LOCALSTATEDIR",
        "PHP_CONFIG_FILE_PATH", "PHP_OUTPUT_HANDLER_START", "PHP_OUTPUT_HANDLER_CONT", "PHP_OUTPUT_HANDLER_END",
        "E_ERROR", "E_WARNING", "E_PARSE", "E_NOTICE", "E_CORE_ERROR", "E_CORE_WARNING", "E_COMPILE_ERROR",
        "E_COMPILE_WARNING", "E_USER_ERROR", "E_USER_WARNING", "E_USER_NOTICE", "E_ALL", "E_STRICT",
        "TRUE", "FALSE", "NULL", "ZEND_THREAD_SAFE",
        "EXTR_OVERWRITE", "EXTR_SKIP", "EXTR_PREFIX_SAME", "EXTR_PREFIX_ALL", "EXTR_PREFIX_INVALID",
        "EXTR_PREFIX_IF_EXISTS", "EXTR_IF_EXISTS", "SORT_ASC", "SORT_DESC", "SORT_REGULAR", "SORT_NUMERIC",
        "SORT_STRING", "CASE_LOWER", "CASE_UPPER", "COUNT_NORMAL", "COUNT_RECURSIVE", "ASSERT_ACTIVE",
        "ASSERT_CALLBACK", "ASSERT_BAIL", "ASSERT_WARNING", "ASSERT_QUIET_EVAL", "CONNECTION_ABORTED",
        "CONNECTION_NORMAL", "CONNECTION_TIMEOUT", "INI_USER", "INI_PERDIR", "INI_SYSTEM", "INI_ALL",
        "M_E", "M_LOG2E", "M_LOG10E", "M_LN2", "M_LN10", "M_PI", "M_PI_2", "M_PI_4", "M_1_PI", "M_2_PI",
        "M_2_SQRTPI", "M_SQRT2", "M_SQRT1_2", "CRYPT_SALT_LENGTH", "CRYPT_STD_DES", "CRYPT_EXT_DES", "CRYPT_MD5",
        "CRYPT_BLOWFISH", "DIRECTORY_SEPARATOR", "SEEK_SET", "SEEK_CUR", "SEEK_END", "LOCK_SH", "LOCK_EX", "LOCK_UN",
        "LOCK_NB", "HTML_SPECIALCHARS", "HTML_ENTITIES", "ENT_COMPAT", "ENT_QUOTES", "ENT_NOQUOTES", "INFO_GENERAL",
        "INFO_CREDITS", "INFO_CONFIGURATION", "INFO_MODULES", "INFO_ENVIRONMENT", "INFO_VARIABLES", "INFO_LICENSE",
        "INFO_ALL", "CREDITS_GROUP", "CREDITS_GENERAL", "CREDITS_SAPI", "CREDITS_MODULES", "CREDITS_DOCS",
        "CREDITS_FULLPAGE", "CREDITS_QA", "CREDITS_ALL", "STR_PAD_LEFT", "STR_PAD_RIGHT", "STR_PAD_BOTH",
        "PATHINFO_DIRNAME", "PATHINFO_BASENAME", "PATHINFO_EXTENSION", "PATH_SEPARATOR", "CHAR_MAX", "LC_CTYPE",
        "LC_NUMERIC", "LC_TIME", "LC_COLLATE", "LC_MONETARY", "LC_ALL", "LC_MESSAGES", "ABDAY_1", "ABDAY_2",
        "ABDAY_3", "ABDAY_4", "ABDAY_5", "ABDAY_6", "ABDAY_7", "DAY_1", "DAY_2", "DAY_3", "DAY_4", "DAY_5",
        "DAY_6", "DAY_7", "ABMON_1", "ABMON_2", "ABMON_3", "ABMON_4", "ABMON_5", "ABMON_6", "ABMON_7", "ABMON_8",
        "ABMON_9", "ABMON_10", "ABMON_11", "ABMON_12", "MON_1", "MON_2", "MON_3", "MON_4", "MON_5", "MON_6", "MON_7",
        "MON_8", "MON_9", "MON_10", "MON_11", "MON_12", "AM_STR", "PM_STR", "D_T_FMT", "D_FMT", "T_FMT", "T_FMT_AMPM",
        "ERA", "ERA_YEAR", "ERA_D_T_FMT", "ERA_D_FMT", "ERA_T_FMT", "ALT_DIGITS", "INT_CURR_SYMBOL", "CURRENCY_SYMBOL",
        "CRNCYSTR", "MON_DECIMAL_POINT", "MON_THOUSANDS_SEP", "MON_GROUPING", "POSITIVE_SIGN", "NEGATIVE_SIGN",
        "INT_FRAC_DIGITS", "FRAC_DIGITS", "P_CS_PRECEDES", "P_SEP_BY_SPACE", "N_CS_PRECEDES", "N_SEP_BY_SPACE",
        "P_SIGN_POSN", "N_SIGN_POSN", "DECIMAL_POINT", "RADIXCHAR", "THOUSANDS_SEP", "THOUSEP", "GROUPING",
        "YESEXPR", "NOEXPR", "YESSTR", "NOSTR", "CODESET", "LOG_EMERG", "LOG_ALERT", "LOG_CRIT", "LOG_ERR",
        "LOG_WARNING", "LOG_NOTICE", "LOG_INFO", "LOG_DEBUG", "LOG_KERN", "LOG_USER", "LOG_MAIL", "LOG_DAEMON",
        "LOG_AUTH", "LOG_SYSLOG", "LOG_LPR", "LOG_NEWS", "LOG_UUCP", "LOG_CRON", "LOG_AUTHPRIV", "LOG_LOCAL0",
        "LOG_LOCAL1", "LOG_LOCAL2", "LOG_LOCAL3", "LOG_LOCAL4", "LOG_LOCAL5", "LOG_LOCAL6", "LOG_LOCAL7",
        "LOG_PID", "LOG_CONS", "LOG_ODELAY", "LOG_NDELAY", "LOG_NOWAIT", "LOG_PERROR"
        ]


#----------------------------------------------------------------------------
# Icon Bitmaps - generated by encode_bitmaps.py
#----------------------------------------------------------------------------
from wx import ImageFromStream, BitmapFromImage
from wx import EmptyIcon
import cStringIO


def getPHPData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x00{IDAT8\x8dclh8\xf0\x9f\x81\x02\xc0D\x89f\xaa\x18\xc0\x82M0<\\\x1c\
\xce^\xb9\xf2%y.\xd0\xd4\xd4$\xde\x05\xf8l\x0c\x0f\x17\x87\x8baS\xc7x\xfd\
\xfa\xf5\xff\xc8\xb6]\xbf~\x1d\xc3\x05\xf8\xc4\x98\x90\x05\xae_\xbf\x8e\xa1\
\x88\x90\xd8 \x8aF\x98\x93`~\xc3\x05\xd0\xd5\xc1\r\x80\t\xc0B\xf7\xfa\xf5\
\xeb(l\\\xeaP\xbc\x80\x1c\x85\xb8\xd8\xe8|&b\x9c\x8dn;2`\x1c\xf0\xdc\x08\x00\
\x8e\xf2S\xed\xb0\xbe\xaa\xbc\x00\x00\x00\x00IEND\xaeB`\x82' 

def getPHPBitmap():
    return BitmapFromImage(getPHPImage())

def getPHPImage():
    stream = cStringIO.StringIO(getPHPData())
    return ImageFromStream(stream)

def getPHPIcon():
    icon = EmptyIcon()
    icon.CopyFromBitmap(getPHPBitmap())
    return icon
