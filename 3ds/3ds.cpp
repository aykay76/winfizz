#include "../stdafx.h"
#include "3ds.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../engine/mesh.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//-------------------------------------------------------
// generic stuff
//-------------------------------------------------------
            
typedef unsigned long ulong;

#define SEEK_START           1900
#define SEEK_CURSOR          1901
             
// common chunks
// colors
#define COLOR_F             0x0010
#define COLOR_24            0x0011
#define LIN_COLOR_24        0x0012
#define LIN_COLOR_F         0x0013

// percentage
#define INT_PERCENTAGE      0x0030
#define FLOAT_PERCENTAGE    0x0031

// ambient light
#define AMBIENT_LIGHT       0x2100

#define MAIN3DS             0x4D4D
#define EDIT3DS             0x3D3D  // this is the start of the editor config

// keyframer chunk ids
#define KFDATA              0xB000  // the keyframer section
#define KFHDR               0xB00A
#define OBJECT_NODE_TAG     0xB002  
#define NODE_HDR            0xB010
#define PIVOT               0xB013
#define POS_TRACK_TAG       0xB020
#define ROT_TRACK_TAG       0xB021
#define SCL_TRACK_TAG       0xB022

// material entries
#define MAT_ENTRY           0xAFFF
#define MAT_NAME            0xA000
#define MAT_AMBIENT         0xA010
#define MAT_DIFFUSE         0xA020
#define MAT_SPECULAR        0xA030
#define MAT_SHININESS       0xA040
#define MAT_SHIN2PCT        0xA041
#define MAT_TRANSPARENCY    0xA050
#define MAT_SHADING         0xA100
#define MAT_TWO_SIDE        0xA081
#define MAT_ADDITIVE        0xA083
#define MAT_WIRE            0xA085
#define MAT_FACEMAP         0xA088
#define MAT_WIRESIZE        0xA087
#define MAT_DECAL           0xA082
#define MAT_TEXMAP          0xA200
#define MAT_MAPNAME         0xA300
#define MAT_MAP_TILING      0xA351
#define MAT_MAP_USCALE      0xA354
#define MAT_MAP_VSCALE      0xA356
#define MAT_MAP_UOFFSET     0xA358
#define MAT_MAP_VOFFSET     0xA35A
#define MAT_MAP_ANG         0xA35C
#define MAT_TEX2MAP         0xA33A
#define MAT_OPACMAP         0xA210
#define MAT_BUMPMAP         0xA230
#define MAT_SPECMAP         0xA204
#define MAT_SHINMAP         0xA33C
#define MAT_REFLMAP         0xA220
#define MAT_ACUBIC          0xA310

#define EDIT_OBJECT         0x4000
#define OBJ_TRIMESH         0x4100
#define OBJ_LIGHT           0x4600
#define OBJ_CAMERA          0x4700

#define CAM_RANGES			0x4720

#define LIT_OFF             0x4620
#define LIT_SPOT            0x4610
#define LIT_INRANGE         0x4659
#define LIT_OUTRANGE        0x465A

#define TRI_VERTEXLIST      0x4110
#define TRI_VERTEXOPTIONS   0x4111
#define TRI_FACELIST        0x4120
#define TRI_MAT_GROUP       0x4130
#define TRI_SMOOTH_GROUP    0x4150
#define TRI_FACEMAPPING     0x4140
#define TRI_MATRIX          0x4160

#define SPOTLIGHT           0x4610

//----------------------------------

#define MAX_SHARED_TRIS     100

// the error reporting routine

void ErrorMsg(const char *msg)
{

}

// globals

Color3 black = {0, 0, 0};
Vector3 zero3;
Map emptyMap = {0, L"", 1, 1, 0, 0, 0};

//-------------------------------------------------------
// Object implementation
//-------------------------------------------------------

Object3d::Object3d()
{
    m_name = L"";
}

Object3d::~Object3d()
{
    // nothing here
}

void Object3d::SetName(const std::wstring& value)
{
    m_name = value;
}

const std::wstring& Object3d::GetName()
{
    return m_name;
}

