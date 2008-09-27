/////////////////////////////////////////////////////////////////////////////
// Name:        scrolbar.h
// Purpose:     interface of wxScrollBar
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxScrollBar

    A wxScrollBar is a control that represents a horizontal or
    vertical scrollbar. It is distinct from the two scrollbars that some windows
    provide automatically, but the two types of scrollbar share the way
    events are received.

    @beginStyleTable
    @style{wxSB_HORIZONTAL}
           Specifies a horizontal scrollbar.
    @style{wxSB_VERTICAL}
           Specifies a vertical scrollbar.
    @endStyleTable

    @library{wxcore}
    @category{ctrl}
    <!-- @appearance{scrollbar.png} -->

    @see @ref overview_scrolling, @ref overview_eventhandling, wxScrolled
*/
class wxScrollBar : public wxControl
{
public:
    /**
      Default constructor
    */
    wxScrollBar();
    
    /**
        Constructor, creating and showing a scrollbar.

        @param parent
            Parent window. Must be non-@NULL.
        @param id
            Window identifier. The value wxID_ANY indicates a default value.
        @param pos
            Window position. If wxDefaultPosition is specified then a default
        position is chosen.
        @param size
            Window size. If wxDefaultSize is specified then a default size
        is chosen.
        @param style
            Window style. See wxScrollBar.
        @param validator
            Window validator.
        @param name
            Window name.

        @see Create(), wxValidator
    */
    wxScrollBar(wxWindow* parent, wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSB_HORIZONTAL,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = "scrollBar");

    /**
        Destructor, destroying the scrollbar.
    */
    virtual ~wxScrollBar();

    /**
        Scrollbar creation function called by the scrollbar constructor.
        See wxScrollBar() for details.
    */
    bool Create(wxWindow* parent, wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSB_HORIZONTAL,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = "scrollBar");

    /**
        Returns the page size of the scrollbar. This is the number of scroll units
        that will be scrolled when the user pages up or down. Often it is the
        same as the thumb size.

        @see SetScrollbar()
    */
    virtual int GetPageSize() const;

    /**
        Returns the length of the scrollbar.

        @see SetScrollbar()
    */
    virtual int GetRange() const;

    /**
        Returns the current position of the scrollbar thumb.

        @see SetThumbPosition()
    */
    virtual int GetThumbPosition() const;

    /**
        Returns the thumb or 'view' size.

        @see SetScrollbar()
    */
    virtual int GetThumbSize() const;

    /**
        Sets the scrollbar properties.

        @param position
            The position of the scrollbar in scroll units.
        @param thumbSize
            The size of the thumb, or visible portion of the scrollbar, in scroll units.
        @param range
            The maximum position of the scrollbar.
        @param pageSize
            The size of the page size in scroll units. This is the number of units
            the scrollbar will scroll when it is paged up or down. Often it is the same
        as
            the thumb size.
        @param refresh
            @true to redraw the scrollbar, @false otherwise.

        @remarks Let's say you wish to display 50 lines of text, using the same
                 font. The window is sized so that you can only see 16
                 lines at a time.
    */
    virtual void SetScrollbar(int position, int thumbSize, int range,
                              int pageSize,
                              bool refresh = true);

    /**
        Sets the position of the scrollbar.

        @param viewStart
            The position of the scrollbar thumb.

        @see GetThumbPosition()
    */
    virtual void SetThumbPosition(int viewStart);
};

