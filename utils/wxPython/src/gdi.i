/////////////////////////////////////////////////////////////////////////////
// Name:        gdi.i
// Purpose:     SWIG interface file for wxDC, wxBrush, wxPen, wxFont, etc.
//
// Author:      Robin Dunn
//
// Created:     7/7/97
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


%module gdi

%{
#include "helpers.h"

#include <wx/metafile.h>
%}

//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

// Import some definitions of other classes, etc.
%import _defs.i
%import misc.i

//---------------------------------------------------------------------------

class wxBitmap {
public:
    wxBitmap(const wxString& name, long type);
    ~wxBitmap();

    void Create(int width, int height, int depth = -1);
    int GetDepth();
    int GetHeight();
    wxPalette* GetPalette();
    wxMask* GetMask();
    int GetWidth();
    bool LoadFile(const wxString& name, long flags);
    bool Ok();
    bool SaveFile(const wxString& name, int type, wxPalette* palette = NULL);
    void SetDepth(int depth);
    void SetHeight(int height);
    void SetMask(wxMask* mask);
    void SetOk(int isOk);
    void SetPalette(wxPalette* palette);
    void SetWidth(int width);
};

%new wxBitmap* wxEmptyBitmap(int width, int height, int depth=-1);
wxBitmap* wxNoRefBitmap(char* name, long flags);
%{                              // Alternate 'constructor'
    wxBitmap* wxEmptyBitmap(int width, int height, int depth=-1) {
        return new wxBitmap(width, height, depth);
    }

                                // This one won't own the reference, so Python won't call
                                // the dtor, this is good for toolbars and such where
                                // the parent will manage the bitmap.
    wxBitmap* wxNoRefBitmap(char* name, long flags) {
        return new wxBitmap(name, flags);
    }
%}

//---------------------------------------------------------------------------

class wxMask {
public:
    wxMask(const wxBitmap& bitmap);
    ~wxMask();
};

%new wxMask* wxMaskColour(const wxBitmap& bitmap, const wxColour& colour);
%{
    wxMask* wxMaskColour(const wxBitmap& bitmap, const wxColour& colour) {
        return new wxMask(bitmap, colour);
    }
%}


//---------------------------------------------------------------------------


class wxIcon : public wxBitmap {
public:
    wxIcon(char *name, long flags);
    ~wxIcon();

    int GetDepth();
    int GetHeight();
    int GetWidth();
    bool LoadFile(const wxString& name, long flags);
    bool Ok();
    void SetDepth(int depth);
    void SetHeight(int height);
    void SetOk(int isOk);
    void SetWidth(int width);
};

//---------------------------------------------------------------------------

class wxCursor : public wxBitmap {
public:
    wxCursor(const wxString& cursorName, long flags, int hotSpotX=0, int hotSpotY=0);
    ~wxCursor();
    bool Ok();
};

%new wxCursor* wxStockCursor(int id);
%{                              // Alternate 'constructor'
    wxCursor* wxStockCursor(int id) {
        return new wxCursor(id);
    }
%}

//----------------------------------------------------------------------

class wxFont {
public:
    // I'll do it this way to use long-lived objects and not have to
    // worry about when python may delete the object.
    %addmethods {
        wxFont( int pointSize, int family, int style, int weight,
                int underline=FALSE, char* faceName = "") {

            return wxTheFontList->FindOrCreateFont(pointSize, family, style, weight,
                                                   underline, faceName);
        }
        // NO Destructor.
    }


    wxString GetFaceName();
    int GetFamily();
    int GetFontId();
    int GetPointSize();
    int GetStyle();
    bool GetUnderlined();
    int GetWeight();
    void SetFaceName(const wxString& faceName);
    void SetFamily(int family);
    void SetPointSize(int pointSize);
    void SetStyle(int style);
    void SetUnderlined(bool underlined);
    void SetWeight(int weight);
};

//----------------------------------------------------------------------

class wxColour {
public:
    wxColour(unsigned char red=0, unsigned char green=0, unsigned char blue=0);
    ~wxColour();
    unsigned char Red();
    unsigned char Green();
    unsigned char Blue();
    bool Ok();
    void Set(unsigned char red, unsigned char green, unsigned char blue);
    %addmethods {
        PyObject* Get() {
            PyObject* rv = PyTuple_New(3);
            PyTuple_SetItem(rv, 0, PyInt_FromLong(self->Red()));
            PyTuple_SetItem(rv, 1, PyInt_FromLong(self->Green()));
            PyTuple_SetItem(rv, 2, PyInt_FromLong(self->Blue()));
            return rv;
        }
    }
};

%new wxColour* wxNamedColour(const wxString& colorName);
%{                                      // Alternate 'constructor'
    wxColour* wxNamedColour(const wxString& colorName) {
        return new wxColour(colorName);
    }
%}


//----------------------------------------------------------------------

typedef unsigned long wxDash;

