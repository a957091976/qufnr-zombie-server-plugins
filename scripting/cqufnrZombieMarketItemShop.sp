#include <loringlib>
#include <zombierelolied2018>
#include "lol_ammofix/lol_ammofix.inc"

public Plugin myinfo = { 
	name = "Zombie Market: Item Shop"
	, description = ""
	, url = ""
	, version = "0.0"
	, author = "qufnr"
}

#define MOTHER_ZOMBIE_POINT_HUMANS	false

#define DISPLAY_ARMOR_AND_DOLLARS	false

#define	NEED_MORE_ACCOUNT(%1)		PrintToChat ( %1, "[ZR] 자금이 부족합니다." );
#define TO_HIGH_HEALTH_VALUE(%1)	PrintToChat ( %1, "[ZR] 체력이 너무 많습니다." );
#define HIT_SOUND		"buttons/arena_switch_press_02.wav"

#define ZOMBIE_GRENADE_SOUND_EFFECT "survival/barrel_fall_02.wav"
#define ZOMBIE_GRENADE_THROW_SPEED	1000.0				/**< 좀비 수류탄 날라가는 속도 */
#define ZOMBIE_GRENADE_TARGET_DAMAGE_DISTANCE	190.0	/**< 좀비 수류탄 인간 피해 거리 */
#define ZOMBIE_GRENADE_PROP_DAMAGE_DISTANCE		150.0	/**< 좀비 수류탄 프롭 피해 거리 */
#define ZOMBIE_GRENADE_KNOCKBACK_SCALE			320.0	/**< 좀비 수류탄 넉백 */
#define ZOMBIE_GRENADE_KNOCKBACK_SCALE_DUCK		250.0	/**< 좀비 수류탄 넉백 (앉을 때) */
#define ZOMBIE_GRENADE_PROP_DAMAGE_MIN		80
#define ZOMBIE_GRENADE_PROP_DAMAGE_MAX		100
#define ZOMBIE_GRENADE_LIFETIME	2.5		/**< 좀비 수류탄 폭발 시간 */
#define ZOMBIE_GRENADE_COOLDOWN		30.0	/**< 좀비 수류탄 재구입 시간 */

char g_strZombieGrenadeModel[2][128] = {
//	"models/props_fairgrounds/elephant.mdl"
//	, "models/props_fairgrounds/giraffe.mdl"
	"models/crow.mdl"
	, "models/crow.mdl"
}

#define DOLLAR_TYPE		2		//	1 = 딜에 따라 달러 지급, 2 = 시간이 지나면 달러 지급
#define DOLLAR_RECEIVE_SOUND	"qufnr/dollars.mp3"	//	달러 지급 소리
#define DOLLAR_RECEIVE_MIN	5000
#define DOLLAR_RECEIVE_MAX	6000

#if MOTHER_ZOMBIE_POINT_HUMANS
int g_iHumanPointEnvSprite[MAXPLAYERS + 1] = { -1, ... };
#endif

#if DISPLAY_ARMOR_AND_DOLLARS
Handle g_hDisplayHandler[MAXPLAYERS + 1] = { null, ... };
#endif
#if DOLLAR_TYPE == 2
Handle g_hReceiveDollarTimer = null;
int g_iReceiveDollarTime = 0;
#endif
Handle g_hHudSync[2] = null;
int g_iClientHudColor[MAXPLAYERS + 1] = { 0, ... };
int g_iClientHudStyle[MAXPLAYERS + 1] = { 0, ... };
bool g_bDisplay[MAXPLAYERS + 1] = { true, ... };
bool g_bSnowballMode[MAXPLAYERS + 1] = { false, ... };
bool g_bZombieGrenadeCooldown[MAXPLAYERS + 1] = { false, ... };
bool g_bHasZombieGrenade[MAXPLAYERS + 1] = { false, ... };

#define MAX_HUD_COLORS	12
int g_iHudColors[MAX_HUD_COLORS][4] = {
	{ 255, 255, 255, 255 }
	, { 255, 255, 255, 255 }
	, { 150, 200, 255, 255 }
	, { 53, 110, 255, 255 }
	, { 200, 100, 255, 255 }
	, { 255, 41, 36, 255 }
	, { 255, 113, 36, 255 }
	, { 255, 247, 36, 255 }
	, { 62, 255, 36, 255 }
	, { 36, 255, 144, 255 }
	, { 255, 121, 153, 255 }
	, { 213, 226, 134, 255 }
};

#define MAX_GRENADES	3
char g_strGrenadeList[MAX_GRENADES][3][64] = {
//	Weapon Index Name, Weapon Display Name, Weapon Price
	{ "weapon_hegrenade", "고폭 수류탄", "3000" }
	, { "weapon_flashbang", "조명탄", "1000" }
	, { "weapon_smokegrenade", "얼음 수류탄", "2500" }
};

#define MAX_ZOMBIE_HEALTHS	3
char g_strZombieHealthList[MAX_ZOMBIE_HEALTHS][2][64] = {
//	Health Value, Price
	{ "1000", "2000" }
	, { "3000", "5000" }
	, { "6000", "8000" }
}; 

public void OnPluginStart () {
	RegConsoleCmd ( "sm_dhide", onClientCommandDHide );
	RegConsoleCmd ( "sm_zbuy", onClientCommandShop );
//	RegConsoleCmd ( "autobuy", onClientCommandShop );
	RegConsoleCmd ( "sm_snowball", onClientCommandSnowball );
#if DOLLAR_TYPE == 2
	HookEvent ( "round_start", onRoundStart );
	HookEvent ( "round_end", onRoundEnd );
#endif
	HookEvent ( "weapon_fire", onWeaponFire );
	HookEvent ( "player_spawn", onPlayerSpawn );
}

