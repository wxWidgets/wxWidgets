/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     To provide a simple _framework_
//              for series of source code parsers with
//              compatible interfaces
// Author:      Aleksandras Gluchovas
// Modified by: AG on 28/12/98
// Created:     22/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleskandars Gluchovas
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __SRCPARSER_G__
#define __SRCPARSER_G__

#if defined( wxUSE_TEMPLATE_STL )
    #include <vector>

    #ifdef WIN32
    #include <bstring.h>
    #else

    #include <strclass.h>
    #include <string.h>

    #endif

#else
    #include "wx/string.h"
    #include "wxstlvec.h"

    // FOR NOW:: quick n' dirty:

    #define  string wxString

#endif

#include "markup.h" // markup tags used in spOperator::GetFullName()

// these methods are used for debugging only and disappear in the release build
#ifdef __WXDEBUG__
    #define DECLARE_DUMP virtual void DumpThis(const wxString& indent) const;
#else
    #define DECLARE_DUMP
#endif

// context class list in "inside-out" order :

class spContext;

class spParameter;
class spAttribute;
class spOperation;
class spEnumeration;
class spTypeDef;
class spPreprocessorLine;
class spClass;
class spNameSpace;
class spFile;

// source context visibilities
enum SRC_VISIBLITY_TYPES
{
    SP_VIS_PUBLIC,
    SP_VIS_PROTECTED,
    SP_VIS_PRIVATE
};

// class types
enum SP_CLASS_TYPES
{
    SP_CLTYPE_INVALID,
    SP_CLTYPE_CLASS,
    SP_CLTYPE_TEMPLATE_CLASS,
    SP_CLTYPE_STRUCTURE,
    SP_CLTYPE_UNION,
    SP_CLTYPE_INTERFACE
};

// inheritance types
enum SP_INHERITANCE_TYPES
{
    SP_INHERIT_VIRTUAL,
    SP_INHERIT_PUBLIC,
    SP_INHERIT_PRIVATE
};

// proprocessor definitions types (specific to C++ code)

enum SP_PREP_DEFINITION_TYPES
{
    SP_PREP_DEF_DEFINE_SYMBOL,
    SP_PREP_DEF_REDEFINE_SYMBOL,
    SP_PREP_DEF_INCLUDE_FILE,
    SP_PREP_DEF_OTHER
};

// common context types

#define SP_CTX_UNKNOWN       0x000
#define    SP_CTX_FILE          0x001
#define    SP_CTX_NAMESPACE     0x002
#define    SP_CTX_CLASS         0x004
#define SP_CTX_TYPEDEF       0x008
#define SP_CTX_PREPROCESSOR  0x010
#define SP_CTX_ENUMERATION   0x020
#define    SP_CTX_ATTRIBUTE     0x040
#define    SP_CTX_OPERATION     0x080
#define    SP_CTX_PARAMETER     0x100

// other (custom) context codes may be defined elsewere, however they should
// not clash with above codes for common type and also should not
// exceed 16-bits of in value

// masks all context types (up to 16 custom context can be defined)

#define SP_CTX_ANY           0xFFFF

class spComment;



#if defined( wxUSE_TEMPLATE_STL )

    // context members
    typedef vector<spContext*> MMemberListT;
    // comments list
    typedef vector<spComment*> MCommentListT;
    // list of parameters
    typedef vector<spParameter*> MParamListT;
    // string list
    typedef vector<string>     StrListT;

#else

    typedef spContext*   spContextPtrT;
    typedef spComment*   spCommentPtrT;
    typedef spParameter* spParameterPtrT;
    typedef WXSTL_VECTOR_SHALLOW_COPY(spContextPtrT)   MMemberListT;
    typedef WXSTL_VECTOR_SHALLOW_COPY(spCommentPtrT)   MCommentListT;
    typedef WXSTL_VECTOR_SHALLOW_COPY(spParameterPtrT) MParamListT;
    typedef WXSTL_VECTOR_SHALLOW_COPY(string)          StrListT;

