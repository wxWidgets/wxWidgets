/////////////////////////////////////////////////////////////////////////////
// Name:        dirctrlg.cpp
// Purpose:     wxGenericDirCtrl
// Author:      Harm van der Heijden, Robert Roebling, Julian Smart
// Modified by:
// Created:     12/12/98
// RCS-ID:      $Id$
// Copyright:   (c) Harm van der Heijden, Robert Roebling and Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dirctrlg.h"
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
#include "wx/icon.h"
#include "wx/log.h"
#include "wx/sizer.h"
#include "wx/tokenzr.h"
#include "wx/dir.h"
#include "wx/settings.h"

#if wxUSE_STATLINE
    #include "wx/statline.h"
#endif

#include "wx/generic/dirctrlg.h"

#ifdef __WXMSW__
#include <direct.h>
#include <stdlib.h>
#include <ctype.h>
#endif

// If compiled under Windows, this macro can cause problems
#ifdef GetFirstChild
#undef GetFirstChild
#endif

#if !defined(__WXMSW__) || wxUSE_XPM_IN_MSW
/* Closed folder */
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

/* Open folder */
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

/* File */
static char * icon3_xpm[] = {
/* width height ncolors chars_per_pixel */
"16 16 3 1",
/* colors */
" 	s None	c None",
".	c #000000",
"+	c #ffffff",
/* pixels */
"                ",
"  ........      ",
"  .++++++..     ",
"  .+.+.++.+.    ",
"  .++++++....   ",
"  .+.+.+++++.   ",
"  .+++++++++.   ",
"  .+.+.+.+.+.   ",
"  .+++++++++.   ",
"  .+.+.+.+.+.   ",
"  .+++++++++.   ",
"  .+.+.+.+.+.   ",
"  .+++++++++.   ",
"  ...........   ",
"                ",
"                "};

/* Computer */
static char * icon4_xpm[] = {
"16 16 7 1",
" 	s None	c None",
".	c #808080",
"X	c #c0c0c0",
"o	c Black",
"O	c Gray100",
"+	c #008080",
"@	c Blue",
"    ........... ",
"   .XXXXXXXXXX.o",
"   .OOOOOOOOO..o",
"   .OoooooooX..o",
"   .Oo+...@+X..o",
"   .Oo+XXX.+X..o",
"   .Oo+....+X..o",
"   .Oo++++++X..o",
"   .OXXXXXXXX.oo",
"   ..........o.o",
"   ...........Xo",
"   .XXXXXXXXXX.o",
"  .o.o.o.o.o...o",
" .oXoXoXoXoXo.o ",
".XOXXXXXXXXX.o  ",
"............o   "};

/* Drive */
static char * icon5_xpm[] = {
"16 16 7 1",
" 	s None	c None",
".	c #808080",
"X	c #c0c0c0",
"o	c Black",
"O	c Gray100",
"+	c Green",
"@	c #008000",
"                ",
"                ",
"                ",
"                ",
"  ............. ",
" .XXXXXXXXXXXX.o",
".OOOOOOOOOOOO..o",
".XXXXXXXXX+@X..o",
".XXXXXXXXXXXX..o",
".X..........X..o",
".XOOOOOOOOOOX..o",
"..............o ",
" ooooooooooooo  ",
"                ",
"                ",
"                "};

/* CD-ROM */
static char *icon6_xpm[] = {
"16 16 10 1",
" 	s None	c None",
".	c #808080",
"X	c #c0c0c0",
"o	c Yellow",
"O	c Blue",
"+	c Black",
"@	c Gray100",
"#	c #008080",
"$	c Green",
"%	c #008000",
"        ...     ",
"      ..XoX..   ",
"     .O.XoXXX+  ",
"    ...O.oXXXX+ ",
"    .O..X.XXXX+ ",
"   ....X.+..XXX+",
"   .XXX.+@+.XXX+",
"   .X@XX.+.X@@X+",
" .....X...#XX@+ ",
".@@@...XXo.O@X+ ",
".@XXX..XXoXOO+  ",
".@++++..XoX+++  ",
".@$%@@XX+++X.+  ",
".............+  ",
" ++++++++++++   ",
"                "};

