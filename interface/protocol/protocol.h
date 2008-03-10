/////////////////////////////////////////////////////////////////////////////
// Name:        protocol/protocol.h
// Purpose:     interface of wxProtocol
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxProtocol
    @headerfile protocol.h wx/protocol/protocol.h


    @library{wxnet}
    @category{FIXME}

    @see wxSocketBase, wxURL
*/
class wxProtocol : public wxSocketClient
{
public:
    /**
        Abort the current stream.
        
        @returns Returns @true, if successful, else @false.
    */
    bool Abort();

    /**
        Returns the type of the content of the last opened stream. It is a mime-type.
    */
    wxString GetContentType();

    /**
        Returns the last occurred error.
        
        @b wxPROTO_NOERR
        
        No error.
        
        @b wxPROTO_NETERR
        
        A generic network error occurred.
        
        @b wxPROTO_PROTERR
        
        An error occurred during negotiation.
        
        @b wxPROTO_CONNERR
        
        The client failed to connect the server.
        
        @b wxPROTO_INVVAL
        
        Invalid value.
        
        @b wxPROTO_NOHNDLR
        
        .
        
        @b wxPROTO_NOFILE
        
        The remote file doesn't exist.
        
        @b wxPROTO_ABRT
        
        Last action aborted.
        
        @b wxPROTO_RCNCT
        
        An error occurred during reconnection.
        
        @b wxPROTO_STREAM
        
        Someone tried to send a command during a transfer.
    */
    wxProtocolError GetError();

    /**
        Creates a new input stream on the specified path. You can use all but seek
        functionality of wxStream. Seek isn't available on all streams. For example,
        HTTP or FTP streams don't deal with it. Other functions like StreamSize and
        Tell aren't available for the moment for this sort of stream.
        You will be notified when the EOF is reached by an error.
        
        @returns Returns the initialized stream. You will have to delete it
                 yourself once you don't use it anymore. The destructor
                 closes the network connection.
        
        @see wxInputStream
    */
    wxInputStream* GetInputStream(const wxString& path);

    /**
        Tries to reestablish a previous opened connection (close and renegotiate
        connection).
        
        @returns @true, if the connection is established, else @false.
    */
    bool Reconnect();

    /**
        Sets the authentication password. It is mainly useful when FTP is used.
    */
    void SetPassword(const wxString& user);

    /**
        Sets the authentication user. It is mainly useful when FTP is used.
    */
    void SetUser(const wxString& user);
};