bool Object3d::IsObject(const std::wstring &name)
{
    return (m_name == name);
}


//-------------------------------------------------------
// Material implementation
//-------------------------------------------------------
/*
Material::Material()
: Object()
{
    m_id = 0;
    m_texMap1 = emptyMap;
    m_texMap2 = emptyMap;
    m_opacMap = emptyMap;
    m_bumpMap = emptyMap;
    m_reflMap = emptyMap;
    m_specMap = emptyMap;
    m_ambient = black;
    m_diffuse = black;
    m_specular = black;
    m_shading = Gouraud;
    m_shininess = 0;
    m_transparency = 0;
}

Material::~Material()
{

}

uint Material::GetID()
{
    return m_id;
}

Map& Material::GetTextureMap1()
{
    return m_texMap1;
}

Map& Material::GetTextureMap2()
{
    return m_texMap2;
}

Map& Material::GetOpacityMap()
{
    return m_opacMap;
}

Map& Material::GetSpecularMap()
{
    return m_specMap;
}

Map& Material::GetBumpMap()
{
    return m_bumpMap;
}

Map& Material::GetReflectionMap()
{
    return m_reflMap;
}

Color3 Material::GetAmbientColor()
{
    return m_ambient;
}

Color3 Material::GetDiffuseColor()
{
    return m_diffuse;
}

Color3 Material::GetSpecularColor()
{
    return m_specular;
}

float Material::GetShininess()
{
    return m_shininess;
}

float Material::GetTransparency()
{
    return m_transparency;
}

Shading Material::GetShadingType()
{
    return m_shading;
}

void Material::SetID(uint value)
{
    m_id = value;
}

void Material::SetAmbientColor(const Color3 &color)
{
    m_ambient = color;
}

void Material::SetDiffuseColor(const Color3 &color)
{
    m_diffuse = color;
}

void Material::SetSpecularColor(const Color3 &color)
{
    m_specular = color;
}

void Material::SetShininess(float value)
{
    m_shininess = value;
    if (m_shininess < 0)
        m_shininess = 0;
    if (m_shininess > 1)
        m_shininess = 1;
}

void Material::SetTransparency(float value)
{
    m_transparency = value;
    if (m_transparency < 0)
        m_transparency = 0;
    if (m_transparency > 1)
        m_transparency = 1;
}

void Material::SetShadingType(Shading shading)
{
    m_shading = shading;
}
*/
//-------------------------------------------------------
// Camera implementation
//-------------------------------------------------------
/*
Camera::Camera()
:   Object3d()
{
    Clear();
}

Camera::~Camera()
{

}

void Camera::Clear()
{
    m_pos.x = m_pos.y = m_pos.z = 0.0f;
    m_target.x = m_target.y = m_target.z = 0.0f;
    m_fov = 80;
	m_bank = 0;;
	m_near = 10;
	m_far = 10000;
}

void Camera::SetPosition(Vector3 vec)
{
    m_pos = vec;
}

Vector3 Camera::GetPosition()
{
    return m_pos;
}

void Camera::SetTarget(Vector3 target)
{
    m_target = target;
}

Vector3 Camera::GetTarget()
{
    return m_target;
}

void Camera::SetFOV(float value)
{
    m_fov = value;
}

float Camera::GetFOV()
{
    return m_fov;
}

void Camera::SetBank(float value)
{
    m_bank = value;
}

float Camera::GetBank()
{
    return m_bank;
}

void Camera::SetNearplane(float value)
{
    m_near = value;
}

float Camera::GetNearplane()
{
    return m_near;
}

void Camera::SetFarplane(float value)
{
    m_far = value;
}

float Camera::GetFarplane()
{
    return m_far;
}
*/
//-------------------------------------------------------
// ight implementation
//-------------------------------------------------------
/*
Light::Light()
:   Object3d()
{
    Clear();
}

Light::~Light()
{

}

void Light::Clear()
{
    m_pos.x = m_pos.y = m_pos.z = 0.0f;
    m_color.r = m_color.g = m_color.b = 0.0f;
    m_spotlight = false;
	m_attenuationend = 100;
	m_attenuationstart = 1000;
}

void Light::SetPosition(Vector3 vec)
{
    m_pos = vec;
}

Vector3 Light::GetPosition()
{
    return m_pos;
}

void Light::SetColor(Color3 color)
{
    m_color = color;
}

Color3 Light::GetColor()
{
    return m_color;
}

void Light::SetSpotlight(bool value)
{
    m_spotlight = value;
}

bool Light::GetSpotlight()
{
    return m_spotlight;
}

void Light::SetTarget(Vector3 target)
{
    m_target = target;
}

Vector3 Light::GetTarget()
{
    return m_target;
}

void Light::SetHotspot(float value)
{
    m_hotspot = value;
}

float Light::GetHotspot()
{
    return m_hotspot;
}

void Light::SetFalloff(float value)
{
    m_falloff = value;
}
    
float Light::GetFalloff()
{
    return m_falloff;
}

void Light::SetAttenuationstart(float value)
{
    m_attenuationend = value;
}

float Light::GetAttenuationstart()
{
    return m_attenuationend;
}

void Light::SetAttenuationend(float value)
{
	m_attenuationstart = value;
}

float Light::GetAttenuationend()
{
	return m_attenuationstart;
}
*/
//-------------------------------------------------------
// Importer implementation
//-------------------------------------------------------

