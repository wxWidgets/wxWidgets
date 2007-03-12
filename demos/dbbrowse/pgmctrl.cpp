//----------------------------------------------------------------------------------------
// Name:        pgmctrl.cpp
// Purpose:     Programm Control with a Tree
// Author:      Mark Johnson
// Modified by: 19990806.mj10777
// Created:     19991010
// RCS-ID:      $Id$
// Copyright:   (c) Mark Johnson, Berlin Germany
// Licence:     wxWindows license
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
#ifndef __WXMSW__
#include "bitmaps/d_closed.xpm"
#include "bitmaps/d_open.xpm"
#include "bitmaps/f_closed.xpm"
#include "bitmaps/f_open.xpm"
#include "bitmaps/logo.xpm"
#include "bitmaps/dsnclose.xpm"
#include "bitmaps/dsnopen.xpm"
#endif
//----------------------------------------------------------------------------------------
//-- all #includes that every .cpp needs             --- 19990807.mj10777 ----------------
//----------------------------------------------------------------------------------------
#include "std.h"    // sorgsam Pflegen !

//----------------------------------------------------------------------------------------
//-- Global functions --------------------------------------------------------------------
//----------------------------------------------------------------------------------------
static inline const wxChar *bool2String(bool b)
{
    return b ? wxEmptyString : _T("not ");
}

//----------------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(PgmCtrl, wxTreeCtrl)
    EVT_MOTION (PgmCtrl::OnMouseMove)
    EVT_LEFT_DCLICK(PgmCtrl::OnSelChanged)
    EVT_TREE_ITEM_RIGHT_CLICK(TREE_CTRL_PGM,PgmCtrl::OnRightSelect)
    EVT_MENU(PGMCTRL_ODBC_USER,PgmCtrl::OnUserPassword)
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// PgmCtrl implementation
//----------------------------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(PgmCtrl, wxTreeCtrl)

//----------------------------------------------------------------------------------------
PgmCtrl::PgmCtrl(wxWindow *parent) : wxTreeCtrl(parent)
{
}

//----------------------------------------------------------------------------------------
PgmCtrl::PgmCtrl(wxWindow *parent, const wxWindowID id,const wxPoint& pos, const wxSize& size, long style)
: wxTreeCtrl(parent, id, pos, size, style)
{
    const int image_size = 16;
    // Make an image list containing small icons
    p_imageListNormal = new wxImageList(image_size, image_size, true);
    // should correspond to TreeIc_xxx enum
    p_imageListNormal->Add(wxBitmap(wxBitmap(wxICON(aLogo)).ConvertToImage().Rescale(image_size, image_size)));
    p_imageListNormal->Add(wxBitmap(wxBitmap(wxICON(DsnClosed)).ConvertToImage().Rescale(image_size, image_size)));
    p_imageListNormal->Add(wxBitmap(wxBitmap(wxICON(DsnOpen)).ConvertToImage().Rescale(image_size, image_size)));
    p_imageListNormal->Add(wxBitmap(wxBitmap(wxICON(DocClosed)).ConvertToImage().Rescale(image_size, image_size)));
    p_imageListNormal->Add(wxBitmap(wxBitmap(wxICON(DocOpen)).ConvertToImage().Rescale(image_size, image_size)));
    p_imageListNormal->Add(wxBitmap(wxBitmap(wxICON(FolderClosed)).ConvertToImage().Rescale(image_size, image_size)));
    p_imageListNormal->Add(wxBitmap(wxBitmap(wxICON(FolderOpen)).ConvertToImage().Rescale(image_size, image_size)));
    SetImageList(p_imageListNormal);
}

//----------------------------------------------------------------------------------------
PgmCtrl::~PgmCtrl()
{
    delete p_imageListNormal;
    delete popupMenu1;
}

//----------------------------------------------------------------------------------------
#undef TREE_EVENT_HANDLER

