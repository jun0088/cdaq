// Dear ImGui: standalone example application for DirectX 11
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs
#include <stdio.h>
#include <math.h> 
#include "daq_ui.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

// Data
static ID3D11Device*            g_pd3dDevice = NULL;
static ID3D11DeviceContext*     g_pd3dDeviceContext = NULL;
static IDXGISwapChain*          g_pSwapChain = NULL;
static ID3D11RenderTargetView*  g_mainRenderTargetView = NULL;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


// Main code
int main(int, char**)
{
    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"ImGui Example", NULL };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui DirectX11 Example", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();


    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Our state
    bool show_demo_window = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        
        ImGui::ShowDemoWindow(&show_demo_window);

        // 3. daq graph
        {
            ImGui::Begin("DAQ");

            // IMGUI_DEMO_MARKER("Widgets/Plotting");
            // if (ImGui::CollapsingHeader("Plotting")){
            // static bool animate = true;
            // ImGui::Checkbox("Animate", &animate);

            // static float values[90] = {};
            // static int values_offset = 0;
            // static double refresh_time = 0.0;
            // if (!animate || refresh_time == 0.0)
            //     refresh_time = ImGui::GetTime();
            // while (refresh_time < ImGui::GetTime()) // Create data at fixed 60 Hz rate for the demo
            // {
            //     static float phase = 0.0f;
            //     values[values_offset] = cosf(phase);
            //     values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
            //     phase += 0.10f * values_offset;
            //     refresh_time += 1.0f / 60.0f;
            // }

            // Plots can display overlay texts
            // (in this example, we will display an average value)
            {
                // float average = 0.0f;
                // for (int n = 0; n < IM_ARRAYSIZE(values); n++)
                //     average += values[n];
                // average /= (float)IM_ARRAYSIZE(values);
                // char overlay[32];
                // sprintf(overlay, "avg %f", average);
                std::ifstream file("output.txt");
                if (!file.is_open()) {
                    std::cerr << "Failed to open file." << std::endl;
                    return 1;
                }

                std::vector<float> floatArray;
                std::string line;
                while (std::getline(file, line)) {
                    std::istringstream iss(line);
                    float num;
                    while (iss >> num) {
                        floatArray.push_back(num);
                    }
                }
                file.close();

                // 将vector转换为数组
                float* floatArrayPtr = new float[floatArray.size()];
                for (size_t i = 0; i < floatArray.size(); ++i) {
                    floatArrayPtr[i] = floatArray[i];
                }

                // 输出读取到的浮点数
                // for (size_t i = 0; i < floatArray.size(); ++i) {
                //     std::cout << floatArrayPtr[i] << " ";
                // }
                // std::cout << std::endl;

                // 释放内存
                // delete[] floatArrayPtr;
                ImGui::PlotLines("daq", floatArrayPtr, floatArray.size(), 0, NULL, -0.005f, 0.005f, ImVec2(800, 600.0f));
                delete[] floatArrayPtr;
            }

            ImGui::End();
        }
        static Board board;
        // board_init(&board);
        static bool trig = false;
        {
            ImGui::Begin("Config"); 
            if (ImGui::TreeNode("Board")) {
                ImGui::Checkbox("trig", &trig);
                static ImGuiTableFlags flags1 = ImGuiTableFlags_BordersV;
                ImGui::BeginTable("board", 6, flags1);
                ImGui::TableSetupColumn("Channel");
                ImGui::TableSetupColumn("ctrl(1)");
                ImGui::TableSetupColumn("gain(2)");
                ImGui::TableSetupColumn("iepe(3)");
                ImGui::TableSetupColumn("coupling(4)");
                ImGui::TableSetupColumn("cal(5)");
                ImGui::TableHeadersRow();
                for (int row = 0; row < 7; row++) {
                    ImGui::TableNextRow();
                    for (int column = 0; column < 8; column++) {
                        ImGui::TableSetColumnIndex(column);
                        if (column == 0) {
                            if (row == 0) {      
                                ImGui::Text("All");
                            } else {
                                ImGui::Text("Ch%d", row);                                
                            }
                        } else {
                            bool *tmp = NULL;
                            if (column == CTRL) {
                                tmp = &(board.chs[row].ctrl);
                            } else if (column == GAIN) {
                                tmp = &(board.chs[row].gain);
                            } else if (column == IEPE) {
                                tmp = &(board.chs[row].iepe);
                            } else if (column == COUPLING) {
                                tmp = &(board.chs[row].coupling);
                            } else if (column == CALENABLE) {
                                tmp = &(board.chs[row].calenable);
                            }
                            char str[10] = {0};
                            sprintf(str, "%d,%d", row, column);
                            ImGui::Checkbox(str, tmp);
                            // ImGui::Button("push", ImVec2(-FLT_MIN, 0.0f));
                            // if (data[row][column-1]) {
                            //     ImGui::SameLine();
                            //     ImGui::Text("ON");
                            // } else {
                            //     ImGui::SameLine();
                            //     ImGui::Text("OFF");
                            // }
                        }

                    }
                }
                ImGui::EndTable();
                
            }


            ImGui::End();                        // Create a window called "Hello, world!" and append into it.
        }

        // Rendering
        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0); // Present with vsync
        //g_pSwapChain->Present(0, 0); // Present without vsync
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_WARP, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
