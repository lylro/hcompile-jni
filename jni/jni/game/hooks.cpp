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
    return pRakClient->Connect("185.189.255.97", 2852, 0, 0, 5);
}

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
                } else {
                }
            }
        }
        else if (guiId == 22) {
                if (nlohmann::json::accept(buffGUI)) {
                    nlohmann::json jsonObj = nlohmann::json::parse(buffGUI);
                    int8_t type = jsonObj.value<int8_t>("t", -1); // Поддержка t = -1 из onPacketIncoming
                    nlohmann::json outputJson;
                    outputJson["t"] = type;

                    FLog("Received GUI 22: Type=%d", type);

                    // Обработка всех типов действий
                    switch (type) {
                        case -1: // BlackPass-related actions
                        {
                            int ty = jsonObj.value<int>("ty", -1);
                            outputJson["ty"] = ty;
                            if (ty == -1) {
                                int sp = jsonObj.value<int>("sp", 0);
                                int a = jsonObj.value<int>("a", 0);
                                outputJson["sp"] = sp;
                                outputJson["a"] = a;
                                FLog("Received GUI 22: Type=-1, SubType=%d, ShowPremium=%d, Additional=%d", ty, sp, a);
                            } else if (ty == 0 || ty == 1) {
                                int la = jsonObj.value<int>("la", 0);
                                outputJson["la"] = la;
                                if (ty == 1 && (la == 0 || la == 1)) {
                                    int s = jsonObj.value<int>("s", 0);
                                    outputJson["s"] = s;
                                    FLog("Received GUI 22: Type=-1, SubType=%d, Layout=%d, Status=%d", ty, la, s);
                                } else if (ty == 1 && la == 3) {
                                    int p = jsonObj.value<int>("p", 0);
                                    outputJson["p"] = p;
                                    FLog("Received GUI 22: Type=-1, SubType=%d, Layout=%d, Position=%d", ty, la, p);
                                } else if (ty == 1 && la == 4) {
                                    int s = jsonObj.value<int>("s", 0);
                                    outputJson["s"] = s;
                                    FLog("Received GUI 22: Type=-1, SubType=%d, Layout=%d, Status=%d", ty, la, s);
                                } else if (ty == 2) {
                                    int la = jsonObj.value<int>("la", 0);
                                    int s = jsonObj.value<int>("s", 0);
                                    outputJson["la"] = la;
                                    outputJson["s"] = s;
                                    FLog("Received GUI 22: Type=-1, SubType=%d, Layout=%d, Status=%d", ty, la, s);
                                } else if (ty == 3) {
                                    int tb = jsonObj.value<int>("tb", -2);
                                    if (tb == -2) {
                                        int e = jsonObj.value<int>("e", 0);
                                        int l = jsonObj.value<int>("l", 0);
                                        outputJson["e"] = e;
                                        outputJson["l"] = l;
                                        FLog("Received GUI 22: Type=-1, SubType=%d, Exp=%d, Level=%d", ty, e, l);
                                    }
                                }
                            }
                            break;
                        }
                        case 0: // buySkinsOrMoneyOrAccessoriesOrServices or buyCar
                        {
                            int itemPrice = jsonObj.value<int>("m", 0);
                            int itemId = jsonObj.value<int>("p", -1);
                            int y = jsonObj.value<int>("y", 0);
                            outputJson["m"] = itemPrice;
                            outputJson["p"] = itemId;
                            outputJson["y"] = y;
                            if (jsonObj.contains("c")) { // buySkinsOrMoneyOrAccessoriesOrServices
                                int itemCount = jsonObj.value<int>("c", -1);
                                outputJson["c"] = itemCount;
                                FLog("Received GUI 22: Type=0, ItemPrice=%d, ItemId=%d, ItemCount=%d, Status=%d", itemPrice, itemId, itemCount, y);
                            } else if (jsonObj.contains("l")) { // buyCar
                                int carColor = jsonObj.value<int>("l", 0);
                                outputJson["l"] = carColor;
                                FLog("Received GUI 22: Type=0, ItemPrice=%d, ItemId=%d, CarColor=%d, Status=%d", itemPrice, itemId, carColor, y);
                            }
                            if (y == 0 && jsonObj.contains("k")) {
                                int k = jsonObj.value<int>("k", 0);
                                std::string m = jsonObj.value<std::string>("m", "");
                                std::string n = jsonObj.value<std::string>("n", "");
                                outputJson["k"] = k;
                                outputJson["m"] = m;
                                outputJson["n"] = n;
                                FLog("Received GUI 22: Type=0, Coins=%d, Message='%s', ExtraMessage='%s'", k, m.c_str(), n.c_str());
                            }
                            break;
                        }
                        case 1: // openHeaderBoxes
                        {
                            FLog("Received GUI 22: Type=1 (openHeaderBoxes)");
                            break;
                        }
                        case 2: // openingSurprise
                        {
                            FLog("Received GUI 22: Type=2 (openingSurprise)");
                            break;
                        }
                        case 3: // closeDialog
                        {
                            int s = jsonObj.value<int>("s", 0);
                            int r = jsonObj.value<int>("r", 0);
                            outputJson["s"] = s;
                            outputJson["r"] = r;
                            FLog("Received GUI 22: Type=3, Coins=%d, Rubles=%d", s, r);
                            break;
                        }
                        case 4: // sendCustomValueOfMoney
                        {
                            int valueOfMoney = jsonObj.value<int>("v", 0);
                            outputJson["v"] = valueOfMoney;
                            FLog("Received GUI 22: Type=4, ValueOfMoney=%d", valueOfMoney);
                            break;
                        }
                        case 5: // sendBuyTokens
                        {
                            int value = jsonObj.value<int>("s", 0);
                            outputJson["s"] = value;
                            FLog("Received GUI 22: Type=5, Value=%d", value);
                            break;
                        }
                        case 6: // sendCloseDialogBuyBC
                        {
                            int id = jsonObj.value<int>("id", 0);
                            outputJson["id"] = id;
                            FLog("Received GUI 22: Type=6, Id=%d", id);
                            break;
                        }
                        case 7: // updateValueOfLimit
                        {
                            if (jsonObj.contains("b")) {
                                outputJson["b"] = jsonObj["b"];
                                FLog("Received GUI 22: Type=7, LimitsArray=%s", jsonObj["b"].dump().c_str());
                            }
                            break;
                        }
                        case 8: // updateNewSaleItems
                        {
                            FLog("Received GUI 22: Type=8 (updateNewSaleItems)");
                            break;
                        }
                        case 9: // showError
                        {
                            std::string m = jsonObj.value<std::string>("m", "");
                            std::string n = jsonObj.value<std::string>("n", "");
                            outputJson["m"] = m;
                            outputJson["n"] = n;
                            FLog("Received GUI 22: Type=9, Message='%s', ExtraMessage='%s'", m.c_str(), n.c_str());
                            break;
                        }
                        default:
                            FLog("Unknown type %d for GUI 22", type);
                            return false;
                    }

                    // Общие параметры из getDataFromJson
                    if (jsonObj.contains("r")) {
                        int r = jsonObj.value<int>("r", 0);
                        outputJson["r"] = r;
                        FLog("Received GUI 22: Rubles=%d", r);
                    }
                    if (jsonObj.contains("d")) {
                        int d = jsonObj.value<int>("d", 0);
                        outputJson["d"] = d;
                        FLog("Received GUI 22: Coins=%d", d);
                    }
                    if (jsonObj.contains("em")) {
                        std::string em = jsonObj.value<std::string>("em", "");
                        outputJson["em"] = em;
                        FLog("Received GUI 22: Email='%s'", em.c_str());
                    }
                    if (jsonObj.contains("nm")) {
                        std::string nm = jsonObj.value<std::string>("nm", "");
                        outputJson["nm"] = nm;
                        FLog("Received GUI 22: Nickname='%s'", nm.c_str());
                    }
                    if (jsonObj.contains("ds")) {
                        int ds = jsonObj.value<int>("ds", 0);
                        outputJson["ds"] = ds;
                        FLog("Received GUI 22: DoubleDonate=%d", ds);
                    }
                    if (jsonObj.contains("lc")) {
                        int lc = jsonObj.value<int>("lc", 0);
                        outputJson["lc"] = lc;
                        FLog("Received GUI 22: LayoutCode=%d", lc);
                    }
                    if (jsonObj.contains("k")) {
                        int k = jsonObj.value<int>("k", 0);
                        outputJson["k"] = k;
                        FLog("Received GUI 22: CurrentPage=%d", k);
                    }

                    std::string jsonString = outputJson.dump();
                    FLog("dump json: %s", jsonString.c_str());
                    size_t jsonSize = jsonString.size();

                    RakNet::BitStream bsSend;
                    bsSend.Write((uint8_t)251);          // Magic number
                    bsSend.Write((uint8_t)22);          // GUI ID
                    bsSend.Write((uint16_t)jsonSize);    // Длина JSON
                    bsSend.Write(jsonString.c_str(), jsonSize);

                    bool result = pRakClient->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
                    if (!result) {
                        FLog("Failed to send GUI 22 data!");
                    }
                    FLog("Data sent successfully! Payload: '%s' (size: %zu bytes)", jsonString.c_str(), jsonSize);
                }
            }
        else if (guiId == 38) {
                if (nlohmann::json::accept(buffGUI)) {
                    nlohmann::json jsonObj = nlohmann::json::parse(buffGUI);
                    uint8_t type = jsonObj.value<uint8_t>("t", 0);
                    std::string email = jsonObj.value<std::string>("s", "");
                    std::string pass = jsonObj.value<std::string>("p", "");

                    FLog("Received: Type=%d, Email='%s', Password='%s'", type, email.c_str(), pass.c_str());

                    nlohmann::json outputJson;
                    outputJson["p"] = pass;
                    outputJson["s"] = email;
                    outputJson["t"] = type;

                    if (type == 0) {
                        return false;
                    }

                    std::string jsonString = outputJson.dump();
                    FLog("dump json: %s", jsonString.c_str());
                    size_t jsonSize = jsonString.size();

                    RakNet::BitStream bsSend;
                    bsSend.Write((uint8_t)251);          // Magic number
                    bsSend.Write((uint8_t)38);          // GUI ID
                    bsSend.Write((uint16_t)jsonSize);    // Длина JSON
                    bsSend.Write(jsonString.c_str(), jsonSize);

                    bool result = pRakClient->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
                    if (!result) {
                        FLog("Failed to send data!");
                    }
                    FLog("Data sent successfully! Payload: '%s' (size: %zu bytes)", jsonString.c_str(), jsonSize);
                }
            }
   else if (guiId == 73) {
    if (jsonLen >= 4096) {
        FLog("JSON length too large for GUI 73: %d", jsonLen);
        return false;
    }
    if (nlohmann::json::accept(buffGUI)) {
        try {
            nlohmann::json jsonObj = nlohmann::json::parse(buffGUI);
            int8_t type = jsonObj.value<int8_t>("t", -1); // Тип действия
            nlohmann::json outputJson;
            outputJson["t"] = type;

            FLog("Received GUI 73: Type=%d", type);

            // Обработка всех типов действий из CasesActionWithJSON
            switch (type) {
                case 1: // selectCase
                {
                    int caseId = jsonObj.value<int>("cs", 0); // CasesKeys.SELECTED_CASE_ID_KEY
                    outputJson["cs"] = caseId;
                    FLog("Received GUI 73: Type=1, CaseId=%d", caseId);
                    break;
                }
                case 2: // openCases
                {
                    int caseId = jsonObj.value<int>("cs", 0); // CasesKeys.SELECTED_CASE_ID_KEY
                    int buttonType = jsonObj.value<int>("type", 0); // CasesKeys.TYPE_OF_BUTTON_KEY
                    outputJson["cs"] = caseId;
                    outputJson["type"] = buttonType;
                    FLog("Received GUI 73: Type=2, CaseId=%d, ButtonType=%d", caseId, buttonType);
                    break;
                }
                case 3: // takeRewards
                {
                    if (jsonObj.contains("bt1")) { // CasesKeys.GET_REWARDS_KEY
                        outputJson["bt1"] = jsonObj["bt1"];
                        FLog("Received GUI 73: Type=3, GetRewards=%s", jsonObj["bt1"].dump().c_str());
                    }
                    if (jsonObj.contains("bt2")) { // CasesKeys.SPRAY_REWARDS_KEY
                        outputJson["bt2"] = jsonObj["bt2"];
                        FLog("Received GUI 73: Type=3, SprayRewards=%s", jsonObj["bt2"].dump().c_str());
                    }
                    break;
                }
                case 4: // backToDonate или bcButtonClick
                {
                    int donateType = jsonObj.value<int>("d", 0); // CasesKeys.DONATE_TYPE_KEY
                    outputJson["d"] = donateType;
                    FLog("Received GUI 73: Type=4, DonateType=%d", donateType);
                    break;
                }
                case 5: // openSuperCase
                {
                    FLog("Received GUI 73: Type=5 (openSuperCase)");
                    break;
                }
                case 6: // openBpRewards
                {
                    FLog("Received GUI 73: Type=6 (openBpRewards)");
                    break;
                }
                case 7: // getBonus
                {
                    int bonusId = jsonObj.value<int>("b", 0); // CasesKeys.BONUS_ID_KEY
                    outputJson["b"] = bonusId;
                    FLog("Received GUI 73: Type=7, BonusId=%d", bonusId);
                    break;
                }
                case 8: // openCasesFromBanner
                {
                    FLog("Received GUI 73: Type=8 (openCasesFromBanner)");
                    break;
                }
                default:
                    FLog("Unknown type %d for GUI 73", type);
                    return false;
            }

            // Общие параметры из CasesViewModel и CasesKeys
            if (jsonObj.contains("cc")) { // CasesKeys.ARRAY_CASES_KEY
                outputJson["cc"] = jsonObj["cc"];
                FLog("Received GUI 73: CasesArray=%s", jsonObj["cc"].dump().c_str());
            }
            if (jsonObj.contains("cb")) { // CasesKeys.ARRAY_BONUS_REWARDS_KEY
                outputJson["cb"] = jsonObj["cb"];
                FLog("Received GUI 73: BonusRewards=%s", jsonObj["cb"].dump().c_str());
            }
            if (jsonObj.contains("s")) { // CasesKeys.IS_NEED_ADD_COUNT_CASE_KEY
                int isNeedAddCount = jsonObj.value<int>("s", 0);
                outputJson["s"] = isNeedAddCount;
                FLog("Received GUI 73: IsNeedAddCount=%d", isNeedAddCount);
            }
            if (jsonObj.contains("i")) { // CasesKeys.IS_SHOWING_TUTORIAL_KEY
                int isShowingTutorial = jsonObj.value<int>("i", 0);
                outputJson["i"] = isShowingTutorial;
                FLog("Received GUI 73: IsShowingTutorial=%d", isShowingTutorial);
            }
            if (jsonObj.contains("d")) { // CasesKeys.OPEN_CASE_NEGATIVE_ADDITIONALLY_KEY
                int negativeAdditionally = jsonObj.value<int>("d", 0);
                outputJson["d"] = negativeAdditionally;
                FLog("Received GUI 73: NegativeAdditionally=%d", negativeAdditionally);
            }
            if (jsonObj.contains("bc")) { // CasesKeys.VALUE_OF_BC_KEY
                int bcValue = jsonObj.value<int>("bc", 0);
                outputJson["bc"] = bcValue;
                FLog("Received GUI 73: BCValue=%d", bcValue);
            }
            if (jsonObj.contains("pc")) { // CasesKeys.VALUE_OF_CURRENT_DUST_KEY
                int dustValue = jsonObj.value<int>("pc", 0);
                outputJson["pc"] = dustValue;
                FLog("Received GUI 73: DustValue=%d", dustValue);
            }
            if (jsonObj.contains("bcc")) { // CasesKeys.VALUE_OF_OPENED_CASES_KEY
                int openedCases = jsonObj.value<int>("bcc", 0);
                outputJson["bcc"] = openedCases;
                FLog("Received GUI 73: OpenedCases=%d", openedCases);
            }
            if (jsonObj.contains("pr")) { // CasesKeys.ID_REWARDS_ARRAY_KEY
                outputJson["pr"] = jsonObj["pr"];
                FLog("Received GUI 73: RewardsArray=%s", jsonObj["pr"].dump().c_str());
            }

            // Параметры для closeBanner и isClosedWithError
            if (jsonObj.contains("c")) {
                int closeFlag = jsonObj.value<int>("c", 0);
                outputJson["c"] = closeFlag;
                FLog("Received GUI 73: CloseFlag=%d", closeFlag);
                if (jsonObj.contains("b")) { // CasesKeys.BONUS_ID_KEY for closeBanner
                    int bannerId = jsonObj.value<int>("b", 0);
                    outputJson["b"] = bannerId;
                    FLog("Received GUI 73: BannerId=%d", bannerId);
                }
            }

            std::string jsonString = outputJson.dump();
            size_t jsonSize = jsonString.size();
            FLog("Sending GUI 73 JSON: %s", jsonString.c_str());

            RakNet::BitStream bsSend;
            bsSend.Write((uint8_t)251); // Magic number
            bsSend.Write((uint8_t)73);  // GUI ID
            bsSend.Write((uint16_t)jsonSize); // Длина JSON
            bsSend.Write(jsonString.c_str(), jsonSize);

            bool result = pRakClient->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
            if (!result) {
                FLog("Failed to send GUI 73 data!");
            } else {
                FLog("GUI 73 data sent successfully. Payload: '%s' (size: %zu bytes)", jsonString.c_str(), jsonSize);
            }
            return result;
        } catch (const std::exception& e) {
            FLog("JSON parse error for GUI 73: %s", e.what());
            return false;
        }
    } else {
        FLog("Invalid JSON format for GUI 73: %s", buffGUI);
        return false;
    }
   }    else {
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