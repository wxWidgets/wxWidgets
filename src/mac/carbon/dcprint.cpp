/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/dcprint.cpp
// Purpose:     wxPrinterDC class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_PRINTING_ARCHITECTURE

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/dcprint.h"

#ifndef WX_PRECOMP
    #include "wx/msgdlg.h"
    #include "wx/math.h"
#endif

#include "wx/mac/uma.h"
#include "wx/mac/private/print.h"
#include "wx/graphics.h"

IMPLEMENT_CLASS(wxPrinterDC, wxDC)

class wxNativePrinterDC
{
public :
    wxNativePrinterDC() {}
    virtual ~wxNativePrinterDC() {}
    virtual bool StartDoc(  wxPrinterDC* dc , const wxString& message ) = 0;
    virtual void EndDoc( wxPrinterDC* dc ) = 0;
    virtual void StartPage( wxPrinterDC* dc ) = 0;
    virtual void EndPage( wxPrinterDC* dc ) = 0;
    virtual void GetSize( int *w , int *h) const = 0 ;
    virtual wxSize GetPPI() const = 0 ;

    // returns 0 in case of no Error, otherwise platform specific error codes
    virtual wxUint32 GetStatus() const = 0 ;
    bool Ok() { return GetStatus() == 0 ; }

    static wxNativePrinterDC* Create(wxPrintData* data) ;
} ;

class wxMacCarbonPrinterDC : public wxNativePrinterDC
{
public :
    wxMacCarbonPrinterDC( wxPrintData* data ) ;
    virtual ~wxMacCarbonPrinterDC() ;
    virtual bool StartDoc(  wxPrinterDC* dc , const wxString& message ) ;
    virtual void EndDoc( wxPrinterDC* dc ) ;
    virtual void StartPage( wxPrinterDC* dc ) ;
    virtual void EndPage( wxPrinterDC* dc ) ;
    virtual wxUint32 GetStatus() const { return m_err ; }
    virtual void GetSize( int *w , int *h) const ;
    virtual wxSize GetPPI() const ;
private :
#if !wxMAC_USE_CORE_GRAPHICS
    GrafPtr m_macPrintFormerPort ;
#endif
    wxCoord m_maxX ;
    wxCoord m_maxY ;
    wxSize  m_ppi ;
    OSStatus m_err ;
} ;

wxMacCarbonPrinterDC::wxMacCarbonPrinterDC( wxPrintData* data )
{
#if !wxMAC_USE_CORE_GRAPHICS
    ::GetPort( & m_macPrintFormerPort ) ;
#endif
    m_err = noErr ;
    wxMacCarbonPrintData *native = (wxMacCarbonPrintData*) data->GetNativeData() ;

    PMRect rPage;
    m_err = PMGetAdjustedPageRect(native->m_macPageFormat, &rPage);
    if ( m_err != noErr )
        return;

    m_maxX = wxCoord(rPage.right - rPage.left) ;
    m_maxY = wxCoord(rPage.bottom - rPage.top);

    PMResolution res;
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5 
    PMPrinter printer;
    PMSessionGetCurrentPrinter(native->m_macPrintSession, &printer);
    PMPrinterGetOutputResolution( printer, native->m_macPrintSettings, &res) ;
#else
    m_err = PMGetResolution((PMPageFormat) (native->m_macPageFormat), &res);
#endif
    m_ppi = wxSize(int(res.hRes), int(res.vRes));
}

wxMacCarbonPrinterDC::~wxMacCarbonPrinterDC()
{
#if !wxMAC_USE_CORE_GRAPHICS
    // nothing to release from print data, as wxPrinterDC has all data in its wxPrintData member
    ::SetPort( m_macPrintFormerPort ) ;
#endif
}

wxNativePrinterDC* wxNativePrinterDC::Create(wxPrintData* data)
{
    return new wxMacCarbonPrinterDC(data) ;
}

