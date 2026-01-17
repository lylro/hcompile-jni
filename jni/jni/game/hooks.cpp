#include "hooks.h"
#include "xorstr.h"
#include "plugin/netrpc.h"

extern bool g_bInitGameProcess;

RakClientInterface* pRakClient = NULL;

void (*orig_CNetGame__ProcessNetwork)();
void hook_CNetGame__ProcessNetwork()
{
    if (!pRakClient)
        pRakClient = RakNetworkFactory::GetRakClientInterface();

    CNetGame::ProcessNetwork();
}

bool (*orig_RakClient__Connect)(uintptr_t, const char*, uint16_t, uint16_t, unsigned int, int);
bool hook_RakClient__Connect(uintptr_t, const char*, uint16_t, uint16_t, unsigned int, int)
{
    if (!pRakClient)
        pRakClient = RakNetworkFactory::GetRakClientInterface();

    if (!pRakClient)
        return false;

    return pRakClient->Connect(xorstr("141.94.184.107"), 1141, 0, 0, 5);
}

bool (*orig_RakClient__RPC)(uintptr_t, BRRpcIds, RakNet::BitStream*, PacketPriority, BRPacketReliability, char, bool, NetworkID, RakNet::BitStream*);
bool hook_RakClient__RPC(uintptr_t, BRRpcIds uniqueID, RakNet::BitStream* bitStream,
    PacketPriority priority, BRPacketReliability reliability,
    char orderingChannel, bool shiftTimestamp,
    NetworkID networkID, RakNet::BitStream* replyFromTarget)
{
    if (!pRakClient || !bitStream)
        return false;

    int sampRpcId = ConvertBRIDToSampID(uniqueID);

    if (uniqueID == 394)
        sampRpcId = 97;

    if (sampRpcId == 97)
    {
        int offset = bitStream->GetReadOffset();
        int actionType = -1;
        unsigned char actionSubtype = 0;

        if (bitStream->GetNumberOfUnreadBits() >= 40)
        {
            bitStream->ReadBits((unsigned char*)&actionType, 32, true);
            bitStream->ReadBits(&actionSubtype, 8, true);
        }

        bitStream->SetReadOffset(offset);
    }

    if (sampRpcId == RPC_RequestClass && g_bInitGameProcess)
        return false;

    if (sampRpcId == -1)
        return false;

    return pRakClient->RPC(
        &sampRpcId,
        bitStream,
        priority,
        ConvertBRToSampReliability(reliability),
        orderingChannel,
        shiftTimestamp,
        networkID,
        replyFromTarget
    );
}

bool (*orig_RakClient__Send)(uintptr_t, RakNet::BitStream*, PacketPriority, BRPacketReliability, char);
bool hook_RakClient__Send(uintptr_t, RakNet::BitStream* bitStream,
    PacketPriority priority, BRPacketReliability reliability, char orderingChannel)
{
    if (!pRakClient || !bitStream)
        return false;

    if (bitStream->GetNumberOfBytesUsed() < 1)
        return false;

    RakNet::BitStream bsCopy(bitStream->GetData(), bitStream->GetNumberOfBytesUsed(), false);

    uint8_t pktId;
    if (!bsCopy.Read(pktId))
        return false;

    if (pktId == BR_ID_USER_INTERFACE_SYNC)
    {
        uint16_t guiId;
        uint32_t jsonLen;

        if (!bsCopy.Read(guiId) || !bsCopy.Read(jsonLen))
            return false;

        if (jsonLen == 0 || jsonLen > 2048)
            return false;

        if (bsCopy.GetNumberOfUnreadBytes() < jsonLen)
            return false;

        char buff[2049];
        memset(buff, 0, sizeof(buff));
        bsCopy.Read(buff, jsonLen);

        static char buffGUI[4096];
        cp1251_to_utf8(buffGUI, buff);

        if (guiId == 10 && nlohmann::json::accept(buffGUI))
        {
            nlohmann::json jsonObj = nlohmann::json::parse(buffGUI);

            uint8_t btn = jsonObj["r"];
            int16_t listInput = jsonObj["l"];
            std::string input = jsonObj["i"];

            uint8_t inputLen = input.length();
            if (inputLen > 255) inputLen = 255;

            RakNet::BitStream bsSend;
            bsSend.WriteBits((unsigned char*)&CNetGame::m_nLastSAMPDialogID, 16);
            bsSend.WriteBits(&btn, 8);
            bsSend.WriteBits((unsigned char*)&listInput, 16);
            bsSend.WriteBits(&inputLen, 8);
            bsSend.Write(input.c_str(), inputLen);

            return pRakClient->RPC(
                &RPC_DialogResponse,
                &bsSend,
                HIGH_PRIORITY,
                RELIABLE_ORDERED,
                0,
                false,
                UNASSIGNED_NETWORK_ID,
                NULL
            );
        }
        else
        {
            CNetGame::SendOnData(guiId, buffGUI, jsonLen);
        }
    }

    return pRakClient->Send(bitStream, priority, reliability, orderingChannel);
}