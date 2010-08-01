/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/notebook.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/notebook.h"

wxNotebook::wxNotebook()
{
}

wxNotebook::wxNotebook(wxWindow *parent,
         wxWindowID id,
         const wxPoint& pos,
         const wxSize& size,
         long style,
         const wxString& name)
{
    Create( parent, id, pos, size, style, name );
}

wxNotebook::~wxNotebook()
{
    delete m_qtTabWidget;
}

bool wxNotebook::Create(wxWindow *parent,
          wxWindowID id,
          const wxPoint& pos,
          const wxSize& size,
          long style,
          const wxString& name)
{
    m_qtTabWidget = new QTabWidget( parent->GetHandle() );

    return wxNotebookBase::Create( parent, id, pos, size, style, name );
}

void wxNotebook::SetPadding(const wxSize& padding)
{
}

void wxNotebook::SetTabSize(const wxSize& sz)
{
}

int wxNotebook::GetSelection() const
{
    return 0;
}

bool wxNotebook::SetPageText(size_t n, const wxString& strText)
{
    return false;
}

wxString wxNotebook::GetPageText(size_t n) const
{
    return wxString();
}

int wxNotebook::GetPageImage(size_t n) const
{
    return 0;
}

bool wxNotebook::SetPageImage(size_t n, int imageId)
{
    return false;
}

bool wxNotebook::InsertPage(size_t n, wxWindow *page, const wxString& text,
    bool bSelect, int imageId)
{
    return false;
}

wxSize wxNotebook::CalcSizeFromPage(const wxSize& sizePage) const
{
    return wxSize();
}

int wxNotebook::SetSelection(size_t n)
{
    return 0;
}

int wxNotebook::ChangeSelection(size_t n)
{
    return 0;
}

wxWindow *wxNotebook::DoRemovePage(size_t page)
{
    return NULL;
}

QTabWidget *wxNotebook::GetHandle() const
{
    return m_qtTabWidget;
}