#endif;
// base class for all visitors of source code contents

class spVisitor
{
protected:
    bool mSiblingSkipped;
    bool mChildSkipped;
    int  mContextMask;

    spContext* mpCurCxt;

public:
    // methods invoked by context

    // method invoked from user's controling code
    // to visit all nodes staring at the given context.
    // Content is sorted if requrired, see comments
    // spClass on sorting the class members

    void VisitAll( spContext& atContext,
                   bool sortContent = TRUE
                 );

    // methods invoked by visitor

    // goes to the next context in the outter scope
    // NOTE:: should not be invoked more than once while
    //        visiting certain context

    void SkipSiblings();

    // prevents going down into the contexts contained by
    // the current context
    // NOTE:: the same as above

    void SkipChildren();

    // can be called only in from visiting procedure
    void RemoveCurrentContext();

    // method enables fast filtered traversal
    // of source content, e.g. collecting only classes,
    // or only global functions

    // arg. context - can contain combination of contexts concatinated
    //           with bitwise OR, e.g. SP_CTX_CLASS | SP_CTX_NAMESPACE
    //
    // method can be invoked from the user's controling as well as
    // from within the visting procedure

    void SetFilter( int contextMask );

    // methods should be implemneted by specific visitor:

    // NOTE:: Do not confuse visiting with parsing, first
    //        the source is parsed, and than can be visited
    //        multiple times by variouse visitors (there can
    //        be more the one visitor visiting content at a time)

    virtual void VisitFile( spFile& fl ) {}

    virtual void VisitNameSpace( spNameSpace& ns ) {}

    virtual void VisitClass( spClass& cl ) {}

    virtual void VisitEnumeration( spEnumeration& en ) {}

    virtual void VisitTypeDef( spTypeDef& td ) {}

    virtual void VisitPreprocessorLine( spPreprocessorLine& pd ) {}

    virtual void VisitAttribute( spAttribute& attr ) {}

    virtual void VisitOperation( spOperation& op ) {}

    virtual void VisitParameter( spParameter& param ) {}

    virtual void VisitCustomContext( spContext& ctx ) {}
};

// stores one section of comments,
// multiple sections can be put to geather
// and attached to some context

class spComment
{
public:
    string mText;
    bool   mIsMultiline; // multiline comments ar those with /**/'s

    // TRUE, if these was an empty empty
    // line above single line comment

    bool   mStartsPar;

public:

    bool    IsMultiline() const;
    bool    StartsParagraph() const;

    string& GetText();

    // contstant version of GetText()
    string  GetText() const;
};

// abstract base class for common (to most languages) code
// contexts (constructs), e.g file, namespace, class, operation,
// etc

class spContext
{
protected:
    // "linked" list of comments belonging to this context
    MCommentListT mComments;

    // NULL, if this is file context
    MMemberListT   mMembers;

    // NULL, if this is top-most context
    spContext*    mpParent;

    // points to context object, where the this context
    // was originally declared, meaning that this object
    // is redeclaration (or if in the case of operation
    // this context object most probably referres to the
    // implemnetation in .cpp file for example)

    // is NULL, if this object referres to the first occurence
    // of the context

    spContext*    mpFirstOccurence;

    // used, to avoid excessive sorting of context's agreggates
    bool          mAlreadySorted;

public:

    // source line number, (-1) if unknown
    int           mSrcLineNo;

    // offset of context in the source file, (-1) if unknown
    int           mSrcOffset;

    // lentgh of the context in characters, (-1) if unknown
    int           mContextLength;

    // source line number, in which this cotext ends, (-1) if unknown
    int           mLastScrLineNo;

    // fields are valid, if the may contain other contexts nested inside
    int           mHeaderLength;
    int           mFooterLength;

    // zero-based index of the first character of
    // this context in the source line, (-1) if unknown
    int           mFirstCharPos;

