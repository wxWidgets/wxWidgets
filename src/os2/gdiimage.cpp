///////////////////////////////////////////////////////////////////////////////
// Name:        msw/gdiimage.cpp
// Purpose:     wxGDIImage implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     20.11.99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "gdiimage.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/string.h"
#endif // WX_PRECOMP

#include "wx/os2/private.h"

#include "wx/app.h"

#include "wx/os2/gdiimage.h"

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// all image handlers are declared/defined in this file because the outside
// world doesn't have to know about them (but only about wxBITMAP_TYPE_XXX ids)

class WXDLLEXPORT wxBMPFileHandler : public wxBitmapHandler
{
public:
    wxBMPFileHandler() : wxBitmapHandler(_T("Windows bitmap file"), _T("bmp"),
                                         wxBITMAP_TYPE_BMP)
    {
    }

    virtual bool LoadFile( wxBitmap*       pBitmap
                          ,const wxString& rName
                          ,HPS             hPs
                          ,long            lFlags
                          ,int             nDesiredWidth
                          ,int             nDesiredHeight
                         );
    virtual bool SaveFile( wxBitmap*        pBitmap
                          ,const wxString&  rName
                          ,int              lType
                          ,const wxPalette* pPalette = NULL
                         );

private:
    DECLARE_DYNAMIC_CLASS(wxBMPFileHandler)
};

class WXDLLEXPORT wxBMPResourceHandler: public wxBitmapHandler
{
public:
    wxBMPResourceHandler() : wxBitmapHandler(_T("Windows bitmap resource"),
                                             wxEmptyString,
                                             wxBITMAP_TYPE_BMP_RESOURCE)
    {
    }

    virtual bool LoadFile( wxBitmap*       pBitmap
                          ,const wxString& rName
                          ,HPS             hPs
                          ,long            lFlags
                          ,int             nDesiredWidth
                          ,int             nDesiredHeight
                         );

private:
    DECLARE_DYNAMIC_CLASS(wxBMPResourceHandler)
};

class WXDLLEXPORT wxIconHandler : public wxGDIImageHandler
{
public:
    wxIconHandler( const wxString& rName
                  ,const wxString& rExt
                  ,long            lType
                 ) : wxGDIImageHandler( rName
                                       ,rExt
                                       ,lType
                                      )
    {
    }

    // creating and saving icons is not supported
    virtual bool Create( wxGDIImage* WXUNUSED(pImage)
                        ,void*       WXUNUSED(pData)
                        ,long        WXUNUSED(lFlags)
                        ,int         WXUNUSED(nWidth)
                        ,int         WXUNUSED(nHeight)
                        ,int         WXUNUSED(nDepth) = 1
                       )
    {
        return(FALSE);
    }

    virtual bool Save( wxGDIImage*     WXUNUSED(pImage)
                      ,const wxString& WXUNUSED(rName)
                      ,int             WXUNUSED(nType)
                     )
    {
        return(FALSE);
    }
    virtual bool Load( wxGDIImage*     pImage
                      ,const wxString& rName
                      ,HPS             hPs
                      ,long            lFlags
                      ,int             nDesiredWidth
                      ,int             nDesiredHeight
                     )
    {
        wxIcon*                     pIcon = wxDynamicCast(pImage, wxIcon);
        wxCHECK_MSG(pIcon, FALSE, _T("wxIconHandler only works with icons"));

        return LoadIcon( pIcon
                        ,rName
                        ,hPs
                        ,lFlags
                        ,nDesiredWidth
                        ,nDesiredHeight
                       );
    }

protected:
    virtual bool LoadIcon( wxIcon*         pIcon
                          ,const wxString& rName
                          ,HPS             hPs
                          ,long            lFlags
                          ,int             nDesiredWidth = -1
                          ,int             nDesiredHeight = -1
                         ) = 0;
};

class WXDLLEXPORT wxICOFileHandler : public wxIconHandler
{
public:
    wxICOFileHandler() : wxIconHandler(_T("ICO icon file"),
                                       _T("ico"),
                                       wxBITMAP_TYPE_ICO)
    {
    }

    virtual bool LoadIcon( wxIcon *        pIcon
                          ,const wxString& rName
                          ,HPS             hPs
                          ,long            lFlags
                          ,int             nDesiredWidth = -1
                          ,int             nDesiredHeight = -1
                         );

private:
    DECLARE_DYNAMIC_CLASS(wxICOFileHandler)
};

class WXDLLEXPORT wxICOResourceHandler: public wxIconHandler
{
public:
    wxICOResourceHandler() : wxIconHandler(_T("ICO resource"),
                                           _T("ico"),
                                           wxBITMAP_TYPE_ICO_RESOURCE)
    {
    }

