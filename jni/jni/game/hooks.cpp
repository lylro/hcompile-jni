#include "hooks.h"
#include "xorstr.h"

extern bool g_bInitGameProcess;

RakClientInterface* pRakClient = RakNetworkFactory::GetRakClientInterface();

void (*orig_CNetTextDrawPool__SetServerLogo)(uintptr_t thiz, std::string url);
void hook_CNetTextDrawPool__SetServerLogo(uintptr_t thiz, std::string url)
{
	//RegisterRPCs(pRakClient);
    orig_CNetTextDrawPool__SetServerLogo(thiz, url);
}

void (*orig_CNetGame__ProcessNetwork)();
void hook_CNetGame__ProcessNetwork()
{
    // Receive zamena packets
    CNetGame::ProcessNetwork();
}
#include "..//settings.h"
extern CSettings* pSettings;
bool (*orig_RakClient__Connect)(uintptr_t thiz, const char* host, uint16_t serverPort, uint16_t clientPort, unsigned int depreciated, int threadSleepTimer);
bool hook_RakClient__Connect(uintptr_t thiz, const char* host, uint16_t serverPort, uint16_t clientPort, unsigned int depreciated, int threadSleepTimer)
{
    return pRakClient->Connect("195.18.27.241", 2874, 0, 0, 5);
}

// 188.127.241.74:6641

void (*orig_RakClient__RegisterAsRemoteProcedureCall)(uintptr_t thiz, BRRpcIds id, void (*functionPointer)(RPCParameters* rpcParams));
void hook_RakClient__RegisterAsRemoteProcedureCall(uintptr_t thiz, BRRpcIds id, void (*functionPointer)(RPCParameters* rpcParams))
{
	int sampRpcId = ConvertBRIDToSampID(id);
	if(sampRpcId != -1) {
		pRakClient->RegisterAsRemoteProcedureCall(&sampRpcId, functionPointer);
	}
}

bool (*orig_RakClient__RPC)( uintptr_t thiz, BRRpcIds uniqueID, RakNet::BitStream *bitStream, PacketPriority priority, BRPacketReliability reliability, char orderingChannel, bool shiftTimestamp, NetworkID networkID, RakNet::BitStream *replyFromTarget );
bool hook_RakClient__RPC( uintptr_t thiz, BRRpcIds uniqueID, RakNet::BitStream *bitStream, PacketPriority priority, BRPacketReliability reliability, char orderingChannel, bool shiftTimestamp, NetworkID networkID, RakNet::BitStream *replyFromTarget )
{
	int sampRpcId = ConvertBRIDToSampID(uniqueID);
	if(sampRpcId != -1) {
		if(sampRpcId == RPC_RequestClass && g_bInitGameProcess) {
			return false;
		}
		return pRakClient->RPC(&sampRpcId, bitStream, priority, ConvertBRToSampReliability(reliability), orderingChannel, shiftTimestamp, networkID, replyFromTarget);
	} else {
		// CChat::AddDebugMessage(xorstr("Unknown RPC: %i"), uniqueID);
	}
	return false;
}


void FLog(const char* fmt, ...)
{
	char buffer[0xFF];
	static FILE* flLog = nullptr;
	const char* pszStorage = "/storage/emulated/0/Android/data/com.bless.client/";


	if (flLog == nullptr && pszStorage != nullptr)
	{
		flLog = fopen(buffer, "a");
	}

	memset(buffer, 0, sizeof(buffer));

	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, arg);
	va_end(arg);

	if (flLog == nullptr) return;
	fprintf(flLog, "%s\n", buffer);
	fflush(flLog);

	return;
}

struct VehicleTurnSignal {
    bool leftOn = false;
    bool rightOn = false;
    bool leftBlinkState = false;
    bool rightBlinkState = false;
    float timer = 0.0f;
};
std::map<int, VehicleTurnSignal> g_VehicleSignals;


