#ifndef PARTICLE_H
#define PARTICLE_H

#include <d3d9.h>
#include <d3dx9.h>
#include <stdint.h>

//...........................................................
// BOUNDING_BOX:...
struct BoundingBox
{
    D3DXVECTOR3 min;
    D3DXVECTOR3 max;
};

void InitBoundingBox(BoundingBox* boundingBox);
bool IsPointInside(BoundingBox* boundingBox, D3DXVECTOR3& p);

//...........................................................
struct Particle
{
    D3DXVECTOR3  position;
    D3DCOLOR     color;
    static const DWORD FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;    
};

struct Attribute
{
    D3DXVECTOR3 position;
    D3DXVECTOR3 velocity;
    D3DXVECTOR3 acceleration;
    float       lifeTime;
    float       age;
    D3DXCOLOR   color;
    D3DXCOLOR   colorFade;
    bool        isAlive;
};

DWORD FloatToDword(float f);

struct ParticleSystem
{
    D3DXVECTOR3             origin;
    BoundingBox             boundingBox;
    float                   emitRate;
    float                   size;
    IDirect3DTexture9*      tex;
    IDirect3DVertexBuffer9* vb;
    Attribute*              particles;
    int                     maxParticles;
    int                     particleCount;

    DWORD vbSize;
    DWORD vbOffset;
    DWORD vbBatchSize;
};

bool Init(ParticleSystem* particleSystem, IDirect3DDevice9* device, char* texFileName);
void Reset(ParticleSystem* particleSystem);
// sometimes we dont want to free the memody of a dead particle
// but rather respawn it instead.
void ResetParticle(BoundingBox* boundingBox, Attribute* attribute);
void AddParticle(ParticleSystem* particleSystem);

void PsUpdate(ParticleSystem* particleSystem, float deltaTime);

void PreRender(ParticleSystem* particleSystem, IDirect3DDevice9* device);
void PsRender(ParticleSystem* particleSystem, IDirect3DDevice9* device);
void PostRender(ParticleSystem* particleSystem, IDirect3DDevice9* device);

bool isEmpty(ParticleSystem* particleSystem);
bool isDead(ParticleSystem* particleSystem);

void RemoveDeadParticles(ParticleSystem* particleSystem);

void InitSnow(ParticleSystem* particleSystem,
              BoundingBox* boundingBox,
              int numParticles);

#endif
