/////////////////////////////////////////////////////////////////////////////
// Name:        cmndlgs.i
// Purpose:     SWIG definitions for the Common Dialog Classes
//
// Author:      Robin Dunn
//
// Created:     7/25/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%module cmndlgs

%{
#include "helpers.h"
#include <wx/colordlg.h>
#include <wx/dirdlg.h>
#include <wx/fontdlg.h>
#include <wx/printdlg.h>
%}

//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

// Import some definitions of other classes, etc.
%import _defs.i
%import misc.i
%import gdi.i
%import windows.i


//----------------------------------------------------------------------

class wxColourData {
public:
    wxColourData();
    ~wxColourData();

    bool GetChooseFull();
    wxColour& GetColour();
    wxColour& GetCustomColour(int i);
    void SetChooseFull(int flag);
    void SetColour(const wxColour& colour);
    void SetCustomColour(int i, const wxColour& colour);
};


class wxColourDialog : public wxDialog {
public:
    wxColourDialog(wxWindow* parent, wxColourData* data = NULL);

    wxColourData& GetColourData();
    int ShowModal();
};


//----------------------------------------------------------------------

class wxDirDialog : public wxDialog {
public:
    wxDirDialog(wxWindow* parent,
                char* message = "Choose a directory",
                char* defaultPath = "",
                long style = 0,
                const wxPoint& pos = wxPyDefaultPosition);

    wxString GetPath();
    wxString GetMessage();
    long GetStyle();
    void SetMessage(const wxString& message);
    void SetPath(const wxString& path);
    int ShowModal();
};

//----------------------------------------------------------------------

class wxFileDialog : public wxDialog {
public:
    wxFileDialog(wxWindow* parent,
                 char* message = "Choose a file",
                 char* defaultDir = "",
                 char* defaultFile = "",
                 char* wildcard = "*.*",
                 long style = 0,
                 const wxPoint& pos = wxPyDefaultPosition);

    wxString GetDirectory();
    wxString GetFilename();
    int GetFilterIndex();
    wxString GetMessage();
    wxString GetPath();
    long GetStyle();
    wxString GetWildcard();
    void SetDirectory(const wxString& directory);
    void SetFilename(const wxString& setfilename);
    void SetFilterIndex(int filterIndex);
    void SetMessage(const wxString& message);
    void SetPath(const wxString& path);
    void SetStyle(long style);
    void SetWildcard(const wxString& wildCard);
    int ShowModal();
};


//----------------------------------------------------------------------

//TODO: wxMultipleChoiceDialog

//----------------------------------------------------------------------

class wxSingleChoiceDialog : public wxDialog {
public:
    %addmethods {
        // TODO: ignoring clientData for now...
        //       SWIG is messing up the &/*'s for some reason.
        wxSingleChoiceDialog(wxWindow* parent,
                             wxString* message,
                             wxString* caption,
                             int LCOUNT, wxString* LIST,
                             //char** clientData = NULL,
                             long style = wxOK | wxCANCEL | wxCENTRE,
                             wxPoint* pos = &wxPyDefaultPosition) {
            return new wxSingleChoiceDialog(parent, *message, *caption,
                                            LCOUNT, LIST, NULL, style, *pos);
        }
    }

    int GetSelection();
    wxString GetStringSelection();
    void SetSelection(int sel);
    int ShowModal();
};


//----------------------------------------------------------------------

class wxTextEntryDialog : public wxDialog {
public:
    wxTextEntryDialog(wxWindow* parent,
                      char* message,
                      char* caption = "Input Text",
                      char* defaultValue = "",
                      long style = wxOK | wxCANCEL | wxCENTRE,
                      const wxPoint& pos = wxPyDefaultPosition);

    wxString GetValue();
    void SetValue(const wxString& value);
    int ShowModal();
};

//----------------------------------------------------------------------

class wxFontData {
public:
    wxFontData();
    ~wxFontData();

