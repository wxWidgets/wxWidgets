/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/classic/dcprint.cpp
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
    virtual wxCoord GetMaxX() const = 0 ;
    virtual wxCoord GetMaxY() const = 0 ;
    // returns 0 in case of no Error, otherwise platform specific error codes
    virtual wxUint32 GetStatus() const = 0 ;
    bool Ok() { return GetStatus() == 0 ; }

    static wxNativePrinterDC* Create(wxPrintData* data) ;
} ;

#if TARGET_CARBON

class wxMacCarbonPrinterDC : public wxNativePrinterDC
{
public :
    wxMacCarbonPrinterDC( wxPrintData* data ) ;
    virtual ~wxMacCarbonPrinterDC() ;
    virtual bool StartDoc(  wxPrinterDC* dc , const wxString& message ) ;
    virtual void EndDoc( wxPrinterDC* dc ) ;
    virtual void StartPage( wxPrinterDC* dc ) ;
    virtual void EndPage( wxPrinterDC* dc ) ;
    virtual wxCoord GetMaxX() const { return m_maxX ; }
    virtual wxCoord GetMaxY() const { return m_maxY ; }
    virtual wxUint32 GetStatus() const { return m_err ; }
private :
    GrafPtr m_macPrintFormerPort ;
    wxCoord m_maxX ;
    wxCoord m_maxY ;
    OSStatus m_err ;
} ;

wxMacCarbonPrinterDC::wxMacCarbonPrinterDC( wxPrintData* data )
{
    ::GetPort( & m_macPrintFormerPort ) ;

    m_err = noErr ;
    wxMacCarbonPrintData *native = (wxMacCarbonPrintData*) data->m_nativePrintData ;

    PMRect rPage;
    m_err = PMGetAdjustedPageRect(native->m_macPageFormat, &rPage);
    if ( m_err != noErr )
        return;

    m_maxX = wxCoord(rPage.right - rPage.left) ;
    m_maxY = wxCoord(rPage.bottom - rPage.top);
}

wxMacCarbonPrinterDC::~wxMacCarbonPrinterDC()
{
    // nothing to release from print data, as wxPrinterDC has all data in its wxPrintData member
    ::SetPort( m_macPrintFormerPort ) ;
}

wxNativePrinterDC* wxNativePrinterDC::Create(wxPrintData* data)
{
    return new wxMacCarbonPrinterDC(data) ;
}

bool wxMacCarbonPrinterDC::StartDoc(  wxPrinterDC* dc , const wxString& WXUNUSED(message)  )
{
    if ( m_err )
        return false ;

    wxMacCarbonPrintData *native = (wxMacCarbonPrintData*) dc->GetPrintData().m_nativePrintData ;

    m_err = PMSessionBeginDocument(native->m_macPrintSession,
              native->m_macPrintSettings,
              native->m_macPageFormat);
    if ( m_err != noErr )
        return false;

    PMRect rPage;
    m_err = PMGetAdjustedPageRect(native->m_macPageFormat, &rPage);
    if ( m_err != noErr )
        return false;

    m_maxX = (wxCoord)(rPage.right - rPage.left);
    m_maxY = (wxCoord)(rPage.bottom - rPage.top);
    return true ;
}

void wxMacCarbonPrinterDC::EndDoc( wxPrinterDC* dc )
{
    if ( m_err )
        return ;

    wxMacCarbonPrintData *native = (wxMacCarbonPrintData*) dc->GetPrintData().m_nativePrintData ;

    m_err = PMSessionEndDocument(native->m_macPrintSession);
}

void wxMacCarbonPrinterDC::StartPage( wxPrinterDC* dc )
{
    if ( m_err )
        return ;

    wxMacCarbonPrintData *native = (wxMacCarbonPrintData*) dc->GetPrintData().m_nativePrintData ;

    m_err = PMSessionBeginPage(native->m_macPrintSession,
                 native->m_macPageFormat,
                 nil);

    if ( m_err == noErr )
    {
        m_err = PMSessionGetGraphicsContext(native->m_macPrintSession,
                                            nil,
                                            (void**) &dc->m_macPort );
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
            dc->m_macLocalOrigin.x = (int) rPage.left;
            dc->m_macLocalOrigin.y = (int) rPage.top;
        }
        // since this is a non-critical error, we set the flag back
        m_err = noErr ;
    }
}

void wxMacCarbonPrinterDC::EndPage( wxPrinterDC* dc )
{
    if ( m_err )
        return ;

    wxMacCarbonPrintData *native = (wxMacCarbonPrintData*) dc->GetPrintData().m_nativePrintData ;

    m_err = PMSessionEndPage(native->m_macPrintSession);
    if ( m_err != noErr )
    {
        PMSessionEndDocument(native->m_macPrintSession);
    }
}

#else

