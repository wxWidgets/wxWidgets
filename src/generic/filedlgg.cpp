/////////////////////////////////////////////////////////////////////////////
// Name:        filedlgg.cpp
// Purpose:     wxFileDialog
// Author:      Robert Roebling
// Modified by:
// Created:     12/12/98
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
#pragma implementation "filedlgg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_FILEDLG

#if !defined(__UNIX__) && !defined(__DOS__) && !defined(__WIN32__)
#error wxFileDialog currently only supports Unix, win32 and DOS
#endif

#include "wx/checkbox.h"
#include "wx/textctrl.h"
#include "wx/choice.h"
#include "wx/checkbox.h"
#include "wx/stattext.h"
#include "wx/filedlg.h"
#include "wx/debug.h"
#include "wx/log.h"
#include "wx/intl.h"
#include "wx/listctrl.h"
#include "wx/msgdlg.h"
#include "wx/sizer.h"
#include "wx/bmpbuttn.h"
#include "wx/tokenzr.h"
#include "wx/mimetype.h"
#include "wx/image.h"
#include "wx/module.h"
#include "wx/config.h"
#include "wx/imaglist.h"
#include "wx/dir.h"
#include "wx/artprov.h"

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

#ifdef __WATCOMC__
    #include <direct.h>
#endif

#include <time.h>
#if defined(__UNIX__) || defined(__DOS__)
#include <unistd.h>
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// the list ctrl fields in report view
enum FileListField
{
    FileList_Name,
    FileList_Type,
    FileList_Date,
    FileList_Time,
#ifdef __UNIX__
    FileList_Perm,
#endif // __UNIX__
    FileList_Max
};

//-----------------------------------------------------------------------------
//  wxFileData
//-----------------------------------------------------------------------------

class wxFileData
{
public:
    wxFileData( const wxString &name, const wxString &fname );
    wxString GetName() const;
    wxString GetFullName() const;
    wxString GetHint() const;
    wxString GetEntry( FileListField num ) const;

    bool IsDir() const { return m_isDir; }
    bool IsLink() const { return m_isLink; }
    bool IsExe() const { return m_isExe; }
    long GetSize() const { return m_size; }

    void MakeItem( wxListItem &item );
    void SetNewName( const wxString &name, const wxString &fname );

private:
    wxString m_name;
    wxString m_fileName;
    long     m_size;
    int      m_hour;
    int      m_minute;
    int      m_year;
    int      m_month;
    int      m_day;
    wxString m_permissions;
    bool     m_isDir;
    bool     m_isLink;
    bool     m_isExe;
};

//-----------------------------------------------------------------------------
//  wxFileCtrl
//-----------------------------------------------------------------------------

class wxFileCtrl : public wxListCtrl
{
public:
    wxFileCtrl();
    wxFileCtrl( wxWindow *win,
                wxStaticText *labelDir,
                wxWindowID id,
                const wxString &wild,
                bool showHidden,
                const wxPoint &pos = wxDefaultPosition,
                const wxSize &size = wxDefaultSize,
                long style = wxLC_LIST,
                const wxValidator &validator = wxDefaultValidator,
                const wxString &name = wxT("filelist") );
    virtual ~wxFileCtrl();

    void ChangeToListMode();
    void ChangeToReportMode();
    void ChangeToIconMode();
    void ShowHidden( bool show = TRUE );
    long Add( wxFileData *fd, wxListItem &item );
    void UpdateFiles();
    virtual void StatusbarText( wxChar *WXUNUSED(text) ) {};
    void MakeDir();
    void GoToParentDir();
    void GoToHomeDir();
    void GoToDir( const wxString &dir );
    void SetWild( const wxString &wild );
    void GetDir( wxString &dir );
    void OnListDeleteItem( wxListEvent &event );
    void OnListEndLabelEdit( wxListEvent &event );

    // Associate commonly used UI controls with wxFileCtrl so that they can be
    // disabled when they cannot be used (e.g. can't go to parent directory
    // if wxFileCtrl already is in the root dir):
    void SetGoToParentControl(wxWindow *ctrl) { m_goToParentControl = ctrl; }
    void SetNewDirControl(wxWindow *ctrl) { m_newDirControl = ctrl; }

private:
    void FreeItemData(const wxListItem& item);
    void FreeAllItemsData();

    wxString      m_dirName;
    bool          m_showHidden;
    wxString      m_wild;

    wxWindow     *m_goToParentControl;
    wxWindow     *m_newDirControl;

    // the label showing the current directory
    wxStaticText *m_labelDir;

    DECLARE_DYNAMIC_CLASS(wxFileCtrl);
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// private classes - icons list management
// ----------------------------------------------------------------------------

class wxFileIconEntry : public wxObject
{
public:
    wxFileIconEntry(int i) { id = i; }

    int id;
};


class wxFileIconsTable
{
public:
    wxFileIconsTable();

