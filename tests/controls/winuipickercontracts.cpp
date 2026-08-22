///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuipickercontracts.cpp
// Purpose:     Deterministic WinUI picker composite contract tests
// Author:      wxWidgets development team
// Created:     2026-07-30
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && \
        wxUSE_COLOURPICKERCTRL && wxUSE_DIRPICKERCTRL && \
        wxUSE_FILEPICKERCTRL && wxUSE_FONTPICKERCTRL

#include "wx/app.h"
#include "wx/button.h"
#include "wx/clrpicker.h"
#include "wx/choice.h"
#include "wx/dialog.h"
#include "wx/dirdlg.h"
#include "wx/filedlg.h"
#include "wx/filename.h"
#include "wx/filepicker.h"
#include "wx/fontdlg.h"
#include "wx/fontpicker.h"
#include "wx/frame.h"
#include "wx/generic/fontdlgg.h"
#include "wx/modalhook.h"
#include "wx/msw/wrapwin.h"
#include "wx/panel.h"
#include "wx/sizer.h"
#include "wx/textctrl.h"
#include "wx/validate.h"
#include "wx/weakref.h"
#include "wx/winui/winui.h"

#if wxUSE_XRC
    #include "wx/sstream.h"
    #include "wx/xml/xml.h"
    #include "wx/xrc/xmlres.h"
#endif

#include "waitfor.h"

#include <memory>

namespace
{

enum class PickerDialogKind
{
    None,
    File,
    Directory,
    Font
};

class PickerModalHook final : public wxModalDialogHook
{
public:
    PickerModalHook()
    {
        Register();
    }

    int Enter(wxDialog *dialog) override
    {
        ++calls;
        lastKind = PickerDialogKind::None;
        fontConfigured = false;

        if ( wxFileDialog * const file =
                 wxDynamicCast(dialog, wxFileDialog) )
        {
            lastKind = PickerDialogKind::File;
            if ( result == wxID_OK )
                file->SetPath(path);
        }
        else if ( wxDirDialog * const directory =
                      wxDynamicCast(dialog, wxDirDialog) )
        {
            lastKind = PickerDialogKind::Directory;
            if ( result == wxID_OK )
                directory->SetPath(path);
        }
        else if ( wxFontDialog * const font =
                      wxDynamicCast(dialog, wxFontDialog) )
        {
            lastKind = PickerDialogKind::Font;
            if ( configureFont )
                ConfigureFont(font);
        }

        if ( windowToReparent )
        {
            reparentSucceeded =
                windowToReparent->Reparent(reparentTarget);
            windowToReparent = nullptr;
        }

        if ( windowToDelete )
        {
            wxWindow * const doomed = windowToDelete;
            windowToDelete = nullptr;
            delete doomed;
        }

        return result;
    }

    int result = wxID_CANCEL;
    wxString path;
    bool configureFont = false;
    wxWindow *windowToReparent = nullptr;
    wxWindow *reparentTarget = nullptr;
    wxWindow *windowToDelete = nullptr;

    int calls = 0;
    PickerDialogKind lastKind = PickerDialogKind::None;
    bool fontConfigured = false;
    bool reparentSucceeded = false;

private:
    void ConfigureFont(wxFontDialog *dialog)
    {
        wxChoice * const family =
            wxDynamicCast(
                dialog->FindWindow(wxID_FONT_FAMILY), wxChoice);
        wxChoice * const style =
            wxDynamicCast(
                dialog->FindWindow(wxID_FONT_STYLE), wxChoice);
        wxChoice * const weight =
            wxDynamicCast(
                dialog->FindWindow(wxID_FONT_WEIGHT), wxChoice);
        wxChoice * const size =
            wxDynamicCast(
                dialog->FindWindow(wxID_FONT_SIZE), wxChoice);
        if ( !family || !style || !weight || !size )
            return;

        family->SetSelection(4);
        style->SetSelection(1);
        weight->SetSelection(2);
        if ( !size->SetStringSelection("37") )
            return;

        wxCommandEvent changed(wxEVT_CHOICE, wxID_FONT_FAMILY);
        changed.SetEventObject(family);
        fontConfigured = dialog->ProcessWindowEvent(changed);
    }

    wxDECLARE_NO_COPY_CLASS(PickerModalHook);
};

void ClickPicker(wxPickerBase *picker)
{
    wxControl * const button = picker->GetPickerCtrl();
    REQUIRE(button);

    wxCommandEvent click(wxEVT_BUTTON, button->GetId());
    click.SetEventObject(button);
    (void)button->ProcessWindowEvent(click);
}

void CheckTabRoundTrip(wxWindow *first, wxWindow *second)
{
    REQUIRE(first);
    REQUIRE(second);

    first->SetFocus();
    REQUIRE(WaitFor(
        "picker initial focus",
        [first]()
        {
            return wxWindow::FindFocus() == first;
        }));

    MSG forward = {};
    forward.hwnd = ::GetFocus();
    forward.message = WM_KEYDOWN;
    forward.wParam = VK_TAB;
    REQUIRE(forward.hwnd);
    CHECK(wxWinUI3ProcessTabNavigationWithModifiers(
        &forward, false, false, false));
    REQUIRE(WaitFor(
        "picker forward Tab focus",
        [second]()
        {
            return wxWindow::FindFocus() == second;
        }));

    MSG backward = {};
    backward.hwnd = ::GetFocus();
    backward.message = WM_KEYDOWN;
    backward.wParam = VK_TAB;
    REQUIRE(backward.hwnd);
    CHECK(wxWinUI3ProcessTabNavigationWithModifiers(
        &backward, true, false, false));
    REQUIRE(WaitFor(
        "picker backward Tab focus",
        [first]()
        {
            return wxWindow::FindFocus() == first;
        }));
}

struct FileDirEventRecord
{
    int count = 0;
    int id = wxID_NONE;
    wxObject *object = nullptr;
    wxString path;

