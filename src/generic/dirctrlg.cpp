/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/dirctrlg.cpp
// Purpose:     wxGenericDirCtrl
// Author:      Harm van der Heijden, Robert Roebling, Julian Smart
// Created:     12/12/98
// Copyright:   (c) Harm van der Heijden, Robert Roebling and Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_DIRDLG || wxUSE_FILEDLG

#include "wx/generic/dirctrlg.h"

#ifndef WX_PRECOMP
    #include "wx/hash.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/utils.h"
    #include "wx/button.h"
    #include "wx/icon.h"
    #include "wx/settings.h"
    #include "wx/msgdlg.h"
    #include "wx/choice.h"
    #include "wx/textctrl.h"
    #include "wx/layout.h"
    #include "wx/sizer.h"
    #include "wx/textdlg.h"
    #include "wx/gdicmn.h"
    #include "wx/image.h"
    #include "wx/module.h"
#endif

#include "wx/filename.h"
#include "wx/filefn.h"
#include "wx/imaglist.h"
#include "wx/tokenzr.h"
#include "wx/dir.h"
#include "wx/artprov.h"
#include "wx/mimetype.h"

#if wxUSE_STATLINE
    #include "wx/statline.h"
#endif

#if defined(__WXMAC__)
    #include  "wx/osx/private.h"  // includes mac headers
#endif

#if defined(__WINDOWS__) || defined(__APPLE__)
#include "wx/volume.h"
#endif

#ifdef __WINDOWS__
#include <windows.h>
#include "wx/msw/winundef.h"

// MinGW has _getdrive() and _chdrive(), Cygwin doesn't.
#if defined(__GNUWIN32__) && !defined(__CYGWIN__)
    #define wxHAS_DRIVE_FUNCTIONS
#endif

#ifdef wxHAS_DRIVE_FUNCTIONS
    #include <direct.h>
#endif

#endif // __WINDOWS__

#if defined(__WXMAC__)
//    #include "MoreFilesX.h"
#endif

extern WXDLLEXPORT_DATA(const char) wxFileSelectorDefaultWildcardStr[];

// If compiled under Windows, this macro can cause problems
#ifdef GetFirstChild
#undef GetFirstChild
#endif

bool wxIsDriveAvailable(const wxString& dirName);

namespace
{

bool wxDirCtrlPathsEqual(const wxString& first, const wxString& second)
{
    const auto makeKey =
        [](const wxString& path)
        {
            wxString key(path);
            key.Replace("\\", wxString(wxFILE_SEP_PATH));
            key.Replace("/", wxString(wxFILE_SEP_PATH));
            while ( key.length() > 1 && wxEndsWithPathSeparator(key) )
                key.RemoveLast();
#if defined(__WINDOWS__)
            key.MakeLower();
#endif
            return key;
        };

    return makeKey(first) == makeKey(second);
}

// wxWindowUpdateLocker keeps a raw pointer and so can't be used around the
// virtual hooks and tree events published by this control. This variant owns
// exactly one Freeze()/Thaw() level while allowing either the dir control or
// its public child tree to be destroyed from a callback.
class wxDirTreeUpdateLocker final
{
public:
    explicit wxDirTreeUpdateLocker(wxTreeCtrl* tree)
        : m_tree(tree)
    {
        if ( tree )
            tree->Freeze();
    }

    ~wxDirTreeUpdateLocker()
    {
        wxTreeCtrl* const tree = m_tree.get();
        if ( tree && tree->IsFrozen() )
            tree->Thaw();
    }

private:
    wxWeakRef<wxTreeCtrl> m_tree;

    wxDECLARE_NO_COPY_CLASS(wxDirTreeUpdateLocker);
};

} // namespace

// ----------------------------------------------------------------------------
// events
// ----------------------------------------------------------------------------

wxDEFINE_EVENT( wxEVT_DIRCTRL_SELECTIONCHANGED, wxTreeEvent );
wxDEFINE_EVENT( wxEVT_DIRCTRL_FILEACTIVATED, wxTreeEvent );

// ----------------------------------------------------------------------------
// wxGetAvailableDrives, for WINDOWS, OSX, UNIX (returns "/")
// ----------------------------------------------------------------------------

size_t wxGetAvailableDrives(wxArrayString &paths, wxArrayString &names, wxArrayInt &icon_ids)
{
#if defined(wxHAS_FILESYSTEM_VOLUMES) || defined(__APPLE__)

#if (defined(__WIN32__) || defined(__APPLE__)) && wxUSE_FSVOLUME
    // TODO: this code (using wxFSVolumeBase) should be used for all platforms
    //       but unfortunately wxFSVolumeBase is not implemented everywhere
    const wxArrayString as = wxFSVolumeBase::GetVolumes();

    for (size_t i = 0; i < as.GetCount(); i++)
    {
        wxString path = as[i];
        wxFSVolume vol(path);
        int imageId;
        switch (vol.GetKind())
        {
            case wxFS_VOL_FLOPPY:
                if ( (path == wxT("a:\\")) || (path == wxT("b:\\")) )
                    imageId = wxFileIconsTable::floppy;
                else
                    imageId = wxFileIconsTable::removeable;
                break;
            case wxFS_VOL_DVDROM:
            case wxFS_VOL_CDROM:
                imageId = wxFileIconsTable::cdrom;
                break;
            case wxFS_VOL_NETWORK:
                if (path[0] == wxT('\\'))
                    continue; // skip "\\computer\folder"
                imageId = wxFileIconsTable::drive;
                break;
            case wxFS_VOL_DISK:
            case wxFS_VOL_OTHER:
            default:
                imageId = wxFileIconsTable::drive;
                break;
        }
        paths.Add(path);
        names.Add(vol.GetDisplayName());
        icon_ids.Add(imageId);
    }
#else // !__WIN32__
    /* If we can switch to the drive, it exists. */
    for ( char drive = 'A'; drive <= 'Z'; drive++ )
    {
        const wxString
            path = wxFileName::GetVolumeString(drive, wxPATH_GET_SEPARATOR);

        if (wxIsDriveAvailable(path))
        {
            paths.Add(path);
            names.Add(wxFileName::GetVolumeString(drive, wxPATH_NO_SEPARATOR));
            icon_ids.Add(drive <= 2 ? wxFileIconsTable::floppy
                                    : wxFileIconsTable::drive);
        }
    }
#endif // __WIN32__/!__WIN32__

#elif defined(__UNIX__)
    paths.Add(wxT("/"));
    names.Add(wxT("/"));
    icon_ids.Add(wxFileIconsTable::computer);
#else
    #error "Unsupported platform in wxGenericDirCtrl!"
#endif
    wxASSERT_MSG( (paths.GetCount() == names.GetCount()), wxT("The number of paths and their human readable names should be equal in number."));
    wxASSERT_MSG( (paths.GetCount() == icon_ids.GetCount()), wxT("Wrong number of icons for available drives."));
    return paths.GetCount();
}

// ----------------------------------------------------------------------------
// wxIsDriveAvailable
// ----------------------------------------------------------------------------

#if defined(__WINDOWS__)

int setdrive(int drive)
{
#if defined(wxHAS_DRIVE_FUNCTIONS)
    return _chdrive(drive);
#else
    wxChar  newdrive[4];

    if (drive < 1 || drive > 31)
        return -1;
    newdrive[0] = (wxChar)(wxT('A') + drive - 1);
    newdrive[1] = wxT(':');
    newdrive[2] = wxT('\0');
#if defined(__WINDOWS__)
    if (::SetCurrentDirectory(newdrive))
#else
    // VA doesn't know what LPSTR is and has its own set
    if (!DosSetCurrentDir((PSZ)newdrive))
#endif
        return 0;
    else
        return -1;
#endif // !GNUWIN32
}

