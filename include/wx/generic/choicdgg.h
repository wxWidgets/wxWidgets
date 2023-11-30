/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/choicdgg.h
// Purpose:     Generic choice dialogs
// Author:      Julian Smart
// Modified by: 03.11.00: VZ to add wxArrayString and multiple sel functions
// Created:     01/02/97
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_CHOICDGG_H_
#define _WX_GENERIC_CHOICDGG_H_

#include "wx/dynarray.h"
#include "wx/dialog.h"

class WXDLLIMPEXP_FWD_CORE wxListBoxBase;

// ----------------------------------------------------------------------------
// some constants
// ----------------------------------------------------------------------------

// These constants are no longer used and only remain here for compatibility.
#define wxCHOICE_HEIGHT 150
#define wxCHOICE_WIDTH 200

// Default style for choice dialogs.
#define wxCHOICEDLG_STYLE \
    (wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxOK | wxCANCEL | wxCENTRE)

// ----------------------------------------------------------------------------
// wxAnyChoiceDialog: a base class for dialogs containing a listbox
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxAnyChoiceDialog : public wxDialog
{
public:
    wxAnyChoiceDialog() : m_listbox(nullptr) { }

    wxAnyChoiceDialog(wxWindow *parent,
                      const wxString& message,
                      const wxString& caption,
                      int n, const wxString *choices,
                      long styleDlg = wxCHOICEDLG_STYLE,
                      const wxPoint& pos = wxDefaultPosition,
                      long styleLbox = wxLB_ALWAYS_SB)
    {
        (void)Create(parent, message, caption, n, choices,
                     styleDlg, pos, styleLbox);
    }
    wxAnyChoiceDialog(wxWindow *parent,
                      const wxString& message,
                      const wxString& caption,
                      const wxArrayString& choices,
                      long styleDlg = wxCHOICEDLG_STYLE,
                      const wxPoint& pos = wxDefaultPosition,
                      long styleLbox = wxLB_ALWAYS_SB)
    {
        (void)Create(parent, message, caption, choices,
                     styleDlg, pos, styleLbox);
    }

    bool Create(wxWindow *parent,
                const wxString& message,
                const wxString& caption,
                int n, const wxString *choices,
                long styleDlg = wxCHOICEDLG_STYLE,
                const wxPoint& pos = wxDefaultPosition,
                long styleLbox = wxLB_ALWAYS_SB);
    bool Create(wxWindow *parent,
                const wxString& message,
                const wxString& caption,
                const wxArrayString& choices,
                long styleDlg = wxCHOICEDLG_STYLE,
                const wxPoint& pos = wxDefaultPosition,
                long styleLbox = wxLB_ALWAYS_SB);

protected:
    wxListBoxBase *m_listbox;

    virtual wxListBoxBase *CreateList(int n,
                                      const wxString *choices,
                                      long styleLbox);

    wxDECLARE_NO_COPY_CLASS(wxAnyChoiceDialog);
};

// ----------------------------------------------------------------------------
// wxSingleChoiceDialog: a dialog with single selection listbox
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxSingleChoiceDialog : public wxAnyChoiceDialog
{
public:
    wxSingleChoiceDialog()
    {
        m_selection = -1;
    }

    wxSingleChoiceDialog(wxWindow *parent,
                         const wxString& message,
                         const wxString& caption,
                         int n,
                         const wxString *choices,
                         void **clientData = nullptr,
                         long style = wxCHOICEDLG_STYLE,
                         const wxPoint& pos = wxDefaultPosition)
    {
        Create(parent, message, caption, n, choices, clientData, style, pos);
    }

    wxSingleChoiceDialog(wxWindow *parent,
                         const wxString& message,
                         const wxString& caption,
                         const wxArrayString& choices,
                         void **clientData = nullptr,
                         long style = wxCHOICEDLG_STYLE,
                         const wxPoint& pos = wxDefaultPosition)
    {
        Create(parent, message, caption, choices, clientData, style, pos);
    }

    bool Create(wxWindow *parent,
                const wxString& message,
                const wxString& caption,
                int n,
                const wxString *choices,
                void **clientData = nullptr,
                long style = wxCHOICEDLG_STYLE,
                const wxPoint& pos = wxDefaultPosition);
    bool Create(wxWindow *parent,
                const wxString& message,
                const wxString& caption,
                const wxArrayString& choices,
                void **clientData = nullptr,
                long style = wxCHOICEDLG_STYLE,
                const wxPoint& pos = wxDefaultPosition);

    void SetSelection(int sel);
    int GetSelection() const { return m_selection; }
    wxString GetStringSelection() const { return m_stringSelection; }
    void* GetSelectionData() const { return m_clientData; }

    // implementation from now on
    void OnOK(wxCommandEvent& event);
    void OnListBoxDClick(wxCommandEvent& event);

protected:
    int         m_selection;
    wxString    m_stringSelection;

    void DoChoice();

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxSingleChoiceDialog);
    wxDECLARE_EVENT_TABLE();
};

