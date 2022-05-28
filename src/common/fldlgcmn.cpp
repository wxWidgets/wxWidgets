/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/fldlgcmn.cpp
// Purpose:     wxFileDialog common functions
// Author:      John Labenski
// Modified by:
// Created:     14.06.03 (extracted from src/*/filedlg.cpp)
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_FILEDLG

#include "wx/filedlg.h"
#include "wx/dirdlg.h"
#include "wx/filename.h"

#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/intl.h"
    #include "wx/window.h"
#endif // WX_PRECOMP

#include "wx/filedlgcustomize.h"
#include "wx/private/filedlgcustomize.h"

extern WXDLLEXPORT_DATA(const char) wxFileDialogNameStr[] = "filedlg";
extern WXDLLEXPORT_DATA(const char) wxFileSelectorPromptStr[] = "Select a file";
extern WXDLLEXPORT_DATA(const char) wxFileSelectorDefaultWildcardStr[] =
#if defined(__WXMSW__)
    "*.*"
#else // Unix/Mac
    "*"
#endif
    ;

// ============================================================================
// File dialog customization support
// ============================================================================

// ----------------------------------------------------------------------------
// wxFileDialogCustomControl and derived classes
// ----------------------------------------------------------------------------

// Everything here is trivial, but has to be defined here, where the private
// "Impl" classes are fully declared.

wxFileDialogCustomControlImpl::~wxFileDialogCustomControlImpl()
{
}

bool wxFileDialogCustomControl::OnDynamicBind(wxDynamicEventTableEntry& entry)
{
    wxUnusedVar(entry); // Needed when debug is disabled.

    wxFAIL_MSG(wxString::Format
              (
                "This custom control doesn't generate the event %d.",
                entry.m_eventType
              ));

    return false;
}

void wxFileDialogCustomControl::Show(bool show)
{
    return m_impl->Show(show);
}

void wxFileDialogCustomControl::Enable(bool enable)
{
    return m_impl->Enable(enable);
}

wxFileDialogCustomControl::~wxFileDialogCustomControl()
{
    delete m_impl;
}

wxFileDialogButton::wxFileDialogButton(wxFileDialogButtonImpl* impl)
    : wxFileDialogCustomControl(impl)
{
}

bool wxFileDialogButton::OnDynamicBind(wxDynamicEventTableEntry& entry)
{
    if ( entry.m_eventType == wxEVT_BUTTON )
        return true;

    return wxFileDialogCustomControl::OnDynamicBind(entry);
}

wxFileDialogButtonImpl* wxFileDialogButton::GetImpl() const
{
    return static_cast<wxFileDialogButtonImpl*>(m_impl);
}

wxFileDialogCheckBox::wxFileDialogCheckBox(wxFileDialogCheckBoxImpl* impl)
    : wxFileDialogCustomControl(impl)
{
}

bool wxFileDialogCheckBox::OnDynamicBind(wxDynamicEventTableEntry& entry)
{
    if ( entry.m_eventType == wxEVT_CHECKBOX )
        return true;

    return wxFileDialogCustomControl::OnDynamicBind(entry);
}

wxFileDialogCheckBoxImpl* wxFileDialogCheckBox::GetImpl() const
{
    return static_cast<wxFileDialogCheckBoxImpl*>(m_impl);
}

bool wxFileDialogCheckBox::GetValue() const
{
    return GetImpl()->GetValue();
}

void wxFileDialogCheckBox::SetValue(bool value)
{
    GetImpl()->SetValue(value);
}

wxFileDialogTextCtrl::wxFileDialogTextCtrl(wxFileDialogTextCtrlImpl* impl)
    : wxFileDialogCustomControl(impl)
{
}

wxFileDialogTextCtrlImpl* wxFileDialogTextCtrl::GetImpl() const
{
    return static_cast<wxFileDialogTextCtrlImpl*>(m_impl);
}

wxString wxFileDialogTextCtrl::GetValue() const
{
    return GetImpl()->GetValue();
}

void wxFileDialogTextCtrl::SetValue(const wxString& value)
{
    GetImpl()->SetValue(value);
}

wxFileDialogStaticText::wxFileDialogStaticText(wxFileDialogStaticTextImpl* impl)
    : wxFileDialogCustomControl(impl)
{
}

wxFileDialogStaticTextImpl* wxFileDialogStaticText::GetImpl() const
{
    return static_cast<wxFileDialogStaticTextImpl*>(m_impl);
}

void wxFileDialogStaticText::SetLabelText(const wxString& text)
{
    GetImpl()->SetLabelText(text);
}

// ----------------------------------------------------------------------------
// wxFileDialogCustomize
// ----------------------------------------------------------------------------

