/////////////////////////////////////////////////////////////////////////////
// Name:        gdi.i
// Purpose:     SWIG interface file for wxDC, wxBrush, wxPen, etc.
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
#include <wx/imaglist.h>
#include <wx/fontmap.h>
#include <wx/fontenc.h>
#include <wx/fontmap.h>
#include <wx/fontutil.h>
#include <wx/dcbuffer.h>
#include <wx/iconbndl.h>
%}

//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

// Import some definitions of other classes, etc.
%import _defs.i
%import misc.i
%import fonts.i


//---------------------------------------------------------------------------
%{
    // Put some wx default wxChar* values into wxStrings.
    static const wxString wxPyEmptyString(wxT(""));
%}
//---------------------------------------------------------------------------

class wxGDIObject : public wxObject {
public:
    wxGDIObject();
    ~wxGDIObject();

    bool GetVisible();
    void SetVisible( bool visible );

    bool IsNull();

};

//---------------------------------------------------------------------------

class wxBitmap : public wxGDIObject
{
public:
    wxBitmap(const wxString& name, wxBitmapType type=wxBITMAP_TYPE_ANY);
    ~wxBitmap();

    wxPalette* GetPalette();
    wxMask* GetMask();
    bool LoadFile(const wxString& name, wxBitmapType type=wxBITMAP_TYPE_ANY);
    bool SaveFile(const wxString& name, wxBitmapType type, wxPalette* palette = NULL);
    void SetMask(wxMask* mask);
#ifdef __WXMSW__
    void SetPalette(wxPalette& palette);
#endif

    // wxGDIImage methods
#ifdef __WXMSW__
    long GetHandle();
    void SetHandle(long handle);
#endif
    bool Ok();
    int GetWidth();
    int GetHeight();
    int GetDepth();
    void SetWidth(int w);
    void SetHeight(int h);
    void SetDepth(int d);
#ifdef __WXMSW__
    void SetSize(const wxSize& size);
#endif

    wxBitmap GetSubBitmap( const wxRect& rect );
    bool CopyFromIcon(const wxIcon& icon);
#ifdef __WXMSW__
    bool CopyFromCursor(const wxCursor& cursor);
    int GetQuality();
    void SetQuality(int q);
#endif

    %pragma(python) addtoclass = "
    def __del__(self,gdic=gdic):
        try:
            if self.thisown == 1 :
                gdic.delete_wxBitmap(self)
        except:
            pass
"
};


// Declarations of some alternate "constructors"
%new wxBitmap* wxEmptyBitmap(int width, int height, int depth=-1);
%new wxBitmap* wxBitmapFromXPMData(PyObject* listOfStrings);
%new wxBitmap* wxBitmapFromIcon(const wxIcon& icon);
%new wxBitmap* wxBitmapFromBits(PyObject* bits, int width, int height, int depth = 1 );

//  #ifdef __WXMSW__
//  %new wxBitmap* wxBitmapFromData(PyObject* data, long type,
//                                  int width, int height, int depth = 1);
//  #endif