bool wxIsDriveAvailable(const wxString& dirName)
{
#ifdef __WIN32__
    UINT errorMode = SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
#endif
    bool success = true;

    // Check if this is a root directory and if so,
    // whether the drive is available.
    if (dirName.length() == 3 && dirName[(size_t)1] == wxT(':'))
    {
        wxString dirNameLower(dirName.Lower());
#ifndef wxHAS_DRIVE_FUNCTIONS
        success = wxDirExists(dirNameLower);
#else
        int currentDrive = _getdrive();
        int thisDrive = (int) (dirNameLower[(size_t)0] - 'a' + 1) ;
        int err = setdrive( thisDrive ) ;
        setdrive( currentDrive );

        if (err == -1)
        {
            success = false;
        }
#endif
    }
#ifdef __WIN32__
    (void) SetErrorMode(errorMode);
#endif

    return success;
}
#endif // __WINDOWS__

#endif // wxUSE_DIRDLG || wxUSE_FILEDLG



#if wxUSE_DIRDLG

//-----------------------------------------------------------------------------
// wxDirItemData
//-----------------------------------------------------------------------------

wxDirItemData::wxDirItemData(const wxString& path, const wxString& name,
                             bool isDir)
    : m_path(path)
    , m_name(name)
{
    /* Insert logic to detect hidden files here
     * In UnixLand we just check whether the first char is a dot
     * For FileNameFromPath read LastDirNameInThisPath ;-) */
    // m_isHidden = (bool)(wxFileNameFromPath(*m_path)[0] == '.');
    m_isHidden = false;
    m_isExpanded = false;
    m_isDir = isDir;
}

void wxDirItemData::SetNewDirName(const wxString& path)
{
    m_path = path;
    m_name = wxFileNameFromPath(path);
}

bool wxDirItemData::HasSubDirs() const
{
    if (m_path.empty())
        return false;

    wxDir dir;
    {
        wxLogNull nolog;
        if ( !dir.Open(m_path) )
            return false;
    }

    return dir.HasSubDirs();
}

bool wxDirItemData::HasFiles(const wxString& WXUNUSED(spec)) const
{
    if (m_path.empty())
        return false;

    wxDir dir;
    {
        wxLogNull nolog;
        if ( !dir.Open(m_path) )
            return false;
    }

    return dir.HasFiles();
}

//-----------------------------------------------------------------------------
// wxGenericDirCtrl
//-----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(wxGenericDirCtrl, wxControl)
  EVT_TREE_ITEM_EXPANDING     (wxID_TREECTRL, wxGenericDirCtrl::OnExpandItem)
  EVT_TREE_ITEM_COLLAPSED     (wxID_TREECTRL, wxGenericDirCtrl::OnCollapseItem)
  EVT_TREE_BEGIN_LABEL_EDIT   (wxID_TREECTRL, wxGenericDirCtrl::OnBeginEditItem)
  EVT_TREE_END_LABEL_EDIT     (wxID_TREECTRL, wxGenericDirCtrl::OnEndEditItem)
  EVT_TREE_SEL_CHANGED        (wxID_TREECTRL, wxGenericDirCtrl::OnTreeSelChange)
  EVT_TREE_ITEM_ACTIVATED     (wxID_TREECTRL, wxGenericDirCtrl::OnItemActivated)
  EVT_SIZE                    (wxGenericDirCtrl::OnSize)
wxEND_EVENT_TABLE()

wxGenericDirCtrl::wxGenericDirCtrl(void)
{
    Init();
}

void wxGenericDirCtrl::ExpandRoot()
{
    ExpandDir(m_rootId); // automatically expand first level

    // Expand and select the default path
    if (!m_defaultPath.empty())
    {
        ExpandPath(m_defaultPath);
    }
#ifdef __UNIX__
    else
    {
        // On Unix, there's only one node under the (hidden) root node. It
        // represents the / path, so the user would always have to expand it;
        // let's do it ourselves
        ExpandPath( wxT("/") );
    }
#endif
}

bool wxGenericDirCtrl::Create(wxWindow *parent,
                              wxWindowID treeid,
                              const wxString& dir,
                              const wxPoint& pos,
                              const wxSize& size,
                              long style,
                              const wxString& filter,
                              int defaultFilter,
                              const wxString& name)
{
    if (!wxControl::Create(parent, treeid, pos, size, style, wxDefaultValidator, name))
        return false;

    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
    SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));

    long treeStyle = wxTR_HAS_BUTTONS;

    treeStyle |= wxTR_HIDE_ROOT;

#ifdef __WXGTK__
    treeStyle |= wxTR_NO_LINES;
#elif defined(__WXMSW__)
    treeStyle |= wxTR_NO_LINES | wxTR_TWIST_BUTTONS | wxTR_FULL_ROW_HIGHLIGHT;
#endif

    if (style & wxDIRCTRL_EDIT_LABELS)
        treeStyle |= wxTR_EDIT_LABELS;

    if (style & wxDIRCTRL_MULTIPLE)
        treeStyle |= wxTR_MULTIPLE;

    if ((style & wxDIRCTRL_3D_INTERNAL) == 0)
        treeStyle |= wxNO_BORDER;

    wxTreeCtrl* const treeCtrl =
        CreateTreeCtrl(this, wxID_TREECTRL,
                       wxPoint(0,0), GetClientSize(), treeStyle);
    if ( !treeCtrl )
        return false;
    m_treeCtrl = wxWeakRef<wxTreeCtrl>(treeCtrl);

    if (!filter.empty() && (style & wxDIRCTRL_SHOW_FILTERS))
    {
        wxDirFilterListCtrl* const filterList =
            new wxDirFilterListCtrl;
        if ( !filterList->Create(this, wxID_FILTERLISTCTRL) )
        {
            delete filterList;
            return false;
        }
        m_filterListCtrl = wxWeakRef<wxDirFilterListCtrl>(filterList);
    }

    m_defaultPath = dir;
    m_filter = filter;

    if (m_filter.empty())
        m_filter = wxFileSelectorDefaultWildcardStr;

    wxArrayString filterDescriptions;
    wxArrayString filterWildcards;
    const int filterCount =
        wxParseCommonDialogsFilter(
            m_filter, filterDescriptions, filterWildcards);
    const int initialFilter =
        defaultFilter >= 0 && defaultFilter < filterCount ? defaultFilter : 0;

    SetFilterIndex(initialFilter);

    if (m_filterListCtrl)
        m_filterListCtrl->FillFilterList(filter, initialFilter);

    // TODO: set the icon size according to current scaling for this window.
    // Currently, there's insufficient API in wxWidgets to determine what icons
    // are available and whether to take the nearest size according to a tolerance
    // instead of scaling.
    // if (!wxTheFileIconsTable->IsOk())
    //     wxTheFileIconsTable->SetSize(scaledSize);

    // Meanwhile, in your application initialisation, where you have better knowledge of what
    // icons are available and whether to scale, you can do this:
    //
    // wxTheFileIconsTable->SetSize(calculatedIconSizeForDPI);
    //
    // Obviously this can't take into account monitors with different DPI.
    m_treeCtrl->SetImageList(wxTheFileIconsTable->GetSmallImageList());

    m_showHidden = false;
    wxDirItemData* rootData = new wxDirItemData(wxEmptyString, wxEmptyString, true);

    wxString rootName;

#if defined(__WINDOWS__)
    rootName = _("Computer");
#else
    rootName = _("Sections");
#endif

    m_rootId = m_treeCtrl->AddRoot( rootName, 3, -1, rootData);
    m_treeCtrl->SetItemHasChildren(m_rootId);

    ExpandRoot();

    SetInitialSize(size);
    DoResize();

    return true;
}

wxGenericDirCtrl::~wxGenericDirCtrl()
{
}

void wxGenericDirCtrl::Init()
{
    m_showHidden = false;
    m_currentFilter = 0;
    m_currentFilterStr.clear(); // Default: any file
    m_treeCtrl.Release();
    m_filterListCtrl.Release();
}

