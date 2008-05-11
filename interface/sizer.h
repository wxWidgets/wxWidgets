/////////////////////////////////////////////////////////////////////////////
// Name:        sizer.h
// Purpose:     interface of wxStdDialogButtonSizer
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxStdDialogButtonSizer
    @wxheader{sizer.h}

    This class creates button layouts which conform to the standard button spacing
    and ordering defined by the platform
    or toolkit's user interface guidelines (if such things exist). By using this
    class, you can ensure that all your
    standard dialogs look correct on all major platforms. Currently it conforms to
    the Windows, GTK+ and Mac OS X
    human interface guidelines.

    When there aren't interface guidelines defined for a particular platform or
    toolkit, wxStdDialogButtonSizer reverts
    to the Windows implementation.

    To use this class, first add buttons to the sizer by calling AddButton (or
    SetAffirmativeButton, SetNegativeButton,
    or SetCancelButton) and then call Realize in order to create the actual button
    layout used. Other than these special
    operations, this sizer works like any other sizer.

    If you add a button with wxID_SAVE, on Mac OS X the button will be renamed to
    "Save" and
    the wxID_NO button will be renamed to "Don't Save" in accordance with the Mac
    OS X Human Interface Guidelines.

    @library{wxcore}
    @category{FIXME}

    @see wxSizer, @ref overview_sizeroverview "Sizer overview",
    wxDialog::CreateButtonSizer
*/
class wxStdDialogButtonSizer : public wxBoxSizer
{
public:
    /**
        Constructor for a wxStdDialogButtonSizer.
    */
    wxStdDialogButtonSizer();

    /**
        Adds a button to the wxStdDialogButtonSizer. The button must have one of the
        following identifiers:
         wxID_OK
         wxID_YES
         wxID_SAVE
         wxID_APPLY
         wxID_CLOSE
         wxID_NO
         wxID_CANCEL
         wxID_HELP
         wxID_CONTEXT_HELP
    */
    void AddButton(wxButton* button);

    /**
        Rearranges the buttons and applies proper spacing between buttons to make them
        match the platform or toolkit's interface guidelines.
    */
    void Realize();

    /**
        Sets the affirmative button for the sizer. This allows you to use identifiers
        other than the standard identifiers outlined above.
    */
    void SetAffirmativeButton(wxButton* button);

    /**
        Sets the cancel button for the sizer. This allows you to use identifiers other
        than the standard identifiers outlined above.
    */
    void SetCancelButton(wxButton* button);

    /**
        Sets the negative button for the sizer. This allows you to use identifiers
        other than the standard identifiers outlined above.
    */
    void SetNegativeButton(wxButton* button);
};



/**
    @class wxSizerItem
    @wxheader{sizer.h}

    The wxSizerItem class is used to track the position, size and other
    attributes of each item managed by a wxSizer. It is not usually necessary
    to use this class because the sizer elements can also be identified by
    their positions or window or sizer pointers but sometimes it may be more
    convenient to use it directly.

    @library{wxcore}
    @category{winlayout}
*/
class wxSizerItem : public wxObject
{
public:
    //@{
    /**
        Construct a sizer item for tracking a subsizer.
    */
    wxSizerItem(int width, int height, int proportion, int flag,
                int border, wxObject* userData);
    wxSizerItem(wxWindow* window, const wxSizerFlags& flags);
    wxSizerItem(wxWindow* window, int proportion, int flag,
                int border,
                wxObject* userData);
    wxSizerItem(wxSizer* window, const wxSizerFlags& flags);
    wxSizerItem(wxSizer* sizer, int proportion, int flag,
                int border,
                wxObject* userData);
    //@}

    /**
        Deletes the user data and subsizer, if any.
    */
    ~wxSizerItem();

    /**
        Calculates the minimum desired size for the item, including any space
        needed by borders.
    */
    wxSize CalcMin();

    /**
        Destroy the window or the windows in a subsizer, depending on the type
        of item.
    */
    void DeleteWindows();

    /**
        Enable deleting the SizerItem without destroying the contained sizer.
    */
    void DetachSizer();

    /**
        Return the border attribute.
    */
    int GetBorder() const;

    /**
        Return the flags attribute.

        See @ref wxsizer_flags "wxSizer flags list" for details.
    */
    int GetFlag() const;

    /**
        Return the numeric id of wxSizerItem, or @c wxID_NONE if the id has
        not been set.
    */
    int GetId() const;

    /**
        Get the minimum size needed for the item.
    */
    wxSize GetMinSize() const;

    /**
        Sets the minimum size to be allocated for this item.

        If this item is a window, the @a size is also passed to
        wxWindow::SetMinSize().
     */
    void SetMinSize(const wxSize& size);

    /**
        @overload
     */
    void SetMinSize(int x, int y);

    /**
        What is the current position of the item, as set in the last Layout.
    */
    wxPoint GetPosition() const;

    /**
        Get the proportion item attribute.
    */
    int GetProportion() const;

    /**
        Get the ration item attribute.
    */
    float GetRatio() const;

    /**
        Get the rectangle of the item on the parent window, excluding borders.
    */
    wxRect GetRect();

    /**
        Get the current size of the item, as set in the last Layout.
    */
    wxSize GetSize() const;

    /**
        If this item is tracking a sizer, return it.  @NULL otherwise.
    */
    wxSizer* GetSizer() const;

    /**
        If this item is tracking a spacer, return its size.
    */
    const wxSize GetSpacer() const;

    /**
        Get the userData item attribute.
    */
    wxObject* GetUserData() const;

    /**
        If this item is tracking a window then return it.  @NULL otherwise.
    */
    wxWindow* GetWindow() const;

