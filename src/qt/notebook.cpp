/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/notebook.cpp
// Author:      Mariano Reingart, Peter Most
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_NOTEBOOK

#include "wx/notebook.h"
#include "wx/qt/private/utils.h"
#include "wx/qt/private/converter.h"
#include "wx/qt/private/winevent.h"

#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTabBar>

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
    m_qtWindow = new wxQtTabWidget( parent, this );

    if ( !wxControl::Create( parent, id, pos, size, style, wxDefaultValidator, name ) )
        return false;

    if ( m_windowStyle & wxBK_RIGHT )
        GetQTabWidget()->setTabPosition( QTabWidget::East );
    else if ( m_windowStyle & wxBK_LEFT )
        GetQTabWidget()->setTabPosition( QTabWidget::West );
    else if ( m_windowStyle & wxBK_BOTTOM )
        GetQTabWidget()->setTabPosition( QTabWidget::South );

    return true;
}

QTabWidget* wxNotebook::GetQTabWidget() const
{
    return static_cast<QTabWidget*>(m_qtWindow);
}

void wxNotebook::SetPadding(const wxSize& WXUNUSED(padding))
{
}

void wxNotebook::SetTabSize(const wxSize& WXUNUSED(sz))
{
}


bool wxNotebook::SetPageText(size_t n, const wxString &text)
{
    GetQTabWidget()->setTabText( n, wxQtConvertString( text ));

    return true;
}

wxString wxNotebook::GetPageText(size_t n) const
{
    return wxQtConvertString( GetQTabWidget()->tabText( n ));
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
        GetQTabWidget()->setTabIcon( n, QIcon( *bitmap.GetHandle() ));
    }
    else
    {
        // remove the image using and empty qt icon:
        GetQTabWidget()->setTabIcon( n, QIcon() );
    }
    m_images[n] = imageId;
    return true;
}

void wxNotebook::OnImagesChanged()
{
    if ( HasImages() )
    {
        wxImageList* const imageList = GetUpdatedImageListFor(this);

        const wxBitmap bitmap = imageList->GetBitmap(0);
        GetQTabWidget()->setIconSize(wxQtConvertSize(bitmap.GetLogicalSize()));
        GetQTabWidget()->update();
    }
}

bool wxNotebook::InsertPage(size_t n, wxWindow *page, const wxString& text,
    bool bSelect, int imageId)
{
    // disable firing qt signals until wx structures are filled
    GetQTabWidget()->blockSignals(true);

    if (imageId != -1)
    {
        if (HasImageList())
        {
            const wxBitmap bitmap = GetImageList()->GetBitmap(imageId);
            GetQTabWidget()->insertTab( n, page->GetHandle(), QIcon( *bitmap.GetHandle() ), wxQtConvertString( text ));
        }
        else
        {
            wxFAIL_MSG("invalid notebook imagelist");
        }
    }
    else
    {
        GetQTabWidget()->insertTab( n, page->GetHandle(), wxQtConvertString( text ));
    }

    m_pages.insert(m_pages.begin() + n, page);
    m_images.insert(m_images.begin() + n, imageId);

    // reenable firing qt signals as internal wx initialization was completed
    GetQTabWidget()->blockSignals(false);

    DoSetSelectionAfterInsertion(n, bSelect);

    return true;
}

wxSize wxNotebook::CalcSizeFromPage(const wxSize& sizePage) const
{
    QTabBar *tabBar = GetQTabWidget()->tabBar();
    const QSize &tabBarSize = tabBar->size();
    return wxSize(sizePage.GetWidth(),
        sizePage.GetHeight() + tabBarSize.height());
}

bool wxNotebook::DeleteAllPages()
{
    // Nothing to do if the notebook was not created yet,
    // and return true just like other ports do.
    if ( !GetQTabWidget() )
        return true;

    // Block signals to not receive selection changed updates
    // which are sent by Qt after the selected page was deleted.
    wxQtEnsureSignalsBlocked blocker(GetQTabWidget());

    // Pages will be deleted one by one in the base class.
    // There's no need to explicitly clear() the Qt control.
    return wxNotebookBase::DeleteAllPages();
}

int wxNotebook::SetSelection(size_t page)
{
    wxCHECK_MSG(page < GetPageCount(), wxNOT_FOUND, "invalid notebook index");

    int selOld = GetSelection();

    // change the QTabWidget selected page:
    GetQTabWidget()->setCurrentIndex( page );
    m_selection = page;

    return selOld;
}

int wxNotebook::ChangeSelection(size_t nPage)
{
    // ChangeSelection() is not supposed to generate events, unlike
    // SetSelection().
    wxQtEnsureSignalsBlocked blocker(GetQTabWidget());

    return SetSelection(nPage);
}

wxWindow *wxNotebook::DoRemovePage(size_t page)
{
    QWidget *qtWidget = GetQTabWidget()->widget( page );
    GetQTabWidget()->removeTab( page );
    wxNotebookBase::DoRemovePage(page);
    m_images.erase( m_images.begin() + page );

    return QtRetrieveWindowPointer( qtWidget );
}

#endif // wxUSE_NOTEBOOK
