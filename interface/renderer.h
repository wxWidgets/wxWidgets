/////////////////////////////////////////////////////////////////////////////
// Name:        renderer.h
// Purpose:     interface of wxSplitterRenderParams
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxSplitterRenderParams
    @wxheader{renderer.h}

    This is just a simple @c struct used as a return value of
    wxRendererNative::GetSplitterParams.

    It doesn't have any methods and all of its fields are constant and so can be
    only examined but not modified.

    @library{wxbase}
    @category{FIXME}
*/
class wxSplitterRenderParams
{
public:
    /**
        const wxCoord border
        The width of the border drawn by the splitter inside it, may be 0.
    */


    /**
        const bool isHotSensitive
        @true if the sash changes appearance when the mouse passes over it, @false
        otherwise.
    */


    /**
        const wxCoord widthSash
        The width of the splitter sash.
    */
};



/**
    @class wxDelegateRendererNative
    @wxheader{renderer.h}

    wxDelegateRendererNative allows reuse of renderers code by forwarding all the
    wxRendererNative methods to the given object and
    thus allowing you to only modify some of its methods -- without having to
    reimplement all of them.

    Note that the "normal'', inheritance-based approach, doesn't work with the
    renderers as it is impossible to derive from a class unknown at compile-time
    and the renderer is only chosen at run-time. So suppose that you want to only
    add something to the drawing of the tree control buttons but leave all the
    other methods unchanged -- the only way to do it, considering that the renderer
    class which you want to customize might not even be written yet when you write
    your code (it could be written later and loaded from a DLL during run-time), is
    by using this class.

    Except for the constructor, it has exactly the same methods as
    wxRendererNative and their implementation is
    trivial: they are simply forwarded to the real renderer. Note that the "real''
    renderer may, in turn, be a wxDelegateRendererNative as well and that there may
    be arbitrarily many levels like this -- but at the end of the chain there must
    be a real renderer which does the drawing.

    @library{wxcore}
    @category{FIXME}
*/
class wxDelegateRendererNative : public wxRendererNative
{
public:
    //@{
    /**
        The default constructor does the same thing as the other one except that it
        uses the @ref wxRendererNative::getgeneric "generic renderer" instead of the
        user-specified @e rendererNative.
        In any case, this sets up the delegate renderer object to follow all calls to
        the specified real renderer.
        Note that this object does not take ownership of (i.e. won't delete)
        @e rendererNative.
    */
    wxDelegateRendererNative();
    wxDelegateRendererNative(wxRendererNative& rendererNative);
    //@}

    /**
        This class also provides all the virtual methods of
        wxRendererNative, please refer to that class
        documentation for the details.
    */
    DrawXXX(...);
};



/**
    @class wxRendererNative
    @wxheader{renderer.h}

    First, a brief introduction to wxRenderer and why it is needed.

    Usually wxWidgets uses the underlying low level GUI system to draw all the
    controls - this is what we mean when we say that it is a "native'' framework.
    However not all controls exist under all (or even any) platforms and in this
    case wxWidgets provides a default, generic, implementation of them written in
    wxWidgets itself.

    These controls don't have the native appearance if only the standard
    line drawing and other graphics primitives are used, because the native
    appearance is different under different platforms while the lines are always
    drawn in the same way.

    This is why we have renderers: wxRenderer is a class which virtualizes the
    drawing, i.e. it abstracts the drawing operations and allows you to draw say, a
    button, without caring about exactly how this is done. Of course, as we
    can draw the button differently in different renderers, this also allows us to
    emulate the native look and feel.

    So the renderers work by exposing a large set of high-level drawing functions
    which are used by the generic controls. There is always a default global
    renderer but it may be changed or extended by the user, see
    @ref overview_samplerender "Render sample".

    All drawing functions take some standard parameters:

     @e win is the window being drawn. It is normally not used and when
    it is it should only be used as a generic wxWindow
    (in order to get its low level handle, for example), but you should
    not assume that it is of some given type as the same renderer
    function may be reused for drawing different kinds of control.
     @e dc is the wxDC to draw on. Only this device
    context should be used for drawing. It is not necessary to restore
    pens and brushes for it on function exit but, on the other hand, you
    shouldn't assume that it is in any specific state on function entry:
    the rendering functions should always prepare it.
     @e rect the bounding rectangle for the element to be drawn.
     @e flags the optional flags (none by default) which can be a
    combination of the @c wxCONTROL_XXX constants below.

    Note that each drawing function restores the wxDC attributes if
    it changes them, so it is safe to assume that the same pen, brush and colours
    that were active before the call to this function are still in effect after it.

    @library{wxcore}
    @category{gdi}
*/
class wxRendererNative
{
public:
    /**
        Virtual destructor as for any base class.
    */
    ~wxRendererNative();

