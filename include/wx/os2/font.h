/////////////////////////////////////////////////////////////////////////////
// Name:        font.h
// Purpose:     wxFont class
// Author:      David Webster
// Modified by:
// Created:     10/06/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FONT_H_
#define _WX_FONT_H_

#include "wx/gdiobj.h"
#include "wx/os2/private.h"

WXDLLEXPORT_DATA(extern const wxChar*) wxEmptyString;

// ----------------------------------------------------------------------------
// wxFont
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxFont : public wxFontBase
{
public:
    // ctors and such
    wxFont() { Init(); }
    wxFont(const wxFont& rFont) { Init(); Ref(rFont); }

    wxFont( int             nSize
           ,int             nFamily
           ,int             nStyle
           ,int             nWeight
           ,bool            bUnderlined = false
           ,const wxString& rsFace = wxEmptyString
           ,wxFontEncoding  vEncoding = wxFONTENCODING_DEFAULT
          )
    {
        Init();

        (void)Create( nSize
                     ,nFamily
                     ,nStyle
                     ,nWeight
                     ,bUnderlined
                     ,rsFace
                     ,vEncoding
                    );
    }

    wxFont( const wxNativeFontInfo& rInfo
           ,WXHFONT                 hFont = 0
          )

    {
        Init();

        (void)Create( rInfo
                     ,hFont
                    );
    }

    wxFont(const wxString& rsFontDesc);

    bool Create( int             nSize
                ,int             nFamily
                ,int             nStyle
                ,int             nWeight
                ,bool            bUnderlined = false
                ,const wxString& rsFace = wxEmptyString
                ,wxFontEncoding  vEncoding = wxFONTENCODING_DEFAULT
               );
    bool Create( const wxNativeFontInfo& rInfo
                ,WXHFONT                 hFont = 0
               );

    virtual ~wxFont();

    //
    // Assignment
    //
    wxFont& operator=(const wxFont& rFont);

    //
    // Implement base class pure virtuals
    //
    virtual int               GetPointSize(void) const;
    virtual int               GetFamily(void) const;
    virtual int               GetStyle(void) const;
    virtual int               GetWeight(void) const;
    virtual bool              GetUnderlined(void) const;
    virtual wxString          GetFaceName(void) const;
    virtual wxFontEncoding    GetEncoding(void) const;
    virtual const wxNativeFontInfo* GetNativeFontInfo() const;

    virtual void SetPointSize(int nPointSize);
    virtual void SetFamily(int nFamily);
    virtual void SetStyle(int nStyle);
    virtual void SetWeight(int nWeight);
    virtual void SetFaceName(const wxString& rsFaceName);
    virtual void SetUnderlined(bool bUnderlined);
    virtual void SetEncoding(wxFontEncoding vEncoding);

    //
    // For internal use only!
    //
    void SetPS(HPS hPS);
    void SetFM( PFONTMETRICS pFM
               ,int          nNumFonts
              );
    //
    // Implementation only from now on
    // -------------------------------
    //
    virtual bool     IsFree(void) const;
    virtual bool     RealizeResource(void);
    virtual WXHANDLE GetResourceHandle(void);
    virtual bool     FreeResource(bool bForce = false);

    WXHFONT GetHFONT(void) const;

protected:
    virtual void DoSetNativeFontInfo(const wxNativeFontInfo& rInfo);

    //
    // Common part of all ctors
    //
    void Init(void);
    void Unshare(void);

private:
    DECLARE_DYNAMIC_CLASS(wxFont)
}; // end of wxFont

#endif // _WX_FONT_H_
