#pragma once
// includes
#include <vector>
#include <string>
#include <map>
#include "../engine/structs.h"

class Mesh;
//---------------------------------------------------------

typedef unsigned int uint;
typedef unsigned char byte;


class Object3d
{
public:
    // the default constructor, initilializes the m_name here
    Object3d();
    // the destructor frees memory (m_name)
    virtual ~Object3d();
    // call this to get the name of the object
    virtual const std::wstring& GetName();
    
    // this methods should not be used by the "user", they're used internally to fill the class
    // with valid data when reading from file. If you're about to add an importer for another format you'
    // have to use these methods
    // call this to set the name of the object
    virtual void SetName(const std::wstring& value);
    // returns true if the object's name is the name passed as parameter
    bool IsObject(const std::wstring &name);
protected:
    // the name of the object
    std::wstring m_name;
};

//------------------------------------------------
/*
class Material : public Object3d
{
public:     
    // the default constructor, does the initialization
    Material();
    // the destructor
    virtual ~Material();
    // returns the material ID    
    uint GetID();
    // returns the pointer to teh texture map 1
    Map& GetTextureMap1();
    // returns the pointer to the texture map 2
    Map& GetTextureMap2();
    // returns the pointer to teh opacity map
    Map& GetOpacityMap();
    // returns the pointer to the specular (gloss) map
    Map& GetSpecularMap();
    // returns the pointer to the bump map
    Map& GetBumpMap();
    // returns the pointer to the reflection map
    Map& GetReflectionMap();
    // returns the ambient color of the material
    Color3 GetAmbientColor();
    // returns the diffuse color of the material
    Color3 GetDiffuseColor();
    // returns the specular color of the material
    Color3 GetSpecularColor();
    // returns the shininess of the material, ranging from 0(matte) to 1(shiny)
    float GetShininess();
    // returns the transparency of the material, ranging from 1(fully transparent) to 0(opaque)
    float GetTransparency();
    // returns the type of shading, see Shading type
    Shading GetShadingType();

    // this methods should not be used by the "user", they're used internally to fill the class
    // with valid data when reading from file. If you're about to add an importer for another format you'
    // have to use these methods
    // sets the material ID to "value"
    void SetID(uint value);
    // call this to set the ambient color of the material
    void SetAmbientColor(const Color3 &color);
    // sets the diffuse color of the material
    void SetDiffuseColor(const Color3 &color);
    // sets the specular color of the material
    void SetSpecularColor(const Color3 &color);
    // sets the shininess of the material
    void SetShininess(float value);
    // sets the transparency of the material
    void SetTransparency(float value);
    // sets the shading type
    void SetShadingType(Shading shading);
protected:
    // the unique material ID
    int m_id;
    // the first texture map
    Map m_texMap1;
    // the second texture map
    Map m_texMap2;
    // the opacity map
    Map m_opacMap;
    // the reflection map
    Map m_reflMap;
    // the bump map
    Map m_bumpMap;
    // specular map
    Map m_specMap;
    // material ambient color
    Color3 m_ambient;
    // material diffuse color
    Color3 m_diffuse;
    // material specular color
    Color3 m_specular;
    // shininess
    float m_shininess;
    // transparency
    float m_transparency;
    // the shading type for the material
    Shading m_shading;
};
*/
//------------------------------------------------

/*
class Camera : public Object3d
{
public:
    // the default constructor
    Camera();
    // the destructor
    virtual ~Camera();
    // clears the data the class holds    
    void Clear();
    // sets the position of the camera - for internal use
    void SetPosition(Vector3 vec);
    // returns the position of the camera
    Vector3 GetPosition();
    // sets the target of the camera - internal use
    void SetTarget(Vector3 target);
    // returns the target of the camera
    Vector3 GetTarget();
    // sets the fov - internal use    
    void SetFOV(float value);
    // returns the fov 
    float GetFOV();
    // sets the bank - internal use    
    void SetBank(float value);
    // returns the bank
    float GetBank();
    // sets the near plane - internal use    
    void SetNearplane(float value);
    // returns the near plane distance
    float GetNearplane();
    // sets the far plane - internal use    
    void SetFarplane(float value);
    // returns the far plane distance
    float GetFarplane();
protected:
    Vector3 m_pos;
    Vector3 m_target;
	float m_bank;
    float m_fov;
	float m_near;
	float m_far;
};
*/
//------------------------------------------------

/*class Light : public Object3d
{
public:
    // the default constructor
    Light();
    // the destructor
    virtual ~Light();
    // clears the data the class holds    
    void Clear();
    // sets the position of the light source - for internal use
    void SetPosition(Vector3 vec);
    // returns the position of the light source
    Vector3 GetPosition();
    // sets the color of the light - for internal use
    void SetColor(Color3 color);
    // returns the color of the light
    Color3 GetColor();
    // sets whether the light is a spotlight or not - internal use
    void SetSpotlight(bool value);
    // returns true if the light is a spotlight
    bool GetSpotlight();
    // sets the target of the light - internal use
    void SetTarget(Vector3 target);
    // returns the target of the spotlight
    Vector3 GetTarget();
    // sets the hotspot - internal use    
    void SetHotspot(float value);
    // returns the hotspot
    float GetHotspot();
    // sets falloff - internal use
    void SetFalloff(float value);
    // returns falloff
    float GetFalloff();
    // sets attenuationstart - internal use
    void SetAttenuationstart(float value);
    // returns attenuationstart
    float GetAttenuationstart();
    // sets attenuationend - internal use
    void SetAttenuationend(float value);
    // returns attenuationend
    float GetAttenuationend();
protected:
    Vector3 m_pos;
    Color3 m_color;
    bool m_spotlight;
    Vector3 m_target;
    float m_hotspot;
    float m_falloff;
	float m_attenuationstart;
	float m_attenuationend;
};
*/
//------------------------------------------------

