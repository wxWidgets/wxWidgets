/////////////////////////////////////////////////////////////////////////////
// Name:        oglstuff.cpp
// Purpose:     OpenGL manager for pyramid sample
// Author:      Manuel Martin
// Created:     2015/01/31
// Copyright:   (c) 2015 Manuel Martin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include <cmath>

#include "oglstuff.h"

// External function for GL errors
myOGLErrHandler* externalMyOGLErrHandler = NULL;

// Allow GL errors to be handled in other part of the app.
bool MyOnGLError(int err, const GLchar* glMsg = NULL)
{
    GLenum GLErrorVal = glGetError();

    if ( err == myoglERR_CLEAR )
    {
        // Clear previous errors
        while ( GLErrorVal != GL_NO_ERROR )
            GLErrorVal = glGetError();
        return true;
    }

    if ( (GLErrorVal == GL_NO_ERROR) && (glMsg == NULL) )
        return true;

    if ( externalMyOGLErrHandler )
    {
        // Use the external error message handler. We pass our err-enum value.
        externalMyOGLErrHandler(err, GLErrorVal, glMsg);
    }

    return err == myoglERR_JUSTLOG ? true : false;
}


// We do calculations with 'doubles'. We pass 'GLFloats' to the shaders
// because OGL added 'doubles' since OGL 4.0, and this sample is for 3.2
// Due to asynchronous nature of OGL, we can't trust in the passed matrix
// to be stored by GPU before the passing-function returns. So we don't use
// temporary storage, but dedicated matrices
void SetAsGLFloat4x4(double *matD, GLfloat *matF, int msize)
{
    for (int i = 0; i < msize; i++)
    {
        matF[i] = (GLfloat) matD[i];
    }
}

// ----------------------------------------------------------------------------
// Data for a regular tetrahedron with edge length 200, centered at the origin
// ----------------------------------------------------------------------------
const GLfloat gVerts[] = { 100.0f, -40.8248f, -57.7350f,
                             0.0f, -40.8248f, 115.4704f,
                          -100.0f, -40.8248f, -57.7350f,
                             0.0f, 122.4745f,   0.0f };

// Transparency (to see also inner faces) is in the last triangle only,
// so that glEnable(GL_BLEND) works well
const GLfloat gColours[] = { 0.0f, 1.0f, 0.0f, 1.0f,
                             1.0f, 0.0f, 0.0f, 1.0f,
                             0.0f, 0.0f, 1.0f, 1.0f,
                             1.0f, 1.0f, 0.0f, 0.3f }; //With transparency

// Normals heading outside of the tetrahedron
const GLfloat gNormals[] = { 0.0f,    -1.0f,      0.0f,     /* face 0 1 2 */
                            -0.81650f, 0.33333f,  0.47140f, /* face 1 2 3 */
                             0.0f,     0.33333f, -0.94281f, /* face 2 3 0 */
                             0.81650f, 0.33333f,  0.47140f  /* face 3 0 1 */ };

// Order would be important if we were using face culling
const GLushort gIndices[] = { 0, 1, 2, 3, 0, 1 };


// ----------------------------------------------------------------------------
// Shaders
// ----------------------------------------------------------------------------
// Note: We use GLSL 1.50 which is the minimum starting with OpenGL >= 3.2 (2009)
// Apple supports OpenGL 3.2 since OS X 10.7 "Lion" (2011)

// Vertex shader for the triangles
const GLchar* triangVertexShader =
{
    "#version 150 \n"

    "in vec3 in_Position; \n"
    "in vec4 in_Colour; \n"
    "in vec3 in_Normal; \n"
    "uniform mat4 mMVP; \n"
    "uniform mat4 mToViewSpace; \n"

    "flat out vec4 theColour; \n"
    "flat out vec3 theNormal; \n"
    "out vec3 pointPos; \n"

    "void main(void) \n"
    "{\n"
    "    gl_Position = mMVP * vec4(in_Position, 1.0); \n"
    "    theColour = in_Colour; \n"

    "    // Operations in View Space \n"
    "    vec4 temp4 = mToViewSpace * vec4(in_Position, 1.0); \n"
    "    pointPos = temp4.xyz; \n"
    "    temp4 = mToViewSpace * vec4(in_Normal, 0.0); \n"
    "    theNormal = normalize(temp4.xyz); \n"
    "}\n"
};

// Common function for fragment shaders
const GLchar* illuminationShader =
{
    "#version 150 \n"

    "vec3 Illuminate(in vec4 LiProps, in vec3 LiColour, in vec4 PColour, \n"
    "                in vec3 PNormal, in vec3 PPos) \n"
    "{\n"
    "    // Ambient illumination. Hardcoded \n"
    "    vec3 liAmbient = vec3(0.2, 0.2, 0.2); \n"

    "    // Operations in View Space \n"
    "    vec3 lightDirec = LiProps.xyz - PPos; \n"
    "    float lightDist = length(lightDirec); \n"
    "    // Normalize. Attention: No lightDist > 0 check \n"
    "    lightDirec = lightDirec / lightDist; \n"
    "    // Attenuation. Hardcoded for this sample distances \n"
    "    float attenu = 260.0 / lightDist; \n"
    "    attenu = attenu * attenu; \n"

    "    // Lambertian diffuse illumination \n"
    "    float diffuse = dot(lightDirec, PNormal); \n"
    "    diffuse = max(0.0, diffuse); \n"
    "    vec3 liDiffuse = LiColour * LiProps.w * diffuse * attenu; \n"

    "    // Gaussian specular illumination. Harcoded values again \n"
    "    // We avoid it for interior faces \n"
    "    vec3 viewDir = vec3(0.0, 0.0, 1.0); \n"
    "    vec3 halfDir = normalize(lightDirec + viewDir); \n"
    "    float angleHalf = acos(dot(halfDir, PNormal)); \n"
    "    float exponent = angleHalf / 0.05; \n"
    "    float specular = 0.0; \n"
    "    if (diffuse > 0.0) \n"
    "        specular = exp(-exponent * exponent); \n"

    "    vec3 lightRes = PColour.rgb * ( liAmbient + liDiffuse ); \n"
    "    // Specular colour is quite similar as light colour \n"
    "    lightRes += (0.2 * PColour.xyz + 0.8 * LiColour) * specular * attenu; \n"
    "    lightRes = clamp(lightRes, 0.0, 1.0); \n"

    "     return lightRes; \n"
    "}\n"
};