    /**
        Draw a check box (used by wxDataViewCtrl).
        @a flags may have the @c wxCONTROL_CHECKED, @c wxCONTROL_CURRENT or
        @c wxCONTROL_UNDETERMINED bit set.
    */
    void DrawCheckBox(wxWindow* win, wxDC& dc, const wxRect& rect,
                      int flags);

    /**
        Draw a button like the one used by wxComboBox to show a
        drop down window. The usual appearance is a downwards pointing arrow.
        @a flags may have the @c wxCONTROL_PRESSED or @c wxCONTROL_CURRENT bit set.
    */
    void DrawComboBoxDropButton(wxWindow* win, wxDC& dc,
                                const wxRect& rect,
                                int flags);

    /**
        Draw a drop down arrow that is suitable for use outside a combo box. Arrow will
        have
        transparent background.
        @a rect is not entirely filled by the arrow. Instead, you should use bounding
        rectangle of a drop down button which arrow matches the size you need.
        @a flags may have the @c wxCONTROL_PRESSED or @c wxCONTROL_CURRENT bit set.
    */
    void DrawDropArrow(wxWindow* win, wxDC& dc, const wxRect& rect,
                       int flags);

    /**
        Draw a focus rectangle using the specified rectangle.
        wxListCtrl. The only supported flags is
        @c wxCONTROL_SELECTED for items which are selected.
    */
    void DrawFocusRect(wxWindow* win, wxDC& dc, const wxRect& rect,
                       int flags = 0);

    /**
        Draw the header control button (used, for example, by
        wxListCtrl).  Depending on platforms the
        @a flags parameter may support the @c wxCONTROL_SELECTED
        @c wxCONTROL_DISABLED and @c wxCONTROL_CURRENT bits.
        The @a sortArrow parameter can be one of
        @c wxHDR_SORT_ICON_NONE, @c wxHDR_SORT_ICON_UP, or
        @c wxHDR_SORT_ICON_DOWN.  Additional values controlling the
        drawing of a text or bitmap label can be passed in @e params.  The
        value returned is the optimal width to contain the the unabreviated
        label text or bitmap, the sort arrow if present, and internal margins.
    */
    int DrawHeaderButton(wxWindow* win, wxDC& dc, const wxRect& rect,
                         int flags = 0,
                         wxHeaderSortIconType sortArrow = wxHDR_SORT_ICON_NONE,
                         wxHeaderButtonParams* params = NULL);

    /**
        Draw a selection rectangle underneath the text as used e.g. in a
        wxListCtrl. The supported @a flags are
        @c wxCONTROL_SELECTED for items which are selected (e.g. often a blue
        rectangle) and @c wxCONTROL_CURRENT for the item that has the focus
        (often a dotted line around the item's text). @c wxCONTROL_FOCUSED may
        be used to indicate if the control has the focus (othewise the the selection
        rectangle is e.g. often grey and not blue). This may be ignored by the renderer
        or deduced by the code directly from the @e win.
    */
    void DrawItemSelectionRect(wxWindow* win, wxDC& dc,
                               const wxRect& rect,
                               int flags = 0);

