/////////////////////////////////////////////////////////////////////////////
// Name:        artprov.h
// Purpose:     interface of wxArtProvider
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    This type identifies the client of the art objects requested to wxArtProvider.
*/
typedef wxString wxArtClient;

/**
    This type identifies a specific art object which can be requested to wxArtProvider.
*/
typedef wxString wxArtID;


const char* wxART_TOOLBAR;
const char* wxART_MENU;
const char* wxART_FRAME_ICON;

const char* wxART_CMN_DIALOG;
const char* wxART_HELP_BROWSER;
const char* wxART_MESSAGE_BOX;
const char* wxART_BUTTON;
const char* wxART_LIST;

const char* wxART_OTHER;


const char* wxART_ADD_BOOKMARK;
const char* wxART_DEL_BOOKMARK;
const char* wxART_HELP_SIDE_PANEL;
const char* wxART_HELP_SETTINGS;
const char* wxART_HELP_BOOK;
const char* wxART_HELP_FOLDER;
const char* wxART_HELP_PAGE;
const char* wxART_GO_BACK;
const char* wxART_GO_FORWARD;
const char* wxART_GO_UP;
const char* wxART_GO_DOWN;
const char* wxART_GO_TO_PARENT;
const char* wxART_GO_HOME;
const char* wxART_GOTO_FIRST;
const char* wxART_GOTO_LAST;
const char* wxART_FILE_OPEN;
const char* wxART_FILE_SAVE;
const char* wxART_FILE_SAVE_AS;
const char* wxART_PRINT;
const char* wxART_HELP;
const char* wxART_TIP;
const char* wxART_REPORT_VIEW;
const char* wxART_LIST_VIEW;
const char* wxART_NEW_DIR;
const char* wxART_HARDDISK;
const char* wxART_FLOPPY;
const char* wxART_CDROM;
const char* wxART_REMOVABLE;
const char* wxART_FOLDER;
const char* wxART_FOLDER_OPEN;
const char* wxART_GO_DIR_UP;
const char* wxART_EXECUTABLE_FILE;
const char* wxART_NORMAL_FILE;
const char* wxART_TICK_MARK;
const char* wxART_CROSS_MARK;
const char* wxART_ERROR;
const char* wxART_QUESTION;
const char* wxART_WARNING;
const char* wxART_INFORMATION;
const char* wxART_MISSING_IMAGE;

const char* wxART_COPY;
const char* wxART_CUT;
const char* wxART_PASTE;
const char* wxART_DELETE;
const char* wxART_NEW;

const char* wxART_UNDO;
const char* wxART_REDO;

const char* wxART_PLUS;
const char* wxART_MINUS;

const char* wxART_CLOSE;
const char* wxART_QUIT;

const char* wxART_FIND;
const char* wxART_FIND_AND_REPLACE;

const char* wxART_FULL_SCREEN;
const char* wxART_EDIT;

const char* wxART_WX_LOGO;