// Fragment shader for the triangles
const GLchar* triangFragmentShader =
{
    "#version 150 \n"

    "uniform vec4 lightProps; // Position in View space, and intensity \n"
    "uniform vec3 lightColour; \n"

    "flat in vec4 theColour; \n"
    "flat in vec3 theNormal; \n"
    "in vec3 pointPos; \n"

    "out vec4 fragColour; \n"

    "// Declare this function \n"
    "vec3 Illuminate(in vec4 LiProps, in vec3 LiColour, in vec4 PColour, \n"
    "                in vec3 PNormal, in vec3 PPos); \n"

    "void main(void) \n"
    "{\n"
    "    vec3 lightRes = Illuminate(lightProps, lightColour, theColour, \n"
    "                               theNormal, pointPos); \n "

    "    fragColour = vec4(lightRes, theColour.a); \n"
    "}\n"
};

// Vertex shader for strings (textures) with illumination
const GLchar* stringsVertexShader =
{
    "#version 150 \n"

    "in vec3 in_sPosition; \n"
    "in vec3 in_sNormal; \n"
    "in vec2 in_TextPos; \n"
    "uniform mat4 mMVP; \n"
    "uniform mat4 mToViewSpace; \n"

    "flat out vec3 theNormal; \n"
    "out vec3 pointPos; \n"
    "out vec2 textCoord; \n"

    "void main(void) \n"
    "{\n"
    "    gl_Position = mMVP * vec4(in_sPosition, 1.0); \n"
    "    textCoord = in_TextPos; \n"

    "    // Operations in View Space \n"
    "    vec4 temp4 = mToViewSpace * vec4(in_sPosition, 1.0); \n"
    "    pointPos = temp4.xyz; \n"
    "    temp4 = mToViewSpace * vec4(in_sNormal, 0.0); \n"
    "    theNormal = normalize(temp4.xyz); \n"
    "}\n"
};

// Fragment shader for strings (textures) with illumination
const GLchar* stringsFragmentShader =
{
    "#version 150 \n"

    "uniform vec4 lightProps; // Position in View space, and intensity \n"
    "uniform vec3 lightColour; \n"
    "uniform sampler2D stringTexture; \n"

    "flat in vec3 theNormal; \n"
    "in vec3 pointPos; \n"
    "in vec2 textCoord; \n"

    "out vec4 fragColour; \n"

    "// Declare this function \n"
    "vec3 Illuminate(in vec4 LiProps, in vec3 LiColour, in vec4 PColour, \n"
    "                in vec3 PNormal, in vec3 PPos); \n"

    "void main(void) \n"
    "{\n"
    "    vec4 colo4 = texture(stringTexture, textCoord); \n"
    "    vec3 lightRes = Illuminate(lightProps, lightColour, colo4, \n"
    "                               theNormal, pointPos); \n "

    "    fragColour = vec4(lightRes, colo4.a); \n"
    "}\n"
};

// Vertex shader for immutable strings (textures)
const GLchar* stringsImmutableVS =
{
    "#version 150 \n"

    "in vec3 in_sPosition; \n"
    "in vec2 in_TextPos; \n"
    "uniform mat4 mMVP; \n"
    "out vec2 textCoord; \n"

    "void main(void) \n"
    "{\n"
    "    gl_Position = mMVP * vec4(in_sPosition, 1.0); \n"
    "    textCoord = in_TextPos; \n"
    "}\n"
};

// Fragment shader for immutable strings (textures)
const GLchar* stringsImmutableFS =
{
    "#version 150 \n"

    "uniform sampler2D stringTexture; \n"
    "in vec2 textCoord; \n"
    "out vec4 fragColour; \n"

    "void main(void) \n"
    "{\n"
    "    fragColour= texture(stringTexture, textCoord); \n"
    "}\n"
};


// ----------------------------------------------------------------------------
// myOGLShaders
// ----------------------------------------------------------------------------

myOGLShaders::myOGLShaders()
{
    m_proId = 0;
    m_SHAinitializated = false;
}

myOGLShaders::~myOGLShaders()
{
    if ( m_proId )
        CleanUp();
}

void myOGLShaders::CleanUp()
{
    StopUse();

    glDeleteProgram(m_proId);

    glFlush();
}

void myOGLShaders::AddCode(const GLchar* shaString, GLenum shaType)
{
    // The code is a null-terminated string
    shaShas sv = {0, shaType, shaString};
    m_shaCode.push_back(sv);
}

void myOGLShaders::AddAttrib(const std::string& name)
{
    shaVars sv = {0, name}; //We will set the location later
    m_shaAttrib.push_back(sv);
    // We don't check the max number of attribute locations (usually 16)
}

void myOGLShaders::AddUnif(const std::string& name)
{
    shaVars sv = {0, name};
    m_shaUnif.push_back(sv);
}

// Inform GL of the locations in program for the vars for buffers used to feed
// the shader. We use glBindAttribLocation (before linking the gl program) with
// the location we want.
// Since GL 3.3 we could avoid this using in the shader "layout(location=x)...".
// The same names as in the shader must be previously set with AddAttrib()
void myOGLShaders::SetAttribLocations()
{
    GLuint loc = 0;
    for(shaVars_v::iterator it = m_shaAttrib.begin(); it != m_shaAttrib.end(); ++it)
    {
        it->loc = loc++;
        glBindAttribLocation(m_proId, it->loc, it->name.c_str());
    }
}

