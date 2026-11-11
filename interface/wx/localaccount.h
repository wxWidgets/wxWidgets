/////////////////////////////////////////////////////////////////////////////
// Name:        localaccount.h
// Purpose:     interface of wxLocalAccount
// Author:      Blake Madden
// Created:     2026-10-11
// Copyright:   (c) 2026 Blake Madden
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxLocalAccount

    Identifies a user account of the local system.

    Objects of this class are returned by wxFileName::GetOwner().

    The identity of an account is its ID, which is the numeric user ID under
    Unix and the string form of the security identifier (SID) under MSW. The
    name is only looked up on a best-effort basis and may be unavailable. For
    example, when a file was copied from another system using different user
    IDs or when the account has been deleted. In this case, GetName() returns
    an empty string even though IsOk() returns @true.

    @library{wxbase}
    @category{file}

    @since 3.3.4
*/
class wxLocalAccount
{
public:
    /**
        Default constructor creates an invalid account object.
    */
    wxLocalAccount();

    /**
        Returns @true if the account was determined.

        This can be @true even if the account name is not known, use GetName()
        to check for this.
    */
    bool IsOk() const;

    /**
        Returns the name of the account without the domain.

        The returned string is empty if the name couldn't be found.
    */
    const wxString& GetName() const;

    /**
        Returns the domain of the account (e.g., @c "CORP").

        This is always empty under Unix, which has no account domains.
    */
    const wxString& GetDomain() const;

    /**
        Returns the ID of the account.

        This is the numeric user ID under Unix and the string form of the SID
        (e.g., @c "S-1-5-18") under MSW.
    */
    const wxString& GetId() const;

    /**
        Returns the account in human-readable form for the user.

        This is:
        - @c "domain\\name" if both the domain and the name are known
        - just the name if there is no domain
        - or the ID if the name is unknown
    */
    wxString GetAsString() const;
};