    // zero-based index of the first character of
    // this context in the last source line of this context, (-1) if unknown
    int           mLastCharPos;

    // see SRC_VISIBLITY_TYPES enumeration
    int           mVisibility;

    // TRUE, if context does not really exist in the source
    //       but was created by external tools (e.g. forward engineering)

    bool         mIsVirtualContext;
    bool         mVirtualContextHasChildren;

    // body of the context in case (mIsVirtual == TRUE)
    string       mVirtualContextBody;
    string       mVittualContextFooter;

    // e.g. can be used by documentation generator to store
    // reference to section object
    void*         mpUserData;

public:
    // universal identifier of the context (e.g. class name)
    string        mName;

public:
    // default constructor
    spContext();

    // automatically destorys all aggregated contexts
    // (thus, it's enought to call destructor of root-context)
    virtual ~spContext();

    // see mUererData member;
    void* GetUserData() { return mpUserData; }

    // sets untyped pointer to user data
    void SetUserData( void* pUserData )
        { mpUserData = pUserData; }

    // searches the whole context tree for the cotnexts
    // which match given masks, pust results into lst array
    void GetContextList( MMemberListT& lst, int contextMask );

    // used by default visitor's implementation
    bool IsSorted();

    /*** forward/reverse ingineering fecilities ***/

    bool PositionIsKnown();

    bool IsVirtualContext();

    bool VitualContextHasChildren();

    void SetVirtualContextBody( const string& body,
                                bool  hasChildren = FALSE,
                                const string& footer = "" );

    string GetVirtualContextBody();
    string GetFooterOfVirtualContextBody();

    // can be overriden by top-level context classes
    // to find-out ot the source-fragment of this
    // context using it's position information
    virtual string GetBody( spContext* pCtx = NULL );

    virtual string GetHeader( spContext* pCtx = NULL );

    // TRUE, if there is at least one entry
    // in the comment list of this context
    bool HasComments();
    MCommentListT& GetCommentList() { return mComments; }
    const MCommentListT& GetCommentList() const { return mComments; }

    // should be overriden, if the context supports sorting
    // of it's members
    virtual void SortMembers() {}

    // returns identifier of this context
    inline string& GetName() { return mName; }

    // returns -1, if souce line # is unknow
    inline int GetSourceLineNo() { return mSrcLineNo; }

    // see comments on mpFirstOccurence member variable
    bool IsFirstOccurence();
    spContext* GetFirstOccurence();

    // returns not-NULL value if this context
    // is aggregated by another cotnext
    spContext* GetOutterContext();

    // perhaps more intuitive alias for `GetOutterContext()'
    inline spContext* GetParent() { return mpParent; }

    bool HasOutterContext();

    // add one aggregate (or child) into this context
    void AddMember ( spContext* pMember );
    MMemberListT& GetMembers();

    // append comment to the comment list decribing
    // this context
    void AddComment( spComment* pComment );

    // returns NULL, if the context with the given
    // name and type is not contained by this context
    // and it's children. Children's children are not
    // searched recursivelly if searchSubMembers is FALSE

    spContext* FindContext( const string& identifier,
                            int   contextType      = SP_CTX_ANY,
                            bool  searchSubMembers = TRUE
                          );

    // removes this context from it's parent
    // (NOTE:: context should have an outter cotnext
    //  to when this method is called, otherwise removal
    //  will result assertion failure)
    void RemoveThisContext();

    // returns TRUE, if this object is aggregated in the file
    bool IsInFile();

    // TRUE, if outter context is a namespace
    bool IsInNameSpace();

    // TRUE, if outter context is a class
    bool IsInClass();

    // TRUE, if outter cotext is an operation (TRUE for "spParameter"s)
    bool IsInOperation();

    // TRUE if the context is public
    bool IsPublic() const { return mVisibility == SP_VIS_PUBLIC; }

