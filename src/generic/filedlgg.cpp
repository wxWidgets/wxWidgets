//////////////////////////////////////////////////////////////////////////////
// Name:        filedlgg.cpp
// Purpose:     wxGenericFileDialog
// Author:      Robert Roebling
// Modified by:
// Created:     12/12/98
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "filedlgg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_FILEDLG

// NOTE : it probably also supports MAC, untested
#if !defined(__UNIX__) && !defined(__DOS__) && !defined(__WIN32__) && !defined(__OS2__)
#error wxGenericFileDialog currently only supports Unix, win32 and DOS
#endif

#include "wx/checkbox.h"
#include "wx/textctrl.h"
#include "wx/choice.h"
#include "wx/checkbox.h"
#include "wx/stattext.h"
#include "wx/debug.h"
#include "wx/log.h"
#include "wx/intl.h"
#include "wx/msgdlg.h"
#include "wx/sizer.h"
#include "wx/bmpbuttn.h"
#include "wx/tokenzr.h"
#include "wx/config.h"
#include "wx/imaglist.h"
#include "wx/dir.h"
#include "wx/artprov.h"
#include "wx/settings.h"
#include "wx/file.h"        // for wxS_IXXX constants only
#include "wx/filedlg.h"     // wxOPEN, wxSAVE...
#include "wx/generic/filedlgg.h"
#include "wx/generic/dirctrlg.h" // for wxFileIconsTable

#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif

#include <sys/types.h>
#include <sys/stat.h>

#ifdef __UNIX__
    #include <dirent.h>
    #include <pwd.h>
    #ifndef __VMS
    # include <grp.h>
    #endif
#endif

#ifdef __WINDOWS__
    #include "wx/msw/wrapwin.h"
    #include "wx/msw/mslu.h"
#endif

#ifdef __WATCOMC__
    #include <direct.h>
#endif

#include <time.h>
#if defined(__UNIX__) || defined(__DOS__)
#include <unistd.h>
#endif

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

static
int wxCALLBACK wxFileDataNameCompare( long data1, long data2, long data)
{
     wxFileData *fd1 = (wxFileData*)data1;
     wxFileData *fd2 = (wxFileData*)data2;
     if (fd1->GetFileName() == wxT("..")) return -data;
     if (fd2->GetFileName() == wxT("..")) return data;
     if (fd1->IsDir() && !fd2->IsDir()) return -data;
     if (fd2->IsDir() && !fd1->IsDir()) return data;
     return data*wxStrcmp( fd1->GetFileName(), fd2->GetFileName() );
}

static
int wxCALLBACK wxFileDataSizeCompare( long data1, long data2, long data)
{
     wxFileData *fd1 = (wxFileData*)data1;
     wxFileData *fd2 = (wxFileData*)data2;
     if (fd1->GetFileName() == wxT("..")) return -data;
     if (fd2->GetFileName() == wxT("..")) return data;
     if (fd1->IsDir() && !fd2->IsDir()) return -data;
     if (fd2->IsDir() && !fd1->IsDir()) return data;
     if (fd1->IsLink() && !fd2->IsLink()) return -data;
     if (fd2->IsLink() && !fd1->IsLink()) return data;
     return data*(fd1->GetSize() - fd2->GetSize());
}

static
int wxCALLBACK wxFileDataTypeCompare( long data1, long data2, long data)
{
     wxFileData *fd1 = (wxFileData*)data1;
     wxFileData *fd2 = (wxFileData*)data2;
     if (fd1->GetFileName() == wxT("..")) return -data;
     if (fd2->GetFileName() == wxT("..")) return data;
     if (fd1->IsDir() && !fd2->IsDir()) return -data;
     if (fd2->IsDir() && !fd1->IsDir()) return data;
     if (fd1->IsLink() && !fd2->IsLink()) return -data;
     if (fd2->IsLink() && !fd1->IsLink()) return data;
     return data*wxStrcmp( fd1->GetFileType(), fd2->GetFileType() );
}

static
int wxCALLBACK wxFileDataTimeCompare( long data1, long data2, long data)
{
     wxFileData *fd1 = (wxFileData*)data1;
     wxFileData *fd2 = (wxFileData*)data2;
     if (fd1->GetFileName() == wxT("..")) return -data;
     if (fd2->GetFileName() == wxT("..")) return data;
     if (fd1->IsDir() && !fd2->IsDir()) return -data;
     if (fd2->IsDir() && !fd1->IsDir()) return data;

     return fd1->GetDateTime().IsLaterThan(fd2->GetDateTime()) ? int(data) : -int(data);
}

#if defined(__UNIX__) && !defined(__OS2__)
#define IsTopMostDir(dir)   (dir == wxT("/"))
#endif

#if defined(__DOS__) || defined(__WINDOWS__) || defined (__OS2__)
#define IsTopMostDir(dir)   (dir.empty())
#endif

#if defined(__DOS__) || defined(__WINDOWS__) || defined(__OS2__)
// defined in src/generic/dirctrlg.cpp
extern bool wxIsDriveAvailable(const wxString& dirName);
#endif

// defined in src/generic/dirctrlg.cpp
extern size_t wxGetAvailableDrives(wxArrayString &paths, wxArrayString &names, wxArrayInt &icon_ids);

//-----------------------------------------------------------------------------
//  wxFileData
//-----------------------------------------------------------------------------

wxFileData::wxFileData( const wxString &filePath, const wxString &fileName, fileType type, int image_id )
{
    m_fileName = fileName;
    m_filePath = filePath;
    m_type = type;
    m_image = image_id;

    ReadData();
}

void wxFileData::Copy( const wxFileData& fileData )
{
    m_fileName = fileData.GetFileName();
    m_filePath = fileData.GetFilePath();
    m_size = fileData.GetSize();
    m_dateTime = fileData.GetDateTime();
    m_permissions = fileData.GetPermissions();
    m_type = fileData.GetType();
    m_image = GetImageId();
}

