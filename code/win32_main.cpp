//////////////////////////
/// TM-Win32-FRAMEWORK ///
//////////////////////////
#include "terrain.h"
#include "particle.h"
#include "camera.h"
#include <stdio.h>
#include "mesh.h"
#include "input.h"

#define global_variable static
#define WNDWIDTH 1000
#define WNDHEIGHT 800

//#define WNDWIDTH 1280
//#define WNDHEIGHT 720

struct Vec2
{
    float x;
    float y;
};

struct QuadVertex
{
    float x, y, z;
    D3DCOLOR color;
    static const DWORD FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
};

enum RenderState
{
    VERTEXSHOW,
    WIREFRAME,
    SOLID,
    WIREFRAMESOLID,
    WIREFRAMEVERTEX,
    FULL, 
};

global_variable BOOL appRunnig;
global_variable Vec2 mouseDefaultPos;
global_variable Vec2 mouseRMovement;
global_variable Vec2 mouseVertexPos;
global_variable BOOL mouseClick;
global_variable BOOL mouseRClick;
IDirect3DVertexBuffer9* Quad = 0;
D3DMATERIAL9 quadMtrl;
D3DXVECTOR3 mouseWorldPos(0.0f, 0.0f, 0.0f);
D3DXVECTOR3 dirVector(0.0f, 0.0f, 0.0f);
D3DXVECTOR3 lightDir(0.5f, 1.0f, 0.0f);

Camera camera(Camera::LANDOBJECT);

global_variable uint8_t heightMap[64 * 64] = {};

global_variable ParticleSystem ps;
global_variable BoundingBox boundingBox;

D3DXVECTOR3 Vector3Add(D3DXVECTOR3 v0, D3DXVECTOR3 v1)
{
    D3DXVECTOR3 result;
    result.x = v0.x + v1.x;
    result.y = v0.y + v1.y;
    result.z = v0.z + v1.z;
    return result;
}


D3DXVECTOR3 Vec3RotateY(D3DXVECTOR3 v, float angle)
{
    D3DXVECTOR3 rotatedVector;
        rotatedVector.x = v.x * cos(angle) + v.z * sin(angle);
        rotatedVector.y = v.y;
        rotatedVector.z = -v.x * sin(angle) + v.z * cos(angle); 
    return rotatedVector;
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result;
    switch(Msg)
    {
        case WM_KEYUP:
        {
 			uint32_t  VKCode = wParam;
			bool wasDown = ((lParam & (1 << 30)) != 0);
			bool isDown = ((lParam & (1 << 31)) == 0);
			if (wasDown != isDown)
			{
				SetScanCodeUp(VKCode);
			}  
        }break;

        case WM_KEYDOWN:
        {
 			uint32_t  VKCode = wParam;
			bool wasDown = ((lParam & (1 << 30)) != 0);
			bool isDown = ((lParam & (1 << 31)) == 0);
			if (wasDown != isDown)
			{
				SetScanCodeDown(VKCode);
			} 
        }break;
        case WM_CLOSE:
        {
            appRunnig = FALSE;
        }break;
        case WM_DESTROY:
        {
            appRunnig = FALSE;
        }break;
        case WM_LBUTTONDOWN:
        {
            mouseClick = TRUE;
        }break;
        case WM_LBUTTONUP:
        {
            mouseClick = FALSE;
        }break;
        case WM_RBUTTONDOWN:
        {
            mouseRClick = TRUE;
        }break;

        case WM_RBUTTONUP:
        {
            mouseRClick = FALSE;
        }break;
        case WM_MOUSEMOVE:
        {
            POINT actualMousePos;
            GetCursorPos(&actualMousePos);
            mouseRMovement.x += actualMousePos.x - mouseDefaultPos.x;
            mouseRMovement.y -= actualMousePos.y - mouseDefaultPos.y;
            SetCursorPos(mouseDefaultPos.x, mouseDefaultPos.y); 
        }break;
        case WM_MOVE:
        {
            float xOffset = (float)(short)LOWORD(lParam);
            float yOffset = (float)(short)HIWORD(lParam);
            mouseDefaultPos.x = (WNDWIDTH / 2.0f) + xOffset;
            mouseDefaultPos.y = (WNDHEIGHT / 2.0f) + yOffset;
        }break;
        default:
        {
            result = DefWindowProc(hwnd, Msg, wParam, lParam); 
        }break;
    }
    return(result);
}

