/////////////////////////////////////////////////////////////////////////////
// Name:        dxfrenderer.cpp
// Purpose:     DXF reader and renderer
// Author:      Sandro Sigala
// Modified by:
// Created:     2005-11-10
// RCS-ID:      $Id$
// Copyright:   (c) Sandro Sigala
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/wfstream.h"
#include "wx/txtstrm.h"

#if !wxUSE_GLCANVAS
    #error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild the library"
#endif

#ifdef __DARWIN__
    #include <OpenGL/glu.h>
#else
    #include <GL/glu.h>
#endif

#include <sstream>

#include "dxfrenderer.h"

#include "wx/listimpl.cpp"
WX_DEFINE_LIST(DXFEntityList)
WX_DEFINE_LIST(DXFLayerList)

// Conversion table from AutoCAD ACI colours to RGB values
static const struct { unsigned char r, g, b; } aci_to_rgb[256] = {
/*   0 */ {255, 255, 255},
/*   1 */ {255,   0,   0},
/*   2 */ {255, 255,   0},
/*   3 */ {  0, 255,   0},
/*   4 */ {  0, 255, 255},
/*   5 */ {  0,   0, 255},
/*   6 */ {255,   0, 255},
/*   7 */ {255, 255, 255},
/*   8 */ {128, 128, 128},
/*   9 */ {192, 192, 192},
/*  10 */ {255,   0,   0},
/*  11 */ {255, 127, 127},
/*  12 */ {204,   0,   0},
/*  13 */ {204, 102, 102},
/*  14 */ {153,   0,   0},
/*  15 */ {153,  76,  76},
/*  16 */ {127,   0,   0},
/*  17 */ {127,  63,  63},
/*  18 */ { 76,   0,   0},
/*  19 */ { 76,  38,  38},
/*  20 */ {255,  63,   0},
/*  21 */ {255, 159, 127},
/*  22 */ {204,  51,   0},
/*  23 */ {204, 127, 102},
/*  24 */ {153,  38,   0},
/*  25 */ {153,  95,  76},
/*  26 */ {127,  31,   0},
/*  27 */ {127,  79,  63},
/*  28 */ { 76,  19,   0},
/*  29 */ { 76,  47,  38},
/*  30 */ {255, 127,   0},
/*  31 */ {255, 191, 127},
/*  32 */ {204, 102,   0},
/*  33 */ {204, 153, 102},
/*  34 */ {153,  76,   0},
/*  35 */ {153, 114,  76},
/*  36 */ {127,  63,   0},
/*  37 */ {127,  95,  63},
/*  38 */ { 76,  38,   0},
/*  39 */ { 76,  57,  38},
/*  40 */ {255, 191,   0},
/*  41 */ {255, 223, 127},
/*  42 */ {204, 153,   0},
/*  43 */ {204, 178, 102},
/*  44 */ {153, 114,   0},
/*  45 */ {153, 133,  76},
/*  46 */ {127,  95,   0},
/*  47 */ {127, 111,  63},
/*  48 */ { 76,  57,   0},
/*  49 */ { 76,  66,  38},
/*  50 */ {255, 255,   0},
/*  51 */ {255, 255, 127},
/*  52 */ {204, 204,   0},
/*  53 */ {204, 204, 102},
/*  54 */ {153, 153,   0},
/*  55 */ {153, 153,  76},
/*  56 */ {127, 127,   0},
/*  57 */ {127, 127,  63},
/*  58 */ { 76,  76,   0},
/*  59 */ { 76,  76,  38},
/*  60 */ {191, 255,   0},
/*  61 */ {223, 255, 127},
/*  62 */ {153, 204,   0},
/*  63 */ {178, 204, 102},
/*  64 */ {114, 153,   0},
/*  65 */ {133, 153,  76},
/*  66 */ { 95, 127,   0},
/*  67 */ {111, 127,  63},
/*  68 */ { 57,  76,   0},
/*  69 */ { 66,  76,  38},
/*  70 */ {127, 255,   0},
/*  71 */ {191, 255, 127},
/*  72 */ {102, 204,   0},
/*  73 */ {153, 204, 102},
/*  74 */ { 76, 153,   0},
/*  75 */ {114, 153,  76},
/*  76 */ { 63, 127,   0},
/*  77 */ { 95, 127,  63},
/*  78 */ { 38,  76,   0},
/*  79 */ { 57,  76,  38},
/*  80 */ { 63, 255,   0},
/*  81 */ {159, 255, 127},
/*  82 */ { 51, 204,   0},
/*  83 */ {127, 204, 102},
/*  84 */ { 38, 153,   0},
/*  85 */ { 95, 153,  76},
/*  86 */ { 31, 127,   0},
/*  87 */ { 79, 127,  63},
/*  88 */ { 19,  76,   0},
/*  89 */ { 47,  76,  38},
/*  90 */ {  0, 255,   0},
/*  91 */ {127, 255, 127},
/*  92 */ {  0, 204,   0},
/*  93 */ {102, 204, 102},
/*  94 */ {  0, 153,   0},
/*  95 */ { 76, 153,  76},
/*  96 */ {  0, 127,   0},
/*  97 */ { 63, 127,  63},
/*  98 */ {  0,  76,   0},
/*  99 */ { 38,  76,  38},
/* 100 */ {  0, 255,  63},
/* 101 */ {127, 255, 159},
/* 102 */ {  0, 204,  51},
/* 103 */ {102, 204, 127},
/* 104 */ {  0, 153,  38},
/* 105 */ { 76, 153,  95},
/* 106 */ {  0, 127,  31},
/* 107 */ { 63, 127,  79},
/* 108 */ {  0,  76,  19},
/* 109 */ { 38,  76,  47},
/* 110 */ {  0, 255, 127},
/* 111 */ {127, 255, 191},
/* 112 */ {  0, 204, 102},
/* 113 */ {102, 204, 153},
/* 114 */ {  0, 153,  76},
/* 115 */ { 76, 153, 114},
/* 116 */ {  0, 127,  63},
/* 117 */ { 63, 127,  95},
/* 118 */ {  0,  76,  38},
/* 119 */ { 38,  76,  57},
/* 120 */ {  0, 255, 191},
/* 121 */ {127, 255, 223},
/* 122 */ {  0, 204, 153},
/* 123 */ {102, 204, 178},
/* 124 */ {  0, 153, 114},
/* 125 */ { 76, 153, 133},
/* 126 */ {  0, 127,  95},
/* 127 */ { 63, 127, 111},
/* 128 */ {  0,  76,  57},
/* 129 */ { 38,  76,  66},
/* 130 */ {  0, 255, 255},
/* 131 */ {127, 255, 255},
/* 132 */ {  0, 204, 204},
/* 133 */ {102, 204, 204},
/* 134 */ {  0, 153, 153},
/* 135 */ { 76, 153, 153},
/* 136 */ {  0, 127, 127},
/* 137 */ { 63, 127, 127},
/* 138 */ {  0,  76,  76},
/* 139 */ { 38,  76,  76},
/* 140 */ {  0, 191, 255},
/* 141 */ {127, 223, 255},
/* 142 */ {  0, 153, 204},
/* 143 */ {102, 178, 204},
/* 144 */ {  0, 114, 153},
/* 145 */ { 76, 133, 153},
/* 146 */ {  0,  95, 127},
/* 147 */ { 63, 111, 127},
/* 148 */ {  0,  57,  76},
/* 149 */ { 38,  66,  76},
/* 150 */ {  0, 127, 255},
/* 151 */ {127, 191, 255},
/* 152 */ {  0, 102, 204},
/* 153 */ {102, 153, 204},
/* 154 */ {  0,  76, 153},
/* 155 */ { 76, 114, 153},
/* 156 */ {  0,  63, 127},
/* 157 */ { 63,  95, 127},
/* 158 */ {  0,  38,  76},
/* 159 */ { 38,  57,  76},
/* 160 */ {  0,  63, 255},
/* 161 */ {127, 159, 255},
/* 162 */ {  0,  51, 204},
/* 163 */ {102, 127, 204},
/* 164 */ {  0,  38, 153},
/* 165 */ { 76,  95, 153},
/* 166 */ {  0,  31, 127},
/* 167 */ { 63,  79, 127},
/* 168 */ {  0,  19,  76},
/* 169 */ { 38,  47,  76},
/* 170 */ {  0,   0, 255},
/* 171 */ {127, 127, 255},
/* 172 */ {  0,   0, 204},
/* 173 */ {102, 102, 204},
/* 174 */ {  0,   0, 153},
/* 175 */ { 76,  76, 153},
/* 176 */ {  0,   0, 127},
/* 177 */ { 63,  63, 127},
/* 178 */ {  0,   0,  76},
/* 179 */ { 38,  38,  76},
/* 180 */ { 63,   0, 255},
/* 181 */ {159, 127, 255},
/* 182 */ { 51,   0, 204},
/* 183 */ {127, 102, 204},
/* 184 */ { 38,   0, 153},
/* 185 */ { 95,  76, 153},
/* 186 */ { 31,   0, 127},
/* 187 */ { 79,  63, 127},
/* 188 */ { 19,   0,  76},
/* 189 */ { 47,  38,  76},
/* 190 */ {127,   0, 255},
/* 191 */ {191, 127, 255},
/* 192 */ {102,   0, 204},
/* 193 */ {153, 102, 204},
/* 194 */ { 76,   0, 153},
/* 195 */ {114,  76, 153},
/* 196 */ { 63,   0, 127},
/* 197 */ { 95,  63, 127},
/* 198 */ { 38,   0,  76},
/* 199 */ { 57,  38,  76},
/* 200 */ {191,   0, 255},
/* 201 */ {223, 127, 255},
/* 202 */ {153,   0, 204},
/* 203 */ {178, 102, 204},
/* 204 */ {114,   0, 153},
/* 205 */ {133,  76, 153},
/* 206 */ { 95,   0, 127},
/* 207 */ {111,  63, 127},
/* 208 */ { 57,   0,  76},
/* 209 */ { 66,  38,  76},
/* 210 */ {255,   0, 255},
/* 211 */ {255, 127, 255},
/* 212 */ {204,   0, 204},
/* 213 */ {204, 102, 204},
/* 214 */ {153,   0, 153},
/* 215 */ {153,  76, 153},
/* 216 */ {127,   0, 127},
/* 217 */ {127,  63, 127},
/* 218 */ { 76,   0,  76},
/* 219 */ { 76,  38,  76},
/* 220 */ {255,   0, 191},
/* 221 */ {255, 127, 223},
/* 222 */ {204,   0, 153},
/* 223 */ {204, 102, 178},
/* 224 */ {153,   0, 114},
/* 225 */ {153,  76, 133},
/* 226 */ {127,   0,  95},
/* 227 */ {127,  63, 111},
/* 228 */ { 76,   0,  57},
/* 229 */ { 76,  38,  66},
/* 230 */ {255,   0, 127},
/* 231 */ {255, 127, 191},
/* 232 */ {204,   0, 102},
/* 233 */ {204, 102, 153},
/* 234 */ {153,   0,  76},
/* 235 */ {153,  76, 114},
/* 236 */ {127,   0,  63},
/* 237 */ {127,  63,  95},
/* 238 */ { 76,   0,  38},
/* 239 */ { 76,  38,  57},
/* 240 */ {255,   0,  63},
/* 241 */ {255, 127, 159},
/* 242 */ {204,   0,  51},
/* 243 */ {204, 102, 127},
/* 244 */ {153,   0,  38},
/* 245 */ {153,  76,  95},
/* 246 */ {127,   0,  31},
/* 247 */ {127,  63,  79},
/* 248 */ { 76,   0,  19},
/* 249 */ { 76,  38,  47},
/* 250 */ { 51,  51,  51},
/* 251 */ { 91,  91,  91},
/* 252 */ {132, 132, 132},
/* 253 */ {173, 173, 173},
/* 254 */ {214, 214, 214},
/* 255 */ {255, 255, 255}
};

