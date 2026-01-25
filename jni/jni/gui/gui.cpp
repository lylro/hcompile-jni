#include "../main.h"
#include "gui.h"
#include "../game/game.h"
#include "../net/netgame.h"
#include "../game/RW/RenderWare.h"
#include "../chatwindow.h"
#include "../playertags.h"
#include "../dialog.h"
#include "../keyboard.h"
#include "../settings.h"
#include "..//scoreboard.h"
#include "../util/CJavaWrapper.h"
#include "../util/util.h"
#include "../game/vehicle.h"
#include <sstream>
#include "../game/snapshothelper.h"
#include "../vendor/imgui/imgui_internal.h"
extern CScoreBoard* pScoreBoard;
extern CChatWindow *pChatWindow;
extern CPlayerTags *pPlayerTags;
extern CDialogWindow *pDialogWindow;
extern CSettings *pSettings;
extern CKeyBoard *pKeyBoard;
extern CNetGame *pNetGame;
extern CSnapShotHelper* pSnapShotHelper;
extern CJavaWrapper *g_pJavaWrapper;
void ImGui_ImplRenderWare_RenderDrawData(ImDrawData* draw_data);
void ImGui_ImplRenderWare_NewFrame();
void ImGui_ImplRenderWare_ShutDown();
bool ImGui_ImplRenderWare_Init();
RwTexture *CGUI::rwTexture = nullptr;
#define MULT_X	0.00052083333f	// *1920
#define MULT_Y	0.00092592592f 	// *1080
RwTexture* logo = nullptr;
RwTexture* sprint = nullptr;
RwTexture* hud_up = nullptr;
RwTexture* hud_down = nullptr;
RwTexture* hud_center = nullptr;
RwTexture* hud_menu = nullptr;
RwTexture* hud_donate = nullptr;
RwTexture* hud_back = nullptr;
RwTexture* hud_heart = nullptr;
RwTexture* hud_armor = nullptr;
RwTexture* hud_heart_progress = nullptr;
RwTexture* hud_armor_progress = nullptr;
RwTexture* hud_scale = nullptr;
RwTexture* hud_ruble = nullptr;
RwTexture* speedo_center = nullptr;
RwTexture* speedo_scale = nullptr;
RwTexture* speedo_health = nullptr;
RwTexture* speedo_ablinker = nullptr;
RwTexture* WidgetGetIn = nullptr;

RwTexture* speedo_arrow = nullptr;
RwTexture* speedo_arrowa = nullptr;
RwTexture* speedo_blinker = nullptr;
RwTexture* speedo_blink = nullptr;
RwTexture* speedo_km = nullptr;
RwTexture* speedo_doors = nullptr;
RwTexture* speedo_efuel = nullptr;
RwTexture* speedo_engine = nullptr;
RwTexture* speedo_fuel = nullptr;
RwTexture* speedo_h = nullptr;
RwTexture* speedo_l = nullptr;
RwTexture* speedo_right = nullptr;
RwTexture* speedo_scale2 = nullptr;
RwTexture* hud_fist[46];
ImFont* fontorig = nullptr;
ImFont* fontmoney = nullptr;
char path2[0xFF];char path[0xFF];
RwTexture* digits = nullptr;
float screenHeight;
float screenWidth;
ImFont* font = nullptr;
#include "..//Radar.h"
float m_fFontSizeSpeed = 30.0f;
int digitspeed[3];
bool bLeftLEDOn = false;
bool bRightLEDOn = false;
bool bCenterLEDOn = false;

bool bLeftButtonPressed = false;
bool bRightButtonPressed = false;
bool bCenterButtonPressed = false;
bool speedometershow = false;
ImVec2 posCurClick[10];
ImVec2 posSizeClick[10];
ImVec2 GetCoord(float X, float Y, bool j = false) {
  float userInputX;
  float userInputY;

  userInputX = X * 1.33f;
  userInputY = Y;

  float scaledX;
  float scaledY;
  
  
	if(!j){
  if (ImGui::GetIO().DisplaySize.x / ImGui::GetIO().DisplaySize.y <= 1.8f) {
    scaledX = (userInputX * 1366 / 2560);
    scaledY = (userInputY * 768 / 1080);
  }
  else {
    scaledX = userInputX * 1366 / 3440;
    scaledY = userInputY * 768 / 1500;
	}}
	else{
		scaledX = (userInputX * 1366 / 2560);
		scaledY = (userInputY * 768 / 1080);
		
	}
float t = 1.0f;
	if(ImGui::GetIO().DisplaySize.x >= 2300 && ImGui::GetIO().DisplaySize.y >= 1000){
		t = 1.5f;
		
	}
	//Log("%f %f", ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);

  ImVec2 out = ImVec2(scaledX*1.44f*t, scaledY*1.44f*t);
  return out;
}
std::string moneyFormat(long long number) {
    std::stringstream ss;
    ss.imbue(std::locale(""));
    ss << number;
    std::string formatted = ss.str();
    for (int i = formatted.length() - 3; i > 0; i -= 3) {
        formatted.insert(i, ".");
    }

    return formatted;
}
CGUI::CGUI()
{
	Log("BRGUI LOAD..");

	m_bMouseDown = 0;
	m_vTouchPos = ImVec2(-1, -1);
	m_bNextClear = false;
	m_bNeedClearMousePos = false;
	

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
    
     screenWidth = io.DisplaySize.x;
     screenHeight = io.DisplaySize.y;
	
	

	ImGui_ImplRenderWare_Init();

	m_vecScale.x = io.DisplaySize.x * MULT_X;
	m_vecScale.y = io.DisplaySize.y * MULT_Y;
	
	m_fFontSize = ScaleY( pSettings->GetReadOnly().fFontSize );
	m_fFontSizeSpeed = ScaleY( 25 );

	Log("GUI | Scale factor: %f, %f Font size: %f", m_vecScale.x, m_vecScale.y, m_fFontSize);
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScrollbarSize = ScaleY(55.0f);
	style.WindowBorderSize = 0.0f;
	ImGui::StyleColorsDark();

    int last_server = pSettings->GetReadOnly().last_server;
    if(last_server == 0) logo = (RwTexture*)LoadTextureFromDB("mobile", "logobrmoscow");
    if(last_server == 1) logo = (RwTexture*)LoadTextureFromDB("mobile", "logobrgreen"); // change to test
	fontorig = ImGui::GetFont();
	
	
	sprint = (RwTexture*)LoadTextureFromDB("mobile", "sprint");
	hud_up = (RwTexture*)LoadTextureFromDB("mobile", "hud_up");
	hud_down = (RwTexture*)LoadTextureFromDB("mobile", "hud_down");
	VECTOR rot;
	rot.X = 0.0f;
	rot.Y = 0.0f;
	rot.Z = 0.0f;
	hud_center = (RwTexture*)LoadTextureFromDB("mobile", "hud_center");
	hud_menu = (RwTexture*)LoadTextureFromDB("mobile", "hud_menu");
	hud_donate = (RwTexture*)LoadTextureFromDB("mobile", "hud_donat_store");
	hud_back = (RwTexture*)LoadTextureFromDB("mobile", "hud_back");
	hud_heart = (RwTexture*)LoadTextureFromDB("mobile", "hud_heart");
	hud_armor = (RwTexture*)LoadTextureFromDB("mobile", "hud_armor");
	hud_heart_progress = (RwTexture*)LoadTextureFromDB("mobile", "hud_health_scale");
	hud_armor_progress = (RwTexture*)LoadTextureFromDB("mobile", "hud_armor_scale");
	hud_scale = (RwTexture*)LoadTextureFromDB("mobile", "hud_scale");
	hud_ruble = (RwTexture*)LoadTextureFromDB("mobile", "hud_ruble");
	
	
	speedo_center = (RwTexture*)LoadTextureFromDB("mobile", "speedo_center");
	speedo_scale = (RwTexture*)LoadTextureFromDB("mobile", "speedo_scale");
	speedo_health = (RwTexture*)LoadTextureFromDB("mobile", "speedo_health");
	speedo_ablinker = (RwTexture*)LoadTextureFromDB("mobile", "speedo_ablinker");
	speedo_arrow = (RwTexture*)LoadTextureFromDB("mobile", "speedo_arrow");
	speedo_arrowa = (RwTexture*)LoadTextureFromDB("mobile", "speedo_arrowa");
	speedo_blinker = (RwTexture*)LoadTextureFromDB("mobile", "speedo_blinker");
	speedo_blink = (RwTexture*)LoadTextureFromDB("mobile", "speedo_blink");
	speedo_km = (RwTexture*)LoadTextureFromDB("mobile", "speedo_km");
	speedo_doors = (RwTexture*)LoadTextureFromDB("mobile", "speedo_doors");
	speedo_efuel = (RwTexture*)LoadTextureFromDB("mobile", "speedo_efuel");
	speedo_engine = (RwTexture*)LoadTextureFromDB("mobile", "speedo_engine");
	speedo_fuel = (RwTexture*)LoadTextureFromDB("mobile", "speedo_fuel");
	speedo_h = (RwTexture*)LoadTextureFromDB("mobile", "speedo_h");
	speedo_l = (RwTexture*)LoadTextureFromDB("mobile", "speedo_l");
	speedo_right = (RwTexture*)LoadTextureFromDB("mobile", "speedo_right");
	speedo_scale2 = (RwTexture*)LoadTextureFromDB("mobile", "speedo_scale2");
	
	
	WidgetGetIn = (RwTexture*)LoadTextureFromDB("mobile", "WidgetGetIn");

	char t[100];
	for(int i = 0; i < 47; i++){
		sprintf(t, "weapon_%d", i);
		hud_fist[i] = (RwTexture*)LoadTextureFromDB("mobile", t);
	}
	
	
    sprintf(path2, "%sSAMP/plates/ru_fonts.ttf", g_pszStorage);
	
	//ImFontAtlas* fontAtlas = 
	// Add default Japanese ranges
      
	
	 digits = (RwTexture*)LoadTextureFromDB("mobile", "speedo_digits");


    
	

	
	
	sprintf(path, "%sSAMP/fonts/%s", g_pszStorage, pSettings->GetReadOnly().szFont);
	static const ImWchar ranges[] = 
	{
		0x0020, 0x0080,
		0x00A0, 0x00C0,
		0x0400, 0x0460,
		0x0490, 0x04A0,
		0x2010, 0x2040,
		0x20A0, 0x20B0,
		0x2110, 0x2130,
		0
	};
	//Log("GUI | Loading font: %s", pSettings->GetReadOnly().szFont);
	m_pFont = io.Fonts->AddFontFromFileTTF(path, m_fFontSize, nullptr, ranges);
	//Log("GUI | ImFont pointer = 0x%X", m_pFont);
	font = io.Fonts->AddFontFromFileTTF(path2, m_fFontSize, nullptr, ranges);
	sprintf(path, "%sSAMP/fonts/%s", g_pszStorage, "muller.ttf");
	fontmoney = io.Fonts->AddFontFromFileTTF(path, m_fFontSize, nullptr, ranges);
	style.WindowRounding = 0.0f;
	
	

	m_pSplashTexture = nullptr;
	SetTexture((RwTexture*)LoadTextureFromDB("samp", "radar_bg"));

	m_pSplashTexture = (RwTexture*)LoadTextureFromDB("txd", "splash_icon");

	CRadarRect::LoadTextures();
	
	
	//RenderPosition();

	m_bKeysStatus = false;
}