void wxFileData::ReadData()
{
    if (IsDrive())
    {
        m_size = 0;
        return;
    }

#if defined(__DOS__) || defined(__WINDOWS__) || defined(__OS2__)
    // c:\.. is a drive don't stat it
    if ((m_fileName == wxT("..")) && (m_filePath.length() <= 5))
    {
        m_type = is_drive;
        m_size = 0;
        return;
    }
#endif // __DOS__ || __WINDOWS__

    wxStructStat buff;

#if defined(__UNIX__) && (!defined( __OS2__ ) && !defined(__VMS))
    lstat( m_filePath.fn_str(), &buff );
    m_type |= S_ISLNK( buff.st_mode ) != 0 ? is_link : 0;
#else // no lstat()
    // only translate to file charset if we don't go by our
    // wxStat implementation
#ifndef wxNEED_WX_UNISTD_H
    wxStat( m_filePath.fn_str() , &buff );
#else
    wxStat( m_filePath, &buff );
#endif
#endif

    m_type |= (buff.st_mode & S_IFDIR) != 0 ? is_dir : 0;
    m_type |= (buff.st_mode & wxS_IXUSR) != 0 ? is_exe : 0;

    // try to get a better icon
    if (m_image == wxFileIconsTable::file)
    {
        if (m_fileName.Find(wxT('.'), true) != wxNOT_FOUND)
        {
            m_image = wxTheFileIconsTable->GetIconID( m_fileName.AfterLast(wxT('.')));
        } else if (IsExe())
        {
            m_image = wxFileIconsTable::executable;
        }
    }

    m_size = (long)buff.st_size;

    m_dateTime = buff.st_mtime;

#if defined(__UNIX__)
    m_permissions.Printf(_T("%c%c%c%c%c%c%c%c%c"),
                         buff.st_mode & wxS_IRUSR ? _T('r') : _T('-'),
                         buff.st_mode & wxS_IWUSR ? _T('w') : _T('-'),
                         buff.st_mode & wxS_IXUSR ? _T('x') : _T('-'),
                         buff.st_mode & wxS_IRGRP ? _T('r') : _T('-'),
                         buff.st_mode & wxS_IWGRP ? _T('w') : _T('-'),
                         buff.st_mode & wxS_IXGRP ? _T('x') : _T('-'),
                         buff.st_mode & wxS_IROTH ? _T('r') : _T('-'),
                         buff.st_mode & wxS_IWOTH ? _T('w') : _T('-'),
                         buff.st_mode & wxS_IXOTH ? _T('x') : _T('-'));
#elif defined(__WIN32__)
    DWORD attribs = GetFileAttributes(m_filePath);
    if (attribs != (DWORD)-1)
    {
        m_permissions.Printf(_T("%c%c%c%c"),
                             attribs & FILE_ATTRIBUTE_ARCHIVE  ? _T('A') : _T(' '),
                             attribs & FILE_ATTRIBUTE_READONLY ? _T('R') : _T(' '),
                             attribs & FILE_ATTRIBUTE_HIDDEN   ? _T('H') : _T(' '),
                             attribs & FILE_ATTRIBUTE_SYSTEM   ? _T('S') : _T(' '));
    }
#endif
}

wxString wxFileData::GetFileType() const
{
    if (IsDir())
        return _("<DIR>");
    else if (IsLink())
        return _("<LINK>");
    else if (IsDrive())
        return _("<DRIVE>");
   else if (m_fileName.Find(wxT('.'), true) != wxNOT_FOUND)
        return m_fileName.AfterLast(wxT('.'));

    return wxEmptyString;
}

wxString wxFileData::GetModificationTime() const
{
    // want time as 01:02 so they line up nicely, no %r in WIN32
    return m_dateTime.FormatDate() + wxT(" ") + m_dateTime.Format(wxT("%I:%M:%S %p"));
}

wxString wxFileData::GetHint() const
{
    wxString s = m_filePath;
    s += wxT("  ");

    if (IsDir())
        s += _("<DIR>");
    else if (IsLink())
        s += _("<LINK>");
    else if (IsDrive())
        s += _("<DRIVE>");
    else // plain file
        s += wxString::Format( _("%ld bytes"), m_size );

    s += wxT(' ');

    if ( !IsDrive() )
    {
        s << GetModificationTime()
          << wxT("  ")
          << m_permissions;
    }

    return s;
};

wxString wxFileData::GetEntry( fileListFieldType num ) const
{
    wxString s;
    switch ( num )
    {
        case FileList_Name:
            s = m_fileName;
            break;

        case FileList_Size:
            if (!IsDir() && !IsLink() && !IsDrive())
                s.Printf(_T("%ld"), m_size);
            break;

        case FileList_Type:
            s = GetFileType();
            break;

        case FileList_Time:
                if (!IsDrive())
                s = GetModificationTime();
            break;

#if defined(__UNIX__) || defined(__WIN32__)
        case FileList_Perm:
            s = m_permissions;
            break;
#endif // defined(__UNIX__) || defined(__WIN32__)

        default:
            wxFAIL_MSG( _T("unexpected field in wxFileData::GetEntry()") );
    }

    return s;
}

void wxFileData::SetNewName( const wxString &filePath, const wxString &fileName )
{
    m_fileName = fileName;
    m_filePath = filePath;
}

void wxFileData::MakeItem( wxListItem &item )
{
    item.m_text = m_fileName;
    item.ClearAttributes();
    if (IsExe())
        item.SetTextColour(*wxRED);
    if (IsDir())
        item.SetTextColour(*wxBLUE);

    item.m_image = m_image;

    if (IsLink())
    {
        wxColour dg = wxTheColourDatabase->Find( _T("MEDIUM GREY") );
        if ( dg.Ok() )
            item.SetTextColour(dg);
    }
    item.m_data = (long)this;
}

//-----------------------------------------------------------------------------
//  wxFileCtrl
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxFileCtrl,wxListCtrl)