/* Floppy */
static char * icon7_xpm[] = {
"16 16 7 1",
" 	s None	c None",
".	c #808080",
"X	c Gray100",
"o	c #c0c0c0",
"O	c Black",
"+	c Cyan",
"@	c Red",
"         ......X",
"        .ooooooO",
"        .+++++OO",
"        .++++++O",
"        .++++++O",
"        .ooooooO",
"  .......o....oO",
" .oooooo.o.O.XoO",
".XXXXXXXXOOOOOO ",
".ooooooooo@o..O ",
".ooo....oooo..O ",
".o..OOOO...o..O ",
".oooXXXXoooo..O ",
".............O  ",
" OOOOOOOOOOOO   ",
"                "};

/* Removeable */
static char * icon8_xpm[] = {
"16 16 7 1",
" 	s None	c None",
".	c #808080",
"X	c #c0c0c0",
"o	c Black",
"O	c Gray100",
"+	c Red",
"@	c #800000",
"                ",
"                ",
"                ",
"  ............. ",
" .XXXXXXXXXXXX.o",
".OOOOOOOOOOOO..o",
".OXXXXXXXXXXX..o",
".O+@.oooooo.X..o",
".OXXOooooooOX..o",
".OXXXOOOOOOXX..o",
".OXXXXXXXXXXX..o",
".O............o ",
" ooooooooooooo  ",
"                ",
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
// wxDirItemDataEx
//-----------------------------------------------------------------------------

wxDirItemDataEx::wxDirItemDataEx(const wxString& path, const wxString& name,
                                 bool isDir)
{
    m_path = path;
    m_name = name;
    /* Insert logic to detect hidden files here
     * In UnixLand we just check whether the first char is a dot
     * For FileNameFromPath read LastDirNameInThisPath ;-) */
    // m_isHidden = (bool)(wxFileNameFromPath(*m_path)[0] == '.');
    m_isHidden = FALSE;
    m_hasSubDirs = HasSubDirs();
    m_isExpanded = FALSE;
    m_isDir = isDir;
}

wxDirItemDataEx::~wxDirItemDataEx()
{
}

void wxDirItemDataEx::SetNewDirName( wxString path )
{
    m_path = path;
    m_name = wxFileNameFromPath( path );
}

bool wxDirItemDataEx::HasSubDirs()
{
    wxString search = m_path + wxT("/*");
    wxLogNull log;
    wxString path = wxFindFirstFile( search, wxDIR );
    return (bool)(!path.IsNull());
}

//-----------------------------------------------------------------------------
// wxGenericDirCtrl
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGenericDirCtrl, wxControl)

BEGIN_EVENT_TABLE(wxGenericDirCtrl, wxControl)
  EVT_TREE_ITEM_EXPANDING     (-1, wxGenericDirCtrl::OnExpandItem)
  EVT_TREE_ITEM_COLLAPSED     (-1, wxGenericDirCtrl::OnCollapseItem)
  EVT_TREE_BEGIN_LABEL_EDIT   (-1, wxGenericDirCtrl::OnBeginEditItem)
  EVT_TREE_END_LABEL_EDIT     (-1, wxGenericDirCtrl::OnEndEditItem)
  EVT_SIZE                    (wxGenericDirCtrl::OnSize)
END_EVENT_TABLE()

wxGenericDirCtrl::wxGenericDirCtrl(void)
{
    Init();
}

bool wxGenericDirCtrl::Create(wxWindow *parent,
                     const wxWindowID id,
                     const wxString& dir,
                     const wxPoint& pos,
                     const wxSize& size,
                     long style,
                     const wxString& filter,
                     int defaultFilter,
                     const wxString& name )
{
    if (!wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name))
        return FALSE;

    SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));

    Init();

    long treeStyle = wxTR_HAS_BUTTONS;
    if ((style & wxDIRCTRL_3D_INTERNAL) == 0)
        treeStyle |= wxNO_BORDER;

    long filterStyle = 0;
    if ((style & wxDIRCTRL_3D_INTERNAL) == 0)
        filterStyle |= wxNO_BORDER;

    m_treeCtrl = new wxTreeCtrl(this, wxID_TREECTRL, pos, size, treeStyle);

    if (!filter.IsEmpty() && (style & wxDIRCTRL_SHOW_FILTERS))
        m_filterListCtrl = new wxDirFilterListCtrl(this, wxID_FILTERLISTCTRL, wxDefaultPosition, wxDefaultSize, filterStyle);

    m_defaultPath = dir;
    m_filter = filter;

    SetFilterIndex(defaultFilter);

    if (m_filterListCtrl)
        m_filterListCtrl->FillFilterList(filter, defaultFilter);

    m_imageList = new wxImageList(16, 16, TRUE);
