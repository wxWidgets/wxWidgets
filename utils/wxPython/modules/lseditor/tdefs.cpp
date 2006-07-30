/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     03/04/1999
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandars Gluchovas
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

#include "wx/file.h"
#include "wx/textdlg.h"
#include "wx/clipbrd.h"
#include "wx/dataobj.h"

#include <stdio.h>

#include "tdefs.h"
#include "finddlg.h"

#include <memory.h>

/***** Implementation for class TBlock *****/

void TBlock::RecalcBlockProperties()
{
	char* cur = mBuf;
	char* end = mBuf + mTextLen;
	mRowCount = 0;

	while( cur < end )
	{
		if ( is_eol_char( *cur ) ) ++mRowCount;

		++cur;
	}
}

/***** Implementation for class TTextIterator *****/

string TTextIterator::mSeparators = ",.()[]\t\\+-*/|=<>:;\t\n~?!%";

bool TTextIterator::IsSeparator( char ch )
{
	size_t sz = mSeparators.size();

	for( size_t i = 0; i != sz; ++i )

		if ( mSeparators[i] == ch ) return TRUE;

	return FALSE;
}

char* TTextIterator::GetClosestPos()
{
	char*  end = GetBlockEnd();
	char*  cur = mpCurRowStart;
	size_t col = 0;

	while( cur < end && col < mPos.mCol && !is_eol_char(*cur) )
	{
		if ( !is_DOS_eol_char( *cur ) ) ++col;
		++cur;
	}

	if ( is_DOS_eol_char( *cur ) ) ++cur;

	return cur;
}

char* TTextIterator::GotoClosestPos()
{
	char*  end = GetBlockEnd();
	char*  cur = mpCurRowStart;
	size_t col = 0;

	while( cur < end && col < mPos.mCol && !is_eol_char(*cur) )
	{
		if ( !is_DOS_eol_char( *cur ) ) ++col;
		++cur;
	}

	mPos.mCol = col;

	if ( is_DOS_eol_char( *cur ) ) ++cur;

	return cur;
}

TTextIterator::TTextIterator()

	: mIsEof( FALSE )
{}

bool TTextIterator::IsLastLine()
{
	TBlockIteratorT nextBlk = mBlockIter;
	++nextBlk;

	if ( nextBlk != mEndOfListIter ) return FALSE;

	char* cur = mpCurRowStart;
	char* end = GetBlockEnd();

	while( cur < end && !is_eol_char( *cur ) ) ++cur;

	if ( cur == end ) return TRUE;

	++cur;

	return ( cur == end );
}

char TTextIterator::GetChar()
{
	char* cur = GetClosestPos();

	if ( is_DOS_eol_char( *cur ) ) 

		return *(cur+1);
	else
		return *cur;
}

bool TTextIterator::IsEol()
{
	return is_eol_char( GetChar() ) || mIsEof;
}

bool TTextIterator::IsEof()
{
	return mIsEof;
}

int TTextIterator::GetDistFromEol()
{
	return 0; // TBD::
}

void TTextIterator::NextChar()
{
	char* cur = GotoClosestPos();

	if ( cur + 1 >= GetBlockEnd() )
	{
		TBlockIteratorT nextBlk = mBlockIter;
		++nextBlk;

		if ( nextBlk == mEndOfListIter )
		{
			if ( cur != GetBlockEnd() ) 
				++mPos.mCol;	

			mIsEof = TRUE;
			return;
		}

		++mPos.mRow ;
		mPos.mCol = 0;

		mBlockIter = nextBlk;

		mFirstRowInBlock = mPos.mRow;
		mActualRow       = mPos.mRow;
		mpCurRowStart    = (*mBlockIter).mBuf;

		mIsEof = ( (*mBlockIter).mTextLen == 0 );
	}
	else
	{
		if ( is_eol_char( *cur ) ) 
		{
			++mPos.mRow;
			mPos.mCol = 0;
			
			mActualRow    = mPos.mRow;
			mpCurRowStart = cur + 1;
		}
		else
			++mPos.mCol;
	}

	mIsEof = (mpCurRowStart + mPos.mCol) == GetBlockEnd();
}

void TTextIterator::PreviousChar()
{
	char* cur = GotoClosestPos();

	if ( cur == (*mBlockIter).mBuf )
	{
		TBlockIteratorT prevBlk = mBlockIter;
		--prevBlk;

		if ( prevBlk == mEndOfListIter )
		{
			mIsEof = TRUE;
			return;
		}

		--mPos.mRow;

		mBlockIter = prevBlk;

		cur = GetBlockEnd() - 1;

		char* eolPos = cur;
		--cur; // skip EOL
		char* start = (*mBlockIter).mBuf;

		while( cur != start && !is_eol_char( *cur ) ) --cur; // goto start of line

		if ( is_eol_char( *cur ) ) ++cur;

		mPos.mCol     = (size_t)(eolPos - cur);
		mpCurRowStart = cur;

		mFirstRowInBlock = mPos.mRow;
		mActualRow       = mPos.mRow;
	}
	else
	{
		do
		{
			// FIXME FIXME:: this is more then messy .... !

			if ( is_eol_char( *(cur-1) ) ) 
			{
				--cur; // goto EOL

				--mPos.mRow;

				char* eolPos = cur;
				--cur; // skip EOL
				char* start = (*mBlockIter).mBuf;

				while( cur != start && !is_eol_char( *cur ) ) --cur; // goto start of line

				if ( is_eol_char( *cur ) ) ++cur;

				mPos.mCol     = (size_t)(eolPos - cur);
				mpCurRowStart = cur;

				if ( eolPos != cur && is_DOS_eol_char( *(eolPos-1) ) ) --mPos.mCol;
				
				mActualRow    = mPos.mRow;

				break;
			}
			else
			if ( is_DOS_eol_char( *(cur-1) ) )
			{
				--cur;

				if ( cur != (*mBlockIter).mBuf && is_eol_char( *(cur-1) ) )

					continue;
				else
				{
					--mPos.mCol;
					--cur;
				}
			}
			else
			{
				--mPos.mCol;
				break;
			}

		} while( 1 );
	}

	mIsEof = (mpCurRowStart + mPos.mCol) == GetBlockEnd();
}

void TTextIterator::NextWord()
{
	GotoClosestPos();

	// skip non-white space ahead

	bool wasSeparator = IsSeparator( GetChar() );

	while( !IsEof() )
	{
		char ch = GetChar();

		if ( ch == ' '  || 
			 ch == '\t' || 
			 is_eol_char(ch) ||
			 wasSeparator != IsSeparator(ch) )

			break;

		NextChar();
	}

	// skip all white stpace if any
	while( !IsEof() )
	{
		char ch = GetChar();

		if ( ch != ' ' && ch != '\t' && !is_eol_char(ch) )

			break;

		NextChar();
	}
}

void TTextIterator::PreviousWord()
{
	GotoClosestPos();

	PreviousChar();

	// skip all white stpace if any
	while( !IsEof() )
	{
		char ch = GetChar();

		if ( ch != ' ' && ch != '\t' && !is_eol_char(ch) )

			break;

		PreviousChar();
	}

	bool wasSeparator = IsSeparator( GetChar() );

	// skip word;
	while( !IsEof() )
	{
		char ch = GetChar();

		if ( ch == ' '       || 
			 ch == '\t'      || 
			 is_eol_char(ch) ||
			 wasSeparator != IsSeparator(ch) 
		   )
		{
			NextChar();
			break;
		}

		PreviousChar();
	}
}

void TTextIterator::ToEndOfLine()
{
	GotoClosestPos();

	while( !IsEof() )
	{
		char ch = GetChar();

		if ( is_eol_char( ch ) ) break;

		NextChar();
	}
}

void TTextIterator::ToStartOfLine()
{
	GotoClosestPos();

	mPos.mCol = 0;
	mPos.mRow = mActualRow;
}

size_t TTextIterator::GetLineLen()
{
	char* cur = mpCurRowStart;
	char* end = GetBlockEnd();

	size_t len = 0;

	while( cur < end && !is_eol_char( *cur ) )
	{
		if ( !is_DOS_eol_char( *cur ) ) ++len;
		++cur;
	}

	return len;
}

TPosition TTextIterator::GetPosition()
{
	return mPos;
}

bool TTextIterator::IsInLastBlock()
{
	TBlockIteratorT next = mBlockIter;
	++next;

	return next == mEndOfListIter;
}

bool TTextIterator::DetectUnixText()
{
	char* cur = GetBlockStart();
	char* end = GetBlockEnd();

	bool isUnixText = IS_UNIX_TEXT_BY_DEFAULT;

	while( cur < end ) 
	{
		if ( is_DOS_eol_char( *cur ) ) return FALSE;

		if ( is_eol_char( *cur ) ) return TRUE;

		++cur;
	}

	return isUnixText;
}

/***** Implementation for class TCppJavaHighlightListener *****/

void TCppJavaHighlightListener::OnTextChanged( wxTextEditorModel* pModel, 
											   size_t atRow, size_t nRows, 
											   TEXT_CHANGE_TYPE ct ) 
{
	mpModel = pModel;
	
	/*

	int state = GetStateAtRow( atRow );

	if ( ct == CT_INSERTED )
	{
		RemoveCommentTags( atRow, atRow + nRows + 1 );
		GenerateTagsForRange( atRows, atRows + nRows + 1 );
	}
	else
	if ( ct == CT_DELETED )
	{
		RemoveCommentTags( atRow, atRow + 1 );
		GenerateTagsForRange( atRows, atRows + 1 );
	}
	*/
}

/***** Implementation for class wxTextEditorModel *****/

/*** protected methods ***/

size_t wxTextEditorModel::GetLineCountInRange( char* from, char* till )
{
	size_t nLines = 0;

	while( from != till )
	{
		if ( is_eol_char( *from ) ) ++nLines;

		++from;
	}

	return nLines;
}

void wxTextEditorModel::DoInsertText( const TPosition& pos, 
									  char* text, size_t len,
									  TRange& actualRange )
{
	// FOR NOW:: very dummy imp.

	char* end = text + len;

	TTextIterator iter = CreateIterator( pos );

	TBlock& blk = (*iter.mBlockIter);

	char* cur = text;

	char*  insertPos = iter.GotoClosestPos();
	actualRange.mFrom = iter.GetPosition();

	if ( is_eol_char( *insertPos ) &&
		 insertPos != iter.GetBlockStart()   &&
		 is_DOS_eol_char( *(insertPos-1) )
	   )
	   --insertPos;

	size_t sizeAfter = (size_t)(iter.GetBlockEnd() - insertPos);

	size_t nLines = GetLineCountInRange( text, text + len );

	if ( blk.mTextLen + len < FILLED_BLOCK_LEN )
	{
		memmove( insertPos + len, insertPos, sizeAfter );

		memcpy( insertPos, text, len );

		blk.mTextLen += len;
		

		blk.RecalcBlockProperties();

		if ( iter.IsInLastBlock() )

			++blk.mRowCount; // last block have always the-last-row-to-spare -
		                     // the "nature" of most text editors

		char* endPos = insertPos + len;

		bool found = FALSE;

		/*
		// OLD STUFF:: slow & buggy

		while( !iter.IsEof() )
		{
			if ( iter.GetClosestPos() == endPos )
			{
				actualRange.mTill = iter.GetPosition();
				found = TRUE;
				break;
			}

			iter.NextChar();
		}

		if ( !found )
		{
			actualRange.mTill = iter.GetPosition();
			++actualRange.mTill.mCol;
			
			//T_ASSERT( found ); // DBG::
		}
		*/

		actualRange.mTill       = actualRange.mFrom;
		actualRange.mTill.mRow += nLines;

		if ( nLines == 0 )

			actualRange.mTill.mCol = actualRange.mFrom.mCol + (len);
		else
		{
			cur = end;

			while( cur != insertPos && !is_eol_char( *cur ) )

				--cur;

			if ( is_eol_char( *cur ) ) ++cur;

			actualRange.mTill.mCol = (int)(end - cur);
		}

		NotifyTextChanged( pos.mRow, nLines, CT_INSERTED );
	}
	else
	{
		// TBD:::

		char buf[16];
		sprintf( buf, "%d", FILLED_BLOCK_LEN );
		string msg = "Sorry!!! Currently editor is limited to files less then ";
		msg += buf;
		msg += " bytes\n(the requested text length is " + 
		sprintf( buf, "%d", blk.mTextLen + len );
		msg += buf; 
		msg += " bytes)\n Please, close this file without making any changes.";

		wxMessageBox( msg );

		GetActiveView()->SetFocus();

		//T_ASSERT(0); // DBG:: for now
	}
}