Importer::Importer()
{
    Clear();
}       

Importer::~Importer()
{
    Clear();
}

/*uint Importer::GetLightCount()
{
    return m_lights.size();
}
*/
/*uint Importer::GetMaterialCount()
{
    return m_materials.size();
}*/

/*uint Importer::GetCameraCount()
{
    return m_cameras.size();
}*/
/*
Light& Importer::GetLight(uint index)
{
    return m_lights[index];
}
*/
/*Material& Importer::GetMaterial(uint index)
{
    return m_materials[index];
}*/
/*
Camera& Importer::GetCamera(uint index)
{
    return m_cameras[index];
}*/

/*Material* Importer::FindMaterial(const std::wstring& name)
{
    for (uint i=0; i<m_materials.size(); i++)
        if (m_materials[i].IsObject(name))
            return &m_materials[i];
    return 0;
}*/
/*
Light* Importer::FindLight(const std::wstring& name)
{
    for (uint i=0; i<m_lights.size(); i++)
        if (m_lights[i].IsObject(name))
            return &m_lights[i];
    return 0;
}
*/
void Importer::Clear()
{
//    m_lights.clear();
//    m_materials.clear();
    m_optevel = Full;
}

void Importer::SetOptimisationLevel(OptimisationLevel value)
{
    m_optevel = value;        
}

OptimisationLevel Importer::GetOptimisationLevel()
{
    return m_optevel;
}

//-------------------------------------------------------
// 3DS implementation
//-------------------------------------------------------

a3DS::a3DS()
: Importer()
{
    m_buffer = 0;
    m_bufferSize = 0;
    m_pos = 0;
    m_eof = false;
} 

a3DS::~a3DS()
{
    if (m_bufferSize > 0)
        free(m_buffer);
}

bool a3DS::LoadFile(const wchar_t *filename, std::vector<Mesh>& meshes)
{
    FILE *f;
#ifdef _2003
	f = _wfopen(filename, L"rb");
#else
    _wfopen_s(&f, filename, L"rb");
#endif
    if (f == 0)
    {
        ErrorMsg("a3DS::LoadFile - cannot open file");
        return false;
    }
    fseek(f, 0, SEEK_END);
    m_bufferSize = ftell(f);
    fseek(f, 0, SEEK_SET);
    m_buffer = (unsigned char*) calloc(m_bufferSize, 1);
    if (m_buffer == 0)
    {
        ErrorMsg("a3DS::LoadFile - not enough memory (malloc failed)");
        return false;
    }
    if (fread(m_buffer, m_bufferSize, 1, f) != 1)
    {
        fclose(f);
        free(m_buffer);
        m_bufferSize = 0;
        ErrorMsg("a3DS::LoadFile - error reading from file");
        return false;
    }
    fclose(f);
    Clear();
    bool res = Read3DS(meshes);
    free(m_buffer);
    m_buffer = 0;
    m_bufferSize = 0;
    return res;
}