%{ // Implementations of some alternate "constructors"

    wxBitmap* wxEmptyBitmap(int width, int height, int depth=-1) {
        return new wxBitmap(width, height, depth);
    }

    static char** ConvertListOfStrings(PyObject* listOfStrings) {
        char**    cArray = NULL;
        int       count;

        if (!PyList_Check(listOfStrings)) {
            PyErr_SetString(PyExc_TypeError, "Expected a list of strings.");
            return NULL;
        }
        count = PyList_Size(listOfStrings);
        cArray = new char*[count];

        for(int x=0; x<count; x++) {
            // TODO: Need some validation and error checking here
            cArray[x] = PyString_AsString(PyList_GET_ITEM(listOfStrings, x));
        }
        return cArray;
    }


    wxBitmap* wxBitmapFromXPMData(PyObject* listOfStrings) {
        char**    cArray = NULL;
        wxBitmap* bmp;

        cArray = ConvertListOfStrings(listOfStrings);
        if (! cArray)
            return NULL;
        bmp = new wxBitmap(cArray);
        delete [] cArray;
        return bmp;
    }


    wxBitmap* wxBitmapFromIcon(const wxIcon& icon) {
        return new wxBitmap(icon);
    }


    wxBitmap* wxBitmapFromBits(PyObject* bits, int width, int height, int depth = 1 ) {
        char* buf;
        int   length;
        PyString_AsStringAndSize(bits, &buf, &length);
        return new wxBitmap(buf, width, height, depth);
    }


//  #ifdef __WXMSW__
//      wxBitmap* wxBitmapFromData(PyObject* data, long type,
//                                 int width, int height, int depth = 1) {
//          if (! PyString_Check(data)) {
//              PyErr_SetString(PyExc_TypeError, "Expected string object");
//              return NULL;
//          }
//          return new wxBitmap((void*)PyString_AsString(data), type, width, height, depth);
//      }
//  #endif
%}

//---------------------------------------------------------------------------

class wxMask : public wxObject {
public:
    wxMask(const wxBitmap& bitmap);
    //~wxMask();

    %addmethods { void Destroy() { delete self; } }
};

%new wxMask* wxMaskColour(const wxBitmap& bitmap, const wxColour& colour);
%{
    wxMask* wxMaskColour(const wxBitmap& bitmap, const wxColour& colour) {
        return new wxMask(bitmap, colour);
    }
%}


//---------------------------------------------------------------------------


class wxIcon : public wxGDIObject
{
public:
    wxIcon(const wxString& name, long flags,
           int desiredWidth = -1, int desiredHeight = -1);
    ~wxIcon();

#ifndef __WXMAC__
    bool LoadFile(const wxString& name, long flags);
#endif

    // wxGDIImage methods
#ifdef __WXMSW__
    long GetHandle();
    void SetHandle(long handle);
#endif
    bool Ok();
    int GetWidth();
    int GetHeight();
    int GetDepth();
    void SetWidth(int w);
    void SetHeight(int h);
    void SetDepth(int d);
#ifdef __WXMSW__
    void SetSize(const wxSize& size);
#endif
    void CopyFromBitmap(const wxBitmap& bmp);

    %pragma(python) addtoclass = "
    def __del__(self,gdic=gdic):
        try:
            if self.thisown == 1 :
                gdic.delete_wxIcon(self)
        except:
            pass
"
};


// Declarations of some alternate "constructors"
%new wxIcon* wxEmptyIcon();
%new wxIcon* wxIconFromXPMData(PyObject* listOfStrings);
%new wxIcon* wxIconFromBitmap(const wxBitmap& bmp);

%{ // Implementations of some alternate "constructors"
    wxIcon* wxEmptyIcon() {
        return new wxIcon();
    }

    wxIcon* wxIconFromXPMData(PyObject* listOfStrings) {
        char**  cArray = NULL;
        wxIcon* icon;

        cArray = ConvertListOfStrings(listOfStrings);
        if (! cArray)
            return NULL;
        icon = new wxIcon(cArray);
        delete [] cArray;
        return icon;
    }

    wxIcon* wxIconFromBitmap(const wxBitmap& bmp) {
        wxIcon* icon = new wxIcon();
        icon->CopyFromBitmap(bmp);
        return icon;
    }
%}

//---------------------------------------------------------------------------

class wxIconBundle
{
public:
    // default constructor
    wxIconBundle();

    // initializes the bundle with the icon(s) found in the file
    %name(wxIconBundleFromFile) wxIconBundle( const wxString& file, long type );

    // initializes the bundle with a single icon
    %name(wxIconBundleFromIcon)wxIconBundle( const wxIcon& icon );

    ~wxIconBundle();

    // adds the icon to the collection, if the collection already
    // contains an icon with the same width and height, it is
    // replaced
    void AddIcon( const wxIcon& icon );