inline DXFVector Cross(const DXFVector& v1, const DXFVector& v2)
{
    return DXFVector(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x);
}

void DXFFace::CalculateNormal()
{
    DXFVector v01, v02;
    v01.x = v0.x - v1.x;
    v01.y = v0.y - v1.y;
    v01.z = v0.z - v1.z;
    v02.x = v0.x - v2.x;
    v02.y = v0.y - v2.y;
    v02.z = v0.z - v2.z;
    n = Cross(v01, v02);
    float mod = sqrt(n.x*n.x + n.y*n.y + n.z*n.z);
    n.x /= mod;
    n.y /= mod;
    n.z /= mod;
}

// convert an AutoCAD ACI colour to wxWidgets RGB colour
inline wxColour ACIColourToRGB(int col)
{
    wxASSERT(col >= 0 && col <= 255);
    return wxColour(aci_to_rgb[col].r, aci_to_rgb[col].g, aci_to_rgb[col].b);
}

// DXFReader constructor
DXFRenderer::DXFRenderer()
{
    m_loaded = false;
}

// DXFReader destructor
DXFRenderer::~DXFRenderer()
{
    Clear();
}

// deallocate all the dynamic data
void DXFRenderer::Clear()
{
    m_loaded = false;
    {
        for (DXFLayerList::compatibility_iterator node = m_layers.GetFirst(); node; node = node->GetNext())
        {
            DXFLayer *current = node->GetData();
            delete current;
        }
    }
    m_layers.Clear();
    {
        for (DXFEntityList::compatibility_iterator node = m_entities.GetFirst(); node; node = node->GetNext())
        {
            DXFEntity *current = node->GetData();
            delete current;
        }
        m_entities.Clear();
    }
}

