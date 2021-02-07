#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "mesh.h"


void LoadOBJFile(IDirect3DDevice9* device, Mesh* mesh, const char* filePhat, const char* texFileName)
{
    mesh->numVertices  = 0;
    mesh->numIndex     = 0;
    mesh->numTexCoords = 0;
    mesh->numNormals   = 0;

    FILE* file;
    file = fopen(filePhat, "r");

    if(file == NULL)
    {
        OutputDebugString("ERROR: loading OBJ_FILE\n");
    }
    char line[1024];

    while(fgets(line, 1024, file) != NULL)
    {    
        // first we have to count the size 
        // becouse we need to allocate memory for
        // the obj object 
        if(strncmp(line, "v ", 2) == 0)
        {
            mesh->numVertices++;
        }
        if(strncmp(line, "vt ", 3) == 0)
        {
            mesh->numTexCoords++;
        }
        if(strncmp(line, "vn ", 3) == 0)
        {
            mesh->numNormals++;
        }
        if(strncmp(line, "f ", 2) == 0)
        {
            mesh->numIndex++;
        }
    } 
    // we allocate memory for the model 
    mesh->vertices      = (D3DXVECTOR3*)VirtualAlloc(0, mesh->numVertices  * sizeof(D3DXVECTOR3), MEM_COMMIT, PAGE_READWRITE);
    mesh->textureCoords = (D3DXVECTOR2*)VirtualAlloc(0, mesh->numTexCoords * sizeof(D3DXVECTOR2), MEM_COMMIT, PAGE_READWRITE);
    mesh->normals       = (D3DXVECTOR3*)VirtualAlloc(0, mesh->numNormals   * sizeof(D3DXVECTOR3), MEM_COMMIT, PAGE_READWRITE);
    mesh->vertexBuffer  = (VertexBuffer*)VirtualAlloc(0, (mesh->numIndex * 3)    * sizeof(VertexBuffer),MEM_COMMIT, PAGE_READWRITE);
    mesh->vertexIndex   = (IndexBuffer*)VirtualAlloc(0, mesh->numIndex     * sizeof(IndexBuffer), MEM_COMMIT, PAGE_READWRITE);
    mesh->textureIndex  = (IndexBuffer* )VirtualAlloc(0, mesh->numIndex    * sizeof(IndexBuffer), MEM_COMMIT, PAGE_READWRITE);
    mesh->normalIndex   = (IndexBuffer*)VirtualAlloc(0, mesh->numIndex     * sizeof(IndexBuffer), MEM_COMMIT, PAGE_READWRITE);
    
    D3DXVECTOR3 test;
    rewind(file);
    int vertexIndex = 0;
    int textIndex = 0;
    int normalIndex = 0;
    int indexIndex = 0;
    while(fgets(line, 1024, file) != NULL)
    {     
        if(strncmp(line, "v ", 2) == 0)
        {
            float x, y, z;
            sscanf(line, "v %f %f %f", &x, &y, &z);
            mesh->vertices[vertexIndex].x = x;
            mesh->vertices[vertexIndex].y = y;
            mesh->vertices[vertexIndex].z = z; 
            vertexIndex++;
        }
        if(strncmp(line, "vt ", 3) == 0)
        {
            float x, y;
            sscanf(line, "vt %f %f", &x, &y);
            mesh->textureCoords[textIndex].x = x;
            mesh->textureCoords[textIndex].y = y;
            textIndex++;
        }
        if(strncmp(line, "vn ", 3) == 0)
        {
            float x, y, z;
            sscanf(line, "vn %f %f %f", &x, &y, &z);
            mesh->normals[normalIndex].x = x;
            mesh->normals[normalIndex].y = y;
            mesh->normals[normalIndex].z = z;
            normalIndex++;
        }
        if(strncmp(line, "f ", 2) == 0)
        {
            int indices[3];
            int textures[3];
            int normals[3];
            sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                &indices[0], &textures[0], &normals[0],
                &indices[1], &textures[1], &normals[1],
                &indices[2], &textures[2], &normals[2]);

            mesh->vertexIndex[indexIndex].a = indices[0];
            mesh->vertexIndex[indexIndex].b = indices[1];
            mesh->vertexIndex[indexIndex].c = indices[2];

            mesh->textureIndex[indexIndex].a = textures[0];
            mesh->textureIndex[indexIndex].b = textures[1];
            mesh->textureIndex[indexIndex].c = textures[2];

            mesh->normalIndex[indexIndex].a = normals[0];
            mesh->normalIndex[indexIndex].b = normals[1];
            mesh->normalIndex[indexIndex].c = normals[2];
            indexIndex++;
        }
    }

    // with all the obj info we have to fill the vertex and index buffer
    int indexCounter = 0;
    for(int i = 0; i < (mesh->numIndex * 3); i += 3)
    {
        // vertice
        mesh->vertexBuffer[i].vertice.x = mesh->vertices[mesh->vertexIndex[indexCounter].a - 1].x;
        mesh->vertexBuffer[i].vertice.y = mesh->vertices[mesh->vertexIndex[indexCounter].a - 1].y;
        mesh->vertexBuffer[i].vertice.z = mesh->vertices[mesh->vertexIndex[indexCounter].a - 1].z;
        mesh->vertexBuffer[i + 1].vertice.x = mesh->vertices[mesh->vertexIndex[indexCounter].b - 1].x;
        mesh->vertexBuffer[i + 1].vertice.y = mesh->vertices[mesh->vertexIndex[indexCounter].b - 1].y;
        mesh->vertexBuffer[i + 1].vertice.z = mesh->vertices[mesh->vertexIndex[indexCounter].b - 1].z;
        mesh->vertexBuffer[i + 2].vertice.x = mesh->vertices[mesh->vertexIndex[indexCounter].c - 1].x;
        mesh->vertexBuffer[i + 2].vertice.y = mesh->vertices[mesh->vertexIndex[indexCounter].c - 1].y;
        mesh->vertexBuffer[i + 2].vertice.z = mesh->vertices[mesh->vertexIndex[indexCounter].c - 1].z;
        // texture Coords
        mesh->vertexBuffer[i].textureCoord.x = mesh->textureCoords[mesh->textureIndex[indexCounter].a - 1].x;
        mesh->vertexBuffer[i].textureCoord.y = mesh->textureCoords[mesh->textureIndex[indexCounter].a - 1].y;
        mesh->vertexBuffer[i + 1].textureCoord.x = mesh->textureCoords[mesh->textureIndex[indexCounter].b - 1].x;
        mesh->vertexBuffer[i + 1].textureCoord.y = mesh->textureCoords[mesh->textureIndex[indexCounter].b - 1].y;
        mesh->vertexBuffer[i + 2].textureCoord.x = mesh->textureCoords[mesh->textureIndex[indexCounter].c - 1].x;
        mesh->vertexBuffer[i + 2].textureCoord.y = mesh->textureCoords[mesh->textureIndex[indexCounter].c - 1].y;
        // normals
        mesh->vertexBuffer[i].normal.x = mesh->normals[mesh->normalIndex[indexCounter].a - 1].x;
        mesh->vertexBuffer[i].normal.y = mesh->normals[mesh->normalIndex[indexCounter].a - 1].y;
        mesh->vertexBuffer[i].normal.z = mesh->normals[mesh->normalIndex[indexCounter].a - 1].z;
        mesh->vertexBuffer[i + 1].normal.x = mesh->normals[mesh->normalIndex[indexCounter].b - 1].x;
        mesh->vertexBuffer[i + 1].normal.y = mesh->normals[mesh->normalIndex[indexCounter].b - 1].y;
        mesh->vertexBuffer[i + 1].normal.z = mesh->normals[mesh->normalIndex[indexCounter].b - 1].z;
        mesh->vertexBuffer[i + 2].normal.x = mesh->normals[mesh->normalIndex[indexCounter].c - 1].x;
        mesh->vertexBuffer[i + 2].normal.y = mesh->normals[mesh->normalIndex[indexCounter].c - 1].y;
        mesh->vertexBuffer[i + 2].normal.z = mesh->normals[mesh->normalIndex[indexCounter].c - 1].z;
        indexCounter++;
    }

    // Cramos el vertexBuffer de DIrect3D9
    device->CreateVertexBuffer(
            (mesh->numIndex * 3) * sizeof(MeshVertex),
            D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
            MeshVertex::FVF,
            D3DPOOL_DEFAULT,
            &mesh->D3DvertexBuffer,
            0);
    MeshVertex* v = 0;
    mesh->D3DvertexBuffer->Lock(0, 0, (void**)&v, 0);

    for(int i = 0; i < (mesh->numIndex * 3); i++)
    {
        MeshVertex vertex = {
            mesh->vertexBuffer[i].vertice.x,
            mesh->vertexBuffer[i].vertice.y,
            mesh->vertexBuffer[i].vertice.z,
            mesh->vertexBuffer[i].normal.x,
            mesh->vertexBuffer[i].normal.y,
            mesh->vertexBuffer[i].normal.z,
            mesh->vertexBuffer[i].textureCoord.x,
            1 - mesh->vertexBuffer[i].textureCoord.y,
        }; 
        v[i] = vertex;       
    }
    mesh->D3DvertexBuffer->Unlock();

    VirtualFree(mesh->vertices, 0, MEM_RELEASE); 
    VirtualFree(mesh->textureCoords, 0, MEM_RELEASE);
    VirtualFree(mesh->normals, 0, MEM_RELEASE); 
    VirtualFree(mesh->vertexBuffer, 0, MEM_RELEASE);
    VirtualFree(mesh->vertexIndex, 0, MEM_RELEASE);
    VirtualFree(mesh->textureIndex, 0, MEM_RELEASE);
    VirtualFree(mesh->normalIndex, 0, MEM_RELEASE); 

    D3DXCreateTextureFromFile(
            device,
            texFileName, 
            &mesh->tex);

}






