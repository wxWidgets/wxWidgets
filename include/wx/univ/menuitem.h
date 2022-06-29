///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/menuitem.h
// Purpose:     wxMenuItem class for wxUniversal
// Author:      Vadim Zeitlin
// Modified by:
// Created:     05.05.01
// Copyright:   (c) 2001 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_MENUITEM_H_
#define _WX_UNIV_MENUITEM_H_

// ----------------------------------------------------------------------------
// wxMenuItem implements wxMenuItemBase
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxMenuItem : public wxMenuItemBase
{
public:
    // ctor & dtor
    wxMenuItem(wxMenu *parentMenu = NULL,
               int id = wxID_SEPARATOR,
               const wxString& name = wxEmptyString,
               const wxString& help = wxEmptyString,
               wxItemKind kind = wxITEM_NORMAL,
               wxMenu *subMenu = NULL);
    virtual ~wxMenuItem();

    // override base class virtuals to update the item appearance on screen
    virtual void SetItemLabel(const wxString& text) wxOVERRIDE;
    virtual void SetCheckable(bool checkable) wxOVERRIDE;

    virtual void Enable(bool enable = true) wxOVERRIDE;
    virtual void Check(bool check = true) wxOVERRIDE;

    // we add some extra functions which are also available under MSW from
    // wxOwnerDrawn class - they will be moved to wxMenuItemBase later
    // hopefully
    void SetBitmaps(const wxBitmapBundle& bmpChecked,
                    const wxBitmapBundle& bmpUnchecked = wxBitmapBundle());
    virtual void SetBitmap(const wxBitmapBundle& bmp) wxOVERRIDE { SetBitmaps(bmp); }
    wxBitmap GetBitmap(bool checked) const
      { return GetBitmapFromBundle(checked ? m_bitmap : m_bmpUnchecked); }
    virtual wxBitmap GetBitmap() const wxOVERRIDE
      { return GetBitmap(true); }

    void SetDisabledBitmap( const wxBitmapBundle& bmpDisabled )
      { m_bmpDisabled = bmpDisabled; }
    wxBitmap GetDisabledBitmap() const
      { return GetBitmapFromBundle(m_bmpDisabled); }

    // mark item as belonging to the given radio group
    void SetAsRadioGroupStart();
    void SetRadioGroupStart(int start);
    void SetRadioGroupEnd(int end);
    int GetRadioGroupStart();
    int GetRadioGroupEnd();

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
        wxASSERT_MSG( m_posY != wxDefaultCoord, wxT("must call SetHeight first!") );

        return m_posY;
    }

    wxCoord GetHeight() const
    {
        wxASSERT_MSG( m_height != wxDefaultCoord, wxT("must call SetHeight first!") );

        return m_height;
    }

protected:
    // notify the menu about the change in this item
    inline void NotifyMenu();

    // set the accel index and string from text
    void UpdateAccelInfo();

    // the bitmaps (may be invalid, then they're not used)
    wxBitmapBundle m_bmpUnchecked,
                   m_bmpDisabled;

    // the positions of the first and last items of the radio group this item
    // belongs to or -1: start is the radio group start and is valid for all
    // but first radio group items (m_isRadioGroupStart == false), end is valid
    // only for the first one
    union
    {
        int start;
        int end;
    } m_radioGroup;

    // does this item start a radio group?
    bool m_isRadioGroupStart;

    // the position of the accelerator in our label, -1 if none
    int m_indexAccel;

    // the accel string (i.e. "Ctrl-Q" or "Alt-F1")
    wxString m_strAccel;

    // the position and height of the displayed item
    wxCoord m_posY,
            m_height;

private:
    wxDECLARE_DYNAMIC_CLASS(wxMenuItem);
};

#endif // _WX_UNIV_MENUITEM_H_

