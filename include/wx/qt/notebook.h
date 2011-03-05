/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/notebook.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_NOTEBOOK_H_
#define _WX_QT_NOTEBOOK_H_

#include "wx/qt/pointer_qt.h"
#include <QtGui/QTabWidget>

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

    virtual int GetSelection() const;

    virtual bool SetPageText(size_t n, const wxString& strText);
    virtual wxString GetPageText(size_t n) const;

    virtual int GetPageImage(size_t n) const;
    virtual bool SetPageImage(size_t n, int imageId);

    virtual bool InsertPage(size_t n, wxWindow *page, const wxString& text,
        bool bSelect = false, int imageId = -1);

    virtual wxSize CalcSizeFromPage(const wxSize& sizePage) const;

    virtual int SetSelection(size_t n);
    virtual int ChangeSelection(size_t n);

    virtual QTabWidget *GetHandle() const;

protected:
    virtual wxWindow *DoRemovePage(size_t page);

private:
    wxQtPointer< QTabWidget > m_qtTabWidget;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY( wxNotebook );
};

#endif // _WX_QT_NOTEBOOK_H_