CGUI::~CGUI()
{
	ImGui_ImplRenderWare_ShutDown();
	ImGui::DestroyContext();
}
#include "..//br/CNetwork.h"
bool g_IsVoiceServer()
{
	return true;
}

extern float g_fMicrophoneButtonPosX;
extern float g_fMicrophoneButtonPosY;
extern uint32_t g_uiLastTickVoice;

#include "..//voice/CVoiceChatClient.h"
extern CVoiceChatClient* pVoice;
void CGUI::PreProcessInput()
{
	ImGuiIO& io = ImGui::GetIO();

	io.MousePos = m_vTouchPos;
	io.MouseDown[0] = m_bMouseDown;
	if (!m_bNeedClearMousePos && m_bNextClear)
	{
		m_bNextClear = false;
	}
	if (m_bNeedClearMousePos && m_bNextClear)
	{
		io.MousePos = ImVec2(-1, -1);
		m_bNextClear = true;
	}
}
void CGUI::Create() {
    //Log(OBFUSCATE("Create BLACK RUSSIA HUD.."));
    
        Log("[gui] render hud up");
        CreateUp(300.0f, 300.0f, 620.0f, 620.0f, "radar_bg"); // верх
    
}

void CGUI::SetTexture(RwTexture* texture)
{
    rwTexture = texture;
}

void CGUI::CreateUp(float x, float y, float width, float height, const char* texture)
{
      CSprite2d* m_pSprite = new CSprite2d();
	  m_pSprite->m_pRwTexture = (RwTexture*)LoadTextureFromDB("samp", "logobr_moscow");
      
      
      CRGBA color_white;
      color_white.A = 255;
      color_white.B = 255;
      color_white.G = 255;
      color_white.R = 255;

      m_pSprite->Draw(x, y, width, height, color_white);
}
void CGUI::CoordsRadar(CRect* rect)
{
        rect->left=83.0f;
        rect->bottom=57.0f;
        rect->right=25.0f;
        rect->top=25.0f;
}

void CGUI::PostProcessInput()
{
	ImGuiIO& io = ImGui::GetIO();

	if (m_bNeedClearMousePos && io.MouseDown[0])
	{
		return;
	}

	if (m_bNeedClearMousePos && !io.MouseDown[0])
	{
		io.MousePos = ImVec2(-1, -1);
		m_bNextClear = true;
	}
}
bool isButtonPressed = false;
#include "..//debug.h"
extern CGame* pGame;
void CGUI::SetSpeed(int speed) {
    m_speed = speed;
}

void CGUI::SetFuelLevel(int fuelLevel) {
    m_fuelLevel = fuelLevel;
}

void CGUI::SetCarState(int carState) {
    m_carState = carState;
}

void drawColoredText(const char* text, const ImVec2& position, ImU32 color) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddText(font, m_fFontSizeSpeed*1.6f, position, color, text);
	
}
void drawColoredTextFuel(const char* text, const ImVec2& position, ImU32 color) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddText(font, m_fFontSizeSpeed/1.5f, position, color, text);
	
}
void drawColoredTextMeliage(const char* text, const ImVec2& position, ImU32 color) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddText(font, m_fFontSizeSpeed/1.3f, position, color, text);
	
}
void updateLEDs() {
	if (!pGame || !pNetGame || !pGame->FindPlayerPed()->IsInVehicle()) {
		return;
	}
	else if (pGame->FindPlayerPed()->IsAPassenger()) {
		return;
	}
	CVehicle *pVehicle = nullptr;
	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
	VEHICLEID id = pVehiclePool->FindIDFromGtaPtr(pPlayerPed->GetGtaVehicle());
	pVehicle = pVehiclePool->GetAt(id);
	if (pVehicle->m_iTurnState == CVehicle::eTurnState::TURN_LEFT) {
		bLeftLEDOn = !bLeftLEDOn; 
		bRightLEDOn = false; 
		bCenterLEDOn = false;	
		//pVehicle->TurnLights(bLeftLEDOn ? 1 : 0);		
	} else if (pVehicle->m_iTurnState == CVehicle::eTurnState::TURN_RIGHT) {
		bRightLEDOn = !bRightLEDOn; 
		bLeftLEDOn = false;
		bCenterLEDOn = false;	
	//	pVehicle->TurnLights(bRightLEDOn ? 2 : 0);			
	} else if (pVehicle->m_iTurnState == CVehicle::eTurnState::TURN_ALL) {
		bLeftLEDOn = !bLeftLEDOn;
		bRightLEDOn = !bRightLEDOn;
		bCenterLEDOn = !bCenterLEDOn;
	//	pVehicle->TurnLights(bCenterLEDOn ? 3 : 0);	
		//	pVehicle->toggleReverseLight(false);	

	} else {
		bLeftLEDOn = false;
		bRightLEDOn = false;
		bCenterLEDOn = false;
		//pVehicle->TurnLights(0);
	}	
}
ImVec2 posCurMenu;
void CircularProgressBar(const char* label, float progress, const ImVec2& size, const ImVec4& color) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return;

	ImGui::BeginGroup();
	ImGui::PushID(label);

	ImVec2 pos = ImGui::GetCursorScreenPos();
	float circleRadius = ImMin(size.x, size.y) * 0.5f;
	ImVec2 center = ImVec2(pos.x + circleRadius, pos.y + circleRadius);

	float arcAngle = 2.0f * IM_PI * progress;

	const ImU32 bgColor = ImGui::GetColorU32(ImVec4(0.2f, 0.2f, 0.2f, 0.8f));
//	window->DrawList->AddCircleFilled(center, circleRadius + 5, bgColor, 32);

	if (progress > 0.0f) {
		ImVec2 innerPos(center.x - circleRadius, center.y - circleRadius);
		ImVec2 outerPos(center.x + circleRadius, center.y + circleRadius);
		window->DrawList->PathArcTo(center, circleRadius, -IM_PI * 0.5f, -IM_PI * 0.5f + arcAngle, 32);
		window->DrawList->PathStroke(ImGui::GetColorU32(color), false, 10.0f);
	}