    /**
        Returns @true if this item is a window or a spacer and it is shown or
        if this item is a sizer and not all of its elements are hidden.

        In other words, for sizer items, all of the child elements must be
        hidden for the sizer itself to be considered hidden.

        As an exception, if the @c wxRESERVE_SPACE_EVEN_IF_HIDDEN flag was
        used for this sizer item, then IsShown() always returns @true for it
        (see wxSizerFlags::ReserveSpaceEvenIfHidden()).
    */
    bool IsShown() const;

    /**
        Is this item a sizer?
    */
    bool IsSizer() const;

    /**
        Is this item a spacer?
    */
    bool IsSpacer() const;

    /**
        Is this item a window?
    */
    bool IsWindow() const;

    /**
        Set the border item attribute.
    */
    void SetBorder(int border);

    /**
        Set the position and size of the space allocated to the sizer, and
        adjust the position and size of the item to be within that space
        taking alignment and borders into account.
    */
    void SetDimension(const wxPoint& pos, const wxSize& size);

    /**
        Set the flag  item attribute.
    */
    void SetFlag(int flag);

    /**
        Sets the numeric id of the wxSizerItem to @e id.
    */
    void SetId(int id);

    /**

    */
    void SetInitSize(int x, int y);

    /**
        Set the proportion item attribute.
    */
    void SetProportion(int proportion);

    //@{
    /**
        Set the ratio item attribute.
    */
    void SetRatio(int width, int height);
    void SetRatio(wxSize size);
    void SetRatio(float ratio);
    //@}

    /**
        Set the sizer tracked by this item.
    */
    void SetSizer(wxSizer* sizer);

    /**
        Set the size of the spacer tracked by this item.
    */
    void SetSpacer(const wxSize& size);

    /**
        Set the window to be tracked by thsi item.
    */
    void SetWindow(wxWindow* window);

    /**
        Set the show item attribute, which sizers use to determine if the item
        is to be made part of the layout or not.  If the item is tracking a
        window then it is shown or hidden as needed.
    */
    void Show(bool show);
};



/**
    @class wxSizerFlags
    @wxheader{sizer.h}

    Normally, when you add an item to a sizer via
    wxSizer::Add, you have to specify a lot of flags and
    parameters which can be unwieldy. This is where wxSizerFlags comes in: it
    allows you to specify all parameters using the named methods instead. For
    example, instead of

    @code
    sizer->Add(ctrl, 0, wxEXPAND | wxALL, 10);
    @endcode

    you can now write

    @code
    sizer->Add(ctrl, wxSizerFlags().Expand().Border(10));
    @endcode

    This is more readable and also allows you to create wxSizerFlags objects which
    can be reused for several sizer items.

    @code
    wxSizerFlags flagsExpand(1);
        flagsExpand.Expand().Border(10);

        sizer->Add(ctrl1, flagsExpand);
        sizer->Add(ctrl2, flagsExpand);
    @endcode

    Note that by specification, all methods of wxSizerFlags return the wxSizerFlags
    object itself to allowing chaining multiple methods calls like in the examples
    above.

    @library{wxcore}
    @category{winlayout}

    @see wxSizer
*/
class wxSizerFlags
{
public:
    /**
        Creates the wxSizer with the proportion specified by @e proportion.
    */
    wxSizerFlags(int proportion = 0);

    /**
        Sets the alignment of this wxSizerFlags to @e align.  Note that if this
        method is not called, the wxSizerFlags has no specified alignment.

        @see Top(), Left(), Right(), Bottom(), Centre()
    */
    wxSizerFlags& Align(int align = 0);

    /**
        Sets the wxSizerFlags to have a border of a number of pixels specified
        by @a borderinpixels with the directions specified by @e direction.
    */
    wxSizerFlags& Border(int direction, int borderinpixels);

    /**
        Sets the wxSizerFlags to have a border with size as returned by
        GetDefaultBorder().

        @param direction Direction(s) to apply the border in.
    */
    wxSizerFlags& Border(int direction = wxALL);

    /**
        Aligns the object to the bottom, shortcut for @c Align(wxALIGN_BOTTOM).

        @see Align()
    */
    wxSizerFlags& Bottom();

    /**
        Sets the object of the wxSizerFlags to center itself in the area it is
        given.
    */
    wxSizerFlags& Center();

    /**
        Center() for people with the other dialect of English.
    */
    wxSizerFlags& Centre();

    /**
        Sets the border in the given @a direction having twice the default
        border size.
    */
    wxSizerFlags& DoubleBorder(int direction = wxALL);

    /**
        Sets the border in left and right directions having twice the default
        border size.
    */
    wxSizerFlags& DoubleHorzBorder();

    /**
        Sets the object of the wxSizerFlags to expand to fill as much area as
        it can.
    */
    wxSizerFlags& Expand();

    /**
        Set the @c wxFIXED_MINSIZE flag which indicates that the initial size
        of the window should be also set as its minimal size.
    */
    wxSizerFlags& FixedMinSize();

    /**
        Set the @c wxRESERVE_SPACE_EVEN_IF_HIDDEN flag. Normally wxSizers
        don't allocate space for hidden windows or other items. This flag
        overrides this behavior so that sufficient space is allocated for the
        window even if it isn't visible. This makes it possible to dynamically
        show and hide controls without resizing parent dialog, for example.

        @since 2.8.8
    */
    wxSizerFlags& ReserveSpaceEvenIfHidden();

    /**
        Returns the border used by default in Border() method.
    */
    static int GetDefaultBorder();

    /**
        Aligns the object to the left, shortcut for @c Align(wxALIGN_LEFT)

        @see Align()
    */
    wxSizerFlags& Left();

    /**
        Sets the proportion of this wxSizerFlags to @e proportion
    */
    wxSizerFlags& Proportion(int proportion = 0);

