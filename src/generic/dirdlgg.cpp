/////////////////////////////////////////////////////////////////////////////
// Name:        dirdlg.cpp
// Purpose:     wxDirDialog
// Author:      Harm van der Heijden and Robert Roebling
// Modified by:
// Created:     12/12/98
// RCS-ID:      $Id$
// Copyright:   (c) Harm van der Heijden and Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dirdlgg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/defs.h"

#if wxUSE_DIRDLG

#include "wx/utils.h"
#include "wx/dialog.h"
#include "wx/button.h"
#include "wx/layout.h"
#include "wx/msgdlg.h"
#include "wx/textdlg.h"
#include "wx/filefn.h"
#include "wx/cmndata.h"
#include "wx/gdicmn.h"
#include "wx/intl.h"
#include "wx/imaglist.h"
#include "wx/icon.h"
#include "wx/log.h"
#include "wx/sizer.h"

#if wxUSE_STATLINE
    #include "wx/statline.h"
#endif

#include "wx/generic/dirdlgg.h"

// If compiled under Windows, this macro can cause problems
#ifdef GetFirstChild
#undef GetFirstChild
#endif

#ifndef __WXMSW__
/* XPM */
static char * icon1_xpm[] = {
/* width height ncolors chars_per_pixel */
"16 16 6 1",
/* colors */
"   s None  c None",
".  c #000000",
"+  c #c0c0c0",
"@  c #808080",
"#  c #ffff00",
"$  c #ffffff",
/* pixels */
"                ",
"   @@@@@        ",
"  @#+#+#@       ",
" @#+#+#+#@@@@@@ ",
" @$$$$$$$$$$$$@.",
" @$#+#+#+#+#+#@.",
" @$+#+#+#+#+#+@.",
" @$#+#+#+#+#+#@.",
" @$+#+#+#+#+#+@.",
" @$#+#+#+#+#+#@.",
" @$+#+#+#+#+#+@.",
" @$#+#+#+#+#+#@.",
" @@@@@@@@@@@@@@.",
"  ..............",
"                ",
"                "};

/* XPM */
static char * icon2_xpm[] = {
/* width height ncolors chars_per_pixel */
"16 16 6 1",
/* colors */
"   s None  c None",
".  c #000000",
"+  c #c0c0c0",
"@  c #808080",
"#  c #ffff00",
"$  c #ffffff",
/* pixels */
"                ",
"   @@@@@        ",
"  @$$$$$@       ",
" @$#+#+#$@@@@@@ ",
" @$+#+#+$$$$$$@.",
" @$#+#+#+#+#+#@.",
"@@@@@@@@@@@@@#@.",
"@$$$$$$$$$$@@+@.",
"@$#+#+#+#+##.@@.",
" @$#+#+#+#+#+.@.",
" @$+#+#+#+#+#.@.",
"  @$+#+#+#+##@..",
"  @@@@@@@@@@@@@.",
"   .............",
"                ",
"                "};

#endif // !wxMSW

static const int ID_DIRCTRL = 1000;
static const int ID_TEXTCTRL = 1001;
static const int ID_OK = 1002;
static const int ID_CANCEL = 1003;
static const int ID_NEW = 1004;
//static const int ID_CHECK = 1005;

//-----------------------------------------------------------------------------
// wxDirItemData
//-----------------------------------------------------------------------------

wxDirItemData::wxDirItemData(wxString& path, wxString& name)
{
    m_path = path;
    m_name = name;
    /* Insert logic to detect hidden files here
     * In UnixLand we just check whether the first char is a dot
     * For FileNameFromPath read LastDirNameInThisPath ;-) */
    // m_isHidden = (bool)(wxFileNameFromPath(*m_path)[0] == '.');
    m_isHidden = FALSE;
    m_hasSubDirs = HasSubDirs();
}

wxDirItemData::~wxDirItemData()
{
}

void wxDirItemData::SetNewDirName( wxString path )
{
    m_path = path;
    m_name = wxFileNameFromPath( path );
}

bool wxDirItemData::HasSubDirs()
{
    wxString search = m_path + "/*";
    wxLogNull log;
    wxString path = wxFindFirstFile( search, wxDIR );
    return (bool)(!path.IsNull());
}

//-----------------------------------------------------------------------------
// wxDirCtrl
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxDirCtrl,wxTreeCtrl)

BEGIN_EVENT_TABLE(wxDirCtrl,wxTreeCtrl)
  EVT_TREE_ITEM_EXPANDING     (-1, wxDirCtrl::OnExpandItem)
  EVT_TREE_ITEM_COLLAPSED     (-1, wxDirCtrl::OnCollapseItem)
  EVT_TREE_BEGIN_LABEL_EDIT   (-1, wxDirCtrl::OnBeginEditItem)
  EVT_TREE_END_LABEL_EDIT     (-1, wxDirCtrl::OnEndEditItem)