#if !defined(__WXMSW__) || wxUSE_XPM_IN_MSW
    m_imageList->Add(wxIcon(icon1_xpm));
    m_imageList->Add(wxIcon(icon2_xpm));
    m_imageList->Add(wxIcon(icon3_xpm));
    m_imageList->Add(wxIcon(icon4_xpm));
    m_imageList->Add(wxIcon(icon5_xpm));
    m_imageList->Add(wxIcon(icon6_xpm));
    m_imageList->Add(wxIcon(icon7_xpm));
    m_imageList->Add(wxIcon(icon8_xpm));
#elif defined(__WXMSW__)
    m_imageList->Add(wxIcon(wxT("wxICON_SMALL_CLOSED_FOLDER"), wxBITMAP_TYPE_RESOURCE));
    m_imageList->Add(wxIcon(wxT("wxICON_SMALL_OPEN_FOLDER"), wxBITMAP_TYPE_RESOURCE));
    m_imageList->Add(wxIcon(wxT("wxICON_SMALL_FILE"), wxBITMAP_TYPE_RESOURCE));
    m_imageList->Add(wxIcon(wxT("wxICON_SMALL_COMPUTER"), wxBITMAP_TYPE_RESOURCE));
    m_imageList->Add(wxIcon(wxT("wxICON_SMALL_DRIVE"), wxBITMAP_TYPE_RESOURCE));
    m_imageList->Add(wxIcon(wxT("wxICON_SMALL_CDROM"), wxBITMAP_TYPE_RESOURCE));
    m_imageList->Add(wxIcon(wxT("wxICON_SMALL_FLOPPY"), wxBITMAP_TYPE_RESOURCE));
    m_imageList->Add(wxIcon(wxT("wxICON_SMALL_REMOVEABLE"), wxBITMAP_TYPE_RESOURCE));
#else
#error "Sorry, we don't have icons available for this platforms."
#endif
    m_treeCtrl->SetImageList(m_imageList);

    m_showHidden = FALSE;
    wxDirItemDataEx* rootData = new wxDirItemDataEx(wxT(""), wxT(""), TRUE);

    wxString rootName;

#ifdef __WXMSW__
    rootName = _("Computer");
#else
    rootName = _("Sections");
#endif

    m_rootId = m_treeCtrl->AddRoot( rootName, 3, -1, rootData);
    m_treeCtrl->SetItemHasChildren(m_rootId);
    m_treeCtrl->Expand(m_rootId); // automatically expand first level

    // Expand and select the default path
    if (!m_defaultPath.IsEmpty())
        ExpandPath(m_defaultPath);

    DoResize();

    return TRUE;
}

wxGenericDirCtrl::~wxGenericDirCtrl()
{
   m_treeCtrl->SetImageList(NULL);
   delete m_imageList;
}

void wxGenericDirCtrl::Init()
{
    m_showHidden = FALSE;
    m_imageList = NULL;
    m_currentFilter = 0;
    m_currentFilterStr = wxEmptyString; // Default: any file
    m_treeCtrl = NULL;
    m_filterListCtrl = NULL;
}

void wxGenericDirCtrl::AddSection(const wxString& path, const wxString& name, int imageId)
{
    wxDirItemDataEx *dir_item = new wxDirItemDataEx(path,name,TRUE);

#ifdef __WXMSW__
    // Windows: sections are displayed as drives
    wxTreeItemId id = m_treeCtrl->AppendItem( m_rootId, name, imageId, -1, dir_item);
#else
    // Unix: sections are displayed as folders
    wxTreeItemId id = m_treeCtrl->AppendItem( m_rootId, name, 0, -1, dir_item);
    m_treeCtrl->SetItemImage( id, 1, wxTreeItemIcon_Expanded );
#endif
    // TODO: other operating systems.

    m_treeCtrl->SetItemHasChildren(id);
}

