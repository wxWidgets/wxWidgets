

from wxPython.wx import *

#----------------------------------------------------------------------

# Some unicode strings
chi_uni = (u'Python ?\u8200?\u619f\u8cdc?\u877a\u51fd?\u96a4\ued67?\u5697',
            '(Python is best programming language)')

lt1_uni = (u'Pythonas yra \u017eaviausia \u0161neka',
            '(Python is the best)')
lt2_uni = (u'A\u0161 m\u0117gstu \u0161okolad\u0105',
            '(I like chocolate)')


# Equivalents in UTF-8.  Should I demo these somehow???
chi_utf8 = ('Python ?\xe8\x88\x80?\xe6\x86\x9f\xe8\xb3\x9c?\xe8\x9d\xba\xe5\x87\xbd?\xe9\x9a\xa4\xee\xb5\xa7?\xe5\x9a\x97',
            '(Python is best programming language)')

lt1_utf8 = ('Pythonas yra \xc5\xbeaviausia \xc5\xa1neka',
            '(Python is the best)')
lt2_utf8 = ('A\xc5\xa1 m\xc4\x97gstu \xc5\xa1okolad\xc4\x85',
            '(I like chocolate)')

#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent, log):
        self.log = log
        wxPanel.__init__(self, parent, -1)

        box = wxBoxSizer(wxVERTICAL)

        if not wxUSE_UNICODE:
            self.AddLine(box)
            self.AddText(box, "Sorry, this wxPython was not built with Unicode support.",
                         font = wxFont(12, wxSWISS, wxNORMAL, wxBOLD))
            self.AddLine(box)

        else:
            f = self.GetFont()
            font = wxFont(14, f.GetFamily(), f.GetStyle(), wxBOLD, false,
                          f.GetFaceName(), f.GetEncoding())

            self.AddLine(box)
            self.AddText(box, chi_uni[0], chi_uni[1], 'Chinese:', font)
            self.AddLine(box)

            self.AddText(box, lt1_uni[0], lt1_uni[1], 'Lithuanian:', font)
            self.AddLine(box)
            self.AddText(box, lt2_uni[0], lt2_uni[1], 'Lithuanian:', font)
            self.AddLine(box)


        border = wxBoxSizer(wxVERTICAL)
        border.Add(box, 1, wxEXPAND|wxALL, 10)
        self.SetAutoLayout(true)
        self.SetSizer(border)


    def AddLine(self, sizer):
        sizer.Add(wxStaticLine(self, -1), 0, wxEXPAND)

    def AddText(self, sizer, text1, text2='', lang='', font=None):
        # create some controls
        lang  = wxStaticText(self, -1, lang)
        text1 = wxStaticText(self, -1, text1)#, style=wxALIGN_CENTRE)
        text2 = wxStaticText(self, -1, text2)
        if font is not None:
            text1.SetFont(font)

        # put them in a sizer
        row = wxBoxSizer(wxHORIZONTAL)
        row.Add(lang)
        row.Add(25,10)
        row.Add(text1, 1, wxEXPAND)
        row.Add(text2)

        # put the row in the main sizer
        sizer.Add(row, 0, wxEXPAND|wxALL, 5)


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>wxPython Unicode Support</center></h2>

wxWindows and wxPython can be compiled with unicode support enabled or
disabled.  Previous to wxPython 2.3.3 non-unicode mode was always
used.  Starting with 2.3.3 either mode is supported, but only if it is
also available in wxWindow on the platform.  Currently wxWindows only
supports unicode on MS Windows platforms, but with the recent release
of GTK+ 2.0 it is only a matter of time until it can be done on wxGTK
(Linux and other unixes) as well.
<p>
When unicode is enabled, then all functions and methods in wxPython
that return a wxString from the C++ function will return a Python
unicode object, and parameters to C++ functions/methods that expect a
wxString can accept either a Python string or unicode object.  If a
string object is passed then it will be decoded into unicode using the
converter pointed to by wxConvCurrent, which will use the default
system encoding.  If you need to use string in some other encoding
then you should convert it to unicode using the Python codecs first
and then pass the unicode to the wxPython method.


</body></html>
"""
