#pragma once

struct Vector3
{
	Vector3() { x=0,y=0,z=0; }
    float x;
    float y;
    float z;
};

struct Vector2
{
    float x;
    float y;
};

struct Color3
{   
    float r;
    float g;
    float b;
};

//------------------------------------------------

struct Triangle
{
    unsigned short a;
    unsigned short b;
    unsigned short c;
};

struct Matrix4
{
    float _11, _12, _13, _14;
    float _21, _22, _23, _24;
    float _31, _32, _33, _34;
    float _41, _42, _43, _44;
};

struct Triangle2
{
    Vector3 vertices[3];
    Vector3 vertexNormals[3];
    Vector2 textureCoords[3];
    Vector3 faceNormal;
    unsigned int materialId;
};

// a structure for a texture map
struct Map
{
    // the strength of the texture map
    float strength;
    // the file name of the map. only 8.3 format in 3ds files :(
    wchar_t mapName[255];
    float uScale;
    float vScale;
    float uOffset;
    float vOffset;
    float angle;
};


struct Chunk
{
    unsigned short id;
    unsigned int start;
    unsigned int end;
};

struct Tri
{
    unsigned short a;
    unsigned short b;
    unsigned short c;
    unsigned long smoothingGroups;
    Vector3 normal;
    Vector3 tangent;
    Vector3 binormal;
    unsigned int materialId;
};
enum Shading { Wireframe, Flat, Gouraud, Phong, Metal };
enum OptimisationLevel { None, Simple, Full };
