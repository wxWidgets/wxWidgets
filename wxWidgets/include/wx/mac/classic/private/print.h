/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/classic/private/print.h
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
#include "wx/mac/private.h"

#if TARGET_CARBON && !defined(__DARWIN__)
#  include <PMApplication.h>
#endif

#ifndef __DARWIN__
#  include "Printing.h"
#endif

#if TARGET_CARBON
#if !PM_USE_SESSION_APIS
#error "only Carbon Printing Session API is supported"
#endif
#endif

class wxNativePrintData
{
public :
                            wxNativePrintData() {}
    virtual                 ~wxNativePrintData() {}

    virtual void            TransferFrom( wxPrintData * ) = 0 ;
    virtual void            TransferTo( wxPrintData * ) = 0 ;

    virtual void            TransferFrom( wxPageSetupDialogData * ) = 0 ;
    virtual void            TransferTo( wxPageSetupDialogData * ) = 0 ;

    virtual void            TransferFrom( wxPrintDialogData * ) = 0 ;
    virtual void            TransferTo( wxPrintDialogData * ) = 0 ;

    virtual void            CopyFrom( wxNativePrintData * ) = 0;

    virtual int             ShowPrintDialog() = 0 ;
    virtual int             ShowPageSetupDialog() = 0 ;

    static wxNativePrintData* Create() ;
} ;

#if TARGET_CARBON

class wxMacCarbonPrintData : public wxNativePrintData
{
public :
                            wxMacCarbonPrintData() ;
                            ~wxMacCarbonPrintData() ;

    virtual void            TransferFrom( wxPrintData * ) ;
    virtual void            TransferTo( wxPrintData * )  ;

    virtual void            TransferFrom( wxPageSetupDialogData * )  ;
    virtual void            TransferTo( wxPageSetupDialogData * ) ;

    virtual void            TransferFrom( wxPrintDialogData * )  ;
    virtual void            TransferTo( wxPrintDialogData * ) ;

    virtual void            CopyFrom( wxNativePrintData * ) ;
    virtual int             ShowPrintDialog() ;
    virtual int             ShowPageSetupDialog() ;
private :
    virtual void            ValidateOrCreate() ;
public :
    PMPrintSession          m_macPrintSession ;
    PMPageFormat            m_macPageFormat ;
    PMPrintSettings         m_macPrintSettings ;
} ;

#else

class wxMacClassicPrintData : public wxNativePrintData
{
public :
                            wxMacClassicPrintData() ;
                            ~wxMacClassicPrintData() ;
    virtual void            TransferFrom( wxPrintData * ) ;
    virtual void            TransferTo( wxPrintData * )  ;
    virtual void            TransferFrom( wxPageSetupDialogData * ) ;
    virtual void            TransferTo( wxPageSetupDialogData * ) ;
    virtual void            TransferFrom( wxPrintDialogData * )  ;
    virtual void            TransferTo( wxPrintDialogData * ) ;
    virtual void            CopyFrom( wxNativePrintData * ) ;
    virtual int             ShowPrintDialog() ;
    virtual int             ShowPageSetupDialog() ;
private :
    virtual void            ValidateOrCreate() ;
public :
    THPrint                 m_macPrintSettings ;
} ;

#endif

#endif
    // _WX_MAC_PRIVATE_PRINT_H_
