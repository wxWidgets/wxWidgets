/////////////////////////////////////////////////////////////////////////////
// Name:        _pen.i
// Purpose:     SWIG interface for wxPen
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

// wxDash is a signed char, byte is unsigned char...
%typemap(in) (int dashes, wxDash* dashes_array ) {
    $1 = PyList_Size($input);
    $2 = (wxDash*)byte_LIST_helper($input);
    if ($2 == NULL) SWIG_fail;
}
%typemap(freearg) (int dashes, wxDash* dashes_array ) {
    if ($2) delete [] $2;
}

//---------------------------------------------------------------------------
%newgroup


class wxPen : public wxGDIObject {
public:
    wxPen(wxColour& colour, int width=1, int style=wxSOLID);
    ~wxPen();

    int GetCap();
    wxColour GetColour();
    int GetJoin();
    int GetStyle();
    int GetWidth();

    bool Ok();

    void SetCap(int cap_style);
    void SetColour(wxColour& colour);
    void SetJoin(int join_style);
    void SetStyle(int style);
    void SetWidth(int width);

    void SetDashes(int dashes, wxDash* dashes_array);
    //int GetDashes(wxDash **dashes);
    %extend {
        PyObject* GetDashes() {
            wxDash* dashes;
            int count = self->GetDashes(&dashes);
            wxPyBeginBlockThreads();
            PyObject* retval = PyList_New(0);
            for (int x=0; x<count; x++)
                PyList_Append(retval, PyInt_FromLong(dashes[x]));
            wxPyEndBlockThreads();
            return retval;
        }
    }

#ifdef __WXMSW__
    wxBitmap* GetStipple();
    void SetStipple(wxBitmap& stipple);
#endif

    %pythoncode { def __nonzero__(self): return self.Ok() }
};


// The list of ints for the dashes needs to exist for the life of the pen
// so we make it part of the class to save it.  See pyclasses.h

%{
wxPyPen::~wxPyPen()
{
    if (m_dash)
        delete [] m_dash;
}

void wxPyPen::SetDashes(int nb_dashes, const wxDash *dash)
{
    if (m_dash)
        delete [] m_dash;
    m_dash = new wxDash[nb_dashes];
    for (int i=0; i<nb_dashes; i++) {
        m_dash[i] = dash[i];
    }
    wxPen::SetDashes(nb_dashes, m_dash);
} 
%}


class wxPyPen : public wxPen {
public:
    wxPyPen(wxColour& colour, int width=1, int style=wxSOLID);
    ~wxPyPen();

    void SetDashes(int dashes, wxDash* dashes_array);
};

// wxPyPen is aliased to wxPen
%pythoncode { Pen = PyPen };

//---------------------------------------------------------------------------