int DXFRenderer::GetLayerColour(const wxString& layer) const
{
    for (DXFLayerList::compatibility_iterator node = m_layers.GetFirst(); node; node = node->GetNext())
    {
        DXFLayer *current = node->GetData();
        if (current->name == layer)
            return current->colour;
    }
    return 7;   // white
}

// read two sequential lines
inline void GetLines(wxTextInputStream& text, wxString& line1, wxString& line2)
{
    line1 = text.ReadLine().Trim().Trim(false);
    line2 = text.ReadLine().Trim().Trim(false);
}

// parse header section: just skip everything
bool DXFRenderer::ParseHeader(wxInputStream& stream)
{
    wxTextInputStream text(stream);
    wxString line1, line2;
    while (stream.CanRead())
    {
        GetLines(text, line1, line2);
        if (line1 == wxT("0") && line2 == wxT("ENDSEC"))
            return true;
    }
    return false;
}

// parse tables section: save layers name and colour
bool DXFRenderer::ParseTables(wxInputStream& stream)
{
    wxTextInputStream text(stream);
    wxString line1, line2;
    bool inlayer=false;
    DXFLayer layer;
    while (stream.CanRead())
    {
        GetLines(text, line1, line2);
        if (line1 == wxT("0") && inlayer)
        {
            // flush layer
            if (!layer.name.IsEmpty() && layer.colour != -1)
            {
                DXFLayer *p = new DXFLayer;
                p->name = layer.name;
                p->colour = layer.colour;
                m_layers.Append(p);
            }
            layer = DXFLayer();
            inlayer = false;
        }
        if (line1 == wxT("0") && line2 == wxT("ENDSEC"))
            return true;
        else if (line1 == wxT("0") && line2 == wxT("LAYER"))
            inlayer = true;
        else if (inlayer)
        {
            if (line1 == wxT("2")) // layer name
                layer.name = line2;
            else if (line1 == wxT("62")) // ACI colour
            {
                long l;
                line2.ToLong(&l);
                layer.colour = l;
            }
        }
    }
    return false;
}

