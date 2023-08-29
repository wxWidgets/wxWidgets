/////////////////////////////////////////////////////////////////////////////
// Name:        oglstuff.h
// Purpose:     OpenGL manager for pyramid sample
// Author:      Manuel Martin
// Created:     2015/01/31
// Copyright:   (c) 2015 Manuel Martin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef OGLSTUFF_H
#define OGLSTUFF_H

#include <string>
#include <vector>

#include "mathstuff.h"

// This file allows access to OpenGL functions used in this sample.
#include "oglpfuncs.h"

/*
  ************  NOTES  *******************************************************
  This is not an OGL tutorial, I mean, this is just a sample of how OGL stuff
  may be arranged. I tried to keep it simple.
  It's likely that the novice to OGL reads this sample, so here are some brief
  notes:
  * Starting with OpenGL >= 2.0 the user defines some special programs that are
    going to be executed in the GPU, not in the CPU. These programs are called
    "Shaders". Since OGL >= 3.2, and if a "Core Profile" context is set, the use
    of shaders is the only way to make an OGL application.
  * A program consists (at least) of a 'vertex shader' who operates on the
    vertices that define the primitive (a triangle, a line, etc) and a 'fragment
    shader' which deals with fragments (points) interpolated from the previously
    processed vertices in the vertex shader.
  * Shaders must be compiled and linked, both operations are done on the GPU.
  * Shaders are written in the GLSL language, that looks like C, but it isn't C.
  * Data (vertices, textures) are stored in GPU memory, so they don't need to be
    loaded each time the scene must be redrawn.
  * Rotations and translations are matrix operations that the GPU may do in the
    [vertex]shaders. The user must define the needed matrices.
  * A vertex shader defines special vars ("attributes") used for reading the
    data from the GPU buffers. Other special vars ("uniforms") are used for
    values that don't change with each vertex (i.e. the matrices).
  * The relationship between a data buffer and the input attributes in a vertex
    shader can be saved in a "Vertex Array Object" (VAO).

  I use several classes for typical OGL jobs: Shaders, Data, Camera.
  Because different GPU cards may behave on their own, I also use very often
  an error-helper. It will inform of the point where something went wrong.

  I decided to keep apart all of this from wxWidgets. You won't find anything
  related to wxWidgets in the oglstuff[.h][.cpp] files.
  That's why I use std::vector and std::string instead of those provided by wx.
*/


// Define our own GL errors
enum
{
    myoglERR_CLEAR = 0,
    myoglERR_JUSTLOG,

    myoglERR_SHADERCREATE,
    myoglERR_SHADERCOMPILE,
    myoglERR_SHADERLINK,
    myoglERR_SHADERLOCATION,

    myoglERR_BUFFER,
    myoglERR_TEXTIMAGE,

    myoglERR_DRAWING_TRI,
    myoglERR_DRAWING_STR
};

// Used to handle GL errors in other part of the app.
typedef void myOGLErrHandler(int err, int glerr, const GLchar* glMsg);

// For shader attributes
struct shaVars
{
    GLuint loc; //The attribute "location", some kind of index in the shader
    std::string name; //The name of the attribute
};

typedef std::vector<shaVars> shaVars_v;

// For shader code
struct shaShas
{
    GLuint shaId;
    GLenum typeSha; //The type of shader
    const GLchar* scode; //The nullptr terminated GLSL code
};

typedef std::vector<shaShas> shaShas_v;


//-----------------------------------------------------------------------------
// This object builds a GPU program by joining several shaders.
class myOGLShaders
{
public:
    myOGLShaders();
    ~myOGLShaders();

    void Init();
    bool Use();
    void StopUse();
    void CleanUp();

    void AddCode(const GLchar* shaString, GLenum shaType);
    void AddAttrib(const std::string& name);
    void AddUnif(const std::string& name);
    GLuint GetAttribLoc(const std::string& name);
    GLuint GetUnifLoc(const std::string& name);
    // Disable generic vertex attribute array
    void DisableGenericVAA();

private:
    void SetAttribLocations();
    bool AskUnifLocations();
    bool Compile(GLuint shaId);
    bool LinkProg(GLuint proId);

    shaVars_v m_shaAttrib; // 'attributes' names and locations
    shaVars_v m_shaUnif; // 'uniforms' names and locations
    shaShas_v m_shaCode; // shaders code and their types
    GLuint m_proId; // program Id

    bool m_SHAinitializated;
};

//-----------------------------------------------------------------------------
// A "point light"
class myLight
{
public:
    myLight() {}
    ~myLight() {}

    void Set(const myVec3& position, GLfloat intensity,
             GLfloat R, GLfloat G, GLfloat B);
    // Return position and intensity
    const GLfloat* GetFLightPos() const
        { return m_PosAndIntensisty; }
    // Return colour
    const GLfloat* GetFLightColour() const
        { return m_Colour; }

private:
    // Light position and intensity
    GLfloat m_PosAndIntensisty[4];
    // Light colour
    GLfloat m_Colour[3];
};

//-----------------------------------------------------------------------------
// An object for triangles
class myOGLTriangles
{
public:
    myOGLTriangles();
    ~myOGLTriangles();

