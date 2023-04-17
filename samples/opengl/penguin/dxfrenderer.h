/////////////////////////////////////////////////////////////////////////////
// Name:        dxfrenderer.h
// Purpose:     DXF reader and renderer
// Author:      Sandro Sigala
// Modified by:
// Created:     2005-11-10
// Copyright:   (c) Sandro Sigala
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _DXFRENDERER_H_
#define _DXFRENDERER_H_

#include <vector>

struct DXFVector
{
    DXFVector() { x = y = z = 0.0f; }
    DXFVector(float _x, float _y, float _z) { x = _x; y = _y; z = _z; }
    float x, y, z;
};

struct DXFEntity
{
    enum Type { Line, Face } type;
    int colour;
};

struct DXFLine: public DXFEntity
{
    DXFLine() { type = Line; }
    DXFVector v0;
    DXFVector v1;
};

struct DXFFace: public DXFEntity
{
    DXFFace() { type = Face; }
    void CalculateNormal();
    DXFVector v0;
    DXFVector v1;
    DXFVector v2;
    DXFVector v3;
    DXFVector n;       // normal
};

struct DXFLayer
{
    DXFLayer() { colour = -1; }
    wxString name;
    int colour;
};

class DXFRenderer
{
public:
    DXFRenderer();
    ~DXFRenderer();

    void Clear();
    bool Load(wxInputStream& stream);
    bool IsOk() const { return m_loaded; }
    void Render() const;

private:
    bool ParseHeader(wxInputStream& stream);
    bool ParseTables(wxInputStream& stream);
    bool ParseEntities(wxInputStream& stream);
    int GetLayerColour(const wxString& layer) const;
    void NormalizeEntities();

    bool m_loaded;
    std::vector<DXFLayer> m_layers;
    std::vector<DXFEntity> m_entities;
};

#endif // !_DXFRENDERER_H_