short a3DS::ReadShort()
{
    if ((m_buffer!=0) && (m_bufferSize != 0) && ((m_pos+2)<m_bufferSize))
    {
        short *w = (short*)(m_buffer+m_pos);
        short s = *w;
        m_pos += 2;
        return s;
    }
    m_eof = true;
    return 0;
}
 
int a3DS::ReadInt()
{
    if ((m_buffer!=0) && (m_bufferSize != 0) && ((m_pos+4)<m_bufferSize))
    {
        int *w = (int*)(m_buffer+m_pos);
        int s = *w;
        m_pos += 4;
        return s;
    }
    m_eof = true;
    return 0;
}

char a3DS::ReadChar()
{
    if ((m_buffer!=0) && (m_bufferSize != 0) && ((m_pos+1)<m_bufferSize))
    {
        char s = (char)*(m_buffer+m_pos);
        m_pos += 1;
        return s;
    }
    m_eof = true;
    return 0;
}

float a3DS::ReadFloat()
{
    if ((m_buffer!=0) && (m_bufferSize != 0) && ((m_pos+4)<m_bufferSize))
    {
        float *w = (float*)(m_buffer+m_pos);
        float s = *w;
        m_pos += 4;
        return s;
    }
    m_eof = true;
    return 0.0;
}

byte a3DS::ReadByte()
{
    if ((m_buffer!=0) && (m_bufferSize != 0) && ((m_pos+1)<m_bufferSize))
    {
        byte s = (byte)*(m_buffer+m_pos);
        m_pos += 1;
        return s;
    }
    m_eof = true;
    return 0;
}

int a3DS::ReadASCIIZ(wchar_t *buf, int max_count)
{
    int count;
    if ((m_buffer==0) || (m_bufferSize == 0) || (m_pos>=m_bufferSize))
    {
        count = 0;
        m_eof = true;
        return count;
    }
    count = 0;
    char c = ReadChar();
    while ((c!=0) && (count<max_count-1))
    {
        buf[count] = c;
        count ++;
        c = ReadChar();
    }
    buf[count] = 0;
    return count;
}

void a3DS::Seek(int offset, int origin)
{
    if (origin == SEEK_START)
        m_pos = offset;
    if (origin == SEEK_CURSOR)
        m_pos += offset;
    if (m_pos < 0)
        m_pos = 0;
    if (m_pos >= m_bufferSize)
        m_pos = m_bufferSize-1;
    m_eof = false;
}

uint a3DS::Pos()
{
    return m_pos;
}

Chunk a3DS::ReadChunk()
{
    Chunk chunk;
    chunk.id = ReadShort();
    int a = ReadInt();
    chunk.start = Pos();
    chunk.end = chunk.start+a-6;
    return chunk;
}

bool a3DS::FindChunk(Chunk &target, const Chunk &parent)
{
    if (Pos() >= parent.end)
        return false;
    Chunk chunk;
    chunk = ReadChunk();
    while (( chunk.id != target.id) && (chunk.end <= parent.end))
    {
        SkipChunk(chunk);
        if (chunk.end >= parent.end)
            break;
        chunk = ReadChunk();
    }
    if (chunk.id == target.id)
    {
        target.start = chunk.start;
        target.end = chunk.end;
        return true;
    }
    return false;
}

void a3DS::SkipChunk(const Chunk &chunk)
{
    Seek(chunk.end, SEEK_START);
}

void a3DS::GotoChunk(const Chunk &chunk)
{
    Seek(chunk.start, SEEK_START);
}