    // Clean up
    void Clear();
    // Load data into the GPU
    void SetBuffers(myOGLShaders* theShader, GLsizei nuPoints, GLsizei nuTriangs,
                    const GLfloat* vert, const GLfloat* colo,
                    const GLfloat* norm, const GLushort* indices);

    //Draw the triangles
    void Draw(const GLfloat* unifMvp, const GLfloat* unifToVw,
              const myLight* theLight);

private:
    GLsizei m_nuTriangs;
    // Buffers ids
    GLuint m_bufVertId;
    GLuint m_bufColNorId;
    GLuint m_bufIndexId;
    // Vertex Arrays Object
    GLuint m_triangVAO;
    // GPU Program used to draw the triangles
    myOGLShaders* m_triangShaders;
};

//-----------------------------------------------------------------------------
// An object for strings
class myOGLString
{
public:
    myOGLString();
    ~myOGLString();

    // Clean up
    void Clear();
    // Load data into the GPU
    void SetStringWithVerts(myOGLShaders* theShader,
                             const unsigned char* tImage, int tWidth, int tHeigh,
                             const GLfloat* vert, const GLfloat* norm);
    // Draw the string
    void Draw(const GLfloat* unifMvp, const GLfloat* unifToVw,
              const myLight* theLight);


private:
    GLuint m_bufPosId;    // Buffer id
    GLuint m_stringVAO;   // Vertex Arrays Object
    GLuint m_textureUnit; // The context unit
    GLuint m_textureId;   // Texture name
    // GPU Program used to draw the texture
    myOGLShaders* m_stringShaders;
};

//-----------------------------------------------------------------------------
// An object for the immutable string
class myOGLImmutString : public myOGLString
{
public:
    myOGLImmutString(){}
    ~myOGLImmutString(){}

    // Load data into the GPU
    void SetImmutString(myOGLShaders* theShader,
                        const unsigned char* tImage, int tWidth, int tHeigh);
    // Update orthogonal projection matrix
    void SetOrtho(int winWidth, int winHeight);
    // The transformation matrix
    const GLfloat* GetFloatMVP() { return m_fOrtho; }

private:
    double m_dOrtho[16]; // The orthogonal projection matrix
    GLfloat m_fOrtho[16]; // Same as float
};

//-----------------------------------------------------------------------------
// The "camera", or the point of view
class myOGLCamera
{
public:
    myOGLCamera();
    ~myOGLCamera() {}

    // Initial positions
    void InitPositions();
    // When the size of the window changes
    void ViewSizeChanged(int newWidth, int newHeight);
    // The whole transformation matrix
    const GLfloat* GetFloatMVP();
    // The 'To View Space' transformation matrix
    const GLfloat* GetFloatToVw();
    // The camera position
    myVec3 GetPosition() {return m_camPosition;}

    // Simulates a virtual trackball and rotates the 'world'
    void MouseRotation(int fromX, int fromY, int toX, int toY);
    double GetTrackballZ(double x, double y, double r);

    // The used matrices
    double m_dMode[16]; // The model matrix, rotation in this sample
    double m_dView[16]; // The view matrix
    double m_dProj[16]; // The projection matrix
    double m_dMVP[16];  // The whole transform matrix
    double m_dToVw[16]; // The 'to View' transform matrix
    // GLFloat versions. GLdouble is available since OGL 4.0, and we use 3.2
    GLfloat m_fMVP[16];
    GLfloat m_fToVw[16];
private:
    bool m_needMVPUpdate;

    void UpdateMatrices();

    // Coordinates in model space
    myVec3 m_centerOfWorld;
    double m_radiusOfWorld;
    myVec3 m_camPosition;
    myVec3 m_camTarget;
    myVec3 m_camUp;

    // Window size in pixels
    int m_winWidth;
    int m_winHeight;

    // Parameters for the projection
    double m_fov;
    double m_nearD;
    double m_farD;
};

//-----------------------------------------------------------------------------
// General manager
class myOGLManager
{
public:
    myOGLManager(myOGLErrHandler* extErrHnd = nullptr);
    ~myOGLManager();

    // Constants, prototypes and pointers to OGL functions
    static bool Init();
    // Strings describing the current GL connection
    const GLubyte* GetGLVersion();
    const GLubyte* GetGLVendor();
    const GLubyte* GetGLRenderer();

    // Load data into the GPU
    void SetShadersAndTriangles();
    // Load the texture for the string in the pyramid
    void SetStringOnPyr(const unsigned char* strImage, int iWidth, int iHeigh);
    // Load the texture for the immutable string
    void SetImmutableString(const unsigned char* strImage, int iWidth, int iHeigh);

    // For window size change
    void SetViewport(int x, int y, int width, int height);

    void Render(); // Total rendering

    // Action events in OpenGL win coordinates (bottom is y=0)
    void OnMouseButDown(int posX, int posY);
    void OnMouseRotDragging(int posX, int posY);

private:
    // Members
    myOGLShaders   m_TriangShaders;
    myOGLShaders   m_StringShaders;
    myOGLShaders   m_ImmutStringSha;
    myLight        m_Light;
    myOGLCamera    m_Camera;

    myOGLTriangles    m_Triangles;
    myOGLString       m_StringOnPyr;
    myOGLImmutString  m_ImmString;

    // For mouse event
    int m_mousePrevX;
    int m_mousePrevY;
};

#endif //OGLSTUFF_H