    /**
        Aligns the object to the right, shortcut for @c Align(wxALIGN_RIGHT)

        @see Align()
    */
    wxSizerFlags& Right();

    /**
        Set the @c wx_SHAPED flag which indicates that the elements should
        always keep the fixed width to height ratio equal to its original value.
    */
    wxSizerFlags& Shaped();

    /**
        Aligns the object to the top, shortcut for @c Align(wxALIGN_TOP)

        @see Align()
    */
    wxSizerFlags& Top();

    /**
        Sets the border in the given @a direction having thrice the default
        border size.
    */
    wxSizerFlags& TripleBorder(int direction = wxALL);
};



/**
    @class wxNotebookSizer
    @wxheader{sizer.h}

    @deprecated
    This class is deprecated and should not be used in new code! It is no
    longer needed, wxNotebook control can be inserted
    into any sizer class and its minimal size will be determined correctly.

    wxNotebookSizer is a specialized sizer to make sizers work in connection
    with using notebooks. This sizer is different from any other sizer as you
    must not add any children to it - instead, it queries the notebook class
    itself.  The only thing this sizer does is to determine the size of the
    biggest page of the notebook and report an adjusted minimal size to a more
    toplevel sizer.

    @library{wxbase}
    @category{winlayout}

    @see wxSizer, wxNotebook,
         @ref overview_sizer "Sizers overview"
*/
class wxNotebookSizer : public wxSizer
{
public:
    /**
        Constructor. It takes an associated notebook as its only parameter.
    */
    wxNotebookSizer(wxNotebook* notebook);

    /**
        Returns the notebook associated with the sizer.
    */
    wxNotebook* GetNotebook();
};



/**
    @class wxFlexGridSizer
    @wxheader{sizer.h}

    A flex grid sizer is a sizer which lays out its children in a two-dimensional
    table with all table fields in one row having the same
    height and all fields in one column having the same width, but all
    rows or all columns are not necessarily the same height or width as in
    the wxGridSizer.

    Since wxWidgets 2.5.0, wxFlexGridSizer can also size items equally in one
    direction but unequally ("flexibly") in the other. If the sizer is only
    flexible in one direction (this can be changed using
    wxFlexGridSizer::SetFlexibleDirection),
    it needs to be decided how the sizer should grow in the other ("non-flexible")
    direction in order to fill the available space. The
    wxFlexGridSizer::SetNonFlexibleGrowMode method
    serves this purpose.

    @library{wxcore}
    @category{winlayout}

    @see wxSizer, @ref overview_sizeroverview "Sizer overview"
*/
class wxFlexGridSizer : public wxGridSizer
{
public:
    //@{
    /**
        Constructor for a wxGridSizer. @a rows and @a cols determine the number of
        columns and rows in the sizer - if either of the parameters is zero, it will be
        calculated to form the total number of children in the sizer, thus making the
        sizer grow dynamically. @a vgap and @a hgap define extra space between
        all children.
    */
    wxFlexGridSizer(int rows, int cols, int vgap, int hgap);
    wxFlexGridSizer(int cols, int vgap = 0, int hgap = 0);
    //@}

    /**
        Specifies that column @a idx (starting from zero) should be grown if
        there is extra space available to the sizer.
        The @a proportion parameter has the same meaning as the stretch factor for
        the sizers() except that if all proportions are 0,
        then all columns are resized equally (instead of not being resized at all).
    */
    void AddGrowableCol(size_t idx, int proportion = 0);

    /**
        Specifies that row idx (starting from zero) should be grown if there
        is extra space available to the sizer.
        See AddGrowableCol() for the description
        of @a proportion parameter.
    */
    void AddGrowableRow(size_t idx, int proportion = 0);

    /**
        Returns a wxOrientation value that specifies whether the sizer flexibly
        resizes its columns, rows, or both (default).

        @return One of the following values:

        @see SetFlexibleDirection()
    */
    int GetFlexibleDirection() const;

    /**
        Returns the value that specifies how the sizer grows in the "non-flexible"
        direction if there is one.

        @return One of the following values:

        @see SetFlexibleDirection(),
             SetNonFlexibleGrowMode()
    */
    int GetNonFlexibleGrowMode() const;

    /**
        Specifies that column idx is no longer growable.
    */
    void RemoveGrowableCol(size_t idx);

    /**
        Specifies that row idx is no longer growable.
    */
    void RemoveGrowableRow(size_t idx);

    /**
        Specifies whether the sizer should flexibly resize its columns, rows, or
        both. Argument @c direction can be @c wxVERTICAL, @c wxHORIZONTAL
        or @c wxBOTH (which is the default value). Any other value is ignored. See
        @ref getflexibledrection() GetFlexibleDirection for the
        explanation of these values.
        Note that this method does not trigger relayout.
    */
    void SetFlexibleDirection(int direction);

    /**
        Specifies how the sizer should grow in the non-flexible direction if
        there is one (so
        SetFlexibleDirection() must have
        been called previously). Argument @a mode can be one of those documented in
        GetNonFlexibleGrowMode(), please
        see there for their explanation.
        Note that this method does not trigger relayout.
    */
    void SetNonFlexibleGrowMode(wxFlexSizerGrowMode mode);
};