    // NOTE:: method returns not the type of this object
    //          but the file/namespace/class/operation or file in which this
    //          attribute is contained. First, check for the type of
    //        context using the above method.

    //          Requiering container which does not exist, will result
    //        in assertion failure

    spClass&     GetClass();
    spFile&      GetFile();
    spNameSpace& GetNameSpace();
    spOperation& GetOperation();

    // each new context should override this method
    // to return it's specific type
    virtual int GetContextType() const { return SP_CTX_UNKNOWN; }

    // perhaps more intuitive short-cut
    inline int GetType() { return GetContextType(); }

    // cast this context to the desired type - returns NULL if type is wrong
    spAttribute *CastToAttribute()
    {
        return GetContextType() == SP_CTX_ATTRIBUTE ? (spAttribute *)this
                                                    : NULL;
    }

    // derived classes override this to invoke VisitXXX method
    // which corresponds to the class of specific context,
    // - this is what the "Visitor" pattern told us ^)

    // if method is not overriden, then it's probably user-defined
    // custom context

    virtual void AcceptVisitor( spVisitor& visitor )

        { visitor.VisitCustomContext( *this );    };

    // called by visitors, to remove given subcontext
    // of this context object
    void RemoveChild( spContext* pChild );

    void RemoveChildren();

    spContext* GetEnclosingContext( int mask = SP_CTX_ANY );

#ifdef __WXDEBUG__
    virtual void Dump(const wxString& indent) const;
#endif  // __WXDEBUG__

    DECLARE_DUMP
};

// stores information about single argument of operation

class spParameter : public spContext
{
public:
    // type of argument (parameter)
    string mType;

    // "stringified" initial value
    string mInitVal;

public:
    virtual int GetContextType() const { return SP_CTX_PARAMETER; }

    virtual void AcceptVisitor( spVisitor& visitor )
        { visitor.VisitParameter( *this ); }

    DECLARE_DUMP
};


// stores information about member(or global) variable

class spAttribute : public spContext
{
public:
    // type of the attribute
    string mType;

    // it's initial value
    string mInitVal;

    // constantness
    bool   mIsConstant;
public:

    virtual int GetContextType() const { return SP_CTX_ATTRIBUTE; }

    virtual void AcceptVisitor( spVisitor& visitor )
        { visitor.VisitAttribute( *this ); }

    DECLARE_DUMP
};

class spOperation : public spContext
{
public:
    // type of return value
    string      mRetType;

    // argument list
    //MParamListT mParams;

    // TRUE, if operation does not modify
    // the content of the object
    bool        mIsConstant;

    // flag, specific to C++
    bool        mIsVirtual;

    // TRUE, if definition follows the declaration immediatelly
    bool        mHasDefinition;

    // scope if any (e.g. MyClass::MyFunction(), scope stirng is "MyClass" )
    // usually found along with implementation of the method, which is now skipped

    string      mScope;

public:
    spOperation();

    // returns full declaration of the operations
    // (ret val., identifier, arg. list),

    // arguments are marked up with italic,
    // default values marked up with bold-italic,
    // all the rest is marked as bold

    // NOTE:: this method may be overriden by class
    //        specific to concrete parser, to provide
    //        language-dependent reperesnetation of
    //        operation and it's argumetn list
    //
    // the default implementation outputs name in
    // C++/Java syntax

    virtual string GetFullName(MarkupTagsT tags);

    virtual int GetContextType() const { return SP_CTX_OPERATION; }

    virtual void AcceptVisitor( spVisitor& visitor )
        { visitor.VisitOperation( *this ); }

    DECLARE_DUMP
};

// stores infromation about preprocessor directive

class spPreprocessorLine : public spContext
{

public:

    // prepocessor statement including '#' and
    // attached multiple lines with '\' character
    string mLine;

    int    mDefType; // see SP_PREP_DEFINITION_TYPES enumeration

public:

    virtual int GetContextType() const { return SP_CTX_PREPROCESSOR; }

