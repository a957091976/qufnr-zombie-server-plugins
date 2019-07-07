#pragma	semicolon	1
#include <loringlib>
#include <qufnrtools>
#include <sourcestore>
#include <zombierelolied2018>
#include <CustomPlayerSkins>
#include <emitsoundany>
#include "lol_ammofix/lol_ammofix.inc"
#pragma newdecls required

#define PLUGIN_VERSION	"1.0"

public Plugin myinfo = {
	name = "cpp"
	, author = "qufnr"
	, description = "Only C++ & SourcePawn Language Script."
	, version = PLUGIN_VERSION
	, url = "https://steamcommunity.com/id/qufnrp"
};

#define MAXSKILLS		255

/**
 * @section 좀비 클래스별 인덱스 값
 */
#define ZR_ZOMBIE_CLASS_NORMAL		0
#define ZR_ZOMBIE_CLASS_FAST		1
#define ZR_ZOMBIE_CLASS_HEAVY		2

/**
 * 스탯 값
 */
#define MAXSTATS		3		/**< Max Stats */
#define STAT_STR		0
#define STAT_DEF		1
#define STAT_AGL		2
#define STAT_HP			3
#define STAT_CRIT		4

/**
 * 무기 옵션 스텟 값
 */
#define MAXWEAPONSTATS			3
#define WEAPONSTAT_RELOADSPD	0
#define WEAPONSTAT_FIRERATE		1
#define WEAPONSTAT_DAMAGE		2

/**
 * Global Variables
 */
#define IN_PLUGIN_TOTAL_HUDSYNCS			13
#define HUDSYNC_PLAYER_STATE_MSG			0
#define HUDSYNC_MEDIC_BULLET				1		//	NOT USED
#define HUDSYNC_PLAYER_STATE_MSG2			2
#define HUDSYNC_AMMOBOX_INFO				3
#define HUDSYNC_SUPPORTER_DISPLAY_PROP_NAME	4
#define HUDSYNC_SKILL_COOLDOWN				5
#define HUDSYNC_MEDIC_BULLET_GET			6		//	NOT USED
#define HUDSYNC_SNIPER_HITMARK_BULLET_INFO	7		//	not used
#define HUDSYNC_ZOMBIE_POISON_SECRET_CLEAR	8
#define HUDSYNC_ROUND_END_SCOREBOARD		9
#define HUDSYNC_LOOK_AT_PLAYER				10
#define HUDSYNC_LOWAMMO_ALERT				11
#define HUDSYNC_RECEIVE_DOLLAR				12

#define	HUDSYNC_ROUNDLEVEL_ALERT			7

Handle g_hGlobalTimer = null;
Handle g_hClientTimer[MAXPLAYERS + 1] = { null, ... };
Handle g_hDamageSyncHud[5] = null;
Handle g_hHudSyncArray[IN_PLUGIN_TOTAL_HUDSYNCS] = { null, ... };

/**
 * Item Variables
 */
#define MAXITEMS		512
#define MAX_ITEM_TYPES			17

/**
 * @section 아이템 유형
 */
#define ITEM_TYPE_NORMAL				0
#define ITEM_TYPE_CONSUME_EXP			1
#define ITEM_TYPE_CONSUME_SP			2
#define ITEM_TYPE_CONSUME_PLAYERPT		3
#define ITEM_TYPE_EQUIPMENT				4
#define ITEM_TYPE_EQ_WEAPON				4
#define ITEM_TYPE_EQ_SOULWEAPON			5
#define ITEM_TYPE_EQ_HEAD				6
#define ITEM_TYPE_EQ_TOP				7
#define ITEM_TYPE_EQ_BOTTOMS			8
#define ITEM_TYPE_EQ_BOOTS				9
#define ITEM_TYPE_CONSUME_AP			10
#define ITEM_TYPE_CONSUME_HP			11
#define ITEM_TYPE_CONSUME_RESET_STAT	12
#define ITEM_TYPE_CONSUME_RESET_CLASS	13
#define ITEM_TYPE_PACKAGE				14
#define ITEM_TYPE_CONSUME_RESET_PLAYERPT	15
#define ITEM_TYPE_SLOT_UNLOCK			16	/**< 캐릭터 슬롯 언락 아이템 */