bool (*orig_RakClient__Send)( uintptr_t thiz, RakNet::BitStream* bitStream, PacketPriority priority, BRPacketReliability reliability, char orderingChannel );
bool hook_RakClient__Send(uintptr_t thiz, RakNet::BitStream* bitStream, PacketPriority priority, BRPacketReliability reliability, char orderingChannel)
{
    RakNet::BitStream bsCopy(bitStream->GetData(), bitStream->GetNumberOfBytesUsed() + 1, false);
    uint8_t pktId;
    bsCopy.Read(pktId);
    if (pktId == BR_ID_GUI_PACKET_ID) {
    uint16_t guiId;
    uint32_t jsonLen;
    bsCopy.Read(guiId);
    bsCopy.Read(jsonLen);

    FLog("Gui id %i", guiId);
    FLog("Last samp id rpc %i", CNetGame::m_nLastSAMPDialogID);

    if (jsonLen > 0 && jsonLen < 4096) {
        bsCopy.Read(CGUI::buffGUI, jsonLen);
        CGUI::buffGUI[jsonLen] = 0;

        static char* buffGUI;
        if (!buffGUI) { buffGUI = new char[4096]; }

        cp1251_to_utf8(buffGUI, CGUI::buffGUI);

        if (guiId == 10) {
            if (nlohmann::json::accept(buffGUI)) {
                nlohmann::json jsonObj = nlohmann::json::parse(buffGUI);
                uint8_t btn = jsonObj["r"];
                int16_t listInput = jsonObj["l"];
                std::string input = jsonObj["i"];
                uint8_t inputLen = input.length();

                RakNet::BitStream bsSend;
                bsSend.WriteBits((unsigned char *)&CNetGame::m_nLastSAMPDialogID, 16);
                bsSend.WriteBits((unsigned char *)&btn, 8);
                bsSend.WriteBits((unsigned char *)&listInput, 16);
                bsSend.WriteBits((unsigned char *)&inputLen, 8);
                bsSend.Write(input.c_str(), inputLen);

                bool result = pRakClient->RPC(&RPC_DialogResponse, &bsSend,
                                              HIGH_PRIORITY, RELIABLE_ORDERED, 0,
                                              false, UNASSIGNED_NETWORK_ID, NULL);
                if (result) {
                    //CChat::AddDebugMessage("Response sended!");
                } else {
                    CChat::AddDebugMessage("Response not sended..");
                }
            }
        }
        else {
            CNetGame::SendOnData(guiId, buffGUI, jsonLen);
        }

        }
    }
    // Обработка остальных пакетов (AIM_SYNC, BULLET_SYNC, PLAYER_SYNC, VEHICLE_SYNC, PASSENGER_SYNC)
    if (pktId == BR_ID_AIM_SYNC) {
        uint8_t pktAimSync = ID_AIM_SYNC;
        uint8_t aimSyncBuffer[31] = {0};
        bsCopy.ReadBits(aimSyncBuffer, 31 * 8);
        RakNet::BitStream bsSend;
        bsSend.WriteBits(&pktAimSync, 8);
        bsSend.WriteBits(aimSyncBuffer, 31 * 8);
        return pRakClient->Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
    }
    if (pktId == BR_ID_BULLET_SYNC) {
        uint8_t pktBltSync = ID_BULLET_SYNC;
        uint8_t bltSyncBuffer[40] = {0};
        bsCopy.ReadBits(bltSyncBuffer, 40 * 8);
        RakNet::BitStream bsSend;
        bsSend.WriteBits(&pktBltSync, 8);
        bsSend.WriteBits(bltSyncBuffer, 40 * 8);
        return pRakClient->Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
    }
    if (pktId == BR_ID_PLAYER_SYNC) {
        uint8_t pktPlayerSync = ID_PLAYER_SYNC;
        BROnFootSyncData data;
        bsCopy.ReadBits((unsigned char *)&data.lrAnalogLeftStick, 16);
        bsCopy.ReadBits((unsigned char *)&data.udAnalogLeftStick, 16);
        bsCopy.ReadBits((unsigned char *)&data.wKeys, 16);
        bsCopy.ReadBits((unsigned char *)&data.vecPos, 96);
        bsCopy.ReadBits((unsigned char *)&data.quatw, 32);
        bsCopy.ReadBits((unsigned char *)&data.quatx, 32);
        bsCopy.ReadBits((unsigned char *)&data.quaty, 32);
        bsCopy.ReadBits((unsigned char *)&data.quatz, 32);
        bsCopy.ReadBits((unsigned char *)&data.health, 16);
        bsCopy.ReadBits((unsigned char *)&data.armour, 16);
        bsCopy.ReadBits((unsigned char *)&data.byteCurrentWeapon, 8);
        bsCopy.ReadBits((unsigned char *)&data.byteSpecialAction, 8);
        bsCopy.ReadBits((unsigned char *)&data.vecMoveSpeed, 96);
        bsCopy.ReadBits((unsigned char *)&data.vecSurfOffsets, 96);
        bsCopy.ReadBits((unsigned char *)&data.wSurfInfo, 16);
        bsCopy.ReadBits((unsigned char *)&data.dwAnimation, 32);
        RakNet::BitStream bsSend;
        bsSend.WriteBits(&pktPlayerSync, 8);
        ConvertBROnFootSyncToSampSync(&bsSend, data);
        return pRakClient->Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
    }
    if (pktId == BR_ID_VEHICLE_SYNC) {
    uint8_t pktVehicleSync = ID_VEHICLE_SYNC;
    BRInCarSyncData data;

// colModel check null-pointer
uint32_t (*CCollision__ProcessVerticalLine)(float *colLine, float *transform, int colModel, int colPoint, int *maxTouchDistance, char seeThrough, int shootThrough, int storedCollPoly);
uint32_t CCollision__ProcessVerticalLine_hook(float *colLine, float *transform, int colModel, int colPoint, int *maxTouchDistance, char seeThrough, int shootThrough, int storedCollPoly)
{
	if(colModel)
		return CCollision__ProcessVerticalLine(colLine, transform, colModel, colPoint, maxTouchDistance, seeThrough, shootThrough, storedCollPoly);

	return 0;
}

int (*_RwTextureDestroy)(RwTexture*);
int _RwTextureDestroy_hook(RwTexture* texture)
{
    if(!texture)
    	return 1;

    return _RwTextureDestroy(texture);
}

INSTALL HOOKS 

	SetUpHook(SA_ADDR(0x29947C), (uintptr_t)CCollision__ProcessVerticalLine_hook, (uintptr_t*)&CCollision__ProcessVerticalLine);
	SetUpHook(SA_ADDR(0x1B1808), (uintptr_t)_RwTextureDestroy_hook, (uintptr_t*)&_RwTextureDestroy);

// фикс пикселей какие та с драйв рашко

void (*RenderEffects)();
void RenderEffects_hook()
{
    // fix render spheres
    if(pSettings->GetReadOnly().iGraphics == 0)
    {
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)0);
        RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)5);
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)6);
        RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)1);
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)1);
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)0);

        CallFunction<void>(g_libGTASA + 0x54C330 + 1);
        CallFunction<void>(g_libGTASA + 0x54D738 + 1);
        CallFunction<void>(g_libGTASA + 0x55D438 + 1); // CVisibilityPlugins::RenderReallyDrawLastObjects(void)	0055D438
        CallFunction<void>(g_libGTASA + 0x52D528 + 1); // CCoronas::Render(void)	0052D528
        CallFunction<void>(g_libGTASA + 0x56AF94 + 1); // CWeaponEffects::Render(void)	0056AF94
    }
    else
    {
        RenderEffects();
    }
}

    InlineHook(g_libGTASA, 0x39AE28, &RenderEffects_hook, &RenderEffects);



    