BEGIN_EVENT_TABLE(wxFileCtrl,wxListCtrl)
    EVT_LIST_DELETE_ITEM(wxID_ANY, wxFileCtrl::OnListDeleteItem)
    EVT_LIST_DELETE_ALL_ITEMS(wxID_ANY, wxFileCtrl::OnListDeleteAllItems)
    EVT_LIST_END_LABEL_EDIT(wxID_ANY, wxFileCtrl::OnListEndLabelEdit)
    EVT_LIST_COL_CLICK(wxID_ANY, wxFileCtrl::OnListColClick)
END_EVENT_TABLE()


wxFileCtrl::wxFileCtrl()
{
    m_showHidden = false;
    m_sort_foward = 1;
    m_sort_field = wxFileData::FileList_Name;
}

wxFileCtrl::wxFileCtrl(wxWindow *win,
                       wxWindowID id,
                       const wxString& wild,
                       bool showHidden,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxValidator &validator,
                       const wxString &name)
          : wxListCtrl(win, id, pos, size, style, validator, name),
            m_wild(wild)
{
    wxImageList *imageList = wxTheFileIconsTable->GetSmallImageList();

    SetImageList( imageList, wxIMAGE_LIST_SMALL );

    m_showHidden = showHidden;

    m_sort_foward = 1;
    m_sort_field = wxFileData::FileList_Name;

    m_dirName = wxT("*");

    if (style & wxLC_REPORT)
        ChangeToReportMode();
}

void wxFileCtrl::ChangeToListMode()
{
    ClearAll();
    SetSingleStyle( wxLC_LIST );
    UpdateFiles();
}

void wxFileCtrl::ChangeToReportMode()
{
    ClearAll();
    SetSingleStyle( wxLC_REPORT );

    // do this since WIN32 does mm/dd/yy UNIX does mm/dd/yyyy
    // don't hardcode since mm/dd is dd/mm elsewhere
    int w, h;
    wxDateTime dt(22, wxDateTime::Dec, 2002, 22, 22, 22);
    wxString txt = dt.FormatDate() + wxT("22") + dt.Format(wxT("%I:%M:%S %p"));
    GetTextExtent(txt, &w, &h);

    InsertColumn( 0, _("Name"), wxLIST_FORMAT_LEFT, w );
    InsertColumn( 1, _("Size"), wxLIST_FORMAT_LEFT, w/2 );
    InsertColumn( 2, _("Type"), wxLIST_FORMAT_LEFT, w/2 );
    InsertColumn( 3, _("Modified"), wxLIST_FORMAT_LEFT, w );
#if defined(__UNIX__)
    GetTextExtent(wxT("Permissions 2"), &w, &h);
    InsertColumn( 4, _("Permissions"), wxLIST_FORMAT_LEFT, w );
#elif defined(__WIN32__)
    GetTextExtent(wxT("Attributes 2"), &w, &h);
    InsertColumn( 4, _("Attributes"), wxLIST_FORMAT_LEFT, w );
#endif

    UpdateFiles();
}

void wxFileCtrl::ChangeToSmallIconMode()
{
    ClearAll();
    SetSingleStyle( wxLC_SMALL_ICON );
    UpdateFiles();
}

void wxFileCtrl::ShowHidden( bool show )
{
    m_showHidden = show;
    UpdateFiles();
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
        for (int i = 1; i < wxFileData::FileList_Max; i++)
            SetItem( item.m_itemId, i, fd->GetEntry((wxFileData::fileListFieldType)i) );
    }
    else if ((my_style & wxLC_LIST) || (my_style & wxLC_SMALL_ICON))
    {
        ret = InsertItem( item );
    }
    return ret;
}

void wxFileCtrl::UpdateItem(const wxListItem &item)
{
    wxFileData *fd = (wxFileData*)GetItemData(item);
    wxCHECK_RET(fd, wxT("invalid filedata"));

    fd->ReadData();

    SetItemText(item, fd->GetFileName());
    SetItemImage(item, fd->GetImageId());

    if (GetWindowStyleFlag() & wxLC_REPORT)
    {
        for (int i = 1; i < wxFileData::FileList_Max; i++)
            SetItem( item.m_itemId, i, fd->GetEntry((wxFileData::fileListFieldType)i) );
    }
}

void wxFileCtrl::UpdateFiles()
{
    // don't do anything before ShowModal() call which sets m_dirName
    if ( m_dirName == wxT("*") )
        return;

    wxBusyCursor bcur; // this may take a while...

    DeleteAllItems();

    wxListItem item;
    item.m_itemId = 0;
    item.m_col = 0;

#if defined(__WINDOWS__) || defined(__DOS__) || defined(__WXMAC__) || defined(__OS2__)
    if ( IsTopMostDir(m_dirName) )
    {
        wxArrayString names, paths;
        wxArrayInt icons;
        size_t n, count = wxGetAvailableDrives(paths, names, icons);

        for (n=0; n<count; n++)
        {
            wxFileData *fd = new wxFileData(paths[n], names[n], wxFileData::is_drive, icons[n]);
            if (Add(fd, item) != -1)
                item.m_itemId++;
            else
                delete fd;
        }
    }
    else
#endif // defined(__DOS__) || defined(__WINDOWS__)
    {
        // Real directory...
        if ( !IsTopMostDir(m_dirName) )
        {
            wxString p(wxPathOnly(m_dirName));
#if defined(__UNIX__) && !defined(__OS2__)
            if (p.empty()) p = wxT("/");
#endif // __UNIX__
            wxFileData *fd = new wxFileData(p, wxT(".."), wxFileData::is_dir, wxFileIconsTable::folder);
            if (Add(fd, item) != -1)
            item.m_itemId++;
            else
                delete fd;
        }

        wxString dirname(m_dirName);
#if defined(__DOS__) || defined(__WINDOWS__) || defined(__OS2__)
        if (dirname.length() == 2 && dirname[1u] == wxT(':'))
            dirname << wxT('\\');
#endif // defined(__DOS__) || defined(__WINDOWS__) || defined(__OS2__)
        wxDir dir(dirname);

        if ( dir.IsOpened() )
        {
            wxString dirPrefix(dirname);
            if (dirPrefix.Last() != wxFILE_SEP_PATH)
                dirPrefix += wxFILE_SEP_PATH;

            int hiddenFlag = m_showHidden ? wxDIR_HIDDEN : 0;

            bool cont;
            wxString f;

            // Get the directories first (not matched against wildcards):
            cont = dir.GetFirst(&f, wxEmptyString, wxDIR_DIRS | hiddenFlag);
            while (cont)
            {
                wxFileData *fd = new wxFileData(dirPrefix + f, f, wxFileData::is_dir, wxFileIconsTable::folder);
                if (Add(fd, item) != -1)
                item.m_itemId++;
                else
                    delete fd;

                cont = dir.GetNext(&f);
            }

            // Tokenize the wildcard string, so we can handle more than 1
            // search pattern in a wildcard.
            wxStringTokenizer tokenWild(m_wild, wxT(";"));
            while ( tokenWild.HasMoreTokens() )
            {
                cont = dir.GetFirst(&f, tokenWild.GetNextToken(),
                                        wxDIR_FILES | hiddenFlag);
                while (cont)
                {
                    wxFileData *fd = new wxFileData(dirPrefix + f, f, wxFileData::is_file, wxFileIconsTable::file);
                    if (Add(fd, item) != -1)
                    item.m_itemId++;
                    else
                        delete fd;

                    cont = dir.GetNext(&f);
                }
            }
        }
    }

    SortItems(m_sort_field, m_sort_foward);
}

