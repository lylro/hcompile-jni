#include "netgame.h"
#include "xorstr.h"
#include "../game/jniutil.h"
#include "plugin.h"
#include "game/BRNotification.h"

#define NETGAME_VERSION 4057

extern RakClientInterface* pRakClient;

uint16_t CNetGame::m_nLastSAMPDialogID;

uint8_t GetPacketID(Packet *p)
{
    if(p == 0) return 255;
    if((uint8_t)p->data[0] == ID_TIMESTAMP) {
        return (uint8_t)p->data[sizeof(uint8_t) + sizeof(unsigned long)];
    }
    return (uint8_t)p->data[0];
}

CPlayerPool* CNetGame::GetPlayerPool()
{
    return *(CPlayerPool **)(CGameAPI::GetBase(xorstr("CNetGame::m_pPlayerPool")));
}

void CNetGame::ProcessNetwork()
{
    Packet* pkt = nullptr;
    uint8_t packetIdentifier;
    while(pkt = pRakClient->Receive())
    {
        packetIdentifier = GetPacketID(pkt);
        switch(packetIdentifier)
        {
            case ID_FAILED_INITIALIZE_ENCRIPTION:
                CChat::AddDebugMessage(xorstr("Failed to initialize encryption."));
                break;
            case ID_CONNECTION_ATTEMPT_FAILED:
                CChat::AddDebugMessage(xorstr("Сервер не отвечает. Переподключение..."));
                SetGameState(GAMESTATE_WAIT_CONNECT);
                break;
            case ID_NO_FREE_INCOMING_CONNECTIONS:
                CChat::AddDebugMessage(xorstr("Сервер полон. Переподключение..."));
                SetGameState(GAMESTATE_WAIT_CONNECT);
                pRakClient->Disconnect(0, 0);
                break;
            case ID_CONNECTION_BANNED:
                CChat::AddDebugMessage(xorstr("Вы были заблокированы на этом сервере."));
                break;
            case ID_INVALID_PASSWORD:
                CChat::AddDebugMessage(xorstr("Wrong server password."));
                pRakClient->Disconnect(0);
                break;
            case ID_AUTH_KEY:
                Packet_AuthKey(pkt);
                break;
            case ID_CONNECTION_REQUEST_ACCEPTED:
                Packet_ConnectionSucceeded(pkt);
                break;
            case ID_CONNECTION_LOST:
                CChat::AddDebugMessage(xorstr("Переподключение через 15 секунд..."));
                Packet_ConnectionLost(pkt);
                break;
            case ID_DISCONNECTION_NOTIFICATION:
                CChat::AddDebugMessage(xorstr("Переподключение через 15 секунд..."));
                pRakClient->Disconnect(2000, 0);
                Packet_ConnectionLost(pkt);
                break;
            case ID_AIM_SYNC:
                Packet_AimSync(pkt);
                break;
            case ID_PLAYER_SYNC:  
                Packet_PlayerSync(pkt);  
                break;
            case ID_VEHICLE_SYNC:  
                Packet_VehicleSync(pkt);  
                break;
            case ID_PASSENGER_SYNC:  
                Packet_PassengerSync(pkt);  
                break;
            case ID_BULLET_SYNC:  
                Packet_BulletSync(pkt);  
                break;
            case 251:
            case 252:
                Packet_CustomRPC(pkt);
                break;
        }
        pRakClient->DeallocatePacket(pkt);
    }
}

void FLog1(const char* fmt, ...)
{
    char buffer[0xFF];
    static FILE* flLog = nullptr;
    const char* pszStorage = "/storage/emulated/0/Android/data/com.bless.client/";

    if(flLog == nullptr && pszStorage != nullptr) {
        sprintf(buffer, "%s/edgar_log.txt", pszStorage);
        flLog = fopen(buffer, "a");
    }

    memset(buffer, 0, sizeof(buffer));
    va_list arg;
    va_start(arg, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, arg);
    va_end(arg);

    if(flLog == nullptr) return;
    fprintf(flLog, "%s\n", buffer);
    fflush(flLog);
}

extern CJavaWrapper* pJavaWrapper;

