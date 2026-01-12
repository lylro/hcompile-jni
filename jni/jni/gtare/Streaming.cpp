#include "Streaming.h"
#include "../util/patch.h"
#include "../game/scripting.h"

int CStreaming::AddImageToList(char const* pFileName, bool bNotPlayerImg) {
    // find a free slot
	Log("loading %s...", pFileName);
    std::int32_t fileIndex = 0;
    for (; fileIndex < 12; fileIndex++) {
        if (!ms_files[fileIndex].m_szName[0])
            break;
    }
    if (fileIndex == 12) 
        return 0;
    // free slot found, load the IMG file
	Log("loaded %s, free slot found!", pFileName);
    strcpy(ms_files[fileIndex].m_szName, pFileName);
    ms_files[fileIndex].m_StreamHandle = CHook::CallFunction<int32_t>(g_libGTASA + 0x00289BA0 + 1, pFileName);
    ms_files[fileIndex].m_bNotPlayerImg = bNotPlayerImg;
    return fileIndex;
}

void CStreaming::RemoveAllUnusedModels() {
    ((void (*) ())(g_libGTASA + 0x293325))();

    // Remove all possibly removable vehicles
//    for (int32 i = 0; i < MAX_VEHICLES_LOADED; i++) {
//        RemoveLoadedVehicle();
//    }

    // Remove majority of models with no refs
  
}

void CStreaming::RequestModel(int32_t index, int32_t flags)
{
	Log("Request Model: %d(id)...", index);
    ((void (*) (int32_t, int32_t))(g_libGTASA + 0x0028EB10 + 1))(index, flags);
}

void CStreaming::LoadAllRequestedModels(bool bPriorityRequestsOnly)
{
    ((void (*) (bool))(g_libGTASA + 0x00294CB4 + 1))(bPriorityRequestsOnly);
}
void decrypt(const char* encrypted_hex, const char* key, char* decrypted);
void CStreaming::InitImageList() {
    /*for (auto & ms_file : ms_files) {
        ms_file.m_szName[0] = 0;
        ms_file.m_StreamHandle = 0;
    }*/
	
	for (auto & ms_file : ms_files) {
        ms_file.m_szName[0] = 0;
        ms_file.m_StreamHandle = 0;
    }
	
	char key[] = "g8g4L7yeqM71Jd2K";
	char decrypted1[strlen("33 7D 3F 70 0E 6B 34 24 38 03 19 6B 03 34") / 3 + 2];
	char decrypted2[strlen("33 7D 3F 70 0E 6B 2A 20 32 02 79 75 64 3E 7B 1B") / 3 + 2];
	decrypt("33 7D 3F 70 0E 6B 34 24 38 03 19 6B 03 34", key, decrypted1);
	decrypt("33 7D 3F 70 0E 6B 2A 20 32 02 79 75 64 3E 7B 1B", key, decrypted2);
//	host = decrypted1;

    CStreaming::AddImageToList(decrypted1, true);
    //CStreaming::AddImageToList("TEXDB\\GTA_INT.IMG", true);
    //..CStreaming::AddImageToList("TEXDB\\SKINS.IMG", true);
    //CStreaming::AddImageToList("TEXDB\\CARS.IMG", true);
	//CStreaming::AddImageToList("TEXDB\\COLLISIONS", true);
    CStreaming::AddImageToList(decrypted2, true);
}
void (*request)(int32_t index, int32_t flags);
void request_hook(int32_t index, int32_t flags){
	CStreaming::RequestModel(index, flags);
}
void CStreaming::InjectHooks() {
    CHook::Redirect(g_libGTASA, 0x28E83C, &CStreaming::InitImageList);
	CHook::Write(g_libGTASA + 0x005CE80C, &CStreaming::ms_files);
	//CHook::SetUpHook(0x28EB10 + g_libGTASA, (uintptr_t)request_hook, (uintptr_t*)&request);
}
void CStreaming::RemoveModel(int32_t index)
{
    ((void (*) (int32_t))(g_libGTASA + 0x00290C4C + 1))(index);
}


void CStreaming::RemoveBuildingsNotInArea(int areaCode) {
    ((void (*) (int))(g_libGTASA + 0x0028FBAC + 1))(areaCode);
}