class wxMacClassicPrinterDC : public wxNativePrinterDC
{
public :
    wxMacClassicPrinterDC( wxPrintData* data ) ;
    virtual ~wxMacClassicPrinterDC() ;
    virtual bool StartDoc(  wxPrinterDC* dc , const wxString& message ) ;
    virtual void EndDoc( wxPrinterDC* dc ) ;
    virtual void StartPage( wxPrinterDC* dc ) ;
    virtual void EndPage( wxPrinterDC* dc ) ;
    virtual wxCoord GetMaxX() const { return m_maxX ; }
    virtual wxCoord GetMaxY() const { return m_maxY ; }
    virtual wxUint32 GetStatus() const { return m_err ; }
private :
    GrafPtr m_macPrintFormerPort ;
    TPPrPort m_macPrintingPort ;
    OSErr m_err ;
    long m_maxX ;
    long m_maxY ;
} ;

wxNativePrinterDC* wxNativePrinterDC::Create(wxPrintData* data)
{
    return new wxMacClassicPrinterDC(data) ;
}

wxMacClassicPrinterDC::wxMacClassicPrinterDC(wxPrintData* data)
{
    ::GetPort( &m_macPrintFormerPort ) ;
    m_err = noErr ;
    ::UMAPrOpen() ;
    m_err = PrError() ;
    if ( m_err != noErr )
        return;

    wxMacClassicPrintData *native = (wxMacClassicPrintData*) data->m_nativePrintData ;

    if ( ::PrValidate( native->m_macPrintSettings ) )
    {
        // the driver has changed in the mean time, should we pop up a page setup dialog ?
        if ( !::PrStlDialog( native->m_macPrintSettings ) )
        {
            m_err = -1 ;
            return;
        }
    }
    m_err = PrError() ;

    if ( m_err == noErr )
    {
        m_maxX = (**native->m_macPrintSettings).prInfo.rPage.right - (**native->m_macPrintSettings).prInfo.rPage.left ;
        m_maxY = (**native->m_macPrintSettings).prInfo.rPage.bottom - (**native->m_macPrintSettings).prInfo.rPage.top ;
    }
}

wxMacClassicPrinterDC::~wxMacClassicPrinterDC()
{
    ::UMAPrClose() ;
    ::SetPort( LMGetWMgrPort() ) ;
}

bool wxMacClassicPrinterDC::StartDoc(  wxPrinterDC* dc , const wxString& WXUNUSED(message)  )
{
    if ( m_err )
        return false ;

    wxMacClassicPrintData *native = (wxMacClassicPrintData*) dc->GetPrintData().m_nativePrintData ;
    m_macPrintingPort = ::PrOpenDoc( native->m_macPrintSettings , NULL , NULL ) ;
    m_err = PrError() ;
    if ( m_err )
        return false ;

    // sets current port
    dc->m_macPort = (GrafPtr ) m_macPrintingPort ;
    m_maxX = (**native->m_macPrintSettings).prInfo.rPage.right - (**native->m_macPrintSettings).prInfo.rPage.left ;
    m_maxY = (**native->m_macPrintSettings).prInfo.rPage.bottom - (**native->m_macPrintSettings).prInfo.rPage.top ;
    return true ;
}

void wxMacClassicPrinterDC::EndDoc( wxPrinterDC* dc )
{
    if ( m_err )
        return ;

    PrCloseDoc( m_macPrintingPort ) ;
    m_err = PrError() ;
}

void wxMacClassicPrinterDC::StartPage( wxPrinterDC* dc )
{
    if ( m_err )
        return ;

    wxMacClassicPrintData *native = (wxMacClassicPrintData*) dc->GetPrintData().m_nativePrintData ;

    PrOpenPage( m_macPrintingPort , NULL ) ;
    dc->m_macLocalOrigin.x =  (**native->m_macPrintSettings).rPaper.left ;
    dc->m_macLocalOrigin.y =  (**native->m_macPrintSettings).rPaper.top ;
    // m_macPrintingPort is now the current port
    Rect clip = { -32000 , -32000 , 32000 , 32000 } ;
    ::ClipRect( &clip ) ;
    m_err = PrError() ;
    if ( m_err != noErr )
        ::PrCloseDoc( m_macPrintingPort ) ;
}

void wxMacClassicPrinterDC::EndPage( wxPrinterDC* dc )
{
    if ( m_err )
        return ;

    PrClosePage( m_macPrintingPort ) ;
    m_err = PrError() ;
    if ( m_err != noErr )
        ::PrCloseDoc( m_macPrintingPort  ) ;
}

#endif

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
    }
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

    m_macFontInstalled = false ;
    m_macBrushInstalled = false ;
    m_macPenInstalled = false ;

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

    if ( width )
        * width = m_nativePrinterDC->GetMaxX() ;
    if ( height )
        * height = m_nativePrinterDC->GetMaxY() ;
}