// ----------------------------------------------------------------------------
// wxMultiChoiceDialog: a dialog with multi selection listbox
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxMultiChoiceDialog : public wxAnyChoiceDialog
{
public:
    wxMultiChoiceDialog() = default;

    wxMultiChoiceDialog(wxWindow *parent,
                        const wxString& message,
                        const wxString& caption,
                        int n,
                        const wxString *choices,
                        long style = wxCHOICEDLG_STYLE,
                        const wxPoint& pos = wxDefaultPosition)
    {
        (void)Create(parent, message, caption, n, choices, style, pos);
    }
    wxMultiChoiceDialog(wxWindow *parent,
                        const wxString& message,
                        const wxString& caption,
                        const wxArrayString& choices,
                        long style = wxCHOICEDLG_STYLE,
                        const wxPoint& pos = wxDefaultPosition)
    {
        (void)Create(parent, message, caption, choices, style, pos);
    }

    bool Create(wxWindow *parent,
                const wxString& message,
                const wxString& caption,
                int n,
                const wxString *choices,
                long style = wxCHOICEDLG_STYLE,
                const wxPoint& pos = wxDefaultPosition);
    bool Create(wxWindow *parent,
                const wxString& message,
                const wxString& caption,
                const wxArrayString& choices,
                long style = wxCHOICEDLG_STYLE,
                const wxPoint& pos = wxDefaultPosition);

    void SetSelections(const wxArrayInt& selections);
    wxArrayInt GetSelections() const { return m_selections; }

    // implementation from now on
    virtual bool TransferDataFromWindow() override;

protected:
#if wxUSE_CHECKLISTBOX
    virtual wxListBoxBase *CreateList(int n,
                                      const wxString *choices,
                                      long styleLbox) override;
#endif // wxUSE_CHECKLISTBOX

    wxArrayInt m_selections;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxMultiChoiceDialog);
};

// ----------------------------------------------------------------------------
// wrapper functions which can be used to get selection(s) from the user
// ----------------------------------------------------------------------------

// get the user selection as a string
WXDLLIMPEXP_CORE wxString wxGetSingleChoice(const wxString& message,
                                       const wxString& caption,
                                       const wxArrayString& choices,
                                       wxWindow *parent = nullptr,
                                       int x = wxDefaultCoord,
                                       int y = wxDefaultCoord,
                                       bool centre = true,
                                       int width = wxCHOICE_WIDTH,
                                       int height = wxCHOICE_HEIGHT,
                                       int initialSelection = 0);

WXDLLIMPEXP_CORE wxString wxGetSingleChoice(const wxString& message,
                                       const wxString& caption,
                                       int n, const wxString *choices,
                                       wxWindow *parent = nullptr,
                                       int x = wxDefaultCoord,
                                       int y = wxDefaultCoord,
                                       bool centre = true,
                                       int width = wxCHOICE_WIDTH,
                                       int height = wxCHOICE_HEIGHT,
                                       int initialSelection = 0);

WXDLLIMPEXP_CORE wxString wxGetSingleChoice(const wxString& message,
                                            const wxString& caption,
                                            const wxArrayString& choices,
                                            int initialSelection,
                                            wxWindow *parent = nullptr);

WXDLLIMPEXP_CORE wxString wxGetSingleChoice(const wxString& message,
                                            const wxString& caption,
                                            int n, const wxString *choices,
                                            int initialSelection,
                                            wxWindow *parent = nullptr);

