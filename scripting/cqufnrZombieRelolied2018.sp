#include <loringlib>
#include <emitsoundany>
#include <fpvm_interface>

/**
	
  ▄███████▄   ▄██████▄     ▄▄▄▄███▄▄▄▄   ▀█████████▄   ▄█     ▄████████         ▄████████    ▄████████  ▄█        ▄██████▄   ▄█        ▄█     ▄████████ ████████▄  
 ██▀     ▄██ ███    ███  ▄██▀▀▀███▀▀▀██▄   ███    ███ ███    ███    ███        ███    ███   ███    ███ ███       ███    ███ ███       ███    ███    ███ ███   ▀███ 
       ▄███▀ ███    ███  ███   ███   ███   ███    ███ ███▌   ███    █▀         ███    ███   ███    █▀  ███       ███    ███ ███       ███▌   ███    █▀  ███    ███ 
  ▀█▀▄███▀▄▄ ███    ███  ███   ███   ███  ▄███▄▄▄██▀  ███▌  ▄███▄▄▄           ▄███▄▄▄▄██▀  ▄███▄▄▄     ███       ███    ███ ███       ███▌  ▄███▄▄▄     ███    ███ 
   ▄███▀   ▀ ███    ███  ███   ███   ███ ▀▀███▀▀▀██▄  ███▌ ▀▀███▀▀▀          ▀▀███▀▀▀▀▀   ▀▀███▀▀▀     ███       ███    ███ ███       ███▌ ▀▀███▀▀▀     ███    ███ 
 ▄███▀       ███    ███  ███   ███   ███   ███    ██▄ ███    ███    █▄       ▀███████████   ███    █▄  ███       ███    ███ ███       ███    ███    █▄  ███    ███ 
 ███▄     ▄█ ███    ███  ███   ███   ███   ███    ███ ███    ███    ███        ███    ███   ███    ███ ███▌    ▄ ███    ███ ███▌    ▄ ███    ███    ███ ███   ▄███ 
  ▀████████▀  ▀██████▀    ▀█   ███   █▀  ▄█████████▀  █▀     ██████████        ███    ███   ██████████ █████▄▄██  ▀██████▀  █████▄▄██ █▀     ██████████ ████████▀  
                                                                               ███    ███              ▀                    ▀                                      
 **/

#define PLUGIN_VERSION	"1.5"

public Plugin myinfo = {
	name = "Zombie Relolied 2018 Version",
	author = "qufnr",
	description = "CS:GO basic Zombie Mod addon",
	version = PLUGIN_VERSION,
	url = "https://steamcommunity.com/id/qufnrp"
}

#define	ZR_ENABLE_TEAM_SELECT_ACTION	false	/**< 좀비 리로리드 만의 팀 선택 엑션을 사용합니다. */

#define ZR_GAMEMODE_ZOMBIE_MOD		1
#define	ZR_GAMEMODE_ZOMBIE_ESCAPE	2
#define ZR_GAMEMODE_NULL			3

#define ZR_INFECT_INFO_CHANNEL	1
#define	ZR_ZTELE_INFO_CHANNEL	2

#define ZR_NORMAL_ZOMBIE_CLASS	0
#define ZR_MOTHER_ZOMBIE_CLASS	1

Handle g_hInfectStartCooldown = null;	/**< Infect start timer handler. */
Handle g_hZombieRespawnCooldown[MAXPLAYERS + 1] = { null, ... };	/**< Zombie respawn cooldown handler. */
Handle g_hZteleCooldown[MAXPLAYERS + 1] = { null, ... };			/**< Ztele cooldown handler. */

int g_iInfectCountdown;		/**< Infect countdown integer. */

bool g_bGameStart = false;		/**< Game start check. */
bool g_bGameEnd = false;		/**< Game end check. */
bool g_bGameWaitPlayer = false;	/**< Waiting for player check. */
bool g_bNemesisRound = false;	/**< Game nemesis round check. */

bool g_bClientGamePlayer[MAXPLAYERS + 1] = { false, ... };	/**< Check client in game player. */
int g_iZombieType[MAXPLAYERS + 1] = { -1, ... };			/**< Check now client zombie type. */
bool g_bSafeHuman[MAXPLAYERS + 1] = { false, ... };		/**< Check infect choose safe. */
bool g_bZombie[MAXPLAYERS + 1] = { false, ... };		/**< Is client zombie? */
bool g_bMotherZombie[MAXPLAYERS + 1] = { false, ... };	/**< Is client mother zombie? */
bool g_bNemesis[MAXPLAYERS + 1] = { false, ... };		/**< Is client nemesis? */
int g_iNemesisEffect = -1;		/**< Nemesis dynamic light entity index. */

