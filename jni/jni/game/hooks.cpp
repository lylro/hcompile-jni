#include "../main.h"
#include <sstream>

#include "../util/armhook.h"
#include "../util/patch.h"
#include "RW/RenderWare.h"
#include "game.h"

#include "../net/netgame.h"
#include "../gui/gui.h"
#include "../util/CJavaWrapper.h"
#include "..//str_obfuscator_no_template.hpp"
#include "../Radar.h"
#include "../CAudioStream.h"

#include "common.h"
#include "clientlogic.h"
extern int dword_207F50;

  uint32_t dword_609F20;
  char byte_609F24;
  int dword_609F28;
  int dword_609F2C;
  int *dword_748F98;
  uint32_t unk_1AD8A1;
  uint32_t unk_61572C;
  char byte_61572E[4];
  uint32_t unk_63E3C4;
  float dword_6295CC;
 uintptr_t* dword_609F1C;
#include "../gtare/CRQ_Commands.h"
#include "../gtare/CSkyBox.h"
#include "../gtare/Streaming.h"
#include "../gtare/TextureDatabaseRuntime.h"

#include "..///..//santrope-tea-gtasa/encryption/CTinyEncrypt.h"
#include "..///..//santrope-tea-gtasa/encryption/encrypt.h"
#include "..///..//santrope-tea-gtasa/encryption/CTEA.h"
#define AES_BLOCK_SIZE 16
#include "..//voice/CVoiceChatClient.h"
#include "..//chatwindow.h"
#include "..//settings.h"
#define MAX_ENCRYPTED_TXD 5
const cryptor::string_encryptor encrArch[MAX_ENCRYPTED_TXD] = {
        cryptor::create("texdb/mobile/mobile.txt", 23),
        cryptor::create("texdb/gta3/gta3.txt", 21),
        cryptor::create("texdb/gta_int/gta_int.txt", 27),
		cryptor::create("texdb/cars/cars.txt", 27),
		cryptor::create("texdb/skins/skins.txt", 27),  
};
extern CClientLogic *pClientLogic;
extern CGame* pGame;
extern CSettings* pSettings;
extern CChatWindow* pChatWindow;
extern CVoiceChatClient* pVoice;
extern BULLET_DATA* 	g_pCurrentBulletData;
extern CAudioStream* pAudioStream;
extern CPlayerPed* 		g_pCurrentFiredPed;
RwRaster* g_WaterDuDvTex;
RwRaster* dword_748C98;
RwRaster* dword_748C94;
RwRaster* dword_748C9C;
RwRaster* dword_748C90;
RwRaster* dword_748C8C;
RwRaster* dword_748C88;
uintptr_t* get_CMirrors_reflBuffer()
{
    return (uintptr_t*)dword_748C98;
}
char* currentFilePath;
uintptr_t* sub_6F030()
{
    return (uintptr_t*)dword_748C90;
}
extern "C"
{
#include "..//..//santrope-tea-gtasa/encryption/aes.h"
}

void InitCTX(AES_ctx& ctx, const uint8_t* pKey)
{
    uint8_t key[16];
    memcpy(&key[0], &pKey[0], 16);
    for (int i = 0; i < 16; i++)
    {
        key[i] = XOR_UNOBFUSCATE(key[i]);
    }
    uint8_t iv[16];
    memcpy(&iv[0], &g_iIV, 16);
    for (int i = 0; i < 16; i++)
    {
        iv[i] = XOR_UNOBFUSCATE(iv[i]);
    }
    AES_init_ctx_iv(&ctx, &key[0], &iv[0]);
}
bool isEncrypted(const char *szArch)
{
 //   if(g_bIsTestMode)return false;
	//return false;
    for (int i = 0; i < MAX_ENCRYPTED_TXD; i++)
    {
        if (!strcmp(encrArch[i].decrypt(), szArch))
            return true;
    }
    return false;
}
void EncryptFile10(const char* filename)
{
	FILE* inputFile = fopen(filename, "rb");
    if (!inputFile) {
        fprintf(stderr, "Error: Cannot open input file.\n");
        return;
    }

    // Создайте выходной файл с расширением "Lock"
    char outputFilename[260]; // Достаточно для большинства путей
    sprintf(outputFilename, "%sLock", filename);
    
    FILE* outputFile = fopen(outputFilename, "wb");
    if (!outputFile) {
        fclose(inputFile);
        fprintf(stderr, "Error: Cannot open output file.\n");
        return;
    }

    AES_ctx ctx;
    InitCTX(ctx, &g_iEncryptionKeyTXD[6]);

    uint8_t buffer[PART_SIZE];
    size_t bytesRead;

    // Читаем и шифруем данные поблочно
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), inputFile)) > 0) {
        AES_CBC_encrypt_buffer(&ctx, buffer, bytesRead);
        fwrite(buffer, 1, bytesRead, outputFile);
    }

    fclose(inputFile);
    fclose(outputFile);
}
void EncryptFile(const char* filename)
{
    FILE* file = fopen(filename, "rb");
    if (!file)
        return;

    fseek(file, 0, SEEK_END);
    int fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    uint8_t* buffer = (uint8_t*)malloc(fileSize);
    if (!buffer)
    {
        fclose(file);
        return;
    }

    fread(buffer, 1, fileSize, file);
    fclose(file);

    AES_ctx ctx;
    InitCTX(ctx, &g_iEncryptionKeyTXD[6]);

    int iters = fileSize / PART_SIZE_TXD;
    uintptr_t pointer = (uintptr_t)buffer;
    for (int i = 0; i < iters; i++)
    {
        AES_CBC_encrypt_buffer(&ctx, (uint8_t*)pointer, PART_SIZE_TXD);
        pointer += PART_SIZE_TXD;
    }
	char pon[0xFF] = {0};
		sprintf(pon, "%sLock", filename);
    file = fopen(pon, "wb");
    if (file)
    {
        fwrite(buffer, 1, fileSize, file);
        fclose(file);
    }

    free(buffer);
}
void DecryptFile(const char* filename)
{
    FILE* file = fopen(filename, "rb");
    if (!file)
        return;

    fseek(file, 0, SEEK_END);
    int fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    uint8_t* buffer = (uint8_t*)malloc(fileSize);
    if (!buffer)
    {
        fclose(file);
        return;
    }

    fread(buffer, 1, fileSize, file);
    fclose(file);

    AES_ctx ctx;
    InitCTX(ctx, &g_iEncryptionKeyTXD[6]);

    int iters = fileSize / PART_SIZE_TXD;
    uintptr_t pointer = (uintptr_t)buffer;
    for (int i = 0; i < iters; i++)
    {
        AES_CBC_decrypt_buffer(&ctx, (uint8_t*)pointer, PART_SIZE_TXD);
        pointer += PART_SIZE_TXD;
    }
	char pon[0xFF] = {0};
		sprintf(pon, "%sunlock", filename);
    file = fopen(pon, "wb");
    if (file)
    {
        fwrite(buffer, 1, fileSize, file);
        fclose(file);
    }

    free(buffer);
}
uintptr_t* getRasterExtOffset()
{
    return /*(uintptr_t*)*/*(uintptr_t**)(g_libGTASA + 0x9B4CD0);
}

int  setRasterExtOffset(uintptr_t* a1, uintptr_t a2)
{
    //(*(void (*)(void*, float, int))(g_libGTASA + 0x197CC4 + 1))(*(void* *)((*(uintptr_t *)(g_libGTASA + 0x611844)) + (*(uintptr_t*)a1)), *(float*)(g_libGTASA + 0x5D2228), 1);
    //((void (*)(uintptr_t, uintptr_t))(((*(uintptr_t *)(g_libGTASA + 0x611844)) + (*(uintptr_t*)a2)) + 24))(((*(uintptr_t *)(g_libGTASA + 0x611844)) + (*(uintptr_t*)a2)), 1);
    //((int (*)(uintptr_t, uintptr_t, uintptr_t))(((*(uintptr_t *)(g_libGTASA + 0x611844)) + (*(uintptr_t*)a2)) + 12))(((*(uintptr_t *)(g_libGTASA + 0x611844)) + (*(uintptr_t*)a2)), 0, 0);

    void* v1; // r4

    v1 = *(void* *)((*(uintptr_t *)(g_libGTASA + 0x611844)) + (*(uintptr_t*)a1));
    (*(void ( **)(void *, int))(*(uintptr_t *)v1 + 24))(v1, 1);
    return (*(int ( **)(void *, uintptr_t, uintptr_t))(*(uintptr_t *)v1 + 12))(v1, 0, 0);
}

#include "..//game/WaterShader.h"
uintptr_t* g_WaterShaderClass = nullptr;
void (*emu_glEndInternal)();
void emu_glEndInternal_hook()
{
    if ( ((*(uintptr_t *)(g_libGTASA + 0x61572C)) & 0x80000) != 0 ) {
        if (g_WaterShaderClass == nullptr) {
            g_WaterShaderClass = (uintptr_t*) malloc(64);
            memset(g_WaterShaderClass, 1, 64);
            WaterShader::BuildShadersSource1(g_WaterShaderClass);
        }
        if(g_WaterShaderClass){
            WaterShader::EmuShader__Select3(g_WaterShaderClass);
        }

    }
    
    emu_glEndInternal();
}

#include <cstdint>

// Замена для &unk_1B29E5 - это просто числовой адрес
constexpr uintptr_t unk_1AD6B9 = 0x1AD6B9;
constexpr uintptr_t unk_1B0649 = 0x1B0649;
constexpr uintptr_t unk_1B023D = 0x1B023D;
constexpr uintptr_t unk_1AF339 = 0x1AF339;
constexpr uintptr_t unk_1AF74D = 0x1AF74D;
constexpr uintptr_t unk_1AFBB1 = 0x1AFBB1;
constexpr uintptr_t unk_1B1B4D = 0x1B1B4D;
constexpr uintptr_t unk_1B2559 = 0x1B2559;
constexpr uintptr_t unk_1E0EA1 = 0x1E0EA1;
constexpr uintptr_t unk_1AEE2D = 0x1AEE2D;
constexpr uintptr_t unk_1E284D = 0x1E284D;
constexpr uintptr_t locret_19A448 = 0x19A448;
constexpr uintptr_t unk_1B92D5 = 0x1B92D5;
constexpr uintptr_t unk_1B29E5 = 0x1B29E5;
constexpr uintptr_t unk_1D6CED = 0x1D6CED;

// Неизвестная функция, вероятно, просто выводит строку


// Замена для uintptr_t - это просто uintptr_t
uintptr_t dword_7BEE74;
uintptr_t dword_7BEE7C;
uintptr_t dword_7BEE78;
uintptr_t dword_7BEE80;
uintptr_t dword_7BEE84;
uintptr_t dword_7BEE88;
uintptr_t dword_7BEE8C;
uintptr_t dword_7BEE90;
uintptr_t dword_7BEE94;
uintptr_t dword_7BEE98;
uintptr_t dword_7BEE9C;
uintptr_t dword_7BEEA0;
uintptr_t dword_7BEEA4;
uintptr_t dword_7BEEA8;
uintptr_t dword_7BEEAC;
uintptr_t dword_7BEEB0;
uintptr_t dword_7BEEB4;
uintptr_t dword_7BEEB8;
uintptr_t dword_7BEEBC;
uintptr_t dword_7BEEC0;
uintptr_t dword_7BEEC4;
uintptr_t dword_7BEEC8;
uintptr_t dword_7BEECC;
uintptr_t dword_7BEED0;
uintptr_t dword_7BEED4;
uintptr_t dword_7BEED8;
uintptr_t dword_7BEEDC;
uintptr_t dword_7BEEE0;
uintptr_t dword_7BEEE4;
uintptr_t dword_7BEEE8;
uintptr_t dword_7BEEEC;
uintptr_t dword_7BEEF0;
uintptr_t dword_7BEEF8;
uintptr_t dword_7BEEF4;
uintptr_t dword_7BEEFC;
uintptr_t dword_7BEF00;
uintptr_t dword_7BEF04;
uintptr_t dword_7BEF08;
uintptr_t dword_7BEF0C;
uintptr_t dword_7BEF10;
uintptr_t dword_7BEF14;
uintptr_t dword_7BEF18;
uintptr_t dword_7BEF20;
uintptr_t dword_7BEF1C;
uintptr_t dword_7BEF24;
uintptr_t dword_7BEF28;
uintptr_t dword_7BEF2C;
uintptr_t dword_7BEF34;
uintptr_t dword_7BEF30;
uintptr_t dword_7BEF3C;
uintptr_t dword_7BEF40;
uintptr_t dword_7BEF48;
uintptr_t dword_7BEF44;
uintptr_t dword_7BEF54;
uintptr_t dword_7BEF50;
uintptr_t dword_7BEF58;
uintptr_t dword_7BEF4C;
uintptr_t dword_7BEF5C;
static uint32_t* dword_207F24 = nullptr;
uintptr_t dword_7BEF60;
extern char* PLAYERS_REALLOC;
void sub_6FA50(int *a1, unsigned int a2){}
void sub_6F21C(int *a1){}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define LOG_BUFFER_SIZE 1024

void decrypt(const char* encrypted_hex, const char* key, char* decrypted) {
    int key_len = strlen(key);
    int encrypted_len = strlen(encrypted_hex) / 3; 
    
    char* encrypted = (char*)calloc(encrypted_len + 1, sizeof(char));
    
    for (int i = 0, j = 0; i < strlen(encrypted_hex); i += 3, j++) {
        char hex_byte[3] = { encrypted_hex[i], encrypted_hex[i + 1], '\0' };
        encrypted[j] = (char)strtol(hex_byte, NULL, 16);
    }
    
    for (int i = 0; i < encrypted_len + 1; i++) {
        decrypted[i] = encrypted[i] ^ key[i % key_len];
    }
    
    decrypted[encrypted_len + 1] = '\0';
    
    free(encrypted);
}
char* formatByteString(const char* data, size_t len) {
    static char buffer[1024];
    buffer[0] = '\0';
    for (size_t i = 0; i < len; i++) {
        sprintf(buffer + strlen(buffer), "%02X ", (unsigned char)data[i]);
    }
    return buffer;
}
int mainDec() {
    /*char key[] = "waViwbFnv936593mvSQvidDh35fjeKlvb3855278cvF46";
    
    const char* encrypted_data1 = "2E 16 3E 4B 17 05 0C 18 22 02 11 45 5F 5C 47 56 22 0E 7D 02 02 1F ";
    const char* encrypted_data2 = "E6 82 66 40 1A D0 22 47 3F C0 C4 2E 61";
    const char* encrypted_data3 = "B6 5C 7B B9 0F E5 72 0B 21 FB 3B BD EE 94 90 4E";
    
	char data1[] = "weikton, rashifroval? ti gay napisi mne a ls sto ya lox(";
    char data2[] = "/data/data/com.dynamicmobilenew.game/lib/libbass.so";
    char data3[] = "/storage/emulated/0/DynamicMobileNew/files/";

    char encrypted1[strlen(data1) + 1];
    char encrypted2[strlen(data2) + 1];
    char encrypted3[strlen(data3) + 1];
	

    encrypt(data1, key, encrypted1);
    Log("Encrypted data 1 (byte string): %s", formatByteString(encrypted1, strlen(encrypted1)));
    

    encrypt(data2, key, encrypted2);
    Log("Encrypted data 2 (byte string): %s", formatByteString(encrypted2, strlen(encrypted2)));
    

    encrypt(data3, key, encrypted3);
    Log("Encrypted data 3 (byte string): %s", formatByteString(encrypted3, strlen(encrypted3)));
    

	char decrypted1[strlen(encrypted_data1) / 3 + 2];
    char decrypted2[strlen(encrypted_data2) / 3 + 2];
    char decrypted3[strlen(encrypted_data3) / 3 + 2];
    
    decrypt(encrypted_data1, key, decrypted1);
    Log("Decrypted data 1: %s", decrypted1);
	
	decrypt(encrypted_data2, key, decrypted2);
    Log("Decrypted data 2: %s", decrypted2);
    
    decrypt(encrypted_data3, key, decrypted3);
    Log("Decrypted data 3: %s", decrypted3);
    */
    return 0;
	
}

void sub_16A674() {}
void j_j_j__ZdlPv(void* arg) {}
int sub_3CD04(int arg1, const char* arg2) { return 0; }

// Пустые переменные для неизвестных переменных
int _stack_chk_guard = 0;
int byte_629564 = 0;
int dword_7E7E40 = 0;
int dword_47A14 = 0;
int dword_200520 = 0;



int sub_3CAE4(int arg) { return 0; }
void *__fastcall sub_18F804(int a1, int a2, int a3)
{
  //return a1;
}
int *__fastcall sub_18F8B4(int *a1, int *a2)
{
  return 0;
}
int *__fastcall sub_4DCC8(int *a1, int *a2, int *a3)
{
 
  return 0;
}
void sub_47AC4(const char*** a1, int *a2) {

}

//LoadCdDirectory from br
int (*dword_629574)(const char *a1, int a2);
int sub_46FE8(const char *a1, int a2)
{
  char *v4; // r10
  unsigned int v5; // r4
  int v6; // r5
  char *v7; // r0
  char *v8; // r11
  int v9; // r1
  int v10; // r0
  const char **v11; // r9
  int v12; // r5
  unsigned int v13; // r4
  char *v14; // r0
  char *v15; // r9
  int v16; // r1
  int v17; // r0
  const char **v18; // r6
  const char *v19; // r5
  const char *v20; // r11
  int v21; // r0
  int v22; // r1
  int v23; // r0
  char *v24; // r4
  int v25; // r10
  FILE *v26; // r0
  FILE *v27; // r9
  unsigned int v28; // r5
  unsigned int v29; // r4
  unsigned int v30; // r5
  unsigned int v31; // r10
  char *v32; // r0
  unsigned int *v33; // r0
  unsigned int v34; // t1
  int v35; // r4
  char *v36; // r0
  char *v37; // r0
  uintptr_t *v38; // r0
  int v39; // r0
  uintptr_t *v40; // r0
  unsigned int v41; // r1
  unsigned int v42; // r2
  void **v43; // r0
  void **v44; // r2
  unsigned int v45; // r3
  void **v46; // r4
  const char **v47; // r4
  int v48; // r6
  unsigned int v49; // r10
  const char *v50; // r4
  const char *v51; // r8
  int v52; // r0
  int v53; // r1
  int v54; // r0
  int v55; // r4
  FILE *v56; // r0
  FILE *v57; // r6
  unsigned int v58; // r4
  unsigned int v59; // r4
  unsigned int v60; // r4
  float v61; // s0
  char *v62; // r0
  unsigned int *v63; // r0
  unsigned int v64; // t1
  int v65; // r4
  char *v66; // r0
  char *v67; // r0
  uintptr_t *v68; // r0
  uintptr_t *v69; // r0
  unsigned int v70; // r1
  unsigned int v71; // r2
  void **v72; // r0
  void **v73; // r2
  unsigned int v74; // r3
  void **v75; // r4
  int v76; // r6
  unsigned int *v77; // r12
  int v78; // r1
  unsigned int *v79; // r12
  int v80; // r1
  const char *v81; // r1
  void *v82; // r0
  int *v83; // r0
  unsigned int *v84; // r12
  int v85; // r1
  unsigned int *v86; // r12
  int v87; // r1
  const char *v88; // r1
  void *v89; // r0
  int *v90; // r0
  char *v92; // [sp+4h] [bp-16Ch]
  int v93; // [sp+Ch] [bp-164h]
  int v94; // [sp+Ch] [bp-164h]
  int v95; // [sp+14h] [bp-15Ch]
  int v96; // [sp+14h] [bp-15Ch]
  char v97[4]; // [sp+18h] [bp-158h] BYREF
  char v98[4]; // [sp+1Ch] [bp-154h] BYREF
  int v99; // [sp+20h] [bp-150h] BYREF
  void *v100; // [sp+24h] [bp-14Ch] BYREF
  void **v101; // [sp+28h] [bp-148h]
  const char **v102; // [sp+2Ch] [bp-144h] BYREF
  const char **v103; // [sp+30h] [bp-140h]
  char v104[260]; // [sp+38h] [bp-138h] BYREF
  int v105; // [sp+13Ch] [bp-34h]

  Log("SET LOADING ARCHIVE %d %s", a2, a1);
  dword_200520 = a2;
  dword_629574(a1, a2);
  dword_200520 = 0xFFFFFFFF;
  Log("LOAD ZALUPA %s %d", a1, a2);
  v4 = (char*)0x28E6C9;
  if ( a2 == 1 )
  {
    v12 = 0;
    v13 = 0;
    v14 = (char*)(0x670488 + g_libGTASA + 0x257);
    v15 = (char*)(0x670488 + g_libGTASA + 0x250);
    v16 = 0x66CC;
    do
    {
      if ( *(uintptr_t *)(v14 + 1) >= v13 && *v14 == 1 )
      {
        v12 = *(uintptr_t *)(v14 + 5);
        v13 = *(uintptr_t *)(v14 + 1);
      }
      v14 += 0x14;
      --v16;
    }
    while ( v16 );
    v17 = sub_3CAE4(1);
  //  sub_47AC4(&v102, &v17);
    v18 = v102;
    v101 = &v100;
    v100 = &v100;
    v96 = (int)v103;
	
    if ( v102 == v103 )
    {
LABEL_40:
      v40 = (uintptr_t*)(v15 + 0x20);
      v41 = 0xFFFFFFFF;
      v42 = 0;
      do
      {
        ++v42;
        if ( *v40 && *((uint16_t *)v40 + 0xFFFFFFFC) == 0xFFFF )
          v41 = v42;
        v40 += 5;
      }
      while ( v42 != 0x66CB );
      v43 = (void **)v100;
      if ( &v100 != v100 )
      {
        v44 = (void **)v100;
        do
        {
          v45 = (unsigned int)v44[2];
          *(char *)&v15[0x14 * v41 + 4] = v45;
          v41 = v45;
          v44 = (void **)*v44;
        }
        while ( &v100 != v44 );
      }
      if ( v43 != &v100 )
      {
        do
        {
          v46 = (void **)*v43;
          operator delete(v43);
          v43 = v46;
        }
        while ( v46 != &v100 );
      }
      v47 = v102;
      v48 = (int)v103;
      if ( v102 == v103 )
        goto LABEL_130;
      do
      {
        v88 = v47[1];
        v89 = (void *)(v88 + 0xFFFFFFF4);
        if ( v88 + 0xFFFFFFF4 != (const char *)&dword_7E7E40 )
        {
          v84 = (unsigned int *)(v88 + 0xFFFFFFFC);
        }
        v90 = (int *)(*v47 + 0xFFFFFFF4);
        if ( v90 != &dword_7E7E40 )
        {
          v86 = (unsigned int *)(*v47 + 0xFFFFFFFC);
        }
        v47 += 6;
      }
      while ( v47 != (const char **)v48 );
      goto LABEL_129;
    }
    v94 = v12 + v13;
    v92 = v15;
    while ( 1 )
    {
      v19 = *v18;
      v20 = &v19[strlen(*v18) + 1];
      do
        v21 = *((uint8_t *)v20-- + 0xFFFFFFFE);
      while ( v21 != 0x2F );
      memset(v104, 0, 0xFFu);
      strcpy(v104, v20);
      *(uintptr_t *)strchr(v104, 0x2E) = 0;
      v99 = 0;
      if ( strstr(v20, ".dff") )
      {
        Log("Trying to search %s", v104);
        if ( ((int (__fastcall *)(char *, int *))(g_libGTASA + 0x336111))(v104, &v99) )
        {
          v22 = v99;
LABEL_25:
          Log("result %d %s", v22, *v18);
          v24 = v4;
          byte_629564 = 1;
          v25 = ((int (__fastcall *)(char *, char *, char *))&v4[g_libGTASA])(&v15[0x14 * v99], v98, v97);
          v26 = fopen(*v18, (const char *)&dword_47A14);
          v27 = v26;
          if ( v25 )
          {
            v4 = v24;
            if ( v26 )
            {
              fseek(v26, 0, 2);
              v28 = ftell(v27);
              fclose(v27);
              v29 = v28 >> 0xB;
            }
            else
            {
              v29 = 0x2000;
            }
            v15 = v92;
            Log("size: %d hash %s name %d", *(uintptr_t *)&v92[0x14 * v99 + 0xC], *v18, v18[2]);
            v32 = &v92[0x14 * v99];
            v34 = *((uintptr_t *)v32 + 3);
            v33 = (unsigned int *)(v32 + 0xC);
            if ( v29 <= v34 )
              *v33 = v29;
          }
          else
          {
            if ( v26 )
            {
              fseek(v26, 0, 2);
              v30 = ftell(v27);
              fclose(v27);
              v31 = (v30 >> 0xB) + 1;
              v15 = v92;
            }
            else
            {
              Log("ERRO FILE NOT FOUND %s", *v18);
              v15 = v92;
              v31 = 0x2000;
            }
            Log("Trying to add model %s %s", v20, v104);
            v35 = v99;
            v36 = &v15[0x14 * v99];
            *((uintptr_t *)v36 + 3) = v31;
            *((uintptr_t *)v36 + 2) = v94;
            if ( *(uintptr_t *)(0x670488 + g_libGTASA) < v31 )
            {
              *(uintptr_t *)(0x670488 + g_libGTASA) = v31 + 1;
              Log("Extended streaming buffer to %u", v31 + 1);
              v35 = v99;
            }
            v37 = &v15[0x14 * v35];
            v37[7] = 1;
            v37[6] = 0;
            *((char *)v37 + 2) = 0xFFFF;
            v38 = (uintptr_t *)operator new(0xCu);
            v38[1] = 0;
            *v38 = 0;
            v38[2] = v35;
            sub_16A674();
            v39 = v94 + v31;
            v4 = (char*)0x28E6C9;
            v94 = v39;
          }
          byte_629564 = 0;
          goto LABEL_39;
        }
      }
      else if ( strstr(v20, ".col") )
      {
        Log("Trying to search %s", v104);
        v23 = sub_3CD04(1, v104);
        if ( v23 != 0xFFFFFFFF )
        {
          v22 = v23 + 0x61A8;
          v99 = v23 + 0x61A8;
          goto LABEL_25;
        }
      }
LABEL_39:
      v18 += 6;
      if ( v18 == (const char **)v96 )
        goto LABEL_40;
    }
  }
  if ( a2 )
    return _stack_chk_guard - v105;
  v5 = 0;
  v6 = 0;
  v7 = (char*)(0x670488 + g_libGTASA + 0x257);
  v8 = (char*)(0x670488 + g_libGTASA + 0x250);
  v9 = (int)(0x66CC);
  do
  {
    if ( *(uintptr_t *)(v7 + 1) >= v5 && !*v7 )
    {
      v6 = *(uintptr_t *)(v7 + 5);
      v5 = *(uintptr_t *)(v7 + 1);
    }
    v7 += 0x14;
    --v9;
  }
  while ( v9 );
  v10 = sub_3CAE4(0);
 // sub_47AC4(&v102, &v10);
  v11 = v102;
  v101 = &v100;
  v100 = &v100;
  v95 = (int)v103;
  if ( v102 != v103 )
  {
    v93 = v5 + v6;
    while ( 1 )
    {
      v50 = *v11;
      v51 = &v50[strlen(*v11) + 1];
      do
        v52 = *((uint8_t *)v51-- + 0xFFFFFFFE);
      while ( v52 != 0x2F );
      memset(v104, 0, 0xFFu);
      strcpy(v104, v51);
      *(uintptr_t *)strchr(v104, 0x2E) = 0;
      v99 = 0;
      if ( !strstr(v51, ".dff") )
        break;
      Log("Trying to search %s", v104);
      if ( ((int (__fastcall *)(char *, int *))(g_libGTASA + 0x336111))(v104, &v99) )
      {
        v53 = v99;
        goto LABEL_61;
      }
LABEL_77:
      v11 += 6;
      if ( v11 == (const char **)v95 )
        goto LABEL_78;
    }
    if ( !strstr(v51, ".col") )
      goto LABEL_77;
    Log("Trying to search %s", v104);
    v54 = sub_3CD04(0, v104);
    if ( v54 == 0xFFFFFFFF )
      goto LABEL_77;
    v53 = v54 + 0x61A8;
    v99 = v54 + 0x61A8;
LABEL_61:
    Log("result %d %s", v53, *v11);
    byte_629564 = 1;
    v55 = ((int (__fastcall *)(char *, char *, char *))(0x28E6C9 + g_libGTASA))(&v8[0x14 * v99], v98, v97);
    v56 = fopen(*v11, (const char *)&dword_47A14);
    v57 = v56;
    if ( v55 )
    {
      if ( v56 )
      {
        fseek(v56, 0, 2);
        v58 = ftell(v57);
        fclose(v57);
        v59 = v58 >> 0xB;
      }
      else
      {
        v59 = 0x2000;
      }
      Log("size: %d hash %s name %d", *(uintptr_t *)&v8[0x14 * v99 + 0xC], *v11, v11[2]);
      v62 = &v8[0x14 * v99];
      v64 = *((uintptr_t *)v62 + 3);
      v63 = (unsigned int *)(v62 + 0xC);
      if ( v59 <= v64 )
        *v63 = v59;
LABEL_76:
      byte_629564 = 0;
      goto LABEL_77;
    }
    if ( v56 )
    {
      fseek(v56, 0, 2);
      v60 = ftell(v57);
      fclose(v57);
      v61 = (float)((v60 >> 0xB) + 1) * 1.5;
    }
    else
    {
      if ( !v11[3] )
      {
        v49 = 0x2000;
LABEL_73:
        Log("Trying to add model %s %s %d", v51, v104, v49);
        v65 = v99;
        v66 = &v8[0x14 * v99];
        *((uintptr_t *)v66 + 3) = v49;
        *((uintptr_t *)v66 + 2) = v93;
        if ( *(uintptr_t *)(0x670488 + g_libGTASA) < v49 )
        {
          *(uintptr_t *)(0x670488 + g_libGTASA) = v49 + 1;
          Log("Extended streaming buffer to %u", v49 + 1);
          v65 = v99;
        }
        v67 = &v8[0x14 * v65];
        *((char *)v67 + 2) = 0xFFFF;
        *((char *)v67 + 3) = 0;
        v68 = (uintptr_t *)operator new(0xCu);
        v68[1] = 0;
        *v68 = 0;
        v68[2] = v65;
        sub_16A674();
        v93 += v49;
        goto LABEL_76;
      }
      v61 = (float)(unsigned int)v11[4] * 1.4;
    }
    v49 = (unsigned int)v61;
    goto LABEL_73;
  }
LABEL_78:
  v69 = (uintptr_t*)(v8 + 0x20);
  v70 = 0xFFFFFFFF;
  v71 = 0;
  do
  {
    ++v71;
    if ( *v69 && *((uint16_t *)v69 + 0xFFFFFFFC) == 0xFFFF )
      v70 = v71;
    v69 += 5;
  }
  while ( v71 != 0x66CB );
  v72 = (void **)v100;
  if ( &v100 != v100 )
  {
    v73 = (void **)v100;
    do
    {
      v74 = (unsigned int)v73[2];
      *(char *)&v8[0x14 * v70 + 4] = v74;
      v70 = v74;
      v73 = (void **)*v73;
    }
    while ( &v100 != v73 );
  }
  if ( v72 != &v100 )
  {
    do
    {
      v75 = (void **)*v72;
      operator delete(v72);
      v72 = v75;
    }
    while ( v75 != &v100 );
  }
  v47 = v102;
  v76 = (int)v103;
  if ( v102 != v103 )
  {
    do
    {
      v81 = v47[1];
      v82 = (void *)(v81 + 0xFFFFFFF4);
      if ( v81 + 0xFFFFFFF4 != (const char *)&dword_7E7E40 )
      {
        v77 = (unsigned int *)(v81 + 0xFFFFFFFC);
      //  if ( &pthread_create )
      //  {
       //   __dmb(0xBu);
       //   do
       //     v78 = __ldrex(v77);
       //   while ( __strex(v78 - 1, v77) );
       //   __dmb(0xBu);
      //  }
     //   else
      //  {
      //    v78 = (*v77)--;
      //  }
       // if ( v78 <= 0 )
      //    j_j_j__ZdlPv(v82);
      }
      v83 = (int *)(*v47 + 0xFFFFFFF4);
      if ( v83 != &dword_7E7E40 )
      {
        v79 = (unsigned int *)(*v47 + 0xFFFFFFFC);
     //   if ( &pthread_create )
     //   {
        //  __dmb(0xBu);
        //  do
          //  v80 = __ldrex(v79);
        //  while ( __strex(v80 - 1, v79) );
        //  __dmb(0xBu);
     //   }
     //   else
     //   {
      //    v80 = (*v79)--;
      //  }
       // if ( v80 <= 0 )
        //  j_j_j__ZdlPv(v83);
      }
      v47 += 6;
    }
    while ( v47 != (const char **)v76 );
LABEL_129:
    v47 = v102;
  }
LABEL_130:
  if ( v47 )
    operator delete(v47);
  return v105;
}


















char* sub_8F3D0()
{
  char* result;

  Log("Initializing RenderWare br..");

 //mainDec();
uintptr_t(*dword_7BEE7C)(uintptr_t) = reinterpret_cast<uintptr_t(*)(uintptr_t)>((char*)0x1AD6B9 + g_libGTASA);
uintptr_t(*dword_7BEE78)(uintptr_t) = reinterpret_cast<uintptr_t(*)(uintptr_t)>((char*)&unk_1AD6B9 + g_libGTASA + 0x10);
uintptr_t(*dword_7BEE80)(uintptr_t, uintptr_t, uintptr_t) = reinterpret_cast<uintptr_t(*)(uintptr_t, uintptr_t, uintptr_t)>((char*)&unk_1AD6B9 + g_libGTASA + 0x20C);
uintptr_t(*dword_7BEE84)(uintptr_t, uintptr_t, uintptr_t, uintptr_t) = reinterpret_cast<uintptr_t(*)(uintptr_t, uintptr_t, uintptr_t, uintptr_t)>((char*)&unk_1B0649 + g_libGTASA + 0x130);
  dword_7BEE88 = unk_1B023D + g_libGTASA + 0x360;
  dword_7BEE8C = unk_1B023D + g_libGTASA + 0x224;
  dword_7BEE90 = unk_1B0649 + g_libGTASA + 0x6C;
  dword_7BEE94 = unk_1B0649 + g_libGTASA + 0xDC;
  dword_7BEE98 = unk_1B023D + g_libGTASA + 0x2C4;
  dword_7BEE9C = unk_1B023D + g_libGTASA + 0x310;
  dword_7BEEA0 = unk_1B023D + g_libGTASA + 0x204;
  dword_7BEEA4 = unk_1B023D + g_libGTASA + 0x3A8;
  dword_7BEEA8 = unk_1B0649 + g_libGTASA + 0x2C;
  dword_7BEEAC = unk_1B023D + g_libGTASA + 0x1D8;
  dword_7BEEB0 = unk_1B023D + g_libGTASA + 0x25C;
  dword_7BEEB4 = unk_1B023D + g_libGTASA + 0x2F0;
  dword_7BEEB8 = unk_1B0649 + g_libGTASA + 0xA8;
  dword_7BEEBC = unk_1B0649 + g_libGTASA + 0x1CC;
 // dword_748F98 = (char*)(g_pszStorage);
  dword_7BEEC0 = unk_1B023D + g_libGTASA + 0x238;
  
  dword_7BEEC4 = unk_1B0649 + g_libGTASA;
  dword_7BEEC8 = unk_1B023D + g_libGTASA + 0x33C;
  dword_7BEECC = unk_1AF339 + g_libGTASA;
  dword_7BEED0 = unk_1AF339 + g_libGTASA + 0x114;
  dword_7BEED4 = unk_1AF339 + g_libGTASA + 0x54;
  dword_7BEED8 = unk_1AF339 + g_libGTASA + 0xE8;
  dword_7BEEDC = unk_1AF74D + g_libGTASA + 0x304;
  dword_7BEEE0 = unk_1AF339 + g_libGTASA + 0x158;
  dword_7BEEE4 = unk_1AFBB1 + g_libGTASA;
  dword_7BEEE8 = unk_1AF339 + g_libGTASA + 0x294;
  dword_7BEEEC = unk_1AFBB1 + g_libGTASA + 0x148;
  dword_7BEEF0 = unk_1AF74D + g_libGTASA;
  dword_7BEEF8 = unk_1B023D + g_libGTASA;
  dword_7BEEF4 = unk_1AF74D + g_libGTASA + 0x234;
  dword_7BEEFC = unk_1B023D + g_libGTASA + 0x24;
  dword_7BEF00 = unk_1B023D + g_libGTASA + 0x120;
  dword_7BEF04 = unk_1B023D + g_libGTASA + 0x190;
  dword_7BEF08 = unk_1B023D + g_libGTASA + 0x48;
  dword_7BEF0C = unk_1B1B4D + g_libGTASA;
  dword_7BEF10 = unk_1B1B4D + g_libGTASA + 0x68;
  dword_7BEF14 = unk_1B2559 + g_libGTASA;
  dword_7BEF18 = unk_1E0EA1 + g_libGTASA;
  dword_7BEF20 = unk_1AEE2D + g_libGTASA;
  dword_7BEF1C = unk_1E284D + g_libGTASA;
  dword_7BEF24 = locret_19A448 + g_libGTASA;
  dword_7BEF28 = unk_1B92D5 + g_libGTASA;
  dword_7BEF2C = g_libGTASA + 0x1B8039;
  dword_7BEF34 = g_libGTASA + 0x1B80A9;
  dword_7BEF30 = g_libGTASA + 0x1B8055;
  dword_7BEF3C = g_libGTASA + 0x1B80C5;
  dword_7BEF40 = g_libGTASA + 0x1B80E1;
  dword_7BEF48 = g_libGTASA + 0x1B8119;
  dword_7BEF44 = g_libGTASA + 0x1B80FD;
  dword_7BEF54 = unk_1B29E5 + g_libGTASA;
  dword_7BEF50 = unk_1B29E5 + g_libGTASA + 0x84;
  dword_7BEF58 = unk_1D6CED + g_libGTASA;

  result = reinterpret_cast<char*>(unk_1B0649 + g_libGTASA + 0x11C0);
  dword_7BEF4C = unk_1B29E5 + g_libGTASA + 0xB8;
  dword_7BEF5C = unk_1D6CED + g_libGTASA + 0x298;
  dword_7BEF60 = reinterpret_cast<uintptr_t>(result);
  
/*	char key[] = "g8g4L7yeqM71Jd2K";
	char data1[] = "TEXDB\\SECOND.ZIP";
	char data2[] = "TEXDB\\MAIN.ZIP";
	char data3[] = "if(isSky(vec4(95.0/255.0,144.0/255.0,188.0/255.0,1.0))||isSky(vec4(125.0/255.0,152.0/255.0,212.0/255.0,1.0))||isSky(vec4(164.0/255.0,110.0/255.0,91.0/255.0,1.0))||isSky(vec4(20.0/255.0,21.0/255.0,28.0/255.0,1.0))){gl_FragColor.rgb = diffuseColor.rgb;}";
	char encrypted1[strlen(data1) + 1];
	char encrypted2[strlen(data2) + 1];
	char encrypted3[strlen(data3) + 1];
	encrypt(data1, key, encrypted1);
	encrypt(data2, key, encrypted2);
	encrypt(data3, key, encrypted3);
	Log("TEXDB\\SECOND.ZIP: %s", formatByteString(encrypted1, strlen(encrypted1)));
	Log("TEXDB\\MAIN.ZIP: %s", formatByteString(encrypted2, strlen(encrypted2)));
	Log("issky: %s", formatByteString(encrypted3, strlen(encrypted3)));
	*/
	
  
  return result;
}