void wxGenericDirCtrl::SetupSections()
{
#ifdef __WXMSW__

#ifdef __WIN32__
    wxChar driveBuffer[256];
    size_t n = (size_t) GetLogicalDriveStrings(255, driveBuffer);
    size_t i = 0;
    while (i < n)
    {
        wxString path, name;
        path.Printf(wxT("%c:\\"), driveBuffer[i]);
        name.Printf(wxT("(%c:)"), driveBuffer[i]);

        int imageId = 4;
        int driveType = ::GetDriveType(path);
        switch (driveType)
        {
            case DRIVE_REMOVABLE:
                if (path == wxT("a:\\") || path == wxT("b:\\"))
                    imageId = 6; // Floppy
                else
                    imageId = 7;
                break;
            case DRIVE_FIXED:
                imageId = 4;
                break;
            case DRIVE_REMOTE:
                imageId = 4;
                break;
            case DRIVE_CDROM:
                imageId = 5;
                break;
            default:
                imageId = 4;
                break;
        }

        AddSection(path, name, imageId);

        while (driveBuffer[i] != wxT('\0'))
            i ++;
        i ++;
        if (driveBuffer[i] == wxT('\0'))
            break;
    }
#else
    int drive;
    int currentDrive;

    /* Save current drive. */
    currentDrive = _getdrive();

    /* If we can switch to the drive, it exists. */
    for( drive = 1; drive <= 26; drive++ )
    {
        wxString path, name;
        path.Printf(wxT("%c:\\"), (char) (drive + 'a' - 1));
        name.Printf(wxT("(%c:)"), (char) (drive + 'a' - 1));

        if( !_chdrive( drive ) )
        {
            
            AddSection(path, name);
        }
    }

    /* Restore original drive.*/
    _chdrive( currentDrive );
#endif

#else
  AddSection(wxT("/"), _("The Computer"), 0);
  AddSection(wxGetHomeDir(), _("My Home"), 0 );
  AddSection(wxT("/mnt"), _("Mounted Devices"), 0 );
  AddSection(wxT("/usr/local"), _("User Local"), 0 );
  AddSection(wxT("/usr"), _("User"), 0 );
  AddSection(wxT("/var"), _("Variables"), 0 );
  AddSection(wxT("/etc"), _("Etcetera"), 0 );
  AddSection(wxT("/tmp"), _("Temporary"), 0 );
#endif
}

void wxGenericDirCtrl::OnBeginEditItem(wxTreeEvent &event)
{
    // don't rename the main entry "Sections"
    if (event.GetItem() == m_rootId)
    {
        event.Veto();
        return;
    }

    // don't rename the individual sections
    if (m_treeCtrl->GetParent( event.GetItem() ) == m_rootId)
    {
        event.Veto();
        return;
    }
}

void wxGenericDirCtrl::OnEndEditItem(wxTreeEvent &event)
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
    wxDirItemDataEx *data = (wxDirItemDataEx*)m_treeCtrl->GetItemData( id );
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

void wxGenericDirCtrl::OnExpandItem(wxTreeEvent &event)
{
    wxTreeItemId parentId = event.GetItem();

    ExpandDir(parentId);
}

void wxGenericDirCtrl::OnCollapseItem(wxTreeEvent &event )
{
    wxTreeItemId child, parent = event.GetItem();

    wxDirItemDataEx *data = (wxDirItemDataEx *) m_treeCtrl->GetItemData(event.GetItem());
    if (!data->m_isExpanded)
        return;

    data->m_isExpanded = FALSE;
    long cookie;
    /* Workaround because DeleteChildren has disapeared (why?) and
     * CollapseAndReset doesn't work as advertised (deletes parent too) */
    child = m_treeCtrl->GetFirstChild(parent, cookie);
    while (child.IsOk())
    {
        m_treeCtrl->Delete(child);
        /* Not GetNextChild below, because the cookie mechanism can't
         * handle disappearing children! */
        child = m_treeCtrl->GetFirstChild(parent, cookie);
    }
}

