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
#include "wx/postscrp.h"

class WXDLLEXPORT wxTextCtrl;
class WXDLLEXPORT wxButton;
class WXDLLEXPORT wxCheckBox;
class WXDLLEXPORT wxChoice;
class WXDLLEXPORT wxStaticText;
class WXDLLEXPORT wxRadioBox;

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
  wxStaticText  *printerMessage;
  wxButton   *setupButton;
  wxButton   *helpButton;
  wxRadioBox *rangeRadioBox;
  wxTextCtrl     *fromText;
  wxTextCtrl     *toText;
  wxTextCtrl     *noCopiesText;
  wxCheckBox *printToFileCheckBox;
  wxCheckBox *collateCopiesCheckBox;

  wxPrintData printData;
  wxGenericPrintDialog(wxWindow *parent, wxPrintData* data);
  ~wxGenericPrintDialog(void);

  void OnSetup(wxCommandEvent& event);
  void OnRange(wxCommandEvent& event);
  void OnOK(wxCommandEvent& event);

  virtual bool TransferDataFromWindow(void);
  virtual bool TransferDataToWindow(void);

  virtual int ShowModal(void);

  inline wxPrintData& GetPrintData(void) { return printData; }
  wxDC *GetPrintDC(void);

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
  wxRadioBox *orientationRadioBox;
  wxTextCtrl     *printerCommandText;
  wxTextCtrl     *printerOptionsText;
  wxCheckBox *colourCheckBox;
  wxChoice   *paperTypeChoice;

  wxPrintSetupData printData;
  wxGenericPrintSetupDialog(wxWindow *parent, wxPrintSetupData* data);
  ~wxGenericPrintSetupDialog(void);

  virtual bool TransferDataFromWindow(void);
  virtual bool TransferDataToWindow(void);

  wxChoice *CreatePaperTypeChoice(int* x, int* y);
  inline wxPrintSetupData& GetPrintData(void) { return printData; }
};

#define wxPRINTID_LEFTMARGIN         30
#define wxPRINTID_RIGHTMARGIN        31
#define wxPRINTID_TOPMARGIN          32
#define wxPRINTID_BOTTOMMARGIN       33

class WXDLLEXPORT wxGenericPageSetupDialog: public wxDialog
{
  DECLARE_CLASS(wxGenericPageSetupDialog)

 public:
  wxButton   *printerButton;
  wxRadioBox *orientationRadioBox;
  wxTextCtrl     *marginLeftText;
  wxTextCtrl     *marginTopText;
  wxTextCtrl     *marginRightText;
  wxTextCtrl     *marginBottomText;
  wxChoice   *paperTypeChoice;

  static bool pageSetupDialogCancelled;
  
  wxPageSetupData pageData;

  wxGenericPageSetupDialog(wxWindow *parent, wxPageSetupData* data = NULL);
  ~wxGenericPageSetupDialog(void);

  virtual bool TransferDataFromWindow(void);
  virtual bool TransferDataToWindow(void);

  void OnPrinter(wxCommandEvent& event);

  wxChoice *CreatePaperTypeChoice(int* x, int* y);
  inline wxPageSetupData& GetPageSetupData(void) { return pageData; }

DECLARE_EVENT_TABLE()
};

#endif
    // __PRINTDLGH_G__