void wxTextEditorModel::DoDeleteRange( const TPosition& from, const TPosition& till,
									   TRange& actualRange
									 )
{
	// FOR NOW:: very dummy imp.

	TTextIterator iterFrom = CreateIterator( from );
	TTextIterator iterTill = CreateIterator( till );

	if ( iterFrom.mBlockIter == iterTill.mBlockIter )
	{
		char* fromPos = iterFrom.GotoClosestPos();
		char* tillPos = iterTill.GotoClosestPos();
		char* blockStart = (*iterFrom.mBlockIter).mBuf;
		
		if ( is_eol_char( *fromPos ) &&
			 fromPos != blockStart   &&
			 is_DOS_eol_char( *(fromPos-1) )
		   )
		   --fromPos;

		if ( is_eol_char( *tillPos ) &&
			 tillPos != blockStart   &&
			 is_DOS_eol_char( *(tillPos-1) )
		   )
		   --tillPos;

		size_t len = (size_t)( tillPos -fromPos );
		
		size_t nLines = GetLineCountInRange( fromPos, fromPos + len );

		size_t sizeAfter = (size_t)(iterFrom.GetBlockEnd() - tillPos);

		memmove( fromPos, tillPos, sizeAfter );

		(*iterFrom.mBlockIter).mTextLen -= len;

		(*iterFrom.mBlockIter).RecalcBlockProperties();

		if ( iterFrom.IsInLastBlock() )

			++(*iterFrom.mBlockIter).mRowCount; // last block have always the-last-row-to-spare -
												// the "nature" of most text editors

		actualRange.mFrom = iterFrom.GetPosition();
		actualRange.mTill = iterTill.GetPosition();

		NotifyTextChanged( from.mRow, nLines, CT_DELETED );
	}
	else
		T_ASSERT(0); // DBG:: for now
}

void wxTextEditorModel::GetTextFromRange( const TPosition& from, const TPosition& till, 
										  char** text, size_t& textLen 
										)
{
	TTextIterator iterFrom = CreateIterator( from );
	TTextIterator iterTill = CreateIterator( till );

	if ( iterFrom.mBlockIter == iterTill.mBlockIter )
	{
		char* blockStart = (*iterFrom.mBlockIter).mBuf;

		char* fromPos = iterFrom.GetClosestPos();
		char* tillPos = iterTill.GetClosestPos();

		if ( is_eol_char( *fromPos ) &&
			 fromPos != blockStart   &&
			 is_DOS_eol_char( *(fromPos-1) )
		   )
		   --fromPos;

		if ( is_eol_char( *tillPos ) &&
			 tillPos != blockStart   &&
			 is_DOS_eol_char( *(tillPos-1) )
		   )
		   --tillPos;

		textLen = (size_t)( tillPos -fromPos );

		*text = AllocCharacters( textLen );

		memcpy( *text, fromPos, textLen );
	}
	else
		T_ASSERT(0); // DBG:: for now
}

void wxTextEditorModel::LoadTextFromFile( const wxString& fname )
{
	T_ASSERT( wxFile::Exists( fname ) );

	DeleteAllText();

	wxFile fl( fname );

	char* buf = AllocCharacters( fl.Length() );

	fl.Read( buf, fl.Length() );

	TRange result;
	DoInsertText( TPosition( 0,0 ), buf, fl.Length(), result );

	FreeCharacters( buf );

	TTextIterator iter = CreateIterator( TPosition( 0,0 ) );

	mIsUnixText = iter.DetectUnixText();

	ClearUndoBuffer();

	NotifyAllViews();
}

void wxTextEditorModel::SaveTextToFile( const wxString& fname )
{
	wxFile fl( fname, wxFile::write );

	char* text = 0;
	size_t len = 0;

	GetTextFromRange( TPosition(0,0), TPosition( GetTotalRowCount()+1,0 ), &text, len );

	fl.Write( text, len );
	fl.Close();

	FreeCharacters( text );
}

void wxTextEditorModel::NotifyTextChanged( size_t atRow, size_t nRows, TEXT_CHANGE_TYPE ct )
{
	if ( nRows > 0  )
	
		MergeChange( atRow, mRowsPerPage );
	else
		MergeChange( atRow, 1 );

	// reposition bookmarsk

	if ( nRows > 0 )
	{
		if ( ct == CT_INSERTED )
		{
			size_t curPin = FindNextPinFrom( atRow + 1 );

			while( curPin != NPOS )
			{
				mPins[curPin]->mRow += nRows;

				++curPin;

				if ( curPin == mPins.size() ) break;
			}
		}
		else
		if ( ct == CT_DELETED )
		{
			size_t curPin  = FindNextPinFrom( atRow + 1 );
			size_t fromPin = curPin;
			size_t tillRow = atRow + nRows;

			while( curPin != NPOS && mPins[curPin]->mRow < tillRow )
			{
				++curPin;

				if ( curPin == mPins.size() ) break;
			}

			if ( fromPin != NPOS && nRows != 0 )
			{
				mPins.erase( &mPins[fromPin], &mPins[curPin] );

				while( curPin < mPins.size() )
				{
					mPins[curPin]->mRow -= nRows;
					++curPin;
				}
			}
		}
	}


	// send notificaitons

	for( size_t i = 0; i != mChangeListeners.size(); ++i )

		mChangeListeners[i]->OnTextChanged( this, atRow, nRows, ct );
}

void wxTextEditorModel::NotifyTextChanged( TPosition from, TPosition till, TEXT_CHANGE_TYPE ct )
{
	ArrangePositions( from, till );

	NotifyTextChanged( from.mRow, till.mRow - from.mRow + 1, ct );
}

void wxTextEditorModel::DoExecuteNewCommand( TCommand& cmd )
{
	if ( cmd.mType == TCMD_INSERT )
	{
		cmd.mPrePos = mCursorPos;
		DoInsertText( cmd.mRange.mFrom, cmd.mData, cmd.mDataLen, cmd.mRange );
	}
	else
	if ( cmd.mType == TCMD_DELETE )
	{
		cmd.mPrePos = mCursorPos;
		DoDeleteRange( cmd.mRange.mFrom, cmd.mRange.mTill, cmd.mRange );
	}
}

void wxTextEditorModel::DoReexecuteCommand( TCommand& cmd )
{
	NotifyTextChanged( mCursorPos.mRow, 1, CT_MODIFIED ); // indicate update of current cursor position

	if ( cmd.mType == TCMD_INSERT )
	{
		DoInsertText( cmd.mRange.mFrom, cmd.mData, cmd.mDataLen, cmd.mRange );
		mCursorPos = cmd.mPostPos;
	}
	else
	if ( cmd.mType == TCMD_DELETE )
	{
		DoDeleteRange( cmd.mRange.mFrom, cmd.mRange.mTill, cmd.mRange );
		mCursorPos = cmd.mPostPos;
	}

	NotifyTextChanged( mCursorPos.mRow, 1, CT_MODIFIED ); // indicate update of current cursor position
}

void wxTextEditorModel::DoUnexecuteCommand( TCommand& cmd )
{
	NotifyTextChanged( mCursorPos.mRow, 1, CT_MODIFIED ); // indicate update of current cursor position
	
	if ( cmd.mType == TCMD_INSERT )
	{
		DoDeleteRange( cmd.mRange.mFrom, cmd.mRange.mTill, cmd.mRange );
		mCursorPos = cmd.mPrePos;
	}
	else					
	if ( cmd.mType == TCMD_DELETE )
	{
		DoInsertText( cmd.mRange.mFrom, cmd.mData, cmd.mDataLen, cmd.mRange );
		mCursorPos = cmd.mPrePos;
	}

	NotifyTextChanged( mCursorPos.mRow, 1, CT_MODIFIED ); // indicate update of current cursor position
}

void wxTextEditorModel::UndoImpl()
{
	--mCurCommand;

	DoUnexecuteCommand( *mCommands[mCurCommand] );
}

void wxTextEditorModel::RedoImpl()
{
	DoReexecuteCommand( *mCommands[mCurCommand] );

	++mCurCommand;
}

void wxTextEditorModel::ExecuteCommand( TCommand* pCmd )
{
	if ( mCurCommand < mCheckPointCmdNo )

		// new command is executed before the checkpoint,
		// and every thing is sliced - invalidate it

		mCheckPointDestroyed = TRUE;

	// slice undo-able commands ahead in the queue,
	// they wont ever be reexecuted

	while( mCommands.size() > mCurCommand )
	{
		delete mCommands.back();

		mCommands.pop_back();
	}

	mCommands.push_back( pCmd );

	DoExecuteNewCommand( *pCmd );
	++mCurCommand;
}

bool wxTextEditorModel::CanPrependCommand( TCommand* pCmd )
{
	if ( mCommands.size() != mCurCommand ||
		 mCommands.size() == 0 ) 
		 
		 return FALSE;

	TCommand& prevCmd = *mCommands.back();

	if ( !(prevCmd.mRange.mTill == pCmd->mRange.mFrom) ) 

		return FALSE;

	char prevCh = prevCmd.mData[ prevCmd.mDataLen - 1];
	char curCh  = pCmd->mData[0];

	if ( prevCh == curCh ) return TRUE;

	if ( prevCh == ' ' || curCh == ' ') return FALSE;

	if ( TTextIterator::IsSeparator(prevCh) !=
		 TTextIterator::IsSeparator(curCh) )

		 return FALSE;
	
	return TRUE;
}

void wxTextEditorModel::PrependCommand( TCommand* pCmd )
{
	if ( mCheckPointCmdNo == mCurCommand )

		mCheckPointDestroyed = TRUE;

	TCommand& prevCmd = *mCommands.back();

	DoExecuteNewCommand( *pCmd );

	TCommand* pComb = new TCommand();

	pComb->mType    = TCMD_INSERT;
	pComb->mDataLen = prevCmd.mDataLen + pCmd->mDataLen;

	pComb->mData        = AllocCharacters( pComb->mDataLen );
	pComb->mRange.mFrom = prevCmd.mRange.mFrom;
	pComb->mRange.mTill = pCmd->mRange.mTill;
	pComb->mPrePos      = prevCmd.mPrePos;
	pComb->mPostPos     = pCmd->mPostPos;

	memcpy( pComb->mData, prevCmd.mData, prevCmd.mDataLen );
	memcpy( pComb->mData + prevCmd.mDataLen, pCmd->mData, pCmd->mDataLen );

	FreeCharacters( prevCmd.mData );
	FreeCharacters( pCmd->mData );

	delete &prevCmd;
	delete pCmd;

	mCommands[ mCommands.size() - 1 ] = pComb;
}

void wxTextEditorModel::SetPostPos( const TPosition& pos )
{
	mCommands[mCurCommand-1]->mPostPos = pos;
}

bool wxTextEditorModel::SelectionIsEmpty()
{
	return mSelectionStart == mSelectionEnd;
}

void wxTextEditorModel::StartBatch()
{
	// TBD::
}

void wxTextEditorModel::FinishBatch()
{
	// TBD::
}

void wxTextEditorModel::DeleteRange( const TPosition& from, const TPosition& till )
{
	TCommand* pCmd = new TCommand();

	pCmd->mType     = TCMD_DELETE;

	pCmd->mRange.mFrom = from;
	pCmd->mRange.mTill = till;
	pCmd->mPrePos      = mCursorPos;

	GetTextFromRange( from, till, &pCmd->mData, pCmd->mDataLen );

	ExecuteCommand( pCmd );
}

void wxTextEditorModel::InsertText( const TPosition& pos, const char* text, size_t len )
{
	TCommand* pCmd = new TCommand();

	pCmd->mType     = TCMD_INSERT;

	pCmd->mRange.mFrom = pos;

	pCmd->mData     = AllocCharacters( len, text ), 
	pCmd->mDataLen  = len;
	pCmd->mPrePos   = mCursorPos;

	ExecuteCommand( pCmd );
}

void wxTextEditorModel::DeleteSelection()
{
	DeleteRange( mSelectionStart, mSelectionEnd );

	ResetSelection();	
}

bool wxTextEditorModel::IsLastLine( const TPosition& pos )
{
	return FALSE;
}

TTextIterator wxTextEditorModel::CreateIterator( const TPosition& pos )
{
	size_t curRow = 0;

	TBlockIteratorT bIter = mBlocks.begin();

	TTextIterator tIter;

	while( bIter != mBlocks.end() )
	{
		TBlockIteratorT nextBlk = bIter;
		++nextBlk;

		if ( nextBlk == mBlocks.end() ||
			 ( pos.mRow >= curRow && 
			   pos.mRow <= curRow + (*bIter).mRowCount )
		   )
		{
			tIter.mFirstRowInBlock = curRow;

			char* cur = (*bIter).mBuf;
			char* end = cur + (*bIter).mTextLen;

			// slightly optimized

			if ( curRow < pos.mRow )
			{
				while( cur < end )
				{
					if ( is_eol_char( *cur ) ) 
					{
						++curRow;

						if ( !(curRow < pos.mRow) ) 
						{
							++cur;
							break;
						}
					}

					++cur;
				}
			}

			tIter.mActualRow    = curRow;
			tIter.mpCurRowStart = cur;
			tIter.mPos          = pos;

			// FOR NOW:: positioning past the end of file is not supported
			tIter.mPos.mRow     = curRow;

			tIter.mBlockIter     = bIter;
			tIter.mEndOfListIter = mBlocks.end();

			break;
		}
		else
		{
			curRow += (*bIter).mRowCount;
			++bIter;
		}
	}

	return tIter;
}

void wxTextEditorModel::ArrangePositions( TPosition& upper, TPosition& lower )
{
	if ( upper > lower )
	{
		TPosition tmp( lower );
		lower = upper;
		upper = tmp;
	}
}

