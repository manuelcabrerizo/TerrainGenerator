#include "particle.h"
#include <cstdlib>


DWORD FloatToDword(float f)
{
    return *((DWORD*)&f);
}

void InitBoundingBox(BoundingBox* boundingBox)
{
    boundingBox->min.x = FLT_MAX;
    boundingBox->min.y = FLT_MAX;
    boundingBox->min.z = FLT_MAX;

    boundingBox->max.x = -FLT_MAX;
    boundingBox->max.y = -FLT_MAX;
    boundingBox->max.z = -FLT_MAX;
}


bool IsPointInside(BoundingBox* boundingBox, D3DXVECTOR3& p)
{
    if(p.x >= boundingBox->min.x && p.y >= boundingBox->min.y && p.z >= boundingBox->min.z &&
	   p.x <= boundingBox->max.x && p.y <= boundingBox->max.y && p.z <= boundingBox->max.z )
    {
        return true;
    }
    else
    {
        return false;
    }
}



bool Init(ParticleSystem* particleSystem, IDirect3DDevice9* device, char* texFileName)
{
    particleSystem->vb = 0;
    particleSystem->tex = 0;
    particleSystem->particleCount = 0;
    HRESULT hr = 0;
    hr = device->CreateVertexBuffer(
            particleSystem->vbSize * sizeof(Particle),
            D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
            Particle::FVF,
            D3DPOOL_DEFAULT,
            &particleSystem->vb,
            0);
    if(FAILED(hr))
    {
        OutputDebugString("ERROR: creating PARTICLE VERTEX_BUFFER\n");
        //return false;
    }
    hr = D3DXCreateTextureFromFile(
            device,
            texFileName, 
            &particleSystem->tex);
    if(FAILED(hr))
    {
        OutputDebugString("ERROR: creating TEXTURE PARTICLE_SYSTEM\n");
        //return false;
    }
    OutputDebugString("PARTICLE_SYSTEM init SUCCESS\n");
    return true;
}

void Reset(ParticleSystem* particleSystem)
{
    for(int i = 0; i < particleSystem->vbSize; i++)
    {
        ResetParticle(&particleSystem->boundingBox, &particleSystem->particles[i]);
    } 
}

void AddParticle(ParticleSystem* particleSystem)
{
    Attribute attribute;
    ResetParticle(&particleSystem->boundingBox, &attribute);
    particleSystem->particles[particleSystem->particleCount] = attribute;
    particleSystem->particleCount++;
}

void PreRender(ParticleSystem* particleSystem, IDirect3DDevice9* device)
{
	device->SetRenderState(D3DRS_LIGHTING, false);
	device->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
	device->SetRenderState(D3DRS_POINTSCALEENABLE, true); 
	device->SetRenderState(D3DRS_POINTSIZE, FloatToDword(particleSystem->size));
	device->SetRenderState(D3DRS_POINTSIZE_MIN, FloatToDword(0.0f));
	// control the size of the particle relative to distance
	device->SetRenderState(D3DRS_POINTSCALE_A, FloatToDword(0.0f));
	device->SetRenderState(D3DRS_POINTSCALE_B, FloatToDword(0.0f));
	device->SetRenderState(D3DRS_POINTSCALE_C, FloatToDword(1.0f));
}

void PostRender(ParticleSystem* particleSystem, IDirect3DDevice9* device)
{
	device->SetRenderState(D3DRS_LIGHTING,          false);
	device->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
	device->SetRenderState(D3DRS_POINTSCALEENABLE,  false);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE,  false);
}

void PsRender(ParticleSystem* particleSystem, IDirect3DDevice9* device)
{
    PreRender(particleSystem, device);

    device->SetTexture(0, particleSystem->tex);
    device->SetFVF(Particle::FVF);
    device->SetStreamSource(0, particleSystem->vb, 0, sizeof(Particle));

    Particle* p= 0;
    particleSystem->vb->Lock(0, 0, (void**)&p, 0);

    for(int i = 0; i < particleSystem->vbSize; i++)
    {

        p[i].position = particleSystem->particles[i].position;
        p[i].color = (D3DCOLOR)particleSystem->particles[i].color;
    }
    particleSystem->vb->Unlock();
    device->DrawPrimitive(D3DPT_POINTLIST, 0, particleSystem->vbSize);
    
    PostRender(particleSystem, device);
}

//************************************************************
// Snow System
//************************************************************

float GetRandomFloat(float lowBound, float highBound)
{
	if( lowBound >= highBound ) // bad input
		return lowBound;

	// get random float in [0, 1] interval
	float f = (rand() % 10000) * 0.0001f; 

	// return float in [lowBound, highBound] interval. 
	return (f * (highBound - lowBound)) + lowBound; 
}

