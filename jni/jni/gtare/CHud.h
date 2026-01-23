#pragma once
#include <jni.h>

struct CGtaRect
{
public:
	float x1;           // x1
	float y1;    // y1

//	float x2;        // x2
//	float y2;          // y2
};

struct CRadarBrRect
{
public:
	float x1;           // x1
	float y1;    // y1

	float x2;        // x2
	float y2;          // y2
};
class CSprite2d;

enum eHudSprites {
    HUDSPRITE_FIST,
    HUDSPRITE_SITEM16,
    HUDSPRITE_SITEROCKET,
    HUDSPRITE_RADARDISC,
    HUDSPRITE_RADARRINGPLANE,
    HUDSPRITE_SKIPICON
};
class CHUD
{
public:
    static bool m_bCreatedCar;
    static bool m_bNewGame;
    static bool m_bShow;
    static bool m_bShowDialog;
    static bool m_bHidenNetGame;
    static RwTexture* hud_radar;

public:
    static void Initialise();

    static CGtaRect radarBgPos1; // x y
    static CGtaRect radarBgPos2; // x y
    static CGtaRect radarPos; // x y
static CSprite2d **Sprites; // static CSprite2d Sprites[6]
    static CRadarBrRect radar1; // x y

    static void Disable()      { m_bShow = false; }
    static void Enable()       { m_bShow = true; };

    static void DisableD()      { m_bShow = false; }
    static void EnableD()       { m_bShow = true; };

    static bool IsEnabled()    { return m_bShow; };
    static bool IsEnabledDialog()    { return m_bShowDialog; };

    static void EditRadarBios(CRect* rect);
    static void EditRadar(CRect* rect);
    static void Render();
    static void ShowHud(bool isShow);
	
	
};