void wxTextEditorModel::ArrangePositions( size_t& upper, size_t& lower )
{
	if ( upper > lower )
	{
		size_t tmp = lower;
		lower = upper;
		upper = tmp;
	}
}

void wxTextEditorModel::MergeChange( size_t fromRow, size_t nRows )
{
	if ( mTextChanged == FALSE )
	{
		mChangedFromRow = fromRow;
		mChangedTillRow = fromRow + nRows;
		mTextChanged    = TRUE;
	}
	else
	{
		if ( mChangedFromRow > fromRow )

			mChangedFromRow = fromRow;

		if ( mChangedTillRow < fromRow + nRows )

			mChangedTillRow = fromRow + nRows;
	}
}

void wxTextEditorModel::TrackSelection()
{
	if ( !mIsSelectionEditMode ) return;

	if ( mPrevCursorPos == mSelectionStart )

		mSelectionStart = mCursorPos;
	else
		mSelectionEnd   = mCursorPos;
		
	ArrangePositions( mSelectionStart, mSelectionEnd );

	NotifyTextChanged( mSelectionStart, mPrevSelectionStart, CT_MODIFIED );
	NotifyTextChanged( mSelectionEnd,   mPrevSelectionEnd,   CT_MODIFIED );
}

void wxTextEditorModel::CheckSelection()
{
	ArrangePositions( mSelectionStart, mSelectionEnd );

	if ( mIsSelectionEditMode && SelectionIsEmpty() )
	{
		mSelectionStart = mCursorPos;
		mSelectionEnd   = mCursorPos;
	}

	if ( !mIsSelectionEditMode && !SelectionIsEmpty() )
	{
		ResetSelection();
	}

	mPrevSelectionStart = mSelectionStart;
	mPrevSelectionEnd   = mSelectionEnd;
	mPrevCursorPos      = mCursorPos;
}

void wxTextEditorModel::ResetSelection()
{
	if ( SelectionIsEmpty() ) return;

	MergeChange( mSelectionStart.mRow, 
				 mSelectionEnd.mRow - mSelectionStart.mRow + 1 );

	NotifyTextChanged( mSelectionStart, mSelectionEnd, CT_MODIFIED );

	mSelectionStart = TPosition(0,0);
	mSelectionEnd   = TPosition(0,0);

}

void wxTextEditorModel::ClearUndoBuffer()
{
	for( size_t i = 0; i != mCommands.size(); ++i )
	{
		TCommand& cmd = *mCommands[i];

		if ( cmd.mData ) delete [] cmd.mData;

		delete &cmd;
	}

	mCommands.erase( mCommands.begin(), mCommands.end() );

	mCurCommand = 0;
}

void wxTextEditorModel::GetAllText( char** text, size_t& textLen )
{
	GetTextFromRange( TPosition(0,0), TPosition( GetTotalRowCount()+1, 0 ),
		              text, textLen
		            );
}

void wxTextEditorModel::DeleteAllText()
{
	ResetSelection();

	DeleteRange( TPosition(0,0), TPosition( GetTotalRowCount()+1, 0 ) );
}

void wxTextEditorModel::SetSelectionEditMode( bool editIsOn )
{
	mIsSelectionEditMode = editIsOn;
}

size_t wxTextEditorModel::GetTotalRowCount()
{
	size_t nRows = 0;

	for( TBlockIteratorT i = mBlocks.begin(); i != mBlocks.end(); ++i )

		nRows += (*i).mRowCount;

	return nRows;
}

void wxTextEditorModel::GetSelection( char** text, size_t& textLen )
{
	GetTextFromRange( GetStartOfSelection(), GetEndOfSelection(), text, textLen );
}

void wxTextEditorModel::NotifyView()
{
	mpActiveView->OnModelChanged();
}

void wxTextEditorModel::NotifyAllViews()
{
	for( size_t i = 0; i != mViews.size(); ++i )

		mViews[i]->OnModelChanged();
}

void wxTextEditorModel::PrepreForCommand()
{
	mTextChanged    = 0;
	mChangedFromRow = 0;
	mChangedTillRow = 0;
}

size_t wxTextEditorModel::TextToScrColumn( const TPosition& pos )
{
	TPosition spos;

	mpActiveView->TextPosToScreenPos( pos, spos );
	
	return spos.mCol + mpActiveView->GetPagePos().mCol;
}

size_t wxTextEditorModel::ScrToTextColumn( TPosition pos )
{
	TPosition tpos;

	pos.mCol -= mpActiveView->GetPagePos().mCol;
	pos.mRow -= mpActiveView->GetPagePos().mRow;

	mpActiveView->ScreenPosToTextPos( pos, tpos );

	return tpos.mCol;
}

void wxTextEditorModel::DoMoveCursor( int rows, int cols )
{
	mCursorPos.mCol = TextToScrColumn( mCursorPos );

	mCursorPos.mRow += rows;
	mCursorPos.mCol += cols;

	mCursorPos.mCol = ScrToTextColumn( mCursorPos );
}

/*** public interface ***/

wxTextEditorModel::wxTextEditorModel()

	: 
	  mpActiveView( NULL ),
	  mTabSize( 4 ),
	  mIsSelectionEditMode( FALSE ),
	  mRowsPerPage( 0 ),
	  mTextChanged( FALSE ),
	  mCurCommand( 0 ),

	  mInsertMode     ( TRUE  ),
	  mAutoIndentMode ( TRUE  ),
	  mSmartIndentMode( TRUE  ),
	  mWasChanged     ( FALSE ),
	  mIsReadOnly     ( FALSE ),
	  mIsUnixText     ( IS_UNIX_TEXT_BY_DEFAULT )
{
	// at least one block should be present
	// (otherwise text-iterators wont work)

	mBlocks.push_back( TBlock() );
}

wxTextEditorModel::~wxTextEditorModel()
{
	ClearUndoBuffer();
}

char* wxTextEditorModel::AllocCharacters( size_t n )
{
	return new char[n];
}

char* wxTextEditorModel::AllocCharacters( size_t n, const char* srcBuf )
{
	char* destBuf = AllocCharacters( n );

	memcpy( destBuf, srcBuf, n );

	return destBuf;
}

void wxTextEditorModel::FreeCharacters( char* buf )
{
	delete [] buf;
}

void wxTextEditorModel::OnInsertChar( char ch )
{
	if ( ch == 27 ) return; // hack

	if ( is_DOS_eol_char( ch ) ) ch = '\n';

	PrepreForCommand();
	StartBatch();

	TCommand* pCmd = new TCommand();

	pCmd->mType    = TCMD_INSERT;

	if ( ch == '\n' && !mIsUnixText ) 
	{
		// DOS text with CR-LF pair
		pCmd->mData    = AllocCharacters( 2 );
		pCmd->mDataLen = 2;
		pCmd->mData[0] = (char)13;
		pCmd->mData[1] = (char)10;
	}
	else
	{
		pCmd->mData    = AllocCharacters( 1 );
		pCmd->mDataLen = 1;
		pCmd->mData[0] = ch;
	}


	if ( !SelectionIsEmpty() )
	{
		mCursorPos = mSelectionStart;
		DeleteSelection();
	}

	pCmd->mRange.mFrom = mCursorPos;

	if ( mInsertMode == FALSE )
	{
		TPosition nextPos( mCursorPos.mRow, mCursorPos.mCol + 1 );
		DeleteRange( mCursorPos, nextPos );

		SetPostPos( mCursorPos );
	}

	TTextIterator iter = CreateIterator( mCursorPos );

	size_t lineLen = iter.GetLineLen();

	bool indentAdded = FALSE;

	if ( mCursorPos.mCol > lineLen )
	{

		wxString s( ' ', mCursorPos.mCol - lineLen );
		InsertText( TPosition( mCursorPos.mRow, lineLen ), s.c_str(), s.length() );

		SetPostPos( mCursorPos );

		indentAdded = TRUE;
	}

	if ( CanPrependCommand( pCmd ) || indentAdded )

		PrependCommand( pCmd );
	else
		ExecuteCommand( pCmd );

	++mCursorPos.mCol;

	if ( is_eol_char( ch ) )
	{
		mCursorPos.mCol = 0;
		++mCursorPos.mRow;

		SetPostPos( mCursorPos );

		if ( mAutoIndentMode )
		{
			iter.ToStartOfLine();
			wxString indent;

			while( !iter.IsEol() )
			{
				char ch = iter.GetChar();

				if ( ch == '\t' || ch == ' ' )

					indent += ch;
				else
					break;

				iter.NextChar();
			}

			if ( indent.length() )
			{
				// auto-indent is always prepended to the command which
				// caused it

				mCursorPos = TPosition( mCursorPos.mRow, 0 );


				TCommand* pICmd = new TCommand();
				pICmd->mType    = TCMD_INSERT;
				pICmd->mData    = AllocCharacters( indent.length() );
				pICmd->mDataLen = indent.length();
				memcpy( pICmd->mData, indent, indent.length() );

				pICmd->mRange.mFrom = TPosition( mCursorPos.mRow, 0 );

				PrependCommand( pICmd );

				SetPostPos( mCursorPos );

				mCursorPos.mCol = indent.length();
			}
		}
	}
	else
		SetPostPos( mCursorPos );

	FinishBatch();

	NotifyAllViews();
}

void wxTextEditorModel::OnDelete()
{
	PrepreForCommand();
	StartBatch();

	if ( !SelectionIsEmpty() )
	{
		TPosition startPos = mSelectionStart;
		DeleteSelection();
		mCursorPos = startPos;
	}
	else
	{
		TTextIterator iter = CreateIterator( mCursorPos );
		
		if ( iter.GetLineLen() == mCursorPos.mCol && !iter.IsLastLine() )
		{
			TPosition nextPos( mCursorPos.mRow+1, 0 );
			DeleteRange( mCursorPos, nextPos );
			NotifyTextChanged( mCursorPos.mRow, 2, CT_DELETED );
		}
		else
		{
			TPosition nextPos( mCursorPos.mRow, mCursorPos.mCol + 1 );
			DeleteRange( mCursorPos, nextPos );
		}
	}

	SetPostPos( mCursorPos );

	FinishBatch();

	NotifyAllViews();
}

void wxTextEditorModel::OnDeleteBack()
{
	PrepreForCommand();
	StartBatch();

	if ( !SelectionIsEmpty() )
	{
		mCursorPos = mSelectionStart;
		DeleteSelection();
	}
	else
	if ( !(mCursorPos == TPosition(0,0)) )
	{
		TPosition prevPos;

		if ( mCursorPos.mCol == 0 )
		{
			TTextIterator iter = CreateIterator( mCursorPos );
			iter.PreviousChar();

			prevPos = iter.GetPosition();
		}
		else
			prevPos = TPosition( mCursorPos.mRow, mCursorPos.mCol - 1 );

		DeleteRange( prevPos, mCursorPos );

		mCursorPos = prevPos;
	}

	SetPostPos( mCursorPos );

	FinishBatch();

	NotifyAllViews();

}

void wxTextEditorModel::OnDeleteLine()
{
	PrepreForCommand();
	StartBatch();

	DeleteSelection();

	TTextIterator iter = CreateIterator( mCursorPos );

	iter.ToStartOfLine();

	TPosition from = iter.GetPosition();

	iter.ToEndOfLine();

	if ( iter.IsLastLine() == FALSE )

		iter.NextChar(); // delete eol-char also, if it's not the last line

	TPosition till = iter.GetPosition();

	DeleteRange( from, till );
	SetPostPos( mCursorPos );

	FinishBatch();

	NotifyAllViews();
}

void wxTextEditorModel::OnShiftSelectionIndent( bool left )
{
	if ( SelectionIsEmpty() ) return;

	PrepreForCommand();
	StartBatch();

	for( size_t row = mSelectionStart.mRow; row != mSelectionEnd.mRow; ++row )
	{
		TTextIterator iter = CreateIterator( TPosition( row, 0 ) );

		if ( left )
		{
			int n = 0, pos = 0;

			while( !iter.IsEol() && !iter.IsEof() )
			{
				char ch = iter.GetChar();

				if ( pos == mTabSize ) break;

				if ( ch != ' ' && ch != '\t' ) break;

				++n;

				if ( ch == '\t' ) break;

				++pos;

				iter.NextChar();
			}

			if ( n ) DeleteRange( TPosition( row,0 ), TPosition( row, n ) );
		}
		else
		{
			char txt = '\t';

			InsertText( TPosition( row, 0 ), &txt, sizeof(char) );
		}
	}

	FinishBatch();
	NotifyAllViews();
}

