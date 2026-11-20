#include <extdll.h>
#include <meta_api.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>

meta_globals_t *gpMetaGlobals;
gamedll_funcs_t *gpGamedllFuncs;
mutil_funcs_t *gpMetaUtilFuncs;
enginefuncs_t g_engfuncs;
globalvars_t *gpGlobals;

plugin_info_t Plugin_info = {
    META_INTERFACE_VERSION,
    "Ozel Yerel Pratik Eklentisi v3",
    "3.0",
    "2026",
    "Genis kapsamli yerel pratik ve sunucu komut eklentisi",
    "OZEL_PRATIK_V3",
    "https://example.com",
    PT_ANYTIME,
    PT_ANYTIME
};

static void Komut_Durum()
{
    const char *mapname = STRING(gpGlobals->mapname);
    LOG_CONSOLE(PLID, "[OZEL_PRATIK_V3] Aktif | Harita: %s | Yercekimi: %.0f",
                mapname ? mapname : "bilinmiyor",
                CVAR_GET_FLOAT("sv_gravity"));
}

static void Komut_Para()
{
    if (CMD_ARGC() < 2)
    {
        SERVER_PRINT("[OZEL_PRATIK_V3] Kullanim: ozel_para <miktar 0-16000>\n");
        return;
    }

    int miktar = std::atoi(CMD_ARGV(1));
    if (miktar < 0) miktar = 0;
    if (miktar > 16000) miktar = 16000;

    LOG_CONSOLE(PLID, "[OZEL_PRATIK_V3] Hedef para: %d$", miktar);
}

static void Komut_Restart()
{
    SERVER_PRINT("[OZEL_PRATIK_V3] Round 1 saniye icinde yeniden baslatiliyor...\n");
    SERVER_COMMAND("sv_restartround 1\n");
}

static void Komut_Yercekimi()
{
    if (CMD_ARGC() < 2)
    {
        SERVER_PRINT("[OZEL_PRATIK_V3] Kullanim: ozel_yercekimi <deger orn. 800 veya 400>\n");
        return;
    }

    int grav = std::atoi(CMD_ARGV(1));
    if (grav < 100) grav = 100;
    if (grav > 2000) grav = 2000;

    char cmd[64];
    std::snprintf(cmd, sizeof(cmd), "sv_gravity %d\n", grav);
    LOG_CONSOLE(PLID, "[OZEL_PRATIK_V3] sv_gravity -> %d", grav);
    SERVER_COMMAND("sv_cheats 1\n");
    SERVER_COMMAND(cmd);
}

static void Komut_RoundSuresi()
{
    if (CMD_ARGC() < 2)
    {
        SERVER_PRINT("[OZEL_PRATIK_V3] Kullanim: ozel_round_suresi <dakika 1-9>\n");
        return;
    }

    int dk = std::atoi(CMD_ARGV(1));
    if (dk < 1) dk = 1;
    if (dk > 9) dk = 9;

    char cmd[64];
    std::snprintf(cmd, sizeof(cmd), "mp_roundtime %d\n", dk);
    LOG_CONSOLE(PLID, "[OZEL_PRATIK_V3] mp_roundtime -> %d dk", dk);
    SERVER_COMMAND("sv_cheats 1\n");
    SERVER_COMMAND(cmd);
}

static void Komut_Freezetime()
{
    if (CMD_ARGC() < 2)
    {
        SERVER_PRINT("[OZEL_PRATIK_V3] Kullanim: ozel_freezetime <saniye 0-15>\n");
        return;
    }

    int sn = std::atoi(CMD_ARGV(1));
    if (sn < 0) sn = 0;
    if (sn > 15) sn = 15;

    char cmd[64];
    std::snprintf(cmd, sizeof(cmd), "mp_freezetime %d\n", sn);
    LOG_CONSOLE(PLID, "[OZEL_PRATIK_V3] mp_freezetime -> %d sn", sn);
    SERVER_COMMAND("sv_cheats 1\n");
    SERVER_COMMAND(cmd);
}