GLuint myOGLShaders::GetAttribLoc(const std::string& name)
{
    for (shaVars_v::iterator it = m_shaAttrib.begin(); it != m_shaAttrib.end(); ++it)
    {
        if ( it->name == name && it->loc != (GLuint)-1 )
            return it->loc;
    }

    return (GLuint) -1;
}

// Store the locations in program for uniforms vars
bool myOGLShaders::AskUnifLocations()
{
    for (shaVars_v::iterator it = m_shaUnif.begin(); it != m_shaUnif.end(); ++it)
    {
        GLint glret = glGetUniformLocation(m_proId, it->name.c_str());
        if ( glret == -1 )
        {
            // Return now, this GPU program cannot be used because we will
            // pass data to unknown/unused uniform locations
            return false;
        }
        it->loc = glret;
    }

    return true;
}

GLuint myOGLShaders::GetUnifLoc(const std::string& name)
{
    for (shaVars_v::iterator it = m_shaUnif.begin(); it != m_shaUnif.end(); ++it)
    {
        if ( it->name == name && it->loc != (GLuint)-1 )
            return it->loc;
    }

    return (GLuint) -1;
}

// Create a GPU program from the given shaders
void myOGLShaders::Init()
{
    MyOnGLError(myoglERR_CLEAR); //clear error stack

    bool resC = false;
    bool resL = false;

    // GLSL code load and compilation
    for (shaShas_v::iterator it = m_shaCode.begin(); it != m_shaCode.end(); ++it)
    {
        it->shaId = glCreateShader(it->typeSha);
        glShaderSource(it->shaId, 1, &(it->scode), NULL);
        MyOnGLError(myoglERR_SHADERCREATE);

        resC = Compile(it->shaId);
        if ( !resC )
            break;
    }

    if ( resC )
    {
        // The program in the GPU
        m_proId = glCreateProgram();
        for (shaShas_v::iterator it = m_shaCode.begin(); it != m_shaCode.end(); ++it)
        {
            glAttachShader(m_proId, it->shaId);
        }

        SetAttribLocations(); //Before linking

        resL = LinkProg(m_proId);
    }

    // We don't need them any more
    for (shaShas_v::iterator it = m_shaCode.begin(); it != m_shaCode.end(); ++it)
    {
        if ( resC && it->shaId )
        {
            glDetachShader(m_proId, it->shaId);
        }
        glDeleteShader(it->shaId);
    }

    if ( !resC || !resL )
        return;

    // Log that shaders are OK
    MyOnGLError(myoglERR_JUSTLOG, "Shaders successfully compiled and linked.");

    // After linking, we can get locations for uniforms
    m_SHAinitializated = AskUnifLocations();
    if ( !m_SHAinitializated )
        MyOnGLError(myoglERR_SHADERLOCATION, " Unused or unrecognized uniform.");
}

// Useful while developing: show shader compilation errors
bool myOGLShaders::Compile(GLuint shaId)
{
    glCompileShader(shaId);

    GLint Param = 0;
    glGetShaderiv(shaId, GL_COMPILE_STATUS, &Param);

    if ( Param == GL_FALSE )
    {
        glGetShaderiv(shaId, GL_INFO_LOG_LENGTH, &Param);

        if ( Param > 0 )
        {
            GLchar* InfoLog = new GLchar[Param];
            int nChars = 0;
            glGetShaderInfoLog(shaId, Param, &nChars, InfoLog);
            MyOnGLError(myoglERR_SHADERCOMPILE, InfoLog);
            delete [] InfoLog;
        }
        return false;
    }
    return true;
}

// Useful while developing: show shader program linkage errors
bool myOGLShaders::LinkProg(GLuint proId)
{
    glLinkProgram(proId);

    GLint Param = 0;
    glGetProgramiv(proId, GL_LINK_STATUS, &Param);

    if ( Param == GL_FALSE )
    {
        glGetProgramiv(proId, GL_INFO_LOG_LENGTH, &Param);

        if ( Param > 0 )
        {
            GLchar* InfoLog = new GLchar[Param];
            int nChars = 0;
            glGetProgramInfoLog(proId, Param, &nChars, InfoLog);
            MyOnGLError(myoglERR_SHADERLINK, InfoLog);
            delete [] InfoLog;
        }
        return false;
    }
    return true;
}

bool myOGLShaders::Use()
{
    if ( !m_SHAinitializated )
        return false;

    glUseProgram(m_proId);
    return true;
}

void myOGLShaders::StopUse()
{
    glUseProgram(0);
}

// Disable generic attributes from VAO.
// This should be needed only for some old card, which uses generic into VAO
void myOGLShaders::DisableGenericVAA()
{
    for(shaVars_v::iterator it = m_shaAttrib.begin(); it != m_shaAttrib.end(); ++it)
    {
        glDisableVertexAttribArray(it->loc);
    }
}


// ----------------------------------------------------------------------------
// A point light
// ----------------------------------------------------------------------------

void myLight::Set(const myVec3& position, GLfloat intensity,
                  GLfloat R, GLfloat G, GLfloat B)
{
    m_PosAndIntensisty[0] = (GLfloat) position.x;
    m_PosAndIntensisty[1] = (GLfloat) position.y;
    m_PosAndIntensisty[2] = (GLfloat) position.z;
    m_PosAndIntensisty[3] = (GLfloat) intensity;
    m_Colour[0] = R;
    m_Colour[1] = G;
    m_Colour[2] = B;
}