// фикс пикселей в углу когда ебашишь с тачкой на 300 км/час

float (*CDraw__SetFOV)(float thiz, float a2);
float CDraw__SetFOV_hook(float thiz, float a2)
{
    float tmp = (float)((float)((float)(*(float *)&*(float *)(g_libGTASA + 0x0098525C) - 1.3333) * 11.0) / 0.44444) + thiz;
    if(tmp > 100) tmp = 100.0;
    *(float *)(g_libGTASA + 0x00610968) = tmp;
    return thiz;
}

в InstallHooks

    InlineHook(g_libGTASA, 0x005311D0, &CDraw__SetFOV_hook, &CDraw__SetFOV);
    
 
 
 
 
 crash.fix
 
 uint32_t
(*CCollision__ProcessVerticalLine)(float *colLine, float *transform, int colModel, int colPoint, int *maxTouchDistance,
                                   char seeThrough, int shootThrough, int storedCollPoly);

uint32_t CCollision__ProcessVerticalLine_hook(float *colLine, float *transform, int colModel, int colPoint,
                                              int *maxTouchDistance, char seeThrough, int shootThrough,
                                              int storedCollPoly) {
    if (colModel) {
        return CCollision__ProcessVerticalLine(colLine, transform, colModel, colPoint,
                                               maxTouchDistance, seeThrough,
                                               shootThrough, storedCollPoly);
    }

    return 0;
}