    virtual bool LoadIcon( wxIcon*         pIcon
                          ,const wxString& rName
                          ,HPS             hPs
                          ,long            lFlags
                          ,int             nDesiredWidth = -1
                          ,int             nDesiredHeight = -1
                         );

private:
    DECLARE_DYNAMIC_CLASS(wxICOResourceHandler)
};

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARIES
    IMPLEMENT_DYNAMIC_CLASS(wxBMPFileHandler, wxBitmapHandler)
    IMPLEMENT_DYNAMIC_CLASS(wxBMPResourceHandler, wxBitmapHandler)
    IMPLEMENT_DYNAMIC_CLASS(wxICOFileHandler, wxGDIImageHandler)
    IMPLEMENT_DYNAMIC_CLASS(wxICOResourceHandler, wxGDIImageHandler)
#endif

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

static wxSize GetHiconSize(WXHICON hicon);

// ============================================================================
// implementation
// ============================================================================

wxList wxGDIImage::ms_handlers;

// ----------------------------------------------------------------------------
// wxGDIImage functions forwarded to wxGDIImageRefData
// ----------------------------------------------------------------------------

bool wxGDIImage::FreeResource(
  bool                              WXUNUSED(bForce)
)
{
    if ( !IsNull() )
    {
        GetGDIImageData()->Free();
        GetGDIImageData()->m_hHandle = 0;
    }

    return(TRUE);
}

WXHANDLE wxGDIImage::GetResourceHandle()
{
    return GetHandle();
}

// ----------------------------------------------------------------------------
// wxGDIImage handler stuff
// ----------------------------------------------------------------------------

void wxGDIImage::AddHandler(
  wxGDIImageHandler*                pHandler
)
{
    ms_handlers.Append(pHandler);
}

void wxGDIImage::InsertHandler(
  wxGDIImageHandler*                pHandler
)
{
    ms_handlers.Insert(pHandler);
}

bool wxGDIImage::RemoveHandler(
  const wxString&                   rName
)
{
    wxGDIImageHandler*              pHandler = FindHandler(rName);

    if (pHandler)
    {
        ms_handlers.DeleteObject(pHandler);
        return(TRUE);
    }
    else
        return(FALSE);
}

wxGDIImageHandler* wxGDIImage::FindHandler(
  const wxString&                   rName
)
{
    wxNode*                         pNode = ms_handlers.First();

    while (pNode)
    {
        wxGDIImageHandler*          pHandler = (wxGDIImageHandler *)pNode->Data();

        if (pHandler->GetName() == rName)
            return(pHandler);
        pNode = pNode->Next();
    }
    return(NULL);
}

wxGDIImageHandler* wxGDIImage::FindHandler(
  const wxString&                   rExtension
, long                              lType
)
{
    wxNode*                         pNode = ms_handlers.First();

    while (pNode)
    {
        wxGDIImageHandler*          pHandler = (wxGDIImageHandler *)pNode->Data();

        if ((pHandler->GetExtension() = rExtension) &&
            (lType == -1 || pHandler->GetType() == lType))
        {
            return(pHandler);
        }
        pNode = pNode->Next();
    }
    return(NULL);
}

wxGDIImageHandler* wxGDIImage::FindHandler(
  long                              lType
)
{
    wxNode*                         pNode = ms_handlers.First();

    while (pNode)
    {
        wxGDIImageHandler*          pHandler = (wxGDIImageHandler *)pNode->Data();

        if (pHandler->GetType() == lType)
            return pHandler;
        pNode = pNode->Next();
    }
    return(NULL);
}

void wxGDIImage::CleanUpHandlers()
{
    wxNode*                         pNode = ms_handlers.First();

    while (pNode)
    {
        wxGDIImageHandler*          pHandler = (wxGDIImageHandler *)pNode->Data();
        wxNode*                     pNext    = pNode->Next();

        delete pHandler;
        delete pNode;
        pNode = pNext;
    }
}

void wxGDIImage::InitStandardHandlers()
{
    AddHandler(new wxBMPResourceHandler);
    AddHandler(new wxBMPFileHandler);

    // Not added by default: include xpmhand.h in your app
    // and call these in your wxApp::OnInit.
    //  AddHandler(new wxXPMFileHandler);
    //  AddHandler(new wxXPMDataHandler);

    AddHandler(new wxICOResourceHandler);
    AddHandler(new wxICOFileHandler);
}

// ----------------------------------------------------------------------------
// wxBitmap handlers
// ----------------------------------------------------------------------------

