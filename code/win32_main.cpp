//////////////////////////
/// TM-Win32-FRAMEWORK ///
//////////////////////////
#include "terrain.h"
#include "camera.h"
#define global_variable static
#define WNDWIDTH 1280
#define WNDHEIGHT 720

global_variable BOOL appRunnig;
Camera camera(Camera::LANDOBJECT);

global_variable uint8_t heightMap[100] = { 
    0, 0, 40, 30, 50, 50, 70, 50, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 5, 5, 5, 5, 5, 5, 0, 0,
    0, 0, 10, 10, 10, 10, 10, 10, 0, 0,
    0, 0, 40, 30, 255, 50, 70, 50, 0, 0,
    0, 0, 10, 10, 10, 10, 10, 10, 0, 0,
    0, 0, 5, 5, 5, 5, 5, 5, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 40, 30, 50, 50, 70, 50, 0, 0
};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result;
    switch(Msg)
    {
        case WM_CLOSE:
        {
            appRunnig = FALSE;
        }break;
        case WM_DESTROY:
        {
            appRunnig = FALSE;
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
	d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;
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
    /*
    // Set Camera.
	D3DXVECTOR3    pos(-15.0f, 1.0f, 5.0f);
	D3DXVECTOR3 target(25.0f, 0.0f, 0.0f);
	D3DXVECTOR3     up(0.0f, 1.0f, 0.0f);
	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &pos, &target, &up);
	device->SetTransform(D3DTS_VIEW, &V);
    */
    // Set projection Matrix
    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.4f, (float)WNDWIDTH / (float)WNDHEIGHT, 1.0f, 1000.f);
    device->SetTransform(D3DTS_PROJECTION, &proj);
    // Set Lighting off
    //device->SetRenderState(D3DRS_LIGHTING, false);
    device->SetRenderState(D3DRS_LIGHTING, true);

    D3DLIGHT9 dir;
    ::ZeroMemory(&dir, sizeof(dir));
	dir.Type      = D3DLIGHT_DIRECTIONAL;
	dir.Diffuse   = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	dir.Specular  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) * 0.3f;
	dir.Ambient   = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f) * 0.6f;
	dir.Direction = D3DXVECTOR3(1.0f, 1.0f, 0.0f);
    
    device->SetLight(0, &dir);
    device->LightEnable(0, true);
	device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	device->SetRenderState(D3DRS_SPECULARENABLE, true);
}

void Render(Terrain* terrain, IDirect3DDevice9* device, float deltaTime)
{
    if(device)
    {
        //device->LightEnable(0, true);

		if(GetAsyncKeyState('W') & 0x8000f)
		    camera.walk(4.0f * deltaTime);
		if(GetAsyncKeyState('S') & 0x8000f)
		    camera.walk(-4.0f * deltaTime);
		if(GetAsyncKeyState('A') & 0x8000f)
		    camera.strafe(-4.0f * deltaTime);
		if(GetAsyncKeyState('D') & 0x8000f)
		    camera.strafe(4.0f * deltaTime);
		if(GetAsyncKeyState('R') & 0x8000f)
		    camera.fly(4.0f * deltaTime);
		if(GetAsyncKeyState('F') & 0x8000f)
		    camera.fly(-4.0f * deltaTime);
		if(GetAsyncKeyState(VK_UP) & 0x8000f)
		    camera.pitch(-1.0f * deltaTime);
		if(GetAsyncKeyState(VK_DOWN) & 0x8000f)
		    camera.pitch(1.0f * deltaTime);
		if(GetAsyncKeyState(VK_LEFT) & 0x8000f)
		    camera.yaw(-1.0f * deltaTime);
		if(GetAsyncKeyState(VK_RIGHT) & 0x8000f)
		    camera.yaw(1.0f * deltaTime);
		if(GetAsyncKeyState('N') & 0x8000f)
		    camera.roll(1.0f * deltaTime);
		if(GetAsyncKeyState('M') & 0x8000f)
		    camera.roll(-1.0f * deltaTime);

        D3DXMATRIX V;
        camera.getViewMatrix(&V);
        device->SetTransform(D3DTS_VIEW, &V);


        device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff4ddbff, 1.0f, 0);

        device->BeginScene();

        device->SetStreamSource(0, terrain->VB, 0, sizeof(Vertex));
        device->SetFVF(Vertex::FVF);
        device->SetIndices(terrain->IB);
        device->SetMaterial(&terrain->mtrl);
        
        D3DXMATRIX trans;
        D3DXMatrixTranslation(&trans, 0.0f, 0.0f, 0.0f);
        device->SetTransform(D3DTS_WORLD, &trans);
        //device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
        device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
        device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
                                     0, 0,
                                     terrain->numVertices,
                                     0,
                                     terrain->numTrinalges);

        device->EndScene();

        device->Present(0, 0, 0, 0);
    }  
}


int WinMain(HINSTANCE hInstance,
            HINSTANCE hPrevInstance,
            LPSTR     lpCmdLine,
            int       nCmdShow)
{
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

    HWND hWnd = CreateWindowA("FrameClass", "Role3DGame",
                  WS_OVERLAPPEDWINDOW,
                  CW_USEDEFAULT, CW_USEDEFAULT,
                  WNDWIDTH, WNDHEIGHT,
                  NULL, NULL,
                  hInstance,
                  NULL);
    

    IDirect3DDevice9* device = 0;
    Terrain terrain;
    SetMapInfo(&terrain, 10, 10, 5, 0.1f);
    SetHeightMapInfo(heightMap, &terrain);
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
        }
    }
    
    if(hWnd)
    {
        appRunnig = TRUE;
        ShowWindow(hWnd, SW_SHOW); 
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
                    float currentTime = (float)timeGetTime();
                    float deltaTime = (currentTime - lastTime) * 0.001f;
                    Render(&terrain, device, deltaTime);
                    lastTime = currentTime;    
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