extern void sub_55718(int *param1, uint8_t a2, uint8_t a3, uint16_t a4, int a5)
{
	param1[1] = a3;
	*param1 = a2;
	param1[2] = a4;
	param1[3] = a5;
}
extern void sub_5578E(int *a1){
	a1[2] | (a1[1] << 8) | (*a1 << 0x10) | (a1[3] << 0x18);
}

uintptr_t sub_57DE0(){
	return (uintptr_t)PLAYERS_REALLOC;
}
#include <cstdint>

constexpr uintptr_t off_1F95D4 = 0x1F95D4;
constexpr uintptr_t unk_1D4EC2 = 0x1D4EC2;
constexpr uintptr_t unk_1D50BA = 0x1D50BA;

// Неизвестные функции, их нужно заменить на реальные реализации


void sub_57F64(int a1, uintptr_t a2)
{
    // TODO: реализовать sub_57F64
}

void sub_55814(uintptr_t a1, uintptr_t a2, int a3)
{
    // TODO: реализовать sub_55814
}

bool sub_57DF0(uintptr_t a1)
{
    // TODO: реализовать sub_57DF0
    return (bool)((int (*)(void))(g_libGTASA + 0x41DD7D))();
}

// Замена для __fastcall - это просто передача аргументов по значению
uintptr_t* sub_5212C(uintptr_t* a1)
{
  *reinterpret_cast<uintptr_t*>(a1 + 8) = 1;
  *reinterpret_cast<uintptr_t*>(a1) = off_1F95D4;
  *reinterpret_cast<uintptr_t*>(a1 + 0xC) = sub_57DE0();
  *reinterpret_cast<uintptr_t*>(a1 + 4) = sub_57DE0();

  uintptr_t v2 = *reinterpret_cast<uintptr_t*>(a1 + 0xC);
  *reinterpret_cast<char*>(a1 + 0x10) = 0;

  sub_57F64(0, v2);
  sub_55814(unk_1D4EC2, *reinterpret_cast<uintptr_t*>(a1 + 8), 0);

  if (*reinterpret_cast<uintptr_t*>(a1 + 0xC) && sub_57DF0(*reinterpret_cast<uintptr_t*>(a1 + 8)))
    sub_55814(unk_1D50BA, *reinterpret_cast<uintptr_t*>(a1 + 8), 1);

  *reinterpret_cast<uintptr_t*>(a1 + 0x11) = 0;
  *reinterpret_cast<uint16_t*>(a1 + 0x1D) = 1;
  *reinterpret_cast<uintptr_t*>(a1 + 0x15) = 0;
  *reinterpret_cast<uintptr_t*>(a1 + 0x19) = 0;
  *reinterpret_cast<uintptr_t*>(a1 + 0x1F) = 0x3C03126F;

  memset(reinterpret_cast<void*>(a1 + 0x50), 0, 0x23Au);

  reinterpret_cast<void(*)(uintptr_t)>(g_libGTASA + 0x43E201)(*reinterpret_cast<uintptr_t*>(a1 + 0xC));
  reinterpret_cast<void(*)(uintptr_t)>(g_libGTASA + 0x43E229)(*reinterpret_cast<uintptr_t*>(a1 + 0xC));

  return a1;
}

void dword_7BEF38(int a1, int a2)
{
    return ((void ( *)(int, int))((char *)0x1B8071 + g_libGTASA))(a1, a2);
}


void sub_6F706(char *a1)
{
    char *v2; // r0
  char *v3; // r0
  int v4; // r5
  int v5; // t1
  int v6; // r0
  int v7; // r0
  int v8; // r1

  v2 = (char *)a1[8];
  if ( v2 == (char *)a1[9] )
  {
    v4 = *(int *)(*(int *)(a1[0xB] - 4) + 0x1FC);
    operator delete(v2);
    v6 = a1[0xB];
    a1[0xB] = v6 - 4;
    v7 = *(int *)(v6 - 4);
    a1[9] = v7;
    v8 = v7 + 0x200;
    v3 = (char *)(v7 + 0x1FC);
    a1[0xA] = v8;
  }
  else
  {
    v5 = *((int *)v2 + 0xFFFFFFFF);
    v3 = v2 + 0xFFFFFFFC;
    v4 = v5;
  }
  a1[8] = *v3;
  (*(void (__fastcall **)(int))(*(int *)v4 + 0x14))(v4);
 // return v4;
}
int nullsub_16(float* a1)
{
    //stub
    return 0;
}

void sub_55754(int a1, int a2)
{
    //pysto br
}

void sub_57158(int a1, int a2, int a3, int a4, int a5, float* a6)
{
    ((int (*)(void))(g_libGTASA + 0x5525F9))(); // sprite2d draw
}

void sub_6FD48()
{
    //pysto br
}

void sub_18CD74() {
}

void sub_18D394() {
}

void sub_6F580(uintptr_t* a1, int a2) {
  int* v4; // r5
  char* v5; // r0
  int result; // r0
  char* v7; // r10
  unsigned int v8; // r2
  unsigned int v9; // r1
  unsigned int v10; // r6
  char* v11; // r11
  char* v12; // r8
  char* v13; // r9
  char* v14; // r9

  // if ( &pthread_create && pthread_mutex_lock((pthread_mutex_t *)&unk_748F9C) )
  // .. sub_18D394();

  v4 = reinterpret_cast<int*>(a1 + 0x10);
  v5 = reinterpret_cast<char*>(a1[0xF]); 

  if (v5 == reinterpret_cast<char*>(a1[0x10])) {
    v7 = reinterpret_cast<char*>(a1[0xE]);
    v8 = (v5 - v7) >> 2;
    if (!v8)
      v8 = 1;
    v9 = v8 + ((v5 - v7) >> 2);
    v10 = v9;
    if (v9 >> 0x1E)
      v10 = 0x3FFFFFFF;
    if (v9 < v8)
      v10 = 0x3FFFFFFF;

    if (v10) {
      if (v10 >= 0x40000000)
        sub_18CD74(); // Вызов функции обработки ошибки
      v11 = reinterpret_cast<char*>(new char[v10]); 
      v7 = reinterpret_cast<char*>(a1[0xE]);
      v12 = v11;
      v5 = reinterpret_cast<char*>(a1[0xF]); 
    } else {
      v11 = nullptr;
      v12 = nullptr;
    }

    *reinterpret_cast<int*>(&v12[v5 - v7]) = a2; 
    v13 = &v12[v5 - v7];
    if ((v5 - v7) >> 2)
      memmove(v11, v7, v5 - v7);

    v14 = v13 + 4;
    if (v7)
      delete[] v7; // Освобождение памяти
    result = reinterpret_cast<int>(&v12[4 * v10]); 
    a1[0xF] = reinterpret_cast<int>(v14);
    a1[0xE] = reinterpret_cast<int>(v11);
  } else {
    *reinterpret_cast<int*>(v5) = a2;
    v4 = reinterpret_cast<int*>(a1 + 0xF);
    result = a1[0xF] + 4;
  }
  *v4 = result;
 //.. if (&pthread_create)
 // .  result = pthread_mutex_unlock(reinterpret_cast<pthread_mutex_t*>(&unk_748F9C)); 
}
int dword_61E00C;
int byte_61E010;
void sub_6FE14()
{
    //pysto br
}

int sub_584E8(unsigned int a1)
{
  int v1; // r0
  int result; // r0
  Log("sub_584E8");
  v1 = *(int *)(g_libGTASA + 4 * a1 + 0x87BF48);
  Log("sub_584E8 2");
  if ( a1 <= 0x4E20 && (v1) != 0 ){
	char buffer[32]; 
	sprintf(buffer, "0x%X", (int)(v1)); 
	Log("sub_584E8 3 %s", buffer);
	  
    //result = (*(int (__fastcall **)(int))(v1 + 0x2C))(v1)/*(int)CSkyBox::GetSkyObject()*/;
	
	Log("sub_584E8 4");
  }
  else
  {
	Log("sub_584E8 5");
	result = 0;
  }

  return result;
}
int sub_43C40(int a1, int a2)
{
  dword_61E00C = a2;
  byte_61E010 = (int)(0x6706DE + 0x14 * a2 + g_libGTASA);
 Log("sub_43c40");
  return ((int ( *)(int, int))(g_libGTASA + 0x28EB11))(a2, 2);
}
int *sub_43C88()
{
  int *result; // r0
Log("sub_43c88");
  ((void (__fastcall *)(int))(g_libGTASA + 0x294CB5))(0);
  if ( dword_61E00C && (byte_61E010 & 2) == 0 )
    ((void (*)(void))(g_libGTASA + 0x2917BD))();
  result = &dword_61E00C;
  dword_61E00C = 0;
  byte_61E010 = 0;
  return result;
}
int sub_3F570(int a1, int a2)
{
	//return (int)CSkyBox::GetSkyObject();
  int v3; // r6
  int v4; // r0
  int v5; // r5

  //if ( ScriptCommand(dword_207F24) )
  //{
  //  v3 = 0;
 // }
 // else
 // {
    sub_43C40(0, a2);
   // sub_43C88();
    v3 = 1;
 // }
 Log("sub_3f470 1");
 // v4 = sub_584E8(a2);
  Log("sub_3f470 2");
  v5 = v4;
  if ( !v4 )
  {
    Log("no model %d", a2);
    return 0;
  }
  if ( !*(int *)(v4 + 4) )
  {
    Log("Bad parent in skybox");
    return 0;
  }
 // if ( v3 == 1 )
 //   nullsub_8(dword_207F24, a2, 0);
  return v5;
}
void sub_6FB40(int a1, int a2, int a3) {
  int* v5; // r1
  int v6; // r0
  unsigned int v7; // r6
  int v8; // r8
  int v9; // r12
  unsigned int v10; // r3
  int v11; // r0
  int* v12; // r5
  int v13; // r2
  int* v14; // r0
  unsigned int v15; // r0
  unsigned int v16; // r6
  unsigned int v17; // r10
  int v18; // r9
  void* v19; // r1
  int v20; // r0
  int v21; // r1
  int v22; // r0
  int result; // r0

  v5 = reinterpret_cast<int*>(a1 + 0x14); 
  v6 = *reinterpret_cast<int*>(a1 + 0x24);
  v7 = *reinterpret_cast<int*>(a1 + 4); 
  v8 = v6 - reinterpret_cast<int>(v5); 
  v9 = ((v6 - reinterpret_cast<int>(v5)) >> 2) + 1;
  v10 = v9 + a2;
  if (v7 <= 2 * (v9 + a2)) {
    v15 = *reinterpret_cast<int*>(a1 + 4); 
    if (v7 < a2)
      v15 = a2;
    v16 = v15 + v7 + 2;
    if (v16 >= 0x40000000)
      sub_18CD74(); // Вызов функции обработки ошибки

    if (!a3)
      a2 = 0;

    v17 = v16 - v10;
    v18 = reinterpret_cast<int>(new int[v16]); // Выделение памяти
    v19 = reinterpret_cast<void*>(a1 + 0x14);
    v12 = reinterpret_cast<int*>(((2 * v17) & 0xFFFFFFFC) + v18 + 4 * a2);
    v20 = *reinterpret_cast<int*>(a1 + 0x24) + 4;

    if (reinterpret_cast<void*>(v20) != v19) {
      // Копирование данных из старой области в новую
      memmove(v12, v19, v20 - reinterpret_cast<int>(v19)); 
    }

    // Освобождение старой области памяти
    delete[] reinterpret_cast<int*>(*reinterpret_cast<int*>(a1)); 
    *reinterpret_cast<int*>(a1 + 4) = v16;
    *reinterpret_cast<int*>(a1) = v18;
  } else {
    v11 = v6 + 4;
    if (!a3)
      a2 = 0;

    v12 = reinterpret_cast<int*>(((2 * (v7 - v10)) & 0xFFFFFFFC) + *reinterpret_cast<int*>(a1) + 4 * a2);
    v13 = v11 - reinterpret_cast<int>(v5);
    if (v12 >= v5) {
      if (v13 >> 2) {
        v14 = &v12[v9 - (v13 >> 2)]; 
        goto LABEL_17;
      }
    } else if (v13) {
      v14 = v12;
    LABEL_17:
      memmove(v14, v5, v13);
      goto LABEL_18;
    }
  }
LABEL_18:
  *reinterpret_cast<int*>(a1 + 0x14) = reinterpret_cast<int>(v12);
  v21 = *v12;
  *reinterpret_cast<int*>(a1 + 0xC) = *v12;
//  *reinterpret_cast<int*>(a1 + 0x24) = &v12[v8 >> 2];
  *reinterpret_cast<int*>(a1 + 0x10) = v21 + 0x200;
  v22 = v12[v8 >> 2];
  *reinterpret_cast<int*>(a1 + 0x1C) = v22;
  result = v22 + 0x200;
  *reinterpret_cast<int*>(a1 + 0x20) = result;
}
void sub_6F6A0(uintptr_t* a1, int a2)
{
  int v4; // r1
  int* v5; // r0
  int v6; // r0
  int v7; // r0

  v4 = a1[0xA];
  v5 = reinterpret_cast<int*>(a1[8]); // reinterpret_cast для приведения к типу int*
  if (v5 == reinterpret_cast<int*>(v4 - 4)) {
    if ((unsigned int)(a1[3] - ((a1[0xB] - a1[2]) >> 2)) <= 1) {
      sub_6FB40((int)a1 + 2, 1, 0);
    }

    // operator new в C++
   // reinterpret_cast<int*>(a1[0xB] + 4) = new int[0x200]; // Замена operator new(0x200u) 
    *v5 = a2;
    v6 = a1[0xB];
    a1[0xB] = v6 + 4;
    v7 = *reinterpret_cast<int*>(v6 + 4); // Приведение к int* перед обращением по адресу
    a1[9] = v7;
    a1[8] = v7;
    a1[0xA] = v7 + 0x200;
  } else {
    *v5 = a2;
    a1[8] += 4;
  }

  // Вызов функции по адресу, указанному в a2 + 0x10
   reinterpret_cast<int(__fastcall *)(int)>(*reinterpret_cast<int*>(a2 + 0x10))(a2); 

}

void sub_6EC24(int a1, int a2)
{
    int v1; // r4

  v1 = *(int *)(*(int *)(0x611844 + g_libGTASA) + a1);
  (*(void (__fastcall **)(int))(*(int *)v1 + 0x18))(v1);
   (*(int (__fastcall **)(int, int, int))(*(int *)v1 + 0xC))(v1, 0, 0);
}

void sub_58518(uintptr_t* result) {
  if (result) {
    uintptr_t v1 = *result;

    if (v1 == 2) {
      uintptr_t function_address = 0x1E0E61 + g_libGTASA; 

      *result = reinterpret_cast<uintptr_t (*)(void)>(function_address)(); 
    } else if (v1 == 1) {
      uintptr_t function_address = result[0x12]; 

      *result = reinterpret_cast<uintptr_t (*)(void)>(function_address)(); 
    }
  }
}

   
extern void sub_3F614(uintptr_t* a1, int a2)
{
  uint32_t* v4; // r5
  int v5; // r6
  uint32_t* v6; // r8
  int v7; // r6
  uint32_t* v8; // r1
  uint32_t* v9; // r0
  bool v10; // zf
  int v11; // r2
  bool v12; // zf
  int v13; // r4
  int v14; // r5
  int v15; // r0
  char v16; // r0
  uint32_t* v17; // r0
  int v18; // r0
  int v19; // r6
  float v20; // s0
  int v21; // r0
  float v22; // s4
  void (__fastcall ***v23)(uint32_t, uint32_t); // r6
  void (__fastcall **v24)(uint32_t, uint32_t); // r0
  void (__fastcall ***v25)(uint32_t, uint32_t); // r6
  void (__fastcall **v26)(uint32_t, uint32_t); // r0
  int* v27; // r1
  int v28; // r1
  int v29; // r0
  int v30; // r0
  uint32_t* v31; // r0
  int v33; // [sp+10h] [bp-48h] BYREF
  int v34[3]; // [sp+14h] [bp-44h] BYREF
  int64_t v35; // [sp+20h] [bp-38h] BYREF
  int v36; // [sp+28h] [bp-30h]
  float v37; // [sp+2Ch] [bp-2Ch] BYREF
  float v38; // [sp+30h] [bp-28h]
  float v39; // [sp+34h] [bp-24h]
  int v40; // [sp+38h] [bp-20h]

  // нету
 

  v4 = dword_207F24;
  if ( dword_207F24 )
  {
    if ( !*dword_207F24 )
    {
    //  v5 = operator new(0x28Au);
      // нету
      sub_5212C(a1);
      *v4 = v5;
    }
//	Log("testcam 3");
    dword_7BEF38(1, 0);
    dword_7BEF38(6, 0);
    dword_7BEF38(8, 0);
    dword_7BEF38(0xC, 0);
    dword_7BEF38(0xA, 5);
    dword_7BEF38(0xB, 6);
    dword_7BEF38(0xE, 0);
    dword_7BEF38(0x14, 1);
	//Log("testcam 4");
    v6 = (uint32_t*)(0x1AD8A1);
//	Log("testcam 44");
    v7 = *(uint32_t*)((char*)g_libGTASA + 0x95B064);
//	Log("testcam 5");
    v33 = 0xFFFFFFFF;
    if ( a2 )
      v7 = a2;
 // Log("testcam 6");
    ((void (__fastcall *)(int, int*, int))(0x1AD8A1 + g_libGTASA))(v7, &v33, 2);
    ((void (__fastcall *)(int, int*, int))(0x1AD8A1 + g_libGTASA))(v7, &v33, 3);
	//Log("testcam 7");
 ///   v8 = (uint32_t*)(a1 + 0x40);
  ///  v9 = (uint32_t*)(a1 + 4 * *v8);
    v10 = v9 == 0;
	//Log("testcam 8");
    if ( v9 )
    {
     /// v8 = (uint32_t*)(a1 + 4 * *v8);
    ///  v6 = (uint32_t*)(v8[4]);
      v10 = v6 == 0;
    }
//	Log("testcam 9");
    if ( !v10 )
    {
      v11 = v8[8];
      v12 = v11 == 0;
      if ( v11 )
        v12 = v8[0xC] == 0;
      if ( !v12 )
      {
	//	  Log("testcam 10");
        if ( !a2 )
          v6 = v9;
        v13 = g_libGTASA;
        v14 = *(uint32_t*)(0x61572C + g_libGTASA);
        v15 = g_libGTASA;
        if ( (v14 & 0x800000) != 0 )
        {

          *(uint32_t*)(0x61572C + g_libGTASA) = v14 & 0xF77FFFFF;
        //  v16 = byte_609F24;
        //  __dmb(0xBu);
	//	Log("testcam 11");
          if ( (v16 & 1) == 0 /*&& j___cxa_guard_acquire((__guard*)&byte_609F24)*/ )
          {
            v17 = (uint32_t*)(::operator new(4u));
            *v17 = 0;
          //  dword_609F20 = (int)(v17);
          //  j___cxa_guard_release((__guard*)&byte_609F24);
          }
         // *(uint32_t*)dword_609F20 = *(uint32_t*)(a1 + 4 * *(uint32_t*)(a1 + 0x40) + 0x30);
     //     v18 = nullsub_16(&v37);
     //     sub_55754(v18, 0xFFFFFFFF);
     //     sub_57158(dword_609F20, 0, 0, 0x44000000, 0x44000000, &v37);
          *(uint32_t*)((char*)0x61572C + v13) |= v14 & 0x8000000 | 0x800000;
          v15 = g_libGTASA;
		//  Log("testcam 12");
        }
      //  v19 = /*v6[1]*/(int)CSkyBox::GetSkyObject();
	//	Log("testcam 12 0");
      //  v38 = (float)((float)((float)*(unsigned int*)(0x63E3C4 + v15) + 150.0) / 150.0) * 0.8;// scale sky
	//	Log("testcam 12 1");
        v37 = v38;
	//	Log("testcam 12 2 %f", v37);
     ///   v39 = v38;
     ///   ((void ( *)(int, float*, uint32_t))(0x1AED7D + v15/* + 0x18*/))(v19, &v37, 0);
		//Log("testcam 12 3");
      ///  v36 = 0x3F800000;
      ///  v35 = 0LL;
	//	Log("testcam 13");
    //    ((void (__fastcall *)(int, int64_t*, uint32_t, int))(0x1AED7D + g_libGTASA + 0x48))( //rotate sky
     //     v19,
     //     &v35,
     //     *(float*)&dword_6295CC * 360.0,
      //    1);
     //   v34[0] = *(uint32_t*)((char*)g_libGTASA + 0x30);
    //    v34[1] = *(uint32_t*)((char*)g_libGTASA + 0x34);
     //   v34[2] = *(uint32_t*)((char*)g_libGTASA + 0x38);
        //((void (__fastcall *)(int, int*, int))(0x1AED7D + g_libGTASA))(v19, v34, 2); //rw translate
      /*  ((void (*)(void))((char*)g_libGTASA + 0x559EF9))();
        ((void (*)(void))((char*)g_libGTASA + 0x559FC9))();
        v20 = 0.0;
	//	Log("testcam 14");
     //   v21 = *(uint32_t*)(a1 + 0x40);
        v22 = 0.0;
        if ( v21 == 3 )
          v22 = 4.0;
        if ( !v21 )
        {
          v20 = 5.0;
          v22 = 11.0;
        }
        if ( v21 == 1 )
          v20 = 12.0;
        if ( v21 == 1 )
          v22 = 18.0;
        if ( v21 == 2 )
        {
          v20 = 19.0;
          v22 = 24.0;
        }
	//	Log("testcam 15");
   //     *(float*)(a1 + 0x44) = (float)((float)*(unsigned char*)(g_libGTASA + 0x8B18A5) - v20) / (float)(v22 - v20);
        if ( !dword_609F28 )
        {
       //   v23 = reinterpret_cast<void (__fastcall ***)(uint32_t, uint32_t)>(::operator new(0xCu));
       //   sub_6FD48();
       //   v24 = *v23;
      //    dword_609F28 = reinterpret_cast<int>(v23);
      //    (*v24)(v23, 0);
         // sub_6F580((uintptr_t*)dword_748F98, dword_609F28);
        //  v25 = reinterpret_cast<void (__fastcall ***)(uint32_t, uint32_t)>(::operator new(0xCu));
       //   sub_6FE14();
        //  v26 = *v25;
        //  dword_609F2C = reinterpret_cast<int>(v25);
     //     (*v26)(v25, 0);
        //  sub_6F580((uintptr_t*)dword_748F98, dword_609F2C);
	//	  Log("testcam 16");
        }
        if ( (byte_61572E[v13] & 0x80) != 0 )
          v27 = &dword_609F2C;
        else
          v27 = &dword_609F28;
        //sub_6F6A0((uintptr_t*)dword_748F98, *v27);
      //  v28 = *(uint32_t*)(a1 + 0x40);
        v29 = v28 + 1;
		//Log("testcam 17");
        if ( v28 > 2 )
          v29 = 0;
     //   v30 = a1 + 4 * v29;
     //   if ( a2 )
    //      v31 = (uint32_t*)(v30 + 0x30);
     //   else
     //     v31 = (uint32_t*)(v30 + 0x20);
	 // Log("testcam 133");
    //    sub_6EC24(*v31, 1);
	///	Log("testcam 441");
      //  sub_58518(v6);
	//	Log("testcam 18");
      //  sub_6F706((char*)dword_748F98);*/
      }
    }
  }
}
extern CNetGame* pNetGame;
extern CGame* pGame;
void sub_6ED20(int a1, uintptr_t* a2, uintptr_t* a3, int a4, int a5, int a6)
{
  int v8 = g_libGTASA; 
  int v9; 
  float *v10;
  float v11; 
  int v12;
  int v13; 
  int *v144;
  bool v14; 
  int v16;
  int v17; 
  int v18;
  int v19;
  float v166;
  float v155;
  VECTOR v77;
  int v222; // r0
  int v233; // [sp+8h] [bp+0h]
  float v288; // [sp+24h] [bp+1Ch]

  int v20;
  float v21;
  int v22[9];
//Log("1");
  if (a2)
  {
    v9 = *(int*)(g_libGTASA + 0x95B064);
    v17 = *(int*)(v9 + 0x60);
    v16 = *(int*)(v9 + 0x64);
    *(int*)(g_libGTASA + 0x9B4CB8) = 3;
    *(uintptr_t*)(v9 + 0x64) = *a3;
    *(uintptr_t*)(v9 + 0x60) = *a2;
    *(int *)((char *)0x61572C + v8) |= a1;
    v22[0] = 0xFFFFFFFF;
    ((void (__fastcall *)(int, int *, int))((char *)0x1AD8A1 + g_libGTASA))(v9, v22, 3);
//Log("11");
   // if (!*(int *)(dword_207F50 + 0x12C))
  //  {
   //   nullsub_16(&v19);
      sub_55718(
        &v19,
        *(uint8_t *)(g_libGTASA + 0x8C9AB4),
        *(uint8_t *)(g_libGTASA + 0x8C9AB6),
        *(uint16_t *)(g_libGTASA + 0x8C9AB8),
        0xFF);
      sub_5578E(&v19);
	  
      ((void ( *)(int, int *, int))((char *)0x1AD8A1 + g_libGTASA))(v9, (int*)&v19, 3);
  //  }
//Log("2");
    if (((int (__fastcall *)(int))(g_libGTASA + 0x463DF1))(v9) == 1)
    {
		*(int *)(g_libGTASA + 0x95B064 + 0x84) = *(int *)(g_libGTASA + 0x9B4C2C);
		*(float *)(g_libGTASA + 0x95B064 + 0x88) = *(float *)(g_libGTASA + 0x9B4C2C) * 100.0; //in gtasa
		
      *(uint8_t *)(v8 + 0x9B4CB2) = 1;
      v21 = 0.0;
      v19 = 0;
      v20 = 0;
	//  Log("22");

     /* if (sub_57DE0())
      {
		  Log("sub_57DE0 succes,  = %d", (int)sub_57DE0());
        v10 = *(float **)(/*sub_57DE0() + 0x14*//*g_libGTASA + 0x8a1144);
		if (v10)
        {
			Log("v10 succes,  = %d", (int)v10);
			Log("666");
          v11 = (float)*((int *)v10 + 0xC);
		  Log("5e55");
          v12 = *((int *)v10 + 0xD);
		  Log("66666");
          v21 = v10[0xE];
		  Log("42343");
          v19 = v11;
		  Log("34234");
          v20 = v12;
        }
      }*/
	  
	  v77.Z = 0.0f;
	  if(pChatWindow->isConnect){
		if ( g_libGTASA + 0x8A1144 )
		{
	/*	  if (g_libGTASA + 0x8B081C)
			v144 = (int *)(g_libGTASA + 0x8B081C + 0x30);
		  else
			v144 = (int *)(g_libGTASA + 0x8B080C);
		  v155 = *((float *)v144 + 1);
		  v166 = *((float *)v144 + 2);
		  v77.X = *v144;
		  v77.Y = v155;
		  v288 = v166;*/
		 // CPlayerPed* pPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
		  //pPed->GetBonePosition(4, &v77);
			v77 = *(VECTOR*)(0x8C1638 + g_libGTASA);
		}
	}
		//Log("3");
      if ((a1 & 0x8000000) == 0)
        v21 = v21 + 2.0;
		//v77.Z += 0.6f;
		float cameraPosition[3] = {v77.X, v77.Y, v77.Z};
      ((void ( *)(float [3]))(0x197E0C + g_libGTASA + 1))(cameraPosition);
	 // Log("v77 = %f", v77.X);
	  
	  
      if (*(int *)(g_libGTASA + 0x8B7D28))
      {
        v18 = 0;
        ((void (__fastcall *)(int, int *, int))((char *)0x1AD8A1 + g_libGTASA))(v9, &v18, 2);
        ((void (__fastcall *)(int, int *, int))((char *)0x1AD8A1 + g_libGTASA))(v9, &v18, 3);
        a4 = 1;
        v13 = a6;
      }
      else
      {
        v13 = a6;
    //    if (dword_609F1C && !a6 /*&& *(int *)(dword_207F50 + 0x12C*/)
        //  ((void (*)(void))sub_3F614)();
	  
      }
      ((void (*)(void))(g_libGTASA + 0x559009))();
      v14 = (a4 & (v13 ^ 1)) == 0;
      if ((a4 & (v13 ^ 1)) == 0)
        v14 = a5 == 0;

      if (v14)
		  
      {
        if (/**(int *)(dword_207F50 + 0x12C) && */dword_609F1C)
          sub_3F614(dword_609F1C, 0);
        ((void (*)(void))(g_libGTASA + 0x3B3B41))();
        ((void (*)(void))(g_libGTASA + 0x3B1609))();
        ((void (*)(void))(g_libGTASA + 0x3B1779))();
      }

	((void ( *)(int))((char *)0x1AD6B9 + g_libGTASA))(v9);
	
      *(uint8_t *)(v8 + 0x9B4CB2) = 0;
    }
    *(int *)((char *)0x61572C + v8) &= ~a1;
    *(int *)(v9 + 0x64) = v16;
    *(int *)(v9 + 0x60) = v17;
    *(int *)(g_libGTASA + 0x9B4CB8) = 0;
  }

}
RwRaster* dword_748F80;
RwRaster* dword_748F84;
RwRaster* dword_748F88;
RwRaster* dword_748F8C;
RwRaster* dword_748F90;
RwRaster* dword_748F94;
//void sub_6ED20(int a1, uintptr_t* a2, uintptr_t* a3, int a4, int a5, int a6);
int sub_6F04C()
{
    unsigned int v1; 
    int v2; 
    int v3;
    int result; 
    int settings_WaterReflLvl = 3; 

  //  if ( *(uintptr_t *)(g_libGTASA + 0x63E464) ) 
    //{
		//Log("1");
        ((void (*)())(g_libGTASA + 0x54DE1D))();// CMirrors::CreateBuffer(CMirrors *this)
   // }
    v1 = *(uintptr_t*)(g_libGTASA + 0x63E464);
    if ( v1 >= 1 )
	//	Log("2");
        sub_6ED20(0x800000, *(uintptr_t**)(g_libGTASA + 0x9B4CD0), *(uintptr_t**)(g_libGTASA + 0x9B4CD4), v1 == 2, 0, v1 == 3);
    v3 = settings_WaterReflLvl;
    if ( v3 >= 1 )                                // water 
    {
        if ( v3 != 1 )
            v3 = 0;
	//	Log("3");
        sub_6ED20(0x8800000, *(uintptr_t**)dword_748F90, *(uintptr_t**)dword_748F94, v3, 0, 0);
        if ( settings_WaterReflLvl >= 3 )
        {
		//	Log("4");
            sub_6ED20(0x8000000, *(uintptr_t**)dword_748F88, *(uintptr_t**)dword_748F8C, 0, 0, 0);
        }
    }
    result = 1;
    if ( result ){
		//Log("5");
        sub_6ED20(0, *(uintptr_t**)dword_748F80, *(uintptr_t**)dword_748F84, 0, 1, 0);
        return 0;
    }

    return result;
}


void sub_3F0C0(int *a1, int a2)
{
  int *v3; // r8
  int *v4; // r6
  bool v5; // zf
  char *v6; // r4
  unsigned int v7; // r1
  int *v8; // r0
  int v9; // r5
  void *v10; // r0
  void *v11; // r0
  unsigned int *v13; // r2
  int v14; // r1
  unsigned int *v15; // r2
  int v16; // r1
  unsigned int *v17; // r2
  int v18; // r1
  int v19; // [sp+4h] [bp-24h] BYREF
  int v20[7]; // [sp+Ch] [bp-1Ch] BYREF

  v3 = (int *)(a1 + 4 * a2);
  v4 = v3 + 4;
  if ( !*v3 )
    goto LABEL_6;
  v5 = *v4 == 0;
  if ( *v4 )
    v5 = v3[8] == 0;
  if ( v5 || !v3[0xC] )
  {
LABEL_6:
    *(int *)(a1 + 4 * a2) = sub_3F570(0x29A0, a2 + 0x29A0);
    *v4 = sub_3F570(0x29A6, a2 + 0x29A6);
/*    v6 = off_1F954C[a2];
    v3[8] = sub_57D48("samp", v6);
    sub_190218((int)v20, v6);
    sub_190218((int)&v19, "_blur");
    v7 = *(uintptr_t *)(v19 - 0xC) + *(uintptr_t *)(v20[0] - 0xC);
    if ( v7 > *(uintptr_t *)(v20[0] - 8) && v7 <= *(uintptr_t *)(v19 - 8) )
      v8 = (int *)sub_18F098(&v19, 0);
    else
      v8 = (int *)sub_18E0B0(v20, &v19);
    v9 = *v8;
    *v8 = (int)algn_7E7E4C;
    v3[0xC] = sub_57D48("samp", v9);
    if ( (int *)(v9 - 0xC) != &dword_7E7E40 )
    {
      v13 = (unsigned int *)(v9 - 4);
      if ( &pthread_create )
      {
        __dmb(0xBu);
        do
          v14 = __ldrex(v13);
        while ( __strex(v14 - 1, v13) );
        __dmb(0xBu);
      }
      else
      {
        v14 = (*v13)--;
      }
      if ( v14 <= 0 )
        j_j_j__ZdlPv((void *)(v9 - 0xC));
    }
    v10 = (void *)(v19 - 0xC);
    if ( (int *)(v19 - 0xC) != &dword_7E7E40 )
    {
      v15 = (unsigned int *)(v19 - 4);
      if ( &pthread_create )
      {
        __dmb(0xBu);
        do
          v16 = __ldrex(v15);
        while ( __strex(v16 - 1, v15) );
        __dmb(0xBu);
      }
      else
      {
        v16 = (*v15)--;
      }
      if ( v16 <= 0 )
        j_j_j__ZdlPv(v10);
    }
    v11 = (void *)(v20[0] - 0xC);
    if ( (int *)(v20[0] - 0xC) != &dword_7E7E40 )
    {
      v17 = (unsigned int *)(v20[0] - 4);
      if ( &pthread_create )
      {
        __dmb(0xBu);
        do
          v18 = __ldrex(v17);
        while ( __strex(v18 - 1, v17) );
        __dmb(0xBu);
      }
      else
      {
        v18 = (*v17)--;
      }
      if ( v18 <= 0 )
        j_j_j__ZdlPv(v11);
    }*/
  }
}
int *__fastcall sub_3F028(int *a1)
{
  a1[0x10] = 0;
  NOP(0x39A844 + g_libGTASA + 0x518, 2);
  NOP(0x39A844 + g_libGTASA, 2);
  NOP(0x39A844 + g_libGTASA + 0x540, 2);
  //NOP(g_libGTASA + 0x53E4EE, 2);
 // NOP(g_libGTASA + 0x53E35E, 2);
  memset(a1, 0, 0x40u);
 // sub_3F0C0(a1, 0);
 // sub_3F0C0(a1, 1);
  //sub_3F0C0(a1, 2);
 // sub_3F0C0(a1, 3);
  return a1;
}
void sub_6F160(){
	int *v2;
	RwRaster* result;
	dword_748F80 = RwRasterCreate(256, 256, 32, 1285);
	v2 = (int *)operator new(0x48u);
    sub_3F028(v2);
    dword_609F1C = (uintptr_t*)v2;
	dword_748F84 = RwRasterCreate(256, 256, 0, 1);
	dword_748F88 = RwRasterCreate(256, 256, 32, 1285);
	dword_748F8C = RwRasterCreate(256, 100, 0, 1);
	dword_748F90 = RwRasterCreate(256, 256, 32, 1285);
//	dword_748C88 = *(RwRaster**)LoadTextureFromDB("mobile", "BR_Nebo_Day");
  //  dword_748C90 = *(RwRaster**)LoadTextureFromDB("mobile", "waterDUDV");
    dword_748C98 = RwRasterCreate(256, 256, 32, 1285);
    dword_748C94 = RwRasterCreate(256, 256, 0, 1);
    dword_748C9C = RwRasterCreate(256, 256, 0, 1);
    //dword_748C8C = /*RwRasterCreate(256, 256, 0, 1);*/*(RwRaster**)LoadTextureFromDB("mobile", "BR_Nebo_Day");
	result = RwRasterCreate(100, 100, 0, 1);
	dword_748F94 = result;
	// dword_7BEE74 = g_libGTASA + 0x95B068;
	 dword_207F24 = (uint32_t*)(g_libGTASA+0x8B1104);
}

