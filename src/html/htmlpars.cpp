/////////////////////////////////////////////////////////////////////////////
// Name:        htmlpars.cpp
// Purpose:     wxHtmlParser class (generic parser)
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation
#endif

#include "wx/wxprec.h"

#include "wx/defs.h"
#if wxUSE_HTML && wxUSE_STREAMS

#ifdef __BORDLANDC__
#pragma hdrstop
#endif

#ifndef WXPRECOMP
#include "wx/wx.h"
#endif

#include "wx/tokenzr.h"
#include "wx/wfstream.h"
#include "wx/url.h"
#include "wx/fontmap.h"
#include "wx/html/htmldefs.h"
#include "wx/html/htmlpars.h"



//-----------------------------------------------------------------------------
// wxHtmlParser
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxHtmlParser,wxObject)

wxHtmlParser::wxHtmlParser() 
    : wxObject(), m_Cache(NULL), m_HandlersHash(wxKEY_STRING), 
      m_FS(NULL), m_HandlersStack(NULL)
{
    m_entitiesParser = new wxHtmlEntitiesParser;
}

wxHtmlParser::~wxHtmlParser()
{
    delete m_HandlersStack;
    m_HandlersHash.Clear();
    m_HandlersList.DeleteContents(TRUE);
    m_HandlersList.Clear();
    delete m_entitiesParser;
}

wxObject* wxHtmlParser::Parse(const wxString& source)
{
    wxObject *result;

    InitParser(source);
    DoParsing();
    result = GetProduct();
    DoneParser();
    return result;
}

void wxHtmlParser::InitParser(const wxString& source)
{
    SetSource(source);
}

void wxHtmlParser::DoneParser()
{
    delete m_Cache;
    m_Cache = NULL;
}

void wxHtmlParser::SetSource(const wxString& src)
{
    m_Source = src;
    delete m_Cache;
    m_Cache = new wxHtmlTagsCache(m_Source);
}

void wxHtmlParser::DoParsing(int begin_pos, int end_pos)
{
    if (end_pos <= begin_pos) return;

    char c;
    char *temp = new char[end_pos - begin_pos + 1];
    int i;
    int templen;

    templen = 0;
    i = begin_pos;

    while (i < end_pos) 
    {
        c = m_Source[(unsigned int) i];

        // continue building word:
        if (c != '<') 
	    {
            temp[templen++] = c;
            i++;
        }

        else if (c == '<')  
	    {
            wxHtmlTag tag(m_Source, i, end_pos, m_Cache, m_entitiesParser);

            if (templen) 
	        {
                temp[templen] = 0;
                AddText(temp);
                templen = 0;
            }
            AddTag(tag);
            if (tag.HasEnding()) i = tag.GetEndPos2();
            else i = tag.GetBeginPos();
        }
    }

    if (templen) 
    { // last word of block :-(
        temp[templen] = 0;
        AddText(temp);
    }
    delete[] temp;
}

void wxHtmlParser::AddTag(const wxHtmlTag& tag)
{
    wxHtmlTagHandler *h;
    bool inner = FALSE;

    h = (wxHtmlTagHandler*) m_HandlersHash.Get(tag.GetName());
    if (h)
        inner = h->HandleTag(tag);
    if (!inner) 
    {
        if (tag.HasEnding())
            DoParsing(tag.GetBeginPos(), tag.GetEndPos1());
    }
}

void wxHtmlParser::AddTagHandler(wxHtmlTagHandler *handler)
{
    wxString s(handler->GetSupportedTags());
    wxStringTokenizer tokenizer(s, ", ");

    while (tokenizer.HasMoreTokens())
        m_HandlersHash.Put(tokenizer.NextToken(), handler);

    if (m_HandlersList.IndexOf(handler) == wxNOT_FOUND)
        m_HandlersList.Append(handler);

    handler->SetParser(this);
}

void wxHtmlParser::PushTagHandler(wxHtmlTagHandler *handler, wxString tags)
{
    wxStringTokenizer tokenizer(tags, ", ");
    wxString key;

    if (m_HandlersStack == NULL) 
    {
        m_HandlersStack = new wxList;
        m_HandlersStack->DeleteContents(TRUE);
    }

    m_HandlersStack->Insert(new wxHashTable(m_HandlersHash));

    while (tokenizer.HasMoreTokens()) 
    {
        key = tokenizer.NextToken();
        m_HandlersHash.Delete(key);
        m_HandlersHash.Put(key, handler);
    }
}