Color3 a3DS::ReadColor(const Chunk &chunk)
{
    Color3 col = black;
    GotoChunk(chunk);
    switch (chunk.id)
    {
    case COLOR_F:
        col.r = ReadFloat();
        col.g = ReadFloat();
        col.b = ReadFloat();
        break;
    case COLOR_24:
        col.r = ReadByte()/255.0f;
        col.g = ReadByte()/255.0f;
        col.b = ReadByte()/255.0f;
        break;
    case LIN_COLOR_F:
        col.r = ReadFloat();
        col.g = ReadFloat();
        col.b = ReadFloat();
        break;
    case LIN_COLOR_24:
        col.r = ReadByte()/255.0f;
        col.g = ReadByte()/255.0f;
        col.b = ReadByte()/255.0f;
        break;
    default:
        ErrorMsg("a3DS::ReadColor - error this is not a color chunk");
    }
    return col;
}

float a3DS::ReadPercentage(const Chunk &chunk)
{
    GotoChunk(chunk);
    switch (chunk.id)
    {
    case INT_PERCENTAGE:
        return (ReadShort()/100.0f);
    case FLOAT_PERCENTAGE:
        return ReadFloat();
    }
    ErrorMsg("a3DS::ReadPercentage - error, the chunk is not a percentage chunk");
    return 0;
}