uint8_t jj = 0;
void updateWater();
void (*CMirrors__RenderReflections)(uintptr_t* thiz);
void CMirrors__RenderReflections_hook(uintptr_t* thiz){
	//
    if (jj != 0) {
      //  CMirrors__RenderReflections(thiz);
        sub_6F04C();
        return ;
    }

    
	sub_6F160();
    jj = 1;
	/*updateWater();
	if (UniformLocationWaveness != -1) {
				waterwave += 0.03;
				
			glUniform1f(glGetUniformLocation(*(uintptr_t *)(UniformLocationWaveness + 1000), "Waveness"), waterwave);
			Log("waterwave %f", waterwave);
		}*/
	 // Или любое другое значение, которое вы хотите установить
   
	
	//g_WaterShaderClass[4] += 0.03f;
	

 //   CMirrors__RenderReflections(thiz);

    sub_6F04C();

    return ;
}




char(*CStreaming__ConvertBufferToObject)(int, int, int);
int __attribute__((noinline)) g_unobfuscate(int a)
{
	return UNOBFUSCATE_DATA(a);
}

#define MAX_ENCRYPTED_TXD 2
//const cryptor::string_encryptor encrArch[MAX_ENCRYPTED_TXD] = { cryptor::create("texdb/samp/samp.txt", 21), cryptor::create("texdb/gui/gui.txt", 19) };
static int lastOpenedFile = 0;

extern CNetGame *pNetGame;
extern CGUI *pGUI;

bool g_bPlaySAMP = false;

void InitInMenu();
void MainLoop();
void HookCPad();

extern "C" uintptr_t get_lib() 
{
 	return g_libGTASA;
}

struct stFile
{
	int isFileExist;
	FILE *f;
};
int (*sub_4A3A8)(uintptr_t *a1);
int sub_4A3A8_hook(uintptr_t *a1)
{
  return sub_4A3A8(a1);
}
int (*MobileSettings__GetMaxResWidth)();
int MobileSettings__GetMaxResWidth_hook()
{
   // Log("res = %d", ((int(*)())(g_libGTASA + 0x0023816C + 1))());
    return (int)( ((int(*)())(g_libGTASA + 0x0023816C + 1))() * 0.75);
}

stFile* (*NvFOpen)(const char*, const char*, int, int);
stFile* NvFOpen_hook(const char* r0, const char* r1, int r2, int r3)
{
    Log(OBFUSCATE("Loading r0: %s, r1: %s..."), r0, r1);
	
	char path[0xFF] = { 0 };
	sprintf(path, "%s%s", g_pszStorage, r1);
	size_t totalLength = strlen(g_pszStorage) + strlen(r1) + 1;

    size_t length = strlen(r1);
    // change
    if (length >= 8)
    {
        const char* lastThree = r1 + (length - 8); // change
        length = strlen(lastThree);

        char* lowerCased = new char[length + 1];

        for (size_t i = 0; i < length; ++i) {
            lowerCased[i] = tolower(lastThree[i]);
        }
        lowerCased[length] = '\0';

        delete[] lowerCased;
    }
    
    // ----------------------------
    if(!strncmp(r1+12, "mainV1.scm", 10))
    {
        sprintf(path, "%sdata/script/mainv1.scm", g_pszStorage);
    }
    // ----------------------------
    else if(!strncmp(r1+12, "SCRIPTV1.IMG", 12))
    {
        sprintf(path, "%sdata/script/scriptv1.img", g_pszStorage);
    }
    // ----------------------------
    else if(!strncmp(r1, "DATA/PEDS.IDE", 13))
    {
        sprintf(path, "%sdata/peds.ide", g_pszStorage);
    }
    // ----------------------------
    else if(!strncmp(r1, "DATA/VEHICLES.IDE", 17))
    {
        sprintf(path, "%sdata/vehicles.ide", g_pszStorage);
    }
    else if (!strncmp(r1, "DATA/GTA.DAT", 12))
    {
        sprintf(path, "%sdata/gta.dat", g_pszStorage);
    }
    else
    {
        return NvFOpen(r0, r1, r2, r3);
    }

    stFile *st = (stFile*)malloc(8);
    st->isFileExist = false;

    FILE *f = fopen(path, "rb");
    if(f)
    {
        st->isFileExist = true;
        st->f = f;

        return st;
    }
    else
    {
        Log(OBFUSCATE("Load failed: (%s)"), path);
        free(st);
        st = nullptr;
        return 0;
    }
}

void ShowHud() 
{
	if(pGame) 
	{
		if(pNetGame) 
		{
			if(pGame->FindPlayerPed() || GamePool_FindPlayerPed()) 
			{
				CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
				if(pPlayerPool) 
				{
					/*g_pJavaWrapper->UpdateHudInfo(
					pGame->FindPlayerPed()->GetHealth(), 
					pGame->FindPlayerPed()->GetArmour(), 
					pGUI->GetEat(), 
					GamePool_FindPlayerPed()->WeaponSlots[GamePool_FindPlayerPed()->byteCurWeaponSlot].dwType, 
					GamePool_FindPlayerPed()->WeaponSlots[GamePool_FindPlayerPed()->byteCurWeaponSlot].dwAmmo, 
					(int)pPlayerPool->GetLocalPlayerID(), 
					pGame->GetLocalMoney(), pGame->GetWantedLevel()
					);*/
				}
				if(pSettings && pSettings->GetReadOnly().iHud)
				{
					*(uint8_t*)(g_libGTASA+0x7165E8) = 0;
				}
				else if(pSettings && !pSettings->GetReadOnly().iHud)
				{
					*(uint8_t*)(g_libGTASA+0x7165E8) = 1;
				}
			}
		}
	}
}

bool bGameStarted = false;
uint32_t bProcessedRender2dstuff = 0;
void (*Render2dStuff)();
void Render2dStuff_hook()
{
	bGameStarted = true;
	MAKE_PROFILE(test, test_time);
	MainLoop();
	LOG_PROFILE(test, test_time);
	bProcessedRender2dstuff = GetTickCount();
	Render2dStuff();

                  // -------------------------
	ShowHud();
                  // -------------------------

	if (pNetGame)
	{
		CLocalPlayer* pPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
		if (pPlayer)
		{
			if (pPlayer->GetPlayerPed())
			{
				pNetGame->GetTextDrawPool()->Draw();
			}
		}
		//if (pNetGame->GetPlayerPool()) FIX ME
		//{
		//	if (pNetGame->GetPlayerPool()->GetLocalPlayer())
		//	{
		//		CPlayerPed* pPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
		//		if (pPed)
		//		{
		//			CInfoBarText::Draw(pPed->GetHealth(), pPed->GetArmour());
		//		}
		//	}
		//}
	}

	return;
}

void __fillArray()
{
}
uintptr_t v2 = g_libGTASA;
uintptr_t v1 = g_libGTASA;
uintptr_t dword_72F0A8;
uintptr_t dword_72F0C8;
uintptr_t dword_72F0E8;
void (*RQTexture__Delete)(uintptr_t* thiz);
void RQTexture__Delete_hook(uintptr_t* thiz);
void (*RQ_Command_rqDeleteTexture)(uintptr_t**);
void RQ_Command_rqDeleteTexture_hook(uintptr_t** thiz);
void (*RQ_Command_rqTargetDelete)(uintptr_t**, int, int, int);
void RQ_Command_rqTargetDelete_hook(uintptr_t** thiz, int a2, int a3, int a4);
void (*ES2Texture__ES2Texture_end1)(uintptr_t*);
void ES2Texture__ES2Texture_end_hook1(uintptr_t* thiz);
void (*ES2Texture__ES2Texture_end)(uintptr_t);
void ES2Texture__ES2Texture_end_hook(uintptr_t thiz);


// ==============================================

uintptr_t dword_72F0EC;
uintptr_t dword_72F0F0;
uintptr_t dword_72F0F4;
uintptr_t dword_72F0F8;
uintptr_t dword_72F0FC;
uintptr_t dword_72F100;
uintptr_t dword_72F104;
void RQTexture__Delete_hook(uintptr_t* R0) {
    // Массив адресов
    uintptr_t addresses[] = {
        dword_72F0E8,
        dword_72F0EC,
        dword_72F0F0,
        dword_72F0F4,
        dword_72F0F8,
        dword_72F0FC,
        dword_72F100,
        dword_72F104
    };

    // Проходим по всем адресам
    for (size_t i = 0; i < sizeof(addresses) / sizeof(uintptr_t); i++) {
        // Проверяем, совпадает ли адрес с R0
        if (addresses[i] == (uintptr_t)R0) { // Сравниваем адреса
            // Если совпадает, заменяем значение по адресу на 0
            *(uintptr_t*)addresses[i] = 0;
        }
    }

    // Вызываем оригинальную функцию
    RQTexture__Delete(R0);
}

uintptr_t dword_72F0AC;
uintptr_t dword_72F0B0;
uintptr_t dword_72F0B4;
uintptr_t dword_72F0B8;
uintptr_t dword_72F0BC;
uintptr_t dword_72F0C0;
uintptr_t dword_72F0C4;
void ES2Texture__ES2Texture_end_hook(uintptr_t R0) {
    uintptr_t R1 = *(uintptr_t*)(R0 + 40);
    uintptr_t *ptruintptr_t_72F0A8 = &dword_72F0A8;

    if (*ptruintptr_t_72F0A8 != R1) {
        *ptruintptr_t_72F0A8 = 0;
    }

    uintptr_t *ptruintptr_t_72F0AC = (uintptr_t*)((uintptr_t)ptruintptr_t_72F0A8 + (dword_72F0AC - dword_72F0A8));
    if (*ptruintptr_t_72F0AC != R1) {
        *ptruintptr_t_72F0AC = 0;
    }

    uintptr_t *ptruintptr_t_72F0B0 = (uintptr_t*)((uintptr_t)ptruintptr_t_72F0A8 + (dword_72F0B0 - dword_72F0A8));
    if (*ptruintptr_t_72F0B0 != R1) {
        *ptruintptr_t_72F0B0 = 0;
    }

    uintptr_t *ptruintptr_t_72F0B4 = (uintptr_t*)((uintptr_t)ptruintptr_t_72F0A8 + (dword_72F0B4 - dword_72F0A8));
    if (*ptruintptr_t_72F0B4 != R1) {
        *ptruintptr_t_72F0B4 = 0;
    }

    uintptr_t *ptruintptr_t_72F0B8 = (uintptr_t*)((uintptr_t)ptruintptr_t_72F0A8 + (dword_72F0B8 - dword_72F0A8));
    if (*ptruintptr_t_72F0B8 != R1) {
        *ptruintptr_t_72F0B8 = 0;
    }

    uintptr_t *ptruintptr_t_72F0BC = (uintptr_t*)((uintptr_t)ptruintptr_t_72F0A8 + (dword_72F0BC - dword_72F0A8));
    if (*ptruintptr_t_72F0BC != R1) {
        *ptruintptr_t_72F0BC = 0;
    }

    uintptr_t *ptruintptr_t_72F0C0 = (uintptr_t*)((uintptr_t)ptruintptr_t_72F0A8 + (dword_72F0C0 - dword_72F0A8));
    if (*ptruintptr_t_72F0C0 != R1) {
        *ptruintptr_t_72F0C0 = 0;
    }

    uintptr_t *ptruintptr_t_72F0C4 = (uintptr_t*)((uintptr_t)ptruintptr_t_72F0A8 + (dword_72F0C4 - dword_72F0A8));
    if (*ptruintptr_t_72F0C4 != R1) {
        *ptruintptr_t_72F0C4 = 0;
    }

    ES2Texture__ES2Texture_end(R0);
}

void ES2Texture__ES2Texture_end_hook1(uintptr_t* R0) {
	uint32_t R1 = R0[0x28 / 4];
    uint32_t PC = reinterpret_cast<uint32_t>(__builtin_return_address(0));  //Current PC value, simulated

    if (dword_72F0A8 != R1) {
        dword_72F0A8 = 0;
    }

    if (dword_72F0AC != R1) {
        dword_72F0AC = 0;
    }

    if (dword_72F0B0 != R1) {
        dword_72F0B0 = 0;
    }

    if (dword_72F0B4 != R1) {
        dword_72F0B4 = 0;
    }

    if (dword_72F0B8 != R1) {
        dword_72F0B8 = 0;
    }

    if (dword_72F0BC != R1) {
        dword_72F0BC = 0;
    }

    if (dword_72F0C0 != R1) {
        dword_72F0C0 = 0;
    }

    if (dword_72F0C4 != R1) {
        dword_72F0C4 = 0;
    }

    ES2Texture__ES2Texture_end1(R0);
}

void RQ_Command_rqDeleteTexture_hook(uint32_t** R0) {
    uint32_t* R1 = *R0;

    if (R1 == 0) {
        RQ_Command_rqDeleteTexture(R0);
        return;
    }

    if (dword_72F0A8 == *reinterpret_cast<uint32_t*>(R1 + 0x28)) {
        dword_72F0A8 = 0;
    }

    if (dword_72F0AC == *reinterpret_cast<uint32_t*>(R1 + 0x28)) {
        dword_72F0AC = 0;
    }

    if (dword_72F0B0 == *reinterpret_cast<uint32_t*>(R1 + 0x28)) {
        dword_72F0B0 = 0;
    }

    if (dword_72F0B4 == *reinterpret_cast<uint32_t*>(R1 + 0x28)) {
        dword_72F0B4 = 0;
    }

    if (dword_72F0B8 == *reinterpret_cast<uint32_t*>(R1 + 0x28)) {
        dword_72F0B8 = 0;
    }

    if (dword_72F0BC == *reinterpret_cast<uint32_t*>(R1 + 0x28)) {
        dword_72F0BC = 0;
    }

    if (dword_72F0C0 == *reinterpret_cast<uint32_t*>(R1 + 0x28)) {
        dword_72F0C0 = 0;
    }

    if (dword_72F0C4 == *reinterpret_cast<uint32_t*>(R1 + 0x28)) {
        dword_72F0C4 = 0;
    }

    RQ_Command_rqDeleteTexture(R0);
}

void (*Render2dStuffAfterFade)();
void Render2dStuffAfterFade_hook()
{
	Render2dStuffAfterFade();
	if (pGUI && bGameStarted) pGUI->Render();
	return;
}

uint16_t gxt_string[0x7F];
uint16_t* (*CText_Get)(uintptr_t thiz, const char* text);
uint16_t* CText_Get_hook(uintptr_t thiz, const char* text)
{
	if(text[0] == 'S' && text[1] == 'A' && text[2] == 'M' && text[3] == 'P')
	{
		const char* code = &text[5];
		if(!strcmp(code, "MP")) CFont::AsciiToGxtChar("MultiPlayer", gxt_string);

    	return gxt_string;
	}

	return CText_Get(thiz, text);
}
int sub_6F350(int result, int a2)
{
  *(int *)(result + 0x34) = 0;
  *(int *)(result + 0x30) = a2;
  return result;
}
int sub_6F358(int result)
{
  *(int *)(result + 0x34) = 0;
  *(int *)(result + 0x30) = 0;
  return result;
}
int (*car_customenvmap)(int result, int a2);
int car_customenvmap_hook(int result, int a2)
{
  int *v2; // r4
  unsigned int v3; // r12

  v2 = *(int **)(0x669E48 + g_libGTASA);
  v3 = *(int *)(a2 + result) - *v2;
  if ( v3 / 0xC < v2[2] )
  {
    *(uintptr_t *)(v2[1] + v3 / 0xC) |= 0x80u;
    *(int *)(result + a2) = 0;
  }
  return result;
}
int (*dword_67E040)();
int sub_4AAFC()
{
  uintptr_t v0; // r5
  int result; // r0

  v0 = *(uintptr_t *)(g_libGTASA + 0x8B08F8);
  //if ( *(uintptr_t *)(dword_207F50 + 0x138) ) //SETTINGS 
 //   *(uintptr_t *)(g_libGTASA + 0x8B08F8) = 0x3DE147AE;
  dword_67E040();
  result = g_libGTASA;
  *(uintptr_t *)(g_libGTASA + 0x8B08F8) = v0;
  return result;
}
int (*dword_67E048)();
int DoSunAndMoon()
{
  float v0; // s16
  int result; // r0

  v0 = *(float *)(g_libGTASA + 0x985264);
  if ( /**(int *)(dword_207F50 + 0x134)*/ pChatWindow->isConnect)
    *(float *)(g_libGTASA + 0x985264) = v0 * 4.5;
  dword_67E048();
  result = g_libGTASA + 0x985264;
  *(float *)(g_libGTASA + 0x985264) = v0;
  return result;
}


int (*CCustomCarEnvMapPipeline__pluginEnvMatDestructor)(int result, int a2);
int CCustomCarEnvMapPipeline__pluginEnvMatDestructor_hook(int result, int a2)
{
    uintptr_t *v2; // r4
    unsigned int v3; // r12

    v2 = *(uintptr_t **)(g_libGTASA + 0x669E48);
    v3 = *(uintptr_t *)(a2 + result) - *v2;
    if ( v3 / 12 < v2[2] )
    {
        *(char *)(v2[1] + v3 / 12) |= 0x80u;
        *(uintptr_t *)(result + a2) = 0;
    }
    return result;
}
uintptr_t dword_61CF08;
uintptr_t dword_61CF10;
uintptr_t dword_61CF18;
uintptr_t dword_61CF20;
uintptr_t dword_61CF28;
uintptr_t dword_61CF30;
uintptr_t dword_61CF38;
uintptr_t dword_61CF40;
uintptr_t dword_61CF48;
uintptr_t dword_61CF50;
uintptr_t dword_61CF58;
uintptr_t dword_61CF60;
uintptr_t dword_61CF68;
uintptr_t dword_61CF70;
uintptr_t dword_61CF78;
uintptr_t dword_61CF80;
uintptr_t dword_61CF88;


char byte_61CF0C = 0;
char byte_61CF14 = 0;
char byte_61CF1C = 0;
char byte_61CF24 = 0;
char byte_61CF2C = 0;
char byte_61CF34 = 0;
char byte_61CF3C = 0;
char byte_61CF44 = 0;
char byte_61CF4C = 0;
char byte_61CF54 = 0;
char byte_61CF5C = 0;
char byte_61CF64 = 0;
char byte_61CF6C = 0;
char byte_61CF74 = 0;
char byte_61CF7C = 0;
char byte_61CF84 = 0;
char byte_61CF8C = 0;

int loc_3D090 = 300000;
int unk_1E8480 = 300000; //ptr singlee
int loc_7CC00 = 50000; //ped pool
int unk_4F77C0 = 15000; //veh pool
int unk_249F00 = 500000; //col model pool

#include "Pools.h"
int (*CPools__Initialise)(uintptr_t* thiz);
int CPools__Initialise_hook(uintptr_t* thiz)
{
	Log("pools init...");
	int result = CPools__Initialise(thiz);
	CPools::Initialise();
  return result;
}
uintptr_t unk_746030;
void MainMenu_OnStartSAMP()
{

	if(g_bPlaySAMP) return;
	NOP(g_libGTASA +  0x3AF20C, 8);
	CPools::InjectHooks();
	
	//g_pJavaWrapper->m_pRenderTarget = new CRenderTarget(512, 512, true);
//SetUpHook(g_libGTASA + 0x3AF1A0, (uintptr_t)CPools__Initialise_hook, (uintptr_t*)&CPools__Initialise); 
	InitInMenu();
	
	Log("init veh upg");
	
	CHook::RET(g_libGTASA + 0x53B004);
	
	
	for(int i = 0; i < 40; i++) {
		char name[55];
		sprintf(name, "remapbody%d", i + 1);
		CVehicle::m_pVinyls[i] = (RwTexture*)LoadTextureFromDB("samp", name);
	}

//	CVehicle::pNeonTex = LoadTextureFromDB("samp", "neeby");
	
	 // UnFuck((char *)0x2769F8 + g_libGTASA);
	 // *(uintptr_t *)((char *)0x2769F8 + g_libGTASA) = 0x3206A2;
//	  memset(&unk_746030, 0, 0x194u);
//	  UnFuck(0x5CF4A0 + g_libGTASA);
	 // v0 = g_libGTASA;
//	  *(uintptr_t *)((char *)0x5CF4A0 + g_libGTASA) = unk_746030;
		
	CSkyBox::Process();

	// StartGameScreen::OnNewGameCheck()
	(( void (*)())(g_libGTASA+0x261C8C+1))();
	sub_8F3D0();
	SetUpHook(g_libGTASA + 0x28B1C8, (uintptr_t)car_customenvmap_hook, (uintptr_t*)&car_customenvmap);
	//SetUpHook(g_libGTASA + 0x28B1C8 + 0x274, (uintptr_t)sub_xuinaKakayaTa_hook, (uintptr_t*)&sub_xuinaKakayaTa);
	//NOP(0x39B2AC + g_libGTASA, 2);
	SetUpHook(g_libGTASA + 0x54DF60, (uintptr_t)CMirrors__RenderReflections_hook, (uintptr_t*)&CMirrors__RenderReflections);
	//CHook::RET(g_libGTASA + 0x54DF60);
	// WriteMemory(0x55CFA4 + g_libGTASA, (uintptr_t)"\xF3\xEE\x04\x4A", 4);
	//WriteMemory(0x55CFA4 + g_libGTASA + 4, (uintptr_t)"\x42\x4B" , 2); //WRITE
	//  SetUpHook(0x55CFA4 + g_libGTASA, (uintptr_t)sub_4AAFC, (uintptr_t*)&dword_67E040); //RENDER WARE CAM
	//Log("lol 1");
	 CHook::InlineHook(g_libGTASA, 0x52EEDC, &DoSunAndMoon, &dword_67E048);
	// Log("lol 2");
	// SetUpHook(g_libGTASA + 0x28B1C8, (uintptr_t)CCustomCarEnvMapPipeline__pluginEnvMatDestructor_hook, (uintptr_t*)&CCustomCarEnvMapPipeline__pluginEnvMatDestructor);
    //WriteMemory(g_libGTASA + 0x55CFA4, (uintptr_t )"\x4A\x04\xEE\xF3", 4);// \x4A\x04\xEE\xF3
	//Log("lol 3");
    //WriteMemory(g_libGTASA + 0x55CFA8, (uintptr_t )"\x4B\x42", 2);// \x4B\x42
    //SetUpHook(g_libGTASA + 0x55CFA4, (uintptr_t)CVisibilityPlugins__SetRenderWareCamera_hook, (uintptr_t*)&CVisibilityPlugins__SetRenderWareCamera);
  //  Log("lol 4");
	//*(bool*)(g_libGTASA+0xA5A15C) = false;
	g_bPlaySAMP = true;
	
	//CMirrors__RenderReflections_hook(0);
	
	return;
}
int (*rqinitbebe)(int a1, int a2, int a3);
int rqinitbebe_hook(int a1, int a2, int a3)
{
	return rqinitbebe(a1,a2,a3);
}
// OsArray<FlowScreen::MenuItem>::Add
void (*MenuItem_add)(int r0, uintptr_t r1);
void MenuItem_add_hook(int r0, uintptr_t r1)
{
	static bool bMenuInited = false;
	char* name = *(char**)(r1+4);

	if(!strcmp(name, "FEP_STG") && !bMenuInited)
	{
                                    Log(OBFUSCATE("Load zalupa: 4 | addr: 0x%X"), r1);

		// "New Game"
		MenuItem_add(r0, r1);

		// "MultiPlayer"
		*(char**)(r1+4) = "SAMP_MP";
		*(uintptr_t*)r1 = LoadTextureFromDB("samp", "menu_mainmp");
		*(uintptr_t*)(r1+8) = (uintptr_t)MainMenu_OnStartSAMP;

		bMenuInited = true;
		goto ret;
	}

	if(g_bPlaySAMP && (
		!strcmp(name, "FEP_STG") ||
		!strcmp(name, "FEH_STA") ||
		!strcmp(name, "FEH_BRI") ))
		return;

ret:
	return MenuItem_add(r0, r1);
}

/* ====================================================== */
void (*LoadCollision)(unsigned char *a1, unsigned int a2, unsigned char a3);
void LoadCollision__hook(unsigned char *a1, unsigned int a2, unsigned char a3)
{
	//Log(OBFUSCATE("Loading: %s %s.."), a1, a3);

	return LoadCollision(a1, a2, a3);
}
enum TextureDatabaseFormat {
 DF_NONE  = 0,  // DF_UNC
 DF_DXT   = 1,
 DF_UNC_2  = 2,
 DF_PVR   = 3,
 DF_ETC   = 4,
 DF_Default = 5
}; 

uintptr_t* (*TextureDatabaseRuntime_Load)(const char *a1, int a2, int a3);
uintptr_t* TextureDatabaseRuntime_Load_hook(const char *a1, int a2, int a3){
	a3 = DF_DXT;
	if(a1 == "BRAks"
		  || a1 == "br_tex_winter"
			|| a1 == "br_radarmap"
			|| a1 == "br_tex_acc_01"
			|| a1 == "br_tex_cars_01_nologo"
			|| a1 == "br_tex_cars"
			|| a1 == "br_tex_patent"
			|| a1 == "br_tex_interiors"
			|| a1 == "br_tex_LOD"
			|| a1 == "br_tex_skins_01"
			|| a1 == "br_tex_mobile"
			|| a1 == "br_tex_generic"
			|| a1 == "br_tex_txd"
			|| a1 == "br_tex_legacy_smp"
			|| a1 == "br_tex_menu"
			|| a1 == "br_tex_gui"){
		a3 = DF_PVR;
	}
	Log("Load %s...", a1);
	return TextureDatabaseRuntime_Load(a1, a2, a3);
}
uintptr_t (*CModelInfo_Init)();
uintptr_t CModelInfo_Init_Hook()
{
	return 0;
}
// CGame::InitialiseRenderWare
void (*InitialiseRenderWare)();
void InitialiseRenderWare_hook()
{
                  //Log(OBFUSCATE("Load zalupa: 2"));
	Log(OBFUSCATE("Loading texdb.."));

                  // Отключение текстур 
	CHook::NOP(g_libGTASA + 0x40C546, 44);
  CHook::NOP(g_libGTASA + 0x40C236, 2); // crash CCamera::ClearPlayerWeaponMode in CCamera::Init
	CHook::RET(g_libGTASA + 0x0033FD9C); // CCutsceneMgr::LoadCutsceneDatа

	TextureDatabaseRuntime::Load("mobile", false, 5);
	TextureDatabaseRuntime::Load("menu", false, 5);
	TextureDatabaseRuntime::Load("txd", false, 5);
	TextureDatabaseRuntime::Load("gta3", false, 5);
	TextureDatabaseRuntime::Load("gta_int", false, 5);
	TextureDatabaseRuntime::Load("samp", false, 5);
	TextureDatabaseRuntime::Load("cutscene", false, 5);
	TextureDatabaseRuntime::Load("player", false, 5);
	
/*	//TextureDatabase* br_tex_winter = TextureDatabaseRuntime::Load("br_tex_winter", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_Default);
   // TextureDatabaseRuntime::Register(br_tex_winter);
//  TextureDatabase* br_radarmap = TextureDatabaseRuntime::Load("br_radarmap", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_Default);
  //  TextureDatabaseRuntime::Register(br_radarmap);
 // TextureDatabase* #br_tex_mobile_hln = TextureDatabaseRuntime::Load("#br_tex_mobile_hln", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_Default);
 //   TextureDatabaseRuntime::Register(#br_tex_mobile_hln);
  TextureDatabase* br_tex_acc_01 = TextureDatabaseRuntime::Load("br_tex_acc_01", false, 5);
    TextureDatabaseRuntime::Register(br_tex_acc_01);
 // TextureDatabase* #br_tex_winter = TextureDatabaseRuntime::Load("#br_tex_winter", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_Default);
 //   TextureDatabaseRuntime::Register(#br_tex_winter);
 // TextureDatabase* br_tex_cars_01_nologo = TextureDatabaseRuntime::Load("br_tex_cars_01_nologo", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_Default);
 //   TextureDatabaseRuntime::Register(br_tex_cars_01_nologo);
  TextureDatabase* br_tex_cars_01 = TextureDatabaseRuntime::Load("br_tex_cars_01", false, 5);
    TextureDatabaseRuntime::Register(br_tex_cars_01);
 // TextureDatabase* br_tex_patent = TextureDatabaseRuntime::Load("br_tex_patent", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_Default);
 //   TextureDatabaseRuntime::Register(br_tex_patent);
  TextureDatabase* br_tex_interiors = TextureDatabaseRuntime::Load("br_tex_interiors", false, 5);
    TextureDatabaseRuntime::Register(br_tex_interiors);
  TextureDatabase* br_tex_01 = TextureDatabaseRuntime::Load("br_tex_01", false, 5);
    TextureDatabaseRuntime::Register(br_tex_01);
  TextureDatabase* br_tex_LOD_01 = TextureDatabaseRuntime::Load("br_tex_LOD", false, 5);
    TextureDatabaseRuntime::Register(br_tex_LOD_01);
  TextureDatabase* br_tex_skins_01 = TextureDatabaseRuntime::Load("br_tex_skins_01", false, 5);
    TextureDatabaseRuntime::Register(br_tex_skins_01);
 // TextureDatabase* br_tex_mobile = TextureDatabaseRuntime::Load("br_tex_mobile", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_Default);
  //  TextureDatabaseRuntime::Register(br_tex_mobile);
 // TextureDatabase* br_tex_generic = TextureDatabaseRuntime::Load("br_tex_generic", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_Default);
 //   TextureDatabaseRuntime::Register(br_tex_generic);
  TextureDatabase* br_tex_main = TextureDatabaseRuntime::Load("br_tex_main", false, 5);
    TextureDatabaseRuntime::Register(br_tex_main);*/
 // TextureDatabase* br_tex_txd = TextureDatabaseRuntime::Load("br_tex_txd", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_Default);
 //   TextureDatabaseRuntime::Register(br_tex_txd);
  //TextureDatabase* br_tex_legacy_smp = TextureDatabaseRuntime::Load("br_tex_legacy_smp", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_Default);
  //  TextureDatabaseRuntime::Register(br_tex_legacy_smp);
 // TextureDatabase* br_tex_menu = TextureDatabaseRuntime::Load("br_tex_menu", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_Default);
 //   TextureDatabaseRuntime::Register(br_tex_menu);
 // TextureDatabase* br_tex_gui_nologo = TextureDatabaseRuntime::Load("br_tex_gui_nologo", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_Default);
  //  TextureDatabaseRuntime::Register(br_tex_gui_nologo);
 // TextureDatabase* br_tex_gui = TextureDatabaseRuntime::Load("br_tex_gui", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_Default);
 //   TextureDatabaseRuntime::Register(br_tex_gui);
	
	InitialiseRenderWare();
}

// туман
//void (*CDraw__SetFOV)(float fFOV, bool isCinematic);
//void CDraw__SetFOV_hook(float fFOV, bool isCinematic)
//{
//	float tmp = (float)((float)((float)(*(float *)(g_libGTASA + 0xA26A90) + -1.3333) * 11.0) / 0.44444) + fFOV;
//	if(tmp > 100) tmp = 100.0;
//	*(float *)(g_libGTASA + 0x006B1CB8) = 1500;
//}
float (*CDraw__SetFOV)(float thiz, float a2);
float CDraw__SetFOV_hook(float thiz, float a2)
{
	float tmp = (float)((float)((float)(*(float *)&*(float *)(g_libGTASA + 0x0098525C) - 1.3333) * 11.0) / 0.44444) + thiz;
	if(tmp > 130) tmp = 130.0;
	if(CFirstPersonCamera::IsEnabled()){
		tmp = 115.0f;
	}
	*(float *)(g_libGTASA + 0x00610968) = tmp;
	return thiz;
}
#include "sprite2d.h"
CSprite2d* radar = nullptr;
void DrawRadarTexture(float x, float y, float size1, float size2)
{
	if(radar == nullptr){
		radar = new CSprite2d();
	}
	
	if(!radar->m_pRwTexture){
		radar->m_pRwTexture = (RwTexture*)LoadTextureFromDB("samp", "hud_back");
	
	}
	

	CRGBA color;
	if(pChatWindow->showinghud){color.A = 255;}else{color.A=0;}
	
	color.R = 255;
	color.G = 255;
	color.B = 255;
	
	//Log("Sprite: posx: %f posy: %f sizex: %f sizey: %f", x, y, size1, size2);
//	Log("Radar: posx: %f posy: %f", CRadar::radarPos.x1, CRadar::radarPos.y1);

	radar->Draw(x, y, size1, size2, color);
}
void RenderSplashScreen();
void (*CLoadingScreen_DisplayPCScreen)();
void CLoadingScreen_DisplayPCScreen_hook()
{
	RwCamera* camera = *(RwCamera**)(g_libGTASA+0x95B064);

	if(RwCameraBeginUpdate(camera))
	{
		DefinedState2d();
		(( void (*)())(g_libGTASA+0x5519C8+1))(); // CSprite2d::InitPerFrame()
		RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void*)rwTEXTUREADDRESSCLAMP);
		(( void (*)(bool))(g_libGTASA+0x198010+1))(false); // emu_GammaSet()
		RenderSplashScreen();
		RwCameraEndUpdate(camera);
		RwCameraShowRaster(camera, 0, 0);
	}

	return;
}
int bBlockCWidgetRegionLookUpdate = 0;

/* ====================================================== */
#include "..//keyboard.h"
extern CKeyBoard* pKeyBoard;
void (*TouchEvent)(int, int, int posX, int posY);
void TouchEvent_hook(int type, int num, int posX, int posY)
{
	//Log("TOUCH EVENT HOOK");

	if (*(uint8_t*)(g_libGTASA + 0x008C9BA3) == 1)
	{
		return TouchEvent(type, num, posX, posY);
	}

	if (g_pWidgetManager)
	{
		g_pWidgetManager->OnTouchEventSingle(WIDGET_CHATHISTORY_UP, type, num, posX, posY);
		g_pWidgetManager->OnTouchEventSingle(WIDGET_CHATHISTORY_DOWN, type, num, posX, posY);
	}
	bool bRet = pGUI->OnTouchEvent(type, num, posX, posY);
	if (!bRet)
	{
		return;
	}

	ImGuiIO& io = ImGui::GetIO();

	if (pKeyBoard && pKeyBoard->IsOpen())
	{
		if (posX >= io.DisplaySize.x - ImGui::GetFontSize() * 4 && posY >= io.DisplaySize.y / 2 - (ImGui::GetFontSize() * 2.5) * 3 && posY <= io.DisplaySize.y / 2) // keys
		{
			return;
		}
	}

	if (g_pWidgetManager)
	{
		g_pWidgetManager->OnTouchEvent(type, num, posX, posY);
		if (g_pWidgetManager->GetWidget(WIDGET_MICROPHONE))
		{
			if (g_pWidgetManager->GetWidget(WIDGET_MICROPHONE)->GetState() == 1)
			{
				bBlockCWidgetRegionLookUpdate = 1;
			}
			if (g_pWidgetManager->GetWidget(WIDGET_MICROPHONE)->GetState() == 2)
			{
				bBlockCWidgetRegionLookUpdate = 0;
			}
		}
	}
	if(bRet) 
		return TouchEvent(type, num, posX, posY);
}

extern char g_iLastBlock[512];
int *(*LoadFullTexture)(uintptr_t *thiz, unsigned int a2);
int *LoadFullTexture_hook(uintptr_t *thiz, unsigned int a2)
{
  strcpy(g_iLastBlock, *(const char **)(*((DWORD *)thiz + 7) + 0x17 * a2));

    return LoadFullTexture(thiz, a2);
}


typedef struct _PED_MODEL
{
	uintptr_t 	vtable;
	uint8_t		data[88];
} PED_MODEL; // SIZE = 92

PED_MODEL PedsModels[370];
int PedsModelsCount = 0;

