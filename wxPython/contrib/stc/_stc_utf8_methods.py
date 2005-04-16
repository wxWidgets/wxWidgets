 
def AddTextUTF8(self, text):
    """
    Add UTF8 encoded text to the document at the current position.
    Works 'natively' in a unicode build of wxPython, and will also work
    in an ansi build if the UTF8 text is compatible with the current
    encoding.
    """
    if not wx.USE_UNICODE:
        u = text.decode('utf-8')
        text = u.encode(wx.GetDefaultPyEncoding())
    self.AddTextRaw(text)

    
def InsertTextUTF8(self, pos, text):
    """
    Insert UTF8 encoded text at a position.  Works 'natively' in a
    unicode build of wxPython, and will also work in an ansi build if
    the UTF8 text is compatible with the current encoding.
    """
    if not wx.USE_UNICODE:
        u = text.decode('utf-8')
        text = u.encode(wx.GetDefaultPyEncoding())
    self.InsertTextRaw(pos, text)

    
def GetCurLineUTF8(self):
    """
    Retrieve the UTF8 text of the line containing the caret, and also
    the index of the caret on the line.  In an ansi build of wxPython
    the text retrieved from the document is assumed to be in the
    current default encoding.
    """
    text, pos = self.GetCurLineRaw()
    if not wx.USE_UNICODE:
        u = text.decode(wx.GetDefaultPyEncoding())
        text = u.encode('utf-8')
    return text, pos

    
def GetLineUTF8(self, line):
    """
    Retrieve the contents of a line as UTF8.  In an ansi build of wxPython
    the text retrieved from the document is assumed to be in the
    current default encoding.
    """
    text = self.GetLineRaw(line)
    if not wx.USE_UNICODE:
        u = text.decode(wx.GetDefaultPyEncoding())
        text = u.encode('utf-8')
    return text


def GetSelectedTextUTF8(self):
    """
    Retrieve the selected text as UTF8.  In an ansi build of wxPython
    the text retrieved from the document is assumed to be in the
    current default encoding.
    """
    text = self.GetSelectedTextRaw()
    if not wx.USE_UNICODE:
        u = text.decode(wx.GetDefaultPyEncoding())
        text = u.encode('utf-8')
    return text


def GetTextRangeUTF8(self, startPos, endPos):
    """
    Retrieve a range of text as UTF8.  In an ansi build of wxPython
    the text retrieved from the document is assumed to be in the
    current default encoding.
    """
    text = self.GetTextRangeRaw(startPos, endPos)
    if not wx.USE_UNICODE:
        u = text.decode(wx.GetDefaultPyEncoding())
        text = u.encode('utf-8')
    return text


def SetTextUTF8(self, text):
    """
    Replace the contents of the document with the UTF8 text given.
    Works 'natively' in a unicode build of wxPython, and will also
    work in an ansi build if the UTF8 text is compatible with the
    current encoding.
    """
    if not wx.USE_UNICODE:
        u = text.decode('utf-8')
        text = u.encode(wx.GetDefaultPyEncoding())
    self.SetTextRaw(text)


def GetTextUTF8(self):
    """
    Retrieve all the text in the document as UTF8.  In an ansi build
    of wxPython the text retrieved from the document is assumed to be
    in the current default encoding.
    """
    text = self.GetTextRaw()
    if not wx.USE_UNICODE:
        u = text.decode(wx.GetDefaultPyEncoding())
        text = u.encode('utf-8')
    return text


def AppendTextUTF8(self, text):
    """
    Append a UTF8 string to the end of the document without changing
    the selection.  Works 'natively' in a unicode build of wxPython,
    and will also work in an ansi build if the UTF8 text is compatible
    with the current encoding.
    """
    if not wx.USE_UNICODE:
        u = text.decode('utf-8')
        text = u.encode(wx.GetDefaultPyEncoding())
    self.AppendTextRaw(text)

