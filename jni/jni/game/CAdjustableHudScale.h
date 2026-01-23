#pragma once
#include "font.h"
#include <string>

class CAdjustableHudScale
{
	static CVector2D2 m_aScale[E_HUD_ELEMENT::HUD_SIZE];
public:
	static CVector2D2 GetElementScale(E_HUD_ELEMENT id);
	static void SetElementScale(E_HUD_ELEMENT id, int x, int y);
};