PED_MODEL* (*CModelInfo_AddPedModel)(int id);
PED_MODEL* CModelInfo_AddPedModel_hook(int id)
{
	PED_MODEL* model = &PedsModels[PedsModelsCount];
	memset(model, 0, sizeof(PED_MODEL));

	((void(*)(void* thiz))(g_libGTASA + 0x0033559C + 1))((void*)model); // CBaseModelInfo::CBaseModelInfo();
                  model->vtable = (uintptr_t)(g_libGTASA+0x5C6E90);					// assign CPedModelInfo vmt

                  (( uintptr_t (*)(PED_MODEL*))(*(void**)(model->vtable+0x1C)))(model);  // CClumpModelInfo::Init()
                  *(PED_MODEL**)(g_libGTASA+0x87BF48+(id*4)) = model; // CModelInfo::ms_modelInfoPtrs

	PedsModelsCount++;
	return model;
}
/* ====================================================== */

uint32_t (*CRadar__GetRadarTraceColor)(uint32_t color, uint8_t bright, uint8_t friendly);
uint32_t CRadar__GetRadarTraceColor_hook(uint32_t color, uint8_t bright, uint8_t friendly)
{
	return TranslateColorCodeToRGBA(color);
}

int (*CRadar__SetCoordBlip)(int r0, float X, float Y, float Z, int r4, int r5, char* name);
int CRadar__SetCoordBlip_hook(int r0, float X, float Y, float Z, int r4, int r5, char* name)
{
	if(pNetGame && !strncmp(name, OBFUSCATE("CODEWAY"), 7))
	{
		float findZ = (( float (*)(float, float))(g_libGTASA+0x3C3DD8+1))(X, Y);
		findZ += 1.5f;

		//Log("OnPlayerClickMap: %f, %f, %f", X, Y, Z);
		RakNet::BitStream bsSend;
		bsSend.Write(X);
		bsSend.Write(Y);
		bsSend.Write(findZ);
		pNetGame->GetRakClient()->RPC(&RPC_MapMarker, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
	}

	return CRadar__SetCoordBlip(r0, X, Y, Z, r4, r5, name);
}

uint8_t bGZ = 0;
void (*CRadar__DrawRadarGangOverlay)(uint8_t v1);
void CRadar__DrawRadarGangOverlay_hook(uint8_t v1)
{
	bGZ = v1;
	if (pNetGame && pNetGame->GetGangZonePool()) 
		pNetGame->GetGangZonePool()->Draw();
}

uint32_t dwParam1, dwParam2;
extern "C" void pickup_ololo()
{
	if(pNetGame && pNetGame->GetPickupPool())
	{
		CPickupPool *pPickups = pNetGame->GetPickupPool();
		pPickups->PickedUp( ((dwParam1-(g_libGTASA+0x70E264))/0x20) );
	}
}

__attribute__((naked)) void PickupPickUp_hook()
{
	//LOGI("PickupPickUp_hook");

	// calculate and save ret address
	__asm__ volatile("push {lr}\n\t"
					"push {r0}\n\t"
					"blx get_lib\n\t"
					"add r0, #0x2D0000\n\t"
					"add r0, #0x009A00\n\t"
					"add r0, #1\n\t"
					"mov r1, r0\n\t"
					"pop {r0}\n\t"
					"pop {lr}\n\t"
					"push {r1}\n\t");
	
	// 
	__asm__ volatile("push {r0-r11, lr}\n\t"
					"mov %0, r4" : "=r" (dwParam1));

	__asm__ volatile("blx pickup_ololo\n\t");


	__asm__ volatile("pop {r0-r11, lr}\n\t");

	// restore
	__asm__ volatile("ldrb r1, [r4, #0x1C]\n\t"
					"sub.w r2, r1, #0xD\n\t"
					"sub.w r2, r1, #8\n\t"
					"cmp r1, #6\n\t"
					"pop {pc}\n\t");
}

extern "C" bool NotifyEnterVehicle(VEHICLE_TYPE *_pVehicle)
{
    //Log("NotifyEnterVehicle");
 
    if(!pNetGame) return false;
 
    CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
    CVehicle *pVehicle;
    VEHICLEID VehicleID = pVehiclePool->FindIDFromGtaPtr(_pVehicle);
 
    if(VehicleID == INVALID_VEHICLE_ID) return false;
    if(!pVehiclePool->GetSlotState(VehicleID)) return false;
    pVehicle = pVehiclePool->GetAt(VehicleID);
    if(pVehicle->m_bDoorsLocked) return false;
    if(pVehicle->m_pVehicle->entity.nModelIndex == TRAIN_PASSENGER) return false;
 
    if(pVehicle->m_pVehicle->pDriver &&
        pVehicle->m_pVehicle->pDriver->dwPedType != 0)
        return false;
 
    CLocalPlayer *pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
 
    //if(pLocalPlayer->GetPlayerPed() && pLocalPlayer->GetPlayerPed()->GetCurrentWeapon() == WEAPON_PARACHUTE)
    //  pLocalPlayer->GetPlayerPed()->SetArmedWeapon(0);
 
    pLocalPlayer->SendEnterVehicleNotification(VehicleID, false);
 
    return true;
}

void (*CTaskComplexEnterCarAsDriver)(uint32_t thiz, uint32_t pVehicle);
extern "C" void call_taskEnterCarAsDriver(uintptr_t a, uint32_t b)
{
	CTaskComplexEnterCarAsDriver(a, b);
}
void __attribute__((naked)) CTaskComplexEnterCarAsDriver_hook(uint32_t thiz, uint32_t pVehicle)
{
    __asm__ volatile("push {r0-r11, lr}\n\t"
                    "mov r2, lr\n\t"
                    "blx get_lib\n\t"
                    "add r0, #0x3A0000\n\t"
                    "add r0, #0xEE00\n\t"
                    "add r0, #0xF7\n\t"
                    "cmp r2, r0\n\t"
                    "bne 1f\n\t" // !=
                    "mov r0, r1\n\t"
                    "blx NotifyEnterVehicle\n\t" // call NotifyEnterVehicle
                    "1:\n\t"  // call orig
                    "pop {r0-r11, lr}\n\t"
    				"push {r0-r11, lr}\n\t"
    				"blx call_taskEnterCarAsDriver\n\t"
    				"pop {r0-r11, pc}");
}

void ProcessPedDamage(PED_TYPE* pIssuer, PED_TYPE* pDamaged)
{
	if (!pNetGame) return;

	PED_TYPE* pPedPlayer = GamePool_FindPlayerPed();
	if (pDamaged && (pPedPlayer == pIssuer))
	{
		if (pNetGame->GetPlayerPool()->FindRemotePlayerIDFromGtaPtr((PED_TYPE*)pDamaged) != INVALID_PLAYER_ID)
		{
			CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
			CAMERA_AIM* caAim = pPlayerPool->GetLocalPlayer()->GetPlayerPed()->GetCurrentAim();

			VECTOR aim;
			aim.X = caAim->f1x;
			aim.Y = caAim->f1y;
			aim.Z = caAim->f1z;

			pPlayerPool->GetLocalPlayer()->SendBulletSyncData(pPlayerPool->FindRemotePlayerIDFromGtaPtr((PED_TYPE*)pDamaged), 1, aim);
		}
	}
}

void (*CTaskComplexLeaveCar)(uintptr_t** thiz, VEHICLE_TYPE *pVehicle, int iTargetDoor, int iDelayTime, bool bSensibleLeaveCar, bool bForceGetOut);
void CTaskComplexLeaveCar_hook(uintptr_t** thiz, VEHICLE_TYPE *pVehicle, int iTargetDoor, int iDelayTime, bool bSensibleLeaveCar, bool bForceGetOut) 
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;
 
 	if (dwRetAddr == 0x3AE905 || dwRetAddr == 0x3AE9CF) 
 	{
 		if (pNetGame) 
 		{
 			if (GamePool_FindPlayerPed()->pVehicle == (uint32_t)pVehicle) 
 			{
 				CVehiclePool *pVehiclePool=pNetGame->GetVehiclePool();
 				VEHICLEID VehicleID=pVehiclePool->FindIDFromGtaPtr((VEHICLE_TYPE *)GamePool_FindPlayerPed()->pVehicle);
 				CLocalPlayer *pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
 				pLocalPlayer->SendExitVehicleNotification(VehicleID);
 			}
 		}
 	}
 
 	(*CTaskComplexLeaveCar)(thiz, pVehicle, iTargetDoor, iDelayTime, bSensibleLeaveCar, bForceGetOut);
}

unsigned int (*MainMenuScreen__Update)(uintptr_t thiz, float a2);
unsigned int MainMenuScreen__Update_hook(uintptr_t thiz, float a2)
{
	unsigned int ret = MainMenuScreen__Update(thiz, a2);
	MainMenu_OnStartSAMP();
	return ret;
}
bool iii = false;
signed int (*OS_FileOpen)(unsigned int a1, int *a2, const char *a3, int a4);
signed int OS_FileOpen_hook(unsigned int a1, int *a2, const char *a3, int a4)
{	
	Log("Loading: %s", a3);
    uintptr_t calledFrom = 0;
    __asm__ volatile("mov %0, lr"
    : "=r"(calledFrom));
    calledFrom -= g_libGTASA;
    signed int retn = OS_FileOpen(a1, a2, a3, a4);

	size_t length = strlen(a3);

		if (length >= 7) {
			const char* lastThree = a3 + (length - 7);
			 length = strlen(lastThree);
    
			// Создаем новый массив для результата. Не забываем об окончании строки '\0'.
			char* lowerCased = new char[length + 1]; // +1 для null-терминатора

			// Преобразование всех заглавных букв в строчные
			for (size_t i = 0; i < length; ++i) {
				lowerCased[i] = tolower(lastThree[i]);
			}
			lowerCased[length] = '\0'; // Добавляем null-терминатор
		//	Log("lowerCased: %s", lowerCased + 4);
			if(!strncmp(lowerCased + 4, "txt", 3) 
			//!strncmp(lowerCased + 4, "ide", 3) || 
			
			//!strncmp(lowerCased + 4, "ipl", 3) ||  
			//!strncmp(lowerCased + 4, "TXT", 3) || 
			//!strncmp(lowerCased + 4, "IDE", 3) || 
			//!strncmp(lowerCased + 4, "IPL", 3) ||
			//!strncmp(lowerCased, "gta.dat", 7)
			)
			{
				
				//lastOpenedFile = *a2;
				
				Log("LAST OPEN: %s", a3);
				
			}
			delete[] lowerCased;
				
		}
    return retn;
}
extern int(*OS_FileRead)(void* a1, void* a2, int a3);
int OS_FileRead_hook(void* a1, void* a2, int a33);
extern char(*CStreaming__ConvertBufferToObject)(int, int, int);
char CStreaming__ConvertBufferToObject_hook(int a1, int a2, int a3);
extern char(*CFileMgr__ReadLine)(int, int, int);
char CFileMgr__ReadLine_hook(int a1, int a2, int a3);

void RedirectCall(uintptr_t addr, uintptr_t func);

static char szLastBufferedName[40];
int (*cHandlingDataMgr__FindExactWord)(uintptr_t thiz, char* line, char* nameTable, int entrySize, int entryCount);
int cHandlingDataMgr__FindExactWord_hook(uintptr_t thiz, char* line, char* nameTable, int entrySize, int entryCount)
{
	strncpy(&szLastBufferedName[0], line, entrySize);
	return cHandlingDataMgr__FindExactWord(thiz, line, nameTable, entrySize, entryCount);
}

void (*cHandlingDataMgr__ConvertDataToGameUnits)(uintptr_t thiz, tHandlingData* handling);
void cHandlingDataMgr__ConvertDataToGameUnits_hook(uintptr_t thiz, tHandlingData* handling)
{
	int iHandling = ((int(*)(uintptr_t, char*))(g_libGTASA + 0x004FBC4C + 1))(thiz, &szLastBufferedName[0]);

	CHandlingDefault::FillDefaultHandling((uint16_t)iHandling, handling);

	return cHandlingDataMgr__ConvertDataToGameUnits(thiz, handling);
}



#include "..//nv_event.h"
int32_t(*NVEventGetNextEvent_hooked)(NVEvent* ev, int waitMSecs);
int32_t NVEventGetNextEvent_hook(NVEvent* ev, int waitMSecs)
{
	int32_t ret = NVEventGetNextEvent_hooked(ev, waitMSecs);

	if (ret)
	{
		if (ev->m_type == NV_EVENT_MULTITOUCH)
		{
			// process manually
			ev->m_type = (NVEventType)228;
		}

	}

	NVEvent event;
	NVEventGetNextEvent(&event);

	if (event.m_type == NV_EVENT_MULTITOUCH)
	{
		int type = event.m_data.m_multi.m_action & NV_MULTITOUCH_ACTION_MASK;
		int num = (event.m_data.m_multi.m_action & NV_MULTITOUCH_POINTER_MASK) >> NV_MULTITOUCH_POINTER_SHIFT;

		int x1 = event.m_data.m_multi.m_x1;
		int y1 = event.m_data.m_multi.m_y1;

		int x2 = event.m_data.m_multi.m_x2;
		int y2 = event.m_data.m_multi.m_y2;

		int x3 = event.m_data.m_multi.m_x3;
		int y3 = event.m_data.m_multi.m_y3;

		if (type == NV_MULTITOUCH_CANCEL)
		{
			type = NV_MULTITOUCH_UP;
		}

		if ((x1 || y1) || num == 0)
		{
			if (num == 0 && type != NV_MULTITOUCH_MOVE)
			{
				((void(*)(int, int, int posX, int posY))(g_libGTASA + 0x00239D5C + 1))(type, 0, x1, y1); // AND_TouchEvent
			}
			else
			{
				((void(*)(int, int, int posX, int posY))(g_libGTASA + 0x00239D5C + 1))(NV_MULTITOUCH_MOVE, 0, x1, y1); // AND_TouchEvent
			}
		}

		if ((x2 || y2) || num == 1)
		{
			if (num == 1 && type != NV_MULTITOUCH_MOVE)
			{
				((void(*)(int, int, int posX, int posY))(g_libGTASA + 0x00239D5C + 1))(type, 1, x2, y2); // AND_TouchEvent
			}
			else
			{
				((void(*)(int, int, int posX, int posY))(g_libGTASA + 0x00239D5C + 1))(NV_MULTITOUCH_MOVE, 1, x2, y2); // AND_TouchEvent
			}
		}
		if ((x3 || y3) || num == 2)
		{
			if (num == 2 && type != NV_MULTITOUCH_MOVE)
			{
				((void(*)(int, int, int posX, int posY))(g_libGTASA + 0x00239D5C + 1))(type, 2, x3, y3); // AND_TouchEvent
			}
			else
			{
				((void(*)(int, int, int posX, int posY))(g_libGTASA + 0x00239D5C + 1))(NV_MULTITOUCH_MOVE, 2, x3, y3); // AND_TouchEvent
			}
		}
	}

	return ret;
}

void(*CStreaming__Init2)();
void CStreaming__Init2_hook()
{
	CStreaming__Init2();
	*(uint32_t*)(g_libGTASA + 0x005DE734) = 536870912;
}
void decrypt(const char* encrypted_hex, const char* key, char* decrypted);
void (*NvUtilInit)(void);
void NvUtilInit_hook(void)
{	
	NvUtilInit();
	*(char**)(g_libGTASA + 0x5D1608) = OBFUSCATE("/storage/emulated/0/DynamicMobileNew/files/");
}
void (*RejectMainMenu)(uintptr_t* thiz);
void RejectMainMenu_hook(uintptr_t* thiz)
{
	  //if(callJavaMenu) 
	  //{ // .. Показываем паузу из джавы
		//  return g_pJavaWrapper->InitPause();
	  //}
      return g_pJavaWrapper->ShowNotification(2, "", 10, "", "");
}
int (*CTextureDatabaseRuntime__GetEntry)(unsigned int a1, const char *a2, bool *a3, int a4);
int CTextureDatabaseRuntime__GetEntry_hook(unsigned int a1, const char *a2, bool *a3, int a4)
{
  int result; // r0

  if ( a1 )
    result = CTextureDatabaseRuntime__GetEntry(a1, a2, a3, a4);
  else
    result = -1;
  return result;
}
std::string encrypt(const char* text, AES_ctx* ctx) {
  int text_length = strlen(text);
  int padding_size = AES_BLOCK_SIZE - (text_length % AES_BLOCK_SIZE);
  int total_length = text_length + padding_size;

  // Выделяем память для зашифрованного текста с учетом padding
  uint8_t* encrypted_data = new uint8_t[total_length];
  memcpy(encrypted_data, text, text_length);

  // Добавляем padding
  for (int i = 0; i < padding_size; i++) {
    encrypted_data[text_length + i] = padding_size;
  }

  // Шифруем данные
  AES_CBC_encrypt_buffer(ctx, encrypted_data, total_length);

  // Преобразуем зашифрованный текст в строку
  std::string encrypted_text(reinterpret_cast<char*>(encrypted_data), total_length);

  // Освобождаем память
  delete[] encrypted_data;

  return encrypted_text;
}
//int objectFull = 0;
std::queue<std::tuple<uintptr_t*, int, int>> objectQueue;
void (*CObject__CObject)(uintptr_t* thiz, int a2, int a3, int a4);
void CObject__CObject_hook(uintptr_t* thiz, int a2, int a3, int a4)
{
    uintptr_t *dwModelArray2 = (uintptr_t*)(g_libGTASA+0x87BF48);

    float loadDistance = (float)(*(float *)(dwModelArray2[a2] + 0x30));

  //  Log("load object %d, index %d, load distance(from ide): %f", a2, objectFull, a3, a4, loadDistance);
  //  ++objectFull;

    
 //   *(float *)(dwModelArray2[a2] + 0x30) = 1500.0;
    

    if (dwModelArray2[a2] == 0 || !dwModelArray2[a2])
    {
        // Если загрузка текущего объекта не удалась, добавляем его в очередь
		
        CObject__CObject(thiz, 18631, a3, a4);
    }
    else
    {
        CObject__CObject(thiz, a2, a3, a4);
    }
}

void (*CStreaming__RequestModel)(int thiz, int a2, int a3);
void CStreaming__RequestModel_hook(int thiz, int a2, int a3)
{
    uintptr_t *dwModelArray2 = (uintptr_t*)(g_libGTASA+0x87BF48);

    //Log("load object %d, index %d, load distance(from ide): %f", a2, objectFull, a3, a4, loadDistance);
  //  ++objectFull;

    
 //   *(float *)(dwModelArray2[a2] + 0x30) = 1500.0;
    

    if (!dwModelArray2[thiz])
    {
        // Если загрузка текущего объекта не удалась, добавляем его в очередь
		Log("load %d failed!", thiz);
        CStreaming__RequestModel(122, a2, a3);
    }
    else
    {
        CStreaming__RequestModel(thiz, a2, a3);
    }
}

int (*CQuadTreeNode__FindSector)(int a1, float *a2);
int CQuadTreeNode__FindSector_hook(int a1, float *a2)
{
    return 3;
}





int (*CFileMgr__OpenFile)(/*uintptr_t* thiz, */const char* a2, const char* a3);
int CFileMgr__OpenFile_hook(/*uintptr_t* thiz, */const char* a2, const char* a3)
{
	Log("Open File: %s", a2);
    return CFileMgr__OpenFile(/*thiz, */a2, a3);
}






//void SetUpGLHooks();
//void InstallHookFixes();
#include "../game/GTASA/Models/ModelInfo.h"
#include "../gtare/CFileMgr.h"
void InstallSpecialHooks()
{
	
	CStreaming::InjectHooks();
	
	//WriteMemory(g_libGTASA + 0x003AF1CA,
	//	(uintptr_t)"\x44\x7B\x4B\x1B\xAF\x00\xB4\x80", 8); //unlock limit PtrSingleLink
	//SetUpHook(g_libGTASA + 0x3B05AC, (uintptr_t)CQuadTreeNode__FindSector_hook, (uintptr_t*)&CQuadTreeNode__FindSector); 
	//	CHook::InlineHook(g_libGTASA, 0x403E00, &sub_46FE8, &dword_629574);
                  //CModelInfo::InjectHooks();
				//  SetUpHook(g_libGTASA + 0x3EE198, (uintptr_t)sub_4A3A8_hook, (uintptr_t*)&sub_4A3A8); //cobject init
	SetUpHook(g_libGTASA + 0x3AF1A0, (uintptr_t)CPools__Initialise_hook, (uintptr_t*)&CPools__Initialise); 
	SetUpHook(g_libGTASA + 0x3EE540, (uintptr_t)CObject__CObject_hook, (uintptr_t*)&CObject__CObject); //cobject init
//	SetUpHook(g_libGTASA + 0x28EB10, (uintptr_t)CStreaming__RequestModel_hook, (uintptr_t*)&CStreaming__RequestModel); //cobject init
				CHook::InlineHook(g_libGTASA, 0x3960DC, &CFileMgr__OpenFile_hook, &CFileMgr__OpenFile);
	CHook::InlineHook(g_libGTASA, 0x1bdc3c, &CTextureDatabaseRuntime__GetEntry_hook, &CTextureDatabaseRuntime__GetEntry);
                  //InstallHookFixes();
                  //SetUpGLHooks();
				 // CHook::RET(g_libGTASA + 0x390AA8);
	SetUpHook(g_libGTASA + 0x1BF244, (uintptr_t) TextureDatabaseRuntime_Load_hook, (uintptr_t *) &TextureDatabaseRuntime_Load);
	WriteMemory(g_libGTASA + 0x0023BEDC, (uintptr_t)"\xF8\xB5", 2);
	WriteMemory(g_libGTASA + 0x0023BEDE, (uintptr_t)"\x00\x46\x00\x46", 4);

	SetUpHook(g_libGTASA + 0x0023BEDC, (uintptr_t)OS_FileRead_hook, (uintptr_t*)& OS_FileRead);
	SetUpHook(g_libGTASA + 0x23B3DC, (uintptr_t)NvFOpen_hook, (uintptr_t*)& NvFOpen);
	SetUpHook(g_libGTASA + 0x241D94, (uintptr_t) NvUtilInit_hook, (uintptr_t *) &NvUtilInit);
	
//SetUpHook(g_libGTASA+0x28E6C8, (uintptr_t)rqinitbebe_hook, (uintptr_t*)&rqinitbebe);
	SetUpHook(g_libGTASA+0x269974, (uintptr_t)MenuItem_add_hook, (uintptr_t*)&MenuItem_add);
	SetUpHook(g_libGTASA+0x4D3864, (uintptr_t)CText_Get_hook, (uintptr_t*)&CText_Get);
	SetUpHook(g_libGTASA+0x40C530, (uintptr_t)InitialiseRenderWare_hook, (uintptr_t*)&InitialiseRenderWare);
	//SetUpHook(g_libGTASA+0x3362A8, (uintptr_t)CModelInfo_Init_Hook, (uintptr_t*)&CModelInfo_Init);
	SetUpHook(g_libGTASA + 0x0025E660, (uintptr_t)MainMenuScreen__Update_hook, (uintptr_t*)& MainMenuScreen__Update);
	SetUpHook(g_libGTASA + 0x0023BB84, (uintptr_t)OS_FileOpen_hook, (uintptr_t*)& OS_FileOpen);
	
	SetUpHook(g_libGTASA+0x3D7CA8, (uintptr_t)CLoadingScreen_DisplayPCScreen_hook, (uintptr_t*)&CLoadingScreen_DisplayPCScreen);
	SetUpHook(g_libGTASA+0x39AEF4, (uintptr_t)Render2dStuff_hook, (uintptr_t*)&Render2dStuff);
	SetUpHook(g_libGTASA+0x39B098, (uintptr_t)Render2dStuffAfterFade_hook, (uintptr_t*)&Render2dStuffAfterFade);
	
	CHook::InlineHook(g_libGTASA, 0x001BDCC4, &LoadFullTexture_hook, &LoadFullTexture);

	SetUpHook(g_libGTASA + 0x004FBBB0, (uintptr_t)cHandlingDataMgr__FindExactWord_hook, (uintptr_t*)& cHandlingDataMgr__FindExactWord);
	SetUpHook(g_libGTASA + 0x004FBCF4, (uintptr_t)cHandlingDataMgr__ConvertDataToGameUnits_hook, (uintptr_t*)& cHandlingDataMgr__ConvertDataToGameUnits);
	SetUpHook(g_libGTASA + 0x0023ACC4, (uintptr_t)NVEventGetNextEvent_hook, (uintptr_t*)& NVEventGetNextEvent_hooked);
	SetUpHook(g_libGTASA + 0x004042A8, (uintptr_t)CStreaming__Init2_hook, (uintptr_t*)& CStreaming__Init2);	// increase stream memory value
	
	//CHook::RET(g_libGTASA + 0x51CEF4);

}

void ProcessPedDamage(PED_TYPE* pIssuer, PED_TYPE* pPlayer);

uintptr_t (*ComputeDamageResponse)(uintptr_t, uintptr_t, int, int);
uintptr_t ComputeDamageResponse_hooked(uintptr_t issuer, uintptr_t ped, int a3, int a4)
{
	ProcessPedDamage((PED_TYPE*)*(uintptr_t*)issuer, (PED_TYPE*)ped );
	return ComputeDamageResponse(issuer, ped, a3, a4);
}

int(*RwFrameAddChild)(int, int);
int RwFrameAddChild_hook(int a1, int a2)
{
	if (a2 && a1)
	{
		return RwFrameAddChild(a1, a2);
	}
	else return 0;
}

void (*CAnimManager__UncompressAnimation)(int, int);
void CAnimManager__UncompressAnimation_hook(int a1, int a2)
{
	if (a1)
	{
		CAnimManager__UncompressAnimation(a1, a2);
	}
}

void(*CCustomRoadsignMgr__RenderRoadsignAtomic)(int, int);
void CCustomRoadsignMgr__RenderRoadsignAtomic_hook(int a1, int a2)
{
	if (a1)
	{
		CCustomRoadsignMgr__RenderRoadsignAtomic(a1, a2);
	}
}

int(*CUpsideDownCarCheck__IsCarUpsideDown)(int, int);
int CUpsideDownCarCheck__IsCarUpsideDown_hook(int a1, int a2)
{
	if (*(uintptr_t*)(a2 + 20))
	{
		return CUpsideDownCarCheck__IsCarUpsideDown(a1, a2);
	}
	return 0;
}

int(*CAnimBlendNode__FindKeyFrame)(int, float, int, int);
int CAnimBlendNode__FindKeyFrame_hook(int a1, float a2, int a3, int a4)
{
	if (*(uintptr_t*)(a1 + 16))
	{
		return CAnimBlendNode__FindKeyFrame(a1, a2, a3, a4);
	}
	else return 0;
}


/* ====================================================== */

typedef struct _ATOMIC_MODEL
{
	uintptr_t 	vtable;
	uint8_t		data[52];
} ATOMIC_MODEL; // SIZE = 56

ATOMIC_MODEL AtomicModels[20000];
int AtomicModelsCount = 0;

ATOMIC_MODEL* (*CModelInfo_AddAtomicModel)(int id);
ATOMIC_MODEL* CModelInfo_AddAtomicModel_hook(int id)
{
	//Log("Load Object %d...", id);
	ATOMIC_MODEL* model = &AtomicModels[AtomicModelsCount];
	memset(model, 0, sizeof(ATOMIC_MODEL));

	((void(*)(void* thiz))(g_libGTASA + 0x0033559C + 1))((void*)model); // CBaseModelInfo::CBaseModelInfo();

	model->vtable = (uintptr_t)(g_libGTASA + 0x005C6C68); // assign CAtomicModelInfo vmt

	((uintptr_t(*)(ATOMIC_MODEL*))(*(void**)(model->vtable + 0x1C)))(model); // CClumpModelInfo::Init()

	*(ATOMIC_MODEL * *)(g_libGTASA + 0x87BF48 + (id * 4)) = model; // CModelInfo::ms_modelInfoPtrs

	AtomicModelsCount++;
	//Log("Object %d loaded!", id);
	return model;
}
void LoadObjectAtomic(int id)
{
	ATOMIC_MODEL* model = &AtomicModels[AtomicModelsCount];
	memset(model, 0, sizeof(ATOMIC_MODEL));

	((void(*)(void* thiz))(g_libGTASA + 0x0033559C + 1))((void*)model); // CBaseModelInfo::CBaseModelInfo();

	model->vtable = (uintptr_t)(g_libGTASA + 0x005C6C68); // assign CAtomicModelInfo vmt

	((uintptr_t(*)(ATOMIC_MODEL*))(*(void**)(model->vtable + 0x1C)))(model); // CClumpModelInfo::Init()

	*(ATOMIC_MODEL * *)(g_libGTASA + 0x87BF48 + (id * 4)) = model; // CModelInfo::ms_modelInfoPtrs

	AtomicModelsCount++;
}
/* ====================================================== */
/*typedef struct _VEHICLE_MODEL
{
	uintptr_t 	vtable;
	uint8_t		data[932];
} VEHICLE_MODEL; // SIZE = 936

VEHICLE_MODEL VehicleModels[370];
int VehicleModelsCount = 0;

VEHICLE_MODEL* (*CModelInfo_AddVehicleModel)(int id);
VEHICLE_MODEL* CModelInfo_AddVehicleModel_hook(int id)
{

	
	VEHICLE_MODEL* model = &VehicleModels[VehicleModelsCount];
	memset(model, 0, sizeof(VEHICLE_MODEL));

	((void(*)(void* thiz))(g_libGTASA + 0x00337AA0 + 1))((void*)model); // CVehicleModelInfo::CVehicleModelInfo();

	model->vtable = (uintptr_t)(g_libGTASA + 0x005C6EE0);			// assign CVehicleModelInfo vmt

	((uintptr_t(*)(VEHICLE_MODEL*))(*(void**)(model->vtable + 0x1C)))(model); // CVehicleModelInfo::Init()

	*(VEHICLE_MODEL * *)(g_libGTASA + 0x87BF48 + (id * 4)) = model; // CModelInfo::ms_modelInfoPtrs

	VehicleModelsCount++;
	return model;
}
/* ====================================================== */
void(*CHud__DrawScriptText)(uintptr_t, uint8_t);

float g_fMicrophoneButtonPosX;
float g_fMicrophoneButtonPosY;
bool g_IsVoiceServer();
void CHud__DrawScriptText_hook(uintptr_t thiz, uint8_t unk)
{
	CHud__DrawScriptText(thiz, unk);
	if (pGame && pNetGame)
	{
	

		if (g_pWidgetManager)
		{
			ImGuiIO& io = ImGui::GetIO();

			/*if (!g_pWidgetManager->GetSlotState(WIDGET_CHATHISTORY_UP))
			{
				g_pWidgetManager->New(WIDGET_CHATHISTORY_UP, 1700.0f, (io.DisplaySize.y * 0.3) - 180.0f, 130.0f, 130.0f, "menu_up");
				g_pWidgetManager->GetWidget(WIDGET_CHATHISTORY_UP)->SetPosWithoutScale(pGUI->ScaleX(1325.0f), io.DisplaySize.y * 0.3);
			}
			if (!g_pWidgetManager->GetSlotState(WIDGET_CHATHISTORY_DOWN))
			{
				g_pWidgetManager->New(WIDGET_CHATHISTORY_DOWN, 1700.0f, (io.DisplaySize.y * 0.3) - 30, 130.0f, 130.0f, "menu_down");
				g_pWidgetManager->GetWidget(WIDGET_CHATHISTORY_DOWN)->SetPosWithoutScale(pGUI->ScaleX(1515.0f), io.DisplaySize.y * 0.3);
			}
			if (!g_pWidgetManager->GetSlotState(WIDGET_MICROPHONE))
			{
				/*if (g_IsVoiceServer())
				{
					// pVoice
					g_fMicrophoneButtonPosX = pSettings->GetReadOnly().fButtonMicrophoneX;
					g_fMicrophoneButtonPosY = pSettings->GetReadOnly().fButtonMicrophoneY;
					g_pWidgetManager->New(WIDGET_MICROPHONE, 
						pSettings->GetReadOnly().fButtonMicrophoneX, 
						pSettings->GetReadOnly().fButtonMicrophoneY,
						pSettings->GetReadOnly().fButtonMicrophoneSize,
						pSettings->GetReadOnly().fButtonMicrophoneSize, "hud_dance");
					g_pWidgetManager->GetWidget(WIDGET_MICROPHONE)->SetTexture("samp", "voice_button");
				}*/
			/*}
*/
			if (!g_pWidgetManager->GetSlotState(WIDGET_CAMERA_CYCLE))
			{
				g_pWidgetManager->New(WIDGET_CAMERA_CYCLE,
					pSettings->GetReadOnly().fButtonCameraCycleX,
					pSettings->GetReadOnly().fButtonCameraCycleY,
					pSettings->GetReadOnly().fButtonCameraCycleSize,
					pSettings->GetReadOnly().fButtonCameraCycleSize, "cam-toggle");
				// cam-toggle
			}
		}
	}
}

#include "..//keyboard.h"
extern CKeyBoard* pKeyBoard;
int(*CWidgetButtonEnterCar_Draw)(uintptr_t);
uint32_t g_uiLastTickVoice = 0;
int CWidgetButtonEnterCar_Draw_hook(uintptr_t thiz)
{
	if (g_pWidgetManager)
	{
		CWidget* pWidget = g_pWidgetManager->GetWidget(WIDGET_CHATHISTORY_UP);
		if (pWidget)
		{
			pWidget->SetDrawState(false);
		}
		pWidget = g_pWidgetManager->GetWidget(WIDGET_CHATHISTORY_DOWN);
		if (pWidget)
		{
			pWidget->SetDrawState(false);
		}

		pWidget = g_pWidgetManager->GetWidget(WIDGET_CAMERA_CYCLE);
		if (pWidget)
		{
			pWidget->SetDrawState(true);
		}


		if (!pGame->IsToggledHUDElement(HUD_ELEMENT_BUTTONS))
		{
			for (int i = 0; i < MAX_WIDGETS; i++)
			{
				CWidget* pWidget = g_pWidgetManager->GetWidget(i);
				if (pWidget)
				{
					pWidget->SetDrawState(false);
				}
			}
		}

		g_pWidgetManager->Draw();
	}
	return CWidgetButtonEnterCar_Draw(thiz);
}

uint64_t(*CWorld_ProcessPedsAfterPreRender)();
uint64_t CWorld_ProcessPedsAfterPreRender_hook()
{
	uint64_t res = CWorld_ProcessPedsAfterPreRender();
	if (pNetGame && pNetGame->GetPlayerPool())
	{
		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			CPlayerPed* pPed = nullptr;
			if (pNetGame->GetPlayerPool()->GetLocalPlayerID() == i)
			{
				pPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
			}
			else
			{
				if (pNetGame->GetPlayerPool()->GetSlotState(i))
				{
					pPed = pNetGame->GetPlayerPool()->GetAt(i)->GetPlayerPed();
				}
			}
			if (!pPed) continue;
			else
			{
				pPed->ProcessAttach();
			}
		}
	}

	return res;
}

int RemoveModelIDs[MAX_REMOVE_MODELS];
VECTOR RemovePos[MAX_REMOVE_MODELS];
float RemoveRad[MAX_REMOVE_MODELS];
int iTotalRemovedObjects = 0;


#include "util.h"

int(*CFileLoader__LoadObjectInstance)(uintptr_t, uintptr_t);
int CFileLoader__LoadObjectInstance_hook(uintptr_t thiz, uintptr_t name)
{
	for (int i = 0; i < iTotalRemovedObjects; i++)
	{
		if (RemoveModelIDs[i] == *(uint32_t*)(thiz + 28))
		{
			VECTOR pos;
			pos.X = *(float*)(thiz);
			pos.Y = *(float*)(thiz + 4);
			pos.Z = *(float*)(thiz + 8);
			if (GetDistanceBetween3DPoints(&pos, &RemovePos[i]) <= RemoveRad[i])
			{
				*(int*)(thiz + 28) = 19300;
			}
		}
	}
	return CFileLoader__LoadObjectInstance(thiz, name);
}

#include <list>

std::list<std::pair<unsigned int*, unsigned int>> resetEntries;

static uint32_t Color32Reverse(uint32_t x)
{
	return
		((x & 0xFF000000) >> 24) |
		((x & 0x00FF0000) >> 8) |
		((x & 0x0000FF00) << 8) |
		((x & 0x000000FF) << 24);
}

static RwRGBA DWORD2RGBAinternal(uint32_t dwColor)
{
	RwRGBA tmp;

	tmp.blue = dwColor & 0xFF; dwColor >>= 8;
	tmp.green = dwColor & 0xFF; dwColor >>= 8;
	tmp.red = dwColor & 0xFF; dwColor >>= 8;
	tmp.alpha = dwColor & 0xFF; /* dwColor >>= 8; */

	return tmp;
}

