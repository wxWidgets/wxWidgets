/////////////////////////////////////////////////////////////////////////////
// Name:        _colour.i
// Purpose:     SWIG interface for wxColour
//
// Author:      Robin Dunn
//
// Created:     7-July-1997
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------
%newgroup;


DocStr(wxColour,
"A colour is an object representing a combination of Red, Green, and Blue (RGB)
intensity values, and is used to determine drawing colours, window colours,
etc.  Valid RGB values are in the range 0 to 255.

In wxPython there are typemaps that will automatically convert from a colour
name, or from a '#RRGGBB' colour hex value string to a wx.Colour object when
calling C++ methods that expect a wxColour.  This means that the following are
all equivallent:

    win.SetBackgroundColour(wxColour(0,0,255))
    win.SetBackgroundColour('BLUE')
    win.SetBackgroundColour('#0000FF')

You can retrieve the various current system colour settings with
wx.SystemSettings.GetColour.");


class wxColour : public wxObject {
public:
    
    DocCtorStr(
        wxColour(unsigned char red=0, unsigned char green=0, unsigned char blue=0),
        "Constructs a colour from red, green and blue values.");
    
    DocCtorStrName(
        wxColour( const wxString& colorName),
        "Constructs a colour object using a colour name listed in wx.TheColourDatabase.",
        NamedColour);
    
    DocCtorStrName(
        wxColour( unsigned long colRGB ),
        "Constructs a colour from a packed RGB value.",
        ColourRGB);

    ~wxColour();

    
    DocDeclStr(
        unsigned char , Red(),
        "Returns the red intensity.");
    
    DocDeclStr(
        unsigned char , Green(),
        "Returns the green intensity.");
    
    DocDeclStr(
        unsigned char , Blue(),
        "Returns the blue intensity.");
    
    DocDeclStr(
        bool , Ok(),
        "Returns True if the colour object is valid (the colour has been\n"
        "initialised with RGB values).");
    
    DocDeclStr(
        void , Set(unsigned char red, unsigned char green, unsigned char blue),
        "Sets the RGB intensity values.");

    DocDeclStrName(
        void , Set(unsigned long colRGB),
        "Sets the RGB intensity values from a packed RGB value.",
        SetRGB);

    DocDeclStrName(
        void , InitFromName(const wxString& colourName),
        "Sets the RGB intensity values using a colour name listed in wx.TheColourDatabase.",
        SetFromName);
    
    
    DocDeclStr(
        long , GetPixel() const,
        "Returns a pixel value which is platform-dependent. On Windows, a\n"
        "COLORREF is returned. On X, an allocated pixel value is returned.\n"
        "-1 is returned if the pixel is invalid (on X, unallocated).");
    
    
    DocDeclStr(
        bool , operator==(const wxColour& colour) const,
        "Compare colours for equality");
    
    DocDeclStr(
        bool , operator!=(const wxColour& colour) const,
        "Compare colours for inequality");
    


    %extend {
        DocAStr(Get,
                "Get() -> (r, g, b)",
                "Returns the RGB intensity values as a tuple.");
        PyObject* Get() {
            PyObject* rv = PyTuple_New(3);
            int red = -1;
            int green = -1;
            int blue = -1;
            if (self->Ok()) {
                red =   self->Red();
                green = self->Green();
                blue =  self->Blue();
            }
            PyTuple_SetItem(rv, 0, PyInt_FromLong(red));
            PyTuple_SetItem(rv, 1, PyInt_FromLong(green));
            PyTuple_SetItem(rv, 2, PyInt_FromLong(blue));
            return rv;
        }

        DocStr(GetRGB,
               "Return the colour as a packed RGB value");
        unsigned long GetRGB() {
            return self->Red() | (self->Green() << 8) | (self->Blue() << 16);
        }
    }


    %pythoncode {
        asTuple = Get
        def __str__(self):                  return str(self.asTuple())
        def __repr__(self):                 return 'wx.Colour' + str(self.asTuple())
        def __nonzero__(self):              return self.Ok()
        __safe_for_unpickling__ = True
        def __reduce__(self):               return (Colour, self.Get())
        }
};

%pythoncode {
    Color = Colour
    NamedColor = NamedColour
    ColorRGB = ColourRGB
}

//---------------------------------------------------------------------------