в InstallHooks

    // -- Crash fix
    installHook(SA_ADDR(0x29947C), (uintptr_t) CCollision__ProcessVerticalLine_hook,
                (uintptr_t *) &CCollision__ProcessVerticalLine);
                
                
                
 



// фикс огней типа

float* float_4DD9E8;
float *ms_fTimeStep1;
float fMagic = 50.0f / 30.0f;

в InstallHooks

    unProtect(g_libGTASA + 0x46CE34); //fix walk and fire









// фиксы уронов с различных ганов и фикс ганов


/* =========================================== Ped damage handler =========================================== */
void onDamage(PED_TYPE *issuer, PED_TYPE *damaged) {
    if (!pNetGame) return;
    PED_TYPE *pPedPlayer = GamePool_FindPlayerPed();
    if (damaged && (pPedPlayer == issuer)) {
        if (CPlayerPool::FindRemotePlayerIDFromGtaPtr((PED_TYPE *)damaged) != INVALID_PLAYER_ID) {
            CAMERA_AIM *caAim = CPlayerPool::GetLocalPlayer()->GetPlayerPed()->GetCurrentAim();

            CVector aim;
            aim.x = caAim->f1x;
            aim.y = caAim->f1y;
            aim.z = caAim->f1z;

            CPlayerPool::GetLocalPlayer()->SendBulletSyncData(
                    CPlayerPool::FindRemotePlayerIDFromGtaPtr((PED_TYPE *) damaged), BULLET_HIT_TYPE_PLAYER, aim);
        }
    }
}

float m_fWeaponDamagess[55] =
        {
                5.0,   // 0 - Fist
                5.0,   // 1 - Brass knuckles
                5.0,   // 2 - Golf club
                5.0,   // 3 - Nitestick
                5.0,   // 4 - Knife
                5.0,   // 5 - Bat
                5.0,   // 6 - Shovel
                5.0,   // 7 - Pool cue
                5.0,   // 8 - Katana
                5.0,   // 9 - Chainsaw
                5.0,   // 10 - Dildo
                5.0,   // 11 - Dildo 2
                5.0,   // 12 - Vibrator
                5.0,   // 13 - Vibrator 2
                5.0,   // 14 - Flowers
                5.0,   // 15 - Cane
                82.5,  // 16 - Grenade
                0.0,   // 17 - Teargas
                1.0,   // 18 - Molotov
                9.9,   // 19 - Vehicle M4 (custom)
                46.2,  // 20 - Vehicle minigun (custom)
                0.0,   // 21
                8.25,  // 22 - Colt 45
                13.2,  // 23 - Silenced
                46.2,  // 24 - Deagle
                3.3,   // 25 - Shotgun
                3.3,   // 26 - Sawed-off
                4.95,  // 27 - Spas
                6.6,   // 28 - UZI
                8.25,  // 29 - MP5
                9.9,   // 30 - AK47
                9.9,   // 31 - M4
                6.6,   // 32 - Tec9
                24.75, // 33 - Cuntgun
                41.25, // 34 - Sniper
                82.5,  // 35 - Rocket launcher
                82.5,  // 36 - Heatseeker
                1.0,   // 37 - Flamethrower
                46.2,  // 38 - Minigun
                82.5,  // 39 - Satchel
                0.0,   // 40 - Detonator
                0.33,  // 41 - Spraycan
                0.33,  // 42 - Fire extinguisher
                0.0,   // 43 - Camera
                0.0,   // 44 - Night vision
                0.0,   // 45 - Infrared
                0.0,   // 46 - Parachute
                0.0,   // 47 - Fake pistol
                2.64,  // 48 - Pistol whip (custom)
                9.9,   // 49 - Vehicle
                330.0, // 50 - Helicopter blades
                82.5,  // 51 - Explosion
                1.0,   // 52 - Car park (custom)
                1.0,   // 53 - Drowning
                165.0  // 54 - Splat
        };

enum ePedPieceTypes {
    PED_PIECE_UNKNOWN = 0,

    PED_PIECE_TORSO = 3,
    PED_PIECE_ASS,
    PED_PIECE_LEFT_ARM,
    PED_PIECE_RIGHT_ARM,
    PED_PIECE_LEFT_LEG,
    PED_PIECE_RIGHT_LEG,
    PED_PIECE_HEAD
};

