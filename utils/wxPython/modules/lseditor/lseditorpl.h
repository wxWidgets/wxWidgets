/////////////////////////////////////////////////////////////////////////////
// Name:        nativeeditorpl.h
// Purpose:     Language-sensative editor plugin for wxStudio
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     11/04/1999
// RCS-ID:      $Id$
// Copyright:   (c) Aleskandars Gluchovas
// Licence:   	GNU General Public License 
/////////////////////////////////////////////////////////////////////////////
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
/////////////////////////////////////////////////////////////////////////////

#ifndef __LSEDITORPL_G__
#define __LSEDITORPL_G__

#include "editorpl.h"

class wxTextEditorModel;
class wxTextEditorView;

class wxsLSEditorPlugin : public wxsSourceEditorPlugin
{
protected:
	wxTextEditorModel* mpModel;
	wxTextEditorView*  mpView;

public:
	wxsLSEditorPlugin();
	~wxsLSEditorPlugin();

	void Create( wxWindow* parent, wxWindowID id );

	virtual void OnOpen( const string& fname );
	virtual void OnSave( const string& fname );

	virtual void OnCopy();
	virtual void OnCut();
	virtual void OnPaste();
	virtual void OnDelete();

	void OnUndo();
	void OnRedo();

	void SelectAll();
	void OnGotoLine( int lineNo, int column = 0 );
	void OnGotoLine();
	void OnProperties();

	void OnFind();
	void OnFindNext();
	void OnFindPrevious();
	void OnReplace();

	virtual void OnToggleBookmark();
	virtual void OnNextBookmark();
	virtual void OnPreviousBookmark();
	virtual void OnShowBookmarks();

	virtual void SetCheckpoint();
	virtual bool CheckpointModified();

	// UI-updates

	bool CanCopy();
	bool CanCut();
	bool CanPaste();
	bool CanUndo();
	bool CanRedo();

	// accesed by framework
    
    virtual string GetName(){ return "Alex's Language Sensitive Editor"; }

	virtual bool IsModified();

	virtual wxWindow* GetWindow();

	virtual void GetAllText( char** ppBuf, size_t* length );

	virtual void SetFileName( const string& fname );

	virtual void HoldCursor( bool hold );

	virtual wxsPluginBase* Clone() { return new wxsLSEditorPlugin(); };

	virtual string FindWordAtCursor();

	virtual void GetCursorPos( int* line, int* column );
	virtual void SetCursorPos( int line, int column );

	virtual void GetPagePos( int* line, int* column );

	virtual void GetText( int fromLine, int fromColumn,
		                  int tillLine, int tillColumn,
						  char** ppBuf, size_t* length );

	virtual void InsertText( int line, int column,
							 char* text, size_t lenght );

	virtual void DeleteText( int fromLine, int fromColumn,
						     int tillLine, int tillColumn  );

	virtual void PositionToXY( int line, int column, int* x, int* y );

	virtual void GetSelectionRange( int* fromLine, int* fromColumn,
								    int* tillLine, int* tillColumn  );

	virtual wxSize GetCharacterSize();

	virtual bool IsUnixText();

	// some extras (just in case..)

	wxTextEditorModel& GetModel();
	wxTextEditorView&  GetView();
};

#endif
	 //	__LSEDITORPL_G__