int InitializeD3D9(IDirect3DDevice9** device, HWND hWnd)
{
    // DIRECT_3D INITIALIZATION..........................................
    // 3 steps 
    HRESULT hr               = 0;
    IDirect3D9* d3d          = 0;
    int vp                   = 0;
    // step 1: create a direct_3d object
    if((d3d = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
    {
        OutputDebugString("FAILED creating D3D\n");
        return(1);
    }
    // step 2: check for hardware vp
    D3DCAPS9 caps;
    d3d->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
    if(caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
    {
        vp = D3DCREATE_HARDWARE_VERTEXPROCESSING; 
    }
    else
    {
        vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }
    // step 3: fill out the pressent_parameters struct
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth            = WNDWIDTH;
	d3dpp.BackBufferHeight           = WNDHEIGHT;
	d3dpp.BackBufferFormat           = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount            = 1;
	d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality         = 0;
	d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD; 
	d3dpp.hDeviceWindow              = hWnd;
	d3dpp.Windowed                   = true;
	d3dpp.EnableAutoDepthStencil     = true; 
	d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
	d3dpp.Flags                      = 0;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_ONE;
    // step 4: create a d3d_device whit all the information we hace until now      
    hr = d3d->CreateDevice(D3DADAPTER_DEFAULT,
                           D3DDEVTYPE_HAL,
                           hWnd,
                           vp,
                           &d3dpp,
                           device);
    if(FAILED(hr))
    {
        OutputDebugString("FAILED creating d3d_device\n");
    } 
    // step 5: release the d3d object
    d3d->Release();
    if(device != NULL)
    {
        OutputDebugString("SUCCEED creating d3d_device\n");
        return(0);
    }
    return(1);
    //.........................................................................
}

void SetUp(IDirect3DDevice9* device)
{
    device->CreateVertexBuffer(
            6 * sizeof(QuadVertex),
            D3DUSAGE_WRITEONLY,
            QuadVertex::FVF,
            D3DPOOL_MANAGED,
            &Quad,
            0);

    QuadVertex* v = 0;
    Quad->Lock(0, 0, (void**)&v, 0);

    // quad built from two triangles, note texture coordinates:
    QuadVertex quadVertex0 = {-1.0f,  0.1f, -1.0f, D3DCOLOR_XRGB(255, 0, 0)};
    QuadVertex quadVertex1 = {-1.0f,  0.1f,  1.0f, D3DCOLOR_XRGB(255, 0, 0)};
    QuadVertex quadVertex2 = { 1.0f,  0.1f,  1.0f, D3DCOLOR_XRGB(255, 0, 0)};

    QuadVertex quadVertex3 = {-1.0f,  0.1f, -1.0f, D3DCOLOR_XRGB(255, 0, 0)};
    QuadVertex quadVertex4 = { 1.0f,  0.1f,  1.0f, D3DCOLOR_XRGB(255, 0, 0)};
    QuadVertex quadVertex5 = { 1.0f,  0.1f, -1.0f, D3DCOLOR_XRGB(255, 0, 0)};
    
    v[0] = quadVertex0;
    v[1] = quadVertex1;
    v[2] = quadVertex2;
    
    v[3] = quadVertex3;
    v[4] = quadVertex4;
    v[5] = quadVertex5;
    
    Quad->Unlock();
     
    ZeroMemory(&quadMtrl, sizeof(quadMtrl));
    quadMtrl.Diffuse  = D3DXCOLOR(0.6f, 0.0f, 0.0f, 1.0f); // red
    quadMtrl.Ambient  = D3DXCOLOR(0.6f, 0.3f, 0.2f, 1.0f); // red
    quadMtrl.Specular = D3DXCOLOR(0.6f, 0.3f, 0.2f, 1.0f); // red
    quadMtrl.Emissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f); // no emission
    quadMtrl.Power    = 5.0f;

    // Set Lighting off
    //device->SetRenderState(D3DRS_LIGHTING, false);
    D3DLIGHT9 dir;
    ::ZeroMemory(&dir, sizeof(dir));
    dir.Type      = D3DLIGHT_DIRECTIONAL;
    dir.Diffuse   = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    dir.Specular  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) * 0.3f;
    dir.Ambient   = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f) * 0.6f;
    dir.Direction = D3DXVECTOR3(0.5f, 1.0f, 0.0f);
    device->SetLight(0, &dir);
    device->LightEnable(0, true);
    device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
    device->SetRenderState(D3DRS_SPECULARENABLE, true);

	device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);


    // Set projection Matrix
    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.4f, (float)WNDWIDTH / (float)WNDHEIGHT, 1.0f, 1000.f);
    device->SetTransform(D3DTS_PROJECTION, &proj);
}