wxFileDialogCustomizeHook::~wxFileDialogCustomizeHook()
{
}

wxFileDialogCustomizeImpl::~wxFileDialogCustomizeImpl()
{
}

wxFileDialogCustomize::~wxFileDialogCustomize()
{
    // For consistency with the rest of wx API, we own all the custom controls
    // pointers and delete them when we're deleted.
    for ( size_t n = 0; n < m_controls.size(); ++n )
        delete m_controls[n];

    // Do not delete m_impl, the derived classes use this object itself as
    // implementation, which allows us to avoid allocating it on the heap in
    // the first place.
}

template <typename T>
T*
wxFileDialogCustomize::StoreAndReturn(T* control)
{
    m_controls.push_back(control);
    return control;
}

wxFileDialogButton*
wxFileDialogCustomize::AddButton(const wxString& label)
{
    return StoreAndReturn(new wxFileDialogButton(m_impl->AddButton(label)));
}

wxFileDialogCheckBox*
wxFileDialogCustomize::AddCheckBox(const wxString& label)
{
    return StoreAndReturn(new wxFileDialogCheckBox(m_impl->AddCheckBox(label)));
}

wxFileDialogTextCtrl*
wxFileDialogCustomize::AddTextCtrl(const wxString& label)
{
    return StoreAndReturn(new wxFileDialogTextCtrl(m_impl->AddTextCtrl(label)));
}

wxFileDialogStaticText*
wxFileDialogCustomize::AddStaticText(const wxString& label)
{
    return StoreAndReturn(new wxFileDialogStaticText(m_impl->AddStaticText(label)));
}

//----------------------------------------------------------------------------
// wxFileDialogBase
//----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxFileDialogBase, wxDialog);

void wxFileDialogBase::Init()
{
    m_filterIndex = 0;
    m_currentlySelectedFilterIndex = wxNOT_FOUND;
    m_windowStyle = 0;
    m_customizeHook = NULL;
    m_extraControl = NULL;
    m_extraControlCreator = NULL;
}

bool wxFileDialogBase::Create(wxWindow *parent,
                              const wxString& message,
                              const wxString& defaultDir,
                              const wxString& defaultFile,
                              const wxString& wildCard,
                              long style,
                              const wxPoint& WXUNUSED(pos),
                              const wxSize& WXUNUSED(sz),
                              const wxString& WXUNUSED(name))
{
    m_message = message;
    m_dir = defaultDir;
    m_fileName = defaultFile;
    m_wildCard = wildCard;

    m_parent = parent;

#ifdef __WXOSX__
    /*
    [DS]
    Remove the (for OS X unnecessary) wxFD_FILE_MUST_EXIST flag. Using it
    causes problems when having an extra panel (either a custom one or
    by showing the filetype filters through calling
    wxSystemOptions::SetOption(wxOSX_FILEDIALOG_ALWAYS_SHOW_TYPES, 1) ).
    Presumably the style flag conflicts with other style flags and an
    assert in wxRegion::DoOffset is triggered later on.
    Another solution was to override GetWindowStyleFlag() to not include
    wxFD_FILE_MUST_EXIST in its return value, but as other wxFileDialog
    style flags (that are actually used) don't seem to cause problems
    this seemed an easier solution.
    */
    style &= ~wxFD_FILE_MUST_EXIST;
#endif

    m_windowStyle = style;

    if (!HasFdFlag(wxFD_OPEN) && !HasFdFlag(wxFD_SAVE))
        m_windowStyle |= wxFD_OPEN;     // wxFD_OPEN is the default

    // check that the styles are not contradictory
    wxASSERT_MSG( !(HasFdFlag(wxFD_SAVE) && HasFdFlag(wxFD_OPEN)),
                  wxT("can't specify both wxFD_SAVE and wxFD_OPEN at once") );

    wxASSERT_MSG( !HasFdFlag(wxFD_SAVE) ||
                    (!HasFdFlag(wxFD_MULTIPLE) && !HasFdFlag(wxFD_FILE_MUST_EXIST)),
                   wxT("wxFD_MULTIPLE or wxFD_FILE_MUST_EXIST can't be used with wxFD_SAVE" ) );

    wxASSERT_MSG( !HasFdFlag(wxFD_OPEN) || !HasFdFlag(wxFD_OVERWRITE_PROMPT),
                  wxT("wxFD_OVERWRITE_PROMPT can't be used with wxFD_OPEN") );

    if ( wildCard.empty() || wildCard == wxFileSelectorDefaultWildcardStr )
    {
        m_wildCard = wxString::Format(_("All files (%s)|%s"),
                                      wxFileSelectorDefaultWildcardStr,
                                      wxFileSelectorDefaultWildcardStr);
    }
    else // have wild card
    {
        // convert m_wildCard from "*.bar" to "bar files (*.bar)|*.bar"
        if ( m_wildCard.Find(wxT('|')) == wxNOT_FOUND )
        {
            wxString::size_type nDot = m_wildCard.find(wxT("*."));
            if ( nDot != wxString::npos )
                nDot++;
            else
                nDot = 0;

            m_wildCard = wxString::Format
                         (
                            _("%s files (%s)|%s"),
                            wildCard.c_str() + nDot,
                            wildCard.c_str(),
                            wildCard.c_str()
                         );
        }
    }

    return true;
}