class Importer
{
public:
    // the default constructor
    Importer();
    // the destructor
    virtual ~Importer();
    // reads the model from a file, must be overriden by the child classes
    virtual bool LoadFile(const wchar_t *filename, std::vector<Mesh>& meshes) = 0;
    // returns the number of meshes in the scene
    uint GetMeshCount();
    // returns the number of lights in the scene
//    uint GetLightCount();
    // returns the number of materials in the scene
  //  uint GetMaterialCount();
	// returns the number of cameras in the scene
//	uint GetCameraCount();
    // returns a pointer to a mesh
    Mesh& GetMesh(uint index);
    // returns a pointer to a camera at a given index
//    Camera& GetCamera(uint index);
    // returns a pointer to a light at a given index
//    Light& GetLight(uint index);
    // returns the pointer to the material
//    Material& GetMaterial(uint index);
    // returns the pointer to the material with a given name, or NU if the material was not found
  //  Material* FindMaterial(const std::wstring &name);
    // returns the pointer to the mesh with a given name, or NU if the mesh with such name 
    // is not present in the scene
    Mesh* FindMesh(const std::string &name);
    // returns the pointer to the camera with a given name, or NU if not found
    //Camera* FindCamera(const std::wstring &name);
    // returns the pointer to the light with a given name, or NU if not found
//    Light* FindLight(const std::wstring &name);
    // sets the optimisation level to a given value
    void SetOptimisationLevel(OptimisationLevel value);
    // returns the current optimisation level
    OptimisationLevel GetOptimisationLevel();
protected:
    // the cameras found in the scene
//    std::vector<Camera> m_cameras;
    // the lights found in the scene
//    std::vector<Light> m_lights;
    // the materials in the scene
//    std::vector<Material> m_materials;
    // level of optimisation to perform on the meshes
    OptimisationLevel m_optevel;
    // clears all data.
    virtual void Clear();
};
//------------------------------------------------

class a3DS : public Importer
{
public:
    // the default contructor
    a3DS();
    // destructor
    virtual ~a3DS();
    // load 3ds file 
	virtual bool LoadFile(const wchar_t *filename, std::vector<Mesh>& meshes);
protected:
    // used internally for reading
    wchar_t m_objName[100];
    // true if end of file is reached
    bool m_eof;
    // buffer for loading, used for speedup
    unsigned char *m_buffer;
    // the size of the buffer
    uint m_bufferSize;
    // the current cursor position in the buffer
    uint m_pos;

    // reads a short value from the buffer
    short ReadShort();
    // reads an int value from the buffer
    int ReadInt();
    // reads a char from the buffer
    char ReadChar();
    //reada a floatvalue from the buffer
    float ReadFloat();
    //reads an unsigned byte from the buffer
    byte ReadByte();
    //reads an asciiz string 
    int ReadASCIIZ(wchar_t *buf, int max_count);
    // seek wihtin the buffer
    void Seek(int offset, int origin);
    // returns the position of the cursor
    uint Pos();

    // read the chunk and return it.
    Chunk ReadChunk();
    // read until given chunk is found
    bool FindChunk(Chunk &target, const Chunk &parent);
    // skip to the end of chunk "chunk"
    void SkipChunk(const Chunk &chunk);
    // goes to the beginning of the data in teh given chunk
    void GotoChunk(const Chunk &chunk);

    // the function read the color chunk (any of the color chunks)
    Color3 ReadColor(const Chunk &chunk);
    // the function that read the percentage chunk and returns a float from 0 to 1
    float ReadPercentage(const Chunk &chunk);
    // this is where 3ds file is being read
	bool Read3DS(std::vector<Mesh>& meshes);
    // read a light chunk 
//    void ReadLight(const Chunk &parent);
	// read a camera chunk 
	void ReadCamera(const Chunk &parent);
    // read a trimesh chunk
	void ReadMesh(const Chunk &parent, std::vector<Mesh>& meshes);
    // reads the face list, face materials, smoothing groups... and fill rthe information into the mesh
    void ReadFaceList(const Chunk &chunk, Mesh &mesh);
    // reads the material
    void ReadMaterial(const Chunk &parent);
    // reads the map info and fills the given map with this information
    void ReadMap(const Chunk &chunk, Map& map);
    // reads keyframer data of the OBJECT_NODE_TAG chunk
    void ReadKeyframeData(const Chunk &parent);
    // reads the keyheader structure from the current offset and returns the frame number
    long ReadKeyheader();
};

//---------------------------------------------------------
