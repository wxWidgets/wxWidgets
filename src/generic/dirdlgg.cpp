/////////////////////////////////////////////////////////////////////////////
// Name:        dirdlg.cpp
// Purpose:     wxDirDialog
// Author:      Harm van der Heijden and Robert Roebling
// Modified by:
// Created:     12/12/98
// Copyright:   (c) Harm van der Heijden and Robert Roebling
// Licence:   	wxWindows licence
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

#include "wx/generic/dirdlgg.h"

// If compiled under Windows, this macro can cause problems
#ifdef GetFirstChild
#undef GetFirstChild
#endif

/* XPM */
static char * icon1_xpm[] = {
/* width height ncolors chars_per_pixel */
"16 16 6 1",
/* colors */
" 	s None	c None",
".	c #000000",
"+	c #c0c0c0",
"@	c #808080",
"#	c #ffff00",
"$	c #ffffff",
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
" 	s None	c None",
".	c #000000",
"+	c #c0c0c0",
"@	c #808080",
"#	c #ffff00",
"$	c #ffffff",
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

static const int ID_DIRCTRL = 1000;
static const int ID_TEXTCTRL = 1001;
static const int ID_OK = 1002;
static const int ID_CANCEL = 1003;
static const int ID_NEW = 1004;
//static const int ID_CHECK = 1005;

//-----------------------------------------------------------------------------
// wxDirItemData
//-----------------------------------------------------------------------------

class wxDirItemData : public wxTreeItemData
{
public:
  wxDirItemData(wxString& path, wxString& name);
  ~wxDirItemData();
  bool HasSubDirs();
  wxString *m_path, *m_name;
  bool m_isHidden;
  bool m_hasSubDirs;
};

//-----------------------------------------------------------------------------
// wxDirCtrl
//-----------------------------------------------------------------------------

class wxDirCtrl: public wxTreeCtrl
{
  DECLARE_DYNAMIC_CLASS(wxDirCtrl)
  
  public:
    bool   m_showHidden;
    wxTreeItemId   m_rootId;
  
    wxDirCtrl(void);
    wxDirCtrl(wxWindow *parent, const wxWindowID id = -1, 
	      const wxString &dir = "/",
	      const wxPoint& pos = wxDefaultPosition,
	      const wxSize& size = wxDefaultSize,
	      const long style = wxTR_HAS_BUTTONS,
	      const wxString& name = "wxTreeCtrl" );
    void OnExpandItem(wxTreeEvent &event );
    void OnCollapseItem(wxTreeEvent &event );
    void ShowHidden( const bool yesno );
    DECLARE_EVENT_TABLE()
 protected:
    void CreateItems(const wxTreeItemId &parent);
    void SetupSections(void);
    wxArrayString m_paths, m_names;
};

//-----------------------------------------------------------------------------
// wxDirItemData
//-----------------------------------------------------------------------------

wxDirItemData::wxDirItemData(wxString& path, wxString& name)
{
  m_path = new wxString(path);
  m_name = new wxString(name);
  /* Insert logic to detect hidden files here 
   * In UnixLand we just check whether the first char is a dot 
   * For FileNameFromPath read LastDirNameInThisPath ;-) */
  // m_isHidden = (bool)(wxFileNameFromPath(*m_path)[0] == '.');
  m_isHidden = FALSE;
  m_hasSubDirs = HasSubDirs();
}

wxDirItemData:: ~wxDirItemData()
{
  delete m_path;
  delete m_name;
}

bool wxDirItemData::HasSubDirs()
{
  wxString search = *m_path + "/*";
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
END_EVENT_TABLE()

wxDirCtrl::wxDirCtrl(void)
{
  m_showHidden = FALSE;
};

wxDirCtrl::wxDirCtrl(wxWindow *parent, const wxWindowID id, const wxString &WXUNUSED(dir),
            const wxPoint& pos, const wxSize& size,
            const long style, const wxString& name )
 :
  wxTreeCtrl( parent, id, pos, size, style, wxDefaultValidator, name )
{
  m_imageListNormal = new wxImageList(16, 16, TRUE);
  m_imageListNormal->Add(wxICON(icon1));
  m_imageListNormal->Add(wxICON(icon2));
  SetImageList(m_imageListNormal);
  
  m_showHidden = FALSE;
  m_rootId = AddRoot("Sections");
  SetItemHasChildren(m_rootId);
  Expand(m_rootId); // automatically expand first level
};

/* Quick macro. Don't worry, I'll #undef it later */
#define ADD_SECTION(a,b) \
  if (wxPathExists((a))) { m_paths.Add( (a) ); m_names.Add( (b) ); };

void wxDirCtrl::SetupSections() 
{
  wxString home;

  m_paths.Clear();
  m_names.Clear();
  ADD_SECTION("/", _("The Computer") )
  wxGetHomeDir(&home);
  ADD_SECTION(home, _("My Home") )
  ADD_SECTION("/mnt", _("Mounted Devices") )
  ADD_SECTION("/usr", _("User") )
  ADD_SECTION("/usr/local", _("User Local") )
  ADD_SECTION("/var", _("Variables") )
  ADD_SECTION("/etc", _("Etcetera") )
  ADD_SECTION("/tmp", _("Temporary") )
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
    id = AppendItem( parent, m_names[i], 0, 1, dir_item);
    if (dir_item->m_hasSubDirs) SetItemHasChildren(id);
  }
}

void wxDirCtrl::OnExpandItem(wxTreeEvent &event )
{
  if (event.GetItem() == m_rootId)
  {
    SetupSections();
    CreateItems(m_rootId);
    return;
  };

  // This may take a longish time. Go to busy cursor
  wxBeginBusyCursor();

  wxDirItemData *data = (wxDirItemData *)GetItemData(event.GetItem());
  wxASSERT(data);

  wxString search,path,filename;

  m_paths.Clear();
  m_names.Clear();
  search = *(data->m_path) + "/*";
  for (path = wxFindFirstFile( search, wxDIR ); !path.IsNull(); 
       path=wxFindNextFile() ) {
    filename = wxFileNameFromPath( path );
    /* Don't add "." and ".." to the tree. I think wxFindNextFile
     * also checks this, but I don't quite understand what happens
     * there. Also wxFindNextFile seems to swallow hidden dirs */
    if ((filename != ".") && (filename != "..")) {
      m_paths.Add(path);
      m_names.Add(filename);
    }
  }
  CreateItems(event.GetItem());
  wxEndBusyCursor();
  
  SortChildren( event.GetItem() );
};


void wxDirCtrl::OnCollapseItem(wxTreeEvent &event )
{
  wxTreeItemId child, parent = event.GetItem();
  long cookie;
  /* Workaround because DeleteChildren has disapeared (why?) and
   * CollapseAndReset doesn't work as advertised (deletes parent too) */
  child = GetFirstChild(parent, cookie);
  while (child.IsOk()) {
    Delete(child);
    /* Not GetNextChild below, because the cookie mechanism can't 
     * handle disappearing children! */
    child = GetFirstChild(parent, cookie);
  }
};

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
	   wxDEFAULT_DIALOG_STYLE|wxDIALOG_MODAL)
{
  m_message = message;
  m_dialogStyle = style;
  m_parent = parent;
  
  m_path = defaultPath;

  m_dir = new wxDirCtrl( this, ID_DIRCTRL, "/", wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS | wxSUNKEN_BORDER ); 
  m_input = new wxTextCtrl( this, ID_TEXTCTRL, m_path, wxDefaultPosition ); 
  // m_check = new wxCheckBox( this, ID_CHECK, _("Show hidden") );
  m_ok = new wxButton( this, ID_OK, _("OK") );
  m_cancel = new wxButton( this, ID_CANCEL, _("Cancel") );
  m_new = new wxButton( this, ID_NEW, _("New...") );

  // m_check->SetValue(TRUE);
  m_ok->SetDefault();
  m_dir->SetFocus();

  doSize();
}