void wxFileCtrl::SetWild( const wxString &wild )
{
    if (wild.Find(wxT('|')) != wxNOT_FOUND)
        return;

    m_wild = wild;
    UpdateFiles();
}

void wxFileCtrl::MakeDir()
{
    wxString new_name( _("NewName") );
    wxString path( m_dirName );
    path += wxFILE_SEP_PATH;
    path += new_name;
    if (wxFileExists(path))
    {
        // try NewName0, NewName1 etc.
        int i = 0;
        do {
            new_name = _("NewName");
            wxString num;
            num.Printf( wxT("%d"), i );
            new_name += num;

            path = m_dirName;
            path += wxFILE_SEP_PATH;
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

    wxFileData *fd = new wxFileData( path, new_name, wxFileData::is_dir, wxFileIconsTable::folder );
    wxListItem item;
    item.m_itemId = 0;
    item.m_col = 0;
    long id = Add( fd, item );

    if (id != -1)
    {
        SortItems(m_sort_field, m_sort_foward);
        id = FindItem( 0, (long)fd );
        EnsureVisible( id );
        EditLabel( id );
    }
    else
        delete fd;
}

void wxFileCtrl::GoToParentDir()
{
    if (!IsTopMostDir(m_dirName))
    {
        size_t len = m_dirName.Len();
        if (wxEndsWithPathSeparator(m_dirName))
            m_dirName.Remove( len-1, 1 );
        wxString fname( wxFileNameFromPath(m_dirName) );
        m_dirName = wxPathOnly( m_dirName );
#if defined(__DOS__) || defined(__WINDOWS__) || defined(__OS2__)
        if (!m_dirName.empty())
        {
            if (m_dirName.Last() == wxT('.'))
                m_dirName = wxEmptyString;
        }
#elif defined(__UNIX__)
        if (m_dirName.empty())
            m_dirName = wxT("/");
#endif
        UpdateFiles();
        long id = FindItem( 0, fname );
        if (id != wxNOT_FOUND)
        {
            SetItemState( id, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
            EnsureVisible( id );
        }
    }
}

void wxFileCtrl::GoToHomeDir()
{
    wxString s = wxGetUserHome( wxString() );
    GoToDir(s);
}

void wxFileCtrl::GoToDir( const wxString &dir )
{
    if (!wxDirExists(dir)) return;

    m_dirName = dir;
    UpdateFiles();
    SetItemState( 0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
    EnsureVisible( 0 );
}

void wxFileCtrl::FreeItemData(wxListItem& item)
{
    if ( item.m_data )
    {
        wxFileData *fd = (wxFileData*)item.m_data;
        delete fd;

        item.m_data = 0;
    }
}

void wxFileCtrl::OnListDeleteItem( wxListEvent &event )
{
    FreeItemData(event.m_item);
}

void wxFileCtrl::OnListDeleteAllItems( wxListEvent & WXUNUSED(event) )
{
    FreeAllItemsData();
}

void wxFileCtrl::FreeAllItemsData()
{
    wxListItem item;
    item.m_mask = wxLIST_MASK_DATA;

    item.m_itemId = GetNextItem( -1, wxLIST_NEXT_ALL );
    while ( item.m_itemId != -1 )
    {
        GetItem( item );
        FreeItemData(item);
        item.m_itemId = GetNextItem( item.m_itemId, wxLIST_NEXT_ALL );
    }
}

void wxFileCtrl::OnListEndLabelEdit( wxListEvent &event )
{
    wxFileData *fd = (wxFileData*)event.m_item.m_data;
    wxASSERT( fd );

    if ((event.GetLabel().empty()) ||
        (event.GetLabel() == _(".")) ||
        (event.GetLabel() == _("..")) ||
        (event.GetLabel().First( wxFILE_SEP_PATH ) != wxNOT_FOUND))
    {
        wxMessageDialog dialog(this, _("Illegal directory name."), _("Error"), wxOK | wxICON_ERROR );
        dialog.ShowModal();
        event.Veto();
        return;
    }

    wxString new_name( wxPathOnly( fd->GetFilePath() ) );
    new_name += wxFILE_SEP_PATH;
    new_name += event.GetLabel();

    wxLogNull log;

    if (wxFileExists(new_name))
    {
        wxMessageDialog dialog(this, _("File name exists already."), _("Error"), wxOK | wxICON_ERROR );
        dialog.ShowModal();
        event.Veto();
    }

    if (wxRenameFile(fd->GetFilePath(),new_name))
    {
        fd->SetNewName( new_name, event.GetLabel() );
        SetItemState( event.GetItem(), wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
        UpdateItem( event.GetItem() );
        EnsureVisible( event.GetItem() );
    }
    else
    {
        wxMessageDialog dialog(this, _("Operation not permitted."), _("Error"), wxOK | wxICON_ERROR );
        dialog.ShowModal();
        event.Veto();
    }
}

void wxFileCtrl::OnListColClick( wxListEvent &event )
{
    int col = event.GetColumn();

    switch (col)
    {
        case wxFileData::FileList_Name :
        case wxFileData::FileList_Size :
        case wxFileData::FileList_Type :
        case wxFileData::FileList_Time : break;
        default : return;
    }

    if ((wxFileData::fileListFieldType)col == m_sort_field)
        m_sort_foward = !m_sort_foward;
    else
        m_sort_field = (wxFileData::fileListFieldType)col;

    SortItems(m_sort_field, m_sort_foward);
}

void wxFileCtrl::SortItems(wxFileData::fileListFieldType field, bool foward)
{
    m_sort_field = field;
    m_sort_foward = foward;
    long sort_dir = foward ? 1 : -1;

    switch (m_sort_field)
    {
        case wxFileData::FileList_Name :
        {
            wxListCtrl::SortItems((wxListCtrlCompare)wxFileDataNameCompare, sort_dir);
            break;
        }
        case wxFileData::FileList_Size :
        {
             wxListCtrl::SortItems((wxListCtrlCompare)wxFileDataSizeCompare, sort_dir);
            break;
        }
        case wxFileData::FileList_Type :
        {
             wxListCtrl::SortItems((wxListCtrlCompare)wxFileDataTypeCompare, sort_dir);
             break;
        }
        case wxFileData::FileList_Time :
        {
             wxListCtrl::SortItems((wxListCtrlCompare)wxFileDataTimeCompare, sort_dir);
             break;
        }
        default : break;
    }
}

wxFileCtrl::~wxFileCtrl()
{
    // Normally the data are freed via an EVT_LIST_DELETE_ALL_ITEMS event and
    // wxFileCtrl::OnListDeleteAllItems. But if the event is generated after
    // the destruction of the wxFileCtrl we need to free any data here:
    FreeAllItemsData();
}

//-----------------------------------------------------------------------------
// wxGenericFileDialog
//-----------------------------------------------------------------------------

#define  ID_LIST_MODE     (wxID_FILEDLGG    )
#define  ID_REPORT_MODE   (wxID_FILEDLGG + 1)
#define  ID_UP_DIR        (wxID_FILEDLGG + 5)
#define  ID_PARENT_DIR    (wxID_FILEDLGG + 6)
#define  ID_NEW_DIR       (wxID_FILEDLGG + 7)
#define  ID_CHOICE        (wxID_FILEDLGG + 8)
#define  ID_TEXT          (wxID_FILEDLGG + 9)
#define  ID_LIST_CTRL     (wxID_FILEDLGG + 10)
#define  ID_ACTIVATED     (wxID_FILEDLGG + 11)
#define  ID_CHECK         (wxID_FILEDLGG + 12)

IMPLEMENT_DYNAMIC_CLASS(wxGenericFileDialog, wxFileDialogBase)

BEGIN_EVENT_TABLE(wxGenericFileDialog,wxDialog)
        EVT_BUTTON(ID_LIST_MODE, wxGenericFileDialog::OnList)
        EVT_BUTTON(ID_REPORT_MODE, wxGenericFileDialog::OnReport)
        EVT_BUTTON(ID_UP_DIR, wxGenericFileDialog::OnUp)
        EVT_BUTTON(ID_PARENT_DIR, wxGenericFileDialog::OnHome)
        EVT_BUTTON(ID_NEW_DIR, wxGenericFileDialog::OnNew)
        EVT_BUTTON(wxID_OK, wxGenericFileDialog::OnListOk)
        EVT_LIST_ITEM_SELECTED(ID_LIST_CTRL, wxGenericFileDialog::OnSelected)
        EVT_LIST_ITEM_ACTIVATED(ID_LIST_CTRL, wxGenericFileDialog::OnActivated)
        EVT_CHOICE(ID_CHOICE,wxGenericFileDialog::OnChoiceFilter)
        EVT_TEXT_ENTER(ID_TEXT,wxGenericFileDialog::OnTextEnter)
        EVT_TEXT(ID_TEXT,wxGenericFileDialog::OnTextChange)
        EVT_CHECKBOX(ID_CHECK,wxGenericFileDialog::OnCheck)
END_EVENT_TABLE()

long wxGenericFileDialog::ms_lastViewStyle = wxLC_LIST;
bool wxGenericFileDialog::ms_lastShowHidden = false;

wxGenericFileDialog::wxGenericFileDialog(wxWindow *parent,
                           const wxString& message,
                           const wxString& defaultDir,
                           const wxString& defaultFile,
                           const wxString& wildCard,
                           long style,
                           const wxPoint& pos,
                           bool bypassGenericImpl )
                    :wxFileDialogBase(parent, message, defaultDir, defaultFile, wildCard, style, pos)
{
    m_bypassGenericImpl = bypassGenericImpl;

    if (!m_bypassGenericImpl)
        Create( parent, message, defaultDir, defaultFile, wildCard, style, pos );
}

bool wxGenericFileDialog::Create( wxWindow *parent,
                                  const wxString& message,
                                  const wxString& WXUNUSED(defaultDir),
                                  const wxString& defaultFile,
                                  const wxString& wildCard,
                                  long WXUNUSED(style),
                                  const wxPoint& pos )
{
    if (!wxDialog::Create( parent, wxID_ANY, message, pos, wxDefaultSize,
                      wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER ))
    {
        return false;
    }

    if (wxConfig::Get(false))
    {
        wxConfig::Get()->Read(wxT("/wxWindows/wxFileDialog/ViewStyle"),
                              &ms_lastViewStyle);
        wxConfig::Get()->Read(wxT("/wxWindows/wxFileDialog/ShowHidden"),
                              &ms_lastShowHidden);
    }

    if (m_dialogStyle == 0)
        m_dialogStyle = wxOPEN;
    if ((m_dialogStyle & wxMULTIPLE ) && !(m_dialogStyle & wxOPEN))
        m_dialogStyle |= wxOPEN;

    if ((m_dir.empty()) || (m_dir == wxT(".")))
    {
        m_dir = wxGetCwd();
    }

    size_t len = m_dir.Len();
    if ((len > 1) && (wxEndsWithPathSeparator(m_dir)))
        m_dir.Remove( len-1, 1 );

    m_path = m_dir;
    m_path += wxFILE_SEP_PATH;
    m_path += defaultFile;
    m_filterExtension = wxEmptyString;

    // layout

    bool is_pda = (wxSystemSettings::GetScreenType() <= wxSYS_SCREEN_PDA);

    wxBoxSizer *mainsizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *buttonsizer = new wxBoxSizer( wxHORIZONTAL );

    wxBitmapButton *but;

    but = new wxBitmapButton(this, ID_LIST_MODE,
                             wxArtProvider::GetBitmap(wxART_LIST_VIEW, wxART_BUTTON));
#if wxUSE_TOOLTIPS
    but->SetToolTip( _("View files as a list view") );
#endif
    buttonsizer->Add( but, 0, wxALL, 5 );

    but = new wxBitmapButton(this, ID_REPORT_MODE,
                             wxArtProvider::GetBitmap(wxART_REPORT_VIEW, wxART_BUTTON));
#if wxUSE_TOOLTIPS
    but->SetToolTip( _("View files as a detailed view") );
#endif
    buttonsizer->Add( but, 0, wxALL, 5 );

    buttonsizer->Add( 30, 5, 1 );

    m_upDirButton = new wxBitmapButton(this, ID_UP_DIR,
                           wxArtProvider::GetBitmap(wxART_GO_DIR_UP, wxART_BUTTON));
#if wxUSE_TOOLTIPS
    m_upDirButton->SetToolTip( _("Go to parent directory") );
#endif
    buttonsizer->Add( m_upDirButton, 0, wxALL, 5 );

#ifndef __DOS__ // VS: Home directory is meaningless in MS-DOS...
    but = new wxBitmapButton(this, ID_PARENT_DIR,
                             wxArtProvider::GetBitmap(wxART_GO_HOME, wxART_BUTTON));
#if wxUSE_TOOLTIPS
    but->SetToolTip( _("Go to home directory") );
#endif
    buttonsizer->Add( but, 0, wxALL, 5);

    buttonsizer->Add( 20, 20 );
#endif //!__DOS__

    m_newDirButton = new wxBitmapButton(this, ID_NEW_DIR,
                           wxArtProvider::GetBitmap(wxART_NEW_DIR, wxART_BUTTON));
#if wxUSE_TOOLTIPS
    m_newDirButton->SetToolTip( _("Create new directory") );
#endif
    buttonsizer->Add( m_newDirButton, 0, wxALL, 5 );

    if (is_pda)
        mainsizer->Add( buttonsizer, 0, wxALL | wxEXPAND, 0 );
    else
        mainsizer->Add( buttonsizer, 0, wxALL | wxEXPAND, 5 );

    wxBoxSizer *staticsizer = new wxBoxSizer( wxHORIZONTAL );
    if (is_pda)
        staticsizer->Add( new wxStaticText( this, wxID_ANY, _("Current directory:") ), 0, wxRIGHT, 10 );
    m_static = new wxStaticText( this, wxID_ANY, m_dir );
    staticsizer->Add( m_static, 1 );
    mainsizer->Add( staticsizer, 0, wxEXPAND | wxLEFT|wxRIGHT|wxBOTTOM, 10 );

    long style2 = ms_lastViewStyle | wxSUNKEN_BORDER;
    if ( !(m_dialogStyle & wxMULTIPLE) )
        style2 |= wxLC_SINGLE_SEL;

    m_list = new wxFileCtrl( this, ID_LIST_CTRL,
                             wxEmptyString, ms_lastShowHidden,
                             wxDefaultPosition, wxSize(540,200),
                             style2);

    if (is_pda)
    {
        // PDAs have a different screen layout
        mainsizer->Add( m_list, 1, wxEXPAND | wxLEFT|wxRIGHT, 5 );

        wxBoxSizer *textsizer = new wxBoxSizer( wxHORIZONTAL );
        m_text = new wxTextCtrl( this, ID_TEXT, m_fileName, wxDefaultPosition, wxDefaultSize, wxPROCESS_ENTER );
        textsizer->Add( m_text, 1, wxCENTER | wxALL, 5 );
        mainsizer->Add( textsizer, 0, wxEXPAND );

        m_check = NULL;
        m_choice = new wxChoice( this, ID_CHOICE );
        textsizer->Add( m_choice, 1, wxCENTER|wxALL, 5 );

        buttonsizer = new wxBoxSizer( wxHORIZONTAL );
        buttonsizer->Add( new wxButton( this, wxID_OK ), 0, wxCENTER | wxALL, 5 );
        buttonsizer->Add( new wxButton( this, wxID_CANCEL ), 0, wxCENTER | wxALL, 5 );
        mainsizer->Add( buttonsizer, 0, wxALIGN_RIGHT );
    }
    else
    {
        mainsizer->Add( m_list, 1, wxEXPAND | wxLEFT|wxRIGHT, 10 );

        wxBoxSizer *textsizer = new wxBoxSizer( wxHORIZONTAL );
        m_text = new wxTextCtrl( this, ID_TEXT, m_fileName, wxDefaultPosition, wxDefaultSize, wxPROCESS_ENTER );
        textsizer->Add( m_text, 1, wxCENTER | wxLEFT|wxRIGHT|wxTOP, 10 );
        textsizer->Add( new wxButton( this, wxID_OK ), 0, wxCENTER | wxLEFT|wxRIGHT|wxTOP, 10 );
        mainsizer->Add( textsizer, 0, wxEXPAND );

        wxBoxSizer *choicesizer = new wxBoxSizer( wxHORIZONTAL );
        m_choice = new wxChoice( this, ID_CHOICE );
        choicesizer->Add( m_choice, 1, wxCENTER|wxALL, 10 );
        m_check = new wxCheckBox( this, ID_CHECK, _("Show hidden files") );
        m_check->SetValue( ms_lastShowHidden );
        choicesizer->Add( m_check, 0, wxCENTER|wxALL, 10 );
        choicesizer->Add( new wxButton( this, wxID_CANCEL ), 0, wxCENTER | wxALL, 10 );
        mainsizer->Add( choicesizer, 0, wxEXPAND );
    }

    SetWildcard(wildCard);

    SetAutoLayout( true );
    SetSizer( mainsizer );

    mainsizer->Fit( this );
    mainsizer->SetSizeHints( this );

    Centre( wxBOTH );

    m_text->SetFocus();

    return true;
}

wxGenericFileDialog::~wxGenericFileDialog()
{
    if (!m_bypassGenericImpl)
    {
        if (wxConfig::Get(false))
        {
            wxConfig::Get()->Write(wxT("/wxWindows/wxFileDialog/ViewStyle"),
                                   ms_lastViewStyle);
            wxConfig::Get()->Write(wxT("/wxWindows/wxFileDialog/ShowHidden"),
                                   ms_lastShowHidden);
        }

        const int count = m_choice->GetCount();
        for ( int i = 0; i < count; i++ )
        {
            delete (wxString *)m_choice->GetClientData(i);
        }
    }
}

int wxGenericFileDialog::ShowModal()
{
    m_list->GoToDir(m_dir);
    UpdateControls();
    m_text->SetValue(m_fileName);

    return wxDialog::ShowModal();
}

bool wxGenericFileDialog::Show( bool show )
{
    if (show)
    {
        m_list->GoToDir(m_dir);
        UpdateControls();
        m_text->SetValue(m_fileName);
    }

    return wxDialog::Show( show );
}

void wxGenericFileDialog::DoSetFilterIndex(int filterindex)
{
    wxString *str = (wxString*) m_choice->GetClientData( filterindex );
    m_list->SetWild( *str );
    m_filterIndex = filterindex;
    if ( str->Left(2) == wxT("*.") )
    {
        m_filterExtension = str->Mid(1);
        if (m_filterExtension == _T(".*"))
            m_filterExtension.clear();
    }
    else
    {
        m_filterExtension.clear();
    }
}

void wxGenericFileDialog::SetWildcard(const wxString& wildCard)
{
    wxFileDialogBase::SetWildcard(wildCard);

    wxArrayString wildDescriptions, wildFilters;
    const size_t count = wxParseCommonDialogsFilter(m_wildCard,
                                                    wildDescriptions,
                                                    wildFilters);
    wxCHECK_RET( count, wxT("wxFileDialog: bad wildcard string") );

    m_choice->Clear();
    for ( size_t n = 0; n < count; n++ )
    {
        m_choice->Append( wildDescriptions[n], new wxString( wildFilters[n] ) );
    }

    SetFilterIndex( 0 );
}

void wxGenericFileDialog::SetFilterIndex( int filterindex )
{
    m_choice->SetSelection( filterindex );

    DoSetFilterIndex(filterindex);
}

void wxGenericFileDialog::OnChoiceFilter( wxCommandEvent &event )
{
    DoSetFilterIndex((int)event.GetInt());
}

void wxGenericFileDialog::OnCheck( wxCommandEvent &event )
{
    m_list->ShowHidden( (ms_lastShowHidden = event.GetInt() != 0) );
}

void wxGenericFileDialog::OnActivated( wxListEvent &event )
{
    HandleAction( event.m_item.m_text );
}

void wxGenericFileDialog::OnTextEnter( wxCommandEvent &WXUNUSED(event) )
{
    wxCommandEvent cevent(wxEVT_COMMAND_BUTTON_CLICKED, wxID_OK);
    cevent.SetEventObject( this );
    GetEventHandler()->ProcessEvent( cevent );
}

static bool ignoreChanges = false;

void wxGenericFileDialog::OnTextChange( wxCommandEvent &WXUNUSED(event) )
{
    if (!ignoreChanges)
    {
        // Clear selections.  Otherwise when the user types in a value they may
        // not get the file whose name they typed.
        if (m_list->GetSelectedItemCount() > 0)
        {
            long item = m_list->GetNextItem(-1, wxLIST_NEXT_ALL,
                wxLIST_STATE_SELECTED);
            while ( item != -1 )
            {
                m_list->SetItemState(item,0, wxLIST_STATE_SELECTED);
                item = m_list->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
            }
        }
    }
}

void wxGenericFileDialog::OnSelected( wxListEvent &event )
{
    wxString filename( event.m_item.m_text );
    if (filename == wxT("..")) return;

    wxString dir = m_list->GetDir();
    if (!IsTopMostDir(dir))
        dir += wxFILE_SEP_PATH;
    dir += filename;
    if (wxDirExists(dir)) return;

    ignoreChanges = true;
    m_text->SetValue( filename );
    ignoreChanges = false;
}

void wxGenericFileDialog::HandleAction( const wxString &fn )
{
    wxString filename( fn );
    wxString dir = m_list->GetDir();
    if (filename.empty()) return;
    if (filename == wxT(".")) return;

    // "some/place/" means they want to chdir not try to load "place"
    bool want_dir = filename.Last() == wxFILE_SEP_PATH;
    if (want_dir)
        filename = filename.RemoveLast();

    if (filename == wxT(".."))
    {
        m_list->GoToParentDir();
        m_list->SetFocus();
        UpdateControls();
        return;
    }

#ifdef __UNIX__
    if (filename == wxT("~"))
    {
        m_list->GoToHomeDir();
        m_list->SetFocus();
        UpdateControls();
        return;
    }

    if (filename.BeforeFirst(wxT('/')) == wxT("~"))
    {
        filename = wxString(wxGetUserHome()) + filename.Remove(0, 1);
    }
#endif // __UNIX__

    if ((filename.Find(wxT('*')) != wxNOT_FOUND) ||
        (filename.Find(wxT('?')) != wxNOT_FOUND))
    {
        if (filename.Find(wxFILE_SEP_PATH) != wxNOT_FOUND)
        {
            wxMessageBox(_("Illegal file specification."), _("Error"), wxOK | wxICON_ERROR );
            return;
        }
        m_list->SetWild( filename );
        return;
    }

    if (!IsTopMostDir(dir))
        dir += wxFILE_SEP_PATH;
    if (!wxIsAbsolutePath(filename))
    {
        dir += filename;
        filename = dir;
    }

    if (wxDirExists(filename))
    {
        m_list->GoToDir( filename );
        UpdateControls();
        return;
    }

    // they really wanted a dir, but it doesn't exist
    if (want_dir)
    {
        wxMessageBox(_("Directory doesn't exist."), _("Error"),
                     wxOK | wxICON_ERROR );
        return;
    }

    // append the default extension to the filename if it doesn't have any
    //
    // VZ: the logic of testing for !wxFileExists() only for the open file
    //     dialog is not entirely clear to me, why don't we allow saving to a
    //     file without extension as well?
    if ( !(m_dialogStyle & wxOPEN) || !wxFileExists(filename) )
    {
        filename = AppendExtension(filename, m_filterExtension);
    }

    // check that the file [doesn't] exist if necessary
    if ( (m_dialogStyle & wxSAVE) &&
            (m_dialogStyle & wxOVERWRITE_PROMPT) &&
                wxFileExists( filename ) )
    {
        wxString msg;
        msg.Printf( _("File '%s' already exists, do you really want to overwrite it?"), filename.c_str() );

        if (wxMessageBox(msg, _("Confirm"), wxYES_NO) != wxYES)
            return;
    }
    else if ( (m_dialogStyle & wxOPEN) &&
                (m_dialogStyle & wxFILE_MUST_EXIST) &&
                    !wxFileExists(filename) )
    {
        wxMessageBox(_("Please choose an existing file."), _("Error"),
                     wxOK | wxICON_ERROR );
    }

    SetPath( filename );

    // change to the directory where the user went if asked
    if ( m_dialogStyle & wxCHANGE_DIR )
    {
        wxString cwd;
        wxSplitPath(filename, &cwd, NULL, NULL);

        if ( cwd != wxGetCwd() )
        {
            wxSetWorkingDirectory(cwd);
        }
    }

    wxCommandEvent event;
    wxDialog::OnOK(event);
}

void wxGenericFileDialog::OnListOk( wxCommandEvent &WXUNUSED(event) )
{
    HandleAction( m_text->GetValue() );
}

void wxGenericFileDialog::OnList( wxCommandEvent &WXUNUSED(event) )
{
    m_list->ChangeToListMode();
    ms_lastViewStyle = wxLC_LIST;
    m_list->SetFocus();
}

void wxGenericFileDialog::OnReport( wxCommandEvent &WXUNUSED(event) )
{
    m_list->ChangeToReportMode();
    ms_lastViewStyle = wxLC_REPORT;
    m_list->SetFocus();
}

void wxGenericFileDialog::OnUp( wxCommandEvent &WXUNUSED(event) )
{
    m_list->GoToParentDir();
    m_list->SetFocus();
    UpdateControls();
}

void wxGenericFileDialog::OnHome( wxCommandEvent &WXUNUSED(event) )
{
    m_list->GoToHomeDir();
    m_list->SetFocus();
    UpdateControls();
}

void wxGenericFileDialog::OnNew( wxCommandEvent &WXUNUSED(event) )
{
    m_list->MakeDir();
}

void wxGenericFileDialog::SetPath( const wxString& path )
{
    // not only set the full path but also update filename and dir
    m_path = path;
    if ( !path.empty() )
    {
        wxString ext;
        wxSplitPath(path, &m_dir, &m_fileName, &ext);
        if (!ext.empty())
        {
            m_fileName += wxT(".");
            m_fileName += ext;
        }
    }
}

void wxGenericFileDialog::GetPaths( wxArrayString& paths ) const
{
    paths.Empty();
    if (m_list->GetSelectedItemCount() == 0)
    {
        paths.Add( GetPath() );
        return;
    }

    paths.Alloc( m_list->GetSelectedItemCount() );

    wxString dir = m_list->GetDir();
#ifdef __UNIX__
    if (dir != wxT("/"))
#endif
        dir += wxFILE_SEP_PATH;

    wxListItem item;
    item.m_mask = wxLIST_MASK_TEXT;

    item.m_itemId = m_list->GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
    while ( item.m_itemId != -1 )
    {
        m_list->GetItem( item );
        paths.Add( dir + item.m_text );
        item.m_itemId = m_list->GetNextItem( item.m_itemId,
            wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
    }
}

void wxGenericFileDialog::GetFilenames(wxArrayString& files) const
{
    files.Empty();
    if (m_list->GetSelectedItemCount() == 0)
    {
        files.Add( GetFilename() );
        return;
    }
    files.Alloc( m_list->GetSelectedItemCount() );

    wxListItem item;
    item.m_mask = wxLIST_MASK_TEXT;

    item.m_itemId = m_list->GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
    while ( item.m_itemId != -1 )
    {
        m_list->GetItem( item );
        files.Add( item.m_text );
        item.m_itemId = m_list->GetNextItem( item.m_itemId,
            wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
    }
}

void wxGenericFileDialog::UpdateControls()
{
    wxString dir = m_list->GetDir();
    m_static->SetLabel(dir);

    bool enable = !IsTopMostDir(dir);
    m_upDirButton->Enable(enable);

#if defined(__DOS__) || defined(__WINDOWS__) || defined(__OS2__)
    m_newDirButton->Enable(enable);
#endif // defined(__DOS__) || defined(__WINDOWS__) || defined(__OS2__)
}

#ifdef USE_GENERIC_FILEDIALOG

IMPLEMENT_DYNAMIC_CLASS(wxFileDialog, wxGenericFileDialog);

#endif // USE_GENERIC_FILEDIALOG

#endif // wxUSE_FILEDLG

