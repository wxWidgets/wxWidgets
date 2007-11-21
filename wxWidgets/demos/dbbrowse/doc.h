//--------------------------------------------------------------------------------------------------
// Name:        doc.h
// Purpose:     a non-MFC Document (a do-it-yourself document)
// Author:      Mark Johnson
// Modified by: 19990808.mj10777
// Created:     19990808
// Copyright:   (c) Mark Johnson
// Licence:     wxWindows license
// RCS-ID:      $Id$

//----------------------------------------------------------------------------------------
//-- Some Global Vars --------------------------------------------------------------------
//----------------------------------------------------------------------------------------
class DSN
{
public:
    wxString  Dsn;
    wxString  Drv;
    wxString  Pas;
    wxString  Usr;
};

//----------------------------------------------------------------------------------------
// Global structure for holding ODBC connection information
//extern wxDbConnectInf DbConnectInf;

//----------------------------------------------------------------------------------------
extern wxConfigBase   *p_ProgramCfg;       // All Config and Path information
extern wxLogTextCtrl  *p_LogBook;          // All Log messages
extern wxString        LogBuf;             // String for all Logs

//----------------------------------------------------------------------------------------
class PgmCtrl;             // Declared in PgmCtrl.h file
class DocSplitterWindow;   // Declared at the end of the file

//----------------------------------------------------------------------------------------
class MainDoc
{
public:
    //---------------------------------------------------------------------------------------
    MainDoc();
    virtual ~MainDoc();
    int Sash;
    //---------------------------------------------------------------------------------------
    //-- declare document Vars here ---------------------------------------------------------
    //---------------------------------------------------------------------------------------
    wxString Temp0, Temp1, Temp2, Temp3, Temp4, Temp5;
    BrowserDB *db_Br;  // Pointer to wxDB
    DSN       *p_DSN;
    int        i_DSN;  // Counter
    wxString   s_BColour;
    wxFont    *ft_Doc;
    //---------------------------------------------------------------------------------------
    DocSplitterWindow    *p_Splitter;
    wxHtmlHelpController *p_Help;
    wxFrame              *p_MainFrame;     // SDI Version
    PgmCtrl              *p_PgmCtrl;
    DBTree               *p_DBTree;
    DBGrid               *p_DBGrid;
    wxTextCtrl           *p_LogWin;
    wxTabbedWindow       *p_TabArea;
    wxPagedWindow        *p_PageArea;
    int                   i_TabNr;         // Amount of active Views in Tab
    int                   i_PageNr;        // Amount of active Views in Page
    //---------------------------------------------------------------------------------------
    //-- declare document Functions here ----------------------------------------------------
    //---------------------------------------------------------------------------------------
    bool OnNewDocument();
    bool OnInitView();
    bool OnInitODBC();
    bool OnChosenDSN(int Which);
    bool OnChosenTbl(int Tab,wxString Table);
    //---------------------------------------------------------------------------------------
    void OnLeer(wxString Aufrufer);  // Dummy Funktion
    //---------------------------------------------------------------------------------------
};

//----------------------------------------------------------------------------------------
class DocSplitterWindow: public wxSplitterWindow
{
public:
    MainDoc *pDoc;
    DocSplitterWindow(wxWindow *parent, wxWindowID id);
    virtual bool OnSashPositionChange(int newSashPosition)
    {
        if ( !wxSplitterWindow::OnSashPositionChange(newSashPosition) )
            return false;
        pDoc->Sash = newSashPosition;
        return true;
    }
    DECLARE_EVENT_TABLE()
};
//----------------------------------------------------------------------------------------
