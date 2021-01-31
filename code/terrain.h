#ifndef TERRAIN_H
#define TERRAIN_H

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <stdint.h>

struct Vertex
{
    float  x,  y,  z;
    //D3DCOLOR color;
    float nx, ny, nz;
    static const DWORD FVF = D3DFVF_XYZ /* | D3DFVF_DIFFUSE */| D3DFVF_NORMAL; 
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
    D3DMATERIAL9 mtrl;
};

void SetMapInfo(Terrain* terrain,
                int numVrow, int numVcol,
                int cellSpace, float heightS);
void SetHeightMapInfo(uint8_t height[], Terrain* terrain);
void GenVertices(Terrain* terrain, IDirect3DDevice9* device);
void GenIndices(Terrain* terrain, IDirect3DDevice9* device);
D3DXVECTOR3 GetVertexNormal(int x, int y, Terrain* terrain);
void UpdateHeightMapWithMousePos(Terrain* terrain, int x, int y,
                                 BOOL value, IDirect3DDevice9* device);
int getHeightmapEntry(int row, int col, Terrain* terrain);
float getHeight(Terrain* terrain, float x, float z);
float Lerp(float a, float b, float t);

#endif
