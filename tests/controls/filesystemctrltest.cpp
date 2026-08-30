///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/filesystemctrltest.cpp
// Purpose:     Tests for the generic directory and file controls.
// Author:      wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_DIRDLG && wxUSE_FILECTRL && wxUSE_TREECTRL && wxUSE_LISTCTRL

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/frame.h"
    #include "wx/panel.h"
#endif

#include "wx/dirctrl.h"
#include "wx/dnd.h"
#include "wx/file.h"
#include "wx/filectrl.h"
#include "wx/filename.h"
#include "wx/listctrl.h"
#include "wx/stopwatch.h"
#include "wx/weakref.h"

#ifdef __WXMSW__
    #include "wx/msw/wrapwin.h"
#endif

namespace
{

class ScopedFilesystemTree
{
public:
    ScopedFilesystemTree()
    {
        static unsigned long s_generation = 0;
        m_tempRoot = wxFileName::GetTempDir();
        m_path = m_tempRoot + wxFILE_SEP_PATH +
                 wxString::Format("wx-filesystemctrl-%lu-%lu",
                                  static_cast<unsigned long>(wxGetProcessId()),
                                  ++s_generation);
        m_created = wxFileName::Mkdir(
            m_path, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    }

    ~ScopedFilesystemTree()
    {
#ifdef __WXMSW__
        for ( const wxString& linkPath : m_directoryLinks )
            (void)::RemoveDirectoryW(AsExtendedPath(linkPath).wc_str());
        for ( const wxString& hiddenPath : m_hiddenPaths )
            ::SetFileAttributesW(hiddenPath.wc_str(), FILE_ATTRIBUTE_NORMAL);
        for ( const wxString& longPath : m_extendedPaths )
            (void)::DeleteFileW(AsExtendedPath(longPath).wc_str());
#endif

        // Recursive cleanup is deliberately constrained to our unique child
        // of the operating system temporary directory.
        const wxString prefix =
            m_tempRoot + wxFILE_SEP_PATH + "wx-filesystemctrl-";
        if ( m_created && m_path.StartsWith(prefix) )
            (void)wxFileName::Rmdir(m_path, wxPATH_RMDIR_RECURSIVE);
    }

    bool IsOk() const { return m_created; }
    const wxString& GetPath() const { return m_path; }

    wxString MakeDirectory(const wxString& relative) const
    {
        const wxString path = Join(relative);
        if ( wxDirExists(path) ||
             wxFileName::Mkdir(path, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL) )
        {
            return path;
        }

        return wxString();
    }

    wxString MakeFile(const wxString& relative,
                      const wxString& contents = "test") const
    {
        const wxString path = Join(relative);
        const wxString parent = wxFileName(path).GetPath();
        if ( !parent.empty() && !wxDirExists(parent) &&
             !wxFileName::Mkdir(
                 parent, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL) )
        {
            return wxString();
        }

#ifdef __WXMSW__
        if ( path.length() >= MAX_PATH )
        {
            const HANDLE handle =
                ::CreateFileW(AsExtendedPath(path).wc_str(),
                              GENERIC_WRITE,
                              0,
                              nullptr,
                              CREATE_ALWAYS,
                              FILE_ATTRIBUTE_NORMAL,
                              nullptr);
            if ( handle == INVALID_HANDLE_VALUE )
                return wxString();

            ::CloseHandle(handle);
            m_extendedPaths.push_back(path);
            return path;
        }
#endif

        wxFile file;
        if ( !file.Create(path, true) || !file.Write(contents) )
            return wxString();

        return path;
    }

#ifdef __WXMSW__
    bool MakeDirectoryLink(const wxString& relative,
                           const wxString& target)
    {
        const wxString path = Join(relative);
        DWORD flags = SYMBOLIC_LINK_FLAG_DIRECTORY;
#ifdef SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE
        flags |= SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE;
#endif
        if ( !::CreateSymbolicLinkW(path.wc_str(), target.wc_str(), flags) )
            return false;

        m_directoryLinks.push_back(path);
        return true;
    }

    static wxString AsExtendedPath(const wxString& path)
    {
        if ( path.StartsWith("\\\\") )
            return "\\\\?\\UNC\\" + path.Mid(2);

        return "\\\\?\\" + path;
    }

    bool Hide(const wxString& path)
    {
        if ( !::SetFileAttributesW(path.wc_str(), FILE_ATTRIBUTE_HIDDEN) )
            return false;

        m_hiddenPaths.push_back(path);
        return true;
    }
#endif

private:
    wxString Join(const wxString& relative) const
    {
        wxString normalized(relative);
        normalized.Replace("/", wxString(wxFILE_SEP_PATH));
        normalized.Replace("\\", wxString(wxFILE_SEP_PATH));
        return m_path + wxFILE_SEP_PATH + normalized;
    }

    wxString m_tempRoot;
    wxString m_path;
    bool m_created = false;
#ifdef __WXMSW__
    wxArrayString m_directoryLinks;
    wxArrayString m_hiddenPaths;
    mutable wxArrayString m_extendedPaths;
#endif
};

bool SamePath(const wxString& lhs, const wxString& rhs)
{
    return wxFileName(lhs).SameAs(wxFileName(rhs));
}

long FindListItem(wxListCtrl* list, const wxString& text)
{
    for ( long item = list->GetNextItem(-1, wxLIST_NEXT_ALL);
          item != wxNOT_FOUND;
          item = list->GetNextItem(item, wxLIST_NEXT_ALL) )
    {
        if ( list->GetItemText(item) == text )
            return item;
    }

    return wxNOT_FOUND;
}

int CountListItems(wxListCtrl* list, const wxString& text)
{
    int count = 0;
    for ( long item = list->GetNextItem(-1, wxLIST_NEXT_ALL);
          item != wxNOT_FOUND;
          item = list->GetNextItem(item, wxLIST_NEXT_ALL) )
    {
        if ( list->GetItemText(item) == text )
            ++count;
    }
    return count;
}

wxTreeItemId FindTreeChild(wxTreeCtrl* tree,
                           const wxTreeItemId& parent,
                           const wxString& text)
{
    wxTreeItemIdValue cookie;
    for ( wxTreeItemId child = tree->GetFirstChild(parent, cookie);
          child.IsOk();
          child = tree->GetNextChild(parent, cookie) )
    {
        if ( tree->GetItemText(child) == text )
            return child;
    }

    return wxTreeItemId();
}

wxString DescribeTreeChildren(wxGenericDirCtrl* ctrl,
                              const wxTreeItemId& parent)
{
    wxString description;
    wxTreeCtrl* const tree = ctrl->GetTreeCtrl();
    if ( !tree || !parent.IsOk() )
        return "<invalid>";

    wxTreeItemIdValue cookie;
    for ( wxTreeItemId child = tree->GetFirstChild(parent, cookie);
          child.IsOk();
          child = tree->GetNextChild(parent, cookie) )
    {
        if ( !description.empty() )
            description += "; ";
        description += tree->GetItemText(child);
        description += "=";
        description += ctrl->GetPath(child);
    }
    return description;
}

wxListItem GetListItem(wxListCtrl* list, long index)
{
    wxListItem item;
    item.SetId(index);
    item.SetMask(wxLIST_MASK_TEXT | wxLIST_MASK_DATA);
    CHECK(list->GetItem(item));
    return item;
}

#if wxUSE_DRAG_AND_DROP
class RecordingFileDropTarget final : public wxFileDropTarget
{
public:
    bool OnDropFiles(wxCoord x,
                     wxCoord y,
                     const wxArrayString& filenames) override
    {
        m_called = true;
        m_point = wxPoint(x, y);
        m_filenames = filenames;
        return true;
    }