/**
    @class wxArtProvider

    wxArtProvider class is used to customize the look of wxWidgets application.

    When wxWidgets needs to display an icon or a bitmap (e.g. in the standard file
    dialog), it does not use a hard-coded resource but asks wxArtProvider for it
    instead. This way users can plug in their own wxArtProvider class and easily
    replace standard art with their own version.

    All that is needed is to derive a class from wxArtProvider, override either its
    wxArtProvider::CreateBitmap() and/or its wxArtProvider::CreateIconBundle() methods
    and register the provider with wxArtProvider::Push():

    @code
      class MyProvider : public wxArtProvider
      {
      protected:
        // Override this method to return a bundle containing the required
        // bitmap in all available sizes.
        wxBitmapBundle CreateBitmapBundle(const wxArtID& id,
                                          const wxArtClient& client,
                                          const wxSize& size) override;

        // If all bitmaps are available in a single size only, it may be
        // simpler to override just this one.
        wxBitmap CreateBitmap(const wxArtID& id,
                              const wxArtClient& client,
                              const wxSize& size) override;

        // optionally override this one as well
        wxIconBundle CreateIconBundle(const wxArtID& id,
                                      const wxArtClient& client) override;
        { ... }
      };
      ...
      wxArtProvider::Push(new MyProvider);
    @endcode

    If you need bitmap images (of the same artwork) that should be displayed at
    different sizes you should probably consider overriding wxArtProvider::CreateIconBundle
    and supplying icon bundles that contain different bitmap sizes.

    There's another way of taking advantage of this class: you can use it in your
    code and use platform native icons as provided by wxArtProvider::GetBitmapBundle
    or wxArtProvider::GetIcon.

    @section artprovider_identify Identifying art resources

    Every bitmap and icon bundle are known to wxArtProvider under a unique ID that
    is used when requesting a resource from it. The ID is represented by the ::wxArtID type
    and can have one of these predefined values (you can see bitmaps represented by these
    constants in the @ref page_samples_artprov):

    <table>
    <tr><td>
     @li @c wxART_ERROR
     @li @c wxART_QUESTION
     @li @c wxART_WARNING
     @li @c wxART_INFORMATION
     @li @c wxART_ADD_BOOKMARK
     @li @c wxART_DEL_BOOKMARK
     @li @c wxART_HELP_SIDE_PANEL
     @li @c wxART_HELP_SETTINGS
     @li @c wxART_HELP_BOOK
     @li @c wxART_HELP_FOLDER
     @li @c wxART_HELP_PAGE
     @li @c wxART_GO_BACK
     @li @c wxART_GO_FORWARD
     @li @c wxART_GO_UP
     @li @c wxART_GO_DOWN
     @li @c wxART_GO_TO_PARENT
     @li @c wxART_GO_HOME
     @li @c wxART_GOTO_FIRST (since 2.9.2)
     </td><td>
     @li @c wxART_GOTO_LAST (since 2.9.2)
     @li @c wxART_PRINT
     @li @c wxART_HELP
     @li @c wxART_TIP
     @li @c wxART_REPORT_VIEW
     @li @c wxART_LIST_VIEW
     @li @c wxART_NEW_DIR
     @li @c wxART_FOLDER
     @li @c wxART_FOLDER_OPEN
     @li @c wxART_GO_DIR_UP
     @li @c wxART_EXECUTABLE_FILE
     @li @c wxART_NORMAL_FILE
     @li @c wxART_TICK_MARK
     @li @c wxART_CROSS_MARK
     @li @c wxART_MISSING_IMAGE
     @li @c wxART_NEW
     @li @c wxART_FILE_OPEN
     @li @c wxART_FILE_SAVE
     </td><td>
     @li @c wxART_FILE_SAVE_AS
     @li @c wxART_DELETE
     @li @c wxART_COPY
     @li @c wxART_CUT
     @li @c wxART_PASTE
     @li @c wxART_UNDO
     @li @c wxART_REDO
     @li @c wxART_PLUS (since 2.9.2)
     @li @c wxART_MINUS (since 2.9.2)
     @li @c wxART_CLOSE
     @li @c wxART_QUIT
     @li @c wxART_FIND
     @li @c wxART_FIND_AND_REPLACE
     @li @c wxART_FULL_SCREEN (since 3.1.0)
     @li @c wxART_EDIT (since 3.1.0)
     @li @c wxART_HARDDISK
     @li @c wxART_FLOPPY
     @li @c wxART_CDROM
     @li @c wxART_REMOVABLE
     @li @c wxART_WX_LOGO (since 3.1.6)
    </td></tr>
    </table>

    @note When building with @c wxNO_IMPLICIT_WXSTRING_ENCODING defined (see
          @ref overview_string for more details), you need to explicitly use
          wxASCII_STR() around these constants.

    Additionally, any string recognized by custom art providers registered using
    wxArtProvider::Push may be used.

    @note
    When running under GTK+ 2, GTK+ stock item IDs (e.g. @c "gtk-cdrom") may be used
    as well:
    @code
    #ifdef __WXGTK__
        wxBitmap bmp = wxArtProvider::GetBitmap("gtk-cdrom", wxART_MENU);
    #endif
    @endcode
    For a list of the GTK+ stock items please refer to the
    <a href="https://developer-old.gnome.org/gtk3/stable/gtk3-Stock-Items.html">GTK+ documentation
    page</a>.
    It is also possible to load icons from the current icon theme by specifying their name
    (without extension and directory components).
    Icon themes recognized by GTK+ follow the freedesktop.org
    <a href="http://freedesktop.org/Standards/icon-theme-spec">Icon Themes specification</a>.
    Note that themes are not guaranteed to contain all icons, so wxArtProvider may
    return ::wxNullBitmap or ::wxNullIcon.
    The default theme is typically installed in @c /usr/share/icons/hicolor.


    @section artprovider_clients Clients

    The @e client is the entity that calls wxArtProvider's GetBitmap() or GetIcon() function.
    It is represented by wxClientID type and can have one of these values:

    @li @c wxART_TOOLBAR
    @li @c wxART_MENU
    @li @c wxART_BUTTON
    @li @c wxART_FRAME_ICON
    @li @c wxART_CMN_DIALOG
    @li @c wxART_HELP_BROWSER
    @li @c wxART_MESSAGE_BOX
    @li @c wxART_OTHER (used for all requests that don't fit into any of the
        categories above)

    Client ID serve as a hint to wxArtProvider that is supposed to help it to
    choose the best looking bitmap. For example it is often desirable to use
    slightly different icons in menus and toolbars even though they represent
    the same action (e.g. wxART_FILE_OPEN). Remember that this is really only a
    hint for wxArtProvider -- it is common that wxArtProvider::GetBitmap returns
    identical bitmap for different client values!

    @library{wxcore}
    @category{misc}

    @see @ref page_samples_artprov for an example of wxArtProvider usage;
         @ref page_stockitems "stock ID list"
*/
class wxArtProvider : public wxObject
{
public:
    /**
        The destructor automatically removes the provider from the provider stack used
        by GetBitmap().
    */
    virtual ~wxArtProvider();