void wxDirDialog::OnSize(wxSizeEvent& WXUNUSED(event))
{
  doSize();
}

void wxDirDialog::doSize()
{
  /* Figure out height of DirCtrl, which is what is left over by
   * the textctrl and the buttons. Manually, because I can't seem
   * to get the constraints stuff to do this */
  int w,h,h2;

  GetClientSize(&w, &h);
  m_input->GetSize(&w,&h2); h -= h2;
  m_ok->GetSize(&w, &h2); h -= h2;
  //m_check->GetSize(&w, &h2); h -= h2;
  h -= 20;

  wxLayoutConstraints *c = new wxLayoutConstraints;
  c->left.SameAs	(this, wxLeft,5);
  c->right.SameAs        (this, wxRight,5);
  c->height.Absolute    (h);
  c->top.SameAs		(this, wxTop,5);
  m_dir->SetConstraints(c);

  c = new wxLayoutConstraints;
  c->left.SameAs	(this, wxLeft,5);
  c->right.SameAs        (this, wxRight,5);
  c->height.AsIs	();
  c->top.Below		(m_dir,5);
  m_input->SetConstraints(c);

  /*  c = new wxLayoutConstraints;
  c->left.SameAs	(this, wxLeft,5);
  c->right.SameAs        (this, wxRight,5);
  c->height.AsIs	();
  c->top.Below		(m_input,5);
  m_check->SetConstraints(c); */

  c = new wxLayoutConstraints;
  c->width.SameAs	(m_cancel, wxWidth);
  c->height.AsIs	();
  c->top.Below		(m_input,5);
  c->centreX.PercentOf	(this, wxWidth, 25);
  m_ok->SetConstraints(c);

  c = new wxLayoutConstraints;
  c->width.SameAs	(m_cancel, wxWidth);
  c->height.AsIs	();
  c->top.Below		(m_input,5);
  c->bottom.SameAs      (this, wxBottom, 5);
  c->centreX.PercentOf	(this, wxWidth, 50);
  m_new->SetConstraints(c);

  c = new wxLayoutConstraints;
  c->width.AsIs	        ();
  c->height.AsIs	();
  c->top.Below		(m_input,5);
  c->centreX.PercentOf	(this, wxWidth, 75);
  m_cancel->SetConstraints(c);

  Layout();
}