    void Record(wxFileDirPickerEvent& event)
    {
        ++count;
        id = event.GetId();
        object = event.GetEventObject();
        path = event.GetPath();
    }
};

struct FontEventRecord
{
    int count = 0;
    int id = wxID_NONE;
    wxObject *object = nullptr;
    wxFont font;

    void Record(wxFontPickerEvent& event)
    {
        ++count;
        id = event.GetId();
        object = event.GetEventObject();
        font = event.GetFont();
    }
};

struct ColourEventRecord
{
    int count = 0;
    int id = wxID_NONE;
    wxObject *object = nullptr;
    wxColour colour;

    void Record(wxColourPickerEvent& event)
    {
        ++count;
        id = event.GetId();
        object = event.GetEventObject();
        colour = event.GetColour();
    }
};

struct ValidatorContext
{
    int calls = 0;
    bool result = true;
};

class PickerValidator final : public wxValidator
{
public:
    explicit PickerValidator(ValidatorContext *context = nullptr)
        : m_context(context)
    {
    }

    wxObject *Clone() const override
    {
        return new PickerValidator(*this);
    }

    bool Validate(wxWindow *) override
    {
        if ( !m_context )
            return true;

        ++m_context->calls;
        return m_context->result;
    }

    bool TransferToWindow() override
    {
        return true;
    }

    bool TransferFromWindow() override
    {
        return true;
    }

private:
    ValidatorContext *m_context;
};

#if wxUSE_XRC
class XrcUnloadGuard final
{
public:
    XrcUnloadGuard(wxXmlResource *resource, const wxString& name)
        : m_resource(resource),
          m_name(name)
    {
    }

    ~XrcUnloadGuard()
    {
        if ( m_resource )
            (void)m_resource->Unload(m_name);
    }

private:
    wxXmlResource *m_resource;
    wxString m_name;

    wxDECLARE_NO_COPY_CLASS(XrcUnloadGuard);
};
#endif // wxUSE_XRC

} // anonymous namespace