public void OnMapStart () {
	for ( int i = 0; i < sizeof ( g_strZombieGrenadeModel ); i ++ )
		PrecacheModel ( g_strZombieGrenadeModel[i], true );
	for ( int i = 0; i < sizeof ( g_hHudSync ); i ++ )
		g_hHudSync[i] = CreateHudSynchronizer ();
	ServerCommand ( "mp_playercashawards 0" );
	ServerCommand ( "mp_teamcashawards 1" );
	if ( isWinter () )	ServerCommand ( "ammo_grenade_limit_snowballs 64" );
	PrecacheSound ( DOLLAR_RECEIVE_SOUND, true );
	PrecacheSound ( HIT_SOUND, true );
	PrecacheSound ( ZOMBIE_GRENADE_SOUND_EFFECT, true );
	
	loringlib_PrecacheEffect ( "ParticleEffect" );
	PrecacheGeneric ( "particles/qufnr/zombie_particles2.pcf", true );
	PrecacheGeneric ( "particles/qufnr/zombiebomb.pcf", true );
	AddFileToDownloadsTable ( "particles/qufnr/zombie_particles2.pcf" );
	AddFileToDownloadsTable ( "particles/qufnr/zombiebomb.pcf" );
	loringlib_PrecacheParticleName ( "zombie_wallhack" );
	loringlib_PrecacheParticleName ( "zombie_bomb_explode" );
	loringlib_PrecacheParticleName ( "zombie_bomb_trail" );
}

public void OnMapEnd () {
	for ( int i = 1; i <= MaxClients; i ++ )
		if ( loringlib_IsValidClient ( i ) )
			for ( int j = 0; j < sizeof ( g_hHudSync ); j ++ )
				ClearSyncHud ( i, g_hHudSync[j] );
}

public void OnClientPostAdminCheck ( int client ) {
#if DISPLAY_ARMOR_AND_DOLLARS
	if ( g_hDisplayHandler[client] == null )
		g_hDisplayHandler[client] = CreateTimer ( 0.1, timerDisplayRepeat, client, TIMER_REPEAT );
#endif
#if MOTHER_ZOMBIE_POINT_HUMANS
	g_iHumanPointEnvSprite[client] = -1;
#endif
	g_bSnowballMode[client] = false;
	g_bZombieGrenadeCooldown[client] = false;
	g_bHasZombieGrenade[client] = false;
	SDKHook ( client, SDKHook_OnTakeDamage, onTakeDamage );
}

public void OnClientDisconnect ( int client ) {
#if DISPLAY_ARMOR_AND_DOLLARS
	if ( g_hDisplayHandler[client] != null ) {
		KillTimer ( g_hDisplayHandler[client] );
		g_hDisplayHandler[client] = null;
	}
#endif
	
	SDKUnhook ( client, SDKHook_OnTakeDamage, onTakeDamage );
}

#if DOLLAR_TYPE == 2
public void onRoundStart ( Event ev, const char[] name, bool dontBroadcast ) {
	float time = GetConVarFloat ( FindConVar ( "mp_roundtime" ) );
	time *= 60.0;
	
	g_iReceiveDollarTime = RoundFloat ( time - 90.0 );
	if ( g_iReceiveDollarTime <= 120 )
		return;
	
	g_hReceiveDollarTimer = CreateTimer ( 1.0, timerReceiveDollarTimer, _, TIMER_FLAG_NO_MAPCHANGE | TIMER_REPEAT );
	
	for ( int i = 0; i < MAXPLAYERS + 1; i ++ ) {
		g_bHasZombieGrenade[i] = false;
		g_bZombieGrenadeCooldown[i] = false;
	}
}

public void onRoundEnd ( Event ev, const char[] name, bool dontBroadcast ) {
/*	if ( g_hReceiveDollarTimer != null ) {
		KillTimer ( g_hReceiveDollarTimer );
		g_hReceiveDollarTimer = null;
	}	*/
}

public Action timerReceiveDollarTimer ( Handle timer ) {
	if ( ZR_IsGameEnd () ) {
		g_hReceiveDollarTimer = null;
		return Plugin_Stop;
	}
	
	g_iReceiveDollarTime --;
	if ( g_iReceiveDollarTime <= 0 ) {
		SetHudTextParamsEx ( -1.0, 0.645, 3.0, { 200, 165, 60, 255 }, { 255, 222, 150, 255 }, 2, 0.3, 0.05, 0.1 );
		int dollars;
		char dollar_format[16];
		for ( int i = 1; i <= MaxClients; i ++ ) {
			if ( loringlib_IsValidClient ( i ) && ZR_IsClientHuman ( i ) ) {
				dollars = GetRandomInt ( DOLLAR_RECEIVE_MIN, DOLLAR_RECEIVE_MAX );
				loringlib_NumberFormat ( dollars, dollar_format, sizeof ( dollar_format ) );
				ShowSyncHudText ( i, g_hHudSync[1], "$%s 를 받았습니다!", dollar_format );
				loringlib_AddEntityAccount ( i, dollars );
				EmitSoundToClient ( i, DOLLAR_RECEIVE_SOUND );
			}
		}
		
		g_hReceiveDollarTimer = null;
		return Plugin_Stop;
	}
	
	else {
		if ( g_iReceiveDollarTime == 30 ) {
		//	PrintToChatAll ( " \x04[Receive Dollars] \x0130초 후 달러를 받게됩니다." );
			SetHudTextParamsEx ( -1.0, 0.645, 5.0, { 200, 165, 60, 255 }, { 255, 222, 150, 255 }, 2, 0.3, 0.05, 0.1 );
			for ( int i = 1; i <= MaxClients; i ++ ) {
				if ( loringlib_IsValidClient ( i ) && ZR_IsClientHuman ( i ) ) {
					ShowSyncHudText ( i, g_hHudSync[1], "30초 후 달러 보급을 받게됩니다." );
					PrintToChat ( i, " \x04[Receive Dollars] \x0130초 후 달러 보급을 받게됩니다." );
				}
			}
		}
		
		switch ( g_iReceiveDollarTime ) {
			case 5, 4, 3, 2, 1: {
			//	PrintToChatAll ( " \x04[Receive Dollars] \x01%d초 후 달러를 받습니다.", g_iReceiveDollarTime );
				SetHudTextParamsEx ( -1.0, 0.645, 1.0, { 200, 165, 60, 255 }, { 255, 222, 150, 255 }, 0, 0.3, 0.5, 0.1 );
				for ( int i = 1; i <= MaxClients; i ++ ) {
					if ( loringlib_IsValidClient ( i ) && ZR_IsClientHuman ( i ) ) {
						ShowSyncHudText ( i, g_hHudSync[1], "%d초 후 달러 보급을 받습니다.", g_iReceiveDollarTime );
						PrintToChat ( i, " \x04[Receive Dollars] \x01%d초 후 달러 보급을 받습니다.", g_iReceiveDollarTime );
					}
				}
			}
		}
	}
	
	return Plugin_Continue;
}
#endif