void wxTextEditorModel::OnPaste()
{
	// FIXME:: "wxLogQueryInterface(..)" linking problems with MSDev4.0
	
#ifdef __HACK_MY_MSDEV40__

	bool alreadyOpen=wxClipboardOpen();
	if (!alreadyOpen)
	{
		wxOpenClipboard();
	}

	char* data = (char*)::wxGetClipboardData( wxDF_TEXT );

	wxCloseClipboard();

	if ( data == NULL ) return;

	PrepreForCommand();
	StartBatch();

	if ( !SelectionIsEmpty() )
	{
		mCursorPos = GetStartOfSelection();
		DeleteSelection();
	}

	InsertText( mCursorPos, data, strlen( data ) );

	delete [] data;
#else

	if ( !wxTheClipboard->Open() ) return;

	wxTextDataObject data;
	if ( !wxTheClipboard->IsSupported(wxDF_TEXT) ) 
	{
		wxTheClipboard->Close();
		return;
	}

	wxTheClipboard->GetData(&data);

	string txt = data.GetText();

	wxTheClipboard->Close();

	PrepreForCommand();
	StartBatch();

	DeleteSelection();

	InsertText( mCursorPos, txt.c_str(), txt.length() );
#endif


	mCursorPos = mCommands.back()->mRange.mTill;
	SetPostPos( mCursorPos );

	FinishBatch();
	NotifyAllViews();
}

void wxTextEditorModel::OnCut()
{
	OnCopy();

	PrepreForCommand();
	StartBatch();

	DeleteSelection();
	SetPostPos( mCursorPos );


	FinishBatch();
	NotifyAllViews();
}

void wxTextEditorModel::OnCopy()
{
	if ( !SelectionIsEmpty() )
	{
		size_t len  = 0;
		char*  text = NULL;

#ifndef __HACK_MY_MSDEV40__

		if ( !wxTheClipboard->Open() ) return;

		GetTextFromRange( mSelectionStart, mSelectionEnd, &text, len );

		wxString s( text, len );

	    wxTheClipboard->AddData( new wxTextDataObject(s) );
		wxTheClipboard->Close();

		FreeCharacters( text );
#else
		bool alreadyOpen=wxClipboardOpen();
		if (!alreadyOpen)
		{
			wxOpenClipboard();
			if (!wxEmptyClipboard()) 
			{
				wxCloseClipboard();
				return;
			}
		}

		GetTextFromRange( mSelectionStart, mSelectionEnd, &text, len );

		wxString s( text, len );

		bool success = ::wxEmptyClipboard();

		success = wxSetClipboardData( wxDF_TEXT, (wxObject*)s.c_str(), 0,0 );

		FreeCharacters( text );

		wxCloseClipboard();

#endif
	}
}

bool wxTextEditorModel::CanCopy()
{
	return !SelectionIsEmpty();
}

bool wxTextEditorModel::CanPaste()
{
	if ( mIsReadOnly ) return FALSE;

#ifndef __HACK_MY_MSDEV40__

	if ( !wxTheClipboard->Open() ) return FALSE;

	if ( !wxTheClipboard->IsSupported(wxDF_TEXT) )
		return FALSE;

	wxTheClipboard->Close();

	return TRUE;

#else

	bool success = ::wxClipboardOpen();

	bool alreadyOpen=wxClipboardOpen();
	if (!alreadyOpen)
	{
		wxOpenClipboard();
	}

	char* data = (char*)::wxGetClipboardData( wxDF_TEXT );

	wxCloseClipboard();

	if ( data != NULL && strlen(data) != 0 )
	{
		delete [] data;
		return TRUE;
	}
	else
	{
		delete [] data;
		return FALSE;
	}

#endif

}

bool wxTextEditorModel::CanUndo()
{
	return !( mCommands.size() == 0 ||
			  mCurCommand == 0 );
}

bool wxTextEditorModel::CanRedo()
{
	return mCurCommand != mCommands.size();
}

void wxTextEditorModel::OnUndo()
{
	if ( !CanUndo() ) return;

	PrepreForCommand();
	StartBatch();

	ResetSelection();

	UndoImpl();

	FinishBatch();
	NotifyAllViews();
}

void wxTextEditorModel::OnRedo()
{
	if ( !CanRedo() ) return;

	PrepreForCommand();
	StartBatch();

	ResetSelection();

	RedoImpl();

	FinishBatch();
	NotifyAllViews();
}

void wxTextEditorModel::OnMoveLeft()
{
	PrepreForCommand();
	CheckSelection();

	if ( mCursorPos.mCol == 0 )
	{
		if ( mCursorPos.mRow != 0 )
		{
			--mCursorPos.mRow;

			TTextIterator iter = CreateIterator( mCursorPos );

			iter.ToEndOfLine();

			mCursorPos.mCol = iter.GetPosition().mCol;
		}
	}
	else
		--mCursorPos.mCol;

	TrackSelection();
	NotifyView();
}

void wxTextEditorModel::OnMoveRight()
{
	PrepreForCommand();
	CheckSelection();

	++mCursorPos.mCol;

	TrackSelection();
	NotifyView();
}

void wxTextEditorModel::OnMoveUp()
{
	PrepreForCommand();
	CheckSelection();

	if ( mCursorPos.mRow != 0 )
	
		DoMoveCursor( -1,0 );

	TrackSelection();
	NotifyView();
}

void wxTextEditorModel::OnMoveDown()
{
	PrepreForCommand();
	CheckSelection();

	if ( mCursorPos.mRow + 1 < GetTotalRowCount() )

		DoMoveCursor( 1,0 );

	TrackSelection();
	NotifyView();
}

void wxTextEditorModel::OnWordRight()
{
	PrepreForCommand();
	CheckSelection();

	TTextIterator iter = CreateIterator( mCursorPos );

	iter.NextWord();

	mCursorPos = iter.GetPosition();

	TrackSelection();
	NotifyView();
}

void wxTextEditorModel::OnWordLeft()
{
	PrepreForCommand();
	CheckSelection();

	TTextIterator iter = CreateIterator( mCursorPos );

	iter.PreviousWord();

	mCursorPos = iter.GetPosition();

	TrackSelection();
	NotifyView();
}

void wxTextEditorModel::OnMoveToPosition( const TPosition& pos )
{
	PrepreForCommand();
	CheckSelection();

	mCursorPos = pos;

	TrackSelection();
	NotifyView();
}

void wxTextEditorModel::OnEndOfLine()
{
	PrepreForCommand();
	CheckSelection();

	TTextIterator iter = CreateIterator( mCursorPos );
	iter.ToEndOfLine();

	mCursorPos = iter.GetPosition();

	TrackSelection();
	NotifyView();
}

void wxTextEditorModel::OnStartOfLine()
{
	PrepreForCommand();
	CheckSelection();

	int prevCol = mCursorPos.mCol;

	TTextIterator iter = CreateIterator( mCursorPos );
	iter.ToStartOfLine();

	// bypass leading white-space at the begining of the line

	while( !iter.IsEol() )
	{
		char ch = iter.GetChar();

		if ( ch != ' ' && ch != '\t' ) break;

		++mCursorPos.mCol;

		iter.NextChar();
	}

	mCursorPos = iter.GetPosition();

	if ( mCursorPos.mCol == prevCol )

		mCursorPos.mCol = 0;

	TrackSelection();
	NotifyView();
}

void wxTextEditorModel::OnPageUp()
{
	PrepreForCommand();
	CheckSelection();

	if ( mCursorPos.mRow < mRowsPerPage )

		mCursorPos.mRow = 0;
	else
		DoMoveCursor( -mRowsPerPage,0 );

	mpActiveView->ScrollView( -(int)mRowsPerPage, 0 );

	TrackSelection();
	NotifyView();
}

void wxTextEditorModel::OnPageDown()
{
	PrepreForCommand();
	CheckSelection();

	if ( mCursorPos.mRow + mRowsPerPage >= GetTotalRowCount() )
	{
		if ( GetTotalRowCount() != 0 )

			mCursorPos.mRow = GetTotalRowCount() - 1;
		else
			mCursorPos.mRow = 0;
	}
	else
		DoMoveCursor( mRowsPerPage,0 );

	mpActiveView->ScrollView( mRowsPerPage, 0 );

	TrackSelection();
	NotifyView();
}

void wxTextEditorModel::OnSlideUp()
{
	PrepreForCommand();
	
	if ( mpActiveView->GetPagePos().mRow + mRowsPerPage - 1 == mCursorPos.mRow  )
	{
		if ( mCursorPos.mRow == 0 )

			return;

		DoMoveCursor( -1,0 );
	}

	mpActiveView->ScrollView( -1, 0 );

	NotifyView();
}

void wxTextEditorModel::OnSlideDown()
{
	PrepreForCommand();
	
	if ( mCursorPos.mRow == mpActiveView->GetPagePos().mRow )
	{
		if ( mCursorPos.mRow + 1 >= GetTotalRowCount() )

			return;

		DoMoveCursor( 1,0 );
	}

	mpActiveView->ScrollView( 1, 0 );

	NotifyView();
}

void wxTextEditorModel::OnStartOfText()
{
	PrepreForCommand();
	CheckSelection();

	mCursorPos.mRow = mCursorPos.mCol = 0;

	TrackSelection();
	NotifyView();
}

void wxTextEditorModel::OnEndOfText()
{
	PrepreForCommand();
	CheckSelection();

	mCursorPos.mRow = GetTotalRowCount() - 1;

	TTextIterator iter = CreateIterator( mCursorPos );

	iter.ToEndOfLine();

	mCursorPos = iter.GetPosition();

	TrackSelection();
	NotifyView();
}

void wxTextEditorModel::OnSelectWord()
{
	PrepreForCommand();

	TTextIterator iter1 = CreateIterator( mCursorPos );
	iter1.GotoClosestPos();

	if ( mCursorPos == iter1.GetPosition() )
	{
		TTextIterator iter2 = iter1;

		// find the left-edge of the word

		bool wasSeparator = TTextIterator::IsSeparator( iter1.GetChar() );

		while( !iter1.IsEol() )
		{
			char ch = iter1.GetChar();

			if ( ch == '\t' || 
				 ch == ' '  ||
				 wasSeparator != TTextIterator::IsSeparator( iter1.GetChar() )
			  ) 
			{
				iter1.NextChar();
				break;
			}

			iter1.PreviousChar();
		}

		// find the left-edge of the word

		while( !iter2.IsEol() )
		{
			char ch = iter2.GetChar();

			if ( ch == '\t' || 
				 ch == ' '  ||
				 wasSeparator != TTextIterator::IsSeparator( iter2.GetChar() ) 
			)
				break;

			iter2.NextChar();
		}

		if ( !(iter1.GetPosition() == iter2.GetPosition()) )
		{
			mSelectionStart = iter1.GetPosition();
			mSelectionEnd   = iter2.GetPosition();
			mCursorPos      = iter2.GetPosition();

			NotifyTextChanged( mSelectionStart.mRow, 1, CT_MODIFIED );
		}
	}

	NotifyView();
}

void wxTextEditorModel::OnSelectAll()
{
	PrepreForCommand();

	ResetSelection();

	mSelectionStart = TPosition(0,0);
	mSelectionEnd   = TPosition( GetTotalRowCount(), 1024 ); // FOR NOW:: hack

	mCursorPos = mSelectionStart;

	NotifyTextChanged( mSelectionStart.mRow, mSelectionEnd.mRow, CT_MODIFIED );

	NotifyView();
}

void wxTextEditorModel::OnToggleBookmark()
{
	size_t curRow = GetCursor().mRow;

	if ( GetPinAt( curRow, TBookmarkPin::GetPinTypeCode() ) != NULL )

		RemovePinAt( curRow, TBookmarkPin::GetPinTypeCode() );
	else
		AddPin( new TBookmarkPin( curRow ) );

	MergeChange( curRow, 1 );

	NotifyAllViews();
}

void wxTextEditorModel::OnNextBookmark()
{
	size_t pinNo = FindNextPinFrom( mCursorPos.mRow + 1 );

	while( pinNo != NPOS )
	{
		TPinBase& pin = *mPins[pinNo];

		if ( pin.mTypeCode == BOOKMARK_PIN_TC )
		{
			OnGotoLine( pin.mRow, 0 );
			break;
		}

		if ( pinNo == mPins.size() ) break;

		++pinNo;
	}
}

void wxTextEditorModel::OnPreviousBookmark()
{
	if ( mCursorPos.mRow == 0 ) return;

	size_t pinNo = FindPreviousPinFrom( mCursorPos.mRow - 1 );

	while( pinNo != NPOS )
	{
		TPinBase& pin = *mPins[pinNo];

		if ( pin.mTypeCode == BOOKMARK_PIN_TC )
		{
			OnGotoLine( pin.mRow, 0 );
			break;
		}

		if ( pinNo == 0 ) break;

		--pinNo;
	}
}

bool wxTextEditorModel::OnFind()
{
	if ( !SelectionIsEmpty() )
	{
		if ( GetStartOfSelection().mRow == GetEndOfSelection().mRow )
		{
			char* buf = NULL; size_t len = 0;

			GetSelection( &buf, len );

			mLastFindExpr = string( buf, 0, len );

			delete [] buf;
		}
	}

	wxFindTextDialog dlg( GetActiveView(), mLastFindExpr );
	//dlg.SetExpr( mLastFindExpr );

	if( dlg.ShowModal() == wxID_OK )
	{
		mLastFindExpr = dlg.GetExpr();

		GetActiveView()->SetFocus();

		return OnFindNext();
	}

	GetActiveView()->SetFocus();

	return FALSE;
}

