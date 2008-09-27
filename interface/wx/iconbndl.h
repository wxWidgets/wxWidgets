/////////////////////////////////////////////////////////////////////////////
// Name:        iconbndl.h
// Purpose:     interface of wxIconBundle
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxIconBundle

    This class contains multiple copies of an icon in different sizes,
    see also wxDialog::SetIcons and
    wxTopLevelWindow::SetIcons.

    @library{wxcore}
    @category{FIXME}

    @stdobjects
    ::wxNullIconBundle
*/
class wxIconBundle : public wxGDIObject
{
public:
    //@{
    /**
        Copy constructor.
    */
    wxIconBundle();
    wxIconBundle(const wxString& file, wxBitmapType type);
    wxIconBundle(const wxIcon& icon);
    wxIconBundle(const wxIconBundle& ic);
    //@}

    /**
        Destructor.
    */
    virtual ~wxIconBundle();

    //@{
    /**
        Adds the icon to the collection; if the collection already
        contains an icon with the same width and height, it is
        replaced by the new one.
    */
    void AddIcon(const wxString& file, wxBitmapType type);
    void AddIcon(const wxIcon& icon);
    //@}

    //@{
    /**
        Same as GetIcon( wxSize( size, size ) ).
    */
    wxIcon GetIcon(const wxSize& size) const;
    const wxIcon  GetIcon(wxCoord size = -1) const;
    //@}

    /**
        Returns the icon with exactly the given size or @c wxNullIcon if this
        size is not available.
    */
    wxIcon GetIconOfExactSize(const wxSize& size) const;

    /**
        Returns @true if the bundle doesn't contain any icons, @false otherwise (in
        which case a call to GetIcon() with default
        parameter should return a valid icon).
    */
    bool IsEmpty() const;

    /**
        Assignment operator, using @ref overview_trefcount "reference counting".
    */
    wxIconBundle operator =(const wxIconBundle& ic);

    /**
        Equality operator. This returns @true if two icon bundles are equal.
    */
    bool operator ==(const wxIconBundle& ic);
};


/**
    An empty wxIconBundle.
*/
wxIconBundle wxNullIconBundle;