public void onPlayerSpawn ( Event ev, const char[] name, bool dontBroadcast ) {
	int client = GetClientOfUserId ( ev.GetInt ( "userid" ) );
	if ( !loringlib_IsValidClient ( client ) )
		return;
	
	g_bHasZombieGrenade[client] = false;
	
	if ( isWinter () )
		CreateTimer ( 0.5, timerGiveSnowball, client );
		
#if	MOTHER_ZOMBIE_POINT_HUMANS
	CreateTimer ( 0.5, timerCreateHumanPoint, client );
#endif
}

#if MOTHER_ZOMBIE_POINT_HUMANS
public Action timerCreateHumanPoint ( Handle timer, int client ) {
	if ( !loringlib_IsValidClient__PlayGame ( client ) )
		return Plugin_Stop;
	
	if ( IsPlayerAlive ( client ) ) {
		if ( g_iHumanPointEnvSprite[client] != -1 ) {
			if ( IsValidEdict ( g_iHumanPointEnvSprite[client] ) ) {
				AcceptEntityInput ( g_iHumanPointEnvSprite[client], "Kill" );
				g_iHumanPointEnvSprite[client] = -1;
			}
		}
		
		if ( !ZR_IsClientHuman ( client ) )
			return Plugin_Stop;
		
/*		g_iHumanPointEnvSprite[client] = CreateEntityByName ( "env_sprite" );
		if ( g_iHumanPointEnvSprite[client] == -1 )
			return Plugin_Stop;
		
		DispatchKeyValue ( g_iHumanPointEnvSprite[client], "spawnflags", "1" );
		DispatchKeyValue ( g_iHumanPointEnvSprite[client], "scale", "0.05" );
		PrecacheModel ( "materials/particle/particle_flares/aircraft_red.vmt" );
		DispatchKeyValue ( g_iHumanPointEnvSprite[client], "model", "materials/particle/particle_flares/aircraft_red.vmt" );
		DispatchSpawn ( g_iHumanPointEnvSprite[client] ); */
	
		float pos[3];
		GetClientAbsOrigin ( client, pos );
		pos[2] += 37.0;
	//	g_iHumanPointEnvSprite[client] = loringlib_CreateParticle ( client, 0, pos, "zombie_wallhack", true, 999.0 );
		
		g_iHumanPointEnvSprite[client] = CreateEntityByName ( "info_particle_system" );
		if ( g_iHumanPointEnvSprite[client] == -1 )
			return Plugin_Stop;
		
		TeleportEntity ( g_iHumanPointEnvSprite[client], pos, NULL_VECTOR, NULL_VECTOR );
		
		DispatchKeyValue ( g_iHumanPointEnvSprite[client], "effect_name", "zombie_wallhack" );
		DispatchKeyValue ( g_iHumanPointEnvSprite[client], "start_active", "1" );
		
		loringlib_SetEntityOwner2 ( g_iHumanPointEnvSprite[client], client );
		SDKHook ( g_iHumanPointEnvSprite[client], SDKHook_SetTransmit, setTransmitOnHumanPoint );
		
		SetVariantString ( "!activator" );
		AcceptEntityInput ( g_iHumanPointEnvSprite[client], "SetParent", client, g_iHumanPointEnvSprite[client], 0 );
		SetEntPropEnt ( g_iHumanPointEnvSprite[client], Prop_Data, "m_pParent", g_iHumanPointEnvSprite[client] );
		
		DispatchSpawn ( g_iHumanPointEnvSprite[client] );
		ActivateEntity ( g_iHumanPointEnvSprite[client] );
		
		SetVariantString ( "OnUser1 !self:kill::999.0:-1" );
		AcceptEntityInput ( g_iHumanPointEnvSprite[client], "AddOutput" );
		AcceptEntityInput ( g_iHumanPointEnvSprite[client], "FireUser1" );
		
/*		TeleportEntity ( g_iHumanPointEnvSprite[client], pos, NULL_VECTOR, NULL_VECTOR );
		
		SetVariantString ( "!activator" );
		AcceptEntityInput ( g_iHumanPointEnvSprite[client], "SetParent", client );	*/
	}
	
	return Plugin_Stop;
}

public Action setTransmitOnHumanPoint ( int entity, int other ) {
	if ( GetEdictFlags ( entity ) & FL_EDICT_ALWAYS )
		SetEdictFlags ( entity, ( GetEdictFlags ( entity ) ^ FL_EDICT_ALWAYS ) );
	
	int owner = loringlib_GetEntityOwner2 ( entity );
	if ( ZR_IsClientHuman ( other ) || owner == other || ZR_IsClientNormalZombie ( other ) )
		return Plugin_Stop;
		
	return Plugin_Continue;
}
#endif

public Action timerGiveSnowball ( Handle timer, any client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Stop;
		
	if ( !ZR_IsClientHuman ( client ) )
		return Plugin_Stop;
	
	if ( !g_bSnowballMode[client] )
		return Plugin_Stop;
	
	int snowball = GivePlayerItem ( client, "weapon_snowball" );
	EquipPlayerWeapon ( client, snowball );
	return Plugin_Stop;
}