bool wxTextEditorModel::OnFindNext()
{
	PrepreForCommand();

	string& val = mLastFindExpr;
	size_t  len = val.length();

	if ( len == 0 ) 
	{
		NotifyView();
		wxMessageBox( "Secarch string not found!" );

		GetActiveView()->SetFocus();

		return FALSE;
	}

	char ch1 = val[0];

	TTextIterator iter = CreateIterator( mCursorPos );

	while( !iter.IsEof() )
	{
		char ch = iter.GetChar();

		if ( ch == ch1 )
		{
			size_t startCol = iter.mPos.mCol;
			iter.NextChar();
			ch = iter.GetChar();

			size_t i = 1;
			while( i < len && !iter.IsEof() && ch == val[i] )
			{
				++i;
				iter.NextChar();
				ch = iter.GetChar();
			}

			if ( i == len )
			{
				if ( !SelectionIsEmpty() )

					ResetSelection();

				SetStartOfSelection( TPosition( iter.mPos.mRow, startCol ) );
				SetEndOfSelection( iter.mPos );

				MergeChange( iter.mPos.mRow, 1 );

				mCursorPos = iter.mPos;

				OnGotoLine( iter.mPos.mRow, iter.mPos.mCol );
				return TRUE;
			}
		}
		else
			iter.NextChar();
	}

	NotifyView();
	MergeChange( mCursorPos.mRow, 2 );
	wxMessageBox( "Secarch string not found!" );

	GetActiveView()->SetFocus();

	return FALSE;
}

bool wxTextEditorModel::OnFindPrevious()
{
	// TBD::
	return FALSE;
}

void wxTextEditorModel::OnGotoLine( int line, int col )
{
	if ( mpActiveView == NULL ) return;

	TPosition pagePos = mpActiveView->GetPagePos();

	if ( line >= pagePos.mRow &&
		 line <  pagePos.mRow + mRowsPerPage )
	{
		mCursorPos.mRow = (size_t)line;
		mCursorPos.mCol = (size_t)col;

		if ( col == - 1)
		{
			mCursorPos.mCol = 0;
			OnStartOfLine();
		}
		else
			NotifyView();

		return;
	}

	size_t third = mRowsPerPage / 3;
	size_t newTop = 0;

	if ( line < third )

		newTop = 0;
	else
		newTop = line - third;


	mpActiveView->ScrollView( (int)newTop - (int)pagePos.mRow, -(int)pagePos.mCol );

	mCursorPos.mRow = line;
	mCursorPos.mCol = col;

	if ( col == - 1)
	{
		mCursorPos.mCol = 0;
		OnStartOfLine();
	}
	else
		NotifyView();
}

void wxTextEditorModel::OnGotoLine()
{
	wxTextEntryDialog* dlg = 
		new wxTextEntryDialog( mpActiveView, "Line number:", "Goto line", "" );

	int nTries = 3;

	while( dlg->ShowModal() == wxID_OK && nTries )
	{
		ResetSelection();

		int i = -1;
		sscanf( dlg->GetValue(), "%d", &i );

		if ( i == -1  )
		{
			wxMessageBox( "Please enter a number" );
			continue;
		}


		if ( i == 0 ) ++i;

		OnGotoLine( (size_t)(i-1), 0 );
		break;

		--nTries;
	}

	GetActiveView()->SetFocus();
}


bool wxTextEditorModel::IsReadOnly()
{
	return mIsReadOnly;
}

bool wxTextEditorModel::IsModified()
{
	return mCurCommand != 0;
}

bool wxTextEditorModel::IsInsertMode()
{
	return mInsertMode;
}

void wxTextEditorModel::SetCheckpoint()
{
	mCheckPointDestroyed = FALSE;
	mCheckPointCmdNo     = mCurCommand;
}

bool wxTextEditorModel::CheckpointModified()
{
	if ( mCheckPointDestroyed ) return TRUE;

	return mCheckPointCmdNo != mCurCommand;
}

TPosition wxTextEditorModel::GetStartOfSelection()
{
	ArrangePositions( mSelectionStart, mSelectionEnd );

	return mSelectionStart;
}

TPosition wxTextEditorModel::GetEndOfSelection()
{
	ArrangePositions( mSelectionStart, mSelectionEnd );

	return mSelectionEnd;
}

TPosition wxTextEditorModel::GetCursor()
{
	return mCursorPos;
}

void wxTextEditorModel::SetStartOfSelection( const TPosition& pos )
{
	mSelectionStart = pos;
}

void wxTextEditorModel::SetEndOfSelection( const TPosition& pos )
{
	mSelectionEnd = pos;
}

void wxTextEditorModel::SetCursor( const TPosition& pos )
{
	mCursorPos = pos;
}

void wxTextEditorModel::AddView( wxTextEditorView* pView )
{
	mViews.push_back( pView );
	pView->SetModel( this );
}

void wxTextEditorModel::RemoveView( wxTextEditorView* pView )
{
	for( size_t i = 0; i != mViews.size(); ++i )

		if ( mViews[i] == pView )
		{
			mViews.erase( & mViews[i] );
			return;
		}
}

void wxTextEditorModel::SetActiveView( wxTextEditorView* pView )
{
	mpActiveView = pView;
}

wxTextEditorView* wxTextEditorModel::GetActiveView()
{
	return mpActiveView;
}

void wxTextEditorModel::SetRowsPerPage( size_t n )
{
	mRowsPerPage = n;
}

void wxTextEditorModel::AddPin( TPinBase* pPin )
{
	// FIXME:: binary search should be used

	size_t beforePin = FindNextPinFrom( pPin->mRow );

	if ( beforePin != NPOS )
	{
		// pins in the same row are ordered in the
		// descending order of their type-codes

		while( beforePin < mPins.size() &&
			   mPins[beforePin]->mRow == pPin->mRow &&
			   mPins[beforePin]->mTypeCode < pPin->mTypeCode )

			   ++beforePin;

		if ( beforePin < mPins.size() )
			
			mPins.insert( &mPins[beforePin], pPin );
		else
			mPins.push_back( pPin );
	}
	else
		mPins.push_back( pPin );
}

PinListT& wxTextEditorModel::GetPins()
{
	return mPins;
}

size_t wxTextEditorModel::FindFirstPinInRange( size_t fromRow, size_t tillRow )
{
	// FIXME:: pefrom binary search instead

	for( size_t i = 0; i != mPins.size(); ++i )
	{
		TPinBase& pin = *mPins[i];

		if ( pin.mRow >= tillRow ) return NPOS;

		if ( pin.mRow >= fromRow )

			return i;
	}

	return NPOS;
}

size_t wxTextEditorModel::FindNextPinFrom( size_t fromRow )
{
	// FIXME:: pefrom binary search instead

	for( size_t i = 0; i != mPins.size(); ++i )
	{
		TPinBase& pin = *mPins[i];

		if ( pin.mRow >= fromRow )

			return i;
	}

	return NPOS;

}

size_t wxTextEditorModel::FindPreviousPinFrom( size_t fromRow )
{
	// FIXME:: pefrom binary search instead

	if ( mPins.size() == 0 ) return NPOS;

	size_t i = mPins.size() - 1;

	for(;;)
	{
		TPinBase& pin = *mPins[i];

		if ( pin.mRow <= fromRow )

			return i;

		if ( i == 0 ) break;

		--i;
	}

	return NPOS;
}

size_t wxTextEditorModel::GetPinNoAt( size_t row, int pinTypeCode )
{
	size_t curPin = FindNextPinFrom( row );

	while( curPin != NPOS )
	{
		TPinBase& pin = *mPins[curPin];

		if ( pin.mRow > row ) return NPOS;

		if ( pin.mTypeCode == pinTypeCode ) return curPin;

		++curPin;

		if ( curPin == mPins.size() ) return NPOS;
	}

	return NPOS;
}

TPinBase* wxTextEditorModel::GetPinAt( size_t row, int pinTypeCode )
{
	size_t pinNo = GetPinNoAt( row, pinTypeCode );

	return ( pinNo == NPOS ) ? NULL : mPins[pinNo];
}

void wxTextEditorModel::RemovePinAt( size_t row, int pinTypeCode )
{
	size_t pinNo = GetPinNoAt( row, pinTypeCode );

	if ( pinNo != NPOS )

		mPins.erase( &mPins[pinNo] );
}

void wxTextEditorModel::AddChangeListener( TTextChangeListenerBase* pListener )
{
	mChangeListeners.push_back( pListener );
}

/***** Implementation for class wxTextEditorView *****/

BEGIN_EVENT_TABLE( wxTextEditorView, wxScrolledWindow )

	EVT_SIZE  ( wxTextEditorView::OnSize   )
#if (( wxVERSION_NUMBER < 2100 ) || (( wxVERSION_NUMBER == 2100 ) && (wxBETA_NUMBER <= 4)))
	EVT_SCROLL( wxTextEditorView::OnScroll )
#else
	EVT_SCROLLWIN( wxTextEditorView::OnScroll )
#endif 
	EVT_PAINT ( wxTextEditorView::OnPaint  )

	EVT_LEFT_DOWN  ( wxTextEditorView::OnLButtonDown )
	EVT_LEFT_UP    ( wxTextEditorView::OnLButtonUp   )
	EVT_MOTION     ( wxTextEditorView::OnMotion      )
	EVT_LEFT_DCLICK( wxTextEditorView::OnDblClick    )

	EVT_SET_FOCUS  ( wxTextEditorView::OnSetFocus  )
	EVT_KILL_FOCUS ( wxTextEditorView::OnKillFocus )

	EVT_CHAR( wxTextEditorView::OnChar )
	EVT_KEY_DOWN( wxTextEditorView::OnKeyDown )

	EVT_ERASE_BACKGROUND( wxTextEditorView::OnEraseBackground )


END_EVENT_TABLE()

TCursorTimer* wxTextEditorView::mpTimer = new TCursorTimer();

wxTextEditorView::wxTextEditorView( wxWindow* parent, 
									wxWindowID id, 
									wxTextEditorModel* pModel,
									int wndStyle,
									bool ownsModel )

	: wxScrolledWindow( parent, id, wxPoint(32768,32768), wxSize(0,0),
						wxHSCROLL | wxVSCROLL | wndStyle
	                  ),
	  mPagePos( 0,0 ),
	  mDragStarted( FALSE ),
	  mpDraggedText( NULL ),
	  mAdjustScrollPending( FALSE ),
	  mLTMode( FALSE ),
	  mMaxColumns( 500 ),

	  mScrollingOn( TRUE ),
	  mCursorOn  ( TRUE ),
	  mOwnsModel ( ownsModel ),

	  mLastRowsTotal( (size_t)(-1) )
{
	SetModel( pModel );

	SetTextDefaults();

	SetSourcePainter( new SourcePainter() );

	mCashedIter.mPos = TPosition( (size_t)(-1), 0 );

	// default
	AddPinPainter( new TBookmarkPainter() );
}

wxTextEditorView::~wxTextEditorView()
{
	if ( mpTimer->GetView() == this && 
		 mCursorOn && !mLTMode )
	{
		mpTimer->SetView( NULL );
		mpTimer->HideCursor( TRUE );
	}

	if ( mOwnsModel && mpModel )

		delete mpModel;
}

void wxTextEditorView::SetTextDefaults()
{
	mLeftMargin   = 22;
	mRightMargin  = 0;
	mTopMargin    = 0;
	mBottomMargin = 0;

	mCharDim.x = -1; // not detected yet
	mCharDim.y = -1;

	mNormalTextCol       = *wxBLACK;
	mIndentifierTextCol  = *wxBLUE;
	mReservedWordTextCol = *wxRED;
	mCommentTextCol      = wxColour( 0,128,128 );

	mNormalBkCol    = wxColour(255,255,255);//*wxWHITE;//wxColour( 128,220,128 );
	mSelectionFgCol = wxColour(255,255,255);//*wxWHITE;
	mSelectionBkCol = wxColour( 0,0,128 );

	mNormalBkBrush   = wxBrush( mNormalBkCol,    wxSOLID );
	mSelectedBkBrush = wxBrush( mSelectionBkCol, wxSOLID );

#if defined(__WXMSW__) || defined(__WINDOWS__)
    mFont.SetFaceName("Fixedsys");
    mFont.SetStyle(40);
    mFont.SetWeight(40);
    mFont.SetPointSize( 11);
#else
    //mFont.SetFamily( wxSWISS );
	mFont = wxSystemSettings::GetSystemFont(wxSYS_OEM_FIXED_FONT);
#endif


#if defined(__WXMSW__) || defined(__WINDOWS__)
    mFont.RealizeResource();
#endif

	// reduce flicker un wxGtk
	SetBackgroundColour( mNormalBkCol );
}

void wxTextEditorView::SetColours( const wxColour& normalBkCol,
								   const wxColour& selectedBkCol,
								   const wxColour& selectedTextCol )
{
	mNormalBkCol    = normalBkCol;
	mSelectionFgCol = selectedTextCol;
	mSelectionBkCol = selectedBkCol;

	mNormalBkBrush   = wxBrush( mNormalBkCol,    wxSOLID );
	mSelectedBkBrush = wxBrush( mSelectionBkCol, wxSOLID );
}

