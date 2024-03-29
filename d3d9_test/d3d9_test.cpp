// d3d9_test.cpp : Define o ponto de entrada para o aplicativo.
//

#include "framework.h"
#include "d3d9_test.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <commdlg.h>

#pragma comment(lib, "d3d9.lib")

#define MAX_LOADSTRING 100
#define SCREEN_HEIGHT 600
#define SCREEN_WIDTH 800

// Variáveis Globais:
HINSTANCE hInst;                                // instância atual
WCHAR szTitle[MAX_LOADSTRING];                  // O texto da barra de título
WCHAR szWindowClass[MAX_LOADSTRING];            // o nome da classe da janela principal
WCHAR g_pFilePath[MAX_PATH];

// Declarações de encaminhamento de funções incluídas nesse módulo de código:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// DirectX9 instances
LPDIRECT3D9 g_pD3d;
LPDIRECT3DDEVICE9 g_pD3dDev;

LPD3DXMESH g_pMesh;
D3DMATERIAL9* g_pMaterial;
DWORD g_nbMaterials;

// functions definition
void InitApp(HWND hWnd);
void render_frame(void);
void CleanDirectX(void);
void setup_graphics(void);
void setup_light(void);

void OpenFileDialog(HWND hwndOwner) {
    char filePath[MAX_PATH] = { 0 };
    OPENFILENAME ofn = { 0 }; // common dialog box structure
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwndOwner;
    ofn.lpstrFile = (LPWSTR)g_pFilePath;
    ofn.nMaxFile = sizeof(g_pFilePath);
    ofn.lpstrFilter = L"DirectX\0*x\0All Files\0*.*\0Text Files\0*.TXT\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    GetOpenFileName(&ofn);

    return;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Coloque o código aqui.

    // Inicializar cadeias de caracteres globais
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_D3D9TEST, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Realize a inicialização do aplicativo:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_D3D9TEST));

    MSG msg;

    // Loop de mensagem principal:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    CleanDirectX();

    return (int) msg.wParam;
}



//
//  FUNÇÃO: MyRegisterClass()
//
//  FINALIDADE: Registra a classe de janela.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_D3D9TEST));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_D3D9TEST);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE)

struct CUSTOM_VERTEX
{
    FLOAT x, y, z;
    DWORD color;
};

void render_frame(void)
{
    g_pD3dDev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
    g_pD3dDev->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

    g_pD3dDev->BeginScene();

    D3DXMATRIX matView;
    D3DXVECTOR3 camera_pos = { 0.0f,4.0f,8.0f };
    D3DXVECTOR3 look_at = { 0.0f,0.0f,0.0f };
    D3DXVECTOR3 up_dir = { 0.0f, 1.0f, 0.0f };

    D3DXMatrixLookAtLH(&matView, &camera_pos, &look_at, &up_dir);
    g_pD3dDev->SetTransform(D3DTS_VIEW, &matView);

    D3DXMATRIX matProjection;
    D3DXMatrixPerspectiveFovLH(&matProjection, D3DXToRadian(45), SCREEN_WIDTH / SCREEN_HEIGHT, 1.0f, 100.0f);
    g_pD3dDev->SetTransform(D3DTS_PROJECTION, &matProjection);

    static float index = 0.03f;
    D3DXMATRIX matRotateY;
    D3DXMatrixRotationY(&matRotateY, index);
    g_pD3dDev->SetTransform(D3DTS_WORLD, &(matRotateY));

    for (DWORD i = 0; i < g_nbMaterials; ++i)
    {
        g_pD3dDev->SetMaterial(&g_pMaterial[i]);
        g_pMesh->DrawSubset(i);
    }

    g_pD3dDev->EndScene();
    g_pD3dDev->Present(NULL, NULL, NULL, NULL);

    return;
}

//
//   FUNÇÃO: InitInstance(HINSTANCE, int)
//
//   FINALIDADE: Salva o identificador de instância e cria a janela principal
//
//   COMENTÁRIOS:
//
//        Nesta função, o identificador de instâncias é salvo em uma variável global e
//        crie e exiba a janela do programa principal.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Armazenar o identificador de instância em nossa variável global

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, SCREEN_WIDTH, SCREEN_HEIGHT, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   InitApp(hWnd);

   return TRUE;
}

void InitApp(HWND hWnd)
{
    g_pD3d = Direct3DCreate9(D3D_SDK_VERSION);

    D3DPRESENT_PARAMETERS d3dpp;

    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow = hWnd;
    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferWidth = SCREEN_WIDTH;
    d3dpp.BackBufferHeight = SCREEN_HEIGHT;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    g_pD3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pD3dDev);

    setup_light();

    g_pD3dDev->SetRenderState(D3DRS_LIGHTING, TRUE);
    g_pD3dDev->SetRenderState(D3DRS_ZENABLE, TRUE);
    g_pD3dDev->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(50, 50, 50));

    return;
}

void CleanDirectX(void)
{
    g_pMesh->Release();
    g_pD3dDev->Release();
    g_pD3d->Release();

    return;
}

void load_mesh(void)
{
    LPD3DXBUFFER buffer;

    HRESULT hr = D3DXLoadMeshFromX(g_pFilePath, D3DXMESH_SYSTEMMEM, g_pD3dDev, NULL, &buffer, NULL, &g_nbMaterials, &g_pMesh);
    D3DXMATERIAL* tempMaterials = (D3DXMATERIAL*)buffer->GetBufferPointer();
    
    g_pMaterial = new D3DMATERIAL9[g_nbMaterials];

    for (DWORD i = 0; i < g_nbMaterials; ++i)
    {
        g_pMaterial[i] = tempMaterials[i].MatD3D;
        g_pMaterial[i].Ambient = g_pMaterial[i].Diffuse;
    }
    return;
}

void setup_light(void)
{
    D3DLIGHT9 light;

    ZeroMemory(&light, sizeof(light));
    light.Type = D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r = 0.5f;
    light.Diffuse.g = 0.5f;
    light.Diffuse.b = 0.5f;
    light.Diffuse.a = 1.0f;

    D3DVECTOR directionVec = { -1.0f,-0.3f,-1.0f };
    light.Direction = directionVec;

    g_pD3dDev->SetLight(0, &light);
    g_pD3dDev->LightEnable(0, TRUE);

    return;
}

//
//  FUNÇÃO: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  FINALIDADE: Processa as mensagens para a janela principal.
//
//  WM_COMMAND  - processar o menu do aplicativo
//  WM_PAINT    - Pintar a janela principal
//  WM_DESTROY  - postar uma mensagem de saída e retornar
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Analise as seleções do menu:
            switch (wmId)
            {
            case IDM_OPEN:
                OpenFileDialog(hWnd);
                load_mesh();
                render_frame();
                break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Adicione qualquer código de desenho que use hdc aqui...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Manipulador de mensagem para a caixa 'sobre'.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
