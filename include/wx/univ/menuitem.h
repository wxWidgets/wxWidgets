///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/menuitem.h
// Purpose:     wxMenuItem class for wxUniversal
// Author:      Vadim Zeitlin
// Modified by:
// Created:     05.05.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_MENUITEM_H_
#define _WX_UNIV_MENUITEM_H_

#ifdef __GNUG__
    #pragma interface "univmenuitem.h"
#endif

// ----------------------------------------------------------------------------
// wxMenuItem implements wxMenuItemBase
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxMenuItem : public wxMenuItemBase
{
public:
    // ctor & dtor
    wxMenuItem(wxMenu *parentMenu = (wxMenu *)NULL,
               int id = wxID_SEPARATOR,
               const wxString& name = wxEmptyString,
               const wxString& help = wxEmptyString,
               wxItemKind kind = wxITEM_NORMAL,
               wxMenu *subMenu = (wxMenu *)NULL);
    virtual ~wxMenuItem();

    // override base class virtuals to update the item appearance on screen
    virtual void SetText(const wxString& text);
    virtual void SetCheckable(bool checkable);

    virtual void Enable(bool enable = TRUE);
    virtual void Check(bool check = TRUE);

    // we add some extra functions which are also available under MSW from
    // wxOwnerDrawn class - they will be moved to wxMenuItemBase later
    // hopefully
    void SetBitmaps(const wxBitmap& bmpChecked,
                    const wxBitmap& bmpUnchecked = wxNullBitmap);
    void SetBitmap(const wxBitmap& bmp) { SetBitmaps(bmp); }
    const wxBitmap& GetBitmap(bool checked = TRUE) const
      { return checked ? m_bmpChecked : m_bmpUnchecked; }

    // wxUniv-specific methods for implementation only starting from here

    // get the accel index of our label or -1 if none
    int GetAccelIndex() const { return m_indexAccel; }

    // get the accel string (displayed to the right of the label)
    const wxString& GetAccelString() const { return m_strAccel; }

    // set/get the y coord and the height of this item: note that it must be
    // set first and retrieved later, the item doesn't calculate it itself
    void SetGeometry(wxCoord y, wxCoord height)
    {
        m_posY = y;
        m_height = height;
    }

    wxCoord GetPosition() const
    {
        wxASSERT_MSG( m_posY != -1, _T("must call SetHeight first!") );

        return m_posY;
    }

    wxCoord GetHeight() const
    {
        wxASSERT_MSG( m_height != -1, _T("must call SetHeight first!") );

        return m_height;
    }

protected:
    // notify the menu about the change in this item
    inline void NotifyMenu();

    // set the accel index and string from text
    void UpdateAccelInfo();

    // the bitmaps (may be invalid, then they're not used)
    wxBitmap m_bmpChecked,
             m_bmpUnchecked;

    // the position of the accelerator in our label, -1 if none
    int m_indexAccel;

    // the accel string (i.e. "Ctrl-Q" or "Alt-F1")
    wxString m_strAccel;

    // the position and height of the displayed item
    wxCoord m_posY,
            m_height;

private:
    DECLARE_DYNAMIC_CLASS(wxMenuItem)
};

#endif // _WX_UNIV_MENUITEM_H_

