/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/filedlg.cpp
// Purpose:     wxFileDialog
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_FILEDLG

#include "wx/filedlg.h"

#ifndef WX_PRECOMP
    #include "wx/msw/wrapcdlg.h"
    #include "wx/msw/missing.h"
    #include "wx/utils.h"
    #include "wx/msgdlg.h"
    #include "wx/filefn.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/app.h"
    #include "wx/math.h"
#endif

#include <stdlib.h>
#include <string.h>

#include "wx/dynlib.h"
#include "wx/filename.h"
#include "wx/scopeguard.h"
#include "wx/tokenzr.h"
#include "wx/modalhook.h"

#include "wx/msw/private/dpiaware.h"
#include "wx/msw/private/filedialog.h"

// Note: this must be done after including the header above, as this is where
// wxUSE_IFILEOPENDIALOG is defined.
#if wxUSE_IFILEOPENDIALOG
    #include "wx/filedlgcustomize.h"
    #include "wx/private/filedlgcustomize.h"

    #include "wx/button.h"
    #include "wx/checkbox.h"
    #include "wx/choice.h"
    #include "wx/radiobut.h"
    #include "wx/stattext.h"
    #include "wx/textctrl.h"

    #include "wx/msw/wrapshl.h"

    #include "wx/msw/private/cotaskmemptr.h"
#endif // wxUSE_IFILEOPENDIALOG

#include <memory>

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

# define wxMAXPATH   65534

# define wxMAXFILE   1024

# define wxMAXEXT    5

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

// standard dialog size for the old Windows systems where the dialog wasn't
// resizable
static wxRect gs_rectDialog(0, 0, 428, 266);

// ============================================================================
// implementation
// ============================================================================

wxIMPLEMENT_CLASS(wxFileDialog, wxFileDialogBase);

// ----------------------------------------------------------------------------

namespace
{

typedef BOOL (WINAPI *GetProcessUserModeExceptionPolicy_t)(LPDWORD);
typedef BOOL (WINAPI *SetProcessUserModeExceptionPolicy_t)(DWORD);

GetProcessUserModeExceptionPolicy_t gs_pfnGetProcessUserModeExceptionPolicy =
    (GetProcessUserModeExceptionPolicy_t) -1;

SetProcessUserModeExceptionPolicy_t gs_pfnSetProcessUserModeExceptionPolicy =
    (SetProcessUserModeExceptionPolicy_t) -1;

DWORD gs_oldExceptionPolicyFlags = 0;

bool gs_changedPolicy = false;

/*
Since Windows 7 by default (callback) exceptions aren't swallowed anymore
with native x64 applications. Exceptions can occur in a file dialog when
using the hook procedure in combination with third-party utilities.
Since Windows 7 SP1 the swallowing of exceptions can be enabled again
by using SetProcessUserModeExceptionPolicy.
*/
void ChangeExceptionPolicy()
{
    gs_changedPolicy = false;

    wxLoadedDLL dllKernel32(wxT("kernel32.dll"));

    if ( gs_pfnGetProcessUserModeExceptionPolicy
        == (GetProcessUserModeExceptionPolicy_t) -1)
    {
        wxDL_INIT_FUNC(gs_pfn, GetProcessUserModeExceptionPolicy, dllKernel32);
        wxDL_INIT_FUNC(gs_pfn, SetProcessUserModeExceptionPolicy, dllKernel32);
    }

    if ( !gs_pfnGetProcessUserModeExceptionPolicy
        || !gs_pfnSetProcessUserModeExceptionPolicy
        || !gs_pfnGetProcessUserModeExceptionPolicy(&gs_oldExceptionPolicyFlags) )
    {
        return;
    }

    if ( gs_pfnSetProcessUserModeExceptionPolicy(gs_oldExceptionPolicyFlags
        | 0x1 /* PROCESS_CALLBACK_FILTER_ENABLED */ ) )
    {
        gs_changedPolicy = true;
    }
}

void RestoreExceptionPolicy()
{
    if (gs_changedPolicy)
    {
        gs_changedPolicy = false;
        (void) gs_pfnSetProcessUserModeExceptionPolicy(gs_oldExceptionPolicyFlags);
    }
}

#if wxUSE_IFILEOPENDIALOG

// ----------------------------------------------------------------------------
// Various IFileDialog-related helpers: they're only used here for now, but if
// they're ever needed in wxDirDialog too, we should put move them to
// wx/msw/private/filedialog.h
// ----------------------------------------------------------------------------

// Register the given event handler with the dialog during its life-time.
class FileDialogEventsRegistrar
{
public:
    // The file dialog scope must be greater than that of this object.
    FileDialogEventsRegistrar(wxMSWImpl::wxIFileDialog& fileDialog,
                              IFileDialogEvents& eventsHandler)
        : m_fileDialog(fileDialog)
    {
        HRESULT hr = m_fileDialog->Advise(&eventsHandler, &m_cookie);
        if ( FAILED(hr) )
            wxLogApiError(wxS("IFileDialog::Advise"), hr);
    }

    ~FileDialogEventsRegistrar()
    {
        HRESULT hr = m_fileDialog->Unadvise(m_cookie);
        if ( FAILED(hr) )
            wxLogApiError(wxS("IFileDialog::Unadvise"), hr);
    }

private:
    wxMSWImpl::wxIFileDialog& m_fileDialog;
    DWORD m_cookie;

    wxDECLARE_NO_COPY_CLASS(FileDialogEventsRegistrar);
};

// Return 1-based index of the currently selected file type.
UINT FileDialogGetFileTypeIndex(IFileDialog* fileDialog)
{
    UINT nFilterIndex;
    HRESULT hr = fileDialog->GetFileTypeIndex(&nFilterIndex);
    if ( FAILED(hr) )
    {
        wxLogApiError(wxS("IFileDialog::GetFileTypeIndex"), hr);

        nFilterIndex = 0;
    }

    return nFilterIndex;
}

// ----------------------------------------------------------------------------
// IFileDialogCustomize-related stuff: all classes use FDC suffix
// ----------------------------------------------------------------------------

// Base class class used only to avoid template bloat: this contains all the
// type-independent parts of wxFileDialogImplFDC.
class wxFileDialogImplFDCBase
{
protected:
    wxFileDialogImplFDCBase(IFileDialogCustomize* fdc, DWORD id)
        : m_fdc(fdc),
          m_id(id)
    {
    }

    void DoUpdateState(CDCONTROLSTATEF stateBit, bool on)
    {
        CDCONTROLSTATEF state = CDCS_INACTIVE;
        HRESULT hr = m_fdc->GetControlState(m_id, &state);
        if ( FAILED(hr) )
            wxLogApiError(wxS("IFileDialogCustomize::GetControlState"), hr);

        if ( on )
            state |= stateBit;
        else
            state &= ~stateBit;

        hr = m_fdc->SetControlState(m_id, state);
        if ( FAILED(hr) )
            wxLogApiError(wxS("IFileDialogCustomize::SetControlState"), hr);
    }