struct CPedDamageResponseInterface
{
    float				fDamageHealth;
    float				fDamageArmor;
    bool				bUnk;
    bool				bForceDeath;
    bool				bDamageCalculated;
    bool				bUnk3;
};

struct CPedDamageResponseCalculatorInterface
{
    ENTITY_TYPE*		pEntity;
    float               fDamage;
    ePedPieceTypes      bodyPart;
    unsigned int        weaponType;
    bool                bSpeak;            // refers to a CPed::Say call (the dying scream?)
};

uintptr_t(*CPedDamageResponseCalculator__ComputeDamageResponse)(CPedDamageResponseCalculatorInterface*, PED_TYPE*, CPedDamageResponseInterface*, bool);
uintptr_t CPedDamageResponseCalculator__ComputeDamageResponse_hook(CPedDamageResponseCalculatorInterface* pPedDamageResponseCalculator, PED_TYPE* pDamagedPed, CPedDamageResponseInterface* pPedDamageResponse, bool bSpeak)
{
    // Make sure that everything is not null
    if (!pNetGame || !pPedDamageResponseCalculator || !pDamagedPed || !pPedDamageResponse || !pPedDamageResponseCalculator->pEntity) return CPedDamageResponseCalculator__ComputeDamageResponse(pPedDamageResponseCalculator, pDamagedPed, pPedDamageResponse, bSpeak);

    CLocalPlayer* pLocalPlayer = CPlayerPool::GetLocalPlayer();
    if(!pLocalPlayer) return CPedDamageResponseCalculator__ComputeDamageResponse(pPedDamageResponseCalculator, pDamagedPed, pPedDamageResponse, bSpeak);

    PED_TYPE* pInflictor = (PED_TYPE*)pPedDamageResponseCalculator->pEntity;
    PED_TYPE* pLocalPed = GamePool_FindPlayerPed();
    if(!pLocalPed) return CPedDamageResponseCalculator__ComputeDamageResponse(pPedDamageResponseCalculator, pDamagedPed, pPedDamageResponse, bSpeak);

    uint32_t weaponid = pPedDamageResponseCalculator->weaponType;
    uint32_t bodypart = pPedDamageResponseCalculator->bodyPart;

    float fDamage = (pPedDamageResponseCalculator->fDamage / 3.0303030303);
    if (fDamage < 1.0) fDamage = m_fWeaponDamagess[weaponid];

    if (weaponid < 0)
        weaponid = 255; //Suicide
    else if (weaponid == 18)
        weaponid = 37; //Flamethower
    else if (weaponid == 35 || weaponid == 16)
        weaponid = 51; //Explosion

    PLAYERID damagedid = CPlayerPool::FindRemotePlayerIDFromGtaPtr((PED_TYPE *)pPedDamageResponseCalculator);
    PLAYERID issuerid = CPlayerPool::FindRemotePlayerIDFromGtaPtr((PED_TYPE *)pDamagedPed);

    VEHICLEID vehicleId = CVehiclePool::FindIDFromGtaPtr((VEHICLE_TYPE*)pPedDamageResponseCalculator->pEntity);
    if (vehicleId != INVALID_VEHICLE_ID)
    {
        CVehicle *pVehicle = CVehiclePool::GetAt(vehicleId);
        if (pVehicle)
        {
            if (pVehicle->GetVehicleSubtype() == VEHICLE_SUBTYPE_PLANE)
                fDamage = m_fWeaponDamagess[50];

            else
                fDamage = m_fWeaponDamagess[49];
        }
    }

    // self damage like fall damage, drowned, etc
    if(issuerid == INVALID_PLAYER_ID && damagedid == INVALID_PLAYER_ID)
    {
        PLAYERID playerId = CPlayerPool::GetLocalPlayerID();
        //CPlayerPool::GetLocalPlayer()->SendTakeDamageEvent(playerId, fDamage, weaponid, bodypart);
    }

    // give player damage
    //if(issuerid != INVALID_PLAYER_ID && damagedid == INVALID_PLAYER_ID)
        //CPlayerPool::GetLocalPlayer()->SendGiveDamageEvent(issuerid, fDamage, weaponid, bodypart);

        // player take damage
    //else if(issuerid == INVALID_PLAYER_ID && damagedid != INVALID_PLAYER_ID)
        //CPlayerPool::GetLocalPlayer()->SendTakeDamageEvent(damagedid, fDamage, weaponid, bodypart);

    return CPedDamageResponseCalculator__ComputeDamageResponse(pPedDamageResponseCalculator, pDamagedPed, pPedDamageResponse, bSpeak);
}