    // adds all the icons contained in the file to the collection,
    // if the collection already contains icons with the same
    // width and height, they are replaced
    %name(AddIconFromFile)void AddIcon( const wxString& file, long type );

    // returns the icon with the given size; if no such icon exists,
    // returns the icon with size wxSYS_ICON_[XY]; if no such icon exists,
    // returns the first icon in the bundle
    const wxIcon& GetIcon( const wxSize& size ) const;
};

//---------------------------------------------------------------------------

class wxCursor : public wxGDIObject
{
public:
#ifdef __WXMSW__
    wxCursor(const wxString& cursorName, long flags, int hotSpotX=0, int hotSpotY=0);
#endif
    ~wxCursor();

    // wxGDIImage methods
#ifdef __WXMSW__
    long GetHandle();
    void SetHandle(long handle);
#endif
    bool Ok();
#ifdef __WXMSW__
    int GetWidth();
    int GetHeight();
    int GetDepth();
    void SetWidth(int w);
    void SetHeight(int h);
    void SetDepth(int d);
    void SetSize(const wxSize& size);
#endif
};

%name(wxStockCursor) %new wxCursor* wxPyStockCursor(int id);
%{                              // Alternate 'constructor'
    wxCursor* wxPyStockCursor(int id) {
        return new wxCursor(id);
    }
%}

//----------------------------------------------------------------------

class wxColour : public wxObject {
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
    %pragma(python) addtoclass = "asTuple = Get"
    %pragma(python) addtoclass = "def __str__(self): return str(self.asTuple())"
    %pragma(python) addtoclass = "def __repr__(self): return str(self.asTuple())"

};

%new wxColour* wxNamedColour(const wxString& colorName);

%{                                      // Alternate 'constructor'
    wxColour* wxNamedColour(const wxString& colorName) {
        return new wxColour(colorName);
    }
%}



class wxColourDatabase : public wxObject {
public:

    wxColour *FindColour(const wxString& colour);
    wxString FindName(const wxColour& colour) const;

    %addmethods {
        void Append(const wxString& name, int red, int green, int blue) {
            // first see if the name is already there
            wxString cName = name;
            cName.MakeUpper();
            wxString cName2 = cName;
            if ( !cName2.Replace(wxT("GRAY"), wxT("GREY")) )
                cName2.clear();

            wxNode *node = self->First();
            while ( node ) {
                const wxChar *key = node->GetKeyString();
                if ( cName == key || cName2 == key ) {
                    wxColour* c = (wxColour *)node->Data();
                    c->Set(red, green, blue);
                    return;
                }
                node = node->Next();
            }

            // otherwise append the new colour
            self->Append(name.c_str(), new wxColour(red, green, blue));
        }
    }
};


//----------------------------------------------------------------------

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


    void SetDashes(int LCOUNT, wxDash* choices);
    //int GetDashes(wxDash **dashes);
    %addmethods {
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
};




// The list of ints for the dashes needs to exist for the life of the pen
// so we make it part of the class to save it.  wxPyPen is aliased to wxPen
// in _extras.py

%{
class wxPyPen : public wxPen {
public:
    wxPyPen(wxColour& colour, int width=1, int style=wxSOLID)
        : wxPen(colour, width, style)
        { m_dash = NULL; }
    ~wxPyPen() {
        if (m_dash)
            delete [] m_dash;
    }

    void SetDashes(int nb_dashes, const wxDash *dash) {
        if (m_dash)
            delete [] m_dash;
        m_dash = new wxDash[nb_dashes];
        for (int i=0; i<nb_dashes; i++) {
            m_dash[i] = dash[i];
        }
        wxPen::SetDashes(nb_dashes, m_dash);
    }

private:
    wxDash* m_dash;
};
%}


class wxPyPen : public wxPen {
public:
    wxPyPen(wxColour& colour, int width=1, int style=wxSOLID);
    ~wxPyPen();

    void SetDashes(int LCOUNT, wxDash* choices);
};