    IFileDialogCustomize* const m_fdc;
    const DWORD m_id;
};

// Template base class for the implementation classes below inheriting from the
// specified Impl subclass.
template <typename T>
class wxFileDialogImplFDC
    : public T,
      protected wxFileDialogImplFDCBase
{
public:
    wxFileDialogImplFDC(IFileDialogCustomize* fdc, DWORD id)
        : wxFileDialogImplFDCBase(fdc, id)
    {
    }

    virtual void Show(bool show) override
    {
        DoUpdateState(CDCS_VISIBLE, show);
    }

    virtual void Enable(bool enable) override
    {
        DoUpdateState(CDCS_ENABLED, enable);
    }
};

class wxFileDialogCustomControlImplFDC
    : public wxFileDialogImplFDC<wxFileDialogCustomControlImpl>
{
public:
    // All custom controls are identified by their ID in this implementation.
    wxFileDialogCustomControlImplFDC(IFileDialogCustomize* fdc, DWORD id)
        : wxFileDialogImplFDC<wxFileDialogCustomControlImpl>(fdc, id)
    {
    }

    wxDECLARE_NO_COPY_CLASS(wxFileDialogCustomControlImplFDC);
};

class wxFileDialogButtonImplFDC
    : public wxFileDialogImplFDC<wxFileDialogButtonImpl>
{
public:
    wxFileDialogButtonImplFDC(IFileDialogCustomize* fdc, DWORD id)
        : wxFileDialogImplFDC<wxFileDialogButtonImpl>(fdc, id)
    {
    }

    virtual bool DoBind(wxEvtHandler* WXUNUSED(handler)) override
    {
        // We don't need to do anything special to get the events here.
        return true;
    }
};

class wxFileDialogCheckBoxImplFDC
    : public wxFileDialogImplFDC<wxFileDialogCheckBoxImpl>
{
public:
    wxFileDialogCheckBoxImplFDC(IFileDialogCustomize* fdc, DWORD id)
        : wxFileDialogImplFDC<wxFileDialogCheckBoxImpl>(fdc, id)
    {
    }

    virtual bool GetValue() override
    {
        BOOL checked = FALSE;
        HRESULT hr = m_fdc->GetCheckButtonState(m_id, &checked);
        if ( FAILED(hr) )
            wxLogApiError(wxS("IFileDialogCustomize::GetCheckButtonState"), hr);

        return checked != FALSE;
    }

    virtual void SetValue(bool value) override
    {
        HRESULT hr = m_fdc->SetCheckButtonState(m_id, value ? TRUE : FALSE);
        if ( FAILED(hr) )
            wxLogApiError(wxS("IFileDialogCustomize::SetCheckButtonState"), hr);
    }

    virtual bool DoBind(wxEvtHandler* WXUNUSED(handler)) override
    {
        // We don't need to do anything special to get the events here.
        return true;
    }
};

class wxFileDialogRadioButtonImplFDC
    : public wxFileDialogImplFDC<wxFileDialogRadioButtonImpl>
{
public:
    wxFileDialogRadioButtonImplFDC(IFileDialogCustomize* fdc, DWORD id, DWORD item)
        : wxFileDialogImplFDC<wxFileDialogRadioButtonImpl>(fdc, id),
          m_item(item)
    {
    }

    virtual bool GetValue() override
    {
        DWORD selected = 0;
        HRESULT hr = m_fdc->GetSelectedControlItem(m_id, &selected);
        if ( FAILED(hr) )
            wxLogApiError(wxS("IFileDialogCustomize::GetSelectedControlItem"), hr);

        return selected == m_item;
    }

    virtual void SetValue(bool value) override
    {
        // We can't implement it using the available API and this shouldn't be
        // ever needed anyhow.
        wxCHECK_RET( value, wxS("clearing radio buttons not supported") );

        HRESULT hr = m_fdc->SetSelectedControlItem(m_id, m_item);
        if ( FAILED(hr) )
            wxLogApiError(wxS("IFileDialogCustomize::SetSelectedControlItem"), hr);
    }

    virtual bool DoBind(wxEvtHandler* WXUNUSED(handler)) override
    {
        // We don't need to do anything special to get the events here.
        return true;
    }

private:
    const DWORD m_item;
};

class wxFileDialogChoiceImplFDC
    : public wxFileDialogImplFDC<wxFileDialogChoiceImpl>
{
public:
    wxFileDialogChoiceImplFDC(IFileDialogCustomize* fdc, DWORD id, DWORD item)
        : wxFileDialogImplFDC<wxFileDialogChoiceImpl>(fdc, id),
          m_firstItem(item)
    {
    }

    virtual int GetSelection() override
    {
        DWORD selected = 0;
        HRESULT hr = m_fdc->GetSelectedControlItem(m_id, &selected);
        if ( hr == E_FAIL )
        {
            // This seems to be returned if there is no selection.
            return -1;
        }

        if ( FAILED(hr) )
            wxLogApiError(wxS("IFileDialogCustomize::GetSelectedControlItem"), hr);

        // See m_firstItem comment for the explanation of subtraction order.
        return m_firstItem - selected;
    }

    virtual void SetSelection(int n) override
    {
        // As above, see m_firstItem comment.
        HRESULT hr = m_fdc->SetSelectedControlItem(m_id, m_firstItem - n);
        if ( FAILED(hr) )
            wxLogApiError(wxS("IFileDialogCustomize::SetSelectedControlItem"), hr);
    }

    virtual bool DoBind(wxEvtHandler* WXUNUSED(handler)) override
    {
        // We don't need to do anything special to get the events here.
        return true;
    }

private:
    // The ID of the first item of the combobox. The subsequent items are
    // consecutive numbers _smaller_ than this one, because auxiliary IDs are
    // assigned in decreasing order by decrementing them.
    const DWORD m_firstItem;
};

class wxFileDialogTextCtrlImplFDC
    : public wxFileDialogImplFDC<wxFileDialogTextCtrlImpl>
{
public:
    wxFileDialogTextCtrlImplFDC(IFileDialogCustomize* fdc, DWORD id)
        : wxFileDialogImplFDC<wxFileDialogTextCtrlImpl>(fdc, id)
    {
    }

    virtual wxString GetValue() override
    {
        wxCoTaskMemPtr<WCHAR> value;
        HRESULT hr = m_fdc->GetEditBoxText(m_id, &value);
        if ( FAILED(hr) )
            wxLogApiError(wxS("IFileDialogCustomize::GetEditBoxText"), hr);

        return wxString(value);
    }

    virtual void SetValue(const wxString& value) override
    {
        HRESULT hr = m_fdc->SetEditBoxText(m_id, value.wc_str());
        if ( FAILED(hr) )
            wxLogApiError(wxS("IFileDialogCustomize::SetEditBoxText"), hr);
    }
};

class wxFileDialogStaticTextImplFDC
    : public wxFileDialogImplFDC<wxFileDialogStaticTextImpl>
{
public:
    wxFileDialogStaticTextImplFDC(IFileDialogCustomize* fdc, DWORD id)
        : wxFileDialogImplFDC<wxFileDialogStaticTextImpl>(fdc, id)
    {
    }

    virtual void SetLabelText(const wxString& text) override
    {
        // Prevent ampersands from being misinterpreted as mnemonics.
        const wxString& label = wxControl::EscapeMnemonics(text);

        HRESULT hr = m_fdc->SetControlLabel(m_id, label.wc_str());
        if ( FAILED(hr) )
            wxLogApiError(wxS("IFileDialogCustomize::SetControlLabel"), hr);
    }
};

// Implementation of wxFileDialogCustomize based on IFileDialogCustomize: to
// simplify things, this class is its own implementation pointer too.
class wxFileDialogCustomizeFDC : public wxFileDialogCustomize,
                                 private wxFileDialogCustomizeImpl
{
public:
    // For convenience, this class has a default ctor, but it can't be really
    // used before Initialize() is called.
    wxFileDialogCustomizeFDC()
        : wxFileDialogCustomize(this)
    {
        m_lastId =
        m_lastAuxId =
        m_radioListId = 0;
    }

    bool Initialize(IFileDialog* fileDialog)
    {
        HRESULT hr = fileDialog->QueryInterface
                                 (
                                  wxIID_PPV_ARGS(IFileDialogCustomize, &m_fdc)
                                 );
        if ( FAILED(hr) )
        {
            wxLogApiError(wxS("IFileDialog::QI(IFileDialogCustomize)"), hr);

            return false;
        }

        return true;
    }

    wxFileDialogCustomControl* FindControl(DWORD id) const
    {
        // Currently there is 1-to-1 correspondence between IDs and the
        // controls we create, except that we start assigning IDs with 1.
        if ( id < 1 || id > m_controls.size() )
            return nullptr;

        return m_controls[id - 1];
    }


    // Implement wxFileDialogCustomizeImpl pure virtual methods.
    wxFileDialogButtonImpl* AddButton(const wxString& label) override
    {
        m_radioListId = 0;

        HRESULT hr = m_fdc->AddPushButton(++m_lastId, label.wc_str());
        if ( FAILED(hr) )
        {
            wxLogApiError(wxS("IFileDialogCustomize::AddPushButton"), hr);
            return nullptr;
        }

        return new wxFileDialogButtonImplFDC(m_fdc, m_lastId);
    }

    wxFileDialogCheckBoxImpl* AddCheckBox(const wxString& label) override
    {
        m_radioListId = 0;

        HRESULT hr = m_fdc->AddCheckButton(++m_lastId, label.wc_str(), FALSE);
        if ( FAILED(hr) )
        {
            wxLogApiError(wxS("IFileDialogCustomize::AddCheckButton"), hr);
            return nullptr;
        }

        return new wxFileDialogCheckBoxImplFDC(m_fdc, m_lastId);
    }

    wxFileDialogRadioButtonImpl* AddRadioButton(const wxString& label) override
    {
        HRESULT hr;

        bool firstButton = false;
        if ( !m_radioListId )
        {
            hr = m_fdc->AddRadioButtonList(--m_lastAuxId);
            if ( FAILED(hr) )
            {
                wxLogApiError(wxS("IFileDialogCustomize::AddRadioButtonList"), hr);
                return nullptr;
            }

            m_radioListId = m_lastAuxId;
            firstButton = true;
        }

        hr = m_fdc->AddControlItem(m_radioListId, ++m_lastId, label.wc_str());
        if ( FAILED(hr) )
        {
            wxLogApiError(wxS("IFileDialogCustomize::AddControlItem"), hr);
            return nullptr;
        }

        wxFileDialogRadioButtonImplFDC* const
            impl = new wxFileDialogRadioButtonImplFDC(m_fdc, m_radioListId, m_lastId);

        // Select the first button of a new radio group.
        if ( firstButton )
            impl->SetValue(true);

        return impl;
    }

    wxFileDialogChoiceImpl* AddChoice(size_t n, const wxString* strings) override
    {
        HRESULT hr = m_fdc->AddComboBox(++m_lastId);
        if ( FAILED(hr) )
        {
            wxLogApiError(wxS("IFileDialogCustomize::AddComboBox"), hr);
            return nullptr;
        }

        // We pass the ID of the first control that will be added to the
        // combobox as the ctor argument.
        std::unique_ptr<wxFileDialogChoiceImplFDC>
            impl(new wxFileDialogChoiceImplFDC(m_fdc, m_lastId, m_lastAuxId - 1));

        for ( size_t i = 0; i < n; ++i )
        {
            hr = m_fdc->AddControlItem(m_lastId, --m_lastAuxId, strings[i].wc_str());
            if ( FAILED(hr) )
            {
                wxLogApiError(wxS("IFileDialogCustomize::AddControlItem"), hr);
                return nullptr;
            }
        }

        return impl.release();
    }

    wxFileDialogTextCtrlImpl* AddTextCtrl(const wxString& label) override
    {
        m_radioListId = 0;

        HRESULT hr;

        if ( !label.empty() )
        {
            hr = m_fdc->StartVisualGroup(--m_lastAuxId, label.wc_str());
            if ( FAILED(hr) )
                wxLogApiError(wxS("IFileDialogCustomize::StartVisualGroup"), hr);
        }

        hr = m_fdc->AddEditBox(++m_lastId, L"");
        if ( FAILED(hr) )
        {
            wxLogApiError(wxS("IFileDialogCustomize::AddEditBox"), hr);
            return nullptr;
        }

        if ( !label.empty() )
        {
            hr = m_fdc->EndVisualGroup();
            if ( FAILED(hr) )
                wxLogApiError(wxS("IFileDialogCustomize::EndVisualGroup"), hr);
        }

        return new wxFileDialogTextCtrlImplFDC(m_fdc, m_lastId);
    }

    wxFileDialogStaticTextImpl* AddStaticText(const wxString& label) override
    {
        m_radioListId = 0;

        HRESULT hr = m_fdc->AddText(++m_lastId, label.wc_str());
        if ( FAILED(hr) )
        {
            wxLogApiError(wxS("IFileDialogCustomize::AddText"), hr);
            return nullptr;
        }

        return new wxFileDialogStaticTextImplFDC(m_fdc, m_lastId);
    }

private:
    wxCOMPtr<IFileDialogCustomize> m_fdc;

    // IDs used for the custom controls returned from the public AddXXX()
    // functions: they are positive and must be consecutive in order to allow
    // accessing the correspond element of m_controls later, see FindControl().
    DWORD m_lastId;

    // IDs used for any other controls, they're negative (which means they
    // decrement from USHORT_MAX down).
    //
    // Note that auxiliary IDs are sometimes used for the main control, at
    // native level, as with the radio buttons, that are represented by
    // separate controls at wx level, and sometimes for the control elements,
    // such as for the combobox, which itself uses a normal ID, as it
    // corresponds to the wx level control.
    DWORD m_lastAuxId;

    // ID of the current radio button list, i.e. the one to which the next call
    // to AddRadioButton() would add a radio button. 0 if none.
    DWORD m_radioListId;
};

#endif // wxUSE_IFILEOPENDIALOG

} // unnamed namespace

