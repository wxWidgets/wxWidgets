/////////////////////////////////////////////////////////////////////////////
// Name:        _cursor.i
// Purpose:     SWIG interface for wxCursor
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

class wxCursor : public wxGDIObject
{
public:
    %extend {
        wxCursor(const wxString* cursorName, long flags, int hotSpotX=0, int hotSpotY=0) {
#ifdef __WXGTK__
            wxCHECK_MSG(FALSE, NULL,
                        wxT("wxCursor constructor not implemented for wxGTK, use wxStockCursor, wxCursorFromImage, or wxCursorFromBits instead."));
#else
            return new wxCursor(*cursorName, flags, hotSpotX, hotSpotY);
#endif
        }
    }

    ~wxCursor();

    // alternate constructors
    %name(StockCursor) wxCursor(int id);
    %name(CursorFromImage) wxCursor(const wxImage& image);
    %extend {
        %name(CursorFromBits) wxCursor(PyObject* bits, int width, int  height,
                                         int hotSpotX=-1, int hotSpotY=-1,
                                         PyObject* maskBits=0) {
            char* bitsbuf;
            char* maskbuf = NULL;
            int   length;
            PyString_AsStringAndSize(bits, &bitsbuf, &length);
            if (maskBits)
                PyString_AsStringAndSize(maskBits, &maskbuf, &length);
            return new wxCursor(bitsbuf, width, height, hotSpotX, hotSpotY, maskbuf);
        }
    }
    
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
    
    %pythoncode { def __nonzero__(self): return self.Ok() }
};


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
