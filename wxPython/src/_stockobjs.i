/////////////////////////////////////////////////////////////////////////////
// Name:        _stockobjs.i
// Purpose:     SWIG interface defining "stock" GDI objects
//
// Author:      Robin Dunn
//
// Created:     13-Sept-2003
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------
%newgroup


// See also wxPy_ReinitStockObjects in helpers.cpp
%immutable;

wxFont* const wxNORMAL_FONT;
wxFont* const wxSMALL_FONT;
wxFont* const wxITALIC_FONT;
wxFont* const wxSWISS_FONT;

wxPen* const wxRED_PEN;
wxPen* const wxCYAN_PEN;
wxPen* const wxGREEN_PEN;
wxPen* const wxBLACK_PEN;
wxPen* const wxWHITE_PEN;
wxPen* const wxTRANSPARENT_PEN;
wxPen* const wxBLACK_DASHED_PEN;
wxPen* const wxGREY_PEN;
wxPen* const wxMEDIUM_GREY_PEN;
wxPen* const wxLIGHT_GREY_PEN;

wxBrush* const wxBLUE_BRUSH;
wxBrush* const wxGREEN_BRUSH;
wxBrush* const wxWHITE_BRUSH;
wxBrush* const wxBLACK_BRUSH;
wxBrush* const wxTRANSPARENT_BRUSH;
wxBrush* const wxCYAN_BRUSH;
wxBrush* const wxRED_BRUSH;
wxBrush* const wxGREY_BRUSH;
wxBrush* const wxMEDIUM_GREY_BRUSH;
wxBrush* const wxLIGHT_GREY_BRUSH;

wxColour* const wxBLACK;
wxColour* const wxWHITE;
wxColour* const wxRED;
wxColour* const wxBLUE;
wxColour* const wxGREEN;
wxColour* const wxCYAN;
wxColour* const wxLIGHT_GREY;

wxCursor* const wxSTANDARD_CURSOR;
wxCursor* const wxHOURGLASS_CURSOR;
wxCursor* const wxCROSS_CURSOR;


const wxBitmap wxNullBitmap;
const wxIcon   wxNullIcon;
const wxCursor wxNullCursor;
const wxPen    wxNullPen;
const wxBrush  wxNullBrush;
const wxPalette wxNullPalette;
const wxFont   wxNullFont;
const wxColour wxNullColour;

%mutable;


//---------------------------------------------------------------------------

class wxPenList : public wxObject {
public:

    void AddPen(wxPen* pen);
    wxPen* FindOrCreatePen(const wxColour& colour, int width, int style);
    void RemovePen(wxPen* pen);

    int GetCount();
};


//---------------------------------------------------------------------------

class wxBrushList : public wxObject {
public:

    void AddBrush(wxBrush *brush);
    wxBrush * FindOrCreateBrush(const wxColour& colour, int style=wxSOLID);
    void RemoveBrush(wxBrush *brush);

    int GetCount();
};

//---------------------------------------------------------------------------

MustHaveApp(wxColourDatabase);

class wxColourDatabase : public wxObject {
public:
    wxColourDatabase();
    ~wxColourDatabase();

    // find colour by name or name for the given colour
    wxColour Find(const wxString& name) const;
    wxString FindName(const wxColour& colour) const;
    %pythoncode { FindColour = Find }
    
    // add a new colour to the database
    void AddColour(const wxString& name, const wxColour& colour);

    %extend {
        void Append(const wxString& name, int red, int green, int blue) {
            self->AddColour(name, wxColour(red, green, blue));
        }
    }
};

//---------------------------------------------------------------------------

class wxFontList : public wxObject {
public:

    void AddFont(wxFont* font);
    wxFont * FindOrCreateFont(int point_size, int family, int style, int weight,
                              bool underline = false, const wxString& facename = wxPyEmptyString,
                              wxFontEncoding encoding = wxFONTENCODING_DEFAULT);
    void RemoveFont(wxFont *font);

    int GetCount();
};

//---------------------------------------------------------------------------
%newgroup



// See also wxPy_ReinitStockObjects in helpers.cpp
%immutable;

wxFontList*       const wxTheFontList;
wxPenList*        const wxThePenList;
wxBrushList*      const wxTheBrushList;
wxColourDatabase* const wxTheColourDatabase;

%mutable;


//---------------------------------------------------------------------------
 
%pythoncode { NullColor = NullColour }




