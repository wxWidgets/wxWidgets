/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/notebook.cpp
// Purpose:
// Author:      Robert Roebling
// Copyright:   (c) 1998 Robert Roebling, Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_NOTEBOOK

#include "wx/notebook.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/utils.h"
    #include "wx/msgdlg.h"
    #include "wx/bitmap.h"
#endif

#include "wx/imaglist.h"
#include "wx/fontutil.h"

#include "wx/gtk/private.h"
#include "wx/gtk/private/image.h"
#include "wx/gtk/private/log.h"
#include "wx/gtk/private/stylecontext.h"

//-----------------------------------------------------------------------------
// wxGtkNotebookPage
//-----------------------------------------------------------------------------

// VZ: this is rather ugly as we keep the pages themselves in an array (it
//     allows us to have quite a few functions implemented in the base class)
//     but the page data is kept in a separate list, so we must maintain them
//     in sync manually... of course, the list had been there before the base
//     class which explains it but it still would be nice to do something
//     about this one day

class wxGtkNotebookPage
{
public:
    GtkWidget* m_box;
    GtkWidget* m_label;
    GtkWidget* m_image;
    int m_imageIndex;
};


extern "C" {
static void event_after(GtkNotebook*, GdkEvent*, wxNotebook*);
}

//-----------------------------------------------------------------------------
// "switch_page"
//-----------------------------------------------------------------------------

extern "C" {
static void
switch_page_after(GtkNotebook* widget, GtkNotebookPage*, guint, wxNotebook* win)
{
    g_signal_handlers_block_by_func(widget, (void*)switch_page_after, win);

    win->GTKOnPageChanged();
}
}

extern "C" {
static void
switch_page(GtkNotebook* widget, GtkNotebookPage*, int page, wxNotebook* win)
{
    win->m_oldSelection = gtk_notebook_get_current_page(widget);

    if (win->SendPageChangingEvent(page))
        // allow change, unblock handler for changed event
        g_signal_handlers_unblock_by_func(widget, (void*)switch_page_after, win);
    else
        // change vetoed, unblock handler to set selection back
        g_signal_handlers_unblock_by_func(widget, (void*)event_after, win);
}
}

//-----------------------------------------------------------------------------
// "event_after" from m_widget
//-----------------------------------------------------------------------------

extern "C" {
static void event_after(GtkNotebook* widget, GdkEvent*, wxNotebook* win)
{
    g_signal_handlers_block_by_func(widget, (void*)event_after, win);
    g_signal_handlers_block_by_func(widget, (void*)switch_page, win);

    // restore previous selection
    gtk_notebook_set_current_page(widget, win->m_oldSelection);

    g_signal_handlers_unblock_by_func(widget, (void*)switch_page, win);
}
}

//-----------------------------------------------------------------------------
// InsertChild callback for wxNotebook
//-----------------------------------------------------------------------------

void wxNotebook::AddChildGTK(wxWindowGTK* child)
{
    // Hack Alert! (Part I): This sets the notebook as the parent of the child
    // widget, and takes care of some details such as updating the state and
    // style of the child to reflect its new location.  We do this early
    // because without it GetBestSize (which is used to set the initial size
    // of controls if an explicit size is not given) will often report
    // incorrect sizes since the widget's style context is not fully known.
    // See https://github.com/wxWidgets/wxWidgets/issues/20825
    gtk_widget_set_parent(child->m_widget, m_widget);

    // NOTE: This should be considered a temporary workaround until we can
    // work out the details and implement delaying the setting of the initial
    // size of widgets until the size is really needed.
}

//-----------------------------------------------------------------------------
// wxNotebook
//-----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(wxNotebook, wxBookCtrlBase)
    EVT_NAVIGATION_KEY(wxNotebook::OnNavigationKey)
wxEND_EVENT_TABLE()

void wxNotebook::Init()
{
    m_padding = 0;
    m_oldSelection = -1;
    m_themeEnabled = true;
}

wxNotebook::wxNotebook()
{
    Init();
}

wxNotebook::wxNotebook( wxWindow *parent, wxWindowID id,
      const wxPoint& pos, const wxSize& size,
      long style, const wxString& name )
{
    Init();
    Create( parent, id, pos, size, style, name );
}

wxNotebook::~wxNotebook()
{
    // Ensure that we don't generate page changing events during the
    // destruction, this is unexpected and may reference the already (half)
    // destroyed parent window, for example. So make sure our switch_page
    // callback is not called from inside DeleteAllPages() by disconnecting all
    // the callbacks associated with this widget.
    GTKDisconnect(m_widget);

    DeleteAllPages();
}