// ----------------------------------------------------------------------------
// myOGLTriangles
// ----------------------------------------------------------------------------
myOGLTriangles::myOGLTriangles()
{
    m_triangVAO = m_bufVertId = m_bufColNorId = m_bufIndexId = 0;
    m_triangShaders = NULL;
}

myOGLTriangles::~myOGLTriangles()
{
    Clear();
}

void myOGLTriangles::Clear()
{
    if ( m_triangShaders )
        m_triangShaders->DisableGenericVAA();

    // Clear graphics card memory
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    if ( m_bufIndexId )
        glDeleteBuffers(1, &m_bufIndexId);
    if ( m_bufColNorId )
        glDeleteBuffers(1, &m_bufColNorId);
    if ( m_bufVertId )
        glDeleteBuffers(1, &m_bufVertId);

    // Unbind from context
    glBindVertexArray(0);
    if ( m_triangVAO )
        glDeleteVertexArrays(1, &m_triangVAO);

    glFlush(); //Tell GL to execute those commands now, but we don't wait for them

    m_triangShaders = NULL;
    m_triangVAO = m_bufIndexId = m_bufColNorId = m_bufVertId = 0;
}

void myOGLTriangles::SetBuffers(myOGLShaders* theShader,
                                GLsizei nuPoints, GLsizei nuTriangs,
                                const GLfloat* vert, const GLfloat* colo,
                                const GLfloat* norm, const GLushort* indices)
{
    MyOnGLError(myoglERR_CLEAR); //clear error stack

    // NOTE: have you realized that I fully trust on parameters being != 0 and != NULL?

    // Part 1: Buffers - - - - - - - - - - - - - - - - - - -

    // Graphics card buffer for vertices.
    // Not shared buffer with colours and normals, why not? Just for fun.
    glGenBuffers(1, &m_bufVertId);
    glBindBuffer(GL_ARRAY_BUFFER, m_bufVertId);
    // Populate the buffer with the array "vert"
    GLsizeiptr nBytes = nuPoints * 3 * sizeof(GLfloat); //3 components {x,y,z}
    glBufferData(GL_ARRAY_BUFFER, nBytes, vert, GL_STATIC_DRAW);

    if ( ! MyOnGLError(myoglERR_BUFFER) )
    {
        // Likely the GPU got out of memory
        Clear();
        return;
    }

    // Graphics card buffer for colours and normals.
    glGenBuffers(1, &m_bufColNorId);
    glBindBuffer(GL_ARRAY_BUFFER, m_bufColNorId);
    // Allocate space for both arrays
    nBytes = (nuPoints * 4 + nuTriangs * 3) * sizeof(GLfloat);
    glBufferData(GL_ARRAY_BUFFER, nBytes, NULL, GL_STATIC_DRAW);
    if ( ! MyOnGLError(myoglERR_BUFFER) )
    {
        // Likely the GPU got out of memory
        Clear();
        return;
    }
    // Populate part of the buffer with the array "colo"
    nBytes = nuPoints * 4 * sizeof(GLfloat); // rgba components
    glBufferSubData(GL_ARRAY_BUFFER, 0, nBytes, colo);
    // Add the array "norm" to the buffer
    GLsizeiptr bufoffset = nBytes;
    nBytes = nuTriangs * 3 * sizeof(GLfloat);
    glBufferSubData(GL_ARRAY_BUFFER, bufoffset, nBytes, norm);

    // Graphics card buffer for indices.
    glGenBuffers(1, &m_bufIndexId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufIndexId);
    // Populate the buffer with the array "indices"
    // We use "triangle strip". An index for each additional vertex.
    nBytes = (3 + nuTriangs - 1) * sizeof(GLushort); //Each triangle needs 3 indices
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, nBytes, indices, GL_STATIC_DRAW);

    if ( ! MyOnGLError(myoglERR_BUFFER) )
    {
        // Likely the GPU got out of memory
        Clear();
        return;
    }

    // Unbind buffers. We will bind them one by one just now, at VAO creation
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    m_nuTriangs = nuTriangs;
    m_triangShaders = theShader;

    MyOnGLError(myoglERR_CLEAR); //clear error stack

    // Part 2: VAO - - - - - - - - - - - - - - - - - - -

    // Vertex Array Object (VAO) that stores the relationship between the
    // buffers and the shader input attributes
    glGenVertexArrays(1, &m_triangVAO);
    glBindVertexArray(m_triangVAO);

    // Set the way of reading (blocks of n floats each) from the current bound
    // buffer and passing data to the shader (through the index of an attribute).
    // Vertices positions
    glBindBuffer(GL_ARRAY_BUFFER, m_bufVertId);
    GLuint loc = m_triangShaders->GetAttribLoc("in_Position");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
    // Colours
    glBindBuffer(GL_ARRAY_BUFFER, m_bufColNorId);
    loc = m_triangShaders->GetAttribLoc("in_Colour");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
    // Normals. Their position in buffer starts at bufoffset
    loc = m_triangShaders->GetAttribLoc("in_Normal");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)bufoffset);
    // Indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufIndexId);

    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Some log
    MyOnGLError(myoglERR_JUSTLOG, "Triangles data loaded into GPU.");
}

void myOGLTriangles::Draw(const GLfloat* unifMvp, const GLfloat* unifToVw,
                          const myLight* theLight)
{
    if ( !m_triangVAO )
        return;

    MyOnGLError(myoglERR_CLEAR); //clear error stack

    if ( ! m_triangShaders->Use() )
        return;

    // Bind the source data for the shader
    glBindVertexArray(m_triangVAO);

    // Pass matrices to the shader in column-major order
    glUniformMatrix4fv(m_triangShaders->GetUnifLoc("mMVP"), 1, GL_FALSE, unifMvp);
    glUniformMatrix4fv(m_triangShaders->GetUnifLoc("mToViewSpace"), 1, GL_FALSE, unifToVw);
    // Pass the light, in View coordinates in this sample
    glUniform4fv(m_triangShaders->GetUnifLoc("lightProps"), 1, theLight->GetFLightPos());
    glUniform3fv(m_triangShaders->GetUnifLoc("lightColour"), 1, theLight->GetFLightColour());

    // We have a flat shading, and we want the first vertex data as the flat value
    glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);

    // Indexed drawing the triangles in strip mode, using 6 indices
    glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_SHORT, (GLvoid *)0);

    MyOnGLError(myoglERR_DRAWING_TRI);

   // Unbind
    glBindVertexArray(0);
    m_triangShaders->StopUse();
}


