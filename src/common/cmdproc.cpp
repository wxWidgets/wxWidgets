///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/cmdproc.cpp
// Purpose:     wxCommand and wxCommandProcessor classes
// Author:      Julian Smart (extracted from docview.h by VZ)
// Modified by:
// Created:     05.11.00
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "cmdproc.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/string.h"
#endif //WX_PRECOMP

#include "wx/cmdproc.h"

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_CLASS(wxCommand, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxCommandProcessor, wxObject)

// ----------------------------------------------------------------------------
// wxCommand
// ----------------------------------------------------------------------------

wxCommand::wxCommand(bool canUndoIt, const wxString& name)
{
    m_canUndo = canUndoIt;
    m_commandName = name;
}

wxCommand::~wxCommand()
{
}

// ----------------------------------------------------------------------------
// Command processor
// ----------------------------------------------------------------------------

wxCommandProcessor::wxCommandProcessor(int maxCommands)
{
    m_maxNoCommands = maxCommands;
    m_currentCommand = (wxNode *) NULL;
#if wxUSE_MENUS
    m_commandEditMenu = (wxMenu *) NULL;
#endif // wxUSE_MENUS
}

wxCommandProcessor::~wxCommandProcessor()
{
    ClearCommands();
}

bool wxCommandProcessor::DoCommand(wxCommand& cmd)
{
    return cmd.Do();
}

bool wxCommandProcessor::UndoCommand(wxCommand& cmd)
{
    return cmd.Undo();
}

// Pass a command to the processor. The processor calls Do();
// if successful, is appended to the command history unless
// storeIt is FALSE.
bool wxCommandProcessor::Submit(wxCommand *command, bool storeIt)
{
    wxCHECK_MSG( command, FALSE, _T("no command in wxCommandProcessor::Submit") );

    if ( !DoCommand(*command) )
        return FALSE;

    if ( storeIt )
        Store(command);

    return TRUE;
}

void wxCommandProcessor::Store(wxCommand *command)
{
    wxCHECK_RET( command, _T("no command in wxCommandProcessor::Store") );

    if (m_commands.Number() == m_maxNoCommands)
    {
        wxNode *firstNode = m_commands.First();
        wxCommand *firstCommand = (wxCommand *)firstNode->Data();
        delete firstCommand;
        delete firstNode;
    }

    // Correct a bug: we must chop off the current 'branch'
    // so that we're at the end of the command list.
    if (!m_currentCommand)
        ClearCommands();
    else
    {
        wxNode *node = m_currentCommand->Next();
        while (node)
        {
            wxNode *next = node->Next();
            delete (wxCommand *)node->Data();
            delete node;
            node = next;
        }
    }

    m_commands.Append(command);
    m_currentCommand = m_commands.Last();
    SetMenuStrings();
}

bool wxCommandProcessor::Undo()
{
    wxCommand *command = GetCurrentCommand();
    if ( command && command->CanUndo() )
    {
        if ( UndoCommand(*command) )
        {
            m_currentCommand = m_currentCommand->Previous();
            SetMenuStrings();
            return TRUE;
        }
    }

    return FALSE;
}

bool wxCommandProcessor::Redo()
{
    wxCommand *redoCommand = (wxCommand *) NULL;
    wxNode *redoNode = (wxNode *) NULL;
    if (m_currentCommand && m_currentCommand->Next())
    {
        redoCommand = (wxCommand *)m_currentCommand->Next()->Data();
        redoNode = m_currentCommand->Next();
    }
    else
    {
        if (m_commands.Number() > 0)
        {
            redoCommand = (wxCommand *)m_commands.First()->Data();
            redoNode = m_commands.First();
        }
    }

    if (redoCommand)
    {
        bool success = DoCommand(*redoCommand);
        if (success)
        {
            m_currentCommand = redoNode;
            SetMenuStrings();
            return TRUE;
        }
    }
    return FALSE;
}

bool wxCommandProcessor::CanUndo() const
{
    wxCommand *command = GetCurrentCommand();

    return command && command->CanUndo();
}

bool wxCommandProcessor::CanRedo() const
{
    if ((m_currentCommand != (wxNode*) NULL) && (m_currentCommand->Next() == (wxNode*) NULL))
        return FALSE;

    if ((m_currentCommand != (wxNode*) NULL) && (m_currentCommand->Next() != (wxNode*) NULL))
        return TRUE;

    if ((m_currentCommand == (wxNode*) NULL) && (m_commands.Number() > 0))
        return TRUE;

    return FALSE;
}

void wxCommandProcessor::Initialize()
{
    m_currentCommand = m_commands.Last();
    SetMenuStrings();
}

void wxCommandProcessor::SetMenuStrings()
{
#if wxUSE_MENUS
    if (m_commandEditMenu)
    {
        wxString buf;
        if (m_currentCommand)
        {
            wxCommand *command = (wxCommand *)m_currentCommand->Data();
            wxString commandName(command->GetName());
            if (commandName == wxT("")) commandName = _("Unnamed command");
            bool canUndo = command->CanUndo();
            if (canUndo)
                buf = wxString(_("&Undo ")) + commandName;
            else
                buf = wxString(_("Can't &Undo ")) + commandName;

            m_commandEditMenu->SetLabel(wxID_UNDO, buf);
            m_commandEditMenu->Enable(wxID_UNDO, canUndo);

            // We can redo, if we're not at the end of the history.
            if (m_currentCommand->Next())
            {
                wxCommand *redoCommand = (wxCommand *)m_currentCommand->Next()->Data();
                wxString redoCommandName(redoCommand->GetName());
                if (redoCommandName == wxT("")) redoCommandName = _("Unnamed command");
                buf = wxString(_("&Redo ")) + redoCommandName;
                m_commandEditMenu->SetLabel(wxID_REDO, buf);
                m_commandEditMenu->Enable(wxID_REDO, TRUE);
            }
            else
            {
                m_commandEditMenu->SetLabel(wxID_REDO, _("&Redo"));
                m_commandEditMenu->Enable(wxID_REDO, FALSE);
            }
        }
        else
        {
            m_commandEditMenu->SetLabel(wxID_UNDO, _("&Undo"));
            m_commandEditMenu->Enable(wxID_UNDO, FALSE);

            if (m_commands.Number() == 0)
            {
                m_commandEditMenu->SetLabel(wxID_REDO, _("&Redo"));
                m_commandEditMenu->Enable(wxID_REDO, FALSE);
            }
            else
            {
                // currentCommand is NULL but there are commands: this means that
                // we've undone to the start of the list, but can redo the first.
                wxCommand *redoCommand = (wxCommand *)m_commands.First()->Data();
                wxString redoCommandName(redoCommand->GetName());
                if (redoCommandName == wxT("")) redoCommandName = _("Unnamed command");
                buf = wxString(_("&Redo ")) + redoCommandName;
                m_commandEditMenu->SetLabel(wxID_REDO, buf);
                m_commandEditMenu->Enable(wxID_REDO, TRUE);
            }
        }
    }
#endif // wxUSE_MENUS
}

void wxCommandProcessor::ClearCommands()
{
    wxNode *node = m_commands.First();
    while (node)
    {
        wxCommand *command = (wxCommand *)node->Data();
        delete command;
        delete node;
        node = m_commands.First();
    }
    m_currentCommand = (wxNode *) NULL;
}