/**
    @class wxSizer
    @wxheader{sizer.h}

    wxSizer is the abstract base class used for laying out subwindows in a window.
    You
    cannot use wxSizer directly; instead, you will have to use one of the sizer
    classes derived from it. Currently there are wxBoxSizer,
    wxStaticBoxSizer,
    wxGridSizer,
    wxFlexGridSizer,
    wxWrapSizer
     and wxGridBagSizer.

    The layout algorithm used by sizers in wxWidgets is closely related to layout
    in other GUI toolkits, such as Java's AWT, the GTK toolkit or the Qt toolkit.
    It is
    based upon the idea of the individual subwindows reporting their minimal
    required
    size and their ability to get stretched if the size of the parent window has
    changed.
    This will most often mean that the programmer does not set the original size of
    a dialog in the beginning, rather the dialog will be assigned a sizer and this
    sizer
    will be queried about the recommended size. The sizer in turn will query its
    children, which can be normal windows, empty space or other sizers, so that
    a hierarchy of sizers can be constructed. Note that wxSizer does not derive
    from wxWindow
    and thus does not interfere with tab ordering and requires very little
    resources compared
    to a real window on screen.

    What makes sizers so well fitted for use in wxWidgets is the fact that every
    control
    reports its own minimal size and the algorithm can handle differences in font
    sizes
    or different window (dialog item) sizes on different platforms without
    problems. If e.g.
    the standard font as well as the overall design of Motif widgets requires more
    space than
    on Windows, the initial dialog size will automatically be bigger on Motif than
    on Windows.

    Sizers may also be used to control the layout of custom drawn items on the
    window.  The Add(), Insert(), and Prepend() functions return a pointer to
    the newly added wxSizerItem. Just add empty space of the desired size and
    attributes, and then use the wxSizerItem::GetRect() method to determine
    where the drawing operations should take place.

    Please notice that sizers, like child windows, are owned by the library and
    will be deleted by it which implies that they must be allocated on the
    heap.  However if you create a sizer and do not add it to another sizer or
    window, the library wouldn't be able to delete such an orphan sizer and in
    this, and only this, case it should be deleted explicitly.

    @b wxPython note: If you wish to create a sizer class in wxPython you should
    derive the class from @c wxPySizer in order to get Python-aware
    capabilities for the various virtual methods.

    @anchor wxsizer_flags
    @par wxSizer flags
    The "flag" argument accepted by wxSizeItem constructors and other
    functions, e.g. wxSizer::Add(), is OR-combination of the following flags.
    Two main behaviours are defined using these flags. One is the border around
    a window: the border parameter determines the border width whereas the
    flags given here determine which side(s) of the item that the border will
    be added.  The other flags determine how the sizer item behaves when the
    space allotted to the sizer changes, and is somewhat dependent on the
    specific kind of sizer used.
    @beginDefList
    @itemdef{wxTOP<br>
             wxBOTTOM<br>
             wxLEFT<br>
             wxRIGHT<br>
             wxALL,
             These flags are used to specify which side(s) of the sizer item
             the border width will apply to.}
    @itemdef{wxEXPAND,
             The item will be expanded to fill the space assigned to the item.}
    @itemdef{wxSHAPED,
             The item will be expanded as much as possible while also
             maintaining its aspect ratio.}
    @itemdef{wxFIXED_MINSIZE,
             Normally wxSizers will use GetAdjustedBestSize() to determine what
             the minimal size of window items should be, and will use that size
             to calculate the layout. This allows layouts to adjust when an
             item changes and its best size becomes different. If you would
             rather have a window item stay the size it started with then use
             wxFIXED_MINSIZE.}
    @itemdef{wxRESERVE_SPACE_EVEN_IF_HIDDEN,
             Normally wxSizers don't allocate space for hidden windows or other
             items. This flag overrides this behavior so that sufficient space
             is allocated for the window even if it isn't visible. This makes
             it possible to dynamically show and hide controls without resizing
             parent dialog, for example. (Available since 2.8.8.)
             }
    @itemdef{wxALIGN_CENTER<br>
             wxALIGN_CENTRE<br>
             wxALIGN_LEFT<br>
             wxALIGN_RIGHT<br>
             wxALIGN_TOP<br>
             wxALIGN_BOTTOM<br>
             wxALIGN_CENTER_VERTICAL<br>
             wxALIGN_CENTRE_VERTICAL<br>
             wxALIGN_CENTER_HORIZONTAL<br>
             wxALIGN_CENTRE_HORIZONTAL,
             The wxALIGN flags allow you to specify the alignment of the item
             within the space allotted to it by the sizer, adjusted for the
             border if any.}
    @endDefList


    @library{wxcore}
    @category{winlayout}

    @see @ref overview_sizeroverview "Sizer overview"
*/
class wxSizer : public wxObject
{
public:
    /**
        The constructor. Note that wxSizer is an abstract base class and may not
        be instantiated.
    */
    wxSizer();

    /**
        The destructor.
    */
    ~wxSizer();

    /** @name Item Management */
    //@{

    /**
        Appends a child to the sizer.

        wxSizer itself is an abstract class, but the parameters are equivalent
        in the derived classes that you will instantiate to use it so they are
        described here:

        @param window
            The window to be added to the sizer. Its initial size (either set
            explicitly by the user or calculated internally when using
            wxDefaultSize) is interpreted as the minimal and in many cases also
            the initial size.
        @param flags
            A wxSizerFlags object that enables you to specify most of the above
            parameters more conveniently.
    */
    wxSizerItem* Add(wxWindow* window, const wxSizerFlags& flags);