    int GetIconID(const wxString& extension, const wxString& mime = wxEmptyString);
    wxImageList *GetImageList() { return &m_ImageList; }

protected:
    wxImageList m_ImageList;
    wxHashTable m_HashTable;
};

static wxFileIconsTable *g_IconsTable = NULL;

#define FI_FOLDER     0
#define FI_UNKNOWN    1
#define FI_EXECUTABLE 2

wxFileIconsTable::wxFileIconsTable() :
                    m_ImageList(16, 16),
                    m_HashTable(wxKEY_STRING)
{
    m_HashTable.DeleteContents(TRUE);
    // FI_FOLDER:
    m_ImageList.Add(wxArtProvider::GetBitmap(wxART_FOLDER, wxART_CMN_DIALOG));
    // FI_UNKNOWN:
    m_ImageList.Add(wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_CMN_DIALOG));
    // FI_EXECUTABLE:
    if (GetIconID(wxEmptyString, _T("application/x-executable")) == FI_UNKNOWN)
    {
        m_ImageList.Add(wxArtProvider::GetBitmap(wxART_EXECUTABLE_FILE, wxART_CMN_DIALOG));
        m_HashTable.Delete(_T("exe"));
        m_HashTable.Put(_T("exe"), new wxFileIconEntry(FI_EXECUTABLE));
    }
    /* else put into list by GetIconID
       (KDE defines application/x-executable for *.exe and has nice icon)
     */
}



#if wxUSE_MIMETYPE
// VS: we don't need this function w/o wxMimeTypesManager because we'll only have
//     one icon and we won't resize it

static wxBitmap CreateAntialiasedBitmap(const wxImage& img)
{
    wxImage smallimg (16, 16);
    unsigned char *p1, *p2, *ps;
    unsigned char mr = img.GetMaskRed(),
                  mg = img.GetMaskGreen(),
                  mb = img.GetMaskBlue();

    unsigned x, y;
    unsigned sr, sg, sb, smask;

    p1 = img.GetData(), p2 = img.GetData() + 3 * 32, ps = smallimg.GetData();
    smallimg.SetMaskColour(mr, mr, mr);

    for (y = 0; y < 16; y++)
    {
        for (x = 0; x < 16; x++)
        {
            sr = sg = sb = smask = 0;
            if (p1[0] != mr || p1[1] != mg || p1[2] != mb)
                sr += p1[0], sg += p1[1], sb += p1[2];
            else smask++;
            p1 += 3;
            if (p1[0] != mr || p1[1] != mg || p1[2] != mb)
                sr += p1[0], sg += p1[1], sb += p1[2];
            else smask++;
            p1 += 3;
            if (p2[0] != mr || p2[1] != mg || p2[2] != mb)
                sr += p2[0], sg += p2[1], sb += p2[2];
            else smask++;
            p2 += 3;
            if (p2[0] != mr || p2[1] != mg || p2[2] != mb)
                sr += p2[0], sg += p2[1], sb += p2[2];
            else smask++;
            p2 += 3;

            if (smask > 2)
                ps[0] = ps[1] = ps[2] = mr;
            else
                ps[0] = sr >> 2, ps[1] = sg >> 2, ps[2] = sb >> 2;
            ps += 3;
        }
        p1 += 32 * 3, p2 += 32 * 3;
    }

    return wxBitmap(smallimg);
}

// finds empty borders and return non-empty area of image:
static wxImage CutEmptyBorders(const wxImage& img)
{
    unsigned char mr = img.GetMaskRed(),
                  mg = img.GetMaskGreen(),
                  mb = img.GetMaskBlue();
    unsigned char *dt = img.GetData(), *dttmp;
    unsigned w = img.GetWidth(), h = img.GetHeight();

    unsigned top, bottom, left, right, i;
    bool empt;

#define MK_DTTMP(x,y)      dttmp = dt + ((x + y * w) * 3)
#define NOEMPTY_PIX(empt)  if (dttmp[0] != mr || dttmp[1] != mg || dttmp[2] != mb) {empt = FALSE; break;}

    for (empt = TRUE, top = 0; empt && top < h; top++)
    {
        MK_DTTMP(0, top);
        for (i = 0; i < w; i++, dttmp+=3)
            NOEMPTY_PIX(empt)
    }
    for (empt = TRUE, bottom = h-1; empt && bottom > top; bottom--)
    {
        MK_DTTMP(0, bottom);
        for (i = 0; i < w; i++, dttmp+=3)
            NOEMPTY_PIX(empt)
    }
    for (empt = TRUE, left = 0; empt && left < w; left++)
    {
        MK_DTTMP(left, 0);
        for (i = 0; i < h; i++, dttmp+=3*w)
            NOEMPTY_PIX(empt)
    }
    for (empt = TRUE, right = w-1; empt && right > left; right--)
    {
        MK_DTTMP(right, 0);
        for (i = 0; i < h; i++, dttmp+=3*w)
            NOEMPTY_PIX(empt)
    }
    top--, left--, bottom++, right++;

    return img.GetSubImage(wxRect(left, top, right - left + 1, bottom - top + 1));
}
#endif // wxUSE_MIMETYPE



int wxFileIconsTable::GetIconID(const wxString& extension, const wxString& mime)
{
#if wxUSE_MIMETYPE
    if (!extension.IsEmpty())
    {
        wxFileIconEntry *entry = (wxFileIconEntry*) m_HashTable.Get(extension);
        if (entry) return (entry -> id);
    }

    wxFileType *ft = (mime.IsEmpty()) ?
                   wxTheMimeTypesManager -> GetFileTypeFromExtension(extension) :
                   wxTheMimeTypesManager -> GetFileTypeFromMimeType(mime);
    wxIcon ic;
    if (ft == NULL || (!ft -> GetIcon(&ic)) || (!ic.Ok()))
    {
        int newid = FI_UNKNOWN;
        m_HashTable.Put(extension, new wxFileIconEntry(newid));
        return newid;
    }
#ifdef __WIN32__
    wxBitmap myBitmap (ic.GetWidth(), ic.GetHeight() ) ;
    wxMemoryDC memDC;
    memDC.SelectObject( myBitmap );
    memDC.DrawIcon(ic,0,0);
    memDC.SelectObject( wxNullBitmap );
    wxImage img = myBitmap.ConvertToImage();
#else
    wxImage img = ic.ConvertToImage();
#endif
    delete ft;

    int id = m_ImageList.GetImageCount();
    if (img.GetWidth() == 16 && img.GetHeight() == 16)
        m_ImageList.Add(wxBitmap(img));
    else
    {
        if (img.GetWidth() != 32 || img.GetHeight() != 32)
            m_ImageList.Add(CreateAntialiasedBitmap(CutEmptyBorders(img).Rescale(32, 32)));
        else
            m_ImageList.Add(CreateAntialiasedBitmap(img));
    }
    m_HashTable.Put(extension, new wxFileIconEntry(id));
    return id;

#else // !wxUSE_MIMETYPE

    if (extension == wxT("exe"))
        return FI_EXECUTABLE;
    else
        return FI_UNKNOWN;
#endif // wxUSE_MIMETYPE/!wxUSE_MIMETYPE
}



// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

static
int ListCompare( long data1, long data2, long WXUNUSED(data))
{
     wxFileData *fd1 = (wxFileData*)data1 ;
     wxFileData *fd2 = (wxFileData*)data2 ;
     if (fd1->GetName() == wxT("..")) return -1;
     if (fd2->GetName() == wxT("..")) return 1;
     if (fd1->IsDir() && !fd2->IsDir()) return -1;
     if (fd2->IsDir() && !fd1->IsDir()) return 1;
     return wxStrcmp( fd1->GetName(), fd2->GetName() );
}

#ifdef __UNIX__
#define IsTopMostDir(dir)   (dir == wxT("/"))
#endif

#if defined(__DOS__) || defined(__WINDOWS__)
#define IsTopMostDir(dir)   (dir.IsEmpty())
#endif

#if defined(__DOS__) || defined(__WINDOWS__)
extern bool wxIsDriveAvailable(const wxString& dirName);
#endif

//-----------------------------------------------------------------------------
//  wxFileData
//-----------------------------------------------------------------------------

wxFileData::wxFileData( const wxString &name, const wxString &fname )
{
    m_name = name;
    m_fileName = fname;

#if defined(__DOS__) || defined(__WINDOWS__)
    // VS: In case the file is root directory of a volume (e.g. "C:"),
    //     we don't want it stat()ed, since the drive may not be in:
    if (name.length() == 2 && name[1u] == wxT(':'))
    {
        m_isDir = TRUE;
        m_isExe = m_isLink = FALSE;
        m_size = 0;
        return;
    }
#endif

    wxStructStat buff;
    wxStat( m_fileName, &buff );

#if defined(__UNIX__) && (!defined( __EMX__ ) && !defined(__VMS))
    struct stat lbuff;
    lstat( m_fileName.fn_str(), &lbuff );
    m_isLink = S_ISLNK( lbuff.st_mode );
    struct tm *t = localtime( &lbuff.st_mtime );
#else
    m_isLink = FALSE;
    struct tm *t = localtime( &buff.st_mtime );
#endif

//  struct passwd *user = getpwuid( buff.st_uid );
//  struct group *grp = getgrgid( buff.st_gid );

#ifdef __VISUALC__
    m_isDir = ((buff.st_mode & _S_IFDIR ) == _S_IFDIR );
#else
	m_isDir = S_ISDIR( buff.st_mode );
#endif // VC++
    m_isExe = ((buff.st_mode & wxS_IXUSR ) == wxS_IXUSR );

    m_size = buff.st_size;

    m_hour = t->tm_hour;
    m_minute = t->tm_min;
    m_month = t->tm_mon+1;
    m_day = t->tm_mday;
    m_year = t->tm_year;
    m_year += 1900;

    char buffer[10];
    sprintf( buffer, "%c%c%c",
     ((( buff.st_mode & wxS_IRUSR ) == wxS_IRUSR ) ? 'r' : '-'),
     ((( buff.st_mode & wxS_IWUSR ) == wxS_IWUSR ) ? 'w' : '-'),
     ((( buff.st_mode & wxS_IXUSR ) == wxS_IXUSR ) ? 'x' : '-') );
#if wxUSE_UNICODE
    m_permissions = wxConvUTF8.cMB2WC( buffer );
#else
    m_permissions = buffer;
#endif

//    m_permissions.sprintf( wxT("%c%c%c"),
//     ((( buff.st_mode & S_IRUSR ) == S_IRUSR ) ? wxT('r') : wxT('-')),
//     ((( buff.st_mode & S_IWUSR ) == S_IWUSR ) ? wxT('w') : wxT('-')),
//     ((( buff.st_mode & S_IXUSR ) == S_IXUSR ) ? wxT('x') : wxT('-')) );
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
    s += wxT("  ");
    if (m_isDir) s += wxT("<DIR> ");
    else if (m_isLink) s += wxT("<LINK> ");
    else
    {
        s += LongToString( m_size );
        s += wxT(" bytes ");
    }
    s += IntToString( m_day );
    s += wxT(".");
    s += IntToString( m_month );
    s += wxT(".");
    s += IntToString( m_year );
    s += wxT("  ");
    s += IntToString( m_hour );
    s += wxT(":");
    s += IntToString( m_minute );
    s += wxT("  ");
    s += m_permissions;
    return s;
};