void CNetGame::Packet_CustomRPC(Packet *p) {
    RakNet::BitStream bs((unsigned char *) p->data, p->length, false);
    bs.IgnoreBits(8);
    uint16_t rpcID;
    bs.Read(rpcID);

    switch(rpcID) {
    case 1: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowMain(jsonString);
        FLog1("RPC 1: %s", jsonString.c_str());
        break;
    }
    case 2: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowShop(jsonString);
        FLog1("RPC 2: %s", jsonString.c_str());
        break;
    }
    case 3: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowInv(jsonString);
        FLog1("RPC 3: %s", jsonString.c_str());
        break;
    }
    case 4: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowMap(jsonString);
        FLog1("RPC 4: %s", jsonString.c_str());
        break;
    }
    case 5: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowChat(jsonString);
        FLog1("RPC 5: %s", jsonString.c_str());
        break;
    }
    case 6: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowGear(jsonString);
        FLog1("RPC 6: %s", jsonString.c_str());
        break;
    }
    case 7: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowBag(jsonString);
        FLog1("RPC 7: %s", jsonString.c_str());
        break;
    }
    case 8: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowMail(jsonString);
        FLog1("RPC 8: %s", jsonString.c_str());
        break;
    }
    case 9: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowRank(jsonString);
        FLog1("RPC 9: %s", jsonString.c_str());
        break;
    }
    case 10: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowLog(jsonString);
        FLog1("RPC 10: %s", jsonString.c_str());
        break;
    }
    case 11: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowInfo(jsonString);
        FLog1("RPC 11: %s", jsonString.c_str());
        break;
    }
    case 12: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowHelp(jsonString);
        FLog1("RPC 12: %s", jsonString.c_str());
        break;
    }
    case 13: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowMenu(jsonString);
        FLog1("RPC 13: %s", jsonString.c_str());
        break;
    }
    case 14: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowJoin(jsonString);
        FLog1("RPC 14: %s", jsonString.c_str());
        break;
    }
    case 15: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowPlay(jsonString);
        FLog1("RPC 15: %s", jsonString.c_str());
        break;
    }
    case 16: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowLoad(jsonString);
        FLog1("RPC 16: %s", jsonString.c_str());
        break;
    }
    case 17: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowSave(jsonString);
        FLog1("RPC 17: %s", jsonString.c_str());
        break;
    }
    case 18: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowOpt(jsonString);
        FLog1("RPC 18: %s", jsonString.c_str());
        break;
    }
    case 19: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowExit(jsonString);
        FLog1("RPC 19: %s", jsonString.c_str());
        break;
    }
    case 20: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowUpg(jsonString);
        FLog1("RPC 20: %s", jsonString.c_str());
        break;
    }
    case 21: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowDown(jsonString);
        FLog1("RPC 21: %s", jsonString.c_str());
        break;
    }
    case 22: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowLeft(jsonString);
        FLog1("RPC 22: %s", jsonString.c_str());
        break;
    }
    case 23: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowRight(jsonString);
        FLog1("RPC 23: %s", jsonString.c_str());
        break;
    }
    case 24: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowTop(jsonString);
        FLog1("RPC 24: %s", jsonString.c_str());
        break;
    }
    case 25: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowBot(jsonString);
        FLog1("RPC 25: %s", jsonString.c_str());
        break;
    }
    case 26: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowHud(jsonString);
        FLog1("RPC 26: %s", jsonString.c_str());
        break;
    }
    case 27: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowWin(jsonString);
        FLog1("RPC 27: %s", jsonString.c_str());
        break;
    }
    case 28: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowLose(jsonString);
        FLog1("RPC 28: %s", jsonString.c_str());
        break;
    }
    case 29: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowPvP(jsonString);
        FLog1("RPC 29: %s", jsonString.c_str());
        break;
    }
    case 30: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowPvE(jsonString);
        FLog1("RPC 30: %s", jsonString.c_str());
        break;
    }
    case 31: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowCoop(jsonString);
        FLog1("RPC 31: %s", jsonString.c_str());
        break;
    }
    case 32: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowClan(jsonString);
        FLog1("RPC 32: %s", jsonString.c_str());
        break;
    }
    case 33: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowAlly(jsonString);
        FLog1("RPC 33: %s", jsonString.c_str());
        break;
    }
    case 34: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowFoe(jsonString);
        FLog1("RPC 34: %s", jsonString.c_str());
        break;
    }
    case 35: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowBuff(jsonString);
        FLog1("RPC 35: %s", jsonString.c_str());
        break;
    }
    case 36: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowDebuff(jsonString);
        FLog1("RPC 36: %s", jsonString.c_str());
        break;
    }
    case 37: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowPet(jsonString);
        FLog1("RPC 37: %s", jsonString.c_str());
        break;
    }
    case 38: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowMount(jsonString);
        FLog1("RPC 38: %s", jsonString.c_str());
        break;
    }
    case 39: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowSkill(jsonString);
        FLog1("RPC 39: %s", jsonString.c_str());
        break;
    }
    case 40: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowTree(jsonString);
        FLog1("RPC 40: %s", jsonString.c_str());
        break;
    }
    case 41: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowQuest(jsonString);
        FLog1("RPC 41: %s", jsonString.c_str());
        break;
    }
    case 42: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowNote(jsonString);
        FLog1("RPC 42: %s", jsonString.c_str());
        break;
    }
    case 43: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowMark(jsonString);
        FLog1("RPC 43: %s", jsonString.c_str());
        break;
    }
    case 44: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowCamp(jsonString);
        FLog1("RPC 44: %s", jsonString.c_str());
        break;
    }
    case 45: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowZone(jsonString);
        FLog1("RPC 45: %s", jsonString.c_str());
        break;
    }
    case 46: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowFort(jsonString);
        FLog1("RPC 46: %s", jsonString.c_str());
        break;
    }
    case 47: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowTrap(jsonString);
        FLog1("RPC 47: %s", jsonString.c_str());
        break;
    }
    case 48: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowSafe(jsonString);
        FLog1("RPC 48: %s", jsonString.c_str());
        break;
    }
    case 49: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowWild(jsonString);
        FLog1("RPC 49: %s", jsonString.c_str());
        break;
    }
    case 50: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowSky(jsonString);
        FLog1("RPC 50: %s", jsonString.c_str());
        break;
    }
    case 51: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowDark(jsonString);
        FLog1("RPC 51: %s", jsonString.c_str());
        break;
    }
    case 52: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowLite(jsonString);
        FLog1("RPC 52: %s", jsonString.c_str());
        break;
    }
    case 53: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowRain(jsonString);
        FLog1("RPC 53: %s", jsonString.c_str());
        break;
    }
    case 54: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowSnow(jsonString);
        FLog1("RPC 54: %s", jsonString.c_str());
        break;
    }
    case 55: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowFire(jsonString);
        FLog1("RPC 55: %s", jsonString.c_str());
        break;
    }
    case 56: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowIce(jsonString);
        FLog1("RPC 56: %s", jsonString.c_str());
        break;
    }
    case 57: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowWind(jsonString);
        FLog1("RPC 57: %s", jsonString.c_str());
        break;
    }
    case 58: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowDust(jsonString);
        FLog1("RPC 58: %s", jsonString.c_str());
        break;
    }
    case 59: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowFog(jsonString);
        FLog1("RPC 59: %s", jsonString.c_str());
        break;
    }
    case 60: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowBoss(jsonString);
        FLog1("RPC 60: %s", jsonString.c_str());
        break;
    }
    case 61: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowMin(jsonString);
        FLog1("RPC 61: %s", jsonString.c_str());
        break;
    }
    case 62: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowMax(jsonString);
        FLog1("RPC 62: %s", jsonString.c_str());
        break;
    }
    case 63: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowLvl(jsonString);
        FLog1("RPC 63: %s", jsonString.c_str());
        break;
    }
    case 64: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowExp(jsonString);
        FLog1("RPC 64: %s", jsonString.c_str());
        break;
    }
    case 65: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowHP(jsonString);
        FLog1("RPC 65: %s", jsonString.c_str());
        break;
    }
    case 66: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowMP(jsonString);
        FLog1("RPC 66: %s", jsonString.c_str());
        break;
    }
    case 67: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowKey(jsonString);
        FLog1("RPC 67: %s", jsonString.c_str());
        break;
    }
    case 68: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowBox(jsonString);
        FLog1("RPC 68: %s", jsonString.c_str());
        break;
    }
    case 69: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowDoor(jsonString);
        FLog1("RPC 69: %s", jsonString.c_str());
        break;
    }
    case 70: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowRoom(jsonString);
        FLog1("RPC 70: %s", jsonString.c_str());
        break;
    }
    case 71: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowHall(jsonString);
        FLog1("RPC 71: %s", jsonString.c_str());
        break;
    }
    case 72: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowScan(jsonString);
        FLog1("RPC 72: %s", jsonString.c_str());
        break;
    }
    case 73: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowPing(jsonString);
        FLog1("RPC 73: %s", jsonString.c_str());
        break;
    }
    case 74: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowNet(jsonString);
        FLog1("RPC 74: %s", jsonString.c_str());
        break;
    }
    case 75: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowIP(jsonString);
        FLog1("RPC 75: %s", jsonString.c_str());
        break;
    }
    case 76: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowMac(jsonString);
        FLog1("RPC 76: %s", jsonString.c_str());
        break;
    }
    case 77: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowLoadout(jsonString);
        FLog1("RPC 77: %s", jsonString.c_str());
        break;
    }
    case 78: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowDrop(jsonString);
        FLog1("RPC 78: %s", jsonString.c_str());
        break;
    }
    case 79: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowPick(jsonString);
        FLog1("RPC 79: %s", jsonString.c_str());
        break;
    }
    case 80: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowUse(jsonString);
        FLog1("RPC 80: %s", jsonString.c_str());
        break;
    }
    case 81: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowCraft(jsonString);
        FLog1("RPC 81: %s", jsonString.c_str());
        break;
    }
    case 82: {
        uint32_t length;
        bs.Read(length);
        std::string jsonString(length, '\0');
        bs.Read(&jsonString[0], length);
        pJavaWrapper->ShowBuild(jsonString);
        FLog1("RPC 82: %s", jsonString.c_str());
        break;
    }
  }
}