// ----------------------------------------------------------------------------
// wxFileDialogMSWData: private data used by the dialog
// ----------------------------------------------------------------------------

class wxFileDialogMSWData
#if wxUSE_IFILEOPENDIALOG
    : public IFileDialogEvents,
      public IFileDialogControlEvents
#endif // wxUSE_IFILEOPENDIALOG
{
public:
    explicit wxFileDialogMSWData(wxFileDialog* fileDialog)
#if wxUSE_IFILEOPENDIALOG
        : m_fileDialog(fileDialog),
          m_typeAlreadyChanged(false)
#endif // wxUSE_IFILEOPENDIALOG
    {
        wxUnusedVar(fileDialog);

        m_bMovedWindow = false;
        m_centreDir = 0;
    }

    // This class is not really used polymorphically, but gcc still complains
    // if it doesn't have a virtual dtor, so pacify it by adding one.
    virtual ~wxFileDialogMSWData() { }

    // Hook function used by the common dialogs: it's a member of this class
    // just to allow it to call the private functions of wxFileDialog.
    static UINT_PTR APIENTRY
    HookFunction(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);

    // remember if our SetPosition() or Centre() (which requires special
    // treatment) was called
    bool m_bMovedWindow;
    int m_centreDir;        // nothing to do if 0


#if wxUSE_IFILEOPENDIALOG
    // Store the extra shortcut directories and their flags.
    struct ShortcutData
    {
        ShortcutData(const wxString& path_, int flags_)
            : path(path_), flags(flags_)
        {
        }

        wxString path;
        int flags;
    };
    wxVector<ShortcutData> m_customShortcuts;


    // IUnknown

    wxSTDMETHODIMP QueryInterface(REFIID iid, void** ppv) override
    {
        if ( iid == IID_IUnknown || iid == IID_IFileDialogEvents )
        {
            // The cast is unnecessary, but do it for clarity and symmetry.
            *ppv = static_cast<IFileDialogEvents*>(this);
        }
        else if ( iid == IID_IFileDialogControlEvents )
        {
            // Here the case is necessary to return the pointer of correct
            // type.
            *ppv = static_cast<IFileDialogControlEvents*>(this);
        }
        else
        {
            *ppv = nullptr;

            return E_NOINTERFACE;
        }

        // No need for AddRef(), we're not really reference-counted as our
        // lifetime is determined by wxFileDialog and there should be no
        // outside references to this object once Unadvise() is called.

        return S_OK;
    }

    // Dummy implementations because we're not really ref-counted.
    STDMETHODIMP_(ULONG) AddRef() override { return 1; }
    STDMETHODIMP_(ULONG) Release() override { return 1; }


    // IFileDialogEvents

    wxSTDMETHODIMP OnFileOk(IFileDialog*) override
    {
        // Note that we need to call this hook function from here as the
        // controls are destroyed later and getting their values wouldn't work
        // any more.
        m_fileDialog->MSWOnFileOK();

        return S_OK;
    }

    wxSTDMETHODIMP OnFolderChanging(IFileDialog*, IShellItem*) override { return E_NOTIMPL; }
    wxSTDMETHODIMP OnFolderChange(IFileDialog*) override { return E_NOTIMPL; }

    wxSTDMETHODIMP OnSelectionChange(IFileDialog* pfd) override
    {
        wxCOMPtr<IShellItem> item;
        HRESULT hr = pfd->GetCurrentSelection(&item);
        if ( FAILED(hr) )
            return hr;

        wxString path;
        hr = wxMSWImpl::GetFSPathFromShellItem(item, path);
        if ( FAILED(hr) )
            return hr;

        m_fileDialog->MSWOnSelChange(path);

        return S_OK;
    }

    wxSTDMETHODIMP OnShareViolation(IFileDialog*, IShellItem*, FDE_SHAREVIOLATION_RESPONSE*) override { return E_NOTIMPL; }

    wxSTDMETHODIMP OnTypeChange(IFileDialog* pfd) override
    {
        // There is no special notification for the dialog initialization, but
        // this function is always called when it's shown, so use it for
        // generating this notification as well.
        if ( !m_typeAlreadyChanged )
        {
            m_typeAlreadyChanged = true;

            wxCOMPtr<IOleWindow> window;
            HRESULT hr = pfd->QueryInterface(wxIID_PPV_ARGS(IOleWindow, &window));
            if ( SUCCEEDED(hr) )
            {
                HWND hwnd;
                hr = window->GetWindow(&hwnd);
                if ( SUCCEEDED(hr) )
                    m_fileDialog->MSWOnInitDone(hwnd);
            }
        }

        m_fileDialog->MSWOnTypeChange(FileDialogGetFileTypeIndex(pfd));

        return S_OK;
    }

    wxSTDMETHODIMP OnOverwrite(IFileDialog*, IShellItem*, FDE_OVERWRITE_RESPONSE*) override { return E_NOTIMPL; }


    // IFileDialogControlEvents

    wxSTDMETHODIMP
    OnItemSelected(IFileDialogCustomize*,
                   DWORD WXUNUSED(dwIDCtl),
                   DWORD dwIDItem) override
    {
        // Note that we don't use dwIDCtl here because we use unique item IDs
        // for all controls.
        if ( wxFileDialogCustomControl* const
                control = m_customize.FindControl(dwIDItem) )
        {
            wxCommandEvent event(wxEVT_RADIOBUTTON, dwIDItem);
            event.SetEventObject(control);
            event.SetInt(true); // Ensure IsChecked() returns true.

            control->SafelyProcessEvent(event);
        }

        return S_OK;
    }

    wxSTDMETHODIMP
    OnButtonClicked(IFileDialogCustomize*, DWORD dwIDCtl) override
    {
        if ( wxFileDialogCustomControl* const
                control = m_customize.FindControl(dwIDCtl) )
        {
            wxCommandEvent event(wxEVT_BUTTON, dwIDCtl);
            event.SetEventObject(control);

            control->SafelyProcessEvent(event);
        }

        return S_OK;
    }

    wxSTDMETHODIMP
    OnCheckButtonToggled(IFileDialogCustomize*,
                         DWORD dwIDCtl,
                         BOOL bChecked) override
    {
        if ( wxFileDialogCustomControl* const
                control = m_customize.FindControl(dwIDCtl) )
        {
            wxCommandEvent event(wxEVT_CHECKBOX, dwIDCtl);
            event.SetEventObject(control);
            event.SetInt(bChecked); // This is for wxCommandEvent::IsChecked().

            control->SafelyProcessEvent(event);
        }

        return S_OK;
    }

    wxSTDMETHODIMP
    OnControlActivating(IFileDialogCustomize*,
                        DWORD WXUNUSED(dwIDCtl)) override
    {
        return S_OK;
    }


    wxFileDialog* const m_fileDialog;

    wxFileDialogCustomizeFDC m_customize;

    bool m_typeAlreadyChanged;
#endif // wxUSE_IFILEOPENDIALOG

    wxDECLARE_NO_COPY_CLASS(wxFileDialogMSWData);
};

