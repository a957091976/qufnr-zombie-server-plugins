#pragma semicolon 1

#include <sdktools>
#include <sdkhooks>
#include <loringlib>
#include <qufnrtools>
#include <emitsoundany>
#include <CustomPlayerSkins>
#pragma newdecls required

public Plugin myinfo = {
  name = "Source Store"
  , author = "qufnr"
  , description = "Store in CS:S."
  , version = "3.0"
  , url = "https://steamcommunity.com/id/qufnrp/"
};

/**
 * Dont edit this.
 */
#define MAXITEMS  255		/**< Total item list. */

#define DEFAULT_LASERBULLET_MODEL	"materials/sprites/laser.vmt"

#define MAXITEMTYPE				5		/**< Item Types */
#define ITEMTYPE_CHARACTER		1
#define ITEMTYPE_TRAIL			2
#define ITEMTYPE_TAGTITLE		3
#define ITEMTYPE_LASERBULLET	4
#define ITEMTYPE_AURA			5

#define ITEMRARE_NONE		0		/**< Unknown rarity value */
#define ITEMRARE_NORMAL		1		/**< Item rarity... */
#define ITEMRARE_RARE		2
#define	ITEMRARE_SR			3
#define	ITEMRARE_SSR		4
#define	ITEMRARE_EVENT		5
#define ITEMRARE_ADMIN		100

#define MAXSTORELEVEL		9	/**< Store level data array. */
/*#define STORELEVEL_NORMAL	0
#define STORELEVEL_BRONZE	1
#define STORELEVEL_SILVER	2
#define STORELEVEL_GOLD		3
#define STORELEVEL_DIAMOND	4
#define STORELEVEL_LEGENDARY	5
#define	STORELEVEL_LEGENDARY_MASTER		6
#define STORELEVEL_SUPREME	7
#define STORELEVEL_GLOBAL_ELITE		8	*/

#define TAGTITLE_COLOR_DEFAULT		0
#define TAGTITLE_COLOR_WHITE		1
#define TAGTITLE_COLOR_RED			2
#define TAGTITLE_COLOR_CUSTOM		3
#define TAGTITLE_COLOR_LIME			4
#define TAGTITLE_COLOR_WHITEGREEN	5
#define TAGTITLE_COLOR_WHITERED		6
#define TAGTITLE_COLOR_GRAY			7
#define TAGTITLE_COLOR_YELLOW		8
#define TAGTITLE_COLOR_0A			9
#define TAGTITLE_COLOR_WHITEBLUE	10
#define TAGTITLE_COLOR_BLUE			11
#define TAGTITLE_COLOR_0D			12
#define TAGTITLE_COLOR_PURPLE		13
#define TAGTITLE_COLOR_WHITERED2	14
#define TAGTITLE_COLOR_ORANGE		15

#define TAGTITLE_MAX_COLORS			16
char g_strItemTagColorTemp[TAGTITLE_MAX_COLORS][2][16] = {
	{ "\x04", "Default" }		//	default color is GREEN.
	, { "\x01", "White" }
	, { "\x02", "Red" }
	, { "\x03", "Custom" }
	, { "\x05", "Lime" }
	, { "\x06", "White Green" }
	, { "\x07", "White Red" }
	, { "\x08", "Gray" }
	, { "\x09", "Yellow" }
	, { "\x0A", "0A" }
	, { "\x0B", "White Blue" }
	, { "\x0C", "Blue" }
	, { "\x0D", "0D" }
	, { "\x0E", "Purple" }
	, { "\x0F", "White Red 2" }
	, { "\x10", "Orange" }
};

#define LASERBULLET_DATALIST	7	/**< Laser bullet item data array. */
#define LASERBULLET_MODELPATH	0
#define LASERBULLET_COLOR_RED	1
#define LASERBULLET_COLOR_GREEN	2
#define LASERBULLET_COLOR_BLUE	3
#define LASERBULLET_COLOR_ALPHA	4
#define LASERBULLET_EQUIP_CHECK	5
#define LASERBULLET_OPTION		6

#define ITEM_SELL_DIV			4

/**
 * @section Item Option Index
 */
#define OPTION_RAINBOW	1

#define ITEM_UNNAMED		"NONE"	/**< Item Unnamed display name. */

#define DEFAULT_ARMS_MODEL	"models/weapons/t_arms_phoenix.mdl"	/**< Default arms model. */
/* */

/**
 * Setting Types
 */
#define MAX_SETTINGS	2
#define SETTINGTYPE_MIRROR			0
#define SETTINGTYPE_THIRDPERSON		1

/**
 * Ticket Array number
 */
#define TICKET_NORMAL	0
#define TICKET_PREMIUM	1

/**
 * Client Variables
 */