    /**
        Appends a child to the sizer.

        wxSizer itself is an abstract class, but the parameters are equivalent
        in the derived classes that you will instantiate to use it so they are
        described here:

        @param window
            The window to be added to the sizer. Its initial size (either set
            explicitly by the user or calculated internally when using
            wxDefaultSize) is interpreted as the minimal and in many cases also
            the initial size.
        @param proportion
            Although the meaning of this parameter is undefined in wxSizer, it
            is used in wxBoxSizer to indicate if a child of a sizer can change
            its size in the main orientation of the wxBoxSizer - where 0 stands
            for not changeable and a value of more than zero is interpreted
            relative to the value of other children of the same wxBoxSizer. For
            example, you might have a horizontal wxBoxSizer with three
            children, two of which are supposed to change their size with the
            sizer. Then the two stretchable windows would get a value of 1 each
            to make them grow and shrink equally with the sizer's horizontal
            dimension.
        @param flag
            OR-combination of flags affecting sizer's behavior. See
            @ref wxsizer_flags "wxSizer flags list" for details.
        @param border
            Determines the border width, if the flag parameter is set to
            include any border flag.
        @param userData
            Allows an extra object to be attached to the sizer item, for use in
            derived classes when sizing information is more complex than the
            proportion and flag will allow for.
    */
    wxSizerItem* Add(wxWindow* window, int proportion = 0,
                     int flag = 0,
                     int border = 0,
                     wxObject* userData = NULL);

    /**
        Appends a child to the sizer.

        wxSizer itself is an abstract class, but the parameters are equivalent
        in the derived classes that you will instantiate to use it so they are
        described here:

        @param sizer
            The (child-)sizer to be added to the sizer. This allows placing a
            child sizer in a sizer and thus to create hierarchies of sizers
            (typically a vertical box as the top sizer and several horizontal
            boxes on the level beneath).
        @param flags
            A wxSizerFlags object that enables you to specify most of the above
            parameters more conveniently.
    */
    wxSizerItem* Add(wxSizer* sizer, const wxSizerFlags& flags);

    /**
        Appends a child to the sizer.

        wxSizer itself is an abstract class, but the parameters are equivalent
        in the derived classes that you will instantiate to use it so they are
        described here:

        @param sizer
            The (child-)sizer to be added to the sizer. This allows placing a
            child sizer in a sizer and thus to create hierarchies of sizers
            (typically a vertical box as the top sizer and several horizontal
            boxes on the level beneath).
        @param proportion
            Although the meaning of this parameter is undefined in wxSizer, it
            is used in wxBoxSizer to indicate if a child of a sizer can change
            its size in the main orientation of the wxBoxSizer - where 0 stands
            for not changeable and a value of more than zero is interpreted
            relative to the value of other children of the same wxBoxSizer. For
            example, you might have a horizontal wxBoxSizer with three
            children, two of which are supposed to change their size with the
            sizer. Then the two stretchable windows would get a value of 1 each
            to make them grow and shrink equally with the sizer's horizontal
            dimension.
        @param flag
            OR-combination of flags affecting sizer's behavior. See
            @ref wxsizer_flags "wxSizer flags list" for details.
        @param border
            Determines the border width, if the flag parameter is set to
            include any border flag.
        @param userData
            Allows an extra object to be attached to the sizer item, for use in
            derived classes when sizing information is more complex than the
            proportion and flag will allow for.
    */
    wxSizerItem* Add(wxSizer* sizer, int proportion = 0,
                     int flag = 0,
                     int border = 0,
                     wxObject* userData = NULL);

    /**
        Appends a spacer child to the sizer.

        wxSizer itself is an abstract class, but the parameters are equivalent
        in the derived classes that you will instantiate to use it so they are
        described here.

        @a width and @a height specify the dimension of a spacer to be added to
        the sizer. Adding spacers to sizers gives more flexibility in the
        design of dialogs; imagine for example a horizontal box with two
        buttons at the bottom of a dialog: you might want to insert a space
        between the two buttons and make that space stretchable using the
        proportion flag and the result will be that the left button will be
        aligned with the left side of the dialog and the right button with the
        right side - the space in between will shrink and grow with the dialog.

        @param width
            Width of the spacer.
        @param height
            Height of the spacer.
        @param proportion
            Although the meaning of this parameter is undefined in wxSizer, it
            is used in wxBoxSizer to indicate if a child of a sizer can change
            its size in the main orientation of the wxBoxSizer - where 0 stands
            for not changeable and a value of more than zero is interpreted
            relative to the value of other children of the same wxBoxSizer. For
            example, you might have a horizontal wxBoxSizer with three
            children, two of which are supposed to change their size with the
            sizer. Then the two stretchable windows would get a value of 1 each
            to make them grow and shrink equally with the sizer's horizontal
            dimension.
        @param flag
            OR-combination of flags affecting sizer's behavior. See
            @ref wxsizer_flags "wxSizer flags list" for details.
        @param border
            Determines the border width, if the flag parameter is set to
            include any border flag.
        @param userData
            Allows an extra object to be attached to the sizer item, for use in
            derived classes when sizing information is more complex than the
            proportion and flag will allow for.
    */
    wxSizerItem* Add(int width, int height, int proportion = 0,
                     int flag = 0,
                     int border = 0,
                     wxObject* userData = NULL);

    /**
        Adds non-stretchable space to the sizer. More readable way of calling
        wxSizer::Add(size, size, 0).
    */
    wxSizerItem* AddSpacer(int size);

    /**
        Adds stretchable space to the sizer. More readable way of calling
        wxSizer::Add(0, 0, prop).
    */
    wxSizerItem* AddStretchSpacer(int prop = 1);

    //@}

    /**
        This method is abstract and has to be overwritten by any derived class.
        Here, the sizer will do the actual calculation of its children's minimal sizes.
    */
    wxSize CalcMin();

    /**
        Detaches all children from the sizer. If @a delete_windows is @true then
        child windows will also be deleted.
    */
    void Clear(bool delete_windows = false);

    /**
        Computes client area size for @a window so that it matches the sizer's
        minimal size. Unlike GetMinSize(), this method accounts for other
        constraints imposed on @e window, namely display's size (returned size
        will never be too large for the display) and maximum window size if
        previously set by wxWindow::SetMaxSize(). The returned value is
        suitable for passing to wxWindow::SetClientSize() or
        wxWindow::SetMinClientSize().

        @since 2.8.8

        @see ComputeFittingWindowSize(), Fit()
    */
    wxSize ComputeFittingClientSize(wxWindow* window);