/**
 * 아이템 사용 인덱스 값
 */
#define ITEM_USE_TYPE_AP		0
#define ITEM_USE_TYPE_HP		1

/**
 * @seciton 아이템 희귀도
 */
#define ITEM_RARITY_NORMAL		0
#define ITEM_RARITY_RARE		1
#define ITEM_RARITY_SR			2
#define ITEM_RARITY_SSR			3

#define ITEM_RARITY_HERO		2
#define ITEM_RARITY_UNIQUE		3
#define ITEM_RARITY_LEGENDARY		4

/**
 * @section 아이템 액션 값
 */
#define ITEM_OPTION_TYPE_STR		100
#define ITEM_OPTION_TYPE_AGL		101
#define ITEM_OPTION_TYPE_DEF		102
#define ITEM_OPTION_TYPE_HP			103
#define ITEM_OPTION_TYPE_RELOAD			200
#define ITEM_OPTION_TYPE_FIRERATE		201
#define ITEM_OPTION_TYPE_FIRE_DAMAGE	202

/**
 * @section 유물 타입
 */
#define ITEM_EQUIPMENT_TYPE_WEAPON	1
#define ITEM_EQUIPMENT_TYPE_CLASS	2
#define ITEM_EQUIPMENT_TYPE_PASSIVE	3

#define MAX_EQUIPMENT_OPTIONS		3		/**< 최대 유물 옵션 갯수 */

#define ITEM_NAME_MAX_LENGTH			64
#define ITEM_DESCRIPTION_MAX_LENGTH		128
#define ITEM_CRAFTITEM_CHAR_MAX_LENGTH	16

int g_iDefineItems = 0;
char g_strItemName[MAXITEMS + 1][ITEM_NAME_MAX_LENGTH];
char g_strItemDescription[MAXITEMS + 1][ITEM_DESCRIPTION_MAX_LENGTH];
int g_iItemType[MAXITEMS + 1];
int g_iItemPrice[MAXITEMS + 1];
int g_iItemRarity[MAXITEMS + 1];
bool g_bItemSell[MAXITEMS + 1];
int g_iItemActionType[MAXITEMS + 1];
float g_fItemAction[MAXITEMS + 1];
int g_iItemEquipLevel[MAXITEMS + 1];	
char g_strItemCraftItems[MAXITEMS + 1][4][ITEM_CRAFTITEM_CHAR_MAX_LENGTH];		/**< 해당 아이템 조합에 필요한 아이템 */
int g_iItemDrop[MAXITEMS + 1] = { 0, ... };
int g_iItemEquipmentType[MAXITEMS + 1];				/**< 유물 타입 */
char g_strItemEquipmentTypeTarget[MAXITEMS + 1][64];		/**< 유물 타입 타겟 ex) 무기 유물, 병과 유물 */
char g_strItemEquipmentOptions[MAXITEMS + 1][MAX_EQUIPMENT_OPTIONS + 1][16];	/**< 유물 옵션 값 */

//float g_fMaxWorldLength;

/**
 * Class Variables
 */
int g_iDefineClass = 0;		/**< 찾은 병과 수 */
#define MAX_CLIENT_CLASS	7
#define CLASS_NAME_MAX_LENGTH	32
#define CLASS_DESCRIPTION_MAX_LENGTH	512
enum ClientClass {
	ClientClass_Normal = 0
	, ClientClass_Assault
	, ClientClass_Sniper
	, ClientClass_Medic
	, ClientClass_Supporter
	, ClientClass_Shotgunner
	, ClientClass_Gunslinger
};
char g_strClientClassName[MAX_CLIENT_CLASS + 1][CLASS_NAME_MAX_LENGTH];
char g_strClientClassDescription[MAX_CLIENT_CLASS + 1][CLASS_DESCRIPTION_MAX_LENGTH];
int g_iClientClassNeedlvl[MAX_CLIENT_CLASS + 1];
int g_iClientClassOnlyAdmin[MAX_CLIENT_CLASS + 1];

/**
 * Skill Variables
 */