wxTreeCtrl* wxGenericDirCtrl::CreateTreeCtrl(wxWindow *parent, wxWindowID treeid, const wxPoint& pos, const wxSize& size, long treeStyle)
{
    return new wxTreeCtrl(parent, treeid, pos, size, treeStyle);
}

void wxGenericDirCtrl::ShowHidden( bool show )
{
    if ( m_showHidden == show )
        return;

    m_showHidden = show;
    const wxWeakRef<wxWindow> weakThis(this);

    if ( HasFlag(wxDIRCTRL_MULTIPLE) )
    {
        wxArrayString paths;
        GetPaths(paths);
        ReCreateTree();
        if ( weakThis.get() != this || IsBeingDeleted() )
            return;
        for ( unsigned n = 0; n < paths.size(); n++ )
        {
            ExpandPath(paths[n]);
            if ( weakThis.get() != this || IsBeingDeleted() )
                return;
        }
    }
    else
    {
        wxString path = GetPath();
        ReCreateTree();
        if ( weakThis.get() != this || IsBeingDeleted() )
            return;
        SetPath(path);
    }
}

const wxTreeItemId
wxGenericDirCtrl::AddSection(const wxString& path, const wxString& name, int imageId)
{
    wxTreeCtrl* const treeCtrl = m_treeCtrl.get();
    if ( !treeCtrl || treeCtrl->IsBeingDeleted() )
        return wxTreeItemId();

    const wxWeakRef<wxWindow> weakThis(this);
    wxDirItemData *dir_item = new wxDirItemData(path,name,true);

    const wxTreeItemId inserted =
        AppendItem(m_rootId, name, imageId, -1, dir_item);
    if ( weakThis.get() != this || IsBeingDeleted() ||
         m_treeCtrl.get() != treeCtrl )
    {
        return wxTreeItemId();
    }
    if ( !inserted.IsOk() )
    {
        delete dir_item;
        return wxTreeItemId();
    }

    const wxTreeItemId treeid = FindItem(path);
    if ( treeid.IsOk() )
        treeCtrl->SetItemHasChildren(treeid);

    return treeid;
}

void wxGenericDirCtrl::SetupSections()
{
    wxTreeCtrl* const treeCtrl = m_treeCtrl.get();
    if ( !treeCtrl || treeCtrl->IsBeingDeleted() )
        return;

    const wxTreeItemId rootId = m_rootId;
    wxDirTreeUpdateLocker updateLocker(treeCtrl);
    wxArrayString paths, names;
    wxArrayInt icons;

    size_t n, count = wxGetAvailableDrives(paths, names, icons);
    const wxWeakRef<wxWindow> weakThis(this);

#ifdef __WXGTK__
    wxString home = wxGetHomeDir();
    AddSection( home, _("Home directory"), 1);
    if ( weakThis.get() != this || IsBeingDeleted() ||
         m_treeCtrl.get() != treeCtrl || m_rootId != rootId )
        return;
    home += wxT("/Desktop");
    AddSection( home, _("Desktop"), 1);
    if ( weakThis.get() != this || IsBeingDeleted() ||
         m_treeCtrl.get() != treeCtrl || m_rootId != rootId )
        return;
#endif

    for (n = 0; n < count; n++)
    {
        AddSection(paths[n], names[n], icons[n]);
        if ( weakThis.get() != this || IsBeingDeleted() ||
             m_treeCtrl.get() != treeCtrl || m_rootId != rootId )
            return;
    }
}

void wxGenericDirCtrl::SetFocus()
{
    // we don't need focus ourselves, give it to the tree so that the user
    // could navigate it
    if (m_treeCtrl && !m_treeCtrl->IsBeingDeleted())
        m_treeCtrl->SetFocus();
}

void wxGenericDirCtrl::OnBeginEditItem(wxTreeEvent &event)
{
    wxTreeCtrl* const treeCtrl = m_treeCtrl.get();
    if ( !treeCtrl || treeCtrl->IsBeingDeleted() )
    {
        event.Veto();
        return;
    }

    // don't rename the main entry "Sections"
    if (event.GetItem() == m_rootId)
    {
        event.Veto();
        return;
    }

    // don't rename the individual sections
    if (treeCtrl->GetItemParent( event.GetItem() ) == m_rootId)
    {
        event.Veto();
        return;
    }
}

void wxGenericDirCtrl::OnEndEditItem(wxTreeEvent &event)
{
    if (event.IsEditCancelled())
        return;

    if ((event.GetLabel().empty()) ||
        (event.GetLabel() == wxT(".")) ||
        (event.GetLabel() == wxT("..")) ||
        (event.GetLabel().Find(wxT('/')) != wxNOT_FOUND) ||
        (event.GetLabel().Find(wxT('\\')) != wxNOT_FOUND) ||
        (event.GetLabel().Find(wxT('|')) != wxNOT_FOUND))
    {
        wxMessageDialog dialog(this, _("Illegal directory name."), _("Error"), wxOK | wxICON_ERROR );
        dialog.ShowModal();
        event.Veto();
        return;
    }

    wxTreeItemId treeid = event.GetItem();
    wxDirItemData *data = GetItemData( treeid );
    wxASSERT( data );

    wxString new_name( wxPathOnly( data->m_path ) );
    new_name += wxString(wxFILE_SEP_PATH);
    new_name += event.GetLabel();

    wxLogNull log;

    if (wxFileExists(new_name) || wxDirExists(new_name))
    {
        wxMessageDialog dialog(this, _("File name exists already."), _("Error"), wxOK | wxICON_ERROR );
        dialog.ShowModal();
        event.Veto();
        return;
    }

    if (wxRenameFile(data->m_path,new_name))
    {
        const wxWeakRef<wxWindow> weakThis(this);
        const wxString oldPath(data->m_path);
        CollapseDir(treeid);
        if ( weakThis.get() != this || IsBeingDeleted() || !m_treeCtrl )
            return;

        // Collapse can synchronously publish child deletion events. Resolve
        // the item again instead of retaining wxDirItemData/tree IDs across
        // arbitrary application callbacks.
        const wxTreeItemId liveItem = FindItem(oldPath);
        wxDirItemData* const liveData =
            liveItem.IsOk() ? GetItemData(liveItem) : nullptr;
        if ( liveData && liveData->m_path == oldPath )
            liveData->SetNewDirName(new_name);
        // If an event rebuilt the tree, it already observed the renamed
        // filesystem entry and no in-place data update is necessary.
    }
    else
    {
        wxMessageDialog dialog(this, _("Operation not permitted."), _("Error"), wxOK | wxICON_ERROR );
        dialog.ShowModal();
        event.Veto();
    }
}

void wxGenericDirCtrl::OnTreeSelChange(wxTreeEvent &event)
{
    // Do not leave the originating tree event marked for propagation while
    // publishing the translated public event: an application is allowed to
    // destroy this composite (and hence the child tree) from that callback.
    event.Skip(false);
    const wxWeakRef<wxWindow> weakThis(this);
    wxTreeEvent changedEvent(wxEVT_DIRCTRL_SELECTIONCHANGED, GetId());

    changedEvent.SetEventObject(this);

    const wxTreeItemId item = event.GetItem();
    if ( item.IsOk() )
    {
        changedEvent.SetItem(item);
        changedEvent.SetClientObject(GetItemData(item));
    }

    const bool processed =
        GetEventHandler()->SafelyProcessEvent(changedEvent);
    if ( weakThis.get() != this || IsBeingDeleted() )
        return;

    if (processed && !changedEvent.IsAllowed())
        event.Veto();
    else
        event.Skip();
}