bool wxNotebook::Create(wxWindow *parent, wxWindowID id,
                        const wxPoint& pos, const wxSize& size,
                        long style, const wxString& name )
{
    if ( (style & wxBK_ALIGN_MASK) == wxBK_DEFAULT )
        style |= wxBK_TOP;

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ))
    {
        wxFAIL_MSG( wxT("wxNoteBook creation failed") );
        return false;
    }


    m_widget = gtk_notebook_new();
    g_object_ref(m_widget);

    gtk_notebook_set_scrollable( GTK_NOTEBOOK(m_widget), 1 );

    g_signal_connect (m_widget, "switch_page",
                      G_CALLBACK(switch_page), this);

    g_signal_connect_after (m_widget, "switch_page",
                      G_CALLBACK(switch_page_after), this);
    g_signal_handlers_block_by_func(m_widget, (void*)switch_page_after, this);

    g_signal_connect(m_widget, "event_after", G_CALLBACK(event_after), this);
    g_signal_handlers_block_by_func(m_widget, (void*)event_after, this);

    m_parent->DoAddChild( this );

    if (m_windowStyle & wxBK_RIGHT)
        gtk_notebook_set_tab_pos( GTK_NOTEBOOK(m_widget), GTK_POS_RIGHT );
    if (m_windowStyle & wxBK_LEFT)
        gtk_notebook_set_tab_pos( GTK_NOTEBOOK(m_widget), GTK_POS_LEFT );
    if (m_windowStyle & wxBK_BOTTOM)
        gtk_notebook_set_tab_pos( GTK_NOTEBOOK(m_widget), GTK_POS_BOTTOM );

    PostCreation(size);

    return true;
}

int wxNotebook::GetSelection() const
{
    wxCHECK_MSG( m_widget != nullptr, wxNOT_FOUND, wxT("invalid notebook") );

    return gtk_notebook_get_current_page( GTK_NOTEBOOK(m_widget) );
}

wxString wxNotebook::GetPageText( size_t page ) const
{
    wxCHECK_MSG(page < GetPageCount(), wxEmptyString, "invalid notebook index");

    GtkLabel* label = GTK_LABEL(GetNotebookPage(page)->m_label);
    return wxString::FromUTF8(gtk_label_get_text(label));
}

int wxNotebook::GetPageImage( size_t page ) const
{
    wxCHECK_MSG(page < GetPageCount(), wxNOT_FOUND, "invalid notebook index");

    return GetNotebookPage(page)->m_imageIndex;
}

wxGtkNotebookPage* wxNotebook::GetNotebookPage( int page ) const
{
    return const_cast<wxGtkNotebookPage*>(&m_pagesData.at(page));
}

int wxNotebook::DoSetSelection( size_t page, int flags )
{
    wxCHECK_MSG(page < GetPageCount(), wxNOT_FOUND, "invalid notebook index");

    int selOld = GetSelection();

    if ( !(flags & SetSelection_SendEvent) )
    {
        g_signal_handlers_block_by_func(m_widget, (void*)switch_page, this);
    }

    gtk_notebook_set_current_page( GTK_NOTEBOOK(m_widget), page );

    if ( !(flags & SetSelection_SendEvent) )
    {
        g_signal_handlers_unblock_by_func(m_widget, (void*)switch_page, this);
    }

    m_selection = page;

    wxNotebookPage *client = GetPage(page);
    if ( client )
        client->SetFocus();

    return selOld;
}

void wxNotebook::GTKOnPageChanged()
{
    m_selection = gtk_notebook_get_current_page(GTK_NOTEBOOK(m_widget));

    SendPageChangedEvent(m_oldSelection);
}

bool wxNotebook::SetPageText( size_t page, const wxString &text )
{
    wxCHECK_MSG(page < GetPageCount(), false, "invalid notebook index");

    GtkLabel* label = GTK_LABEL(GetNotebookPage(page)->m_label);
    gtk_label_set_text(label, text.utf8_str());

    return true;
}

bool wxNotebook::SetPageImage( size_t page, int image )
{
    wxCHECK_MSG(page < GetPageCount(), false, "invalid notebook index");

    wxGtkNotebookPage* pageData = GetNotebookPage(page);

    const wxBitmapBundle bundle = GetBitmapBundle(image);
    if ( bundle.IsOk() )
    {
        if (pageData->m_image == nullptr)
        {
            pageData->m_image = wxGtkImage::New();
            gtk_widget_show(pageData->m_image);
            gtk_box_pack_start(GTK_BOX(pageData->m_box),
                pageData->m_image, false, false, m_padding);
        }
        WX_GTK_IMAGE(pageData->m_image)->Set(bundle);
    }
    else if (pageData->m_image)
    {
        gtk_container_remove(GTK_CONTAINER(pageData->m_box), pageData->m_image);
        pageData->m_image = nullptr;
    }
    pageData->m_imageIndex = image;

    return true;
}