wxString wxFileDialogBase::AppendExtension(const wxString &filePath,
                                           const wxString &extensionList)
{
    // strip off path, to avoid problems with "path.bar/foo"
    wxString fileName = filePath.AfterLast(wxFILE_SEP_PATH);

    // if fileName is of form "foo.bar" it's ok, return it
    int idx_dot = fileName.Find(wxT('.'), true);
    if ((idx_dot != wxNOT_FOUND) && (idx_dot < (int)fileName.length() - 1))
        return filePath;

    // get the first extension from extensionList, or all of it
    wxString ext = extensionList.BeforeFirst(wxT(';'));

    // if ext == "foo" or "foo." there's no extension
    int idx_ext_dot = ext.Find(wxT('.'), true);
    if ((idx_ext_dot == wxNOT_FOUND) || (idx_ext_dot == (int)ext.length() - 1))
        return filePath;
    else
        ext = ext.AfterLast(wxT('.'));

    // if ext == "*" or "bar*" or "b?r" or " " then its not valid
    if ((ext.Find(wxT('*')) != wxNOT_FOUND) ||
        (ext.Find(wxT('?')) != wxNOT_FOUND) ||
        (ext.Strip(wxString::both).empty()))
        return filePath;

    // if fileName doesn't have a '.' then add one
    if (filePath.Last() != wxT('.'))
        ext = wxT(".") + ext;

    return filePath + ext;
}

bool wxFileDialogBase::SetCustomizeHook(wxFileDialogCustomizeHook& customizeHook)
{
    if ( !SupportsExtraControl() )
        return false;

    wxASSERT_MSG( !m_extraControlCreator,
                  "Call either SetExtraControlCreator() or SetCustomizeHook()" );

    m_customizeHook = &customizeHook;
    return true;
}

bool wxFileDialogBase::SetExtraControlCreator(ExtraControlCreatorFunction creator)
{
    wxCHECK_MSG( !m_extraControlCreator, false,
                 "wxFileDialog::SetExtraControl() called second time" );

    m_extraControlCreator = creator;
    return SupportsExtraControl();
}

bool wxFileDialogBase::CreateExtraControl()
{
    // We're not supposed to be called more than once normally, but just do
    // nothing if we had already created the custom controls somehow.
    if ( m_extraControl )
        return true;

    if ( m_extraControlCreator )
    {
        m_extraControl = (*m_extraControlCreator)(this);

        return true;
    }

    // It's not an error to call this function if there are no extra controls
    // to create, just do nothing in this case.
    return false;
}

wxSize wxFileDialogBase::GetExtraControlSize()
{
    if ( !m_extraControlCreator )
        return wxDefaultSize;

    // create the extra control in an empty dialog just to find its size: this
    // is not terribly efficient but we do need to know the size before
    // creating the native dialog and this seems to be the only way
    wxDialog dlg(NULL, wxID_ANY, wxString());
    return (*m_extraControlCreator)(&dlg)->GetSize();
}

void wxFileDialogBase::UpdateExtraControlUI()
{
    if ( m_customizeHook )
        m_customizeHook->UpdateCustomControls();

    if ( m_extraControl )
        m_extraControl->UpdateWindowUI(wxUPDATE_UI_RECURSE);
}

void wxFileDialogBase::SetPath(const wxString& path)
{
    wxString ext;
    wxFileName::SplitPath(path, &m_dir, &m_fileName, &ext);
    if ( !ext.empty() )
    {
        SetFilterIndexFromExt(ext);

        m_fileName << wxT('.') << ext;
    }

    m_path = path;
}

void wxFileDialogBase::SetDirectory(const wxString& dir)
{
    m_dir = dir;
    m_path = wxFileName(m_dir, m_fileName).GetFullPath();
}

void wxFileDialogBase::SetFilename(const wxString& name)
{
    m_fileName = name;
    m_path = wxFileName(m_dir, m_fileName).GetFullPath();
}