//----------------------------------------------------------------------------------------
int  PgmCtrl::OnPopulate()
{
    SetFont(* pDoc->ft_Doc);
    wxTreeItemId Root, Folder, Docu;
    //---------------------------------------------------------------------------------------
    int i;
    double dTmp = 1234567.89;
    Temp0.Printf(_("%s Functions"),p_ProgramCfg->GetAppName().c_str());
    Root   = AddRoot(Temp0,TreeIc_Logo,TreeIc_Logo, new TreeData(_T("Root")));
    //---------------------------------------------------------------------------------------
    Folder = AppendItem(Root, _("Program settings")   ,TreeIc_FolderClosed, TreeIc_FolderOpen, new TreeData(_T("Settings")));
    p_ProgramCfg->Read(_T("/Local/langid"),&Temp0); p_ProgramCfg->Read(_T("/Local/language"),&Temp2);
    Temp1.Printf(_("locale (%s) ; Language (%s) ; Number(%2.2f)"),Temp0.c_str(), Temp2.c_str(), dTmp); Temp0.Empty();  Temp2.Empty();
    Docu   = AppendItem(Folder, Temp1,TreeIc_DocClosed,TreeIc_DocOpen,new TreeData(_T("Setting Language")));
    p_ProgramCfg->Read(_T("/Paths/Work"),&Temp0); Temp1.Printf(_("Work Path : %s"),Temp0.c_str()); Temp0.Empty();
    Docu   = AppendItem(Folder,Temp1,TreeIc_DocClosed,TreeIc_DocOpen,new TreeData(_T("Path Work")));
    Docu   = AppendItem(Folder, _("Change the language to English") ,TreeIc_DocClosed,TreeIc_DocOpen,new TreeData(_T("Language English")));
    Docu   = AppendItem(Folder, _("Change the language to German")  ,TreeIc_DocClosed,TreeIc_DocOpen,new TreeData(_T("Language German")));
    Docu   = AppendItem(Folder, _("Delete all wxConfigBase Entry's"),TreeIc_DocClosed,TreeIc_DocOpen,new TreeData(_T("wxConfigBase Delete")));
    Folder = AppendItem(Root, _T("ODBC DSN"),TreeIc_FolderClosed,TreeIc_FolderOpen,new TreeData(_T("ODBC-DSN")));
    for (i=0;i<pDoc->i_DSN;i++)
    {
        Temp0.Printf(_T("ODBC-%s"),(pDoc->p_DSN+i)->Dsn.c_str());
        Docu   = AppendItem(Folder,(pDoc->p_DSN+i)->Dsn ,TreeIc_DsnClosed,TreeIc_DsnOpen, new TreeData(Temp0));
    }
    //---------------------------------------------------------------------------------------
    popupMenu1 = NULL;
    popupMenu1 = new wxMenu;
    popupMenu1->Append(PGMCTRL_ODBC_USER, _("Set Username and Password"));
    // popupMenu1->AppendSeparator();
    //---------------------------------------------------------------------------------------
    Expand(Root);
    Expand(Folder);
    //---------------------------------------------------------------------------------------
    return 0;
}

//----------------------------------------------------------------------------------------
void PgmCtrl::OnSelChanged(wxMouseEvent& WXUNUSED(event))
{
    int i;
    Temp0.Empty();   Temp1.Empty();
#if wxUSE_STATUSBAR
    pDoc->p_MainFrame->SetStatusText(Temp0,0);
#endif // wxUSE_STATUSBAR
    // Get the Information that we need
    wxTreeItemId itemId = GetSelection();
    TreeData *item = (TreeData *)GetItemData(itemId);
    if (item != NULL )
    {
        int Treffer = 0;
        Temp1.Printf(_T("%s"),item->m_desc.c_str());
        //--------------------------------------------------------------------------------------
        if (Temp1 == _T("Language English"))
        {
            Temp0 = _T("std");
            p_ProgramCfg->Write(_T("/Local/langid"),Temp0);
            Temp0 = _("-I-> After a programm restart, the language will be changed to English.");
            wxMessageBox(Temp0);
        }
        if (Temp1 == _T("Language German"))
        {
            Temp0 = _T("de");
            p_ProgramCfg->Write(_T("/Local/langid"),Temp0);
            Temp0 = _("-I-> After a programm restart, the language will be changed to German.");
            wxMessageBox(Temp0);
        }
        //--------------------------------------------------------------------------------------
        if (Temp1 == _T("wxConfigBase Delete"))
        {
            if (p_ProgramCfg->DeleteAll()) // Default Diretory for wxFileSelector
                Temp0 = _("-I-> wxConfigBase.p_ProgramCfg->DeleteAll() was successful.");
            else
                Temp0 = _("-E-> wxConfigBase.p_ProgramCfg->DeleteAll() was not successful !");
            wxBell();     // Ding_a_Ling
            Treffer++;
        }
        //--------------------------------------------------------------------------------------
        if (Temp1.Contains(_T("ODBC-")))
        {
            Temp1 = Temp1.Mid(5);
            for (i=0;i<pDoc->i_DSN;i++)
            {
                if (Temp1 == (pDoc->p_DSN+i)->Dsn)
                {
                    pDoc->OnChosenDSN(i);
                }
            }
            Treffer++;
        }
        //--------------------------------------------------------------------------------------
        if (Treffer == 0)
        {
            //-------------------------------------------------------------------------------------
            Temp0.Printf(_("Item '%s': %sselected, %sexpanded, %sbold, %u children (%u immediately under this item)."),
                item->m_desc.c_str(),
                bool2String(IsSelected(itemId)),
                bool2String(IsExpanded(itemId)),
                bool2String(IsBold(itemId)),
                GetChildrenCount(itemId),
                GetChildrenCount(itemId));
            LogBuf.Printf(_T("-I-> PgmCtrl::OnSelChanged - %s"),Temp0.c_str());
            wxLogMessage( _T("%s"), LogBuf.c_str() );
            //-------------------------------------------------------------------------------------
        }
    }
}

