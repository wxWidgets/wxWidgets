/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/notebook.h
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_NOTEBOOK_H_
#define _WX_QT_NOTEBOOK_H_

class QTabWidget;

class WXDLLIMPEXP_CORE wxNotebook : public wxNotebookBase
{
public:
    wxNotebook();
    wxNotebook(wxWindow *parent,
             wxWindowID id,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxString& name = wxNotebookNameStr);
    
    bool Create(wxWindow *parent,
              wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = 0,
              const wxString& name = wxNotebookNameStr);

    virtual void SetPadding(const wxSize& padding);
    virtual void SetTabSize(const wxSize& sz);

    virtual bool SetPageText(size_t n, const wxString& strText);
    virtual wxString GetPageText(size_t n) const;

    virtual int GetPageImage(size_t n) const;
    virtual bool SetPageImage(size_t n, int imageId);

    virtual bool InsertPage(size_t n, wxWindow *page, const wxString& text,
        bool bSelect = false, int imageId = -1);

    virtual wxSize CalcSizeFromPage(const wxSize& sizePage) const;

    int SetSelection(size_t nPage) { return DoSetSelection(nPage, SetSelection_SendEvent); }
    int ChangeSelection(size_t nPage) { return DoSetSelection(nPage); }

    virtual QWidget *GetHandle() const;

protected:
    virtual wxWindow *DoRemovePage(size_t page);
    int DoSetSelection(size_t nPage, int flags = 0);

private:
    QTabWidget *m_qtTabWidget;

    // internal array to store imageId for each page:
    wxVector<int> m_images;

    wxDECLARE_DYNAMIC_CLASS( wxNotebook );
};


#endif // _WX_QT_NOTEBOOK_H_
