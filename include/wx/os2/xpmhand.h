/////////////////////////////////////////////////////////////////////////////
// Name:        xpmhand.h
// Purpose:     XPM bitmap handler
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma interface "xpmhand.h"
#endif

#ifndef _WX_XPMHAND_H_
#define _WX_XPMHAND_H_

class WXDLLEXPORT wxXPMFileHandler: public wxBitmapHandler
{
    DECLARE_DYNAMIC_CLASS(wxXPMFileHandler)
public:
    inline wxXPMFileHandler(void)
    {
        m_sName = "XPM bitmap file";
        m_sExtension = "xpm";
        m_lType = wxBITMAP_TYPE_XPM;
    };

    virtual bool LoadFile( wxBitmap*       pBitmap
                          ,const wxString& rsName
                          ,long            lFlags
                          ,int             nDesiredWidth = -1
                          ,int             nDesiredHeight = -1
                         );
    virtual bool SaveFile( wxBitmap*        pBitmap
                          ,const wxString&  rsName
                          ,int              nType
                          ,const wxPalette* pPalette = NULL
                         );
}; // end of CLASS wxWPMFileHandler

class WXDLLEXPORT wxXPMDataHandler: public wxBitmapHandler
{
    DECLARE_DYNAMIC_CLASS(wxXPMDataHandler)
public:
    inline wxXPMDataHandler(void)
    {
        m_sName = "XPM bitmap data";
        m_sExtension = "xpm";
        m_lType = wxBITMAP_TYPE_XPM_DATA;
    };
    virtual bool Create( wxBitmap* pBitmap
                        ,void*     pData
                        ,long      lFlags
                        ,int       lWidth
                        ,int       lHeight
                        ,int       lDepth = 1
                       );
}; // end of wxXPMDataHandler

#endif
  // _WX_XPMHAND_H_

