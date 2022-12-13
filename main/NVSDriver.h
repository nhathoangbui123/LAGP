#ifndef __NVSDriver__
#define __NVSDriver__

#include "nvs_flash.h"

#ifdef __cplusplus
extern "C" {
#endif

void NVSDriverInit();
void NVSDriverOpen(const char* paramname);
void NVSDriverClose();
esp_err_t NVSDriverCommit();
bool NVSDriverRead(const char* paramname, char **out);
esp_err_t NVSDriverWrite(const char* paramname, char *val);
esp_err_t NVSDriverEraseKey(const char* paramname);
esp_err_t NVSDriverEraseAll();

#ifdef __cplusplus
}
#endif

#endif //__NVSDriver__