    void EnableEffects(bool enable);
    bool GetAllowSymbols();
    wxColour& GetColour();
    wxFont GetChosenFont();
    bool GetEnableEffects();
    wxFont GetInitialFont();
    bool GetShowHelp();
    void SetAllowSymbols(bool allowSymbols);
    void SetChosenFont(const wxFont& font);
    void SetColour(const wxColour& colour);
    void SetInitialFont(const wxFont& font);
    void SetRange(int min, int max);
    void SetShowHelp(bool showHelp);
};


class wxFontDialog : public wxDialog {
public:
    wxFontDialog(wxWindow* parent, wxFontData* data = NULL);

    wxFontData& GetFontData();
    int ShowModal();
};


//----------------------------------------------------------------------

class wxPageSetupData {
public:
    wxPageSetupData();
    ~wxPageSetupData();

    void EnableHelp(bool flag);
    void EnableMargins(bool flag);
    void EnableOrientation(bool flag);
    void EnablePaper(bool flag);
    void EnablePrinter(bool flag);
    wxPoint GetPaperSize();
    wxPoint GetMarginTopLeft();
    wxPoint GetMarginBottomRight();
    wxPoint GetMinMarginTopLeft();
    wxPoint GetMinMarginBottomRight();
    int GetOrientation();
    bool GetDefaultMinMargins();
    bool GetEnableMargins();
    bool GetEnableOrientation();
    bool GetEnablePaper();
    bool GetEnablePrinter();
    bool GetEnableHelp();
    bool GetDefaultInfo();
    void SetPaperSize(const wxPoint& size);
    void SetMarginTopLeft(const wxPoint& pt);
    void SetMarginBottomRight(const wxPoint& pt);
    void SetMinMarginTopLeft(const wxPoint& pt);
    void SetMinMarginBottomRight(const wxPoint& pt);
    void SetOrientation(int orientation);
    void SetDefaultMinMargins(bool flag);
    void SetDefaultInfo(bool flag);
};


class wxPageSetupDialog : public wxDialog {
public:
    wxPageSetupDialog(wxWindow* parent, wxPageSetupData* data = NULL);

    wxPageSetupData& GetPageSetupData();
    int ShowModal();
};

//----------------------------------------------------------------------

class wxPrintData {
public:
    wxPrintData();
    ~wxPrintData();

    void EnableHelp(bool flag);
    void EnablePageNumbers(bool flag);
    void EnablePrintToFile(bool flag);
    void EnableSelection(bool flag);
    bool GetAllPages();
    bool GetCollate();
    int GetFromPage();
    int GetMaxPage();
    int GetMinPage();
    int GetNoCopies();
    int GetOrientation();
    int GetToPage();
    void SetCollate(bool flag);
    void SetFromPage(int page);
    void SetMaxPage(int page);
    void SetMinPage(int page);
    void SetOrientation(int orientation);
    void SetNoCopies(int n);
    void SetPrintToFile(bool flag);
    void SetSetupDialog(bool flag);
    void SetToPage(int page);
};


class wxPrintDialog : public wxDialog {
public:
    wxPrintDialog(wxWindow* parent, wxPrintData* data = NULL);

    wxPrintData& GetPrintData();
    wxDC* GetPrintDC();
    int ShowModal();
};

//----------------------------------------------------------------------

class wxMessageDialog : public wxDialog {
public:
    wxMessageDialog(wxWindow* parent,
                    char* message,
                    char* caption = "Message box",
                    long style = wxOK | wxCANCEL | wxCENTRE,
                    const wxPoint& pos = wxPyDefaultPosition);

    int ShowModal();
};

//----------------------------------------------------------------------


/////////////////////////////////////////////////////////////////////////////
//
// $Log$
// Revision 1.2  1998/08/15 07:36:25  RD
// - Moved the header in the .i files out of the code that gets put into
// the .cpp files.  It caused CVS conflicts because of the RCS ID being
// different each time.
//
// - A few minor fixes.
//
// Revision 1.1  1998/08/09 08:25:49  RD
// Initial version
//
//