// ----------------------------------------------------------------------------
// hook function for moving the dialog
// ----------------------------------------------------------------------------

UINT_PTR APIENTRY
wxFileDialogMSWData::HookFunction(HWND      hDlg,
                                  UINT      iMsg,
                                  WPARAM    WXUNUSED(wParam),
                                  LPARAM    lParam)
{
    switch ( iMsg )
    {
        case WM_INITDIALOG:
            {
                OPENFILENAME* ofn = reinterpret_cast<OPENFILENAME *>(lParam);
                reinterpret_cast<wxFileDialog *>(ofn->lCustData)
                    ->MSWOnInitDialogHook((WXHWND)hDlg);
            }
            break;

        case WM_NOTIFY:
            {
                NMHDR* const pNM = reinterpret_cast<NMHDR*>(lParam);
                if ( pNM->code > CDN_LAST && pNM->code <= CDN_FIRST )
                {
                    const OPENFILENAME&
                        ofn = *reinterpret_cast<OFNOTIFY*>(lParam)->lpOFN;
                    wxFileDialog* const
                        dialog = reinterpret_cast<wxFileDialog*>(ofn.lCustData);

                    switch ( pNM->code )
                    {
                        case CDN_INITDONE:
                            // Note the dialog is the parent window: hDlg is a
                            // child of it when OFN_EXPLORER is used
                            dialog->MSWOnInitDone((WXHWND)::GetParent(hDlg));

                            // Call selection change handler so that update
                            // handler will be called once with no selection.
                            dialog->MSWOnSelChange(wxString());
                            break;

                        case CDN_SELCHANGE:
                            {
                                TCHAR buf[MAX_PATH];
                                LRESULT len = SendMessage
                                              (
                                               ::GetParent(hDlg),
                                               CDM_GETFILEPATH,
                                               MAX_PATH,
                                               reinterpret_cast<LPARAM>(buf)
                                              );

                                wxString str;
                                if ( len )
                                    str = buf;
                                dialog->MSWOnSelChange(str);
                            }
                            break;

                        case CDN_TYPECHANGE:
                            dialog->MSWOnTypeChange(ofn.nFilterIndex);
                            break;

                        case CDN_FILEOK:
                            dialog->MSWOnFileOK();
                            break;
                    }
                }
            }
            break;

        case WM_DESTROY:
            // reuse the position used for the dialog the next time by default
            //
            // NB: at least under Windows 2003 this is useless as after the
            //     first time it's shown the dialog always remembers its size
            //     and position itself and ignores any later SetWindowPos calls
            wxCopyRECTToRect(wxGetWindowRect(::GetParent(hDlg)), gs_rectDialog);
            break;
    }

    // do the default processing
    return 0;
}

