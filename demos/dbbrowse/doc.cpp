//----------------------------------------------------------------------------------------
// Name:        doc.cpp
// Purpose:     Holds information for DBBrowser - (a do-it-yourself document)
// Author:      Mark Johnson
// Modified by: 19990808.mj10777
// BJO        : Bart A.M. JOURQUIN
// Created:     19990808
// Copyright:   (c) Mark Johnson
// Licence:     wxWindows license
// RCS-ID:      $Id$
//----------------------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
//----------------------------------------------------------------------------------------
#ifdef __BORLANDC__
#pragma hdrstop
#endif
//----------------------------------------------------------------------------------------
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
//----------------------------------------------------------------------------------------
//-- all #includes that every .cpp needs             ----19990807.mj10777 ----------------
//----------------------------------------------------------------------------------------
#include "std.h"       // sorgsam Pflegen !
//----------------------------------------------------------------------------------------
//-- Some Global Vars for all Files (extern in ?.h needed) -------------------------------
// Global structure for holding ODBC connection information
extern wxDbConnectInf DbConnectInf;

//----------------------------------------------------------------------------------------
wxConfigBase   *p_ProgramCfg;       // All Config and Path information
wxLogTextCtrl  *p_LogBook;          // All Log messages
wxString        LogBuf;             // String for all Logs

//----------------------------------------------------------------------------------------
MainDoc::MainDoc()
{
    db_Br          = NULL;
    p_DSN          = NULL;
    i_DSN          = 0;
    p_Splitter     = NULL;
    p_MainFrame    = NULL;
    p_PgmCtrl      = NULL;               // Is not active
    p_DBTree       = NULL;
    p_DBGrid       = NULL;
    p_LogWin       = NULL;
    p_TabArea      = NULL;
    p_PageArea     = NULL;
    i_TabNr        = 0;
    i_PageNr       = 0;
    s_BColour      = _T("WHEAT");
    ft_Doc         = new wxFont(wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT));
}

//----------------------------------------------------------------------------------------
MainDoc::~MainDoc()
{
    p_TabArea->Show(false);    // Deactivate the Window
    p_PageArea->Show(false);    // Deactivate the Window

    // ----------------------------------------------------------
    // -E-> The Tree Controls take to long to close : Why ??
    // ----------------------------------------------------------
    delete ft_Doc;     ft_Doc  = NULL;
    delete p_PgmCtrl;  p_PgmCtrl = NULL;
    delete [] p_DSN;
    delete p_DBTree;
    delete p_TabArea;  p_TabArea  = NULL;
    delete p_PageArea; p_PageArea = NULL;
    delete p_Splitter; p_Splitter = NULL;
    delete [] db_Br;   db_Br      = NULL;
    //  wxMessageBox("~MainDoc");
}

//----------------------------------------------------------------------------------------
bool MainDoc::OnNewDocument()
{
    wxStopWatch sw;
    //---------------------------------------------------------------------------------------
    if (!OnInitView())
        return false;
    p_PgmCtrl->OnPopulate();
    //---------------------------------------------------------------------------------------
    wxLogMessage(_("-I-> MainDoc::OnNewDocument() - End - Time needed : %ld ms"),sw.Time());
    return true;
}