class wxPen {
public:
    // I'll do it this way to use long-lived objects and not have to
    // worry about when python may delete the object.
    %addmethods {
        wxPen(wxColour* colour, int width=1, int style=wxSOLID) {
            return wxThePenList->FindOrCreatePen(*colour, width, style);
        }
        // NO Destructor.
    }

    int GetCap();
    wxColour& GetColour();

            // **** This one needs to return a list of ints (wxDash)
    int GetDashes(wxDash **dashes);
    int GetJoin();
    wxBitmap* GetStipple();
    int GetStyle();
    int GetWidth();
    bool Ok();
    void SetCap(int cap_style);
    void SetColour(wxColour& colour);
    void SetDashes(int LCOUNT, wxDash* LIST);
    void SetJoin(int join_style);
    void SetStipple(wxBitmap * stipple);
    void SetStyle(int style);
    void SetWidth(int width);
};

//----------------------------------------------------------------------

class wxBrush {
public:
    // I'll do it this way to use long-lived objects and not have to
    // worry about when python may delete the object.
    %addmethods {
        wxBrush(wxColour* colour, int style=wxSOLID) {
            return wxTheBrushList->FindOrCreateBrush(*colour, style);
        }
        // NO Destructor.
    }

    wxColour& GetColour();
    wxBitmap * GetStipple();
    int GetStyle();
    bool Ok();
    void SetColour(wxColour &colour);
    void SetStipple(wxBitmap *bitmap);
    void SetStyle(int style);
};

//----------------------------------------------------------------------



class wxDC {
public:
    wxDC();
    ~wxDC();

    void BeginDrawing();
    bool Blit(long xdest, long ydest, long width, long height,
                wxDC *source, long xsrc, long ysrc, long logical_func);
    void Clear();
    void CrossHair(long x, long y);
    void DestroyClippingRegion();
    long DeviceToLogicalX(long x);
    long DeviceToLogicalXRel(long x);
    long DeviceToLogicalY(long y);
    long DeviceToLogicalYRel(long y);
    void DrawArc(long x1, long y1, long x2, long y2, long xc, long yc);
    void DrawEllipse(long x, long y, long width, long height);
    void DrawEllipticArc(long x, long y, long width, long height, long start, long end);
    void DrawIcon(const wxIcon& icon, long x, long y);
    void DrawLine(long x1, long y1, long x2, long y2);
    void DrawLines(int LCOUNT, wxPoint* LIST, long xoffset=0, long yoffset=0);
    void DrawPolygon(int LCOUNT, wxPoint* LIST, long xoffset=0, long yoffset=0,
                     int fill_style=wxODDEVEN_RULE);
    void DrawPoint(long x, long y);
    void DrawRectangle(long x, long y, long width, long height);
    void DrawRoundedRectangle(long x, long y, long width, long height, long radius=20);
    void DrawSpline(int LCOUNT, wxPoint* LIST);
    void DrawText(const wxString& text, long x, long y);
    void EndDoc();
    void EndDrawing();
    void EndPage();
    void FloodFill(long x, long y, const wxColour& colour, int style=wxFLOOD_SURFACE);
    wxBrush * GetBackground();
    wxBrush * GetBrush();
    long GetCharHeight();
    long GetCharWidth();
    void GetClippingBox(long *OUTPUT, long *OUTPUT,
                        long *OUTPUT, long *OUTPUT);
    wxFont * GetFont();
    int GetLogicalFunction();
    int GetMapMode();
    bool GetOptimization();
    wxPen * GetPen();
    //bool GetPixel(int x, int y, wxColour *T_OUTPUT);  ****  See below.
    void GetSize(int* OUTPUT, int* OUTPUT); //void GetSize(long* OUTPUT, long* OUTPUT);
    wxColour& GetTextBackground();
    void GetTextExtent(const wxString& string, long *OUTPUT, long *OUTPUT,
                       long *OUTPUT, long *OUTPUT);
    wxColour& GetTextForeground();
    long LogicalToDeviceX(long x);
    long LogicalToDeviceXRel(long x);
    long LogicalToDeviceY(long y);
    long LogicalToDeviceYRel(long y);
    long MaxX();
    long MaxY();
    long MinX();
    long MinY();
    bool Ok();
    void SetDeviceOrigin(long x, long y);
    void SetBackground(const wxBrush& brush);
    void SetBackgroundMode(int mode);
    void SetClippingRegion(long x, long y, long width, long height);
    void SetPalette(const wxPalette& colourMap);
    void SetBrush(const wxBrush& brush);
    void SetFont(const wxFont& font);
    void SetLogicalFunction(int function);
    void SetMapMode(int mode);
    void SetOptimization(bool optimize);
    void SetPen(const wxPen& pen);
    void SetTextBackground(const wxColour& colour);
    void SetTextForeground(const wxColour& colour);
    void SetUserScale(double x_scale, double y_scale);
    bool StartDoc(const wxString& message);
    void StartPage();


