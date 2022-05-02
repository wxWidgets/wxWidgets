/////////////////////////////////////////////////////////////////////////////
// Name:        clntdata.h
// Purpose:     interface of wxClientData[Container] and wxStringClientData
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxClientDataContainer

    This class is a mixin that provides storage and management of "client data".
    This data can either be of type void - in which case the data
    @e container does not take care of freeing the data again or it is of
    type wxClientData or its derivatives. In that case the container will free
    the memory itself later. Note that you @e must not assign both void data
    and data derived from the wxClientData class to a container.

    @note This functionality is currently duplicated in wxEvtHandler in order
          to avoid having more than one vtable in that class hierarchy.

    @library{wxbase}
    @category{containers}

    @see wxSharedClientDataContainer, wxEvtHandler, wxClientData
*/
class wxClientDataContainer
{
public:
    /**
        Default constructor.
    */
    wxClientDataContainer();

    /**
        Destructor.
    */
    virtual ~wxClientDataContainer();

    /**
        Get the untyped client data.
    */
    void* GetClientData() const;

    /**
        Get a pointer to the client data object.
    */
    wxClientData* GetClientObject() const;

    /**
        Set the untyped client data.
    */
    void SetClientData(void* data);

    /**
        Set the client data object. Any previous object will be deleted.
    */
    void SetClientObject(wxClientData* data);
};



/**
    @class wxSharedClientDataContainer

    This class is a replacement for @ref wxClientDataContainer, and unlike
    wxClientDataContainer the wxSharedClientDataContainer client data is
    copiable, so it can be copied when objects containing it are cloned.
    Like wxClientDataContainer, wxSharedClientDataContainer is a mixin
    that provides storage and management of "client data.". The client data
    is reference counted and managed by the container. As the client data
    is a shared object, changing the client data used by any object changes
    it for all other objects, too.

    @note If your class has a Clone function and needs to store client data,
          use wxSharedClientDataContainer and not wxClientDataContainer!

    @library{wxbase}
    @category{containers}

    @see wxClientDataContainer, wxClientData
    @since 3.1.7
*/
class wxSharedClientDataContainer
{
public:
    // Provide the same functions as in wxClientDataContainer, so that objects
    // using it and this class could be used in exactly the same way.

    /**
        Get the untyped client data.
    */
    void* GetClientData() const;

    /**
        Get a pointer to the client data object.
    */
    wxClientData* GetClientObject() const;

    /**
        Set the untyped client data.
    */
    void SetClientData(void* data);

    /**
        Set the client data object. Any previous object will be deleted.
    */
    void SetClientObject(wxClientData* data);
};



/**
    @class wxClientData

    All classes deriving from wxEvtHandler (such as all controls and wxApp) can
    hold arbitrary data which is here referred to as "client data". This is
    useful e.g. for scripting languages which need to handle shadow objects for
    most of wxWidgets' classes and which store a handle to such a shadow class
    as client data in that class. This data can either be of type void - in
    which case the data @e container does not take care of freeing the data
    again or it is of type wxClientData or its derivatives. In that case the
    container (e.g. a control) will free the memory itself later. Note that you
    @e must not assign both void data and data derived from the wxClientData
    class to a container.

    Some controls can hold various items and these controls can additionally
    hold client data for each item. This is the case for wxChoice, wxComboBox
    and wxListBox. wxTreeCtrl has a specialized class wxTreeItemData for each
    item in the tree.

    If you want to add client data to your own classes, you may use the mix-in
    class wxClientDataContainer.

    @library{wxbase}
    @category{containers}

    @see wxEvtHandler, wxTreeItemData, wxStringClientData,
         wxClientDataContainer
*/
class wxClientData
{
public:
    /**
        Constructor.
    */
    wxClientData();

    /**
        Virtual destructor.
    */
    virtual ~wxClientData();
};



/**
    @class wxStringClientData

    Predefined client data class for holding a string.

    @library{wxbase}
    @category{containers}
*/
class wxStringClientData : public wxClientData
{
public:
    /**
        Default constructor.
    */
    wxStringClientData();

    /**
        Create client data with string.
    */
    wxStringClientData(const wxString& data);

    /**
        Get string client data.
    */
    const wxString& GetData() const;

    /**
        Set string client data.
    */
    void SetData(const wxString& data);
};

