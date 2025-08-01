#include "pch.h"

#include "unity/unity_cpp_dumper.hpp"

struct UnityModuleBackendInfo
{
    void* module;
    UnityResolve::Mode mode;
};

UnityModuleBackendInfo GetUnityBackend()
{
    LOG_INFO("Finding Unity backend...");

    UnityModuleBackendInfo info;
    info.module = nullptr;
    info.mode = UnityResolve::Mode::Mono;

    // Check if Unity is loaded
    if (const auto unityModule = GetModuleHandleA("UnityPlayer.dll"); unityModule == nullptr)
    {
        LOG_ERROR("UnityPlayer.dll not found! Is this a Unity game?");
        return info;
    }

    auto assembly = GetModuleHandleA("GameAssembly.dll");
    if (assembly)
    {
        info.module = assembly;
        info.mode = UnityResolve::Mode::Il2Cpp;
        LOG_INFO("Found Il2Cpp backend!");
        return info;
    }
    LOG_INFO("GameAssembly.dll not found, trying fallback to Mono...");

    std::vector<std::string> monoModules = {
        "mono-2.0-bdwgc.dll",
        "mono.dll"
    };

    for (const auto& monoModule : monoModules)
    {
        if (const auto monoHandle = GetModuleHandleA(monoModule.c_str()); monoHandle)
        {
            info.module = monoHandle;
            info.mode = UnityResolve::Mode::Mono;
            LOG_INFO("Found Mono backend: {}", monoModule.c_str());
            return info;
        }
    }

    LOG_ERROR("Unable to find Unity backend! Neither GameAssembly.dll nor mono.dll found.");
    return info;
}

bool InitializeUnity()
{
    auto [module, mode] = GetUnityBackend();
    if (module == nullptr)
    {
        LOG_ERROR("Unity backend not found!");
        return false;
    }

    UnityResolve::Init(module, mode);

    return true;
}

void Main()
{
    Logger::attach().consoleOnly().enableColors();
    if (!InitializeUnity()) LOG_ERROR("Unable to initialize Unity! Maybe assemblies are not found?");

    char rawPath[MAX_PATH];
    if (GetModuleFileNameA(nullptr, rawPath, MAX_PATH) == 0)
    {
        LOG_ERROR("Faileed to get module file name!");
        return;
    }

    std::filesystem::path exePath = rawPath;
    std::filesystem::path outDir = exePath.parent_path() / ".dump\\";

    create_directories(outDir);

    LOG_INFO("Dumping in directory: {}", outDir.string());
    DumpToCppHeaders(outDir.string());
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        std::thread(Main).detach();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