static void Komut_BotDondur()
{
    SERVER_PRINT("[OZEL_PRATIK_V3] Botlar durduruldu (bot_stop 1).\n");
    SERVER_COMMAND("bot_stop 1\n");
}

static void Komut_BotCoz()
{
    SERVER_PRINT("[OZEL_PRATIK_V3] Botlar serbest birakildi (bot_stop 0).\n");
    SERVER_COMMAND("bot_stop 0\n");
}

static void Komut_BotTemizle()
{
    SERVER_PRINT("[OZEL_PRATIK_V3] Tum botlar temizleniyor (bot_kick).\n");
    SERVER_COMMAND("bot_kick\n");
}

static void Komut_HaritaBilgi()
{
    const char *mapname = STRING(gpGlobals->mapname);
    LOG_CONSOLE(PLID, "[OZEL_PRATIK_V3] Harita: %s | Maks Oyuncu: %d | Zaman: %.1f",
                mapname ? mapname : "yok",
                gpGlobals->maxClients,
                gpGlobals->time);
}

static void Komut_Yardim()
{
    SERVER_PRINT("=== OZEL_PRATIK_V3 KOMUT LISTESI ===\n");
    SERVER_PRINT("ozel_durum | ozel_harita_bilgi | ozel_restart\n");
    SERVER_PRINT("ozel_para <0-16000> | ozel_yercekimi <100-2000>\n");
    SERVER_PRINT("ozel_round_suresi <1-9> | ozel_freezetime <0-15>\n");
    SERVER_PRINT("ozel_bot_dondur | ozel_bot_coz | ozel_bot_temizle\n");
}

C_DLLEXPORT void WINAPI GiveFnptrsToDll(enginefuncs_t *pengfuncsFromEngine, globalvars_t *pGlobals)
{
    std::memcpy(&g_engfuncs, pengfuncsFromEngine, sizeof(enginefuncs_t));
    gpGlobals = pGlobals;
}

C_DLLEXPORT int Meta_Query(const char *ifvers, plugin_info_t **pPlugInfo, mutil_funcs_t *pMetaUtilFuncs)
{
    *pPlugInfo = &Plugin_info;
    gpMetaUtilFuncs = pMetaUtilFuncs;
    return TRUE;
}

C_DLLEXPORT int Meta_Attach(PLUG_LOADTIME now, META_FUNCTIONS *pFunctionTable, meta_globals_t *pMGlobals, gamedll_funcs_t *pGamedllFuncs)
{
    gpMetaGlobals = pMGlobals;
    gpGamedllFuncs = pGamedllFuncs;

    REG_SVR_COMMAND("ozel_durum", Komut_Durum);
    REG_SVR_COMMAND("ozel_para", Komut_Para);
    REG_SVR_COMMAND("ozel_restart", Komut_Restart);
    REG_SVR_COMMAND("ozel_yercekimi", Komut_Yercekimi);
    REG_SVR_COMMAND("ozel_round_suresi", Komut_RoundSuresi);
    REG_SVR_COMMAND("ozel_freezetime", Komut_Freezetime);
    REG_SVR_COMMAND("ozel_bot_dondur", Komut_BotDondur);
    REG_SVR_COMMAND("ozel_bot_coz", Komut_BotCoz);
    REG_SVR_COMMAND("ozel_bot_temizle", Komut_BotTemizle);
    REG_SVR_COMMAND("ozel_harita_bilgi", Komut_HaritaBilgi);
    REG_SVR_COMMAND("ozel_yardim", Komut_Yardim);

    SERVER_PRINT("[OZEL_PRATIK_V3] Eklenti ve 11 komut yerel sunucuya yuklendi.\n");
    return TRUE;
}

C_DLLEXPORT int Meta_Detach(PLUG_LOADTIME now, PL_UNLOAD_REASON reason)
{
    SERVER_PRINT("[OZEL_PRATIK_V3] Eklenti kaldirildi.\n");
    return TRUE;
}

C_DLLEXPORT int GetEntityAPI2(DLL_FUNCTIONS *pFunctionTable, int *interfaceVersion)
{
    return TRUE;
}