#define SKILL_TYPE_PASSIVE	0
#define SKILL_TYPE_ACTIVE	1
int g_iDefineClassSkills[MAX_CLIENT_CLASS + 1] = { 0, ... };		/**< 각 병과에서 찾은 스킬 수 */
#define SKILL_NAME_MAX_LENGTH	64
#define SKILL_DESCRIPTION_MAX_LENGTH	512
char g_strSkillName[MAX_CLIENT_CLASS + 1][MAXSKILLS + 1][SKILL_NAME_MAX_LENGTH];	/**< 스킬 이름 */
char g_strSkillDescription[MAX_CLIENT_CLASS + 1][MAXSKILLS + 1][SKILL_DESCRIPTION_MAX_LENGTH];	/**< 스킬 설명 */
int g_iSkillMaxlvl[MAX_CLIENT_CLASS + 1][MAXSKILLS + 1];		/**< 스킬 최대 레벨 */
int g_iSkillNeedPrice[MAX_CLIENT_CLASS + 1][MAXSKILLS + 1];		/**< 스킬 익힐 시 필요한 플레이어pt */
int g_iSkillType[MAX_CLIENT_CLASS + 1][MAXSKILLS + 1];			/**< 스킬 유형, 0 - 패시브 1 - 액티브 */
int g_iSkillConsumeAP[MAX_CLIENT_CLASS + 1][MAXSKILLS + 1];		/**< 스킬 사용 시 필요한 AP 량 (액티브 스킬에만 해당됨.) */
float g_fSkillAction[MAX_CLIENT_CLASS + 1][MAXSKILLS + 1];		/**< 스킬 효과 값 */
int g_iSkillCooldown[MAX_CLIENT_CLASS + 1][MAXSKILLS + 1];	/**< 스킬 쿨다운 시간 */
int g_iSkillBlockSkill_Id[MAX_CLIENT_CLASS + 1][MAXSKILLS + 1];	/**< 값에 들어간 스킬 아이디가 있으면 배울 수 없는 스킬 */
int g_iSkillActivitySkill_Id[MAX_CLIENT_CLASS + 1][MAXSKILLS + 1];	/**< 값에 들어간 스킬 아이디를 배워야 배울 수 있는 스킬 */
int g_iSkillActivitySkillMaxlvl_Id[MAX_CLIENT_CLASS + 1][MAXSKILLS + 1];	/**< 값에 들어간 스킬 아이디의 레벨이 최대 값이여야 배울 수 있는 스킬 */

/**
 * Client Class Variables
 */
ClientClass g_iClientClass[MAXPLAYERS + 1] = { ClientClass_Normal, ... };
int g_iSkill[MAX_CLIENT_CLASS + 1][MAXSKILLS + 1][MAXPLAYERS + 1];

/**
 * Client Variables
 */
