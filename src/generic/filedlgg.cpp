/////////////////////////////////////////////////////////////////////////////
// Name:        filedlgg.cpp
// Purpose:     wxFileDialog
// Author:      Robert Roebling
// Modified by:
// Created:     12/12/98
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "filedlgg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef __UNIX__
#error wxFileDialog currently only supports unix
#endif

#include "wx/filedlg.h"
#include "wx/debug.h"
#include "wx/log.h"
#include "wx/intl.h"
#include "wx/msgdlg.h"
#include "wx/sizer.h"
#include "wx/bmpbuttn.h"

#include "sys/types.h"
#include "sys/stat.h"
#include "dirent.h"
#include "pwd.h"
#include "grp.h"
#include "time.h"

#include "wx/generic/home.xpm"
#include "wx/generic/listview.xpm"
#include "wx/generic/repview.xpm"

/* XPM */
static char * folder_xpm[] = {
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

//-----------------------------------------------------------------------------
//  wxFileData
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxFileData,wxObject);

wxFileData::wxFileData( const wxString &name, const wxString &fname )
{
    m_name = name;
    m_fileName = fname;
    
    struct stat buff;
    stat( m_fileName.GetData(), &buff );
    struct stat lbuff;
    lstat( m_fileName.GetData(), &lbuff );

    struct tm *t = localtime( &lbuff.st_mtime );
//  struct passwd *user = getpwuid( buff.st_uid );
//  struct group *grp = getgrgid( buff.st_gid );

    m_isDir = S_ISDIR( buff.st_mode );
    m_isLink = S_ISLNK( lbuff.st_mode );
    m_isExe = ((buff.st_mode & S_IXUSR ) == S_IXUSR );

    m_size = buff.st_size;

    m_hour = t->tm_hour;
    m_minute = t->tm_min;
    m_month = t->tm_mon+1;
    m_day = t->tm_mday;
    m_year = t->tm_year;

    m_permissions.sprintf( "%c%c%c", 
     ((( buff.st_mode & S_IRUSR ) == S_IRUSR ) ? 'r' : '-'),
     ((( buff.st_mode & S_IWUSR ) == S_IWUSR ) ? 'w' : '-'),
     ((( buff.st_mode & S_IXUSR ) == S_IXUSR ) ? 'x' : '-') );
}

wxString wxFileData::GetName() const
{
    return m_name;
}

wxString wxFileData::GetFullName() const
{
    return m_fileName;
}

wxString wxFileData::GetHint() const
{
    wxString s = m_fileName;
    s += "  ";
    if (m_isDir) s += _("<DIR> ");
    else if (m_isLink) s += _("<LINK> ");
    else
    {
        s += LongToString( m_size );
        s += _(" bytes ");
    }
    s += IntToString( m_day );
    s += _T(".");
    s += IntToString( m_month );
    s += _T(".");
    s += IntToString( m_year );
    s += _T("  ");
    s += IntToString( m_hour );
    s += _T(":");
    s += IntToString( m_minute );
    s += _T("  ");
    s += m_permissions;
    return s;
};

wxString wxFileData::GetEntry( const int num )
{
    wxString s;
    switch (num)
    {
        case 0:
            s = m_name;
            break;
        case 1:
            if (m_isDir) s = _("<DIR>");
            else if (m_isLink) s = _("<LINK>");
            else s = LongToString( m_size );
            break;
        case 2:
            if (m_day < 10) s = _T("0"); else s = _T("");
            s += IntToString( m_day );
            s += _T(".");
            if (m_month < 10) s += _T("0"); 
            s += IntToString( m_month );
            s += _T(".");
            if (m_year < 10) s += _T("0"); // this should happen real soon...
            s += IntToString( m_year );
            break;
        case 3:
            if (m_hour < 10) s = _T("0"); else s = _T("");
            s += IntToString( m_hour );
            s += _T(":");
            if (m_minute < 10) s += _T("0");
            s += IntToString( m_minute );
            break;
        case 4:
            s = m_permissions;
            break;
        default:
            s = _T("No entry");
            break;
    }
    return s;
}

bool wxFileData::IsDir()
{
    return m_isDir;
}

bool wxFileData::IsExe()
{
    return m_isExe;
}

bool wxFileData::IsLink()
{
    return m_isLink;
}

long wxFileData::GetSize()
{
    return m_size;
}

void wxFileData::SetNewName( const wxString &name, const wxString &fname )
{
    m_name = name;
    m_fileName = fname;
}

