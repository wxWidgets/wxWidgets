/////////////////////////////////////////////////////////////////////////////
// Name:        wx/palmos/fdrepdlg.h
// Purpose:     wxFindReplaceDialog class
// Author:      William Osborne
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id: 
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "mswfdrepdlg.h"
#endif

// ----------------------------------------------------------------------------
// wxFindReplaceDialog: dialog for searching / replacing text
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxFindReplaceDialog : public wxFindReplaceDialogBase
{
public:
    // ctors and such
    wxFindReplaceDialog() { Init(); }
    wxFindReplaceDialog(wxWindow *parent,
                        wxFindReplaceData *data,
                        const wxString &title,
                        int style = 0);

    bool Create(wxWindow *parent,
                wxFindReplaceData *data,
                const wxString &title,
                int style = 0);

    virtual ~wxFindReplaceDialog();

    // implementation only from now on

    wxFindReplaceDialogImpl *GetImpl() const { return m_impl; }

    // override some base class virtuals
    virtual bool Show(bool show = TRUE);
    virtual void SetTitle( const wxString& title);
    virtual wxString GetTitle() const;

protected:
    virtual void DoGetSize(int *width, int *height) const;
    virtual void DoGetClientSize(int *width, int *height) const;
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);

    void Init();

    wxString                m_title;

    wxFindReplaceDialogImpl *m_impl;

    DECLARE_DYNAMIC_CLASS(wxFindReplaceDialog)
    DECLARE_NO_COPY_CLASS(wxFindReplaceDialog)
};