void wxTextEditorView::SetHeighlightingColours( const wxColour& normalTextCol,
											    const wxColour& identifierTextCol,
											    const wxColour& reservedWordTextCol,
											    const wxColour& commentTextCol )
{
	mNormalTextCol       = normalTextCol;
	mIndentifierTextCol  = identifierTextCol;
	mReservedWordTextCol = reservedWordTextCol;
	mCommentTextCol      = commentTextCol;
}

void wxTextEditorView::SetMargins( int top, int left, int bottom, int right )
{
	mLeftMargin   = left;
	mRightMargin  = right;
	mTopMargin    = top;
	mBottomMargin = bottom;
}

void wxTextEditorView::RecalcPagingInfo()
{
	bool firstRefresh = mCharDim.x == -1;

	if ( firstRefresh )

		ObtainFontProperties();

	int w = 0, h = 0;
	GetClientSize( &w, &h );

	w -= mLeftMargin + mRightMargin;
	h -= mTopMargin  + mBottomMargin;

	mColsPerPage = ( ( w / mCharDim.x ) +
				   ( ( w % mCharDim.x ) ? 0 : 0 ) );


	mRowsPerPage = ( ( h / mCharDim.y ) +
				   ( ( h % mCharDim.y ) ? 0 : 0 ) );

	if ( mpModel->GetActiveView() == this )

		mpModel->SetRowsPerPage( mRowsPerPage );

	if ( firstRefresh ) 
	{
		// scrolling should not happen at DC-level
		EnableScrolling( FALSE, FALSE );

		if ( mScrollingOn )

			SetScrollbars( mCharDim.x, mCharDim.y,
						   mMaxColumns,
						   mpModel->GetTotalRowCount(),
						   mPagePos.mCol,
						   mPagePos.mRow,
						   TRUE
						 );
	}

	PositionCursor();
}

#if (( wxVERSION_NUMBER < 2100 ) || (( wxVERSION_NUMBER == 2100 ) && (wxBETA_NUMBER <= 4)))
 // this changed in ver 2.1
void wxTextEditorView::OnScroll( wxScrollEvent& event )
#else
void wxTextEditorView::OnScroll( wxScrollWinEvent& event )
#endif
{
	if ( !mScrollingOn ) return;

	// overriden implementation of wxScrolledWindow::OnScroll,
	// to reduce flicker on wxGtk, by using wxClientDC 
	// instead of Refresh()

	int orient = event.GetOrientation();

	int nScrollInc = CalcScrollInc(event);
	if (nScrollInc == 0) return;

	if (orient == wxHORIZONTAL)
	{
		int newPos = m_xScrollPosition + nScrollInc;
		SetScrollPos(wxHORIZONTAL, newPos, TRUE );
	}
	else
	{
		int newPos = m_yScrollPosition + nScrollInc;
		SetScrollPos(wxVERTICAL, newPos, TRUE );
	}

	if (orient == wxHORIZONTAL)
	{
		m_xScrollPosition += nScrollInc;
	}
	else
	{
		m_yScrollPosition += nScrollInc;
	}

	int x,y;
	ViewStart( &x, &y );

	mPagePos.mRow = y;
	mPagePos.mCol = x;

	PositionCursor();

	if ( mAdjustScrollPending )
	{
		mLastRowsTotal = mpModel->GetTotalRowCount();
		SetScrollbars( mCharDim.x, mCharDim.y,
					   mMaxColumns,   // FOR NOW:: maximal line-length not calculated
					   mLastRowsTotal,
					   mPagePos.mCol,
					   mPagePos.mRow,
					   TRUE
					 );

		mLastViewStart = mPagePos;

		mAdjustScrollPending = FALSE;

		return;
	}

	wxClientDC dc( this );

	mFullRefreshPending = TRUE;

	PaintRows( mPagePos.mRow, mPagePos.mRow + mRowsPerPage, dc );
}

void wxTextEditorView::OnPaint( wxPaintEvent& event )
{
	//wxScrolledWindow::OnPaint( event );
	if ( mCharDim.x == -1 ) ObtainFontProperties();

	wxPaintDC dc( this );

	mFullRefreshPending = TRUE;

	PaintRows( mPagePos.mRow, mPagePos.mRow + mRowsPerPage, dc );
}

void wxTextEditorView::OnSize( wxSizeEvent& event )
{
	RecalcPagingInfo();

	SyncScrollbars();

	event.Skip();
}

void wxTextEditorView::OnEraseBackground( wxEraseEvent& event )
{
#if 0
	int w = 0, h = 0;

	GetClientSize( &w, &h );
	
	wxPaintDC dc( this );

	dc.SetPen( *wxTRANSPARENT_PEN );
	dc.SetBrush( *wxWHITE_BRUSH );
	dc.DrawRectangle( 0,0, w,h );
#endif
}

void wxTextEditorView::OnLButtonDown( wxMouseEvent& event )
{
	if ( mDragStarted ) return;

	mDragStarted = TRUE;

	TPosition textPos;
	PixelsToTextPos( event.m_x, event.m_y, textPos );

	mpModel->SetSelectionEditMode( FALSE );

	mpModel->OnMoveToPosition( textPos );

	mpModel->SetSelectionEditMode( TRUE );

	SetFocus();

	CaptureMouse();
}

void wxTextEditorView::OnLButtonUp( wxMouseEvent& event )
{
	if ( mDragStarted )
	{
		OnMotion( event ); // simulate last motion event

		mpModel->SetSelectionEditMode( FALSE );

		ReleaseMouse();
		mDragStarted = FALSE;
	}
}

void wxTextEditorView::OnMotion( wxMouseEvent& event )
{
	if ( mDragStarted )
	{
		TPosition textPos;

		if ( event.m_y < 0 && mpModel->GetCursor().mRow == 0 )
		
			event.m_y = 0;

		PixelsToTextPos( event.m_x, event.m_y, textPos );

		mpModel->OnMoveToPosition( textPos );
	}
}

void wxTextEditorView::OnDblClick( wxMouseEvent& event )
{
	event.Skip();
	mpModel->OnSelectWord();
}

void wxTextEditorView::OnSetFocus( wxFocusEvent& event )
{
	if ( !mLTMode && mCursorOn )
	{
		mpTimer->SetView( this );
		mpTimer->ShowCursor( TRUE );
	}
}

void wxTextEditorView::OnKillFocus( wxFocusEvent& event )
{
	if ( !mLTMode && mCursorOn )
	{
		mpTimer->HideCursor( TRUE );
		mpTimer->SetView( NULL );
	}
}

void wxTextEditorView::HoldCursor( bool hold )
{
	if ( mLTMode || !mCursorOn ) return;

	if ( !hold )
	{
		if ( wxWindow::FindFocus() != this )
		{	
			mpTimer->HideCursor();
			mpTimer->SetView( NULL );
		}
	}
	else
	{
		mpTimer->SetView( this );
		mpTimer->ShowCursor();
	}
}

void wxTextEditorView::OnKeyDown( wxKeyEvent& event )
{
	// FOR NOW:: hard-coded key-bindings

	mpModel->SetSelectionEditMode( event.ShiftDown() );

	if ( event.ControlDown() )
	{
		if ( event.m_keyCode == WXK_LEFT ) 
		
			mpModel->OnWordLeft();
		else
		if ( event.m_keyCode == WXK_RIGHT ) 
		
			mpModel->OnWordRight();
		else
		if ( event.m_keyCode == WXK_UP ) 
		
			mpModel->OnSlideUp();
		else
		if ( event.m_keyCode == WXK_DOWN ) 
		
			mpModel->OnSlideDown();
		else
		if ( event.m_keyCode == WXK_HOME ) 
		
			mpModel->OnStartOfText();
		else
		if ( event.m_keyCode == WXK_END ) 
		
			mpModel->OnEndOfText();
		else
		if ( event.m_keyCode == WXK_INSERT )

			mpModel->OnCopy();
		else
			event.Skip();

		/*
		else
		if ( event.m_keyCode == WXK_NEXT ) 
		
			mpModel->();
		else
		if ( event.m_keyCode == WXK_PRIOR ) 
		
			mpModel->();
		*/
	}
	else
	{
		if ( event.m_keyCode == WXK_LEFT ) 
		
			mpModel->OnMoveLeft();
		else
		if ( event.m_keyCode == WXK_RIGHT ) 
		
			mpModel->OnMoveRight();
		else
		if ( event.m_keyCode == WXK_UP ) 
		
			mpModel->OnMoveUp();
		else
		if ( event.m_keyCode == WXK_DOWN ) 
		
			mpModel->OnMoveDown();
		else
		if ( event.m_keyCode == WXK_HOME ) 
		
			mpModel->OnStartOfLine();
		else
		if ( event.m_keyCode == WXK_END ) 
		
			mpModel->OnEndOfLine();
		else
		if ( event.m_keyCode == WXK_NEXT ) 
		
			mpModel->OnPageDown();
		else
		if ( event.m_keyCode == WXK_PRIOR ) 
		
			mpModel->OnPageUp();
		else
		if ( event.m_keyCode == WXK_DELETE )

			mpModel->OnDelete();
		else
		if ( event.m_keyCode ==  WXK_INSERT && event.ShiftDown() )
		
			mpModel->OnPaste();
		else
			event.Skip();
	}	
}

void wxTextEditorView::OnChar( wxKeyEvent& event )
{
	if ( event.ControlDown() )
	{
		if ( event.m_keyCode == 'y' )

			mpModel->OnDeleteLine();
		else
		if ( event.m_keyCode == 'v' )

			mpModel->OnPaste();
		else
		if ( event.m_keyCode == 'c' )

			mpModel->OnCopy();
		else
		if ( event.m_keyCode == 'z' )

			mpModel->OnUndo();
		else
		if ( event.m_keyCode == 'a' )

			mpModel->OnRedo();
		else
			event.Skip();
	}
	else
	if ( event.AltDown() )
	{
		if ( event.m_keyCode == WXK_BACK ) 

			mpModel->OnUndo();
		else
			event.Skip();
	}
	else
	if ( event.m_keyCode ==  WXK_BACK )
	
		mpModel->OnDeleteBack();
	else
	if ( event.m_keyCode == WXK_TAB && event.ShiftDown() )

		mpModel->OnShiftSelectionIndent( TRUE );
	else
	{
		if ( !mpModel->SelectionIsEmpty() && event.m_keyCode == WXK_TAB )

			mpModel->OnShiftSelectionIndent( FALSE );
		else
			mpModel->OnInsertChar( event.m_keyCode );
	}
}

void wxTextEditorView::SetModel( wxTextEditorModel* pModel )
{
	mpModel = pModel;
	mSelectionStart = pModel->GetStartOfSelection();
	mSelectionEnd   = pModel->GetEndOfSelection();
	mCursorPos      = pModel->GetCursor();
}

void wxTextEditorView::SetSourcePainter( SourcePainter* pPainter )
{
	mpPainter = pPainter;
}

void wxTextEditorView::AddPinPainter( TPinPainterBase* pPainter )
{
	mPinPainters.push_back( pPainter );
}

void wxTextEditorView::SetDefaultFont( const wxFont& font )
{
	mFont = font;

#if defined(__WXMSW__) || defined(__WINDOWS__)
    mFont.RealizeResource();
#endif

	mCharDim.x = -1;
	mCharDim.y = -1;

	RecalcPagingInfo();
}

void wxTextEditorView::SetRowsPerPage( size_t n )
{
	mpModel->SetRowsPerPage( n );

	mRowsPerPage = n;
	SyncScrollbars();
	PositionCursor();
}

void wxTextEditorView::SetMaxColumns( size_t n )
{
	mMaxColumns = n;

	SyncScrollbars();
	PositionCursor();
}

wxFont& wxTextEditorView::GetDefaultFont()
{
	return mFont;
}

void wxTextEditorView::SetLineTrackingMode( bool on, const wxColour& col )
{
	mLTColour = col;
	mLTMode   = on;

	if ( mpTimer->GetView() == this )

		mpTimer->HideCursor();
}

void wxTextEditorView::EnableCursor( bool enable )
{
	mCursorOn = enable;
}

void wxTextEditorView::EnableScrollbars( bool enable )
{
	mScrollingOn = enable;
}

bool wxTextEditorView::IsActiveView()
{
	return this == mpModel->GetActiveView();
}

void wxTextEditorView::PositionCursor()
{
	if ( !IsActiveView() || 
		 mLTMode || !mCursorOn ) return;

	mpTimer->HideCursor();

	TextPosToScreenPos( mpModel->GetCursor(), mCursorScrPos );

	mpTimer->ShowCursor();
}

void wxTextEditorView::PixelsToScrPos( int x, int y, int& scrRow, int& scrCol )
{
	x -= mLeftMargin;
	y -= mTopMargin;

	//if ( x < 0 ) x = 0; // FOR NOW:: horizontal auto-scroll disabled

	scrCol = x / mCharDim.x;
	scrRow = y / mCharDim.y;
}

