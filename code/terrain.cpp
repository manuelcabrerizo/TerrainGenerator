#include "terrain.h"

//#define D3DCOLOR_XRGB(r,g,b) D3DCOLOR_ARGB(0x33,r,g,b)

void SetMapInfo(Terrain* terrain, int numVrow, int numVcol, int cellSpace, float heightS)
{
    terrain->numVertexRow = numVrow;
    terrain->numVertexCol = numVcol;
    terrain->cellSpacing  = cellSpace;
    terrain->numCellRow   = terrain->numVertexRow - 1;
    terrain->numCellCol   = terrain->numVertexCol - 1;
    terrain->numVertices  = terrain->numVertexRow * terrain->numVertexCol; 
    terrain->numTrinalges = terrain->numCellRow   * terrain->numCellCol * 2;
    terrain->heightScale  = heightS;
    terrain->VB = 0;
    terrain->IB = 0;    
    if(terrain->heightMap)
    {
        VirtualFree(terrain->heightMap, 0, MEM_RELEASE);
    }
    terrain->heightMap = (uint8_t*)VirtualAlloc(0, terrain->numVertices * sizeof(uint8_t), MEM_COMMIT, PAGE_READWRITE);
    
    ZeroMemory(&terrain->mtrl, sizeof(terrain->mtrl));
    terrain->mtrl.Diffuse  = D3DXCOLOR(0.0f, 0.5f, 0.0f, 1.0f); // red
    terrain->mtrl.Ambient  = D3DXCOLOR(0.0f, 0.5f, 0.0f, 1.0f); // red
    terrain->mtrl.Specular = D3DXCOLOR(0.0f, 0.5f, 0.0f, 1.0f); // red
    terrain->mtrl.Emissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f); // no emission
    terrain->mtrl.Power    = 5.0f;
}

void UpdateHeightMapWithMousePos(Terrain* terrain, int x, int y, BOOL value, IDirect3DDevice9* device)
{
    int index = (y * terrain->numVertexRow) + x;
    if(value == TRUE)
    {
        terrain->heightMap[index] += 3;
    }
    else
    {
        terrain->heightMap[index]--;
    }
    Vertex* v = 0;    
    terrain->VB->Lock(0, 0, (void**)&v, 0);

    D3DXVECTOR3 normals = GetVertexNormal(x, y, terrain);
    Vertex vertex = {
        (float)x * (float)terrain->cellSpacing,
        (float)terrain->heightMap[(y * terrain->numVertexRow) + x] * terrain->heightScale,
        (float)y * (float)terrain->cellSpacing,
        normals.x,
        normals.y,
        normals.z
    };
    v[(y * terrain->numVertexRow) + x] = vertex;
    terrain->VB->Unlock();
}

void SetHeightMapInfo(uint8_t height[], Terrain* terrain)
{
    for(int y = 0; y < terrain->numVertexCol; y++)
    {
        for(int x = 0; x < terrain->numVertexRow; x++)
        {
            int index = (y * terrain->numVertexRow) + x;
            terrain->heightMap[index] = height[index]; 
        }  
    }
}

void GenVertices(Terrain* terrain, IDirect3DDevice9* device)
{
    if(terrain->VB == NULL)
    {
        OutputDebugString("LOADING VB\n");
    }
    device->CreateVertexBuffer(terrain->numVertices * sizeof(Vertex),
                               D3DUSAGE_WRITEONLY,
                               Vertex::FVF,
                               D3DPOOL_MANAGED,
                               &terrain->VB,
                               0);
    Vertex* v = 0;    
    terrain->VB->Lock(0, 0, (void**)&v, 0);
    for(int y = 0; y < terrain->numVertexCol; y++)
    {
       for(int x = 0; x < terrain->numVertexRow; x++)
       {
           /*
            D3DCOLOR color;  
            if(x % 2 == 0)
            {
                color = D3DCOLOR_XRGB(0, 255, 0);
            }
            else if(y % 2 == 0)
            {
                color = D3DCOLOR_XRGB(0, 255, 0);
            }
            else
            {
                color = D3DCOLOR_XRGB(0, 255, 0);
            }
            */
            D3DXVECTOR3 normals = GetVertexNormal(x, y, terrain);

            Vertex vertex = {
                (float)x * (float)terrain->cellSpacing,
                (float)terrain->heightMap[(y * terrain->numVertexRow) + x] * terrain->heightScale,
                (float)y * (float)terrain->cellSpacing,
                normals.x,
                normals.y,
                normals.z
            };
            v[(y * terrain->numVertexRow) + x] = vertex;
       }  
    }
    terrain->VB->Unlock();
    if(terrain->VB == NULL)
    {
        OutputDebugString("ERROR LOADING VERITICES\n");
    }
    else
    {
        OutputDebugString("SUCCEED LOADING VERITICES\n");
    }

}

void GenIndices(Terrain* terrain, IDirect3DDevice9* device)
{
    device->CreateIndexBuffer(terrain->numTrinalges * 3 * sizeof(WORD),
                              D3DUSAGE_WRITEONLY,
                              D3DFMT_INDEX16,
                              D3DPOOL_MANAGED,
                              &terrain->IB,
                              0);
    WORD* indices = 0;
    terrain->IB->Lock(0, 0, (void**)&indices, 0);
    int baseIndex = 0;
    for(int y = 0; y < terrain->numCellCol; y++)
    {
        for(int x = 0; x < terrain->numCellRow; x++)
        {
            indices[baseIndex + 2]     = y * terrain->numVertexRow +  x;  
            indices[baseIndex + 1] = y * terrain->numVertexRow + (x + 1);
            indices[baseIndex + 0] = (y + 1) * terrain->numVertexRow + x;

            indices[baseIndex + 5] = (y + 1) * terrain->numVertexRow + x;
            indices[baseIndex + 4] = y * terrain->numVertexRow + (x + 1);
            indices[baseIndex + 3] = (y + 1) * terrain->numVertexRow + (x + 1);
            baseIndex += 6;
        }
    }
    terrain->IB->Unlock();
    if(terrain->IB == NULL)
    {
        OutputDebugString("ERROR LOADING INDICES\n");
    }

}

float GetYComponent(int x, int z, Terrain* terrain)
{
    return (float)terrain->heightMap[(z * terrain->numVertexRow) + x];
}

D3DXVECTOR3 GetVertexNormal(int x, int y, Terrain* terrain)
{
    float heightA = GetYComponent(x, y, terrain);
    float heightB = GetYComponent(x + 1, y, terrain);
    float heightC = GetYComponent(x, y + 1, terrain);

    D3DXVECTOR3 u((float)terrain->cellSpacing, heightB - heightA, 0.0f);
    D3DXVECTOR3 v(0.0f, heightC - heightA, (float)terrain->cellSpacing);

    D3DXVECTOR3 n;
    D3DXVec3Cross(&n, &u, &v);
    D3DXVec3Normalize(&n, &n);
    return n;     
}