float g_fSpawnPosition[MAXPLAYERS + 1][3];		/**< Client spawn vectors. */
float g_fZombieRespawnTime[MAXPLAYERS + 1] = { 0.0, ... };	/**< Client zombie respawn time. */
float g_fHumanRespawnTime[MAXPLAYERS + 1] = { 0.0, ... };	/**< Client human respawn time. */

int g_iClientZombieClass[MAXPLAYERS + 1] = { 0, ... };				/**< Player is default zombie class. */
int g_iZombieHealth[2][MAXPLAYERS + 1];				/**< Player zombie hp value. */
float g_fZombieSpeed[2][MAXPLAYERS + 1];				/**< Player zombie walk speed value. */
float g_fZombieGravity[2][MAXPLAYERS + 1];			/**< Player zombie gravity. */
float g_fZombieKnockbackScale[2][MAXPLAYERS + 1];	/**< Player zombie knockback scale. */
char g_strZombieModel[2][MAXPLAYERS + 1][256];						/**< Player zombie model path. */
char g_strZombieArmsModel[MAXPLAYERS + 1][256];						/**< Player zombie arms model path. */

int g_iTeleport[MAXPLAYERS + 1];			/**< 텔레포트 중 인지 채크하는 변수 0이면 Not teleporting, 2이면 좀비 상태에서 텔레포트, 3이면 인간 상태에서 텔레포트 */
int g_iZteleCounts[MAXPLAYERS + 1][4];		/**< Ztele 갯수를 담을 변수 */
float g_fZteleCooldown[MAXPLAYERS + 1][4];	/**< Ztele 쿨 다운을 담을 변수 */

#define	ZR_ZMARKET_PRIMARY_WEAPON	0
#define ZR_ZMARKET_SECONDARY_WEAPON	1
bool g_bBuyWeapon[MAXPLAYERS + 1][2];	/**< 무기를 구입했는지 채크하는 변수 */
bool g_bSaveMode[MAXPLAYERS + 1] = { false, ... };		/**< Client zmarket save mode. */
int g_iAutobuyMode[MAXPLAYERS + 1] = { 0, ... };	/**< Client zmarket auto buy mode. */

#define	ZR_ZMARKET_WEAPON_NAME		0
#define ZR_ZMARKET_WEAPON_ALIAS		1
char g_strPrimaryWeaponData[MAXPLAYERS + 1][2][64];		/**< 저장된 주 무기 데이터 값 0은 무기 명, 1은 무기 별명  */
char g_strSecondaryWeaponData[MAXPLAYERS + 1][2][64];		/**< 저장된 보조 무기 데이터 값 0은 무기 명, 1은 무기 별명 */

#define ZR_ZMARKET_WEAPON_PRICE		0
#define ZR_ZMARKET_WEAPON_TEAMIDX	1
int g_iPrimaryWeaponData[MAXPLAYERS + 1][2];		/**< 저장된 주 무기 데이터 값 0은 무기 가격, 1은 무기 팀 값 */
int g_iSecondaryWeaponData[MAXPLAYERS + 1][2];		/**< 저장된 보조 무기 데이터 값 0은 무기 가격, 1은 무기 팀 값 */

#define	ZR_ZMARKET_DEFAULT_WEAPON_NAME	"[EMPTY]"
 
char g_strWeaponDatabase[PLATFORM_MAX_PATH];	

int g_iTotalWeapons[6] = { 0, ... };	/**< Total weapon datas.  */

/**
 * @note CS:GO에서 재대로 된 스코어보드를 표시하기 위해 아래 변수를 사용 합니다.
 */
#define CLIENT_ACTION_KILL		0
#define CLIENT_ACTION_DEATH		1
#define CLIENT_ACTION_SCORE		2
int g_iClientActions[3][MAXPLAYERS + 1];
int g_iHumanWinCount = 0;
int g_iZombieWinCount = 0;

bool g_bIsWhitelistLevel = false;		/**< 화이트 리스트 맵 채크 변수 */

//	Forward handlers
#define ZR_MAX_FORWARDS		13
Handle g_hForwardHandlers[ZR_MAX_FORWARDS];

//	Forward handler types
#define ZR_FORWARD_ON_CHANGE_ZOMBIE			0
#define ZR_FORWARD_ON_CHANGE_MOTHER_ZOMBIE	1
#define ZR_FORWARD_ON_CHANGE_HUMAN			2
#define ZR_FORWARD_ON_ZOMBIE_DEFINE_TICKS	3
#define ZR_FORWARD_ON_ZOMBIE_INFECT_STARTED	4
#define ZR_FORWARD_ON_CHOOSE_NEMESIS		5
#define ZR_FORWARD_ON_CHANGE_NEMESIS		6
#define ZR_FORWARD_ON_CLIENT_ZTELE_STARTED		7
#define ZR_FORWARD_ON_CLIENT_ZTELE_TELEPORTED	8
#define ZR_FORWARD_ON_LIGHT_GRENADE_EXPLODE		9
#define ZR_FORWARD_ON_FREEZE_GRENADE_EXPLODE	10
#define ZR_FORWARD_ON_THROW_LIGHT_GRENADE		11
#define ZR_FORWARD_ON_THROW_FREEZE_GRENADE		12