// ----------------------------------------------------------------------------
// myOGLString
// ----------------------------------------------------------------------------
myOGLString::myOGLString()
{
    m_bufPosId = m_textureId = m_stringVAO = m_textureUnit = 0;
    m_stringShaders = NULL;
}

myOGLString::~myOGLString()
{
    Clear();
}

void myOGLString::Clear()
{
    if ( m_stringShaders )
        m_stringShaders->DisableGenericVAA();

    // Clear graphics card memory
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    if ( m_bufPosId )
        glDeleteBuffers(1, &m_bufPosId);

    // Unbind from context
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &m_stringVAO);

    if ( m_textureUnit && m_textureId )
    {
        glActiveTexture(GL_TEXTURE0 + m_textureUnit);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &m_textureId);
    }
    glActiveTexture(GL_TEXTURE0);

    glFlush(); //Tell GL to execute those commands now, but we don't wait for them

    m_bufPosId = m_textureId = m_stringVAO = m_textureUnit = 0;
    m_stringShaders = NULL;
}

void myOGLString::SetStringWithVerts(myOGLShaders* theShader,
                             const unsigned char* tImage, int tWidth, int tHeigh,
                             const GLfloat* vert, const GLfloat* norm)
{
    MyOnGLError(myoglERR_CLEAR); //clear error stack

    if ( !tImage )
        return;

    // Part 1: Buffers - - - - - - - - - - - - - - - - - - -

    // Graphics card buffer for vertices, normals, and texture coords
    glGenBuffers(1, &m_bufPosId);
    glBindBuffer(GL_ARRAY_BUFFER, m_bufPosId);
    // (4+4) (vertices + normals) x 3 components + 4 text-vertices x 2 components
    GLsizeiptr nBytes = (8 * 3 + 4 * 2) * sizeof(GLfloat);
    glBufferData(GL_ARRAY_BUFFER, nBytes, NULL, GL_STATIC_DRAW);

    if ( ! MyOnGLError(myoglERR_BUFFER) )
    {
        // Likely the GPU got out of memory
        Clear();
        return;
    }

    // Populate part of the buffer with the array "vert"
    nBytes = 12 * sizeof(GLfloat);
    glBufferSubData(GL_ARRAY_BUFFER, 0, nBytes, vert);
    // Add the array "norm" to the buffer
    GLsizeiptr bufoffset = nBytes;
    if ( norm )
    {
        // Just for string on face, not immutable string
        glBufferSubData(GL_ARRAY_BUFFER, bufoffset, nBytes, norm);
    }

    // Add the array of texture coordinates to the buffer.
    // Order is set accordingly with the vertices
    // See myOGLManager::SetStringOnPyr()
    GLfloat texcoords[8] = { 0.0, 1.0,  0.0, 0.0,  1.0, 1.0,  1.0, 0.0 };
    bufoffset += nBytes;
    nBytes = 8 * sizeof(GLfloat);
    glBufferSubData(GL_ARRAY_BUFFER, bufoffset, nBytes, texcoords);

    m_stringShaders = theShader;

    MyOnGLError(myoglERR_CLEAR); //clear error stack

    // Part 2: VAO - - - - - - - - - - - - - - - - - - -

    // Vertex Array Object (VAO) that stores the relationship between the
    // buffers and the shader input attributes
    glGenVertexArrays(1, &m_stringVAO);
    glBindVertexArray(m_stringVAO);

    // Set the way of reading (blocks of n floats each) from the current bound
    // buffer and passing data to the shader (through the index of an attribute).
    // Vertices positions
    GLuint loc = m_stringShaders->GetAttribLoc("in_sPosition");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
    // Normals. Their position in buffer starts at bufoffset
    bufoffset = 12 * sizeof(GLfloat);
    if ( norm )
    {
        // Just for string on face, not immutable string
        loc = m_stringShaders->GetAttribLoc("in_sNormal");
        glEnableVertexAttribArray(loc);
        glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)bufoffset);
    }
    // Texture coordinates
    bufoffset *= 2; //Normals take same amount of space as vertices
    loc = m_stringShaders->GetAttribLoc("in_TextPos");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)bufoffset);

    // Part 3: The texture with the string as an image - - - - - - - -

    // Create the bind for the texture
    // Same unit for both textures (strings) since their characteristics are the same.
    m_textureUnit = 1;
    glActiveTexture(GL_TEXTURE0 + m_textureUnit);
    glGenTextures(1, &m_textureId); //"Name" of the texture object
    glBindTexture(GL_TEXTURE_2D, m_textureId);
    // Avoid some artifacts
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Do this before glTexImage2D because we only have 1 level, no mipmap
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    // For RGBA default alignment (4) is good. In other circumstances, we may
    // need glPixelStorei(GL_UNPACK_ALIGNMENT, 1)
    // Load texture into card. No mipmap, so 0-level
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 (GLsizei)tWidth, (GLsizei)tHeigh, 0,
                  GL_RGBA, GL_UNSIGNED_BYTE, tImage);
    if ( ! MyOnGLError(myoglERR_TEXTIMAGE) )
    {
        // Likely the GPU got out of memory
        Clear();
        return;
    }

    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);

    // Some log
    MyOnGLError(myoglERR_JUSTLOG, "Texture for string loaded into GPU.");
}