// ----------------------------------------------------------------------------
// wxFileDialog
// ----------------------------------------------------------------------------

wxFileDialog::wxFileDialog(wxWindow *parent,
                           const wxString& message,
                           const wxString& defaultDir,
                           const wxString& defaultFileName,
                           const wxString& wildCard,
                           long style,
                           const wxPoint& pos,
                           const wxSize& sz,
                           const wxString& name)
            : wxFileDialogBase(parent, message, defaultDir, defaultFileName,
                               wildCard, style, pos, sz, name)

{
    // NB: all style checks are done by wxFileDialogBase::Create

    m_data = nullptr;

    // Must set to zero, otherwise the wx routines won't size the window
    // the second time you call the file dialog, because it thinks it is
    // already at the requested size.. (when centering)
    gs_rectDialog.x =
    gs_rectDialog.y = 0;
}

wxFileDialog::~wxFileDialog()
{
    delete m_data;
}

wxFileDialogMSWData& wxFileDialog::MSWData()
{
    if ( !m_data )
        m_data = new wxFileDialogMSWData(this);

    return *m_data;
}

void wxFileDialog::GetPaths(wxArrayString& paths) const
{
    paths.Empty();

    wxString dir(m_dir);
    if ( m_dir.empty() || m_dir.Last() != wxT('\\') )
        dir += wxT('\\');

    size_t count = m_fileNames.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        if (wxFileName(m_fileNames[n]).IsAbsolute())
            paths.Add(m_fileNames[n]);
        else
            paths.Add(dir + m_fileNames[n]);
    }
}

void wxFileDialog::GetFilenames(wxArrayString& files) const
{
    files = m_fileNames;
}

void wxFileDialog::DoGetPosition(int *x, int *y) const
{
    // Return the actual HWND position if we have it.
    if ( GetHwnd() )
    {
        wxFileDialogBase::DoGetPosition(x, y);
        return;
    }

    if ( x )
        *x = gs_rectDialog.x;
    if ( y )
        *y = gs_rectDialog.y;
}

void wxFileDialog::DoGetSize(int *width, int *height) const
{
    // Return the actual HWND size if we have it.
    if ( GetHwnd() )
    {
        wxFileDialogBase::DoGetSize(width, height);
        return;
    }

    if ( width )
        *width = gs_rectDialog.width;
    if ( height )
        *height = gs_rectDialog.height;
}

