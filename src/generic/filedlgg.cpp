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
#include "wx/dnd.h"
#include "wx/debug.h"
#include "wx/log.h"
#include "wx/intl.h"
#include "wx/msgdlg.h"
#include "wx/sizer.h"

#include "sys/types.h"
#include "sys/stat.h"
#include "dirent.h"
#include "pwd.h"
#include "grp.h"
#include "time.h"

#include "wx/generic/folder.xpm"
#include "wx/generic/txt.xpm"
#include "wx/generic/list.xpm"
#include "wx/generic/find.xpm"

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

bool wxFileData::NewNameIsLegal( const wxString &s )
{
    wxString fileName = wxPathOnly( m_fileName );
    fileName += _T("/");
    fileName += s;
    return (!wxFileExists( fileName ));
}

bool wxFileData::Rename( const wxString &s )
{
    wxString fileName = wxPathOnly( m_fileName );
    fileName += _T("/");
    fileName += s;
    bool ret = wxRenameFile( m_fileName, fileName );
    if (ret)
    {
        m_fileName = fileName;
        m_name = s;
    }
    return ret;
}

void wxFileData::MakeItem( wxListItem &item )
{
    item.m_text = m_name;
    item.m_colour = wxBLACK;
    if (IsExe()) item.m_colour = wxRED;
    if (IsDir()) item.m_colour = wxBLUE;
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
END_EVENT_TABLE()

wxFileCtrl::wxFileCtrl()
{
    m_dirName = _T("/");
    m_showHidden = FALSE;
}

wxFileCtrl::wxFileCtrl( wxWindow *win, wxWindowID id, const wxString &dirName,
    const wxPoint &pos, const wxSize &size,
    long style, const wxValidator &validator, const wxString &name ) :
  wxListCtrl( win, id, pos, size, style, validator, name )
{
    SetItemSpacing( 40 );
    wxImageList *imageList = new wxImageList( 30, 30 );
    imageList->Add( wxBitmap( folder_xpm ) );
    imageList->Add( wxBitmap( txt_xpm ) );
    imageList->Add( wxBitmap( list_xpm ) );
    imageList->Add( wxBitmap( find_xpm ) );
  
    SetImageList( imageList, wxIMAGE_LIST_NORMAL );
  
    m_dirName = dirName;
    m_showHidden = FALSE;
    Update();
  
//  SetDropTarget( new wxFileDropTarget() );
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
     if (fd1->IsDir() && !fd2->IsDir()) return -1;
     if (fd2->IsDir() && !fd1->IsDir()) return 1;
     return strcmp( fd1->GetName(), fd2->GetName() );
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
    item.m_mask = wxLIST_MASK_TEXT + wxLIST_MASK_DATA;
    if (my_style & wxLC_ICON) item.m_mask += wxLIST_MASK_IMAGE;
    item.m_itemId = 0;
    item.m_col = 0;
    wxString s;
    wxString res = m_dirName + _T("/*");
    wxString f( wxFindFirstFile( res.GetData(), 0 ) );
    while (!f.IsEmpty())
    {
        res = wxFileNameFromPath( f );
        fd = new wxFileData( res, f );
        s = fd->GetName();
        if (m_showHidden || (s[0] != '.'))
        {
            fd->MakeItem( item );
            if (my_style & wxLC_REPORT)
            {
                InsertItem( item );
                for (int i = 1; i < 5; i++) SetItem( item.m_itemId, i, fd->GetEntry( i) );
            }
            else if (my_style & wxLC_LIST)
            {
                InsertItem( item );
            } 
            else if (my_style & wxLC_ICON)
            {
                if (fd->IsDir()) item.m_image = 0; else item.m_image = 1;
                InsertItem( item );
            } 
            item.m_itemId++;
        }
        f = wxFindNextFile();
    }
    SortItems( ListCompare, 0 );
}

int wxFileCtrl::FillList( wxStringList &list )
{
    long index = -1;
    int count = 0;
    wxString s;
    for (;;)
    {
        index = GetNextItem( index, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
        if (index == -1) break;
        wxListItem item;
        item.m_itemId = index;
        GetItem( item );
        wxFileData *fd = (wxFileData*)item.m_data;
        list.Add( fd->GetFullName() );
        index++;
        count++;
    }
    if (count == 0)
    {
        index = GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_FOCUSED );
        if (index == -1) return 0;
        wxListItem item;
        item.m_itemId = index;
        GetItem( item );
        wxFileData *fd = (wxFileData*)item.m_data;
        list.Add( fd->GetFullName() );
        count = 1;
    }
    return count;
}

void wxFileCtrl::DeleteFiles()
{
/*
  wxStringList list;
  int count = FillList( list );
  if (count > 0)
  { 
    wxString s = "Delete ";
    s += wxIntToString( count );
    s += " selected file";
    if (count > 1) s += "s";
    s += " or director";
    if (count > 1) s += "ies?"; else s+= "y?";
    if (wxYES == wxMessageBox( s, "Delete", wxYES_NO ))
      wxDeleteStatusDia( NULL, &list );
  };
*/
}

void wxFileCtrl::CopyFiles( char *WXUNUSED(dest) )
{
/*
  wxStringList list;
  int count = FillList( list );
  wxString s = dest;
  int ret = 0; // 0 = nix, 1 = copy, 2 = move
  wxCopyMoveDia( (wxFrame*)GetParent(), count, &ret, &s );
  if (ret == 1) 
    wxCopyStatusDia( NULL, s, &list );
*/
}

void wxFileCtrl::MoveFiles( char *WXUNUSED(dest) )
{
}

void wxFileCtrl::RenameFile()
{
}

void wxFileCtrl::MakeDir()
{
/*
  wxString s = wxGetTextFromUser( "Enter new directory name:", "Make directory" );
  if (s.IsNull()) return;
  if (s == "") return;
  if ((s == ".") || (s == ".."))
  {
    wxMessageBox( "This was obviously an invalid directory name.", "Go away." );
    return;
  };
  wxString dir;
  GetDir( dir );
  dir += "/";
  dir += s;
  if (wxFileExists( dir ))
  {
    wxMessageBox( "Filename exists already. Cannot create directoy.", "Make directory" );
    return;
  };
  wxMkdir( dir );
  Update();
*/
}

void wxFileCtrl::GoToParentDir()
{
    wxString s = m_dirName;
    int pos = s.Last( _T('/') );  
    if ((pos >= 0) && (s != _T("/")))
    {
        s.Remove( pos, s.Length()-pos );
        if (s.Length() == 0) s = _T("/");
        m_dirName = s;
        Update();
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

/*
void wxFileCtrl::OnDropFiles( int WXUNUSED(n), char **WXUNUSED(data), int WXUNUSED(x), int WXUNUSED(y) )
{
  wxString destDir;
  wxPoint pt( x, y );
  int flag = wxLIST_HITTEST_ONITEM;
  long hit = HitTest( pt, flag );
  if (hit > -1)
  {
    wxListItem li;
    li.m_itemId = hit;
    GetItem( li );
    wxFileData *fd = (wxFileData*)li.m_data;
    if (fd->IsDir()) fd->GetFullName( destDir );
  };
  if (destDir.IsNull()) destDir = m_dirName;
  int ret = 0; // 0 = nix, 1 = copy, 2 = move
  wxCopyMoveDia( (wxFrame*)GetParent(), n, &ret, &destDir );
  if (ret == 1)
  {  
     wxStringList slist;
     for (int i = 0; i < n; i++) slist.Add( data[i] );
     wxCopyStatusDia( NULL, destDir.GetData(), &slist );
     Update();
  };
};
*/

void wxFileCtrl::OnListDeleteItem( wxListEvent &event )
{
    wxFileData *fd = (wxFileData*)event.m_item.m_data;
    delete fd;
}

void wxFileCtrl::OnListKeyDown( wxListEvent &event )
{
    wxFileData *fd = (wxFileData*)event.m_item.m_data;
    if (fd->IsDir())
    {
        m_dirName = fd->GetFullName();
        Update();
        Refresh();
        return;
    }
    if (fd->IsExe())
    {
        wxExecute( fd->GetFullName() );
        return;
    }
}

void wxFileCtrl::OnListEndLabelEdit( wxListEvent &event )
{
    wxFileData *fd = (wxFileData*)event.m_item.m_data;
    wxString newName = event.m_item.m_text;
    if (fd->NewNameIsLegal( newName ))
    {
        if (fd->Rename( newName ))
        {
            Update();
        }
        else
        {
            wxString s = _("Could not rename file to ");
            s += newName;
            s += ".";
            wxMessageBox( s, _("File dialog"), wxOK );
        }
    }
    else
    {
        wxString s = "File name ";
        s += newName;
        s += " exists already or is invalid.\n";
        s += "Could not rename file.";
        wxMessageBox( s, _("File dialog"), wxOK );
    }
    return;
}

//-----------------------------------------------------------------------------
// wxFileDialog
//-----------------------------------------------------------------------------

/* **** */

#define  ID_LIST_CTRL     5010
#define  ID_LIST_MODE     5000
#define  ID_REPORT_MODE   5001
#define  ID_ICON_MODE     5002
#define  ID_UP_DIR        5005
#define  ID_PARENT_DIR    5006

IMPLEMENT_DYNAMIC_CLASS(wxFileDialog,wxDialog)

BEGIN_EVENT_TABLE(wxFileDialog,wxDialog)
        EVT_BUTTON(ID_LIST_MODE, wxFileDialog::OnList)
        EVT_BUTTON(ID_REPORT_MODE, wxFileDialog::OnReport)
        EVT_BUTTON(ID_ICON_MODE, wxFileDialog::OnIcon)
        EVT_BUTTON(ID_UP_DIR, wxFileDialog::OnUp)
        EVT_BUTTON(ID_PARENT_DIR, wxFileDialog::OnHome)
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
    
    buttonsizer->Add( new wxButton( this, ID_LIST_MODE, "List" ), 0, wxALL, 5 );
    buttonsizer->Add( new wxButton( this, ID_REPORT_MODE, "Report" ), 0, wxALL, 5 );
    buttonsizer->Add( new wxButton( this, ID_ICON_MODE, "Icon" ), 0, wxALL, 5 );
    buttonsizer->Add( 30, 5 );
    buttonsizer->Add( new wxButton( this, ID_UP_DIR, "Up" ), 0, wxALL, 5 );
    buttonsizer->Add( new wxButton( this, ID_PARENT_DIR, "Home" ), 0, wxALL, 5 );
    buttonsizer->Add( new wxButton( this, -1, "New..." ), 0, wxALL, 5 );
    mainsizer->Add( buttonsizer, 0, wxALL | wxALIGN_RIGHT, 5 );
    
    m_list = new wxFileCtrl( this, ID_LIST_CTRL, "/", wxDefaultPosition, wxSize(200,180), 
      wxLC_LIST | wxSUNKEN_BORDER | wxLC_SINGLE_SEL );
    mainsizer->Add( m_list, 1, wxEXPAND | wxALL, 10 );
    
    wxBoxSizer *textsizer = new wxBoxSizer( wxHORIZONTAL );
    m_text = new wxTextCtrl( this, -1, m_fileName );
    textsizer->Add( m_text, 1, wxCENTER|wxALL, 10 );
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
    
    dir += _T("/");
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
}

void wxFileDialog::OnReport( wxCommandEvent &WXUNUSED(event) )
{
    m_list->ChangeToReportMode();
}

void wxFileDialog::OnIcon( wxCommandEvent &WXUNUSED(event) )
{
    m_list->ChangeToIconMode();
}

void wxFileDialog::OnUp( wxCommandEvent &WXUNUSED(event) )
{
    m_list->GoToParentDir();
}

void wxFileDialog::OnHome( wxCommandEvent &WXUNUSED(event) )
{
    m_list->GoToHomeDir();
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