bool a3DS::Read3DS(std::vector<Mesh>& meshes)
{
    Chunk mainchunk;
    Chunk edit;
    edit.id = EDIT3DS;
    mainchunk = ReadChunk();
    if (mainchunk.id != MAIN3DS)
    {
        ErrorMsg("a3DS::Read3DS - wrong file format");
        return false;
    }
    if (!FindChunk(edit, mainchunk))
        return false;
    Chunk obj;
    Chunk ml;

    GotoChunk(edit);
    obj.id = MAT_ENTRY;
    while (FindChunk(obj, edit))
    {
        ReadMaterial(obj);
        SkipChunk(obj);
    }
    GotoChunk(edit);

    obj.id = EDIT_OBJECT;
    {
        while (FindChunk(obj, edit))
        {
            ReadASCIIZ(m_objName, 99);
            ml = ReadChunk();
            if (ml.id == OBJ_TRIMESH)
                ReadMesh(ml, meshes);
			else
            if (ml.id == OBJ_LIGHT)
//                ReadLight(ml);
//			else
  //          if (ml.id == OBJ_CAMERA)
//                ReadCamera(ml);
            SkipChunk(obj);
        }
    }
    
    // read the keyframer data here to find out correct object orientation

    Chunk keyframer;
    keyframer.id = KFDATA;

    Chunk objtrack;
    objtrack.id = OBJECT_NODE_TAG;

    GotoChunk(mainchunk);
    if (FindChunk(keyframer, mainchunk)) 
    {   // keyframer chunk is present
        GotoChunk(keyframer);
        while (FindChunk(objtrack, keyframer))
        {
            ReadKeyframeData(objtrack);
            SkipChunk(objtrack);
        }
    }

    m_pos = 0;
#ifdef _2003
	wcscpy(m_objName, L"");
#else
    wcscpy_s(m_objName, 100, L"");
#endif
    return true;
}
/*
void a3DS::ReadLight(const Chunk &parent)
{
    float t;
    Vector3 v;
    Light light;
    light.SetName(m_objName);
    v.x = ReadFloat();
    v.y = ReadFloat();
    v.z = ReadFloat();
	light.SetPosition(v);
    Chunk chunk = ReadChunk();
    while (chunk.end <= parent.end)
    {
        switch (chunk.id)
        {
        case COLOR_24:
        case COLOR_F:
        case LIN_COLOR_F:
        case LIN_COLOR_24:
            light.SetColor(ReadColor(chunk));
            break;
        case SPOTLIGHT:
            v.x = ReadFloat();
            v.y = ReadFloat();
            v.z = ReadFloat();
            light.SetTarget(v);
            t = ReadFloat();
            light.SetHotspot(t);
            t = ReadFloat();
            light.SetFalloff(t);
            break;
		case LIT_INRANGE:
			light.SetAttenuationstart(ReadFloat());
			break;
		case LIT_OUTRANGE:
			light.SetAttenuationend(ReadFloat());
			break;
        default:
            break;
        }
        SkipChunk(chunk);
        if (chunk.end >= parent.end)
            break;
        chunk = ReadChunk();

    }
    m_lights.push_back(light);
}
*/
/*
void a3DS::ReadCamera(const Chunk &parent)
{
    Vector3 v,t;
    Camera camera;
    camera.SetName(m_objName);
    v.x = ReadFloat();
    v.y = ReadFloat();
    v.z = ReadFloat();
    t.x = ReadFloat();
    t.y = ReadFloat();
    t.z = ReadFloat();
	camera.SetPosition(v);
	camera.SetTarget(t);
	camera.SetBank(ReadFloat());
	camera.SetFOV(2400.0f/ReadFloat());
	Chunk chunk = ReadChunk();
    while (chunk.end <= parent.end)
    {
        switch (chunk.id)
        {
		case CAM_RANGES:
			camera.SetNearplane(ReadFloat());
			camera.SetFarplane(ReadFloat());
			break;
        default:
            break;
        }
        SkipChunk(chunk);
        if (chunk.end >= parent.end)
            break;
        chunk = ReadChunk();

    }
    m_cameras.push_back(camera);
}
*/
void a3DS::ReadMesh(const Chunk &parent, std::vector<Mesh>& meshes)
{
    unsigned short count, i;
    Vector3 p;
    Matrix4 m;
    Vector2 t;
    Mesh mesh;
    GotoChunk(parent);
    Chunk chunk = ReadChunk();
    while (chunk.end <= parent.end)
    {
        switch (chunk.id)
        {
        case TRI_VERTEXLIST:
            count = ReadShort();
            mesh.SetVertexArraySize(count);
            for (i=0; i < count; i++)
            {
                p.x = ReadFloat();
                p.y = ReadFloat();
                p.z = ReadFloat();
                mesh.SetVertex(p, i);
            }
            break;
        case TRI_FACEMAPPING:
            count = ReadShort();
            if (mesh.GetVertexCount() == 0)
                mesh.SetVertexArraySize(count);
            for (i=0; i < count; i++)
            {
                t.x = ReadFloat();
                t.y = ReadFloat();
                mesh.SetUV(t, i);
            }
            break;
        case TRI_FACELIST:
            ReadFaceList(chunk, mesh);
            break;
        case TRI_MATRIX:
            m._11 = ReadFloat();
            m._12 = ReadFloat();
            m._13 = ReadFloat();

            m._21 = ReadFloat();
            m._22 = ReadFloat();
            m._23 = ReadFloat();

            m._31 = ReadFloat();
            m._32 = ReadFloat();
            m._33 = ReadFloat();

            m._41 = ReadFloat();
            m._42 = ReadFloat();
            m._43 = ReadFloat();

            m._14 = 0.0f;
            m._24 = 0.0f;
            m._34 = 0.0f;
            m._44 = 1.0f;

            break;
        default:
            break;
        }
        SkipChunk(chunk);
        if (chunk.end >= parent.end)
            break;
        chunk = ReadChunk();
    }

	meshes.push_back(mesh);
}

void a3DS::ReadFaceList(const Chunk &chunk, Mesh &mesh)
{
    // variables 
    unsigned short count, t;    
    uint i;
    Tri tri;
    Chunk ch;
    wchar_t str[20];

    // consistency checks
    if (chunk.id != TRI_FACELIST)
    {
        ErrorMsg("a3DS::ReadFaceList - internal error: wrong chunk passed as parameter");
        return;
    }
    GotoChunk(chunk);
    tri.smoothingGroups = 1;
    // read the number of faces
    count = ReadShort();
    mesh.SetTriangleArraySize(count);
    for (i=0; i<count; i++)
    {
        tri.a = ReadShort();
        tri.b = ReadShort();
        tri.c = ReadShort();
        ReadShort();
        mesh.SetTri(tri, i);
    }
    // now read the optional chunks
    ch = ReadChunk();
    int mat_id;
    while (ch.end <= chunk.end)
    {
        switch (ch.id)
        {
        case TRI_MAT_GROUP:
            ReadASCIIZ(str, 20);
            mat_id = 0;//FindMaterial(str)->GetID();
            mesh.AddMaterial(mat_id);
            count = ReadShort();
            for (i=0; i<count; i++) 
            {
                t = ReadShort();
                mesh.GetTri(t).materialId = mat_id;
            }                
            break;
        case TRI_SMOOTH_GROUP:
            for (i=0; i<mesh.GetTriangleCount(); i++)
                mesh.GetTri(i).smoothingGroups = (ulong) ReadInt();
            break;
        }
        SkipChunk(ch);
        ch = ReadChunk();
    }
}

