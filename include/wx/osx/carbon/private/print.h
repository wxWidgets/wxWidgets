/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/carbon/private/print.h
// Purpose:     private implementation for printing on OS X
// Author:      Stefan Csomor
// Modified by:
// Created:     03/02/99
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MAC_PRIVATE_PRINT_H_
#define _WX_MAC_PRIVATE_PRINT_H_

#include "wx/cmndata.h"
#include "wx/print.h"

// for PrintingManager
#include "ApplicationServices/ApplicationServices.h"

class WXDLLIMPEXP_CORE wxOSXPrintData : public wxPrintNativeDataBase
{
public:
                            wxOSXPrintData();
    virtual                 ~wxOSXPrintData();

    virtual bool            TransferTo( wxPrintData &data ) wxOVERRIDE;
    virtual bool            TransferFrom( const wxPrintData &data ) wxOVERRIDE;

    virtual bool            IsOk() const wxOVERRIDE;

    virtual void            TransferFrom( const wxPageSetupDialogData * ) wxOVERRIDE;
    virtual void            TransferTo( wxPageSetupDialogData * ) wxOVERRIDE;

    virtual void            TransferFrom( const wxPrintDialogData * );
    virtual void            TransferTo( wxPrintDialogData * );

    PMPrintSession          GetPrintSession() { return m_macPrintSession; }
    PMPageFormat            GetPageFormat() { return m_macPageFormat; }
    PMPrintSettings         GetPrintSettings() { return m_macPrintSettings; }
protected :
    virtual void            TransferPrinterNameFrom( const wxPrintData &data );
    virtual void            TransferPaperInfoFrom( const wxPrintData &data );
    virtual void            TransferResolutionFrom( const wxPrintData &data );

    virtual void            TransferPrinterNameTo( wxPrintData &data );
    virtual void            TransferPaperInfoTo( wxPrintData &data );
    virtual void            TransferResolutionTo( wxPrintData &data );


    virtual void            UpdateFromPMState();
    virtual void            UpdateToPMState();

    PMPrintSession          m_macPrintSession ;
    PMPageFormat            m_macPageFormat ;
    PMPrintSettings         m_macPrintSettings ;
    PMPaper                 m_macPaper;
private:
    wxDECLARE_DYNAMIC_CLASS(wxOSXPrintData);
} ;

WXDLLIMPEXP_CORE wxPrintNativeDataBase* wxOSXCreatePrintData();

#if wxOSX_USE_COCOA
class WXDLLIMPEXP_CORE wxOSXCocoaPrintData : public wxOSXPrintData
{
public:
                            wxOSXCocoaPrintData();
    virtual                 ~wxOSXCocoaPrintData();

    WX_NSPrintInfo          GetNSPrintInfo() { return m_macPrintInfo; }
protected:
    virtual void            UpdateFromPMState() wxOVERRIDE;
    virtual void            UpdateToPMState() wxOVERRIDE;

    WX_NSPrintInfo          m_macPrintInfo;
private:
    wxDECLARE_DYNAMIC_CLASS(wxOSXCocoaPrintData);
} ;
#endif

#endif // _WX_MAC_PRIVATE_PRINT_H_