void wxTextEditorView::PixelsToTextPos( int x, int y, TPosition& textPos )
{
	int scrRow = 0, scrCol = 0;
	PixelsToScrPos( x, y, scrRow, scrCol );

	if ( scrRow + (int)mPagePos.mRow < 0 )

		scrRow = -(int)mPagePos.mRow;

	if ( scrCol + (int)mPagePos.mCol < 0 )

		scrCol = -(int)mPagePos.mCol;

	ScreenPosToTextPos( TPosition( scrRow, scrCol ), textPos );
}

void wxTextEditorView::ScreenPosToPixels( const TPosition& scrPos, int& x, int& y )
{
	x = mLeftMargin + scrPos.mCol * mCharDim.x;
	y = mTopMargin  + scrPos.mRow * mCharDim.y;
}

void wxTextEditorView::TextPosToScreenPos( const TPosition& txtPos, TPosition& scrPos )
{
	TTextIterator iter;

	if ( txtPos.mRow != mCashedIter.mPos.mRow )
	{
		iter = mpModel->CreateIterator( txtPos );
		mCashedIter = iter;
	}
	else
	{
		iter = mCashedIter;
		iter.mPos.mCol = txtPos.mCol;
	}

	iter.ToStartOfLine();

	size_t scrCol = 0;
	size_t txtCol = 0;

	while( !iter.IsEol() && txtCol < txtPos.mCol )
	{
		if ( iter.GetChar() == '\t' )
		{
			size_t spacing = ( (scrCol / mpModel->mTabSize) + 1 ) * mpModel->mTabSize - scrCol;

			scrCol += spacing;
		}
		else
			++scrCol;

		++txtCol;
		iter.NextChar();
	}

	TPosition actualPos = iter.GetPosition();

	scrCol += txtPos.mCol - txtCol;

	scrPos.mRow = actualPos.mRow - mPagePos.mRow;
	scrPos.mCol = scrCol - mPagePos.mCol;
}

void wxTextEditorView::ScreenPosToTextPos( const TPosition& scrPos, TPosition& txtPos )
{
	TPosition absScrPos( scrPos.mRow + mPagePos.mRow, scrPos.mCol + mPagePos.mCol );

	TTextIterator iter = mpModel->CreateIterator( TPosition( absScrPos.mRow, 0 ) );

	size_t scrCol = 0;
	size_t txtCol = 0;

	// iterate over all possible on-screen positions, and find one which matches "absScrPos"

	while( !iter.IsEol() && scrCol < absScrPos.mCol )
	{
		if ( iter.GetChar() == '\t' )
		{
			size_t spacing = ( (scrCol / mpModel->mTabSize) + 1 ) * mpModel->mTabSize - scrCol;

			scrCol += spacing;
		}
		else
			++scrCol;

		++txtCol;
		iter.NextChar();
	}

	TPosition actualPos = iter.GetPosition();

	if ( scrCol == absScrPos.mCol )
	{
		txtPos = actualPos;
		return;
	}
	else
	if ( scrCol < absScrPos.mCol )
	{
		// the absScrPos points past the eol

		txtPos = actualPos;
		txtPos.mCol += absScrPos.mCol - scrCol;
	}
	else
	if ( scrCol > absScrPos.mCol )
	{
		// there should have been a '\t' char, which made us jump too far forward

		txtPos = actualPos;
		--txtPos.mCol;
	}
}

bool wxTextEditorView::IsClipboardCmd( wxKeyEvent& key )
{
	if ( key.ControlDown() && key.m_keyCode == WXK_CONTROL )

		return TRUE;

	if ( key.ShiftDown() && key.m_keyCode == WXK_SHIFT )

		return TRUE;

	if ( key.ControlDown() )
	{
		return ( key.m_keyCode == 'C' ||
				 key.m_keyCode == 'c' ||
				 key.m_keyCode == WXK_INSERT );
	}

	return FALSE;
}

void wxTextEditorView::ObtainFontProperties()
{
	wxClientDC dc(this);
	dc.SetFont( mFont );

	long w,h;

	dc.GetTextExtent( "X", &w, &h );

	mCharDim.x = w;
	mCharDim.y = h;
}

void wxTextEditorView::SyncViewPortPosition()
{

	TPosition pos = mpModel->GetCursor();

	TextPosToScreenPos( pos, pos );
	pos.mRow += mPagePos.mRow;
	pos.mCol += mPagePos.mCol;

	if ( pos.mRow < mPagePos.mRow )
	{
		mPagePos.mRow = pos.mRow;
		mFullRefreshPending = TRUE;
	}
	else
	if ( pos.mRow >= mPagePos.mRow + mRowsPerPage && mRowsPerPage != 0 )
	{
		mPagePos.mRow = pos.mRow - mRowsPerPage + 1;
		mFullRefreshPending = TRUE;
	}

	if ( pos.mCol < mPagePos.mCol )
	{
		mPagePos.mCol = pos.mCol;
		mFullRefreshPending = TRUE;
	}
	else
	if ( pos.mCol >= mPagePos.mCol + mColsPerPage )
	{
		mPagePos.mCol = pos.mCol - mColsPerPage + 1;
		mFullRefreshPending = TRUE;
	}
}

void wxTextEditorView::SyncScrollbars()
{
	if ( !mScrollingOn ) return;

	size_t nRows = mpModel->GetTotalRowCount();

#if !defined(__WINDOWS__)

	if ( mLastViewStart == mPagePos )
	{
		if ( mLastRowsTotal != nRows )

			mAdjustScrollPending = TRUE;

		 return;
	}
#else
	if ( mLastViewStart == mPagePos &&
		 mLastRowsTotal == nRows )

		 return;
#endif
	SetScrollbars( mCharDim.x, mCharDim.y,
				   mMaxColumns,
				   nRows,
				   mPagePos.mCol,
				   mPagePos.mRow,
				   TRUE
				 );

	mLastViewStart = mPagePos;
	mLastRowsTotal = nRows;
}

void wxTextEditorView::ScrollView( int rows, int cols )
{
	int pageRow = (int)mPagePos.mRow; 
	int pageCol = (int)mPagePos.mCol;

	if ( pageRow + rows < 0 ) 
		pageRow = 0;
	else
	if ( pageRow + rows > (int)mpModel->GetTotalRowCount() )

		pageRow = mpModel->GetTotalRowCount();
	else
		pageRow = pageRow + rows;

	mPagePos.mRow = (size_t)pageRow;

	if ( pageCol + cols < 0 )

		pageCol = 0;
	else
		pageCol = pageCol + cols;

	mPagePos.mCol = pageCol;

	mFullRefreshPending = TRUE;
}

void wxTextEditorView::OnModelChanged()
{
	// invalidate pre-cached iterator
	mCashedIter.mPos = TPosition( (size_t)(-1), 0 );

	SyncViewPortPosition();

	if ( mLTMode ) mFullRefreshPending = TRUE;

	if ( mpModel->mTextChanged && !mFullRefreshPending )
	{
		wxClientDC dc( this );
		PaintRows( mpModel->mChangedFromRow, mpModel->mChangedTillRow, dc );
	}
	else
	if ( mFullRefreshPending )
	{
		wxClientDC dc( this );
		PaintRows( mPagePos.mRow, mPagePos.mRow + mRowsPerPage, dc );
	}

	if ( IsActiveView() )
	{	
		PositionCursor();
		SyncScrollbars();
	}
}

void wxTextEditorView::Activate()
{
	mpModel->SetStartOfSelection( mSelectionStart );
	mpModel->SetEndOfSelection( mSelectionEnd );
	mpModel->SetCursor( mCursorPos );

	mpModel->SetRowsPerPage( mRowsPerPage );

	if ( !mLTMode && mCursorOn )
	{
		mpTimer->SetView( this );
		mpTimer->ShowCursor();
	}

	mpModel->SetActiveView( this );
}

void wxTextEditorView::Deactivate()
{
	mSelectionStart = mpModel->GetStartOfSelection();
	mSelectionEnd   = mpModel->GetEndOfSelection();
	mCursorPos      = mpModel->GetCursor();

	if ( mpTimer->GetView() == this && 
		 !mLTMode && mCursorOn )
		
		mpTimer->HideCursor( TRUE );
}

/*** protected methods ***/

char*  wxTextEditorView::mpLineBuffer    = NULL;
size_t wxTextEditorView::mpLineBufferLen = 0;

char* wxTextEditorView::GetLineBuffer( size_t len )
{
	if ( mpLineBuffer == NULL || mpLineBufferLen < len )
	{
		if ( !mpLineBuffer ) mpModel->FreeCharacters( mpLineBuffer );

		mpLineBuffer = mpModel->AllocCharacters( len );

		mpLineBufferLen = len;
	}

	return mpLineBuffer;
}

TPinPainterBase* wxTextEditorView::FindPainterForPin( TPinBase& pin )
{
	int pinTc = pin.mTypeCode;

	for( size_t i = 0; i != mPinPainters.size(); ++i )

		if ( mPinPainters[i]->mPinTypeCode == pinTc )

			return mPinPainters[i];

	return NULL;
}

void wxTextEditorView::PaintDecorations( size_t fromRow, 
										 size_t tillRow, 
										 wxDC& dc, TTextIterator& iter )
{
	int dcY = ( fromRow - mPagePos.mRow ) * mCharDim.y + mTopMargin;
		
	size_t curPin = mpModel->FindFirstPinInRange( fromRow, tillRow );

	PinListT& pins = mpModel->GetPins();
	TPinPainterBase* pPainter = NULL;

	size_t prevRow = fromRow;
	int    prevY   = dcY;

	wxPoint pos;
	wxSize  dim( mLeftMargin, mCharDim.y );

	while( curPin != NPOS )
	{
		TPinBase& pin = *pins[curPin];

		if ( pPainter == NULL ||
			 pPainter->mPinTypeCode != pin.mTypeCode )

			 pPainter = FindPainterForPin( pin );
	

		// only pins which have their painters can be "visualized"

		if ( pPainter )
		{
			pos.x = 0;
			pos.y = ( pin.mRow - mPagePos.mRow )* mCharDim.y + mTopMargin;

			if ( prevRow < pin.mRow )
			{
				// fill upper gap

				dc.SetBrush( mNormalBkBrush );
				dc.SetPen( *wxTRANSPARENT_PEN );
				dc.DrawRectangle( 0, prevY, 
								  mLeftMargin + 1, 
								  mCharDim.y * ( pin.mRow - prevRow ) + 1 );
			}

			pPainter->DrawPin( &pin, *this, dc, pos, dim );

			prevRow = pin.mRow + 1;
			prevY = pos.y + mCharDim.y;
		}

		++curPin;

		if ( curPin >= pins.size() ||
			 pins[curPin]->mRow >= tillRow )

			 break;
	}

	// fill the reminder 

	if ( prevRow < tillRow )
	{
		dc.SetBrush( mNormalBkBrush );
		dc.SetPen( *wxTRANSPARENT_PEN );
		dc.DrawRectangle( 0, prevY, 
						  mLeftMargin + 1, 
						  mCharDim.y * ( tillRow - prevRow ) + 1 );
	}

	dc.SetPen( *wxTRANSPARENT_PEN );
}

