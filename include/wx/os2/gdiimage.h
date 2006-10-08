///////////////////////////////////////////////////////////////////////////////
// Name:        include/wx/os2/gdiimage.h
// Purpose:     wxGDIImage class: base class for wxBitmap, wxIcon, wxCursor
//              under OS/2
// Author:      David Webster (adapted from msw version by Vadim Zeitlin)
// Modified by:
// Created:     20.11.99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 David Webster
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// NB: this is a private header, it is not intended to be directly included by
//     user code (but may be included from other, public, wxWin headers

#ifndef _WX_OS2_GDIIMAGE_H_
#define _WX_OS2_GDIIMAGE_H_

#include "wx/gdiobj.h"          // base class
#include "wx/gdicmn.h"          // wxBITMAP_TYPE_INVALID
#include "wx/list.h"

class WXDLLEXPORT wxGDIImageRefData;
class WXDLLEXPORT wxGDIImageHandler;
class WXDLLEXPORT wxGDIImage;

WX_DECLARE_EXPORTED_LIST(wxGDIImageHandler, wxGDIImageHandlerList);

// ----------------------------------------------------------------------------
// wxGDIImageRefData: common data fields for all derived classes
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxGDIImageRefData : public wxGDIRefData
{
public:
    wxGDIImageRefData()
    {
        m_nWidth = m_nHeight = m_nDepth = 0;

        m_hHandle = 0;
    }

    // accessors
    bool IsOk() const
    {
        if (m_hHandle == 0)
            return FALSE;
        return TRUE;
    }

    void SetSize( int nW
                 ,int nH
                )
        { m_nWidth = nW; m_nHeight = nH; }

    // free the ressources we allocated
    virtual void Free() { };

    // for compatibility, the member fields are public

    // the size of the image
    int                             m_nWidth;
    int                             m_nHeight;

    // the depth of the image
    int                             m_nDepth;

    // the handle to it
    union
    {
        WXHANDLE                    m_hHandle;     // for untyped access
        WXHBITMAP                   m_hBitmap;
        WXHICON                     m_hIcon;
        WXHCURSOR                   m_hCursor;
    };

    UINT                            m_uId;
};

// ----------------------------------------------------------------------------
// wxGDIImageHandler: a class which knows how to load/save wxGDIImages.
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxGDIImageHandler : public wxObject
{
public:
    // ctor
    wxGDIImageHandler() { m_lType = wxBITMAP_TYPE_INVALID; }
    wxGDIImageHandler( const wxString& rName
                      ,const wxString& rExt
                      ,long            lType
                     )
                     : m_sName(rName)
                     , m_sExtension(rExt)
    {
        m_lType = lType;
    }

    // accessors
    void SetName(const wxString& rName) { m_sName = rName; }
    void SetExtension(const wxString& rExt) { m_sExtension = rExt; }
    void SetType(long lType) { m_lType = lType; }

    wxString GetName() const { return m_sName; }
    wxString GetExtension() const { return m_sExtension; }
    long GetType() const { return m_lType; }

    // real handler operations: to implement in derived classes
    virtual bool Create( wxGDIImage* pImage
                        ,void*       pData
                        ,long        lFlags
                        ,int         nWidth
                        ,int         nHeight
                        ,int         nDepth = 1
                       ) = 0;
    virtual bool Load( wxGDIImage*     pImage
                      ,const wxString& rName
                      ,HPS             hPs
                      ,long            lFlags
                      ,int             nDesiredWidth
                      ,int             nDesiredHeight
                     ) = 0;
    virtual bool Load( wxGDIImage*     pImage
                      ,int             nId
                      ,long            lFlags
                      ,int             nDesiredWidth
                      ,int             nDesiredHeight
                     ) = 0;
    virtual bool Save( wxGDIImage*     pImage
                      ,const wxString& rName
                      ,int             lType
                     ) = 0;

protected:
    wxString                        m_sName;
    wxString                        m_sExtension;
    long                            m_lType;
}; // end of wxGDIImageHandler

// ----------------------------------------------------------------------------
// wxGDIImage: this class supports GDI image handlers which may be registered
// dynamically and will be used for loading/saving the images in the specified
// format. It also falls back to wxImage if no appropriate image is found.
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxGDIImage : public wxGDIObject
{
public:
    // handlers list interface
    static wxGDIImageHandlerList& GetHandlers() { return ms_handlers; }

    static void AddHandler(wxGDIImageHandler* hHandler);
    static void InsertHandler(wxGDIImageHandler* hHandler);
    static bool RemoveHandler(const wxString& rName);

    static wxGDIImageHandler* FindHandler(const wxString& rName);
    static wxGDIImageHandler* FindHandler(const wxString& rExtension, long lType);
    static wxGDIImageHandler* FindHandler(long lType);

    static void InitStandardHandlers();
    static void CleanUpHandlers();

    // access to the ref data casted to the right type
    wxGDIImageRefData *GetGDIImageData() const
        { return (wxGDIImageRefData *)m_refData; }

    // create data if we don't have it yet
    void EnsureHasData() { if ( IsNull() ) m_refData = CreateData(); }

    // accessors
    WXHANDLE GetHandle() const
    {
        wxGDIImageRefData*               pData;

        pData = GetGDIImageData();
        if (!pData)
            return 0;
        else
            return pData->m_hHandle;
    }
    void SetHandle(WXHANDLE hHandle)
    {
        wxGDIImageRefData*               pData;

        EnsureHasData();
        pData = GetGDIImageData();
        pData->m_hHandle = hHandle;
    }

    bool Ok() const { return IsOk(); }
    bool IsOk() const { return GetHandle() != 0; }

    int GetWidth() const { return IsNull() ? 0 : GetGDIImageData()->m_nWidth; }
    int GetHeight() const { return IsNull() ? 0 : GetGDIImageData()->m_nHeight; }
    int GetDepth() const { return IsNull() ? 0 : GetGDIImageData()->m_nDepth; }

    void SetWidth(int nW) { EnsureHasData(); GetGDIImageData()->m_nWidth = nW; }
    void SetHeight(int nH) { EnsureHasData(); GetGDIImageData()->m_nHeight = nH; }
    void SetDepth(int nD) { EnsureHasData(); GetGDIImageData()->m_nDepth = nD; }

    void SetSize( int nW
                 ,int nH
                )
    {
        EnsureHasData();
        GetGDIImageData()->SetSize(nW, nH);
    }
    void SetSize(const wxSize& rSize) { SetSize(rSize.x, rSize.y); }

    UINT GetId(void) const
    {
        wxGDIImageRefData*          pData;

        pData = GetGDIImageData();
        if (!pData)
            return 0;
        else
            return pData->m_uId;
    } // end of WxWinGdi_CGDIImage::GetId
    void SetId(UINT uId)
    {
        wxGDIImageRefData*          pData;

        EnsureHasData();
        pData = GetGDIImageData();
        pData->m_uId = uId;
    }
    // forward some of base class virtuals to wxGDIImageRefData
    bool             FreeResource(bool bForce = FALSE);
    virtual WXHANDLE GetResourceHandle();

protected:
    // create the data for the derived class here
    virtual wxGDIImageRefData* CreateData() const = 0;

    static wxGDIImageHandlerList    ms_handlers;
};

#endif // _WX_MSW_GDIIMAGE_H_