int CNetGame::GetGameState()
{
    return *(int *)(CGameAPI::GetBase(xorstr("CNetGame::m_iGameState")));
}

void CNetGame::SetGameState(int state)
{
    *(int *)(CGameAPI::GetBase(xorstr("CNetGame::m_iGameState"))) = state;
}

void gen_auth_key(char buf[260], char* auth_in);
void CNetGame::Packet_AuthKey(Packet* pkt)
{
    RakNet::BitStream bsAuth((unsigned char *)pkt->data, pkt->length, false);
    uint8_t byteAuthLen;
    char szAuth[260];
    bsAuth.IgnoreBits(8);
    bsAuth.Read(byteAuthLen);
    bsAuth.Read(szAuth, byteAuthLen);
    szAuth[byteAuthLen] = '\0';
    char szAuthKey[260];
    gen_auth_key(szAuthKey, szAuth);
    RakNet::BitStream bsKey;
    uint8_t byteAuthKeyLen = (uint8_t)strlen(szAuthKey);
    bsKey.Write((uint8_t)ID_AUTH_KEY);
    bsKey.Write((uint8_t)byteAuthKeyLen);
    bsKey.Write(szAuthKey, byteAuthKeyLen);
    pRakClient->Send(&bsKey, SYSTEM_PRIORITY, RELIABLE, 0);
}

