/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     22/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleskandars Gluchovas
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __DOCRIPPER_G__
#define __DOCRIPPER_G__

#include "scriptbinder.h"
#include "srcparser.h"
#include "sourcepainter.h"

#if defined( wxUSE_TEMPLATE_STL )
		
	#include <vector>

	typedef vector<ScriptTemplate*> STemplateListT;

#else

	#include "wxstlvec.h"

	typedef ScriptTemplate* ScriptTemplatePtrT; 
	typedef WXSTL_VECTOR_SHALLOW_COPY(ScriptTemplatePtrT) STemplateListT;

#endif


// specific DocGenerator class for "Ripper", 
// also acts as source code visitor

class RipperDocGen : public DocGeneratorBase, public spVisitor
{
protected:
	// templates for various sections
	ScriptTemplate mTopTempl;
	ScriptTemplate mContentIdxTempl;
	ScriptTemplate mSuperContentTempl;
	ScriptTemplate mSubContentTempl;
	ScriptTemplate mOutLineTempl;
	ScriptTemplate mOutLine1Templ;

	// template used for corss-references
	ScriptTemplate mRefTempl;

	// template used to show not-existing sections
	ScriptTemplate mDeadRefTempl;

	// template collection for generation of class-tree
	STemplateListT mTreeTemplates;

	// pointers to all major index sections
	ScriptSection* mpTopIdx;
	ScriptSection* mpClassIdx;
	ScriptSection* mpEnumIdx;
	ScriptSection* mpTypeDefIdx;
	ScriptSection* mpMacroIdx;
	ScriptSection* mpGlobalVarsIdx;
	ScriptSection* mpGlobalFuncIdx;
	ScriptSection* mpConstIdx;

	// parser set up from user-code for sepcific language
	SourceParserBase* mpParser;

	// class section, which is currently being
	// assembled
	ScriptSection*    mpCurClassSect;
	
	// source syntax heighlighter object
	SourcePainter  mSrcPainter;

	// context, to which all file contexts
	// are assembled
	spContext*     mpFileBinderCtx;

	// script tags set up from usesr code
	MarkupTagsT    mTags;

protected:
	// helpers
	void AppendComments( spContext& fromContext, string& str );

	void AppendMulitilineStr( string& st, string& mlStr );

	void AppendHighlightedSource( string& st, string source );

	// returns TRUE, if no comments found in the context,
	// plus, creates dummy(empty) section, and puts a 
	// reference woth "dead-link" template to it in the 
	// given index-section "toSect"

	bool CheckIfUncommented( spContext& ctx, ScriptSection& toSect );

	// checks if context has any comments, then returns
	// template of normal reference, otherwise of dead reference

	ScriptTemplate* GetRefTemplFor( spContext& ctx );

	// adds "someClass::" perfix to the context name,
	// if it's not in the file scope (i.e. if it's not global)

	string GetScopedName( spContext& ofCtx );

	// adds section to currently assembled class section
	// and places references to it from "public", "protected"
	// or "private" indexing-subsections of the class, depending
	// on the visibility of the context

	void AddToCurrentClass( ScriptSection* pSection, spContext& ctx, 
							const char* subSectionName );

	// called, after all files are processed, to
	// resolve possible super/derived class relations,
	// and put cross references to them - where resolution was
	// successful
	void LinkSuperClassRefs();

	// implementations of "visiting procedures", declared in spVisitor

	virtual void VisitClass( spClass& cl );
	virtual void VisitEnumeration( spEnumeration& en );
	virtual void VisitTypeDef( spTypeDef& td );
	virtual void VisitPreprocessorLine( spPreprocessorLine& pd );
	virtual void VisitAttribute( spAttribute& attr );
	virtual void VisitOperation( spOperation& op );

	// overriden member of DocGernatorBase

	virtual bool OnSaveDocument( ScriptStream& stm );

	virtual ScriptSection* GetTopSection() 
		{ return mpTopIdx; }

public:
	RipperDocGen();
	~RipperDocGen();

	// should be called onece to set user-code provided,
	// parser for specific source code language
	// (NOTE:: it's the user-code's responsibility to
	//  relseas memory of pParser)

	void Init( SourceParserBase* pParser );
	
	// should be called on each file
	
	void ProcessFile( const char* sourceFile );
};


#endif