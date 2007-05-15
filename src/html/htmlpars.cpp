/////////////////////////////////////////////////////////////////////////////
// Name:        src/html/htmlpars.cpp
// Purpose:     wxHtmlParser class (generic parser)
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_HTML && wxUSE_STREAMS

#ifndef WX_PRECOMP
    #include "wx/dynarray.h"
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/app.h"
    #include "wx/wxcrtvararg.h"
#endif

#include "wx/tokenzr.h"
#include "wx/wfstream.h"
#include "wx/url.h"
#include "wx/fontmap.h"
#include "wx/html/htmldefs.h"
#include "wx/html/htmlpars.h"
#include "wx/arrimpl.cpp"

#ifdef __WXWINCE__
    #include "wx/msw/wince/missing.h"       // for bsearch()
#endif

// DLL options compatibility check:
WX_CHECK_BUILD_OPTIONS("wxHTML")

const wxChar *wxTRACE_HTML_DEBUG = _T("htmldebug");

//-----------------------------------------------------------------------------
// wxHtmlParser helpers
//-----------------------------------------------------------------------------

class wxHtmlTextPiece
{
public:
    wxHtmlTextPiece(int pos, int lng) : m_pos(pos), m_lng(lng) {}
    int m_pos, m_lng;
};

WX_DECLARE_OBJARRAY(wxHtmlTextPiece, wxHtmlTextPieces);
WX_DEFINE_OBJARRAY(wxHtmlTextPieces)

class wxHtmlParserState
{
public:
    wxHtmlTag         *m_curTag;
    wxHtmlTag         *m_tags;
    wxHtmlTextPieces  *m_textPieces;
    int                m_curTextPiece;
    wxString           m_source;
    wxHtmlParserState *m_nextState;
};

//-----------------------------------------------------------------------------
// wxHtmlParser
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxHtmlParser,wxObject)

wxHtmlParser::wxHtmlParser()
    : wxObject(), m_HandlersHash(wxKEY_STRING),
      m_FS(NULL), m_HandlersStack(NULL)
{
    m_entitiesParser = new wxHtmlEntitiesParser;
    m_Tags = NULL;
    m_CurTag = NULL;
    m_TextPieces = NULL;
    m_CurTextPiece = 0;
    m_SavedStates = NULL;
}

wxHtmlParser::~wxHtmlParser()
{
    while (RestoreState()) {}
    DestroyDOMTree();

    if (m_HandlersStack)
    {
        wxList& tmp = *m_HandlersStack;
        wxList::iterator it, en;
        for( it = tmp.begin(), en = tmp.end(); it != en; ++it )
            delete (wxHashTable*)*it;
        tmp.clear();
    }
    delete m_HandlersStack;
    m_HandlersHash.Clear();
    WX_CLEAR_LIST(wxList, m_HandlersList);
    delete m_entitiesParser;
}

wxObject* wxHtmlParser::Parse(const wxString& source)
{
    InitParser(source);
    DoParsing();
    wxObject *result = GetProduct();
    DoneParser();
    return result;
}

void wxHtmlParser::InitParser(const wxString& source)
{
    SetSource(source);
    m_stopParsing = false;
}

void wxHtmlParser::DoneParser()
{
    DestroyDOMTree();
}

void wxHtmlParser::SetSource(const wxString& src)
{
    DestroyDOMTree();
    m_Source = src;
    CreateDOMTree();
    m_CurTag = NULL;
    m_CurTextPiece = 0;
}

void wxHtmlParser::CreateDOMTree()
{
    wxHtmlTagsCache cache(m_Source);
    m_TextPieces = new wxHtmlTextPieces;
    CreateDOMSubTree(NULL, 0, m_Source.length(), &cache);
    m_CurTextPiece = 0;
}

extern bool wxIsCDATAElement(const wxChar *tag);