//	char textBuffer[32];
//	snprintf(textBuffer, sizeof(textBuffer), "%.0f%%", progress * 100.0f);
//	ImVec2 textSize = ImGui::CalcTextSize(textBuffer);
	//ImVec2 textPos = ImVec2(center.x - textSize.x * 0.5f, center.y - textSize.y * 0.5f);
	//window->DrawList->AddText(textPos, ImGui::GetColorU32(ImGuiCol_Text), textBuffer);

	ImGui::PopID();
	ImGui::EndGroup();
}
static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) 
{ 
    return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); 
}
void ImageRotated(ImTextureID tex_id, ImVec2 center, ImVec2 size, float angle)
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
	
	angle = angle + 3.14f;

    float cos_a = cosf(angle);
    float sin_a = sinf(angle);

    // Поворот относительно нижней точки по центру
    // Определяем вершины относительно центра нижней части изображения
    ImVec2 pos[4] = {
        center + ImRotate(ImVec2(-size.x * 0.5f, -size.y), cos_a, sin_a), // Левая верхняя
        center + ImRotate(ImVec2(+size.x * 0.5f, -size.y), cos_a, sin_a), // Правая верхняя
        center + ImRotate(ImVec2(+size.x * 0.5f, 0.0f), cos_a, sin_a), // Правая нижняя
        center + ImRotate(ImVec2(-size.x * 0.5f, 0.0f), cos_a, sin_a) // Левая нижняя
    };

    ImVec2 uvs[4] = {
        ImVec2(0.0f, 0.0f), 
        ImVec2(1.0f, 0.0f), 
        ImVec2(1.0f, 1.0f), 
        ImVec2(0.0f, 1.0f) 
    };
    draw_list->PushClipRect(center + ImVec2(-size.x * 0.5f, 0.0f - (size.y * 2)), center + ImVec2(+size.x * 0.5f, 0.0f), false);

    draw_list->AddImageQuad(tex_id, pos[0], pos[1], pos[2], pos[3], uvs[0], uvs[1], uvs[2], uvs[3], IM_COL32_WHITE);
	draw_list->PopClipRect(); 
}
float angle = 0.0f;
void CGUI::InitButtonPassenger(){
	ImGuiIO& io = ImGui::GetIO();
	
	ImVec2 displaySize = io.DisplaySize;
	
	ImVec2 windowPos = ImVec2(ImVec2(displaySize.x - (displaySize.x * 0.2), displaySize.y * 0.5));
	
	

	ImGui::SetNextWindowPos(windowPos);
	ImGui::Begin("###passengerbutton", nullptr,
    ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoScrollbar |
    ImGuiWindowFlags_NoSavedSettings |
    ImGuiWindowFlags_AlwaysAutoResize);
	
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	
	
	
	//ImGui::SetCursorScreenPos(ImVec2(displaySize.x - (displaySize.x * 0.2), displaySize.y * 0.5));
	
	ImVec2 imageSize = GetCoord(150.0f, 150.0f);
	
	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
		if (pVehiclePool)
		{
			VEHICLEID ClosetVehicleID = pVehiclePool->FindNearestToLocalPlayerPed();
			if (ClosetVehicleID < MAX_VEHICLES && pVehiclePool->GetSlotState(ClosetVehicleID))
			{
				CVehicle* pVehicle = pVehiclePool->GetAt(ClosetVehicleID);
				if (pVehicle)
				{
					if (pVehicle->GetDistanceFromLocalPlayerPed() < 7.0f)
					{
						
						//ImVec4 textColor2 = bSeatPasseger ? ImVec4(1.0f, 1.0f, 1.0f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
						CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
						if (pPlayerPool)
						{
							CLocalPlayer* pLocalPlayer;
							if (!pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsInVehicle() && !pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsAPassenger())
							{
								//if (ImGui::Button("G", vecButSize))
							//	{
								if(ImGui::ImageButton((ImTextureID)WidgetGetIn->raster, imageSize)){
									if (pNetGame)
									{
										if (pPlayerPool)
										{
											pLocalPlayer = pPlayerPool->GetLocalPlayer();
											if (pLocalPlayer)
											{
												pLocalPlayer->HandlePassengerEntryEx();
											}
										}
									}
									//seat passenger
								}
									
								//}
							}
						}
					}
				}
			}
		}
	
	ImGui::End();
}
void CGUI::InitHudSpeedometer(){
	float coefsir = 1.0f;
	 isButtonPressed = false;
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 vecButSize = GetCoord(175.0f, 175.0f);
	vecButSize = ImVec2(vecButSize.x * coefsir, vecButSize.y);
	ImVec2 imageSize = GetCoord(180.0f, 90.0f);
	ImVec2 vecButSize2 = GetCoord(vecButSize.x/1.25f, vecButSize.y/2.5f);
	ImVec2 vecButSizeKm = GetCoord(26.0f, 14.0f);
	ImVec2 vecButBlink = ImVec2(vecButSize.x/7.7f, vecButSize.y/7.7f);
	ImVec2 windowPos = ImVec2(0, 0); // Изменяем позицию окна, чтобы оно было в левом нижнем углу экрана

	ImGui::SetNextWindowPos(windowPos);
	ImGui::Begin("###speed", nullptr,
    ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoScrollbar |
    ImGuiWindowFlags_NoSavedSettings |
    ImGuiWindowFlags_AlwaysAutoResize);
	
	char fuel[20];
	char speed[10];
	char heal[10];
	char log[100];
	char meliage[50];
	
	int i_speed = 0;
	bDoor =0;
	bEngine = 0;
	bLights = 0;
	float fHealth = 0;
	CVehicle *pVehicle = nullptr;
	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
    VEHICLEID id = pVehiclePool->FindIDFromGtaPtr(pPlayerPed->GetGtaVehicle());
    pVehicle = pVehiclePool->GetAt(id);
    if(pPlayerPed)
    {
        if(pVehicle)
        {
            VECTOR vecMoveSpeed;
            pVehicle->GetMoveSpeedVector(&vecMoveSpeed);
            i_speed = sqrt((vecMoveSpeed.X * vecMoveSpeed.X) + (vecMoveSpeed.Y * vecMoveSpeed.Y) + (vecMoveSpeed.Z * vecMoveSpeed.Z)) * 180;
            fHealth = pVehicle->GetHealth();
            bDoor = pVehicle->GetDoorState();
            bEngine = pVehicle->GetEngineState();
            bLights = pVehicle->GetLightsState();
        }
    }
	sprintf(speed, "%03d", static_cast<int>(round(i_speed)));
	sprintf(fuel, "%03d", static_cast<int>(round(m_fuel)));
	sprintf(meliage, "%06d", static_cast<int>(round(bMeliage)));
	sprintf(heal, "%d%s", static_cast<int>(round(fHealth/10.0f)), "%");
	
	ImVec2 vecButSizeFuel = ImVec2(vecButSize2.y/2.7f, vecButSize2.y/2);
	
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	
	ImGui::SetCursorScreenPos(ImVec2(io.DisplaySize.x/2 - vecButSize.x/2, io.DisplaySize.y - vecButSize.y - GetCoord(9.0f, 9.0f).x));
	
	ImVec2 posCur = ImGui::GetCursorScreenPos();
	
	ImVec2 textPos = ImVec2(posCur.x + vecButSize.x / 2, posCur.y + vecButSize.y / 2);
	
	posCurClick[2] = ImGui::GetCursorScreenPos();
	posSizeClick[2] = vecButSize;
	ImVec2 poss = ImGui::GetCursorScreenPos();
	int speed2 = static_cast<int>(round(i_speed));
	ImGui::SetCursorScreenPos(poss);
	int spped = speed2;
	if(speed2 > 339){
		spped = 340;
	}
	ImageRotated(speedo_scale2->raster, ImVec2(ImGui::GetCursorScreenPos().x + vecButSize.x/2, ImGui::GetCursorScreenPos().y + vecButSize.y/1.95), ImVec2(vecButSize.x, vecButSize.y/2), /*170.0f + */(static_cast<float>(spped) / (340.0f / 3.14f)));

	ImGui::SetCursorScreenPos(poss);
	ImGui::Image((ImTextureID)speedo_center->raster, vecButSize);
	
	ImGui::SetCursorScreenPos(poss);
	//CircularProgressBar("", i_speed/300, ImVec2(vecButSize.x*2, vecButSize.y), ImVec4(1.0f, 1.0f,1.0f,1.0f));
	//	ImGui::SetCursorScreenPos(poss);
	ImVec4 textColor2 = bCenterLEDOn ? ImVec4(1.0f, 1.0f, 1.0f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
//	 ImGui::PushStyleColor(ImGuiCol_Text, textColor);
	ImGui::Image((ImTextureID)speedo_blink->raster, vecButSize, ImVec2(0, 0), ImVec2(1, 1), ImColor(textColor2));
	
	ImDrawList* drawList = ImGui::GetWindowDrawList();
//	drawList->AddImageRotated((ImTextureID)speedo_scale->raster, posCur, ImVec2(vecButSize.x, vecButSize.y/2) rotationAngle, ImVec2(0, vecButSize.y/2), ImVec2(1, 0));
	
	
	//drawList->PopClipRect();
	//Log("%f %f", rotatedBottomLeft, rotatedTopRight);

	ImGui::SetCursorScreenPos(ImVec2((posCur.x+vecButSize.x/2)-vecButBlink.x/2, posCur.y+vecButBlink.y*1.1f));
	
	ImGui::Image(bCenterLEDOn ? (ImTextureID)speedo_ablinker->raster : (ImTextureID)speedo_blinker->raster, vecButBlink);
	
	ImVec2 h = font->CalcTextSizeA(m_fFontSizeSpeed*1.6f, FLT_MAX, 0.0f, "000");
	 vecButSizeKm = GetCoord(h.x/2, h.y/2);
    ImGui::SetCursorPosX(textPos.x - h.x/2);
	ImGui::SetCursorPosY(ImGui::GetCursorScreenPos().y + (float)GetCoord(1.0f, 1.0f).x);
    //ImGui::GetWindowDrawList()->AddText(font, m_fFontSizeSpeed*2, , ImColor(255, 255, 255, 102), speed);
	
	ImVec2 startPos = ImGui::GetCursorScreenPos();
	ImVec2 h2 = font->CalcTextSizeA(m_fFontSizeSpeed*1.6f, FLT_MAX, 0.0f, "0");
	ImColor speed11 = ImColor(0,0, 0, 0);
	ImColor speed22 = ImColor(0, 0, 0, 0);
	ImColor speed33 = ImColor(0, 0, 0, 0);
	ImColor speed44 = ImColor(0,0, 0, 0);
	ImColor speed55 = ImColor(0, 0, 0, 0);
	ImColor speed66 = ImColor(0, 0, 0, 0);

	if (speed2 == 0) {
		speed11 = ImColor(255, 255, 255, 102);
		speed22 = ImColor(255, 255, 255, 102);
		speed33 = ImColor(255, 255, 255, 102);
		drawColoredText(speed, startPos, speed11); 
	} else if (speed2 > 0 && speed2 < 10) {
		speed11 = ImColor(255, 255, 255, 102);
		speed22 = ImColor(255, 255, 255, 102); // Установка прозрачности обратно на полную
		speed33 = ImColor(255, 255, 255, 255); // Установка прозрачности обратно на полную
		drawColoredText("00", startPos, speed11); 
		drawColoredText(&speed[2], ImVec2(startPos.x + h2.x*2, startPos.y), speed33);
	} else if (speed2 > 9 && speed2 < 100) {
		speed11 = ImColor(255, 255, 255, 102);
		speed22 = ImColor(255, 255, 255, 255);
		speed33 = ImColor(255, 255, 255, 255); 
		drawColoredText(&speed[0], startPos, speed11); 
		drawColoredText(&speed[1], ImVec2(startPos.x + h2.x, startPos.y), speed22); 
		drawColoredText(&speed[2], ImVec2(startPos.x + h2.x*2, startPos.y), speed33);
	}
	else if (speed2 > 99 && speed2 < 99999) {
		speed11 = ImColor(255, 255, 255, 255);
		speed22 = ImColor(255, 255, 255, 255);
		speed33 = ImColor(255, 255, 255, 255); // Установка прозрачности обратно на полную
		drawColoredText(&speed[0], startPos, speed11); 
		drawColoredText(&speed[1], ImVec2(startPos.x + h2.x, startPos.y), speed22); 
		drawColoredText(&speed[2], ImVec2(startPos.x + h2.x*2, startPos.y), speed33);
	}

	ImGui::SetCursorPosY(ImGui::GetCursorScreenPos().y + (float)GetCoord(0.3f, 1.0f).x + (float)GetCoord(6.0f, 1.0f).x + h.y);
	ImGui::SetCursorPosX(textPos.x - vecButSizeKm.x/2);
	ImGui::Image((ImTextureID)speedo_h->raster, vecButSizeKm);
	 h = font->CalcTextSizeA(m_fFontSizeSpeed/1.3f, FLT_MAX, 0.0f, "000000");
    ImGui::SetCursorPosX(textPos.x - h.x/2 - GetCoord(22.0f, 22.0f).x/2);
	ImGui::SetCursorPosY(ImGui::GetCursorScreenPos().y + (float)GetCoord(4.0f, 1.0f).x);
     h = font->CalcTextSizeA(m_fFontSizeSpeed/1.3f, FLT_MAX, 0.0f, "000000");
    ImGui::SetCursorPosX(textPos.x - h.x/2 - GetCoord(22.0f, 22.0f).x/2);
	//ImGui::SetCursorPosY(ImGui::GetCursorScreenPos().y + (float)GetCoord(1.0f, 1.0f).x);

	 startPos = ImGui::GetCursorScreenPos();
	 h2 = font->CalcTextSizeA(m_fFontSizeSpeed/1.3f, FLT_MAX, 0.0f, "0");
		
	if (bMeliage == 0) {
		speed11 = ImColor(255, 255, 255, 102);
		speed22 = ImColor(255, 255, 255, 102);
		speed33 = ImColor(255, 255, 255, 102);
		speed44 = ImColor(255, 255, 255, 102);
		speed55 = ImColor(255, 255, 255, 102);
		speed66 = ImColor(255, 255, 255, 102);
		drawColoredTextMeliage(meliage, startPos, speed11); 
	} else if (bMeliage > 0 && bMeliage < 10) {
		speed11 = ImColor(255, 255, 255, 102);
		speed22 = ImColor(255, 255, 255, 102);
		speed33 = ImColor(255, 255, 255, 102);
		speed44 = ImColor(255, 255, 255, 102);
		speed55 = ImColor(255, 255, 255, 102);
		speed66 = ImColor(255, 255, 255, 255); 
		
		drawColoredTextMeliage("00000", startPos, speed11); 
	drawColoredTextMeliage(&meliage[5], ImVec2(startPos.x + h2.x*5, startPos.y), speed66);
	} else if (bMeliage > 9 && bMeliage < 100) {
		speed11 = ImColor(255, 255, 255, 102);
		speed22 = ImColor(255, 255, 255, 102);
		speed33 = ImColor(255, 255, 255, 102);
		speed44 = ImColor(255, 255, 255, 102);
		speed66 = ImColor(255, 255, 255, 255);
		speed55 = ImColor(255, 255, 255, 255);
		drawColoredTextMeliage("0000", startPos, speed11); 
	drawColoredTextMeliage(&meliage[4], ImVec2(startPos.x + h2.x*4, startPos.y), speed55);
	drawColoredTextMeliage(&meliage[5], ImVec2(startPos.x + h2.x*5, startPos.y), speed66);
	}
	else if (bMeliage > 99 && bMeliage < 1000) {
		speed11 = ImColor(255, 255, 255, 102);
		speed22 = ImColor(255, 255, 255, 102);
		speed33 = ImColor(255, 255, 255, 102);
		speed55 = ImColor(255, 255, 255, 255);
		speed66 = ImColor(255, 255, 255, 255);
		speed44 = ImColor(255, 255, 255, 255); 
		drawColoredTextMeliage("000", startPos, speed11); 
	drawColoredTextMeliage(&meliage[3], ImVec2(startPos.x + h2.x*3, startPos.y), speed44);
	drawColoredTextMeliage(&meliage[4], ImVec2(startPos.x + h2.x*4, startPos.y), speed55);
	drawColoredTextMeliage(&meliage[5], ImVec2(startPos.x + h2.x*5, startPos.y), speed66);
	}
	else if (bMeliage > 999 && bMeliage < 10000) {
		speed11 = ImColor(255, 255, 255, 102);
		speed22 = ImColor(255, 255, 255, 102);
		speed55 = ImColor(255, 255, 255, 255);
		speed66 = ImColor(255, 255, 255, 255);
		speed44 = ImColor(255, 255, 255, 255);
speed33 = ImColor(255, 255, 255, 255);		
drawColoredTextMeliage("00", startPos, speed11); 
	drawColoredTextMeliage(&meliage[2], ImVec2(startPos.x + h2.x*2, startPos.y), speed33);
	drawColoredTextMeliage(&meliage[3], ImVec2(startPos.x + h2.x*3, startPos.y), speed44);
	drawColoredTextMeliage(&meliage[4], ImVec2(startPos.x + h2.x*4, startPos.y), speed55);
	drawColoredTextMeliage(&meliage[5], ImVec2(startPos.x + h2.x*5, startPos.y), speed66);
	}
	else if (bMeliage > 9999 && bMeliage < 100000) {
		speed11 = ImColor(255, 255, 255, 102);
		speed55 = ImColor(255, 255, 255, 255);
		speed66 = ImColor(255, 255, 255, 255);
		speed44 = ImColor(255, 255, 255, 255);
speed33 = ImColor(255, 255, 255, 255);
speed22 = ImColor(255, 255, 255, 255);		
drawColoredTextMeliage(&meliage[0], startPos, speed11); 
	drawColoredTextMeliage(&meliage[1], ImVec2(startPos.x + h2.x, startPos.y), speed22); 
	drawColoredTextMeliage(&meliage[2], ImVec2(startPos.x + h2.x*2, startPos.y), speed33);
	drawColoredTextMeliage(&meliage[3], ImVec2(startPos.x + h2.x*3, startPos.y), speed44);
	drawColoredTextMeliage(&meliage[4], ImVec2(startPos.x + h2.x*4, startPos.y), speed55);
	drawColoredTextMeliage(&meliage[5], ImVec2(startPos.x + h2.x*5, startPos.y), speed66);
	}
	else if (bMeliage > 99999 && bMeliage < 100000000) {
		speed55 = ImColor(255, 255, 255, 255);
		speed66 = ImColor(255, 255, 255, 255);
		speed44 = ImColor(255, 255, 255, 255);
speed33 = ImColor(255, 255, 255, 255);
speed22 = ImColor(255, 255, 255, 255);
speed11 = ImColor(255, 255, 255, 255);		
drawColoredTextMeliage(&meliage[0], startPos, speed11); 
	drawColoredTextMeliage(&meliage[1], ImVec2(startPos.x + h2.x, startPos.y), speed22); 
	drawColoredTextMeliage(&meliage[2], ImVec2(startPos.x + h2.x*2, startPos.y), speed33);
	drawColoredTextMeliage(&meliage[3], ImVec2(startPos.x + h2.x*3, startPos.y), speed44);
	drawColoredTextMeliage(&meliage[4], ImVec2(startPos.x + h2.x*4, startPos.y), speed55);
	drawColoredTextMeliage(&meliage[5], ImVec2(startPos.x + h2.x*5, startPos.y), speed66);
	}
	
	//ImGui::SameLine( 0,0);
	ImGui::SetCursorPosX(textPos.x + h.x/2 - GetCoord(22.0f, 22.0f).x/2);
	//ImGui::SetCursorPosY(ImGui::GetCursorScreenPos().y);
	//ImGui::SetCursorPosY(ImGui::GetCursorScreenPos().y + (float)GetCoord(0.3f, 1.0f).x);
	ImGui::Image((ImTextureID)speedo_km->raster, ImVec2(GetCoord(22.0f, 22.0f).x, h.y));
	ImGui::SetCursorScreenPos(ImVec2(posCur.x + GetCoord(2.0f, 2.0f).x, posCur.y+ GetCoord(2.0f, 2.0f).x));
	
	ImGui::Image((ImTextureID)speedo_scale->raster, ImVec2(vecButSize.x- GetCoord(4.0f, 4.0f).x, vecButSize.y/2));
	
	ImGui::SameLine( 0,ImGui::GetCursorPosX() + GetCoord(5.5f, 5.5f).x);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY()+((vecButSize.y-vecButSize2.y)/2));
	ImVec2 posCurHealth = ImGui::GetCursorScreenPos();
	ImGui::Image((ImTextureID)speedo_right->raster, vecButSize2);

	ImGui::SameLine( 0,ImGui::GetCursorPosX() + GetCoord(5.5f, 5.5f).x);
	float y = ImGui::GetCursorPosY();
	posCurClick[0] = ImGui::GetCursorScreenPos();
	posSizeClick[0] = vecButSize2;
	ImTextureID tex2 = nullptr;
	if(bRightLEDOn){
		tex2 = (ImTextureID)speedo_arrowa->raster;
	}else{
		tex2 = (ImTextureID)speedo_arrow->raster;
	}
	
	ImGui::Image(tex2, vecButSize2);
	
	float invert = /*(posCur.x + vecButSize.x/2)-*/(posCur.x - GetCoord(5.5f, 5.5f).x) - (vecButSize2.x*2) - GetCoord(5.5f, 5.5f).x/* * (-1)*/;
	ImGui::SetCursorPosY(y);
	ImGui::SetCursorPosX(invert);
	//Log("posCur x: %f posCur y: %f invert: %f", posCur.x, posCur.y, invert);
	//Log("today posCur x: %f posCur y: %f", ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);
	posCurClick[1] = ImGui::GetCursorScreenPos();
	posSizeClick[1] = vecButSize2;
	ImTextureID tex = nullptr;
	if(bLeftLEDOn){
		tex = (ImTextureID)speedo_arrowa->raster;
	}else{
		tex = (ImTextureID)speedo_arrow->raster;
	}
	// = bLeftLEDOn ? (ImTextureID)speedo_arrowa->raster : (ImTextureID)speedo_arrow->raster;
    ImGui::Image(tex, vecButSize2, ImVec2(1.0f, 0.0f), ImVec2(0.0f, 1.0f));
	//                                       /\                   /\     это инверт
	ImGui::SameLine( 0,ImGui::GetCursorPosX() + GetCoord(5.5f, 5.5f).x);
	//float y = ImGui::GetCursorPosY();
	ImVec2 posCurFuel = ImGui::GetCursorScreenPos();
	ImGui::Image((ImTextureID)speedo_right->raster, vecButSize2, ImVec2(1.0f, 0.0f), ImVec2(0.0f, 1.0f));
	
	

	//ImGui::PushFont(io.Fonts->AddFontFromFileTTF(path2, 20.0f));
	
	ImGui::SetCursorScreenPos(ImVec2(textPos.x - vecButBlink.x*1.1f, (textPos.y+vecButSize.y/2)- vecButBlink.x*1.1f));
	ImVec4 textColor = bDoor ? ImVec4(1.0f, 0.0f, 0.0f, 1.0f) : ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
//	ImGui::PushStyleColor(ImGuiCol_Text, textColor);
	ImGui::Image((ImTextureID)speedo_doors->raster, ImVec2(vecButBlink.x/1.3f, vecButBlink.y/1.5f), ImVec2(0, 0), ImVec2(1, 1), ImColor(textColor));
	//ImGui::GetWindowDrawList()->AddImage((ImTextureID)speedo_doors->raster, ImGui::GetCursorScreenPos(), ImVec2(vecButBlink.x/1.3f, vecButBlink.y/1.5f), ImVec2(0, 0), ImVec2(1, 1), ImColor(textColor));
	//ImGui::PopStyleColor();
	ImGui::SetCursorScreenPos(ImVec2(textPos.x + vecButBlink.x*0.1f, (textPos.y+vecButSize.y/2)- vecButBlink.x*1.1f));
	 textColor = bEngine ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
//	 ImGui::PushStyleColor(ImGuiCol_Text, textColor);
	ImGui::Image((ImTextureID)speedo_engine->raster, ImVec2(vecButBlink.x/1.3f, vecButBlink.y/1.5f), ImVec2(0, 0), ImVec2(1, 1), ImColor(textColor));
	//ImGui::GetWindowDrawList()->AddImage((ImTextureID)speedo_engine->raster, ImGui::GetCursorScreenPos(), ImVec2(vecButBlink.x/1.3f, vecButBlink.y/1.5f), ImVec2(0, 0), ImVec2(1, 1), ImColor(textColor));
	//ImGui::PopStyleColor();
	ImGui::SetCursorScreenPos(ImVec2(posCurFuel.x + vecButSizeFuel.x*2.1f, posCurFuel.y+vecButSizeFuel.y/2));
	ImGui::Image((ImTextureID)speedo_fuel->raster, vecButSizeFuel);
	ImGui::SameLine( 0,ImGui::GetCursorPosX() + GetCoord(2.5f, 2.5f).x);
	 h = font->CalcTextSizeA(m_fFontSizeSpeed/1.5f, FLT_MAX, 0.0f, "000");
	 

	 
	ImGui::SetCursorPosY(textPos.y-h.y-h.y/4);
	ImVec2 ff = ImGui::GetCursorScreenPos();

	 startPos = ImGui::GetCursorScreenPos();
	 h2 = font->CalcTextSizeA(m_fFontSizeSpeed/1.5f, FLT_MAX, 0.0f, "0");

	if (m_fuel == 0) {
		speed11 = ImColor(255, 255, 255, 102);
		speed22 = ImColor(255, 255, 255, 102);
		speed33 = ImColor(255, 255, 255, 102);
		drawColoredTextFuel(fuel, startPos, speed11); 
	} else if (m_fuel > 0 && m_fuel < 10) {
		speed11 = ImColor(255, 255, 255, 102);
		speed22 = ImColor(255, 255, 255, 102); // Установка прозрачности обратно на полную
		speed33 = ImColor(255, 255, 255, 255); // Установка прозрачности обратно на полную
		drawColoredTextFuel("00", startPos, speed11); 
		drawColoredTextFuel(&fuel[2], ImVec2(startPos.x + h2.x*2, startPos.y), speed33);
	} else if (m_fuel > 9 && m_fuel < 100) {
		speed11 = ImColor(255, 255, 255, 102);
		speed22 = ImColor(255, 255, 255, 255);
		speed33 = ImColor(255, 255, 255, 255); // Установка прозрачности обратно на полную
		drawColoredTextFuel(&fuel[0], startPos, speed11); 
		drawColoredTextFuel(&fuel[1], ImVec2(startPos.x + h2.x, startPos.y), speed22); 
		drawColoredTextFuel(&fuel[2], ImVec2(startPos.x + h2.x*2, startPos.y), speed33);
	}
	else if (m_fuel > 99 && m_fuel < 99999) {
		speed11 = ImColor(255, 255, 255, 255);
		speed22 = ImColor(255, 255, 255, 255);
		speed33 = ImColor(255, 255, 255, 255); // Установка прозрачности обратно на полную
		drawColoredTextFuel(&fuel[0], startPos, speed11); 
		drawColoredTextFuel(&fuel[1], ImVec2(startPos.x + h2.x, startPos.y), speed22); 
		drawColoredTextFuel(&fuel[2], ImVec2(startPos.x + h2.x*2, startPos.y), speed33);
	}
	//ImGui::GetWindowDrawList()->AddText(font, m_fFontSizeSpeed/1.5f, ImGui::GetCursorScreenPos(), ImColor(255, 255, 255, 102), "000");
	ImGui::SetCursorPosY(ff.y+h.y+(h.y/7));
	ImGui::SetCursorPosX(ff.x);
	ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetCursorScreenPos(), ImVec2(ImGui::GetCursorScreenPos().x + h.x, ImGui::GetCursorScreenPos().y + h.y/4), ImColor(255, 255, 255, 102));
	ImGui::SetCursorPosY(ImGui::GetCursorScreenPos().y+(h.y/7)+ h.y/4);
	ImGui::GetWindowDrawList()->AddText(font, m_fFontSizeSpeed/1.5f, ImGui::GetCursorScreenPos(), ImColor(255, 255, 255, 255), "150");
	ImGui::SameLine( 0,GetCoord(4.5f, 4.5f).x + h.x);
	 ImGui::SetCursorPosY(textPos.y-vecButSizeFuel.y/4);
	 ImGui::Image((ImTextureID)speedo_l->raster, ImVec2(vecButSizeFuel.x/2, vecButSizeFuel.y/2));
	ImGui::SetCursorScreenPos(ImVec2(posCurHealth.x + vecButSizeFuel.x/2.0f, posCurHealth.y+vecButSizeFuel.y/2));
	ImGui::Image((ImTextureID)speedo_health->raster, vecButSizeFuel);
	ImGui::SameLine( 0,ImGui::GetCursorPosX() + GetCoord(2.5f, 2.5f).x);
	 h = font->CalcTextSizeA(m_fFontSizeSpeed/1.3f, FLT_MAX, 0.0f, heal);
	ImGui::SetCursorPosY(textPos.y-h.y/2);
	ImGui::GetWindowDrawList()->AddText(font, m_fFontSizeSpeed/1.3f, ImGui::GetCursorScreenPos(), ImColor(1.0f, 1.0f, 1.0f), heal);
	
	//if(!speedometershow){
	//posCurMenu = ImGui::GetCursorScreenPos()
	if(pChatWindow->isConnect){
		if(!pKeyBoard->m_bEnable)
		{
			ImGui::SetCursorPosX(posCurMenu.x);
			ImGui::SetCursorPosY(posCurMenu.y + GetCoord(7.5f, 7.5f).x + GetCoord(4.5f, 4.5f).x);
			if(ImGui::ImageButton((ImTextureID)hud_menu->raster, GetCoord(196.0f, 71.0f))){
				g_pJavaWrapper->MenuHud();
			}
			ImGui::SameLine( 0,ImGui::GetCursorPosX() + GetCoord(5.5f, 5.5f).x);
			if(ImGui::ImageButton((ImTextureID)hud_donate->raster, GetCoord(71.0f, 71.0f))){
				pNetGame->SendChatCommand("/donate");
				//((void(*)(/*void* thiz*/))(g_libGTASA + 0x39AFE8 + 1))();
			//	((void(*)(/*void* thiz*/))(g_libGTASA + 0x26A2AC + 1))();

			}//}
		}	
	}
	//ImGui::Image(maskID, imageSize);