//----------------------------------------------------------------------------------------
void PgmCtrl::OnRightSelect(wxTreeEvent& WXUNUSED(event))
{
    int i;
    Temp0.Empty();
    // Get the Information that we need
    wxTreeItemId itemId = GetSelection();
    DBTreeData *item = (DBTreeData *)GetItemData(itemId);
    SaveDSN.Empty();
    if ( item != NULL )
    {
        int Treffer = 0;
        Temp1.Printf(_T("%s"),item->m_desc.c_str());
        //--------------------------------------------------------------------------------------
        if (Temp1.Contains(_T("ODBC-")))
        {
            Temp1 = Temp1.Mid(5);
            for (i=0;i<pDoc->i_DSN;i++)
            {
                if (Temp1 == (pDoc->p_DSN+i)->Dsn)
                {
                    SaveDSN = Temp1;
                    PopupMenu(popupMenu1,TreePos.x,TreePos.y);
                }
            }
            Treffer++;
        }
        //--------------------------------------------------------------------------------------
        if (Treffer == 0)
        {
        /*
        Temp0.Printf(_("Item '%s': %sselected, %sexpanded, %sbold,"
        "%u children (%u immediately under this item)."),
        item->m_desc.c_str(),
        bool2String(IsSelected(itemId)),
        bool2String(IsExpanded(itemId)),
        bool2String(IsBold(itemId)),
        GetChildrenCount(itemId),
        GetChildrenCount(itemId));
        LogBuf.Printf("-I-> DBTree::OnSelChanged - %s",Temp0.c_str());
        wxLogMessage( "%s", LogBuf.c_str() );
            */
        }
        //--------------------------------------------------------------------------------------
    }
} // void PgmCtrl::OnRightSelect(wxTreeEvent& WXUNUSED(event))

//----------------------------------------------------------------------------------------
void PgmCtrl::OnMouseMove(wxMouseEvent &event)
{
    TreePos = event.GetPosition();
}

//----------------------------------------------------------------------------------------
void PgmCtrl::OnUserPassword(wxCommandEvent& WXUNUSED(event))
{
    // wxMessageBox(SaveDSN);
    int i;
    //--------------------------------------------
    DlgUser *p_Dlg = new DlgUser(this,pDoc,wxEmptyString);
    //-------------------------------------------
    for (i=0;i<pDoc->i_DSN;i++)
    {
        wxYield();
        if (SaveDSN == (pDoc->p_DSN+i)->Dsn)
        {
            p_Dlg->pDoc       = pDoc;
            p_Dlg->s_DSN      = (pDoc->p_DSN+i)->Dsn;
            p_Dlg->s_User     = (pDoc->p_DSN+i)->Usr;
            p_Dlg->s_Password = (pDoc->p_DSN+i)->Pas;
            p_Dlg->OnInit();
            p_Dlg->Fit();
            //--------------------
            // Temp0.Printf("i(%d) ; s_DSN(%s) ; s_User(%s) ; s_Password(%s)",i,p_Dlg.s_DSN,p_Dlg.s_User,p_Dlg.s_Password);
            // wxMessageBox(Temp0);
            bool OK = false;
            if (p_Dlg->ShowModal() == wxID_OK)
            {
                (pDoc->p_DSN+i)->Usr = p_Dlg->s_User;
                (pDoc->p_DSN+i)->Pas = p_Dlg->s_Password;
                (pDoc->db_Br+i)->UserName  = (pDoc->p_DSN+i)->Usr;
                (pDoc->db_Br+i)->Password  = (pDoc->p_DSN+i)->Pas;
                OK = true;
            }
            delete p_Dlg;
            if (!OK)
                return;
            //--------------------
            break; // We have what we want, leave
        }
    }
    //-------------------------------------------
    SaveDSN.Empty();
}
//----------------------------------------------------------------------------------------