class wxPenList : public wxObject {
public:

    void AddPen(wxPen* pen);
    wxPen* FindOrCreatePen(const wxColour& colour, int width, int style);
    void RemovePen(wxPen* pen);

    int GetCount();
};



//----------------------------------------------------------------------

class wxBrush : public wxGDIObject {
public:
    wxBrush(const wxColour& colour, int style=wxSOLID);
    ~wxBrush();

    wxColour GetColour();
    wxBitmap * GetStipple();
    int GetStyle();
    bool Ok();
    void SetColour(wxColour &colour);
    void SetStipple(wxBitmap& bitmap);
    void SetStyle(int style);
};


class wxBrushList : public wxObject {
public:

    void AddBrush(wxBrush *brush);
    wxBrush * FindOrCreateBrush(const wxColour& colour, int style);
    void RemoveBrush(wxBrush *brush);

    int GetCount();
};

//----------------------------------------------------------------------



class wxDC : public wxObject {
public:
//    wxDC(); **** abstract base class, can't instantiate.
    ~wxDC();

    void BeginDrawing();
//  %name(BlitXY)
    bool Blit(long xdest, long ydest,
              long width, long height,
              wxDC *source, long xsrc, long ysrc,
              int logicalFunc = wxCOPY, int useMask = FALSE);
//      bool Blit(const wxPoint& destPt, const wxSize& sz,
//                wxDC *source, const wxPoint& srcPt,
//                int logicalFunc = wxCOPY, int useMask = FALSE);

    void Clear();
    void CrossHair(long x, long y);
    void DestroyClippingRegion();
    long DeviceToLogicalX(long x);
    long DeviceToLogicalXRel(long x);
    long DeviceToLogicalY(long y);
    long DeviceToLogicalYRel(long y);
    void DrawArc(long x1, long y1, long x2, long y2, long xc, long yc);
    void DrawCircle(long x, long y, long radius);
    void DrawEllipse(long x, long y, long width, long height);
    void DrawEllipticArc(long x, long y, long width, long height, long start, long end);
    void DrawIcon(const wxIcon& icon, long x, long y);

    void DrawLabel(const wxString& text, const wxRect& rect,
                   int alignment = wxALIGN_LEFT | wxALIGN_TOP,
                   int indexAccel = -1);

    %addmethods {
        wxRect DrawImageLabel(const wxString& text,
                              const wxBitmap& image,
                              const wxRect& rect,
                              int alignment = wxALIGN_LEFT | wxALIGN_TOP,
                              int indexAccel = -1) {
            wxRect rv;
            self->DrawLabel(text, image, rect, alignment, indexAccel, &rv);
            return rv;
        }
    }

