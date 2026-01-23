#pragma once

uintptr_t FindLibrary(const char* library);
void cp1251_to_utf8(char *out, const char *in, unsigned int len = 0);
bool IsGameEntityArePlaceable(ENTITY_TYPE* pEntity);
void AND_OpenLink(const char* szLink);