void myOGLString::Draw(const GLfloat* unifMvp, const GLfloat* unifToVw,
                       const myLight* theLight)
{
    if ( !m_stringVAO )
        return;

    MyOnGLError(myoglERR_CLEAR); //clear error stack

    if ( ! m_stringShaders->Use() )
        return;

    // Bind the source data for the shader
    glBindVertexArray(m_stringVAO);

    // Pass matrices to the shader in column-major order
    glUniformMatrix4fv(m_stringShaders->GetUnifLoc("mMVP"), 1, GL_FALSE, unifMvp);
    if ( unifToVw && theLight )
    {
        // Just for string on face, not immutable string
        glUniformMatrix4fv(m_stringShaders->GetUnifLoc("mToViewSpace"), 1, GL_FALSE, unifToVw);
        // Pass the light, in View coordinates in this sample
        glUniform4fv(m_stringShaders->GetUnifLoc("lightProps"), 1, theLight->GetFLightPos());
        glUniform3fv(m_stringShaders->GetUnifLoc("lightColour"), 1, theLight->GetFLightColour());

        // We have a flat shading, and we want the first vertex normal as the flat value
        glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);
    }

    // Use our texture unit
    glActiveTexture(GL_TEXTURE0 + m_textureUnit);
    glBindTexture(GL_TEXTURE_2D, m_textureId);
    // The fragment shader will read texture values (pixels) from the texture
    // currently active
    glUniform1i(m_stringShaders->GetUnifLoc("stringTexture"), m_textureUnit);

    // Draw the rectangle made up of two triangles
    glDrawArrays(GL_TRIANGLE_STRIP, 0,  4);

    MyOnGLError(myoglERR_DRAWING_STR);

    // Unbind
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);

    m_stringShaders->StopUse();
}

// ----------------------------------------------------------------------------
// myOGLImmutString
// ----------------------------------------------------------------------------
void myOGLImmutString::SetImmutString(myOGLShaders* theShader,
                        const unsigned char* tImage, int tWidth, int tHeigh)
{
    // Make a rectangle of the same size as the image. Order of vertices matters.
    // Set a 2 pixels margin
    double imaVerts[12];
    imaVerts[0] = 2.0         ;   imaVerts[1] = 2.0         ; imaVerts[2]  = -1.0;
    imaVerts[3] = 2.0         ;   imaVerts[4] = 2.0 + tHeigh; imaVerts[5]  = -1.0;
    imaVerts[6] = 2.0 + tWidth;   imaVerts[7] = 2.0         ; imaVerts[8]  = -1.0;
    imaVerts[9] = 2.0 + tWidth;  imaVerts[10] = 2.0 + tHeigh; imaVerts[11] = -1.0;

    // GLFloat version
    GLfloat fimaVerts[12];
    SetAsGLFloat4x4(imaVerts, fimaVerts, 12);

    // Call the base class without normals, it will handle this case
    SetStringWithVerts(theShader, tImage, tWidth, tHeigh, fimaVerts, NULL);
}

void myOGLImmutString::SetOrtho(int winWidth, int winHeight)
{
    // We want an image always of the same size, regardless of window size.
    // The orthogonal projection with the whole window achieves it.
    MyOrtho(0.0, winWidth, 0.0, winHeight, -1.0, 1.0, m_dOrtho);
    // Store the 'float' matrix
    SetAsGLFloat4x4(m_dOrtho, m_fOrtho, 16);
}


// ----------------------------------------------------------------------------
// myOGLCamera
// ----------------------------------------------------------------------------
myOGLCamera::myOGLCamera()
{
    m_needMVPUpdate = true; //Matrix must be updated
    InitPositions();
}

void myOGLCamera::InitPositions()
{
    // We have a tetrahedron centered at origin and edge length = 200
    m_centerOfWorld.x = m_centerOfWorld.y = m_centerOfWorld.z = 0.0;

    // The radius of the bounding sphere
    m_radiusOfWorld = 122.4745;

    // From degrees to radians
    double degToRad = (double) 4.0 * atan(1.0) / 180.0;

    // Angle of the field of view
    m_fov = 40.0 * degToRad; //radians

    // Position the camera far enough so we can see the whole world.
    // The camera is between X and Z axis, below the pyramid
    double tmpv = m_radiusOfWorld / sin(m_fov/2.0);
    tmpv *= 1.05; // 5% margin
    m_camPosition.x = m_centerOfWorld.x + tmpv * cos(75.0 * degToRad);
    m_camPosition.z = m_centerOfWorld.z + tmpv * sin(75.0 * degToRad);
    m_camPosition.y = m_centerOfWorld.y - m_radiusOfWorld;

    // This camera looks always at center
    m_camTarget = m_centerOfWorld;

    // A vector perpendicular to Position-Target heading Y+
    myVec3 vper = MyNormalize(m_camTarget - m_camPosition);
    m_camUp = myVec3(0.0, 1.0, 0.0);
    m_camUp = MyCross(m_camUp, vper);
    m_camUp = MyNormalize( MyCross(vper, m_camUp) );

    tmpv = MyDistance(m_camPosition, m_centerOfWorld);
    // Calculate distances, not coordinates, with some margins
    // Near clip-plane distance to the camera
    m_nearD = tmpv - 1.10 * m_radiusOfWorld - 5.0;
    // Far clip-plane distance to the camera
    m_farD = tmpv + 1.10 * m_radiusOfWorld + 5.0;

    // The "View" matrix. We will not change it any more in this sample
    MyLookAt(m_camPosition, m_camUp, m_camTarget, m_dView);

    // The initial "Model" matrix is the Identity matrix
    MyRotate(myVec3(0.0, 0.0, 1.0), 0.0, m_dMode);

    // Nothing else. "View" matrix is calculated at ViewSizeChanged()
}