void CNetGame::Packet_ConnectionLost(Packet* pkt)
{
    reinterpret_cast<void(*)()>(CGameAPI::GetBase(xorstr("CNetGame::Packet_ConnectionLost")))();
}

void CNetGame::Packet_ConnectionSucceeded(Packet* pkt)
{
    RakNet::BitStream bsSuccAuth((unsigned char *)pkt->data, pkt->length, false);
    uint16_t MyPlayerID;
    unsigned int uiChallenge;
    bsSuccAuth.IgnoreBits(8);
    bsSuccAuth.IgnoreBits(32);
    bsSuccAuth.IgnoreBits(16);
    bsSuccAuth.Read(MyPlayerID);
    bsSuccAuth.Read(uiChallenge);
    GetPlayerPool()->GetLocalPlayer()->SetLocalPlayerID(MyPlayerID);
    int iVersion = NETGAME_VERSION;
    char byteMod = 0x01;
    unsigned int uiClientChallengeResponse = uiChallenge ^ iVersion;
    const char* sampVersion = xorstr("0.3.7");
    const char* auth_bs = xorstr("15121F6F18550C00AC4B4F8A167D0379BB0ACA99043");
    const char* localPlayerName = (const char *)(GetPlayerPool()->GetLocalPlayer()->GetLocalPlayerName());
    char byteAuthBSLen = (char)strlen(auth_bs);
    char byteNameLen = (char)strlen(localPlayerName);
    char byteClientverLen = (char)strlen(sampVersion);
    RakNet::BitStream bsSend;
    bsSend.Write(iVersion);
    bsSend.Write(byteMod);
    bsSend.Write(byteNameLen);
    bsSend.Write(localPlayerName, byteNameLen);
    bsSend.Write(uiClientChallengeResponse);
    bsSend.Write(byteAuthBSLen);
    bsSend.Write(auth_bs, byteAuthBSLen);
    bsSend.Write(byteClientverLen);
    bsSend.Write(sampVersion, byteClientverLen);
    pRakClient->RPC(&RPC_ClientJoin, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, NULL);
    SetGameState(GAMESTATE_AWAIT_JOIN);
}