int g_iItem[MAXITEMS + 1][MAXPLAYERS + 1];
char g_strEquipItem[MAX_ITEM_TYPES + 1][MAXPLAYERS + 1][ITEM_NAME_MAX_LENGTH];
//int g_iJewel[MAXPLAYERS + 1];
int g_iLevel[MAXPLAYERS + 1];
int g_iExp[MAXPLAYERS + 1];
int g_iStatPoint[MAXPLAYERS + 1];
int g_iStats[MAXSTATS + 1][MAXPLAYERS + 1];
int g_iBonusStats[MAXSTATS + 1][MAXPLAYERS + 1];
float g_iBonusWeaponStat[MAXWEAPONSTATS + 1][MAXPLAYERS + 1];
int g_iNeedStatPoint[MAXSTATS + 1];
int g_iStatMaxlvl[MAXSTATS + 1];
int g_iPlayerPoint[MAXPLAYERS + 1];
int g_iSkillCooldownTicks[MAX_CLIENT_CLASS + 1][MAXSKILLS + 1][MAXPLAYERS + 1];
float g_fModifyNextAtt[MAXPLAYERS + 1] = { 0.0, ... };		/**< 공격 속도 변수 */
bool g_bPressKey[MAX_KEY_PRESS + 1][MAXPLAYERS + 1];
bool g_bSkillCooldown[MAX_CLIENT_CLASS + 1][MAXSKILLS + 1][MAXPLAYERS + 1];
bool g_bSkillUse[MAX_CLIENT_CLASS + 1][MAXSKILLS + 1][MAXPLAYERS + 1];
float g_fDamages[MAXPLAYERS + 1] = { 0.0, ... };
int g_iGameJoinEventIndex[MAXPLAYERS + 1] = { 0, ... };		/**< 접속 할 때 게임 이벤트 인덱스 값을 받아온다. */
bool g_bZombiePoison[MAXPLAYERS + 1] = { false, ... };		/**< 좀비 감염 피해 채크 */
float g_fZombiePoisonTickTime[MAXPLAYERS + 1] = { 0.0, ... };
int g_iZombiePoisonAttacker[MAXPLAYERS + 1] = { false, ... };	/**< 좀비 감염 피해준 attacker. */
int g_iResetReserve[MAXPLAYERS + 1] = { -1, ... };
int g_iSurvivePoint[MAXPLAYERS + 1] = { 0, ... };		/**< 생존 점수 */
int g_iSurviveTime[MAXPLAYERS + 1] = { 0, ... };		/**< 생존 시간 */
int g_iPotionUseCount[2][MAXPLAYERS + 1];				/**< 포션 사용 갯수 */
int g_iTotalLevels[MAXPLAYERS + 1] = { 0, ... };		/**< 슬롯 캐릭터 총 합 레벨 */
int g_iRankLevel[MAXPLAYERS + 1] = { 0, ... };			/**< 랭크 레벨 */
int g_iSurviveCounts[MAXPLAYERS + 1] = { 0, ... };		/**< 누적 생존 횟수 */
int g_iTotalActPoints[MAXPLAYERS + 1] = { 0, ... };		/**< 누적 행동 점수 */

/**
 * Equipment variables
 */
#define		EQUIPMENT_BONUS_STAT_STR		0
#define		EQUIPMENT_BONUS_STAT_DEF		1
#define		EQUIPMENT_BONUS_STAT_AGL		2
#define		EQUIPMENT_BONUS_STAT_RELOADSPD	3
#define		EQUIPMENT_BONUS_STAT_FIRERATE	4
#define		EQUIPMENT_BONUS_STAT_DAMAGE		5

#define		MAX_EQUIPMENT_BONUS_STAT		6

#define		EQUIPITEM_MAX_SLOTS		3	/**< 최대 유물 칸 */
int g_iEquipItemSlotUnlock[EQUIPITEM_MAX_SLOTS] = { 10, 20, 40 };	/**< 유물 칸 언락 레벨 커트라인 */
int g_iEquipItemIndex[EQUIPITEM_MAX_SLOTS][MAXPLAYERS + 1];
int g_iEquipBonusSkill[MAX_CLIENT_CLASS + 1][MAXSKILLS + 1][MAXPLAYERS + 1];
float g_fEquipBonusStat[EQUIPITEM_MAX_SLOTS][MAX_EQUIPMENT_BONUS_STAT][MAXPLAYERS + 1];

/**
 * Party Variables
 */
#define PARTY_MAX_MEMBERS		4	/**< 파티 최대 인원 */

#define PARTY_MEMBER_INDEX		0
#define PARTY_MEMBER_NICKNAME	1

#define PARTY_NO_PARTY_NAME			"#UNKNOWN_PARTY"
#define PARTY_NO_MEMBER_NICKNAME	"#UNKNOWN_USER_NAME"
int g_iPartyMembers[MAXPLAYERS + 1] = { 0, ... };
int g_iPartyManager[MAXPLAYERS + 1] = { 0, ... };
char g_strPartyMembers[MAXPLAYERS + 1][PARTY_MAX_MEMBERS][2][32];
char g_strPartyName[MAXPLAYERS + 1][24];
bool g_bPartyJoined[MAXPLAYERS + 1] = { false, ... };

/**
 * Character Slot Variables
 */
#define MAX_SELECT_SLOT		6			/**< 최대 슬롯 수 */
#define UNLOCK_SLOTS		3			/**< 기본 언락된 슬롯 수 */
#define SLOT_SELECT_TIME	30.0		/**< 슬롯 선택 쿨다운 */

