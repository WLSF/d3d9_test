// d3d9_test.cpp : Define o ponto de entrada para o aplicativo.
//

#include "framework.h"
#include "d3d9_test.h"
#include <d3d9.h>
#include <d3dx9math.h>

#pragma comment(lib, "d3d9.lib")

#define MAX_LOADSTRING 100
#define SCREEN_HEIGHT 600
#define SCREEN_WIDTH 800

// Variáveis Globais:
HINSTANCE hInst;                                // instância atual
WCHAR szTitle[MAX_LOADSTRING];                  // O texto da barra de título
WCHAR szWindowClass[MAX_LOADSTRING];            // o nome da classe da janela principal

// Declarações de encaminhamento de funções incluídas nesse módulo de código:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// DirectX9 instances
LPDIRECT3D9 g_pD3d;
LPDIRECT3DDEVICE9 g_pD3dDev;

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
    if (!InitInstance (hInstance, nCmdShow))
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
    g_pD3dDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    g_pD3dDev->SetRenderState(D3DRS_ZENABLE, TRUE);
    g_pD3dDev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
    g_pD3dDev->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
    g_pD3dDev->BeginScene();

    CUSTOM_VERTEX vv[] = {
        { -3.0f, 3.0f, -3.0f, D3DCOLOR_XRGB(0, 0, 255), },
        { 3.0f, 3.0f, -3.0f, D3DCOLOR_XRGB(0, 255, 0), },
        { -3.0f, -3.0f, -3.0f, D3DCOLOR_XRGB(255, 0, 0), },
        { 3.0f, -3.0f, -3.0f, D3DCOLOR_XRGB(0, 255, 255), },
        { -3.0f, 3.0f, 3.0f, D3DCOLOR_XRGB(0, 0, 255), },
        { 3.0f, 3.0f, 3.0f, D3DCOLOR_XRGB(255, 0, 0), },
        { -3.0f, -3.0f, 3.0f, D3DCOLOR_XRGB(0, 255, 0), },
        { 3.0f, -3.0f, 3.0f, D3DCOLOR_XRGB(0, 255, 255), },
    };

    short indices[] =
    {
        0, 1, 2,    // side 1
        2, 1, 3,
        4, 0, 6,    // side 2
        6, 0, 2,
        7, 5, 6,    // side 3
        6, 5, 4,
        3, 1, 7,    // side 4
        7, 1, 5,
        4, 5, 0,    // side 5
        0, 5, 1,
        3, 7, 2,    // side 6
        2, 7, 6,
    };

    LPDIRECT3DVERTEXBUFFER9 v_buffer;
    LPDIRECT3DINDEXBUFFER9 i_buffer;

    g_pD3dDev->CreateVertexBuffer(8*sizeof(CUSTOM_VERTEX), 0, CUSTOMFVF, D3DPOOL_MANAGED, &v_buffer, NULL);

    void* pVoid;

    v_buffer->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, vv, sizeof(vv));
    v_buffer->Unlock();

    g_pD3dDev->CreateIndexBuffer(36 * sizeof(indices), 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &i_buffer, NULL);

    i_buffer->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, indices, sizeof(indices));
    i_buffer->Unlock();

    g_pD3dDev->SetFVF(CUSTOMFVF);

    D3DXMATRIX matRotateY;
    float idx = 0.5f;

    D3DXMatrixRotationY(&matRotateY, idx);
    g_pD3dDev->SetTransform(D3DTS_WORLD, &matRotateY);

    D3DXMATRIX matView;

    D3DXVECTOR3 camera_pos = { 0.0f, 0.0f, 10.0f };
    D3DXVECTOR3 look_at = { 0.0f, 0.0f, 0.0f };
    D3DXVECTOR3 up = { 0.0f, 1.0f, 0.0f };

    D3DXMatrixLookAtLH(&matView, &camera_pos, &look_at, &up);
    g_pD3dDev->SetTransform(D3DTS_VIEW, &matView);

    D3DXMATRIX matProjection;

    D3DXMatrixPerspectiveFovLH(&matProjection, D3DXToRadian(45), (FLOAT)SCREEN_WIDTH / (FLOAT)SCREEN_HEIGHT, 1.0f, 100.0f);
    g_pD3dDev->SetTransform(D3DTS_PROJECTION, &matProjection);
    
    g_pD3dDev->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOM_VERTEX));
    g_pD3dDev->SetIndices(i_buffer);
    g_pD3dDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);

    g_pD3dDev->EndScene();
    g_pD3dDev->Present(NULL, NULL, NULL, NULL);

    v_buffer->Release();
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

   if (D3D_OK == g_pD3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pD3dDev))
   {
       render_frame();

       g_pD3dDev->Release();
       g_pD3d->Release();
   }

   return TRUE;
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