    virtual int GetStatementType() const { return mDefType; }

    string CPP_GetIncludedFileNeme() const;

    virtual void AcceptVisitor( spVisitor& visitor )
        { visitor.VisitPreprocessorLine( *this ); }

    DECLARE_DUMP
};

// stores information about the class

class spClass : public spContext
{
public:
    // list of superclasses/interfaces
    StrListT     mSuperClassNames;

    // see SP_CLASS_TYPES enumeration
    int          mClassSubType;

    // see SP_INHERITANCE_TYPES enumeration
    int          mInheritanceType;

    // valid if mClassSubType is SP_CLTYPE_TEMPLATE_CLASS
    string       mTemplateTypes;

    // TRUE, if it's and interface of abstract base class
    bool         mIsAbstract;

public:
    // sorts class members in the following order:
    //
    // (by "privacy level" - first private, than protected, public)
    //
    //     within above set
    //
    //       (by member type - attributes first, than methods, nested classes)
    //
    //          within above set
    //
    //             (by identifier of the member)

    virtual void SortMembers();

    virtual int GetContextType() const { return SP_CTX_CLASS; }

    virtual void AcceptVisitor( spVisitor& visitor )
        { visitor.VisitClass( *this ); }

    DECLARE_DUMP
};

// stores information about enum statement

class spEnumeration  : public spContext
{
public:
    string mEnumContent; // full-text content of enumeration

public:
    virtual int GetContextType() const { return SP_CTX_ENUMERATION; }

    virtual void AcceptVisitor( spVisitor& visitor )
        { visitor.VisitEnumeration( *this ); }

    DECLARE_DUMP
};

class spTypeDef  : public spContext
{
public:
    // the original type which is redefined
    // by this type definition
    string mOriginalType;

public:
    virtual int GetContextType() const { return SP_CTX_TYPEDEF; }

    virtual void AcceptVisitor( spVisitor& visitor )
        { visitor.VisitTypeDef( *this ); }

    DECLARE_DUMP
};

// NOTE:: files context may be put to other
//        file context, resulting in a collection
//        of parsed file contexts, with a virtual "superfile"

class spFile : public spContext
{
public:
    // since file name cannot be determined from
    // source code, filling in this field is optional
    string mFileName;

public:
    virtual int GetContextType() const { return SP_CTX_FILE; }

    virtual void AcceptVisitor( spVisitor& visitor )
        { visitor.VisitFile( *this ); }

    DECLARE_DUMP
};

//TODO:: comments.

class SourceParserPlugin
{
public:
    virtual bool CanUnderstandContext( char* cur, char* end, spContext* pOuttterCtx ) = 0;
    virtual void ParseContext( char* start, char*& cur, char* end, spContext* pOuttterCtx ) = 0;
};

// abstract interface for source parsers
// which can output parsing results in the
// form of context-tree, where each node
// should be derivative of spContext, (see
// above classes)

class SourceParserBase
{
private:
    // auto-resizing file buffer, created in ParseFile()
    // to reuse large heap block for multiple parsings

    char* mpFileBuf;
    int   mFileBufSz;

protected:
    SourceParserPlugin* mpPlugin;

protected:
    // value is set in the derived parser classes
    int mParserStatus;

public:
    SourceParserBase();
    virtual ~SourceParserBase();

    // loads entier source file(as text) into memory,
    // and passes it's contents to ParseAll() method,
    // memory occupied by source text is released after
    // parsing is done
    //
    // (NOTE:: this is the default implementation),

    virtual spFile* ParseFile( const char* fname );

    // should returns the root-node of the created context tree
    // (user is responsible for releasing it from the heep)
    // "end" should point to the (last character + 1) of the
    // source text area

    virtual spFile* Parse( char* start, char* end ) = 0;

    // returns parser "status word" (specific to concrete parser)
    int GetParserStatus() { return mParserStatus; }

    void SetPlugin( SourceParserPlugin* pPlugin );
};

#endif