void wxGenericDirCtrl::ExpandDir(wxTreeItemId parentId)
{
    wxDirItemDataEx *data = (wxDirItemDataEx *) m_treeCtrl->GetItemData(parentId);

    if (data->m_isExpanded)
        return;

    data->m_isExpanded = TRUE;

    if (parentId == m_rootId)
    {
        SetupSections();
        return;
    }

    wxASSERT(data);

    wxString search,path,filename;

    wxString dirName(data->m_path);

#ifdef __WXMSW__
    // Check if this is a root directory and if so,
    // whether the drive is avaiable.
    if (dirName.Len() == 3 && dirName[1] == wxT(':'))
    {
        int currentDrive = _getdrive();
        int thisDrive = (int) (dirName[0] - 'a' + 1) ;
        int err = _chdrive( thisDrive ) ;
        _chdrive( currentDrive );

        if (err == -1)
        {
            data->m_isExpanded = FALSE;
            wxMessageBox(wxT("Sorry, this drive is not available."));
            return;
        }
    }
#endif

    // This may take a longish time. Go to busy cursor
    wxBusyCursor busy;

#ifdef __WXMSW__
    if (dirName.Last() == ':')
        dirName += wxString(wxFILE_SEP_PATH);
#endif

    wxArrayString dirs;
    wxArrayString filenames;

    wxDir d;
    wxString eachFilename;

    d.Open(dirName);

    if (d.IsOpened())
    {
        if (d.GetFirst(& eachFilename, wxEmptyString, wxDIR_DIRS))
        {
            do
            {
                if ((eachFilename != wxT(".")) && (eachFilename != wxT("..")))
                {
                    dirs.Add(eachFilename);
                }
            }
            while (d.GetNext(& eachFilename)) ;
        }
    }
    dirs.Sort();

    // Now do the filenames -- but only if we're allowed to
    if ((GetWindowStyle() & wxDIRCTRL_DIR_ONLY) == 0)
    {
        d.Open(dirName);
        
        if (d.IsOpened())
        {
            if (d.GetFirst(& eachFilename, m_currentFilterStr, wxDIR_FILES))
            {
                do
                {
                    if ((eachFilename != wxT(".")) && (eachFilename != wxT("..")))
                    {
                        filenames.Add(eachFilename);
                    }
                }
                while (d.GetNext(& eachFilename)) ;
            }
        }
        filenames.Sort();
    }

    // Add the sorted dirs
    size_t i;
    for (i = 0; i < dirs.Count(); i++)
    {
        wxString eachFilename(dirs[i]);
        path = dirName;
        if (path.Last() != wxFILE_SEP_PATH)
            path += wxString(wxFILE_SEP_PATH);
        path += eachFilename;

        wxDirItemDataEx *dir_item = new wxDirItemDataEx(path,eachFilename,TRUE);
        wxTreeItemId id = m_treeCtrl->AppendItem( parentId, eachFilename, 0, -1, dir_item);
        m_treeCtrl->SetItemImage( id, 1, wxTreeItemIcon_Expanded );
        
        // Has this got any children? If so, make it expandable.
        int options = wxDIR_DEFAULT;
        if (GetWindowStyle() & wxDIRCTRL_DIR_ONLY) // If only showing dirs, then we specify dirs only here
        {
            options = wxDIR_DIRS;
        }

        wxDir dir2(path);
        wxString str;
        // Have to test for wxDIR_DIRS separately in case m_currentFilterStr is non-empty and
        // and filters out any directories
        if (dir2.GetFirst(& str, m_currentFilterStr, options) || dir2.GetFirst(& str, wxEmptyString, wxDIR_DIRS))
        {
            m_treeCtrl->SetItemHasChildren(id);
        }
    }

    // Add the sorted filenames
    if ((GetWindowStyle() & wxDIRCTRL_DIR_ONLY) == 0)
    {
        for (i = 0; i < filenames.Count(); i++)
        {
            wxString eachFilename(filenames[i]);
            path = dirName;
            if (path.Last() != wxFILE_SEP_PATH)
                path += wxString(wxFILE_SEP_PATH);
            path += eachFilename;
            //path = dirName + wxString(wxT("/")) + eachFilename;
            wxDirItemDataEx *dir_item = new wxDirItemDataEx(path,eachFilename,FALSE);
            (void)m_treeCtrl->AppendItem( parentId, eachFilename, 2, -1, dir_item);
        }
    }
}