void myOGLCamera::ViewSizeChanged(int newWidth, int newHeight)
{
    // These values are also used for MouseRotation()
    m_winWidth = newWidth;
    m_winHeight = newHeight;

    // Calculate the projection matrix
    double aspect = (double) newWidth / newHeight;
    MyPerspective(m_fov, aspect, m_nearD, m_farD, m_dProj);

    // Inform we need to calculate MVP matrix
    m_needMVPUpdate = true;
}

const GLfloat* myOGLCamera::GetFloatMVP()
{
    UpdateMatrices();

    return m_fMVP;
}

const GLfloat* myOGLCamera::GetFloatToVw()
{
    UpdateMatrices();

    return m_fToVw;
}

void myOGLCamera::UpdateMatrices()
{
   if ( m_needMVPUpdate )
    {
        MyMatMul4x4(m_dView, m_dMode, m_dToVw);
        MyMatMul4x4(m_dProj, m_dToVw, m_dMVP);
        // Store the 'float' matrices
        SetAsGLFloat4x4(m_dToVw, m_fToVw, 16);
        SetAsGLFloat4x4(m_dMVP, m_fMVP, 16);
        m_needMVPUpdate = false;
    }
}

void myOGLCamera::MouseRotation(int fromX, int fromY, int toX, int toY)
{
    if ( fromX == toX && fromY == toY )
        return; //no rotation

    // 1. Obtain axis of rotation and angle simulating a virtual trackball "r"

    // 1.1. Calculate normalized coordinates (2x2x2 box).
    // The trackball is a part of sphere of radius "r" (the rest is hyperbolic)
    // Use r= 0.8 for better maximum rotation (more-less 150 degrees)
    double xw1 = (2.0 * fromX - m_winWidth) / m_winWidth;
    double yw1 = (2.0 * fromY - m_winHeight) / m_winHeight;
    double xw2 = (2.0 * toX - m_winWidth) / m_winWidth;
    double yw2 = (2.0 * toY - m_winHeight) / m_winHeight;
    double z1 = GetTrackballZ(xw1, yw1, 0.8);
    double z2 = GetTrackballZ(xw2, yw2, 0.8);

    // 1.2. With normalized vectors, compute axis from 'cross' and angle from 'dot'
    myVec3 v1(xw1, yw1, z1);
    myVec3 v2(xw2, yw2, z2);
    v1 = MyNormalize(v1);
    v2 = MyNormalize(v2);
    myVec3 axis(MyCross(v1, v2));

    // 'axis' is in camera coordinates. Transform it to world coordinates.
    double mtmp[16];
    MyMatInverse(m_dView, mtmp);
    myVec4 res = MyMatMul4x1(mtmp, myVec4(axis));
    axis.x = res.x;
    axis.y = res.y;
    axis.z = res.z;
    axis = MyNormalize(axis);

    double angle = AngleBetween(v1, v2);

    // 2. Compute the model transformation (rotate the model) matrix
    MyRotate(axis, angle, mtmp);
    // Update "Model" matrix
    double mnew[16];
    MyMatMul4x4(mtmp, m_dMode, mnew);
    for (size_t i = 0; i<16; ++i)
        m_dMode[i] = mnew[i];

    // Inform we need to calculate MVP matrix
    m_needMVPUpdate = true;
}

// Return the orthogonal projection of (x,y) into a sphere centered on the screen
// and radius 'r'. This makes some (x,y) to be outside of circle r='r'. We avoid
// this issue by using a hyperbolic sheet for (x,y) outside of r = 0.707 * 'r'.
double myOGLCamera::GetTrackballZ(double x, double y, double r)
{
    double d = x*x + y*y;
    double r2 = r*r;
    return  (d < r2/2.0) ? sqrt(r2 - d) : r2/2.0/sqrt(d);
}


// ----------------------------------------------------------------------------
// myOGLManager
// ----------------------------------------------------------------------------

myOGLManager::myOGLManager(myOGLErrHandler* extErrHnd)
{
    externalMyOGLErrHandler = extErrHnd;
    MyOnGLError(myoglERR_CLEAR); //clear error stack
}

myOGLManager::~myOGLManager()
{
    MyOnGLError(myoglERR_CLEAR); //clear error stack

    // Force GPU finishing before the context is deleted
    glFinish();
}

/* Static */
bool myOGLManager::Init()
{
    // Retrieve the pointers to OGL functions we use in this sample
    return MyInitGLPointers();
}

const GLubyte* myOGLManager::GetGLVersion()
{
    return glGetString(GL_VERSION);
}

const GLubyte* myOGLManager::GetGLVendor()
{
    return glGetString(GL_VENDOR);
}

const GLubyte* myOGLManager::GetGLRenderer()
{
    return glGetString(GL_RENDERER);
}

