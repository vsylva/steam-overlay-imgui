#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imgui_impl_dx12.h"
#include "../vendor/imgui/imgui_impl_win32.h"

#include "menu.h"

#include <d3d12.h>
#include <dxgi1_4.h>
#include <stdio.h>
#include <vector>
#include <windows.h>

using tPresent = HRESULT(__fastcall*)(IDXGISwapChain3*, UINT, UINT);
using tResizeBuffers = HRESULT(__fastcall*)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);

bool g_Initialized = false;

tPresent g_Present = nullptr;
tResizeBuffers g_ResizeBuffers = nullptr;
WNDPROC g_WndProc = nullptr;

ID3D12DescriptorHeap* g_FontDescriptorHeap = nullptr;
ID3D12DescriptorHeap* g_RTVDescriptorHeap = nullptr;
ID3D12CommandQueue* g_CommandQueue = nullptr;
ID3D12GraphicsCommandList* g_CommandList = nullptr;

struct FrameContext {
    ID3D12CommandAllocator* CommandAllocator;
    ID3D12Resource* BackBuffer;
    D3D12_CPU_DESCRIPTOR_HANDLE RTVHandle;
    UINT64 FenceValue;
};

std::vector<FrameContext> g_FrameContexts;

ID3D12Fence* g_Fence = nullptr;
HANDLE g_FenceEvent = NULL;
UINT64 g_FenceValue = 0;

UINT g_BufferCount = 0;
HWND g_hWindow = NULL;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

BOOL GetCommandQueueOffset(UINT* offset) {
    HMODULE hMod = GetModuleHandleW(L"ntdll.dll");
    if (!hMod)
        return FALSE;

    typedef LONG(WINAPI * RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);
    RtlGetVersionPtr rtlGetVersion = (RtlGetVersionPtr)GetProcAddress(hMod, "RtlGetVersion");

    RTL_OSVERSIONINFOW osInfo = {sizeof(RTL_OSVERSIONINFOW)};
    if (rtlGetVersion && rtlGetVersion(&osInfo) == 0) {
        if (osInfo.dwBuildNumber >= 26100)
            *offset = 0x138;
        else if (osInfo.dwBuildNumber >= 21996)
            *offset = 0x168;
        else
            *offset = 0x118;
        return TRUE;
    }
    return FALSE;
}

void WaitForLastSubmittedFrame() {
    if (!g_CommandQueue || !g_Fence || !g_FenceEvent)
        return;
    UINT64 fence = g_FenceValue;
    if (SUCCEEDED(g_CommandQueue->Signal(g_Fence, fence))) {
        g_FenceValue++;
        if (g_Fence->GetCompletedValue() < fence) {
            WaitForSingleObject(g_FenceEvent, INFINITE);
        }
    }
}

void CleanupRenderTarget() {
    WaitForLastSubmittedFrame();
    for (auto& ctx : g_FrameContexts) {
        if (ctx.BackBuffer)
            ctx.BackBuffer->Release();
        if (ctx.CommandAllocator)
            ctx.CommandAllocator->Release();
        ctx.BackBuffer = nullptr;
        ctx.CommandAllocator = nullptr;
    }
    g_FrameContexts.clear();
    if (g_RTVDescriptorHeap) {
        g_RTVDescriptorHeap->Release();
        g_RTVDescriptorHeap = nullptr;
    }
}

void InitOrUpdateRenderTargets(IDXGISwapChain* swapChain, ID3D12Device* device) {
    DXGI_SWAP_CHAIN_DESC desc;
    if (FAILED(swapChain->GetDesc(&desc)))
        return;

    CleanupRenderTarget();

    g_BufferCount = desc.BufferCount;
    g_FrameContexts.resize(g_BufferCount);

    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.NumDescriptors = g_BufferCount;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    if (FAILED(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&g_RTVDescriptorHeap))))
        return;

    UINT rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

    for (UINT i = 0; i < g_BufferCount; i++) {
        g_FrameContexts[i].RTVHandle = rtvHandle;
        if (SUCCEEDED(swapChain->GetBuffer(i, IID_PPV_ARGS(&g_FrameContexts[i].BackBuffer)))) {
            device->CreateRenderTargetView(g_FrameContexts[i].BackBuffer, nullptr, rtvHandle);
        }
        device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_FrameContexts[i].CommandAllocator));
        rtvHandle.ptr += rtvDescriptorSize;
    }
}

LRESULT CALLBACK hkWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
        return 1L;
    return CallWindowProcA(g_WndProc, hwnd, uMsg, wParam, lParam);
}

HRESULT __fastcall
hkResizeBuffers(IDXGISwapChain* swapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT flags) {
    CleanupRenderTarget();
    if (g_Initialized) {
        ImGui_ImplDX12_InvalidateDeviceObjects();
    }

    HRESULT hr = g_ResizeBuffers(swapChain, bufferCount, width, height, newFormat, flags);

    ID3D12Device* device = nullptr;
    if (SUCCEEDED(swapChain->GetDevice(IID_PPV_ARGS(&device)))) {
        InitOrUpdateRenderTargets(swapChain, device);
        if (g_Initialized)
            ImGui_ImplDX12_CreateDeviceObjects();
        device->Release();
    }
    return hr;
}