wxString wxFileData::GetEntry( FileListField num ) const
{
    wxString s;
    switch ( num )
    {
        case FileList_Name:
            s = m_name;
            break;

        case FileList_Type:
            if (m_isDir)
                s = _("<DIR>");
            else if (m_isLink)
                s = _("<LINK>");
            else
                s.Printf(_T("%ld"), m_size);
            break;

        case FileList_Date:
            s.Printf(_T("%02d.%02d.%d"), m_day, m_month, m_year);
            break;

        case FileList_Time:
            s.Printf(_T("%02d:%02d"), m_hour, m_minute);
            break;

#ifdef __UNIX__
        case FileList_Perm:
            s = m_permissions;
            break;
#endif // __UNIX__

        default:
            wxFAIL_MSG( _T("unexpected field in wxFileData::GetEntry()") );
    }

    return s;
}

void wxFileData::SetNewName( const wxString &name, const wxString &fname )
{
    m_name = name;
    m_fileName = fname;
}

void wxFileData::MakeItem( wxListItem &item )
{
    item.m_text = m_name;
    item.ClearAttributes();
    if (IsExe())
        item.SetTextColour(*wxRED);
    if (IsDir())
        item.SetTextColour(*wxBLUE);

    if (IsDir())
        item.m_image = FI_FOLDER;
    else if (IsExe())
        item.m_image = FI_EXECUTABLE;
    else if (m_name.Find(wxT('.')) != wxNOT_FOUND)
        item.m_image = g_IconsTable->GetIconID(m_name.AfterLast(wxT('.')));
    else
        item.m_image = FI_UNKNOWN;

    if (IsLink())
    {
        wxColour *dg = wxTheColourDatabase->FindColour( _T("MEDIUM GREY") );
        item.SetTextColour(*dg);
    }
    item.m_data = (long)this;
}

//-----------------------------------------------------------------------------
//  wxFileCtrl
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxFileCtrl,wxListCtrl)

BEGIN_EVENT_TABLE(wxFileCtrl,wxListCtrl)
    EVT_LIST_DELETE_ITEM(-1, wxFileCtrl::OnListDeleteItem)
    EVT_LIST_END_LABEL_EDIT(-1, wxFileCtrl::OnListEndLabelEdit)
END_EVENT_TABLE()


wxFileCtrl::wxFileCtrl()
{
    m_showHidden = FALSE;
}

wxFileCtrl::wxFileCtrl(wxWindow *win,
                       wxStaticText *labelDir,
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
    if (! g_IconsTable)
        g_IconsTable = new wxFileIconsTable;
    wxImageList *imageList = g_IconsTable->GetImageList();

    SetImageList( imageList, wxIMAGE_LIST_SMALL );

    m_goToParentControl =
    m_newDirControl = NULL;

    m_labelDir = labelDir;

    m_showHidden = showHidden;
}

void wxFileCtrl::ChangeToListMode()
{
    SetSingleStyle( wxLC_LIST );
    UpdateFiles();
}

void wxFileCtrl::ChangeToReportMode()
{
    SetSingleStyle( wxLC_REPORT );
    UpdateFiles();
}

void wxFileCtrl::ChangeToIconMode()
{
    SetSingleStyle( wxLC_ICON );
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
        for (int i = 1; i < FileList_Max; i++)
            SetItem( item.m_itemId, i, fd->GetEntry((FileListField)i) );
    }
    else if (my_style & wxLC_LIST)
    {
        ret = InsertItem( item );
    }
    return ret;
}

void wxFileCtrl::UpdateFiles()
{
    // don't do anything before ShowModal() call which sets m_dirName
    if ( m_dirName.empty() )
        return;

    wxBusyCursor bcur; // this may take a while...

    long my_style = GetWindowStyleFlag();
    int name_col_width = 0;
    if (my_style & wxLC_REPORT)
    {
        if (GetColumnCount() > 0)
            name_col_width = GetColumnWidth( 0 );
    }

    FreeAllItemsData();
    ClearAll();

    if (my_style & wxLC_REPORT)
    {
        if (name_col_width < 140) name_col_width = 140;
        InsertColumn( 0, _("Name"), wxLIST_FORMAT_LEFT, name_col_width );
        InsertColumn( 1, _("Size"), wxLIST_FORMAT_LEFT, 60 );
        InsertColumn( 2, _("Date"), wxLIST_FORMAT_LEFT, 65 );
        InsertColumn( 3, _("Time"), wxLIST_FORMAT_LEFT, 50 );
#ifdef __UNIX__
        InsertColumn( 4, _("Permissions"), wxLIST_FORMAT_LEFT, 120 );
#endif
    }
    wxFileData *fd = (wxFileData *) NULL;
    wxListItem item;
    item.m_itemId = 0;
    item.m_col = 0;

#if defined(__DOS__) || defined(__WINDOWS__)
    if ( IsTopMostDir(m_dirName) )
    {
        // Pseudo-directory with all available drives listed...
        for (int drive = 1; drive <= 26; drive++)
        {
            wxString path;
            path.Printf(wxT("%c:\\"), (char)(drive + 'A' - 1));
            if ( wxIsDriveAvailable(path) )
            {
                path.RemoveLast();
                fd = new wxFileData(path, path);
                Add(fd, item);
                item.m_itemId++;
            }
        }
    }
    else
#endif
    {
        // Real directory...
        if ( !IsTopMostDir(m_dirName) )
        {
            wxString p(wxPathOnly(m_dirName));
#ifdef __UNIX__
            if (p.IsEmpty()) p = wxT("/");
#endif
            fd = new wxFileData( wxT(".."), p );
            Add(fd, item);
            item.m_itemId++;
        }

        wxString dirname(m_dirName);
#if defined(__DOS__) || defined(__WINDOWS__)
        if (dirname.length() == 2 && dirname[1u] == wxT(':'))
            dirname << wxT('\\');
#endif
        wxDir dir(dirname);

        if ( dir.IsOpened() )
        {
            wxString dirPrefix(dirname + wxFILE_SEP_PATH);
            int hiddenFlag = m_showHidden ? wxDIR_HIDDEN : 0;

            bool cont;
            wxString f;

            // Get the directories first (not matched against wildcards):
            cont = dir.GetFirst(&f, wxEmptyString, wxDIR_DIRS | hiddenFlag);
            while (cont)
            {
                fd = new wxFileData(f, dirPrefix + f);
                Add(fd, item);
                item.m_itemId++;
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
                    fd = new wxFileData(f, dirPrefix + f);
                    Add(fd, item);
                    item.m_itemId++;
                    cont = dir.GetNext(&f);
                }
            }
        }
    }

    SortItems((wxListCtrlCompare)ListCompare, 0);

    if ( my_style & wxLC_REPORT )
    {
       SetColumnWidth(1, wxLIST_AUTOSIZE);
       SetColumnWidth(2, wxLIST_AUTOSIZE);
       SetColumnWidth(3, wxLIST_AUTOSIZE);
    }

    // Finally, enable/disable context-dependent controls:
    if ( m_goToParentControl )
        m_goToParentControl->Enable(!IsTopMostDir(m_dirName));