//	ImGui::PushFont(fontorig);
	//font = nullptr;
	
	ImGui::End();
}
void CGUI::InitHudLogo(){
	 isButtonPressed = false;
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 vecButSize = ImVec2(ImGui::GetFontSize() * 8.0, ImGui::GetFontSize() * 4.0);

	ImVec2 windowPos = ImVec2(0, io.DisplaySize.y - vecButSize.y); // Изменяем позицию окна, чтобы оно было в левом нижнем углу экрана

	ImGui::SetNextWindowPos(windowPos);
	ImGui::Begin("###logo", nullptr,
    ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoScrollbar |
    ImGuiWindowFlags_NoSavedSettings |
    ImGuiWindowFlags_AlwaysAutoResize);
	
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	
	
	
	ImGui::Image((ImTextureID)logo->raster, vecButSize);

	ImGui::End();
}
void formattermon(const int number, char *formattedNumber) {
    sprintf(formattedNumber, "%d", number);
  

}
#include "..//game/pad.h"
char* formatmoney(int money) {
    const int MAX_SIZE = 20; 
    char moneyStr[MAX_SIZE];

    sprintf(moneyStr, "%d", money); 

    int length = 0;
    while (moneyStr[length] != '\0') {
        length++;
    }

    char* result = new char[MAX_SIZE + length / 3 + 2]; // Добавлен дополнительный символ для точки
    int separatorCount = 0;
    int index = 0;
    bool addedPoint = false; // Флаг, указывающий, что точка уже добавлена
    for (int i = length - 1; i >= 0; i--) {
        if (separatorCount == 3 && !addedPoint) {
            result[index] = '.'; // Добавьте точку, если она еще не добавлена
            index++;
            addedPoint = true;
            separatorCount = 0;
        }

        result[index] = moneyStr[i];
        separatorCount++;
        index++;
    }
    result[index] = '\0';

    for (int i = 0; i < index / 2; i++) {
      char temp = result[i];
result[i] = result[index - i - 1];
result[index - i - 1] = temp;
    }

    return result;
}
float EntityGetDistanceFromCamera(ENTITY_TYPE* m_pEntity)
{
	if(!m_pEntity || m_pEntity->vtable == SA_ADDR(0x5C7358))
		return 100000.0f;
	
	float tmpX = ( m_pEntity->mat->pos.X - *(float*)(SA_ADDR(0x8B1134)));
	float tmpY = ( m_pEntity->mat->pos.Y - *(float*)(SA_ADDR(0x8B1138)));
	float tmpZ = ( m_pEntity->mat->pos.Z - *(float*)(SA_ADDR(0x8B113C)));

	return sqrt(tmpX*tmpX + tmpY*tmpY + tmpZ*tmpZ);
}