bool wxMacCarbonPrinterDC::StartDoc(  wxPrinterDC* dc , const wxString& WXUNUSED(message)  )
{
    if ( m_err )
        return false ;

    wxMacCarbonPrintData *native = (wxMacCarbonPrintData*) dc->GetPrintData().GetNativeData() ;

#if MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_4 && wxMAC_USE_CORE_GRAPHICS
    {
        CFStringRef s[1] = { kPMGraphicsContextCoreGraphics };
        CFArrayRef  graphicsContextsArray = CFArrayCreate(NULL, (const void**)s, 1, &kCFTypeArrayCallBacks);
        PMSessionSetDocumentFormatGeneration(native->m_macPrintSession, kPMDocumentFormatPDF, graphicsContextsArray, NULL);
        CFRelease(graphicsContextsArray);
    }
#endif
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_4 && wxMAC_USE_CORE_GRAPHICS
    m_err = PMSessionBeginCGDocument(native->m_macPrintSession,
              native->m_macPrintSettings,
              native->m_macPageFormat);
#else
    m_err = PMSessionBeginDocument(native->m_macPrintSession,
              native->m_macPrintSettings,
              native->m_macPageFormat);

#endif

    if ( m_err != noErr )
        return false;

    PMRect rPage;
    m_err = PMGetAdjustedPageRect(native->m_macPageFormat, &rPage);
    if ( m_err != noErr )
        return false ;

    m_maxX = wxCoord(rPage.right - rPage.left) ;
    m_maxY = wxCoord(rPage.bottom - rPage.top);

    PMResolution res;
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5 
    PMPrinter printer;
    PMSessionGetCurrentPrinter(native->m_macPrintSession, &printer);
    PMPrinterGetOutputResolution( printer, native->m_macPrintSettings, &res) ;
#else
    m_err = PMGetResolution((PMPageFormat) (native->m_macPageFormat), &res);
#endif
    m_ppi = wxSize(int(res.hRes), int(res.vRes));
    return true ;
}

void wxMacCarbonPrinterDC::EndDoc( wxPrinterDC* dc )
{
    if ( m_err )
        return ;

    wxMacCarbonPrintData *native = (wxMacCarbonPrintData*) dc->GetPrintData().GetNativeData() ;

    m_err = PMSessionEndDocument(native->m_macPrintSession);
}

void wxMacCarbonPrinterDC::StartPage( wxPrinterDC* dc )
{
    if ( m_err )
        return ;

    wxMacCarbonPrintData *native = (wxMacCarbonPrintData*) dc->GetPrintData().GetNativeData() ;

    m_err = PMSessionBeginPage(native->m_macPrintSession,
                 native->m_macPageFormat,
                 nil);

#if wxMAC_USE_CORE_GRAPHICS
    CGContextRef pageContext;
#endif
    if ( m_err == noErr )
    {
#if wxMAC_USE_CORE_GRAPHICS
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_4
        m_err = PMSessionGetCGGraphicsContext(native->m_macPrintSession,
                                            &pageContext );

#else
        m_err = PMSessionGetGraphicsContext(native->m_macPrintSession,
                                            kPMGraphicsContextCoreGraphics,
                                            (void**) &pageContext );
#endif
#else
        m_err = PMSessionGetGraphicsContext(native->m_macPrintSession,
                                            kPMGraphicsContextQuickdraw,
                                            (void**) &dc->m_macPort );
#endif
    }

    if ( m_err != noErr )
    {
        PMSessionEndPage(native->m_macPrintSession);
        PMSessionEndDocument(native->m_macPrintSession);
    }
    else
    {
        PMRect rPage;

        m_err = PMGetAdjustedPageRect(native->m_macPageFormat, &rPage);
        if ( !m_err )
        {
#if wxMAC_USE_CORE_GRAPHICS
            PMRect paperRect ;
            PMGetAdjustedPaperRect( native->m_macPageFormat , &paperRect ) ;
            // make sure (0,0) is at the upper left of the printable area (wx conventions)
            // Core Graphics initially has the lower left of the paper as 0,0
            CGContextTranslateCTM( pageContext , -paperRect.left , paperRect.bottom ) ;
            CGContextScaleCTM( pageContext , 1 , -1 ) ;
#else
            dc->SetDeviceLocalOrigin( (wxCoord) rPage.left, (wxCoord) rPage.top );
#endif
        }
        // since this is a non-critical error, we set the flag back
        m_err = noErr ;
    }
#if wxMAC_USE_CORE_GRAPHICS
    dc->SetGraphicsContext( wxGraphicsContext::CreateFromNative( pageContext ) );
#endif
}

void wxMacCarbonPrinterDC::EndPage( wxPrinterDC* dc )
{
    if ( m_err )
        return ;

    wxMacCarbonPrintData *native = (wxMacCarbonPrintData*) dc->GetPrintData().GetNativeData() ;

    m_err = PMSessionEndPage(native->m_macPrintSession);
    if ( m_err != noErr )
    {
        PMSessionEndDocument(native->m_macPrintSession);
    }
#if wxMAC_USE_CORE_GRAPHICS
    // the cg context we got when starting the page isn't valid anymore, so replace it
    dc->SetGraphicsContext( wxGraphicsContext::Create() );
#endif
}

void wxMacCarbonPrinterDC::GetSize( int *w , int *h) const
{
    if ( w )
        *w = m_maxX ;
    if ( h )
        *h = m_maxY ;
}

wxSize wxMacCarbonPrinterDC::GetPPI() const 
{
     return m_ppi ;
};

