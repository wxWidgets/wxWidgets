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


enum {
    wxC2S_NAME,             // return colour name, when possible
    wxC2S_CSS_SYNTAX,       // return colour in rgb(r,g,b) syntax
    wxC2S_HTML_SYNTAX,      // return colour in #rrggbb syntax     
};

enum {
    wxALPHA_TRANSPARENT,
    wxALPHA_OPAQUE
};


DocStr(wxColour,
"A colour is an object representing a combination of Red, Green, and
Blue (RGB) intensity values, and is used to determine drawing colours,
window colours, etc.  Valid RGB values are in the range 0 to 255.

In wxPython there are typemaps that will automatically convert from a
colour name, from a '#RRGGBB' colour hex value string, or from a 3
integer tuple to a wx.Colour object when calling C++ methods that
expect a wxColour.  This means that the following are all
equivallent::

    win.SetBackgroundColour(wxColour(0,0,255))
    win.SetBackgroundColour('BLUE')
    win.SetBackgroundColour('#0000FF')
    win.SetBackgroundColour((0,0,255))

Additional colour names and their coresponding values can be added
using `wx.ColourDatabase`.  Various system colours (as set in the
user's system preferences) can be retrieved with
`wx.SystemSettings.GetColour`.
", "");


MustHaveApp( wxColour(const wxString& colorName) );

class wxColour : public wxObject {
public:
    
    DocCtorStr(
        wxColour(byte red=0, byte green=0, byte blue=0, byte alpha=wxALPHA_OPAQUE),
        "Constructs a colour from red, green and blue values.

:see: Alternate constructors `wx.NamedColour` and `wx.ColourRGB`.
", "");
    
    DocCtorStrName(
        wxColour( const wxString& colorName),
        "Constructs a colour object using a colour name listed in
``wx.TheColourDatabase``.", "",
        NamedColour);
    
    DocCtorStrName(
        wxColour( unsigned long colRGB ),
        "Constructs a colour from a packed RGB value.", "",
        ColourRGB);

    ~wxColour();

    
    DocDeclStr(
        byte , Red(),
        "Returns the red intensity.", "");
    
    DocDeclStr(
        byte , Green(),
        "Returns the green intensity.", "");
    
    DocDeclStr(
        byte , Blue(),
        "Returns the blue intensity.", "");
    
    DocDeclStr(
        byte , Alpha(),
        "Returns the Alpha value.", "");
    
    DocDeclStr(
        bool , Ok(),
        "Returns True if the colour object is valid (the colour has been
initialised with RGB values).", "");
    
    DocDeclStr(
        void , Set(byte red, byte green, byte blue, byte alpha=wxALPHA_OPAQUE),
        "Sets the RGB intensity values.", "");

    DocDeclStrName(
        void , Set(unsigned long colRGB),
        "Sets the RGB intensity values from a packed RGB value.", "",
        SetRGB);

    DocDeclStrName(
        void , InitFromName(const wxString& colourName),
        "Sets the RGB intensity values using a colour name listed in
``wx.TheColourDatabase``.", "",
        SetFromName);
    
    DocDeclStr(
        wxString , GetAsString(long flags = wxC2S_NAME | wxC2S_CSS_SYNTAX) const,
        "Return the colour as a string.  Acceptable flags are:

            =================== ==================================
            wx.C2S_NAME          return colour name, when possible
            wx.C2S_CSS_SYNTAX    return colour in rgb(r,g,b) syntax
            wx.C2S_HTML_SYNTAX   return colour in #rrggbb syntax     
            =================== ==================================", "");
    
    
    DocDeclStr(
        long , GetPixel() const,
        "Returns a pixel value which is platform-dependent. On Windows, a
COLORREF is returned. On X, an allocated pixel value is returned.  -1
is returned if the pixel is invalid (on X, unallocated).", "");
    
    
    %extend {
        KeepGIL(__eq__);
        DocStr(__eq__, "Compare colours for equality.", "");
        bool __eq__(PyObject* other) {
            wxColour  temp, *obj = &temp;
            if ( other == Py_None ) return false;
            if ( ! wxColour_helper(other, &obj) ) {
                PyErr_Clear();
                return false;
            }
            return self->operator==(*obj);
        }

        
        KeepGIL(__ne__);
        DocStr(__ne__, "Compare colours for inequality.", "");
        bool __ne__(PyObject* other) {
            wxColour  temp, *obj = &temp;
            if ( other == Py_None ) return true;
            if ( ! wxColour_helper(other, &obj)) {
                PyErr_Clear();
                return true;
            }
            return self->operator!=(*obj);
        }
    }


    %extend {
        DocAStr(Get,
                "Get() -> (r, g, b)",
                "Returns the RGB intensity values as a tuple.", "");
        PyObject* Get(bool includeAlpha=false) {
            PyObject* rv = PyTuple_New(includeAlpha ? 4 : 3);
            int red = -1;
            int green = -1;
            int blue = -1;
            int alpha = wxALPHA_OPAQUE;
            if (self->Ok()) {
                red =   self->Red();
                green = self->Green();
                blue =  self->Blue();
                alpha = self->Alpha();
            }
            PyTuple_SetItem(rv, 0, PyInt_FromLong(red));
            PyTuple_SetItem(rv, 1, PyInt_FromLong(green));
            PyTuple_SetItem(rv, 2, PyInt_FromLong(blue));
            if (includeAlpha)
                PyTuple_SetItem(rv, 3, PyInt_FromLong(alpha));                
            return rv;
        }

        DocStr(GetRGB,
               "Return the colour as a packed RGB value", "");
        unsigned long GetRGB() {
            return self->Red() | (self->Green() << 8) | (self->Blue() << 16);
        }
    }


    %pythoncode {
        asTuple = wx._deprecated(Get, "asTuple is deprecated, use `Get` instead")
        def __str__(self):                  return str(self.Get(True))
        def __repr__(self):                 return 'wx.Colour' + str(self.Get(True))
        def __nonzero__(self):              return self.Ok()
        __safe_for_unpickling__ = True
        def __reduce__(self):               return (Colour, self.Get(True))
        }
};

%pythoncode {
    Color = Colour
    NamedColor = NamedColour
    ColorRGB = ColourRGB
}

//---------------------------------------------------------------------------