#include <algorithm>
#include <string>
std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
	size_t start_pos = 0;
	while((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
    	}

    	return str;
}
bool isTouchOnButton(ImVec2 touchPos, ImVec2 buttonPos, ImVec2 buttonSize) {
    return (touchPos.x >= buttonPos.x && touchPos.x <= buttonPos.x + buttonSize.x &&
            touchPos.y >= buttonPos.y && touchPos.y <= buttonPos.y + buttonSize.y);
}
void CGUI::InitHud(){
	
	 isButtonPressed = false;
	ImGuiIO& io = ImGui::GetIO();
	//ImVec2 vecButSize = ImVec2(ImGui::GetFontSize() * 8.0, ImGui::GetFontSize() * 4.0);
	
	float userInputHeight = 87.8f;
	float userInputWidth = 443.0f;
	
	float scaledWidth = userInputWidth * 1366 / 1920; // Приведение пользовательской ширины к ширине 1366
	float scaledHeight = userInputHeight * 768 / 1080; // Приведение пользовательской высоты к высоте 768
	ImVec2 windowPos = GetCoord(41.0f, 49.0f); // Изменяем позицию окна, чтобы оно было в левом нижнем углу экрана

	ImGui::SetNextWindowPos(windowPos); 
	ImGui::Begin("###hud_main", nullptr,
    ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoScrollbar |
    ImGuiWindowFlags_NoSavedSettings |
    ImGuiWindowFlags_AlwaysAutoResize);
	
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	
	//ImGui::Image((ImTextureID)logo->raster, vecButSize);
	
	/*if(ImGui::ImageButton((ImTextureID)sprint->raster, GetCoord(198.0f, 198.0f))){
		isButtonPressed = true;
	}
	if(isButtonPressed){
		pChatWindow->sprint = 255;
	}else{
		pChatWindow->sprint = 0;
	}
	isButtonPressed = false;*/
	float heightProgress = GetCoord(198.0f, 79.0f).y / 4.5;
	float widthProgress = GetCoord(198.0f, 79.0f).x * 0.6f;
	
	float halfHeight = heightProgress * 0.7f;
	
	
	float centerX = ImGui::GetCursorScreenPos().x+(GetCoord(288.0f, 58.5f).x / 2);
	float centerY = ImGui::GetCursorScreenPos().y;
	ImVec2 posCur = ImGui::GetCursorScreenPos();
	ImGui::Image((ImTextureID)hud_up->raster, GetCoord(288.0f, 65.0f));

	ImGui::SetCursorScreenPos(ImVec2(posCur.x+(GetCoord(288.0f, 58.5f).y/3), posCur.y+(GetCoord(288.0f, 58.5f).y/3)));
	
	ImGui::Image((ImTextureID)hud_ruble->raster, ImVec2(GetCoord(288.0f, 58.5f).y/3, GetCoord(288.0f, 58.5f).y/2.5));

	
	ImGui::SetCursorScreenPos(ImVec2(posCur.x, posCur.y+(GetCoord(288.0f, 58.5f).y/3.5)));

	ImVec2 textSize = ImGui::CalcTextSize(moneyFormat((long)pGame->GetLocalMoney()).c_str()); // Размер текста
	float textOffsetX = textSize.x/2; 
	float textOffsetY = textSize.y;
	float g = (float)centerX - (float)textOffsetX - (float)textOffsetX;
	ImGui::SetCursorPosX(g);
	ImGui::SetCursorPosY(centerY - textOffsetY - (textOffsetY/8));
	ImGui::Text("%s", moneyFormat((long)pGame->GetLocalMoney()).c_str());
	

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + GetCoord(20.0f, 20.0f).x);
	 posCur = ImGui::GetCursorScreenPos();
	float xStart = ImGui::GetCursorScreenPos().x;
	ImGui::Image((ImTextureID)hud_center->raster, GetCoord(198.0f, 79.0f));
	//ImGui::SameLine(0,0);
	float health = 0;
	if(pGame->FindPlayerPed()->GetHealth() > 100.0f){
		health = 1.0f;
	}else{
		health = (float)pGame->FindPlayerPed()->GetHealth()/100.0f;
	}
	float armorr = 0;
	if(pGame->FindPlayerPed()->GetArmour() > 100.0f){
		armorr = 1.0f;
	}else{
		armorr = (float)pGame->FindPlayerPed()->GetArmour()/100.0f;
	}
	
	ImGui::SetCursorScreenPos(ImVec2(posCur.x + GetCoord(10.0f, 10.0f).x, posCur.y + halfHeight));//картинка хп
	ImGui::Image((ImTextureID)hud_heart->raster, ImVec2(heightProgress * 1.4f, heightProgress*1.4f));
	ImGui::SameLine(0 ,ImGui::GetCursorPosX() + GetCoord(10.5f, 10.5f).x);
	posCur = ImGui::GetCursorScreenPos();
	ImGui::Image((ImTextureID)hud_scale->raster, ImVec2(widthProgress, heightProgress));
	ImGui::SetCursorScreenPos(ImVec2(posCur.x, posCur.y));
	ImGui::Image((ImTextureID)hud_heart_progress->raster, ImVec2(widthProgress * (health), heightProgress));
	
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + heightProgress); 
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + GetCoord(10.0f, 10.0f).x);
	ImGui::Image((ImTextureID)hud_armor->raster, ImVec2(heightProgress * 1.4f, heightProgress*1.4f));//картинка брони
	ImGui::SameLine(0 ,ImGui::GetCursorPosX() + GetCoord(10.5f, 10.5f).x);
	posCur = ImGui::GetCursorScreenPos();
	ImGui::Image((ImTextureID)hud_scale->raster, ImVec2(widthProgress, heightProgress));
	ImGui::SetCursorScreenPos(ImVec2(posCur.x, posCur.y));
	ImGui::Image((ImTextureID)hud_armor_progress->raster, ImVec2(widthProgress * (armorr), heightProgress));
	
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + GetCoord(13.5f, 13.5f).x);//5.7f
	posCur = ImGui::GetCursorScreenPos();
	ImGui::Image((ImTextureID)hud_down->raster, GetCoord(232.0f, 127.5f));
	ImGui::SetCursorScreenPos(ImVec2(posCur.x+GetCoord(12.0f,12.0f).x, posCur.y+GetCoord(9.0f,9.0f).x));
	if(ImGui::ImageButton((ImTextureID)hud_fist[GamePool_FindPlayerPed()->WeaponSlots[GamePool_FindPlayerPed()->byteCurWeaponSlot].dwType]->raster, GetCoord(132.0f, 112.0f))){
		if(!pGame->FindPlayerPed()) return;

		if(!pChatWindow->bWeaponClicked) {
			pChatWindow->bWeaponClicked = true;
		}
		else {
			pChatWindow->bWeaponClicked = false;
		}
	}
	if(!speedometershow && pChatWindow->isConnect){
		posCurMenu = ImGui::GetCursorScreenPos();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + GetCoord(7.5f, 7.5f).x + GetCoord(4.5f, 4.5f).x);
		if(!pKeyBoard->m_bEnable)
		{
			if(ImGui::ImageButton((ImTextureID)hud_menu->raster, GetCoord(196.0f, 71.0f))){
				g_pJavaWrapper->MenuHud();
			}
			ImGui::SameLine( 0,ImGui::GetCursorPosX() + GetCoord(5.5f, 5.5f).x);
			if(ImGui::ImageButton((ImTextureID)hud_donate->raster, GetCoord(71.0f, 71.0f)))
			{
				/**(int32_t *)(g_libGTASA + 0x63E384) = 3; //shaders
				*(int32_t *)(g_libGTASA + 0x63E3C4) = 33; //distance
				*(int32_t *)(g_libGTASA + 0x63E3A4) = 300; //pixels
				*(int32_t *)(g_libGTASA + 0x63E424) = 3; //shadows
				*(int32_t *)(g_libGTASA + 0x63E464) = 3; //reflection*/
				//*(uint8_t*)(g_libGTASA + 0x008C9BA3) = 1; //pause
				pNetGame->SendChatCommand("/donate");
			//	((void(*)())(g_libGTASA + 0x259E50 + 1))();
			}
		}
	}
	//ImGui::SetCursorPos(ImVec2(scaledX, scaledY));
	float x1 = (GetCoord(214.0f, 89.0f).x + GetCoord(234.0f, 234.0f).x / 2) * 0.331498778f;
	float y1 = (GetCoord(214.0f, 89.0f).y + GetCoord(234.0f, 234.0f).y / 2) * 0.434498778f;
	
	//Log("radar x1: %f y1: %f", x1, y1);
	
	
	
	CRadar::radarBgPos1.x1 = x1 * 1.9380983f;
	CRadar::radarBgPos1.y1 = y1 * 1.0573447f;
	
	
	
	CRadar::radarBgPos2.x1 = x1 * 2.113233f;
	CRadar::radarBgPos2.y1 = x1 * 2.113233f;
	
	// все координаты
	
	screenWidth = io.DisplaySize.x;
    screenHeight = io.DisplaySize.y;

	float real_prcX = (((float)CRadar::radarBgPos1.x1 + (float)((float)CRadar::radarBgPos2.x1 / (float)2.0f)) / (float)screenWidth) * (float)100.0f;
	float real_prcY = (((float)CRadar::radarBgPos1.y1 + (float)((float)CRadar::radarBgPos2.y1 / (float)2.26f)) / (float)screenHeight) * (float)100.0f; // fix
	float gtaX = ((float)640.0f * ((float)real_prcX / (float)100.0f));
	float gtaY = ((float)480.0f * ((float)real_prcY / (float)100.0f));
	
	//Log("radar screenX: %f screenY: %f bgPosX: %f bgPosY: %f", screenWidth, screenHeight, CRadar::radarBgPos1.x1, CRadar::radarBgPos1.y1);


	CRadar::radarPos.x1 = gtaX;
	CRadar::radarPos.y1 = gtaY;
	
	ImGui::End();
}
static uint32_t time2;
bool isPlus = false;
void CGUI::DrawTextureInfo(uintptr_t pEntity) {
	ENTITY_TYPE* sEntity = (ENTITY_TYPE*)pEntity;
	if(!sEntity->mat) {
		return;
	}

	uint16_t usEntityID = *(uint16_t*)(pEntity + 34);
	uintptr_t* pTexturesInfo = (uintptr_t*)(g_libGTASA + 0x6706E4);
	uintptr_t pTextureInfo = (uintptr_t)pTexturesInfo[usEntityID];

//	if(pTextureInfo) 
	//{
		char szTextureBufferInfo[0xFF] = { '\0' };

		VECTOR TagPos;
		TagPos.X = sEntity->mat->pos.X;
		TagPos.Y = sEntity->mat->pos.Y;
		TagPos.Z = sEntity->mat->pos.Z;
		TagPos.Z += 0.25f + (EntityGetDistanceFromCamera(sEntity) * 0.0475f);
		sprintf(szTextureBufferInfo, "ID: %d", usEntityID);
		std::string s(szTextureBufferInfo);
		std::string from("4095");
		std::string to("4");
		std::string repl = ReplaceAll(s, from, to);
		strcpy(szTextureBufferInfo, repl.c_str());

		
	
		VECTOR Out;

		// CSprite::CalcScreenCoors
		((void (*)(VECTOR*, VECTOR*, float*, float*, bool, bool))(SA_ADDR(0x54EEC0 + 1)))(&TagPos, &Out, 0, 0, 0, 0);
	
		//if (Out.Z < 1.0f) {
	//	//	return;
	//	}
	
		ImVec2 pos = ImVec2(Out.X, Out.Y);
		RenderText(pos, 0xFFFFFFFF, true, szTextureBufferInfo);
	//}
}
void CGUI::Render()
{
	PreProcessInput();

                  //RenderGUI();

	ProcessPushedTextdraws();
	if (pChatWindow)
	{
		pChatWindow->ProcessPushedCommands();
	}

	ImGui_ImplRenderWare_NewFrame();
	ImGui::NewFrame();	
	ImGuiIO& io = ImGui::GetIO();
	static const ImWchar ranges[] = 
	{
		0x0020, 0x0080,
		0x00A0, 0x00C0,
		0x0400, 0x0460,
		0x0490, 0x04A0,
		0x2010, 0x2040,
		0x20A0, 0x20B0,
		0x2110, 0x2130,
		0
	};
	
	
	
	//CreateUp(io.DisplaySize.x / 30, io.DisplaySize.y / 28, 220.0f, 220.0f, "radar_bg");
//m_pFont = io.Fonts->AddFontFromFileTTF(path, m_fFontSize, nullptr, ranges);
	//RenderVersion();
                 
                  
	
	
		
	if (pKeyBoard)
	{
		pKeyBoard->ProcessInputCommands();
	}

	if (pPlayerTags) pPlayerTags->Render();
	
	if(pNetGame && pNetGame->GetLabelPool())
	{
		pNetGame->GetLabelPool()->Draw();
	}
if(bShowTexDebugger) {
		uint32_t ms_nNoOfVisibleEntities = *(uint32_t *)(g_libGTASA + 0x8C162C);
	//	if (!ms_nNoOfVisibleEntities) {
	//		return;
	//	}
		if(ms_nNoOfVisibleEntities){
		uintptr_t* ms_aVisibleEntityPtrs = (uintptr_t*)(g_libGTASA + 0x8C0680);
		
		do {
			uintptr_t m_pEntityPointer = (uintptr_t)ms_aVisibleEntityPtrs[ms_nNoOfVisibleEntities];
			if(m_pEntityPointer) {
				DrawTextureInfo(m_pEntityPointer);
			}
			--ms_nNoOfVisibleEntities;
		} while(ms_nNoOfVisibleEntities);
		}
	}
	if (pChatWindow) pChatWindow->Render();
/*	if(pGame) CGUI::ShowSpeed();*/
	if (pScoreBoard) pScoreBoard->Draw();
	if (pKeyBoard) pKeyBoard->Render();
	if (pDialogWindow) pDialogWindow->Render();
	
	
	
	
	

	//RwTexture* texture = (RwTexture*)LoadTextureFromDB("samp", "remapbody0");
//ImGui::SetCursorPos(ImVec2(0, ImGui::GetIO().DisplaySize.y - ImGui::GetTextLineHeightWithSpacing()));
//ImVec2 imageSize(100, 100);
//ImGui::Image(texture, imageSize);

	
	
	CDebugInfo::Draw();
	RenderPosition();
	if(pChatWindow->showinghud){
		InitHudLogo();
		
		if (!pGame || !pNetGame || !pGame->FindPlayerPed()->IsInVehicle()) {
		//g_pJavaWrapper->HideSpeed();
		bMeliage =0;
		m_fuel = 0;
		speedometershow = false;
		}
		else if (pGame->FindPlayerPed()->IsAPassenger()) {
			//g_pJavaWrapper->HideSpeed();
				bMeliage =0;
			m_fuel = 0;
			speedometershow = false;
		}
		else if (pChatWindow->vehp == true){
			//return;
			speedometershow = false;
		}
		else{
			InitHudSpeedometer();
			speedometershow = true;
		}
		InitHud();
		InitButtonPassenger();
		}
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplRenderWare_RenderDrawData(ImGui::GetDrawData());

	PostProcessInput();
	//if(!pChatWindow->vehp){
	//time2 = GetTickCount();
//	static uint32_t time = GetTickCount();
//	if (GetTickCount() - time >= 700)
//	{
	//	updateLEDs();
		
	//	time = GetTickCount();
	//}
	//}
}