wxSize wxNotebook::CalcSizeFromPage(const wxSize& sizePage) const
{
    // Compute the max size of the tab labels.
    wxSize sizeTabMax;
    const size_t pageCount = GetPageCount();
    for ( size_t n = 0; n < pageCount; n++ )
    {
        GtkRequisition req;
        gtk_widget_get_preferred_size(GetNotebookPage(n)->m_box, nullptr, &req);
        sizeTabMax.IncTo(wxSize(req.width, req.height));
    }

    wxSize sizeFull(sizePage);
#ifdef __WXGTK3__
    GtkBorder b;
    if (gtk_check_version(3,20,0) == nullptr)
    {
        wxGtkStyleContext sc;
        sc.Add(GTK_TYPE_NOTEBOOK, "notebook", "notebook", "frame", nullptr);
        gtk_style_context_get_border(sc, GTK_STATE_FLAG_NORMAL, &b);
        sizeFull.IncBy(b.left + b.right, b.top + b.bottom);

        sc.Add(G_TYPE_NONE, "header", IsVertical() ? "top" : "left", nullptr);
        sc.Add(G_TYPE_NONE, "tabs", nullptr);
        sc.Add(G_TYPE_NONE, "tab", nullptr);

        wxSize tabMin;
        gtk_style_context_get(sc, GTK_STATE_FLAG_NORMAL,
            "min-width", &tabMin.x, "min-height", &tabMin.y, nullptr);
        sizeTabMax.IncTo(tabMin);

        gtk_style_context_get_margin(sc, GTK_STATE_FLAG_NORMAL, &b);
        sizeTabMax.IncBy(b.left + b.right, b.top + b.bottom);
        gtk_style_context_get_border(sc, GTK_STATE_FLAG_NORMAL, &b);
        sizeTabMax.IncBy(b.left + b.right, b.top + b.bottom);
        gtk_style_context_get_padding(sc, GTK_STATE_FLAG_NORMAL, &b);
        sizeTabMax.IncBy(b.left + b.right, b.top + b.bottom);
    }
    else
    {
        // Size of tab labels may be reported as zero if we're called too early.
        // 17 is observed value for text-only label with default Adwaita theme.
        sizeTabMax.IncTo(wxSize(17, 17));

        GtkStyleContext* sc = gtk_widget_get_style_context(m_widget);
        gtk_style_context_save(sc);

        wxGCC_WARNING_SUPPRESS(deprecated-declarations)
        gtk_style_context_add_region(sc, "tab", GtkRegionFlags(0));
        wxGCC_WARNING_RESTORE(deprecated-declarations)

        gtk_style_context_add_class(sc, "top");
        gtk_style_context_get_padding(sc, GTK_STATE_FLAG_NORMAL, &b);
        sizeTabMax.IncBy(b.left + b.right, b.top + b.bottom);
        gtk_style_context_restore(sc);

        sizeFull.IncBy(2 * gtk_container_get_border_width(GTK_CONTAINER(m_widget)));
    }
#else // !__WXGTK3__
    // Size of tab labels may be reported as zero if we're called too early.
    // 21 is observed value for text-only label with default Adwaita theme.
    sizeTabMax.IncTo(wxSize(21, 21));

    int tab_hborder, tab_vborder, focus_width;
    g_object_get(G_OBJECT(m_widget),
        "tab-hborder", &tab_hborder, "tab-vborder", &tab_vborder, nullptr);
    gtk_widget_style_get(m_widget, "focus-line-width", &focus_width, nullptr);
    sizeTabMax.x += 2 * (tab_hborder + focus_width + m_widget->style->xthickness);
    sizeTabMax.y += 2 * (tab_vborder + focus_width + m_widget->style->ythickness);

    sizeFull.x += 2 * m_widget->style->xthickness;
    sizeFull.y += 2 * m_widget->style->ythickness;
#endif // !__WXGTK3__

    if ( IsVertical() )
        sizeFull.y += sizeTabMax.y;
    else
        sizeFull.x += sizeTabMax.x;

    return sizeFull;
}