    /**
        Delete the given @a provider.
    */
    static bool Delete(wxArtProvider* provider);

    /**
        Query registered providers for bitmap with given ID.

        Note that applications using wxWidgets 3.1.6 or later should prefer
        calling GetBitmapBundle().

        @param id
            wxArtID unique identifier of the bitmap.
        @param client
            wxArtClient identifier of the client (i.e. who is asking for the bitmap).
        @param size
            Size of the returned bitmap or wxDefaultSize if size doesn't matter.

        @return The bitmap if one of registered providers recognizes the ID or
                wxNullBitmap otherwise.
    */
    static wxBitmap GetBitmap(const wxArtID& id,
                              const wxArtClient& client = wxART_OTHER,
                              const wxSize& size = wxDefaultSize);

    /**
        Query registered providers for a bundle of bitmaps with given ID.

        @since 3.1.6

        @param id
            wxArtID unique identifier of the bitmap.
        @param client
            wxArtClient identifier of the client (i.e. who is asking for the bitmap).
        @param size
            Default size for the returned bundle, i.e. the size of the bitmap
            in normal DPI (this implies that wxWindow::FromDIP() must @e not be
            used with it).

        @return If any of the registered providers recognizes the ID in its
            CreateBitmapBundle(), this bundle is returned. Otherwise, if any of
            providers returns a valid bitmap from CreateBitmap(), the bundle
            containing only this bitmap is returned. Otherwise, an empty bundle
            is returned.
     */
    static wxBitmapBundle GetBitmapBundle(const wxArtID& id,
                                          const wxArtClient& client = wxART_OTHER,
                                          const wxSize& size = wxDefaultSize);

    /**
        Same as wxArtProvider::GetBitmap, but return a wxIcon object
        (or ::wxNullIcon on failure).
    */
    static wxIcon GetIcon(const wxArtID& id,
                          const wxArtClient& client = wxART_OTHER,
                          const wxSize& size = wxDefaultSize);

    /**
        Returns native icon size for use specified by @a client hint in DIPs.

        If the platform has no commonly used default for this use or if
        @a client is not recognized, returns wxDefaultSize.

        @note In some cases, a platform may have @em several appropriate
              native sizes (for example, wxART_FRAME_ICON for frame icons).
              In that case, this method returns only one of them, picked
              reasonably.

        @since 3.1.6
     */
    static wxSize GetNativeDIPSizeHint(const wxArtClient& client);

    /**
        Returns native icon size for use specified by @a client hint.

        This function does the same thing as GetNativeDIPSizeHint(), but uses
        @a win to convert the returned value to logical pixels. If @a win is
        @NULL, default DPI scaling (i.e. that of the primary display) is used.

        @since 2.9.0 (@a win parameter is available only since 3.1.6)
     */
    static wxSize GetNativeSizeHint(const wxArtClient& client, wxWindow* win = nullptr);

    /**
        Returns a suitable size hint for the given @e wxArtClient in DIPs.

        Return the size used by the topmost wxArtProvider for the given @a
        client. @e wxDefaultSize may be returned if the client doesn't have a
        specified size, like wxART_OTHER for example.

        @see GetNativeDIPSizeHint()
    */
    static wxSize GetDIPSizeHint(const wxArtClient& client);

    /**
        Returns a suitable size hint for the given @e wxArtClient.

        This function does the same thing as GetDIPSizeHint(), but uses @a win
        to convert the returned value to logical pixels. If @a win is @NULL,
        default DPI scaling (i.e. that of the primary display) is used.

        Note that @a win parameter is only available since wxWidgets 3.1.6.
     */
    static wxSize GetSizeHint(const wxArtClient& client, wxWindow* win = nullptr);

