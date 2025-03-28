#pragma once
#include "../include.hpp"

enum ZBID
{
    ZBID_DEFAULT = 0,
    ZBID_DESKTOP = 1,
    ZBID_UIACCESS = 2,
    ZBID_IMMERSIVE_IHM = 3,
    ZBID_IMMERSIVE_NOTIFICATION = 4,
    ZBID_IMMERSIVE_APPCHROME = 5,
    ZBID_IMMERSIVE_MOGO = 6,
    ZBID_IMMERSIVE_EDGY = 7,
    ZBID_IMMERSIVE_INACTIVEMOBODY = 8,
    ZBID_IMMERSIVE_INACTIVEDOCK = 9,
    ZBID_IMMERSIVE_ACTIVEMOBODY = 10,
    ZBID_IMMERSIVE_ACTIVEDOCK = 11,
    ZBID_IMMERSIVE_BACKGROUND = 12,
    ZBID_IMMERSIVE_SEARCH = 13,
    ZBID_GENUINE_WINDOWS = 14,
    ZBID_IMMERSIVE_RESTRICTED = 15,
    ZBID_SYSTEM_TOOLS = 16,
    ZBID_LOCK = 17,
    ZBID_ABOVELOCK_UX = 18,
};

class COverlay
{
public:
    ImVec2 m_vecWindowSize;
    ImVec2 m_vecWindowPos;
    
    ImVec2 m_vecRenderMargins = ImVec2(10, 10);
    ImVec2 m_vecPadding = ImVec2(6, 3);

    HWND m_Hwnd;

    void Update();
    void Render(const std::wstring& pszTargetWindow); 
    
    std::wstring GetForegroundWindowTitle();
};

inline std::unique_ptr<COverlay> g_pOverlay;