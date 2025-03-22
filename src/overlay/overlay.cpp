#include "overlay.hpp"
#include "uiaccess/uiaccess.hpp"

static ID3D11Device* g_pD3DDevice = nullptr;                
static ID3D11DeviceContext* g_pD3DDeviceContext = nullptr; 
static IDXGISwapChain* g_pSwapChain = nullptr;
static UINT g_nResizeWidth = 0;
static UINT g_nResizeHeight = 0; 
static ID3D11RenderTargetView* g_pMainRenderTargetView = nullptr; 

void CleanupRenderTarget()
{
    if (g_pMainRenderTargetView) { 
        g_pMainRenderTargetView->Release(); 
        g_pMainRenderTargetView = nullptr; 
    }
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();

    if (g_pSwapChain) 
    { 
        g_pSwapChain->Release(); 
        g_pSwapChain = nullptr; 
    }

    if (g_pD3DDeviceContext) 
    { 
        g_pD3DDeviceContext->Release(); 
        g_pD3DDeviceContext = nullptr; 
    }

    if (g_pD3DDevice) 
    {
        g_pD3DDevice->Release(); 
        g_pD3DDevice = nullptr; 
    }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pD3DDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pMainRenderTargetView);
    pBackBuffer->Release();
}

bool CreateDeviceD3D(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC dSwapChainDesc;
    ZeroMemory(&dSwapChainDesc, sizeof(dSwapChainDesc));
    dSwapChainDesc.BufferCount = 2;
    dSwapChainDesc.BufferDesc.Width = 0;
    dSwapChainDesc.BufferDesc.Height = 0;
    dSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    dSwapChainDesc.BufferDesc.RefreshRate.Numerator = 165;
    dSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    dSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    dSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    dSwapChainDesc.OutputWindow = hWnd;
    dSwapChainDesc.SampleDesc.Count = 1;
    dSwapChainDesc.SampleDesc.Quality = 0;
    dSwapChainDesc.Windowed = TRUE;
    dSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT nCreateDeviceFlags = 0;
    D3D_FEATURE_LEVEL dFeatureLevel;
    const D3D_FEATURE_LEVEL dFeatureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

    HRESULT hResult = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, nCreateDeviceFlags, dFeatureLevelArray, 2, D3D11_SDK_VERSION, &dSwapChainDesc, &g_pSwapChain, &g_pD3DDevice, &dFeatureLevel, &g_pD3DDeviceContext);
    if (hResult == DXGI_ERROR_UNSUPPORTED)
        hResult = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, nCreateDeviceFlags, dFeatureLevelArray, 2, D3D11_SDK_VERSION, &dSwapChainDesc, &g_pSwapChain, &g_pD3DDevice, &dFeatureLevel, &g_pD3DDeviceContext);
    if (hResult != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, nMsg, wParam, lParam))
    {
        return true;
    }

    switch (nMsg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_nResizeWidth = (UINT)LOWORD(lParam);
        g_nResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }

    return ::DefWindowProcW(hWnd, nMsg, wParam, lParam);
}

std::wstring COverlay::GetForegroundWindowTitle()
{
    HWND hWndCurrent = GetForegroundWindow();
    if (!hWndCurrent)
        return L""; 

    int nLength = GetWindowTextLengthW(hWndCurrent); 
    if (!nLength)
        return L""; 

    std::wstring wsTitle(nLength + 1, L'\0');
    int nResult = GetWindowTextW(hWndCurrent, &wsTitle[0], nLength + 1);

    if (!nResult) {
        return L""; 
    }

    wsTitle.resize(nLength);
    return wsTitle;
}

void COverlay::Update()
{
    HWND hwnd = FindWindow(nullptr, "Counter-Strike 2");
    RECT windowRect;
    RECT clientRect;
    POINT clientTopLeft = {0, 0}; 
    
    if (GetWindowRect(hwnd, &windowRect) && GetClientRect(hwnd, &clientRect)) {
        ClientToScreen(hwnd, &clientTopLeft);

        int nPosX = clientTopLeft.x;                       
        int nPosY = clientTopLeft.y;    
        int nWidth = clientRect.right - clientRect.left;
        int nHeight = clientRect.bottom - clientRect.top;

        if (m_vecWindowSize.x != nWidth || m_vecWindowSize.y != nHeight || m_vecWindowPos.x != nPosX || m_vecWindowPos.y != nPosY)
        {
            m_vecWindowSize.x = nWidth;
            m_vecWindowSize.y = nHeight;
            m_vecWindowPos.x = nPosX;
            m_vecWindowPos.y = nPosY;

            SetWindowPos(
                m_Hwnd, 
                HWND_TOP, 
                static_cast<int>(m_vecWindowPos.x), 
                static_cast<int>(m_vecWindowPos.y), 
                static_cast<int>(m_vecWindowSize.x), 
                static_cast<int>(m_vecWindowSize.y), 
                SWP_NOZORDER | SWP_SHOWWINDOW
            );
        }        
    }
}