int g_iItem[MAXPLAYERS + 1][MAXITEMS + 1];			/**< Client total item amount. */
int g_iBalance[MAXPLAYERS + 1] = { 0, ... };						/**< Client currently balance. */
int g_iTicket[2][MAXPLAYERS + 1];						/**< Client gasya tickets. */
int g_iGasyaPoint[MAXPLAYERS + 1];						/**< 가챠 포인트 (플레티넘 뽑기 시 포인트가 올라갑니다.) */
char g_strEquipItemName[MAXPLAYERS + 1][MAXITEMTYPE + 1][64];	/**< Client equipments. */
bool g_bUseTag[MAXPLAYERS + 1] = { false, ... };		/**< 클라이언트가 태그를 사용하고 있는지 채크하는 변수 */
int g_iTrailEntity[MAXPLAYERS + 1] = { INVALID_ENT_REFERENCE, ... };		/**< Client trail sprite entindex. */
char g_strLaserBulletData[MAXPLAYERS + 1][LASERBULLET_DATALIST][256];	/**< Laser bullet datas. */
Handle g_hRewardTimer[MAXPLAYERS + 1] = { null, ... };	/**< Balance reward timer. */
Handle g_hRewardMessageHud = null;
int g_iClientSettingToggle[MAX_SETTINGS][MAXPLAYERS + 1];	/**< Client setting toggles */
int g_iFireCount[MAXPLAYERS + 1];	/**< 이 변수는 무지개 총알에 사용 됩니다. */
bool g_bUsePreview[MAXPLAYERS + 1] = { false, ... };	/**< Client preview cooldown check. */
int g_iAuraEntity[MAXPLAYERS + 1] = { INVALID_ENT_REFERENCE, ... };		/**< Client aura entity 'particle system' */

/**
 * Donator Functions variables
 */
bool g_bIsMvp[MAXPLAYERS + 1] = { false, ... };				/**< VIP */
bool g_bIsMvpPlus[MAXPLAYERS + 1] = { false, ... };			/**< SVIP */
bool g_bToggleRainbow[MAXPLAYERS + 1] = { false, ... };		/**< 몸 무지개로 변하는 거 채크할때 씀. */
float g_fRainbowTickTimes[MAXPLAYERS + 1] = { 1.0, ... };	/**< 무지개 틱 시간 */

/**
 * Temp Variables
 */
int g_iTempSelector[MAXPLAYERS + 1];
int g_iTempTagItem[MAXPLAYERS + 1];

/**
 * Item Variables
 */
int g_iDefineStoreItem = 0;			/**< Items defined. */
char g_strItemName[MAXITEMS + 1][64];			/**< Item Alias */
char g_strItemModel[MAXITEMS + 1][256];			/**< Item Model Path */
char g_strItemArmsModel[MAXITEMS + 1][256];	/**< Only CS:GO work. */
int g_iItemType[MAXITEMS + 1];					/**< Item Type */
int g_iItemPrice[MAXITEMS + 1];					/**< Item Price */
int g_iItemRarity[MAXITEMS + 1];					/**< Item Rare Level */
int g_iItemClassLevel[MAXITEMS + 1];			/**< Item Class Level */
int g_iItemColor[MAXITEMS + 1][4];				/**< Item RGBA Color */
int g_iItemOption[MAXITEMS + 1];				/**< Item Option Values */
char g_strItemAdmSkinSteamID[MAXITEMS + 1][32];	/**< Item Admin skins Steam ID64 */
int g_iItemRandomDropType[MAXITEMS + 1] = { 0, ... };	/**< Except for Item random box. */
int g_iItemNotSell[MAXITEMS + 1] = { 0, ... };			/**<  */
int g_iItemTagColor[MAXITEMS + 1] = { 0, ... };			/**< Tag Title item color index. default 0. */

/**
 * Cvar Handler
 */
ConVar g_hCvarAllowThridperson = null;

/**
 * Forward Handler
 */
Handle g_hForwardHandlerArray[8] = { null, ... };
#define FORWARD_ON_BUY_ITEM			0
#define FORWARD_ON_DROP_GASYA_ITEM	1
#define FORWARD_ON_SELL_ITEM		2

#include "sourcestore/config.inc"
#include "sourcestore/function.inc"
#include "sourcestore/event.inc"
#include "sourcestore/database.inc"
#include "sourcestore/command.inc"
#include "sourcestore/item.inc"
#include "sourcestore/menu.inc"
#include "sourcestore/tag.inc"
#include "sourcestore/reward.inc"
#include "sourcestore/gasya.inc"
#include "sourcestore/donator.inc"

#include "sourcestore/sourcestore.inc"

public void OnPluginStart () {
//	serverStartToDefineItems ();
	onRegisterCommands ();
	onRegisterEvent ( true );
}

public void OnPluginStop () {
	onRegisterEvent ( false );
}

public void OnMapStart () {
	g_hRewardMessageHud = CreateHudSynchronizer ();
	onLoadConfig ();
	mapStartToDefineItems ();
	findCvars ();
}

public void OnMapEnd () {
	for ( int i = 1; i <= MaxClients; i ++ )
		if ( loringlib_IsValidClient ( i ) )
			ClearSyncHud ( i, g_hRewardMessageHud );
}

public void OnClientPutInServer ( int client ) {
	clearPreviewVariables ( client );
	GASYA_ConnectOnGasyaDataClear ( client );
}

public void OnClientPostAdminCheck ( int client ) {
	outputPlayerData ( client );
	createRewardTimer ( client );
	removeAuraParticles ( client );
	removeTrailParticles ( client );
	DONATOR_ConnectOnRainbowBodyHook ( client );
}

public void OnClientDisconnect ( int client ) {
	inputPlayerData ( client );

	removeAuraParticles ( client );
	removeTrailParticles ( client );
	DONATOR_DisconnectOnUnhook ( client );
	killRewardTimer ( client );
}

public void OnGameFrame () {
	for ( int i = 1; i <= MaxClients; i ++ ) if ( loringlib_IsValidClient ( i ) ) {
		displayMirrorModeMsg ( i );
	}
}