//	Weapon numbers
#define ZR_WEAPON_HG	0
#define ZR_WEAPON_SG	1
#define ZR_WEAPON_SMG	2
#define ZR_WEAPON_AR	3
#define ZR_WEAPON_SR	4
#define ZR_WEAPON_MG	5

//	Weapon datas
#define ZR_DATA_WEAPON_NAME		0
#define ZR_DATA_WEAPON_ALIAS	1
#define ZR_DATA_WEAPON_PRICE	2
#define ZR_DATA_WEAPON_TEAM		3
//	[6]: 무기 타입, [64]: 무기 갯수, [4]: 무기 내장 데이터, [64]: 문자열 길이
char g_strWeaponData[6][64][4][64];	/**< 무기 데이터를 받을 4차원 배열 */
#define ZR_ZMARKET_WEAPON_DATA_LENGTH	63

//	Config file loader.
#include "zombierelolied/config.inc"

#include "zombierelolied/zombie/respawn.inc"
#include "zombierelolied/zombie/zombie_infect.inc"
#include "zombierelolied/zombie/class.inc"
#include "zombierelolied/zombie/health_recharge.inc"
#include "zombierelolied/human/respawn.inc"

#include "zombierelolied/commands.inc"
#include "zombierelolied/event.inc"
#include "zombierelolied/round.inc"
#include "zombierelolied/sound.inc"
#include "zombierelolied/client.inc"
#include "zombierelolied/client_keyvalue.inc"
#include "zombierelolied/nemesis.inc"
#include "zombierelolied/infect.inc"
#include "zombierelolied/damage.inc"
#include "zombierelolied/knockback.inc"
#include "zombierelolied/map.inc"
#include "zombierelolied/ztele.inc"
#include "zombierelolied/zmarket.inc"
#include "zombierelolied/zitem.inc"

#include "zombierelolied/api.inc"

public void OnPluginStart () {
	hookEvents ();
	hookCommands ();
	registerCommands ();
}

public void OnMapStart () {
	OnPrecacheParticles ();

	clearRoundWinCounts ();

	Map_MapStartOnLoadWhitelistMaps ();
		
	kvConfLoader ();
	onLoadWeaponDB ();
	onLoadZombieClassDB ();
	randomSoundPrecache ();
	onChangeMapStyle ();
	
	createAlertHudMsgSyncHandler ( true );
}

public void OnMapEnd () {
	zombieSoundsOnEnd ();
	
	createAlertHudMsgSyncHandler ( false );
}

public void OnPluginStop () {
	hookEvents ( false );
}

public void OnClientPostAdminCheck ( int client ) {
	connectToChangeClientValue ( client );
	loadClientData ( client );
//	zmarketOutputValues ( client );
//	zclassOutputValues ( client );
	connectToSetupZombieData ( client );
	createHealthRechargeTimer ( client );
	zombieSoundsClientInit ( client );
}

public void OnClientDisconnect ( int client ) {
	saveClientData ( client );
	disconnectToClearClientValue ( client );
	disconnectToGameRestart ( client );
//	zmarketInputValues ( client );
//	zclassInputValues ( client );
}

public void OnClientDisconnect_Post ( int client ) {
}

public void OnEntityCreated ( int entity, const char[] classname ) {
	onSpawnEntPost ( entity );
	onFlashbangCreated ( entity, classname );
	onSmokeGrenadeCreated ( entity, classname );
}

public void OnGameFrame () {
	displayRoundWinCountInScoreboard ();
//	changeConnectPlayerInfo ();
	for ( int i = 1; i <= MaxClients; i ++ ) {
		if ( !loringlib_IsValidClient ( i ) )	continue;
		
		nemesisLongJump ( i );
		infinityWeaponAmmo ( i );
	//	PrintHintText ( i, "%s\n%s", g_bBuyWeapon[i][ZR_ZMARKET_PRIMARY_WEAPON] ? "BUY PRIMARY WEAPON" : "NOT BUY PRIMARY WEAPON", g_bBuyWeapon[i][ZR_ZMARKET_SECONDARY_WEAPON] ? "BUY SECONDARY WEAPON" : "NOT BUY SECONDARY WEAPON" );
	}
}


public Action zrtest ( int client, int args ) {

}