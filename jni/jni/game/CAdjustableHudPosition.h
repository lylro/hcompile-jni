#pragma once
#include "font.h"
#include <string>

class CAdjustableHudPosition
{
	static CVector2D2 m_aPosition[E_HUD_ELEMENT::HUD_SIZE];
public:
	static CVector2D2 GetElementPosition(E_HUD_ELEMENT id);
	static void SetElementPosition(E_HUD_ELEMENT id, int x, int y);
};