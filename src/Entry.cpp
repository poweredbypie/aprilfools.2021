#include "pch.h"
#include <minhook.h>
#include "memory/Tools.h"
#include "memory/Hooks.h"
#include <iostream>
#include <fstream>

DWORD WINAPI Entry(LPVOID hModule) {
    if (gd::init()) {

        HMODULE cocosBase = GetModuleHandleA("libcocos2d.dll");
        if (!cocosBase) {
            MessageBoxA(nullptr, "Could not find cocos library.", "", MB_OK | MB_ICONERROR);
            goto exit;
        }

        if (MH_Initialize()) MessageBoxA(nullptr, "Couldn't initalize MinHook.", "", MB_ICONERROR | MB_OK);

        MH_CreateHook(
            gd::base + 0x126F40,
            hooks::GaragePage_create,
            reinterpret_cast<LPVOID*>(&gates::GaragePage_create)
        );
        MH_CreateHook(
            gd::base + 0x224D80,
            hooks::SecretLayer3_init,
            reinterpret_cast<LPVOID*>(&gates::SecretLayer3_init)
        );
        MH_CreateHook(
            GetProcAddress(cocosBase, "?dispatchKeyboardMSG@CCKeyboardDispatcher@cocos2d@@QAE_NW4enumKeyCodes@2@_N@Z"),
            hooks::CCKeyboardDispatcher_dispatchKeyboardMSG,
            reinterpret_cast<LPVOID*>(&gates::CCKeyboardDispatcher_dispatchKeyboardMSG)
        );

        if (MH_EnableHook(MH_ALL_HOOKS)) MessageBoxA(nullptr, "Couldn't enable hooks.", "", MB_ICONERROR | MB_OK);

        //overwrite SecretLayer3's ccTouchesBegan so we can write our coin pickup logic. (it doesnt do anything by default)
        //ugly, but vtables are readonly.
        DWORD old;
        VirtualProtect(gd::base + 0x2DB134, 4, PAGE_EXECUTE_READWRITE, &old);
        auto addr = &hooks::SecretLayer3_ccTouchesBegan;
        patch(gd::base + 0x2DB134, &addr, nullptr, 4);
        VirtualProtect(gd::base + 0x2DB134, 4, PAGE_READONLY, &old);

        //change function prologue of SecretLayer::onSubmit so it breaks and the app crashes lol
        patch(gd::base + 0x21C6C0, "\x51", nullptr, 1);

        //change 2 enums passed to GJRewardObject::create so we get the meltdown cube instead
        patch(gd::base + 0x101360, "\x42", nullptr, 1);
        patch(gd::base + 0x101364, "\x09", nullptr, 1);
    }
    else {
    exit:
        MessageBoxA(nullptr, "Could not find module base addresses. Exiting.", "", MB_OK | MB_ICONERROR);
        FreeLibraryAndExitThread(static_cast<HMODULE>(hModule), 0);
    }
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        HANDLE _ = CreateThread(0, 0, Entry, hModule, 0, nullptr);
        if (_) CloseHandle(_);
    }
    return TRUE;
}