// This method is used instead of numStr.ToDouble(d) because the latter
// (wxString::ToDouble()) takes the systems proper locale into account,
// whereas the implementation below works with the default locale.
// (Converting numbers that are formatted in the default locale can fail
//  with system locales that use e.g. the comma as the decimal separator.)
static double ToDouble(const wxString& numStr)
{
    double             d;
    std::string        numStr_(numStr.c_str());
    std::istringstream iss(numStr_);

    iss >> d;

    return d;
}

// parse entities section: save 3DFACE and LINE entities
bool DXFRenderer::ParseEntities(wxInputStream& stream)
{
    wxTextInputStream text(stream);
    wxString line1, line2;
    int state = 0;  // 0: none, 1: 3DFACE, 2: LINE
    DXFVector v[4];
    int colour = -1;
    wxString layer;
    while (stream.CanRead())
    {
        GetLines(text, line1, line2);
        if (line1 == wxT("0") && state > 0)
        {
            // flush entity
            if (state == 1) // 3DFACE
            {
                DXFFace *p = new DXFFace;
                p->v0 = v[0];
                p->v1 = v[1];
                p->v2 = v[2];
                p->v3 = v[3];
                p->CalculateNormal();
                if (colour != -1)
                    p->colour = colour;
                else
                    p->colour = GetLayerColour(layer);
                m_entities.Append(p);
                colour = -1; layer = wxEmptyString;
                v[0] = v[1] = v[2] = v[3] = DXFVector();
                state = 0;
            }
            else if (state == 2) // LINE
            {
                DXFLine *p = new DXFLine;
                p->v0 = v[0];
                p->v1 = v[1];
                if (colour != -1)
                    p->colour = colour;
                else
                    p->colour = GetLayerColour(layer);
                m_entities.Append(p);
                colour = -1; layer = wxEmptyString;
                v[0] = v[1] = v[2] = v[3] = DXFVector();
                state = 0;
            }
        }
        if (line1 == wxT("0") && line2 == wxT("ENDSEC"))
            return true;
        else if (line1 == wxT("0") && line2 == wxT("3DFACE"))
            state = 1;
        else if (line1 == wxT("0") && line2 == wxT("LINE"))
            state = 2;
        else if (state > 0)
        {
            const double d=ToDouble(line2);

            if (line1 == wxT("10"))
                v[0].x = d;
            else if (line1 == wxT("20"))
                v[0].y = d;
            else if (line1 == wxT("30"))
                v[0].z = d;
            else if (line1 == wxT("11"))
                v[1].x = d;
            else if (line1 == wxT("21"))
                v[1].y = d;
            else if (line1 == wxT("31"))
                v[1].z = d;
            else if (line1 == wxT("12"))
                v[2].x = d;
            else if (line1 == wxT("22"))
                v[2].y = d;
            else if (line1 == wxT("32"))
                v[2].z = d;
            else if (line1 == wxT("13"))
                v[3].x = d;
            else if (line1 == wxT("23"))
                v[3].y = d;
            else if (line1 == wxT("33"))
                v[3].z = d;
            else if (line1 == wxT("8"))  // layer
                layer = line2;
            else if (line1 == wxT("62")) // colour
            {
                long l;
                line2.ToLong(&l);
                colour = l;
            }
        }
    }
    return false;
}

