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

%noautorepr wxColour;

class wxColour : public wxObject {
public:
    wxColour(unsigned char red=0, unsigned char green=0, unsigned char blue=0);
    ~wxColour();

    %name(NamedColour) wxColour( const wxString& colorName);
    %name(ColourRGB) wxColour( unsigned long colRGB );

    unsigned char Red();
    unsigned char Green();
    unsigned char Blue();
    bool Ok();
    void Set(unsigned char red, unsigned char green, unsigned char blue);
    %name(SetRBG) void Set(unsigned long colRGB);

    bool operator==(const wxColour& colour) const;
    bool operator != (const wxColour& colour) const;

    %extend {
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
//         bool __eq__(PyObject* obj) {
//             wxColour  tmp;
//             wxColour* ptr = &tmp;
//             if (obj == Py_None)    return FALSE;
//             wxPyBLOCK_THREADS(bool success = wxColour_helper(obj, &ptr); PyErr_Clear());
//             if (! success)         return FALSE;
//             return *self == *ptr;
//         }
//         bool __ne__(PyObject* obj) {
//             wxColour  tmp;
//             wxColour* ptr = &tmp;
//             if (obj == Py_None)    return TRUE;
//             wxPyBLOCK_THREADS(bool success = wxColour_helper(obj, &ptr); PyErr_Clear());
//             if (! success)         return TRUE;
//             return *self != *ptr;
//         }
    }


    %pythoncode {
        asTuple = Get
        def __str__(self):                  return str(self.asTuple())
        def __repr__(self):                 return 'wxColour' + str(self.asTuple())
        def __nonzero__(self):              return self.Ok()
        def __getinitargs__(self):          return ()
        def __getstate__(self):             return self.asTuple()
        def __setstate__(self, state):      self.Set(*state)
        }
};

//---------------------------------------------------------------------------

