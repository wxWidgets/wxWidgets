/////////////////////////////////////////////////////////////////////////////
// Name:        prntdlgg.h
// Purpose:     wxGenericPrintDialog, wxGenericPrintSetupDialog,
//              wxGenericPageSetupDialog
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __PRINTDLGH_G_
#define __PRINTDLGH_G_

#ifdef __GNUG__
#pragma interface "prntdlgg.h"
#endif

#include "wx/defs.h"
#include "wx/dialog.h"
#include "wx/dc.h"
#include "wx/cmndata.h"
#include "wx/dialog.h"

#if wxUSE_POSTSCRIPT
#include "wx/dcps.h"
#endif

class WXDLLEXPORT wxTextCtrl;
class WXDLLEXPORT wxButton;
class WXDLLEXPORT wxCheckBox;
class WXDLLEXPORT wxChoice;
class WXDLLEXPORT wxStaticText;
class WXDLLEXPORT wxRadioBox;
class WXDLLEXPORT wxPrintSetupData;

/*
* Simulated Print and Print Setup dialogs
* for non-Windows platforms (and Windows using PostScript print/preview)
*/

#define wxPRINTID_STATIC        10
#define wxPRINTID_RANGE         11
#define wxPRINTID_FROM          12
#define wxPRINTID_TO            13
#define wxPRINTID_COPIES        14
#define wxPRINTID_PRINTTOFILE   15
#define wxPRINTID_SETUP         16

class WXDLLEXPORT wxGenericPrintDialog: public wxDialog
{
    DECLARE_DYNAMIC_CLASS(wxGenericPrintDialog)
        
public:
    wxGenericPrintDialog(wxWindow *parent, wxPrintDialogData* data = (wxPrintDialogData*) NULL);
    ~wxGenericPrintDialog();

    void OnSetup(wxCommandEvent& event);
    void OnRange(wxCommandEvent& event);
    void OnOK(wxCommandEvent& event);
    
    virtual bool TransferDataFromWindow();
    virtual bool TransferDataToWindow();

    virtual int ShowModal();

    inline wxPrintDialogData& GetPrintDialogData() { return m_printDialogData; }
    wxDC *GetPrintDC();

public:
    wxStaticText*       m_printerMessage;
    wxButton*           m_setupButton;
    wxButton*           m_helpButton;
    wxRadioBox*         m_rangeRadioBox;
    wxTextCtrl*         m_fromText;
    wxTextCtrl*         m_toText;
    wxTextCtrl*         m_noCopiesText;
    wxCheckBox*         m_printToFileCheckBox;
    wxCheckBox*         m_collateCopiesCheckBox;
    
    wxPrintDialogData   m_printDialogData;
    
    
    DECLARE_EVENT_TABLE()
};

#define wxPRINTID_PRINTCOLOUR       10
#define wxPRINTID_ORIENTATION       11
#define wxPRINTID_COMMAND           12
#define wxPRINTID_OPTIONS           13
#define wxPRINTID_PAPERSIZE         14

class WXDLLEXPORT wxGenericPrintSetupDialog: public wxDialog
{
    DECLARE_CLASS(wxGenericPrintSetupDialog)
        
public:
    // There are no configuration options for the dialog, so we
    // just pass the wxPrintData object (no wxPrintSetupDialogData class needed)
    wxGenericPrintSetupDialog(wxWindow *parent, wxPrintData* data);
    wxGenericPrintSetupDialog(wxWindow *parent, wxPrintSetupData* data);
    ~wxGenericPrintSetupDialog();

    void Init(wxPrintData* data);

    virtual bool TransferDataFromWindow();
    virtual bool TransferDataToWindow();

    wxChoice *CreatePaperTypeChoice(int* x, int* y);
    
public:
    wxRadioBox*         m_orientationRadioBox;
    wxTextCtrl*         m_printerCommandText;
    wxTextCtrl*         m_printerOptionsText;
    wxCheckBox*         m_colourCheckBox;
    wxChoice*           m_paperTypeChoice;
    
#if wxUSE_POSTSCRIPT
    wxPrintData         m_printData;
    inline wxPrintData& GetPrintData() { return m_printData; }
#endif
    
};

#define wxPRINTID_LEFTMARGIN         30
#define wxPRINTID_RIGHTMARGIN        31
#define wxPRINTID_TOPMARGIN          32
#define wxPRINTID_BOTTOMMARGIN       33

class WXDLLEXPORT wxGenericPageSetupDialog: public wxDialog
{
    DECLARE_CLASS(wxGenericPageSetupDialog)
        
public:
    wxGenericPageSetupDialog(wxWindow *parent, wxPageSetupData* data = (wxPageSetupData*) NULL);
    ~wxGenericPageSetupDialog();

    virtual bool TransferDataFromWindow();
    virtual bool TransferDataToWindow();

    void OnPrinter(wxCommandEvent& event);
    
    wxChoice *CreatePaperTypeChoice(int* x, int* y);
    inline wxPageSetupData& GetPageSetupData() { return m_pageData; }

public:
    wxButton*       m_printerButton;
    wxRadioBox*     m_orientationRadioBox;
    wxTextCtrl*     m_marginLeftText;
    wxTextCtrl*     m_marginTopText;
    wxTextCtrl*     m_marginRightText;
    wxTextCtrl*     m_marginBottomText;
    wxChoice*       m_paperTypeChoice;
    
    static bool     m_pageSetupDialogCancelled;
    
    wxPageSetupData m_pageData;
    
    DECLARE_EVENT_TABLE()
};

#endif
// __PRINTDLGH_G__