/*struct CPedDamageResponseInterface {
    float fDamageHealth;
    float fDamageArmor;
    bool bUnk;
    bool bForceDeath;
    bool bDamageCalculated;
    bool bUnk3;
};

struct CPedDamageResponseCalculatorInterface {
    ENTITY_TYPE *pEntity;
    float fDamage;
    ePedPieceTypes bodyPart;
    unsigned int weaponType;
    bool bSpeak;            // refers to a CPed::Say call (the dying scream?)
};

uintptr_t (*CPedDamageResponseCalculator__ComputeDamageResponse)(CPedDamageResponseCalculatorInterface *, PED_TYPE *,
                                                                 CPedDamageResponseInterface *, bool);

uintptr_t CPedDamageResponseCalculator__ComputeDamageResponse_hook(
        CPedDamageResponseCalculatorInterface *pPedDamageResponseCalculator, PED_TYPE *pDamagedPed,
        CPedDamageResponseInterface *pPedDamageResponse, bool bSpeak) {
    //if (pDamagedPed && pPedDamageResponseCalculator)
        //onDamage((PED_TYPE *) *(uintptr_t *) pPedDamageResponseCalculator, (PED_TYPE *) pDamagedPed);
    // Make sure that everything is not null
    if (!pNetGame || !pPedDamageResponseCalculator || !pDamagedPed || !pPedDamageResponse ||
        !pPedDamageResponseCalculator->pEntity)
        return CPedDamageResponseCalculator__ComputeDamageResponse(pPedDamageResponseCalculator, pDamagedPed,
                                                                   pPedDamageResponse, bSpeak);

    CLocalPlayer *pLocalPlayer = CPlayerPool::GetLocalPlayer();
    if (!pLocalPlayer)
        return CPedDamageResponseCalculator__ComputeDamageResponse(pPedDamageResponseCalculator, pDamagedPed,
                                                                   pPedDamageResponse, bSpeak);

    PED_TYPE *pInflictor = (PED_TYPE *) pPedDamageResponseCalculator->pEntity;
    PED_TYPE *pLocalPed = GamePool_FindPlayerPed();
    if (!pLocalPed)
        return CPedDamageResponseCalculator__ComputeDamageResponse(pPedDamageResponseCalculator, pDamagedPed,
                                                                   pPedDamageResponse, bSpeak);

    uint32_t weaponid = pPedDamageResponseCalculator->weaponType;
    uint32_t bodypart = pPedDamageResponseCalculator->bodyPart;

    float fDamage = (pPedDamageResponseCalculator->fDamage / 3.0303030303);
    if (fDamage < 1.0) fDamage = m_fWeaponDamagess[weaponid];

    if (weaponid < 0)
        weaponid = 255; //Suicide
    else if (weaponid == 18)
        weaponid = 37; //Flamethower
    else if (weaponid == 35 || weaponid == 16)
        weaponid = 51; //Explosion

    PLAYERID damagedid = CPlayerPool::FindRemotePlayerIDFromGtaPtr((ENTITY_TYPE*)pPedDamageResponseCalculator);
    PLAYERID issuerid = CPlayerPool::FindRemotePlayerIDFromGtaPtr((ENTITY_TYPE*)pDamagedPed);

    VEHICLEID vehicleId = CVehiclePool::FindIDFromGtaPtr((VEHICLE_TYPE *) pPedDamageResponseCalculator->pEntity);
    if (vehicleId != INVALID_VEHICLE_ID) {
        CVehicle *pVehicle = CVehiclePool::GetAt(vehicleId);
        if (pVehicle) {
            if (pVehicle->GetVehicleSubtype() == VEHICLE_SUBTYPE_PLANE)
                fDamage = m_fWeaponDamagess[50];

            else
                fDamage = m_fWeaponDamagess[49];
        }
    }

    // self damage like fall damage, drowned, etc
    if (issuerid == INVALID_PLAYER_ID && damagedid == INVALID_PLAYER_ID) {
        PLAYERID playerId = CPlayerPool::GetLocalPlayerID();
        CPlayerPool::GetLocalPlayer()->GiveTakeDamage(true, playerId, fDamage, weaponid, bodypart);
    }

    // give player damage
    if (issuerid != INVALID_PLAYER_ID && damagedid == INVALID_PLAYER_ID)
        CPlayerPool::GetLocalPlayer()->GiveTakeDamage(false, issuerid, fDamage, weaponid, bodypart);

        // player take damage
    else if (issuerid == INVALID_PLAYER_ID && damagedid != INVALID_PLAYER_ID)
        CPlayerPool::GetLocalPlayer()->GiveTakeDamage(true, damagedid, fDamage, weaponid, bodypart);

    return CPedDamageResponseCalculator__ComputeDamageResponse(pPedDamageResponseCalculator, pDamagedPed,
                                                               pPedDamageResponse, bSpeak);
}*/