    /**
        Like ComputeFittingClientSize(), but converts the result into window
        size. The returned value is suitable for passing to wxWindow::SetSize()
        or wxWindow::SetMinSize().

        @since 2.8.8

        @see ComputeFittingClientSize(), Fit()
    */
    wxSize ComputeFittingWindowSize(wxWindow* window);

    //@{
    /**
        Detach a child from the sizer without destroying it. @a window is the window to
        be
        detached, @a sizer is the equivalent sizer and @a index is the position of
        the child in the sizer, typically 0 for the first item. This method does not
        cause any layout or resizing to take place, call Layout()
        to update the layout "on screen" after detaching a child from the sizer.
        Returns @true if the child item was found and detached, @false otherwise.

        @see Remove()
    */
    bool Detach(wxWindow* window);
    bool Detach(wxSizer* sizer);
    bool Detach(size_t index);
    //@}

    /**
        Tell the sizer to resize the @a window so that its client area matches the
        sizer's minimal size
        (ComputeFittingClientSize() is called
        to determine it).
        This is commonly done in the constructor of the window
        itself, see sample in the description
        of wxBoxSizer. Returns the new window size.

        @see ComputeFittingClientSize(), ComputeFittingWindowSize()
    */
    wxSize Fit(wxWindow* window);

    /**
        Tell the sizer to resize the virtual size of the @a window to match the sizer's
        minimal size.  This will not alter the on screen size of the window, but may
        cause
        the addition/removal/alteration of scrollbars required to view the virtual area
        in
        windows which manage it.

        @see wxScrolled::SetScrollbars(), SetVirtualSizeHints()
    */
    void FitInside(wxWindow* window);

    //@{
    /**
        Returns the list of the items in this sizer. The elements of type-safe
        wxList @c wxSizerItemList are objects of type
        @ref overview_wxsizeritem "wxSizerItem *".
    */
    const wxSizerItemList GetChildren();
    const wxSizerItemList GetChildren();
    //@}

    /**
        Returns the window this sizer is used in or @NULL if none.
    */
    wxWindow* GetContainingWindow() const;

    //@{
    /**
        Finds item of the sizer which holds given @e window, @a sizer or is located
        in sizer at position @e index.
        Use parameter @a recursive to search in subsizers too.
        Returns pointer to item or @NULL.
    */
    wxSizerItem* GetItem(wxWindow* window, bool recursive = false);
    wxSizerItem* GetItem(wxSizer* sizer, bool recursive = false);
    wxSizerItem* GetItem(size_t index);
    //@}

    /**
        Finds item of the sizer which has the given @e id.  This @a id is not the
        window id but the id of the wxSizerItem itself.  This is mainly useful for
        retrieving the sizers created from XRC resources.
        Use parameter @a recursive to search in subsizers too.
        Returns pointer to item or @NULL.
    */
    wxSizerItem* GetItemById(int id, bool recursive = false);

    /**
        Returns the minimal size of the sizer. This is either the combined minimal
        size of all the children and their borders or the minimal size set by
        SetMinSize(), depending on which is bigger.
        Note that the returned value is client size, not window size.
        In particular, if you use the value to set toplevel window's minimal or
        actual size, use wxWindow::SetMinClientSize
        or wxWindow::SetClientSize, not
        wxWindow::SetMinSize
        or wxWindow::SetSize.
    */
    wxSize GetMinSize();

    /**
        Returns the current position of the sizer.
    */
    wxPoint GetPosition();

    /**
        Returns the current size of the sizer.
    */
    wxSize GetSize();

    //@{
    /**
        Hides the @e window, @e sizer, or item at @e index.
        To make a sizer item disappear, use Hide() followed by Layout().
        Use parameter @a recursive to hide elements found in subsizers.
        Returns @true if the child item was found, @false otherwise.

        @see IsShown(), Show()
    */
    bool Hide(wxWindow* window, bool recursive = false);
    bool Hide(wxSizer* sizer, bool recursive = false);
    bool Hide(size_t index);
    //@}

    //@{
    /**
        Insert a child into the sizer before any existing item at

        See Add() for the meaning of the other parameters.

        @param index The position this child should assume in the sizer.
    */
    wxSizerItem* Insert(size_t index, wxWindow* window,
                        const wxSizerFlags& flags);
    wxSizerItem* Insert(size_t index, wxWindow* window,
                        int proportion = 0,
                        int flag = 0,
                        int border = 0,
                        wxObject* userData = NULL);
    wxSizerItem* Insert(size_t index, wxSizer* sizer,
                        const wxSizerFlags& flags);
    wxSizerItem* Insert(size_t index, wxSizer* sizer,
                        int proportion = 0,
                        int flag = 0,
                        int border = 0,
                        wxObject* userData = NULL);
    wxSizerItem* Insert(size_t index, int width, int height,
                        int proportion = 0,
                        int flag = 0,
                        int border = 0,
                        wxObject* userData = NULL);
    //@}

    /**
        Inserts non-stretchable space to the sizer. More readable way of calling
        wxSizer::Insert(size, size, 0).
    */
    wxSizerItem* InsertSpacer(size_t index, int size);

    /**
        Inserts stretchable space to the sizer. More readable way of calling
        wxSizer::Insert(0, 0, prop).
    */
    wxSizerItem* InsertStretchSpacer(size_t index, int prop = 1);

    /**
        Returns @true if the @e window is shown.

        @see Hide(), Show(), wxSizerItem::IsShown()
    */
    bool IsShown(wxWindow* window) const;

