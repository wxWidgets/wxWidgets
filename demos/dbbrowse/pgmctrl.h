//----------------------------------------------------------------------------------------
// Name:        pgmctrl.h
// Purpose:     Programm Control with a Tree
// Author:      Mark Johnson
// Modified by: 19990806.mj10777
// Created:     19991010
// RCS-ID:      $Id$
// Copyright:   (c) Mark Johnson, Berlin Germany, mj10777@gmx.net
// Licence:     wxWindows license

//----------------------------------------------------------------------------------------
class MainDoc;             // Declared in doc.h file

//----------------------------------------------------------------------------------------
class TreeData : public wxTreeItemData
{
public:
    TreeData(const wxString& desc) : m_desc(desc) { }
    void ShowInfo(wxTreeCtrl *tree);
    wxString m_desc;
};

//----------------------------------------------------------------------------------------
class PgmCtrl : public wxTreeCtrl
{
public:
    enum
    { // The order here must be the same as in m_imageListNormal !
        TreeIc_Logo,                         // logo.ico
            TreeIc_DsnClosed,                // dsnclose.ico
            TreeIc_DsnOpen,                  // dsnopen.ico
            TreeIc_DocClosed,                // d_closed.ico
            TreeIc_DocOpen,                  // d_open.ico
            TreeIc_FolderClosed,             // f_closed.ico
            TreeIc_FolderOpen                // f_open.ico
    };
    wxString Temp0, Temp1, Temp2, Temp3, Temp4, Temp5;
    //----------------------------------------------------------------------------------------
    PgmCtrl() { }
    PgmCtrl(wxWindow *parent);
    PgmCtrl(wxWindow *parent, const wxWindowID id,const wxPoint& pos, const wxSize& size,long style);
    virtual ~PgmCtrl();
    MainDoc *pDoc;
    //----------------------------------------------------------------------------------------
    int i_TabArt;   // Tab = 0 ; Page = 1;
    int i_ViewNr;   // View Nummer in Tab / Page
    //----------------------------------------------------------------------------------------
    wxImageList *p_imageListNormal;
    wxMenu   *popupMenu1; // OnDBClass
    wxPoint  TreePos;
    wxString SaveDSN;     // Needed for User and Password
    //----------------------------------------------------------------------------------------
public:
    int  OnPopulate();
    void OnSelChanged(wxMouseEvent& event);
    void OnRightSelect(wxTreeEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnUserPassword(wxCommandEvent& event);
    
public:
    //----------------------------------------------------------------------------------------
    // NB: due to an ugly wxMSW hack you _must_ use DECLARE_DYNAMIC_CLASS()
    //     if you want your overloaded OnCompareItems() to be called.
    //     OTOH, if you don't want it you may omit the next line - this will
    //     make default (alphabetical) sorting much faster under wxMSW.
    DECLARE_DYNAMIC_CLASS(PgmCtrl)
        DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------------------
#define PGMCTRL_01_BEGIN     1000
#define PGMCTRL_ODBC_USER    1001
#define PGMCTRL_01_END       1010
//----------------------------------------------------------------------------------------