void a3DS::ReadMaterial(const Chunk &parent)
{
    // variables
    Chunk chunk;
    Chunk child;
    wchar_t str[30];
//    Material mat;
    short sh;

    GotoChunk(parent);

    chunk = ReadChunk();
    while (chunk.end <= parent.end)
    {
        switch (chunk.id)
        {
        case MAT_NAME:
            ReadASCIIZ(str, 30);
  //          mat.SetName(str);
            break;
        case MAT_AMBIENT:
            child = ReadChunk();
    //        mat.SetAmbientColor(ReadColor(child));
            break;
        case MAT_DIFFUSE:
            child = ReadChunk();
      //      mat.SetDiffuseColor(ReadColor(child));
            break;
        case MAT_SPECULAR:
            child = ReadChunk();
        //    mat.SetSpecularColor(ReadColor(child));
            break;
        case MAT_SHININESS:
            child = ReadChunk();
          //  mat.SetShininess(ReadPercentage(child));
            break;
        case MAT_TRANSPARENCY:
            child = ReadChunk();
            //mat.SetTransparency(ReadPercentage(child));
            break;
        case MAT_SHADING:
            sh = ReadShort();
            switch (sh)
            {
            case 0:
//                mat.SetShadingType(Wireframe);
                break;
            case 1:
  //              mat.SetShadingType(Flat);
                break;
            case 2:
    //            mat.SetShadingType(Gouraud);
                break;
            case 3:
      //          mat.SetShadingType(Phong);
                break;
            case 4:
        //        mat.SetShadingType(Metal);
                break;
            }
            break;
        case MAT_WIRE:
          //  mat.SetShadingType(Wireframe);
            break;
        case MAT_TEXMAP:
            //ReadMap(chunk, mat.GetTextureMap1());
            break;
        case MAT_TEX2MAP:
//            ReadMap(chunk, mat.GetTextureMap2());
            break;
        case MAT_OPACMAP:
  //          ReadMap(chunk, mat.GetOpacityMap());
            break;
        case MAT_BUMPMAP:
    //        ReadMap(chunk, mat.GetBumpMap());
            break;
        case MAT_SPECMAP:
      //      ReadMap(chunk, mat.GetSpecularMap());
            break;
        case MAT_REFLMAP:
            child = ReadChunk();
        //    mat.GetReflectionMap().strength = ReadPercentage(child);
            SkipChunk(child);
            child = ReadChunk();
            if (child.id != MAT_MAPNAME)
            {
                ErrorMsg("a3DS::ReadMaterial - error, expected chunk not found");
                return;
            }
            ReadASCIIZ(str, 30);
            if (wcscmp(str, L"") == 0)
//                wcscpy(mat.GetReflectionMap().mapName, L"auto");
            break;
        }
        
        SkipChunk(chunk);
        chunk = ReadChunk();
    }
//    m_materials.push_back(mat);
  //  m_materials[m_materials.size()-1].SetID(m_materials.size()-1);
}