    void DrawLine(long x1, long y1, long x2, long y2);
    void DrawLines(int PCOUNT, wxPoint* points, long xoffset=0, long yoffset=0);
    void DrawPolygon(int PCOUNT, wxPoint* points, long xoffset=0, long yoffset=0,
                     int fill_style=wxODDEVEN_RULE);
    void DrawPoint(long x, long y);
    void DrawRectangle(long x, long y, long width, long height);
    %name(DrawRectangleRect)void DrawRectangle(const wxRect& rect);
    void DrawRotatedText(const wxString& text, wxCoord x, wxCoord y, double angle);
    void DrawRoundedRectangle(long x, long y, long width, long height, long radius=20);
    void DrawSpline(int PCOUNT, wxPoint* points);
    void DrawText(const wxString& text, long x, long y);
    void EndDoc();
    void EndDrawing();
    void EndPage();
    bool FloodFill(long x, long y, const wxColour& colour, int style=wxFLOOD_SURFACE);
    wxBrush  GetBackground();
    wxBrush  GetBrush();
    long GetCharHeight();
    long GetCharWidth();
    void GetClippingBox(long *OUTPUT, long *OUTPUT,
                        long *OUTPUT, long *OUTPUT);
    wxFont GetFont();
    int GetLogicalFunction();
    void GetLogicalScale(double *OUTPUT, double *OUTPUT);
    int GetMapMode();
    bool GetOptimization();
    wxPen GetPen();
    %addmethods {
        %new wxColour* GetPixel(long x, long y) {
            wxColour* wc = new wxColour();
            self->GetPixel(x, y, wc);
            return wc;
        }
    }
    %name(GetSizeTuple)void GetSize(int* OUTPUT, int* OUTPUT);
    wxSize GetSize();
    wxSize GetSizeMM();
    wxColour GetTextBackground();
    void GetTextExtent(const wxString& string, long *OUTPUT, long *OUTPUT);
    %name(GetFullTextExtent)void GetTextExtent(const wxString& string,
                       long *OUTPUT, long *OUTPUT, long *OUTPUT, long* OUTPUT,
                       const wxFont* font = NULL);
    wxColour GetTextForeground();
    void GetUserScale(double *OUTPUT, double *OUTPUT);
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
    %name(SetClippingRegionAsRegion) void SetClippingRegion(const wxRegion& region);
    %name(SetClippingRect) void SetClippingRegion(const wxRect& rect);
    void SetPalette(const wxPalette& colourMap);
    void SetBrush(const wxBrush& brush);
    void SetFont(const wxFont& font);
    void SetLogicalFunction(int function);
    void SetLogicalScale(double x, double y);
    void SetMapMode(int mode);
    void SetOptimization(bool optimize);
    void SetPen(const wxPen& pen);
    void SetTextBackground(const wxColour& colour);
    void SetTextForeground(const wxColour& colour);
    void SetUserScale(double x_scale, double y_scale);
    bool StartDoc(const wxString& message);
    void StartPage();



    void DrawBitmap(const wxBitmap& bitmap, long x, long y,
                    int useMask = FALSE);

    bool CanDrawBitmap();
    bool CanGetTextExtent();
    int  GetDepth();
    wxSize GetPPI();

    void GetLogicalOrigin(int *OUTPUT, int *OUTPUT);
    void SetLogicalOrigin(int x, int y);
    void GetDeviceOrigin(int *OUTPUT, int *OUTPUT);
    void SetAxisOrientation(bool xLeftRight, bool yBottomUp);

    void CalcBoundingBox(int x, int y);
    void ResetBoundingBox();

    %addmethods {
        void GetBoundingBox(int* OUTPUT, int* OUTPUT, int* OUTPUT, int* OUTPUT);
        // See below for implementation
    }

#ifdef __WXMSW__
    long GetHDC();
#endif