// Find the child that matches the first part of 'path'.
// E.g. if a child path is "/usr" and 'path' is "/usr/include"
// then the child for /usr is returned.
wxTreeItemId wxGenericDirCtrl::FindChild(wxTreeItemId parentId, const wxString& path, bool& done)
{
    wxString path2(path);
    
    // Make sure all separators are as per the current platform
    path2.Replace(wxT("\\"), wxString(wxFILE_SEP_PATH));
    path2.Replace(wxT("/"), wxString(wxFILE_SEP_PATH));
    
    // Append a separator to foil bogus substring matching
    path2 += wxString(wxFILE_SEP_PATH);
    
    // In MSW, case is not significant
#ifdef __WXMSW__
    path2.MakeLower();
#endif
    
    long cookie;
    wxTreeItemId childId = m_treeCtrl->GetFirstChild(parentId, cookie);
    while (childId != 0)
    {
        wxDirItemDataEx* data = (wxDirItemDataEx*) m_treeCtrl->GetItemData(childId);
        
        if (data && data->m_path != "")
        {
            wxString childPath(data->m_path);
            if (childPath.Last() != wxFILE_SEP_PATH)
                childPath += wxString(wxFILE_SEP_PATH);
            
            // In MSW, case is not significant
#ifdef __WXMSW__
            childPath.MakeLower();
#endif
            
            if (childPath.Len() <= path2.Len())
            {
                wxString path3 = path2.Mid(0, childPath.Len());
                if (childPath == path3)
                {
                    if (path3.Len() == path2.Len())
                        done = TRUE;
                    else
                        done = FALSE;
                    return childId;
                }
            }
        }
        
        childId = m_treeCtrl->GetNextChild(childId, cookie);
    }
    wxTreeItemId invalid;
    return invalid;
}

// Try to expand as much of the given path as possible,
// and select the given tree item.
bool wxGenericDirCtrl::ExpandPath(const wxString& path)
{
    bool done = FALSE;
    wxTreeItemId id = FindChild(m_rootId, path, done);
    wxTreeItemId lastId = id; // The last non-zero id
    while ((id > 0) && !done)
    {
        ExpandDir(id);

        id = FindChild(id, path, done);
        if (id != 0)
            lastId = id;
    }
    if (lastId > 0)
    {
        wxDirItemDataEx *data = (wxDirItemDataEx *) m_treeCtrl->GetItemData(lastId);
        if (data->m_isDir)
        {
            m_treeCtrl->Expand(lastId);
        }
        if ((GetWindowStyle() & wxDIRCTRL_SELECT_FIRST) && data->m_isDir)
        {
            // Find the first file in this directory
            long cookie;
            wxTreeItemId childId = m_treeCtrl->GetFirstChild(lastId, cookie);
            bool selectedChild = FALSE;
            while (childId != 0)
            {
                wxDirItemDataEx* data = (wxDirItemDataEx*) m_treeCtrl->GetItemData(childId);
                
                if (data && data->m_path != "" && !data->m_isDir)
                {
                    m_treeCtrl->SelectItem(childId);
                    m_treeCtrl->EnsureVisible(childId);
                    selectedChild = TRUE;
                    break;
                }
                childId = m_treeCtrl->GetNextChild(lastId, cookie);
            }
            if (!selectedChild)
            {
                m_treeCtrl->SelectItem(lastId);
                m_treeCtrl->EnsureVisible(lastId);
            }
        }
        else
        {
            m_treeCtrl->SelectItem(lastId);
            m_treeCtrl->EnsureVisible(lastId);
        }

        return TRUE;
    }
    else
        return FALSE;
}

wxString wxGenericDirCtrl::GetPath() const
{
    wxTreeItemId id = m_treeCtrl->GetSelection();
    if (id)
    {
        wxDirItemDataEx* data = (wxDirItemDataEx*) m_treeCtrl->GetItemData(id);
        return data->m_path;
    }
    else
        return wxEmptyString;
}

wxString wxGenericDirCtrl::GetFilePath() const
{
    wxTreeItemId id = m_treeCtrl->GetSelection();
    if (id)
    {
        wxDirItemDataEx* data = (wxDirItemDataEx*) m_treeCtrl->GetItemData(id);
        if (data->m_isDir)
            return wxEmptyString;
        else
            return data->m_path;
    }
    else
        return wxEmptyString;
}

void wxGenericDirCtrl::SetPath(const wxString& path)
{
    m_defaultPath = path;
    if (m_rootId)
        ExpandPath(path);
}