void wxFileDialog::DoMoveWindow(int x, int y, int WXUNUSED(w), int WXUNUSED(h))
{
    gs_rectDialog.x = x;
    gs_rectDialog.y = y;

    // our HWND is only set when we're called from MSWOnInitDone(), test if
    // this is the case
    HWND hwnd = GetHwnd();
    if ( hwnd )
    {
        // size of the dialog can't be changed because the controls are not
        // laid out correctly then
       ::SetWindowPos(hwnd, HWND_TOP, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
    }
    else // just remember that we were requested to move the window
    {
        wxFileDialogMSWData& data = MSWData();
        data.m_bMovedWindow = true;

        // if Centre() had been called before, it shouldn't be taken into
        // account now
        data.m_centreDir = 0;
    }
}

void wxFileDialog::DoCentre(int dir)
{
    wxFileDialogMSWData& data = MSWData();

    data.m_centreDir = dir;
    data.m_bMovedWindow = true;

    // it's unnecessary to do anything else at this stage as we'll redo it in
    // MSWOnInitDone() anyhow
}

void wxFileDialog::MSWOnInitDone(WXHWND hDlg)
{
    if ( !m_data || !m_data->m_bMovedWindow )
    {
        // We only use this to position the dialog, so nothing to do.
        return;
    }

    // set HWND so that our DoMoveWindow() works correctly
    TempHWNDSetter set(this, hDlg);

    if ( m_data->m_centreDir )
    {
        // now we have the real dialog size, remember it
        RECT rect;
        GetWindowRect(hDlg, &rect);
        gs_rectDialog = wxRectFromRECT(rect);

        // and position the window correctly: notice that we must use the base
        // class version as our own doesn't do anything except setting flags
        wxFileDialogBase::DoCentre(m_data->m_centreDir);
    }
    else // need to just move it to the correct place
    {
        SetPosition(gs_rectDialog.GetPosition());
    }
}

void wxFileDialog::MSWOnSelChange(const wxString& selectedFilename)
{
    m_currentlySelectedFilename = selectedFilename;

    UpdateExtraControlUI();
}

void wxFileDialog::MSWOnTypeChange(int nFilterIndex)
{
    // Filter indices are 1-based, while we want to use 0-based index, as
    // usual. However the input index can apparently also be 0 in some
    // circumstances, so take care before decrementing it.
    m_currentlySelectedFilterIndex = nFilterIndex ? nFilterIndex - 1 : 0;

    UpdateExtraControlUI();
}

void wxFileDialog::MSWOnFileOK()
{
    TransferDataFromExtraControl();
}

// helper used below in ShowCommFileDialog(): style is used to determine
// whether to show the "Save file" dialog (if it contains wxFD_SAVE bit) or
// "Open file" one; returns true on success or false on failure in which case
// err is filled with the CDERR_XXX constant
static bool DoShowCommFileDialog(OPENFILENAME *of, long style, DWORD *err)
{
    // Extra controls do not handle per-monitor DPI, fall back to system DPI
    // so entire file-dialog is resized.
    std::unique_ptr<wxMSWImpl::AutoSystemDpiAware> dpiAwareness;
    if ( of->Flags & OFN_ENABLEHOOK )
        dpiAwareness.reset(new wxMSWImpl::AutoSystemDpiAware());

    if ( style & wxFD_SAVE ? GetSaveFileName(of) : GetOpenFileName(of) )
        return true;

    if ( err )
    {
        *err = CommDlgExtendedError();
    }

    return false;
}

void wxFileDialog::MSWOnInitDialogHook(WXHWND hwnd)
{
    // Remember the HWND so that various operations using the dialog geometry
    // work correctly while it's shown.
    SetHWND(hwnd);

    CreateExtraControl();
}

bool wxFileDialog::AddShortcut(const wxString& directory, int flags)
{
#if wxUSE_IFILEOPENDIALOG
    if ( !HasExtraControlCreator() )
    {
        MSWData().m_customShortcuts.push_back(
            wxFileDialogMSWData::ShortcutData(directory, flags)
        );

        return true;
    }
    else
    {
        // It could be surprising if AddShortcut() silently didn't work, so
        // warn the developer about this incompatibility.
        wxFAIL_MSG("Can't use both AddShortcut() and SetExtraControlCreator()");
    }
#endif // wxUSE_IFILEOPENDIALOG

    return false;
}

int wxFileDialog::ShowModal()
{
    WX_HOOK_MODAL_DIALOG();

    wxWindow* const parent = GetParentForModalDialog(m_parent, GetWindowStyle());
    WXHWND hWndParent = parent ? GetHwndOf(parent) : nullptr;

    wxWindowDisabler disableOthers(this, parent);

    /*
        We need to use the old style dialog in order to use a hook function
        which allows us to use custom controls in it but, if possible, we
        prefer to use the new style one instead.
    */
#if wxUSE_IFILEOPENDIALOG
    if ( !HasExtraControlCreator() )
    {
        const int rc = ShowIFileDialog(hWndParent);
        if ( rc != wxID_NONE )
            return rc;
        //else: Failed to use IFileDialog, fall back to the traditional one.
    }
#endif // wxUSE_IFILEOPENDIALOG

    return ShowCommFileDialog(hWndParent);
}

int wxFileDialog::ShowCommFileDialog(WXHWND hWndParent)
{
    static wxChar fileNameBuffer [ wxMAXPATH ];           // the file-name
    wxChar        titleBuffer    [ wxMAXFILE+1+wxMAXEXT ];  // the file-name, without path

    *fileNameBuffer = wxT('\0');
    *titleBuffer    = wxT('\0');

    long msw_flags = OFN_HIDEREADONLY;

    if ( HasFdFlag(wxFD_NO_FOLLOW) )
        msw_flags |= OFN_NODEREFERENCELINKS;

    if ( HasFdFlag(wxFD_FILE_MUST_EXIST) )
        msw_flags |= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if ( HasFlag(wxFD_SHOW_HIDDEN) )
        msw_flags |= OFN_FORCESHOWHIDDEN;
    /*
        If the window has been moved the programmer is probably
        trying to center or position it.  Thus we set the callback
        or hook function so that we can actually adjust the position.
        Without moving or centering the dlg, it will just stay
        in the upper left of the frame, it does not center
        automatically.
    */
    if ((m_data && m_data->m_bMovedWindow) ||
            HasExtraControlCreator() ||
                m_customizeHook)
    {
        ChangeExceptionPolicy();
        msw_flags |= OFN_EXPLORER|OFN_ENABLEHOOK;
        msw_flags |= OFN_ENABLESIZING;
    }

    wxON_BLOCK_EXIT0(RestoreExceptionPolicy);

    if ( HasFdFlag(wxFD_MULTIPLE) )
    {
        // OFN_EXPLORER must always be specified with OFN_ALLOWMULTISELECT
        msw_flags |= OFN_EXPLORER | OFN_ALLOWMULTISELECT;
    }

    // if wxFD_CHANGE_DIR flag is not given we shouldn't change the CWD which the
    // standard dialog does by default (notice that under NT it does it anyhow,
    // OFN_NOCHANGEDIR or not, see below)
    if ( !HasFdFlag(wxFD_CHANGE_DIR) )
    {
        msw_flags |= OFN_NOCHANGEDIR;
    }

    if ( HasFdFlag(wxFD_OVERWRITE_PROMPT) )
    {
        msw_flags |= OFN_OVERWRITEPROMPT;
    }

    OPENFILENAME of;
    wxZeroMemory(of);

    of.lStructSize       = sizeof(OPENFILENAME);
    of.hwndOwner         = hWndParent;
    of.lpstrTitle        = m_message.t_str();
    of.lpstrFileTitle    = titleBuffer;
    of.nMaxFileTitle     = wxMAXFILE + 1 + wxMAXEXT;

    GlobalPtr hgbl;
    if ( HasExtraControlCreator() || m_customizeHook )
    {
        msw_flags |= OFN_ENABLETEMPLATEHANDLE;

        hgbl.Init(256, GMEM_ZEROINIT);
        GlobalPtrLock hgblLock(hgbl);
        LPDLGTEMPLATE lpdt = static_cast<LPDLGTEMPLATE>(hgblLock.Get());

        // Define a dialog box.

        lpdt->style = DS_CONTROL | WS_CHILD | WS_CLIPSIBLINGS;
        lpdt->cdit = 0;         // Number of controls
        lpdt->x = 0;
        lpdt->y = 0;

        // create the extra control in an empty dialog just to find its size: this
        // is not terribly efficient but we do need to know the size before
        // creating the native dialog and this seems to be the only way
        wxDialog dlg(nullptr, wxID_ANY, wxString());
        const wxSize extraSize = CreateExtraControlWithParent(&dlg)->GetSize();

        // convert the size of the extra controls to the dialog units
        const LONG baseUnits = ::GetDialogBaseUnits();
        lpdt->cx = ::MulDiv(extraSize.x, 4, LOWORD(baseUnits));
        lpdt->cy = ::MulDiv(extraSize.y, 8, HIWORD(baseUnits));

        // after the DLGTEMPLATE there are 3 additional WORDs for dialog menu,
        // class and title, all three set to zeros.

        of.hInstance = (HINSTANCE)lpdt;
    }

    // Convert forward slashes to backslashes (file selector doesn't like
    // forward slashes) and also squeeze multiple consecutive slashes into one
    // as it doesn't like two backslashes in a row either

    wxString  dir;
    size_t    i, len = m_dir.length();
    dir.reserve(len);
    for ( i = 0; i < len; i++ )
    {
        wxChar ch = m_dir[i];
        switch ( ch )
        {
            case wxT('/'):
                // convert to backslash
                ch = wxT('\\');
                wxFALLTHROUGH;

            case wxT('\\'):
                while ( i < len - 1 )
                {
                    wxChar chNext = m_dir[i + 1];
                    if ( chNext != wxT('\\') && chNext != wxT('/') )
                        break;

                    // ignore the next one, unless it is at the start of a UNC path
                    if (i > 0)
                        i++;
                    else
                        break;
                }
                wxFALLTHROUGH;

            default:
                // normal char
                dir += ch;
        }
    }

    of.lpstrInitialDir   = dir.c_str();

    of.Flags             = msw_flags;
    of.lpfnHook          = wxFileDialogMSWData::HookFunction;
    of.lCustData         = (LPARAM)this;

    wxArrayString wildDescriptions, wildFilters;

    size_t items = wxParseCommonDialogsFilter(m_wildCard, wildDescriptions, wildFilters);

    wxASSERT_MSG( items > 0 , wxT("empty wildcard list") );

    wxString filterBuffer;

    for (i = 0; i < items ; i++)
    {
        filterBuffer += wildDescriptions[i];
        filterBuffer += wxT("|");
        filterBuffer += wildFilters[i];
        filterBuffer += wxT("|");
    }

    // Replace | with \0
    for (i = 0; i < filterBuffer.length(); i++ ) {
        if ( filterBuffer.GetChar(i) == wxT('|') ) {
            filterBuffer[i] = wxT('\0');
        }
    }

    of.lpstrFilter  = filterBuffer.t_str();
    of.nFilterIndex = m_filterIndex + 1;
    m_currentlySelectedFilterIndex = m_filterIndex;

    //=== Setting defaultFileName >>=========================================

    wxStrlcpy(fileNameBuffer, m_fileName.c_str(), WXSIZEOF(fileNameBuffer));

    of.lpstrFile = fileNameBuffer;  // holds returned filename
    of.nMaxFile  = wxMAXPATH;

    // we must set the default extension because otherwise Windows would check
    // for the existing of a wrong file with wxFD_OVERWRITE_PROMPT (i.e. if the
    // user types "foo" and the default extension is ".bar" we should force it
    // to check for "foo.bar" existence and not "foo")
    wxString defextBuffer; // we need it to be alive until GetSaveFileName()!
    if (HasFdFlag(wxFD_SAVE))
    {
        const wxChar* extension = filterBuffer.t_str();
        int maxFilter = (int)(of.nFilterIndex*2L) - 1;

        for( int j = 0; j < maxFilter; j++ )           // get extension
            extension = extension + wxStrlen( extension ) + 1;

        // use dummy name a to avoid assert in AppendExtension
        defextBuffer = AppendExtension(wxT("a"), extension);
        if (defextBuffer.StartsWith(wxT("a.")))
        {
            defextBuffer = defextBuffer.Mid(2); // remove "a."
            of.lpstrDefExt = defextBuffer.c_str();
        }
    }

    // Create a temporary struct to restore the CWD when we exit this function
    // store off before the standard windows dialog can possibly change it
    struct CwdRestore
    {
        wxString value;
        ~CwdRestore()
        {
            if (!value.empty())
                wxSetWorkingDirectory(value);
        }
    } cwdOrig;

    // GetOpenFileName will always change the current working directory
    // (according to MSDN) because the flag OFN_NOCHANGEDIR has no effect.
    // If the user did not specify wxFD_CHANGE_DIR let's restore the
    // current working directory to what it was before the dialog was shown.
    if (msw_flags & OFN_NOCHANGEDIR)
        cwdOrig.value = wxGetCwd();

    //== Execute FileDialog >>=================================================

    DWORD errCode;
    bool success = DoShowCommFileDialog(&of, m_windowStyle, &errCode);

    // When using a hook, our HWND was set from MSWOnInitDialogHook() called
    // above, but it's not valid any longer once the dialog was destroyed, so
    // reset it now.
    if ( msw_flags & OFN_ENABLEHOOK )
        SetHWND(0);

    if ( !success &&
            errCode == FNERR_INVALIDFILENAME &&
                of.lpstrFile[0] )
    {
        // this can happen if the default file name is invalid, try without it
        // now
        of.lpstrFile[0] = wxT('\0');
        success = DoShowCommFileDialog(&of, m_windowStyle, &errCode);
    }

    if ( !success )
    {
        // common dialog failed - why?
        if ( errCode != 0 )
        {
            wxLogError(_("File dialog failed with error code %0lx."), errCode);
        }
        //else: it was just cancelled

        return wxID_CANCEL;
    }

    m_fileNames.Empty();

    if ( ( HasFdFlag(wxFD_MULTIPLE) ) &&
         ( fileNameBuffer[of.nFileOffset-1] == wxT('\0') )
       )
    {
        m_dir = fileNameBuffer;
        i = of.nFileOffset;
        m_fileName = &fileNameBuffer[i];
        m_fileNames.Add(m_fileName);
        i += m_fileName.length() + 1;

        while (fileNameBuffer[i] != wxT('\0'))
        {
            m_fileNames.Add(&fileNameBuffer[i]);
            i += wxStrlen(&fileNameBuffer[i]) + 1;
        }

        m_path = m_dir;
        if ( m_dir.Last() != wxT('\\') )
            m_path += wxT('\\');

        m_path += m_fileName;
        m_filterIndex = (int)of.nFilterIndex - 1;
    }
    else
    {
        //=== Adding the correct extension >>=================================

        m_filterIndex = (int)of.nFilterIndex - 1;

        if ( !of.nFileExtension || fileNameBuffer[of.nFileExtension] == wxT('\0') )
        {
            // User has typed a filename without an extension:
            const wxChar* extension = filterBuffer.t_str();
            int   maxFilter = (int)(of.nFilterIndex*2L) - 1;

            for( int j = 0; j < maxFilter; j++ )           // get extension
                extension = extension + wxStrlen( extension ) + 1;

            m_fileName = AppendExtension(fileNameBuffer, extension);
            wxStrlcpy(fileNameBuffer, m_fileName.c_str(), WXSIZEOF(fileNameBuffer));
        }

        m_path = fileNameBuffer;
        m_fileName = wxFileNameFromPath(fileNameBuffer);
        m_fileNames.Add(m_fileName);
        m_dir = wxPathOnly(fileNameBuffer);
    }

    return wxID_OK;

}

#if wxUSE_IFILEOPENDIALOG

int wxFileDialog::ShowIFileDialog(WXHWND hWndParent)
{
    // Create the dialog.
    wxMSWImpl::wxIFileDialog
        fileDialog(HasFlag(wxFD_SAVE) ? CLSID_FileSaveDialog
                                      : CLSID_FileOpenDialog);

    if ( !fileDialog.IsOk() )
        return wxID_NONE;

    // Register our event handler with the dialog.
    wxFileDialogMSWData& data = MSWData();

    FileDialogEventsRegistrar registerEvents(fileDialog, data);

    // Add custom controls, if any.
    if ( m_customizeHook )
    {
        if ( data.m_customize.Initialize(fileDialog.Get()) )
            m_customizeHook->AddCustomControls(data.m_customize);
    }

    // Configure the dialog before showing it.
    fileDialog.SetTitle(m_message);

    HRESULT hr;

    wxArrayString wildDescriptions, wildFilters;
    const UINT nWildcards = wxParseCommonDialogsFilter(m_wildCard,
                                                       wildDescriptions,
                                                       wildFilters);
    if ( nWildcards )
    {
        wxVector<COMDLG_FILTERSPEC> filterSpecs(nWildcards);
        for ( UINT n = 0; n < nWildcards; ++n )
        {
            filterSpecs[n].pszName = wildDescriptions[n].wc_str();
            filterSpecs[n].pszSpec = wildFilters[n].wc_str();
        }

        hr = fileDialog->SetFileTypes(nWildcards, &filterSpecs[0]);
        if ( FAILED(hr) )
            wxLogApiError(wxS("IFileDialog::SetFileTypes"), hr);

        hr = fileDialog->SetFileTypeIndex(m_filterIndex + 1);
        if ( FAILED(hr) )
            wxLogApiError(wxS("IFileDialog::SetFileTypeIndex"), hr);

        // We need to call SetDefaultExtension() to make the file dialog append
        // the selected extension by default. It will append the correct
        // extension depending on the current file type choice if we call this
        // function, but won't do anything at all without it, so find the first
        // extension associated with the selected filter and use it here.
        wxString defExt =
            wildFilters[m_filterIndex].BeforeFirst(';').AfterFirst('.');
        if ( !defExt.empty() && defExt != wxS("*") )
        {
            hr = fileDialog->SetDefaultExtension(defExt.wc_str());
            if ( FAILED(hr) )
                wxLogApiError(wxS("IFileDialog::SetDefaultExtension"), hr);
        }
    }

    if ( !m_dir.empty() )
    {
        fileDialog.SetInitialPath(m_dir);
    }

    if ( !m_fileName.empty() )
    {
        hr = fileDialog->SetFileName(m_fileName.wc_str());
        if ( FAILED(hr) )
            wxLogApiError(wxS("IFileDialog::SetFileName"), hr);
    }


    for ( wxVector<wxFileDialogMSWData::ShortcutData>::const_iterator
            it = data.m_customShortcuts.begin();
            it != data.m_customShortcuts.end();
            ++it )
    {
        FDAP fdap = FDAP_BOTTOM;
        if ( it->flags & wxFD_SHORTCUT_TOP )
        {
            wxASSERT_MSG( !(it->flags & wxFD_SHORTCUT_BOTTOM),
                          wxS("Can't use both wxFD_SHORTCUT_TOP and BOTTOM") );

            fdap = FDAP_TOP;
        }

        fileDialog.AddPlace(it->path, fdap);
    }

    // We never set the following flags currently:
    //
    //  - FOS_STRICTFILETYPES
    //  - FOS_NOVALIDATE
    //  - FOS_CREATEPROMPT
    //  - FOS_SHAREAWARE
    //  - FOS_NOREADONLYRETURN
    //  - FOS_NOTESTFILECREATE
    //  - FOS_OKBUTTONNEEDSINTERACTION
    //  - FOS_DONTADDTORECENT
    //  - FOS_DEFAULTNOMINIMODE
    //  - FOS_FORCEPREVIEWPANEON
    //
    // We might want to add wxFD_XXX equivalents for some of them in the future.
    int options = 0;
    if ( HasFlag(wxFD_OVERWRITE_PROMPT) )
        options |= FOS_OVERWRITEPROMPT;
    if ( !HasFlag(wxFD_CHANGE_DIR) )
        options |= FOS_NOCHANGEDIR;
    if ( HasFlag(wxFD_FILE_MUST_EXIST) )
        options |= FOS_PATHMUSTEXIST | FOS_FILEMUSTEXIST;
    if ( HasFlag(wxFD_MULTIPLE) )
        options |= FOS_ALLOWMULTISELECT;
    if ( HasFlag(wxFD_SHOW_HIDDEN) )
        options |= FOS_FORCESHOWHIDDEN;
    if ( HasFlag(wxFD_NO_FOLLOW) )
        options |= FOS_NODEREFERENCELINKS;

    // Finally do show the dialog.
    const int rc = fileDialog.Show(hWndParent, options, &m_fileNames, &m_path);
    if ( rc == wxID_OK )
    {
        // As with the common dialog, the index is 1-based here, but don't make
        // it negative if we somehow failed to retrieve it at all.
        m_filterIndex = FileDialogGetFileTypeIndex(fileDialog.Get());
        if ( m_filterIndex > 0 )
            m_filterIndex--;

        if ( HasFlag(wxFD_MULTIPLE) )
        {
            // This shouldn't the case, but check to be absolutely sure.
            if ( !m_fileNames.empty() )
                m_dir = wxFileName(m_fileNames[0]).GetPath();
        }
        else // Single selected file is in m_path.
        {
            // Append the extension if necessary.
            m_path = AppendExtension(m_path, wildFilters[m_filterIndex]);

            const wxFileName fn(m_path);
            m_dir = fn.GetPath();
            m_fileName = fn.GetFullName();

            // For compatibility, our GetFilenames() must also return the same
            // file, so put it into the array too.
            m_fileNames.Clear();
            m_fileNames.Add(m_fileName);
        }
    }

    return rc;
}

#endif // wxUSE_IFILEOPENDIALOG

#endif // wxUSE_FILEDLG