void wxGenericDirCtrl::OnItemActivated(wxTreeEvent &event)
{
    wxTreeItemId treeid = event.GetItem();
    const wxDirItemData *data = GetItemData(treeid);
    if ( !data )
        return;

    if (data->m_isDir)
    {
        // is dir
        event.Skip();
    }
    else
    {
        // is file
        event.Skip(false);
        const wxWeakRef<wxWindow> weakThis(this);
        wxTreeEvent changedEvent(wxEVT_DIRCTRL_FILEACTIVATED, GetId());

        changedEvent.SetEventObject(this);
        changedEvent.SetItem(treeid);
        changedEvent.SetClientObject(GetItemData(treeid));

        const bool processed =
            GetEventHandler()->SafelyProcessEvent(changedEvent);
        if ( weakThis.get() != this || IsBeingDeleted() )
            return;

        if (processed && !changedEvent.IsAllowed())
            event.Veto();
        else
            event.Skip();
    }
}

void wxGenericDirCtrl::OnExpandItem(wxTreeEvent &event)
{
    wxTreeCtrl* const treeCtrl = m_treeCtrl.get();
    if ( !treeCtrl || treeCtrl->IsBeingDeleted() )
        return;

    wxTreeItemId parentId = event.GetItem();

    // VS: this is needed because the event handler is called from wxTreeCtrl
    //     ctor when wxTR_HIDE_ROOT was specified

    if (!m_rootId.IsOk())
        m_rootId = treeCtrl->GetRootItem();

    ExpandDir(parentId);
}

void wxGenericDirCtrl::OnCollapseItem(wxTreeEvent &event )
{
    CollapseDir(event.GetItem());
}

void wxGenericDirCtrl::CollapseDir(wxTreeItemId parentId)
{
    wxTreeCtrl* const treeCtrl = m_treeCtrl.get();
    if ( !treeCtrl || treeCtrl->IsBeingDeleted() )
        return;

    wxDirItemData *data = GetItemData(parentId);
    if (!data || !data->m_isExpanded)
        return;

    data->m_isExpanded = false;

    const wxWeakRef<wxWindow> weakThis(this);
    wxDirTreeUpdateLocker updateLocker(treeCtrl);
    if (parentId != treeCtrl->GetRootItem())
    {
        treeCtrl->CollapseAndReset(parentId);
    }
    else
    {
        treeCtrl->DeleteChildren(parentId);
    }
    if ( weakThis.get() != this || IsBeingDeleted() ||
         m_treeCtrl.get() != treeCtrl )
    {
        return;
    }
}

void wxGenericDirCtrl::PopulateNode(wxTreeItemId parentId)
{
    wxTreeCtrl* const treeCtrl = m_treeCtrl.get();
    if ( !treeCtrl || treeCtrl->IsBeingDeleted() )
        return;

    const wxWeakRef<wxWindow> weakThis(this);
    const auto isAlive =
        [this, &weakThis, treeCtrl]()
        {
            return weakThis.get() == this &&
                   !IsBeingDeleted() &&
                   m_treeCtrl.get() == treeCtrl;
        };
    wxDirItemData *data = GetItemData(parentId);

    if (!data || data->m_isExpanded)
        return;

    data->m_isExpanded = true;

    if (parentId == treeCtrl->GetRootItem())
    {
        SetupSections();
        return;
    }

    wxASSERT(data);

    wxString path;

    wxString dirName(data->m_path);
    const wxString parentPath(data->m_path);

#if defined(__WINDOWS__)
    // Check if this is a root directory and if so,
    // whether the drive is available.
    if (!wxIsDriveAvailable(dirName))
    {
        data->m_isExpanded = false;
        //wxMessageBox(wxT("Sorry, this drive is not available."));
        return;
    }
#endif

    // This may take a longish time. Go to busy cursor
    wxBusyCursor busy;

#if defined(__WINDOWS__)
    if (dirName.Last() == ':')
        dirName += wxString(wxFILE_SEP_PATH);
#endif

    wxArrayString dirs;
    wxArrayString filenames;

    wxDir d;
    wxString eachFilename;

    wxLogNull log;
    d.Open(dirName);

    if (d.IsOpened())
    {
        int style = wxDIR_DIRS;
        if (m_showHidden) style |= wxDIR_HIDDEN;
        if (d.GetFirst(& eachFilename, wxEmptyString, style))
        {
            do
            {
                if ((eachFilename != wxT(".")) && (eachFilename != wxT("..")))
                {
                    dirs.Add(eachFilename);
                }
            }
            while (d.GetNext(&eachFilename));
        }
    }

    // An explicit default/current path must remain reachable even when one of
    // its ancestors is hidden (the Windows user temp directory commonly sits
    // below the hidden AppData directory). Keep just the next required
    // directory visible; unrelated hidden siblings remain excluded.
    if ( !m_showHidden && !m_defaultPath.empty() )
    {
        wxString parentPrefix(dirName);
        if ( !wxEndsWithPathSeparator(parentPrefix) )
            parentPrefix += wxFILE_SEP_PATH;

        wxString targetPath(m_defaultPath);
        targetPath.Replace("/", wxString(wxFILE_SEP_PATH));
        targetPath.Replace("\\", wxString(wxFILE_SEP_PATH));

        wxString comparableParent(parentPrefix);
        wxString comparableTarget(targetPath);
#if defined(__WINDOWS__)
        comparableParent.MakeLower();
        comparableTarget.MakeLower();
#endif
        if ( comparableTarget.StartsWith(comparableParent) )
        {
            const wxString remainder =
                targetPath.Mid(parentPrefix.length());
            const wxString requiredChild =
                remainder.BeforeFirst(wxFILE_SEP_PATH);
            if ( !requiredChild.empty() &&
                 dirs.Index(requiredChild) == wxNOT_FOUND &&
                 wxDirExists(parentPrefix + requiredChild) )
            {
                dirs.Add(requiredChild);
            }
        }
    }
    dirs.Sort(wxCmpNatural);

    // Now do the filenames -- but only if we're allowed to
    if (!HasFlag(wxDIRCTRL_DIR_ONLY))
    {
        d.Open(dirName);

        if (d.IsOpened())
        {
            int style = wxDIR_FILES;
            if (m_showHidden) style |= wxDIR_HIDDEN;
            // Process each filter (ex: "JPEG Files (*.jpg;*.jpeg)|*.jpg;*.jpeg")
            wxStringTokenizer strTok;
            wxString curFilter;
            strTok.SetString(m_currentFilterStr,wxT(";"));
            while(strTok.HasMoreTokens())
            {
                curFilter = strTok.GetNextToken();
                if (d.GetFirst(& eachFilename, curFilter, style))
                {
                    do
                    {
                        if ((eachFilename != wxT(".")) && (eachFilename != wxT("..")))
                        {
                            if ( filenames.Index(eachFilename) == wxNOT_FOUND )
                                filenames.Add(eachFilename);
                        }
                    }
                    while (d.GetNext(& eachFilename));
                }
            }
        }
        filenames.Sort(wxCmpNatural);
    }

    // Now we really know whether we have any children so tell the tree control
    // about it.
    wxDirTreeUpdateLocker updateLocker(treeCtrl);
    treeCtrl->SetItemHasChildren(parentId, !dirs.empty() || !filenames.empty());
    if ( !isAlive() )
        return;

    // Add the sorted dirs
    size_t i;
    for (i = 0; i < dirs.GetCount(); i++)
    {
        eachFilename = dirs[i];
        path = dirName;
        if (!wxEndsWithPathSeparator(path))
            path += wxString(wxFILE_SEP_PATH);
        path += eachFilename;

        wxDirItemData* liveParentData = GetItemData(parentId);
        if ( !liveParentData ||
             !wxDirCtrlPathsEqual(liveParentData->m_path, parentPath) )
        {
            parentId = FindItem(parentPath);
        }
        if ( !parentId.IsOk() )
            return;

        wxDirItemData *dir_item = new wxDirItemData(path,eachFilename,true);
        wxTreeItemId treeid = AppendItem( parentId, eachFilename,
                                       wxFileIconsTable::folder, -1, dir_item);
        if ( !isAlive() )
            return;
        if ( !treeid.IsOk() )
        {
            delete dir_item;
            return;
        }
        wxDirItemData* liveItemData =
            treeid.IsOk() ? GetItemData(treeid) : nullptr;
        if ( !liveItemData ||
             !wxDirCtrlPathsEqual(liveItemData->m_path, path) )
        {
            treeid = FindItem(path);
        }
        if ( !treeid.IsOk() )
            return;
        treeCtrl->SetItemImage( treeid, wxFileIconsTable::folder_open,
                                wxTreeItemIcon_Expanded );

        // assume that it does have children by default as it can take a long
        // time to really check for this (think remote drives...)
        //
        // and if we're wrong, we'll correct the icon later if
        // the user really tries to open this item
        treeCtrl->SetItemHasChildren(treeid);
        if ( !isAlive() )
            return;
    }

    // Add the sorted filenames
    if (!HasFlag(wxDIRCTRL_DIR_ONLY))
    {
        for (i = 0; i < filenames.GetCount(); i++)
        {
            eachFilename = filenames[i];
            path = dirName;
            if (!wxEndsWithPathSeparator(path))
                path += wxString(wxFILE_SEP_PATH);
            path += eachFilename;
            //path = dirName + wxString(wxT("/")) + eachFilename;

            wxDirItemData* liveParentData = GetItemData(parentId);
            if ( !liveParentData ||
                 !wxDirCtrlPathsEqual(liveParentData->m_path, parentPath) )
            {
                parentId = FindItem(parentPath);
            }
            if ( !parentId.IsOk() )
                return;

            wxDirItemData *dir_item = new wxDirItemData(path,eachFilename,false);
            int image_id = wxFileIconsTable::file;
            if (eachFilename.Find(wxT('.')) != wxNOT_FOUND)
                image_id = wxTheFileIconsTable->GetIconID(eachFilename.AfterLast(wxT('.')));
            const wxTreeItemId inserted =
                AppendItem(parentId, eachFilename, image_id, -1, dir_item);
            if ( !isAlive() )
                return;
            if ( !inserted.IsOk() )
            {
                delete dir_item;
                return;
            }
        }
    }
}