uintptr_t RwFrameForAllObjectsCALLBACK1(uintptr_t object, CObject* pObject)
{
	if (*(uint8_t*)object != 1)
	{
		return object;
	}
	uintptr_t pAtomic = object;
	RpGeometry* pGeom = *(RpGeometry * *)(pAtomic + 24);
	if (!pGeom)
	{
		return object;
	}

	int numMats = pGeom->matList.numMaterials;
	if (numMats > 16)
	{
		numMats = 16;
	}
	for (int i = 0; i < numMats; i++)
	{
		RpMaterial* pMat = pGeom->matList.materials[i];
		if (!pMat)
		{
			continue;
		}
		if (pObject->m_pMaterials[i].m_bCreated)
		{
			RpMaterial* pMat = pGeom->matList.materials[i];
			if (!pMat)
			{
				continue;
			}
			if (pObject->m_pMaterials[i].pTex)
			{
				resetEntries.push_back(std::make_pair(reinterpret_cast<unsigned int*>(&pMat->texture), *reinterpret_cast<unsigned int*>(&pMat->texture)));
				pMat->texture = pObject->m_pMaterials[i].pTex;
			}
			/*if (pObject->m_pMaterials[i].dwColor)
			{
				//((int(*)())(g_libGTASA + 0x00559FC8 + 1))();
				resetEntries.push_back(std::make_pair(reinterpret_cast<unsigned int*>(&pGeom->flags), *reinterpret_cast<unsigned int*>(&pGeom->flags)));
				pGeom->flags |= 0x00000040;
				pGeom->flags &= 0xFFFFFFF7;
				RwRGBA r = DWORD2RGBAinternal(pObject->m_pMaterials[i].dwColor);

				resetEntries.push_back(std::make_pair(reinterpret_cast<unsigned int*>(&pMat->color), *reinterpret_cast<unsigned int*>(&pMat->color)));
				pMat->color = r;
				pMat->surfaceProps.ambient = 1.0f;
				pMat->surfaceProps.specular = 0.0f;
				pMat->surfaceProps.diffuse = 1.0f;
			}*/
		}
	}
	return object;
}

int g_iLastRenderedObject;
void(*CObject__Render)(ENTITY_TYPE*);
void CObject__Render_hook(ENTITY_TYPE* thiz)
{
	// 0051ABB0 + 1
	// 004353FE + 1
	// 004352C4 + 1

	if (CSkyBox::GetSkyObject())
	{
		if (CSkyBox::GetSkyObject()->m_pEntity == thiz && !CSkyBox::IsNeedRender())
			return;
	}

	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;

//	if (dwRetAddr == 0x0051ABB0 + 1 || dwRetAddr == 0x004353FE + 1 || dwRetAddr == 0x004352C4 + 1)
//	{
//		return CObject__Render(thiz);
//	}

	uintptr_t pAtomic = thiz->m_RwObject;
	if (!pAtomic)
	{
		return CObject__Render(thiz); // CObject::Render(CObject*)
	}
	if (!*(uintptr_t*)(pAtomic + 4))
	{
		return CObject__Render(thiz); // CObject::Render(CObject*)
	}

	if (pNetGame)
	{
		CObjectPool* pObjectPool = pNetGame->GetObjectPool();
		if (pObjectPool)
		{
			CObject* pObject = pObjectPool->GetObjectFromGtaPtr(thiz);
			if (pObject)
			{
				if (pObject->m_pEntity)
				{
					g_iLastRenderedObject = pObject->m_pEntity->nModelIndex;
				}
				
				((void(*)())(g_libGTASA + 0x00559EF8 + 1))(); // DeActivateDirectional
				if (pObject->m_bMaterials)
				{
					//thiz->m_bLightObject = 0;
					((uintptr_t(*)(uintptr_t, uintptr_t, uintptr_t))(g_libGTASA + 0x001AEE2C + 1))(*(uintptr_t*)(pAtomic + 4), (uintptr_t)RwFrameForAllObjectsCALLBACK1, (uintptr_t)pObject); // RwFrameForAllObjects
					//((void(*)(ENTITY_TYPE*, bool))(g_libGTASA + 0x003B1F1C + 1))(thiz, 1); // CObject::RemoveLighting(CObject*, bool)
					//((void(*)())(g_libGTASA + 0x00559EF8 + 1))(); // DeActivateDirectional

				}
			}
			
		}
	}

	CObject__Render(thiz); // CObject::Render(CObject*)
	for (auto& p : resetEntries)
		* p.first = p.second;
	resetEntries.clear();
}


void (*CRenderer__RenderEverythingBarRoads)();
void CRenderer__RenderEverythingBarRoads_hook()
{
	CSkyBox::Process();

                  //pClientLogic->UpdateSnow();
//	updateReflections();
	//Log("gtasa xuetan: %d", *(int *)(g_libGTASA + 0x88FFF0));
	//*(int *)(g_libGTASA + 0x88FFF0) = 9999999999999;
	CRenderer__RenderEverythingBarRoads();
}

#pragma optimize( "", off )

char CStreaming__ConvertBufferToObject_hook(int a1, int a2, int a3)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));
	

	uint32_t tickStart = GetTickCount();
	//CGameResourcesDecryptor::CStreaming__ConvertBufferToObject_hook((char*)a1, a2, a3);
	if (a2 >= 15000 && a2 <= 15100)
	{
		//pChatWindow->AddDebugMessage("loading time %d", GetTickCount() - tickStart);
	}
	char a12 = CStreaming__ConvertBufferToObject(a1, a2, a3);
	return a12;
}


/*signed int (*OS_FileOpen)(unsigned int a1, int* a2, const char* a3, int a4);
signed int OS_FileOpen_hook(unsigned int a1, int* a2, const char* a3, int a4)
{
	uintptr_t calledFrom = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (calledFrom));
	calledFrom -= g_libGTASA;
	signed int retn = OS_FileOpen(a1, a2, a3, a4);

	if (calledFrom == 0x001BCE9A + 1)
	{
		if (isEncrypted(a3))
		{
			lastOpenedFile = *a2;
		}
	}
	return retn;
}*/

// CGameIntegrity
// CodeObfuscation



static uint32_t dwRLEDecompressSourceSize = 0;

int(*OS_FileRead)(void* a1, void* a2, int a3);

int OS_FileRead_hook(void* a1, void* a2, int a3)
{
	if(!a1) return 1;
	uintptr_t calledFrom = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (calledFrom));
	calledFrom -= g_libGTASA;

	if (!a3)
	{
		return 0;
	}
	//int fileSize = a3;
	
	//Log("last open load..");
	if (calledFrom == 0x001BCEE0 + 1 && a1 == (void*)lastOpenedFile)
	{
		lastOpenedFile = 0;

        AES_ctx ctx;
		InitCTX(ctx, &g_iEncryptionKeyTXD[6]);

        size_t retv = OS_FileRead(a1, a2, a3);
        int fileSize = a3;
        int iters = fileSize / PART_SIZE_TXD;
        uintptr_t pointer = (uintptr_t)a2;
        for (int i = 0; i < iters; i++)
        {
            AES_CBC_decrypt_buffer(&ctx, (uint8_t *)pointer, PART_SIZE_TXD);
            pointer += PART_SIZE_TXD;
        }
        return retv;
	}

	if (calledFrom == 0x001BDD34 + 1)
	{
		int retn = OS_FileRead(a1, a2, a3);

		dwRLEDecompressSourceSize = *(uint32_t*)a2;

		return retn;
	}
	else
	{
		int retn = OS_FileRead(a1, a2, a3);

		return retn;
	}
}

/*int(*OS_FileRead)(void* a1, void* a2, int a3);
int OS_FileRead_hook(void* a1, void* a2, int a3)
{
	uintptr_t calledFrom = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (calledFrom));
	calledFrom -= g_libGTASA;

	if (!a3)
	{
		return 0;
	}

	if (calledFrom == 0x001BCEE0 + 1 && a1 == (void*)lastOpenedFile)
	{
		lastOpenedFile = 0;
		CTinyEncrypt tinyEnc;
		tinyEnc.SetKey(&g_iEncryptionKeyVersion2TXD[0]);
		int retv = OS_FileRead(a1, a2, a3);
		int fileSize = a3;
		int iters = fileSize / PART_SIZE_TXD;
		uintptr_t pointer = (uintptr_t)a2;
		for (int i = 0; i < iters; i++)
		{
			tinyEnc.DecryptData((void*)pointer, PART_SIZE_TXD, 16);
			pointer += PART_SIZE_TXD;
		}
		return retv;
	}
	else
	{
		int retn = OS_FileRead(a1, a2, a3);

		return retn;
	}
}*/

#include <sstream>
char(*CFileMgr__ReadLine)(int, int, int);
char CFileMgr__ReadLine_hook(int a1, int a2, int a3)
{
	char retv = CFileMgr__ReadLine(a1, a2, a3);
	char* pStr = (char*)a2;
	int value = *(int*)pStr;
	if (value == g_unobfuscate(g_iIdentifierVersion2))
	{
		pStr += 4;
		int length = *(int*)pStr;
		pStr -= 4;
		memcpy((void*)pStr, (const void*)& pStr[8], length);

		pStr[length] = 0;
		std::stringstream ss;

		uint32_t keyi = g_unobfuscate(g_i64Encrypt);

		ss << keyi;

		std::string key(ss.str());
		std::string val(pStr);

		std::string decr = decrypt(val, key);

		strcpy((char*)a2, decr.c_str());
	}
	return retv;
}

#pragma optimize( "", on )

uintptr_t(*GetTexture_orig)(const char*);
uintptr_t GetTexture_hook(const char* a1)
{
	auto tex = ((uintptr_t(*)(const char*))(g_libGTASA + 0x001BE990 + 1))(a1);
    if (!tex)
    {
        tex = LoadTextureFromDB("samp", "replaceme");
        Log("TEXTURE NOT FOUND = %s", a1);
    }
    ++* (uintptr_t*)(tex + 84);

    return tex;
}
uintptr_t(*CPlayerInfo__FindObjectToSteal)(uintptr_t, uintptr_t);
uintptr_t CPlayerInfo__FindObjectToSteal_hook(uintptr_t a1, uintptr_t a2)
{
	return 0;
}
//typedef uintptr_t RpClump;
RwFrame* CClumpModelInfo_GetFrameFromId_Post(RwFrame* pFrameResult, RpClump* pClump, int id)
{
	if (pFrameResult)
		return pFrameResult;

	uintptr_t calledFrom = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (calledFrom));
	calledFrom -= g_libGTASA;

	//                                 
	if (calledFrom == 0x00515708                // CVehicle::SetWindowOpenFlag
		|| calledFrom == 0x00515730             // CVehicle::ClearWindowOpenFlag
		|| calledFrom == 0x00338698             // CVehicleModelInfo::GetOriginalCompPosition
		|| calledFrom == 0x00338B2C)            // CVehicleModelInfo::CreateInstance
		return NULL;

	for (uint i = 2; i < 40; i++)
	{
		RwFrame* pNewFrameResult = NULL;
		uint     uiNewId = id + (i / 2) * ((i & 1) ? -1 : 1);
		
		pNewFrameResult = ((RwFrame * (*)(RpClump * pClump, int id))(g_libGTASA + 0x00335CC0 + 1))(pClump, i);

		if (pNewFrameResult)
		{
			return pNewFrameResult;
		}
	}

	return NULL;
}
RwFrame* (*CClumpModelInfo_GetFrameFromId)(RpClump*, int);
RwFrame* CClumpModelInfo_GetFrameFromId_hook(RpClump* a1, int a2)
{
	return CClumpModelInfo_GetFrameFromId_Post(CClumpModelInfo_GetFrameFromId(a1, a2), a1, a2);
}
void (*CWidgetRegionLook__Update)(uintptr_t thiz);
void CWidgetRegionLook__Update_hook(uintptr_t thiz)
{
	if (bBlockCWidgetRegionLookUpdate)
	{
		return;
	}
	else
	{
		return CWidgetRegionLook__Update(thiz);
	}
}
uint8_t* (*RLEDecompress)(uint8_t* pDest, size_t uiDestSize, uint8_t const* pSrc, size_t uiSegSize, uint32_t uiEscape);
uint8_t* RLEDecompress_hook(uint8_t* pDest, size_t uiDestSize, uint8_t const* pSrc, size_t uiSegSize, uint32_t uiEscape) {
	if (!pDest) 
	{
		return pDest;
	}

	if (!pSrc) 
	{
		return pDest;
	}

	uint8_t* pTempDest = pDest;
	const uint8_t* pTempSrc = pSrc;
	uint8_t* pEndOfDest = &pDest[uiDestSize];

	uint8_t* pEndOfSrc = (uint8_t*)&pSrc[dwRLEDecompressSourceSize];

	if (pDest < pEndOfDest) 
	{
		do 
		{
			if (*pTempSrc == uiEscape) 
			{
				uint8_t ucCurSeg = pTempSrc[1];
				if (ucCurSeg) 
				{
					uint8_t* ucCurDest = pTempDest;
					uint8_t ucCount = 0;
					do 
					{
						++ucCount;
						//Log("WRITE111 TO 0x%x FROM 0x%x SIZE %d", ucCurDest, pTempSrc + 2, uiSegSize);
						pDest = (uint8_t*)memcpy(ucCurDest, pTempSrc + 2, uiSegSize);
						
						ucCurDest += uiSegSize; //crash
					} while (ucCurSeg != ucCount);


					pTempDest += uiSegSize * ucCurSeg;
				}
				pTempSrc += 2 + uiSegSize;
			}

			else 
			{
				if (pTempSrc + uiSegSize >= pEndOfSrc)
				{
					//Log("AVOID CRASH TO 0x%x FROM 0x%x SIZE %d END OF SRC 0x%x", pTempDest, pTempSrc, pEndOfSrc - pTempSrc - 1, pEndOfSrc);
					return pDest;
				}
				else
				{
					//Log("WRITE222 TO 0x%x FROM 0x%x SIZE %d END OF SRC 0x%x", pTempDest, pTempSrc, uiSegSize, pEndOfSrc);
					pDest = (uint8_t*)memcpy(pTempDest, pTempSrc, uiSegSize);
					pTempDest += uiSegSize;
					pTempSrc += uiSegSize;
				}
			}
		} while (pEndOfDest > pTempDest);
	}

	dwRLEDecompressSourceSize = 0;

	return pDest;
}

#include "..//crashlytics.h"

char g_bufRenderQueueCommand[200];
uintptr_t g_dwRenderQueueOffset;
int dword_6295A4 = -2105696048;
char* (*RenderQueue__ProcessCommand)(uintptr_t thiz, char* a2);
char* RenderQueue__ProcessCommand_hook(uintptr_t thiz, char* a2)
{
	return RenderQueue__ProcessCommand(thiz, a2);
	if (thiz && a2)
	{
		uintptr_t* dwRenderQueue = (uintptr_t*)thiz;

		memset(g_bufRenderQueueCommand, 0, sizeof(g_bufRenderQueueCommand));

		g_dwRenderQueueOffset = *(uintptr_t*)a2;
		snprintf(g_bufRenderQueueCommand, 190, "offset: %d | name: %s", g_dwRenderQueueOffset, (const char*)(*(dwRenderQueue + 100 + g_dwRenderQueueOffset)));

		return RenderQueue__ProcessCommand(thiz, a2);
	}
	else
	{
		return nullptr;
	}
	
	
}

int (*_rwFreeListFreeReal)(int a1, unsigned int a2);
int _rwFreeListFreeReal_hook(int a1, unsigned int a2)
{
	if (a1)
	{
		return _rwFreeListFreeReal(a1, a2);
	}
	else
	{
		return 0;
	}
}

uintptr_t* CTouchInterface__m_bWidgets;

static bool ShouldBeProcessedButton(int result)
{
	CTouchInterface__m_bWidgets = (uintptr_t*)(g_libGTASA + 0x00657E48);

	if (result == CTouchInterface__m_bWidgets[18])
	{
		return false;
	}

	if (result == CTouchInterface__m_bWidgets[26] || result == CTouchInterface__m_bWidgets[27])
	{
		if (pNetGame)
		{
			CLocalPlayer* pLocal = pNetGame->GetPlayerPool()->GetLocalPlayer();
			if (pLocal)
			{
				if (pLocal->GetPlayerPed())
				{
					VEHICLE_TYPE* pVehicle = pLocal->GetPlayerPed()->GetGtaVehicle();
					if (pVehicle)
					{
						uintptr_t this_vtable = pVehicle->entity.vtable;
						this_vtable -= g_libGTASA;

						if (this_vtable == 0x5CCE60)
						{
							return 1;
						}
					}
				}
			}
		}
		return 0;
	}
	return 1;
}


int (*CWidget__IsTouched)(uintptr_t a1);
int CWidget__IsTouched_hook(uintptr_t a1)
{
	if (ShouldBeProcessedButton(a1))
	{
		return CWidget__IsTouched(a1);
	}
	else
	{
		return 0;
	}
}
void readVehiclesAudioSettings();
void (*CVehicleModelInfo__SetupCommonData)();
void CVehicleModelInfo__SetupCommonData_hook()
{
	CVehicleModelInfo__SetupCommonData();
	readVehiclesAudioSettings();
}

extern VehicleAudioPropertiesStruct VehicleAudioProperties[20000];
static uintptr_t addr_veh_audio = (uintptr_t)& VehicleAudioProperties[0];

void (*CAEVehicleAudioEntity__GetVehicleAudioSettings)(uintptr_t thiz, int16_t a2, int a3);
void CAEVehicleAudioEntity__GetVehicleAudioSettings_hook(uintptr_t dest, int16_t a2, int ID)
{
	memcpy((void*)dest, &VehicleAudioProperties[(ID - 400)], sizeof(VehicleAudioPropertiesStruct));
}
// https://beeg.com/1256386731?t=85

void (*CDarkel__RegisterCarBlownUpByPlayer)(void* pVehicle, int arg2);
void CDarkel__RegisterCarBlownUpByPlayer_hook(void* pVehicle, int arg2)
{
	return;
}
void (*CDarkel__ResetModelsKilledByPlayer)(int playerid);
void CDarkel__ResetModelsKilledByPlayer_hook(int playerid)
{
	return;
}
int(*CDarkel__QueryModelsKilledByPlayer)(int, int);
int CDarkel__QueryModelsKilledByPlayer_hook(int player, int modelid)
{
	return 0;
}

int (*CDarkel__FindTotalPedsKilledByPlayer)(int player);
int CDarkel__FindTotalPedsKilledByPlayer_hook(int player)
{
	return 0;
}

void (*CDarkel__RegisterKillByPlayer)(void* pKilledPed, unsigned int damageWeaponID, bool bHeadShotted, int arg4);
void CDarkel__RegisterKillByPlayer_hook(void* pKilledPed, unsigned int damageWeaponID, bool bHeadShotted, int arg4)
{
	return;
}

std::list<std::pair<unsigned int*, unsigned int>> resetEntriesVehicle;

RpMaterial* CVehicle__SetupRenderMatCB(RpMaterial* mat, void* data)
{
	auto pVeh = (CVehicle*)data;
	int color = *(int*)&mat->color;
	if (mat)
	{
	//	Log("color: %d %d %d", (int)mat->color.red, (int)mat->color.green, (int)mat->color.blue);
		if ( mat->color.red == 60 && mat->color.green == 255 && mat->color.blue == 0 ) //ABGR
		{ // first color
			resetEntriesVehicle.emplace_back(reinterpret_cast<unsigned int*>(&(mat->color)), *reinterpret_cast<unsigned int*>(&(mat->color)));
          //  if(pVeh->m_iVinylId != -1) {
         //       resetEntriesVehicle.emplace_back(reinterpret_cast<unsigned int*>(&(mat->texture)), *reinterpret_cast<unsigned int*>(&(mat->texture)));
         //       mat->texture = CVehicle::m_pVinyls[ pVeh->m_iVinylId ];
         //       mat->color = {255, 255, 255, 255};
          //      return mat;
          //  }
			mat->color = pVeh->mainColor;
			return mat;
		}
		//204 204 204 - шины
		/*else if (mat->color.red == 255 && mat->color.green ==255 && mat->color.blue == 255)
		{ // toner
			resetEntriesVehicle.emplace_back(reinterpret_cast<unsigned int*>(&(mat->color)), *reinterpret_cast<unsigned int*>(&(mat->color)));

			mat->color = pVeh->tonerColor;
			Log("upper - toner color.");
			return mat;
		}
		//8 8 8 - rear lights
		else if (mat->color.red == 255 && mat->color.green == 0 && mat->color.blue == 175) 
        { // wheel
	
            resetEntriesVehicle.emplace_back(reinterpret_cast<unsigned int*>(&(mat->color)), *reinterpret_cast<unsigned int*>(&(mat->color)));

          	mat->color = pVeh->wheelColor;
			Log("upper - wheel color.");
            return mat;
        }*/
	
	}
	return mat;
}


uintptr_t CVehicle__SetupRenderCB(uintptr_t atomic, void* data)
{
	if (*(RpGeometry * *)(atomic + 24))
	{
		((RpGeometry * (*)(RpGeometry*, uintptr_t, void*))(g_libGTASA + 0x001E284C + 1))(*(RpGeometry * *)(atomic + 24), (uintptr_t)CVehicle__SetupRenderMatCB, (void*)data); // RpGeometryForAllMaterials
	}

	return atomic;
}
#include "..//cryptors/MODELINFO_EDITABLE_result.h"
//typedef uintptr_t RpClump;
void(*CVehicleModelInfo__SetEditableMaterials)(uintptr_t);
void CVehicleModelInfo__SetEditableMaterials_hook(uintptr_t clump)
{
	PROTECT_CODE_MODELINFO_EDITABLE;
	RpClump* pClump = (RpClump*)clump;	

	if (pNetGame && pClump)
	{
		if (pNetGame->GetVehiclePool())
		{
			VEHICLEID vehId = pNetGame->GetVehiclePool()->FindIDFromRwObject((RwObject*)clump);
			CVehicle* pVehicle = pNetGame->GetVehiclePool()->GetAt(vehId);
			if (pVehicle)
			{
				if (pVehicle->m_bReplacedTexture)
				{
				/*FIXXX*/ RpClump* RpClumpForAllAtomics(RpClump* clump, RpAtomicCallBack callback, void* pData);
					((RpClump * (*)(RpClump*, uintptr_t, void*))(g_libGTASA + 0x001E0EA0 + 1))(pClump, (uintptr_t)CVehicle__SetupRenderCB, (void*)pVehicle); // RpClumpForAllAtomics
				}
			}
		}
	}

	CVehicleModelInfo__SetEditableMaterials(clump);
}

#include "..//cryptors/RESET_AFTER_RENDER_result.h"

void (*CVehicle__ResetAfterRender)(uintptr_t);
void CVehicle__ResetAfterRender_hook(uintptr_t thiz)
{
	PROTECT_CODE_RESET_AFTER_RENDER;

	for (auto& p : resetEntriesVehicle)
	{
		*p.first = p.second;
	}
	resetEntriesVehicle.clear();

	CVehicle__ResetAfterRender(thiz);
}

void (*CRopes__Update)();
void CRopes__Update_hook()
{
	
}

void* (*CCustomCarEnvMapPipeline__pluginEnvMatDestructorCB)(void* object, RwInt32 offset, RwInt32 size);
void* CCustomCarEnvMapPipeline__pluginEnvMatDestructorCB_hook(void* object, RwInt32 offset, RwInt32 size)
{
	if(pChatWindow) pChatWindow->AddDebugMessage("m_objects %x", *(uintptr_t * *)(g_libGTASA + 0x00669E48));
	return CCustomCarEnvMapPipeline__pluginEnvMatDestructorCB(object, offset, size);
}
int __fastcall sub_3E99C(int *a1)
{
  int v2; // r0
  int v3; // r0
  int v4; // r0
  int v5; // r5
  int v6; // r0
  int v7; // r0
  int v9[2]; // [sp+0h] [bp-48h] BYREF
  uint64_t v10; // [sp+8h] [bp-40h] BYREF
  int v11; // [sp+10h] [bp-38h]
  uint64_t v12[2]; // [sp+18h] [bp-30h] BYREF
  v2 = ((int (__fastcall *)(int))(0x1E3861 + g_libGTASA))(2);
 // a1[2] = v2;
  if ( !v2 )
    return 0;
  v12[1] = 0x3F8000003F800000LL;
  v12[0] = 0x3F8000003F800000LL;
  ((void (__fastcall *)(int, uint64_t *))(0x1E333D + g_libGTASA))(v2, v12);
  v3 = ((int (*)(void))(0x1AD6F5 + g_libGTASA + 0x328))();
//  a1[1] = v3;
  if ( !v3 )
    return 0;
  v4 = ((int (*)(void))(0x1AE9E1 + g_libGTASA))();
  v5 = 0;
//  a1[3] = v4;
  if ( v4 )
  {
    v11 = 0x42480000;
    v10 = 0LL;
    ((void (__fastcall *)(int, uint64_t *, int))(0x1AE9E1 + g_libGTASA + 0x39C))(v4, &v10, 1);
    v10 = 0x3F800000LL;
    v11 = 0;
    ((void (__fastcall *)(int, uint64_t *, int, int))(0x1AE9E1 + g_libGTASA + 0x3E4))(
      v4,
      &v10,
      0x42B40000,
      1);
    v6 = (int)RwRasterCreate(100, 100, 0, 1);
  //  a1[4] = v6;
    if ( v6 )
    {
    //  *(int *)(a1[1] + 0x64) = v6;
      ((void (__fastcall *)(int, int))(0x1B2989 + g_libGTASA))(v3, v4);
      ((void (__fastcall *)(int, int))(0x1AD6F5 + g_libGTASA + 0x1C))(v3, 0x43960000);
      ((void (__fastcall *)(int, int))(0x1AD6F5 + g_libGTASA))(v3, 0x3C23D70A);
      v9[0] = 0x3F000000;
      v9[1] = 0x3F000000;
      ((void (__fastcall *)(int, int *))(0x1AD6F5 + g_libGTASA + 0x230))(v3, v9);
      ((void (__fastcall *)(int, int))(0x1AD6F5 + g_libGTASA + 0x1E8))(v3, 1);
      v7 = *(int *)(g_libGTASA + 0x95B060);
      if ( v7 )
      {
        ((void (__fastcall *)(int, int))(0x1EB119 + g_libGTASA))(v7, v3);
        return 1;
      }
      return 0;
    }
  }
  return v5;
}
int __fastcall sub_3EE70(int a1, int a2, int a3, char a4)
{
	//Log("me pon 6");
  
  sub_3E99C((int *)a1);
  return a1;
}

#include "CRenderTarget.h"
#include "..//gui/CFontInstance.h"
#include "..//gui/CFontRenderer.h"
#include "CCustomPlateManager.h"
#include "../util/CSnapShotWrapper.h"
#include "../util/CSnapShots.h"
static bool g_bFirstPersonOnFootEnabled = false;
extern bool bDisableTestGovno;
bool megapon = false;
void (*CGame__Process)();
void CGame__Process_hook()
{
	CGame__Process();

	static bool once = false;
	if (!once)
	{
		CCustomPlateManager::Initialise();
		CSnapShots::Initialise();
		CSnow::Initialise();
		once = true;
	}
	else
	{
		CSnapShotWrapper::Process();
	}
	 
	

//                 WriteMemory(g_libGTASA + 0x52DD38, (uintptr_t)"\x00\x20\x70\x47", 4); // CCoronas::RenderReflections
  //               NOP(g_libGTASA + 0x39AD14, 1); //render clouds, sunrefl, raineffect 
    //             memcpy((uint32_t*)(g_libGTASA+0x5DE734), "0x10000000", 10); // CStreaming::ms_memoryAvailable(limit);
	uintptr_t v0;
	v0 = 100;
	*(float *)(g_libGTASA + 0x8C8670) = 1.0f;
	
	if ( *(uintptr_t *)(g_libGTASA + 0x8B7D28) )
	{
		v0 = 100;
	}
	
	if(*(int32_t *)(g_libGTASA + 0x63E464) == 2){
		*(int32_t *)(g_libGTASA + 0x63E464) = 3;
	}
	*(uintptr_t *)(0x63E584 + g_libGTASA) = v0; //brightness always 100
	
	int v6;
//	Log("me pon 1");
	if(!megapon){
	//	Log("me pon 2");
		v6 = *new int[0x1F];
	//	Log("me pon 3");
		sub_3EE70(v6, 0x100, 0x100, 0);
	//	Log("me pon 4");
		megapon = true;
	}
	
	if (pNetGame && pNetGame->GetPlayerPool() && pNetGame->GetPlayerPool()->GetLocalPlayer())
	{
		CSnow::Process(pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed(), pGame->GetActiveInterior());
	}

	if (pNetGame)
	{
		CTextDrawPool* pTextDrawPool = pNetGame->GetTextDrawPool();
		if (pTextDrawPool) {
			pTextDrawPool->SnapshotProcess();
		}
	}
	if (g_pWidgetManager)
	{
		PED_TYPE* pPed = GamePool_FindPlayerPed();
		if (g_pWidgetManager->GetSlotState(WIDGET_CAMERA_CYCLE) && pPed)
		{
			static uint32_t lastTick = GetTickCount();
			bool bPressed = false;
			if (g_pWidgetManager->IsTouched(WIDGET_CAMERA_CYCLE) && GetTickCount() - lastTick >= 250)
			{
				bPressed = true;
				lastTick = GetTickCount();
			}

			if (!CFirstPersonCamera::IsEnabled() && g_bFirstPersonOnFootEnabled)
			{
				CFirstPersonCamera::SetEnabled(true);
			}

			if (CFirstPersonCamera::IsEnabled() && !g_bFirstPersonOnFootEnabled)
			{
				CFirstPersonCamera::SetEnabled(false);
			}

			if (bPressed && !IN_VEHICLE(pPed))
			{
				CFirstPersonCamera::Toggle();
				if (CFirstPersonCamera::IsEnabled())
				{
					g_bFirstPersonOnFootEnabled = true;
				}
				else
				{
					g_bFirstPersonOnFootEnabled = false;
				}
			}
		}
	}

	if (pAudioStream) pAudioStream->Process();

	CCustomPlateManager::Process();
}

#include "..//cryptors/AUTOMOBILE_COLLISION_result.h"

uint16_t g_usLastProcessedModelIndexAutomobile = 0;
int g_iLastProcessedModelIndexAutoEnt = 0;
void (*CAutomobile__ProcessEntityCollision)(VEHICLE_TYPE* a1, ENTITY_TYPE* a2, int a3);
void CAutomobile__ProcessEntityCollision_hook(VEHICLE_TYPE* a1, ENTITY_TYPE* a2, int a3)
{
	PROTECT_CODE_AUTOMOBILE_COLLISION;

	g_usLastProcessedModelIndexAutomobile = a1->entity.nModelIndex;
	g_iLastProcessedModelIndexAutoEnt = a2->nModelIndex;

	bool bReplace = false;
	void* pOld = nullptr;
	uint8_t* pColData = nullptr;

	

	if (pNetGame)
	{
		if (pNetGame->GetVehiclePool())
		{
			uint16_t vehId = pNetGame->GetVehiclePool()->FindIDFromGtaPtr(a1);
			CVehicle* pVeh = pNetGame->GetVehiclePool()->GetAt(vehId);
			if (pVeh)
			{
				if (pVeh->bHasSuspensionLines && pVeh->GetVehicleSubtype() == VEHICLE_SUBTYPE_CAR)
				{
					pColData = GetCollisionDataFromModel(a1->entity.nModelIndex);
					if (pColData && pVeh->m_pSuspensionLines)
					{
						if (*(void**)(pColData + 16))
						{
							pOld = *(void**)(pColData + 16);
							*(void**)(pColData + 16) = pVeh->m_pSuspensionLines;
							bReplace = true;
						}
					}
				}
			}
		}
	}
	CAutomobile__ProcessEntityCollision(a1, a2, a3);
	if (bReplace)
	{
		*(void**)(pColData + 16) = pOld;
	}
}

bool (*CGame__Shutdown)();
bool CGame__Shutdown_hook()
{
	Log("Exiting game...");
	NOP(g_libGTASA + 0x00341FCC, 2); // nop PauseOpenSLES
	NOP(g_libGTASA + 0x0046389E, 2); // nop saving
	if (pNetGame)
	{
		if (pNetGame->GetRakClient())
		{
			pNetGame->GetRakClient()->Disconnect(500, 0);
		}
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	return CGame__Shutdown();
}

// TODO: VEHICLE RESET SUSPENSION
void (*CShadows__StoreCarLightShadow)(VEHICLE_TYPE* vehicle, int id, RwTexture* texture, VECTOR* posn, float frontX, float frontY, float sideX, float sideY, unsigned char red, unsigned char green, unsigned char blue, float maxViewAngle);
void CShadows__StoreCarLightShadow_hook(VEHICLE_TYPE* vehicle, int id, RwTexture* texture, VECTOR* posn, float frontX, float frontY, float sideX, float sideY, unsigned char red, unsigned char green, unsigned char blue, float maxViewAngle)
{
	uint8_t r, g, b;
	r = red;
	g = green;
	b = blue;
	if (pNetGame)
	{
		if (pNetGame->GetVehiclePool())
		{
			uint16_t vehid = pNetGame->GetVehiclePool()->FindIDFromGtaPtr(vehicle);
			CVehicle* pVeh = pNetGame->GetVehiclePool()->GetAt(vehid);
			if (pVeh)
			{
				pVeh->ProcessHeadlightsColor(r, g, b);
			}
		}
	}

	return CShadows__StoreCarLightShadow(vehicle, id, texture, posn, frontX, frontY, sideX, sideY, r, g, b, maxViewAngle);
}

void CVehicle__DoHeadLightReflectionTwin(CVehicle* pVeh, MATRIX4X4* a2)
{
	VEHICLE_TYPE* v2; // r4
	int v3; // r2
	MATRIX4X4* v4; // r5
	float* v5; // r3
	float v6; // s12
	float v7; // s5
	float* v8; // r2
	float v9; // r0
	float v10; // r1
	float v11; // r2
	float v12; // s14
	float v13; // s11
	float v14; // s15
	float v15; // s13
	float v16; // s10
	float v17; // s12
	float v18; // s15
	float v19; // ST08_4

	uintptr_t* dwModelarray = (uintptr_t*)(g_libGTASA + 0x87BF48);

	v2 = pVeh->m_pVehicle;
	v3 = *((uintptr_t*)v2 + 5);
	v4 = a2;
	v5 = *(float**)(dwModelarray[v2->entity.nModelIndex] + 116);
	v6 = *v5;
	v7 = v5[1];
	if (v3)
		v8 = (float*)(v3 + 48);
	else
		v8 = (float*)((char*)v2 + 4);
	v9 = *v8;
	v10 = v8[1];
	v11 = v8[2];
	v12 = *((float*)v4 + 5);
	v13 = *((float*)v4 + 4);
	v14 = (float)(v12 * v12) + (float)(v13 * v13);
	if (v14 != 0.0)
		v14 = 1.0 / sqrtf(v14);
	v15 = v6 * 4.0;
	v16 = (float)(v15 + v15) + 1.0;
	v17 = v13 * v14;
	v18 = v12 * v14;

	v19 = v15 * v18;

	VECTOR pos;
	memcpy(&pos, &(v2->entity.mat->pos), sizeof(VECTOR));
	pos.Z += 2.0f;

	CShadows__StoreCarLightShadow(
		v2,
		(uintptr_t)v2 + 24,
		pVeh->m_Shadow.pTexture,
		&pos,
		(float)(v15 + v15) * v17 * pVeh->m_Shadow.fSizeX,
		(float)(v15 + v15) * v18 * pVeh->m_Shadow.fSizeX,
		v19 * pVeh->m_Shadow.fSizeY,
		-(float)(v15 * v17) * pVeh->m_Shadow.fSizeY,
		pVeh->m_Shadow.r, pVeh->m_Shadow.g, pVeh->m_Shadow.b,
		7.0f);
}

void (*CVehicle__DoHeadLightBeam)(VEHICLE_TYPE* vehicle, int arg0, MATRIX4X4& matrix, unsigned char arg2);
void CVehicle__DoHeadLightBeam_hook(VEHICLE_TYPE* vehicle, int arg0, MATRIX4X4& matrix, unsigned char arg2)
{
	uint8_t r, g, b;
	r = 0xFF;
	g = 0xFF;
	b = 0xFF;
	if (pNetGame)
	{
		if (pNetGame->GetVehiclePool())
		{
			uint16_t vehid = pNetGame->GetVehiclePool()->FindIDFromGtaPtr(vehicle);
			CVehicle* pVeh = pNetGame->GetVehiclePool()->GetAt(vehid);
			if (pVeh)
			{
				pVeh->ProcessHeadlightsColor(r, g, b);
			}
		}
	}

	*(uint8_t*)(g_libGTASA + 0x9BAA70) = r;
	*(uint8_t*)(g_libGTASA + 0x9BAA71) = g;
	*(uint8_t*)(g_libGTASA + 0x9BAA72) = b;

	*(uint8_t*)(g_libGTASA + 0x9BAA94) = r;
	*(uint8_t*)(g_libGTASA + 0x9BAA95) = g;
	*(uint8_t*)(g_libGTASA + 0x9BAA96) = b;

	*(uint8_t*)(g_libGTASA + 0x9BAB00) = r;
	*(uint8_t*)(g_libGTASA + 0x9BAB01) = g;
	*(uint8_t*)(g_libGTASA + 0x9BAB02) = b;

	CVehicle__DoHeadLightBeam(vehicle, arg0, matrix, arg2);

}

static CVehicle* g_pLastProcessedVehicleMatrix = nullptr;
static int g_iLastProcessedWheelVehicle = -1;

void (*CMatrix__Rotate)(void* thiz, float a1, float a2, float a3);
void CMatrix__Rotate_hook(void* thiz, float a1, float a2, float a3)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;

	if (dwRetAddr == 0x003A9D76 + 1)
	{
		CMatrix__Rotate(thiz, a1, a2, a3);
		return;
	}

	CMatrix__Rotate(thiz, a1, a2, a3);
	if (g_pLastProcessedVehicleMatrix && g_iLastProcessedWheelVehicle != -1)
	{
		if (g_pLastProcessedVehicleMatrix->m_bWheelAlignmentX || g_pLastProcessedVehicleMatrix->m_bWheelAlignmentY)
		{
			if (g_iLastProcessedWheelVehicle == 2)
			{
				((void(*)(void*, float))(g_libGTASA + 0x003E8BE4 + 1))(thiz, 0.0f - g_pLastProcessedVehicleMatrix->m_fWheelAlignmentX); // CMatrix::RotateY
			}
			if (g_iLastProcessedWheelVehicle == 4)
			{
				((void(*)(void*, float))(g_libGTASA + 0x003E8BE4 + 1))(thiz, 0.0f - g_pLastProcessedVehicleMatrix->m_fWheelAlignmentY); // CMatrix::RotateY
			}
			if (g_iLastProcessedWheelVehicle == 5)
			{
				((void(*)(void*, float))(g_libGTASA + 0x003E8BE4 + 1))(thiz, g_pLastProcessedVehicleMatrix->m_fWheelAlignmentX); // CMatrix::RotateY
			}
			if (g_iLastProcessedWheelVehicle == 7)
			{
				((void(*)(void*, float))(g_libGTASA + 0x003E8BE4 + 1))(thiz, g_pLastProcessedVehicleMatrix->m_fWheelAlignmentY); // CMatrix::RotateY
			}
		}
	}
}

