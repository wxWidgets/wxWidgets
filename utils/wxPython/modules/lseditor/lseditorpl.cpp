/////////////////////////////////////////////////////////////////////////////
// Name:        lseditorpl.cpp
// Purpose:     Language-sensative editor plugin for wxStudio
// Copyright:   (c) Aleksandars Gluchovas
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


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif


#include "lseditorpl.h"
#include "tdefs.h"

/***** Impelmentation for class wxsLSEditorPlugin *****/

wxsLSEditorPlugin::wxsLSEditorPlugin()

	: mpModel( NULL ),
	  mpView( NULL )
{}

wxsLSEditorPlugin::~wxsLSEditorPlugin()
{
	// view is destroyed by wxWindows along
	// with it's owned model
}

void wxsLSEditorPlugin::Create( wxWindow* parent, wxWindowID id )
{
	mpModel = new wxTextEditorModel();

	mpView = new wxTextEditorView( parent, id, mpModel );

	mpModel->AddView( mpView );
	mpView->Activate();

	mpView->AddPinPainter( new TBreakpointPainter() );

	mpView->SyncScrollbars();
}

void wxsLSEditorPlugin::OnOpen( const string& fname )
{
	mpModel->LoadTextFromFile( fname );

	SetFileName( fname );
}

void wxsLSEditorPlugin::OnSave( const string& fname )
{
	mpModel->SaveTextToFile( fname );
}

void wxsLSEditorPlugin::OnCopy()
{
	mpModel->OnCopy();
}

void wxsLSEditorPlugin::OnCut()
{
	mpModel->OnCut();
}

void wxsLSEditorPlugin::OnPaste()
{
	mpModel->OnPaste();
}

void wxsLSEditorPlugin::OnDelete()
{
	mpModel->OnDelete();
}

void wxsLSEditorPlugin::OnUndo()
{
	mpModel->OnUndo();
}

void wxsLSEditorPlugin::OnRedo()
{
	mpModel->OnRedo();
}

void wxsLSEditorPlugin::SelectAll()
{
	mpModel->OnSelectAll();
}

void wxsLSEditorPlugin::OnGotoLine()
{
	mpModel->OnGotoLine();
}

void wxsLSEditorPlugin::OnGotoLine( int lineNo, int column )
{
	mpModel->ResetSelection();
	mpModel->OnGotoLine( lineNo, column );
}

void wxsLSEditorPlugin::OnProperties()
{
	// not impl.
}

void wxsLSEditorPlugin::OnFind()
{
	mpModel->OnFind();
}

void wxsLSEditorPlugin::OnFindNext()
{
	mpModel->OnFindNext();
}

void wxsLSEditorPlugin::OnFindPrevious()
{
	mpModel->OnFindPrevious();
}

void wxsLSEditorPlugin::OnReplace()
{
	// not impl.
}

void wxsLSEditorPlugin::OnToggleBookmark()
{
	mpModel->OnToggleBookmark();
}

void wxsLSEditorPlugin::OnNextBookmark()
{
	mpModel->OnNextBookmark();
}

void wxsLSEditorPlugin::OnPreviousBookmark()
{
	mpModel->OnPreviousBookmark();
}

void wxsLSEditorPlugin::OnShowBookmarks()
{
	// not impl.
}

void wxsLSEditorPlugin::SetCheckpoint()
{
	mpModel->SetCheckpoint();
}

bool wxsLSEditorPlugin::CheckpointModified()
{
	return mpModel->CheckpointModified();
}

// UI-updates

bool wxsLSEditorPlugin::CanCopy()
{
	return mpModel->CanCopy();
}

bool wxsLSEditorPlugin::CanCut()
{
	return mpModel->CanCopy();
}

bool wxsLSEditorPlugin::CanPaste()
{
	return mpModel->CanPaste();
}

bool wxsLSEditorPlugin::CanUndo()
{
	return mpModel->CanUndo();
}

bool wxsLSEditorPlugin::CanRedo()
{
	return mpModel->CanRedo();
}

// accesed by framework

bool wxsLSEditorPlugin::IsModified()
{
	return mpModel->IsModified();
}

wxWindow* wxsLSEditorPlugin::GetWindow()
{
	return mpView;
}

void wxsLSEditorPlugin::GetAllText( char** ppBuf, size_t* length )
{
	mpModel->GetAllText( ppBuf, *length );
}

void wxsLSEditorPlugin::SetFileName( const string& fname )
{
	mFileName = fname;

	if ( mpView ) 
		
		mpView->SetName( fname );
}

void wxsLSEditorPlugin::HoldCursor( bool hold )
{
	mpView->HoldCursor( hold );
}

string wxsLSEditorPlugin::FindWordAtCursor()
{
	mpModel->OnSelectWord();

	char* buf = NULL; size_t len = 0;
	
	mpModel->GetSelection( &buf, len );

	if ( buf ) 
	{
		string word = string( buf, 0, len );
		delete [] buf;
		return word;
	}
	else
		return "";
}

void wxsLSEditorPlugin::GetCursorPos( int* line, int* column )
{
	TPosition pos = mpModel->GetCursor();

	*line   = (int)pos.mRow;
	*column = (int)pos.mCol;
}

void wxsLSEditorPlugin::SetCursorPos( int line, int column )
{
	mpModel->OnGotoLine( line, column );
}

void wxsLSEditorPlugin::GetPagePos( int* line, int* column )
{
	TPosition pos = mpView->GetPagePos();

	*line   = pos.mRow;
	*column = pos.mCol;
}

void wxsLSEditorPlugin::GetText( int fromLine, int fromColumn,
								 int tillLine, int tillColumn,
								 char** ppBuf, size_t* length )
{
	mpModel->GetTextFromRange( TPosition( fromLine, fromColumn ),
							   TPosition( tillLine, tillColumn ),
							   ppBuf, *length );
}

void wxsLSEditorPlugin::InsertText( int line, int column,
								    char* text, size_t lenght )
{
	mpModel->InsertText( TPosition( line, column ),
						 text, lenght );
}

void wxsLSEditorPlugin::DeleteText( int fromLine, int fromColumn,
									int tillLine, int tillColumn  )
{
	mpModel->DeleteRange( TPosition( fromLine, fromColumn ),
						  TPosition( tillLine, tillColumn ) );
}

void wxsLSEditorPlugin::PositionToXY( int line, int column, int* x, int* y )
{
	TPosition scrPos;

	mpView->TextPosToScreenPos( TPosition( line, column ), scrPos );
	mpView->ScreenPosToPixels( scrPos, *x, *y );

	*y += mpView->mCharDim.y; // lower-right corner
}

void wxsLSEditorPlugin::GetSelectionRange( int* fromLine, int* fromColumn,
										   int* tillLine, int* tillColumn  )
{
	TPosition start = mpModel->GetStartOfSelection();
	TPosition end   = mpModel->GetEndOfSelection();

	*fromLine   = (int)start.mRow;
	*fromColumn = (int)start.mCol;
	*tillLine   = (int)end.mRow;
	*tillColumn = (int)end.mCol;
}

wxSize wxsLSEditorPlugin::GetCharacterSize()
{
	return mpView->GetCharacterSize();
}

bool wxsLSEditorPlugin::IsUnixText()
{
	return mpModel->IsUnixText();
}

wxTextEditorModel& wxsLSEditorPlugin::GetModel() 
{ 
	return *mpModel; 
}

wxTextEditorView&  wxsLSEditorPlugin::GetView()  
{ 
	return *mpView;  
}