/* ========================================================================================================== */

int (*CWeapon__GenerateDamageEvent)(PED_TYPE *victim, ENTITY_TYPE *creator, unsigned int weaponType, int damageFactor,
                                    ePedPieceTypes pedPiece, int direction);

int CWeapon__GenerateDamageEvent_hook(PED_TYPE *victim, ENTITY_TYPE *creator, unsigned int weaponType, int damageFactor,
                                      ePedPieceTypes pedPiece, int direction) {
    if (pedPiece != PED_PIECE_HEAD) {
        // Disable ComputeDamageAnim when it's not a head!
        makeNOP(SA_ADDR(0x566B30), 2);
        makeNOP(SA_ADDR(0x566B88), 2);
    }

    int result = CWeapon__GenerateDamageEvent(victim, creator, weaponType, damageFactor, pedPiece, direction);
    if (pedPiece != PED_PIECE_HEAD) {
        // Restore bytes
        WriteMemory(SA_ADDR(0x566B30), (uintptr_t) "\xC1\xF5\xD8\xF9", 4);
        WriteMemory(SA_ADDR(0x566B88), (uintptr_t) "\xBB\xF5\xFE\xF8", 4);
    }

    return result;
}

int (*CTaskSimpleUseGun__SetPedPosition)(uintptr_t thiz, PED_TYPE *pPed);

int CTaskSimpleUseGun__SetPedPosition_hook(uintptr_t thiz, PED_TYPE *pPed) {
    PED_TYPE *pLocalPed = pGame->FindPlayerPed()->GetGtaActor();
    if (pLocalPed != (PED_TYPE *) pPed) {

        PLAYERID playerPedId = CPlayerPool::FindRemotePlayerIDFromGtaPtr((PED_TYPE *) pPed);
        if (playerPedId) {
            CPlayerPed *playerPed = CPlayerPool::GetSpawnedPlayer(playerPedId)->GetPlayerPed();
            if (playerPed) {
                if (playerPed->GetWaitAnimationGunFlash()) {
                    playerPed->SetWaitAnimationGunFlash(false);
                    ((void (*)(uintptr_t, PED_TYPE *, bool)) (SA_ADDR(0x46D4E4 + 1)))(thiz, pPed, false);
                }
            }
        }
    }

    if (pPed && pPed->WeaponSlots && pPed->byteCurWeaponSlot < 13) {
        WEAPON_SLOT_TYPE curWeaponSlot = pPed->WeaponSlots[pPed->byteCurWeaponSlot];
        if ((curWeaponSlot.dwType == 37) || (curWeaponSlot.dwType == 41) || (curWeaponSlot.dwType == 42))
            *(uint8_t *) (thiz + 13) |= 1;
    }

    return CTaskSimpleUseGun__SetPedPosition(thiz, pPed);
}









фикс прицела с снайперов

uint32_t
(*CWeapon__FireSniper)(WEAPON_SLOT_TYPE *pWeaponSlot, PED_TYPE *pFiringEntity, ENTITY_TYPE *a3, CVector *vecOrigin);

uint32_t
CWeapon__FireSniper_hook(WEAPON_SLOT_TYPE *pWeaponSlot, PED_TYPE *pFiringEntity, ENTITY_TYPE *a3, CVector *vecOrigin) {
    if (GamePool_FindPlayerPed() == pFiringEntity) {
        if (pGame) {
            CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
            if (pPlayerPed)
                pPlayerPed->FireInstant();
        }
    }

    return 1;
}