public void onWeaponFire ( Event ev, const char[] name, bool dontBroadcast ) {
	int client = GetClientOfUserId ( ev.GetInt ( "userid" ) );
	char weapon[32];
	
	if ( isWinter () ) {
		if ( !g_bSnowballMode[client] )
			return;
			
		if ( loringlib_IsValidClient ( client ) ) {
			if ( ZR_IsClientHuman ( client ) ) {
				ev.GetString ( "weapon", weapon, sizeof ( weapon ) );
				
				if ( StrContains ( weapon, "snowball", false ) != -1 ) {
					int snowball = GivePlayerItem ( client, "weapon_snowball" );
					EquipPlayerWeapon ( client, snowball );
				}
			}
		}
	}
}

public Action onTakeDamage ( int victim, int& attacker, int& inflictor, float& dmg, int& damagetype, int& weapon, float damageForce[3], float damagePosition[3] ) {
	if ( !loringlib_IsValidClient ( victim ) ||
		!loringlib_IsValidClient ( attacker ) )
		return Plugin_Continue;
	
	if ( GetClientTeam ( victim ) == GetClientTeam ( attacker ) )
		return Plugin_Continue;
	
	char name[32];
	if ( weapon != -1 ) {
		GetEdictClassname ( weapon, name, sizeof ( name ) );
		if ( StrContains ( name, "knife", false ) != -1 ) {
			if ( ZR_IsClientHuman ( attacker ) && !ZR_IsClientHuman ( victim ) ) {
				loringlib_AddEntityAccount ( victim, RoundToZero ( dmg / 2.5 ) );
				if ( loringlib_GetEntityAccount ( victim ) > FindConVar ( "mp_maxmoney" ).IntValue )
					loringlib_SetEntityAccount ( victim, FindConVar ( "mp_maxmoney" ).IntValue );
			}
		}
	}
	
	if ( ZR_IsClientHuman ( attacker ) && ZR_IsClientZombie ( victim ) ) {
		EmitSoundToClient ( attacker, HIT_SOUND, _, _, _, _, 0.4 );
	}
	
	if ( ZR_IsClientHuman ( attacker ) && !ZR_IsClientHuman ( victim ) ) {
		//	zombie
		loringlib_AddEntityAccount ( victim, RoundFloat ( dmg / 1.2 ) );
			
		if ( loringlib_GetEntityAccount ( victim ) > FindConVar ( "mp_maxmoney" ).IntValue )
			loringlib_SetEntityAccount ( victim, FindConVar ( "mp_maxmoney" ).IntValue );

#if	DOLLAR_TYPE == 1
		//	human
		if ( StrContains ( name, "snowball", false ) != -1 )
			loringlib_AddEntityAccount ( attacker, RoundToZero ( dmg * GetRandomFloat ( 20.0, 30.0 ) ) );
		else
			loringlib_AddEntityAccount ( attacker, RoundToZero ( dmg / 1.3 ) );
			
		if ( loringlib_GetEntityAccount ( attacker ) > FindConVar ( "mp_maxmoney" ).IntValue )
			loringlib_SetEntityAccount ( attacker, FindConVar ( "mp_maxmoney" ).IntValue );
#endif
	}
	
	return Plugin_Continue;
}

public Action onClientCommandDHide ( int client, int args ) {
	g_bDisplay[client] = !g_bDisplay[client];
	PrintToChat ( client, "[ZR] 달러 표시가 %s 되었습니다.", g_bDisplay[client] ? "활성화" : "비활성화" );
	return Plugin_Stop;
}

public Action onClientCommandShop ( int client, int args ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Handled;
	
	if ( !IsPlayerAlive ( client ) )
		return Plugin_Handled;
	
	if ( ZR_IsClientNemesis ( client ) )
		return Plugin_Handled;
	
	Menu menu = new Menu ( displayShopMenuHandler );
	menu.SetTitle ( "ZMarket: Item Shop\n\tType %s\n　", ZR_IsClientHuman ( client ) ? "Human" : "Zombie" );
	if ( ZR_IsClientHuman ( client ) ) {
		menu.AddItem ( "", "투척 무기 구입" );
		menu.AddItem ( "", "탄창 채우기" );
		menu.AddItem ( "", "일회용 아이템 사용", ITEMDRAW_DISABLED );
	}
	
	else {
		menu.AddItem ( "", "체력 구입" );
		menu.AddItem ( "", "일회용 아이템 사용" );
	}
	
	menu.Display ( client, MENU_TIME_FOREVER );
	return Plugin_Handled;
}

public int displayShopMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	if ( !loringlib_IsValidClient ( param1 ) )
		delete menu;
	
	switch ( action ) {
		case MenuAction_Select: {
			if ( !IsPlayerAlive ( param1 ) )
				return;
		
			if ( ZR_IsClientHuman ( param1 ) ) {
				switch ( param2 ) {
					case 0: buyGrenadeItems ( param1 );
					case 1: buyAmmos ( param1 );
				}
			}
			
			else {
				switch ( param2 ) {
					case 0: buyZombieHealth ( param1 );
					case 1: buyItems ( param1 );
				}
			}
		}
	}
}

/**
 * 투척 무기 메뉴
 */