#if defined(__DOS__) || defined(__WINDOWS__)
    if ( m_newDirControl )
        m_newDirControl->Enable(!IsTopMostDir(m_dirName));
#endif
}

void wxFileCtrl::SetWild( const wxString &wild )
{
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

    wxFileData *fd = new wxFileData( new_name, path );
    wxListItem item;
    item.m_itemId = 0;
    item.m_col = 0;
    long id = Add( fd, item );

    if (id != -1)
    {
        SortItems( (wxListCtrlCompare) ListCompare, 0 );
        id = FindItem( 0, (long)fd );
        EnsureVisible( id );
        EditLabel( id );
    }
}

void wxFileCtrl::GoToParentDir()
{
    if (!IsTopMostDir(m_dirName))
    {
        size_t len = m_dirName.Len();
        if (m_dirName[len-1] == wxFILE_SEP_PATH)
            m_dirName.Remove( len-1, 1 );
        wxString fname( wxFileNameFromPath(m_dirName) );
        m_dirName = wxPathOnly( m_dirName );
#ifdef __UNIX__
        if (m_dirName.IsEmpty())
            m_dirName = wxT("/");
#endif
        UpdateFiles();
        long id = FindItem( 0, fname );
        if (id != -1)
        {
            SetItemState( id, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
            EnsureVisible( id );
        }

        m_labelDir->SetLabel(m_dirName);
    }
}

void wxFileCtrl::GoToHomeDir()
{
    wxString s = wxGetUserHome( wxString() );
    GoToDir(s);
}

void wxFileCtrl::GoToDir( const wxString &dir )
{
    m_dirName = dir;
    UpdateFiles();
    SetItemState( 0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
    EnsureVisible( 0 );

    m_labelDir->SetLabel(dir);
}

void wxFileCtrl::GetDir( wxString &dir )
{
    dir = m_dirName;
}

void wxFileCtrl::FreeItemData(const wxListItem& item)
{
    wxFileData *fd = (wxFileData*)item.m_data;
    delete fd;
}

void wxFileCtrl::OnListDeleteItem( wxListEvent &event )
{
    FreeItemData(event.m_item);
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

    if ((event.GetLabel().IsEmpty()) ||
        (event.GetLabel() == _(".")) ||
        (event.GetLabel() == _("..")) ||
        (event.GetLabel().First( wxFILE_SEP_PATH ) != wxNOT_FOUND))
    {
        wxMessageDialog dialog(this, _("Illegal directory name."), _("Error"), wxOK | wxICON_ERROR );
        dialog.ShowModal();
        event.Veto();
        return;
    }

    wxString new_name( wxPathOnly( fd->GetFullName() ) );
    new_name += wxFILE_SEP_PATH;
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

wxFileCtrl::~wxFileCtrl()
{
    FreeAllItemsData();
}

//-----------------------------------------------------------------------------
// wxFileDialog
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
        EVT_CHOICE(ID_CHOICE,wxFileDialog::OnChoiceFilter)
        EVT_TEXT_ENTER(ID_TEXT,wxFileDialog::OnTextEnter)
        EVT_TEXT(ID_TEXT,wxFileDialog::OnTextChange)
        EVT_CHECKBOX(ID_CHECK,wxFileDialog::OnCheck)
END_EVENT_TABLE()

long wxFileDialog::ms_lastViewStyle = wxLC_LIST;
bool wxFileDialog::ms_lastShowHidden = FALSE;

wxFileDialog::wxFileDialog(wxWindow *parent,
                           const wxString& message,
                           const wxString& defaultDir,
                           const wxString& defaultFile,
                           const wxString& wildCard,
                           long style,
                           const wxPoint& pos )
            : wxDialog( parent, -1, message, pos, wxDefaultSize,
                        wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER )
{
    if (wxConfig::Get(FALSE))
    {
        wxConfig::Get()->Read(wxT("/wxWindows/wxFileDialog/ViewStyle"),
                              &ms_lastViewStyle);
        wxConfig::Get()->Read(wxT("/wxWindows/wxFileDialog/ShowHidden"),
                              &ms_lastShowHidden);
    }

    m_message = message;
    m_dialogStyle = style;

    if (m_dialogStyle == 0)
        m_dialogStyle = wxOPEN;
    if ((m_dialogStyle & wxMULTIPLE ) && !(m_dialogStyle & wxOPEN))
        m_dialogStyle |= wxOPEN;

    m_dir = defaultDir;
    if ((m_dir.empty()) || (m_dir == wxT(".")))
    {
        m_dir = wxGetCwd();
    }

    size_t len = m_dir.Len();
    if ((len > 1) && (m_dir[len-1] == wxFILE_SEP_PATH))
        m_dir.Remove( len-1, 1 );

    m_path = m_dir;
    m_path += wxFILE_SEP_PATH;
    m_path += defaultFile;
    m_fileName = defaultFile;
    m_wildCard = wildCard;
    m_filterIndex = 0;
    m_filterExtension = wxEmptyString;

    // interpret wildcards

    if (m_wildCard.IsEmpty())
        m_wildCard = _("All files (*)|*");

    wxStringTokenizer tokens( m_wildCard, wxT("|") );
    wxString firstWild;
    wxString firstWildText;
    if (tokens.CountTokens() == 1)
    {
        firstWildText = tokens.GetNextToken();
        firstWild = firstWildText;
    }
    else
    {
        wxASSERT_MSG( tokens.CountTokens() % 2 == 0, wxT("Wrong file type descripition") );
        firstWildText = tokens.GetNextToken();
        firstWild = tokens.GetNextToken();
    }
    if ( firstWild.Left( 2 ) == wxT("*.") )
        m_filterExtension = firstWild.Mid( 1 );
    if ( m_filterExtension == wxT(".*") )
        m_filterExtension = wxEmptyString;

    // layout
    
    bool is_pda = (wxSystemSettings::GetScreenType() <= wxSYS_SCREEN_PDA);

    wxBoxSizer *mainsizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *buttonsizer = new wxBoxSizer( wxHORIZONTAL );

    wxBitmapButton *but;

    but = new wxBitmapButton(this, ID_LIST_MODE,
                             wxArtProvider::GetBitmap(wxART_LIST_VIEW, wxART_CMN_DIALOG));
#if wxUSE_TOOLTIPS
    but->SetToolTip( _("View files as a list view") );
#endif
    buttonsizer->Add( but, 0, wxALL, 5 );

    but = new wxBitmapButton(this, ID_REPORT_MODE,
                             wxArtProvider::GetBitmap(wxART_REPORT_VIEW, wxART_CMN_DIALOG));
#if wxUSE_TOOLTIPS
    but->SetToolTip( _("View files as a detailed view") );
#endif
    buttonsizer->Add( but, 0, wxALL, 5 );

    buttonsizer->Add( 30, 5, 1 );

    wxWindow *butDirUp =
        new wxBitmapButton(this, ID_UP_DIR,
                           wxArtProvider::GetBitmap(wxART_GO_DIR_UP, wxART_CMN_DIALOG));
#if wxUSE_TOOLTIPS
    butDirUp->SetToolTip( _("Go to parent directory") );
#endif
    buttonsizer->Add( butDirUp, 0, wxALL, 5 );

#ifndef __DOS__ // VS: Home directory is meaningless in MS-DOS...
    but = new wxBitmapButton(this, ID_PARENT_DIR,
                             wxArtProvider::GetBitmap(wxART_GO_HOME, wxART_CMN_DIALOG));
#if wxUSE_TOOLTIPS
    but->SetToolTip( _("Go to home directory") );
#endif
    buttonsizer->Add( but, 0, wxALL, 5);

    buttonsizer->Add( 20, 20 );
#endif //!__DOS__

    wxWindow *butNewDir =
        new wxBitmapButton(this, ID_NEW_DIR,
                           wxArtProvider::GetBitmap(wxART_NEW_DIR, wxART_CMN_DIALOG));
#if wxUSE_TOOLTIPS
    butNewDir->SetToolTip( _("Create new directory") );
#endif
    buttonsizer->Add( butNewDir, 0, wxALL, 5 );

    if (is_pda)
        mainsizer->Add( buttonsizer, 0, wxALL | wxEXPAND, 0 );
    else
        mainsizer->Add( buttonsizer, 0, wxALL | wxEXPAND, 5 );

    wxBoxSizer *staticsizer = new wxBoxSizer( wxHORIZONTAL );
    if (is_pda)
        staticsizer->Add( new wxStaticText( this, -1, _("Current directory:") ), 0, wxRIGHT, 10 );
    m_static = new wxStaticText( this, -1, m_dir );
    staticsizer->Add( m_static, 1 );
    mainsizer->Add( staticsizer, 0, wxEXPAND | wxLEFT|wxRIGHT|wxBOTTOM, 10 );

    long style2 = ms_lastViewStyle | wxSUNKEN_BORDER;
    if ( !(m_dialogStyle & wxMULTIPLE) )
        style2 |= wxLC_SINGLE_SEL;

    m_list = new wxFileCtrl( this, m_static, ID_LIST_CTRL,
                             firstWild, ms_lastShowHidden,
                             wxDefaultPosition, wxSize(540,200),
                             style2);

    m_list->SetNewDirControl(butNewDir);
    m_list->SetGoToParentControl(butDirUp);

    if (is_pda)
    {
        // PDAs have a different screen layout
        mainsizer->Add( m_list, 1, wxEXPAND | wxLEFT|wxRIGHT, 5 );

        wxBoxSizer *choicesizer = new wxBoxSizer( wxHORIZONTAL );
        m_choice = new wxChoice( this, ID_CHOICE );
        choicesizer->Add( m_choice, 1, wxCENTER|wxALL, 5 );
        mainsizer->Add( choicesizer, 0, wxEXPAND );

        wxBoxSizer *textsizer = new wxBoxSizer( wxHORIZONTAL );
        m_text = new wxTextCtrl( this, ID_TEXT, m_fileName, wxDefaultPosition, wxDefaultSize, wxPROCESS_ENTER );
        textsizer->Add( m_text, 1, wxCENTER | wxALL, 5 );
        mainsizer->Add( textsizer, 0, wxEXPAND );

        m_check = new wxCheckBox( this, ID_CHECK, _("Show hidden files") );
        m_check->SetValue( ms_lastShowHidden );
        textsizer->Add( m_check, 0, wxCENTER|wxALL, 5 );

        buttonsizer = new wxBoxSizer( wxHORIZONTAL );
        buttonsizer->Add( new wxButton( this, wxID_OK, _("OK") ), 0, wxCENTER | wxALL, 5 );
        buttonsizer->Add( new wxButton( this, wxID_CANCEL, _("Cancel") ), 0, wxCENTER | wxALL, 5 );
        mainsizer->Add( buttonsizer, 0, wxALIGN_RIGHT );
    }
    else
    {
        mainsizer->Add( m_list, 1, wxEXPAND | wxLEFT|wxRIGHT, 10 );

        wxBoxSizer *textsizer = new wxBoxSizer( wxHORIZONTAL );
        m_text = new wxTextCtrl( this, ID_TEXT, m_fileName, wxDefaultPosition, wxDefaultSize, wxPROCESS_ENTER );
        textsizer->Add( m_text, 1, wxCENTER | wxLEFT|wxRIGHT|wxTOP, 10 );
        textsizer->Add( new wxButton( this, wxID_OK, _("OK") ), 0, wxCENTER | wxLEFT|wxRIGHT|wxTOP, 10 );
        mainsizer->Add( textsizer, 0, wxEXPAND );

        wxBoxSizer *choicesizer = new wxBoxSizer( wxHORIZONTAL );
        m_choice = new wxChoice( this, ID_CHOICE );
        choicesizer->Add( m_choice, 1, wxCENTER|wxALL, 10 );
        m_check = new wxCheckBox( this, ID_CHECK, _("Show hidden files") );
        m_check->SetValue( ms_lastShowHidden );
        choicesizer->Add( m_check, 0, wxCENTER|wxALL, 10 );
        choicesizer->Add( new wxButton( this, wxID_CANCEL, _("Cancel") ), 0, wxCENTER | wxALL, 10 );
        mainsizer->Add( choicesizer, 0, wxEXPAND );
    }

    m_choice->Append( firstWildText, (void*) new wxString( firstWild ) );
    while (tokens.HasMoreTokens())
    {
        firstWildText = tokens.GetNextToken();
        firstWild = tokens.GetNextToken();
        m_choice->Append( firstWildText, (void*) new wxString( firstWild ) );
    }
    m_choice->SetSelection( 0 );

    SetAutoLayout( TRUE );
    SetSizer( mainsizer );

    mainsizer->Fit( this );
    mainsizer->SetSizeHints( this );

    Centre( wxBOTH );

    m_text->SetFocus();
}

wxFileDialog::~wxFileDialog()
{
    if (wxConfig::Get(FALSE))
    {
        wxConfig::Get()->Write(wxT("/wxWindows/wxFileDialog/ViewStyle"),
                               ms_lastViewStyle);
        wxConfig::Get()->Write(wxT("/wxWindows/wxFileDialog/ShowHidden"),
                               ms_lastShowHidden);
    }

    const size_t count = m_choice->GetCount();
    size_t i;
    for ( i = 0; i < count; i++ )
    {
        delete (wxString *)m_choice->GetClientData(i);
    }
}

int wxFileDialog::ShowModal()
{
    m_list->GoToDir(m_dir);
    m_text->SetValue(m_fileName);

    return wxDialog::ShowModal();
}

void wxFileDialog::DoSetFilterIndex(int filterindex)
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

void wxFileDialog::SetFilterIndex( int filterindex )
{
    m_choice->SetSelection( filterindex );

    DoSetFilterIndex(filterindex);
}

void wxFileDialog::OnChoiceFilter( wxCommandEvent &event )
{
    DoSetFilterIndex((int)event.GetInt());
}

void wxFileDialog::OnCheck( wxCommandEvent &event )
{
    m_list->ShowHidden( (ms_lastShowHidden = event.GetInt() != 0) );
}

void wxFileDialog::OnActivated( wxListEvent &event )
{
    HandleAction( event.m_item.m_text );
}

void wxFileDialog::OnTextEnter( wxCommandEvent &WXUNUSED(event) )
{
    wxCommandEvent cevent(wxEVT_COMMAND_BUTTON_CLICKED, wxID_OK);
    cevent.SetEventObject( this );
    GetEventHandler()->ProcessEvent( cevent );
}

static bool ignoreChanges = FALSE;

void wxFileDialog::OnTextChange( wxCommandEvent &WXUNUSED(event) )
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

void wxFileDialog::OnSelected( wxListEvent &event )
{
    wxString filename( event.m_item.m_text );
    if (filename == wxT("..")) return;

    wxString dir;
    m_list->GetDir( dir );
    if (!IsTopMostDir(dir))
        dir += wxFILE_SEP_PATH;
    dir += filename;
    if (wxDirExists(dir)) return;

    ignoreChanges = TRUE;
    m_text->SetValue( filename );
    ignoreChanges = FALSE;
}

void wxFileDialog::HandleAction( const wxString &fn )
{
    wxString filename( fn );
    wxString dir;
    m_list->GetDir( dir );
    if (filename.IsEmpty()) return;
    if (filename == wxT(".")) return;

    if (filename == wxT(".."))
    {
        m_list->GoToParentDir();
        m_list->SetFocus();
        return;
    }

#ifdef __UNIX__
    if (filename == wxT("~"))
    {
        m_list->GoToHomeDir();
        m_list->SetFocus();
        return;
    }

    if (filename[0u] == wxT('~'))
    {
        filename.Remove( 0, 1 );
        wxString tmp( wxGetUserHome() );
        tmp += wxT('/');
        tmp += filename;
        filename = tmp;
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
        return;
    }

    // append the default extension to the filename if it doesn't have any
    //
    // VZ: the logic of testing for !wxFileExists() only for the open file
    //     dialog is not entirely clear to me, why don't we allow saving to a
    //     file without extension as well?
    if ( !(m_dialogStyle & wxOPEN) || !wxFileExists(filename) )
    {
        wxString ext;
        wxSplitPath(filename, NULL, NULL, &ext);
        if ( ext.empty() )
        {
            // append the first extension of the filter string
            filename += m_filterExtension.BeforeFirst(_T(';'));
        }
    }

    // check that the file [doesn't] exist if necessary
    if ( (m_dialogStyle & wxSAVE) &&
            (m_dialogStyle & wxOVERWRITE_PROMPT) &&
                wxFileExists( filename ) )
    {
        wxString msg;
        msg.Printf( _("File '%s' already exists, do you really want to "
                     "overwrite it?"), filename.c_str() );

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

        if ( cwd != wxGetWorkingDirectory() )
        {
            wxSetWorkingDirectory(cwd);
        }
    }

    wxCommandEvent event;
    wxDialog::OnOK(event);
}

void wxFileDialog::OnListOk( wxCommandEvent &WXUNUSED(event) )
{
    HandleAction( m_text->GetValue() );
}

void wxFileDialog::OnList( wxCommandEvent &WXUNUSED(event) )
{
    m_list->ChangeToListMode();
    ms_lastViewStyle = wxLC_LIST;
    m_list->SetFocus();
}

void wxFileDialog::OnReport( wxCommandEvent &WXUNUSED(event) )
{
    m_list->ChangeToReportMode();
    ms_lastViewStyle = wxLC_REPORT;
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

void wxFileDialog::GetPaths( wxArrayString& paths ) const
{
    paths.Empty();
    if (m_list->GetSelectedItemCount() == 0)
    {
        paths.Add( GetPath() );
        return;
    }

    paths.Alloc( m_list->GetSelectedItemCount() );

    wxString dir;
    m_list->GetDir( dir );
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

void wxFileDialog::GetFilenames(wxArrayString& files) const
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



// ----------------------------------------------------------------------------
// global functions
// ----------------------------------------------------------------------------

wxString
wxFileSelectorEx(const wxChar *message,
                 const wxChar *default_path,
                 const wxChar *default_filename,
                 int *WXUNUSED(indexDefaultExtension),
                 const wxChar *wildcard,
                 int flags,
                 wxWindow *parent,
                 int x, int y)
{
    // TODO: implement this somehow
    return wxFileSelector(message, default_path, default_filename, wxT(""),
                          wildcard, flags, parent, x, y);
}

wxString wxFileSelector( const wxChar *title,
                      const wxChar *defaultDir, const wxChar *defaultFileName,
                      const wxChar *defaultExtension, const wxChar *filter, int flags,
                      wxWindow *parent, int x, int y )
{
    wxString filter2;
    if ( defaultExtension && !filter )
        filter2 = wxString(wxT("*.")) + wxString(defaultExtension) ;
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

static wxString GetWildcardString(const wxChar *ext)
{
    wxString wild;
    if ( ext )
    {
        if ( *ext == wxT('.') )
            ext++;

        wild << _T("*.") << ext;
    }
    else // no extension specified
    {
        wild = wxFileSelectorDefaultWildcardStr;
    }

    return wild;
}

wxString wxLoadFileSelector(const wxChar *what,
                            const wxChar *ext,
                            const wxChar *nameDef,
                            wxWindow *parent)
{
    wxString prompt;
    if ( what && *what )
        prompt = wxString::Format(_("Load %s file"), what);
    else
        prompt = _("Load file");

    return wxFileSelector(prompt, NULL, nameDef, ext,
                          GetWildcardString(ext), 0, parent);
}

wxString wxSaveFileSelector(const wxChar *what,
                            const wxChar *ext,
                            const wxChar *nameDef,
                            wxWindow *parent)
{
    wxString prompt;
    if ( what && *what )
        prompt = wxString::Format(_("Save %s file"), what);
    else
        prompt = _("Save file");

    return wxFileSelector(prompt, NULL, nameDef, ext,
                          GetWildcardString(ext), 0, parent);
}

// A module to allow icons table cleanup

class wxFileDialogGenericModule: public wxModule
{
DECLARE_DYNAMIC_CLASS(wxFileDialogGenericModule)
public:
    wxFileDialogGenericModule() {}
    bool OnInit() { return TRUE; }
    void OnExit() { if (g_IconsTable) {delete g_IconsTable; g_IconsTable = NULL;} }
};

IMPLEMENT_DYNAMIC_CLASS(wxFileDialogGenericModule, wxModule)

#endif // wxUSE_FILEDLG

