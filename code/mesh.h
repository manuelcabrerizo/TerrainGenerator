#ifndef MESH_H
#define MESH_H

#include <d3d9.h>
#include <d3dx9.h>
#include <stdint.h>

struct MeshVertex
{
    float  x,  y,  z;
    float u, v;
    float nx, ny, nz;
    static const DWORD FVF = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_NORMAL; 
};

struct IndexBuffer
{
    int a, b, c;
};

struct VertexBuffer
{
    D3DXVECTOR3 vertice;
    D3DXVECTOR2 textureCoord;
    D3DXVECTOR3 normal;   
};

struct Mesh
{
    int numVertices;
    int numTexCoords;
    int numNormals;
    int numIndex; 
    D3DXVECTOR3* vertices;
    D3DXVECTOR2* textureCoords;
    D3DXVECTOR3* normals;

    IndexBuffer* vertexIndex;
    IndexBuffer* textureIndex;
    IndexBuffer* normalIndex;

    VertexBuffer* vertexBuffer;
    IDirect3DVertexBuffer9* D3DvertexBuffer;
};

void LoadOBJFile(IDirect3DDevice9* device, Mesh* mesh, const char* filePhat);

#endif