    /**
        Returns @true if the @e sizer is shown.

        @see Hide(), Show(), wxSizerItem::IsShown()
    */
    bool IsShown(wxSizer* sizer) const;

    /**
        Returns @true if the item at @a index is shown.

        @see Hide(), Show(), wxSizerItem::IsShown()
    */
    bool IsShown(size_t index) const;

    /**
        Call this to force layout of the children anew, e.g. after having added a child
        to or removed a child (window, other sizer or space) from the sizer while
        keeping
        the current dimension.
    */
    void Layout();

    /**
        Same as Add(), but prepends the items to the beginning of the
        list of items (windows, subsizers or spaces) owned by this sizer.
    */
    wxSizerItem* Prepend(wxWindow* window, const wxSizerFlags& flags);

    /**
        Same as Add(), but prepends the items to the beginning of the
        list of items (windows, subsizers or spaces) owned by this sizer.
    */
    wxSizerItem* Prepend(wxWindow* window, int proportion = 0,
                         int flag = 0,
                         int border = 0,
                         wxObject* userData = NULL);

    /**
        Same as Add(), but prepends the items to the beginning of the
        list of items (windows, subsizers or spaces) owned by this sizer.
    */
    wxSizerItem* Prepend(wxSizer* sizer,
                         const wxSizerFlags& flags);

    /**
        Same as Add(), but prepends the items to the beginning of the
        list of items (windows, subsizers or spaces) owned by this sizer.
    */
    wxSizerItem* Prepend(wxSizer* sizer, int proportion = 0,
                         int flag = 0,
                         int border = 0,
                         wxObject* userData = NULL);

    /**
        Same as Add(), but prepends the items to the beginning of the
        list of items (windows, subsizers or spaces) owned by this sizer.
    */
    wxSizerItem* Prepend(int width, int height,
                         int proportion = 0,
                         int flag = 0,
                         int border = 0,
                         wxObject* userData = NULL);

    /**
        Prepends non-stretchable space to the sizer. More readable way of
        calling wxSizer::Prepend(size, size, 0).
    */
    wxSizerItem* PrependSpacer(int size);

    /**
        Prepends stretchable space to the sizer. More readable way of calling
        wxSizer::Prepend(0, 0, prop).
    */
    wxSizerItem* PrependStretchSpacer(int prop = 1);

    /**
        This method is abstract and has to be overwritten by any derived class.
        Here, the sizer will do the actual calculation of its children's
        positions and sizes.
    */
    void RecalcSizes();

    /**
        Removes a child window from the sizer, but does @b not destroy it
        (because windows are owned by their parent window, not the sizer).

        @deprecated
        The overload of this method taking a wxWindow* parameter
        is deprecated as it does not destroy the window as would usually be
        expected from Remove(). You should use Detach() in new code instead.
        There is currently no wxSizer method that will both detach and destroy
        a wxWindow item.

        @note This method does not cause any layout or resizing to take
              place, call Layout() to update the layout "on screen" after
              removing a child from the sizer.

        @return @true if the child item was found and removed, @false otherwise.
    */
    bool Remove(wxWindow* window);

    /**
        Removes a sizer child from the sizer and destroys it.

        @note This method does not cause any layout or resizing to take
              place, call Layout() to update the layout "on screen" after
              removing a child from the sizer.

        @param sizer The wxSizer to be removed.

        @return @true if the child item was found and removed, @false otherwise.
    */
    bool Remove(wxSizer* sizer);

    /**
        Removes a child from the sizer and destroys it if it is a sizer or a
        spacer, but not if it is a window (because windows are owned by their
        parent window, not the sizer).

        @note This method does not cause any layout or resizing to take
              place, call Layout() to update the layout "on screen" after
              removing a child from the sizer.

        @param index  The position of the child in the sizer, e.g. 0 for the
                      first item.

        @return @true if the child item was found and removed, @false otherwise.
    */
    bool Remove(size_t index);

    //@{
    /**
        Detaches the given @e oldwin, @a oldsz child from the sizer and
        replaces it with the given window, sizer, or wxSizerItem.
        The detached child is removed @b only if it is a sizer or a spacer
        (because windows are owned by their parent window, not the sizer).
        Use parameter @a recursive to search the given element recursively in subsizers.

        This method does not cause any layout or resizing to take place, call
        Layout() to update the layout "on screen" after replacing a
        child from the sizer.
        Returns @true if the child item was found and removed, @false otherwise.
    */
    bool Replace(wxWindow* oldwin, wxWindow* newwin,
                 bool recursive = false);
    bool Replace(wxSizer* oldsz, wxSizer* newsz,
                 bool recursive = false);
    bool Remove(size_t oldindex, wxSizerItem* newitem);
    //@}

    /**
        Call this to force the sizer to take the given dimension and thus force
        the items owned by the sizer to resize themselves according to the
        rules defined by the parameter in the Add() and Prepend() methods.
    */
    void SetDimension(int x, int y, int width, int height);
    void SetDimension(const wxPoint& pos, const wxSize& size);

    //@{
    /**
        Set an item's minimum size by window, sizer, or position.

        The item will be found recursively in the sizer's descendants. This
        function enables an application to set the size of an item after
        initial creation.

        @see wxSizerItem::SetMinSize()
    */
    void SetItemMinSize(wxWindow* window, int width, int height);
    void SetItemMinSize(wxSizer* sizer, int width, int height);
    void SetItemMinSize(size_t index, int width, int height);
    //@}

    /**
        Call this to give the sizer a minimal size. Normally, the sizer will
        calculate its minimal size based purely on how much space its children
        need. After calling this method GetMinSize() will return either the
        minimal size as requested by its children or the minimal size set here,
        depending on which is bigger.
    */
    void SetMinSize(const wxSize& size);

