/////////////////////////////////////////////////////////////////////////////
// Name:        dcclient.h
// Purpose:     wxClientDC class
// Author:      David Webster
// Modified by:
// Created:     09/12/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows license
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
    DECLARE_DYNAMIC_CLASS(wxWindowDC)

public:
    wxWindowDC();

    // Create a DC corresponding to the whole window
    wxWindowDC(wxWindow *win);

    virtual ~wxWindowDC();

    // PM specific stuff
    HPS                     m_hPS;
private:
    HAB                     m_hab;
    SIZEL                   m_PageSize;
};

class WXDLLEXPORT wxClientDC : public wxWindowDC
{
    DECLARE_DYNAMIC_CLASS(wxClientDC)

public:
    wxClientDC();

    // Create a DC corresponding to the client area of the window
    wxClientDC(wxWindow *win);

    virtual ~wxClientDC();
};

class WXDLLEXPORT wxPaintDC : public wxWindowDC
{
    DECLARE_DYNAMIC_CLASS(wxPaintDC)

public:
    wxPaintDC();

    // Create a DC corresponding for painting the window in OnPaint()
    wxPaintDC(wxWindow *win);

    virtual ~wxPaintDC();

protected:
    static wxArrayDCInfo ms_cache;

    // find the entry for this DC in the cache (keyed by the window)
    wxPaintDCInfo *FindInCache(size_t *index = NULL) const;
};

#endif
    // _WX_DCCLIENT_H_
