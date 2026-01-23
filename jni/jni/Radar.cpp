#include "main.h"
#include "game/game.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "Radar.h"
#include "util/patch.h"
#include <string>

// GTASA HOOK
#include "gtare/CHud.h"

extern CGame* pGame;
extern CNetGame *pNetGame;
extern CGUI *pGUI;

bool CRadar::m_bShow = false;
bool CRadar::m_bShowDialog = false;

CGtaRect2 CRadar::radarBgPos1; // X Y
CGtaRect2 CRadar::radarBgPos2; // X Y
CGtaRect2 CRadar::radarPos;        // X Y

RwTexture *CRadar::hud_radar;

void CRadar::Initialise()
{   
    Log("CRadar: Initialise");

    hud_radar = nullptr;
    hud_radar = (RwTexture*)LoadTextureFromDB("samp", "hud_back");
    Log("CRadar: Loading..");
}

void CRadar::EditRadar(CRect* rect)
{
//        posX posY ScaleX ScaleY

//        rect->left      = 60.0f; // posX
//        rect->bottom    = 70.0f; // posY
//        -----------------------------------------
//        ((void(*)())(g_libGTASA + 0x003D4ED8 + 1))(); // CRadar::DrawRadar(void)
//        -----------------------------------------
//        rect->right     = 45.0f; // ScaleX
//        rect->top       = 45.0f; //ScaleY

        rect->left      = CRadar::radarPos.x1; // posX
        rect->bottom    = CRadar::radarPos.y1; // posY
		//Log("HUD BACK Radar.h: %f %f", CRadar::radarPos.x1, CRadar::radarPos.y1);
      rect->right     = CRadar::radarPos.x1 / 3.2f; // ScaleX
        rect->top       = CRadar::radarPos.x1 / 3.2f; //ScaleY

}

void CRadar::Render()
{
                      if(CRadar::IsEnabled())
                      {
                                    Log("CRadar: Render");
                     } else Log("CRadar: NoRender");
}