void (*CMatrix__SetScale)(void* thiz, float x, float y, float z);
void CMatrix__SetScale_hook(void* thiz, float x, float y, float z)
{
	if (g_pLastProcessedVehicleMatrix && g_iLastProcessedWheelVehicle != -1)
	{
		if (g_iLastProcessedWheelVehicle >= 2 || g_iLastProcessedWheelVehicle <= 7)
		{
			// front wheel
			if (g_pLastProcessedVehicleMatrix->m_bWheelSize)
			{
				y *= g_pLastProcessedVehicleMatrix->m_fWheelSize * 1.3f; //                   scale          
				z *= g_pLastProcessedVehicleMatrix->m_fWheelSize * 1.3f;
			}
			if (g_pLastProcessedVehicleMatrix->m_bWheelWidth)
			{
				x = g_pLastProcessedVehicleMatrix->m_fWheelWidth;
			}
		}
	}

	CMatrix__SetScale(thiz, x, y, z);
}

void (*CAutomobile__UpdateWheelMatrix)(VEHICLE_TYPE* thiz, int, int);
void CAutomobile__UpdateWheelMatrix_hook(VEHICLE_TYPE* thiz, int nodeIndex, int flags)
{
	if (g_pLastProcessedVehicleMatrix)
	{
		g_iLastProcessedWheelVehicle = nodeIndex;
	}

	CAutomobile__UpdateWheelMatrix(thiz, nodeIndex, flags);
}

void (*CAutomobile__PreRender)(VEHICLE_TYPE* thiz);
void CAutomobile__PreRender_hook(VEHICLE_TYPE* thiz)
{
	uintptr_t* dwModelarray = (uintptr_t*)(g_libGTASA + 0x87BF48);
	uint8_t* pModelInfoStart = (uint8_t*)dwModelarray[thiz->entity.nModelIndex];

	float fOldFront = *(float*)(pModelInfoStart + 88);
	float fOldRear = *(float*)(pModelInfoStart + 92);
	CVehicle* pVeh = nullptr;
	if (pNetGame)
	{
		if (pNetGame->GetVehiclePool())
		{
			uint16_t vehid = pNetGame->GetVehiclePool()->FindIDFromGtaPtr(thiz);
			pVeh = pNetGame->GetVehiclePool()->GetAt(vehid);
			if (pVeh)
			{
				pVeh->ProcessWheelsOffset();
				g_pLastProcessedVehicleMatrix = pVeh;

				if (pVeh->m_bWheelSize)
				{
					*(float*)(pModelInfoStart + 92) = pVeh->m_fWheelSize;
					*(float*)(pModelInfoStart + 88) = pVeh->m_fWheelSize;
				}
				if (pVeh->m_bShadow && pVeh->m_Shadow.pTexture)
				{
					CVehicle__DoHeadLightReflectionTwin(pVeh, pVeh->m_pVehicle->entity.mat);
				}
			}
		}
	}

	CAutomobile__PreRender(thiz);

	*(float*)(pModelInfoStart + 88) = fOldFront;
	*(float*)(pModelInfoStart + 92) = fOldRear;

	g_pLastProcessedVehicleMatrix = nullptr;
	g_iLastProcessedWheelVehicle = -1;
}

#include "..//cryptors/INSTALLHOOKS_result.h"

void (*CTaskSimpleUseGun__RemoveStanceAnims)(void* thiz, void* ped, float a3);
void CTaskSimpleUseGun__RemoveStanceAnims_hook(void* thiz, void* ped, float a3)
{
	uint32_t v5 = *((uint32_t*)thiz + 11);
	if (v5)
	{
		if (*(uint32_t*)(v5 + 20))
		{
			CTaskSimpleUseGun__RemoveStanceAnims(thiz, ped, a3);
			return;
		}
		else
		{
			return;
		}
	}
	else
	{
		CTaskSimpleUseGun__RemoveStanceAnims(thiz, ped, a3);
	}
}

float (*CRadar__LimitRadarPoint)(float* a1);
float CRadar__LimitRadarPoint_hook(float* a1)
{
	if (*(uint8_t*)(g_libGTASA + 0x0063E0B4))
	{
		return sqrtf((float)(a1[1] * a1[1]) + (float)(*a1 * *a1));
	}

	if (!CRadarRect::IsEnabled())
	{
		return CRadar__LimitRadarPoint(a1);
	}
	float value = CRadarRect::CRadar__LimitRadarPoint_hook(a1);

	if (pChatWindow)
	{
		//pChatWindow->AddDebugMessage("VALUE %f", value);
	}

	return value;
}

void (*CSprite2d__DrawBarChart)(float x, float y, unsigned short width, unsigned char height, float progress, signed char progressAdd,
	unsigned char drawPercentage, unsigned char drawBlackBorder, CRGBA* color, CRGBA* addColor);
void CSprite2d__DrawBarChart_hook(float x, float y, unsigned short width, unsigned char height, float progress, signed char progressAdd,
	unsigned char drawPercentage, unsigned char drawBlackBorder, CRGBA* color, CRGBA* addColor)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;

	float fX = x;
	float fY = y;

	unsigned short usWidth = width;
	unsigned char usHeight = height;

	if (dwRetAddr == 0x0027D524 + 1)
	{
		if (CAdjustableHudColors::IsUsingHudColor(E_HUD_ELEMENT::HUD_HP))
		{
			color->A = CAdjustableHudColors::GetHudColor(E_HUD_ELEMENT::HUD_HP).A;
			color->R = CAdjustableHudColors::GetHudColor(E_HUD_ELEMENT::HUD_HP).R;
			color->G = CAdjustableHudColors::GetHudColor(E_HUD_ELEMENT::HUD_HP).G;
			color->B = CAdjustableHudColors::GetHudColor(E_HUD_ELEMENT::HUD_HP).B;
		}
		color->A = 0;
		color->R = CAdjustableHudColors::GetHudColor(E_HUD_ELEMENT::HUD_HP).R;
		color->G = CAdjustableHudColors::GetHudColor(E_HUD_ELEMENT::HUD_HP).G;
		color->B = CAdjustableHudColors::GetHudColor(E_HUD_ELEMENT::HUD_HP).B;
	}
	else if (dwRetAddr == 0x0027D83E + 1)
	{
		if (CAdjustableHudColors::IsUsingHudColor(E_HUD_ELEMENT::HUD_ARMOR))
		{
			color->A = CAdjustableHudColors::GetHudColor(E_HUD_ELEMENT::HUD_ARMOR).A;
			color->R = CAdjustableHudColors::GetHudColor(E_HUD_ELEMENT::HUD_ARMOR).R;
			color->G = CAdjustableHudColors::GetHudColor(E_HUD_ELEMENT::HUD_ARMOR).G;
			color->B = CAdjustableHudColors::GetHudColor(E_HUD_ELEMENT::HUD_ARMOR).B;
		}
		color->A = 0;
		color->R = CAdjustableHudColors::GetHudColor(E_HUD_ELEMENT::HUD_ARMOR).R;
		color->G = CAdjustableHudColors::GetHudColor(E_HUD_ELEMENT::HUD_ARMOR).G;
		color->B = CAdjustableHudColors::GetHudColor(E_HUD_ELEMENT::HUD_ARMOR).B;
	}
	CSprite2d__DrawBarChart(fX, fY, usWidth, usHeight, progress, progressAdd, drawPercentage, drawBlackBorder, color, addColor);
}
static int g_iCurrentWanted = 0;
static float g_fInitialPos = 0.0f;
void (*CWidgetPlayerInfo__DrawWanted)(void*);
void CWidgetPlayerInfo__DrawWanted_hook(void* thiz)
{
	g_iCurrentWanted = 0;
	g_fInitialPos = *((float*)thiz + 10);
	CWidgetPlayerInfo__DrawWanted(thiz);
	g_iCurrentWanted = 0;
}

void (*CFont__PrintString)(float x, float y, uint16_t* text);
void CFont__PrintString_hook(float x, float y, uint16_t* text)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;

	float fX = x;
	float fY = y;
                  if (dwRetAddr == 0x0027D9E6 + 1) // wanted
	{
		if (CAdjustableHudColors::IsUsingHudColor(E_HUD_ELEMENT::HUD_WANTED))
		{
			CRGBA col = CAdjustableHudColors::GetHudColor(E_HUD_ELEMENT::HUD_WANTED);
			uint32_t m1 = col.ToInt();
			CFont::SetColor(&m1);
		}

		if (CAdjustableHudScale::GetElementScale(E_HUD_ELEMENT::HUD_WANTED).X != -1)
		{
			float value = (float)CAdjustableHudScale::GetElementScale(E_HUD_ELEMENT::HUD_WANTED).X / 100.0f;
			CFont::SetScale(value, 0.0f);
		}
		
		if (CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_WANTED).X != -1)
		{
			fX -= g_fInitialPos;
			fX += pGUI->ScaleX(CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_WANTED).X);
		}
		if (CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_WANTED).Y != -1)
		{
			fY = pGUI->ScaleY(CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_WANTED).Y);
		}
		g_iCurrentWanted++;

	}
	else if (dwRetAddr == 0x0027D330 + 1) // ammo text
	{
		if (CAdjustableHudColors::IsUsingHudColor(E_HUD_ELEMENT::HUD_AMMO))
		{
			CRGBA col = CAdjustableHudColors::GetHudColor(E_HUD_ELEMENT::HUD_AMMO);
			uint32_t m1 = col.ToInt();
			CFont::SetColor(&m1);
		}

		if (CAdjustableHudScale::GetElementScale(E_HUD_ELEMENT::HUD_AMMO).X != -1)
		{
			float value = (float)CAdjustableHudScale::GetElementScale(E_HUD_ELEMENT::HUD_AMMO).X / 100.0f;
			CFont::SetScale(value, 0.0f);
		}

		if (CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_AMMO).X != -1)
		{
			fX = pGUI->ScaleX(CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_AMMO).X);
		}
		if (CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_AMMO).Y != -1)
		{
			fY = pGUI->ScaleY(CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_AMMO).Y);
		}
	}
	CFont__PrintString(fX, fY, text);
}



void(*CHud__Draw)();
void CHud__Draw_hook()
{
                  if(CRadar::IsEnabled()) DrawRadarTexture(CRadar::radarBgPos1.x1, CRadar::radarBgPos1.y1, CRadar::radarBgPos2.x1, CRadar::radarBgPos2.y1);

	//float* thiz = (float*) * (uintptr_t*)(g_libGTASA + 0x6580C8);
	//if (thiz)
	//{
	//	thiz[5] = 45.0f;
	//	thiz[6] = 45.0f;
	//}

	CHud__Draw();
}

void (*CSprite2d__Draw)(CSprite2d* a1, CRect* a2, CRGBA* a3);
void CSprite2d__Draw_hook(CSprite2d* a1, CRect* a2, CRGBA* a3)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;

	if (!pGUI)
	{
		return CSprite2d__Draw(a1, a2, a3);
	}

	if (dwRetAddr == 0x003D3796 + 1 || dwRetAddr == 0x003D376C + 1 || dwRetAddr == 0x003D373E + 1 || dwRetAddr == 0x003D3710 + 1)
	{
		float* thiz = (float*) * (uintptr_t*)(g_libGTASA + 0x6580C8);
		if (thiz)
		{
			if(CRadar::IsEnabled())
			{
				/*
				thiz[3]; // posX
				thiz[4]; // posY

				thiz[5]; // ScaleX
				thiz[6]; // ScaleY
				*/

				CRect* rect = (CRect*)&thiz[3];
				
				CRadar::EditRadar(rect);
                                                                        // CRadar::Render();
                                                                        // ((void(*)())(g_libGTASA + 0x003D4ED8 + 1))(); // CRadar::DrawRadar(void)
			}
		}

	}
	return CSprite2d__Draw(a1, a2, a3);
}

// CRadar::DrawRadar
void RenderBackgroundHud();
void (*DrawRadar)();
void DrawRadar_hook()
{
                  RenderBackgroundHud();
	return;
}

void (*CWidgetPlayerInfo__DrawWeaponIcon)(float* thiz, void* ped, CRect rect, float a4);
void CWidgetPlayerInfo__DrawWeaponIcon_hook(float* thiz, void* ped, CRect rect, float a4)
{
	float diffX = rect.right - rect.left;
	float diffY = rect.bottom - rect.top;
	if (CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_FIST).X != -1)
	{
		rect.left = pGUI->ScaleX(CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_FIST).X);
		rect.right = pGUI->ScaleX(CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_FIST).X) + diffX;

		thiz[38] = rect.left;
		thiz[40] = rect.right;
	}
	if (CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_FIST).Y != -1)
	{
		rect.top = pGUI->ScaleY(CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_FIST).Y);
		rect.bottom = pGUI->ScaleY(CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_FIST).Y) + diffY;

		thiz[39] = rect.bottom;
		thiz[41] = rect.top;
	}

	if (CAdjustableHudScale::GetElementScale(E_HUD_ELEMENT::HUD_FIST).X != -1)
	{
		float coef = (float)CAdjustableHudScale::GetElementScale(E_HUD_ELEMENT::HUD_FIST).X / 100.0f;
		float diffX = rect.right - rect.left;
		float diffY = rect.bottom - rect.top;
		diffX *= coef;
		diffY *= coef;

		rect.right = rect.left + diffX;
		rect.bottom = rect.top + diffY;
	}

	return CWidgetPlayerInfo__DrawWeaponIcon(thiz, ped, rect, a4);
}

void (*CCam__Process)(uintptr_t);
void CCam__Process_hook(uintptr_t thiz)
{

	if (pChatWindow)
	{
		//pChatWindow->AddDebugMessage("time %d %d %d", *(uint32_t*)(g_libGTASA + 0x008B0808 + 116), *(uint32_t*)(g_libGTASA + 0x008B0808 + 120), *(uint32_t*)(g_libGTASA + 0x008B0808 + 124));
	//	pChatWindow->AddDebugMessage("camera %d %f %f", *(uint16_t*)(thiz + 14), *(float*)(thiz + 132), *(float*)(thiz + 148)); // 140 - fov, 132 - vertical, 148 - horizontal angle
	}

	VECTOR vecSpeed;
	CVehicle* pVeh = nullptr;
	if (pNetGame && (*(uint16_t*)(thiz + 14) == 18 || *(uint16_t*)(thiz + 14) == 16) && CFirstPersonCamera::IsEnabled())
	{
		if (pNetGame->GetPlayerPool())
		{
			if (pNetGame->GetPlayerPool()->GetLocalPlayer())
			{
				CPlayerPed* pPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
				pVeh = pNetGame->GetVehiclePool()->GetAt(pNetGame->GetPlayerPool()->GetLocalPlayer()->m_CurrentVehicle);
			/*	if (pVeh)
				{
					pVeh->GetMoveSpeedVector(&vecSpeed);
					VECTOR vec;
					vec.X = vecSpeed.X * 6.0f;
					vec.Y = vecSpeed.Y * 6.0f;
					vec.Z = vecSpeed.Z * 6.0f;
					pVeh->SetMoveSpeedVector(vec);
				}*/
			}
		}
	}
CCam__Process(thiz);
	if (*(uint16_t*)(thiz + 14) == 4 || *(uint16_t*)(thiz + 14) == 53) // 53 is weapon
	{
		if (pNetGame)
		{
			if (pNetGame->GetPlayerPool())
			{
				if (pNetGame->GetPlayerPool()->GetLocalPlayer())
				{
					CPlayerPed* pPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
					if (pPed)
					{
						//*(uint32_t*)(g_libGTASA + 0x008B0808 + 120) = 0xFFFFFFFF;
					//	*(uint32_t*)(g_libGTASA + 0x008B0808 + 124) = 0xFFFFFFFF;
						//*(uint8_t*)(g_libGTASA + 0x008B0808 + 40) = 0;
						CFirstPersonCamera::ProcessCameraOnFoot(thiz, pPed);
					}
				}
			}
		}
	}
	if(* (uint16_t*)(thiz + 14) == 18 || *(uint16_t*)(thiz + 14) == 16) // in car
	{
		if (pNetGame)
		{
			if (pNetGame->GetPlayerPool())
			{
				if (pNetGame->GetPlayerPool()->GetLocalPlayer())
				{
					CPlayerPed* pPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
					if (pPed)
					{
						//*(uint32_t*)(g_libGTASA + 0x008B0808 + 120) = 0xFFFFFFFF;
						//*(uint32_t*)(g_libGTASA + 0x008B0808 + 124) = 0xFFFFFFFF;
						//*(uint8_t*)(g_libGTASA + 0x008B0808 + 40) = 0;
						CFirstPersonCamera::ProcessCameraInVeh(thiz, pPed, pVeh);
					}
				}
			}
		}
	}
	
}
int g_iCounterVehicleCamera = 0;
int (*CPad__CycleCameraModeDownJustDown)(void*);
int CPad__CycleCameraModeDownJustDown_hook(void* thiz)
{
	if (!g_pWidgetManager)
	{
		return 0;
	}
	if (!g_pWidgetManager->GetSlotState(WIDGET_CAMERA_CYCLE))
	{
		return 0;
	}
	PED_TYPE* pPed = GamePool_FindPlayerPed();
	if (!pPed)
	{
		return 0;
	}

	static uint32_t lastTick = GetTickCount();
	bool bPressed = false;
	if (g_pWidgetManager->IsTouched(WIDGET_CAMERA_CYCLE) && GetTickCount() - lastTick >= 250)
	{
		bPressed = true;
		lastTick = GetTickCount();
	}

	if (IN_VEHICLE(pPed))
	{
		if (bPressed)
		{
			g_iCounterVehicleCamera++;
		}
		if (g_iCounterVehicleCamera == 6)
		{
			CFirstPersonCamera::SetEnabled(true);
			return 0;
		}
		else if (g_iCounterVehicleCamera >= 7)
		{
			g_iCounterVehicleCamera = 0;
			CFirstPersonCamera::SetEnabled(false);
			return 1;
			
		}
		else
		{
			CFirstPersonCamera::SetEnabled(false);
		}

		return bPressed;
	}
	return 0;
}

int (*CEntity__GetIsOnScreen)(ENTITY_TYPE*);
int CEntity__GetIsOnScreen_hook(ENTITY_TYPE* thiz)
{
	int retn = CEntity__GetIsOnScreen(thiz);

	return retn;
}

void (*FxEmitterBP_c__Render)(uintptr_t* a1, int a2, int a3, float a4, char a5);
void FxEmitterBP_c__Render_hook(uintptr_t* a1, int a2, int a3, float a4, char a5)
{
	uintptr_t* temp = *((uintptr_t**)a1 + 3);
	if (!temp)
	{
		return;
	}
	FxEmitterBP_c__Render(a1, a2, a3, a4, a5);
}

void(*CStreaming__RemoveModel)(int);
void CStreaming__RemoveModel_hook(int model)
{
	Log("Removing model %d", model);
	CStreaming__RemoveModel(model);
}

int g_iLastProcessedSkinCollision = 228;
int g_iLastProcessedEntityCollision = 228;

void (*CPed__ProcessEntityCollision)(PED_TYPE* thiz, ENTITY_TYPE* ent, void* colPoint);
void CPed__ProcessEntityCollision_hook(PED_TYPE* thiz, ENTITY_TYPE* ent, void* colPoint)
{
	g_iLastProcessedSkinCollision = thiz->entity.nModelIndex;
	g_iLastProcessedEntityCollision = ent->nModelIndex;

	CPed__ProcessEntityCollision(thiz, ent, colPoint);
}

// colModel check null-pointer

int V15lastest = 0;
int (*CustomPipeRenderCB)(uintptr_t resEntry, uintptr_t object, uint8_t type, uint32_t flags);
int CustomPipeRenderCB_hook(uintptr_t resEntry, uintptr_t object, uint8_t type, uint32_t flags)
{
	if(!resEntry) return 0;
    uint16_t size = *(uint16_t *)(resEntry + 26);
    if(size)
    {

        RES_ENTRY_OBJ* arr = (RES_ENTRY_OBJ*)(resEntry + 28);
		if(!arr) return 0;
        uint32_t validFlag = flags & 0x84;
        for(int i = 0; i < size; i++)
        {
            if(!arr[i].validate) break;
            if(validFlag)
            {
                uintptr_t* v4 = *(uintptr_t **)(arr[i].validate);
				if(!v4);
				else
				{
					if((uintptr_t)v4 > (uintptr_t)0xFFFFFF00) return 0;
                	else
					{	
                    	if(!*(uintptr_t **)v4) return 0;
					}
				}
                
            }
        }
    }
    return CustomPipeRenderCB(resEntry, object, type, flags);
}

int (*rxOpenGLDefaultAllInOneRenderCB)(uintptr_t resEntry, uintptr_t object, uint8_t type, uint32_t flags);
int rxOpenGLDefaultAllInOneRenderCB_hook(uintptr_t resEntry, uintptr_t object, uint8_t type, uint32_t flags)
{
    if(!resEntry) return 0;
    uint16_t size = *(uint16_t *)(resEntry + 26);
    if(size)
    {
        RES_ENTRY_OBJ* arr = (RES_ENTRY_OBJ*)(resEntry + 28);
		if(!arr) return 0;
        uint32_t validFlag = flags & 0x84;
        for(int i = 0; i < size; i++)
        {
            if(!arr[i].validate) break;
            if(validFlag)
            {
                uintptr_t* v4 = *(uintptr_t **)(arr[i].validate);
				if(!v4);
				else
				{
					if((uintptr_t)v4 > (uintptr_t)0xFFFFFF00) return 0;
                	else
					{	
                    	if(!*(uintptr_t **)v4) return 0;
					}
				}
                
            }
        }
    }
    return rxOpenGLDefaultAllInOneRenderCB(resEntry, object, type, flags);
}

int (*RwResourcesFreeResEntry)(int);
int RwResourcesFreeResEntry_hook(int a1)
{
  int result; // r0

  if ( a1 )
    result = RwResourcesFreeResEntry(a1);
  else
    result = 0;
  return result;
}

int (*_RpMaterialDestroy)(RpMaterial* a1);
int _RpMaterialDestroy_hook(RpMaterial* a1)
{
  int result; // r0

  if ( a1 )
    result = _RpMaterialDestroy(a1);
  else
    result = 1;
  return result;
}

int (*CPlayerPedDataSaveStructure__Construct)(int, int);
int CPlayerPedDataSaveStructure__Construct_hook(int a1, int a2)
{
  bool v2; // zf
  int result; // r0

  v2 = a1 == 0;
  if ( a1 )
    v2 = a2 == 0;
  if ( v2 || !*(uint32_t *)a2 )
    result = 0;
  else
    result = CPlayerPedDataSaveStructure__Construct(a1, a2);
  return result;
}



int (*CAEVehicleAudioEntity__GetAccelAndBrake)(int, int);
int CAEVehicleAudioEntity__GetAccelAndBrake_hook(int a1, int a2)
{
  bool v2; // zf
  int result; // r0

  v2 = a1 == 0;
  if ( a1 )
    v2 = a2 == 0;
  if ( v2 || !*(uint32_t *)(a1 + 12) )
    result = 0;
  else
    result = CAEVehicleAudioEntity__GetAccelAndBrake(a1, a2);
  return result;
}

int (*CAudioEngine__Service)(unsigned int a1);
int CAudioEngine__Service_hook(unsigned int a1)
{
  int result; // r0

  if ( a1 )
    result = CAudioEngine__Service(a1);
  else
    result = 0;
  return result;
}

int (*emu_ArraysGetID)(unsigned int);
int emu_ArraysGetID_hook(unsigned int a1)
{
  bool v1; // zf
  int result; // r0

  v1 = a1 == 0;
  if ( a1 )
    v1 = *(uint32_t *)(a1 + 36) == 0;
  if ( v1 )
    result = 0;
  else
    result = emu_ArraysGetID(a1);
  return result;
}

int (*SetCompAlphaCB)(int, int);
int SetCompAlphaCB_hook(int a1, char a2)
{
  int result; // r0

  if ( a1 )
    result = SetCompAlphaCB(a1, a2);
  else
    result = 0;
  return result;
}

int (*CCollision__ProcessVerticalLine)(float *a1, float *a2, int a3, int a4, int *a5, int a6, int a7, int a8);
int CCollision__ProcessVerticalLine_hook(float *a1, float *a2, int a3, int a4, int *a5, int a6, int a7, int a8)
{
  int result; // r0

  if ( a3 )
    result = CCollision__ProcessVerticalLine(a1, a2, a3, a4, a5, a6, a7, a8);
  else
    result = 0;
  return result;
}
int (*CFileLoader__LoadObjectTypes)(uintptr_t *thiz, const char *a2, const char *a3);
int CFileLoader__LoadObjectTypes_hook(uintptr_t *thiz, const char *a2, const char *a3)
{
  Log("FileLoader(IDE): %s | %s", a2, a3);
  return CFileLoader__LoadObjectTypes(thiz ,a2, a3);
}
float (*SetIfIplIsRequired)(int result, uintptr_t *a2);
float __fastcall SetIfIplIsRequired_hook(int result, uintptr_t *a2)
{
  return result;
}
int (*_RwTextureDestroy)(RwTexture*);
int _RwTextureDestroy_hook(RwTexture* texture)
{
    

    return 0;
}
int (*LoadColFile)(uintptr_t *thiz, const char *a2, unsigned int a3);
int LoadColFile_hook(uintptr_t *thiz, const char *a2, unsigned int a3)
{
    Log("FileLoader(COL): %s", *a2);
    return LoadColFile(thiz, a2, a3);
}
void InstallRQHooks()
{
    int v0; // r0
    int v1; // r0
    int v2; // r0
/*
    
    */
// WriteMemory(g_libGTASA + 0x1A6680, (uintptr_t)"\xB5\x90", 2u);// \xB5\x90
   // WriteMemory(g_libGTASA + 0x1A6684, (uintptr_t)"\x4B\x38", 2u);// \x4B\x38
    //SetUpHook(g_libGTASA + 0x1A6680, (uintptr_t)RQTexture__Delete_hook, (uintptr_t*)&RQTexture__Delete);
    SetUpHook(
            g_libGTASA + 0x1A7C9C,
            (uintptr_t)ES2Texture__ES2Texture_end_hook,
            (uintptr_t*)&ES2Texture__ES2Texture_end);
    SetUpHook(
            g_libGTASA + 0x1A7D80,
            (uintptr_t)ES2Texture__ES2Texture_end_hook1,
            (uintptr_t*)&ES2Texture__ES2Texture_end1);
   // SetUpHook(g_libGTASA + 0x1A7644, (uintptr_t)RQ_Command_rqTargetDelete_hook, (uintptr_t*)&RQ_Command_rqTargetDelete);
    SetUpHook(
            g_libGTASA + 0x1A6848,
            (uintptr_t)RQ_Command_rqDeleteTexture_hook,
            (uintptr_t*)&RQ_Command_rqDeleteTexture);
    UnFuck(g_libGTASA + 0x5D02D4, 100);
    v0 = g_libGTASA;
    *(void**)&dword_72F0A8 = *(uintptr_t* *)(g_libGTASA + 0x5D02D4);
    UnFuck(v0 + 0x5CFFFC, 100);
    v1 = g_libGTASA;
    *(void**)&dword_72F0C8 = *(uintptr_t* *)(g_libGTASA + 0x5CFFFC);
    UnFuck(v1 + 0x5CF40C, 100);
    v2 = g_libGTASA;
    *(void**)&dword_72F0E8 = *(uintptr_t* *)(g_libGTASA + 0x5CF40C);
//    WriteMemory(v2 + 0x1A656A, (uintptr_t)"\x2C\x08", 2u);// \x2C\x08
//    WriteMemory(g_libGTASA + 0x1A7D04, (uintptr_t)"\x2C\x08", 2u);// \x2C\x08
//    WriteMemory(g_libGTASA + 0x1A7DE8, (uintptr_t)"\x2C\x08", 2u);// \x2C\x08
//    WriteMemory(g_libGTASA + 0x1A76D0, (uintptr_t)"\x2C\x08", 2u);// \x2C\x08
//    WriteMemory(g_libGTASA + 0x1A68D0, (uintptr_t)"\x2C\x08", 2u);// \x2C\x08
}
int (*emu_ArraysDelete)(unsigned int a1, int a2, int a3, int a4);
int emu_ArraysDelete_hook(unsigned int a1, int a2, int a3, int a4)
{
	if(a1 > g_libGTASA) return emu_ArraysDelete(a1, a2, a3, a4);
	return 0;
}

int (*CObject__ProcessGarageDoorBehaviour)(uintptr_t, int);
int CObject__ProcessGarageDoorBehaviour_hook(uintptr_t thiz, int a2)
{	
	if(thiz) if(!*(uintptr_t*)(thiz + 372)) return 0;
	return CObject__ProcessGarageDoorBehaviour(thiz, a2);
}
uintptr_t sub_3E984(uintptr_t a1)
{
  Log("sub_3E984: renderQueue + 0x264 = %d", *(int *)(a1 + 0x264));
  return *(uintptr_t *)(a1 + 0x264);
}
uint64_t **__fastcall sub_29E10(uint64_t** result) {
    uint64_t value1 = **result;
    int value2 = (int)(*result + 1);
    uint64_t* ptr = (uint64_t*)(0x617220 + g_libGTASA);
    ptr[1] = value2;
    *ptr = value1;
    *result = ptr + (value2 + static_cast<int>(value1 >> 32));
    return result;
}

uintptr_t sub_3E98A(uintptr_t a1)
{
	Log("sub_3E98A: renderQueue + 0x270 = %d", *(int *)(a1 + 0x270));
  return *(uintptr_t *)(a1 + 0x270);
}
uintptr_t sub_3E896(uintptr_t result, uintptr_t a2)
{
	Log("sub_3E896: renderQueue + 0x274 = %d", *(int *)(result + 0x274));
  **(uintptr_t **)(result + 0x274) = a2;
  *(uintptr_t *)(result + 0x274) += 4;
  return result;
}
uintptr_t sub_3E880(uintptr_t result, uintptr_t a2)
{
  uintptr_t *v2; // r2

  v2 = *(uintptr_t **)(result + 0x274);
  *(uintptr_t *)(result + 0x278) = a2;
  *v2 = a2;
  *(uintptr_t *)(result + 0x274) += 4;
  return result;
}
void qmemcpy(void* dst, uintptr_t src, int n) { 
    memcpy(dst, (const void*)src, (size_t)n);
}
unsigned int sub_3E8A8(uintptr_t a1, uintptr_t a2, int a3)
{
  unsigned int result; // r0

  qmemcpy(*(void **)(a1 + 0x274), a2, a3);
  result = *(uintptr_t *)(a1 + 0x274) + ((a3 + 3) & 0xFFFFFFFC);
  *(uintptr_t *)(a1 + 0x274) = result;
  return result;
}


uintptr_t sub_3E910(uintptr_t result)
{
  if ( *(unsigned char *)(result + 0x259) )
    result = ((uintptr_t ( *)(uintptr_t))((char *)0x2384D1 + g_libGTASA))(*(uintptr_t *)(result + 0x25C));
  return result;
}
unsigned int sub_3E938(uintptr_t result)
{
  if ( *(unsigned char *)(result + 0x259) ){
	result = ((unsigned int ( *)(uintptr_t))((char *)0x2384FD + g_libGTASA))(*(uintptr_t *)(result + 0x25C));
  }
  return result;
}
unsigned int sub_3E8E4(uintptr_t result)
{
  if ( *(int *)(*(uintptr_t *)(result) + 0x264) < *(int *)(*(uintptr_t *)(result) + 0x270) )
    result = ((unsigned int (*)(uintptr_t))(g_libGTASA + 0x1A8709))(*(uintptr_t *)(result));
  return reinterpret_cast<unsigned int>(result);
}

unsigned int sub_3E8CC()
{
	//uintptr_t *ptr = reinterpret_cast<uintptr_t*> (result);

// Вызов функции
    
  return ((unsigned int (*)(uintptr_t))(g_libGTASA + 0x1A8709))(*(uintptr_t *)((char *)0x61B2E8 + g_libGTASA)); 
}