void COverlay::Render(const std::wstring& pszTargetWindow)
{
    WNDCLASSEXW dWndClass = { 
        sizeof(dWndClass), 
        CS_CLASSDC, 
        WndProc, 
        0L, 
        0L, 
        GetModuleHandle(nullptr), 
        nullptr, 
        nullptr, 
        nullptr, 
        nullptr, 
        L"twoj stary", 
        nullptr 
    };

    auto aResult = RegisterClassExW(&dWndClass);

    DWORD dwErr = PrepareForUIAccess();
    if (dwErr != ERROR_SUCCESS)
    {
        std::cout << "[!] failed to get uiaccess: " << dwErr << "\n";
        return;
    }

    typedef HWND(WINAPI* CreateWindowInBand)(_In_ DWORD dwExStyle, _In_opt_ ATOM atom, _In_opt_ LPCWSTR lpWindowName, _In_ DWORD dwStyle, _In_ int X, _In_ int Y, _In_ int nWidth, _In_ int nHeight, _In_opt_ HWND hWndParent, _In_opt_ HMENU hMenu, _In_opt_ HINSTANCE hInstance, _In_opt_ LPVOID lpParam, DWORD band);

    CreateWindowInBand pCreateWindowInBand = reinterpret_cast<CreateWindowInBand>(GetProcAddress(LoadLibraryA("user32.dll"), "CreateWindowInBand"));
    HWND hwnd = pCreateWindowInBand(WS_EX_TOPMOST | WS_EX_NOACTIVATE, aResult, L"twoj stary", WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), NULL, NULL, dWndClass.hInstance, NULL, ZBID_UIACCESS);
    m_Hwnd = hwnd;

    if (!hwnd) 
    {
        std::cout << "[!] failed to create overlay \n";
        return;
    }
    std::cout << "[+] created overlay \n";

    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 255, LWA_ALPHA);
    MARGINS margin = { -1 };
    DwmExtendFrameIntoClientArea(hwnd, &margin);

    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(dWndClass.lpszClassName, dWndClass.hInstance);
        std::cout << "[!] failed to create D3D device \n";
        return;
    }

    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pD3DDevice, g_pD3DDeviceContext);

    ImVec4 vecClearColor = ImVec4(0.f, 0.f, 0.f, 0.f);

    const int nFrameTime = 1000 / 200; // overlay fps

    bool bDone = false;
    while (bDone == false)
    {

        auto frameStart = std::chrono::high_resolution_clock::now();

        MSG stMsg;
        while (::PeekMessage(&stMsg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&stMsg);
            ::DispatchMessage(&stMsg);

            if (stMsg.message == WM_QUIT)
            {
                bDone = true;
                break;
            }
        }

        if (bDone == true)
        {
            break;
        }
        
        if (g_nResizeWidth != 0 && g_nResizeHeight != 0)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_nResizeWidth, g_nResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_nResizeWidth = g_nResizeHeight = 0;
            CreateRenderTarget();
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        std::wstring wsWindowTitle = GetForegroundWindowTitle();
        
        SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED);
        if (!wsWindowTitle.empty())
        {
            if (wsWindowTitle == pszTargetWindow) 
            {
                Update();
                
                ImGui::SetNextWindowPos(ImVec2(0, 0));
                ImGui::SetNextWindowSize(ImVec2(m_vecWindowSize.x, m_vecWindowSize.y));

                ImGui::Begin("Overlay", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground |  ImGuiWindowFlags_NoInputs); 
                {
                    ImGui::GetWindowDrawList()->AddText(ImVec2(10, 10), IM_COL32(255, 255, 255, 255), "overlay-base");
                };
                ImGui::End();
            }
        }

        ImGui::Render();

        const float vecClearColorWithAlpha[4] = {vecClearColor.x * vecClearColor.w, vecClearColor.y * vecClearColor.w, vecClearColor.z * vecClearColor.w, vecClearColor.w};
        g_pD3DDeviceContext->OMSetRenderTargets(1, &g_pMainRenderTargetView, nullptr);
        g_pD3DDeviceContext->ClearRenderTargetView(g_pMainRenderTargetView, vecClearColorWithAlpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(0, 0);

        auto frameEnd = std::chrono::high_resolution_clock::now();
        auto frameDuration = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart).count();

        if (frameDuration < nFrameTime)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(nFrameTime - frameDuration));
        }
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(dWndClass.lpszClassName, dWndClass.hInstance);
};