    /**
        Draw a blank push button that looks very similar to wxButton.
        @a flags may have the @c wxCONTROL_PRESSED, @c wxCONTROL_CURRENT or
        @c wxCONTROL_ISDEFAULT bit set.
    */
    void DrawPushButton(wxWindow* win, wxDC& dc, const wxRect& rect,
                        int flags);

    /**
        Draw the border for sash window: this border must be such that the sash
        drawn by @ref drawsplittersash() DrawSash blends into it
        well.
    */
    void DrawSplitterBorder(wxWindow* win, wxDC& dc,
                            const wxRect& rect,
                            int flags = 0);

    /**
        Draw a sash. The @a orient parameter defines whether the sash should be
        vertical or horizontal and how the @a position should be interpreted.
    */
    void DrawSplitterSash(wxWindow* win, wxDC& dc,
                          const wxSize& size,
                          wxCoord position,
                          wxOrientation orient,
                          int flags = 0);

    /**
        Draw the expanded/collapsed icon for a tree control item. To draw an expanded
        button the @a flags parameter must contain @c wxCONTROL_EXPANDED bit.
    */
    void DrawTreeItemButton(wxWindow* win, wxDC& dc,
                            const wxRect& rect,
                            int flags = 0);

    /**
        Return the currently used renderer.
    */
    wxRendererNative Get();

    /**
        Return the default (native) implementation for this platform -- this is also
        the one used by default but this may be changed by calling
        Set() in which case the return value of this
        method may be different from the return value of Get().
    */
    wxRendererNative GetDefault();

    /**
        Return the generic implementation of the renderer. Under some platforms, this
        is the default renderer implementation, others have platform-specific default
        renderer which can be retrieved by calling GetDefault().
    */
    wxRendererNative GetGeneric();

    /**
        Returns the height of a header button, either a fixed platform height if
        available, or a
        generic height based on the window's font.
    */
    int GetHeaderButtonHeight(const wxWindow* win);

    /**
        Get the splitter parameters, see
        wxSplitterRenderParams.
    */
    wxSplitterRenderParams GetSplitterParams(const wxWindow* win);

    /**
        This function is used for version checking: Load()
        refuses to load any shared libraries implementing an older or incompatible
        version.
        The implementation of this method is always the same in all renderers (simply
        construct wxRendererVersion using the
        @c wxRendererVersion::Current_XXX values), but it has to be in the derived,
        not base, class, to detect mismatches between the renderers versions and so you
        have to implement it anew in all renderers.
    */
    wxRendererVersion GetVersion() const;

    /**
        Load the renderer from the specified DLL, the returned pointer must be
        deleted by caller if not @NULL when it is not used any more.
        The @a name should be just the base name of the renderer and not the full
        name of the DLL file which is constructed differently (using
        wxDynamicLibrary::CanonicalizePluginName)
        on different systems.
    */
    wxRendererNative* Load(const wxString& name);

    /**
        Set the renderer to use, passing @NULL reverts to using the default
        renderer (the global renderer must always exist).
        Return the previous renderer used with Set() or @NULL if none.
    */
    wxRendererNative* Set(wxRendererNative* renderer);
};



/**
    @class wxRendererVersion
    @wxheader{renderer.h}

    This simple struct represents the wxRendererNative
    interface version and is only used as the return value of
    wxRendererNative::GetVersion.

    The version has two components: the version itself and the age. If the main
    program and the renderer have different versions they are never compatible with
    each other because the version is only changed when an existing virtual
    function is modified or removed. The age, on the other hand, is incremented
    each time a new virtual method is added and so, at least for the compilers
    using a common C++ object model, the calling program is compatible with any
    renderer which has the age greater or equal to its age. This verification is
    done by IsCompatible() method.

    @library{wxbase}
    @category{FIXME}
*/
class wxRendererVersion
{
public:
    /**
        Checks if the main program is compatible with the renderer having the version
        @e ver, returns @true if it is and @false otherwise.
        This method is used by
        wxRendererNative::Load to determine whether a
        renderer can be used.
    */
    static bool IsCompatible(const wxRendererVersion& ver);

    /**
        const int age
        The age component.
    */


    /**
        const int version
        The version component.
    */
};