void wxGenericDirCtrl::ExpandDir(wxTreeItemId parentId)
{
    // ExpandDir() will not actually expand the tree node, just populate it
    PopulateNode(parentId);
}

void wxGenericDirCtrl::ReCreateTree()
{
    wxTreeCtrl* const treeCtrl = m_treeCtrl.get();
    if ( !treeCtrl || treeCtrl->IsBeingDeleted() )
        return;

    const wxWeakRef<wxWindow> weakThis(this);
    wxDirTreeUpdateLocker updateLocker(treeCtrl);
    CollapseDir(treeCtrl->GetRootItem());
    if ( weakThis.get() != this || IsBeingDeleted() ||
         m_treeCtrl.get() != treeCtrl )
    {
        return;
    }
    ExpandRoot();
}

void wxGenericDirCtrl::CollapseTree()
{
    if ( !m_treeCtrl || m_treeCtrl->IsBeingDeleted() )
        return;

    const wxWeakRef<wxWindow> weakThis(this);
    wxArrayString sectionPaths;
    wxTreeItemIdValue cookie;
    wxTreeItemId child = m_treeCtrl->GetFirstChild(m_rootId, cookie);
    while (child.IsOk())
    {
        const wxString path = GetPath(child);
        if ( !path.empty() )
            sectionPaths.Add(path);
        child = m_treeCtrl->GetNextChild(m_rootId, cookie);
    }

    for ( const wxString& path : sectionPaths )
    {
        const wxTreeItemId liveChild = FindItem(path);
        if ( liveChild.IsOk() )
            CollapseDir(liveChild);
        if ( weakThis.get() != this || IsBeingDeleted() || !m_treeCtrl )
            return;
    }
}

// Find the child that matches the first part of 'path'.
// E.g. if a child path is "/usr" and 'path' is "/usr/include"
// then the child for /usr is returned.
wxTreeItemId wxGenericDirCtrl::FindChild(wxTreeItemId parentId, const wxString& path, bool& done)
{
    wxTreeCtrl* const treeCtrl = m_treeCtrl.get();
    if ( !treeCtrl || treeCtrl->IsBeingDeleted() )
        return wxTreeItemId();

    wxString path2(path);

    // Make sure all separators are as per the current platform
    path2.Replace(wxT("\\"), wxString(wxFILE_SEP_PATH));
    path2.Replace(wxT("/"), wxString(wxFILE_SEP_PATH));

    // Append a separator to foil bogus substring matching
    path2 += wxString(wxFILE_SEP_PATH);

    // In MSW case is not significant
#if defined(__WINDOWS__)
    path2.MakeLower();
#endif

    wxTreeItemIdValue cookie;
    wxTreeItemId childId = treeCtrl->GetFirstChild(parentId, cookie);
    while (childId.IsOk())
    {
        wxDirItemData* data = GetItemData(childId);

        if (data && !data->m_path.empty())
        {
            wxString childPath(data->m_path);
            if (!wxEndsWithPathSeparator(childPath))
                childPath += wxString(wxFILE_SEP_PATH);

            // In MSW case is not significant
#if defined(__WINDOWS__)
            childPath.MakeLower();
#endif

            if (childPath.length() <= path2.length())
            {
                wxString path3 = path2.Mid(0, childPath.length());
                if (childPath == path3)
                {
                    if (path3.length() == path2.length())
                        done = true;
                    else
                        done = false;
                    return childId;
                }
            }
        }

        childId = treeCtrl->GetNextChild(parentId, cookie);
    }
    wxTreeItemId invalid;
    return invalid;
}

wxTreeItemId wxGenericDirCtrl::FindItem(const wxString& path)
{
    if ( !m_treeCtrl || m_treeCtrl->IsBeingDeleted() || !m_rootId.IsOk() )
        return wxTreeItemId();

    bool done = false;
    wxTreeItemId item = FindChild(m_rootId, path, done);
    while ( item.IsOk() )
    {
        if ( done || wxDirCtrlPathsEqual(GetPath(item), path) )
            return item;

        item = FindChild(item, path, done);
    }

    return wxTreeItemId();
}

// Try to expand as much of the given path as possible,
// and select the given tree item.
bool wxGenericDirCtrl::ExpandPath(const wxString& path)
{
    wxTreeCtrl* const treeCtrl = m_treeCtrl.get();
    if ( !treeCtrl || treeCtrl->IsBeingDeleted() )
        return false;

    const wxWeakRef<wxWindow> weakThis(this);
    wxDirTreeUpdateLocker updateLocker(treeCtrl);
    const auto isAlive =
        [this, &weakThis, treeCtrl]()
        {
            return weakThis.get() == this &&
                   !IsBeingDeleted() &&
                   m_treeCtrl.get() == treeCtrl;
        };

    bool done = false;
    wxTreeItemId parent = m_rootId;
    wxString lastPath;
    while ( parent.IsOk() )
    {
        wxTreeItemId child = FindChild(parent, path, done);
        if ( !child.IsOk() )
        {
            // The filesystem may have changed after this node was populated.
            // Refresh the closest known ancestor once and retry instead of
            // silently leaving the old selection in place. This is also what
            // makes SetPath() useful for a newly-created long/Unicode branch.
            const bool parentIsRoot = parent == m_rootId;
            const wxString parentPath =
                parentIsRoot ? wxString() : GetPath(parent);
            CollapseDir(parent);
            if ( !isAlive() )
                return false;

            parent = parentIsRoot ? m_rootId : FindItem(parentPath);
            if ( !parent.IsOk() )
                break;

            ExpandDir(parent);
            if ( !isAlive() )
                return false;

            parent = parentIsRoot ? m_rootId : FindItem(parentPath);
            if ( !parent.IsOk() )
                break;

            child = FindChild(parent, path, done);
        }
        if ( !child.IsOk() )
            break;

        lastPath = GetPath(child);
        if ( done )
            break;

        ExpandDir(child);
        if ( !isAlive() )
            return false;

        // ExpandDir()/AppendItem() are virtual and list/tree events can run
        // arbitrary application code. Never carry the pre-callback item ID
        // into the next iteration.
        parent = FindItem(lastPath);
    }

    if ( lastPath.empty() )
        return false;

    wxTreeItemId lastId = FindItem(lastPath);
    wxDirItemData* data =
        lastId.IsOk() ? GetItemData(lastId) : nullptr;
    if ( !data )
        return false;

    if ( data->m_isDir )
    {
        treeCtrl->Expand(lastId);
        if ( !isAlive() )
            return false;

        lastId = FindItem(lastPath);
        data = lastId.IsOk() ? GetItemData(lastId) : nullptr;
        if ( !data )
            return false;
    }

    const auto selectAndReveal =
        [this, &isAlive, treeCtrl](const wxString& itemPath)
        {
            wxTreeItemId item = FindItem(itemPath);
            if ( !item.IsOk() )
                return false;

            treeCtrl->SelectItem(item);
            if ( !isAlive() )
                return false;

            item = FindItem(itemPath);
            if ( !item.IsOk() )
                return false;

            treeCtrl->EnsureVisible(item);
            return isAlive();
        };

    if (HasFlag(wxDIRCTRL_SELECT_FIRST) && data->m_isDir)
    {
        // Find the first file in this directory
        wxString firstFilePath;
        wxTreeItemIdValue cookie;
        wxTreeItemId childId = treeCtrl->GetFirstChild(lastId, cookie);
        while (childId.IsOk())
        {
            data = GetItemData(childId);

            if (data && !data->m_path.empty() && !data->m_isDir)
            {
                firstFilePath = data->m_path;
                break;
            }
            childId = treeCtrl->GetNextChild(lastId, cookie);
        }

        return selectAndReveal(firstFilePath.empty()
                                 ? lastPath
                                 : firstFilePath);
    }

    return selectAndReveal(lastPath);
}