    %addmethods {
        // NOTE: These methods are VERY SIMILAR in implentation.  It would be
        // nice to factor out common code and or turn them into a set of
        // template-like macros.

        // Draw a point for every set of coordinants in pyPoints, optionally
        // setting a new pen for each
        PyObject* _DrawPointList(PyObject* pyPoints, PyObject* pyPens) {
            wxPyBeginBlockThreads();

            bool      isFastSeq  = PyList_Check(pyPoints) || PyTuple_Check(pyPoints);
            bool      isFastPens = PyList_Check(pyPens) || PyTuple_Check(pyPens);
            int       numObjs = 0;
            int       numPens = 0;
            wxPen*    pen;
            PyObject* obj;
            int       x1, y1;
            int       i = 0;
            PyObject* retval;

            if (!PySequence_Check(pyPoints)) {
                goto err0;
            }
            if (!PySequence_Check(pyPens)) {
                goto err1;
            }
            numObjs = PySequence_Length(pyPoints);
            numPens = PySequence_Length(pyPens);

            for (i = 0; i < numObjs; i++) {
                // Use a new pen?
                if (i < numPens) {
                    if (isFastPens) {
                        obj = PySequence_Fast_GET_ITEM(pyPens, i);
                    }
                    else {
                        obj = PySequence_GetItem(pyPens, i);
                    }
                    if (SWIG_GetPtrObj(obj, (void **) &pen, "_wxPen_p")) {
                        if (!isFastPens)
                            Py_DECREF(obj);
                        goto err1;
                    }

                    self->SetPen(*pen);
                    if (!isFastPens)
                        Py_DECREF(obj);
                }

                // Get the point coordinants
                if (isFastSeq) {
                    obj = PySequence_Fast_GET_ITEM(pyPoints, i);
                }
                else {
                    obj = PySequence_GetItem(pyPoints, i);
                }
                if (! _2int_seq_helper(obj, &x1, &y1)) {
                    if (!isFastPens)
                        Py_DECREF(obj);
                    goto err0;
                }
                if (PyErr_Occurred()) {
                    retval = NULL;
                    if (!isFastPens)
                        Py_DECREF(obj);
                    goto exit;
                }


                // Now draw the point
                self->DrawPoint(x1, y1);

                if (!isFastSeq)
                    Py_DECREF(obj);
            }

            Py_INCREF(Py_None);
            retval = Py_None;
            goto exit;

        err1:
            PyErr_SetString(PyExc_TypeError, "Expected a sequence of wxPens");
            retval = NULL;
            goto exit;
        err0:
            PyErr_SetString(PyExc_TypeError, "Expected a sequence of (x,y) sequences.");
            retval = NULL;
            goto exit;

        exit:
            wxPyEndBlockThreads();
            return retval;
        }


        // Draw a line for every set of coordinants in pyLines, optionally
        // setting a new pen for each
        PyObject* _DrawLineList(PyObject* pyLines, PyObject* pyPens) {
            wxPyBeginBlockThreads();

            bool      isFastSeq  = PyList_Check(pyLines) || PyTuple_Check(pyLines);
            bool      isFastPens = PyList_Check(pyPens) || PyTuple_Check(pyPens);
            int       numObjs = 0;
            int       numPens = 0;
            wxPen*    pen;
            PyObject* obj;
            int       x1, y1, x2, y2;
            int       i = 0;
            PyObject* retval;

            if (!PySequence_Check(pyLines)) {
                goto err0;
            }
            if (!PySequence_Check(pyPens)) {
                goto err1;
            }
            numObjs = PySequence_Length(pyLines);
            numPens = PySequence_Length(pyPens);

            for (i = 0; i < numObjs; i++) {
                // Use a new pen?
                if (i < numPens) {
                    if (isFastPens) {
                        obj = PySequence_Fast_GET_ITEM(pyPens, i);
                    }
                    else {
                        obj = PySequence_GetItem(pyPens, i);
                    }
                    if (SWIG_GetPtrObj(obj, (void **) &pen, "_wxPen_p")) {
                        if (!isFastPens)
                            Py_DECREF(obj);
                        goto err1;
                    }

                    self->SetPen(*pen);
                    if (!isFastPens)
                        Py_DECREF(obj);
                }

                // Get the line coordinants
                if (isFastSeq) {
                    obj = PySequence_Fast_GET_ITEM(pyLines, i);
                }
                else {
                    obj = PySequence_GetItem(pyLines, i);
                }
                if (! _4int_seq_helper(obj, &x1, &y1, &x2, &y2)) {
                    if (!isFastPens)
                        Py_DECREF(obj);
                    goto err0;
                }
                if (PyErr_Occurred()) {
                    retval = NULL;
                    if (!isFastPens)
                        Py_DECREF(obj);
                    goto exit;
                }

                // Now draw the line
                self->DrawLine(x1, y1, x2, y2);

                if (!isFastSeq)
                    Py_DECREF(obj);
            }

            Py_INCREF(Py_None);
            retval = Py_None;
            goto exit;

        err1:
            PyErr_SetString(PyExc_TypeError, "Expected a sequence of wxPens");
            retval = NULL;
            goto exit;

        err0:
            PyErr_SetString(PyExc_TypeError, "Expected a sequence of (x1,y1, x2,y2) sequences.");
            retval = NULL;
            goto exit;

        exit:
            wxPyEndBlockThreads();
            return retval;
        }
    }


