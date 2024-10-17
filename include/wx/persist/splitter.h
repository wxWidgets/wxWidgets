///////////////////////////////////////////////////////////////////////////////
// Name:        wx/persist/splitter.h
// Purpose:     Persistence support for wxSplitterWindow.
// Author:      Vadim Zeitlin
// Created:     2011-08-31
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PERSIST_SPLITTER_H_
#define _WX_PERSIST_SPLITTER_H_

#include "wx/persist/window.h"

#include "wx/splitter.h"

// ----------------------------------------------------------------------------
// string constants used by wxPersistentSplitter
// ----------------------------------------------------------------------------

#define wxPERSIST_SPLITTER_KIND wxASCII_STR("Splitter")

// Special position value of -1 means the splitter is not split at all.
#define wxPERSIST_SPLITTER_POSITION wxASCII_STR("Position")

#define wxPERSIST_SPLITTER_DEFAULT_HORIZONTAL wxASCII_STR("LastHorz")
#define wxPERSIST_SPLITTER_DEFAULT_VERTICAL wxASCII_STR("LastVert")

// ----------------------------------------------------------------------------
// wxPersistentSplitter: supports saving/restoring splitter position
// ----------------------------------------------------------------------------

class wxPersistentSplitter : public wxPersistentWindow<wxSplitterWindow>
{
public:
    wxPersistentSplitter(wxSplitterWindow* splitter)
        : wxPersistentWindow<wxSplitterWindow>(splitter)
    {
    }

    virtual void Save() const override
    {
        wxSplitterWindow* const splitter = Get();

        int pos = splitter->IsSplit() ? splitter->GetSashPosition() : -1;
        SaveValue(wxPERSIST_SPLITTER_POSITION, pos);

        // Save the previously used position too if we have them.
        const wxPoint lastSplitPos = splitter->GetLastSplitPosition();
        if ( lastSplitPos.x || lastSplitPos.y )
        {
            SaveValue(wxPERSIST_SPLITTER_DEFAULT_HORIZONTAL, lastSplitPos.y);
            SaveValue(wxPERSIST_SPLITTER_DEFAULT_VERTICAL, lastSplitPos.x);
        }
    }

    virtual bool Restore() override
    {
        int pos;
        if ( !RestoreValue(wxPERSIST_SPLITTER_POSITION, &pos) )
            return false;

        if ( pos == -1 )
            Get()->Unsplit();
        else
            Get()->SetSashPosition(pos);

        // Note that it's possible that default position was not stored, in
        // which case lastSplitPos will just remain as (0, 0) and that's ok.
        wxPoint lastSplitPos;
        RestoreValue(wxPERSIST_SPLITTER_DEFAULT_HORIZONTAL, &lastSplitPos.x);
        RestoreValue(wxPERSIST_SPLITTER_DEFAULT_VERTICAL, &lastSplitPos.y);

        Get()->SetLastSplitPosition(lastSplitPos);

        return true;
    }

    virtual wxString GetKind() const override { return wxPERSIST_SPLITTER_KIND; }
};

inline wxPersistentObject *wxCreatePersistentObject(wxSplitterWindow* splitter)
{
    return new wxPersistentSplitter(splitter);
}

#endif // _WX_PERSIST_SPLITTER_H_