void wxHtmlParser::CreateDOMSubTree(wxHtmlTag *cur,
                                    int begin_pos, int end_pos,
                                    wxHtmlTagsCache *cache)
{
    if (end_pos <= begin_pos) return;

    wxChar c;
    int i = begin_pos;
    int textBeginning = begin_pos;

    // If the tag contains CDATA text, we include the text between beginning
    // and ending tag verbosely. Setting i=end_pos will skip to the very
    // end of this function where text piece is added, bypassing any child
    // tags parsing (CDATA element can't have child elements by definition):
    if (cur != NULL && wxIsCDATAElement(cur->GetName().c_str()))
    {
        i = end_pos;
    }

    while (i < end_pos)
    {
        c = m_Source.GetChar(i);

        if (c == wxT('<'))
        {
            // add text to m_TextPieces:
            if (i - textBeginning > 0)
                m_TextPieces->Add(
                    wxHtmlTextPiece(textBeginning, i - textBeginning));

            // if it is a comment, skip it:
            wxString::const_iterator iter = m_Source.begin() + i;
            if ( SkipCommentTag(iter, m_Source.end()) )
            {
                textBeginning =
                i = iter - m_Source.begin() + 1; // skip closing '>' too
            }

            // add another tag to the tree:
            else if (i < end_pos-1 && m_Source.GetChar(i+1) != wxT('/'))
            {
                wxHtmlTag *chd;
                if (cur)
                    chd = new wxHtmlTag(cur, m_Source,
                                        i, end_pos, cache, m_entitiesParser);
                else
                {
                    chd = new wxHtmlTag(NULL, m_Source,
                                        i, end_pos, cache, m_entitiesParser);
                    if (!m_Tags)
                    {
                        // if this is the first tag to be created make the root
                        // m_Tags point to it:
                        m_Tags = chd;
                    }
                    else
                    {
                        // if there is already a root tag add this tag as
                        // the last sibling:
                        chd->m_Prev = m_Tags->GetLastSibling();
                        chd->m_Prev->m_Next = chd;
                    }
                }

                if (chd->HasEnding())
                {
                    CreateDOMSubTree(chd,
                                     chd->GetBeginPos(), chd->GetEndPos1(),
                                     cache);
                    i = chd->GetEndPos2();
                }
                else
                    i = chd->GetBeginPos();

                textBeginning = i;
            }

            // ... or skip ending tag:
            else
            {
                while (i < end_pos && m_Source.GetChar(i) != wxT('>')) i++;
                textBeginning = i+1;
            }
        }
        else i++;
    }

    // add remaining text to m_TextPieces:
    if (end_pos - textBeginning > 0)
        m_TextPieces->Add(
            wxHtmlTextPiece(textBeginning, end_pos - textBeginning));
}

void wxHtmlParser::DestroyDOMTree()
{
    wxHtmlTag *t1, *t2;
    t1 = m_Tags;
    while (t1)
    {
        t2 = t1->GetNextSibling();
        delete t1;
        t1 = t2;
    }
    m_Tags = m_CurTag = NULL;

    delete m_TextPieces;
    m_TextPieces = NULL;
}

void wxHtmlParser::DoParsing()
{
    m_CurTag = m_Tags;
    m_CurTextPiece = 0;
    DoParsing(0, m_Source.length());
}