bool CGUI::OnTouchEvent(int type, bool multi, int x, int y)
{
	if(!pKeyBoard->OnTouchEvent(type, multi, x, y)) return false;

	if (!pScoreBoard->OnTouchEvent(type, multi, x, y)) return false;
	
	
	if(isTouchOnButton(ImVec2(x, y), posCurClick[0], posSizeClick[0])) {
		CVehicle *pVehicle = nullptr;
	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
	VEHICLEID id = pVehiclePool->FindIDFromGtaPtr(pPlayerPed->GetGtaVehicle());
	pVehicle = pVehiclePool->GetAt(id);
	if(pVehicle){
		switch(type)
	{
		case TOUCH_PUSH:
		{
			m_bMouseDown = true;
			m_bNeedClearMousePos = false;
			bRightLEDOn = true;
			bLeftLEDOn = false;
			bCenterLEDOn = false;
			bLeftButtonPressed = false;
			bRightButtonPressed = !bRightButtonPressed;
			bCenterButtonPressed = false;
		//	time2 = GetTickCount();
			pVehicle->m_iTurnState = CVehicle::eTurnState::TURN_OFF;
			if(bRightButtonPressed){
			pVehicle->m_iTurnState = CVehicle::eTurnState::TURN_RIGHT;
		}else{
			pVehicle->m_iTurnState = CVehicle::eTurnState::TURN_OFF;
		}
			break;
		}

		case TOUCH_POP:
		{
			m_bMouseDown = false;
			m_bNeedClearMousePos = true;
		/*	bRightLEDOn = true;
		bLeftLEDOn = false;
		bLeftButtonPressed = false;
        bRightButtonPressed = !bRightButtonPressed;
        bCenterButtonPressed = false;*/
			break;
	}}
		
	return false;}
	}
	if(isTouchOnButton(ImVec2(x, y), posCurClick[1], posSizeClick[1])) {
		CVehicle *pVehicle = nullptr;
	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
	VEHICLEID id = pVehiclePool->FindIDFromGtaPtr(pPlayerPed->GetGtaVehicle());
	pVehicle = pVehiclePool->GetAt(id);
	if(pVehicle){
		switch(type)
	{
		case TOUCH_PUSH:
		{
			m_bMouseDown = true;
			m_bNeedClearMousePos = false;
		bLeftLEDOn = true;
		bRightLEDOn = false;
		bCenterLEDOn = false;
		bLeftButtonPressed = !bLeftButtonPressed;
        bRightButtonPressed = false;
        bCenterButtonPressed = false;
	//	time2 = GetTickCount();
		pVehicle->m_iTurnState = CVehicle::eTurnState::TURN_OFF;
		if(bLeftButtonPressed){
			pVehicle->m_iTurnState = CVehicle::eTurnState::TURN_LEFT;
		}else{
			pVehicle->m_iTurnState = CVehicle::eTurnState::TURN_OFF;
		}
			break;
		}

		case TOUCH_POP:
		{
			m_bMouseDown = false;
			m_bNeedClearMousePos = true;
		/*bLeftLEDOn = true;
		bRightLEDOn = false;
		bLeftButtonPressed = !bLeftButtonPressed;
        bRightButtonPressed = false;
        bCenterButtonPressed = false;*/
			break;
	}}
	return false;}
	}
	if(isTouchOnButton(ImVec2(x, y), posCurClick[2], posSizeClick[2])) {
		CVehicle *pVehicle = nullptr;
	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
	VEHICLEID id = pVehiclePool->FindIDFromGtaPtr(pPlayerPed->GetGtaVehicle());
	pVehicle = pVehiclePool->GetAt(id);
	if(pVehicle){
		switch(type)
	{
		case TOUCH_PUSH:
		{
			m_bMouseDown = true;
			m_bNeedClearMousePos = false;
		bLeftLEDOn = true;
		bRightLEDOn = true;
		bCenterLEDOn = true;
		bCenterButtonPressed = !bCenterButtonPressed;
        bRightButtonPressed = false;
        bLeftButtonPressed = false;
	//	time2 = GetTickCount();
		pVehicle->m_iTurnState = CVehicle::eTurnState::TURN_OFF;
		if(bCenterButtonPressed){
			pVehicle->m_iTurnState = CVehicle::eTurnState::TURN_ALL;
		}else{
			pVehicle->m_iTurnState = CVehicle::eTurnState::TURN_OFF;
		}
		
			break;
		}

		case TOUCH_POP:
		{
			m_bMouseDown = false;
			m_bNeedClearMousePos = true;
		/*bLeftLEDOn = true;
		bRightLEDOn = false;
		bLeftButtonPressed = !bLeftButtonPressed;
        bRightButtonPressed = false;
        bCenterButtonPressed = false;*/
			break;
	}}
	return false;}
	}
	
	

	bool bFalse = true;
	if (pNetGame)
	{
		if (pNetGame->GetTextDrawPool()->OnTouchEvent(type, multi, x, y))
		{
			if (!pChatWindow->OnTouchEvent(type, multi, x, y)) return false;
		}
		else
		{
			bFalse = false;
		}
	}

	switch(type)
	{
		case TOUCH_PUSH:
		{
			m_vTouchPos = ImVec2(x, y);
			m_bMouseDown = true;
			m_bNeedClearMousePos = false;
			break;
		}

		case TOUCH_POP:
		{
			m_bMouseDown = false;
			m_bNeedClearMousePos = true;
			break;
		}

		case TOUCH_MOVE:
		{
			m_bNeedClearMousePos = false;
			m_vTouchPos = ImVec2(x, y);
			break;
		}
	}
	if (!bFalse)
	{
		return false;
	}
	return true;
}


