//
// Created by EDGAR 3.0
//
#include <jni.h>
#include "TurnLeftAndRight.h"
#include "main.h"
#include "../game/game.h"
#include "../game/vehicle.h"
#include "../game/object.h"
#include "net/netgame.h"
#include "settings.h"
extern CGame *pGame;
extern CNetGame* pNetGame;
extern CSettings* pSettings;
extern CTurnLeftAndRight* pTurnLeftAndRight;

CTurnLeftAndRight::CTurnLeftAndRight(/*CVehicle* pVeh*/)
{
    //veh = pVeh;
}

CTurnLeftAndRight::~CTurnLeftAndRight()
{
}
void CTurnLeftAndRight::CVehSet(CVehicle* pVeh)
{
    pTurnLeftAndRight->veh = pVeh;
}

extern "C"
JNIEXPORT void JNICALL
Java_ru_edgar_matrp_gui_Speedometer_sendClick(JNIEnv *env, jobject thiz, jint click_id) {
    switch(click_id)
    {
        case 0:
        {
            pNetGame->SendChatCommand("/engine");
            break;
        }
        case 4:
        {
            pNetGame->SendChatCommand("/lights");
            break;
        }
        case 1:
        {
            pTurnLeftAndRight->veh->toggleRightTurnLight(true);
            break;
        }
        case 2:
        {
            pTurnLeftAndRight->veh->toggleRightTurnLight(false);
            break;
        }
        case 3:
        {
           /* CPlayerPed *pPlayerPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->m_pPlayerPed;
            CVehicle* pVehicle = pPlayerPed->GetCurrentVehicle();

            if(pVehicle->m_iTurnState == CVehicle::eTurnState::TURN_ALL)
                pVehicle->m_iTurnState = CVehicle::eTurnState::TURN_OFF;
            else
                pVehicle->m_iTurnState = CVehicle::eTurnState::TURN_ALL;
*/
            break;
        }
    }
}