void wxFileDialogBase::SetFilterIndexFromExt(const wxString& ext)
{
    // if filter is of form "All files (*)|*|..." set correct filter index
    if ( !ext.empty() && m_wildCard.find(wxT('|')) != wxString::npos )
    {
        int filterIndex = -1;

        wxArrayString descriptions, filters;
        // don't care about errors, handled already by wxFileDialog
        (void)wxParseCommonDialogsFilter(m_wildCard, descriptions, filters);
        for (size_t n=0; n<filters.GetCount(); n++)
        {
            if (filters[n].Contains(ext))
            {
                filterIndex = n;
                break;
            }
        }

        if (filterIndex >= 0)
            SetFilterIndex(filterIndex);
    }
}

//----------------------------------------------------------------------------
// wxFileDialog convenience functions
//----------------------------------------------------------------------------

wxString wxFileSelector(const wxString& title,
                        const wxString& defaultDir,
                        const wxString& defaultFileName,
                        const wxString& defaultExtension,
                        const wxString& filter,
                        int flags,
                        wxWindow *parent,
                        int x, int y)
{
    // The defaultExtension, if non-empty, is
    // appended to the filename if the user fails to type an extension. The new
    // implementation (taken from wxFileSelectorEx) appends the extension
    // automatically, by looking at the filter specification. In fact this
    // should be better than the native Microsoft implementation because
    // Windows only allows *one* default extension, whereas here we do the
    // right thing depending on the filter the user has chosen.

    // If there's a default extension specified but no filter, we create a
    // suitable filter.

    wxString filter2;
    if ( !defaultExtension.empty() && filter.empty() )
        filter2 = wxString(wxT("*.")) + defaultExtension;
    else if ( !filter.empty() )
        filter2 = filter;

    wxFileDialog fileDialog(parent, title, defaultDir,
                            defaultFileName, filter2,
                            flags, wxPoint(x, y));

    fileDialog.SetFilterIndexFromExt(defaultExtension);

    wxString filename;
    if ( fileDialog.ShowModal() == wxID_OK )
    {
        filename = fileDialog.GetPath();
    }

    return filename;
}

//----------------------------------------------------------------------------
// wxFileSelectorEx
//----------------------------------------------------------------------------

wxString wxFileSelectorEx(const wxString& title,
                          const wxString& defaultDir,
                          const wxString& defaultFileName,
                          int*            defaultFilterIndex,
                          const wxString& filter,
                          int             flags,
                          wxWindow*       parent,
                          int             x,
                          int             y)

{
    wxFileDialog fileDialog(parent,
                            title,
                            defaultDir,
                            defaultFileName,
                            filter,
                            flags, wxPoint(x, y));

    wxString filename;
    if ( fileDialog.ShowModal() == wxID_OK )
    {
        if ( defaultFilterIndex )
            *defaultFilterIndex = fileDialog.GetFilterIndex();

        filename = fileDialog.GetPath();
    }

    return filename;
}

//----------------------------------------------------------------------------
// wxDefaultFileSelector - Generic load/save dialog (for internal use only)
//----------------------------------------------------------------------------

static wxString wxDefaultFileSelector(bool load,
                                      const wxString& what,
                                      const wxString& extension,
                                      const wxString& default_name,
                                      wxWindow *parent)
{
    wxString prompt;
    wxString str;
    if (load)
        str = _("Load %s file");
    else
        str = _("Save %s file");
    prompt.Printf(str, what);

    wxString wild;
    wxString ext;
    if ( !extension.empty() )
    {
        if ( extension[0u] == wxT('.') )
            ext = extension.substr(1);
        else
            ext = extension;

        wild.Printf(wxT("*.%s"), ext);
    }
    else // no extension specified
    {
        wild = wxFileSelectorDefaultWildcardStr;
    }

    return wxFileSelector(prompt, wxEmptyString, default_name, ext, wild,
                          load ? (wxFD_OPEN | wxFD_FILE_MUST_EXIST) : wxFD_SAVE,
                          parent);
}

//----------------------------------------------------------------------------
// wxLoadFileSelector
//----------------------------------------------------------------------------

WXDLLEXPORT wxString wxLoadFileSelector(const wxString& what,
                                        const wxString& extension,
                                        const wxString& default_name,
                                        wxWindow *parent)
{
    return wxDefaultFileSelector(true, what, extension, default_name, parent);
}

//----------------------------------------------------------------------------
// wxSaveFileSelector
//----------------------------------------------------------------------------

WXDLLEXPORT wxString wxSaveFileSelector(const wxString& what,
                                        const wxString& extension,
                                        const wxString& default_name,
                                        wxWindow *parent)
{
    return wxDefaultFileSelector(false, what, extension, default_name, parent);
}


//----------------------------------------------------------------------------
// wxDirDialogBase
//----------------------------------------------------------------------------

#endif // wxUSE_FILEDLG