void wxNotebook::SetPadding( const wxSize &padding )
{
    wxCHECK_RET( m_widget != nullptr, wxT("invalid notebook") );

    m_padding = padding.GetWidth();

    for (size_t i = GetPageCount(); i--;)
    {
        wxGtkNotebookPage* pageData = GetNotebookPage(i);
        if (pageData->m_image)
        {
            gtk_box_set_child_packing(GTK_BOX(pageData->m_box),
                pageData->m_image, false, false, m_padding, GTK_PACK_START);
        }
        gtk_box_set_child_packing(GTK_BOX(pageData->m_box),
            pageData->m_label, false, false, m_padding, GTK_PACK_END);
    }
}

void wxNotebook::SetTabSize(const wxSize& WXUNUSED(sz))
{
    wxFAIL_MSG( wxT("wxNotebook::SetTabSize not implemented") );
}

bool wxNotebook::DeleteAllPages()
{
    for (size_t i = GetPageCount(); i--;)
        DeletePage(i);

    return wxNotebookBase::DeleteAllPages();
}

wxNotebookPage *wxNotebook::DoRemovePage( size_t page )
{
    // We cannot remove the page yet, as GTK sends the "switch_page"
    // signal before it has removed the notebook-page from its
    // corresponding list. Thus, if we were to remove the page from
    // m_pages at this point, the two lists of pages would be out
    // of sync during the PAGE_CHANGING/PAGE_CHANGED events.
    wxNotebookPage *client = GetPage(page);
    if ( !client )
        return nullptr;

    // Suppress bogus assertion failures happening deep inside ATK (used by
    // GTK) that can't be avoided in any other way, see #22176.
    wxGTKImpl::LogFilterByMessage filterLog(
        "gtk_notebook_get_tab_label: assertion 'list != nullptr' failed"
    );

    // we don't need to unparent the client->m_widget; GTK+ will do
    // that for us (and will throw a warning if we do it!)
    gtk_notebook_remove_page( GTK_NOTEBOOK(m_widget), page );

    // It's safe to remove the page now.
    wxASSERT_MSG(GetPage(page) == client, wxT("pages changed during delete"));
    wxNotebookBase::DoRemovePage(page);

    m_pagesData.erase(m_pagesData.begin() + page);

    return client;
}

bool wxNotebook::InsertPage( size_t position,
                             wxNotebookPage* win,
                             const wxString& text,
                             bool select,
                             int imageId )
{
    wxCHECK_MSG( m_widget != nullptr, false, wxT("invalid notebook") );

    wxCHECK_MSG( win->GetParent() == this, false,
               wxT("Can't add a page whose parent is not the notebook!") );

    wxCHECK_MSG( position <= GetPageCount(), false,
                 wxT("invalid page index in wxNotebookPage::InsertPage()") );

    // Hack Alert! (Part II): See above in wxNotebook::AddChildGTK
    // why this has to be done.
    gtk_widget_unparent(win->m_widget);

    if (m_themeEnabled)
        win->SetThemeEnabled(true);

    GtkNotebook *notebook = GTK_NOTEBOOK(m_widget);

    m_pages.insert(m_pages.begin() + position, win);
    m_pagesData.insert(m_pagesData.begin() + position, wxGtkNotebookPage());

    wxGtkNotebookPage* const pageData = &m_pagesData[position];

    // set the label image and text
    // this must be done before adding the page, as GetPageText
    // and GetPageImage will otherwise return wrong values in
    // the page-changed event that results from inserting the
    // first page.
    pageData->m_imageIndex = imageId;

    pageData->m_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
#ifndef __WXGTK3__
    gtk_container_set_border_width(GTK_CONTAINER(pageData->m_box), 2);
#endif

    const wxBitmapBundle bundle = GetBitmapBundle(imageId);
    if ( bundle.IsOk() )
    {
        pageData->m_image = wxGtkImage::New();
        WX_GTK_IMAGE(pageData->m_image)->Set(bundle);
        gtk_box_pack_start(GTK_BOX(pageData->m_box),
            pageData->m_image, false, false, m_padding);
    }
    else
    {
        pageData->m_image = nullptr;
    }

    /* set the label text */
    pageData->m_label = gtk_label_new(wxStripMenuCodes(text).utf8_str());

    if (m_windowStyle & wxBK_LEFT)
        gtk_label_set_angle(GTK_LABEL(pageData->m_label), 90);
    if (m_windowStyle & wxBK_RIGHT)
        gtk_label_set_angle(GTK_LABEL(pageData->m_label), 270);

    gtk_box_pack_end(GTK_BOX(pageData->m_box),
        pageData->m_label, false, false, m_padding);

    gtk_widget_show_all(pageData->m_box);

    // Inserting the page may generate selection changing events that are not
    // expected here: we will send them ourselves below if necessary.
    g_signal_handlers_block_by_func(m_widget, (void*)switch_page, this);
    gtk_notebook_insert_page(notebook, win->m_widget, pageData->m_box, position);
    g_signal_handlers_unblock_by_func(m_widget, (void*)switch_page, this);

    /* apply current style */
#ifdef __WXGTK3__
    GTKApplyStyle(pageData->m_label, nullptr);
#else
    GtkRcStyle *style = GTKCreateWidgetStyle();
    if ( style )
    {
        gtk_widget_modify_style(pageData->m_label, style);
        g_object_unref(style);
    }
#endif

    DoSetSelectionAfterInsertion(position, select);

    InvalidateBestSize();
    return true;
}