// Not used
#if 0
void wxGenericDirCtrl::FindChildFiles(wxTreeItemId id, int dirFlags, wxArrayString& filenames)
{
    wxDirItemDataEx *data = (wxDirItemDataEx *) m_treeCtrl->GetItemData(id);

    // This may take a longish time. Go to busy cursor
    wxBusyCursor busy;

    wxASSERT(data);

    wxString search,path,filename;

    wxString dirName(data->m_path);

#ifdef __WXMSW__
    if (dirName.Last() == ':')
        dirName += wxString(wxFILE_SEP_PATH);
#endif

    wxDir d;
    wxString eachFilename;

    d.Open(dirName);

    if (d.IsOpened())
    {
        if (d.GetFirst(& eachFilename, m_currentFilterStr, dirFlags))
        {
            do
            {
                if ((eachFilename != wxT(".")) && (eachFilename != wxT("..")))
                {
                    filenames.Add(eachFilename);
                }
            }
            while (d.GetNext(& eachFilename)) ;
        }
    }
}
#endif

void wxGenericDirCtrl::SetFilterIndex(int n)
{
    m_currentFilter = n;

    wxString f, d;
    if (ExtractWildcard(m_filter, n, f, d))
        m_currentFilterStr = f;
    else
        m_currentFilterStr = wxT("*.*");
}

void wxGenericDirCtrl::SetFilter(const wxString& filter)
{
    m_filter = filter;

    wxString f, d;
    if (ExtractWildcard(m_filter, m_currentFilter, f, d))
        m_currentFilterStr = f;
    else
        m_currentFilterStr = wxT("*.*");
}

// Extract description and actual filter from overall filter string
bool wxGenericDirCtrl::ExtractWildcard(const wxString& filterStr, int n, wxString& filter, wxString& description)
{
    wxArrayString filters, descriptions;
    int count = ParseFilter(filterStr, filters, descriptions);
    if (count > 0 && n < count)
    {
        filter = filters[n];
        description = descriptions[n];
        return TRUE;
    }
    else
        return FALSE;
}

// Parses the global filter, returning the number of filters.
// Returns 0 if none or if there's a problem.
// filterStr is in the form:
//
// "All files (*.*)|*.*|JPEG Files (*.jpeg)|*.jpg"

int wxGenericDirCtrl::ParseFilter(const wxString& filterStr, wxArrayString& filters, wxArrayString& descriptions)
{
    wxString str(filterStr);

    wxString description, filter;
    int pos;
    bool finished = FALSE;
    do
    {
        pos = str.Find(wxT('|'));
        if (pos == -1)
            return 0; // Problem
        description = str.Left(pos);
        str = str.Mid(pos+1);
        pos = str.Find(wxT('|'));
        if (pos == -1)
        {
            filter = str;
            finished = TRUE;
        }
        else
        {
            filter = str.Left(pos);
            str = str.Mid(pos+1);
        }
        descriptions.Add(description);
        filters.Add(filter);
    }
    while (!finished) ;

    return filters.Count();
}

void wxGenericDirCtrl::DoResize()
{
    wxSize sz = GetClientSize();
    int verticalSpacing = 3;
    if (m_treeCtrl)
    {
        wxSize filterSz ;
        if (m_filterListCtrl)
        {
            filterSz = m_filterListCtrl->GetSize();
            sz.y -= (filterSz.y + verticalSpacing);
        }
        m_treeCtrl->SetSize(0, 0, sz.x, sz.y);
        if (m_filterListCtrl)
        {
            m_filterListCtrl->SetSize(0, sz.y + verticalSpacing, sz.x, filterSz.y);
            // Don't know why, but this needs refreshing after a resize (wxMSW)
            m_filterListCtrl->Refresh();
        }
    }
}


void wxGenericDirCtrl::OnSize(wxSizeEvent &event)
{
    DoResize();
}

//-----------------------------------------------------------------------------
// wxDirFilterListCtrl
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxDirFilterListCtrl, wxChoice)

BEGIN_EVENT_TABLE(wxDirFilterListCtrl, wxChoice)
    EVT_CHOICE(-1, wxDirFilterListCtrl::OnSelFilter)
END_EVENT_TABLE()