void wxHtmlParser::DoParsing(int begin_pos, int end_pos)
{
    if (end_pos <= begin_pos) return;

    wxHtmlTextPieces& pieces = *m_TextPieces;
    size_t piecesCnt = pieces.GetCount();

    while (begin_pos < end_pos)
    {
        while (m_CurTag && m_CurTag->GetBeginPos() < begin_pos)
            m_CurTag = m_CurTag->GetNextTag();
        while (m_CurTextPiece < piecesCnt &&
               pieces[m_CurTextPiece].m_pos < begin_pos)
            m_CurTextPiece++;

        if (m_CurTextPiece < piecesCnt &&
            (!m_CurTag ||
             pieces[m_CurTextPiece].m_pos < m_CurTag->GetBeginPos()))
        {
            // Add text:
            AddText(GetEntitiesParser()->Parse(
                       m_Source.Mid(pieces[m_CurTextPiece].m_pos,
                                    pieces[m_CurTextPiece].m_lng)));
            begin_pos = pieces[m_CurTextPiece].m_pos +
                        pieces[m_CurTextPiece].m_lng;
            m_CurTextPiece++;
        }
        else if (m_CurTag)
        {
            if (m_CurTag->HasEnding())
                begin_pos = m_CurTag->GetEndPos2();
            else
                begin_pos = m_CurTag->GetBeginPos();
            wxHtmlTag *t = m_CurTag;
            m_CurTag = m_CurTag->GetNextTag();
            AddTag(*t);
            if (m_stopParsing)
                return;
        }
        else break;
    }
}

void wxHtmlParser::AddTag(const wxHtmlTag& tag)
{
    wxHtmlTagHandler *h;
    bool inner = false;

    h = (wxHtmlTagHandler*) m_HandlersHash.Get(tag.GetName());
    if (h)
    {
        inner = h->HandleTag(tag);
        if (m_stopParsing)
            return;
    }
    if (!inner)
    {
        if (tag.HasEnding())
            DoParsing(tag.GetBeginPos(), tag.GetEndPos1());
    }
}

void wxHtmlParser::AddTagHandler(wxHtmlTagHandler *handler)
{
    wxString s(handler->GetSupportedTags());
    wxStringTokenizer tokenizer(s, wxT(", "));

    while (tokenizer.HasMoreTokens())
        m_HandlersHash.Put(tokenizer.GetNextToken(), handler);

    if (m_HandlersList.IndexOf(handler) == wxNOT_FOUND)
        m_HandlersList.Append(handler);

    handler->SetParser(this);
}

void wxHtmlParser::PushTagHandler(wxHtmlTagHandler *handler, const wxString& tags)
{
    wxStringTokenizer tokenizer(tags, wxT(", "));
    wxString key;

    if (m_HandlersStack == NULL)
    {
        m_HandlersStack = new wxList;
    }

    m_HandlersStack->Insert((wxObject*)new wxHashTable(m_HandlersHash));

    while (tokenizer.HasMoreTokens())
    {
        key = tokenizer.GetNextToken();
        m_HandlersHash.Delete(key);
        m_HandlersHash.Put(key, handler);
    }
}

void wxHtmlParser::PopTagHandler()
{
    wxList::compatibility_iterator first;

    if ( !m_HandlersStack ||
#if wxUSE_STL
         !(first = m_HandlersStack->GetFirst())
#else // !wxUSE_STL
         ((first = m_HandlersStack->GetFirst()) == NULL)
#endif // wxUSE_STL/!wxUSE_STL
        )
    {
        wxLogWarning(_("Warning: attempt to remove HTML tag handler from empty stack."));
        return;
    }
    m_HandlersHash = *((wxHashTable*) first->GetData());
    delete (wxHashTable*) first->GetData();
    m_HandlersStack->Erase(first);
}

void wxHtmlParser::SetSourceAndSaveState(const wxString& src)
{
    wxHtmlParserState *s = new wxHtmlParserState;

    s->m_curTag = m_CurTag;
    s->m_tags = m_Tags;
    s->m_textPieces = m_TextPieces;
    s->m_curTextPiece = m_CurTextPiece;
    s->m_source = m_Source;

    s->m_nextState = m_SavedStates;
    m_SavedStates = s;

    m_CurTag = NULL;
    m_Tags = NULL;
    m_TextPieces = NULL;
    m_CurTextPiece = 0;
    m_Source = wxEmptyString;

    SetSource(src);
}

