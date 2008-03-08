/////////////////////////////////////////////////////////////////////////////
// Name:        clntdata.h
// Purpose:     documentation for wxClientDataContainer class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxClientDataContainer
    @wxheader{clntdata.h}
    
    This class is a mixin that provides storage and management of "client
    data." This data can either be of type void - in which case the data
    @e container does not take care of freeing the data again
    or it is of type wxClientData or its derivatives. In that case the
    container will free the memory itself later.
    Note that you @e must not assign both void data and data
    derived from the wxClientData class to a container.
    
    NOTE: This functionality is currently duplicated in wxEvtHandler in
    order to avoid having more than one vtable in that class hierarchy.
    
    @library{wxbase}
    @category{FIXME}
    
    @seealso
    wxEvtHandler, wxClientData
*/
class wxClientDataContainer 
{
public:
    /**
        
    */
    wxClientDataContainer();

    /**
        
    */
    ~wxClientDataContainer();

    /**
        Get the untyped client data.
    */
    void* GetClientData();

    /**
        Get a pointer to the client data object.
    */
    wxClientData* GetClientObject();

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
    @wxheader{clntdata.h}
    
    All classes deriving from wxEvtHandler
    (such as all controls and wxApp)
    can hold arbitrary data which is here referred to as "client data".
    This is useful e.g. for scripting languages which need to handle
    shadow objects for most of wxWidgets' classes and which store
    a handle to such a shadow class as client data in that class.
    This data can either be of type void - in which case the data
    @e container does not take care of freeing the data again
    or it is of type wxClientData or its derivatives. In that case the
    container (e.g. a control) will free the memory itself later.
    Note that you @e must not assign both void data and data
    derived from the wxClientData class to a container.
    
    Some controls can hold various items and these controls can
    additionally hold client data for each item. This is the case for
    wxChoice, wxComboBox
    and wxListBox. wxTreeCtrl
    has a specialized class wxTreeItemData
    for each item in the tree.
    
    If you want to add client data to your own classes, you may
    use the mix-in class wxClientDataContainer.
    
    @library{wxbase}
    @category{FIXME}
    
    @seealso
    wxEvtHandler, wxTreeItemData, wxStringClientData, wxClientDataContainer
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
    ~wxClientData();
};


/**
    @class wxStringClientData
    @wxheader{clntdata.h}
    
    Predefined client data class for holding a string.
    
    @library{wxbase}
    @category{FIXME}
*/
class wxStringClientData : public wxClientData
{
public:
    //@{
    /**
        Create client data with string.
    */
    wxStringClientData();
        wxStringClientData(const wxString& data);
    //@}

    /**
        Get string client data.
    */
    const wxString GetData();

    /**
        Set string client data.
    */
    void SetData(const wxString& data);
};