фикс крашей с хуавея, вставлять в InstallSpecialHooks

    // -- Huawei (Y7) crash fix
    if (!*(uintptr_t *)(g_libGTASA + 0x61B298))
        *(uintptr_t *)(g_libGTASA + 0x61B298) = ((uintptr_t(*)(const char *))(g_libGTASA + 0x179A20))("glAlphaFuncQCOM");

    if (!*(uintptr_t *)(g_libGTASA + 0x61B298))
        *(uintptr_t *)(g_libGTASA + 0x61B298) = ((uintptr_t(*)(const char *))(g_libGTASA + 0x179A20))("glAlphaFunc");
    // ---           

    // Читаем данные из пакета
    bsCopy.ReadBits((unsigned char *)&data.VehicleID, 16);
    bsCopy.ReadBits((unsigned char *)&data.lrAnalogLeftStick, 16);
    bsCopy.ReadBits((unsigned char *)&data.udAnalogLeftStick, 16);
    bsCopy.ReadBits((unsigned char *)&data.wKeys, 16);
    bsCopy.ReadBits((unsigned char *)&data.quatw, 32);
    bsCopy.ReadBits((unsigned char *)&data.quatx, 32);
    bsCopy.ReadBits((unsigned char *)&data.quaty, 32);
    bsCopy.ReadBits((unsigned char *)&data.quatz, 32);
    bsCopy.ReadBits((unsigned char *)&data.vecPos.x, 32);
    bsCopy.ReadBits((unsigned char *)&data.vecPos.y, 32);
    bsCopy.ReadBits((unsigned char *)&data.vecPos.z, 32);
    bsCopy.ReadBits((unsigned char *)&data.vecMoveSpeed.x, 32);
    bsCopy.ReadBits((unsigned char *)&data.vecMoveSpeed.y, 32);
    bsCopy.ReadBits((unsigned char *)&data.vecMoveSpeed.z, 32);
    bsCopy.ReadBits((unsigned char *)&data.fCarHealth, 32);
    bsCopy.ReadBits((unsigned char *)&data.playerHealth, 16);
    bsCopy.ReadBits((unsigned char *)&data.playerArmour, 16);
    bsCopy.ReadBits((unsigned char *)&data.byteCurrentWeapon, 8);
    bsCopy.ReadBits((unsigned char *)&data.byteSirenOn, 8);
    bsCopy.ReadBits((unsigned char *)&data.byteLandingGearState, 8);
    bsCopy.ReadBits((unsigned char *)&data.TrailerID, 16);

    int vehPool = *(int *)(CGameAPI::GetBase(xorstr("CNetGame::m_pVehiclePool")));
    int veh = *(int *)(vehPool + 4 * data.VehicleID);
    if (veh) {
    }
    RakNet::BitStream bsSend;
    bsSend.WriteBits(&pktVehicleSync, 8);
    ConvertBRInCarSyncToSampSync(&bsSend, data);
    return pRakClient->Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}

    if (pktId == BR_ID_PASSENGER_SYNC) {
        uint8_t pktPassengerSync = ID_PASSENGER_SYNC;
        BRPassengerSyncData data;
        bsCopy.ReadBits((unsigned char *)&data.VehicleID, 16);
        uint8_t tempByte;
        bsCopy.ReadBits((unsigned char *)&tempByte, 7);
        data.byteSeatFlags = tempByte;
        bsCopy.ReadBits((unsigned char *)&tempByte, 1);
        data.byteDriveBy = tempByte;
        bsCopy.ReadBits((unsigned char *)&data.byteCurrentWeapon, 8);
        bsCopy.ReadBits((unsigned char *)&data.playerHealth, 16);
        bsCopy.ReadBits((unsigned char *)&data.playerArmour, 16);
        bsCopy.ReadBits((unsigned char *)&data.lrAnalog, 16);
        bsCopy.ReadBits((unsigned char *)&data.udAnalog, 16);
        bsCopy.ReadBits((unsigned char *)&data.wKeys, 16);
        bsCopy.ReadBits((unsigned char *)&data.vecPos, 96);
        RakNet::BitStream bsSend;
        bsSend.WriteBits(&pktPassengerSync, 8);
        ConvertBRPassengerSyncToSampSync(&bsSend, data);
        return pRakClient->Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
    }

    return false;
}