bool wxHtmlParser::RestoreState()
{
    if (!m_SavedStates) return false;

    DestroyDOMTree();

    wxHtmlParserState *s = m_SavedStates;
    m_SavedStates = s->m_nextState;

    m_CurTag = s->m_curTag;
    m_Tags = s->m_tags;
    m_TextPieces = s->m_textPieces;
    m_CurTextPiece = s->m_curTextPiece;
    m_Source = s->m_source;

    delete s;
    return true;
}

wxString wxHtmlParser::GetInnerSource(const wxHtmlTag& tag)
{
    return GetSource()->Mid(tag.GetBeginPos(),
                            tag.GetEndPos1() - tag.GetBeginPos());
}

//-----------------------------------------------------------------------------
// wxHtmlTagHandler
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxHtmlTagHandler,wxObject)

void wxHtmlTagHandler::ParseInnerSource(const wxString& source)
{
    // It is safe to temporarily change the source being parsed,
    // provided we restore the state back after parsing
    m_Parser->SetSourceAndSaveState(source);
    m_Parser->DoParsing();
    m_Parser->RestoreState();
}


//-----------------------------------------------------------------------------
// wxHtmlEntitiesParser
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxHtmlEntitiesParser,wxObject)

wxHtmlEntitiesParser::wxHtmlEntitiesParser()
#if wxUSE_WCHAR_T && !wxUSE_UNICODE
    : m_conv(NULL), m_encoding(wxFONTENCODING_SYSTEM)
#endif
{
}

wxHtmlEntitiesParser::~wxHtmlEntitiesParser()
{
#if wxUSE_WCHAR_T && !wxUSE_UNICODE
    delete m_conv;
#endif
}

void wxHtmlEntitiesParser::SetEncoding(wxFontEncoding encoding)
{
#if wxUSE_WCHAR_T && !wxUSE_UNICODE
    if (encoding == m_encoding)
        return;

    delete m_conv;

    m_encoding = encoding;
    if (m_encoding == wxFONTENCODING_SYSTEM)
        m_conv = NULL;
    else
        m_conv = new wxCSConv(wxFontMapper::GetEncodingName(m_encoding));
#else
    (void) encoding;
#endif
}

wxString wxHtmlEntitiesParser::Parse(const wxString& input)
{
    const wxChar *c, *last;
    const wxChar *in_str = input.c_str();
    wxString output;

    output.reserve(input.length());

    for (c = in_str, last = in_str; *c != wxT('\0'); c++)
    {
        if (*c == wxT('&'))
        {
            if (c - last > 0)
                output.append(last, c - last);
            if ( *++c == wxT('\0') )
                break;

            wxString entity;
            const wxChar *ent_s = c;
            wxChar entity_char;

            for (; (*c >= wxT('a') && *c <= wxT('z')) ||
                   (*c >= wxT('A') && *c <= wxT('Z')) ||
                   (*c >= wxT('0') && *c <= wxT('9')) ||
                   *c == wxT('_') || *c == wxT('#'); c++) {}
            entity.append(ent_s, c - ent_s);
            if (*c != wxT(';')) c--;
            last = c+1;
            entity_char = GetEntityChar(entity);
            if (entity_char)
                output << entity_char;
            else
            {
                output.append(ent_s-1, c-ent_s+2);
                wxLogTrace(wxTRACE_HTML_DEBUG,
                           wxT("Unrecognized HTML entity: '%s'"),
                           entity.c_str());
            }
        }
    }
    if (*last != wxT('\0'))
        output.append(last);
    return output;
}

#if !wxUSE_UNICODE
wxChar wxHtmlEntitiesParser::GetCharForCode(unsigned code)
{
#if wxUSE_WCHAR_T
    char buf[2];
    wchar_t wbuf[2];
    wbuf[0] = (wchar_t)code;
    wbuf[1] = 0;
    wxMBConv *conv = m_conv ? m_conv : &wxConvLocal;
    if (conv->WC2MB(buf, wbuf, 2) == (size_t)-1)
        return '?';
    return buf[0];
#else
    return (code < 256) ? (wxChar)code : '?';
#endif
}
#endif

