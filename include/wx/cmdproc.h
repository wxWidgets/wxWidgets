///////////////////////////////////////////////////////////////////////////////
// Name:        wx/cmdproc.h
// Purpose:     undo/redo capable command processing framework
// Author:      Julian Smart (extracted from docview.h by VZ)
// Modified by:
// Created:     05.11.00
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CMDPROC_H_
#define _WX_CMDPROC_H_

#ifdef __GNUG__
    #pragma interface "cmdproc.h"
#endif

#include "wx/object.h"
#include "wx/list.h"

// ----------------------------------------------------------------------------
// wxCommand: a single command capable of performing itself
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxCommand : public wxObject
{
public:
    wxCommand(bool canUndoIt = FALSE, const wxString& name = "");
    ~wxCommand();

    // Override this to perform a command
    virtual bool Do() = 0;

    // Override this to undo a command
    virtual bool Undo() = 0;

    virtual bool CanUndo() const { return m_canUndo; }
    virtual wxString GetName() const { return m_commandName; }

protected:
    bool     m_canUndo;
    wxString m_commandName;

private:
    DECLARE_CLASS(wxCommand)
};

// ----------------------------------------------------------------------------
// wxCommandProcessor: wxCommand manager
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxCommandProcessor : public wxObject
{
public:
    // if max number of commands is -1, it is unlimited
    wxCommandProcessor(int maxCommands = -1);
    virtual ~wxCommandProcessor();

    // Pass a command to the processor. The processor calls Do(); if
    // successful, is appended to the command history unless storeIt is FALSE.
    virtual bool Submit(wxCommand *command, bool storeIt = TRUE);

    // just store the command without executing it
    virtual void Store(wxCommand *command);

    virtual bool Undo();
    virtual bool Redo();
    virtual bool CanUndo() const;
    virtual bool CanRedo() const;

    virtual void Initialize();
    virtual void SetMenuStrings();

#if wxUSE_MENUS
    // Call this to manage an edit menu.
    void SetEditMenu(wxMenu *menu) { m_commandEditMenu = menu; }
    wxMenu *GetEditMenu() const { return m_commandEditMenu; }
#endif // wxUSE_MENUS

    // command list access
    wxList& GetCommands() const { return (wxList&) m_commands; }
    wxCommand *GetCurrentCommand() const
    {
        return (wxCommand *)(m_currentCommand ? m_currentCommand->Data() : NULL);
    }
    int GetMaxCommands() const { return m_maxNoCommands; }
    virtual void ClearCommands();

protected:
    // for further flexibility, command processor doesn't call wxCommand::Do()
    // and Undo() directly but uses these functions which can be overridden in
    // the derived class
    virtual bool DoCommand(wxCommand& cmd);
    virtual bool UndoCommand(wxCommand& cmd);

    int           m_maxNoCommands;
    wxList        m_commands;
    wxNode*       m_currentCommand;

#if wxUSE_MENUS
    wxMenu*       m_commandEditMenu;
#endif // wxUSE_MENUS

private:
    DECLARE_DYNAMIC_CLASS(wxCommandProcessor)
};

#endif // _WX_CMDPROC_H_
