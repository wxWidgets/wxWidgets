/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/notebook.cpp
// Author:      Mariano Reingart, Peter Most
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/notebook.h"
#include "wx/qt/private/utils.h"
#include "wx/qt/private/converter.h"
#include "wx/qt/private/winevent.h"

#include <QtWidgets/QTabWidget>

class wxQtTabWidget : public wxQtEventSignalHandler< QTabWidget, wxNotebook >
{
public:
    wxQtTabWidget( wxWindow *parent, wxNotebook *handler );
    void currentChanged(int index);
};

wxQtTabWidget::wxQtTabWidget( wxWindow *parent, wxNotebook *handler )
    : wxQtEventSignalHandler< QTabWidget, wxNotebook >( parent, handler )
{
    connect(this, &QTabWidget::currentChanged, this, &wxQtTabWidget::currentChanged);
}

void wxQtTabWidget::currentChanged(int index)
{
    wxNotebook *handler = GetHandler();
    if ( handler )
    {
        int old = handler->GetSelection();
        // revert change be able to simulate veto (select back the old tab):
        if (old >= 0)
        {
            handler->ChangeSelection(old);
        }
        // send the wx event and check if accepted (and then show the new tab):
        if (handler->SendPageChangingEvent(index))
        {
            // not vetoed, send the event and store new index
            handler->ChangeSelection(index);
            handler->SendPageChangedEvent(old, index);
        }
    }
}


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
    m_qtTabWidget = new wxQtTabWidget( parent, this );

    return QtCreateControl( parent, id, pos, size, style, wxDefaultValidator, name );
}

void wxNotebook::SetPadding(const wxSize& WXUNUSED(padding))
{
}

void wxNotebook::SetTabSize(const wxSize& WXUNUSED(sz))
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
    wxCHECK_MSG(n < GetPageCount(), wxNOT_FOUND, "invalid notebook index");

    return m_images[n];

}

bool wxNotebook::SetPageImage(size_t n, int imageId)
{
    wxCHECK_MSG(n < GetPageCount(), false, "invalid notebook index");

    if (imageId >= 0)
    {
        wxCHECK_MSG(HasImageList(), false, "invalid notebook imagelist");
        const wxBitmap bitmap = GetImageList()->GetBitmap(imageId);
        // set the new image:
        m_qtTabWidget->setTabIcon( n, QIcon( *bitmap.GetHandle() ));
    }
    else
    {
        // remove the image using and empty qt icon:
        m_qtTabWidget->setTabIcon( n, QIcon() );
    }
    m_images[n] = imageId;
    return true;
}

bool wxNotebook::InsertPage(size_t n, wxWindow *page, const wxString& text,
    bool bSelect, int imageId)
{
    // disable firing qt signals until wx structures are filled
    m_qtTabWidget->blockSignals(true);

    if (imageId != -1)
    {
        if (HasImageList())
        {
            const wxBitmap bitmap = GetImageList()->GetBitmap(imageId);
            m_qtTabWidget->insertTab( n, page->GetHandle(), QIcon( *bitmap.GetHandle() ), wxQtConvertString( text ));
        }
        else
        {
            wxFAIL_MSG("invalid notebook imagelist");
        }
    }
    else
    {
        m_qtTabWidget->insertTab( n, page->GetHandle(), wxQtConvertString( text ));
    }

    m_pages.insert(m_pages.begin() + n, page);
    m_images.insert(m_images.begin() + n, imageId);

    // reenable firing qt signals as internal wx initialization was completed
    m_qtTabWidget->blockSignals(false);

    if (bSelect && GetPageCount() > 1)
    {
        SetSelection( n );
    }

    return true;
}

wxSize wxNotebook::CalcSizeFromPage(const wxSize& sizePage) const
{
    return sizePage;
}

int wxNotebook::DoSetSelection(size_t page, int flags)
{
    wxCHECK_MSG(page < GetPageCount(), wxNOT_FOUND, "invalid notebook index");

    int selOld = GetSelection();

    // do not fire signals for certain methods (i.e. ChangeSelection
    if ( !(flags & SetSelection_SendEvent) )
    {
        m_qtTabWidget->blockSignals(true);
    }
    // change the QTabWidget selected page:
    m_selection = page;
    m_qtTabWidget->setCurrentIndex( page );
    if ( !(flags & SetSelection_SendEvent) )
    {
        m_qtTabWidget->blockSignals(false);
    }
    return selOld;
}


wxWindow *wxNotebook::DoRemovePage(size_t page)
{
    QWidget *qtWidget = m_qtTabWidget->widget( page );
    m_qtTabWidget->removeTab( page );
    wxNotebookBase::DoRemovePage(page);
    m_images.erase( m_images.begin() + page );

    return QtRetrieveWindowPointer( qtWidget );
}

QWidget *wxNotebook::GetHandle() const
{
    return m_qtTabWidget;
}