public void buyGrenadeItems ( int client ) {
	static char format[256];
	Menu menu = new Menu ( buyGrenadeItemMenuHandler );
	menu.SetTitle ( "ZMarket: Item Shop\n\t투척 무기 구입\n　" );
	for ( int i = 0; i < MAX_GRENADES; i ++ ) {
		Format ( format, sizeof ( format ), "%s [$%s]", g_strGrenadeList[i][1], g_strGrenadeList[i][2] );
		menu.AddItem ( "", format, StringToInt ( g_strGrenadeList[i][2] ) <= loringlib_GetEntityAccount ( client ) ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED );
	}
	
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int buyGrenadeItemMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			if ( StringToInt ( g_strGrenadeList[param2][2] ) > loringlib_GetEntityAccount ( param1 ) ) {
				NEED_MORE_ACCOUNT ( param1 )
				buyGrenadeItems ( param1 );
				return;
			}
			
			GivePlayerItem ( param1, g_strGrenadeList[param2][0] );
			loringlib_SubEntityAccount ( param1, StringToInt ( g_strGrenadeList[param2][2] ) );
			PrintToChat ( param1, "[ZR] %s 을(를) 구입했습니다. \x07[$%d]", g_strGrenadeList[param2][1], loringlib_GetEntityAccount ( param1 ) );
			buyGrenadeItems ( param1 );
		}
		
		case MenuAction_Cancel: {
			switch ( param2 ) {
				case MenuCancel_ExitBack: {
					onClientCommandShop ( param1, 0 );
					return;
				}
			}
		}
	}
}

/**
 * 탄창 구입
 */
public void buyAmmos ( int client ) {
	Menu menu = new Menu ( buyAmmoMenuHandler );
	menu.SetTitle ( "ZMarket: Item Shop\n\t탄창 채우기\n　\n\t\t들고 있는 무기의 탄창을 채웁니다.\n　" );
	menu.AddItem ( "5000", "탄창 채우기 [$5000]", loringlib_GetEntityAccount ( client ) < 5000 ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT );
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int buyAmmoMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			if ( ZR_IsClientZombie ( param1 ) )
				return;
				
			if ( loringlib_HasWeaponGrenade ( param1 ) || loringlib_HasWeaponName ( param1, "weapon_knife" ) ) {
				PrintToChat ( param1, "[ZR] 주 무기 및 보조 무기만 사용할 수 있습니다." );
				return;
			}
		
			int ammo = loringlib_GetWeaponAmmo ( param1 );
			if ( ammo < 0 ) {
				PrintToChat ( param1, "[ZR] ERROR: INVALID WEAPON AMMO AMOUNT INDEX: %d", ammo );
				return;
			}	
			
			int active = loringlib_GetActiveWeapon ( param1 );
			if ( active < 0 ) {
				PrintToChat ( param1, "[ZR] ERROR: INVALID WEAPON INDEX: %d", active );
				return;
			}
			
			if ( ammo >= LOLAmmoFix_GetWeaponMaxAmmo ( active ) ) {
				PrintToChat ( param1, "[ZR] 그정도 탄창이면 충분합니다." );
				return;
			}
			
			char price_str[16];
			menu.GetItem ( param2, price_str, sizeof ( price_str ) );
			if ( StringToInt ( price_str ) > loringlib_GetEntityAccount ( param1 ) ) {
				NEED_MORE_ACCOUNT ( param1 )
				buyAmmos ( param1 );
				return;
			}
			
			loringlib_SubEntityAccount ( param1, StringToInt ( price_str ) );

			PrintToChat ( param1, "[ZR] 탄창을 채웠습니다." );
		//	loringlib_SetWeaponAmmo ( param1, ammo + charge_ammo );
			loringlib_SetWeaponAmmoIndex ( param1, active, LOLAmmoFix_GetWeaponMaxAmmo ( active ), _, _, _ );
			return;
		}
	}
}

/**
 * 좀비 체력 메뉴
 */
public void buyZombieHealth ( int client ) {
	Menu menu = new Menu ( buyZombieHealthMenuHandler );
	menu.SetTitle ( "ZMarket: Item Shop\n\t체력 구입\n　" );
	static char format[256];
	for ( int i = 0; i < MAX_ZOMBIE_HEALTHS; i ++ ) {
		Format ( format, sizeof ( format ), "+%s HP [$%s]", g_strZombieHealthList[i][0], g_strZombieHealthList[i][1] );
		menu.AddItem ( "", format, StringToInt ( g_strZombieHealthList[i][1] ) <= loringlib_GetEntityAccount ( client ) ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED );
	}
	
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int buyZombieHealthMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	if ( !loringlib_IsValidClient ( param1 ) )
		delete menu;
	
	switch ( action ) {
		case MenuAction_Select: {
			if ( ZR_IsClientHuman ( param1 ) )
				return;
		
			if ( StringToInt ( g_strZombieHealthList[param2][1] ) > loringlib_GetEntityAccount ( param1 ) ) {
				NEED_MORE_ACCOUNT ( param1 )
				buyZombieHealth ( param1 );
				return;
			}
			
						
			loringlib_SubEntityAccount ( param1, StringToInt ( g_strZombieHealthList[param2][1] ) );
			PrintToChat ( param1, "[ZR] +%s HP 을(를) 구입했습니다. \x07[$%d]", g_strZombieHealthList[param2][0], loringlib_GetEntityAccount ( param1 ) );
			
			if ( ( loringlib_GetEntityHealth ( param1 ) + StringToInt ( g_strZombieHealthList[param2][0] ) ) >= loringlib_GetEntityMaxHealth ( param1 ) ) {
				PrintToChat ( param1, "[ZR] 최대 체력이 \x07%d\x01 에서 \x07%d\x01 (으)로 증가했습니다.", loringlib_GetEntityMaxHealth ( param1 ), loringlib_GetEntityMaxHealth ( param1 ) + StringToInt ( g_strZombieHealthList[param2][0] ) );
				loringlib_SetEntityMaxHealth ( param1, loringlib_GetEntityMaxHealth ( param1 ) + StringToInt ( g_strZombieHealthList[param2][0] ) );
				loringlib_SetEntityHealth ( param1, loringlib_GetEntityMaxHealth ( param1 ) );
				buyZombieHealth ( param1 );
			}
			else {
				PrintToChat ( param1, "[ZR] 체력이 \x07%d\x01 에서 \x07%d\x01 (으)로 증가했습니다.", loringlib_GetEntityHealth ( param1 ), loringlib_GetEntityHealth ( param1 ) + StringToInt ( g_strZombieHealthList[param2][0] ) );
				loringlib_SetEntityHealth ( param1, loringlib_GetEntityHealth ( param1 ) + StringToInt ( g_strZombieHealthList[param2][0] ) );
				buyZombieHealth ( param1 );
			}

		}
		
		case MenuAction_Cancel: {
			switch ( param2 ) {
				case MenuCancel_ExitBack: {
					onClientCommandShop ( param1, 0 );
					return;
				}
			}
		}
	}
}