bool wxGenericDirCtrl::CollapsePath(const wxString& path)
{
    if ( !m_treeCtrl || m_treeCtrl->IsBeingDeleted() )
        return false;

    const wxWeakRef<wxWindow> weakThis(this);
    const wxTreeItemId item = FindItem(path);
    if ( !item.IsOk() )
        return false;

    CollapseDir(item);
    if ( weakThis.get() != this || IsBeingDeleted() || !m_treeCtrl )
        return false;

    const wxTreeItemId liveItem = FindItem(path);
    if ( !liveItem.IsOk() )
        return false;

    m_treeCtrl->SelectItem(liveItem);
    if ( weakThis.get() != this || IsBeingDeleted() || !m_treeCtrl )
        return false;

    const wxTreeItemId visibleItem = FindItem(path);
    if ( !visibleItem.IsOk() )
        return false;
    m_treeCtrl->EnsureVisible(visibleItem);

    return true;
}

wxDirItemData* wxGenericDirCtrl::GetItemData(wxTreeItemId itemId)
{
    wxTreeCtrl* const treeCtrl = m_treeCtrl.get();
    return treeCtrl && !treeCtrl->IsBeingDeleted()
        ? static_cast<wxDirItemData*>(treeCtrl->GetItemData(itemId))
        : nullptr;
}

wxString wxGenericDirCtrl::GetPath(wxTreeItemId itemId) const
{
    wxTreeCtrl* const treeCtrl = m_treeCtrl.get();
    if ( !treeCtrl || treeCtrl->IsBeingDeleted() )
        return wxString();

    const wxDirItemData*
        data = static_cast<wxDirItemData*>(treeCtrl->GetItemData(itemId));

    return data ? data->m_path : wxString();
}

wxString wxGenericDirCtrl::GetPath() const
{
    wxTreeCtrl* const treeCtrl = m_treeCtrl.get();
    if ( !treeCtrl || treeCtrl->IsBeingDeleted() )
        return wxString();

    // Allow calling GetPath() in multiple selection from OnSelFilter
    if (treeCtrl->HasFlag(wxTR_MULTIPLE))
    {
        wxArrayTreeItemIds items;
        treeCtrl->GetSelections(items);
        if (items.size() > 0)
        {
            // return first string only
            wxTreeItemId treeid = items[0];
            return GetPath(treeid);
        }

        return wxEmptyString;
    }

    wxTreeItemId treeid = treeCtrl->GetSelection();
    if (treeid)
    {
        return GetPath(treeid);
    }
    else
        return wxEmptyString;
}

void wxGenericDirCtrl::GetPaths(wxArrayString& paths) const
{
    paths.clear();

    wxTreeCtrl* const treeCtrl = m_treeCtrl.get();
    if ( !treeCtrl || treeCtrl->IsBeingDeleted() )
        return;

    wxArrayTreeItemIds items;
    treeCtrl->GetSelections(items);
    for ( unsigned n = 0; n < items.size(); n++ )
    {
        wxTreeItemId treeid = items[n];
        paths.push_back(GetPath(treeid));
    }
}

wxString wxGenericDirCtrl::GetFilePath() const
{
    wxTreeCtrl* const treeCtrl = m_treeCtrl.get();
    if ( !treeCtrl || treeCtrl->IsBeingDeleted() )
        return wxString();

    wxTreeItemId treeid = treeCtrl->GetSelection();
    if (treeid)
    {
        wxDirItemData* data =
            static_cast<wxDirItemData*>(treeCtrl->GetItemData(treeid));
        if ( !data || data->m_isDir )
            return wxEmptyString;
        else
            return data->m_path;
    }
    else
        return wxEmptyString;
}

void wxGenericDirCtrl::GetFilePaths(wxArrayString& paths) const
{
    paths.clear();

    wxTreeCtrl* const treeCtrl = m_treeCtrl.get();
    if ( !treeCtrl || treeCtrl->IsBeingDeleted() )
        return;

    wxArrayTreeItemIds items;
    treeCtrl->GetSelections(items);
    for ( unsigned n = 0; n < items.size(); n++ )
    {
        wxTreeItemId treeid = items[n];
        wxDirItemData* data =
            static_cast<wxDirItemData*>(treeCtrl->GetItemData(treeid));
        if ( data && !data->m_isDir )
            paths.Add(data->m_path);
    }
}

void wxGenericDirCtrl::SetPath(const wxString& path)
{
    m_defaultPath = path;
    if (m_rootId)
        ExpandPath(path);
}

void wxGenericDirCtrl::SelectPath(const wxString& path, bool select)
{
    const wxTreeItemId item = FindItem(path);
    if ( item.IsOk() && m_treeCtrl && !m_treeCtrl->IsBeingDeleted() )
        m_treeCtrl->SelectItem(item, select);
}

void wxGenericDirCtrl::SelectPaths(const wxArrayString& paths)
{
    if ( HasFlag(wxDIRCTRL_MULTIPLE) )
    {
        const wxWeakRef<wxWindow> weakThis(this);
        UnselectAll();
        if ( weakThis.get() != this || IsBeingDeleted() )
            return;
        for ( unsigned n = 0; n < paths.size(); n++ )
        {
            SelectPath(paths[n]);
            if ( weakThis.get() != this || IsBeingDeleted() )
                return;
        }
    }
}

void wxGenericDirCtrl::UnselectAll()
{
    if ( m_treeCtrl && !m_treeCtrl->IsBeingDeleted() )
        m_treeCtrl->UnselectAll();
}