void wxHtmlParser::PopTagHandler()
{
    wxNode *first;
    
    if (m_HandlersStack == NULL || 
        (first = m_HandlersStack->GetFirst()) == NULL) 
    {
        wxLogWarning(_("Warning: attempt to remove HTML tag handler from empty stack."));
        return;
    }
    m_HandlersHash = *((wxHashTable*) first->GetData());
    m_HandlersStack->DeleteNode(first);
}

//-----------------------------------------------------------------------------
// wxHtmlTagHandler
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxHtmlTagHandler,wxObject)


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
    delete m_conv;
}

void wxHtmlEntitiesParser::SetEncoding(wxFontEncoding encoding)
{
#if wxUSE_WCHAR_T && !wxUSE_UNICODE
    if (encoding == m_encoding) return;
    delete m_conv;
    m_conv = NULL;
    m_encoding = encoding;
    if (m_encoding != wxFONTENCODING_SYSTEM)
        m_conv = new wxCSConv(wxFontMapper::GetEncodingName(m_encoding));
#endif
}

wxString wxHtmlEntitiesParser::Parse(const wxString& input)
{
    const wxChar *c, *last;
    const wxChar *in_str = input.c_str();
    wxString output;
    
    for (c = in_str, last = in_str; *c != wxT('\0'); c++)
    {
        if (*c == wxT('&'))
        {
            if (c - last > 0)
                output.append(last, c - last);
            if (++c == wxT('\0')) break;
            wxString entity;
            const wxChar *ent_s = c;
            for (; (*c >= wxT('a') && *c <= wxT('z')) ||
                   (*c >= wxT('A') && *c <= wxT('Z')) ||
                   (*c >= wxT('0') && *c <= wxT('9')) ||
                   *c == wxT('_') || *c == wxT('#'); c++) {}
            entity.append(ent_s, c - ent_s);
            if (*c == wxT(';')) c++;
            output << GetEntityChar(entity);
            last = c;
        }
    }
    if (*last != wxT('\0'))
        output.append(last);
    return output;
}

struct wxHtmlEntityInfo
{
    const wxChar *name;
    unsigned code;
};

static int compar_entity(const void *key, const void *item)
{
    return wxStrcmp((wxChar*)key, ((wxHtmlEntityInfo*)item)->name);
}