void CNetGame::Packet_AimSync(Packet* pkt)
{
    RakNet::BitStream bsData(pkt->data, pkt->length, false);
    if(GetGameState() != GAMESTATE_CONNECTED) return;
    uint8_t pktId;
    uint16_t playerId;
    uint8_t aimSyncBuffer[31] = {0};
    bsData.ReadBits((unsigned char *)&pktId, 8);
    bsData.ReadBits((unsigned char *)&playerId, 16);
    bsData.ReadBits((unsigned char *)&aimSyncBuffer, 31 * 8);
    CRemotePlayer* remote_player = GetPlayerPool()->GetAt(playerId);
    if(remote_player) {
        remote_player->StoreAimSyncData(aimSyncBuffer, 0);
    }
}

void CNetGame::Packet_PlayerSync(Packet* pkt)
{
    RakNet::BitStream bsData(pkt->data, pkt->length, false);
    if(GetGameState() != GAMESTATE_CONNECTED) return;
    bool accepted_sync = false;
    uint8_t pktId;
    uint16_t playerId;
    uint32_t timestamp;
    int16_t lrAnalog, udAnalog;
    uint16_t wKeys;
    CVector vecPos;
    float tw, tx, ty, tz;
    uint8_t byteHealthArmour, byteHealth, byteArmour;
    CVector vecMoveSpeed;
    float mx, my, mz;
    uint16_t wSurfInfo;
    CVector vecSurfOffsets;
    uint8_t byteCurrentWeapon, byteSpecialAction;
    if(pkt->data[0] == ID_TIMESTAMP) {
        bsData.ReadBits((unsigned char *)&pktId, 8);
        bsData.ReadBits((unsigned char *)&timestamp, 32);
    }
    bsData.ReadBits((unsigned char *)&pktId, 8);
    bsData.ReadBits((unsigned char *)&playerId, 16);
    int readOffset = bsData.readOffset;
    int numberOfBitsUsed = bsData.numberOfBitsUsed;
    if(readOffset >= numberOfBitsUsed) return;
    int v6 = bsData.data[readOffset >> 3];
    char v7 = readOffset & 7;
    int v8 = readOffset + 1;
    bsData.readOffset = v8;
    if( ((v6 << v7) & 0x80) != 0 ) {
        if(!bsData.ReadBits((unsigned char *)&lrAnalog, 16)) return;
        v8 = bsData.readOffset;
        numberOfBitsUsed = bsData.numberOfBitsUsed;
    }
    if(v8 < numberOfBitsUsed) {
        int v9 = bsData.data[v8 >> 3];
        bsData.readOffset = v8 + 1;
        if( (((v9 << (v8 & 7)) & 0x80) == 0 || bsData.ReadBits((unsigned char *)&udAnalog, 16)) && bsData.ReadBits((unsigned char *)&wKeys, 16) && bsData.Read((char *)&vecPos, 12) && bsData.ReadNormQuat<float>(tw, tx, ty, tz) && bsData.ReadBits((unsigned char *)&byteHealthArmour, 8) && bsData.ReadBits((unsigned char *)&byteCurrentWeapon, 8) && bsData.ReadBits((unsigned char *)&byteSpecialAction, 8) && bsData.ReadVector<float>(mx, my, mz)) {
            readOffset = bsData.readOffset;
            if(readOffset >= bsData.numberOfBitsUsed) return;
            int v7_ = bsData.data[readOffset >> 3];
            bsData.readOffset = readOffset + 1;
            if(((v7_ << (readOffset & 7)) & 0x80) != 0) {
                if(bsData.ReadBits((unsigned char *)&wSurfInfo, 16) && bsData.ReadBits((unsigned char *)&vecSurfOffsets.x, 32) && bsData.ReadBits((unsigned char *)&vecSurfOffsets.y, 32) && bsData.ReadBits((unsigned char *)&vecSurfOffsets.z, 32)) {}
            }
            accepted_sync = true;
        }
    }
    uint8_t byteArmTemp = 0, byteHlTemp = 0;
    byteArmTemp = (byteHealthArmour & 0x0F);
    byteHlTemp = (byteHealthArmour >> 4);
    if(byteArmTemp == 0xF) byteArmour = 100;
    else if(byteArmTemp == 0) byteArmour = 0;
    else byteArmour = byteArmTemp * 7;
    if(byteHlTemp == 0xF) byteHealth = 100;
    else if(byteHlTemp == 0) byteHealth = 0;
    else byteHealth = byteHlTemp * 7;
    BROnFootSyncData ofSync = {0};
    ofSync.lrAnalogLeftStick = lrAnalog;
    ofSync.udAnalogLeftStick = udAnalog;
    ofSync.wKeys = wKeys;
    ofSync.vecPos = vecPos;
    ofSync.quatw = tw;
    ofSync.quatx = tx;
    ofSync.quaty = ty;
    ofSync.quatz = tz;
    ofSync.health = byteHealth;
    ofSync.armour = byteArmour;
    ofSync.byteCurrentWeapon = byteCurrentWeapon;
    ofSync.byteSpecialAction = byteSpecialAction;
    ofSync.vecMoveSpeed = vecMoveSpeed;
    ofSync.vecSurfOffsets = vecSurfOffsets;
    ofSync.wSurfInfo = wSurfInfo;
    CRemotePlayer* remote_player = GetPlayerPool()->GetAt(playerId);
    if(remote_player && accepted_sync) {
        remote_player->StoreSyncData(&ofSync, 0);
    }
}