TEST_CASE("WinUIPickerContracts::FileAndDirectoryComposite",
          "[winui-picker-contract][winui-file-picker]"
          "[winui-dir-picker]")
{
    wxFrame owner(
        nullptr, wxID_ANY, "picker owner",
        wxPoint(-32000, -32000), wxSize(700, 420));
    wxPanel first(&owner, wxID_ANY, wxPoint(0, 0), wxSize(340, 400));
    wxPanel second(&owner, wxID_ANY, wxPoint(350, 0), wxSize(340, 400));
    owner.Show();
    wxYield();

    const int fileId = wxWindow::NewControlId();
    const wxString initialFile =
        "C:/picker/" + wxString(280, 'a') +
        wxString::FromUTF8("/départ_日本語.txt");
    const wxString acceptedFile =
        "C:/picker/" + wxString(300, 'b') +
        wxString::FromUTF8("/accepté_العربية.txt");

    ValidatorContext validatorContext;
    wxFilePickerCtrl file(
        &first, fileId, initialFile, "Choose a file",
        "Text files (*.txt)|*.txt",
        wxDefaultPosition, wxDefaultSize,
        wxFLP_USE_TEXTCTRL | wxFLP_OPEN,
        PickerValidator(&validatorContext));
    REQUIRE(file.HasTextCtrl());
    REQUIRE(file.GetTextCtrl());
    REQUIRE(file.GetPickerCtrl());
    CHECK(file.GetPath() == initialFile);
    CHECK(file.GetTextCtrl()->GetValue() == initialFile);
    CHECK(file.HasFlag(wxTAB_TRAVERSAL));
    CHECK(file.GetTextCtrl()->AcceptsFocusFromKeyboard());
    CHECK(file.GetPickerCtrl()->AcceptsFocusFromKeyboard());
    CheckTabRoundTrip(file.GetTextCtrl(), file.GetPickerCtrl());

    validatorContext.result = false;
    CHECK_FALSE(file.Validate());
    CHECK(validatorContext.calls == 1);
    validatorContext.result = true;
    CHECK(file.Validate());
    CHECK(validatorContext.calls == 2);

    FileDirEventRecord fileEventsFirst;
    FileDirEventRecord fileEventsSecond;
    first.Bind(
        wxEVT_FILEPICKER_CHANGED,
        [&fileEventsFirst](wxFileDirPickerEvent& event)
        {
            fileEventsFirst.Record(event);
        },
        fileId);
    second.Bind(
        wxEVT_FILEPICKER_CHANGED,
        [&fileEventsSecond](wxFileDirPickerEvent& event)
        {
            fileEventsSecond.Record(event);
        },
        fileId);

    PickerModalHook hook;
    hook.result = wxID_CANCEL;
    hook.path = acceptedFile;
    ClickPicker(&file);
    CHECK(hook.lastKind == PickerDialogKind::File);
    CHECK(file.GetPath() == initialFile);
    CHECK(fileEventsFirst.count == 0);

    hook.result = wxID_OK;
    ClickPicker(&file);
    CHECK(file.GetPath() == acceptedFile);
    CHECK(file.GetTextCtrl()->GetValue() == acceptedFile);
    CHECK(fileEventsFirst.count == 1);
    CHECK(fileEventsFirst.id == fileId);
    CHECK(fileEventsFirst.object == &file);
    CHECK(fileEventsFirst.path == acceptedFile);

    // Text mode follows the same public event identity as browse acceptance.
    const wxString typedFile =
        wxString::FromUTF8("C:/saisie/résumé_日本語.txt");
    const wxString canonicalTypedFile =
        wxFileName(typedFile).GetFullPath();
    file.GetTextCtrl()->SetValue(typedFile);
    CHECK(file.GetPath() == canonicalTypedFile);
    CHECK(fileEventsFirst.count == 2);
    CHECK(fileEventsFirst.id == fileId);
    CHECK(fileEventsFirst.object == &file);
    CHECK(fileEventsFirst.path == canonicalTypedFile);

    // Reparent while the hybrid dialog is active. The accepted event must use
    // the new public parent, not the dialog's original owner or the child ID.
    hook.path = acceptedFile;
    hook.windowToReparent = &file;
    hook.reparentTarget = &second;
    ClickPicker(&file);
    CHECK(hook.reparentSucceeded);
    CHECK(file.GetParent() == &second);
    CHECK(fileEventsFirst.count == 2);
    CHECK(fileEventsSecond.count == 1);
    CHECK(fileEventsSecond.object == &file);
    CHECK(fileEventsSecond.id == fileId);

    file.Disable();
    CHECK_FALSE(file.IsEnabled());
    CHECK_FALSE(file.GetTextCtrl()->IsEnabled());
    CHECK_FALSE(file.GetPickerCtrl()->IsEnabled());
    file.Enable();
    file.Hide();
    CHECK_FALSE(file.IsShown());
    CHECK_FALSE(file.GetTextCtrl()->IsShownOnScreen());
    CHECK_FALSE(file.GetPickerCtrl()->IsShownOnScreen());
    file.Show();

    wxFilePickerCtrl buttonOnly(
        &first, wxID_ANY, initialFile, "Button only",
        "All files (*.*)|*.*",
        wxDefaultPosition, wxDefaultSize, wxFLP_OPEN);
    CHECK_FALSE(buttonOnly.HasTextCtrl());
    CHECK(buttonOnly.GetPath() == initialFile);

    const int dirId = wxWindow::NewControlId();
    const wxString initialDirectory =
        wxString::FromUTF8("C:/répertoire/日本語");
    const wxString acceptedDirectory =
        "C:/directory/" + wxString(300, 'd') +
        wxString::FromUTF8("/العربية");
    wxDirPickerCtrl directory(
        &first, dirId, initialDirectory, "Choose a directory",
        wxDefaultPosition, wxDefaultSize,
        wxDIRP_USE_TEXTCTRL | wxDIRP_DIR_MUST_EXIST);
    FileDirEventRecord dirEvents;
    first.Bind(
        wxEVT_DIRPICKER_CHANGED,
        [&dirEvents](wxFileDirPickerEvent& event)
        {
            dirEvents.Record(event);
        },
        dirId);
    CHECK(directory.HasFlag(wxTAB_TRAVERSAL));
    CHECK(directory.GetTextCtrl()->AcceptsFocusFromKeyboard());
    CHECK(directory.GetPickerCtrl()->AcceptsFocusFromKeyboard());
    CheckTabRoundTrip(
        directory.GetTextCtrl(), directory.GetPickerCtrl());

    hook.result = wxID_CANCEL;
    hook.path = acceptedDirectory;
    ClickPicker(&directory);
    CHECK(hook.lastKind == PickerDialogKind::Directory);
    CHECK(directory.GetPath() == initialDirectory);
    CHECK(dirEvents.count == 0);

    hook.result = wxID_OK;
    ClickPicker(&directory);
    wxString canonicalAcceptedDirectory = acceptedDirectory;
    canonicalAcceptedDirectory.Replace("/", "\\");
    CHECK(directory.GetPath() == canonicalAcceptedDirectory);
    CHECK(directory.GetTextCtrl()->GetValue() ==
          canonicalAcceptedDirectory);
    CHECK(dirEvents.count == 1);
    CHECK(dirEvents.id == dirId);
    CHECK(dirEvents.object == &directory);
    CHECK(dirEvents.path == canonicalAcceptedDirectory);

    FileDirEventRecord dirEventsSecond;
    second.Bind(
        wxEVT_DIRPICKER_CHANGED,
        [&dirEventsSecond](wxFileDirPickerEvent& event)
        {
            dirEventsSecond.Record(event);
        },
        dirId);
    hook.windowToReparent = &directory;
    hook.reparentTarget = &second;
    ClickPicker(&directory);
    CHECK(directory.GetParent() == &second);
    CHECK(dirEvents.count == 1);
    CHECK(dirEventsSecond.count == 1);
    CHECK(dirEventsSecond.id == dirId);
    CHECK(dirEventsSecond.object == &directory);
    CHECK(dirEventsSecond.path == canonicalAcceptedDirectory);

    // Destroying the public composite from the modal boundary must not allow
    // the button handler to read the accepted result or emit a late event.
    const int doomedId = wxWindow::NewControlId();
    wxFilePickerCtrl * const doomed =
        new wxFilePickerCtrl(
            &first, doomedId, initialFile, "Destroy during dialog",
            "All files (*.*)|*.*",
            wxDefaultPosition, wxDefaultSize, wxFLP_OPEN);
    const wxWeakRef<wxFilePickerCtrl> weakDoomed(doomed);
    int doomedEvents = 0;
    first.Bind(
        wxEVT_FILEPICKER_CHANGED,
        [&doomedEvents](wxFileDirPickerEvent&)
        {
            ++doomedEvents;
        },
        doomedId);
    hook.result = wxID_OK;
    hook.path = acceptedFile;
    hook.windowToDelete = doomed;
    ClickPicker(doomed);
    CHECK_FALSE(weakDoomed);
    CHECK(doomedEvents == 0);

    const int doomedDirectoryId = wxWindow::NewControlId();
    wxDirPickerCtrl * const doomedDirectory =
        new wxDirPickerCtrl(
            &first, doomedDirectoryId, initialDirectory, "Destroy directory",
            wxDefaultPosition, wxDefaultSize, wxDIRP_USE_TEXTCTRL);
    const wxWeakRef<wxDirPickerCtrl> weakDoomedDirectory(doomedDirectory);
    int doomedDirectoryEvents = 0;
    first.Bind(
        wxEVT_DIRPICKER_CHANGED,
        [&doomedDirectoryEvents](wxFileDirPickerEvent&)
        {
            ++doomedDirectoryEvents;
        },
        doomedDirectoryId);
    hook.result = wxID_OK;
    hook.path = acceptedDirectory;
    hook.windowToDelete = doomedDirectory;
    ClickPicker(doomedDirectory);
    CHECK_FALSE(weakDoomedDirectory);
    CHECK(doomedDirectoryEvents == 0);

    // Native file/directory dialogs can retain their wx parent without being
    // owned children of it. Owner teardown during the modal hook must clear
    // that stale identity before their local dialog objects are destroyed.
    wxFrame * const doomedFileOwner =
        new wxFrame(
            nullptr, wxID_ANY, "file picker doomed owner",
            wxPoint(-32000, -32000), wxSize(420, 260));
    doomedFileOwner->ShowWithoutActivating();
    wxFilePickerCtrl * const ownerDoomedFile =
        new wxFilePickerCtrl(
            doomedFileOwner, wxID_ANY, initialFile, "Owner teardown file",
            "All files (*.*)|*.*",
            wxDefaultPosition, wxDefaultSize, wxFLP_OPEN);
    const wxWeakRef<wxWindow> weakDoomedFileOwner(doomedFileOwner);
    const wxWeakRef<wxFilePickerCtrl> weakOwnerDoomedFile(
        ownerDoomedFile);
    hook.result = wxID_OK;
    hook.path = acceptedFile;
    hook.windowToDelete = doomedFileOwner;
    ClickPicker(ownerDoomedFile);
    CHECK_FALSE(weakDoomedFileOwner);
    CHECK_FALSE(weakOwnerDoomedFile);

    wxFrame * const doomedDirOwner =
        new wxFrame(
            nullptr, wxID_ANY, "directory picker doomed owner",
            wxPoint(-32000, -32000), wxSize(420, 260));
    doomedDirOwner->ShowWithoutActivating();
    wxDirPickerCtrl * const ownerDoomedDirectory =
        new wxDirPickerCtrl(
            doomedDirOwner, wxID_ANY, initialDirectory,
            "Owner teardown directory",
            wxDefaultPosition, wxDefaultSize, wxDIRP_USE_TEXTCTRL);
    const wxWeakRef<wxWindow> weakDoomedDirOwner(doomedDirOwner);
    const wxWeakRef<wxDirPickerCtrl> weakOwnerDoomedDirectory(
        ownerDoomedDirectory);
    hook.result = wxID_OK;
    hook.path = acceptedDirectory;
    hook.windowToDelete = doomedDirOwner;
    ClickPicker(ownerDoomedDirectory);
    CHECK_FALSE(weakDoomedDirOwner);
    CHECK_FALSE(weakOwnerDoomedDirectory);

    // The accepted event is application code. It may destroy the TLW, so the
    // native dialog must already be gone before this callback is entered.
    const int acceptedFileDeleteId = wxWindow::NewControlId();
    wxFrame * const acceptedFileDeleteOwner =
        new wxFrame(
            nullptr, wxID_ANY, "file accepted-event owner",
            wxPoint(-32000, -32000), wxSize(420, 260));
    acceptedFileDeleteOwner->ShowWithoutActivating();
    wxFilePickerCtrl * const acceptedFileDeletePicker =
        new wxFilePickerCtrl(
            acceptedFileDeleteOwner, acceptedFileDeleteId, initialFile,
            "Accepted event deletes owner", "All files (*.*)|*.*",
            wxDefaultPosition, wxDefaultSize, wxFLP_OPEN);
    const wxWeakRef<wxWindow> weakAcceptedFileDeleteOwner(
        acceptedFileDeleteOwner);
    const wxWeakRef<wxFilePickerCtrl> weakAcceptedFileDeletePicker(
        acceptedFileDeletePicker);
    int acceptedFileDeleteEvents = 0;
    acceptedFileDeleteOwner->Bind(
        wxEVT_FILEPICKER_CHANGED,
        [acceptedFileDeleteOwner, &acceptedFileDeleteEvents](
            wxFileDirPickerEvent&)
        {
            ++acceptedFileDeleteEvents;
            delete acceptedFileDeleteOwner;
        },
        acceptedFileDeleteId);
    hook.result = wxID_OK;
    hook.path = acceptedFile;
    ClickPicker(acceptedFileDeletePicker);
    CHECK(acceptedFileDeleteEvents == 1);
    CHECK_FALSE(weakAcceptedFileDeleteOwner);
    CHECK_FALSE(weakAcceptedFileDeletePicker);

    const int acceptedDirDeleteId = wxWindow::NewControlId();
    wxFrame * const acceptedDirDeleteOwner =
        new wxFrame(
            nullptr, wxID_ANY, "directory accepted-event owner",
            wxPoint(-32000, -32000), wxSize(420, 260));
    acceptedDirDeleteOwner->ShowWithoutActivating();
    wxDirPickerCtrl * const acceptedDirDeletePicker =
        new wxDirPickerCtrl(
            acceptedDirDeleteOwner, acceptedDirDeleteId, initialDirectory,
            "Accepted event deletes owner",
            wxDefaultPosition, wxDefaultSize, wxDIRP_USE_TEXTCTRL);
    const wxWeakRef<wxWindow> weakAcceptedDirDeleteOwner(
        acceptedDirDeleteOwner);
    const wxWeakRef<wxDirPickerCtrl> weakAcceptedDirDeletePicker(
        acceptedDirDeletePicker);
    int acceptedDirDeleteEvents = 0;
    acceptedDirDeleteOwner->Bind(
        wxEVT_DIRPICKER_CHANGED,
        [acceptedDirDeleteOwner, &acceptedDirDeleteEvents](
            wxFileDirPickerEvent&)
        {
            ++acceptedDirDeleteEvents;
            delete acceptedDirDeleteOwner;
        },
        acceptedDirDeleteId);
    hook.result = wxID_OK;
    hook.path = acceptedDirectory;
    ClickPicker(acceptedDirDeletePicker);
    CHECK(acceptedDirDeleteEvents == 1);
    CHECK_FALSE(weakAcceptedDirDeleteOwner);
    CHECK_FALSE(weakAcceptedDirDeletePicker);

    wxFilePickerCtrl twoStageFile;
    REQUIRE(twoStageFile.Create(
        &first, wxID_ANY, initialFile, "Two stage",
        "All files (*.*)|*.*",
        wxDefaultPosition, wxDefaultSize,
        wxFLP_USE_TEXTCTRL | wxFLP_OPEN));
    CHECK(twoStageFile.GetPath() == initialFile);

    wxDirPickerCtrl twoStageDirectory;
    REQUIRE(twoStageDirectory.Create(
        &first, wxID_ANY, initialDirectory, "Two stage directory",
        wxDefaultPosition, wxDefaultSize,
        wxDIRP_USE_TEXTCTRL));
    CHECK(twoStageDirectory.GetPath() == initialDirectory);

    for ( int cycle = 0; cycle < 100; ++cycle )
    {
        wxFilePickerCtrl * const cycleFile =
            new wxFilePickerCtrl(
                &first, wxID_ANY, initialFile, "File cycle",
                "All files (*.*)|*.*",
                wxDefaultPosition, wxDefaultSize, wxFLP_OPEN);
        wxDirPickerCtrl * const cycleDirectory =
            new wxDirPickerCtrl(
                &first, wxID_ANY, initialDirectory, "Directory cycle",
                wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE);
        delete cycleDirectory;
        delete cycleFile;
    }
}