#define SLOT_NO_SELECT		0

int g_iSelectSlotIndex[MAXPLAYERS + 1] = { SLOT_NO_SELECT, ... };	/**< 선택 된 슬롯 */
int g_iUnlockSlots[MAXPLAYERS + 1] = { UNLOCK_SLOTS, ... };			/**< 언락 된 슬롯 갯수 */
bool g_bSelectSlotCooldown[MAXPLAYERS + 1] = { false, ... };		/**< 슬롯 선택 쿨다운 채크 */

/**
 * 보스 관련 변수
 */
bool g_bBossLevel = false;	/**< 보스 맵인지 채크한다. */

/**
 * 라운드 관련
 */
#define		ROUNDLEVEL_CHANGE_LEVELNAME_RLEVEL	4	/**< 이 값의 배수만큼 증가할 때 라운드레벨 이름이 바뀐다. */
#define		ROUNDLEVEL_MAXIMUM_RLEVEL			20	/**< 최대 라운드 레벨 값 */
#define		ROUNDLEVEL_DEFAULT_RLEVEL			5	/**< 라운드 레벨 시작 값 */
int g_iRoundLevel;		/**< 라운드 레벨 변수 */
int g_iDifficulty;		/**< 난이도 변수 */
char g_strRoundLevelName[64];
char g_strRoundLevelNameArray[5][3][64] = {
//	난이도, 난이도 명, 난이도 명 색상
	{ "0", "EASY", "\x01" },
	{ "4", "NORMAL", "\x05" },
	{ "8", "HARD", "\x0F" },
	{ "12", "NIGHTMARE", "\x0E" },
	{ "16", "APOCALYPSE", "\x02" }
};

/**
 * 접속 이벤트
 */
#define		JOIN_MODE_EVENT_NONE				0
#define		JOIN_MODE_EVENT_RESETSTATS			8
#define		JOIN_MODE_EVENT_RESETSKILLS			16
#define		JOIN_MODE_EVENT_2019_CHARACTER_SLOT_EVENT	32
#define		JOIN_MODE_EVENT_2019_REMAKE_EVENT	20190101

/**
 * 포워드 핸들
 */
Handle g_hForwardHandlers[8];
#define		ZD_FORWARD_ON_CHANGE_LEVEL		0
#define		ZD_FORWARD_ON_BUY_ITEM			1

/**
 * 좀비 강화 변수
 */
#define		ZBUY_ZOMBIE_UPGRADE_MAX			10			/**< 좀비 강화 최대 수치 */
int g_iZombieUpgrade[MAXPLAYERS + 1] = { 0, ... };    /**< 좀비 강화 횟수 변수 */

#include "byeolug_sama/settings.inc"
#include "byeolug_sama/functions.inc"

#include "byeolug_sama/join_mode_event.inc"
#include "byeolug_sama/join_select.inc"
#include "byeolug_sama/iteminit.inc"
#include "byeolug_sama/itemdrop.inc"
#include "byeolug_sama/class/ccinit.inc"
#include "byeolug_sama/class/skillinit.inc"

#include "byeolug_sama/class/normal/normal_skills.inc"
#include "byeolug_sama/class/assaulter/assaulter_skills.inc"
#include "byeolug_sama/class/sniper/sniper_skills.inc"
#include "byeolug_sama/class/medic/medic_skills.inc"
#include "byeolug_sama/class/supporter/supporter_skills.inc"
#include "byeolug_sama/class/shotgunner/shotgunner_skills.inc"
#include "byeolug_sama/class/gunslinger/gunslinger_skills.inc"
#include "byeolug_sama/class/zombie/zombie_stats.inc"
#include "byeolug_sama/p_strong_zombie.inc"
#include "byeolug_sama/class/zombie_active_skill.inc"
#include "byeolug_sama/zbuy.inc"

#include "byeolug_sama/weapon/weapon_reload.inc"
#include "byeolug_sama/weapon/weapon_firerate.inc"
#include "byeolug_sama/weapon/weapon_reload_alert.inc"