// parse and load a DXF file
// currently pretty limited, but knows enought do handle 3DFACEs and LINEs
bool DXFRenderer::Load(wxInputStream& stream)
{
    Clear();
    wxTextInputStream text(stream);

    wxString line1, line2;
    while (stream.CanRead())
    {
        GetLines(text, line1, line2);
        if (line1 == wxT("999")) // comment
            continue;
        else if (line1 == wxT("0") && line2 == wxT("SECTION"))
        {
            GetLines(text, line1, line2);
            if (line1 == wxT("2"))
            {
                if (line2 == wxT("HEADER"))
                {
                    if (!ParseHeader(stream))
                        return false;
                }
                else if (line2 == wxT("TABLES"))
                {
                    if (!ParseTables(stream))
                        return false;
                }
                else if (line2 == wxT("ENTITIES"))
                {
                    if (!ParseEntities(stream))
                        return false;
                }
            }
        }
    }

    NormalizeEntities();
    m_loaded = true;
    return true;
}

inline float mymin(float x, float y) { return x < y ? x : y; }
inline float mymax(float x, float y) { return x > y ? x : y; }

// Scale object boundings to [-5,5]
void DXFRenderer::NormalizeEntities()
{
    // calculate current min and max boundings of object
    DXFVector minv(10e20f, 10e20f, 10e20f);
    DXFVector maxv(-10e20f, -10e20f, -10e20f);
    for (DXFEntityList::compatibility_iterator node = m_entities.GetFirst(); node; node = node->GetNext())
    {
        DXFEntity *p = node->GetData();
        if (p->type == DXFEntity::Line)
        {
            DXFLine *line = (DXFLine *)p;
            const DXFVector *v[2] = { &line->v0, &line->v1 };
            for (int i = 0; i < 2; ++i)
            {
                minv.x = mymin(v[i]->x, minv.x);
                minv.y = mymin(v[i]->y, minv.y);
                minv.z = mymin(v[i]->z, minv.z);
                maxv.x = mymax(v[i]->x, maxv.x);
                maxv.y = mymax(v[i]->y, maxv.y);
                maxv.z = mymax(v[i]->z, maxv.z);
            }
        } else if (p->type == DXFEntity::Face)
        {
            DXFFace *face = (DXFFace *)p;
            const DXFVector *v[4] = { &face->v0, &face->v1, &face->v2, &face->v3 };
            for (int i = 0; i < 4; ++i)
            {
                minv.x = mymin(v[i]->x, minv.x);
                minv.y = mymin(v[i]->y, minv.y);
                minv.z = mymin(v[i]->z, minv.z);
                maxv.x = mymax(v[i]->x, maxv.x);
                maxv.y = mymax(v[i]->y, maxv.y);
                maxv.z = mymax(v[i]->z, maxv.z);
            }
       }
    }

    // rescale object down to [-5,5]
    DXFVector span(maxv.x - minv.x, maxv.y - minv.y, maxv.z - minv.z);
    float factor = mymin(mymin(10.0f / span.x, 10.0f / span.y), 10.0f / span.z);
    for (DXFEntityList::compatibility_iterator node2 = m_entities.GetFirst(); node2; node2 = node2->GetNext())
    {
        DXFEntity *p = node2->GetData();
        if (p->type == DXFEntity::Line)
        {
            DXFLine *line = (DXFLine *)p;
            DXFVector *v[2] = { &line->v0, &line->v1 };
            for (int i = 0; i < 2; ++i)
            {
                v[i]->x -= minv.x + span.x/2; v[i]->x *= factor;
                v[i]->y -= minv.y + span.y/2; v[i]->y *= factor;
                v[i]->z -= minv.z + span.z/2; v[i]->z *= factor;
            }
        } else if (p->type == DXFEntity::Face)
        {
            DXFFace *face = (DXFFace *)p;
            DXFVector *v[4] = { &face->v0, &face->v1, &face->v2, &face->v3 };
            for (int i = 0; i < 4; ++i)
            {
                v[i]->x -= minv.x + span.x/2; v[i]->x *= factor;
                v[i]->y -= minv.y + span.y/2; v[i]->y *= factor;
                v[i]->z -= minv.z + span.z/2; v[i]->z *= factor;
            }
       }
    }
}