HRESULT __fastcall hkPresent(IDXGISwapChain3* SwapChain, UINT SyncInterval, UINT Flags) {
    if (!g_Initialized) {
        DXGI_SWAP_CHAIN_DESC desc;
        if (FAILED(SwapChain->GetDesc(&desc)))
            return g_Present(SwapChain, SyncInterval, Flags);

        g_hWindow = desc.OutputWindow;
        ID3D12Device* device = nullptr;
        if (FAILED(SwapChain->GetDevice(IID_PPV_ARGS(&device))))
            return g_Present(SwapChain, SyncInterval, Flags);

        if (!g_CommandQueue) {
            UINT queueOffset = 0;
            if (GetCommandQueueOffset(&queueOffset)) {
                g_CommandQueue = *reinterpret_cast<ID3D12CommandQueue**>((uintptr_t)SwapChain + queueOffset);
            }
        }

        if (!g_CommandQueue) {
            device->Release();
            return g_Present(SwapChain, SyncInterval, Flags);
        }

        device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_Fence));
        g_FenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

        InitOrUpdateRenderTargets(SwapChain, device);

        D3D12_DESCRIPTOR_HEAP_DESC fontHeapDesc = {};
        fontHeapDesc.NumDescriptors = 1;
        fontHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        fontHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        device->CreateDescriptorHeap(&fontHeapDesc, IID_PPV_ARGS(&g_FontDescriptorHeap));

        device->CreateCommandList(
            0,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            g_FrameContexts[0].CommandAllocator,
            nullptr,
            IID_PPV_ARGS(&g_CommandList)
        );
        g_CommandList->Close();

        ImGui::CreateContext();
        ImGui_ImplWin32_Init(g_hWindow);
        ImGui_ImplDX12_Init(
            device,
            desc.BufferCount,
            desc.BufferDesc.Format,
            g_FontDescriptorHeap,
            g_FontDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
            g_FontDescriptorHeap->GetGPUDescriptorHandleForHeapStart()
        );

        g_WndProc = (WNDPROC)SetWindowLongPtr(g_hWindow, GWLP_WNDPROC, (LONG_PTR)hkWndProc);

        device->Release();

        ImGuiIO& io = ImGui::GetIO();

        ImFont* font =
            io.Fonts->AddFontFromFileTTF("c:\\windows\\fonts\\msyh.ttc", 20.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());

        IM_ASSERT(font != NULL);

        io.FontDefault = font;

        io.Fonts->Build();

        g_Initialized = true;
    }

    UINT bufferIdx = SwapChain->GetCurrentBackBufferIndex();
    auto& currentFrame = g_FrameContexts[bufferIdx];

    if (g_Fence->GetCompletedValue() < currentFrame.FenceValue) {
        g_Fence->SetEventOnCompletion(currentFrame.FenceValue, g_FenceEvent);
        WaitForSingleObject(g_FenceEvent, INFINITE);
    }

    currentFrame.CommandAllocator->Reset();

    D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    g_CommandList->Reset(currentFrame.CommandAllocator, nullptr);

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = currentFrame.BackBuffer;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    g_CommandList->ResourceBarrier(1, &barrier);

    g_CommandList->OMSetRenderTargets(1, &currentFrame.RTVHandle, FALSE, nullptr);
    g_CommandList->SetDescriptorHeaps(1, &g_FontDescriptorHeap);

    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    RenderFPSToolDemo();

    ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_CommandList);

    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    g_CommandList->ResourceBarrier(1, &barrier);

    g_CommandList->Close();
    ID3D12CommandList* ppCommandLists[] = {g_CommandList};
    g_CommandQueue->ExecuteCommandLists(1, ppCommandLists);

    g_FenceValue++;
    g_CommandQueue->Signal(g_Fence, g_FenceValue);
    currentFrame.FenceValue = g_FenceValue;

    return g_Present(SwapChain, SyncInterval, Flags);
}

INT D3D12HookThread() {
    uintptr_t create_hook_addr = (uintptr_t)GetModuleHandleW(L"GameOverlayRenderer64.dll") + 0x8EE40;
    uintptr_t paddr = (uintptr_t)GetModuleHandleW(L"GameOverlayRenderer64.dll") + 0x95230;
    uintptr_t raddr = (uintptr_t)GetModuleHandleW(L"GameOverlayRenderer64.dll") + 0x95600;

    using SteamCreateHook_t = __int64(__fastcall*)(unsigned __int64, __int64, unsigned __int64*, int, const char*);
    auto SteamCreateHook = reinterpret_cast<SteamCreateHook_t>(create_hook_addr);

    __int64 result =
        SteamCreateHook((unsigned __int64)paddr, (__int64)&hkPresent, (unsigned __int64*)&g_Present, 1, "DXGISwapChain_Present");

    __int64 result1 = SteamCreateHook(
        (unsigned __int64)raddr,
        (__int64)&hkResizeBuffers,
        (unsigned __int64*)&g_ResizeBuffers,
        1,
        "DXGISwapChain_ResizeBuffers"
    );

    return 0;
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID) {
    DisableThreadLibraryCalls(hInstance);
    if (fdwReason == DLL_PROCESS_ATTACH) {
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)D3D12HookThread, NULL, 0, NULL);
    }

    return TRUE;
}