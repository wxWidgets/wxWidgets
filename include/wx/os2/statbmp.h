/////////////////////////////////////////////////////////////////////////////
// Name:        statbmp.h
// Purpose:     wxStaticBitmap class
// Author:      David Webster
// Modified by:
// Created:     11/27/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_STATBMP_H_
#define _WX_STATBMP_H_

#ifdef __GNUG__
#pragma interface "statbmp.h"
#endif

#include "wx/control.h"
#include "wx/icon.h"

WXDLLEXPORT_DATA(extern const char*) wxStaticBitmapNameStr;

class WXDLLEXPORT wxStaticBitmap: public wxControl
{
    DECLARE_DYNAMIC_CLASS(wxStaticBitmap)
 public:
    inline wxStaticBitmap() { Init(); }

    inline wxStaticBitmap( wxWindow*         pParent
                          ,wxWindowID        nId
                          ,const wxGDIImage& rLabel
                          ,const wxPoint&    rPos = wxDefaultPosition
                          ,const wxSize&     rSize = wxDefaultSize
                          ,long              lStyle = 0
                          ,const wxString&   rName = wxStaticBitmapNameStr
                         )
    {
        Create(pParent, nId, rLabel, rPos, rSize, lStyle, rName);
    }

    bool Create( wxWindow*         pParent
                ,wxWindowID        nId
                ,const wxGDIImage& rLabel
                ,const wxPoint&    rPos = wxDefaultPosition
                ,const wxSize&     rSize = wxDefaultSize
                ,long              lStyle = 0
                ,const wxString&   rName = wxStaticBitmapNameStr
               );
    inline virtual ~wxStaticBitmap() { Free(); }

    virtual void SetIcon(const wxIcon& rIcon) { SetImage(rIcon); }
    virtual void SetBitmap(const wxBitmap& rBitmap) { SetImage(rBitmap); };

    // assert failure is provoked by an attempt to get an icon from bitmap or
    // vice versa
    const wxIcon& GetIcon() const
      { wxASSERT( m_bIsIcon ); return *(wxIcon *)m_pImage; }
    const wxBitmap& GetBitmap() const
      { wxASSERT( !m_bIsIcon ); return *(wxBitmap *)m_pImage; }

    // overriden base class virtuals
    virtual bool AcceptsFocus() const { return FALSE; }

protected:
    virtual wxSize DoGetBestSize() const;

    void Init() { m_bIsIcon = TRUE; m_pImage = NULL; }
    void Free();

    // TRUE if icon/bitmap is valid
    bool ImageIsOk() const;

    void SetImage(const wxGDIImage& rImage);

    // we can have either an icon or a bitmap
    bool                            m_bIsIcon;
    wxGDIImage*                     m_pImage;
};

#endif
    // _WX_STATBMP_H_