struct wxHtmlEntityInfo
{
    const wxStringCharType *name;
    unsigned code;
};

extern "C" int LINKAGEMODE wxHtmlEntityCompare(const void *key, const void *item)
{
#if wxUSE_UNICODE_UTF8
    return strcmp((char*)key, ((wxHtmlEntityInfo*)item)->name);
#else
    return wxStrcmp((wxChar*)key, ((wxHtmlEntityInfo*)item)->name);
#endif
}

wxChar wxHtmlEntitiesParser::GetEntityChar(const wxString& entity)
{
    unsigned code = 0;

    if (entity[0] == wxT('#'))
    {
        const wxChar *ent_s = entity.c_str();
        const wxChar *format;

        if (ent_s[1] == wxT('x') || ent_s[1] == wxT('X'))
        {
            format = wxT("%x");
            ent_s++;
        }
        else
            format = wxT("%u");
        ent_s++;

        if (wxSscanf(ent_s, format, &code) != 1)
            code = 0;
    }
    else
    {
        // store the literals in wx's internal representation (either char*
        // in UTF-8 or wchar_t*) for best performance:
        #define ENTITY(name, code) { wxSTRING_TEXT(name), code }

        static wxHtmlEntityInfo substitutions[] = {
            ENTITY("AElig", 198),
            ENTITY("Aacute", 193),
            ENTITY("Acirc", 194),
            ENTITY("Agrave", 192),
            ENTITY("Alpha", 913),
            ENTITY("Aring", 197),
            ENTITY("Atilde", 195),
            ENTITY("Auml", 196),
            ENTITY("Beta", 914),
            ENTITY("Ccedil", 199),
            ENTITY("Chi", 935),
            ENTITY("Dagger", 8225),
            ENTITY("Delta", 916),
            ENTITY("ETH", 208),
            ENTITY("Eacute", 201),
            ENTITY("Ecirc", 202),
            ENTITY("Egrave", 200),
            ENTITY("Epsilon", 917),
            ENTITY("Eta", 919),
            ENTITY("Euml", 203),
            ENTITY("Gamma", 915),
            ENTITY("Iacute", 205),
            ENTITY("Icirc", 206),
            ENTITY("Igrave", 204),
            ENTITY("Iota", 921),
            ENTITY("Iuml", 207),
            ENTITY("Kappa", 922),
            ENTITY("Lambda", 923),
            ENTITY("Mu", 924),
            ENTITY("Ntilde", 209),
            ENTITY("Nu", 925),
            ENTITY("OElig", 338),
            ENTITY("Oacute", 211),
            ENTITY("Ocirc", 212),
            ENTITY("Ograve", 210),
            ENTITY("Omega", 937),
            ENTITY("Omicron", 927),
            ENTITY("Oslash", 216),
            ENTITY("Otilde", 213),
            ENTITY("Ouml", 214),
            ENTITY("Phi", 934),
            ENTITY("Pi", 928),
            ENTITY("Prime", 8243),
            ENTITY("Psi", 936),
            ENTITY("Rho", 929),
            ENTITY("Scaron", 352),
            ENTITY("Sigma", 931),
            ENTITY("THORN", 222),
            ENTITY("Tau", 932),
            ENTITY("Theta", 920),
            ENTITY("Uacute", 218),
            ENTITY("Ucirc", 219),
            ENTITY("Ugrave", 217),
            ENTITY("Upsilon", 933),
            ENTITY("Uuml", 220),
            ENTITY("Xi", 926),
            ENTITY("Yacute", 221),
            ENTITY("Yuml", 376),
            ENTITY("Zeta", 918),
            ENTITY("aacute", 225),
            ENTITY("acirc", 226),
            ENTITY("acute", 180),
            ENTITY("aelig", 230),
            ENTITY("agrave", 224),
            ENTITY("alefsym", 8501),
            ENTITY("alpha", 945),
            ENTITY("amp", 38),
            ENTITY("and", 8743),
            ENTITY("ang", 8736),
            ENTITY("aring", 229),
            ENTITY("asymp", 8776),
            ENTITY("atilde", 227),
            ENTITY("auml", 228),
            ENTITY("bdquo", 8222),
            ENTITY("beta", 946),
            ENTITY("brvbar", 166),
            ENTITY("bull", 8226),
            ENTITY("cap", 8745),
            ENTITY("ccedil", 231),
            ENTITY("cedil", 184),
            ENTITY("cent", 162),
            ENTITY("chi", 967),
            ENTITY("circ", 710),
            ENTITY("clubs", 9827),
            ENTITY("cong", 8773),
            ENTITY("copy", 169),
            ENTITY("crarr", 8629),
            ENTITY("cup", 8746),
            ENTITY("curren", 164),
            ENTITY("dArr", 8659),
            ENTITY("dagger", 8224),
            ENTITY("darr", 8595),
            ENTITY("deg", 176),
            ENTITY("delta", 948),
            ENTITY("diams", 9830),
            ENTITY("divide", 247),
            ENTITY("eacute", 233),
            ENTITY("ecirc", 234),
            ENTITY("egrave", 232),
            ENTITY("empty", 8709),
            ENTITY("emsp", 8195),
            ENTITY("ensp", 8194),
            ENTITY("epsilon", 949),
            ENTITY("equiv", 8801),
            ENTITY("eta", 951),
            ENTITY("eth", 240),
            ENTITY("euml", 235),
            ENTITY("euro", 8364),
            ENTITY("exist", 8707),
            ENTITY("fnof", 402),
            ENTITY("forall", 8704),
            ENTITY("frac12", 189),
            ENTITY("frac14", 188),
            ENTITY("frac34", 190),
            ENTITY("frasl", 8260),
            ENTITY("gamma", 947),
            ENTITY("ge", 8805),
            ENTITY("gt", 62),
            ENTITY("hArr", 8660),
            ENTITY("harr", 8596),
            ENTITY("hearts", 9829),
            ENTITY("hellip", 8230),
            ENTITY("iacute", 237),
            ENTITY("icirc", 238),
            ENTITY("iexcl", 161),
            ENTITY("igrave", 236),
            ENTITY("image", 8465),
            ENTITY("infin", 8734),
            ENTITY("int", 8747),
            ENTITY("iota", 953),
            ENTITY("iquest", 191),
            ENTITY("isin", 8712),
            ENTITY("iuml", 239),
            ENTITY("kappa", 954),
            ENTITY("lArr", 8656),
            ENTITY("lambda", 955),
            ENTITY("lang", 9001),
            ENTITY("laquo", 171),
            ENTITY("larr", 8592),
            ENTITY("lceil", 8968),
            ENTITY("ldquo", 8220),
            ENTITY("le", 8804),
            ENTITY("lfloor", 8970),
            ENTITY("lowast", 8727),
            ENTITY("loz", 9674),
            ENTITY("lrm", 8206),
            ENTITY("lsaquo", 8249),
            ENTITY("lsquo", 8216),
            ENTITY("lt", 60),
            ENTITY("macr", 175),
            ENTITY("mdash", 8212),
            ENTITY("micro", 181),
            ENTITY("middot", 183),
            ENTITY("minus", 8722),
            ENTITY("mu", 956),
            ENTITY("nabla", 8711),
            ENTITY("nbsp", 160),
            ENTITY("ndash", 8211),
            ENTITY("ne", 8800),
            ENTITY("ni", 8715),
            ENTITY("not", 172),
            ENTITY("notin", 8713),
            ENTITY("nsub", 8836),
            ENTITY("ntilde", 241),
            ENTITY("nu", 957),
            ENTITY("oacute", 243),
            ENTITY("ocirc", 244),
            ENTITY("oelig", 339),
            ENTITY("ograve", 242),
            ENTITY("oline", 8254),
            ENTITY("omega", 969),
            ENTITY("omicron", 959),
            ENTITY("oplus", 8853),
            ENTITY("or", 8744),
            ENTITY("ordf", 170),
            ENTITY("ordm", 186),
            ENTITY("oslash", 248),
            ENTITY("otilde", 245),
            ENTITY("otimes", 8855),
            ENTITY("ouml", 246),
            ENTITY("para", 182),
            ENTITY("part", 8706),
            ENTITY("permil", 8240),
            ENTITY("perp", 8869),
            ENTITY("phi", 966),
            ENTITY("pi", 960),
            ENTITY("piv", 982),
            ENTITY("plusmn", 177),
            ENTITY("pound", 163),
            ENTITY("prime", 8242),
            ENTITY("prod", 8719),
            ENTITY("prop", 8733),
            ENTITY("psi", 968),
            ENTITY("quot", 34),
            ENTITY("rArr", 8658),
            ENTITY("radic", 8730),
            ENTITY("rang", 9002),
            ENTITY("raquo", 187),
            ENTITY("rarr", 8594),
            ENTITY("rceil", 8969),
            ENTITY("rdquo", 8221),
            ENTITY("real", 8476),
            ENTITY("reg", 174),
            ENTITY("rfloor", 8971),
            ENTITY("rho", 961),
            ENTITY("rlm", 8207),
            ENTITY("rsaquo", 8250),
            ENTITY("rsquo", 8217),
            ENTITY("sbquo", 8218),
            ENTITY("scaron", 353),
            ENTITY("sdot", 8901),
            ENTITY("sect", 167),
            ENTITY("shy", 173),
            ENTITY("sigma", 963),
            ENTITY("sigmaf", 962),
            ENTITY("sim", 8764),
            ENTITY("spades", 9824),
            ENTITY("sub", 8834),
            ENTITY("sube", 8838),
            ENTITY("sum", 8721),
            ENTITY("sup", 8835),
            ENTITY("sup1", 185),
            ENTITY("sup2", 178),
            ENTITY("sup3", 179),
            ENTITY("supe", 8839),
            ENTITY("szlig", 223),
            ENTITY("tau", 964),
            ENTITY("there4", 8756),
            ENTITY("theta", 952),
            ENTITY("thetasym", 977),
            ENTITY("thinsp", 8201),
            ENTITY("thorn", 254),
            ENTITY("tilde", 732),
            ENTITY("times", 215),
            ENTITY("trade", 8482),
            ENTITY("uArr", 8657),
            ENTITY("uacute", 250),
            ENTITY("uarr", 8593),
            ENTITY("ucirc", 251),
            ENTITY("ugrave", 249),
            ENTITY("uml", 168),
            ENTITY("upsih", 978),
            ENTITY("upsilon", 965),
            ENTITY("uuml", 252),
            ENTITY("weierp", 8472),
            ENTITY("xi", 958),
            ENTITY("yacute", 253),
            ENTITY("yen", 165),
            ENTITY("yuml", 255),
            ENTITY("zeta", 950),
            ENTITY("zwj", 8205),
            ENTITY("zwnj", 8204),
            {NULL, 0}};
        #undef ENTITY
        static size_t substitutions_cnt = 0;

        if (substitutions_cnt == 0)
            while (substitutions[substitutions_cnt].code != 0)
                substitutions_cnt++;

        wxHtmlEntityInfo *info = NULL;
#ifdef __WXWINCE__
        // bsearch crashes under WinCE for some reason
        size_t i;
        for (i = 0; i < substitutions_cnt; i++)
        {
            if (entity == substitutions[i].name)
            {
                info = & substitutions[i];
                break;
            }
        }
#else
        info = (wxHtmlEntityInfo*) bsearch(entity.wx_str(), substitutions,
                                           substitutions_cnt,
                                           sizeof(wxHtmlEntityInfo),
                                           wxHtmlEntityCompare);
#endif
        if (info)
            code = info->code;
    }

    if (code == 0)
        return 0;
    else
        return GetCharForCode(code);
}