void GetRandomVector(
	  D3DXVECTOR3* out,
	  D3DXVECTOR3* min,
	  D3DXVECTOR3* max)
{
	out->x = GetRandomFloat(min->x, max->x);
	out->y = GetRandomFloat(min->y, max->y);
	out->z = GetRandomFloat(min->z, max->z);
}

void InitSnow(ParticleSystem* particleSystem,
              BoundingBox* boundingBox,
              int numParticles)
{
    particleSystem->boundingBox = *boundingBox;
    particleSystem->size = 0.25f;
    particleSystem->vbSize = 2048;
    particleSystem->vbOffset = 0;
    particleSystem->vbBatchSize = 512;

    if(particleSystem->particles)
    {
       VirtualFree(particleSystem->particles, 0, MEM_RELEASE); 
    }
    particleSystem->particles = (Attribute*)VirtualAlloc(
            0, numParticles * sizeof(Attribute), MEM_COMMIT, PAGE_READWRITE);
    if(!particleSystem->particles)
    {
        OutputDebugString("ERROR: creating PARTICLES_ARRAY\n");
    }


    for(int i = 0; i < numParticles; i++)
    {
        AddParticle(particleSystem); 
    }
}


void ResetParticle(BoundingBox* boundingBox, Attribute* attribute)
{
    attribute->isAlive = true;
    GetRandomVector(&attribute->position, 
                    &boundingBox->min,
                    &boundingBox->max);
    attribute->position.y = boundingBox->max.y;

    attribute->velocity.x = GetRandomFloat(0.0f, 1.0f) * -3.0f;
    attribute->velocity.y = GetRandomFloat(0.0f, 1.0f) * -10.0f;
    attribute->velocity.z = -GetRandomFloat(0.0f, 1.0f) * -3.0f;

    attribute->color = (D3DCOLOR)D3DCOLOR_XRGB(255, 255, 255); 
}

void PsUpdate(ParticleSystem* particleSystem, float deltaTime)
{
    for(int i = 0; i < particleSystem->vbSize; i++)
    {
        particleSystem->particles[i].position += particleSystem->particles[i].velocity * deltaTime;
        if(!IsPointInside(&particleSystem->boundingBox, particleSystem->particles[i].position))
        {
            ResetParticle(&particleSystem->boundingBox, &particleSystem->particles[i]);
        }
    }
}

/*
void PsRender(ParticleSystem* particleSystem, IDirect3DDevice9* device)
{
    PreRender(particleSystem, device);

    device->SetTexture(0, particleSystem->tex);
    device->SetFVF(Particle::FVF);
    device->SetStreamSource(0, particleSystem->vb, 0, sizeof(Particle));
    // Render Baches one by one
    if(particleSystem->vbOffset >= particleSystem->vbSize)
    {
        particleSystem->vbOffset = 0;
    }
    Particle* p= 0;
    particleSystem->vb->Lock(
            particleSystem->vbOffset * sizeof(Particle),
            particleSystem->vbBatchSize * sizeof(Particle),
            (void**)&p,
            particleSystem->vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);
    DWORD numParticlesInBatch = 0;

    // Until all particles have been rendered.
    for(int i = 0; i < particleSystem->vbSize; i++)
    {
        if(particleSystem->particles[i].isAlive)
        {
            p->position = particleSystem->particles[i].position;
            p->color = (D3DCOLOR)particleSystem->particles[i].color;
            p++;
            numParticlesInBatch++;
            if(numParticlesInBatch == particleSystem->vbBatchSize)
            {
                particleSystem->vb->Unlock();
                device->DrawPrimitive(D3DPT_POINTLIST,
                                      particleSystem->vbOffset,
                                      particleSystem->vbBatchSize);
                particleSystem->vbOffset += particleSystem->vbBatchSize;
                if(particleSystem->vbOffset >= particleSystem->vbSize)
                {
                    particleSystem->vbOffset = 0;
                }
                particleSystem->vb->Lock(
                        particleSystem->vbOffset * sizeof(Particle),
                        particleSystem->vbBatchSize * sizeof(Particle),
                        (void**)&p,
                        particleSystem->vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);
                DWORD numParticlesInBatch = 0;
            }
        }
    }
    particleSystem->vb->Unlock();
    if(numParticlesInBatch)
    {
        device->DrawPrimitive(D3DPT_POINTLIST,
                              particleSystem->vbOffset,
                              numParticlesInBatch);

    }
    particleSystem->vbOffset += particleSystem->vbBatchSize;
    PostRender(particleSystem, device);
}
*/