    %pragma(python) addtoclass = "
    def DrawPointList(self, points, pens=None):
        if pens is None:
           pens = []
        elif isinstance(pens, wxPenPtr):
           pens = [pens]
        elif len(pens) != len(points):
           raise ValueError('points and pens must have same length')
        return self._DrawPointList(points, pens)

    def DrawLineList(self, lines, pens=None):
        if pens is None:
           pens = []
        elif isinstance(pens, wxPenPtr):
           pens = [pens]
        elif len(pens) != len(lines):
           raise ValueError('lines and pens must have same length')
        return self._DrawLineList(lines, pens)
"


};



%{
static void wxDC_GetBoundingBox(wxDC* dc, int* x1, int* y1, int* x2, int* y2) {
    *x1 = dc->MinX();
    *y1 = dc->MinY();
    *x2 = dc->MaxX();
    *y2 = dc->MaxY();
}
%}

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

class wxBufferedDC : public wxMemoryDC {
public:
        // Construct a wxBufferedDC using a user supplied buffer.
    wxBufferedDC( wxDC *dc, const wxBitmap &buffer );

        // Construct a wxBufferedDC with an internal buffer of 'area'
        // (where area is usually something like the size of the window
        // being buffered)
    %name(wxBufferedDCInternalBuffer)wxBufferedDC( wxDC *dc, const wxSize &area );

        // Blits the buffer to the dc, and detaches the dc from
        // the buffer.  Usually called in the dtor or by the dtor
        // of derived classes if the BufferedDC must blit before
        // the derived class (which may own the dc it's blitting
        // to) is destroyed.
    void UnMask();


    %pragma(python) addtomethod =
        "__init__:self._dc = _args[0] # save a ref so the other dc won't be deleted before self"
    %pragma(python) addtomethod =
        "wxBufferedDCInternalBuffer:val._dc = _args[0] # save a ref so the other dc won't be deleted before self"
};


class wxBufferedPaintDC : public wxBufferedDC
{
public:
    wxBufferedPaintDC( wxWindow *window, const wxBitmap &buffer = wxNullBitmap );
};

//---------------------------------------------------------------------------

class wxScreenDC : public wxDC {
public:
    wxScreenDC();

    %name(StartDrawingOnTopWin) bool StartDrawingOnTop(wxWindow* window);
    bool StartDrawingOnTop(wxRect* rect = NULL);
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

class wxWindowDC : public wxDC {
public:
      wxWindowDC(wxWindow* win);
};

//---------------------------------------------------------------------------


#ifdef __WXMSW__

%{
#include <wx/metafile.h>
%}

class wxMetaFile : public wxObject {
public:
    wxMetaFile(const wxString& filename = wxPyEmptyString);
    ~wxMetaFile();

    bool Ok();
    bool SetClipboard(int width = 0, int height = 0);

    wxSize GetSize();
    int GetWidth();
    int GetHeight();

    const wxString& GetFileName() const { return m_filename; }

};

// bool wxMakeMetaFilePlaceable(const wxString& filename,
//                              int minX, int minY, int maxX, int maxY, float scale=1.0);


class wxMetaFileDC : public wxDC {
public:
    wxMetaFileDC(const wxString& filename = wxPyEmptyString,
                 int width = 0, int height = 0,
                 const wxString& description = wxPyEmptyString);
    wxMetaFile* Close();
};

#endif

//---------------------------------------------------------------------------

class wxPalette : public wxGDIObject {
public:
    wxPalette(int LCOUNT, byte* choices, byte* choices, byte* choices);
    ~wxPalette();

