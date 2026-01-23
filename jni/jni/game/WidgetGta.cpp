//
// Created by plaka on 03.02.2023.
//

#include "WidgetGta.h"
#include "main.h"
#include "game/game.h"
#include "net/netgame.h"
#include "util/patch.h"
uintptr_t *CWidgetGta::pWidgets = nullptr; // int CTouchInterface::m_pWidgets[]
extern CNetGame *pNetGame;
extern CGame *pGame;

void (*CWidgetButton__Draw)(int thiz);

void CWidgetButton__Draw_hook(int thiz) {
//    if (thiz == CWidgetGta::pWidgets[WIDGET_POSITION_HORN]) {
//        return;
//    }

    CWidgetButton__Draw(thiz);
}

void CWidgetGta::setEnabled(int thiz, bool bEnabled) {
    *(BYTE *) (thiz + 0x4D) = bEnabled; // this->m_bEnabled
}
void (*CWidgetButton__Update)(int result, int a2, int a3, int a4);

void CWidgetButton__Update_hook(int result, int a2, int a3, int a4) {
    if (!result) {
        return;
    }

    //  CWidgetGta::setEnabled(CWidgetGta::pWidgets[WIDGET_POSITION_ENTER_CAR], false);

  //  CWidgetGta::setEnabled(CWidgetGta::pWidgets[WIDGET_POSITION_ENTER_CAR], false);
    /*CWidgetGta::setEnabled(CWidgetGta::pWidgets[WIDGET_POSITION_CAR_SHOOT], false);
    CWidgetGta::setEnabled(CWidgetGta::pWidgets[WIDGET_POSITION_ACCELERATE], false);
    CWidgetGta::setEnabled(CWidgetGta::pWidgets[WIDGET_POSITION_BUTTON_SPRINT_AND_SWIM], false);
    CWidgetGta::setEnabled(CWidgetGta::pWidgets[WIDGET_POSITION_BRAKE], false);
    CWidgetGta::setEnabled(CWidgetGta::pWidgets[WIDGET_POSITION_HORN], false);
    CWidgetGta::setEnabled(CWidgetGta::pWidgets[WIDGET_POSITION_HORN_ALT], false);
    CWidgetGta::setEnabled(CWidgetGta::pWidgets[WIDGET_POSITION_VEHICLE_SHOOT_LEFT], false);
    CWidgetGta::setEnabled(CWidgetGta::pWidgets[WIDGET_POSITION_VEHICLE_SHOOT_RIGHT], false);
    CWidgetGta::setEnabled(CWidgetGta::pWidgets[WIDGET_POSITION_TARGETING_AND_ATTACK], false);
    CWidgetGta::setEnabled(CWidgetGta::pWidgets[WIDGET_POSITION_VC_SHOOT], false);
    CWidgetGta::setEnabled(CWidgetGta::pWidgets[WIDGET_POSITION_VC_SHOOT_ALT], false);
    */
    CWidgetButton__Update(result, a2, a3, a4);
}


void CWidgetGta::init() {
    CWidgetGta::pWidgets = (uintptr_t *) (g_libGTASA + 0x00657E48);

    CHook::InlineHook(g_libGTASA, 0x00274AB4, &CWidgetButton__Update_hook, &CWidgetButton__Update);
   // CHook::SetUpHook(g_libGTASA + 0x00274748, (uintptr_t) CWidgetButton__Draw_hook,(uintptr_t *) &CWidgetButton__Draw);
}


