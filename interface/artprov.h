/////////////////////////////////////////////////////////////////////////////
// Name:        artprov.h
// Purpose:     documentation for wxArtProvider class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxArtProvider
    @wxheader{artprov.h}

    wxArtProvider class is used to customize the look of wxWidgets application.
    When wxWidgets needs to display an icon or a bitmap (e.g. in the standard file
    dialog), it does not use a hard-coded resource but asks wxArtProvider for it
    instead. This way users can plug in their own wxArtProvider class and easily
    replace standard art with their own version. All
    that is needed is to derive a class from wxArtProvider, override either its
    wxArtProvider::CreateBitmap and/or its
    wxArtProvider::CreateIconBundle methods
    and register the provider with
    wxArtProvider::Push:

    @code
    class MyProvider : public wxArtProvider
      {
      protected:
        wxBitmap CreateBitmap(const wxArtID& id,
                              const wxArtClient& client,
                              const wxSize size)

        // optionally override this one as well
        wxIconBundle CreateIconBundle(const wxArtID& id,
                                      const wxArtClient& client)
        { ... }
      };
      ...
      wxArtProvider::Push(new MyProvider);
    @endcode

    If you need bitmap images (of the same artwork) that should be displayed at
    different sizes
    you should probably consider overriding wxArtProvider::CreateIconBundle
    and supplying icon bundles that contain different bitmap sizes.

    There's another way of taking advantage of this class: you can use it in your
    code and use
    platform native icons as provided by wxArtProvider::GetBitmap or
    wxArtProvider::GetIcon (NB: this is not yet really
    possible as of wxWidgets 2.3.3, the set of wxArtProvider bitmaps is too
    small).


    wxArtProvider::~wxArtProvider
    wxArtProvider::CreateBitmap
    wxArtProvider::CreateIconBundle
    wxArtProvider::Delete
    wxArtProvider::GetBitmap
    wxArtProvider::GetIconBundle
    wxArtProvider::GetIcon
    wxArtProvider::Insert
    wxArtProvider::Pop
    wxArtProvider::Push
    wxArtProvider::Remove


    Identifying art resources

    Every bitmap and icon bundle are known to wxArtProvider under an unique ID that
    is used when
    requesting a resource from it. The ID is represented by wxArtID type and can
    have one of these predefined values (you can see bitmaps represented by these
    constants in the artprov sample):

     wxART_ERROR
     wxART_QUESTION
     wxART_WARNING
     wxART_INFORMATION
     wxART_ADD_BOOKMARK
     wxART_DEL_BOOKMARK
     wxART_HELP_SIDE_PANEL
     wxART_HELP_SETTINGS
     wxART_HELP_BOOK
     wxART_HELP_FOLDER
     wxART_HELP_PAGE
     wxART_GO_BACK
     wxART_GO_FORWARD
     wxART_GO_UP
     wxART_GO_DOWN
     wxART_GO_TO_PARENT
     wxART_GO_HOME
     wxART_PRINT
     wxART_HELP
     wxART_TIP
     wxART_REPORT_VIEW
     wxART_LIST_VIEW
     wxART_NEW_DIR
     wxART_FOLDER
     wxART_FOLDER_OPEN
     wxART_GO_DIR_UP
     wxART_EXECUTABLE_FILE
     wxART_NORMAL_FILE
     wxART_TICK_MARK
     wxART_CROSS_MARK
     wxART_MISSING_IMAGE
     wxART_NEW
     wxART_FILE_OPEN
     wxART_FILE_SAVE
     wxART_FILE_SAVE_AS
     wxART_DELETE
     wxART_COPY
     wxART_CUT
     wxART_PASTE
     wxART_UNDO
     wxART_REDO
     wxART_QUIT
     wxART_FIND
     wxART_FIND_AND_REPLACE
     wxART_HARDDISK
     wxART_FLOPPY
     wxART_CDROM
     wxART_REMOVABLE


    Additionally, any string recognized by custom art providers registered using
    wxArtProvider::Push may be used.

    @library{wxcore}
    @category{FIXME}

    @seealso
    See the artprov sample for an example of wxArtProvider usage.
*/
class wxArtProvider : public wxObject
{
public:
    /**
        The destructor automatically removes the provider from the provider stack used
        by GetBitmap().
    */
    ~wxArtProvider();

    /**
        Client is the entity that calls wxArtProvider's GetBitmap or GetIcon
        function. It is represented by wxClientID type and can have one of these
        values:
         wxART_TOOLBAR
         wxART_MENU
         wxART_BUTTON
         wxART_FRAME_ICON
         wxART_CMN_DIALOG
         wxART_HELP_BROWSER
         wxART_MESSAGE_BOX
         wxART_OTHER (used for all requests that don't fit into any of the categories
        above)
        Client ID servers as a hint to wxArtProvider that is supposed to help it to
        choose the best looking bitmap. For example it is often desirable to use
        slightly different icons in menus and toolbars even though they represent the
        same action (e.g. @c wx_ART_FILE_OPEN). Remember that this is really
        only a hint for wxArtProvider -- it is common that
        GetBitmap()
        returns identical bitmap for different @e client values!
        
        @see See the artprov sample for an example of wxArtProvider usage.
    */


