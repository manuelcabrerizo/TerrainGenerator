#include "terrain.h"
#include <stdio.h>
#include <math.h>

#define global_variable static

global_variable float heightMapFloat[256 * 256] = {};


texture_t LoadBMP(const char* filename)
{
    bitmapHeader header;
    texture_t texture;
    FILE* file = fopen(filename, "rb");
    if(file != NULL)
    {
        fread(&header, sizeof(header), 1, file);
        texture.pixels = (uint32_t*)malloc(sizeof(uint32_t) * header.width * header.height);
        fseek(file, header.bitmapOffset, SEEK_SET);
        fread(texture.pixels, sizeof(uint32_t), header.width * header.height, file);
        texture.width = header.width;
        texture.height = header.height;
    }
    fclose(file);
    return texture;
}


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
    terrain->mtrl.Diffuse  = D3DXCOLOR(0.0f, 0.6f, 0.0f, 1.0f); // red
    terrain->mtrl.Ambient  = D3DXCOLOR(0.0f, 0.4f, 0.0f, 1.0f); // red
    terrain->mtrl.Specular = D3DXCOLOR(0.0f, 0.4f, 0.0f, 1.0f); // red
    terrain->mtrl.Emissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f); // no emission
    terrain->mtrl.Power    = 5.0f;
}

uint8_t SetProperHeight(uint8_t value)
{
    if(value > 255)
    {
        value = 255;
    }
    if(value < 0)
    {
        value = 0;
    }
    return value;
}


Vertex CreateVertex(Terrain* terrain, int x, int y, float nx, float ny, float nz)
{   
    int index = (y * terrain->numVertexRow) + x;
    float uCoordIncrementSize = 1.0f / (float)terrain->numCellRow;
    float vCoordIncrementSize = 1.0f / (float)terrain->numCellCol;
    Vertex vertex = {
        (float)x * (float)terrain->cellSpacing,
        (float)terrain->heightMap[index] * terrain->heightScale,
        (float)y * (float)terrain->cellSpacing,
        nx,
        ny,
        nz,
        (float)x * uCoordIncrementSize,
        (float)y * vCoordIncrementSize
    };
    return vertex;
}


void UpdateHeightMapWithMousePos(Terrain* terrain, int x, int y, BOOL value, float deltaTime, IDirect3DDevice9* device, D3DXVECTOR3 directionToLight)
{

    if(x >= terrain->numVertexRow)
        x = terrain->numVertexRow - 1;
    if(y >= terrain->numVertexCol)
        y = terrain->numVertexCol - 1;
    if(x < 0)
        x = 0;
    if(y < 0)
        y = 0; 
    int index = (y * terrain->numVertexRow) + x;
    int index1 = (y * terrain->numVertexRow) + (x + 1);
    int index2 = ((y + 1) * terrain->numVertexRow) + x;
    int index3 = ((y + 1) * terrain->numVertexRow) + (x + 1);
    if(index1 >= terrain->numVertices)
    {
        index1 = terrain->numVertices - 1;
    }
    if(index2 >= terrain->numVertices)
    {
        index2 = terrain->numVertices - 1;
    }
    if(index3 >= terrain->numVertices)
    {
        index3 = terrain->numVertices - 1;
    }

    float incrementFactor = 50.0f;

    if(value == TRUE)
    {
        if(heightMapFloat[index] < 254)
            heightMapFloat[index] += (incrementFactor * deltaTime);
        if(heightMapFloat[index1] < 254)
            heightMapFloat[index1] += (incrementFactor * deltaTime);
        if(heightMapFloat[index2] < 254)
            heightMapFloat[index2] += (incrementFactor * deltaTime);
        if(heightMapFloat[index3] < 254)
            heightMapFloat[index3] += (incrementFactor * deltaTime);
    }
    else
    {
        if(heightMapFloat[index] > 1)
            heightMapFloat[index] -= (incrementFactor * deltaTime);
        if(heightMapFloat[index1] > 1)
            heightMapFloat[index1] -= (incrementFactor * deltaTime);
        if(heightMapFloat[index2] > 1)
            heightMapFloat[index2] -= (incrementFactor * deltaTime);
        if(heightMapFloat[index3] > 1)
            heightMapFloat[index3] -= (incrementFactor * deltaTime);
    }

    terrain->heightMap[index]  = heightMapFloat[index];
    terrain->heightMap[index1] = heightMapFloat[index1];
    terrain->heightMap[index2] = heightMapFloat[index2];
    terrain->heightMap[index3] = heightMapFloat[index3];

    Vertex* v = 0;    
    terrain->VB->Lock(0, 0, (void**)&v, 0);

    D3DXVECTOR3 normals = GetVertexNormal(x, y, terrain);
    D3DXVECTOR3 normals1 = GetVertexNormal(x + 1, y, terrain);
    D3DXVECTOR3 normals2 = GetVertexNormal(x, y + 1, terrain);
    D3DXVECTOR3 normals3 = GetVertexNormal(x + 1, y + 1, terrain);

    Vertex vertex =  CreateVertex(terrain, x, y, normals.x, normals.y, normals.z);
    Vertex vertex1 = CreateVertex(terrain, (x + 1), y, normals1.x, normals1.y, normals1.z);
    Vertex vertex2 = CreateVertex(terrain, x, (y + 1), normals2.x, normals2.y, normals2.z);
    Vertex vertex3 = CreateVertex(terrain, (x + 1), (y + 1), normals3.x, normals3.y, normals3.z);

    v[index] = vertex;
    v[index1] = vertex1;
    v[index2] = vertex2;
    v[index3] = vertex3;

    terrain->VB->Unlock();

    UpdateTexture(x, y, terrain, directionToLight);
}

