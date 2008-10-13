/////////////////////////////////////////////////////////////////////////////
// Name:        protocol/protocol.h
// Purpose:     interface of wxProtocol
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    Error values returned by wxProtocol.
*/
enum wxProtocolError
{
    wxPROTO_NOERR = 0,          //!< No error.
    wxPROTO_NETERR,             //!< A generic network error occurred.
    wxPROTO_PROTERR,            //!< An error occurred during negotiation.
    wxPROTO_CONNERR,            //!< The client failed to connect the server.
    wxPROTO_INVVAL,             //!< Invalid value.
    wxPROTO_NOHNDLR,            //!< Not currently used.
    wxPROTO_NOFILE,             //!< The remote file doesn't exist.
    wxPROTO_ABRT,               //!< Last action aborted.
    wxPROTO_RCNCT,              //!< An error occurred during reconnection.
    wxPROTO_STREAMING           //!< Someone tried to send a command during a transfer.
};

/**
    @class wxProtocol

    Represents a protocol for use with wxURL.

    @library{wxnet}
    @category{net}

    @see wxSocketBase, wxURL
*/
class wxProtocol : public wxSocketClient
{
public:
    /**
        Abort the current stream.

        @warning
        It is advised to destroy the input stream instead of aborting the stream
        this way.

        @return Returns @true, if successful, else @false.
    */
    virtual bool Abort() = 0;

    /**
        Returns the type of the content of the last opened stream. It is a mime-type.
    */
    virtual wxString GetContentType();

    /**
        Returns the last occurred error.

        @see wxProtocolError
    */
    virtual wxProtocolError GetError() = 0;

    /**
        Creates a new input stream on the specified path.

        You can use all but seek() functionality of wxStream.
        Seek() isn't available on all streams. For example, HTTP or FTP streams
        don't deal with it. Other functions like StreamSize() and Tell() aren't
        available for the moment for this sort of stream.
        You will be notified when the EOF is reached by an error.

        @return Returns the initialized stream. You will have to delete it
                 yourself once you don't use it anymore. The destructor
                 closes the network connection.

        @see wxInputStream
    */
    virtual wxInputStream* GetInputStream(const wxString& path) = 0;

    /**
        Tries to reestablish a previous opened connection (close and renegotiate
        connection).

        @return @true, if the connection is established, else @false.
    */
    bool Reconnect();

    /**
        Sets the authentication password. It is mainly useful when FTP is used.
    */
    virtual void SetPassword(const wxString& user);

    /**
        Sets the authentication user. It is mainly useful when FTP is used.
    */
    virtual void SetUser(const wxString& user);
};