bool wxDirFilterListCtrl::Create(wxGenericDirCtrl* parent, const wxWindowID id,
              const wxPoint& pos,
              const wxSize& size,
              long style)
{
    m_dirCtrl = parent;
    return wxChoice::Create(parent, id, pos, size, 0, NULL, style);
}

void wxDirFilterListCtrl::Init()
{
    m_dirCtrl = NULL;
}

void wxDirFilterListCtrl::OnSelFilter(wxCommandEvent& event)
{
    int sel = GetSelection();

    wxString currentPath = m_dirCtrl->GetPath();
        
    m_dirCtrl->SetFilterIndex(sel);

    // If the filter has changed, the view is out of date, so
    // collapse the tree.
    m_dirCtrl->GetTreeCtrl()->Collapse(m_dirCtrl->GetRootId());
    m_dirCtrl->GetTreeCtrl()->Expand(m_dirCtrl->GetRootId());

    // Try to restore the selection, or at least the directory
    m_dirCtrl->ExpandPath(currentPath);
}

void wxDirFilterListCtrl::FillFilterList(const wxString& filter, int defaultFilter)
{
    Clear();
    wxArrayString descriptions, filters;
    size_t n = (size_t) m_dirCtrl->ParseFilter(filter, filters, descriptions);

    if (n > 0 && defaultFilter < (int) n)
    {
        size_t i = 0;
        for (i = 0; i < n; i++)
            Append(descriptions[i]);
        SetSelection(defaultFilter);
    }
}

// wxGenericDirDialog implementation
// This should be moved into dirdlgg.cpp eventually

BEGIN_EVENT_TABLE(wxGenericDirDialog, wxDialog)
    EVT_BUTTON(wxID_OK,  wxGenericDirDialog::OnOK)
    EVT_CLOSE(wxGenericDirDialog::OnCloseWindow)
END_EVENT_TABLE()

wxGenericDirDialog::wxGenericDirDialog(wxWindow* parent, const wxString& title,
        const wxString& defaultPath, long style, const wxPoint& pos, const wxSize& sz, const wxString& name):
   wxDialog(parent, ID_DIRCTRL, title, pos, sz, style, name)
{
    wxBusyCursor cursor;

    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

    // 1) dir ctrl
    m_dirCtrl = new wxGenericDirCtrl(this, ID_DIRCTRL,
        defaultPath, wxPoint(5, 5),
        wxSize(300, 200), wxDIRCTRL_DIR_ONLY);

    topsizer->Add( m_dirCtrl, 1, wxTOP|wxLEFT|wxRIGHT | wxEXPAND, 10 );

    // 2) TODO: text control for entering path?

#if wxUSE_STATLINE
    // 3) Static line
    topsizer->Add( new wxStaticLine( this, -1 ), 0, wxEXPAND | wxLEFT|wxRIGHT|wxTOP, 10 );
#endif

    // 4) Buttons
    wxSizer* buttonsizer = new wxBoxSizer( wxHORIZONTAL );
    wxButton* okButton = new wxButton(this, wxID_OK, _("OK"));
    buttonsizer->Add( okButton, 0, wxLEFT|wxRIGHT, 10 );
    wxButton* cancelButton = new wxButton(this, wxID_CANCEL, _("Cancel"));
    buttonsizer->Add( cancelButton, 0, wxLEFT|wxRIGHT, 10 );

/* TODO: new directory button
    wxButton* newButton = new wxButton( this, ID_NEW, _("New...") );
    buttonsizer->Add( newButton, 0, wxLEFT|wxRIGHT, 10 );
*/
    topsizer->Add( buttonsizer, 0, wxALL | wxCENTER, 10 );

    okButton->SetDefault();
    m_dirCtrl->SetFocus();

    SetAutoLayout( TRUE );
    SetSizer( topsizer );

    topsizer->SetSizeHints( this );
    topsizer->Fit( this );

    Centre( wxBOTH );
}

void wxGenericDirDialog::OnCloseWindow(wxCloseEvent& event)
{
    EndModal(wxID_CANCEL);
}

void wxGenericDirDialog::OnOK(wxCommandEvent& event)
{
    EndModal(wxID_OK);
}

void wxGenericDirDialog::SetPath(const wxString& path)
{
    m_dirCtrl->SetPath(path);
}

wxString wxGenericDirDialog::GetPath(void) const
{
    return m_dirCtrl->GetPath();
}