void wxTextEditorView::PaintRows( size_t fromRow, size_t tillRow, wxDC& dc )
{
	// NOTE:: raws are painted from "fromRow" but not including "tillRow" - [fromRow,tillRow)

	dc.SetPen( *wxTRANSPARENT_PEN );

	// how much on-screen columns are visable?

	size_t fromScrCol = mPagePos.mCol;
	size_t tillScrCol = fromScrCol + mColsPerPage;

	TPosition selStart = mpModel->GetStartOfSelection();
	TPosition selEnd   = mpModel->GetEndOfSelection();

	bool selectionIsEmpty = ( selStart == selEnd );

	wxColour curFgCol;
	wxColour curBkCol;

	wxBrush mLTBrush( mLTColour, wxSOLID );

	// clip given row-region to the current page

	if ( ( fromRow >= mPagePos.mRow + mRowsPerPage) || 
		 ( tillRow <= mPagePos.mRow )
	   )

		return;

	if ( fromRow < mPagePos.mRow ) fromRow = mPagePos.mRow;
	if ( tillRow > mPagePos.mRow + mRowsPerPage ) tillRow = mPagePos.mRow + mRowsPerPage;

	if ( fromRow >= tillRow ) return;

	// now start the renderng

	if ( mpTimer->GetView() == this && mCursorOn && !mLTMode ) 
	{
		mpTimer->Lock();
		mpTimer->SetIsShown( FALSE );
	}

	dc.SetFont( mFont );
	dc.SetBackgroundMode( wxSOLID );

	TTextIterator iter = mpModel->CreateIterator( TPosition( fromRow, 0 ) );

	PaintDecorations( fromRow, tillRow, dc, iter );

	size_t cursorRow = mpModel->GetCursor().mRow;

	size_t curRow = fromRow;
	for( ; curRow != tillRow; ++curRow )
	{
		// place text into line-buffer

		iter.ToStartOfLine();
		size_t lineLen = iter.GetLineLen();

		char* lineBuf = GetLineBuffer( lineLen + 1 );

		size_t i = 0;

  		while( !iter.IsEof() && !iter.IsEol() )
  		{
  			lineBuf[i++] = iter.GetChar();
  			iter.NextChar();
		}

		iter.NextChar(); // skip eol

		// obtain "highlights"
  		
		mpPainter->SetState( FALSE, FALSE );
		mpPainter->Init( FALSE );
		mpPainter->ProcessSource( lineBuf, lineLen );
		IntListT& blocks = mpPainter->GetBlocks();

		// setup state vars

		int dcY = ( curRow - mPagePos.mRow ) * mCharDim.y + mTopMargin;
		
		size_t scrCol = 0;
		size_t txtCol = 0;

		size_t curBlk = 0;
		size_t curBlkCol = 0;

		int    chunkLen      = -1;
		size_t chunkTxtStart = 0;
		size_t chunkScrStart = 0;

		// pre-detect occurance of selection

		bool lineHasSelection = ( selStart.mRow == curRow ) ||
			                    ( selEnd.mRow == curRow   );

		bool isInSelection    = ( selStart.mRow <= curRow ) &&
			                    ( selEnd.mRow >= curRow   );

		if ( isInSelection && selStart.mRow == curRow &&
			 selStart.mCol != 0 )

			 isInSelection = FALSE;

		if ( selStart == selEnd ) 
		{
			lineHasSelection = FALSE;
			isInSelection = FALSE;
		}
		
		char ch = '\0';

		// loop though the text in this row

		do
		{
			TPosition curPos( curRow, txtCol );

			// first check if we can finish the current chunk

			bool finishChunk = FALSE;

			if ( curBlk < blocks.size() &&
				 curBlkCol + get_src_block_len( blocks[curBlk] ) == txtCol )
			{
				curBlkCol += get_src_block_len( blocks[curBlk] );
				++curBlk;
				finishChunk = TRUE;
			}
			else
			if ( ( !selectionIsEmpty && ( curPos == selStart || curPos == selEnd ) )
				 || lineBuf[txtCol] == '\t' 
				 || txtCol == lineLen )

				 finishChunk = TRUE;
			
			if ( finishChunk && chunkLen != -1 )
			{
				// is any part of the chunk visable?

				size_t chunkScrEnd = chunkScrStart + chunkLen;

				if ( ( // if hits from one side or is inside
					   ( chunkScrStart >= fromScrCol &&
					     chunkScrStart <  tillScrCol    ) ||
					   ( chunkScrEnd   >= fromScrCol &&
					     chunkScrEnd   <  tillScrCol    ) ) ||

					   // if overlaps the whole range
					 (	chunkScrStart <   fromScrCol &&
					    chunkScrEnd   >=  tillScrCol      )

				   )
				{
					// render chunk data to the given DC

					dc.SetTextForeground( curFgCol );
					dc.SetTextBackground( curBkCol );

					// clip left edge

					if ( chunkScrStart < fromScrCol )
					{
						size_t diff = fromScrCol - chunkScrStart;
						chunkLen      -= diff;
						chunkTxtStart += diff;
						chunkScrStart += diff;
					}

					// clip right edge

					if ( chunkScrEnd > tillScrCol )
					{
						size_t diff  = chunkScrEnd - tillScrCol;
						chunkLen    -= diff;
						chunkScrEnd -= diff;
					}

					// create string

					char tmp = lineBuf[chunkTxtStart + chunkLen];
					
					lineBuf[chunkTxtStart + chunkLen] = '\0';

					// use member-variable, reuse heap-buffer between outputs
					mFragment = lineBuf + chunkTxtStart;

					lineBuf[chunkTxtStart + chunkLen] = tmp;

					// draw it

					int dcX = (chunkScrStart - fromScrCol) * mCharDim.x + mLeftMargin;

					dc.DrawText( mFragment, dcX, dcY );
				}

				chunkLen = -1;

			} // end of "if ( finishChunk )"

			if ( txtCol == lineLen ) 
				break;

			if ( chunkLen == -1 )
			{
				// prepare the new chunk

				if ( curBlk < blocks.size() )
				{
						switch( get_src_block_rank( blocks[curBlk] ) )
						{
							case RANK_BLACK : curFgCol = mNormalTextCol; break;
							case RANK_BLUE  : curFgCol = mIndentifierTextCol; break;
							case RANK_RED   : curFgCol = mReservedWordTextCol; break;
							case RANK_GREEN : curFgCol = mCommentTextCol; break;
							default : break;
						}
				}

				// track occurence of selection

				if ( lineHasSelection )
				{
					isInSelection = TRUE;

					if ( selEnd.mRow == curRow &&
						 selEnd.mCol <= txtCol )

						 isInSelection = FALSE;

					if ( selStart.mRow == curRow &&
						 selStart.mCol > txtCol )

						 isInSelection = FALSE;
				}

				if ( isInSelection )
				{
					curFgCol = mSelectionFgCol;
					curBkCol = mSelectionBkCol;
				}
				else
				{
					if ( mLTMode && curRow == cursorRow ) 

						curBkCol = mLTColour;
					else
						curBkCol = mNormalBkCol ;
				}

				chunkScrStart = scrCol;
				chunkTxtStart = txtCol;
				chunkLen      = 0;
			}

			
			ch = lineBuf[txtCol];

			if ( ch == '\t' )
			{
				// tab's are treated specially (for simplicity and speed)

				int dcX = (chunkScrStart - fromScrCol) * mCharDim.x + mLeftMargin;

				if ( !isInSelection ) 
				{
					if ( mLTMode && curRow == cursorRow ) 
						
						dc.SetBrush( mLTBrush );
					else 
						dc.SetBrush( mNormalBkBrush );
				}
				else dc.SetBrush( mSelectedBkBrush );

				// *** "the rule of TAB..." ***

				size_t spacing = ( (scrCol / mpModel->mTabSize) + 1 ) * mpModel->mTabSize - scrCol;

				int width = spacing * mCharDim.x + 1;

				if ( dcX < mLeftMargin )
				{
					width -= mLeftMargin - dcX;

					dcX = mLeftMargin;
				}

				if ( width > 0 )

					dc.DrawRectangle( dcX, dcY, width, mCharDim.y + 1 );

				scrCol += spacing;
				txtCol += 1;
				
				// move chunk-start forward, after the occurance of '\t'

				chunkLen = -1;
			}
			else
			{
				// increase on-screen/in-text positions

				++scrCol;
				++txtCol;
				++chunkLen;
			}
		
		} while( TRUE );

		// fill the reminding white-space after eol

		if ( scrCol < tillScrCol && 
			 ( !isInSelection ||
			   ( isInSelection && curRow == selEnd.mRow ) )
		   )
		{
			if ( scrCol < fromScrCol ) scrCol = fromScrCol;

			int dcX = ( scrCol - fromScrCol ) * mCharDim.x + mLeftMargin;

			if ( mLTMode && curRow == cursorRow )

				dc.SetBrush ( mLTBrush );
			else
				dc.SetBrush( mNormalBkBrush );

			dc.DrawRectangle( dcX, dcY, 
							  mCharDim.x * ( tillScrCol - scrCol ) + 1, 
							  mCharDim.y + 1 );
		}

		// render selection which is located past the eol

		if ( ( lineHasSelection || isInSelection ) && 
			 !( selEnd.mRow == curRow && selEnd.mCol <= txtCol )
		   )
		{
			// determine start of selection on-screen

			size_t scrSelStart = scrCol + ( selStart.mCol - txtCol );

			if ( isInSelection )

				scrSelStart = scrCol;

			size_t scrSelEnd = tillScrCol;

			if ( selEnd.mRow == curRow )

				scrSelEnd = scrCol + ( selEnd.mCol - txtCol );

			// clipping 

			if ( scrSelStart < fromScrCol ) scrSelStart = fromScrCol;
			if ( scrSelEnd   > tillScrCol ) scrSelEnd = tillScrCol;

			// drawing 

			if ( scrSelEnd > scrSelStart )
			{
				int dcX = ( scrSelStart - fromScrCol ) * mCharDim.x + mLeftMargin;

				dc.SetBrush( mSelectedBkBrush );
				dc.DrawRectangle( dcX, dcY, 
								  mCharDim.x * ( scrSelEnd - scrSelStart ) + 1, 
								  mCharDim.y + 1 );
			}
		}

		if ( iter.IsEof() ) 
		{
			++curRow;
			break;
		}

	} // end of "for(...)"

	if ( curRow < tillRow )
	{
		dc.SetBrush( mNormalBkBrush );

		int dcY = mTopMargin + (curRow - mPagePos.mRow)*mCharDim.y;
		int dcX = mLeftMargin;

		dc.DrawRectangle( dcX, dcY, mColsPerPage*mCharDim.x + 1, 
						  ( tillRow - curRow ) * mCharDim.y + 1
				        );
	}

	if ( mFullRefreshPending )
	{
		dc.SetBrush( mNormalBkBrush );

		// fill in "corners" which are never reached by characters
		
		int w,h;
		GetClientSize( &w, &h );

		dc.SetBrush( mNormalBkBrush );

		int dcX = tillScrCol*mCharDim.x + mLeftMargin;

		dc.DrawRectangle( dcX, mTopMargin, w - dcX + 1, h );

		int dcY = mTopMargin + mRowsPerPage*mCharDim.y;

		dc.DrawRectangle( 0, dcY, w, h - dcY + 2 );

		++curRow;

		// any past-the-eof lines left at the bottom?
	}

	mFullRefreshPending = FALSE;

	if ( mpTimer->GetView() == this && mCursorOn && !mLTMode ) 

		mpTimer->Unlock();

} // end of PaintRows(..)

/***** Implementation for class TBookmarkPainter *****/

TBookmarkPainter::TBookmarkPainter() 

	: TPinPainterBase( BOOKMARK_PIN_TC ),
	  mBkBrush( wxColour( 0,255,255 ), wxSOLID )
{
}

void TBookmarkPainter::DrawPin( TPinBase* pPin, wxTextEditorView& view, wxDC& dc, 
							    const wxPoint& pos, const wxSize& dim )
{
	dc.SetPen( *wxBLACK_PEN );
	dc.SetBrush( mBkBrush );
	dc.DrawRoundedRectangle( pos.x+2, pos.y, dim.x-4, dim.y, 4 );
}

/***** Implementation for class TBreakpointPainter *****/

TBreakpointPainter::TBreakpointPainter() 

	: TPinPainterBase( BRKPOINT_PIN_TC ),
	  mBkBrush( wxColour( 196,0,0 ), wxSOLID )
{
}

void TBreakpointPainter::DrawPin( TPinBase* pPin, wxTextEditorView& view, wxDC& dc, 
								  const wxPoint& pos, const wxSize& dim )
{
	dc.SetPen( *wxBLACK_PEN );
	dc.SetBrush( mBkBrush );
	dc.DrawRoundedRectangle( pos.x+6, pos.y+2, dim.x-12, dim.y-4, 30 );
}

/***** Implementation for class TCursorTimer *****/

TCursorTimer::TCursorTimer()

	: mIsLocked( FALSE ),
	  mIsShown ( FALSE ),
	  mBlinkInterval( 500 ),
	  mBrush( wxColour(0,0,0), wxSOLID ),
	  mMissOneTick( FALSE )
{
}

void TCursorTimer::Notify()
{
	if ( mIsLocked ) return;

	if ( mMissOneTick )
	{
		// this trick is used because it's not
		// possible to restart the timer under wxGtk

		mMissOneTick = FALSE;
		return;
	}


	mIsLocked = TRUE;

	DrawCursor();

	mIsShown = !mIsShown;

	mIsLocked = FALSE;
}

void TCursorTimer::SetView( wxTextEditorView* pView )
{
	mpView = pView;
}

wxTextEditorView* TCursorTimer::GetView()
{
	return mpView;

}

void TCursorTimer::HideCursor( bool forceHide )
{
	Lock();

	if ( mIsShown  ) 
	{
		DrawCursor();
		mIsShown = FALSE;
	}

	Unlock();
}

void TCursorTimer::ShowCursor( bool forceShow )
{
	Lock();

	if ( !forceShow ) 
	{
		DrawCursor();
		mIsShown = TRUE;

		if ( mStarted )
			mMissOneTick = TRUE;
	}

	Unlock();

	if ( !mStarted )
	{
		Start( mBlinkInterval );
		mStarted = TRUE;
	}
}

void TCursorTimer::Lock()
{
//	while( mIsLocked );

	mIsLocked = TRUE;
}

void TCursorTimer::Unlock()
{
	mIsLocked = FALSE;
}

void TCursorTimer::SetIsShown( bool isShown )
{
	mIsShown = isShown;
}

/*** protected methods ***/

void TCursorTimer::DrawCursor()
{
	if ( mpView == NULL ) return;

	wxClientDC dc( mpView );

	int x = 0, y = 0;

	mpView->ScreenPosToPixels( mpView->mCursorScrPos, x, y );

	dc.SetLogicalFunction( wxINVERT );
	dc.SetBrush( mBrush );

	dc.SetPen( *wxTRANSPARENT_PEN );
	dc.DrawRectangle( x,y, 3, mpView->mCharDim.y + 1 );
	dc.SetBackgroundMode( wxSOLID );
}