TEST_CASE("WinUIPickerContracts::FontComposite",
          "[winui-picker-contract][winui-font-picker]")
{
    wxFrame owner(
        nullptr, wxID_ANY, "font picker owner",
        wxPoint(-32000, -32000), wxSize(720, 420));
    wxPanel panel(&owner, wxID_ANY, wxDefaultPosition, owner.GetClientSize());
    owner.ShowWithoutActivating();

    const int pickerId = wxWindow::NewControlId();
    const wxFont initial(
        wxFontInfo(11)
            .Family(wxFONTFAMILY_ROMAN)
            .Style(wxFONTSTYLE_NORMAL)
            .Weight(wxFONTWEIGHT_NORMAL));
    REQUIRE(initial.IsOk());

    wxFontPickerCtrl picker(
        &panel, pickerId, initial,
        wxDefaultPosition, wxDefaultSize,
        wxFNTP_USE_TEXTCTRL |
            wxFNTP_FONTDESC_AS_LABEL |
            wxFNTP_USEFONT_FOR_LABEL);
    REQUIRE(picker.GetTextCtrl());
    REQUIRE(picker.GetPickerCtrl());
    CHECK(picker.GetSelectedFont() == initial);
    CHECK_FALSE(picker.GetTextCtrl()->GetValue().empty());

    wxButton trailing(
        &panel, wxID_ANY, "Trailing control",
        wxDefaultPosition, wxDefaultSize);
    wxBoxSizer * const pickerRow = new wxBoxSizer(wxHORIZONTAL);
    pickerRow->Add(&picker, 0, wxALIGN_CENTER_VERTICAL);
    pickerRow->Add(&trailing, 0, wxALIGN_CENTER_VERTICAL);
    panel.SetSizer(pickerRow);
    panel.Layout();

    FontEventRecord events;
    panel.Bind(
        wxEVT_FONTPICKER_CHANGED,
        [&events](wxFontPickerEvent& event)
        {
            events.Record(event);
        },
        pickerId);

    PickerModalHook hook;
    hook.configureFont = true;
    hook.result = wxID_CANCEL;
    ClickPicker(&picker);
    CHECK(hook.lastKind == PickerDialogKind::Font);
    CHECK(hook.fontConfigured);
    CHECK(picker.GetSelectedFont() == initial);
    CHECK(events.count == 0);

    hook.result = wxID_OK;
    ClickPicker(&picker);
    const wxFont accepted = picker.GetSelectedFont();
    REQUIRE(accepted.IsOk());
    CHECK(accepted.GetPointSize() == 37);
    CHECK(accepted.GetFamily() == wxFONTFAMILY_SWISS);
    CHECK(accepted.GetStyle() == wxFONTSTYLE_ITALIC);
    CHECK(accepted.GetWeight() == wxFONTWEIGHT_BOLD);
    CHECK(events.count == 1);
    CHECK(events.id == pickerId);
    CHECK(events.object == &picker);
    CHECK(events.font == accepted);
    CHECK_FALSE(picker.GetTextCtrl()->GetValue().empty());

    // A realized long/large description grows the composite monotonically.
    const wxSize beforeLongLabel = picker.GetSize();
    const wxSize beforeRowMinimum = pickerRow->GetMinSize();
    const int beforeTrailingX = trailing.GetPosition().x;
    const wxFont longFace(
        wxFontInfo(28)
            .FaceName("Microsoft JhengHei UI")
            .Weight(wxFONTWEIGHT_BOLD));
    REQUIRE(longFace.IsOk());
    picker.SetSelectedFont(longFace);
    const wxString buttonLabel = picker.GetPickerCtrl()->GetLabel();
    CHECK(buttonLabel.Contains(longFace.GetFaceName()));
    CHECK(picker.GetSize().x >= beforeLongLabel.x);
    CHECK(picker.GetSize().y >= beforeLongLabel.y);
    CHECK(picker.GetMinSize().x >= picker.GetPickerCtrl()->GetBestSize().x);
    CHECK(pickerRow->GetMinSize().x > beforeRowMinimum.x);
    CHECK(trailing.GetPosition().x > beforeTrailingX);
    CHECK(picker.GetRect().GetRight() <= trailing.GetRect().GetLeft());

    picker.Disable();
    CHECK_FALSE(picker.IsEnabled());
    CHECK_FALSE(picker.GetTextCtrl()->IsEnabled());
    CHECK_FALSE(picker.GetPickerCtrl()->IsEnabled());
    picker.Enable();

    const int doomedId = wxWindow::NewControlId();
    wxFontPickerCtrl * const doomed =
        new wxFontPickerCtrl(
            &panel, doomedId, initial,
            wxDefaultPosition, wxDefaultSize,
            wxFNTP_FONTDESC_AS_LABEL);
    const wxWeakRef<wxFontPickerCtrl> weakDoomed(doomed);
    int doomedEvents = 0;
    panel.Bind(
        wxEVT_FONTPICKER_CHANGED,
        [&doomedEvents](wxFontPickerEvent&)
        {
            ++doomedEvents;
        },
        doomedId);
    hook.result = wxID_OK;
    hook.windowToDelete = doomed;
    ClickPicker(doomed);
    CHECK_FALSE(weakDoomed);
    CHECK(doomedEvents == 0);

    // The dialog itself is parented to the resolved TLW. If that owner goes
    // away during the modal boundary, both the composite and the dialog are
    // retired before ShowModal() returns; the button handler must not perform
    // a second delete or publish the accepted value.
    wxFrame * const doomedOwner =
        new wxFrame(
            nullptr, wxID_ANY, "font picker doomed owner",
            wxPoint(-32000, -32000), wxSize(420, 260));
    doomedOwner->ShowWithoutActivating();
    wxFontPickerCtrl * const ownerDoomedPicker =
        new wxFontPickerCtrl(
            doomedOwner, wxID_ANY, initial,
            wxDefaultPosition, wxDefaultSize,
            wxFNTP_FONTDESC_AS_LABEL);
    const wxWeakRef<wxWindow> weakDoomedOwner(doomedOwner);
    const wxWeakRef<wxFontPickerCtrl> weakOwnerDoomedPicker(
        ownerDoomedPicker);
    hook.result = wxID_OK;
    hook.windowToDelete = doomedOwner;
    ClickPicker(ownerDoomedPicker);
    CHECK_FALSE(weakDoomedOwner);
    CHECK_FALSE(weakOwnerDoomedPicker);

    const int acceptedDeleteId = wxWindow::NewControlId();
    wxFrame * const acceptedDeleteOwner =
        new wxFrame(
            nullptr, wxID_ANY, "font accepted-event owner",
            wxPoint(-32000, -32000), wxSize(420, 260));
    acceptedDeleteOwner->ShowWithoutActivating();
    wxFontPickerCtrl * const acceptedDeletePicker =
        new wxFontPickerCtrl(
            acceptedDeleteOwner, acceptedDeleteId, initial,
            wxDefaultPosition, wxDefaultSize,
            wxFNTP_FONTDESC_AS_LABEL);
    const wxWeakRef<wxWindow> weakAcceptedDeleteOwner(acceptedDeleteOwner);
    const wxWeakRef<wxFontPickerCtrl> weakAcceptedDeletePicker(
        acceptedDeletePicker);
    int acceptedDeleteEvents = 0;
    acceptedDeleteOwner->Bind(
        wxEVT_FONTPICKER_CHANGED,
        [acceptedDeleteOwner, &acceptedDeleteEvents](wxFontPickerEvent&)
        {
            ++acceptedDeleteEvents;
            delete acceptedDeleteOwner;
        },
        acceptedDeleteId);
    hook.result = wxID_OK;
    ClickPicker(acceptedDeletePicker);
    CHECK(acceptedDeleteEvents == 1);
    CHECK_FALSE(weakAcceptedDeleteOwner);
    CHECK_FALSE(weakAcceptedDeletePicker);

    wxFontPickerCtrl twoStage;
    REQUIRE(twoStage.Create(
        &panel, wxID_ANY, initial,
        wxDefaultPosition, wxDefaultSize,
        wxFNTP_USE_TEXTCTRL | wxFNTP_FONTDESC_AS_LABEL));
    CHECK(twoStage.GetSelectedFont() == initial);

    for ( int cycle = 0; cycle < 100; ++cycle )
    {
        wxFontPickerCtrl * const cyclePicker =
            new wxFontPickerCtrl(
                &panel, wxID_ANY, initial,
                wxDefaultPosition, wxDefaultSize,
                wxFNTP_FONTDESC_AS_LABEL);
        delete cyclePicker;
    }
}