END_EVENT_TABLE()

wxDirCtrl::wxDirCtrl(void)
{
    m_showHidden = FALSE;
}

wxDirCtrl::wxDirCtrl(wxWindow *parent, const wxWindowID id, const wxString &WXUNUSED(dir),
            const wxPoint& pos, const wxSize& size,
            const long style, const wxString& name )
 :
  wxTreeCtrl( parent, id, pos, size, style, wxDefaultValidator, name )
{
 #ifndef __WXMSW__
    m_imageListNormal = new wxImageList(16, 16, TRUE);
    m_imageListNormal->Add(wxICON(icon1));
    m_imageListNormal->Add(wxICON(icon2));
    SetImageList(m_imageListNormal);
 #endif

    m_showHidden = FALSE;
    m_rootId = AddRoot( _("Sections") );
    SetItemHasChildren(m_rootId);
    Expand(m_rootId); // automatically expand first level
}

/* Quick macro. Don't worry, I'll #undef it later */
#define ADD_SECTION(a,b) \
  if (wxPathExists((a))) { m_paths.Add( (a) ); m_names.Add( (b) ); };

void wxDirCtrl::SetupSections()
{
  wxString home;

  m_paths.Clear();
  m_names.Clear();
#ifdef __WXMSW__
  // better than nothing
  ADD_SECTION(wxT("c:\\"), _("My Harddisk") )
#else
  ADD_SECTION(wxT("/"), _("The Computer") )
  wxGetHomeDir(&home);
  ADD_SECTION(home, _("My Home") )
  ADD_SECTION(wxT("/mnt"), _("Mounted Devices") )
  ADD_SECTION(wxT("/usr"), _("User") )
  ADD_SECTION(wxT("/usr/local"), _("User Local") )
  ADD_SECTION(wxT("/var"), _("Variables") )
  ADD_SECTION(wxT("/etc"), _("Etcetera") )
  ADD_SECTION(wxT("/tmp"), _("Temporary") )
#endif
}
#undef ADD_SECTION

void wxDirCtrl::CreateItems(const wxTreeItemId &parent)
{
    wxTreeItemId id;
    wxDirItemData *dir_item;

//  wxASSERT(m_paths.Count() == m_names.Count());  ?

    for (unsigned int i=0; i<m_paths.Count(); i++)
    {
  dir_item = new wxDirItemData(m_paths[i],m_names[i]);
#ifdef __WXMSW__
  id = AppendItem( parent, m_names[i], -1, -1, dir_item);
#else
  id = AppendItem( parent, m_names[i], 0, 1, dir_item);
#endif
        if (dir_item->m_hasSubDirs) SetItemHasChildren(id);
    }
}

void wxDirCtrl::OnBeginEditItem(wxTreeEvent &event)
{
    // don't rename the main entry "Sections"
    if (event.GetItem() == m_rootId)
    {
        event.Veto();
        return;
    }

    // don't rename the individual sections
    if (GetParent( event.GetItem() ) == m_rootId)
    {
        event.Veto();
        return;
    }
}

void wxDirCtrl::OnEndEditItem(wxTreeEvent &event)
{
    if ((event.GetLabel().IsEmpty()) ||
        (event.GetLabel() == _(".")) ||
        (event.GetLabel() == _("..")) ||
  (event.GetLabel().First( wxT("/") ) != wxNOT_FOUND))
    {
        wxMessageDialog dialog(this, _("Illegal directory name."), _("Error"), wxOK | wxICON_ERROR );
  dialog.ShowModal();
        event.Veto();
  return;
    }

    wxTreeItemId id = event.GetItem();
    wxDirItemData *data = (wxDirItemData*)GetItemData( id );
    wxASSERT( data );

    wxString new_name( wxPathOnly( data->m_path ) );
    new_name += wxT("/");
    new_name += event.GetLabel();

    wxLogNull log;

    if (wxFileExists(new_name))
    {
        wxMessageDialog dialog(this, _("File name exists already."), _("Error"), wxOK | wxICON_ERROR );
  dialog.ShowModal();
        event.Veto();
    }

    if (wxRenameFile(data->m_path,new_name))
    {
        data->SetNewDirName( new_name );
    }
    else
    {
        wxMessageDialog dialog(this, _("Operation not permitted."), _("Error"), wxOK | wxICON_ERROR );
  dialog.ShowModal();
        event.Veto();
    }
}