    bool WasCalled() const { return m_called; }
    const wxPoint& GetPoint() const { return m_point; }
    const wxArrayString& GetFilenames() const { return m_filenames; }

private:
    bool m_called = false;
    wxPoint m_point;
    wxArrayString m_filenames;
};
#endif

} // anonymous namespace

TEST_CASE("wxGenericDirCtrl filters and refresh",
          "[filesystemctrl][dirctrl][dirfilterlistctrl]")
{
    ScopedFilesystemTree fs;
    REQUIRE(fs.IsOk());
    REQUIRE_FALSE(fs.MakeFile("alpha.txt").empty());
    REQUIRE_FALSE(fs.MakeFile("shared.log").empty());
    REQUIRE_FALSE(fs.MakeFile("ignored.bin").empty());

    wxGenericDirCtrl* const ctrl =
        new wxGenericDirCtrl(wxTheApp->GetTopWindow(), wxID_ANY, fs.GetPath(),
                             wxDefaultPosition, wxSize(480, 280),
                             wxDIRCTRL_DEFAULT_STYLE |
                                 wxDIRCTRL_SHOW_FILTERS,
                             "Text and log|*.txt;*.log|Log only|*.log", 0);

    wxTreeCtrl* const tree = ctrl->GetTreeCtrl();
    wxDirFilterListCtrl* const choice = ctrl->GetFilterListCtrl();
    REQUIRE(tree);
    REQUIRE(choice);
    REQUIRE(choice->GetCount() == 2);
    CHECK(choice->GetString(0) == "Text and log");
    CHECK(choice->GetString(1) == "Log only");
    CHECK(ctrl->GetFilterIndex() == 0);
    INFO("selected path: " << ctrl->GetPath());
    INFO("root children: " <<
         DescribeTreeChildren(ctrl, ctrl->GetRootId()));
    CHECK(SamePath(ctrl->GetPath(), fs.GetPath()));

    wxTreeItemId selected = tree->GetSelection();
    REQUIRE(selected.IsOk());
    CHECK(FindTreeChild(tree, selected, "alpha.txt").IsOk());
    CHECK(FindTreeChild(tree, selected, "shared.log").IsOk());
    CHECK_FALSE(FindTreeChild(tree, selected, "ignored.bin").IsOk());

    int selectionEvents = 0;
    int activationEvents = 0;
    ctrl->Bind(wxEVT_DIRCTRL_SELECTIONCHANGED,
               [&selectionEvents](wxTreeEvent&)
               {
                   ++selectionEvents;
               });
    ctrl->Bind(wxEVT_DIRCTRL_FILEACTIVATED,
               [&activationEvents](wxTreeEvent&)
               {
                   ++activationEvents;
               });
    ctrl->SetPath(fs.GetPath() + wxFILE_SEP_PATH + "alpha.txt");
    CHECK(selectionEvents == 1);
    CHECK(SamePath(ctrl->GetFilePath(),
                   fs.GetPath() + wxFILE_SEP_PATH + "alpha.txt"));

    wxTreeItemId selectedFile = tree->GetSelection();
    REQUIRE(selectedFile.IsOk());
    wxTreeEvent activate(
        wxEVT_TREE_ITEM_ACTIVATED, tree, selectedFile);
    (void)tree->ProcessWindowEvent(activate);
    CHECK(activationEvents == 1);
    ctrl->SetPath(fs.GetPath());

    ctrl->SetFilterIndex(-1);
    ctrl->SetFilterIndex(2);
    CHECK(ctrl->GetFilterIndex() == 0);
    CHECK(choice->GetSelection() == 0);

    choice->SetSelection(1);
    wxCommandEvent change(wxEVT_CHOICE, choice->GetId());
    change.SetEventObject(choice);
    change.SetInt(1);
    REQUIRE(choice->ProcessWindowEvent(change));
    CHECK(ctrl->GetFilterIndex() == 1);
    CHECK(choice->GetSelection() == 1);
    CHECK(SamePath(ctrl->GetPath(), fs.GetPath()));

    selected = tree->GetSelection();
    REQUIRE(selected.IsOk());
    CHECK_FALSE(FindTreeChild(tree, selected, "alpha.txt").IsOk());
    CHECK(FindTreeChild(tree, selected, "shared.log").IsOk());

    ctrl->SetFilter("Everything|*.*|Text|*.txt");
    REQUIRE(choice->GetCount() == 2);
    CHECK(choice->GetString(0) == "Everything");
    CHECK(choice->GetString(1) == "Text");
    CHECK(ctrl->GetFilterIndex() == 0);
    CHECK(choice->GetSelection() == 0);

    wxDirFilterListCtrl* const originalChoice = choice;
    ctrl->SetFilter(wxString());
    CHECK(ctrl->GetFilter().empty());
    CHECK_FALSE(originalChoice->IsShown());
    CHECK(originalChoice->GetCount() == 0);

    ctrl->SetFilter("Text|*.txt");
    CHECK(ctrl->GetFilterListCtrl() == originalChoice);
    CHECK(originalChoice->IsShown());
    REQUIRE(originalChoice->GetCount() == 1);
    CHECK(originalChoice->GetString(0) == "Text");
    CHECK(originalChoice->Enable(false));
    CHECK_FALSE(originalChoice->IsEnabled());
    CHECK(originalChoice->Enable());

#if wxUSE_DRAG_AND_DROP
    RecordingFileDropTarget* const dropTarget =
        new RecordingFileDropTarget;
    tree->SetDropTarget(dropTarget);
    REQUIRE(tree->GetDropTarget() == dropTarget);
    wxArrayString droppedFiles;
    droppedFiles.Add(fs.GetPath() + wxFILE_SEP_PATH + "alpha.txt");
    CHECK(dropTarget->OnDropFiles(7, 11, droppedFiles));
    CHECK(dropTarget->WasCalled());
    CHECK(dropTarget->GetPoint() == wxPoint(7, 11));
    CHECK(dropTarget->GetFilenames() == droppedFiles);
#endif

    delete ctrl;

    wxGenericDirCtrl* const invalidDefault =
        new wxGenericDirCtrl(wxTheApp->GetTopWindow(), wxID_ANY, fs.GetPath(),
                             wxDefaultPosition, wxSize(480, 280),
                             wxDIRCTRL_DEFAULT_STYLE |
                                 wxDIRCTRL_SHOW_FILTERS,
                             "Text|*.txt|Log|*.log", 42);
    REQUIRE(invalidDefault->GetFilterListCtrl());
    CHECK(invalidDefault->GetFilterIndex() == 0);
    CHECK(invalidDefault->GetFilterListCtrl()->GetSelection() == 0);
    CHECK(SamePath(invalidDefault->GetPath(), fs.GetPath()));
    delete invalidDefault;
}

TEST_CASE("wxGenericDirCtrl mutation Unicode hidden and long paths",
          "[filesystemctrl][dirctrl]")
{
    ScopedFilesystemTree fs;
    REQUIRE(fs.IsOk());
    REQUIRE_FALSE(fs.MakeFile("visible.txt").empty());
    REQUIRE_FALSE(fs.MakeDirectory("rename-me").empty());
    REQUIRE_FALSE(fs.MakeFile("rename-me/child.txt").empty());
#ifdef __WXMSW__
    const bool hasDirectoryLink =
        fs.MakeDirectoryLink("cycle", fs.GetPath());
#endif

#ifdef __WXMSW__
    const wxString hidden = fs.MakeFile("hidden.txt");
    REQUIRE_FALSE(hidden.empty());
    REQUIRE(fs.Hide(hidden));
#endif

    wxGenericDirCtrl* const ctrl =
        new wxGenericDirCtrl(wxTheApp->GetTopWindow(), wxID_ANY, fs.GetPath(),
                             wxDefaultPosition, wxSize(480, 280),
                             wxDIRCTRL_DEFAULT_STYLE |
                                 wxDIRCTRL_EDIT_LABELS,
                             "All|*.*");
    wxTreeCtrl* const tree = ctrl->GetTreeCtrl();
    REQUIRE(tree);
    REQUIRE(SamePath(ctrl->GetPath(), fs.GetPath()));

    wxTreeItemId selected = tree->GetSelection();
    REQUIRE(selected.IsOk());
    CHECK(FindTreeChild(tree, selected, "visible.txt").IsOk());
#ifdef __WXMSW__
    CHECK_FALSE(FindTreeChild(tree, selected, "hidden.txt").IsOk());
    ctrl->ShowHidden(true);
    REQUIRE(SamePath(ctrl->GetPath(), fs.GetPath()));
    selected = tree->GetSelection();
    REQUIRE(selected.IsOk());
    CHECK(FindTreeChild(tree, selected, "hidden.txt").IsOk());
#endif

    REQUIRE_FALSE(fs.MakeFile("created-after-refresh.txt").empty());
    ctrl->ReCreateTree();
    REQUIRE(SamePath(ctrl->GetPath(), fs.GetPath()));
    selected = tree->GetSelection();
    REQUIRE(selected.IsOk());
    CHECK(FindTreeChild(tree, selected, "created-after-refresh.txt").IsOk());

#ifdef __WXMSW__
    if ( hasDirectoryLink )
    {
        wxString boundedCycle =
            fs.GetPath() + wxFILE_SEP_PATH + "cycle";
        for ( int i = 0; i < 4; ++i )
            boundedCycle += wxFILE_SEP_PATH + wxString("cycle");

        wxStopWatch stopwatch;
        ctrl->SetPath(boundedCycle);
        CHECK(stopwatch.Time() < 5000);
        CHECK(ctrl->GetPath().CmpNoCase(boundedCycle) == 0);
    }
    else
    {
        WARN("Directory symlink creation is unavailable; bounded cycle "
             "coverage remains capability-gated on this machine.");
    }
#endif

    const wxString renamePath =
        fs.GetPath() + wxFILE_SEP_PATH + "rename-me";
    ctrl->SetPath(renamePath);
    REQUIRE(SamePath(ctrl->GetPath(), renamePath));
    wxTreeItemId renameItem = tree->GetSelection();
    REQUIRE(renameItem.IsOk());

    // Repeated rebuild requests from deletion events must be coalesced until
    // native deletion returns, including events sent by the rebuild itself.
    // Keep the control/tree alive to also exercise stale item ID/data handling.
    bool rebuildFromDelete = true;
    bool inDeleteCallback = false;
    bool recursiveDeleteCallback = false;
    unsigned rebuildRequests = 0;
    tree->Bind(
        wxEVT_TREE_DELETE_ITEM,
        [ctrl, &rebuildFromDelete, &inDeleteCallback,
         &recursiveDeleteCallback, &rebuildRequests](wxTreeEvent& event)
        {
            if ( rebuildFromDelete )
            {
                if ( inDeleteCallback )
                {
                    recursiveDeleteCallback = true;
                }
                else
                {
                    inDeleteCallback = true;
                    ++rebuildRequests;
                    ctrl->ReCreateTree();
                    ctrl->ReCreateTree();
                    inDeleteCallback = false;
                }
            }
            event.Skip();
        });

    wxTreeEvent rename(wxEVT_TREE_END_LABEL_EDIT, tree, renameItem);
    rename.SetLabel("renamed-\u00e9");
    ctrl->OnEndEditItem(rename);
    rebuildFromDelete = false;
    CHECK(rename.IsAllowed());
    CHECK(rebuildRequests > 0);
    CHECK_FALSE(recursiveDeleteCallback);
    CHECK(wxDirExists(fs.GetPath() + wxFILE_SEP_PATH + "renamed-\u00e9"));
    REQUIRE(SamePath(ctrl->GetPath(), fs.GetPath()));
    selected = tree->GetSelection();
    REQUIRE(selected.IsOk());
    CHECK(FindTreeChild(tree, selected, "renamed-\u00e9").IsOk());

    wxString relativeLong;
    for ( int i = 0;
          fs.GetPath().length() + 1 + relativeLong.length() < 220;
          ++i )
    {
        if ( !relativeLong.empty() )
            relativeLong += wxFILE_SEP_PATH;
        relativeLong += wxString::Format("segment-%02d-abcdefghij", i);
    }
    relativeLong += wxFILE_SEP_PATH;
    relativeLong +=
        "fichier-" + wxString(64, 'x') + "-\u65e5\u672c\u8a9e.txt";

    const wxString longFile = fs.MakeFile(relativeLong);
    REQUIRE_FALSE(longFile.empty());
    CHECK(longFile.length() > 260);
    ctrl->SetPath(longFile);
    INFO("long path: " << longFile);
    INFO("selected path: " << ctrl->GetPath());
    INFO("selected file: " << ctrl->GetFilePath());
    CHECK(SamePath(ctrl->GetFilePath(), longFile));

    delete ctrl;
}

TEST_CASE("wxDirFilterListCtrl association and callback lifetime",
          "[filesystemctrl][dirctrl][dirfilterlistctrl]")
{
    ScopedFilesystemTree fs;
    REQUIRE(fs.IsOk());
    const wxString child = fs.MakeDirectory("child");
    REQUIRE_FALSE(child.empty());

    wxGenericDirCtrl* ctrl =
        new wxGenericDirCtrl(wxTheApp->GetTopWindow(), wxID_ANY, fs.GetPath(),
                             wxDefaultPosition, wxSize(480, 280),
                             wxDIRCTRL_DEFAULT_STYLE |
                                 wxDIRCTRL_SHOW_FILTERS,
                             "All|*.*");
    wxWeakRef<wxWindow> weakCtrl(ctrl);
    ctrl->Bind(wxEVT_DIRCTRL_SELECTIONCHANGED,
               [ctrl](wxTreeEvent&)
               {
                   delete ctrl;
               });
    ctrl->SetPath(child);
    CHECK_FALSE(weakCtrl);

    wxGenericDirCtrl* rawEventCtrl =
        new wxGenericDirCtrl(wxTheApp->GetTopWindow(), wxID_ANY, fs.GetPath(),
                             wxDefaultPosition, wxSize(480, 280),
                             wxDIRCTRL_DEFAULT_STYLE |
                                 wxDIRCTRL_SHOW_FILTERS,
                             "All|*.*");
    wxTreeCtrl* const rawEventTree = rawEventCtrl->GetTreeCtrl();
    wxWeakRef<wxWindow> weakRawEventCtrl(rawEventCtrl);
    bool rawEventDeleted = false;
    rawEventTree->Bind(
        wxEVT_TREE_SEL_CHANGED,
        [rawEventCtrl, &rawEventDeleted](wxTreeEvent&)
        {
            if ( !rawEventDeleted )
            {
                rawEventDeleted = true;
                delete rawEventCtrl;
            }
        });
    rawEventCtrl->SetPath(child);
    CHECK(rawEventDeleted);
    CHECK_FALSE(weakRawEventCtrl.get());

    wxPanel* const survivorParent =
        new wxPanel(wxTheApp->GetTopWindow(), wxID_ANY);
    wxGenericDirCtrl* associated =
        new wxGenericDirCtrl(wxTheApp->GetTopWindow(), wxID_ANY, fs.GetPath(),
                             wxDefaultPosition, wxSize(480, 280),
                             wxDIRCTRL_DEFAULT_STYLE |
                                 wxDIRCTRL_SHOW_FILTERS,
                             "All|*.*|Text|*.txt");
    wxDirFilterListCtrl* const survivingChoice =
        associated->GetFilterListCtrl();
    REQUIRE(survivingChoice);
    REQUIRE(survivingChoice->Reparent(survivorParent));
    delete associated;

    survivingChoice->SetSelection(1);
    wxCommandEvent staleChange(wxEVT_CHOICE, survivingChoice->GetId());
    staleChange.SetEventObject(survivingChoice);
    staleChange.SetInt(1);
    CHECK(survivingChoice->ProcessWindowEvent(staleChange));
    delete survivorParent;

    wxGenericDirCtrl* const recreated =
        new wxGenericDirCtrl(wxTheApp->GetTopWindow(), wxID_ANY, fs.GetPath(),
                             wxDefaultPosition, wxSize(480, 280),
                             wxDIRCTRL_DEFAULT_STYLE |
                                 wxDIRCTRL_SHOW_FILTERS,
                             "All|*.*");
    wxDirFilterListCtrl* const oldChoice = recreated->GetFilterListCtrl();
    REQUIRE(oldChoice);
    wxWeakRef<wxWindow> weakOldChoice(oldChoice);
    delete oldChoice;
    CHECK(recreated->GetFilterListCtrl() == nullptr);
    CHECK_FALSE(weakOldChoice.get());

    recreated->SetFilter("Text|*.txt");
    REQUIRE(recreated->GetFilterListCtrl());
    CHECK(recreated->GetFilterListCtrl()->GetCount() == 1);
    CHECK_FALSE(weakOldChoice.get());

    wxGenericDirCtrl* const treeChildOwner =
        new wxGenericDirCtrl(wxTheApp->GetTopWindow(), wxID_ANY, fs.GetPath(),
                             wxDefaultPosition, wxSize(480, 280),
                             wxDIRCTRL_DEFAULT_STYLE |
                                 wxDIRCTRL_SHOW_FILTERS,
                             "All|*.*");
    wxTreeCtrl* const independentlyDestroyedTree =
        treeChildOwner->GetTreeCtrl();
    REQUIRE(independentlyDestroyedTree);
    wxWeakRef<wxWindow> weakTree(independentlyDestroyedTree);
    delete independentlyDestroyedTree;
    CHECK_FALSE(weakTree.get());
    CHECK(treeChildOwner->GetTreeCtrl() == nullptr);
    CHECK(treeChildOwner->GetPath().empty());
    CHECK(treeChildOwner->GetFilePath().empty());
    wxArrayString emptyPaths;
    treeChildOwner->GetPaths(emptyPaths);
    CHECK(emptyPaths.empty());
    treeChildOwner->ShowHidden(true);
    treeChildOwner->SetFilterIndex(0);
    treeChildOwner->SelectPath(child);
    treeChildOwner->UnselectAll();
    delete treeChildOwner;

    wxPanel* const reparentedOwner =
        new wxPanel(wxTheApp->GetTopWindow(), wxID_ANY);
    REQUIRE(recreated->Reparent(reparentedOwner));
    recreated->SetPath(child);
    CHECK(SamePath(recreated->GetPath(), child));
    delete reparentedOwner;
}

TEST_CASE("wxGenericFileCtrl API filters refresh and modes",
          "[filesystemctrl][filectrl]")
{
    ScopedFilesystemTree fs;
    REQUIRE(fs.IsOk());
    const wxString alpha = fs.MakeFile("alpha.txt");
    const wxString shared = fs.MakeFile("shared.log");
    const wxString unicodeLog = fs.MakeFile("journal-\u65e5\u672c\u8a9e.log");
    REQUIRE_FALSE(alpha.empty());
    REQUIRE_FALSE(shared.empty());
    REQUIRE_FALSE(unicodeLog.empty());
    REQUIRE_FALSE(fs.MakeFile("ignored.bin").empty());
    const wxString navigationDir = fs.MakeDirectory("navigation");
    REQUIRE_FALSE(navigationDir.empty());

    wxGenericFileCtrl* const ctrl =
        new wxGenericFileCtrl(wxTheApp->GetTopWindow(), wxID_ANY, fs.GetPath(),
                              wxString(),
                              "Text|*.txt|Log|*.log",
                              wxFC_OPEN);
    wxFileListCtrl* const list = ctrl->GetFileList();
    REQUIRE(list);
    CHECK(wxDynamicCast(ctrl, wxGenericFileCtrl) != nullptr);
    CHECK_FALSE(ctrl->HasMultipleFileSelection());
    CHECK(ctrl->GetFilterIndex() == 0);
    CHECK(FindListItem(list, "alpha.txt") != wxNOT_FOUND);
    CHECK(FindListItem(list, "shared.log") == wxNOT_FOUND);

    int filterEvents = 0;
    int filterIndexFromEvent = wxNOT_FOUND;
    ctrl->Bind(wxEVT_FILECTRL_FILTERCHANGED,
               [&filterEvents, &filterIndexFromEvent](wxFileCtrlEvent& event)
               {
                   ++filterEvents;
                   filterIndexFromEvent = event.GetFilterIndex();
               });
    ctrl->SetFilterIndex(1);
    CHECK(ctrl->GetFilterIndex() == 1);
    CHECK(filterEvents == 1);
    CHECK(filterIndexFromEvent == 1);
    CHECK(FindListItem(list, "alpha.txt") == wxNOT_FOUND);
    CHECK(FindListItem(list, "shared.log") != wxNOT_FOUND);
    CHECK(FindListItem(list, "journal-\u65e5\u672c\u8a9e.log") != wxNOT_FOUND);

    ctrl->SetFilterIndex(-1);
    ctrl->SetFilterIndex(2);
    CHECK(ctrl->GetFilterIndex() == 1);
    CHECK(filterEvents == 1);

    CHECK_FALSE(ctrl->SetFilename("alpha.txt"));
    CHECK(ctrl->GetFilename().empty());
    CHECK_FALSE(ctrl->SetPath(alpha));
    CHECK(SamePath(ctrl->GetDirectory(), fs.GetPath()));
    CHECK(ctrl->GetFilename().empty());
    REQUIRE(ctrl->SetFilename("shared.log"));
    CHECK(ctrl->GetFilename() == "shared.log");

    int activationEvents = 0;
    ctrl->Bind(wxEVT_FILECTRL_FILEACTIVATED,
               [&activationEvents](wxFileCtrlEvent&)
               {
                   ++activationEvents;
               });
    const long sharedIndex = FindListItem(list, "shared.log");
    REQUIRE(sharedIndex != wxNOT_FOUND);
    wxListEvent activate(wxEVT_LIST_ITEM_ACTIVATED, list->GetId());
    activate.SetEventObject(list);
    activate.SetIndex(sharedIndex);
    activate.SetItem(GetListItem(list, sharedIndex));
    REQUIRE(list->ProcessWindowEvent(activate));
    CHECK(activationEvents == 1);

    int folderEvents = 0;
    ctrl->Bind(wxEVT_FILECTRL_FOLDERCHANGED,
               [&folderEvents](wxFileCtrlEvent&)
               {
                   ++folderEvents;
               });
    REQUIRE(ctrl->SetDirectory(navigationDir));
    ctrl->GoToParentDir();
    CHECK(SamePath(ctrl->GetDirectory(), fs.GetPath()));
    CHECK(ctrl->GetFilename().empty());
    CHECK(folderEvents == 1);

    const long navigationIndex = FindListItem(list, "navigation");
    REQUIRE(navigationIndex != wxNOT_FOUND);
    wxListEvent navigate(wxEVT_LIST_ITEM_ACTIVATED, list->GetId());
    navigate.SetEventObject(list);
    navigate.SetIndex(navigationIndex);
    navigate.SetItem(GetListItem(list, navigationIndex));
    REQUIRE(list->ProcessWindowEvent(navigate));
    CHECK(folderEvents == 2);
    CHECK(SamePath(ctrl->GetDirectory(), navigationDir));
    REQUIRE(ctrl->SetDirectory(fs.GetPath()));

    ctrl->SetWildcard("Overlapping|*.log;shared.*");
    CHECK(ctrl->GetFilterIndex() == 0);
    CHECK(CountListItems(list, "shared.log") == 1);

    ctrl->ChangeToReportMode();
    CHECK(list->InReportView());
    CHECK(list->GetColumnCount() >= 4);
    ctrl->ChangeToListMode();
    CHECK_FALSE(list->InReportView());

    REQUIRE_FALSE(fs.MakeFile("created.log").empty());
    list->UpdateFiles();
    CHECK(FindListItem(list, "created.log") != wxNOT_FOUND);

    wxString relativeLong;
    for ( int i = 0;
          fs.GetPath().length() + 1 + relativeLong.length() < 220;
          ++i )
    {
        if ( !relativeLong.empty() )
            relativeLong += wxFILE_SEP_PATH;
        relativeLong += wxString::Format("folder-%02d-abcdefghijk", i);
    }
    relativeLong += wxFILE_SEP_PATH;
    relativeLong += "long-" + wxString(64, 'x') + "-\u00e9.log";
    const wxString longLog = fs.MakeFile(relativeLong);
    REQUIRE_FALSE(longLog.empty());
    CHECK(longLog.length() > 260);
    REQUIRE(ctrl->SetPath(longLog));
    CHECK(ctrl->GetFilename() ==
          "long-" + wxString(64, 'x') + "-\u00e9.log");
    CHECK(SamePath(ctrl->GetPath(), longLog));

#if wxUSE_DRAG_AND_DROP
    RecordingFileDropTarget* const dropTarget =
        new RecordingFileDropTarget;
    list->SetDropTarget(dropTarget);
    REQUIRE(list->GetDropTarget() == dropTarget);
    wxArrayString droppedFiles;
    droppedFiles.Add(shared);
    droppedFiles.Add(unicodeLog);
    CHECK(dropTarget->OnDropFiles(13, 17, droppedFiles));
    CHECK(dropTarget->WasCalled());
    CHECK(dropTarget->GetPoint() == wxPoint(13, 17));
    CHECK(dropTarget->GetFilenames() == droppedFiles);
#endif

    delete ctrl;

    wxGenericFileCtrl* const saveCtrl =
        new wxGenericFileCtrl(wxTheApp->GetTopWindow(), wxID_ANY, fs.GetPath(),
                              wxString(), "Text|*.txt", wxFC_SAVE);
    REQUIRE(saveCtrl->SetFilename("new-document.txt"));
    CHECK(saveCtrl->GetFilename() == "new-document.txt");
    delete saveCtrl;
}

TEST_CASE("wxGenericFileCtrl multiselect and rename",
          "[filesystemctrl][filectrl]")
{
    ScopedFilesystemTree fs;
    REQUIRE(fs.IsOk());
    REQUIRE_FALSE(fs.MakeFile("first.txt").empty());
    REQUIRE_FALSE(fs.MakeFile("second.txt").empty());

    wxGenericFileCtrl* const ctrl =
        new wxGenericFileCtrl(wxTheApp->GetTopWindow(), wxID_ANY, fs.GetPath(),
                              wxString(), "Text|*.txt",
                              wxFC_OPEN | wxFC_MULTIPLE);
    wxFileListCtrl* const list = ctrl->GetFileList();
    REQUIRE(list);
    CHECK(ctrl->HasMultipleFileSelection());
    const long first = FindListItem(list, "first.txt");
    const long second = FindListItem(list, "second.txt");
    REQUIRE(first != wxNOT_FOUND);
    REQUIRE(second != wxNOT_FOUND);

    list->SetItemState(first, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
    list->SetItemState(second, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
    CHECK(list->GetSelectedItemCount() == 2);

    wxArrayString names;
    ctrl->GetFilenames(names);
    REQUIRE(names.size() == 2);
    CHECK(names.Index("first.txt") != wxNOT_FOUND);
    CHECK(names.Index("second.txt") != wxNOT_FOUND);

    wxListItem renamedItem = GetListItem(list, first);
    renamedItem.SetText("renamed-\u00e9.txt");
    wxListEvent rename(wxEVT_LIST_END_LABEL_EDIT, list->GetId());
    rename.SetEventObject(list);
    rename.SetIndex(first);
    rename.SetItem(renamedItem);
    list->OnListEndLabelEdit(rename);
    CHECK(rename.IsAllowed());
    CHECK(wxFileExists(fs.GetPath() + wxFILE_SEP_PATH +
                       "renamed-\u00e9.txt"));

    delete ctrl;
}

TEST_CASE("wxGenericFileCtrl callback destruction",
          "[filesystemctrl][filectrl]")
{
    ScopedFilesystemTree fs;
    REQUIRE(fs.IsOk());
    REQUIRE_FALSE(fs.MakeFile("selected.txt").empty());
    REQUIRE_FALSE(fs.MakeFile("ignored.bin").empty());
    REQUIRE_FALSE(fs.MakeDirectory("folder").empty());

    wxGenericFileCtrl* const reentrantCtrl =
        new wxGenericFileCtrl(wxTheApp->GetTopWindow(), wxID_ANY, fs.GetPath(),
                              wxString(), "All|*.*", wxFC_OPEN);
    wxFileListCtrl* const reentrantList = reentrantCtrl->GetFileList();
    bool reentered = false;
    reentrantList->Bind(
        wxEVT_LIST_INSERT_ITEM,
        [reentrantList, &reentered](wxListEvent& event)
        {
            if ( !reentered )
            {
                reentered = true;
                reentrantList->SetWild("*.txt");
            }
            event.Skip();
        });
    reentrantList->UpdateFiles();
    CHECK(reentered);
    CHECK(reentrantList->GetWild() == "*.txt");
    CHECK(FindListItem(reentrantList, "selected.txt") != wxNOT_FOUND);
    CHECK(FindListItem(reentrantList, "ignored.bin") == wxNOT_FOUND);
    delete reentrantCtrl;

    wxGenericFileCtrl* const filterReentrantCtrl =
        new wxGenericFileCtrl(wxTheApp->GetTopWindow(), wxID_ANY, fs.GetPath(),
                              wxString(),
                              "All|*.*|Binary|*.bin",
                              wxFC_OPEN);
    wxFileListCtrl* const filterReentrantList =
        filterReentrantCtrl->GetFileList();
    bool filterReentered = false;
    int filterIndexSeenDuringInsert = wxNOT_FOUND;
    filterReentrantList->Bind(
        wxEVT_LIST_INSERT_ITEM,
        [filterReentrantCtrl, &filterReentered,
         &filterIndexSeenDuringInsert](wxListEvent& event)
        {
            if ( !filterReentered )
            {
                filterReentered = true;
                filterIndexSeenDuringInsert =
                    filterReentrantCtrl->GetFilterIndex();
                filterReentrantCtrl->SetWildcard("Text|*.txt");
            }
            event.Skip();
        });
    filterReentrantCtrl->SetFilterIndex(1);
    CHECK(filterReentered);
    CHECK(filterIndexSeenDuringInsert == 1);
    CHECK(filterReentrantCtrl->GetWildcard() == "Text|*.txt");
    CHECK(filterReentrantCtrl->GetFilterIndex() == 0);
    CHECK(filterReentrantList->GetWild() == "*.txt");
    CHECK(FindListItem(filterReentrantList, "selected.txt") != wxNOT_FOUND);
    CHECK(FindListItem(filterReentrantList, "ignored.bin") == wxNOT_FOUND);
    delete filterReentrantCtrl;

    wxGenericFileCtrl* deleteAllCtrl =
        new wxGenericFileCtrl(wxTheApp->GetTopWindow(), wxID_ANY, fs.GetPath(),
                              wxString(), "All|*.*", wxFC_OPEN);
    wxFileListCtrl* const deleteAllList = deleteAllCtrl->GetFileList();
    wxWeakRef<wxFileListCtrl> weakDeleteAllList(deleteAllList);
    bool deleteAllCallback = false;
    deleteAllList->Bind(
        wxEVT_LIST_DELETE_ALL_ITEMS,
        [deleteAllCtrl, &deleteAllCallback](wxListEvent&)
        {
            if ( !deleteAllCallback )
            {
                deleteAllCallback = true;
                delete deleteAllCtrl;
            }
        });
    deleteAllList->UpdateFiles();
    CHECK(deleteAllCallback);
    CHECK_FALSE(weakDeleteAllList.get());

    wxGenericFileCtrl* insertCtrl =
        new wxGenericFileCtrl(wxTheApp->GetTopWindow(), wxID_ANY, fs.GetPath(),
                              wxString(), "All|*.*", wxFC_OPEN);
    wxFileListCtrl* const insertList = insertCtrl->GetFileList();
    wxWeakRef<wxFileListCtrl> weakInsertList(insertList);
    bool insertCallback = false;
    insertList->Bind(
        wxEVT_LIST_INSERT_ITEM,
        [insertCtrl, &insertCallback](wxListEvent&)
        {
            insertCallback = true;
            delete insertCtrl;
        });
    insertList->UpdateFiles();
    CHECK(insertCallback);
    CHECK_FALSE(weakInsertList.get());

    wxGenericFileCtrl* navigationCtrl =
        new wxGenericFileCtrl(wxTheApp->GetTopWindow(), wxID_ANY, fs.GetPath(),
                              wxString(), "All|*.*", wxFC_OPEN);
    wxFileListCtrl* const navigationList = navigationCtrl->GetFileList();
    wxWeakRef<wxFileListCtrl> weakNavigationList(navigationList);
    bool navigationCallback = false;
    navigationList->Bind(
        wxEVT_LIST_DELETE_ALL_ITEMS,
        [navigationCtrl, &navigationCallback](wxListEvent&)
        {
            if ( !navigationCallback )
            {
                navigationCallback = true;
                delete navigationCtrl;
            }
        });
    navigationList->GoToDir(
        fs.GetPath() + wxFILE_SEP_PATH + "folder");
    CHECK(navigationCallback);
    CHECK_FALSE(weakNavigationList.get());

    wxGenericFileCtrl* selectedCtrl =
        new wxGenericFileCtrl(wxTheApp->GetTopWindow(), wxID_ANY, fs.GetPath(),
                              wxString(), "All|*.*", wxFC_OPEN);
    wxFileListCtrl* selectedList = selectedCtrl->GetFileList();
    const long selectedIndex = FindListItem(selectedList, "selected.txt");
    REQUIRE(selectedIndex != wxNOT_FOUND);
    wxWeakRef<wxWindow> weakSelected(selectedCtrl);
    selectedCtrl->Bind(wxEVT_FILECTRL_SELECTIONCHANGED,
                       [selectedCtrl](wxFileCtrlEvent&)
                       {
                           delete selectedCtrl;
                       });
    selectedList->SetItemState(
        selectedIndex, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
    CHECK_FALSE(weakSelected);

    wxGenericFileCtrl* folderCtrl =
        new wxGenericFileCtrl(wxTheApp->GetTopWindow(), wxID_ANY, fs.GetPath(),
                              wxString(), "All|*.*", wxFC_OPEN);
    wxFileListCtrl* const folderList = folderCtrl->GetFileList();
    const long folderIndex = FindListItem(folderList, "folder");
    REQUIRE(folderIndex != wxNOT_FOUND);
    wxWeakRef<wxWindow> weakFolder(folderCtrl);
    folderCtrl->Bind(wxEVT_FILECTRL_FOLDERCHANGED,
                     [folderCtrl](wxFileCtrlEvent&)
                     {
                         delete folderCtrl;
                     });

    wxListEvent activate(wxEVT_LIST_ITEM_ACTIVATED, folderList->GetId());
    activate.SetEventObject(folderList);
    activate.SetIndex(folderIndex);
    activate.SetItem(GetListItem(folderList, folderIndex));
    REQUIRE(folderList->ProcessWindowEvent(activate));
    CHECK_FALSE(weakFolder);

    wxPanel* const parent =
        new wxPanel(wxTheApp->GetTopWindow(), wxID_ANY);
    wxGenericFileCtrl* const childCtrl =
        new wxGenericFileCtrl(parent, wxID_ANY, fs.GetPath(),
                              wxString(), "All|*.*", wxFC_OPEN);
    wxWeakRef<wxGenericFileCtrl> weakChild(childCtrl);
    delete parent;
    CHECK_FALSE(weakChild.get());
}

#endif // wxUSE_DIRDLG && wxUSE_FILECTRL && wxUSE_TREECTRL && wxUSE_LISTCTRL