//----------------------------------------------------------------------------------------
bool MainDoc::OnInitView()
{
    Sash = p_ProgramCfg->Read(_T("/MainFrame/Sash"), 200);
    // wxMessageBox("OnInitView() - Begin ","-I->MainDoc::OnInitView");
    //---------------------------------------------------------------------------------------
    // create "workplace" window
    //---------------------------------------------------------------------------------------
    p_TabArea = new wxTabbedWindow(); // Init the Pointer
    p_TabArea->Create(p_Splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE);
    //---------------------------------------------------------------------------------------
    p_PgmCtrl = new PgmCtrl(p_TabArea, TREE_CTRL_PGM,wxDefaultPosition, wxDefaultSize,
        wxTR_HAS_BUTTONS | wxSUNKEN_BORDER);
    p_PgmCtrl->i_TabArt = 0; // 0 = Tab ; 1 = Page
    p_PgmCtrl->i_ViewNr = p_TabArea->GetTabCount()-1;
    //---------------------------------------------------------------------------------------
    wxBitmap *p_FolderClose = new wxBitmap(_T("PgmCtrl")); //, wxBITMAP_TYPE_BMP_RESOURCE); // BJO20000115
    //---------------------------------------------------------------------------------------
    p_TabArea->AddTab(p_PgmCtrl,_T("PgmCtrl"),p_FolderClose);
    delete p_FolderClose;      // Memory leak
    p_FolderClose = NULL;
    wxUnusedVar(p_FolderClose);
    //---------------------------------------------------------------------------------------
    // now create "output" window
    //---------------------------------------------------------------------------------------
    p_PageArea = new wxPagedWindow();  // Init the Pointer
    p_PageArea->Create(p_Splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE);
    //---------------------------------------------------------------------------------------
    p_LogWin = new wxTextCtrl(p_PageArea,wxID_ANY,wxEmptyString,
        wxDefaultPosition, wxDefaultSize,wxTE_MULTILINE );
    p_LogWin->SetFont(* ft_Doc);
    // Don't forget ! This is always : i_TabArt = 0 ; i_ViewNr = 1;
    //---------------------------------------------------------------------------------------
    p_LogBook = new wxLogTextCtrl(p_LogWin); // make p_LogWin the LogBook
    p_LogBook->SetActiveTarget(p_LogBook);
    p_LogBook->SetTimestamp( NULL );
    //---------------------------------------------------------------------------------------
    p_PageArea->AddTab(p_LogWin,_("LogBook"), _T("what is this?") );
    i_TabNr  = p_TabArea->GetTabCount()-1;  // Add one when a new AddTab is done;
    i_PageNr = p_PageArea->GetTabCount()-1; // Add one when a new AddTab is done;
    //---------------------------------------------------------------------------------------
    p_PgmCtrl->pDoc = this;
    p_TabArea->SetActiveTab(i_PageNr);
    //---------------------------------------------------------------------------------------
    p_Splitter->Initialize(p_TabArea);
    p_Splitter->SplitHorizontally(p_TabArea,p_PageArea,Sash);
    //---------------------------------------------------------------------------------------
    // if (!OnInitODBC())
    //  return false;
    OnInitODBC();
    //---------------------------------------------------------------------------------------
#if wxUSE_STATUSBAR
    Temp0.Printf(_("-I-> MainDoc::OnInitView() - End - %d DSN's found"),i_DSN);
    p_MainFrame->SetStatusText(Temp0, 0);
    wxLogMessage(Temp0);
#endif // wxUSE_STATUSBAR
    return true;
}

//----------------------------------------------------------------------------------------
bool MainDoc::OnInitODBC()
{
    wxChar Dsn[SQL_MAX_DSN_LENGTH+1];
    wxChar DsDesc[254+1]; // BJO20002501 instead of 512
    Temp0 = wxEmptyString;
    i_DSN = 0;       // Counter
    int i;
    //---------------------------------------------------------------------------------------
    // Initialize the ODBC Environment for Database Operations

    if (!DbConnectInf.AllocHenv())
    {
        return false;
    }

    //---------------------------------------------------------------------------------------
    const wxChar sep = 3; // separator character used in string between DSN and DsDesc
    wxSortedArrayString s_SortDSNList, s_SortDsDescList;
    // BJO-20000127
    // In order to have same sort result on both Dsn and DsDesc, create a 'keyed' string.
    // The key will be removed after sorting
    wxString KeyString;
    //---------------------------------------------------------------------------------------
    while (wxDbGetDataSource(DbConnectInf.GetHenv(), Dsn, SQL_MAX_DSN_LENGTH, DsDesc, 254))
    {
        i_DSN++;   // How many Dsn have we ?
        KeyString.Printf(_T("%s%c%s"),Dsn, sep, DsDesc);
        s_SortDSNList.Add(Dsn);
        s_SortDsDescList.Add(KeyString);
    }

    //---------------------------------------------------------------------------------------
    // Allocate n ODBC-DSN objects to hold the information
    // Allocate n wxDatabase objects to hold the column information
    p_DSN = new DSN[i_DSN];  //BJO
    db_Br = new BrowserDB[i_DSN];
    for (i=0;i<i_DSN;i++)
    {
        KeyString = s_SortDsDescList[i];
        KeyString = KeyString.AfterFirst(sep);

        // ODBC-DSN object
        (p_DSN+i)->Dsn = s_SortDSNList[i];
        (p_DSN+i)->Drv = KeyString;
        (p_DSN+i)->Usr = wxEmptyString;
        (p_DSN+i)->Pas = wxEmptyString;
        Temp0.Printf(_T("%02d) Dsn(%s) DsDesc(%s)"),i,(p_DSN+i)->Dsn.c_str(),(p_DSN+i)->Drv.c_str());
        wxLogMessage(Temp0);

        // wxDataBase object
        (db_Br+i)->p_LogWindow = p_LogWin;
        (db_Br+i)->ODBCSource  = (p_DSN+i)->Dsn;
        (db_Br+i)->UserName    = (p_DSN+i)->Usr;
        (db_Br+i)->Password    = (p_DSN+i)->Pas;
        (db_Br+i)->pDoc        = this;
        (db_Br+i)->i_Which     = i;
    }

    DbConnectInf.FreeHenv();

    //---------------------------------------------------------------------------------------
    if (!i_DSN)
    {
        wxString message = _("No Dataset names found in ODBC!\n");
        message += _("           Program will exit!\n\n");
        message += _("                       Ciao");
        wxMessageBox( message,_("-E-> Fatal situation"));
        return false;
    }
    //---------------------------------------------------------------------------------------
    return true;
}

