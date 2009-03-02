///////////////////////////////////////////////////////////////////////////////
// Name:        wx/textentry.h
// Purpose:     declares wxTextEntry interface defining a simple text entry
// Author:      Vadim Zeitlin
// Created:     2007-09-24
// RCS-ID:      $Id$
// Copyright:   (c) 2007 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TEXTENTRY_H_
#define _WX_TEXTENTRY_H_

// wxTextPos is the position in the text (currently it's hardly used anywhere
// and should probably be replaced with int anyhow)
typedef long wxTextPos;

class WXDLLIMPEXP_FWD_BASE wxArrayString;
class WXDLLIMPEXP_FWD_CORE wxTextEntryHintData;

// ----------------------------------------------------------------------------
// wxTextEntryBase
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxTextEntryBase
{
public:
    wxTextEntryBase() { m_eventsBlock = 0; m_hintData = NULL; }
    virtual ~wxTextEntryBase();


    // accessing the value
    // -------------------

    // SetValue() generates a text change event, ChangeValue() doesn't
    virtual void SetValue(const wxString& value)
        { DoSetValue(value, SetValue_SendEvent); }
    virtual void ChangeValue(const wxString& value)
        { DoSetValue(value, SetValue_NoEvent); }

    // writing text inserts it at the current position replacing any current
    // selection, appending always inserts it at the end and doesn't remove any
    // existing text (but it will reset the selection if there is any)
    virtual void WriteText(const wxString& text) = 0;
    virtual void AppendText(const wxString& text);

    virtual wxString GetValue() const;
    virtual wxString GetRange(long from, long to) const;
    bool IsEmpty() const { return GetLastPosition() <= 0; }


    // editing operations
    // ------------------

    virtual void Replace(long from, long to, const wxString& value);
    virtual void Remove(long from, long to) = 0;
    virtual void Clear() { SetValue(wxString()); }
    void RemoveSelection();


    // clipboard operations
    // --------------------

    virtual void Copy() = 0;
    virtual void Cut() = 0;
    virtual void Paste() = 0;

    virtual bool CanCopy() const;
    virtual bool CanCut() const;
    virtual bool CanPaste() const;

    // undo/redo
    // ---------

    virtual void Undo() = 0;
    virtual void Redo() = 0;

    virtual bool CanUndo() const = 0;
    virtual bool CanRedo() const = 0;


    // insertion point
    // ---------------

    // note that moving insertion point removes any current selection
    virtual void SetInsertionPoint(long pos) = 0;
    virtual void SetInsertionPointEnd() { SetInsertionPoint(-1); }
    virtual long GetInsertionPoint() const = 0;
    virtual long GetLastPosition() const = 0;


    // selection
    // ---------

    virtual void SetSelection(long from, long to) = 0;
    virtual void SelectAll() { SetSelection(-1, -1); }
    virtual void GetSelection(long *from, long *to) const = 0;
    bool HasSelection() const;
    virtual wxString GetStringSelection() const;


    // auto-completion
    // ---------------

    // these functions allow to auto-complete the text already entered into the
    // control using either the given fixed list of strings, the paths from the
    // file system or, in the future, an arbitrary user-defined completer
    //
    // they all return true if completion was enabled or false on error (most
    // commonly meaning that this functionality is not available under the
    // current platform)

    virtual bool AutoComplete(const wxArrayString& WXUNUSED(choices))
    {
        return false;
    }

    virtual bool AutoCompleteFileNames() { return false; }


    // status
    // ------

    virtual bool IsEditable() const = 0;
    virtual void SetEditable(bool editable) = 0;


    // set the max number of characters which may be entered in a single line
    // text control
    virtual void SetMaxLength(unsigned long WXUNUSED(len)) { }


    // hints
    // -----

    // hint is the (usually greyed out) text shown in the control as long as
    // it's empty and doesn't have focus, it is typically used in controls used
    // for searching to let the user know what is supposed to be entered there

    virtual bool SetHint(const wxString& hint);
    virtual wxString GetHint() const;


protected:
    // flags for DoSetValue(): common part of SetValue() and ChangeValue() and
    // also used to implement WriteText() in wxMSW
    enum
    {
        SetValue_NoEvent = 0,
        SetValue_SendEvent = 1,
        SetValue_SelectionOnly = 2
    };

    virtual void DoSetValue(const wxString& value, int flags);
    virtual wxString DoGetValue() const = 0;

    // class which should be used to temporarily disable text change events
    //
    // if suppress argument in ctor is false, nothing is done
    class EventsSuppressor
    {
    public:
        EventsSuppressor(wxTextEntryBase *text, bool suppress = true)
            : m_text(text),
              m_suppress(suppress)
        {
            if ( m_suppress )
                m_text->SuppressTextChangedEvents();
        }

        ~EventsSuppressor()
        {
            if ( m_suppress )
                m_text->ResumeTextChangedEvents();
        }

    private:
        wxTextEntryBase *m_text;
        bool m_suppress;
    };

    friend class EventsSuppressor;

    // return true if the events are currently not suppressed
    bool EventsAllowed() const { return m_eventsBlock == 0; }

private:
    // override this to return the associated window, it will be used for event
    // generation and also by generic hints implementation
    virtual wxWindow *GetEditableWindow() = 0;


    // suppress or resume the text changed events generation: don't use these
    // functions directly, use EventsSuppressor class above instead
    void SuppressTextChangedEvents()
    {
        if ( !m_eventsBlock++ )
            EnableTextChangedEvents(false);
    }

    void ResumeTextChangedEvents()
    {
        if ( !--m_eventsBlock )
            EnableTextChangedEvents(true);
    }


    // this must be overridden in the derived classes if our implementation of
    // SetValue() or Replace() is used to disable (and enable back) generation
    // of the text changed events
    //
    // initially the generation of the events is enabled
    virtual void EnableTextChangedEvents(bool WXUNUSED(enable)) { }

    // if this counter is non-null, events are blocked
    unsigned m_eventsBlock;

    // hint-related stuff, only allocated if/when SetHint() is used
    wxTextEntryHintData *m_hintData;
};

#ifdef __WXUNIVERSAL__
    // TODO: we need to use wxTextEntryDelegate here, but for now just prevent
    //       the GTK/MSW classes from being used in wxUniv build
    class WXDLLIMPEXP_CORE wxTextEntry : public wxTextEntryBase
    {
    };
#elif defined(__WXGTK20__)
    #include "wx/gtk/textentry.h"
#elif defined(__WXMSW__)
    #include "wx/msw/textentry.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/textentry.h"
#elif defined(__WXPM__)
    #include "wx/os2/textentry.h"
#else
    // no platform-specific implementation of wxTextEntry yet
    class WXDLLIMPEXP_CORE wxTextEntry : public wxTextEntryBase
    {
    };
#endif

#endif // _WX_TEXTENTRY_H_