// Not used
#if 0
void wxGenericDirCtrl::FindChildFiles(wxTreeItemId treeid, int dirFlags, wxArrayString& filenames)
{
    wxDirItemData *data = (wxDirItemData *) m_treeCtrl->GetItemData(treeid);

    // This may take a longish time. Go to busy cursor
    wxBusyCursor busy;

    wxASSERT(data);

    wxString search,path,filename;

    wxString dirName(data->m_path);

#if defined(__WINDOWS__)
    if (dirName.Last() == ':')
        dirName += wxString(wxFILE_SEP_PATH);
#endif

    wxDir d;
    wxString eachFilename;

    wxLogNull log;
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
    wxArrayString descriptions;
    wxArrayString filters;
    const int count = m_filter.empty()
        ? 0
        : wxParseCommonDialogsFilter(m_filter, descriptions, filters);

    if ( count > 0 )
    {
        if ( n < 0 || n >= count )
            return;
    }
    else if ( n != 0 )
    {
        return;
    }

    wxArrayString selectedPaths;
    wxString selectedPath;
    const bool canRestore = m_treeCtrl && m_rootId.IsOk();
    if ( canRestore )
    {
        if ( HasFlag(wxDIRCTRL_MULTIPLE) )
            wxGenericDirCtrl::GetPaths(selectedPaths);
        else
            selectedPath = wxGenericDirCtrl::GetPath();
    }

    m_currentFilter = n;

    if ( count > 0 )
        m_currentFilterStr = filters[static_cast<size_t>(n)];
    else
#ifdef __UNIX__
        m_currentFilterStr = wxT("*");
#else
        m_currentFilterStr = wxT("*.*");
#endif

    if ( m_filterListCtrl &&
         !m_filterListCtrl->IsBeingDeleted() &&
         m_filterListCtrl->GetSelection() != n )
    {
        m_filterListCtrl->SetSelection(n);
    }

    if ( !canRestore )
        return;

    const wxWeakRef<wxWindow> weakThis(this);
    ReCreateTree();
    if ( weakThis.get() != this || IsBeingDeleted() )
        return;

    if ( HasFlag(wxDIRCTRL_MULTIPLE) )
    {
        for ( const wxString& path : selectedPaths )
        {
            ExpandPath(path);
            if ( weakThis.get() != this || IsBeingDeleted() )
                return;
        }
    }
    else if ( !selectedPath.empty() )
    {
        ExpandPath(selectedPath);
    }
}

void wxGenericDirCtrl::SetFilter(const wxString& filter)
{
    const wxWeakRef<wxWindow> weakThis(this);
    m_filter = filter;

    if (!filter.empty() && !m_filterListCtrl && HasFlag(wxDIRCTRL_SHOW_FILTERS))
    {
        wxDirFilterListCtrl* const filterList =
            new wxDirFilterListCtrl;
        if ( filterList->Create(this, wxID_FILTERLISTCTRL) )
            m_filterListCtrl = wxWeakRef<wxDirFilterListCtrl>(filterList);
        else
            delete filterList;
    }
    wxDirFilterListCtrl* const filterList = m_filterListCtrl.get();
    if (filterList && !filterList->IsBeingDeleted())
    {
        // Keep an empty associated child alive but hidden. Destroy() is
        // deferred on several ports, and recreating another child immediately
        // would leave two controls with wxID_FILTERLISTCTRL and a queued stale
        // event.
        filterList->FillFilterList(m_filter, 0);
        if ( weakThis.get() != this || IsBeingDeleted() ||
             m_filterListCtrl.get() != filterList )
        {
            return;
        }

        if ( !filter.empty() )
            filterList->Show();
        else
            filterList->Hide();
        if ( weakThis.get() != this || IsBeingDeleted() )
            return;
    }

    DoResize();
    if ( weakThis.get() != this || IsBeingDeleted() )
        return;

    // The previous index is meaningless for a new filter. This also refreshes
    // the tree and restores the closest still-visible selection.
    SetFilterIndex(0);
}

// Extract description and actual filter from overall filter string
bool wxGenericDirCtrl::ExtractWildcard(const wxString& filterStr, int n, wxString& filter, wxString& description)
{
    if ( filterStr.empty() )
        return false;

    wxArrayString filters, descriptions;
    int count = wxParseCommonDialogsFilter(filterStr, descriptions, filters);
    if (count > 0 && n < count)
    {
        filter = filters[n];
        description = descriptions[n];
        return true;
    }

    return false;
}


void wxGenericDirCtrl::DoResize()
{
    wxSize sz = GetClientSize();
    int verticalSpacing = 3;
    wxTreeCtrl* const treeCtrl = m_treeCtrl.get();
    if (treeCtrl && !treeCtrl->IsBeingDeleted() &&
        treeCtrl->GetParent() == this)
    {
        const wxWeakRef<wxWindow> weakThis(this);
        wxSize filterSz ;
        wxDirFilterListCtrl* filterList = m_filterListCtrl.get();
        wxDirFilterListCtrl* const measuredFilter = filterList;
        if (filterList && filterList->GetParent() == this &&
            !filterList->IsBeingDeleted() &&
            filterList->IsShown())
        {
            filterSz = filterList->GetBestSize();
            sz.y -= (filterSz.y + verticalSpacing);
        }
        treeCtrl->SetSize(0, 0, sz.x, sz.y);
        if ( weakThis.get() != this || IsBeingDeleted() ||
             m_treeCtrl.get() != treeCtrl )
        {
            return;
        }

        filterList = m_filterListCtrl.get();
        if (filterList && filterList == measuredFilter &&
            filterList->GetParent() == this &&
            !filterList->IsBeingDeleted() &&
            filterList->IsShown())
        {
            filterList->SetSize(
                0, sz.y + verticalSpacing, sz.x, filterSz.y);
            if ( weakThis.get() != this || IsBeingDeleted() )
                return;
            // Don't know why, but this needs refreshing after a resize (wxMSW)
            if ( m_filterListCtrl.get() == filterList &&
                 !filterList->IsBeingDeleted() )
            {
                filterList->Refresh();
            }
        }
    }
}


void wxGenericDirCtrl::OnSize(wxSizeEvent& WXUNUSED(event))
{
    DoResize();
}

wxTreeItemId wxGenericDirCtrl::AppendItem (const wxTreeItemId & parent,
                                           const wxString & text,
                                           int image, int selectedImage,
                                           wxTreeItemData * data)
{
  wxTreeCtrl *treeCtrl = GetTreeCtrl ();

  wxASSERT (treeCtrl);

  if (treeCtrl)
  {
    return treeCtrl->AppendItem (parent, text, image, selectedImage, data);
  }
  else
  {
    return wxTreeItemId();
  }
}


//-----------------------------------------------------------------------------
// wxDirFilterListCtrl
//-----------------------------------------------------------------------------

wxIMPLEMENT_CLASS(wxDirFilterListCtrl, wxChoice);

wxBEGIN_EVENT_TABLE(wxDirFilterListCtrl, wxChoice)
    EVT_CHOICE(wxID_ANY, wxDirFilterListCtrl::OnSelFilter)
wxEND_EVENT_TABLE()

bool wxDirFilterListCtrl::Create(wxGenericDirCtrl* parent,
                                 wxWindowID treeid,
                                 const wxPoint& pos,
                                 const wxSize& size,
                                 long style)
{
    if ( !wxChoice::Create(parent, treeid, pos, size, 0, nullptr, style) )
        return false;

    m_dirCtrl = wxWeakRef<wxGenericDirCtrl>(parent);
    return true;
}

void wxDirFilterListCtrl::Init()
{
    m_dirCtrl.Release();
}

void wxDirFilterListCtrl::OnSelFilter(wxCommandEvent& WXUNUSED(event))
{
    int sel = GetSelection();
    wxGenericDirCtrl* const dirCtrl = m_dirCtrl.get();
    if ( sel == wxNOT_FOUND || !dirCtrl || dirCtrl->IsBeingDeleted() )
        return;

    // SetFilterIndex() owns the atomic filter/tree update and selection
    // restoration for both single and multiple selection controls.
    dirCtrl->SetFilterIndex(sel);
}

void wxDirFilterListCtrl::FillFilterList(const wxString& filter, int defaultFilter)
{
    Clear();
    if ( filter.empty() )
    {
        SetSelection(wxNOT_FOUND);
        return;
    }

    wxArrayString descriptions, filters;
    size_t n = (size_t) wxParseCommonDialogsFilter(filter, descriptions, filters);

    if ( n > 0 )
    {
        for (size_t i = 0; i < n; i++)
            Append(descriptions[i]);
        SetSelection(defaultFilter >= 0 && defaultFilter < static_cast<int>(n)
                         ? defaultFilter
                         : 0);
    }
    else
        SetSelection(wxNOT_FOUND);
}
#endif // wxUSE_DIRDLG

