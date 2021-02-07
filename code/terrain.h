#ifndef TERRAIN_H
#define TERRAIN_H

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <stdint.h>

struct Vertex
{
    float  x,  y,  z;
    float nx, ny, nz;
    float u, v;
    static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1; 
};

struct Terrain 
{
    int numVertexRow;
    int numVertexCol;
    int numCellRow;
    int numCellCol;
    int cellSpacing;
    int numVertices;
    int numTrinalges;
    float heightScale;
    uint8_t* heightMap;
    IDirect3DVertexBuffer9* VB;
    IDirect3DIndexBuffer9*  IB;
    IDirect3DTexture9* tex;
    D3DMATERIAL9 mtrl;
};


#pragma pack(push, 1)
struct bitmapHeader
{
    uint16_t fileType;
    uint32_t fileSize;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t bitmapOffset;
    uint32_t size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bitsPerPixel;
};
#pragma pack(pop)

struct texture_t
{
    uint32_t* pixels;
    int width;
    int height;
};


void SetMapInfo(Terrain* terrain,
                int numVrow, int numVcol,
                int cellSpace, float heightS);
void SetHeightMapInfo(uint8_t height[], Terrain* terrain);
void GenVertices(Terrain* terrain, IDirect3DDevice9* device);
void GenIndices(Terrain* terrain, IDirect3DDevice9* device);
D3DXVECTOR3 GetVertexNormal(int x, int y, Terrain* terrain);
void UpdateHeightMapWithMousePos(Terrain* terrain, int x, int y,
                                 BOOL value, float deltaTime, IDirect3DDevice9* device,
                                 D3DXVECTOR3 directionToLight);
int getHeightmapEntry(int row, int col, Terrain* terrain);
float getHeight(Terrain* terrain, float x, float z);
float Lerp(float a, float b, float t);
void GenSenBaseHeight(Terrain* terrain);
void GenerateTexture(Terrain* terrain, IDirect3DDevice9* device, D3DXVECTOR3 directionToLight);
void UpdateTexture(int x, int y, Terrain* terrain, D3DXVECTOR3 directionToLight);
void LightTerrain(Terrain* terrain, D3DXVECTOR3 directionToLight);
void UpdateLightTerrain(int x, int y, Terrain* terrain, D3DXVECTOR3 directionToLight);
float ComputeShade(Terrain* terrain, int x, int y, D3DXVECTOR3 directionToLight);
texture_t LoadBMP(const char* filename);


#endif
