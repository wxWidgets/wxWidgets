/////////////////////////////////////////////////////////////////////////////
// Name:        treebook.h
// Purpose:     interface of wxTreebook
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxTreebook

    This class is an extension of the wxNotebook class that allows a tree
    structured set of pages to be shown in a control. A classic example is a
    netscape preferences dialog that shows a tree of preference sections on
    the left and select section page on the right.

    To use the class simply create it and populate with pages using
    InsertPage(), InsertSubPage(), AddPage(), AddSubPage().

    If your tree is no more than 1 level in depth then you could simply use
    AddPage() and AddSubPage() to sequentially populate your tree by adding at
    every step a page or a subpage to the end of the tree.

    @beginEventTable{wxBookCtrlEvent}
    @event{EVT_TREEBOOK_PAGE_CHANGED(id, func)}
        The page selection was changed. Processes a @c
        wxEVT_COMMAND_TREEBOOK_PAGE_CHANGED event.
    @event{EVT_TREEBOOK_PAGE_CHANGING(id, func)}
        The page selection is about to be changed. Processes a @c
        wxEVT_COMMAND_TREEBOOK_PAGE_CHANGING event. This event can be @ref
        wxNotifyEvent::Veto() "vetoed".
    @event{EVT_TREEBOOK_NODE_COLLAPSED(id, func)}
        The page node is going to be collapsed. Processes a @c
        wxEVT_COMMAND_TREEBOOK_NODE_COLLAPSED event.
    @event{EVT_TREEBOOK_NODE_EXPANDED(id, func)}
        The page node is going to be expanded. Processes a @c
        wxEVT_COMMAND_TREEBOOK_NODE_EXPANDED event.
    @endEventTable

    @library{wxcore}
    @category{miscwnd}

    @see wxBookCtrl, wxBookCtrlEvent, wxNotebook, wxTreeCtrl, wxImageList,
         @ref overview_bookctrl, @ref page_samples_notebook
*/
class wxTreebook : public wxBookCtrlBase
{
public:
    /**
        Default constructor.
    */
    wxTreebook();

    /**
        Creates an empty wxTreebook.

        @param parent
            The parent window. Must be non-@NULL.
        @param id
            The window identifier.
        @param pos
            The window position.
        @param size
            The window size.
        @param style
            The window style. See wxNotebook.
        @param name
            The name of the control (used only under Motif).
    */
    wxTreebook(wxWindow* parent, wxWindowID id,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxBK_DEFAULT,
               const wxString& name = wxEmptyString);

    /**
        Destroys the wxTreebook object. Also deletes all the pages owned by the
        control (inserted previously into it).
    */
    virtual ~wxTreebook();

    /**
        Adds a new page. The page is placed at the topmost level after all other
        pages. @NULL could be specified for page to create an empty page.
    */
    virtual bool AddPage(wxWindow* page, const wxString& text,
                         bool bSelect = false, int imageId = wxNOT_FOUND);

    /**
        Adds a new child-page to the last top-level page. @NULL could be
        specified for page to create an empty page.
    */
    virtual bool AddSubPage(wxWindow* page, const wxString& text,
                            bool bSelect = false, int imageId = wxNOT_FOUND);

    /**
        Sets the image list for the page control and takes ownership of the
        list.

        @see wxImageList, SetImageList()
    */
    virtual void AssignImageList(wxImageList* imageList);

    /**
        Changes the selection for the given page, returning the previous
        selection.

        The call to this function does not generate the page changing events.
        This is the only difference with SetSelection(). See
        @ref overview_eventhandling_prog for more info.
    */
    virtual int ChangeSelection(size_t page);

    /**
        Shortcut for @ref wxTreebook::ExpandNode() "ExpandNode"( @a pageId,
        @false ).
    */
    bool CollapseNode(size_t pageId);

    /**
        Creates a treebook control. See wxTreebook::wxTreebook() for the
        description of the parameters.
    */
    bool Create(wxWindow* parent, wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxBK_DEFAULT,
                const wxString& name = wxEmptyString);

    /**
        Deletes all pages inserted into the treebook. No event is generated.
    */
    virtual bool DeleteAllPages();

    /**
        Deletes the page at the specified position and all its children. Could
        trigger page selection change in a case when selected page is removed.
        In that case its parent is selected (or the next page if no parent).
    */
    virtual bool DeletePage(size_t pagePos);

    /**
        Expands (collapses) the @a pageId node. Returns the previous state. May
        generate page changing events (if selected page is under the collapsed
        branch, then its parent is autoselected).
    */
    virtual bool ExpandNode(size_t pageId, bool expand = true);

    /**
        Returns the image index for the given page.
    */
    virtual int GetPageImage(size_t n) const;

    /**
        Returns the parent page of the given one or @c wxNOT_FOUND if this is a
        top-level page.
    */
    int GetPageParent(size_t page) const;

    /**
        Returns the string for the given page.
    */
    virtual wxString GetPageText(size_t n) const;

    /**
        Returns the currently selected page, or @c wxNOT_FOUND if none was
        selected.

        @note This method may return either the previously or newly selected
            page when called from the EVT_TREEBOOK_PAGE_CHANGED() handler
            depending on the platform and so wxBookCtrlEvent::GetSelection()
            should be used instead in this case.
    */
    virtual int GetSelection() const;

    /**
        Inserts a new page just before the page indicated by @a pagePos. The new
        page is placed before @a pagePos page and on the same level. @NULL could
        be specified for page to create an empty page.
    */
    virtual bool InsertPage(size_t pagePos, wxWindow* page,
                            const wxString& text, bool bSelect = false,
                            int imageId = wxNOT_FOUND);

    /**
        Inserts a sub page under the specified page.

        @NULL could be specified for page to create an empty page.
    */
    virtual bool InsertSubPage(size_t pagePos, wxWindow* page,
                               const wxString& text, bool bSelect = false,
                               int imageId = wxNOT_FOUND);

    /**
        Returns @true if the page represented by @a pageId is expanded.
    */
    virtual bool IsNodeExpanded(size_t pageId) const;

    /**
        Sets the image list for the page control. It does not take ownership of
        the image list, you must delete it yourself.

        @see wxImageList, AssignImageList()
    */
    virtual void SetImageList(wxImageList* imageList);

    /**
        Sets the image index for the given @a page. @a imageId is an index into
        the image list which was set with SetImageList().
    */
    virtual bool SetPageImage(size_t page, int imageId);

    /**
        Sets the @a text for the given @a page.
    */
    virtual bool SetPageText(size_t page, const wxString& text);

    /**
        @deprecated Please use ChangeSelection() instead.

        Sets the selection for the given page, returning the previous selection.

        The call to this function generates page changing events.

        @see GetSelection(), ChangeSelection()
    */
    virtual int SetSelection(size_t n);
};