#if wxUSE_DIRDLG || wxUSE_FILEDLG

// ----------------------------------------------------------------------------
// wxFileIconsTable & friends
// ----------------------------------------------------------------------------

// global instance of a wxFileIconsTable
wxFileIconsTable* wxTheFileIconsTable = nullptr;

// A module to allow icons table cleanup

class wxFileIconsTableModule: public wxModule
{
    wxDECLARE_DYNAMIC_CLASS(wxFileIconsTableModule);
public:
    wxFileIconsTableModule() {}
    bool OnInit() override { wxTheFileIconsTable = new wxFileIconsTable; return true; }
    void OnExit() override
    {
        wxDELETE(wxTheFileIconsTable);
    }
};

wxIMPLEMENT_DYNAMIC_CLASS(wxFileIconsTableModule, wxModule);

class wxFileIconEntry : public wxObject
{
public:
    wxFileIconEntry(int i) { iconid = i; }

    int iconid;
};

wxFileIconsTable::wxFileIconsTable()
    : m_size(16, 16)
{
    m_HashTable = nullptr;
    m_smallImageList = nullptr;
}

wxFileIconsTable::~wxFileIconsTable()
{
    if (m_HashTable)
    {
        WX_CLEAR_HASH_TABLE(*m_HashTable);
        delete m_HashTable;
    }
    delete m_smallImageList;
}

// delayed initialization - wait until first use (wxArtProv not created yet)
void wxFileIconsTable::Create(const wxSize& sz)
{
    wxCHECK_RET(!m_smallImageList && !m_HashTable, wxT("creating icons twice"));
    m_HashTable = new wxHashTable(wxKEY_STRING);
    m_smallImageList = new wxImageList(sz.x, sz.y);

    // folder:
    m_smallImageList->Add(wxArtProvider::GetBitmap(wxART_FOLDER,
                                                   wxART_CMN_DIALOG,
                                                   sz));
    // folder_open
    m_smallImageList->Add(wxArtProvider::GetBitmap(wxART_FOLDER_OPEN,
                                                   wxART_CMN_DIALOG,
                                                   sz));
    // computer
#ifdef __WXGTK__
    // GTK24 uses this icon in the file open dialog
    m_smallImageList->Add(wxArtProvider::GetBitmap(wxART_HARDDISK,
                                                   wxART_CMN_DIALOG,
                                                   sz));
#else
    m_smallImageList->Add(wxArtProvider::GetBitmap(wxART_HARDDISK,
                                                   wxART_CMN_DIALOG,
                                                   sz));
    // TODO: add computer icon if really necessary
    //m_smallImageList->Add(wxIcon(file_icons_tbl_computer_xpm));
#endif
    // drive
    m_smallImageList->Add(wxArtProvider::GetBitmap(wxART_HARDDISK,
                                                   wxART_CMN_DIALOG,
                                                   sz));
    // cdrom
    m_smallImageList->Add(wxArtProvider::GetBitmap(wxART_CDROM,
                                                   wxART_CMN_DIALOG,
                                                   sz));
    // floppy
    m_smallImageList->Add(wxArtProvider::GetBitmap(wxART_FLOPPY,
                                                   wxART_CMN_DIALOG,
                                                   sz));
    // removeable
    m_smallImageList->Add(wxArtProvider::GetBitmap(wxART_REMOVABLE,
                                                   wxART_CMN_DIALOG,
                                                   sz));
    // file
    m_smallImageList->Add(wxArtProvider::GetBitmap(wxART_NORMAL_FILE,
                                                   wxART_CMN_DIALOG,
                                                   sz));
    // executable
    if (GetIconID(wxEmptyString, wxT("application/x-executable")) == file)
    {
        m_smallImageList->Add(wxArtProvider::GetBitmap(wxART_EXECUTABLE_FILE,
                                                       wxART_CMN_DIALOG,
                                                       sz));
        delete m_HashTable->Get(wxT("exe"));
        m_HashTable->Delete(wxT("exe"));
        m_HashTable->Put(wxT("exe"), new wxFileIconEntry(executable));
    }
    /* else put into list by GetIconID
       (KDE defines application/x-executable for *.exe and has nice icon)
     */
}

wxImageList *wxFileIconsTable::GetSmallImageList()
{
    if (!m_smallImageList)
        Create(m_size);

    return m_smallImageList;
}

int wxFileIconsTable::GetIconID(const wxString& extension, const wxString& mime)
{
    if (!m_smallImageList)
        Create(m_size);

#if wxUSE_MIMETYPE
    if (!extension.empty())
    {
        wxFileIconEntry *entry = (wxFileIconEntry*) m_HashTable->Get(extension);
        if (entry) return (entry -> iconid);
    }

    wxFileType *ft = (mime.empty()) ?
                   wxTheMimeTypesManager -> GetFileTypeFromExtension(extension) :
                   wxTheMimeTypesManager -> GetFileTypeFromMimeType(mime);

    wxIconLocation iconLoc;
    wxIcon ic;

    {
        wxLogNull logNull;
        if ( ft && ft->GetIcon(&iconLoc) )
        {
            ic = wxIcon( iconLoc );
        }
    }

    delete ft;

    if ( !ic.IsOk() )
    {
        int newid = file;
        m_HashTable->Put(extension, new wxFileIconEntry(newid));
        return newid;
    }

    wxBitmap bmp;
    bmp.CopyFromIcon(ic);

    if ( !bmp.IsOk() )
    {
        int newid = file;
        m_HashTable->Put(extension, new wxFileIconEntry(newid));
        return newid;
    }

    int size = m_size.x;

    int treeid = m_smallImageList->GetImageCount();
    if ((bmp.GetWidth() == (int) size) && (bmp.GetHeight() == (int) size))
    {
        m_smallImageList->Add(bmp);
    }
#if wxUSE_IMAGE && (!defined(__WINDOWS__) || wxUSE_WXDIB)
    else
    {
        wxImage img = bmp.ConvertToImage();

        if (img.HasMask())
            img.InitAlpha();

        wxBitmap bmp2;
        if ((img.GetWidth() != size) || (img.GetHeight() != size))
        {
            // TODO: replace with public API that gets the bitmap scale.
            // But this function may be called from code that doesn't pass a window,
            // so we will need to be able to get the scaling factor of the current
            // display, somehow. We could use wxTheApp->GetTopWindow() but sometimes
            // this won't be available.
#if defined(__WXOSX_COCOA__)
            if (wxOSXGetMainScreenContentScaleFactor() > 1.0)
            {
                img.Rescale(2*size, 2*size, wxIMAGE_QUALITY_HIGH);
                bmp2 = wxBitmap(img, -1, 2.0);
            }
            else
#endif
            {
                // Double, using normal quality scaling.
                img.Rescale(2*img.GetSize());

                // Then scale to the desired size. This gives the best quality,
                // and better than CreateAntialiasedBitmap.
                if ((img.GetWidth() != size) || (img.GetHeight() != size))
                    img.Rescale(size, size, wxIMAGE_QUALITY_HIGH);

                bmp2 = wxBitmap(img);
            }
        }
        else
            bmp2 = wxBitmap(img);

        m_smallImageList->Add(bmp2);
    }
#endif // wxUSE_IMAGE

    m_HashTable->Put(extension, new wxFileIconEntry(treeid));
    return treeid;

#else // !wxUSE_MIMETYPE

    wxUnusedVar(mime);
    if (extension == wxT("exe"))
        return executable;
    else
        return file;
#endif // wxUSE_MIMETYPE/!wxUSE_MIMETYPE
}

#endif // wxUSE_DIRDLG || wxUSE_FILEDLG