void wxDirCtrl::OnExpandItem(wxTreeEvent &event)
{
    if (event.GetItem() == m_rootId)
    {
        SetupSections();
        CreateItems(m_rootId);
        return;
    }

    // This may take a longish time. Go to busy cursor
    wxBeginBusyCursor();

    wxDirItemData *data = (wxDirItemData *)GetItemData(event.GetItem());
    wxASSERT(data);

    wxString search,path,filename;

    m_paths.Clear();
    m_names.Clear();
#ifdef __WXMSW__
    search = data->m_path + "\\*.*";
#else
    search = data->m_path + "/*";
#endif
    for (path = wxFindFirstFile( search, wxDIR ); !path.IsNull();
       path=wxFindNextFile() )
    {
        filename = wxFileNameFromPath( path );
        /* Don't add "." and ".." to the tree. I think wxFindNextFile
         * also checks this, but I don't quite understand what happens
         * there. Also wxFindNextFile seems to swallow hidden dirs */
        if ((filename != ".") && (filename != ".."))
        {
            m_paths.Add(path);
            m_names.Add(filename);
        }
    }

    CreateItems( event.GetItem() );
    SortChildren( event.GetItem() );

    wxEndBusyCursor();
}

void wxDirCtrl::OnCollapseItem(wxTreeEvent &event )
{
    wxTreeItemId child, parent = event.GetItem();
    long cookie;
    /* Workaround because DeleteChildren has disapeared (why?) and
     * CollapseAndReset doesn't work as advertised (deletes parent too) */
    child = GetFirstChild(parent, cookie);
    while (child.IsOk())
    {
        Delete(child);
        /* Not GetNextChild below, because the cookie mechanism can't
         * handle disappearing children! */
        child = GetFirstChild(parent, cookie);
    }
}

//-----------------------------------------------------------------------------
// wxDirDialog
//-----------------------------------------------------------------------------


#if !USE_SHARED_LIBRARY
IMPLEMENT_CLASS(wxDirDialog, wxDialog)
#else
IMPLEMENT_DYNAMIC_CLASS( wxDirDialog, wxDialog )
#endif

BEGIN_EVENT_TABLE( wxDirDialog, wxDialog )
  EVT_TREE_KEY_DOWN        (ID_DIRCTRL,   wxDirDialog::OnTreeKeyDown)
  EVT_TREE_SEL_CHANGED     (ID_DIRCTRL,   wxDirDialog::OnTreeSelected)
  EVT_SIZE                 (              wxDirDialog::OnSize)
  EVT_BUTTON               (ID_OK,        wxDirDialog::OnOK)
  EVT_BUTTON               (ID_CANCEL,    wxDirDialog::OnCancel)
  EVT_BUTTON               (ID_NEW,       wxDirDialog::OnNew)
  EVT_TEXT_ENTER           (ID_TEXTCTRL,  wxDirDialog::OnOK)
  //  EVT_CHECKBOX             (ID_CHECK,     wxDirDialog::OnCheck)
END_EVENT_TABLE()

wxDirDialog::wxDirDialog(wxWindow *parent, const wxString& message,
       const wxString& defaultPath, long style,
       const wxPoint& pos) :
  wxDialog(parent, -1, message, pos, wxSize(300,300),
     wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    m_message = message;
    m_dialogStyle = style;
    m_parent = parent;

    m_path = defaultPath;

    wxBeginBusyCursor();

    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

    // 1) dir ctrl
    m_dir = new wxDirCtrl( this, ID_DIRCTRL, "/", wxDefaultPosition, wxSize(200,200),
      wxTR_HAS_BUTTONS | wxSUNKEN_BORDER | wxTR_EDIT_LABELS);
    topsizer->Add( m_dir, 1, wxTOP|wxLEFT|wxRIGHT | wxEXPAND, 10 );

    // 2) text ctrl
    m_input = new wxTextCtrl( this, ID_TEXTCTRL, m_path, wxDefaultPosition );
    topsizer->Add( m_input, 0, wxTOP|wxLEFT|wxRIGHT | wxEXPAND, 10 );

    // m_check = new wxCheckBox( this, ID_CHECK, _("Show hidden") );
    // m_check->SetValue(TRUE);

#if wxUSE_STATLINE
    // 3) static line
    topsizer->Add( new wxStaticLine( this, -1 ), 0, wxEXPAND | wxLEFT|wxRIGHT|wxTOP, 10 );
#endif

    // 4) buttons
    wxSizer* buttonsizer = new wxBoxSizer( wxHORIZONTAL );
    m_ok = new wxButton( this, ID_OK, _("OK") );
    buttonsizer->Add( m_ok, 0, wxLEFT|wxRIGHT, 10 );
    m_cancel = new wxButton( this, ID_CANCEL, _("Cancel") );
    buttonsizer->Add( m_cancel, 0, wxLEFT|wxRIGHT, 10 );
    m_new = new wxButton( this, ID_NEW, _("New...") );
    buttonsizer->Add( m_new, 0, wxLEFT|wxRIGHT, 10 );

    topsizer->Add( buttonsizer, 0, wxALL | wxCENTER, 10 );

    m_ok->SetDefault();
    m_dir->SetFocus();

    SetAutoLayout( TRUE );
    SetSizer( topsizer );

    topsizer->SetSizeHints( this );
    topsizer->Fit( this );

    Centre( wxBOTH );

    wxEndBusyCursor();
}

