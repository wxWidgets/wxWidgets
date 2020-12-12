/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/choice.h
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_CHOICE_H_
#define _WX_QT_CHOICE_H_

class QComboBox;

class WXDLLIMPEXP_CORE wxChoice : public wxChoiceBase
{
public:
    wxChoice();

    wxChoice( wxWindow *parent, wxWindowID id,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            int n = 0, const wxString choices[] = (const wxString *) NULL,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxASCII_STR(wxChoiceNameStr) );

    wxChoice( wxWindow *parent, wxWindowID id,
            const wxPoint& pos,
            const wxSize& size,
            const wxArrayString& choices,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxASCII_STR(wxChoiceNameStr) );

    bool Create( wxWindow *parent, wxWindowID id,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            int n = 0, const wxString choices[] = NULL,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxASCII_STR(wxChoiceNameStr) );

    bool Create( wxWindow *parent, wxWindowID id,
            const wxPoint& pos,
            const wxSize& size,
            const wxArrayString& choices,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxASCII_STR(wxChoiceNameStr) );

    virtual wxSize DoGetBestSize() const wxOVERRIDE;

    virtual unsigned int GetCount() const wxOVERRIDE;
    virtual wxString GetString(unsigned int n) const wxOVERRIDE;
    virtual void SetString(unsigned int n, const wxString& s) wxOVERRIDE;

    virtual void SetSelection(int n) wxOVERRIDE;
    virtual int GetSelection() const wxOVERRIDE;

    virtual QWidget *GetHandle() const wxOVERRIDE;

protected:
    virtual int DoInsertItems(const wxArrayStringsAdapter & items,
                              unsigned int pos,
                              void **clientData,
                              wxClientDataType type) wxOVERRIDE;
    virtual int DoInsertOneItem(const wxString& item, unsigned int pos) wxOVERRIDE;

    virtual void DoSetItemClientData(unsigned int n, void *clientData) wxOVERRIDE;
    virtual void *DoGetItemClientData(unsigned int n) const wxOVERRIDE;

    virtual void DoClear() wxOVERRIDE;
    virtual void DoDeleteOneItem(unsigned int pos) wxOVERRIDE;

    void QtInitSort(QComboBox *combo);

    QComboBox *m_qtComboBox;

private:

    wxDECLARE_DYNAMIC_CLASS(wxChoice);
};

#endif // _WX_QT_CHOICE_H_
