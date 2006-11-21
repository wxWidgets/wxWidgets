/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/carbon/textctrl.h
// Purpose:     wxTextCtrl class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TEXTCTRL_H_
#define _WX_TEXTCTRL_H_

#if wxUSE_SYSTEM_OPTIONS
    // set this to 'true' if you want to use the 'classic' MLTE-based implementation
    // instead of the HIView-based implementation in 10.3 and upwards, the former
    // has more features (backgrounds etc.), but may show redraw artefacts and other
    // problems depending on your usage; hence, the default is 'false'.
    #define wxMAC_TEXTCONTROL_USE_MLTE wxT("mac.textcontrol-use-mlte")
#endif

#include "wx/control.h"
#include "wx/textctrl.h"

class wxMacTextControl;


class WXDLLEXPORT wxTextCtrl: public wxTextCtrlBase
{
  DECLARE_DYNAMIC_CLASS(wxTextCtrl)

public:
    wxTextCtrl()
    { Init(); }

    wxTextCtrl(wxWindow *parent,
        wxWindowID id,
        const wxString& value = wxEmptyString,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxTextCtrlNameStr)
    {
        Init();
        Create(parent, id, value, pos, size, style, validator, name);
    }

    virtual ~wxTextCtrl();

    bool Create(wxWindow *parent,
        wxWindowID id,
        const wxString& value = wxEmptyString,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxTextCtrlNameStr);

    // accessors
    // ---------
    virtual wxString GetValue() const;

    virtual int GetLineLength(long lineNo) const;
    virtual wxString GetLineText(long lineNo) const;
    virtual int GetNumberOfLines() const;

    virtual bool IsModified() const;
    virtual bool IsEditable() const;

    // If the return values from and to are the same, there is no selection.
    virtual void GetSelection(long* from, long* to) const;

    // operations
    // ----------

    // editing
    virtual void Clear();
    virtual void Replace(long from, long to, const wxString& value);
    virtual void Remove(long from, long to);

    // sets/clears the dirty flag
    virtual void MarkDirty();
    virtual void DiscardEdits();

    // set the max number of characters which may be entered
    // in a single line text control
    virtual void SetMaxLength(unsigned long len);

    // text control under some platforms supports the text styles: these
    // methods apply the given text style to the given selection or to
    // set/get the style which will be used for all appended text
    virtual bool SetFont( const wxFont &font );
    virtual bool SetStyle(long start, long end, const wxTextAttr& style);
    virtual bool SetDefaultStyle(const wxTextAttr& style);

    // writing text inserts it at the current position;
    // appending always inserts it at the end
    virtual void WriteText(const wxString& text);
    virtual void AppendText(const wxString& text);

    // translate between the position (which is just an index into the textctrl
    // considering all its contents as a single strings) and (x, y) coordinates
    // which represent column and line.
    virtual long XYToPosition(long x, long y) const;
    virtual bool PositionToXY(long pos, long *x, long *y) const;

    virtual void ShowPosition(long pos);

    // Clipboard operations
    virtual void Copy();
    virtual void Cut();
    virtual void Paste();

    virtual bool CanCopy() const;
    virtual bool CanCut() const;
    virtual bool CanPaste() const;

    // Undo/redo
    virtual void Undo();
    virtual void Redo();

    virtual bool CanUndo() const;
    virtual bool CanRedo() const;

    // Insertion point
    virtual void SetInsertionPoint(long pos);
    virtual void SetInsertionPointEnd();
    virtual long GetInsertionPoint() const;
    virtual wxTextPos GetLastPosition() const;

    virtual void SetSelection(long from, long to);
    virtual void SetEditable(bool editable);

    // Implementation
    // --------------
    virtual void Command(wxCommandEvent& event);

    virtual bool AcceptsFocus() const;

    // callbacks
    void OnDropFiles(wxDropFilesEvent& event);
    void OnChar(wxKeyEvent& event); // Process 'enter' if required

    void OnCut(wxCommandEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnPaste(wxCommandEvent& event);
    void OnUndo(wxCommandEvent& event);
    void OnRedo(wxCommandEvent& event);
    void OnDelete(wxCommandEvent& event);
    void OnSelectAll(wxCommandEvent& event);

    void OnUpdateCut(wxUpdateUIEvent& event);
    void OnUpdateCopy(wxUpdateUIEvent& event);
    void OnUpdatePaste(wxUpdateUIEvent& event);
    void OnUpdateUndo(wxUpdateUIEvent& event);
    void OnUpdateRedo(wxUpdateUIEvent& event);
    void OnUpdateDelete(wxUpdateUIEvent& event);
    void OnUpdateSelectAll(wxUpdateUIEvent& event);

    void OnEraseBackground(wxEraseEvent& event);
    void OnContextMenu(wxContextMenuEvent& event);

    virtual bool MacCanFocus() const
    { return true; }

    virtual bool MacSetupCursor( const wxPoint& pt );

    virtual void MacVisibilityChanged();
    virtual void MacEnabledStateChanged();
    virtual void MacSuperChangedPosition();
    virtual void MacCheckSpelling(bool check);

#ifndef __WXMAC_OSX__
    virtual void MacControlUserPaneDrawProc(wxInt16 part);
    virtual wxInt16 MacControlUserPaneHitTestProc(wxInt16 x, wxInt16 y);
    virtual wxInt16 MacControlUserPaneTrackingProc(wxInt16 x, wxInt16 y, void* actionProc);
    virtual void MacControlUserPaneIdleProc();
    virtual wxInt16 MacControlUserPaneKeyDownProc(wxInt16 keyCode, wxInt16 charCode, wxInt16 modifiers);
    virtual void MacControlUserPaneActivateProc(bool activating);
    virtual wxInt16 MacControlUserPaneFocusProc(wxInt16 action);
    virtual void MacControlUserPaneBackgroundProc(void* info);
#endif

    wxMacTextControl * GetPeer() const
    { return (wxMacTextControl*) m_peer; }

protected:
    // common part of all ctors
    void Init();

    virtual wxSize DoGetBestSize() const;

    virtual void DoSetValue(const wxString& value, int flags = 0);

    bool  m_editable;

    // flag is set to true when the user edits the controls contents
    bool m_dirty;

  // need to make this public because of the current implementation via callbacks
    unsigned long  m_maxLength;

    bool GetTriggerOnSetValue() const
    {
        return m_triggerOnSetValue;
    }

    void SetTriggerOnSetValue(bool trigger)
    {
        m_triggerOnSetValue = trigger;
    }

    bool m_triggerOnSetValue ;

private :
  wxMenu  *m_privateContextMenu;

  DECLARE_EVENT_TABLE()
};

#endif // _WX_TEXTCTRL_H_