wxFSFile *wxHtmlParser::OpenURL(wxHtmlURLType WXUNUSED(type),
                                const wxString& url) const
{
    return m_FS ? m_FS->OpenFile(url) : NULL;

}


//-----------------------------------------------------------------------------
// wxHtmlParser::ExtractCharsetInformation
//-----------------------------------------------------------------------------

class wxMetaTagParser : public wxHtmlParser
{
public:
    wxMetaTagParser() { }

    wxObject* GetProduct() { return NULL; }

protected:
    virtual void AddText(const wxChar* WXUNUSED(txt)) {}

    DECLARE_NO_COPY_CLASS(wxMetaTagParser)
};

class wxMetaTagHandler : public wxHtmlTagHandler
{
public:
    wxMetaTagHandler(wxString *retval) : wxHtmlTagHandler(), m_retval(retval) {}
    wxString GetSupportedTags() { return wxT("META,BODY"); }
    bool HandleTag(const wxHtmlTag& tag);

private:
    wxString *m_retval;

    DECLARE_NO_COPY_CLASS(wxMetaTagHandler)
};

bool wxMetaTagHandler::HandleTag(const wxHtmlTag& tag)
{
    if (tag.GetName() == _T("BODY"))
    {
        m_Parser->StopParsing();
        return false;
    }

    if (tag.HasParam(_T("HTTP-EQUIV")) &&
        tag.GetParam(_T("HTTP-EQUIV")).IsSameAs(_T("Content-Type"), false) &&
        tag.HasParam(_T("CONTENT")))
    {
        wxString content = tag.GetParam(_T("CONTENT")).Lower();
        if (content.Left(19) == _T("text/html; charset="))
        {
            *m_retval = content.Mid(19);
            m_Parser->StopParsing();
        }
    }
    return false;
}