void CNetGame::Packet_VehicleSync(Packet* pkt)
{
    RakNet::BitStream bsData(pkt->data, pkt->length, false);
    if(GetGameState() != GAMESTATE_CONNECTED) return;
    uint8_t pktId;
    uint16_t playerId;
    BRInCarSyncData icsync = {0};
    bsData.ReadBits((unsigned char *)&pktId, 8);
    bsData.ReadBits((unsigned char *)&playerId, 16);
    bsData.ReadBits((unsigned char *)&icsync.VehicleID, 16);
    bsData.ReadBits((unsigned char *)&icsync.lrAnalogLeftStick, 16);
    bsData.ReadBits((unsigned char *)&icsync.udAnalogLeftStick, 16);
    bsData.ReadBits((unsigned char *)&icsync.wKeys, 16);
    bsData.ReadNormQuat<float>(icsync.quatw, icsync.quatx, icsync.quaty, icsync.quatz);
    bsData.ReadBits((unsigned char *)&icsync.vecPos, 96);
    bsData.ReadVector<float>(icsync.vecMoveSpeed.x, icsync.vecMoveSpeed.y, icsync.vecMoveSpeed.z);
    uint16_t wTempCarHealth;
    bsData.ReadBits((unsigned char *)&wTempCarHealth, 16);
    icsync.fCarHealth = wTempCarHealth;
    uint8_t byteHealthArmour;
    uint8_t byteArmTemp = 0, byteHlTemp = 0;
    bsData.Read(byteHealthArmour);
    byteArmTemp = (byteHealthArmour & 0x0F);
    byteHlTemp = (byteHealthArmour >> 4);
    if(byteArmTemp == 0xF) icsync.playerArmour = 100;
    else if(byteArmTemp == 0) icsync.playerArmour = 0;
    else icsync.playerArmour = byteArmTemp * 7;
    if(byteHlTemp == 0xF) icsync.playerHealth = 100;
    else if(byteHlTemp == 0) icsync.playerHealth = 0;
    else icsync.playerHealth = byteHlTemp * 7;
    uint8_t byteTempWeapon;
    bsData.Read(byteTempWeapon);
    icsync.byteCurrentWeapon ^= (byteTempWeapon ^ icsync.byteCurrentWeapon) & 0x3F;
    bool bCheck;
    bsData.ReadCompressed(bCheck);
    if(bCheck) icsync.byteSirenOn = 1;
    bsData.ReadCompressed(bCheck);
    if(bCheck) icsync.byteLandingGearState = 1;
    bsData.ReadCompressed(bCheck);
    if(bCheck) bsData.Read(icsync.TrailerID);
    CRemotePlayer* remote_player = GetPlayerPool()->GetAt(playerId);
    if(remote_player) {
        remote_player->StoreInCarSyncData(&icsync, 0);
    }
}

