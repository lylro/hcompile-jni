#include "app.h"
#include "plugin.h"

// Автор: @xss3ox
// Автор: @xss3ox
// Автор: @xss3ox

#include "game/rw/rw.h"
#include "gui/gui.h"

void CApp::Initialise(eAppInit init_type)
{
	if(init_type == eAppInit::APP_INIT_OFFSETS)
	{
		COffset::Initialise();
	}
	if(init_type == eAppInit::APP_INIT_RW)
	{
		rw::Initialise();
	}
	if(init_type == eAppInit::APP_INIT_GUI)
	{
		CGUI::Initialise();
	}
}

void CApp::Render()
{
	CGUI::Render();
}

void CApp::Render2dStuff()
{
	CGUI::Render2dStuff();
}

void CApp::Process()
{
	
}

// Автор: @xss3ox

bool CApp::OnTouchEvent(int action, int pointer, int x, int y)
{
	if(!CGUI::OnTouchEvent(action, pointer, x, y)) {
		return false;
	}
	return true;
}

// Автор: @xss3ox