void SetHeightMapInfo(uint8_t height[], Terrain* terrain)
{

    texture_t baseMap = LoadBMP("./data/terrain.bmp");

    for(int y = 0; y < terrain->numVertexCol; y++)
    {
        for(int x = 0; x < terrain->numVertexRow; x++)
        {
            int index = (y * terrain->numVertexRow) + x;
            uint32_t pixel = (uint32_t)baseMap.pixels[index];
            uint8_t no_alpha = (uint8_t)(pixel & 0xFFFFFF);
            terrain->heightMap[index] = no_alpha;
            heightMapFloat[index] = (float)no_alpha;
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
                               D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
                               Vertex::FVF,
                               D3DPOOL_DEFAULT,
                               &terrain->VB,
                               0);

    Vertex* v = 0;    
    terrain->VB->Lock(0, 0, (void**)&v, 0);
    for(int y = 0; y < terrain->numVertexCol; y++)
    {
       for(int x = 0; x < terrain->numVertexRow; x++)
       {
            D3DXVECTOR3 normals = GetVertexNormal(x, y, terrain);
            Vertex vertex = CreateVertex(terrain, x, y, normals.x, normals.y, normals.z);
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
    if(x >= terrain->numVertexRow)
        x = terrain->numVertexRow - 1;
    if(z >= terrain->numVertexCol)
        z = terrain->numVertexCol - 1;
    if(x < 0)
        x = 0;
    if(z < 0)
        z = 0; 
    return (float)terrain->heightMap[(z * terrain->numVertexRow) + x];
}

D3DXCOLOR HeightColor(Terrain* terrain, int x, int y)
{
    D3DXCOLOR c;
    float height = GetYComponent(x, y, terrain);

    if(height < 10.0f ) c = D3DCOLOR_XRGB(124, 197, 118);
    else if(height < 40.5f) c = D3DCOLOR_XRGB(0, 166, 81);
    else if(height < 60.0f) c = D3DCOLOR_XRGB(25, 123, 48);
    else if(height < 100.5f) c = D3DCOLOR_XRGB(115, 100,  87);
    else                     c = D3DCOLOR_XRGB(255, 255, 255);
    return c;

}


void GenerateTexture(Terrain* terrain, IDirect3DDevice9* device,  D3DXVECTOR3 directionToLight)
{
    HRESULT hr = 0;

    int texWidth = terrain->numCellRow;
    int texHeight = terrain->numCellCol;

    hr = D3DXCreateTexture(
            device,
            texWidth, texHeight,
            0, 0,
            D3DFMT_X8R8G8B8,
            D3DPOOL_MANAGED,
            &terrain->tex);

    if(FAILED(hr))
    {
        OutputDebugString("FAILED creating TEXTURE\n");
        return;
    }
    D3DSURFACE_DESC textureDesc;
    terrain->tex->GetLevelDesc(0, &textureDesc);

    if(textureDesc.Format != D3DFMT_X8R8G8B8)
    {
        OutputDebugString("FAILED seting the FORMAT of the TEXTURE\n");
        return;
    }

    D3DLOCKED_RECT lockedRect;
    terrain->tex->LockRect(0, &lockedRect, 0, 0);
    DWORD* imageData = (DWORD*)lockedRect.pBits;
    
    for(int y = 0; y < texHeight; y++)
    {
        for(int x = 0; x < texWidth; x++)
        {
            imageData[(y * lockedRect.Pitch / 4) + x] = HeightColor(terrain, x, y);
        }
    }
    terrain->tex->UnlockRect(0);
    LightTerrain(terrain, directionToLight);
    hr = D3DXFilterTexture(terrain->tex, 0, 0, D3DX_DEFAULT);
    if(FAILED(hr))
    {
        OutputDebugString("FAILED FILTERING the TEXTURE\n");
    }
}
void UpdateTexture(int x, int y, Terrain* terrain,  D3DXVECTOR3 directionToLight)
{
    int texWidth = terrain->numCellRow;
    int texHeight = terrain->numCellCol;
    D3DLOCKED_RECT lockedRect;
    terrain->tex->LockRect(0, &lockedRect, 0, 0);
    DWORD* imageData = (DWORD*)lockedRect.pBits;
    imageData[(y * lockedRect.Pitch / 4) + x] = HeightColor(terrain, x, y);
    terrain->tex->UnlockRect(0); 
    UpdateLightTerrain(x, y, terrain, directionToLight);
}

void UpdateLightTerrain(int x, int y, Terrain* terrain, D3DXVECTOR3 directionToLight)
{
    HRESULT hr = 0;
    D3DSURFACE_DESC textureDesc;
    terrain->tex->GetLevelDesc(0, &textureDesc);
	if( textureDesc.Format != D3DFMT_X8R8G8B8 )
    {
		return;
    }
    D3DLOCKED_RECT lockedRect;
    terrain->tex->LockRect(0, &lockedRect, 0, 0);
    DWORD* imageData = (DWORD*)lockedRect.pBits;
    int index = (y * lockedRect.Pitch / 4) + x;
    D3DXCOLOR c;
    float height = GetYComponent(x, y, terrain);
    if(height < 10.0f )
    {
        c = D3DCOLOR_XRGB(124, 197, 118);
    }
    else if(height < 40.5f)
    {
        c = D3DCOLOR_XRGB(0, 166, 81);
    }
    else if(height < 60.0f)
    {
        c = D3DCOLOR_XRGB(25, 123, 48);
    }
    else if(height < 100.5f){
        c = D3DCOLOR_XRGB(115, 100,  87);
    }
    else
    {
        c = D3DCOLOR_XRGB(255, 255, 255);
    }
    c *= ComputeShade(terrain, x, y, directionToLight);
    imageData[index] = (D3DXCOLOR)c;  
    terrain->tex->UnlockRect(0);
}


void LightTerrain(Terrain* terrain, D3DXVECTOR3 directionToLight)
{
    HRESULT hr = 0;
    D3DSURFACE_DESC textureDesc;
    terrain->tex->GetLevelDesc(0, &textureDesc);
	if( textureDesc.Format != D3DFMT_X8R8G8B8 )
    {
		return;
    }
    D3DLOCKED_RECT lockedRect;
    terrain->tex->LockRect(0, &lockedRect, 0, 0);
    DWORD* imageData = (DWORD*)lockedRect.pBits;
    for(int j = 0; j < textureDesc.Height; j++)
    {
        for(int i = 0; i < textureDesc.Width; i++)
        {
            int index = (i * lockedRect.Pitch / 4) + j;
            D3DXCOLOR c(imageData[index]);
            c *= ComputeShade(terrain, j, i, directionToLight);
            imageData[index] = (D3DXCOLOR)c;  
        }
    }
    terrain->tex->UnlockRect(0);
}

float ComputeShade(Terrain* terrain, int x, int y, D3DXVECTOR3 directionToLight)
{
    float heightA = GetYComponent(x, y, terrain);
    float heightB = GetYComponent(x + 1, y, terrain);
    float heightC = GetYComponent(x, y + 1, terrain);

    D3DXVECTOR3 u((float)terrain->cellSpacing, heightB - heightA, 0.0f);
    D3DXVECTOR3 v(0.0f, heightC - heightA, (float)terrain->cellSpacing);

    D3DXVECTOR3 n;
    D3DXVec3Cross(&n, &u, &v);
    D3DXVec3Normalize(&n, &n);
	float cosine = -D3DXVec3Dot(&n, &directionToLight);
	if(cosine < 0.0f)
		cosine = 0.0f;
	return cosine;
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

float getHeight(Terrain* terrain, float x, float z)
{
	float col = floorf(x);
	float row = floorf(z);

	float A = getHeightmapEntry(row,   col, terrain);
	float B = getHeightmapEntry(row,   col+1, terrain);
	float C = getHeightmapEntry(row+1, col, terrain);
	float D = getHeightmapEntry(row+1, col+1, terrain);

	float dx = x - col;
	float dz = z - row;

	float height = 0.0f;
	if(dz < 1.0f - dx)  // upper triangle ABC
	{
		float uy = B - A; // A->B
		float vy = C - A; // A->C

		height = A + Lerp(0.0f, uy, dx) + Lerp(0.0f, vy, dz);
	}
	else // lower triangle DCB
	{
		float uy = C - D; // D->C
		float vy = B - D; // D->B
		height = D + Lerp(0.0f, uy, 1.0f - dx) + Lerp(0.0f, vy, 1.0f - dz);
	}

	return height;
}

float Lerp(float a, float b, float t)
{
    return a - (a*t) + (b*t);
}

int getHeightmapEntry(int row, int col, Terrain* terrain)
{
    if(row >= terrain->numVertexRow)
        row = terrain->numVertexRow - 1;
    if(col >= terrain->numVertexCol)
        col = terrain->numVertexCol - 1;
    if(row < 0)
        row = 0;
    if(col < 0)
        col = 0; 
    return terrain->heightMap[(row * terrain->numVertexRow) + col];
}