wxChar wxHtmlEntitiesParser::GetCharForCode(unsigned code)
{
#if wxUSE_UNICODE
    return (wxChar)code;
#elif wxUSE_WCHAR_T
    char buf[2];
    wchar_t wbuf[2];
    wbuf[0] = (wchar_t)code;
    wbuf[1] = 0;
    wxMBConv *conv = m_conv ? m_conv : &wxConvLocal;
    if (conv->WC2MB(buf, wbuf, 1) == (size_t)-1)
        return '?';
    return buf[0];
#else
    return (code < 256) ? (wxChar)code : '?';
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
        static wxHtmlEntityInfo substitutions[] = {
            { wxT("AElig"),198 },
            { wxT("Aacute"),193 },
            { wxT("Acirc"),194 },
            { wxT("Agrave"),192 },
            { wxT("Alpha"),913 },
            { wxT("Aring"),197 },
            { wxT("Atilde"),195 },
            { wxT("Auml"),196 },
            { wxT("Beta"),914 },
            { wxT("Ccedil"),199 },
            { wxT("Chi"),935 },
            { wxT("Dagger"),8225 },
            { wxT("Delta"),916 },
            { wxT("ETH"),208 },
            { wxT("Eacute"),201 },
            { wxT("Ecirc"),202 },
            { wxT("Egrave"),200 },
            { wxT("Epsilon"),917 },
            { wxT("Eta"),919 },
            { wxT("Euml"),203 },
            { wxT("Gamma"),915 },
            { wxT("Iacute"),205 },
            { wxT("Icirc"),206 },
            { wxT("Igrave"),204 },
            { wxT("Iota"),921 },
            { wxT("Iuml"),207 },
            { wxT("Kappa"),922 },
            { wxT("Lambda"),923 },
            { wxT("Mu"),924 },
            { wxT("Ntilde"),209 },
            { wxT("Nu"),925 },
            { wxT("OElig"),338 },
            { wxT("Oacute"),211 },
            { wxT("Ocirc"),212 },
            { wxT("Ograve"),210 },
            { wxT("Omega"),937 },
            { wxT("Omicron"),927 },
            { wxT("Oslash"),216 },
            { wxT("Otilde"),213 },
            { wxT("Ouml"),214 },
            { wxT("Phi"),934 },
            { wxT("Pi"),928 },
            { wxT("Prime"),8243 },
            { wxT("Psi"),936 },
            { wxT("Rho"),929 },
            { wxT("Scaron"),352 },
            { wxT("Sigma"),931 },
            { wxT("THORN"),222 },
            { wxT("Tau"),932 },
            { wxT("Theta"),920 },
            { wxT("Uacute"),218 },
            { wxT("Ucirc"),219 },
            { wxT("Ugrave"),217 },
            { wxT("Upsilon"),933 },
            { wxT("Uuml"),220 },
            { wxT("Xi"),926 },
            { wxT("Yacute"),221 },
            { wxT("Yuml"),376 },
            { wxT("Zeta"),918 },
            { wxT("aacute"),225 },
            { wxT("acirc"),226 },
            { wxT("acute"),180 },
            { wxT("aelig"),230 },
            { wxT("agrave"),224 },
            { wxT("alefsym"),8501 },
            { wxT("alpha"),945 },
            { wxT("amp"),38 },
            { wxT("and"),8743 },
            { wxT("ang"),8736 },
            { wxT("aring"),229 },
            { wxT("asymp"),8776 },
            { wxT("atilde"),227 },
            { wxT("auml"),228 },
            { wxT("bdquo"),8222 },
            { wxT("beta"),946 },
            { wxT("brvbar"),166 },
            { wxT("bull"),8226 },
            { wxT("cap"),8745 },
            { wxT("ccedil"),231 },
            { wxT("cedil"),184 },
            { wxT("cent"),162 },
            { wxT("chi"),967 },
            { wxT("circ"),710 },
            { wxT("clubs"),9827 },
            { wxT("cong"),8773 },
            { wxT("copy"),169 },
            { wxT("crarr"),8629 },
            { wxT("cup"),8746 },
            { wxT("curren"),164 },
            { wxT("dArr"),8659 },
            { wxT("dagger"),8224 },
            { wxT("darr"),8595 },
            { wxT("deg"),176 },
            { wxT("delta"),948 },
            { wxT("diams"),9830 },
            { wxT("divide"),247 },
            { wxT("eacute"),233 },
            { wxT("ecirc"),234 },
            { wxT("egrave"),232 },
            { wxT("empty"),8709 },
            { wxT("emsp"),8195 },
            { wxT("ensp"),8194 },
            { wxT("epsilon"),949 },
            { wxT("equiv"),8801 },
            { wxT("eta"),951 },
            { wxT("eth"),240 },
            { wxT("euml"),235 },
            { wxT("euro"),8364 },
            { wxT("exist"),8707 },
            { wxT("fnof"),402 },
            { wxT("forall"),8704 },
            { wxT("frac12"),189 },
            { wxT("frac14"),188 },
            { wxT("frac34"),190 },
            { wxT("frasl"),8260 },
            { wxT("gamma"),947 },
            { wxT("ge"),8805 },
            { wxT("gt"),62 },
            { wxT("hArr"),8660 },
            { wxT("harr"),8596 },
            { wxT("hearts"),9829 },
            { wxT("hellip"),8230 },
            { wxT("iacute"),237 },
            { wxT("icirc"),238 },
            { wxT("iexcl"),161 },
            { wxT("igrave"),236 },
            { wxT("image"),8465 },
            { wxT("infin"),8734 },
            { wxT("int"),8747 },
            { wxT("iota"),953 },
            { wxT("iquest"),191 },
            { wxT("isin"),8712 },
            { wxT("iuml"),239 },
            { wxT("kappa"),954 },
            { wxT("lArr"),8656 },
            { wxT("lambda"),955 },
            { wxT("lang"),9001 },
            { wxT("laquo"),171 },
            { wxT("larr"),8592 },
            { wxT("lceil"),8968 },
            { wxT("ldquo"),8220 },
            { wxT("le"),8804 },
            { wxT("lfloor"),8970 },
            { wxT("lowast"),8727 },
            { wxT("loz"),9674 },
            { wxT("lrm"),8206 },
            { wxT("lsaquo"),8249 },
            { wxT("lsquo"),8216 },
            { wxT("lt"),60 },
            { wxT("macr"),175 },
            { wxT("mdash"),8212 },
            { wxT("micro"),181 },
            { wxT("middot"),183 },
            { wxT("minus"),8722 },
            { wxT("mu"),956 },
            { wxT("nabla"),8711 },
            { wxT("nbsp"),160 },
            { wxT("ndash"),8211 },
            { wxT("ne"),8800 },
            { wxT("ni"),8715 },
            { wxT("not"),172 },
            { wxT("notin"),8713 },
            { wxT("nsub"),8836 },
            { wxT("ntilde"),241 },
            { wxT("nu"),957 },
            { wxT("oacute"),243 },
            { wxT("ocirc"),244 },
            { wxT("oelig"),339 },
            { wxT("ograve"),242 },
            { wxT("oline"),8254 },
            { wxT("omega"),969 },
            { wxT("omicron"),959 },
            { wxT("oplus"),8853 },
            { wxT("or"),8744 },
            { wxT("ordf"),170 },
            { wxT("ordm"),186 },
            { wxT("oslash"),248 },
            { wxT("otilde"),245 },
            { wxT("otimes"),8855 },
            { wxT("ouml"),246 },
            { wxT("para"),182 },
            { wxT("part"),8706 },
            { wxT("permil"),8240 },
            { wxT("perp"),8869 },
            { wxT("phi"),966 },
            { wxT("pi"),960 },
            { wxT("piv"),982 },
            { wxT("plusmn"),177 },
            { wxT("pound"),163 },
            { wxT("prime"),8242 },
            { wxT("prod"),8719 },
            { wxT("prop"),8733 },
            { wxT("psi"),968 },
            { wxT("quot"),34 },
            { wxT("rArr"),8658 },
            { wxT("radic"),8730 },
            { wxT("rang"),9002 },
            { wxT("raquo"),187 },
            { wxT("rarr"),8594 },
            { wxT("rceil"),8969 },
            { wxT("rdquo"),8221 },
            { wxT("real"),8476 },
            { wxT("reg"),174 },
            { wxT("rfloor"),8971 },
            { wxT("rho"),961 },
            { wxT("rlm"),8207 },
            { wxT("rsaquo"),8250 },
            { wxT("rsquo"),8217 },
            { wxT("sbquo"),8218 },
            { wxT("scaron"),353 },
            { wxT("sdot"),8901 },
            { wxT("sect"),167 },
            { wxT("shy"),173 },
            { wxT("sigma"),963 },
            { wxT("sigmaf"),962 },
            { wxT("sim"),8764 },
            { wxT("spades"),9824 },
            { wxT("sub"),8834 },
            { wxT("sube"),8838 },
            { wxT("sum"),8721 },
            { wxT("sup"),8835 },
            { wxT("sup1"),185 },
            { wxT("sup2"),178 },
            { wxT("sup3"),179 },
            { wxT("supe"),8839 },
            { wxT("szlig"),223 },
            { wxT("tau"),964 },
            { wxT("there4"),8756 },
            { wxT("theta"),952 },
            { wxT("thetasym"),977 },
            { wxT("thinsp"),8201 },
            { wxT("thorn"),254 },
            { wxT("tilde"),732 },
            { wxT("times"),215 },
            { wxT("trade"),8482 },
            { wxT("uArr"),8657 },
            { wxT("uacute"),250 },
            { wxT("uarr"),8593 },
            { wxT("ucirc"),251 },
            { wxT("ugrave"),249 },
            { wxT("uml"),168 },
            { wxT("upsih"),978 },
            { wxT("upsilon"),965 },
            { wxT("uuml"),252 },
            { wxT("weierp"),8472 },
            { wxT("xi"),958 },
            { wxT("yacute"),253 },
            { wxT("yen"),165 },
            { wxT("yuml"),255 },
            { wxT("zeta"),950 },
            { wxT("zwj"),8205 },
            { wxT("zwnj"),8204 },
            {NULL, 0}};
        static size_t substitutions_cnt = 0;
        
        if (substitutions_cnt == 0)
            while (substitutions[substitutions_cnt].code != 0)
                substitutions_cnt++;

        wxHtmlEntityInfo *info;
        info = (wxHtmlEntityInfo*) bsearch(entity.c_str(), substitutions, 
                                           substitutions_cnt,
                                           sizeof(wxHtmlEntityInfo),
                                           compar_entity);
        if (info)
            code = info->code;
    }
    
    if (code == 0)
        return wxT('?');
    else
        return GetCharForCode(code);
}

#endif