void Update(RenderState& renderState, ParticleSystem* particleSystem, Terrain* terrain, IDirect3DDevice9* device, float deltaTime)
{
    dirVector.x = mouseRMovement.x * 0.02;
    dirVector.y = 0.0f; 
    dirVector.z = mouseRMovement.y * 0.02;

    static float angle = 0.0f;
    float speed = 100.0f * deltaTime;
    float rotationSpeed = 3.0f * deltaTime;
    if(KeyDown(VK_ESCAPE))
        appRunnig = FALSE;
    if(KeyDown('1'))
        renderState = SOLID;
    if(KeyDown('2'))
        renderState = WIREFRAME;
    if(KeyDown('3'))
        renderState = VERTEXSHOW;
    if(KeyDown('4'))
        renderState = WIREFRAMEVERTEX;
    if(KeyDown('5'))
        renderState = WIREFRAMESOLID;
    if(KeyDown('6'))
        renderState = FULL;
	if(KeyDown('R'))
		camera.fly(speed);
	if(KeyDown('F'))
		camera.fly(-speed);
	if(KeyDown(VK_UP))
		camera.pitch(-rotationSpeed);
	if(KeyDown(VK_DOWN))
		camera.pitch(rotationSpeed);
	if(KeyDown(VK_LEFT))
    {
		camera.yaw(-rotationSpeed);
        angle -= rotationSpeed;
    }
	if(KeyDown(VK_RIGHT))
    {
		camera.yaw(rotationSpeed);
        angle += rotationSpeed;
    }
	if(KeyDown('N'))
		camera.roll(rotationSpeed);
	if(KeyDown('M'))
		camera.roll(-rotationSpeed);


    D3DXVECTOR3 pos;
    camera.getPosition(&pos);
	if(KeyDown('W'))
    {
        pos += D3DXVECTOR3(camera.look.x, 0.0f, camera.look.z) * speed;
    }
	if(KeyDown('S'))
    {
		pos += D3DXVECTOR3(camera.look.x, 0.0f, camera.look.z) * -speed;
    }
	if(KeyDown('A'))
    {
		pos += D3DXVECTOR3(camera.right.x, 0.0f, camera.right.z) * -speed;
    }
	if(KeyDown('D'))
    {
		pos += D3DXVECTOR3(camera.right.x, 0.0f, camera.right.z) * speed;
    } 
    D3DXVECTOR3 upNormal(0.0f, 1.0f, 0.0f);
    D3DXVECTOR3 actualPosNormal = GetVertexNormal(pos.x / terrain->cellSpacing, pos.z / terrain->cellSpacing, terrain);
    D3DXVec3Normalize(&actualPosNormal, &actualPosNormal);
    float slope = -D3DXVec3Dot(&actualPosNormal, &upNormal);
    char message[63];
    sprintf(message, "slope: %f\n", slope);
    OutputDebugString(message);
    bool colition = false;
    if(slope > 0.9f || !colition)
    {
        float heightCam = getHeight(terrain, pos.x / terrain->cellSpacing, pos.z / terrain->cellSpacing) * terrain->heightScale;
        pos.y = heightCam + 2.5f;
        camera.setPosition(&pos);
    }



    D3DXMATRIX V;
    camera.getViewMatrix(&V);
    device->SetTransform(D3DTS_VIEW, &V);

    dirVector = Vec3RotateY(dirVector, angle);
    mouseWorldPos = Vector3Add(mouseWorldPos, dirVector);

    mouseRMovement.x = 0;
    mouseRMovement.y = 0;

    // mouse handler stuff...
    mouseVertexPos.x = mouseWorldPos.x / terrain->cellSpacing;
    mouseVertexPos.y = mouseWorldPos.z / terrain->cellSpacing;

    if(mouseVertexPos.x < 0)
    {
        mouseVertexPos.x = 0;
    }
    else if(mouseVertexPos.x > terrain->numVertexRow - 1)
    {
        mouseVertexPos.x = terrain->numVertexRow - 1;
    }

    if(mouseVertexPos.y < 0)
    {
        mouseVertexPos.y = 0;
    }
    else if(mouseVertexPos.y > terrain->numVertexCol - 1)
    {
        mouseVertexPos.y = terrain->numVertexCol - 1;
    }
    if(mouseClick == TRUE)
    { 
        UpdateHeightMapWithMousePos(terrain, mouseVertexPos.x, mouseVertexPos.y, true, deltaTime, device, lightDir);
        //mouseClick = FALSE;
    }
    if(mouseRClick == TRUE)
    {
        UpdateHeightMapWithMousePos(terrain, mouseVertexPos.x, mouseVertexPos.y, false, deltaTime, device, lightDir);
        //mouseRClick = FALSE;
    }


    PsUpdate(particleSystem, deltaTime);

}