/*static*/
wxString wxHtmlParser::ExtractCharsetInformation(const wxString& markup)
{
    wxString charset;
    wxMetaTagParser *parser = new wxMetaTagParser();
    if(parser)
    {
        parser->AddTagHandler(new wxMetaTagHandler(&charset));
        parser->Parse(markup);
        delete parser;
    }
    return charset;
}

/* static */
bool
wxHtmlParser::SkipCommentTag(wxString::const_iterator& start,
                             wxString::const_iterator end)
{
    wxASSERT_MSG( *start == '<', _T("should be called on the tag start") );

    wxString::const_iterator p = start;

    // comments begin with "<!--" in HTML 4.0
    if ( end - p < 3 || *++p != '!' || *++p != '-' || *++p != '-' )
    {
        // not a comment at all
        return false;
    }

    // skip the start of the comment tag in any case, if we don't find the
    // closing tag we should ignore broken markup
    start = p;

    // comments end with "--[ \t\r\n]*>", i.e. white space is allowed between
    // comment delimiter and the closing tag character (section 3.2.4 of
    // http://www.w3.org/TR/html401/)
    int dashes = 0;
    while ( ++p < end )
    {
        const wxChar c = *p;

        if ( (c == wxT(' ') || c == wxT('\n') ||
              c == wxT('\r') || c == wxT('\t')) && dashes >= 2 )
        {
            // ignore white space before potential tag end
            continue;
        }

        if ( c == wxT('>') && dashes >= 2 )
        {
            // found end of comment
            start = p;
            break;
        }

        if ( c == wxT('-') )
            dashes++;
        else
            dashes = 0;
    }

    return true;
}

#endif // wxUSE_HTML
