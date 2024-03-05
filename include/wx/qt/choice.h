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
            int n = 0, const wxString choices[] = nullptr,
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
            int n = 0, const wxString choices[] = nullptr,
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

    virtual wxSize DoGetBestSize() const override;

    virtual unsigned int GetCount() const override;
    virtual wxString GetString(unsigned int n) const override;
    virtual void SetString(unsigned int n, const wxString& s) override;

    virtual void SetSelection(int n) override;
    virtual int GetSelection() const override;

    virtual QWidget *GetHandle() const override;

protected:
    virtual int DoInsertItems(const wxArrayStringsAdapter & items,
                              unsigned int pos,
                              void **clientData,
                              wxClientDataType type) override;
    virtual int DoInsertOneItem(const wxString& item, unsigned int pos) override;

    virtual void DoSetItemClientData(unsigned int n, void *clientData) override;
    virtual void *DoGetItemClientData(unsigned int n) const override;

    virtual void DoClear() override;
    virtual void DoDeleteOneItem(unsigned int pos) override;

    void QtInitSort(QComboBox *combo);

    QComboBox *m_qtComboBox;

private:

    wxDECLARE_DYNAMIC_CLASS(wxChoice);
};

#endif // _WX_QT_CHOICE_H_