void wxFileData::MakeItem( wxListItem &item )
{
    item.m_text = m_name;
    item.m_colour = wxBLACK;
    if (IsExe()) item.m_colour = wxRED;
    if (IsDir()) item.m_colour = wxBLUE;
    if (IsDir()) item.m_image = 0; else item.m_image = -1;
    if (IsLink())
    {
        wxColour *dg = wxTheColourDatabase->FindColour( "MEDIUM GREY" );
        item.m_colour = dg;
    }
    item.m_data = (long)this;
}
  
//-----------------------------------------------------------------------------
//  wxFileCtrl
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxFileCtrl,wxListCtrl);

BEGIN_EVENT_TABLE(wxFileCtrl,wxListCtrl)
    EVT_LIST_DELETE_ITEM(-1, wxFileCtrl::OnListDeleteItem)
    EVT_LIST_END_LABEL_EDIT(-1, wxFileCtrl::OnListEndLabelEdit)
END_EVENT_TABLE()

wxFileCtrl::wxFileCtrl()
{
    m_dirName = _T("/");
    m_showHidden = FALSE;
}

wxFileCtrl::wxFileCtrl( wxWindow *win, wxWindowID id, 
    const wxString &dirName, const wxString &wild,
    const wxPoint &pos, const wxSize &size,
    long style, const wxValidator &validator, const wxString &name ) :
  wxListCtrl( win, id, pos, size, style, validator, name )
{
    wxImageList *imageList = new wxImageList( 16, 16 );
    imageList->Add( wxBitmap( folder_xpm ) );
    SetImageList( imageList, wxIMAGE_LIST_SMALL );
    
    m_dirName = dirName;
    m_wild = wild;
    m_showHidden = FALSE;
    Update();
}

void wxFileCtrl::ChangeToListMode()
{
    SetSingleStyle( wxLC_LIST );
    Update();
}

void wxFileCtrl::ChangeToReportMode()
{
    SetSingleStyle( wxLC_REPORT );
    Update();
}

void wxFileCtrl::ChangeToIconMode()
{
    SetSingleStyle( wxLC_ICON );
    Update();
}

void wxFileCtrl::ShowHidden( bool show )
{
    m_showHidden = show;
    Update();
}

int ListCompare( const long data1, const long data2, const long WXUNUSED(data) )
{
     wxFileData *fd1 = (wxFileData*)data1 ;
     wxFileData *fd2 = (wxFileData*)data2 ;
     if (fd1->GetName() == _T("..")) return -1;
     if (fd2->GetName() == _T("..")) return 1;
     if (fd1->IsDir() && !fd2->IsDir()) return -1;
     if (fd2->IsDir() && !fd1->IsDir()) return 1;
     return strcmp( fd1->GetName(), fd2->GetName() );
}

long wxFileCtrl::Add( wxFileData *fd, wxListItem &item )
{
    long ret = -1;
    item.m_mask = wxLIST_MASK_TEXT + wxLIST_MASK_DATA + wxLIST_MASK_IMAGE;
    fd->MakeItem( item );
    long my_style = GetWindowStyleFlag();
    if (my_style & wxLC_REPORT)
    {
        ret = InsertItem( item );
        for (int i = 1; i < 5; i++) SetItem( item.m_itemId, i, fd->GetEntry( i) );
    }
    else if (my_style & wxLC_LIST)
    {
        ret = InsertItem( item );
    } 
    return ret;
}

void wxFileCtrl::Update()
{ 
    ClearAll();
    long my_style = GetWindowStyleFlag();
    if (my_style & wxLC_REPORT)
    {
        InsertColumn( 0, _("Name"), wxLIST_FORMAT_LEFT, 110 );
        InsertColumn( 1, _("Size"), wxLIST_FORMAT_LEFT, 60 );
        InsertColumn( 2, _("Date"), wxLIST_FORMAT_LEFT, 55 );
        InsertColumn( 3, _("Time"), wxLIST_FORMAT_LEFT, 50 );
        InsertColumn( 4, _("Permissions"), wxLIST_FORMAT_LEFT, 120 );
    }
    wxFileData *fd = (wxFileData *) NULL;
    wxListItem item;
    item.m_itemId = 0;
    item.m_col = 0;

    if (m_dirName != _T("/"))
    {
        wxString p( wxPathOnly(m_dirName) );
	if (p.IsEmpty()) p = _T("/");
        fd = new wxFileData( _T(".."), p );
	Add( fd, item );
        item.m_itemId++;
    }

    wxString res = m_dirName + _T("/") + m_wild;
    wxString f( wxFindFirstFile( res.GetData(), 0 ) );
    while (!f.IsEmpty())
    {
        res = wxFileNameFromPath( f );
        fd = new wxFileData( res, f );
        wxString s = fd->GetName();
        if (m_showHidden || (s[0] != _T('.')))
        {
	    Add( fd, item );
            item.m_itemId++;
        }
        f = wxFindNextFile();
    }
    SortItems( ListCompare, 0 );
}

