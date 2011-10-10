///////////////////////////////////////////////////////////////////////////////
// Name:        interface/wx/nonownedwnd.h
// Purpose:     wxNonOwnedWindow class documentation
// Author:      Vadim Zeitlin
// Created:     2011-10-09
// RCS-ID:      $Id$
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/**
    Common base class for all non-child windows.

    This is the common base class of wxTopLevelWindow and wxPopupWindow and is
    not used directly.

    Currently the only additional functionality it provides, compared to base
    wxWindow class, is the ability to set the window shape.

    @since 2.9.3
 */
class wxNonOwnedWindow : public wxWindow
{
public:
    /**
        If the platform supports it, sets the shape of the window to that
        depicted by @a region. The system will not display or respond to any
        mouse event for the pixels that lie outside of the region. To reset the
        window to the normal rectangular shape simply call SetShape() again with
        an empty wxRegion. Returns @true if the operation is successful.

        This method is available in this class only since wxWidgets 2.9.3,
        previous versions only provided it in wxTopLevelWindow.
    */
    virtual bool SetShape(const wxRegion& region);
};