void CNetGame::Packet_PassengerSync(Packet* pkt)
{
    RakNet::BitStream bsData(pkt->data, pkt->length, false);
    if(GetGameState() != GAMESTATE_CONNECTED) return;
    uint8_t pktId;
    uint16_t playerId;
    uint8_t passengerSync[26] = {0};
    bsData.ReadBits((unsigned char *)&pktId, 8);
    bsData.ReadBits((unsigned char *)&playerId, 16);
    bsData.ReadBits((unsigned char *)&passengerSync, 26 * 8);
    CRemotePlayer* remote_player = GetPlayerPool()->GetAt(playerId);
    if(remote_player) {
        remote_player->StorePassengerSyncData(passengerSync, 0);
    }
}

void CNetGame::Packet_BulletSync(Packet* pkt)
{
    RakNet::BitStream bsData(pkt->data, pkt->length, false);
    if(GetGameState() != GAMESTATE_CONNECTED) return;
    uint8_t pktId;
    uint16_t playerId;
    uint8_t bulletSync[40] = {0};
    bsData.ReadBits((unsigned char *)&pktId, 8);
    bsData.ReadBits((unsigned char *)&playerId, 16);
    bsData.ReadBits((unsigned char *)&bulletSync, 40 * 8);
    CRemotePlayer* remote_player = GetPlayerPool()->GetAt(playerId);
    if(remote_player) {
        CLocalPlayer* local_player = GetPlayerPool()->GetLocalPlayer();
        if(local_player->GetLocalPlayerID() != playerId) {
            remote_player->StoreBulletSyncData(bulletSync, 0);
        }
    }
}

/*void CNetGame::Packet_Turnlights(Packet* pkt)
{
    RakNet::BitStream bs(pkt->data, pkt->length, false);
    
    uint8_t packetId;
    uint16_t vehicleId;
    uint8_t lightsState;
    
    bs.ReadBits((unsigned char*)&packetId, 8);
    bs.ReadBits((unsigned char*)&vehicleId, 16);
    bs.ReadBits((unsigned char*)&lightsState, 8);
    
    int vehPool = *(int*)(CGameAPI::GetBase(xorstr("CNetGame::m_pVehiclePool")));
    if (!vehPool) return;
    
    int pVehicle = *(int*)(vehPool + 4 * vehicleId);
    if (!pVehicle) return;
    
    *(uint8_t*)(pVehicle + 0x1C0) = lightsState;
    
    int localPlayerVeh = *(int*)(CGameAPI::GetBase(xorstr("FindPlayerVehicle")));
    if (localPlayerVeh == pVehicle && lightsState)
    {
        uint16_t soundId = (lightsState == 3) ? 0x14 : 0x13;
    }
}*/