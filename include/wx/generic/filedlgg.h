/////////////////////////////////////////////////////////////////////////////
// Name:        filedlgg.h
// Purpose:     wxGenericFileDialog
// Author:      Robert Roebling
// Modified by:
// Created:     8/17/99
// Copyright:   (c) Robert Roebling
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FILEDLGG_H_
#define _WX_FILEDLGG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "filedlgg.h"
#endif

#include "wx/dialog.h"
#include "wx/listctrl.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxCheckBox;
class wxChoice;
class wxFileData;
class wxFileCtrl;
class wxGenericFileDialog;
class wxListEvent;
class wxListItem;
class wxStaticText;
class wxTextCtrl;

#if defined(__WXUNIVERSAL__)||defined(__WXGTK__)||defined(__WXX11__)||defined(__WXMGL__)||defined(__WXCOCOA__)
    #define USE_GENERIC_FILEDIALOG
#endif

#ifdef USE_GENERIC_FILEDIALOG

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

WXDLLEXPORT_DATA(extern const wxChar *)wxFileSelectorPromptStr;
WXDLLEXPORT_DATA(extern const wxChar *)wxFileSelectorDefaultWildcardStr;

#endif // USE_GENERIC_FILEDIALOG

//-------------------------------------------------------------------------
// File selector
//-------------------------------------------------------------------------

class WXDLLEXPORT wxGenericFileDialog: public wxDialog
{
public:
    wxGenericFileDialog() { }

    wxGenericFileDialog(wxWindow *parent,
                 const wxString& message = wxFileSelectorPromptStr,
                 const wxString& defaultDir = _T(""),
                 const wxString& defaultFile = _T(""),
                 const wxString& wildCard = wxFileSelectorDefaultWildcardStr,
                 long style = 0,
                 const wxPoint& pos = wxDefaultPosition);
    virtual ~wxGenericFileDialog();

    void SetMessage(const wxString& message) { SetTitle(message); }
    void SetPath(const wxString& path);
    void SetDirectory(const wxString& dir) { m_dir = dir; }
    void SetFilename(const wxString& name) { m_fileName = name; }
    void SetWildcard(const wxString& wildCard) { m_wildCard = wildCard; }
    void SetStyle(long style) { m_dialogStyle = style; }
    void SetFilterIndex(int filterIndex);

    wxString GetMessage() const { return m_message; }
    wxString GetPath() const { return m_path; }
    wxString GetDirectory() const { return m_dir; }
    wxString GetFilename() const { return m_fileName; }
    wxString GetWildcard() const { return m_wildCard; }
    long GetStyle() const { return m_dialogStyle; }
    int GetFilterIndex() const { return m_filterIndex; }

    // for multiple file selection
    void GetPaths(wxArrayString& paths) const;
    void GetFilenames(wxArrayString& files) const;

    // implementation only from now on
    // -------------------------------

    virtual int ShowModal();

    void OnSelected( wxListEvent &event );
    void OnActivated( wxListEvent &event );
    void OnList( wxCommandEvent &event );
    void OnReport( wxCommandEvent &event );
    void OnUp( wxCommandEvent &event );
    void OnHome( wxCommandEvent &event );
    void OnListOk( wxCommandEvent &event );
    void OnNew( wxCommandEvent &event );
    void OnChoiceFilter( wxCommandEvent &event );
    void OnTextEnter( wxCommandEvent &event );
    void OnTextChange( wxCommandEvent &event );
    void OnCheck( wxCommandEvent &event );

    void HandleAction( const wxString &fn );

protected:
    // use the filter with the given index
    void DoSetFilterIndex(int filterindex);

    wxString       m_message;
    long           m_dialogStyle;
    wxString       m_dir;
    wxString       m_path; // Full path
    wxString       m_fileName;
    wxString       m_wildCard;
    int            m_filterIndex;
    wxString       m_filterExtension;
    wxChoice      *m_choice;
    wxTextCtrl    *m_text;
    wxFileCtrl    *m_list;
    wxCheckBox    *m_check;
    wxStaticText  *m_static;

private:
    DECLARE_DYNAMIC_CLASS(wxGenericFileDialog)
    DECLARE_EVENT_TABLE()

    // these variables are preserved between wxGenericFileDialog calls
    static long ms_lastViewStyle;     // list or report?
    static bool ms_lastShowHidden;    // did we show hidden files?
};

#ifdef USE_GENERIC_FILEDIALOG

class WXDLLEXPORT wxFileDialog: public wxGenericFileDialog
{
     DECLARE_DYNAMIC_CLASS(wxFileDialog)

public:
     wxFileDialog() {}