/**
 * 아이템 메뉴
 */
public void buyItems ( int client ) {
	Menu menu = new Menu ( buyItemMenuHandler );
	menu.SetTitle ( "ZMarket: Item Shop\n\t일회용 아이템 구입\n　" );
	if ( ZR_IsClientZombie ( client ) ) {
		menu.AddItem ( "2000", "+2000 AP [$2,000]", loringlib_GetEntityAccount ( client ) >= 2000 ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED );
		menu.AddItem ( "5000", "+5000 AP [$5,000]", loringlib_GetEntityAccount ( client ) >= 5000 ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED );
		menu.AddItem ( "3000", "넉백 감소 (소) [$3,000]", loringlib_GetEntityAccount ( client ) >= 3000 ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED );
		menu.AddItem ( "5000", "넉백 감소 (중) [$5,000]", loringlib_GetEntityAccount ( client ) >= 5000 ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED );
		menu.AddItem ( "7000", "넉백 감소 (대) [$7,000]", loringlib_GetEntityAccount ( client ) >= 7000 ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED );
		if ( g_bZombieGrenadeCooldown[client] )
			menu.AddItem ( "0", "좀비 수류탄 [쿨 다운]", ITEMDRAW_DISABLED );
		else
			menu.AddItem ( "0", g_bHasZombieGrenade[client] ? "좀비 수류탄 [소지 중]" : "좀비 수류탄", g_bHasZombieGrenade[client] ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT );
	}
	else {
		menu.AddItem ( "", "비어있음", ITEMDRAW_DISABLED );
	}
	
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int buyItemMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	if ( !loringlib_IsValidClient ( param1 ) )
		delete menu;
		
	switch ( action ) {
		case MenuAction_Select: {
			if ( ZR_IsClientZombie ( param1 ) ) {
				static char price[16];
				menu.GetItem ( param2, price, sizeof ( price ) );
				int price_money = StringToInt ( price );
				
				if ( price_money > 0 ) {
					if ( loringlib_GetEntityAccount ( param1 ) < price_money ) {
						NEED_MORE_ACCOUNT ( param1 )
						buyItems ( param1 );
						return;
					}
				
					loringlib_SubEntityAccount ( param1, price_money );
				}
			
				switch ( param2 ) {
					case 0: {
						loringlib_SetEntityArmor ( param1, loringlib_GetEntityArmor ( param1 ) + 2000 );
						PrintToChat ( param1, "[ZR] AP가 2000만큼 증가했습니다. \x07[$%d]", loringlib_GetEntityAccount ( param1 ) );
					}
					
					case 1: {
						loringlib_SetEntityArmor ( param1, loringlib_GetEntityArmor ( param1 ) + 5000 );
						PrintToChat ( param1, "[ZR] AP가 5000만큼 증가했습니다. \x07[$%d]", loringlib_GetEntityAccount ( param1 ) );
					}
					
					case 2: {
						ZR_SetClientKnockbackScale ( param1, ZR_GetClientZombieType ( param1 ), ZR_GetClientKnockbackScale ( param1, ZR_GetClientZombieType ( param1 ) ) + 0.7 );
						PrintToChat ( param1, "[ZR] 넉백이 감소되었습니다. \x07[$%d]", loringlib_GetEntityAccount ( param1 ) );
					}
					
					case 3: {
						ZR_SetClientKnockbackScale ( param1, ZR_GetClientZombieType ( param1 ), ZR_GetClientKnockbackScale ( param1, ZR_GetClientZombieType ( param1 ) ) + 1.2 );
						PrintToChat ( param1, "[ZR] 넉백이 감소되었습니다. \x07[$%d]", loringlib_GetEntityAccount ( param1 ) );
					}
					
					case 4: {
						ZR_SetClientKnockbackScale ( param1, ZR_GetClientZombieType ( param1 ), ZR_GetClientKnockbackScale ( param1, ZR_GetClientZombieType ( param1 ) ) + 1.65 );
						PrintToChat ( param1, "[ZR] 넉백이 감소되었습니다. \x07[$%d]", loringlib_GetEntityAccount ( param1 ) );
					}
					
					case 5: {
						g_bZombieGrenadeCooldown[param1] = true;
						CreateTimer ( ZOMBIE_GRENADE_COOLDOWN, timerZombieGrenadeCooldown, param1, TIMER_FLAG_NO_MAPCHANGE );
						g_bHasZombieGrenade[param1] = true;
						PrintToChat ( param1, "[ZR] 좀비 수류탄을 구입했습니다." );
						PrintToChat ( param1, "[ZR] \x09찍기 공격\x01 으로 좀비 수류탄을 날릴 수 있습니다." );
					}
				}
			}
		}
		
		case MenuAction_Cancel: {
			switch ( param2 ) {
				case MenuCancel_ExitBack: {
					onClientCommandShop ( param1, 0 );
					return;
				}
			}
		}
	}
}

public Action timerZombieGrenadeCooldown ( Handle timer, any client ) {
	if ( loringlib_IsValidClient ( client ) ) {
		if ( g_bZombieGrenadeCooldown[client] ) {
			PrintToChat ( client, "[ZR] 좀비 수류탄을 구입할 수 있습니다." );
			g_bZombieGrenadeCooldown[client] = false;
		}
	}
	
	return Plugin_Stop;
}