//#include "byeolug_sama/gasya.inc"
#include "byeolug_sama/newbie_help.inc"
#include "byeolug_sama/craftitem.inc"
#include "byeolug_sama/menu.inc"
#include "byeolug_sama/client.inc"
#include "byeolug_sama/roundlevel.inc"
#include "byeolug_sama/cmds.inc"
#include "byeolug_sama/events.inc"
#include "byeolug_sama/reward.inc"
#include "byeolug_sama/stat.inc"
#include "byeolug_sama/admin.inc"
#include "byeolug_sama/party.inc"
#include "byeolug_sama/rank.inc"

#include "byeolug_sama/boss/bossinit.inc"

#include "byeolug_sama/zombiedepartment.inc"

public void OnPluginStart () {
	registerCommands ();
	JOINSELECT_registerCommands ();
	ZBUY_RegisterCommand ();
	
	registerEvents ();
	
//	AddNormalSoundHook ( view_as <NormalSHook> ( onPlayNormalSounds ) );
//	AddAmbientSoundHook ( view_as <AmbientSHook> ( onPlayAmbientSounds ) );
}

/*
public Action onPlayNormalSounds ( int clients[MAXPLAYERS], int& numClients, char sample[PLATFORM_MAX_PATH], int& entity, int& channel, float& vol, int& level, int& pitch, int& flags, char soundEntry[PLATFORM_MAX_PATH], int& seed ) {
	PrintToServer ( "Normal Sound: %s | Game Sound: %s | Sound Level: %d | Sound Seed: %d", sample, soundEntry, level, seed );
	return Plugin_Continue;
	
}

public Action onPlayAmbientSounds ( char sample[PLATFORM_MAX_PATH], int& entity, float& vol, int& level, int& pitch, float pos[3], int& flags, float& delay ) {
	PrintToServer ( "Ambient Sound: %s | Sound Level: %d | Sound Delay: %.3f | Sound Entity: %d | Sound Positions[3]: %.2f %.2f %.2f", sample, level, delay, entity, pos[0], pos[1], pos[2] );
	return Plugin_Continue;
}
*/

public void OnPluginStop () {
}

public void OnMapEnd () {
	createHudSyncHandler ( false );
	damageHudsync ( false );
}

public void OnMapStart () {
//	getMaxWorldLength ();
	
	//	round level init
	ROUNDLEVEL_StartOnCalcRoundLevel ();
	
	//	Checking boss level.
	BOSS_MapStartOnCheckBossLevel ();
	
	//	Precaches
	onPrecacheSoundsAll ();
	onPrecacheModelsAll ();
	onPrecacheParticlesAll ();
	ZOMBIESKILL_ZombieSkillPrecacheAll ();
	ZBUY_MapStartOnPrecacheAll ();
	RANK_StartOnPrecacheAll ();
	
	//	Define datas
	defineNeedStatpoints ();
	defineStatMaxlvl ();
	defineItemKeyValue ();
	defineClassKeyValue ();
	defineSkillKeyValue ();
	
//	defineNormalGasyaItems ();
	
	//	Create functions
	createGlobalTimer ();
	createHudSyncHandler ( true );
	damageHudsync ( true );
	
	ITEMDROP_OnClearDroppedItemIndex ();
}

public void OnGameFrame () {
	onChangeLevelup ();
//	clientClass_Medic_DisplayMedicBullet ();
	OnGameFrameToClient ();
}

public void OnEntityCreated ( int edict, const char[] classname ) {
	grenadeMasterlyDamage ( edict, classname );
	createWeaponEdictReloadHook ( edict, classname );
	clientClass_Medic_CreatePoisonGrenade ( edict, classname );
	clientClass_Shotgunner_CreateLandmineGrenade ( edict, classname );
}

public Action timerGlobalTimer ( Handle timer ) {
	runningCooldownTime ();
	OnGlobalTimerCallToClient ();
}

public void ZR_OnChangeZombie ( int client, int zombieType, bool isMotherZombie ) {
	STAT_OnZombieArmorChange ( client );
	ZOMBIESTAT_ChangeZombieOnSetStats ( client );
	if ( isMotherZombie )	CLIENT_MOTHER_ZOMBIE_SKILL_MSG ( client )
}