/////////////////////////////////////////////////////////////////////////////
// Name:        dcclient.h
// Purpose:     wxClientDC class
// Author:      David Webster
// Modified by:
// Created:     09/12/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCCLIENT_H_
#define _WX_DCCLIENT_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/dc.h"
#include "wx/dynarray.h"

// ----------------------------------------------------------------------------
// array types
// ----------------------------------------------------------------------------

// this one if used by wxPaintDC only
struct WXDLLEXPORT wxPaintDCInfo;

WX_DECLARE_OBJARRAY(wxPaintDCInfo, wxArrayDCInfo);

// ----------------------------------------------------------------------------
// DC classes
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxWindowDC : public wxDC
{
public:
    wxWindowDC();

    //
    // Create a DC corresponding to the whole window
    //
    wxWindowDC(wxWindow* pWin);

protected:
    void InitDC(void);

    //
    // Override some base class virtuals
    //
    virtual void DoGetSize( int* pWidth
                           ,int* pHeight
                          ) const;

private:
    SIZEL                   m_PageSize;
    DECLARE_DYNAMIC_CLASS(wxWindowDC)
}; // end of CLASS wxWindowDC

class WXDLLEXPORT wxClientDC : public wxWindowDC
{
public:
    wxClientDC();
    virtual ~wxClientDC();

    wxClientDC(wxWindow *win);

protected:
    void InitDC(void);

    //
    // Override some base class virtuals
    //
    virtual void DoGetSize( int* pWidth
                           ,int* pHeight
                          ) const;

private:
    DECLARE_DYNAMIC_CLASS(wxClientDC)
}; // end of CLASS wxClientDC

class WXDLLEXPORT wxPaintDC : public wxClientDC
{
public:
    wxPaintDC();

    // Create a DC corresponding for painting the window in OnPaint()
    wxPaintDC(wxWindow* pWin);

    virtual ~wxPaintDC();

    // find the entry for this DC in the cache (keyed by the window)
    static WXHDC FindDCInCache(wxWindow* pWin);

protected:
    static wxArrayDCInfo ms_cache;

    // find the entry for this DC in the cache (keyed by the window)
    wxPaintDCInfo* FindInCache(size_t* pIndex = NULL) const;
private:
    DECLARE_DYNAMIC_CLASS(wxPaintDC)
}; // end of wxPaintDC

#endif
    // _WX_DCCLIENT_H_
