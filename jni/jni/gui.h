void SendFamilyInfoPackage(int playerid)
{
    
    std::cout << "[GUI] Отправка пакета для FamilyInfo игроку id: " << playerid << std::endl;
    // подключение гуи систем by rm
}
static cell AMX_NATIVE_CALL SendFamilyInfoPackage(AMX* amx, cell* params)
{
    int playerid = static_cast<int>(params[1]); 
    SendFamilyInfoPackage(playerid);
    return 1; 
}

void SendFamilyInfoPackage()
{
    AMX_NATIVE_INFO gui_Natives[] = {
        { "SendFamilyInfoPackage", SendFamilyInfoPackage },
        { NULL, NULL }
    };
    
     amx_Family(amx, gui_Natives, -1);
}