TEST_CASE("WinUIPickerContracts::ColourPairAndComposite",
          "[winui-picker-contract][winui-colour-picker]")
{
    wxFrame owner(
        nullptr, wxID_ANY, "colour picker owner",
        wxPoint(-32000, -32000), wxSize(720, 420));
    wxPanel first(&owner, wxID_ANY, wxPoint(0, 0), wxSize(350, 400));
    wxPanel second(&owner, wxID_ANY, wxPoint(360, 0), wxSize(350, 400));
    owner.ShowWithoutActivating();

    const int pickerId = wxWindow::NewControlId();
    const wxColour initial(12, 34, 56, 78);
    wxColourPickerCtrl picker(
        &first, pickerId, initial,
        wxDefaultPosition, wxDefaultSize,
        wxCLRP_USE_TEXTCTRL | wxCLRP_SHOW_LABEL | wxCLRP_SHOW_ALPHA);
    REQUIRE(picker.GetTextCtrl());
    wxWinUIColourButton * const button =
        wxDynamicCast(picker.GetPickerCtrl(), wxWinUIColourButton);
    REQUIRE(button);
    CHECK(picker.GetColour() == initial);

    wxColour peerColour;
    bool alphaEnabled = false;
    wxString peerLabel;
    REQUIRE(button->WinUIGetPeerStateForTesting(
        &peerColour, &alphaEnabled, &peerLabel));
    CHECK(peerColour == initial);
    CHECK(alphaEnabled);
    CHECK(peerLabel == initial.GetAsString(wxC2S_HTML_SYNTAX));

    ColourEventRecord current;
    ColourEventRecord changed;
    ColourEventRecord cancelled;
    first.Bind(
        wxEVT_COLOURPICKER_CURRENT_CHANGED,
        [&current](wxColourPickerEvent& event)
        {
            current.Record(event);
        },
        pickerId);
    first.Bind(
        wxEVT_COLOURPICKER_CHANGED,
        [&changed](wxColourPickerEvent& event)
        {
            changed.Record(event);
        },
        pickerId);
    first.Bind(
        wxEVT_COLOURPICKER_DIALOG_CANCELLED,
        [&cancelled](wxColourPickerEvent& event)
        {
            cancelled.Record(event);
        },
        pickerId);

    const wxColour liveColour(90, 80, 70, 60);
    REQUIRE(button->WinUISetPeerColourForTesting(liveColour));
    REQUIRE(WaitFor(
        "colour picker current event",
        [&current]()
        {
            return current.count == 1;
        }));
    CHECK(picker.GetColour() == liveColour);
    CHECK(current.id == pickerId);
    CHECK(current.object == &picker);
    CHECK(current.colour == liveColour);
    REQUIRE(button->WinUIGetPeerStateForTesting(
        &peerColour, &alphaEnabled, &peerLabel));
    CHECK(peerColour == liveColour);
    CHECK(peerLabel == liveColour.GetAsString(wxC2S_HTML_SYNTAX));

    REQUIRE(button->WinUIDeliverClosedForTesting());
    CHECK(changed.count == 1);
    CHECK(changed.id == pickerId);
    CHECK(changed.object == &picker);
    CHECK(changed.colour == liveColour);

    // Exercise the third child event variant explicitly: every backend event
    // must cross the same public ID/object boundary.
    wxColourPickerEvent cancelEvent(
        button, button->GetId(), liveColour,
        wxEVT_COLOURPICKER_DIALOG_CANCELLED);
    (void)button->ProcessWindowEvent(cancelEvent);
    CHECK(cancelled.count == 1);
    CHECK(cancelled.id == pickerId);
    CHECK(cancelled.object == &picker);
    CHECK(cancelled.colour == liveColour);

    const wxColour programmatic(1, 2, 3, 4);
    picker.SetColour(programmatic);
    CHECK(picker.GetColour() == programmatic);
    CHECK(current.count == 1);
    REQUIRE(button->WinUIGetPeerStateForTesting(
        &peerColour, &alphaEnabled, &peerLabel));
    CHECK(peerColour == programmatic);
    CHECK(peerLabel == programmatic.GetAsString(wxC2S_HTML_SYNTAX));

    picker.SetToolTip("public picker tooltip");
    CHECK(button->GetToolTipText() == "public picker tooltip");
    CHECK(picker.GetTextCtrl()->GetToolTipText() ==
          "public picker tooltip");
    picker.Disable();
    CHECK_FALSE(picker.IsEnabled());
    CHECK_FALSE(button->IsEnabled());
    CHECK_FALSE(picker.GetTextCtrl()->IsEnabled());
    picker.Enable();
    picker.Hide();
    CHECK_FALSE(button->IsShownOnScreen());
    CHECK_FALSE(picker.GetTextCtrl()->IsShownOnScreen());
    picker.Show();

    ColourEventRecord reparented;
    second.Bind(
        wxEVT_COLOURPICKER_CURRENT_CHANGED,
        [&reparented](wxColourPickerEvent& event)
        {
            reparented.Record(event);
        },
        pickerId);
    REQUIRE(picker.Reparent(&second));
    const wxColour afterReparent(44, 55, 66, 77);
    REQUIRE(button->WinUISetPeerColourForTesting(afterReparent));
    REQUIRE(WaitFor(
        "reparented colour picker event",
        [&reparented]()
        {
            return reparented.count == 1;
        }));
    CHECK(current.count == 1);
    CHECK(reparented.id == pickerId);
    CHECK(reparented.object == &picker);
    CHECK(reparented.colour == afterReparent);

    const unsigned baselineCallbacks =
        wxWinUIColourButton::
            WinUIGetLiveCallbackStateCountForTesting();
    const int doomedId = wxWindow::NewControlId();
    wxColourPickerCtrl *doomed =
        new wxColourPickerCtrl(
            &second, doomedId, *wxBLACK,
            wxDefaultPosition, wxDefaultSize,
            wxCLRP_SHOW_ALPHA);
    wxWinUIColourButton * const doomedButton =
        wxDynamicCast(doomed->GetPickerCtrl(), wxWinUIColourButton);
    REQUIRE(doomedButton);
    const wxWeakRef<wxColourPickerCtrl> weakDoomed(doomed);
    second.Bind(
        wxEVT_COLOURPICKER_CURRENT_CHANGED,
        [&doomed](wxColourPickerEvent& event)
        {
            wxColourPickerCtrl * const victim = doomed;
            doomed = nullptr;
            delete victim;
            event.Skip(false);
        },
        doomedId);
    REQUIRE(doomedButton->WinUISetPeerColourForTesting(
        wxColour(100, 110, 120, 130)));
    CHECK_FALSE(weakDoomed);
    REQUIRE(WaitFor(
        "colour callback state teardown",
        [baselineCallbacks]()
        {
            return wxWinUIColourButton::
                       WinUIGetLiveCallbackStateCountForTesting() ==
                   baselineCallbacks;
        }));

    wxColourPickerCtrl twoStage;
    REQUIRE(twoStage.Create(
        &first, wxID_ANY, initial,
        wxDefaultPosition, wxDefaultSize,
        wxCLRP_SHOW_ALPHA | wxCLRP_SHOW_LABEL));
    CHECK(twoStage.GetColour() == initial);

    const unsigned cycleBaseline =
        wxWinUIColourButton::
            WinUIGetLiveCallbackStateCountForTesting();
    for ( int cycle = 0; cycle < 100; ++cycle )
    {
        wxColourPickerCtrl * const item =
            new wxColourPickerCtrl(
                &first, wxID_ANY,
                wxColour(cycle, 255 - cycle, cycle / 2, 200),
                wxDefaultPosition, wxDefaultSize,
                wxCLRP_SHOW_ALPHA);
        delete item;
        if ( cycle % 10 == 0 )
            wxYield();
    }
    REQUIRE(WaitFor(
        "colour picker hundred-cycle teardown",
        [cycleBaseline]()
        {
            return wxWinUIColourButton::
                       WinUIGetLiveCallbackStateCountForTesting() ==
                   cycleBaseline;
        }));
}