void Render(RenderState renderState, Mesh* mesh, ParticleSystem* particleSystem, Terrain* terrain, IDirect3DDevice9* device, float deltaTime)
{
    if(device)
    {

        D3DXMATRIX trans;
        D3DXMATRIX world;
        D3DXMATRIX scaleMatrix;

        device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000055, 1.0f, 0);
        device->BeginScene();
        

        device->SetRenderState(D3DRS_LIGHTING, false);
        //device->SetMaterial(&quadMtrl);
        device->SetTexture(0, mesh->tex); 
        device->SetStreamSource(0, mesh->D3DvertexBuffer, 0, sizeof(MeshVertex));
        device->SetFVF(MeshVertex::FVF);
  
        for(int y = 4; y < terrain->numVertexCol; y += 8)
        {
            for(int x = 4; x < terrain->numVertexRow; x += 8)
            {
                D3DXMatrixTranslation(&trans, x * terrain->cellSpacing, (terrain->heightMap[(y * terrain->numVertexRow) + x] * terrain->heightScale)  - 0.5f, y * terrain->cellSpacing); 
                D3DXMatrixScaling(&scaleMatrix, 8.0f, 8.0f, 8.0f);
                world = scaleMatrix * trans;
                device->SetTransform(D3DTS_WORLD, &world);
                device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, mesh->numIndex);
            }
        }
  
        device->SetTexture(0, 0);


        device->SetRenderState(D3DRS_LIGHTING, false);
        device->SetStreamSource(0, terrain->VB, 0, sizeof(Vertex));
        device->SetFVF(Vertex::FVF);
        device->SetIndices(terrain->IB);
        device->SetTexture(0, terrain->tex);
        D3DXMatrixTranslation(&trans, 0.0f, 0.0f, 0.0f);

        if(renderState == SOLID || renderState == WIREFRAMESOLID || renderState == FULL)
        {
        // Draw the terrain:
        //device->SetMaterial(&terrain->mtrl);    
        device->SetTransform(D3DTS_WORLD, &trans);
        //device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
        device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
        device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
        device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
                                     0, 0,
                                     terrain->numVertices,
                                     0,
                                     terrain->numTrinalges);
        }

        if(renderState == WIREFRAME || renderState == WIREFRAMEVERTEX ||
           renderState == WIREFRAMESOLID || renderState == FULL)
        {
        // Draw the wireframe terrain
        device->SetRenderState(D3DRS_LIGHTING, true);
        device->SetMaterial(&quadMtrl);
        device->SetTransform(D3DTS_WORLD, &trans);
        device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME); 
        device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
                                     0, 0,
                                     terrain->numVertices,
                                     0,
                                     terrain->numTrinalges);
        }

        // Draw Particle_System
        //D3DXMATRIX tranforParticles;
        //D3DXMatrixTranslation(&tranforParticles, 400.0f, 100.0f, 0.0f);
        //device->SetTransform(D3DTS_WORLD, &tranforParticles);
        PsRender(particleSystem, device);
        device->SetTexture(0, 0);

                                     
        // Draw the MousePosition
        device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
        device->SetStreamSource(0, Quad, 0, sizeof(QuadVertex));
        device->SetFVF(QuadVertex::FVF);
        
        D3DXMatrixScaling(&scaleMatrix, 0.6f, 0.0f, 0.6f);
        float height = getHeight(terrain, mouseVertexPos.x, mouseVertexPos.y);
        D3DXMatrixTranslation(&trans, mouseWorldPos.x, (height * terrain->heightScale) + 0.1f, mouseWorldPos.z);
        world = scaleMatrix * trans;
        device->SetTransform(D3DTS_WORLD, &world);
        device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
        
        if(renderState == VERTEXSHOW || renderState == WIREFRAMEVERTEX || renderState == FULL)
        {
        // Draw all the VertexPositions:
        for(int z = 0; z < terrain->numVertexCol; z++)
        {
            for(int x = 0; x < terrain->numVertexRow; x++)
            {
                D3DXMatrixTranslation(&trans, x * terrain->cellSpacing, (terrain->heightMap[(z * terrain->numVertexRow) + x] * terrain->heightScale) + 0.001f, z * terrain->cellSpacing);
                world = scaleMatrix * trans;
                device->SetTransform(D3DTS_WORLD, &world);
                device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
            }
        }
        D3DXMATRIX zero;
        D3DXMatrixTranslation(&zero, 0.0f, 0.0f, 0.0f);
        device->SetTransform(D3DTS_WORLD, &zero);
        }

        device->EndScene();
        device->Present(0, 0, 0, 0);
    }  
}