void a3DS::ReadMap(const Chunk &chunk, Map& map)
{
    Chunk child;
    wchar_t str[20];
    GotoChunk(chunk);
    child = ReadChunk();
    while (child.end <= chunk.end)
    {
        switch (child.id)
        {
        case INT_PERCENTAGE:
            map.strength = ReadPercentage(child);
            break;
        case MAT_MAPNAME:
            ReadASCIIZ(str, 20);
#ifdef _2003
			wcscpy(map.mapName, str);
#else
            wcscpy_s(map.mapName, 255, str);
#endif
            break;
		case MAT_MAP_USCALE:
            map.uScale = ReadFloat();
            break;
        case MAT_MAP_VSCALE:
            map.vScale = ReadFloat();
            break;
        case MAT_MAP_UOFFSET:
            map.uOffset = ReadFloat();
            break;
        case MAT_MAP_VOFFSET:
            map.vOffset = ReadFloat();
            break;
        case MAT_MAP_ANG:
            map.angle = ReadFloat();
            break;
        }
        SkipChunk(child);
        child = ReadChunk();
    }
}

void a3DS::ReadKeyframeData(const Chunk &parent)
{
    uint frames = 0;

    Chunk node_hdr;
    node_hdr.id = NODE_HDR;

    wchar_t str[20];
    
    GotoChunk(parent);
    if (!FindChunk(node_hdr, parent))
        return;
    GotoChunk(node_hdr);
    ReadASCIIZ(str, 19);
//    mesh = FindMesh(str);
  //  if (mesh == 0)
    //    return;
    GotoChunk(parent);

    // read the pivot
    Vector3 pivot;

    Chunk pivotchunk;
    pivotchunk.id = PIVOT;
    if (FindChunk(pivotchunk, parent))
    {
        GotoChunk(pivotchunk);
        pivot.x = ReadFloat();
        pivot.y = ReadFloat();
        pivot.z = ReadFloat();
    }
    GotoChunk(parent);

    // read frame 0 from the position track
    Vector3 pos;

    frames = 0;

    Chunk poschunk;
    poschunk.id = POS_TRACK_TAG;
    if (FindChunk(poschunk, parent))
    {
        GotoChunk(poschunk);
        // read the trackheader structure
        ReadShort();
        ReadInt();
        ReadInt();
        frames = ReadInt();
        if (frames > 0)
        {
            ReadKeyheader();
            pos.x = ReadFloat();
            pos.y = ReadFloat();
            pos.z = ReadFloat();
        }
    }
    GotoChunk(parent);

    // now read the rotation track
    Vector3 rot = zero3;

    Chunk rotchunk;
    rotchunk.id = ROT_TRACK_TAG;

    frames = 0;
    if (FindChunk(rotchunk, parent))
    {
        GotoChunk(rotchunk);
        // read the trackheader structure
        ReadShort();
        ReadInt();
        ReadInt();
        frames = ReadInt();
        if (frames > 0)
        {
            ReadKeyheader();
            rot.x = ReadFloat();
            rot.y = ReadFloat();
            rot.z = ReadFloat();
            ReadFloat();
        }
    }
    GotoChunk(parent);

    // now read the scaling chunk
    Vector3 scale;
    scale.x = 1;
    scale.y = 1;
    scale.z = 1;

    Chunk scalechunk;
//    scalechunk.id = SC_TRACK_TAG;

    frames = 0;

    if (FindChunk(scalechunk, parent))
    {
        GotoChunk(scalechunk);
        // read the trackheader structure
        ReadShort();
        ReadInt();
        ReadInt();
        frames = ReadInt();
        if (frames > 0)
        {
            ReadKeyheader();
            scale.x = ReadFloat();
            scale.y = ReadFloat();
            scale.z = ReadFloat();
        }
    }
    GotoChunk(parent);
}

long a3DS::ReadKeyheader()
{
    long frame;
    frame = ReadInt();
    short opts = ReadShort();
    if (opts & 32768) // 32768 is 1000000000000000 binary
    {  // tension is present
        ReadFloat();
    }
    if (opts & 16384) // 16384 is 0100000000000000 binary
    {  // continuity is present
        ReadFloat();
    }
    if (opts & 8192)
    {  // bias info present
        ReadFloat();
    }
    if (opts & 4096)
    {  // "ease to" present
        ReadFloat();
    }
    if (opts & 2048)
    {  // "ease from" present
        ReadFloat();
    }
    return frame;
}