    int GetPixel(byte red, byte green, byte blue);
    bool GetRGB(int pixel, byte* OUTPUT, byte* OUTPUT, byte* OUTPUT);
    bool Ok();
};

//---------------------------------------------------------------------------

enum {
    wxIMAGELIST_DRAW_NORMAL ,
    wxIMAGELIST_DRAW_TRANSPARENT,
    wxIMAGELIST_DRAW_SELECTED,
    wxIMAGELIST_DRAW_FOCUSED,
    wxIMAGE_LIST_NORMAL,
    wxIMAGE_LIST_SMALL,
    wxIMAGE_LIST_STATE
};

class wxImageList : public wxObject {
public:
    wxImageList(int width, int height, int mask=TRUE, int initialCount=1);
    ~wxImageList();

    int Add(const wxBitmap& bitmap, const wxBitmap& mask = wxNullBitmap);
    %name(AddWithColourMask)int Add(const wxBitmap& bitmap, const wxColour& maskColour);
    %name(AddIcon)int Add(const wxIcon& icon);
#ifdef __WXMSW__
    bool Replace(int index, const wxBitmap& bitmap, const wxBitmap& mask = wxNullBitmap);
#else
//      %name(ReplaceIcon)bool Replace(int index, const wxIcon& icon);
//      int Add(const wxBitmap& bitmap);
    bool Replace(int index, const wxBitmap& bitmap);
#endif

    bool Draw(int index, wxDC& dc, int x, int x, int flags = wxIMAGELIST_DRAW_NORMAL,
              const bool solidBackground = FALSE);

    int GetImageCount();
    bool Remove(int index);
    bool RemoveAll();
    void GetSize(int index, int& OUTPUT, int& OUTPUT);
};


//---------------------------------------------------------------------------
// Regions, etc.

enum wxRegionContain {
	wxOutRegion, wxPartRegion, wxInRegion
};


class wxRegion : public wxGDIObject {
public:
    wxRegion(long x=0, long y=0, long width=0, long height=0);
#ifndef __WXMAC__
    %name(wxRegionFromPoints)wxRegion(int PCOUNT, wxPoint* points, int fillStyle = wxWINDING_RULE);
#endif
    ~wxRegion();

    void Clear();
#ifndef __WXMAC__
    bool Offset(wxCoord x, wxCoord y);
#endif

    wxRegionContain Contains(long x, long y);
    %name(ContainsPoint)wxRegionContain Contains(const wxPoint& pt);
    %name(ContainsRect)wxRegionContain Contains(const wxRect& rect);
    %name(ContainsRectDim)wxRegionContain Contains(long x, long y, long w, long h);

    wxRect GetBox();

    bool Intersect(long x, long y, long width, long height);
    %name(IntersectRect)bool Intersect(const wxRect& rect);
    %name(IntersectRegion)bool Intersect(const wxRegion& region);

    bool IsEmpty();

    bool Union(long x, long y, long width, long height);
    %name(UnionRect)bool Union(const wxRect& rect);
    %name(UnionRegion)bool Union(const wxRegion& region);

    bool Subtract(long x, long y, long width, long height);
    %name(SubtractRect)bool Subtract(const wxRect& rect);
    %name(SubtractRegion)bool Subtract(const wxRegion& region);

    bool Xor(long x, long y, long width, long height);
    %name(XorRect)bool Xor(const wxRect& rect);
    %name(XorRegion)bool Xor(const wxRegion& region);
};



class wxRegionIterator : public wxObject {
public:
    wxRegionIterator(const wxRegion& region);
    ~wxRegionIterator();

    long GetX();
    long GetY();
    long GetW();
    long GetWidth();
    long GetH();
    long GetHeight();
    wxRect GetRect();
    bool HaveRects();
    void Reset();

    %addmethods {
        void Next() {
            (*self) ++;
        }
    };
};


//---------------------------------------------------------------------------

%readonly
%{
#if 0
%}

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


extern wxFontList*       wxTheFontList;
extern wxPenList*        wxThePenList;
extern wxBrushList*      wxTheBrushList;
extern wxColourDatabase* wxTheColourDatabase;


%readwrite
%{
#endif
%}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