uintptr_t __fastcall sub_3E960(uintptr_t result) {
  int v1; 
  int v2; 
  unsigned int v3; 

  v1 = *(int*)(result + 0x270); 
  v2 = *(int*)(result + 0x274); 

  do {
    v3 = *(unsigned int*)(result + 0x270);
  } while (*(unsigned int*)(result + 0x270) != (v3 + v2 - v1)); 

  return result; 
}
void SendBulletSync(VECTOR* vecOrigin, VECTOR* vecEnd, VECTOR* vecPos, ENTITY_TYPE** ppEntity)
{
	BULLET_DATA bulletData;
	memset(&bulletData, 0, sizeof(BULLET_DATA));

	bulletData.vecOrigin.X = vecOrigin->X;
	bulletData.vecOrigin.Y = vecOrigin->Y;
	bulletData.vecOrigin.Z = vecOrigin->Z;
	bulletData.vecPos.X = vecPos->X;
	bulletData.vecPos.Y = vecPos->Y;
	bulletData.vecPos.Z = vecPos->Z;

	if (ppEntity) {
		ENTITY_TYPE *pEntity = *ppEntity;
		if (pEntity) {
			if (pEntity->mat) 
			{
				bulletData.vecOffset.X = vecPos->X - pEntity->mat->pos.X;
				bulletData.vecOffset.Y = vecPos->Y - pEntity->mat->pos.Y;
				bulletData.vecOffset.Z = vecPos->Z - pEntity->mat->pos.Z;
			}

			bulletData.pEntity = pEntity;
		}
	}

	pGame->FindPlayerPed()->ProcessBulletData(&bulletData);
}
#include "Weapon.h"
int sub_3E996(int result, int a2)
{
  *(uintptr_t *)(result + 0x274) = a2;
  return result;
}
int __fastcall sub_3E990(uintptr_t a1)
{
  return *(int *)(a1 + 0x274);
}
int *byte_5D2470;
unsigned int (*sub_29E40)(const void *a1, size_t a2, void (__fastcall *a3)(int));
int *__fastcall sub_29E40_hook(const void *a1, size_t a2, void (__fastcall *a3)(int))
{
  size_t v6; // r5
  int v7; // r4
  unsigned int v8; // r11
  unsigned int v9; // r6
  int v10; // r0
  int v11; // r0
  int *result; // r0

  v6 = a2 + 3;
  v7 = g_libGTASA;
  uintptr_t dword_609F18 = *(uintptr_t *)(0x61B2E8 + g_libGTASA);
  if ( *(uintptr_t *)(0x61B2E8 + g_libGTASA + 0x10) )
    ((void (__fastcall *)(uintptr_t))(g_libGTASA + 0x1A8B15))(0);
  v8 = v6 & 0xFFFFFFFC;
  if ( *(uintptr_t *)(0x61B38C + v7) )
  {
    *(uintptr_t *)(0x61B38C + v7) = 0;
    sub_3E880(dword_609F18, 0);
 //   sub_3E896(dword_609F18, 0);
    sub_3E910(dword_609F18);
    sub_3E960(dword_609F18);
    sub_3E938(dword_609F18);
	Log("em 1");
    sub_3E8E4(dword_609F18);
	Log("em 2");
  }
  v9 = sub_3E984(dword_609F18);
  if ( v9 < sub_3E98A(dword_609F18) + v8 + 0xC )
    sub_3E8CC();
  sub_3E880(dword_609F18, 3);
//  sub_3E896(dword_609F18, a2);
Log("testt 133");
  v10 = sub_3E896(dword_609F18, v8);Log("testt 244");
  if ( a3 )
  {
    a3(v10);
	Log("testt 441");
    v11 = sub_3E990(dword_609F18);
    sub_3E996(dword_609F18, (v11 + 3) & 0xFFFFFFFC);
  }
  else
  {
	  Log("testt 1222");
    sub_3E8A8(dword_609F18, (uintptr_t)a1, (uintptr_t)a2);
  }
  sub_3E910(dword_609F18);
  sub_3E960(dword_609F18);
  sub_3E938(dword_609F18);
  Log("em 3");
  sub_3E8E4(dword_609F18);
  Log("em 4");
  result = byte_5D2470;
  byte_5D2470[v7] = 1;
  return result;
}
unsigned int (*sub_29C0C)(uintptr_t* a1, unsigned int a2, const void* a3);
unsigned int sub_29C0C_hook(uintptr_t* a1, unsigned int a2, const void* a3)
{
  int v6; // r11
  unsigned int v7; // r9
  int v8; // r10
  unsigned int v9; // r1
  unsigned int result; // r0

  uintptr_t dword_609F18 = *(uintptr_t *)((char *)0x61B2E8 + g_libGTASA);
//  Log("присвоение v6");
  v6 = ((int ( *)(uintptr_t*, unsigned int))((char *)0x1A9CE5 + g_libGTASA))(reinterpret_cast<uintptr_t*>(a1), a2);
//  Log("присвоение v7");
  v7 = sub_3E984(dword_609F18);
//  Log("присвоение v8");
  v8 = sub_3E98A(dword_609F18);
 // Log("присвоение sub_3E880");
  sub_3E880(dword_609F18, 0x16);
 // Log("присвоение первой sub_3E896");
  sub_3E896(dword_609F18, (uintptr_t)a1);
 // Log("присвоение второй sub_3E896");
  sub_3E896(dword_609F18, (uintptr_t)a2);
 // Log("присвоение третей sub_3E896");
  sub_3E896(dword_609F18, v6);
 // Log("присвоение v9");
  v9 = (v6 + 3) & 0xFFFFFFFC;
  if ( v7 >= v9 + v8 + 0x40 )
  {
	// Log("проверка тру0");
    sub_3E896(dword_609F18, v9);
	//Log("проверка тру1");
    sub_3E8A8(dword_609F18, (uintptr_t)a3, (uintptr_t)v6);
	//Log("проверка тру2");
    sub_3E910(dword_609F18);
	//Log("проверка тру3");
    sub_3E960(dword_609F18);
//	Log("проверка тру4");
    sub_3E938(dword_609F18);
//	Log("проверка тру5");
    result = (unsigned int)sub_3E8E4(dword_609F18);
//	Log("проверка тру6");
  }
  else
  {
	//  Log("проверка фалс0");
    sub_3E896(dword_609F18, 0);
//	Log("проверка фалс1");
    sub_3E896(dword_609F18, (uintptr_t)a3);
	//Log("проверка фалс2");
    sub_3E910(dword_609F18);
//	Log("проверка фалс3");
    sub_3E960(dword_609F18);
	//Log("проверка фалс4");
    sub_3E938(dword_609F18);
	//Log("проверка фалс5");
    sub_3E8E4(dword_609F18);
	//Log("проверка фалс6");
    result = (unsigned int)sub_3E8CC();
	//Log("проверка фалс7");
  }
//  Log("result: %d", result);
  return result;
}
unsigned int (*sub_29D0C)(int a1, int a2);
unsigned int __fastcall sub_29D0C_hook(int a1, int a2)
{
  int v4; // r4
  int v5; // r5
  unsigned int v6; // r9
  unsigned int v7; // r5

  v4 = a2 + 3;
  v5 = g_libGTASA;
  uintptr_t dword_609F18 = *(uintptr_t *)(0x61B2E8 + g_libGTASA);
  if ( *(uintptr_t *)(0x61B2E8 + g_libGTASA + 0x10) )
    ((void (__fastcall *)(uintptr_t))(g_libGTASA + 0x1A8B15))(0);
  v6 = v4 & 0xFFFFFFFC;
  if ( *(uintptr_t *)(0x61B388 + v5) )
  {
    *(uintptr_t *)(0x61B388 + v5) = 0;
	Log("testt 1");
    sub_3E880(dword_609F18, 5);
	Log("testt 2");
    sub_3E896(dword_609F18, 0);
	Log("testt 3");
    sub_3E910(dword_609F18);
	Log("testt 4");
    sub_3E960(dword_609F18);
	Log("testt 5");
    sub_3E938(dword_609F18);
	Log("testt 6");
    sub_3E8E4(dword_609F18);
	Log("testt 7");
  }
  v7 = sub_3E984(dword_609F18);
  Log("testt 8");
  if ( v7 < sub_3E98A(dword_609F18) + v6 + 0xC )
	Log("testt 9");
    sub_3E8CC();
  sub_3E880(dword_609F18, 8);
  
  Log("testt 10");
  sub_3E896(dword_609F18, a2);Log("testt 11");
  sub_3E896(dword_609F18, v6);Log("testt 12");
  sub_3E8A8(dword_609F18, a1, a2);Log("testt 13");
  sub_3E910(dword_609F18);Log("testt 14");
  sub_3E960(dword_609F18);Log("testt 15");
  sub_3E938(dword_609F18);Log("testt 16");
  return sub_3E8E4(dword_609F18);
}
uint32_t(*CWeapon__FireInstantHit)(WEAPON_SLOT_TYPE* _this, PED_TYPE* pFiringEntity, VECTOR* vecOrigin, VECTOR* muzzlePosn, ENTITY_TYPE* targetEntity,
	VECTOR* target, VECTOR* originForDriveBy, int arg6, int muzzle);
uint32_t CWeapon__FireInstantHit_hook(WEAPON_SLOT_TYPE* _this, PED_TYPE* pFiringEntity, VECTOR* vecOrigin, VECTOR* muzzlePosn, ENTITY_TYPE* targetEntity,
	VECTOR* target, VECTOR* originForDriveBy, int arg6, int muzzle)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));


	dwRetAddr -= g_libGTASA;

	if (dwRetAddr == 0x569A84 + 1 ||
		dwRetAddr == 0x569616 + 1 ||
		dwRetAddr == 0x56978A + 1 ||
		dwRetAddr == 0x569C06 + 1)
	{
		//if (GamePool_FindPlayerPed() != pFiringEntity) return 0;

		g_pCurrentFiredPed = pGame->FindPlayerPed();
	}

	return CWeapon__FireInstantHit(_this, pFiringEntity, vecOrigin, muzzlePosn, targetEntity, target, originForDriveBy, arg6, muzzle);
}
int fullipl = 0;
uintptr_t(*CEntity__GetBoundRect)(uintptr_t* thiz, int a2);
uintptr_t CEntity__GetBoundRect_hook(uintptr_t* thiz, int a2)
{
	if(!pNetGame){
		int objectId = (int)(*(int*)(a2 + 0x22));
		Log("Load from ipl: %d... index %d", objectId, fullipl);
		fullipl++;
		

		
			uintptr_t *dwModelArray2 = (uintptr_t*)(g_libGTASA+0x87BF48);

			uintptr_t vectorPos = (uintptr_t)(*(uintptr_t *)(dwModelArray2[objectId] + 0x2C));
			
			if(!vectorPos || !dwModelArray2){
				Log("error from load...");
				*(int*)(a2 + 0x22) = 18631;
				return CEntity__GetBoundRect(thiz, a2);
			}else{
				return CEntity__GetBoundRect(thiz, a2);
			}
		}
	
	else{
		return CEntity__GetBoundRect(thiz, a2);
	}
}

enum ePedPieceTypes
{
	PED_PIECE_UNKNOWN = 0,

	PED_PIECE_TORSO = 3,
	PED_PIECE_ASS,
	PED_PIECE_LEFT_ARM,
	PED_PIECE_RIGHT_ARM,
	PED_PIECE_LEFT_LEG,
	PED_PIECE_RIGHT_LEG,
	PED_PIECE_HEAD
};
void gooo(){
	//SetUpHook(SA_ADDR(0x1A7A58), (uintptr_t)sub_29C0C_hook, (uintptr_t*)&sub_29C0C);
	//sub_1B4A8(0x1A8E20 + g_libGTASA + 0x3FC, sub_29D0C, &unk_207F64);
	//SetUpHook(SA_ADDR(0x1A8E20 + 0x3FC), (uintptr_t)sub_29D0C_hook, (uintptr_t*)&sub_29D0C);
 // sub_1B4A8((char *)sub_1A1F40 + g_libGTASA + 0x274, sub_29E10, &unk_207F68);
	//SetUpHook(SA_ADDR(0x1A8E20), (uintptr_t)sub_29E40_hook, (uintptr_t*)&sub_29E40);
}
struct CPedDamageResponseCalculatorInterface
{
	CEntity *pEntity;
	float fDamage;
	ePedPieceTypes bodyPart;
	unsigned int weaponType;
	bool m_bJumpedOutOfMovingCar; // refers to a CPed::Say call (the dying scream?)
};

// thanks Codeesar
struct stPedDamageResponse
{
	CEntity* pEntity;
	float fDamage;
	int iBodyPart;
	eWeaponType iWeaponType;
	bool bSpeak;
};

extern float m_fWeaponDamages[43 + 1];

void (*CPedDamageResponseCalculator__ComputeDamageResponse)(stPedDamageResponse* thiz, CEntity* pEntity, uintptr_t pDamageResponse, bool bSpeak);
void CPedDamageResponseCalculator__ComputeDamageResponse_hook(stPedDamageResponse* thiz, CEntity* pEntity, uintptr_t pDamageResponse, bool bSpeak)
{
	/*if( thiz->iWeaponType < 0 || thiz->iWeaponType > sizeof(m_fWeaponDamages) )
	{
		thiz->fDamage /= 3.0303030303;
	}
	else {
		thiz->fDamage = m_fWeaponDamages[thiz->iWeaponType];
	}
	float fDamage = thiz->fDamage;

	int bodypart = thiz->iBodyPart;

    auto pPlayerPool = pNetGame->GetPlayerPool();
    if(pPlayerPool)
	{

        auto damagedid = pPlayerPool->FindRemotePlayerIDFromGtaPtr((PED_TYPE *) thiz->pEntity); // отправитель урона
		auto issuerid = pPlayerPool->FindRemotePlayerIDFromGtaPtr((PED_TYPE *) pEntity); // получатель

        PLAYERID byteLocalId = pPlayerPool->GetLocalPlayerID();

		auto pedGive = (PED_TYPE *)thiz->pEntity;
		auto pedTake = (PED_TYPE *)pEntity;

		auto pLocalPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();

		//if(!pLocalPed->m_bIsSpawned)
	//		return;

        // player give damage
        if (pedGive == pLocalPed->m_pPed)
		{
          //  CHUD::addGiveDamageNotify(issuerid, thiz->iWeaponType, fDamage);
			CNetGame::sendGiveDamage(issuerid, thiz->iWeaponType, fDamage, bodypart);
        }

        // player take damage
        else if (pedTake == pLocalPed->m_pPed)
		{
			CNetGame::sendTakeDamage(damagedid, thiz->iWeaponType, fDamage, bodypart);

            char nick[MAX_PLAYER_NAME];
            strcpy(nick, pPlayerPool->GetPlayerName(damagedid));

        //    CHUD::addTakeDamageNotify(pPlayerPool->GetPlayerName(damagedid), thiz->iWeaponType,fDamage);
        }
    }*/

	CPedDamageResponseCalculator__ComputeDamageResponse(thiz, pEntity, pDamageResponse, bSpeak);
}
uint32_t (*CWeapon__FireSniper)(CWeapon *pWeaponSlot, PED_TYPE *pFiringEntity, CEntity *a3, VECTOR *vecOrigin);
uint32_t CWeapon__FireSniper_hook(CWeapon *pWeaponSlot, PED_TYPE *pFiringEntity, CEntity *a3, VECTOR *vecOrigin)
{
	if(GamePool_FindPlayerPed() == pFiringEntity)
	{
		if(pGame)
		{
			CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
			//if(pPlayerPed)
				//pPlayerPed->FireInstant();
		}
	}

	return 1;
}

signed int (*CBulletInfo_AddBullet)(CEntity* pEntity, CWeapon* pWeapon, VECTOR vec1, VECTOR vec2);
signed int CBulletInfo_AddBullet_hook(CEntity* pEntity, CWeapon* pWeapon, VECTOR vec1, VECTOR vec2)
{
	vec2.X *= 50.0f;
	vec2.Y *= 50.0f;
	vec2.Z *= 50.0f;
	CBulletInfo_AddBullet(pEntity, pWeapon, vec1, vec2);

	// CBulletInfo::Update
	(( void (*)())(g_libGTASA+0x55E170+1))();
	return 1;
}
bool g_bForceWorldProcessLineOfSight = false;
uint32_t (*CWeapon__ProcessLineOfSight)(VECTOR *vecOrigin, VECTOR *vecEnd, VECTOR *vecPos, CPed **ppEntity, CWeapon *pWeaponSlot, CPed **ppEntity2, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7);
uint32_t CWeapon__ProcessLineOfSight_hook(VECTOR *vecOrigin, VECTOR *vecEnd, VECTOR *vecPos, CPed **ppEntity, CWeapon *pWeaponSlot, CPed **ppEntity2, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr":"=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;

	if(dwRetAddr == 0x567AFC+1 || dwRetAddr == 0x5681BA+1 || dwRetAddr == 0x5688D0+1 || dwRetAddr == 0x568DB8+1)
		g_bForceWorldProcessLineOfSight = true;

	return CWeapon__ProcessLineOfSight(vecOrigin, vecEnd, vecPos, ppEntity, pWeaponSlot, ppEntity2, b1, b2, b3, b4, b5, b6, b7);
}
uint32_t(*CWorld__ProcessLineOfSight)(VECTOR*, VECTOR*, VECTOR*, PED_TYPE**, bool, bool, bool, bool, bool, bool, bool, bool);
uint32_t CWorld__ProcessLineOfSight_hook(VECTOR* vecOrigin, VECTOR* vecEnd, VECTOR* vecPos, PED_TYPE** ppEntity,
	bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7, bool b8)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));

	dwRetAddr -= g_libGTASA;
	if (dwRetAddr == 0x55E2FE + 1 ||
		dwRetAddr == 0x5681BA + 1 ||
		dwRetAddr == 0x567AFC + 1)
	{
		ENTITY_TYPE* pEntity = nullptr;
		MATRIX4X4* pMatrix = nullptr;
		static VECTOR vecPosPlusOffset;

		if (pNetGame->GetLagCompensation() != 2) {
			if (g_pCurrentFiredPed != pGame->FindPlayerPed()) {
				if (g_pCurrentBulletData) {
					if (g_pCurrentBulletData->pEntity) {
						if (!IsGameEntityArePlaceable(g_pCurrentBulletData->pEntity)) {
							pMatrix = g_pCurrentBulletData->pEntity->mat;
							if (pMatrix) {
								if (pNetGame->GetLagCompensation()) {
									vecPosPlusOffset.X = pMatrix->pos.X + g_pCurrentBulletData->vecOffset.X;
									vecPosPlusOffset.Y = pMatrix->pos.Y + g_pCurrentBulletData->vecOffset.Y;
									vecPosPlusOffset.Z = pMatrix->pos.Z + g_pCurrentBulletData->vecOffset.Z;
								}
								else {
									ProjectMatrix(&vecPosPlusOffset, pMatrix, &g_pCurrentBulletData->vecOffset);
								}

								vecEnd->X = vecPosPlusOffset.X - vecOrigin->X + vecPosPlusOffset.X;
								vecEnd->Y = vecPosPlusOffset.Y - vecOrigin->Y + vecPosPlusOffset.Y;
								vecEnd->Z = vecPosPlusOffset.Z - vecOrigin->Z + vecPosPlusOffset.Z;
							}
						}
					}
				}
			}
		}

		static uint32_t result = 0;
		result = CWorld__ProcessLineOfSight(vecOrigin, vecEnd, vecPos, ppEntity, b1, b2, b3, b4, b5, b6, b7, b8);

		if (pNetGame->GetLagCompensation() == 2) {
			if (g_pCurrentFiredPed) {
				if (g_pCurrentFiredPed == pGame->FindPlayerPed()) {
					SendBulletSync(vecOrigin, vecEnd, vecPos, (ENTITY_TYPE**)ppEntity);
				}
			}

			return result;
		}

		if (g_pCurrentFiredPed) {
			if (g_pCurrentFiredPed != pGame->FindPlayerPed()) {
				if (g_pCurrentBulletData) {
					if (!g_pCurrentBulletData->pEntity) {
						PED_TYPE* pLocalPed = pGame->FindPlayerPed()->GetGtaActor();
						if (*ppEntity == pLocalPed || (IN_VEHICLE(pLocalPed) && *(uintptr_t*)ppEntity == pLocalPed->pVehicle)) {
							*ppEntity = nullptr;
							vecPos->X = 0.0f;
							vecPos->Y = 0.0f;
							vecPos->Z = 0.0f;
							return 0;
						}
					}
				}
			}
		}

		if (g_pCurrentFiredPed) {
			if (g_pCurrentFiredPed == pGame->FindPlayerPed()) {
				SendBulletSync(vecOrigin, vecEnd, vecPos, (ENTITY_TYPE**)ppEntity);
			}
		}

		return result;
	}

	return CWorld__ProcessLineOfSight(vecOrigin, vecEnd, vecPos, ppEntity, b1, b2, b3, b4, b5, b6, b7, b8);
}
uintptr_t(*gettex)(const char *a1);
uintptr_t gettex_hook(const char *a1)
{
  int result; // r0
  
  return gettex("E_Aks_Fnaf");
  
  if ( !strcmp(a1, "grass_128hv")){
	  return gettex("Grass_128HV");
  }
	result = gettex(a1);
 /* if ( !strcmp(a1, "white") || (result = gettex(a1), !result) )
  {
    
    if ( !result )
    {
    //  Log("NO TEXTURE %s", a1);
     // result = (uintptr_t)LoadTextureFromDB("samp", "replaceme");
    }
  }*/
  return result;
}
int(*LoadAtomicFile)( int a1,  int a2);
int LoadAtomicFile_hook( int a1,  int a2)
{
  int v3; // r4

  v3 = LoadAtomicFile(a1, a2);
  if ( !v3 )
  {
    Log("Invalid atomic %d", a2);
   // if ( *(uintptr_t *)(g_libGTASA + 0x88F7CC) )
    //  ((void (*)(void))(g_libGTASA + 0x337A89))();
	return LoadAtomicFile(a1, 18631);
  }
  return v3;
}
int (*sub_4CBDC)(int a1, int a2, int a3, int a4, int a5, float a6, float a7, float a8);
int  sub_4CBDC_hook(int a1, int a2, int a3, int a4, int a5, float a6, float a7, float a8)
{
  ((void ( *)(int, int, int, int, int, float, float, float))(0x55E091 + g_libGTASA))(
    a1,
    a2,
    a3,
    a4,
    a5,
    a6 * 50.0,
    a7 * 50.0,
    a8 * 50.0);
  return ((int (*)(void))(0x55E091 + g_libGTASA + 0xE0))();
}
/*int __fastcall sub_40EC0(uint8_t a1 = 1) {
  // Пустая функция - замените ее реализацией, если это необходимо.
  return 0;
}

int __fastcall sub_40E04() {
  // Пустая функция - замените ее реализацией, если это необходимо.
  return 0;
}

int __fastcall sub_40E80(uint8_t a1) {
  // Пустая функция - замените ее реализацией, если это необходимо.
  return 0;
}

void __fastcall sub_40FB0(int a1) {
  // Пустая функция - замените ее реализацией, если это необходимо.
}

void __fastcall sub_41000(uint8_t a1) {
  // Пустая функция - замените ее реализацией, если это необходимо.
}

int __fastcall sub_57DF0(uintptr_t a1, int* a2) {
  // Пустая функция - замените ее реализацией, если это необходимо.
  return 0;
}

int __fastcall sub_58358(uintptr_t a1) {
  // Пустая функция - замените ее реализацией, если это необходимо.
  return 0;
}

int __fastcall sub_55814(int a1, uintptr_t a2, char* a3) {
  // Пустая функция - замените ее реализацией, если это необходимо.
  return 0;
}

int __fastcall sub_40E1C() {
  // Пустая функция - замените ее реализацией, если это необходимо.
  return 0;
}

void __fastcall sub_40FD0(int a1) {
  // Пустая функция - замените ее реализацией, если это необходимо.
}


int __fastcall sub_53C2C(int a1) {
  uint16_t v1; // r8
  uint16_t v2; // r9
  int v4; // r0
  int v5; // r0
  int v6; // r0
  float *v7; // r0
  int v8; // r2
  int v9; // r1
  int v10; // r0
  int v11; // r4
  int v12; // r0
  int v14; // [sp+18h] [bp-40h] BYREF
  int v15; // [sp+1Ch] [bp-3Ch]
  float v16; // [sp+20h] [bp-38h]
  uintptr_t v17[2]; // [sp+28h] [bp-30h] BYREF
  float v18; // [sp+30h] [bp-28h]
  char v19[4]; // [sp+38h] [bp-20h] BYREF
  int v20; // [sp+3Ch] [bp-1Ch]

  if (*(uint8_t*)(a1 + 0x10)) {
    v1 = *(uint16_t*)dword_617B60;
    *(uint16_t*)dword_617B60 = static_cast<uint16_t>(sub_40EC0());
    v2 = *(uint16_t*)(g_libGTASA + 0x8B0FBC);
    v4 = sub_40EC0(*(uint8_t*)(a1 + 0x10));
    if (v4 == 4) {
      v4 = 0;
    }
    *(uint16_t*)(g_libGTASA + 0x8B0FBC) = static_cast<uint16_t>(v4);
    sub_40E04();
    v5 = sub_40E80(*(uint8_t*)(a1 + 0x10));
    sub_40FB0(v5);
    sub_41000(*(uint8_t*)(a1 + 0x10));
  }

  dword_744060 = a1;
  v6 = *(uint8_t*)(a1 + 0x23);
  if (*(uint8_t*)(a1 + 0x23)) {
    v6 = a1 + 0x24;
  }
  dword_744064 = v6;

  v18 = 0.0f;
  v17[0] = 0;
  v17[1] = 0;
    v16 = 0.0f;
  v14 = 0;
  v15 = 0;

  if (*(uintptr_t*)(a1 + 0xC) &&
      sub_57DF0(*(uintptr_t*)(a1 + 8), &dword_744064) &&
      sub_58358(*(uintptr_t*)(a1 + 0xC)) == 1 &&
      (sub_55814(word_1D4F0A, *(uintptr_t*)(a1 + 8), v19), v19[0] == 0x22)) {
    reinterpret_cast<void(__fastcall*)(int, uintptr_t, uintptr_t, uintptr_t)>(0x56668D + g_libGTASA)(
        *(uintptr_t*)(a1 + 0xC) + 0x1C * *(uint8_t*)(*(uintptr_t*)(a1 + 0xC) + 0x718) + 0x5A0,
        *(uintptr_t*)(a1 + 0xC),
        0,
        0);
  } else {
    v7 = reinterpret_cast<float*>(reinterpret_cast<int(__fastcall*)(uintptr_t, int)>((g_libGTASA + 0x56BD61))(*(uintptr_t*)(*(uintptr_t*)(a1 + 0xC) + 0x1C * *(uint8_t*)(*(uintptr_t*)(a1 + 0xC) + 0x718) + 0x5A0), 1));
    v8 = reinterpret_cast<int>(v7 + 0xA);
    v9 = reinterpret_cast<int>(v7 + 9);
    v16 = v7[0xB];
    v14 = v9;
    v15 = v8;
    v10 = *(uintptr_t*)(a1 + 0xC);
    if (v10) {
      reinterpret_cast<void(__fastcall*)(int, uintptr_t*, int, uintptr_t)>((g_libGTASA + 0x436591))(v10, v17, 0x18, 0);
      v11 = *(uintptr_t*)(a1 + 0xC);
    } else {
      v11 = 0;
    }
    v18 = (float)(v16 + v18) + 0.15f;
    if (v11) {
      reinterpret_cast<void(__fastcall*)(int, int*, int, uintptr_t)>((g_libGTASA + 0x4383C1))(v11, &v14, 0x18, 0);
      v11 = *(uintptr_t*)(a1 + 0xC);
    }
    if (v11) {
      reinterpret_cast<void(__fastcall*)(int, int, uintptr_t*, int*, uintptr_t, uintptr_t, uintptr_t, uintptr_t, int)>((g_libGTASA + 0x567965))(
          v11 + 0x1C * *(uint8_t*)(v11 + 0x718) + 0x5A0,
          v11,
          v17,
          &v14,
          0,
          0,
          0,
          0,
          1);
    }
  }

  dword_744064 = 0;
  dword_744060 = 0;

  if (*(uintptr_t*)(a1 + 0x10)) {
    *(int*)(g_libGTASA + 0x8E864C) = 0;
    *(uint16_t*)dword_617B60 = v1;
    *(uint16_t*)(g_libGTASA + 0x8B0FBC) = v2;
    v12 = sub_40E1C();
    sub_40FD0(v12);
  }
  return 0;
}
*/


/*int (*sub_4CC4C)(int a1, int a2);
int  sub_4CC4C_hook(int a1, int a2)
{
  int *v3; // r6
  int v4; // r5

  if ( sub_57DE0() == a2 )
  {
    v3 = (int *)dword_207F24;
    if ( dword_207F24 )
    {
      v4 = *(int *)dword_207F24;
      if ( !*(int *)dword_207F24 )
      {
        v4 = operator new(0x28Au);
   //     sub_5212C();
        *v3 = v4;
      }
      sub_53C2C(v4);
    }
  }
  return a1;
}*/
int (*rwOpenGLSetRenderStateNoExtras)(int a1, int a2);
int rwOpenGLSetRenderStateNoExtras_hook(int a1, int a2)
{
    int v4; // lr
    int v8; // r1

    if ( a1 != 1 )
    {
        return 1;
    }

    uintptr_t dwRetAddr = 0;
    __asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));
    dwRetAddr -= g_libGTASA;

    if ( v4 - g_libGTASA > 0x19BD8E )
    {
        if ( dwRetAddr != 0x19BFFD )
        {
            v8 = -17009;
            LABEL_8:
            v8 = 25;
            //if(g_HZClass){
            //    sub_6F4B8(*(uintptr_t **)g_HZClass, a2);
            //}
            //if ( dwRetAddr != v8 ){
            //	*(uintptr_t *)(dword_748CA0 + 52) = a4;
            //}
        }
    }
    else if ( dwRetAddr != 0x19BC57 )
    {
        v8 = -17053;
        goto LABEL_8;
    }
    return rwOpenGLSetRenderStateNoExtras(1, a2);
}
typedef struct _RxOpenGLMeshInstanceData
{
    //RxOpenGLMeshInstanceData struc ; (sizeof=0x38, align=0x4, copyof_396)

    uintptr_t primType;                      // 00000000
    uintptr_t vertexDesc;                    // 00000000
    uintptr_t vertexStride;                  // 00000004
    uintptr_t numVertices;                   // 00000008
    uintptr_t vertexDataSize;                // 0000000C
    uintptr_t emuArrayRef;                   // 00000010
    uintptr_t minVertexIdx;                  // 00000014
    uintptr_t vertexBufferRef;               // 00000018
    uintptr_t indexBufferRef;                // 0000001C
    uintptr_t indexOffset;                   // 00000020
    uintptr_t numIndices;                    // 00000024
    uintptr_t indexData;                     // 00000028
    RpMaterial* material;                    // 0000002C
    uintptr_t vertexAlpha;                   // 00000030

    // RxOpenGLMeshInstanceData
} RxOpenGLMeshInstanceData;
int (*ScanWorld)(uintptr_t *thiz);
int ScanWorld_hook(uintptr_t *thiz)
{
    float v1; // s16
    uintptr_t *v2; // r0
    float v3; // s14
    float v4; // s15
    int v5; // r5
    uintptr_t v6; // r0
    int v7; // r5
    float *v8; // r3
    bool v9; // r1
    int *v10; // r2
    int v11; // r0
    int v12; // r1
    int v13; // r2

    float v15; // [sp+4h] [bp+4h] BYREF
    float v16; // [sp+8h] [bp+8h]
    float v17; // [sp+Ch] [bp+Ch]
    float v18; // [sp+10h] [bp+10h]
    float v19; // [sp+14h] [bp+14h]
    float v20; // [sp+18h] [bp+18h]
    float v21; // [sp+1Ch] [bp+1Ch]
    float v22; // [sp+20h] [bp+20h]
    float v23; // [sp+24h] [bp+24h]
    float v24; // [sp+28h] [bp+28h]
    float v25[42]; // [sp+2Ch] [bp+2Ch] BYREF

    // Проверяем и освобождаем защиту глобальных переменных
  //  if ((*(int *)(g_libGTASA + 0x8BF2C4) & 1) == 0 && _cxa_guard_acquire(g_libGTASA + 0x8BF2C4))
  //      _cxa_guard_release(g_libGTASA + 0x8BF2C4);
  //  if ((*(int *)(g_libGTASA + 0x8BF2C8) & 1) == 0 && _cxa_guard_acquire(g_libGTASA + 0x8BF2C8))
       // _cxa_guard_release(g_libGTASA + 0x8BF2C8);

    // Инициализируем переменные
    v1 = (float)(g_libGTASA + 0x8B10E0 + 0x84);
    v2 = (uintptr_t *)memset(v25, 0, 0x9Cu);
    v3 = (float)(g_libGTASA + 0x8B10E0 + 0x68);
    v4 = (float)(g_libGTASA + 0x8B10E0 + 0x6C);

    // Вызов функции: CVisibilityPlugins::InitAlphaEntityList(v2)
   

    // Создаем объект CWorld и очищаем текущий код сканирования
    v6 = ((uintptr_t (*)(uintptr_t *))(g_libGTASA + 0x55D780 + 1))(v2);
    // Вызов функции: CWorld::ClearScanCodes(v6)
    ((void (*)(uintptr_t ))(g_libGTASA + 0x3C1C34 + 1))(v6);

    // Получаем данные о видимости
    v8 = (float *)(g_libGTASA + 0x8B081C + 0x30);
    if (!*(int *)(g_libGTASA + 0x8B081C))
        v8 = (float *)(g_libGTASA + 0x8B080C);

    // Добавляем проверку на расстояние до камеры
    float cameraX = *(float *)(g_libGTASA + 0x8B080C);
    float cameraY = *(float *)(g_libGTASA + 0x8B0810);
    float cameraZ = *(float *)(g_libGTASA + 0x8B0814);
    float* pBF2CC = (float*)(g_libGTASA + 0x8BF2CC);
    float* pBF2D8 = (float*)(g_libGTASA + 0x8BF2D8);
    float distSqr = (v25[0] - cameraX) * (v25[0] - cameraX) +
                    (v25[1] - cameraY) * (v25[1] - cameraY) +
                    (v25[2] - cameraZ) * (v25[2] - cameraZ);
    float minDistSqr = 25.0f * 25.0f; // Минимальное расстояние 25 единиц

    if (distSqr > minDistSqr)
    {
        float distX = (float)(g_libGTASA + 0x8B080C) - pBF2CC[0];
        float distY = (float)(g_libGTASA + 0x8B0810) - pBF2CC[1];
        float distZ = (float)(g_libGTASA + 0x8B0814) - pBF2CC[2];
        if (sqrt(distX * distX + distY * distY + distZ * distZ) < 16.0f)
            v9 = (float)(((float)(*(int *)(g_libGTASA + 0x8B1118) * pBF2D8[1]) +
                        (float)(*(int *)(g_libGTASA + 0x8B1114) * pBF2D8[0]) +
                        (float)(*(float *)(g_libGTASA + 0x8B111C) * pBF2D8[2])) > 0.98);
        else
            v9 = 0;
        *(char *)(g_libGTASA + 0x8BF2E4) = v9;
    }
    else
    {
        // Объект слишком близко к камере, пропускаем его
        *(char *)(g_libGTASA + 0x8BF2E4) = 0;
    }

	if ( (int*)(g_libGTASA + 0x8B081C) )
    v10 = (int *)((uintptr_t)(g_libGTASA + 0x8B081C) + 0x30);
  else
    v10 = (int *)(g_libGTASA + 0x8B080C);

	v11 = *v10;
    v12 = v10[1];
    v13 = v10[2];
	
	//float* pBF2D8 = (float*)(g_libGTASA + 0x8BF2D8);
	
    *(float*)(g_libGTASA + 0x8BF2CC) = v11;
*(float*)(g_libGTASA + 0x8BF2CC + 4) = v12;
*(float*)(g_libGTASA + 0x8BF2CC + 8) = v13;

// Присваиваем значения в flt_8BF2D8
*(int*)(g_libGTASA + 0x8BF2D8) = *(int*)(g_libGTASA + 0x8B1114);
*(int*)(g_libGTASA + 0x8BF2D8 + 4) = *(int*)(g_libGTASA + 0x8B1118);
*(float*)(g_libGTASA + 0x8BF2D8 + 8) = *(float*)(g_libGTASA + 0x8B111C);
    // Вычисляем и присваиваем значения в массив v25
    v25[0xF] = (float)(1.0 / v1) * (float)(v25[3] * 300.0);
    v25[0x10] = (float)(1.0 / v1) * (float)(v25[4] * 300.0);
    v25[0x11] = (float)(1.0 / v1) * (float)(v25[5] * 300.0);
    v25[0x17] = v25[0x11];
    v25[0x12] = (float)(1.0 / v1) * (float)(v25[6] * 300.0);
    v25[0x13] = (float)(1.0 / v1) * (float)(v25[7] * 300.0);
    v25[0x14] = (float)(1.0 / v1) * (float)(v25[8] * 300.0);
    v25[0x1A] = v25[0x14];
    v25[0x15] = v25[0xF] * 0.2;
    v25[0x1F] = (float)(1.0 / v1) * (float)(v25[0xD] * 300.0);
    v25[0x1B] = (float)(1.0 / v1) * (float)(v25[9] * 300.0);
    v25[0x1D] = (float)(1.0 / v1) * (float)(v25[0xB] * 300.0);
    v25[0x1C] = (float)(1.0 / v1) * (float)(v25[0xA] * 300.0);
    v25[0x1E] = (float)(1.0 / v1) * (float)(v25[0xC] * 300.0);
    v25[0x23] = v25[0x1D];
    v25[0x20] = (float)(1.0 / v1) * (float)(v25[0xE] * 300.0);
    v25[0x26] = v25[0x20];
    v25[0x18] = v25[0x12] * 0.2;
    v25[0x16] = v25[0x10] * 0.2;
    v25[0x19] = v25[0x13] * 0.2;
    v25[0x21] = v25[0x1B] * 0.2;
    v25[0x24] = v25[0x1E] * 0.2;
    v25[0x22] = v25[0x1C] * 0.2;
    v25[0x25] = v25[0x1F] * 0.2;


    // Вызов функции: RwV3dTransformPoints(v25, v25, 0xD, v7)
  //  ((void (*)(float *, float *, int, int))(g_libGTASA + 0x1BBA48 + 1))(v25, v25, 0xD, v7);

    // Сканируем мир
    *(int *)(g_libGTASA + 0x8BF30D) = 0;
	
	v15 = (float)(v25[0] / 50.0) + 60.0;
    v17 = (float)(v25[0xF] / 50.0) + 60.0;
    v19 = (float)(v25[0x12] / 50.0) + 60.0;
    v21 = (float)(v25[0x1B] / 50.0) + 60.0;
    v16 = (float)(v25[1] / 50.0) + 60.0;
    v18 = (float)(v25[0x10] / 50.0) + 60.0;
    v23 = (float)(v25[0x1E] / 50.0) + 60.0;
    v20 = (float)(v25[0x13] / 50.0) + 60.0;
    v22 = (float)(v25[0x1C] / 50.0) + 60.0;
    v24 = (float)(v25[0x1F] / 50.0) + 60.0;
	
    // Вызов функции: CWorldScan::ScanWorld((int)&v15, 5, (void (__fastcall *)(int, int))CRenderer::ScanSectorList)
    ((int (*)(int, int, void (__fastcall *)(int, int)))(g_libGTASA + 0x5572C0 + 1))((int)&v15, 5, (void (__fastcall *)(int, int))(g_libGTASA + 0x3B3290 + 1));
    // Вызов функции: CWorldScan::ScanWorld((int)&v15, 5, (void (__fastcall *)(int, int))CRenderer::ScanBigBuildingList)
	
    v15 = (float)(v25[0] / 200.0) + 15.0;
    v17 = (float)(v25[3] / 200.0) + 15.0;
    v19 = (float)(v25[6] / 200.0) + 15.0;
    v21 = (float)(v25[9] / 200.0) + 15.0;
    v16 = (float)(v25[1] / 200.0) + 15.0;
    v18 = (float)(v25[4] / 200.0) + 15.0;
    v20 = (float)(v25[7] / 200.0) + 15.0;
    v22 = (float)(v25[0xA] / 200.0) + 15.0;
    v23 = (float)(v25[0xC] / 200.0) + 15.0;
    v24 = (float)(v25[0xD] / 200.0) + 15.0;
	
    return ((int (*)(int, int, void (__fastcall *)(int, int)))(g_libGTASA + 0x5572C0 + 1))((int)&v15, 5, (void (__fastcall *)(int, int))(g_libGTASA + 0x3B380C + 1));
}

typedef struct _CustomEnvMapPipeMaterialData
{
    // 00000000 CustomEnvMapPipeMaterialData struc ; (sizeof=0xC, align=0x4, copyof_2309)

    uintptr_t cfEnvScaleX;                   // 00000000
    uintptr_t cfEnvScaleY;                   // 00000001
    uintptr_t cfEnvTransSclX;                // 00000002
    uintptr_t cfEnvTransSclY;                // 00000003
    uintptr_t cfShininess;                   // 00000004
    uintptr_t __pad0;                        // 00000005
    uintptr_t nCurrentRenderFrame;           // 00000006
    RwTexture* pEnvTexture;                   // 00000008

    // CustomEnvMapPipeMaterialData
} CustomEnvMapPipeMaterialData;