int wxDirDialog::ShowModal()
{
    m_input->SetValue( m_path );
    return wxDialog::ShowModal();
}

void wxDirDialog::OnTreeSelected( wxTreeEvent &event )
{
    wxDirItemData *data = (wxDirItemData*)m_dir->GetItemData(event.GetItem());
    if (data)
       m_input->SetValue( data->m_path );
};

void wxDirDialog::OnTreeKeyDown( wxTreeEvent &WXUNUSED(event) )
{
    wxDirItemData *data = (wxDirItemData*)m_dir->GetItemData(m_dir->GetSelection());
    if (data)
        m_input->SetValue( data->m_path );
};

void wxDirDialog::OnOK( wxCommandEvent& WXUNUSED(event) )
{
  m_path = m_input->GetValue();
  // Does the path exist? (User may have typed anything in m_input)
  if (wxPathExists(m_path)) {
    // OK, path exists, we're done.
    EndModal(wxID_OK);
    return;
  }
  // Interact with user, find out if the dir is a typo or to be created
  wxString msg( _("The directory ") );
  msg = msg + m_path;
  msg = msg + _("\ndoes not exist\nCreate it now?") ;
  wxMessageDialog dialog(this, msg, _("Directory does not exist"), wxYES_NO | wxICON_WARNING );
  if ( dialog.ShowModal() == wxID_YES ) {
    // Okay, let's make it
    wxLogNull log;
    if (wxMkdir(m_path)) {
      // The new dir was created okay.
      EndModal(wxID_OK);
      return;
    }
    else {
      // Trouble...
      msg = _("Failed to create directory ")+m_path+
  _("\n(Do you have the required permissions?)");
      wxMessageDialog errmsg(this, msg, _("Error creating directory"), wxOK | wxICON_ERROR);
      errmsg.ShowModal();
      // We still don't have a valid dir. Back to the main dialog.
    }
  }
  // User has answered NO to create dir.
}

void wxDirDialog::OnCancel( wxCommandEvent& WXUNUSED(event) )
{
  EndModal(wxID_CANCEL);
}

void wxDirDialog::OnNew( wxCommandEvent& WXUNUSED(event) )
{
    wxTreeItemId id = m_dir->GetSelection();
    if ((id == m_dir->GetRootItem()) ||
        (m_dir->GetParent(id) == m_dir->GetRootItem()))
    {
        wxMessageDialog msg(this, _("You cannot add a new directory to this section."),
                            _("Create directory"), wxOK | wxICON_INFORMATION );
        msg.ShowModal();
        return;
    }

    wxTreeItemId parent = m_dir->GetParent( id );
    wxDirItemData *data = (wxDirItemData*)m_dir->GetItemData( parent );
    wxASSERT( data );

    wxString new_name( wxT("NewName") );
    wxString path( data->m_path );
    path += wxT("/");
    path += new_name;
    if (wxFileExists(path))
    {
        // try NewName0, NewName1 etc.
        int i = 0;
  do {
            new_name = wxT("NewName");
      wxString num;
      num.Printf( wxT("%d"), i );
      new_name += num;

            path = data->m_path;
            path += wxT("/");
            path += new_name;
      i++;
  } while (wxFileExists(path));
    }

    wxLogNull log;
    if (!wxMkdir(path))
    {
        wxMessageDialog dialog(this, _("Operation not permitted."), _("Error"), wxOK | wxICON_ERROR );
  dialog.ShowModal();
        return;
    }

    wxDirItemData *new_data = new wxDirItemData( path, new_name );
    wxTreeItemId new_id = m_dir->AppendItem( parent, new_name, 0, 1, new_data );
    m_dir->EnsureVisible( new_id );
    m_dir->EditLabel( new_id );
}

/*
void wxDirDialog::OnCheck( wxCommandEvent& WXUNUSED(event) )
{
  printf("Checkbox clicked: %s\n", ( m_check->GetValue() ? "on" : "off" ) );
}
*/

#endif