    %addmethods {
        %new wxColour* GetPixel(long x, long y) {
            wxColor* wc = new wxColor();
            self->GetPixel(x, y, wc);
            return wc;
        }

            // This one is my own creation...
        void DrawBitmap(wxBitmap* bitmap, long x, long y, bool swapPalette=TRUE) {
            wxMemoryDC* memDC = new wxMemoryDC;
            memDC->SelectObject(bitmap);
            if (swapPalette)
                self->SetPalette(bitmap->GetPalette());
            self->Blit(x, y, bitmap->GetWidth(), bitmap->GetHeight(), memDC,
                    0, 0, self->GetLogicalFunction());
            memDC->SelectObject(wxNullBitmap);
            delete memDC;
        }
    }
};


//----------------------------------------------------------------------

class wxMemoryDC : public wxDC {
public:
    wxMemoryDC();

    void SelectObject(const wxBitmap& bitmap);
}

%new wxMemoryDC* wxMemoryDCFromDC(wxDC* oldDC);
%{                                      // Alternate 'constructor'
    wxMemoryDC* wxMemoryDCFromDC(wxDC* oldDC) {
        return new wxMemoryDC(oldDC);
    }
%}


//---------------------------------------------------------------------------

class wxScreenDC : public wxDC {
public:
    wxScreenDC();

    bool StartDrawingOnTop(wxWindow* window);
    %name(StartDrawingOnTopRect) bool StartDrawingOnTop(wxRect* rect = NULL);
    bool EndDrawingOnTop();
};

//---------------------------------------------------------------------------

class wxClientDC : public wxDC {
public:
      wxClientDC(wxWindow* win);
};

//---------------------------------------------------------------------------

class wxPaintDC : public wxDC {
public:
      wxPaintDC(wxWindow* win);
};

//---------------------------------------------------------------------------

class wxPostScriptDC : public wxDC {
public:
      wxPostScriptDC(const wxString& output, bool interactive = TRUE, wxWindow* win = NULL);
};

//---------------------------------------------------------------------------

class  wxPrinterDC : public wxDC {
public:
    wxPrinterDC(const wxString& driver, const wxString& device, const wxString& output,
                bool interactive = TRUE, int orientation = wxPORTRAIT);
};

//---------------------------------------------------------------------------

class wxMetaFileDC : public wxDC {
public:
    wxMetaFileDC(const wxString& filename = wxPyEmptyStr);
    wxMetaFile* Close();
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


%readonly
extern wxFont *wxNORMAL_FONT;
extern wxFont *wxSMALL_FONT;
extern wxFont *wxITALIC_FONT;
extern wxFont *wxSWISS_FONT;
extern wxPen *wxRED_PEN;

extern wxPen *wxCYAN_PEN;
extern wxPen *wxGREEN_PEN;
extern wxPen *wxBLACK_PEN;
extern wxPen *wxWHITE_PEN;
extern wxPen *wxTRANSPARENT_PEN;
extern wxPen *wxBLACK_DASHED_PEN;
extern wxPen *wxGREY_PEN;
extern wxPen *wxMEDIUM_GREY_PEN;
extern wxPen *wxLIGHT_GREY_PEN;

extern wxBrush *wxBLUE_BRUSH;
extern wxBrush *wxGREEN_BRUSH;
extern wxBrush *wxWHITE_BRUSH;
extern wxBrush *wxBLACK_BRUSH;
extern wxBrush *wxTRANSPARENT_BRUSH;
extern wxBrush *wxCYAN_BRUSH;
extern wxBrush *wxRED_BRUSH;
extern wxBrush *wxGREY_BRUSH;
extern wxBrush *wxMEDIUM_GREY_BRUSH;
extern wxBrush *wxLIGHT_GREY_BRUSH;

extern wxColour *wxBLACK;
extern wxColour *wxWHITE;
extern wxColour *wxRED;
extern wxColour *wxBLUE;
extern wxColour *wxGREEN;
extern wxColour *wxCYAN;
extern wxColour *wxLIGHT_GREY;

extern wxCursor *wxSTANDARD_CURSOR;
extern wxCursor *wxHOURGLASS_CURSOR;
extern wxCursor *wxCROSS_CURSOR;

extern wxBitmap wxNullBitmap;
extern wxIcon   wxNullIcon;
extern wxCursor wxNullCursor;
extern wxPen    wxNullPen;
extern wxBrush  wxNullBrush;
extern wxPalette wxNullPalette;
extern wxFont   wxNullFont;
extern wxColour wxNullColour;

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
//
// $Log$
// Revision 1.2  1998/08/15 07:36:35  RD
// - Moved the header in the .i files out of the code that gets put into
// the .cpp files.  It caused CVS conflicts because of the RCS ID being
// different each time.
//
// - A few minor fixes.
//
// Revision 1.1  1998/08/09 08:25:50  RD
// Initial version
//
//
