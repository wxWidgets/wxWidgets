/////////////////////////////////////////////////////////////////////////////
// Name:        treebase.h
// Purpose:     documentation for wxTreeItemId class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxTreeItemId
    @wxheader{treebase.h}

    An opaque reference to a tree item.

    @library{wxcore}
    @category{FIXME}

    @seealso
    wxTreeCtrl, wxTreeItemData, @ref overview_wxtreectrloverview "wxTreeCtrl
    overview"
*/
class wxTreeItemId
{
public:
    /**
        Default constructor. wxTreemItemIds are not meant to be constructed explicitly
        by
        the user; they are returned by the wxTreeCtrl functions instead.
    */
    wxTreeItemId();

    /**
        Returns @true if this instance is referencing a valid tree item.
    */
    bool IsOk() const;

    //@{
    /**
        Operators for comparison between wxTreeItemId objects.
    */
    void operator !() const;
    const bool operator ==(const wxTreeItemId& item) const;
    const bool operator !=(const wxTreeItemId& item) const;
    //@}
};
