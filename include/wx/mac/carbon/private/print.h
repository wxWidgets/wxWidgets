/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/carbon/private/print.h
// Purpose:     private implementation for printing on MacOS
// Author:      Stefan Csomor
// Modified by:
// Created:     03/02/99
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MAC_PRIVATE_PRINT_H_
#define _WX_MAC_PRIVATE_PRINT_H_

#include "wx/cmndata.h"
#include "wx/print.h"
#include "wx/mac/private.h"

class WXDLLEXPORT wxMacCarbonPrintData : public wxPrintNativeDataBase
{
public :
                            wxMacCarbonPrintData() ;
    virtual                 ~wxMacCarbonPrintData() ;

    virtual bool            TransferTo( wxPrintData &data );
    virtual bool            TransferFrom( const wxPrintData &data );

    virtual bool            IsOk() const ;

    virtual void            TransferFrom( wxPageSetupDialogData * )  ;
    virtual void            TransferTo( wxPageSetupDialogData * ) ;

    virtual void            TransferFrom( wxPrintDialogData * )  ;
    virtual void            TransferTo( wxPrintDialogData * ) ;
private :
    virtual void            ValidateOrCreate() ;
public :
    PMPrintSession          m_macPrintSession ;
    PMPageFormat            m_macPageFormat ;
    PMPrintSettings         m_macPrintSettings ;
private:
    DECLARE_DYNAMIC_CLASS(wxMacCarbonPrintData)
} ;

#endif // _WX_MAC_PRIVATE_PRINT_H_