    /**
        @overload
     */
    void SetMinSize(int width, int height);

    /**
        This method first calls Fit() and then
        wxTopLevelWindow::SetSizeHints on the @e window
        passed to it. This only makes sense when @a window is actually a
        wxTopLevelWindow such as a wxFrame or a
        wxDialog, since SetSizeHints only has any effect in these classes.
        It does nothing in normal windows or controls.
        This method is implicitly used by wxWindow::SetSizerAndFit
        which is commonly invoked in the constructor of a toplevel window itself (see
        the sample in the description of wxBoxSizer) if the
        toplevel window is resizable.
    */
    void SetSizeHints(wxWindow* window);

    /**
        Tell the sizer to set the minimal size of the @a window virtual area to match
        the sizer's
        minimal size. For windows with managed scrollbars this will set them
        appropriately.

        @see wxScrolled::SetScrollbars()
    */
    void SetVirtualSizeHints(wxWindow* window);

    //@{
    /**
        Shows or hides the @e window, @e sizer, or item at @e index.
        To make a sizer item disappear or reappear, use Show() followed by Layout().
        Use parameter @a recursive to show or hide elements found in subsizers.
        Returns @true if the child item was found, @false otherwise.

        @see Hide(), IsShown()
    */
    bool Show(wxWindow* window, bool show = true,
              bool recursive = false);
    bool Show(wxSizer* sizer, bool show = true,
              bool recursive = false);
    bool Show(size_t index, bool show = true);
    //@}
};



/**
    @class wxGridSizer
    @wxheader{sizer.h}

    A grid sizer is a sizer which lays out its children in a two-dimensional
    table with all table fields having the same size,
    i.e. the width of each field is the width of the widest child,
    the height of each field is the height of the tallest child.

    @library{wxcore}
    @category{winlayout}

    @see wxSizer, @ref overview_sizeroverview "Sizer overview"
*/
class wxGridSizer : public wxSizer
{
public:
    //@{
    /**
        Constructor for a wxGridSizer. @a rows and @a cols determine the number of
        columns and rows in the sizer - if either of the parameters is zero, it will be
        calculated to form the total number of children in the sizer, thus making the
        sizer grow dynamically. @a vgap and @a hgap define extra space between
        all children.
    */
    wxGridSizer(int rows, int cols, int vgap, int hgap);
    wxGridSizer(int cols, int vgap = 0, int hgap = 0);
    //@}

    /**
        Returns the number of columns in the sizer.
    */
    int GetCols();

    /**
        Returns the horizontal gap (in pixels) between cells in the sizer.
    */
    int GetHGap();

    /**
        Returns the number of rows in the sizer.
    */
    int GetRows();

    /**
        Returns the vertical gap (in pixels) between the cells in the sizer.
    */
    int GetVGap();

    /**
        Sets the number of columns in the sizer.
    */
    void SetCols(int cols);

    /**
        Sets the horizontal gap (in pixels) between cells in the sizer.
    */
    void SetHGap(int gap);

    /**
        Sets the number of rows in the sizer.
    */
    void SetRows(int rows);

    /**
        Sets the vertical gap (in pixels) between the cells in the sizer.
    */
    void SetVGap(int gap);
};



/**
    @class wxStaticBoxSizer
    @wxheader{sizer.h}

    wxStaticBoxSizer is a sizer derived from wxBoxSizer but adds a static
    box around the sizer. This static box may be either created independently or
    the sizer may create it itself as a convenience. In any case, the sizer owns
    the wxStaticBox control and will delete it if it is
    deleted.

    @library{wxcore}
    @category{winlayout}

    @see wxSizer, wxStaticBox, wxBoxSizer, @ref overview_sizeroverview "Sizer
    overview"
*/
class wxStaticBoxSizer : public wxBoxSizer
{
public:
    //@{
    /**
        The first constructor uses an already existing static box. It takes the
        associated static box and the orientation @e orient, which can be either
        @c wxVERTICAL or @c wxHORIZONTAL as parameters.
        The second one creates a new static box with the given label and parent window.
    */
    wxStaticBoxSizer(wxStaticBox* box, int orient);
    wxStaticBoxSizer(int orient, wxWindow parent,
                     const wxString& label = wxEmptyString);
    //@}

    /**
        Returns the static box associated with the sizer.
    */
    wxStaticBox* GetStaticBox();
};



/**
    @class wxBoxSizer
    @wxheader{sizer.h}

    The basic idea behind a box sizer is that windows will most often be laid out
    in rather
    simple basic geometry, typically in a row or a column or several hierarchies of
    either.

    For more information, please see @ref overview_boxsizerprogramming "Programming
    with wxBoxSizer".

    @library{wxcore}
    @category{winlayout}

    @see wxSizer, @ref overview_sizeroverview "Sizer overview"
*/
class wxBoxSizer : public wxSizer
{
public:
    /**
        Constructor for a wxBoxSizer. @a orient may be either of wxVERTICAL
        or wxHORIZONTAL for creating either a column sizer or a row sizer.
    */
    wxBoxSizer(int orient);

    /**
        Implements the calculation of a box sizer's minimal. It is used internally
        only and must not be called by the user. Documented for information.
    */
    wxSize CalcMin();

    /**
        Returns the orientation of the box sizer, either wxVERTICAL
        or wxHORIZONTAL.
    */
    int GetOrientation();

    /**
        Implements the calculation of a box sizer's dimensions and then sets
        the size of its children (calling wxWindow::SetSize
        if the child is a window). It is used internally only and must not be called
        by the user (call Layout() if you want to resize). Documented for information.
    */
    void RecalcSizes();
};