// OpenGL renderer for DXF entities
void DXFRenderer::Render() const
{
    if (!m_loaded)
        return;

    for (DXFEntityList::compatibility_iterator node = m_entities.GetFirst(); node; node = node->GetNext())
    {
        DXFEntity *p = node->GetData();
        wxColour c = ACIColourToRGB(p->colour);
        if (p->type == DXFEntity::Line)
        {
            DXFLine *line = (DXFLine *)p;
            glBegin(GL_LINES);
            glColor3f(c.Red()/255.0,c.Green()/255.0,c.Blue()/255.0);
            glVertex3f(line->v0.x, line->v0.y, line->v0.z);
            glVertex3f(line->v1.x, line->v1.y, line->v1.z);
            glEnd();
        }
        else if (p->type == DXFEntity::Face)
        {
            DXFFace *face = (DXFFace *)p;
            glBegin(GL_TRIANGLES);
            glColor3f(c.Red()/255.0,c.Green()/255.0,c.Blue()/255.0);
            glNormal3f(face->n.x, face->n.y, face->n.z);
            glVertex3f(face->v0.x, face->v0.y, face->v0.z);
            glVertex3f(face->v1.x, face->v1.y, face->v1.z);
            glVertex3f(face->v2.x, face->v2.y, face->v2.z);
            glEnd();
        }
    }
}