public void getClientHudStyle ( QueryCookie cookie, int client, ConVarQueryResult result, const char[] name, const char[] value ) {
	if ( StringToInt ( value ) > 1 )
		g_iClientHudStyle[client] = 0;
	else
		g_iClientHudStyle[client] = StringToInt ( value );
}

public void getClientHudColor ( QueryCookie cookie, int client, ConVarQueryResult result, const char[] name, const char[] value ) {
	if ( StringToInt ( value ) < 0 || StringToInt ( value ) > 11 )
		g_iClientHudColor[client] = 0;
	else
		g_iClientHudColor[client] = StringToInt ( value );
}

public Action timerDisplayRepeat ( Handle timer, int client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Stop;
	
	if ( !IsPlayerAlive ( client ) )
		return Plugin_Continue;
	
	if ( !g_bDisplay[client] )
		return Plugin_Continue;
	
	QueryClientConVar ( client, "cl_hud_color", getClientHudColor );
	QueryClientConVar ( client, "cl_hud_healthammo_style", getClientHudStyle );
	
	int color[4];
	color[0] = g_iHudColors[g_iClientHudColor[client]][0];
	color[1] = g_iHudColors[g_iClientHudColor[client]][1];
	color[2] = g_iHudColors[g_iClientHudColor[client]][2];
	color[3] = g_iHudColors[g_iClientHudColor[client]][3];

	if ( g_iClientHudStyle[client] == 1 )
		SetHudTextParams ( 0.33, 0.96, 0.5, color[0], color[1], color[2], color[3], 0, 0.01, 0.01, 0.05 );
	else
		SetHudTextParams ( 0.26, 0.96, 0.5, color[0], color[1], color[2], color[3], 0, 0.01, 0.01, 0.05 );
	
	static char ap[32];
	static char acc[32];
	loringlib_NumberFormat ( loringlib_GetEntityArmor ( client ), ap, sizeof ( ap ) );
	loringlib_NumberFormat ( loringlib_GetEntityAccount ( client ), acc, sizeof ( acc ) );
	
	if ( g_iClientHudStyle[client] == 1 ) {
		SetHudTextParams ( 0.26, 0.96, 0.5, color[0], color[1], color[2], color[3], 0, 0.01, 0.01, 0.05 );
		ShowSyncHudText ( client, g_hHudSync[0], "AP %s　　$%s", ap, acc );
	} else {
		SetHudTextParams ( 0.34, 0.936, 0.5, color[0], color[1], color[2], color[3], 0, 0.01, 0.01, 0.05 );
		ShowSyncHudText ( client, g_hHudSync[0], "AP %s\n$%s", ap, acc );
	}
	
	return Plugin_Continue;
}

public Action onClientCommandSnowball ( int client, int args ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Continue;
	
	if ( !isWinter () ) {
		g_bSnowballMode[client] = false;
		PrintToChat ( client, " \x04[Snowball Mode]\x01 이런, 더 이상 눈덩이를 만들 수 없어요. 차갑고 아름다운 겨울은 가버렸으니깐요." );
		return Plugin_Handled;
	}
	
	g_bSnowballMode[client] = !g_bSnowballMode[client];
	PrintToChat ( client, " \x04[Snowball Mode]\x01 눈덩이 모드가 %s되었습니다.", g_bSnowballMode[client] ? "활성화" : "비활성화" );
	if ( g_bSnowballMode[client] )
		PrintToChat ( client, " \x04[Snowball Mode]\x01 다음 라운드 스폰 부터 눈덩이를 받게 됩니다." );
		
	return Plugin_Handled;
}

public Action OnPlayerRunCmd ( int client, int &key ) {
	if ( loringlib_IsValidClient ( client ) ) {
		throwZombieGrenade ( client, key );
	}
}

void throwZombieGrenade ( int client, int keys ) {
	if ( !ZR_IsClientHuman ( client ) ) {
		if ( g_bHasZombieGrenade[client] ) {
			if ( keys & IN_ATTACK2 ) {
				if ( !IsPlayerAlive ( client ) )
					return;
				
				g_bHasZombieGrenade[client] = false;
				
				int throwEntity = CreateEntityByName ( "decoy_projectile" );
				if ( throwEntity != -1 ) {
				//	int randomModel = GetRandomInt ( 1, sizeof ( g_strZombieGrenadeModel ) ) - 1;
				//	DispatchKeyValue ( throwEntity, "model", g_strZombieGrenadeModel[randomModel] );
					SDKHook ( throwEntity, SDKHook_SpawnPost, spawnPostOnZombieGrenade );
					
					loringlib_SetEntityOwner2 ( throwEntity, client );
					SetEntProp ( throwEntity, Prop_Data, "m_takedamage", 0 );
					
					DispatchSpawn ( throwEntity );
					ActivateEntity ( throwEntity );
					
					float ang[3], pos[3], vec[3], result[3];
					
					GetClientEyeAngles ( client, ang );
					GetClientEyePosition ( client, pos );
					GetAngleVectors ( ang, vec, NULL_VECTOR, NULL_VECTOR );
					NormalizeVector ( vec, vec );
					AddVectors ( pos, vec, result );
					NormalizeVector ( vec, vec );
					
					ScaleVector ( vec, ZOMBIE_GRENADE_THROW_SPEED );
					SetEntityMoveType ( throwEntity, MOVETYPE_FLYGRAVITY );
					TeleportEntity ( throwEntity, result, ang, vec );
					
					TE_SetupBeamFollow ( throwEntity, PrecacheModel ( "materials/sprites/laserbeam.vmt" ), 0, 1.4, 5.0, 5.0, 1, { 165, 102, 255, 32 } );
					TE_SendToAll ();
					
					float throwOrigin[3];
					loringlib_GetEntityOriginEx ( throwEntity, throwOrigin );
					loringlib_CreateParticle ( throwEntity, 0, throwOrigin, "zombie_bomb_trail", true, ZOMBIE_GRENADE_LIFETIME );
					
					CreateTimer ( ZOMBIE_GRENADE_LIFETIME, zombieGrenadeExplode, throwEntity, TIMER_FLAG_NO_MAPCHANGE );
					
					SDKHookEx ( throwEntity, SDKHook_Touch, zombieGrenadeTouchToKillVel );
				}
			}
		}
	}
}