    /**
        Derived art provider classes must override this method to create requested art
        resource. Note that returned bitmaps are cached by wxArtProvider and it is
        therefore not necessary to optimize CreateBitmap() for speed (e.g. you may
        create wxBitmap objects from XPMs here).
        
        @param id
            wxArtID unique identifier of the bitmap.
        @param client
            wxArtClient identifier of the client (i.e. who is asking for the bitmap).
            This only servers as a hint.
        @param size
            Preferred size of the bitmap. The function may return a bitmap of different
            dimensions, it will be automatically rescaled to meet client's request.
        
        @see CreateIconBundle()
    */
    wxBitmap CreateBitmap(const wxArtID& id,
                          const wxArtClient& client,
                          const wxSize& size);

    /**
        This method is similar to CreateBitmap() but
        can be used when a bitmap (or an icon) exists in several sizes.
    */
    wxIconBundle CreateIconBundle(const wxArtID& id,
                                  const wxArtClient& client);

    /**
        Delete the given @e provider.
    */
    static bool Delete(wxArtProvider* provider);

    /**
        Query registered providers for bitmap with given ID.
        
        @param id
            wxArtID unique identifier of the bitmap.
        @param client
            wxArtClient identifier of the client (i.e. who is asking for the bitmap).
        @param size
            Size of the returned bitmap or wxDefaultSize if size doesn't matter.
        
        @returns The bitmap if one of registered providers recognizes the ID or
                 wxNullBitmap otherwise.
    */
    static wxBitmap GetBitmap(const wxArtID& id,
                              const wxArtClient& client = wxART_OTHER,
                              const wxSize& size = wxDefaultSize);

    //@{
    /**
        Returns a suitable size hint for the given @e wxArtClient. If
        @a platform_default is @true, return a size based on the current platform,
        otherwise return the size from the topmost wxArtProvider. @e wxDefaultSize may
        be
        returned if the client doesn't have a specified size, like wxART_OTHER for
        example.
    */
    static wxIcon GetIcon(const wxArtID& id,
                          const wxArtClient& client = wxART_OTHER,
                          const wxSize& size = wxDefaultSize);
    static wxSize GetSizeHint(const wxArtClient& client,
                              bool platform_default = false);
    //@}

    /**
        Query registered providers for icon bundle with given ID.
        
        @param id
            wxArtID unique identifier of the icon bundle.
        @param client
            wxArtClient identifier of the client (i.e. who is asking for the icon
        bundle).
        
        @returns The icon bundle if one of registered providers recognizes the ID
                 or wxNullIconBundle otherwise.
    */
    static wxIconBundle GetIconBundle(const wxArtID& id,
                                      const wxArtClient& client = wxART_OTHER);

    /**
        Every bitmap and icon bundle are known to wxArtProvider under an unique ID that
        is used when
        requesting a resource from it. The ID is represented by wxArtID type and can
        have one of these predefined values (you can see bitmaps represented by these
        constants in the artprov sample):
        
         wxART_ERROR
         wxART_QUESTION
         wxART_WARNING
         wxART_INFORMATION
         wxART_ADD_BOOKMARK
         wxART_DEL_BOOKMARK
         wxART_HELP_SIDE_PANEL
         wxART_HELP_SETTINGS
         wxART_HELP_BOOK
         wxART_HELP_FOLDER
         wxART_HELP_PAGE
         wxART_GO_BACK
         wxART_GO_FORWARD
         wxART_GO_UP
         wxART_GO_DOWN
         wxART_GO_TO_PARENT
         wxART_GO_HOME
         wxART_PRINT
         wxART_HELP
         wxART_TIP
         wxART_REPORT_VIEW
         wxART_LIST_VIEW
         wxART_NEW_DIR
         wxART_FOLDER
         wxART_FOLDER_OPEN
         wxART_GO_DIR_UP
         wxART_EXECUTABLE_FILE
         wxART_NORMAL_FILE
         wxART_TICK_MARK
         wxART_CROSS_MARK
         wxART_MISSING_IMAGE
         wxART_NEW
         wxART_FILE_OPEN
         wxART_FILE_SAVE
         wxART_FILE_SAVE_AS
         wxART_DELETE
         wxART_COPY
         wxART_CUT
         wxART_PASTE
         wxART_UNDO
         wxART_REDO
         wxART_QUIT
         wxART_FIND
         wxART_FIND_AND_REPLACE
         wxART_HARDDISK
         wxART_FLOPPY
         wxART_CDROM
         wxART_REMOVABLE
        Additionally, any string recognized by custom art providers registered using
        Push() may be used.
    */


    /**
        Register new art provider and add it to the bottom of providers stack (i.e.
        it will be queried as the last one).
        
        @see Push()
    */
    static void Insert(wxArtProvider* provider);

    /**
        Remove latest added provider and delete it.
    */
    static bool Pop();

    /**
        Register new art provider and add it to the top of providers stack (i.e. it
        will be queried as the first provider).
        
        @see Insert()
    */
    static void Push(wxArtProvider* provider);

    /**
        Remove a provider from the stack if it is on it. The provider is not
        deleted, unlike when using Delete().
    */
    static bool Remove(wxArtProvider* provider);
};