int WinMain(HINSTANCE hInstance,
            HINSTANCE hPrevInstance,
            LPSTR     lpCmdLine,
            int       nCmdShow)
{
    IDirect3DDevice9* device = 0;
    Terrain terrain;
    RenderState renderState = SOLID;
    SetMapInfo(&terrain, 256, 256, 10, 1.0f);
    SetHeightMapInfo(heightMap, &terrain);
    Mesh avion; 

    WNDCLASS wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.style         = CS_HREDRAW | CS_VREDRAW;;
    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance; 
    wc.lpszClassName = "FrameClass";

    if(!RegisterClassA(&wc))
    {
        OutputDebugString("FAILED register class\n");
        return(1);    
    }

    RECT wr;
	wr.left = 0;
	wr.right = WNDWIDTH;
	wr.top = 0;
	wr.bottom = WNDHEIGHT;
	AdjustWindowRect(&wr, WS_OVERLAPPED, FALSE);

    HWND hWnd = CreateWindowA("FrameClass", "Role3DGame",
                  WS_OVERLAPPEDWINDOW,
                  0, 0,
                  wr.right - wr.left, wr.bottom - wr.top,
                  NULL, NULL,
                  hInstance,
                  NULL);
    LARGE_INTEGER perfCountFrequency;
    QueryPerformanceFrequency(&perfCountFrequency);
    uint64_t frequency = (uint64_t)perfCountFrequency.QuadPart;

    if(hWnd)
    {
        appRunnig = TRUE;
        ShowWindow(hWnd, SW_SHOW);
        ShowCursor(false); 
        mouseDefaultPos.x = WNDWIDTH / 2.0f;
        mouseDefaultPos.y = WNDHEIGHT / 2.0f;
        SetCursorPos(mouseDefaultPos.x, mouseDefaultPos.y);

        if(InitializeD3D9(&device, hWnd) == 0)
        {
            OutputDebugString("D3D9 INITIALIZED\n");
            if(device == NULL)
            {
                OutputDebugString("device is NULL ptr\n");
            }
            else
            {
                SetUp(device);  
                GenVertices(&terrain, device);
                GenIndices(&terrain, device);
                GenerateTexture(&terrain, device, lightDir);
                boundingBox.max = D3DXVECTOR3( 256.0f * terrain.cellSpacing,  400.0f,  256.0f * terrain.cellSpacing);
                boundingBox.min = D3DXVECTOR3( (256.0f * terrain.cellSpacing)  / 2,    0.0f, (256.0f * terrain.cellSpacing) / 2); 
                InitSnow(&ps, &boundingBox, 10000);
                Init(&ps, device, "./data/snowball.bmp");
                LoadOBJFile(device, &avion, "./data/tree4.obj", "./data/tree4.bmp");

            }
        }
        LARGE_INTEGER lastCounter;
        QueryPerformanceCounter(&lastCounter);
        while(appRunnig == TRUE)
        {
            MSG  msg;
            BOOL bRet; 
            static float lastTime = (float)timeGetTime();
            if(PeekMessageA(&msg, hWnd, 0, 0, PM_REMOVE))
            {            
                TranslateMessage(&msg); 
                DispatchMessage(&msg); 
            }
            else
            {
                if(device != NULL)
                {
                    LARGE_INTEGER endCounter;
                    QueryPerformanceCounter(&endCounter);
                    uint64_t counterElapsed =  endCounter.QuadPart - lastCounter.QuadPart; 
                    float deltaTime = (counterElapsed / (float)frequency);
                    Update(renderState, &ps, &terrain, device, deltaTime);
                    Render(renderState, &avion, &ps, &terrain, device, deltaTime);
                    lastCounter = endCounter; 
                }
                else
                {
                    OutputDebugString("Device is NULL\n");
                }          
            }
        }
    }
    else
    {
        return(1);
    }
    return 0;
}