void CGUI::RenderPosition()
{
    ImGuiIO& io = ImGui::GetIO();
float size_font = 23.0f;
    ImVec2 _ImVec2 = ImVec2(2.0f, io.DisplaySize.y - size_font);

    char text[256];
	sprintf(text, "DYNAMIC MOBILE | Last build: %s",  __DATE__);
	ImVec2 posCur = _ImVec2;
	float iOffset = 1.0f;
	char* text_begin = text;
	char* text_end = nullptr;
	
	posCur.x -= iOffset;
			ImGui::GetBackgroundDrawList()->AddText(fontmoney, size_font, posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
			posCur.x += iOffset;
			// right 
			posCur.x += iOffset;
			ImGui::GetBackgroundDrawList()->AddText(fontmoney, size_font, posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
			posCur.x -= iOffset;
			// above
			posCur.y -= iOffset;
			ImGui::GetBackgroundDrawList()->AddText(fontmoney, size_font, posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
			posCur.y += iOffset;
			// below
			posCur.y += iOffset;
			ImGui::GetBackgroundDrawList()->AddText(fontmoney, size_font, posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
			posCur.y -= iOffset;

    // Преобразование времени в строку соответствующего формата
  //  std::ostringstream oss;
   // oss << "Native build: " << std::put_time(currentTM, "%b %d %Y %H:%M:%S") << " | Hash: (хеш)";

   // NULL, 0.0f, pos, col, text_begin, text_end
	ImGui::GetBackgroundDrawList()->AddText(fontmoney, size_font, posCur, ImColor(255, 255, 255, 255), text_begin, text_end);
   // RenderText(_ImVec2, ImColor(255, 255, 255, 255), true, text, nullptr);
	
	
}

void CGUI::RenderGUI()
{
		CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
		if (pVehiclePool)
		{
			VEHICLEID ClosetVehicleID = pVehiclePool->FindNearestToLocalPlayerPed();
			if (ClosetVehicleID < MAX_VEHICLES && pVehiclePool->GetSlotState(ClosetVehicleID))
			{
				CVehicle* pVehicle = pVehiclePool->GetAt(ClosetVehicleID);
				if (pVehicle)
				{
					if (pVehicle->GetDistanceFromLocalPlayerPed() < 7.0f)
					{
						CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
						if (pPlayerPool)
						{
							CLocalPlayer* pLocalPlayer;
							if (!pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsInVehicle() && !pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsAPassenger())
							{
								//if (ImGui::Button("G", vecButSize))
							//	{
									if (pNetGame)
									{
										if (pPlayerPool)
										{
											pLocalPlayer = pPlayerPool->GetLocalPlayer();
											if (pLocalPlayer)
											{
												pLocalPlayer->HandlePassengerEntryEx();
											}
										}
									}
								//}
							}
						}
					}
				}
			}
		}
		
}

void CGUI::RenderServer()
{
	if (pNetGame)
	{
		if (pVoice && g_IsVoiceServer())
		{
			if (pVoice->IsRecording() && GetTickCount() - g_uiLastTickVoice >= 20000)
			{
				char buf[64];
				sprintf(&buf[0], "%d", (int)((30000 - (GetTickCount() - g_uiLastTickVoice)) / 1000) + 1);
				ImVec2 test(ScaleX(pSettings->GetReadOnly().fButtonMicrophoneX + pSettings->GetReadOnly().fButtonMicrophoneSize / 2.0f) - ImGui::CalcTextSize(&buf[0]).x / 2.0f, ScaleY(g_fMicrophoneButtonPosY) - GetFontSize() * 2.6f);
				//RenderText(test, 0xFF0000FF, true, &buf[0]);
			}
			ImVec2 centre(ScaleX(35.0f), ScaleY(35.0f));
			if (pVoice->IsDisconnected())
			{
				ImGui::GetBackgroundDrawList()->AddCircleFilled(centre, 18.0f, ImColor(1.0f, 0.0f, 0.0f));
			}
			if (pVoice->GetNetworkState() == VOICECHAT_CONNECTING || pVoice->GetNetworkState() == VOICECHAT_WAIT_CONNECT)
			{
				ImGui::GetBackgroundDrawList()->AddCircleFilled(centre, 18.0f, ImColor(1.0f, 1.0f, 0.0f));
			}
			if (pVoice->GetNetworkState() == VOICECHAT_CONNECTED)
			{
				ImGui::GetBackgroundDrawList()->AddCircleFilled(centre, 18.0f, ImColor(0.0f, 1.0f, 0.0f));
			}
		}
	}
}


void CGUI::RenderVersion()
{
     ImVec2 verpos = ImVec2(ScaleX(10), ScaleY(10));
	RenderText(verpos, ImColor(0xFFFFFFFF), true, "v1.5.2"); 
}

void CGUI::ProcessPushedTextdraws()
{
	BUFFERED_COMMAND_TEXTDRAW* pCmd = nullptr;
	while (pCmd = m_BufferedCommandTextdraws.ReadLock())
	{
		RakNet::BitStream bs;
		bs.Write(pCmd->textdrawId);
		pNetGame->GetRakClient()->RPC(&RPC_ClickTextDraw, &bs, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, 0);
		m_BufferedCommandTextdraws.ReadUnlock();
	}
}

void CGUI::RenderRakNetStatistics()
{

}

extern uint32_t g_uiBorderedText;
void CGUI::RenderTextForChatWindow(ImVec2& posCur, ImU32 col, bool bOutline, const char* text_begin, const char* text_end)
{
	int iOffset = pSettings->GetReadOnly().iFontOutline;


	ImColor colOutline = ImColor(IM_COL32_BLACK);
	ImColor colDef = ImColor(col);
	colOutline.Value.w = colDef.Value.w;
float offsetX = 5.0f;
    float offsetY = 5.0f; 
    float width = 1100.0f; 
    float height = GetFontSize() + (offsetY * 2); 
	if (bOutline)
	{
		if (g_uiBorderedText)
		{
			posCur.x -= iOffset;
			ImGui::GetBackgroundDrawList()->AddText(posCur, colOutline, text_begin, text_end);
			posCur.x += iOffset;
			// right 
			posCur.x += iOffset;
			ImGui::GetBackgroundDrawList()->AddText(posCur, colOutline, text_begin, text_end);
			posCur.x -= iOffset;
			// above
			posCur.y -= iOffset;
			ImGui::GetBackgroundDrawList()->AddText(posCur, colOutline, text_begin, text_end);
			posCur.y += iOffset;
			// below
			posCur.y += iOffset;
			ImGui::GetBackgroundDrawList()->AddText(posCur, colOutline, text_begin, text_end);
			posCur.y -= iOffset;
		}
		else
		{
			
		}
	}
	ImColor co(0.0f, 0.0f, 0.0f, 0.4f);
	if (colOutline.Value.w <= 0.4)
	{
		co.Value.w = colOutline.Value.w;
	}
	

			
	ImGui::GetBackgroundDrawList()->AddText(posCur, col, text_begin, text_end);
}

void CGUI::PushToBufferedQueueTextDrawPressed(uint16_t textdrawId)
{
	BUFFERED_COMMAND_TEXTDRAW* pCmd = m_BufferedCommandTextdraws.WriteLock();

	pCmd->textdrawId = textdrawId;

	m_BufferedCommandTextdraws.WriteUnlock();
}

void CGUI::RenderText(ImVec2& posCur, ImU32 col, bool bOutline, const char* text_begin, const char* text_end)
{
	int iOffset = pSettings->GetReadOnly().iFontOutline;

	if (bOutline)
	{
		if (g_uiBorderedText)
		{
			posCur.x -= iOffset;
			ImGui::GetBackgroundDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
			posCur.x += iOffset;
			// right 
			posCur.x += iOffset;
			ImGui::GetBackgroundDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
			posCur.x -= iOffset;
			// above
			posCur.y -= iOffset;
			ImGui::GetBackgroundDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
			posCur.y += iOffset;
			// below
			posCur.y += iOffset;
			ImGui::GetBackgroundDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
			posCur.y -= iOffset;
		}
		else
		{
			ImVec2 b(posCur.x + ImGui::CalcTextSize(text_begin, text_end).x, posCur.y + GetFontSize());
			if (m_pSplashTexture)
			{
				ImColor co(1.0f, 1.0f, 1.0f, 0.4f);
				ImGui::GetBackgroundDrawList()->AddImage((ImTextureID)m_pSplashTexture->raster, posCur, b, ImVec2(0, 0), ImVec2(1, 1), co);
			}
			else
			{
				ImColor co(0.0f, 0.0f, 0.0f, 0.4f);
				ImGui::GetBackgroundDrawList()->AddRectFilled(posCur, b, co);
			}
		}
	}

	ImGui::GetBackgroundDrawList()->AddText(posCur, col, text_begin, text_end);
}


void CGUI::SetHealth(float fhpcar){
   bHealth = static_cast<int>(fhpcar);
}

int CGUI::GetHealth(){
	return 1;//static_cast<int>(pVehicle->GetHealth());
}

void CGUI::SetDoor(int door){
	bDoor = door;
}

void CGUI::SetEngine(int engine){
	bEngine = engine;
}

void CGUI::SetLights(int lights){
	bLights = lights;
}

void CGUI::SetMeliage(float meliage){
	bMeliage = static_cast<int>(meliage);
}

void CGUI::SetEat(float eate){
	eat = static_cast<int>(eate);
}

int CGUI::GetEat(){
	return eat;
}

void CGUI::SetFuel(float fuel){
   m_fuel = static_cast<int>(fuel);
}

void CGUI::ShowSpeed(){
}
