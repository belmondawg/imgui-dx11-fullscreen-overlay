#include "src/overlay/overlay.hpp"

int main()
{
    g_pOverlay = std::make_unique<COverlay>();
    g_pOverlay->Render(L"Counter-Strike 2");
}