// Same as above but gets position in list of strings, instead of string,
// or -1 if no selection
WXDLLIMPEXP_CORE int wxGetSingleChoiceIndex(const wxString& message,
                                       const wxString& caption,
                                       const wxArrayString& choices,
                                       wxWindow *parent = nullptr,
                                       int x = wxDefaultCoord,
                                       int y = wxDefaultCoord,
                                       bool centre = true,
                                       int width = wxCHOICE_WIDTH,
                                       int height = wxCHOICE_HEIGHT,
                                       int initialSelection = 0);

WXDLLIMPEXP_CORE int wxGetSingleChoiceIndex(const wxString& message,
                                       const wxString& caption,
                                       int n, const wxString *choices,
                                       wxWindow *parent = nullptr,
                                       int x = wxDefaultCoord,
                                       int y = wxDefaultCoord,
                                       bool centre = true,
                                       int width = wxCHOICE_WIDTH,
                                       int height = wxCHOICE_HEIGHT,
                                       int initialSelection = 0);

WXDLLIMPEXP_CORE int wxGetSingleChoiceIndex(const wxString& message,
                                            const wxString& caption,
                                            const wxArrayString& choices,
                                            int initialSelection,
                                            wxWindow *parent = nullptr);

WXDLLIMPEXP_CORE int wxGetSingleChoiceIndex(const wxString& message,
                                            const wxString& caption,
                                            int n, const wxString *choices,
                                            int initialSelection,
                                            wxWindow *parent = nullptr);

// Return client data instead or nullptr if canceled
WXDLLIMPEXP_CORE void* wxGetSingleChoiceData(const wxString& message,
                                        const wxString& caption,
                                        const wxArrayString& choices,
                                        void **client_data,
                                        wxWindow *parent = nullptr,
                                        int x = wxDefaultCoord,
                                        int y = wxDefaultCoord,
                                        bool centre = true,
                                        int width = wxCHOICE_WIDTH,
                                        int height = wxCHOICE_HEIGHT,
                                        int initialSelection = 0);

WXDLLIMPEXP_CORE void* wxGetSingleChoiceData(const wxString& message,
                                        const wxString& caption,
                                        int n, const wxString *choices,
                                        void **client_data,
                                        wxWindow *parent = nullptr,
                                        int x = wxDefaultCoord,
                                        int y = wxDefaultCoord,
                                        bool centre = true,
                                        int width = wxCHOICE_WIDTH,
                                        int height = wxCHOICE_HEIGHT,
                                        int initialSelection = 0);

WXDLLIMPEXP_CORE void* wxGetSingleChoiceData(const wxString& message,
                                             const wxString& caption,
                                             const wxArrayString& choices,
                                             void **client_data,
                                             int initialSelection,
                                             wxWindow *parent = nullptr);


WXDLLIMPEXP_CORE void* wxGetSingleChoiceData(const wxString& message,
                                             const wxString& caption,
                                             int n, const wxString *choices,
                                             void **client_data,
                                             int initialSelection,
                                             wxWindow *parent = nullptr);

// fill the array with the indices of the chosen items, it will be empty
// if no items were selected or Cancel was pressed - return the number of
// selections or -1 if cancelled
WXDLLIMPEXP_CORE int wxGetSelectedChoices(wxArrayInt& selections,
                                        const wxString& message,
                                        const wxString& caption,
                                        int n, const wxString *choices,
                                        wxWindow *parent = nullptr,
                                        int x = wxDefaultCoord,
                                        int y = wxDefaultCoord,
                                        bool centre = true,
                                        int width = wxCHOICE_WIDTH,
                                        int height = wxCHOICE_HEIGHT);

WXDLLIMPEXP_CORE int wxGetSelectedChoices(wxArrayInt& selections,
                                        const wxString& message,
                                        const wxString& caption,
                                        const wxArrayString& choices,
                                        wxWindow *parent = nullptr,
                                        int x = wxDefaultCoord,
                                        int y = wxDefaultCoord,
                                        bool centre = true,
                                        int width = wxCHOICE_WIDTH,
                                        int height = wxCHOICE_HEIGHT);

#endif // _WX_GENERIC_CHOICDGG_H_