//
//
//

wxPrinterDC::wxPrinterDC(const wxPrintData& printdata)
{
    m_ok = false ;
    m_printData = printdata ;
    m_printData.ConvertToNative() ;
    m_nativePrinterDC = wxNativePrinterDC::Create( &m_printData ) ;
    if ( m_nativePrinterDC )
    {
        m_ok = m_nativePrinterDC->Ok() ;
        if ( !m_ok )
        {
            wxString message ;
            message.Printf( wxT("Print Error %u"), m_nativePrinterDC->GetStatus() ) ;
            wxMessageDialog dialog( NULL , message , wxEmptyString, wxICON_HAND | wxOK) ;
            dialog.ShowModal();
        }
        else
        {
            wxSize sz = GetPPI();
            m_mm_to_pix_x = mm2inches * sz.x;
            m_mm_to_pix_y = mm2inches * sz.y;        
        }
#if wxMAC_USE_CORE_GRAPHICS
        // we need at least a measuring context because people start measuring before a page
        // gets printed at all
        SetGraphicsContext( wxGraphicsContext::Create() );
#endif
    }
}

wxSize wxPrinterDC::GetPPI() const
{
    return m_nativePrinterDC->GetPPI() ;
}

wxPrinterDC::~wxPrinterDC(void)
{
    delete m_nativePrinterDC ;
}

bool wxPrinterDC::StartDoc( const wxString& message )
{
    wxASSERT_MSG( Ok() , wxT("Called wxPrinterDC::StartDoc from an invalid object") ) ;

    if ( !m_ok )
        return false ;

    if ( m_nativePrinterDC->StartDoc(this, message ) )
    {
        // in case we have to do additional things when successful
    }
    m_ok = m_nativePrinterDC->Ok() ;
    if ( !m_ok )
    {
        wxString message ;
        message.Printf( wxT("Print Error %u"), m_nativePrinterDC->GetStatus() ) ;
        wxMessageDialog dialog( NULL , message , wxEmptyString, wxICON_HAND | wxOK) ;
        dialog.ShowModal();
    }

    return m_ok ;
}

void wxPrinterDC::EndDoc(void)
{
    if ( !m_ok )
        return ;

    m_nativePrinterDC->EndDoc( this ) ;
    m_ok = m_nativePrinterDC->Ok() ;

    if ( !m_ok )
    {
        wxString message ;
        message.Printf( wxT("Print Error %u"), m_nativePrinterDC->GetStatus() ) ;
        wxMessageDialog dialog( NULL , message , wxEmptyString, wxICON_HAND | wxOK) ;
        dialog.ShowModal();
    }
}

wxRect wxPrinterDC::GetPaperRect()
{
    wxCoord w, h;
    GetSize(&w, &h);
    wxRect pageRect(0, 0, w, h);
    wxMacCarbonPrintData *native = (wxMacCarbonPrintData*) m_printData.GetNativeData() ;
    OSStatus err = noErr ;
    PMRect rPaper;
    err = PMGetAdjustedPaperRect(native->m_macPageFormat, &rPaper);
    if ( err != noErr )
        return pageRect;
    return wxRect(wxCoord(rPaper.left), wxCoord(rPaper.top),
        wxCoord(rPaper.right - rPaper.left), wxCoord(rPaper.bottom - rPaper.top));
}

void wxPrinterDC::StartPage(void)
{
    if ( !m_ok )
        return ;

    m_logicalFunction = wxCOPY;
    //  m_textAlignment = wxALIGN_TOP_LEFT;
    m_backgroundMode = wxTRANSPARENT;

    m_textForegroundColour = *wxBLACK;
    m_textBackgroundColour = *wxWHITE;
    m_pen = *wxBLACK_PEN;
    m_font = *wxNORMAL_FONT;
    m_brush = *wxTRANSPARENT_BRUSH;
    m_backgroundBrush = *wxWHITE_BRUSH;
#if !wxMAC_USE_CORE_GRAPHICS
    m_macFontInstalled = false ;
    m_macBrushInstalled = false ;
    m_macPenInstalled = false ;
#endif

    m_nativePrinterDC->StartPage(this) ;
    m_ok = m_nativePrinterDC->Ok() ;

}

void wxPrinterDC::EndPage(void)
{
    if ( !m_ok )
        return ;

    m_nativePrinterDC->EndPage(this) ;
    m_ok = m_nativePrinterDC->Ok() ;
}

void wxPrinterDC::DoGetSize(int *width, int *height) const
{
    wxCHECK_RET( m_ok , _T("GetSize() doesn't work without a valid wxPrinterDC") );
    m_nativePrinterDC->GetSize(width,  height ) ;
}

#endif