#if wxUSE_XRC
TEST_CASE("WinUIPickerContracts::XRC",
          "[winui-picker-contract][winui-picker-xrc][xrc]")
{
    static const char xrcName[] = "winui-picker-contracts.xrc";
    static const char xrcText[] =
        "<?xml version=\"1.0\" ?>"
        "<resource>"
        "  <object class=\"wxDialog\" name=\"pickerDialog\">"
        "    <title>Picker XRC contract</title>"
        "    <object class=\"wxBoxSizer\">"
        "      <orient>wxVERTICAL</orient>"
        "      <object class=\"sizeritem\">"
        "        <object class=\"wxFilePickerCtrl\" name=\"filePicker\">"
        "          <value>C:/xrc/file.txt</value>"
        "          <message>Choose XRC file</message>"
        "          <wildcard>Text (*.txt)|*.txt</wildcard>"
        "          <style>wxFLP_USE_TEXTCTRL|wxFLP_OPEN</style>"
        "        </object>"
        "      </object>"
        "      <object class=\"sizeritem\">"
        "        <object class=\"wxDirPickerCtrl\" name=\"dirPicker\">"
        "          <value>C:/xrc/directory</value>"
        "          <message>Choose XRC directory</message>"
        "          <style>wxDIRP_USE_TEXTCTRL|wxDIRP_DIR_MUST_EXIST</style>"
        "        </object>"
        "      </object>"
        "      <object class=\"sizeritem\">"
        "        <object class=\"wxFontPickerCtrl\" name=\"fontPicker\">"
        "          <style>wxFNTP_USE_TEXTCTRL|wxFNTP_FONTDESC_AS_LABEL</style>"
        "          <value>"
        "            <size>20</size>"
        "            <sysfont>wxSYS_DEFAULT_GUI_FONT</sysfont>"
        "          </value>"
        "        </object>"
        "      </object>"
        "      <object class=\"sizeritem\">"
        "        <object class=\"wxColourPickerCtrl\" name=\"colourPicker\">"
        "          <value>#123456</value>"
        "          <style>wxCLRP_USE_TEXTCTRL|wxCLRP_SHOW_LABEL|"
        "wxCLRP_SHOW_ALPHA</style>"
        "        </object>"
        "      </object>"
        "    </object>"
        "  </object>"
        "</resource>";

    wxXmlResource * const xrc = wxXmlResource::Get();
    xrc->InitAllHandlers();
    wxStringInputStream stream(wxString::FromAscii(xrcText));
    std::unique_ptr<wxXmlDocument> document(new wxXmlDocument(stream));
    REQUIRE(document->IsOk());
    REQUIRE(xrc->LoadDocument(document.release(), xrcName));
    XrcUnloadGuard unloadGuard(xrc, xrcName);

    wxDialog dialog;
    REQUIRE(xrc->LoadDialog(&dialog, nullptr, "pickerDialog"));
    wxFilePickerCtrl * const file =
        XRCCTRL(dialog, "filePicker", wxFilePickerCtrl);
    wxDirPickerCtrl * const directory =
        XRCCTRL(dialog, "dirPicker", wxDirPickerCtrl);
    wxFontPickerCtrl * const font =
        XRCCTRL(dialog, "fontPicker", wxFontPickerCtrl);
    wxColourPickerCtrl * const colour =
        XRCCTRL(dialog, "colourPicker", wxColourPickerCtrl);
    REQUIRE(file);
    REQUIRE(directory);
    REQUIRE(font);
    REQUIRE(colour);

    CHECK(file->GetPath() == "C:/xrc/file.txt");
    CHECK(file->HasTextCtrl());
    CHECK(file->HasFlag(wxFLP_OPEN));
    CHECK(directory->GetPath() == "C:/xrc/directory");
    CHECK(directory->HasTextCtrl());
    CHECK(directory->HasFlag(wxDIRP_DIR_MUST_EXIST));
    CHECK(font->HasTextCtrl());
    CHECK(font->HasFlag(wxFNTP_FONTDESC_AS_LABEL));
    CHECK(font->GetSelectedFont().GetPointSize() == 20);
    CHECK(colour->GetColour() == wxColour("#123456"));
    CHECK(colour->HasTextCtrl());
    CHECK(colour->HasFlag(wxCLRP_SHOW_LABEL));
    CHECK(colour->HasFlag(wxCLRP_SHOW_ALPHA));

    wxWinUIColourButton * const colourButton =
        wxDynamicCast(colour->GetPickerCtrl(), wxWinUIColourButton);
    REQUIRE(colourButton);
    bool alphaEnabled = false;
    REQUIRE(colourButton->WinUIGetPeerStateForTesting(
        nullptr, &alphaEnabled, nullptr));
    CHECK(alphaEnabled);
}
#endif // wxUSE_XRC

#endif // __WXWINUI__ && wxUSE_WINUI3 && all picker controls
