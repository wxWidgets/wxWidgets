/////////////////////////////////////////////////////////////////////////////
// Name:        msw/ole/activex.h
// Purpose:     documentation for wxActiveXEvent class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxActiveXEvent
    @headerfile ole/activex.h wx/msw/ole/activex.h

    An event class for handling activex events passed from
    wxActiveXContainer. ActiveX events are basically
    a function call with the parameters passed through an array of wxVariants along
    with a return value that is a wxVariant itself. What type the parameters or
    return value are depends on the context (i.e. what the .idl specifies).

    Note that unlike the third party wxActiveX function names are not supported.

    @library{wxbase}
    @category{FIXME}
*/
class wxActiveXEvent : public wxCommandEvent
{
public:
    /**
        Returns the dispatch id of this activex event. This is the numeric value from
        the .idl file specified by the id().
    */
    DISPID GetDispatchId(int idx);

    /**
        Obtains the number of parameters passed through the activex event.
    */
    size_t ParamCount();

    /**
        Obtains the param name of the param number idx specifies as a string.
    */
    wxString ParamName(size_t idx);

    /**
        Obtains the param type of the param number idx specifies as a string.
    */
    wxString ParamType(size_t idx);

    /**
        Obtains the actual parameter value specified by idx.
    */
    wxVariant operator[](size_t idx);
};


/**
    @class wxActiveXContainer
    @headerfile ole/activex.h wx/msw/ole/activex.h

    wxActiveXContainer is a host for an activex control on Windows (and
    as such is a platform-specific class). Note that the HWND that the class
    contains is the actual HWND of the activex control so using dynamic events
    and connecting to wxEVT_SIZE, for example, will recieve the actual size
    message sent to the control.

    It is somewhat similar to the ATL class CAxWindow in operation.

    The size of the activex control's content is generally gauranteed to be that
    of the client size of the parent of this wxActiveXContainer.

    You can also process activex events through wxEVT_ACTIVEX or the
    corresponding message map macro EVT_ACTIVEX.

    @library{wxbase}
    @category{FIXME}

    @seealso
    wxActiveXEvent
*/
class wxActiveXContainer : public wxControl
{
public:
    /**
        Creates this activex container.
        
        @param parent
            parent of this control.  Must not be @NULL.
        @param iid
            COM IID of pUnk to query. Must be a valid interface to an activex control.
        @param pUnk
            Interface of activex control
    */
    wxActiveXContainer(wxWindow* parent, REFIID iid, IUnknown* pUnk);
};