    /**
        Query registered providers for icon bundle with given ID.

        @param id
            wxArtID unique identifier of the icon bundle.
        @param client
            wxArtClient identifier of the client (i.e. who is asking for the icon
            bundle).

        @return The icon bundle if one of registered providers recognizes the ID
                or wxNullIconBundle otherwise.
    */
    static wxIconBundle GetIconBundle(const wxArtID& id,
                                      const wxArtClient& client = wxART_OTHER);

    /**
        Returns true if the platform uses native icons provider that should
        take precedence over any customizations.

        This is true for any platform that has user-customizable icon themes,
        currently only wxGTK.

        A typical use for this method is to decide whether a custom art provider
        should be plugged in using Push() or PushBack().

        @since 2.9.0
     */
    static bool HasNativeProvider();

    /**
        @deprecated Use PushBack() instead.
    */
    static void Insert(wxArtProvider* provider);

    /**
        Remove latest added provider and delete it.
    */
    static bool Pop();

    /**
        Register new art provider and add it to the top of providers stack
        (i.e. it will be queried as the first provider).

        @see PushBack()
    */
    static void Push(wxArtProvider* provider);

    /**
        Register new art provider and add it to the bottom of providers stack.
        In other words, it will be queried as the last one, after all others,
        including the default provider.

        @see Push()

        @since 2.9.0
    */
    static void PushBack(wxArtProvider* provider);

    /**
        Remove a provider from the stack if it is on it. The provider is not
        deleted, unlike when using Delete().
    */
    static bool Remove(wxArtProvider* provider);

    /**
     * Helper used by GetMessageBoxIcon(): return the art id corresponding to
     * the standard wxICON_INFORMATION/WARNING/ERROR/QUESTION flags (only one
     * can be set)
     */
    static wxArtID GetMessageBoxIconId(int flags);

    /**
     * Helper used by several generic classes: return the icon corresponding to
     * the standard wxICON_INFORMATION/WARNING/ERROR/QUESTION flags (only one
     * can be set)
     */
    static wxIcon GetMessageBoxIcon(int flags);


protected:
    /**
        Derived art provider classes may override this method to return the
        size of the images used by this provider.

        Note that the returned size should be in DPI-independent pixels, i.e.
        DIPs. The default implementation returns the result of
        GetNativeDIPSizeHint().
     */
    virtual wxSize DoGetSizeHint(const wxArtClient& client);

    /**
        Derived art provider classes may override this method to create requested art
        resource.

        For bitmaps available in more than one size, CreateBitmapBundle()
        should be overridden instead.

        Note that returned bitmaps are cached by wxArtProvider and it is
        therefore not necessary to optimize CreateBitmap() for speed (e.g. you may
        create wxBitmap objects from XPMs here).

        @param id
            wxArtID unique identifier of the bitmap.
        @param client
            wxArtClient identifier of the client (i.e. who is asking for the bitmap).
            This only serves as a hint.
        @param size
            Preferred size of the bitmap. The function may return a bitmap of different
            dimensions, it will be automatically rescaled to meet client's request.

        @note
        This is not part of wxArtProvider's public API, use wxArtProvider::GetBitmap
        or wxArtProvider::GetIconBundle or wxArtProvider::GetIcon to query wxArtProvider
        for a resource.

        @see CreateIconBundle()
    */
    virtual wxBitmap CreateBitmap(const wxArtID& id,
                                  const wxArtClient& client,
                                  const wxSize& size);

    /**
        Override this method to create the requested art resources available in
        more than one size.

        Unlike CreateBitmap(), this method can be overridden to return the same
        bitmap in several (or all, if wxBitmapBundle::FromSVG() is used) sizes
        at once, which will allow selecting the size best suited for the
        current display resolution automatically.

        @param id
            wxArtID unique identifier of the bitmap.
        @param client
            wxArtClient identifier of the client (i.e. who is asking for the bitmap).
            This only serves as a hint.
        @param size
            Default size of the bitmaps returned by the bundle.

        @since 3.1.6
     */
    virtual wxBitmapBundle CreateBitmapBundle(const wxArtID& id,
                                              const wxArtClient& client,
                                              const wxSize& size);

    /**
        This method is similar to CreateBitmap() but can be used when a bitmap
        (or an icon) exists in several sizes.
    */
    virtual wxIconBundle CreateIconBundle(const wxArtID& id,
                                          const wxArtClient& client);
};

