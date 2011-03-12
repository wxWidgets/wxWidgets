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
#include "wx/qt/utils.h"
#include "wx/qt/converter.h"

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

bool wxNotebook::Create(wxWindow *parent,
          wxWindowID id,
          const wxPoint& pos,
          const wxSize& size,
          long style,
          const wxString& name)
{
    m_qtTabWidget = new QTabWidget( parent->GetHandle() );

    return QtCreateControl( parent, id, pos, size, style, wxDefaultValidator, name );
}

void wxNotebook::SetPadding(const wxSize& padding)
{
}

void wxNotebook::SetTabSize(const wxSize& sz)
{
}


bool wxNotebook::SetPageText(size_t n, const wxString &text)
{
    m_qtTabWidget->setTabText( n, wxQtConvertString( text ));

    return true;
}

wxString wxNotebook::GetPageText(size_t n) const
{
    return wxQtConvertString( m_qtTabWidget->tabText( n ));
}

int wxNotebook::GetPageImage(size_t n) const
{
    wxMISSING_FUNCTION();

    return 0;
}

bool wxNotebook::SetPageImage(size_t n, int imageId)
{
    wxMISSING_FUNCTION();
    return false;
}

bool wxNotebook::InsertPage(size_t n, wxWindow *page, const wxString& text,
    bool bSelect, int imageId)
{
    m_qtTabWidget->insertTab( n, page->GetHandle(), wxQtConvertString( text ));
    m_qtTabWidget->setTabEnabled( n, bSelect );

//    AddChild( page );

    return true;
}

wxSize wxNotebook::CalcSizeFromPage(const wxSize& sizePage) const
{
    return wxSize();
}

int wxNotebook::SetSelection(size_t n)
{
    int currentSelection = GetSelection();

    m_qtTabWidget->setCurrentIndex( n );

    return currentSelection;
}

int wxNotebook::GetSelection() const
{
    return m_qtTabWidget->currentIndex();
}

int wxNotebook::ChangeSelection(size_t n)
{
    wxMISSING_FUNCTION();

    return SetSelection( n );
}

wxWindow *wxNotebook::DoRemovePage(size_t page)
{
    QWidget *qtWidget = m_qtTabWidget->widget( page );
    m_qtTabWidget->removeTab( page );

    return QtRetrieveWindowPointer( qtWidget );
}

QTabWidget *wxNotebook::GetHandle() const
{
    return m_qtTabWidget;
}