public void zombieGrenadeTouchToKillVel ( int entity, int other ) {
	if ( IsValidEdict ( entity ) ) {
		float angles[3];
		GetEntPropVector ( entity, Prop_Data, "m_angRotation", angles );
		float origin[3];
		loringlib_GetEntityOriginEx ( entity, origin );
		TeleportEntity ( entity, origin, angles, NULL_VECTOR_FLY_BLOCK );
		SetEntityMoveType ( entity, MOVETYPE_NONE );
	}
}

public void spawnPostOnZombieGrenade ( int entity ) {
	if ( IsValidEdict ( entity ) ) {
		int random = GetRandomInt ( 1, sizeof ( g_strZombieGrenadeModel ) ) - 1;
		SetEntityModel ( entity, g_strZombieGrenadeModel[random] );
	}
}

public Action zombieGrenadeExplode ( Handle timer, any entity ) {
	if ( !IsValidEdict ( entity ) )
		return;
	
	float touchOrigin[3];
	loringlib_GetEntityOriginEx ( entity, touchOrigin );
	touchOrigin[2] += 5.0;
	
	float targetOrigin[3];
	int owner = loringlib_GetEntityOwner2 ( entity );
	
	float vectors[3], vectorVelocity[3];
	float targetEyePos[3];
	
	float punchAngles[3];
	punchAngles[0] = GetRandomFloat ( -89.0, 89.0 );
	punchAngles[1] = GetRandomFloat ( -89.0, 89.0 );
	punchAngles[2] = 0.0;
	float punchVelocity[3] = { 180.0, 180.0, 180.0 };
	for ( int target = 1; target <= MaxClients; target ++ ) {
		if ( loringlib_IsValidClient__PlayGame ( target ) ) {
			loringlib_GetEntityOriginEx ( target, targetOrigin );
			if ( GetVectorDistance ( touchOrigin, targetOrigin ) <= ZOMBIE_GRENADE_TARGET_DAMAGE_DISTANCE ) {
				loringlib_ShowAimPunch ( target, punchAngles );
				loringlib_SetAimPunchVel ( target, punchVelocity );
				
				GetClientEyePosition ( target, targetEyePos );
				MakeVectorFromPoints ( touchOrigin, targetEyePos, vectors );
				NormalizeVector ( vectors, vectors );
				
				if ( GetEntityFlags ( target ) & FL_DUCKING )
					ScaleVector ( vectors, ZOMBIE_GRENADE_KNOCKBACK_SCALE_DUCK );
				else
					ScaleVector ( vectors, ZOMBIE_GRENADE_KNOCKBACK_SCALE );
				
				GetEntPropVector ( target, Prop_Data, "m_vecVelocity", vectorVelocity );
				AddVectors ( vectorVelocity, vectors, vectors );
				vectors[2] += GetRandomFloat ( 25.0, 35.0 );
				TeleportEntity ( target, NULL_VECTOR, NULL_VECTOR, vectors );
				
				if ( loringlib_IsValidClient ( owner ) ) {
					if ( ZR_IsClientHuman ( target ) ) {
						loringlib_MakeDamage ( owner, target, GetRandomInt ( 1, 3 ), "weapon_axe", DMG_SHOCK );
					}
				}
			}
		}
	}
	
	float edictOrigin[3];
	char edictName[64];
	for ( int props = MaxClients + 1; props <= SOURCE_MAXENTITIES; props ++ ) {
		if ( IsValidEdict ( props ) && IsValidEntity ( props ) ) {
			GetEdictClassname ( props, edictName, sizeof ( edictName ) );
			if ( StrContains ( edictName, "prop_", false ) != -1 ) {
				if ( GetEntProp ( props, Prop_Data, "m_takedamage" ) == 2 && ( HasEntProp ( props, Prop_Send, "m_iHealth" ) || HasEntProp ( props, Prop_Data, "m_iHealth" ) ) ) {
					loringlib_GetEntityOriginEx ( props, edictOrigin );
					if ( GetVectorDistance ( touchOrigin, edictOrigin ) <= ZOMBIE_GRENADE_PROP_DAMAGE_DISTANCE ) {
						if ( loringlib_IsValidClient ( owner ) )
							loringlib_MakeDamage ( owner, props, GetRandomInt ( ZOMBIE_GRENADE_PROP_DAMAGE_MIN, ZOMBIE_GRENADE_PROP_DAMAGE_MAX ), "weapon_knife", DMG_SLASH );
						else
							loringlib_SetEntityHealth ( props, loringlib_GetEntityHealth ( props ) - GetRandomInt ( ZOMBIE_GRENADE_PROP_DAMAGE_MIN, ZOMBIE_GRENADE_PROP_DAMAGE_MAX ) );
					}
				}
			}
		}
	}
	
	EmitAmbientSound ( ZOMBIE_GRENADE_SOUND_EFFECT, touchOrigin );
//	TE_SetupExplosion ( touchOrigin, PrecacheModel ( "materials/editor/env_explosion.vmt" ), 50.0, 1, TE_EXPLFLAG_NONE, 150, 50 );
//	TE_SendToAll ();
	loringlib_CreateParticle ( entity, 0, touchOrigin, "zombie_bomb_explode" );
	
	AcceptEntityInput ( entity, "Kill" );
}

stock bool isWinter () {
	char date[32];
	FormatTime ( date, sizeof ( date ), "%m", GetTime () );
	if ( StrEqual ( date, "12" ) || StrEqual ( date, "01" ) )
		return true;
	
	return false;
}