void wxFileCtrl::SetWild( const wxString &wild )
{
    m_wild = wild;
    Update();
}

void wxFileCtrl::MakeDir()
{
    wxString new_name( _T("NewName") );
    wxString path( m_dirName );
    path += _T( "/" );
    path += new_name;
    if (wxFileExists(path))
    {
        // try NewName0, NewName1 etc.
        int i = 0;
	do {
            new_name = _("NewName");
	    wxString num;
	    num.Printf( _T("%d"), i );
	    new_name += num;
	    
            path = m_dirName;
            path += _T("/");
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

    wxFileData *fd = new wxFileData( new_name, path );
    wxListItem item;
    item.m_itemId = 0;
    item.m_col = 0;
    int id = Add( fd, item );
    
    if (id != -1)
    {
        SortItems( ListCompare, 0 );
	id = FindItem( 0, (long)fd );
        EnsureVisible( id );
        EditLabel( id );
    }
}

void wxFileCtrl::GoToParentDir()
{
    if (m_dirName != _T("/"))
    {
	wxString fname( wxFileNameFromPath(m_dirName) );
        m_dirName = wxPathOnly( m_dirName );
	if (m_dirName.IsEmpty()) m_dirName = _T("/");
        Update();
	int id = FindItem( 0, fname );
	if (id != -1)
	{
	    SetItemState( id, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
	    EnsureVisible( id );
	}
    }
}

void wxFileCtrl::GoToHomeDir()
{
    wxString s = wxGetUserHome( wxString() );
    m_dirName = s;
    Update();
}

void wxFileCtrl::GoToDir( const wxString &dir )
{
    m_dirName = dir;
    Update();
}

void wxFileCtrl::GetDir( wxString &dir )
{
    dir = m_dirName;
}

void wxFileCtrl::OnListDeleteItem( wxListEvent &event )
{
    wxFileData *fd = (wxFileData*)event.m_item.m_data;
    delete fd;
}

void wxFileCtrl::OnListEndLabelEdit( wxListEvent &event )
{
    wxFileData *fd = (wxFileData*)event.m_item.m_data;
    wxASSERT( fd );
    
    if ((event.GetLabel().IsEmpty()) ||
        (event.GetLabel() == _(".")) ||
        (event.GetLabel() == _("..")) ||
	(event.GetLabel().First( _T("/") ) != wxNOT_FOUND))
    {
        wxMessageDialog dialog(this, _("Illegal directory name."), _("Error"), wxOK | wxICON_ERROR );
	dialog.ShowModal();
        event.Veto();
	return;
    }
    
    wxString new_name( wxPathOnly( fd->GetFullName() ) );
    new_name += _T("/");
    new_name += event.GetLabel();
    
    wxLogNull log;
    
    if (wxFileExists(new_name))
    {
        wxMessageDialog dialog(this, _("File name exists already."), _("Error"), wxOK | wxICON_ERROR );
	dialog.ShowModal();
        event.Veto();
    }
    
    if (wxRenameFile(fd->GetFullName(),new_name))
    {
        fd->SetNewName( new_name, event.GetLabel() );
	SetItemState( event.GetItem(), wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
        EnsureVisible( event.GetItem() );
    }
    else
    {
        wxMessageDialog dialog(this, _("Operation not permitted."), _("Error"), wxOK | wxICON_ERROR );
	dialog.ShowModal();
        event.Veto();
    }
}

//-----------------------------------------------------------------------------
// wxFileDialog
//-----------------------------------------------------------------------------

#define  ID_LIST_CTRL     5010
#define  ID_LIST_MODE     5000
#define  ID_REPORT_MODE   5001
#define  ID_UP_DIR        5005
#define  ID_PARENT_DIR    5006
#define  ID_NEW_DIR       5007

IMPLEMENT_DYNAMIC_CLASS(wxFileDialog,wxDialog)

BEGIN_EVENT_TABLE(wxFileDialog,wxDialog)
        EVT_BUTTON(ID_LIST_MODE, wxFileDialog::OnList)
        EVT_BUTTON(ID_REPORT_MODE, wxFileDialog::OnReport)
        EVT_BUTTON(ID_UP_DIR, wxFileDialog::OnUp)
        EVT_BUTTON(ID_PARENT_DIR, wxFileDialog::OnHome)
        EVT_BUTTON(ID_NEW_DIR, wxFileDialog::OnNew)
        EVT_BUTTON(wxID_OK, wxFileDialog::OnListOk)
        EVT_LIST_ITEM_SELECTED(ID_LIST_CTRL, wxFileDialog::OnSelected)
	EVT_LIST_ITEM_ACTIVATED(ID_LIST_CTRL, wxFileDialog::OnActivated)
END_EVENT_TABLE()

wxFileDialog::wxFileDialog(wxWindow *parent,
                 const wxString& message,
                 const wxString& defaultDir,
                 const wxString& defaultFile,
                 const wxString& wildCard,
                 long style,
                 const wxPoint& pos ) :
  wxDialog( parent, -1, message )
{
    wxBeginBusyCursor();
    
    m_message = message;
    m_dialogStyle = style;
    m_dir = defaultDir;
    if (m_dir.IsEmpty()) m_dir = wxGetUserHome();
    m_path = defaultDir;
    m_path += _T("/");
    m_path += defaultFile;
    m_fileName = defaultFile;
    m_wildCard = wildCard;
    m_filterIndex = 0;
    
    wxBoxSizer *mainsizer = new wxBoxSizer( wxVERTICAL );
    
    wxBoxSizer *buttonsizer = new wxBoxSizer( wxHORIZONTAL );
    
    buttonsizer->Add( new wxBitmapButton( this, ID_LIST_MODE, wxBitmap( listview_xpm ) ), 0, wxALL, 5 );
    buttonsizer->Add( new wxBitmapButton( this, ID_REPORT_MODE, wxBitmap( repview_xpm ) ), 0, wxALL, 5 );
    buttonsizer->Add( 30, 5, 1 );
    buttonsizer->Add( new wxButton( this, ID_UP_DIR, "Up" ), 0, wxALL, 5 );
    buttonsizer->Add( new wxBitmapButton( this, ID_PARENT_DIR, wxBitmap(home_xpm) ), 0, wxALL, 5 );
    buttonsizer->Add( new wxButton( this, ID_NEW_DIR, "New..." ), 0, wxALL, 5 );
    mainsizer->Add( buttonsizer, 0, wxALL | wxEXPAND, 5 );
    
    m_list = new wxFileCtrl( this, ID_LIST_CTRL, m_dir, "*", wxDefaultPosition, wxSize(450,180), 
      wxLC_LIST | wxSUNKEN_BORDER | wxLC_SINGLE_SEL );
    mainsizer->Add( m_list, 1, wxEXPAND | wxLEFT|wxRIGHT, 10 );
    
    wxBoxSizer *textsizer = new wxBoxSizer( wxHORIZONTAL );
    m_text = new wxTextCtrl( this, -1, m_fileName );
    textsizer->Add( m_text, 1, wxCENTER | wxLEFT|wxRIGHT|wxTOP, 10 );
    textsizer->Add( new wxButton( this, wxID_OK, _("OK") ), 0, wxCENTER | wxLEFT|wxRIGHT|wxTOP, 10 );
    mainsizer->Add( textsizer, 0, wxEXPAND );

    wxBoxSizer *choicesizer = new wxBoxSizer( wxHORIZONTAL );
    m_choice = new wxChoice( this, -1 );
    m_choice->Append( "*.txt" );
    choicesizer->Add( m_choice, 1, wxCENTER|wxALL, 10 );
    choicesizer->Add( new wxButton( this, wxID_CANCEL, _("Cancel") ), 0, wxCENTER | wxALL, 10 );
    mainsizer->Add( choicesizer, 0, wxEXPAND );

    SetAutoLayout( TRUE );
    SetSizer( mainsizer );
    
    mainsizer->Fit( this );
    mainsizer->SetSizeHints( this );
    
    Centre( wxBOTH );
    
    m_list->SetFocus();
    
    wxEndBusyCursor();
}

void wxFileDialog::OnActivated( wxListEvent &WXUNUSED(event) )
{
    wxCommandEvent cevent(wxEVT_COMMAND_BUTTON_CLICKED, wxID_OK);
    cevent.SetEventObject( this );
    GetEventHandler()->ProcessEvent( cevent );
}

void wxFileDialog::OnSelected( wxListEvent &event )
{
    m_text->SetValue( event.m_item.m_text );
}

void wxFileDialog::OnListOk( wxCommandEvent &event )
{
    wxString filename( m_text->GetValue() );
    wxString dir;
    m_list->GetDir( dir );
    if (filename.IsEmpty()) return;
    
    if (filename == _T(".."))
    {
        m_list->GoToParentDir();
        m_list->SetFocus();
	return;
    }

    if (dir != _T("/")) dir += _T("/");
    dir += filename;
    filename = dir;
    
    if (wxDirExists(filename))
    {
        m_list->GoToDir( filename );
	m_text->SetValue( _T("") );
	return;
    }
    
    if ( (m_dialogStyle & wxSAVE) && (m_dialogStyle & wxOVERWRITE_PROMPT) )
    {
        if (wxFileExists( filename ))
        {
            wxString msg;
            msg.Printf( _("File '%s' already exists, do you really want to "
                         "overwrite it?"), filename.c_str() );

            if (wxMessageBox(msg, _("Confirm"), wxYES_NO) != wxYES)
                return;
        }
    }
    else if ( (m_dialogStyle & wxOPEN) && (m_dialogStyle & wxFILE_MUST_EXIST) )
    {
        if ( !wxFileExists( filename ) )
        {
            wxMessageBox(_("Please choose an existing file."), _("Error"), wxOK);

            return;
        }
    }

    SetPath( filename );
    event.Skip();
}

void wxFileDialog::OnList( wxCommandEvent &WXUNUSED(event) )
{
    m_list->ChangeToListMode();
    m_list->SetFocus();
}

void wxFileDialog::OnReport( wxCommandEvent &WXUNUSED(event) )
{
    m_list->ChangeToReportMode();
    m_list->SetFocus();
}

void wxFileDialog::OnUp( wxCommandEvent &WXUNUSED(event) )
{
    m_list->GoToParentDir();
    m_list->SetFocus();
}

void wxFileDialog::OnHome( wxCommandEvent &WXUNUSED(event) )
{
    m_list->GoToHomeDir();
    m_list->SetFocus();
}

void wxFileDialog::OnNew( wxCommandEvent &WXUNUSED(event) )
{
    m_list->MakeDir();
}

void wxFileDialog::SetPath( const wxString& path )
{
    // not only set the full path but also update filename and dir
    m_path = path;
    if ( !!path )
    {
        wxString ext;
        wxSplitPath(path, &m_dir, &m_fileName, &ext);
	if (!ext.IsEmpty())
	{
	    m_fileName += _T(".");
            m_fileName += ext;
	}
    }
}
    
// ----------------------------------------------------------------------------
// global functions
// ----------------------------------------------------------------------------

wxString
wxFileSelectorEx(const wxChar *message,
                 const wxChar *default_path,
                 const wxChar *default_filename,
                 int *indexDefaultExtension,
                 const wxChar *wildcard,
                 int flags,
                 wxWindow *parent,
                 int x, int y)
{
    // TODO: implement this somehow
    return wxFileSelector(message, default_path, default_filename, _T(""),
                          wildcard, flags, parent, x, y);
}

wxString wxFileSelector( const wxChar *title,
                      const wxChar *defaultDir, const wxChar *defaultFileName,
                      const wxChar *defaultExtension, const wxChar *filter, int flags,
                      wxWindow *parent, int x, int y )
{
    wxString filter2;
    if ( defaultExtension && !filter )
        filter2 = wxString(_T("*.")) + wxString(defaultExtension) ;
    else if ( filter )
        filter2 = filter;

    wxString defaultDirString;
    if (defaultDir)
        defaultDirString = defaultDir;

    wxString defaultFilenameString;
    if (defaultFileName)
        defaultFilenameString = defaultFileName;

    wxFileDialog fileDialog( parent, title, defaultDirString, defaultFilenameString, filter2, flags, wxPoint(x, y) );

    if ( fileDialog.ShowModal() == wxID_OK )
    {
        return fileDialog.GetPath();
    }
    else
    {
        return wxEmptyString;
    }
}

wxString wxLoadFileSelector( const wxChar *what, const wxChar *extension, const wxChar *default_name, wxWindow *parent )
{
    wxChar *ext = (wxChar *)extension;

    wxChar prompt[50];
    wxString str = _("Load %s file");
    wxSprintf(prompt, str, what);

    if (*ext == _T('.')) ext++;
    wxChar wild[60];
    wxSprintf(wild, _T("*.%s"), ext);

    return wxFileSelector (prompt, (const wxChar *) NULL, default_name, ext, wild, 0, parent);
}

wxString wxSaveFileSelector(const wxChar *what, const wxChar *extension, const wxChar *default_name,
         wxWindow *parent )
{
    wxChar *ext = (wxChar *)extension;

    wxChar prompt[50];
    wxString str = _("Save %s file");
    wxSprintf(prompt, str, what);

    if (*ext == _T('.')) ext++;
    wxChar wild[60];
    wxSprintf(wild, _T("*.%s"), ext);

    return wxFileSelector (prompt, (const wxChar *) NULL, default_name, ext, wild, 0, parent);
}