// helper for HitTest(): check if the point lies inside the given widget which
// is the child of the notebook whose position and border size are passed as
// parameters
static bool
IsPointInsideWidget(const wxPoint& pt, GtkWidget *w,
                    gint x, gint y, gint border = 0)
{
    GtkAllocation a;
    gtk_widget_get_allocation(w, &a);
    return
        (pt.x >= a.x - x - border) &&
        (pt.x <= a.x - x + border + a.width) &&
        (pt.y >= a.y - y - border) &&
        (pt.y <= a.y - y + border + a.height);
}

int wxNotebook::HitTest(const wxPoint& pt, long *flags) const
{
    GtkAllocation a;
    gtk_widget_get_allocation(m_widget, &a);
    const int x = a.x;
    const int y = a.y;

    const size_t count = GetPageCount();

    for (size_t i = 0; i < count; i++)
    {
        wxGtkNotebookPage* pageData = GetNotebookPage(i);
        GtkWidget* box = pageData->m_box;

        if (!gtk_widget_get_child_visible(box))
            continue;

        const gint border = gtk_container_get_border_width(GTK_CONTAINER(box));

        if ( IsPointInsideWidget(pt, box, x, y, border) )
        {
            // ok, we're inside this tab -- now find out where, if needed
            if ( flags )
            {
                if (pageData->m_image && IsPointInsideWidget(pt, pageData->m_image, x, y))
                {
                    *flags = wxBK_HITTEST_ONICON;
                }
                else if (IsPointInsideWidget(pt, pageData->m_label, x, y))
                {
                    *flags = wxBK_HITTEST_ONLABEL;
                }
                else
                {
                    *flags = wxBK_HITTEST_ONITEM;
                }
            }

            return i;
        }
    }

    if ( flags )
    {
        *flags = wxBK_HITTEST_NOWHERE;
        wxWindowBase * page = GetCurrentPage();
        if ( page )
        {
            // rect origin is in notebook's parent coordinates
            wxRect rect = page->GetRect();

            // adjust it to the notebook's coordinates
            wxPoint pos = GetPosition();
            rect.x -= pos.x;
            rect.y -= pos.y;
            if ( rect.Contains( pt ) )
                *flags |= wxBK_HITTEST_ONPAGE;
        }
    }

    return wxNOT_FOUND;
}

void wxNotebook::OnNavigationKey(wxNavigationKeyEvent& event)
{
    if (event.IsWindowChange())
        AdvanceSelection( event.GetDirection() );
    else
        event.Skip();
}

#if wxUSE_CONSTRAINTS

// override these 2 functions to do nothing: everything is done in OnSize
void wxNotebook::SetConstraintSizes( bool WXUNUSED(recurse) )
{
    // don't set the sizes of the pages - their correct size is not yet known
    wxControl::SetConstraintSizes(false);
}

bool wxNotebook::DoPhase( int WXUNUSED(nPhase) )
{
    return true;
}

#endif

void wxNotebook::DoApplyWidgetStyle(GtkRcStyle *style)
{
    GTKApplyStyle(m_widget, style);
    for (size_t i = GetPageCount(); i--;)
        GTKApplyStyle(GetNotebookPage(i)->m_label, style);
}

GdkWindow *wxNotebook::GTKGetWindow(wxArrayGdkWindows& windows) const
{
    windows.push_back(gtk_widget_get_window(m_widget));
#ifdef __WXGTK3__
    GdkWindow* window = GTKFindWindow(m_widget);
    if (window)
        windows.push_back(window);
#else
    windows.push_back(GTK_NOTEBOOK(m_widget)->event_window);
#endif

    return nullptr;
}

// static
wxVisualAttributes
wxNotebook::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
    return GetDefaultAttributesFromGTKWidget(gtk_notebook_new());
}

#endif