int wxDirDialog::ShowModal()
{
  m_input->SetValue( m_path );
  return wxDialog::ShowModal();
}

void wxDirDialog::OnTreeSelected( wxTreeEvent &event )
{
  wxDirItemData *data = 
    (wxDirItemData*)m_dir->GetItemData(event.GetItem());
  if (data) 
    m_input->SetValue( *(data->m_path) );
};

void wxDirDialog::OnTreeKeyDown( wxTreeEvent &WXUNUSED(event) )
{
  wxDirItemData *data = 
    (wxDirItemData*)m_dir->GetItemData(m_dir->GetSelection());
  if (data) 
    m_input->SetValue( *(data->m_path) );
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
  wxMessageDialog dialog(this, msg, _("Directory does not exist"), wxYES_NO);
  if ( dialog.ShowModal() == wxID_YES ) {
    // Okay, let's make it
    if (wxMkdir(m_path)) {
      // The new dir was created okay.
      EndModal(wxID_OK);
      return;
    }
    else {
      // Trouble...
      msg = _("Failed to create directory ")+m_path+
	_("\n(Do you have the required permissions?)");
      wxMessageDialog errmsg(this, msg, _("Error creating directory"), wxOK);
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
  wxTextEntryDialog dialog(this, _("Enter the name of the directory to create"),
  	_("Create New Directory"), m_input->GetValue(), wxOK|wxCANCEL);

  while (dialog.ShowModal() == wxID_OK)
  {
    // Okay, let's make it
    if (wxMkdir(dialog.GetValue())) {
      // The new dir was created okay.
      m_path = dialog.GetValue();
      return;
    }
    wxString msg = _("Failed to create directory ")+dialog.GetValue()+
      _("\n(Do you have the required permissions?)") ;
    wxMessageDialog errmsg(this, msg, _("Error creating directory"), wxOK);
    errmsg.ShowModal();
    // Show the create dialog again, until user clicks cancel or enters
    // a valid dir.
  }
}

/*
void wxDirDialog::OnCheck( wxCommandEvent& WXUNUSED(event) )
{
  printf("Checkbox clicked: %s\n", ( m_check->GetValue() ? "on" : "off" ) );
}
*/