    wxFileDialog(wxWindow *parent,
                 const wxString& message = wxFileSelectorPromptStr,
                 const wxString& defaultDir = _T(""),
                 const wxString& defaultFile = _T(""),
                 const wxString& wildCard = wxFileSelectorDefaultWildcardStr,
                 long style = 0,
                 const wxPoint& pos = wxDefaultPosition)
          :wxGenericFileDialog(parent, message, defaultDir, defaultFile, wildCard, style, pos)
     {
     }
};

// File selector - backward compatibility
WXDLLEXPORT wxString
wxFileSelector(const wxChar *message = wxFileSelectorPromptStr,
               const wxChar *default_path = NULL,
               const wxChar *default_filename = NULL,
               const wxChar *default_extension = NULL,
               const wxChar *wildcard = wxFileSelectorDefaultWildcardStr,
               int flags = 0,
               wxWindow *parent = NULL,
               int x = -1, int y = -1);

// An extended version of wxFileSelector
WXDLLEXPORT wxString
wxFileSelectorEx(const wxChar *message = wxFileSelectorPromptStr,
                 const wxChar *default_path = NULL,
                 const wxChar *default_filename = NULL,
                 int *indexDefaultExtension = NULL,
                 const wxChar *wildcard = wxFileSelectorDefaultWildcardStr,
                 int flags = 0,
                 wxWindow *parent = NULL,
                 int x = -1, int y = -1);

// Ask for filename to load
WXDLLEXPORT wxString
wxLoadFileSelector(const wxChar *what,
                   const wxChar *extension,
                   const wxChar *default_name = (const wxChar *)NULL,
                   wxWindow *parent = (wxWindow *) NULL);

// Ask for filename to save
WXDLLEXPORT wxString
wxSaveFileSelector(const wxChar *what,
                   const wxChar *extension,
                   const wxChar *default_name = (const wxChar *) NULL,
                   wxWindow *parent = (wxWindow *) NULL);

#endif // USE_GENERIC_FILEDIALOG

//-----------------------------------------------------------------------------
//  wxFileData
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxFileData
{
public:
    enum fileType
    {
        is_file  = 0,
        is_dir   = 0x0001,
        is_link  = 0x0002,
        is_exe   = 0x0004,
        is_drive = 0x0008
    };

    wxFileData( const wxString &name, const wxString &fname, fileType type );
    wxString GetName() const;
    wxString GetFullName() const;
    wxString GetHint() const;

    // the wxFileCtrl fields in report view
    enum fileListFieldType
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

    wxString GetEntry( fileListFieldType num ) const;

    bool IsDir() const;
    bool IsLink() const;
    bool IsExe() const;
    bool IsDrive() const;
    long GetSize() const { return m_size; }
    int GetHour() const { return m_hour; }
    int GetMinute() const { return m_minute; }
    int GetYear() const { return m_year; }
    int GetMonth() const { return m_month; }
    int GetDay() const { return m_day; }

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
    int      m_type;
};

//-----------------------------------------------------------------------------
//  wxFileCtrl
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxFileCtrl : public wxListCtrl
{
public:
    wxFileCtrl();
    wxFileCtrl( wxWindow *win,
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
    bool GetShowHidden() const { return m_showHidden; }

    long Add( wxFileData *fd, wxListItem &item );
    void UpdateFiles();
    virtual void StatusbarText( wxChar *WXUNUSED(text) ) {};
    void MakeDir();
    void GoToParentDir();
    void GoToHomeDir();
    void GoToDir( const wxString &dir );
    void SetWild( const wxString &wild );
    wxString GetWild() const { return m_wild; }
    void GetDir( wxString &dir );
    wxString GetDir() const { return m_dirName; }

    void OnListDeleteItem( wxListEvent &event );
    void OnListEndLabelEdit( wxListEvent &event );
    void OnListColClick( wxListEvent &event );

    // Associate commonly used UI controls with wxFileCtrl so that they can be
    // disabled when they cannot be used (e.g. can't go to parent directory
    // if wxFileCtrl already is in the root dir):
    void SetGoToParentControl(wxWindow *ctrl) { m_goToParentControl = ctrl; }
    void SetNewDirControl(wxWindow *ctrl) { m_newDirControl = ctrl; }

    void SortItems(wxFileData::fileListFieldType field, bool foward);
    bool GetSortDirection() const { return m_sort_foward > 0; }
    wxFileData::fileListFieldType GetSortField() const { return m_sort_field; }

private:
    void FreeItemData(const wxListItem& item);
    void FreeAllItemsData();

    wxString      m_dirName;
    bool          m_showHidden;
    wxString      m_wild;

    wxWindow     *m_goToParentControl;
    wxWindow     *m_newDirControl;

    int m_sort_foward;
    wxFileData::fileListFieldType m_sort_field;

    DECLARE_DYNAMIC_CLASS(wxFileCtrl);
    DECLARE_EVENT_TABLE()
};

#endif
    // _WX_FILEDLGG_H_