float*  m_fDNBalanceParam;
int* rwOpenGLLightingEnabled;
int* rwOpenGLColorMaterialEnabled;
float* rwOpenGLOpaqueBlack;
int* RasterExtOffset;
char* doPop;

void                (*DrawStoredMeshData)(RxOpenGLMeshInstanceData*);
void                (*ResetEnvMap)();
void                (*SetSecondVertexColor)(uint8_t, float);
void                (*EnableAlphaModulate)(float);
void                (*DisableAlphaModulate)();
void                (*SetEnvMap)(void*, float, int);
void                (*_rwOpenGLSetRenderState)(RwRenderState, int); //
void                (*_rwOpenGLGetRenderState)(RwRenderState, void*); //
void                (*_rwOpenGLLightsSetMaterialPropertiesORG)(const RpMaterial *mat, RwUInt32 flags); //
void                (*SetNormalMatrix)(float, float, RwV2d); //
void                (*emu_glColor4fv)(const GLfloat *v);
void                (*emu_glLightModelfv)(GLenum pname, const GLfloat *params);
void                (*emu_glMaterialfv)(GLenum face, GLenum pname, const GLfloat *params);
void                (*emu_glColorMaterial)(GLenum face, GLenum mode);
void                (*emu_glEnable)(GLenum cap);
void                (*emu_glDisable)(GLenum cap);


void (*CustomPipeRenderCB_Mobile)(RwResEntry *entry, void *obj, RwUInt8 type, RwUInt32 flags);
void CustomPipeRenderCB_Mobile_hook(RwResEntry *entry, void *obj, RwUInt8 type, RwUInt32 flags)
{
    RpMeshHeader* header = *(RpMeshHeader**)(entry);
    RxOpenGLMeshInstanceData* meshData = *(RxOpenGLMeshInstanceData**)(&entry[1]);
    int numMeshes = header->numMeshes;

    static int* ms_envMapPluginOffset = *(int**)(g_libGTASA + 0x5DE71C);

    //SetSecondVertexColor(1, *m_fDNBalanceParam);
    while(--numMeshes >= 0)
    {
        RpMaterial* material = meshData->material;
        RwTexture* texture = material->texture;

        uint8_t alpha = material->color.alpha;
        bool hasAlphaVertexEnabled = meshData->vertexAlpha || alpha != 255;

        if(hasAlphaVertexEnabled && alpha == 0) // Fully invisible?
        {
            ++meshData;
            continue;
        }

        if(hasAlphaVertexEnabled) EnableAlphaModulate(alpha / 255.0f);
        _rwOpenGLSetRenderState(rwRENDERSTATEVERTEXALPHAENABLE, hasAlphaVertexEnabled);
        if(*rwOpenGLLightingEnabled)
        {
            _rwOpenGLLightsSetMaterialPropertiesORG(material, flags);
        }
        else
        {
            if(*rwOpenGLColorMaterialEnabled)
            {
                emu_glDisable(0x0B57);
                *rwOpenGLColorMaterialEnabled = 0;
            }
            if((flags & rxGEOMETRY_PRELIT) == 0)
            {
                emu_glColor4fv(rwOpenGLOpaqueBlack);
            }
        }

        bool hasEnvMap = (*(int*)&material->surfaceProps.specular & 0x1) != 0; // This part is VERY WEIRD. Optimizer?
        if(hasEnvMap)
        {
            CustomEnvMapPipeMaterialData* envData = *(CustomEnvMapPipeMaterialData**)((int)material + *ms_envMapPluginOffset);
            RwTexture* envTexture = envData->pEnvTexture;
            envTexture->filterAddressing = 17; // What is this? Special ENV texture mode?
            SetEnvMap(*(void **)((char*)&envTexture->raster->parent + *RasterExtOffset), envData->cfShininess * 1.5f, 0);
            RwV2d vec;
            vec.x = 0.0;
            vec.y = 0.0;
            SetNormalMatrix(envData->cfEnvScaleX, envData->cfEnvScaleY, vec);
            *doPop = 0;
        }

        if(!(flags & (rxGEOMETRY_TEXTURED2 | rxGEOMETRY_TEXTURED)) || texture == NULL)
        {
            _rwOpenGLSetRenderState(rwRENDERSTATETEXTURERASTER, false);
            DrawStoredMeshData(meshData);
            if (hasAlphaVertexEnabled) DisableAlphaModulate();
            if (hasEnvMap) ResetEnvMap();
        }
        else if((texture->raster->privateFlags & 0x1) != 0)
        {
            if (hasAlphaVertexEnabled) DisableAlphaModulate();
        }
        else
        {
            rwOpenGLSetRenderStateNoExtras(rwRENDERSTATETEXTURERASTER, (int)texture->raster);
            _rwOpenGLSetRenderState(rwRENDERSTATETEXTUREFILTER, texture->filterAddressing);
            DrawStoredMeshData(meshData); // Dual Pass here? Or shaders?!
            if (hasAlphaVertexEnabled) DisableAlphaModulate();
            if (hasEnvMap) ResetEnvMap();
        }
        ++meshData;
    }
    SetSecondVertexColor(0, 0.0f);
}

uintptr_t DoHeadLightEffect_RegisterCorona_hook(uintptr_t thiz, int a1, uintptr_t a2, int a3, int a4, int a5, int a6, VECTOR const &a7, int a8, int a9, int a10, int a11, int a12, int a13, int a14, int a15, int a16, int a17, int a18, int a19, int a20)
{
  int v21; // r10
  int v22; // r12
  bool v23; // zf
  float v24; // s2

//  v21 = (uint8_t)(byte_745FC9 & byte_745FC8);
//  v22 = a5;
 // v23 = v21 == 0xFF;
 // if ( v21 == 0xFF )
   // v23 = (uint8_t)byte_745FCA == 0xFF;
 // if ( !v23 )
 // {
	 Log("a5 %d, a4 %d, a3 %d", (int)a5, (int)a4, (int)a3);
   // v22 = (unsigned int)(float)((float)((float)(unsigned int)a5 / 255.0) * (float)(uint8_t)1.0);
  //  a4 = (unsigned int)(float)((float)((float)(unsigned int)a4 / 255.0) * (float)(uint8_t)1.0);
  //  a3 = (unsigned int)(float)((float)((float)(unsigned int)a3 / 255.0) * (float)(uint8_t)1.0);
 // }
  v24 = a10 * 4.0f;
 //if ( !byte_746028 )
   // v24 = a10;
//unsigned int id, CEntity *attachTo, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha, CVector const &posn, float radius, float farClip, eCoronaType coronaType, eCoronaFlareType flaretype, bool enableReflection, bool checkObstacles, int _param_not_used, float angle, bool longDistance, float nearClip, unsigned char fadeState, float fadeSpeed, bool onlyFromBelow, bool reflectionDelay

  return ((uintptr_t ( *)(uintptr_t, int, uintptr_t, int, int, int, int, VECTOR const &posn, int, int, int, int, int, int, int, int, int, int, int, int, int))(0x52EDD1 + g_libGTASA))(
           thiz,
		   a1,
           a2,
           a3,
           a4,
           /*v22*/a5,
           a6,
           a7,
           a8,
           a9,
           a10,
           a11,
           a12,
           a13,
           a14,
           a15,
           a16,
           a17,
           a18,
           a19,
           a20);
}
int (*CVisibilityPlugins__SetRenderWareCamera)();
int CVisibilityPlugins__SetRenderWareCamera_hook()
{
    int v0; // r5
    int result; // r0

    v0 = *(uintptr_t *)(g_libGTASA + 0x8B08F8);
    *(uintptr_t *) (g_libGTASA + 0x8B08F8) = 0x3DE147AE;
    result = CVisibilityPlugins__SetRenderWareCamera();
    //result = g_libGTASA;
    *(uintptr_t *)(g_libGTASA + 0x8B08F8) = v0;
    return result;
}
int (*CVehicleModelInfo__CLinkedUpgradeList__AddUpgradeLink)(int result, uint16_t a2, uint16_t a3);
int CVehicleModelInfo__CLinkedUpgradeList__AddUpgradeLink_hook(int result, uint16_t a2, uint16_t a3)
{
  unsigned int v3; // r3
  int v4; // r1

  v3 = *(unsigned int *)(result + 0x190);
  //if ( v3 <= 0x63 )
//  {
    *(uint16_t *)(result + 2 * v3) = a2;
    v4 = *(int *)(result + 0x190);
    *(int *)(result + 0x190) = v4 + 1;
    result += 2 * v4;
    *(uint16_t *)(result + 0xC8) = a3;
//  }
  return result;
}
unsigned int (*CVehicleModelInfo__CLinkedUpgradeList__FindOtherUpgrade)(uintptr_t* thiz, uint16_t gg);
unsigned int CVehicleModelInfo__CLinkedUpgradeList__FindOtherUpgrade_hook(uintptr_t* thiz, uint16_t gg)
{
  return 0xFFFFFFFF;
}
int (*CVehicleModelInfo__AddVehicleUpgrade)(uintptr_t* thiz, int modelId);
int CVehicleModelInfo__AddVehicleUpgrade_hook(uintptr_t* thiz, int modelId)
{
//	Log("Trying to add upgrade %d...", modelId);
	/*int huetan = (int)(g_libGTASA + 0x7159E8);
	int huetan2 = huetan >> 8;
	thiz = (uintptr_t*)(g_libGTASA + 0x8B93D0 + 0x208 * huetan2); */
	//thiz = ((uintptr_t* ( *)(int, int))((char *)0x3AC4AD + g_libGTASA))(0xFFFFFFFF, 0); //find ped veh
	
	//pChatWindow->AddDebugMessage("Trying to add upgrade %d...", modelId);
  return CVehicleModelInfo__AddVehicleUpgrade(thiz, modelId);
}
extern int dword_200490;


int (*CAEVehicleAudioEntity__JustFinishedAccelerationLoop)(uintptr_t* thiz);
int CAEVehicleAudioEntity__JustFinishedAccelerationLoop_hook(uintptr_t* thiz){
	CVehicle *pVehicle = nullptr;
	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
	VEHICLEID id = pVehiclePool->FindIDFromGtaPtr(pPlayerPed->GetGtaVehicle());
	pVehicle = pVehiclePool->GetAt(id);
	pVehicle->CreateExhaust(nullptr);
	return CAEVehicleAudioEntity__JustFinishedAccelerationLoop(thiz);
}

int (*dword_629554)(int a1, int a2);
int __fastcall sub_465A4(int a1, int a2) {
	
  uintptr_t calledFrom = 0;
    __asm__ volatile("mov %0, lr"
    : "=r"(calledFrom));
    calledFrom -= g_libGTASA;
  int v4 = g_libGTASA;
  if (calledFrom == 0x289A63) {
    char* v5 = (char*)0x6706D8 + g_libGTASA;
    int v6 = 0;
    int v7 = 8;
    int v13 = a1;
    do {
      if (a2 == *reinterpret_cast<uintptr_t*>(v5 + v7) << 0xB) {
        Log("LAST REQUEST %d", v6);
        v4 = g_libGTASA;
      }
      ++v6;
      v5 = (char*)0x6706D8 + v4;
      v7 += 0x14;
	  //uintptr_t *dwModelArray2 = (uintptr_t*)(g_libGTASA+0x87BF48);
	  
	  
    } while (v6 != 0x66CC);

    uintptr_t* v8 = reinterpret_cast<uintptr_t*>(v5 + 8);
    int v9 = 0;
  //  dword_200490 = 0xFFFFFFFF;
    do {
      if (a2 == *v8 << 0xB) {
        int v10 = reinterpret_cast<unsigned char*>(v8)[0xFFFFFFFF];
     //   if (sub_3C388(v13) == v10)
         // dword_200490 = v9;
      }
      ++v9;
      v8 += 5;
    } while (v9 != 0x66CC);

    Log("pizdec ahuenno %d", static_cast<unsigned char>(v5[0x7A343]));
     Log("LAST mega pon %d", v6);
    a1 = v13;
  } 
  
  return dword_629554(a1, a2);
}

void InstallHooks()
{
	PROTECT_CODE_INSTALLHOOKS;
	
	  WriteMemory(0x394C80 + g_libGTASA, (uintptr_t)"\x2D\xE9\xF0\x41", 4);
	WriteMemory(0x394C80 + g_libGTASA + 4, (uintptr_t)"\x23\x4B", 2);
	
	SetUpHook(g_libGTASA + 0x00394C80, (uintptr_t)LoadAtomicFile_hook, (uintptr_t*)& LoadAtomicFile);
	
	//CHook::RET(g_libGTASA + 0x39044C);
	//NOP(g_libGTASA + 0x3906C8, 2);
	CModelInfo::injectHooks();
	CHook::RET(g_libGTASA + 0x314150);
	CHook::RET(g_libGTASA + 0x31436C);
	CHook::RET(g_libGTASA + 0x314808);
	
	//CHook::RET(g_libGTASA + 0x2919B8);
	
	
	CHook::RET(g_libGTASA + 0x289A0C); // cd stream tread
	
	  
	//SetUpHook(SA_ADDR(0x1A7A58), (uintptr_t)sub_29C0C_hook, (uintptr_t*)&sub_29C0C);
   // SetUpHook(SA_ADDR(0x248784), (uintptr_t)SetIfIplIsRequired_hook, (uintptr_t*)&SetIfIplIsRequired);                                                                    
	SetUpHook(SA_ADDR(0x29947C), (uintptr_t)CCollision__ProcessVerticalLine_hook, (uintptr_t*)&CCollision__ProcessVerticalLine);
	//SetUpHook(SA_ADDR(0x1B1808), (uintptr_t)_RwTextureDestroy_hook, (uintptr_t*)&_RwTextureDestroy);
	//SetUpHook(SA_ADDR(0x4025E8), (uintptr_t)LoadColFile_hook, (uintptr_t*)&LoadColFile);
	CHook::InlineHook(g_libGTASA, 0x567964, &CWeapon__FireInstantHit_hook, &CWeapon__FireInstantHit);
	
	CHook::RET(SA_ADDR(0x1B1808));
	
	CHook::InlineHook(g_libGTASA, 0x390AA8, &CEntity__GetBoundRect_hook, &CEntity__GetBoundRect);
	
	
	
	CHook::InlineHook(g_libGTASA, 0x3379F8 + 0x1C, &CVehicleModelInfo__CLinkedUpgradeList__FindOtherUpgrade_hook, &CVehicleModelInfo__CLinkedUpgradeList__FindOtherUpgrade);
	CHook::InlineHook(g_libGTASA, 0x3379F8, &CVehicleModelInfo__CLinkedUpgradeList__AddUpgradeLink_hook, &CVehicleModelInfo__CLinkedUpgradeList__AddUpgradeLink);
	//CHook::InlineHook(g_libGTASA, 0x3B2210, &ScanWorld_hook, &ScanWorld);
	CHook::InlineHook(g_libGTASA, 0x516090, &CVehicleModelInfo__AddVehicleUpgrade_hook, &CVehicleModelInfo__AddVehicleUpgrade);
	CHook::InlineHook(0x23B910, g_libGTASA, &sub_465A4, &dword_629554);
	/* 
                                                                                                          CRASHFIXES END                                                                                                      */

	/* 
                                                                                                                  GTA SA HOOKS                                                                                                         */
	//SetUpHook(SA_ADDR(0x395714), (uintptr_t)LoadCollision__hook, (uintptr_t*)&LoadCollision);
//	SetUpHook(g_libGTASA+0x40C530, (uintptr_t)InitialiseRenderWare_hook, (uintptr_t*)&InitialiseRenderWare);
	//CHook::InlineHook(g_libGTASA, 0x3C70C0, &CWorld__ProcessLineOfSight_hook, &CWorld__ProcessLineOfSight);    
	CHook::InlineHook(g_libGTASA, 0x5667D0, &sub_4CBDC_hook, &sub_4CBDC); 
	//CHook::InlineHook(g_libGTASA, 0x569A80 + 0x52, &sub_4CC4C_hook, &sub_4CC4C); 	
	
	SetUpHook(g_libGTASA + 0x00281398, (uintptr_t)CWidgetRegionLook__Update_hook, (uintptr_t*)& CWidgetRegionLook__Update);
	
	SetUpHook(g_libGTASA+0x239D5C, (uintptr_t)TouchEvent_hook, (uintptr_t*)&TouchEvent);
	//SetUpHook(g_libGTASA+0x28E83C, (uintptr_t)CStreaming_InitImageList_hook, (uintptr_t*)&CStreaming_InitImageList);
	SetUpHook(g_libGTASA+0x336690, (uintptr_t)CModelInfo_AddPedModel_hook, (uintptr_t*)&CModelInfo_AddPedModel); // hook is dangerous
	SetUpHook(g_libGTASA+0x3DBA88, (uintptr_t)CRadar__GetRadarTraceColor_hook, (uintptr_t*)&CRadar__GetRadarTraceColor); // dangerous
	SetUpHook(g_libGTASA+0x3DAF84, (uintptr_t)CRadar__SetCoordBlip_hook, (uintptr_t*)&CRadar__SetCoordBlip);
	SetUpHook(g_libGTASA+0x3DE9A8, (uintptr_t)CRadar__DrawRadarGangOverlay_hook, (uintptr_t*)&CRadar__DrawRadarGangOverlay);
	//sub_6446C(0x518D06 + g_libGTASA, sub_62A1C);
  //sub_6446C(0x518D06 + g_libGTASA + 0xBA, sub_62A1C);
	//CHook::Redirect(g_libGTASA, 0x518D06, &DoHeadLightEffect_RegisterCorona_hook);
	//CHook::Redirect(g_libGTASA, 0x518D06 + 0xBA, &DoHeadLightEffect_RegisterCorona_hook);
	////SetUpHook(g_libGTASA + 0x28AAAC, (uintptr_t)CustomPipeRenderCB_Mobile_hook, (uintptr_t*)&CustomPipeRenderCB_Mobile);
	//SetUpHook(g_libGTASA + 0x186248, (uintptr_t)rwOpenGLSetRenderStateNoExtras_hook, (uintptr_t*)&rwOpenGLSetRenderStateNoExtras);
	
	

	SetUpHook(g_libGTASA+0x482E60, (uintptr_t)CTaskComplexEnterCarAsDriver_hook, (uintptr_t*)&CTaskComplexEnterCarAsDriver);
	SetUpHook(g_libGTASA+0x4833CC, (uintptr_t)CTaskComplexLeaveCar_hook, (uintptr_t*)&CTaskComplexLeaveCar);
	//SetUpHook(g_libGTASA + 0x0044A4CC, (uintptr_t)PointGunInDirection_hook, (uintptr_t*)&PointGunInDirection);
	CodeInject(g_libGTASA+0x2D99F4, (uintptr_t)PickupPickUp_hook, 1);
	SetUpHook(g_libGTASA + 0x00327528, (uintptr_t)ComputeDamageResponse_hooked, (uintptr_t*)(&ComputeDamageResponse));

	SetUpHook(g_libGTASA + 0x00336268, (uintptr_t)CModelInfo_AddAtomicModel_hook, (uintptr_t*)& CModelInfo_AddAtomicModel);

	//SetUpHook(g_libGTASA + 0x00336618, (uintptr_t)CModelInfo_AddVehicleModel_hook, (uintptr_t*)& CModelInfo_AddVehicleModel); // dangerous

	SetUpHook(g_libGTASA + 0x0033DA5C, (uintptr_t)CAnimManager__UncompressAnimation_hook, (uintptr_t*)& CAnimManager__UncompressAnimation);
	SetUpHook(g_libGTASA + 0x00531118, (uintptr_t)CCustomRoadsignMgr__RenderRoadsignAtomic_hook, (uintptr_t*)& CCustomRoadsignMgr__RenderRoadsignAtomic);
	SetUpHook(g_libGTASA + 0x001AECC0, (uintptr_t)RwFrameAddChild_hook, (uintptr_t*)& RwFrameAddChild);
	SetUpHook(g_libGTASA + 0x002DFD30, (uintptr_t)CUpsideDownCarCheck__IsCarUpsideDown_hook, (uintptr_t*)& CUpsideDownCarCheck__IsCarUpsideDown);
	SetUpHook(g_libGTASA + 0x0033AD78, (uintptr_t)CAnimBlendNode__FindKeyFrame_hook, (uintptr_t*)& CAnimBlendNode__FindKeyFrame);
	// TextDraw render
	SetUpHook(g_libGTASA + 0x003D5894, (uintptr_t)CHud__DrawScriptText_hook, (uintptr_t*)& CHud__DrawScriptText);

	//CHook::InlineHook(g_libGTASA, 0x564E28, &CWeapon__ProcessLineOfSight_hook, &CWeapon__ProcessLineOfSight);
	//CHook::InlineHook(g_libGTASA, 0x55E090, &CBulletInfo_AddBullet_hook, &CBulletInfo_AddBullet);
	//CHook::InlineHook(g_libGTASA, 0x56668C, &CWeapon__FireSniper_hook, &CWeapon__FireSniper);
	//CHook::InlineHook(g_libGTASA, 0x327528, &CPedDamageResponseCalculator__ComputeDamageResponse_hook, &CPedDamageResponseCalculator__ComputeDamageResponse);
	//CHook::InlineHook(g_libGTASA, 0x567964, &CWeapon__FireInstantHit_hook, &CWeapon__FireInstantHit);
	//CHook::InlineHook(g_libGTASA, 0x3C70C0, &CWorld__ProcessLineOfSight_hook, &CWorld__ProcessLineOfSight);


	//widget
	SetUpHook(g_libGTASA + 0x00276510, (uintptr_t)CWidgetButtonEnterCar_Draw_hook, (uintptr_t*)& CWidgetButtonEnterCar_Draw);
	// attached objects
	SetUpHook(g_libGTASA + 0x003C1BF8, (uintptr_t)CWorld_ProcessPedsAfterPreRender_hook, (uintptr_t*)& CWorld_ProcessPedsAfterPreRender);
	//remove building
	SetUpHook(g_libGTASA + 0x00395994, (uintptr_t)CFileLoader__LoadObjectInstance_hook, (uintptr_t*)& CFileLoader__LoadObjectInstance);

	// retexture
	SetUpHook(g_libGTASA + 0x00391E20, (uintptr_t)CObject__Render_hook, (uintptr_t*)& CObject__Render);

	// gettexture fix crash
	SetUpHook(g_libGTASA + 0x00258910, (uintptr_t)GetTexture_hook, (uintptr_t*)& GetTexture_orig);

	// steal objects fix
	SetUpHook(g_libGTASA + 0x003AC114, (uintptr_t)CPlayerInfo__FindObjectToSteal_hook, (uintptr_t*)& CPlayerInfo__FindObjectToSteal);
//	SetUpHook(g_libGTASA + 0x001BE990, (uintptr_t)gettex_hook, (uintptr_t*)& gettex);

	// GetFrameFromID fix
	SetUpHook(g_libGTASA + 0x00335CC0, (uintptr_t)CClumpModelInfo_GetFrameFromId_hook, (uintptr_t*)& CClumpModelInfo_GetFrameFromId);
	// RLEDecompress fix
	SetUpHook(g_libGTASA + 0x1BC314, (uintptr_t)RLEDecompress_hook, (uintptr_t*)&RLEDecompress);
	
//	CHook::RET(g_libGTASA + 0x1BC314);

	// todo: 3 pools fix crash

	// random crash fix
	SetUpHook(g_libGTASA + 0x001A8530, (uintptr_t)RenderQueue__ProcessCommand_hook, (uintptr_t*)& RenderQueue__ProcessCommand);
	// fix
	SetUpHook(g_libGTASA + 0x001B9D74, (uintptr_t)_rwFreeListFreeReal_hook, (uintptr_t*)& _rwFreeListFreeReal);

//	SetUpHook(g_libGTASA + 0x00274AB4, (uintptr_t)CWidgetButton__Update_hook, (uintptr_t*)& CWidgetButton__Update);
	SetUpHook(g_libGTASA + 0x00274218, (uintptr_t)CWidget__IsTouched_hook, (uintptr_t*)& CWidget__IsTouched);

	SetUpHook(g_libGTASA + 0x004052B8, (uintptr_t)CVehicleModelInfo__SetupCommonData_hook, (uintptr_t*)& CVehicleModelInfo__SetupCommonData);

	SetUpHook(g_libGTASA + 0x0035BE30, (uintptr_t)CAEVehicleAudioEntity__GetVehicleAudioSettings_hook, (uintptr_t*)& CAEVehicleAudioEntity__GetVehicleAudioSettings);

	SetUpHook(g_libGTASA + 0x002C0304, (uintptr_t)CDarkel__RegisterCarBlownUpByPlayer_hook, (uintptr_t*)& CDarkel__RegisterCarBlownUpByPlayer);
	SetUpHook(g_libGTASA + 0x002C072C, (uintptr_t)CDarkel__ResetModelsKilledByPlayer_hook, (uintptr_t*)&CDarkel__ResetModelsKilledByPlayer);
	SetUpHook(g_libGTASA + 0x002C0758, (uintptr_t)CDarkel__QueryModelsKilledByPlayer_hook, (uintptr_t*)& CDarkel__QueryModelsKilledByPlayer);
	SetUpHook(g_libGTASA + 0x002C0778, (uintptr_t)CDarkel__FindTotalPedsKilledByPlayer_hook, (uintptr_t*)& CDarkel__FindTotalPedsKilledByPlayer);
	SetUpHook(g_libGTASA + 0x002C0D04, (uintptr_t)CDarkel__RegisterKillByPlayer_hook, (uintptr_t*)& CDarkel__RegisterKillByPlayer);

	SetUpHook(g_libGTASA + 0x00338CBC, (uintptr_t)CVehicleModelInfo__SetEditableMaterials_hook, (uintptr_t*)& CVehicleModelInfo__SetEditableMaterials);
	SetUpHook(g_libGTASA + 0x0050DEF4, (uintptr_t)CVehicle__ResetAfterRender_hook, (uintptr_t*)& CVehicle__ResetAfterRender);

	SetUpHook(g_libGTASA + 0x003B67F8, (uintptr_t)CRopes__Update_hook, (uintptr_t*)&CRopes__Update);

	SetUpHook(g_libGTASA + 0x3986CC, (uintptr_t)CGame__Process_hook, (uintptr_t*)& CGame__Process);

	SetUpHook(g_libGTASA + 0x004D4A6C, (uintptr_t)CAutomobile__ProcessEntityCollision_hook, (uintptr_t*)& CAutomobile__ProcessEntityCollision);
	
	SetUpHook(g_libGTASA + 0x00398334, (uintptr_t)CGame__Shutdown_hook, (uintptr_t*)& CGame__Shutdown);
	CHook::InlineHook(g_libGTASA, 0x0026CE30, &MobileSettings__GetMaxResWidth_hook, &MobileSettings__GetMaxResWidth);
	WriteMemory(g_libGTASA + 0x003DA86C,
		(uintptr_t)"\x80\xB4"\
		"\x00\xAF"\
		"\x1B\x4B"\
		"\x7B\x44"\
		"\x1B\x68", 10);
		
		
		
	NOP(g_libGTASA + 0x003DA876, 3);
	
	//CHook::RET(g_libGTASA + 0x3B0844);
	
	NOP(g_libGTASA + 0x004021FA, 2);
	NOP(g_libGTASA + 0x004024F6, 2);
	NOP(g_libGTASA + 0x004024E4, 2); //deleting stroke with DeleteColission if dist > 300

	// headlights color, wheel size, wheel align
	SetUpHook(g_libGTASA + 0x005466EC, (uintptr_t)CShadows__StoreCarLightShadow_hook, (uintptr_t*)& CShadows__StoreCarLightShadow);
	SetUpHook(g_libGTASA + 0x0035C7A4, (uintptr_t)CAEVehicleAudioEntity__JustFinishedAccelerationLoop_hook, (uintptr_t*)& CAEVehicleAudioEntity__JustFinishedAccelerationLoop);
	SetUpHook(g_libGTASA + 0x00518EC4, (uintptr_t)CVehicle__DoHeadLightBeam_hook, (uintptr_t*)& CVehicle__DoHeadLightBeam);

	SetUpHook(g_libGTASA + 0x004E671C, (uintptr_t)CAutomobile__PreRender_hook, (uintptr_t*)& CAutomobile__PreRender);
	SetUpHook(g_libGTASA + 0x004DC6E8, (uintptr_t)CAutomobile__UpdateWheelMatrix_hook, (uintptr_t*)& CAutomobile__UpdateWheelMatrix);
	SetUpHook(g_libGTASA + 0x003E8D48, (uintptr_t)CMatrix__Rotate_hook, (uintptr_t*)& CMatrix__Rotate);
	SetUpHook(g_libGTASA + 0x003E8884, (uintptr_t)CMatrix__SetScale_hook, (uintptr_t*)& CMatrix__SetScale);

	SetUpHook(g_libGTASA + 0x0046CEF4, (uintptr_t)CTaskSimpleUseGun__RemoveStanceAnims_hook, (uintptr_t*)& CTaskSimpleUseGun__RemoveStanceAnims);

	SetUpHook(g_libGTASA + 0x003DA86C, (uintptr_t)CRadar__LimitRadarPoint_hook, (uintptr_t*)& CRadar__LimitRadarPoint); // TO FIX
	SetUpHook(g_libGTASA + 0x005529FC, (uintptr_t)CSprite2d__DrawBarChart_hook, (uintptr_t*)& CSprite2d__DrawBarChart);
	SetUpHook(g_libGTASA + 0x005353B4, (uintptr_t)CFont__PrintString_hook, (uintptr_t*)& CFont__PrintString);
	SetUpHook(g_libGTASA + 0x0055265C, (uintptr_t)CSprite2d__Draw_hook, (uintptr_t*)& CSprite2d__Draw);

	SetUpHook(g_libGTASA + 0x0027D8A8, (uintptr_t)CWidgetPlayerInfo__DrawWanted_hook, (uintptr_t*)& CWidgetPlayerInfo__DrawWanted);

	SetUpHook(g_libGTASA + 0x0027CE88, (uintptr_t)CWidgetPlayerInfo__DrawWeaponIcon_hook, (uintptr_t*)& CWidgetPlayerInfo__DrawWeaponIcon);
	SetUpHook(g_libGTASA + 0x00389D74, (uintptr_t)CCam__Process_hook, (uintptr_t*)& CCam__Process);

	SetUpHook(g_libGTASA + 0x003D6E6C, (uintptr_t)CHud__Draw_hook, (uintptr_t*)& CHud__Draw);
	SetUpHook(g_libGTASA + 0x0039DC68, (uintptr_t)CPad__CycleCameraModeDownJustDown_hook, (uintptr_t*)& CPad__CycleCameraModeDownJustDown);

	SetUpHook(g_libGTASA + 0x00391FE0, (uintptr_t)CEntity__GetIsOnScreen_hook, (uintptr_t*)& CEntity__GetIsOnScreen);
	SetUpHook(g_libGTASA + 0x0031B164, (uintptr_t)FxEmitterBP_c__Render_hook, (uintptr_t*)& FxEmitterBP_c__Render);
	//
	SetUpHook(g_libGTASA + 0x0043A17C, (uintptr_t)CPed__ProcessEntityCollision_hook, (uintptr_t*)&CPed__ProcessEntityCollision);

	//SetUpHook(g_libGTASA + 0x3B1778, &CRenderer__RenderEverythingBarRoads_hook, &CRenderer__RenderEverythingBarRoads);
	SetUpHook(g_libGTASA + 0x3B1778, (uintptr_t)CRenderer__RenderEverythingBarRoads_hook, (uintptr_t*)&CRenderer__RenderEverythingBarRoads);
	
	NOP(g_libGTASA + 0x40C63E, 2); //CHud::Initialise(void)
    NOP(g_libGTASA + 0x40C646, 2); //CPostEffects::Initialise(void)
SetUpHook(g_libGTASA + 0x28AAAC, (uintptr_t)CustomPipeRenderCB_hook, (uintptr_t*)&CustomPipeRenderCB);
	//SetUpHook(g_libGTASA + 0x3D4ED8, (uintptr_t)DrawRadar_hook, (uintptr_t*)&DrawRadar);
	
	/*SetUpHook(g_libGTASA + 0x1EEC90, (uintptr_t)rxOpenGLDefaultAllInOneRenderCB_hook, (uintptr_t*)&rxOpenGLDefaultAllInOneRenderCB);
	SetUpHook(g_libGTASA + 0x2CE788, (uintptr_t)CObject__ProcessGarageDoorBehaviour_hook, (uintptr_t*)&CObject__ProcessGarageDoorBehaviour);
	
	//SetUpHook(g_libGTASA + 0x1B1808, (uintptr_t)_RwTextureDestroy_hook, (uintptr_t*)&_RwTextureDestroy);
	SetUpHook(g_libGTASA + 0x194B04, (uintptr_t)emu_ArraysDelete_hook, (uintptr_t*)&emu_ArraysDelete);
	SetUpHook(g_libGTASA + 0x1BA580, (uintptr_t)RwResourcesFreeResEntry_hook, (uintptr_t*)&RwResourcesFreeResEntry);
  	SetUpHook(g_libGTASA + 0x194B20, (uintptr_t)emu_ArraysGetID_hook, (uintptr_t*)&emu_ArraysGetID);
  	SetUpHook(g_libGTASA + 0x50C5F8, (uintptr_t)SetCompAlphaCB_hook, (uintptr_t*)&SetCompAlphaCB);
  	SetUpHook(g_libGTASA + 0x41EAB4, (uintptr_t)CPlayerPedDataSaveStructure__Construct_hook, (uintptr_t*)&CPlayerPedDataSaveStructure__Construct);
	//SetUpHook(g_libGTASA + 0x1bdc3c, (uintptr_t)CTextureDatabaseRuntime__GetEntry_hook, (uintptr_t)CTextureDatabaseRuntime__GetEntry);
  	SetUpHook(g_libGTASA + 0x368850, (uintptr_t)CAudioEngine__Service_hook, (uintptr_t*)&CAudioEngine__Service);
  	SetUpHook(g_libGTASA + 0x35AC44, (uintptr_t)CAEVehicleAudioEntity__GetAccelAndBrake_hook, (uintptr_t*)&CAEVehicleAudioEntity__GetAccelAndBrake);
	//SetUpHook(g_libGTASA + 0x29947C, (uintptr_t)CCollision__ProcessVerticalLine_hook, (uintptr_t*)&CCollision__ProcessVerticalLine);
*/
//	SetUpHook(g_libGTASA + 0x3B1778, (uintptr_t)CRenderer__RenderEverythingBarRoads_hook, (uintptr_t*)CRenderer__RenderEverythingBarRoads);
	
	//DrawRadarTexture(CRadar::radarBgPos1.x1, CRadar::radarBgPos1.y1, CRadar::radarBgPos2.x1, CRadar::radarBgPos2.y1, false);
	HookCPad();
	// Настройки
	CHook::NOP(g_libGTASA + 0x266460, 2); // Game - TrafficMode
	CHook::NOP(g_libGTASA + 0x266496, 2); // Game - AimMode
	CHook::NOP(g_libGTASA + 0x261A50, 2); // Main - Language
	CHook::NOP(g_libGTASA + 0x2665EE, 2); // Game - SocialClub
	
	SetUpHook(g_libGTASA + 0x18DAAC, (uintptr_t)emu_glEndInternal_hook, (uintptr_t*)&emu_glEndInternal);
	CHook::InlineHook(g_libGTASA, 0x005311D0, &CDraw__SetFOV_hook, &CDraw__SetFOV);
	
	/* WriteMemory(0x55CFA4 + g_libGTASA, (uintptr_t)"\xF3\xEE\x04\x4A", 4);
	WriteMemory(0x55CFA4 + g_libGTASA + 4, (uintptr_t)"\x42\x4B" , 2);
    SetUpHook(g_libGTASA + 0x55CFA4, (uintptr_t)CVisibilityPlugins__SetRenderWareCamera_hook, (uintptr_t*)&CVisibilityPlugins__SetRenderWareCamera);
    CHook::RET(0x55D4A0 + g_libGTASA);
	CHook::RET(0x55D4EC + g_libGTASA);
	CHook::RET(0x55D7F8 + g_libGTASA);*/
	/*SetUpHook(
            g_libGTASA + 0x1A7C9C,
            (uintptr_t)ES2Texture__ES2Texture_end_hook,
            (uintptr_t*)&ES2Texture__ES2Texture_end);
    
	SetUpHook(
            g_libGTASA + 0x1A7D80,
            (uintptr_t)ES2Texture__ES2Texture_end_hook1,
            (uintptr_t*)&ES2Texture__ES2Texture_end1);*/
  //  InstallRQHooks();
 // WriteMemory(v2 + 0x1A656A, (uintptr_t)"\x2C\x08", 2u);// \x2C\x08
  //  WriteMemory(g_libGTASA + 0x1A7D04, (uintptr_t)"\x2C\x08", 2u);// \x2C\x08
  //  WriteMemory(g_libGTASA + 0x1A7DE8, (uintptr_t)"\x2C\x08", 2u);// \x2C\x08
  // / WriteMemory(g_libGTASA + 0x1A76D0, (uintptr_t)"\x2C\x08", 2u);// \x2C\x08
  //  WriteMemory(g_libGTASA + 0x1A68D0, (uintptr_t)"\x2C\x08", 2u);// \x2C\x08
	
}