bool wxBMPResourceHandler::LoadFile(
  wxBitmap*                         pBitmap
, const wxString&                   rName
, HPS                               hPs
, long                              lFlags
, int                               nDesiredWidth
, int                               nDesiredHeight
)
{
    // TODO: load a bitmap from a file
    /*
    rBitmap->SetHBITMAP((WXHBITMAP)::LoadBitmap(wxGetInstance(), rName));

    wxBitmapRefData*                pData = bitmap->GetBitmapData();

    if (pBitmap->Ok())
    {
        BITMAP                       bm;

        if ( !::GetObject(GetHbitmapOf(*pBitmap), sizeof(BITMAP), (LPSTR) &bm) )
        {
            wxLogLastError("GetObject(HBITMAP)");
        }

        data->m_width = bm.bmWidth;
        data->m_height = bm.bmHeight;
        data->m_depth = bm.bmBitsPixel;
    }
    else
    {
        // it's probably not found
        wxLogError(wxT("Can't load bitmap '%s' from resources! Check .rc file."),
                   name.c_str());
    }

    return bitmap->Ok();
    */
    return(FALSE);
}

bool wxBMPFileHandler::LoadFile(
  wxBitmap*                         pBitmap
, const wxString&                   rName
, HPS                               hPs
, long                              WXUNUSED(lFlags)
, int                               WXUNUSED(nDesiredWidth)
, int                               WXUNUSED(nDesiredHeight)
)
{
#if wxUSE_IMAGE_LOADING_IN_OS2
    wxPalette*                      pPalette = NULL;

    bool                            bSuccess = FALSE; /* wxLoadIntoBitmap( WXSTRINGCAST rName
                                                                ,pBitmap
                                                                ,&pPalette
                                                               ) != 0; */
    if (bSuccess && pPalette)
    {
        pBitmap->SetPalette(*pPalette);
    }

    // it was copied by the bitmap if it was loaded successfully
    delete pPalette;

    return(bSuccess);
#else
    return(FALSE);
#endif
}

bool wxBMPFileHandler::SaveFile(
  wxBitmap*                         pBitmap
, const wxString&                   rName
, int                               WXUNUSED(nType)
, const wxPalette*                  pPal
)
{
#if wxUSE_IMAGE_LOADING_IN_OS2
    wxPalette*                      pActualPalette = (wxPalette *)pPal;

    if (!pActualPalette)
        pActualPalette = pBitmap->GetPalette();
    /* return(wxSaveBitmap( WXSTRINGCAST rName
                        ,pBitmap
                        ,pActualPalette
                       ) != 0); */
    return(FALSE);
#else
    return(FALSE);
#endif
}

// ----------------------------------------------------------------------------
// wxIcon handlers
// ----------------------------------------------------------------------------

bool wxICOFileHandler::LoadIcon(
  wxIcon*                           pIcon
, const wxString&                   rName
, HPS                               hPs
, long                              lFlags
, int                               nDesiredWidth
, int                               nDesiredHeight
)
{
#if wxUSE_RESOURCE_LOADING_IN_OS2
    pIcon->UnRef();

    // actual size
    wxSize                          vSize;

    // TODO: load icon directly from a file
    /*
#ifdef __WIN32__
    HICON hicon = ::ExtractIcon(wxGetInstance(), name, first);
    if ( !hicon )
    {
        wxLogSysError(_T("Failed to load icon from the file '%s'"),
                      name.c_str());

        return FALSE;
    }

    size = GetHiconSize(hicon);
#else // Win16
    HICON hicon = ReadIconFile((wxChar *)name.c_str(),
                               wxGetInstance(),
                               &size.x, &size.y);
#endif // Win32/Win16

    if ( (desiredWidth != -1 && desiredWidth != size.x) ||
         (desiredHeight != -1 && desiredHeight != size.y) )
    {
        wxLogDebug(_T("Returning FALSE from wxICOFileHandler::Load because "
                      "of the size mismatch: actual (%d, %d), "
                      "requested (%d, %d)"),
                      size.x, size.y,
                      desiredWidth, desiredHeight);

        ::DestroyIcon(hicon);

        return FALSE;
    }

    icon->SetHICON((WXHICON)hicon);
    icon->SetSize(size.x, size.y);

    return icon->Ok();
    */
    return(FALSE);
#else
    return(FALSE);
#endif
}

bool wxICOResourceHandler::LoadIcon(
  wxIcon*                           pIcon
, const wxString&                   rName
, HPS                               hPs
, long                              lFlags
, int                               WXUNUSED(nDesiredWidth)
, int                               WXUNUSED(nDesiredHeight)
)
{
    HPOINTER                        hIcon;

    hIcon = ::WinLoadFileIcon( (PSZ)rName.c_str()
                              ,TRUE // load for private use
                             );

    pIcon->SetSize(32, 32); // all OS/2 icons are 32 x 32


    pIcon->SetHICON((WXHICON)hIcon);

    return pIcon->Ok();
} // end of wxICOResourceHandler::LoadIcon

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

static wxSize GetHiconSize(
  WXHICON                           hicon
)
{
    wxSize                          vSize(32, 32);    // default

    // all OS/2 icons are 32x32
    return(vSize);
}