void myOGLManager::SetShadersAndTriangles()
{
    // The shaders attributes and uniforms
    m_TriangShaders.AddAttrib("in_Position");
    m_TriangShaders.AddAttrib("in_Colour");
    m_TriangShaders.AddAttrib("in_Normal");
    m_TriangShaders.AddUnif("mMVP");
    m_TriangShaders.AddUnif("mToViewSpace");
    m_TriangShaders.AddUnif("lightProps");
    m_TriangShaders.AddUnif("lightColour");
    m_TriangShaders.AddCode(triangVertexShader, GL_VERTEX_SHADER);
    m_TriangShaders.AddCode(illuminationShader, GL_FRAGMENT_SHADER);
    m_TriangShaders.AddCode(triangFragmentShader, GL_FRAGMENT_SHADER);
    m_TriangShaders.Init();
    m_StringShaders.AddAttrib("in_sPosition");
    m_StringShaders.AddAttrib("in_sNormal");
    m_StringShaders.AddAttrib("in_TextPos");
    m_StringShaders.AddUnif("mMVP");
    m_StringShaders.AddUnif("mToViewSpace");
    m_StringShaders.AddUnif("lightProps");
    m_StringShaders.AddUnif("lightColour");
    m_StringShaders.AddUnif("stringTexture");
    m_StringShaders.AddCode(stringsVertexShader, GL_VERTEX_SHADER);
    m_StringShaders.AddCode(illuminationShader, GL_FRAGMENT_SHADER);
    m_StringShaders.AddCode(stringsFragmentShader, GL_FRAGMENT_SHADER);
    m_StringShaders.Init();
    m_ImmutStringSha.AddAttrib("in_sPosition");
    m_ImmutStringSha.AddAttrib("in_TextPos");
    m_ImmutStringSha.AddUnif("mMVP");
    m_ImmutStringSha.AddUnif("stringTexture");
    m_ImmutStringSha.AddCode(stringsImmutableVS, GL_VERTEX_SHADER);
    m_ImmutStringSha.AddCode(stringsImmutableFS, GL_FRAGMENT_SHADER);
    m_ImmutStringSha.Init();
    // The point light. Set its color as full white.
    // In this sample we set the light position same as the camera position
    // In View space, camera position is {0, 0, 0}
    m_Light.Set(myVec3(0.0, 0.0, 0.0), 1.0, 1.0, 1.0, 1.0);
    // The triangles data
    m_Triangles.SetBuffers(&m_TriangShaders, 4, 4, gVerts, gColours, gNormals, gIndices);
}

void myOGLManager::SetStringOnPyr(const unsigned char* strImage, int iWidth, int iHeigh)
{
    // Some geometry. We want a rectangle close to face 0-1-2 (X-Z plane).
    // The rectangle must preserve strImage proportions. If the height of the
    // rectangle is "h" and we want to locate it with its largest side parallel
    // to the edge of the face and at distance= h/2, then the rectangle width is
    // rw = edgeLength - 2 * ((h/2 + h + h/2)/tan60) = edgeLength - 4*h/sqrt(3)
    // If h/rw = Prop then
    //    rw = edgeLength / (1+4/sqrt(3)*Prop) and h = Prop * rw

    double edgeLen = MyDistance(myVec3(gVerts[0], gVerts[1], gVerts[2]),
                                 myVec3(gVerts[6], gVerts[7], gVerts[8]));
    GLfloat prop = ((GLfloat) iHeigh) / ((GLfloat) iWidth);
    GLfloat rw = float(edgeLen) / (1 + 4 * prop / std::sqrt(3.0f));
    GLfloat h = prop * rw;
    GLfloat de = 2 * h / std::sqrt(3.0f);
    // A bit of separation of the face so as to avoid z-fighting
    GLfloat rY = gVerts[1] - 0.01f; // Towards outside
    GLfloat sVerts[12];
    // The image was created top to bottom, but OpenGL axis are bottom to top.
    // The image would display upside down. We avoid it choosing the right
    // order of vertices and texture coords. See myOGLString::SetStringWithVerts()
    sVerts[0] = gVerts[6] + de;  sVerts[1] = rY;   sVerts[2] = gVerts[8] + h / 2;
    sVerts[3] = sVerts[0]     ;  sVerts[4] = rY;   sVerts[5] = sVerts[2] + h;
    sVerts[6] = sVerts[0] + rw;  sVerts[7] = rY;   sVerts[8] = sVerts[2];
    sVerts[9] = sVerts[6]     ; sVerts[10] = rY;  sVerts[11] = sVerts[5];

    // Normals for the rectangle illumination, same for the four vertices
    const GLfloat strNorms[] = { gNormals[0], gNormals[1], gNormals[2],
                                 gNormals[0], gNormals[1], gNormals[2],
                                 gNormals[0], gNormals[1], gNormals[2],
                                 gNormals[0], gNormals[1], gNormals[2]};

    // The texture data for the string on the face of the pyramid
    m_StringOnPyr.SetStringWithVerts(&m_StringShaders, strImage, iWidth, iHeigh,
                                     sVerts, strNorms);
}

void myOGLManager::SetImmutableString(const unsigned char* strImage,
                                      int iWidth, int iHeigh)
{
    m_ImmString.SetImmutString(&m_ImmutStringSha, strImage, iWidth, iHeigh);
}

void myOGLManager::SetViewport(int x, int y, int width, int height)
{
    if (width < 1) width = 1;
    if (height < 1) height = 1;

    glViewport(x, y, (GLsizei)width, (GLsizei)height);

    // The camera handles perspective projection
    m_Camera.ViewSizeChanged(width, height);
    // And this object handles its own orthogonal projection
    m_ImmString.SetOrtho(width, height);
}

void myOGLManager::Render()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor((GLfloat)0.15, (GLfloat)0.15, 0.0, (GLfloat)1.0); // Dark, but not black.
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    m_Triangles.Draw(m_Camera.GetFloatMVP(), m_Camera.GetFloatToVw(), &m_Light);
    m_StringOnPyr.Draw(m_Camera.GetFloatMVP(), m_Camera.GetFloatToVw(), &m_Light);
    // This string is at the very front, whatever z-coords are given
    glDisable(GL_DEPTH_TEST);
    m_ImmString.Draw(m_ImmString.GetFloatMVP(), NULL, NULL);
}

void myOGLManager::OnMouseButDown(int posX, int posY)
{
    // Just save mouse position
    m_mousePrevX = posX;
    m_mousePrevY = posY;
}

void myOGLManager::OnMouseRotDragging(int posX, int posY)
{
    m_Camera.MouseRotation(m_mousePrevX, m_mousePrevY, posX, posY);
    m_mousePrevX = posX;
    m_mousePrevY = posY;
}