//----------------------------------------------------------------------------------------
bool MainDoc::OnChosenDSN(int Which)
{
    // wxLogMessage("OnChosenDSN(%d) - Begin",Which);
    //---------------------------------------------------------------------------------------
    if (p_DBTree != NULL)
    {
        p_TabArea->Show(false);    // Deactivate the Window
        p_TabArea->RemoveTab(p_DBTree->i_ViewNr);
        p_TabArea->Show(true);     // Activate the Window
        OnChosenTbl(77,wxEmptyString);
    }
    //-------------------------
    p_TabArea->Show(false);    // Deactivate the Window
    p_DBTree = new DBTree(p_TabArea, TREE_CTRL_DB,wxDefaultPosition, wxDefaultSize,
        wxTR_HAS_BUTTONS | wxSUNKEN_BORDER);
    p_TabArea->AddTab(p_DBTree,(p_DSN+Which)->Dsn,_T(" ? "));
    p_DBTree->i_ViewNr = p_TabArea->GetTabCount()-1;
    p_TabArea->Show(true);    // Deactivate the Window
    p_DBTree->i_Which  = Which;
    p_DBTree->s_DSN    = (p_DSN+Which)->Dsn;
    p_DBTree->i_TabArt = 0;
    p_DBTree->pDoc     = this;
    p_DBTree->OnPopulate();
    p_TabArea->SetActiveTab(p_DBTree->i_ViewNr);
    //---------------------------------------------------------------------------------------
    // wxLogMessage("OnChosenDSN(%d) - End",Which);
    return true;
}

//----------------------------------------------------------------------------------------
bool MainDoc::OnChosenTbl(int Tab,wxString Table)
{
    // wxLogMessage("OnChosenTbl(%d,%s)",Tab,Table.c_str());
    //-------------------------
    if (p_DBGrid != NULL)
    {
        if (p_DBGrid->i_TabArt == 0)
        {
            p_TabArea->Show(false);    // Deactivate the Window
            p_TabArea->RemoveTab(p_DBGrid->i_ViewNr);
            p_TabArea->Show(true);     // Activate the Window
        }
        if (p_DBGrid->i_TabArt == 1)
        {
            p_PageArea->Show(false);   // Deactivate the Window
            p_PageArea->RemoveTab(p_DBGrid->i_ViewNr);
            p_PageArea->Show(true);    // Activate the Window
        }
        p_DBGrid = NULL;
        delete p_DBGrid;
    }
    if (Tab == 77)               // Delete only
        return true;
    //-------------------------
    if (Tab == 0)  // Tabview
    {
        p_TabArea->Show(false);    // Deactivate the Window
        p_DBGrid = new DBGrid(p_TabArea,GRID_CTRL_DB,wxDefaultPosition, wxDefaultSize,
            wxSUNKEN_BORDER);
        p_TabArea->AddTab(p_DBGrid, Table, wxEmptyString);
        p_DBGrid->i_ViewNr = p_TabArea->GetTabCount()-1;
        p_DBGrid->pDoc       = this;
        p_DBGrid->db_Br      = db_Br;
        p_DBGrid->OnTableView(Table);
        p_TabArea->SetActiveTab(p_DBGrid->i_ViewNr);
        p_TabArea->Show(true);     // Activate the Window
    }
    if (Tab == 1)  // Pageview
    {
        p_PageArea->Show(false);   // Deactivate the Window
        p_DBGrid = new DBGrid(p_PageArea,GRID_CTRL_DB,wxDefaultPosition, wxDefaultSize,
            wxSUNKEN_BORDER);
        p_PageArea->AddTab(p_DBGrid, Table, wxEmptyString);
        p_DBGrid->i_ViewNr = p_PageArea->GetTabCount()-1;
        p_DBGrid->pDoc       = this;
        p_DBGrid->db_Br      = db_Br;
        p_DBGrid->i_Which    = p_DBTree->i_Which;
        p_PageArea->Show(true);    // Activate the Window
        p_DBGrid->OnTableView(Table);
        p_PageArea->SetActiveTab(p_DBGrid->i_ViewNr);
    }
    p_DBGrid->i_TabArt = Tab;
    //--------------------------
    return true;
}

//----------------------------------------------------------------------------------------
void MainDoc::OnLeer(wxString Aufrufer)
{
    // Temp0.Printf(_("\nMainDoc::OnLeer(%s) : auch diese funktion steht eines Tages zur Verfügung !"),Aufrufer.c_str());
    Temp0.Printf(_("\nMainDoc::OnLeer(%s) : even this function will one day be available !"),Aufrufer.c_str());
    wxLogMessage(Temp0);  Temp0.Empty();
    return;
}

//----------------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(DocSplitterWindow, wxSplitterWindow)
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
DocSplitterWindow::DocSplitterWindow(wxWindow *parent, wxWindowID id) : wxSplitterWindow(parent, id)
{ // Define a constructor for my p_Splitter
}
//----------------------------------------------------------------------------------------
