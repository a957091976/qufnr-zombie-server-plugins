#define		SOUND_BARRICADE_SELECT		"ui/csgo_ui_crate_item_scroll.wav"
#define		SOUND_SUPPORT_FREEZE_BULLET	"qufnr/support/support_freeze_bullet.mp3"
#define		SOUND_SUPPORT_IGNITE_BULLET	"qufnr/support/support_ignite_bullet.mp3"

#define		SUPPORTER_FREEZE_BULLET_DAMAGE		800
#define		SUPPORTER_IGNITE_BULLET_DAMAGE		800

#define		SOUND_BARRICADE_SPAWN			"qufnr/support/support_barricade_spawn.mp3"
#define		SOUND_BARRICADE_KILL			"qufnr/support/support_barricade_end.mp3"
#define		SOUND_BARRICADE_BREAK			"qufnr/support/support_barricade_break.mp3"
#define		SOUND_BARRICADE_ACTIVE			"qufnr/support/support_barricade_active.mp3"
#define		SOUND_BARRICADE_EXPLODE			"qufnr/support/support_barricade_explode.mp3"

#define		SUPPORTER_BARRICADE_MODEL		"models/props/de_train/hr_t/window_c/window_c_glass.mdl"
#define		SUPPORTER_BARRICADE_SPAWN_DISTANCE	50.0
#define		SUPPORTER_BARRICADE_HEALTH			400	/**< 쉴드 체력 */
#define		SUPPORTER_BARRICADE_MASTERY_HEALTH	30	/**< 쉴드 체력 증가 값 (마스터리 배울 때) */
#define		SUPPORTER_SHIELD_KNOCKBACK_SCALE	350.0	/**< 쉴드 밀치는 힘 */
#define		SUPPORTER_SHIELD_KNOCKBACK_DMG_MIN	50		/**< 쉴드로 밀칠 때 까지는 쉴드 체력 값 */
#define		SUPPORTER_SHIELD_KNOCKBACK_DMG_MAX	70		/**< */

#define		SUPPORTER_BARRICADE_PARTICLE_Y_FIX	46.0

#define		AMMO_BOX_ALL_WEAPON	33

char g_strBreakSounds[5][128] = {
	"survival/container_damage_01.wav"
	, "survival/container_damage_02.wav"
	, "survival/container_damage_03.wav"
	, "survival/container_damage_04.wav"
	, "survival/container_damage_05.wav"
};

/**
 * Entity Variables
 */
int g_iAmmoboxOwner[SOURCE_MAXENTITIES + 1] = { false, ... };
bool g_bAmmobox[SOURCE_MAXENTITIES + 1] = { false, ... };
bool g_bArmorPiercingAmmobox[SOURCE_MAXENTITIES + 1] = { false, ... };

int g_iBarricadeOwner[SOURCE_MAXENTITIES + 1] = { -1, ... };
bool g_bExplosiveBarricade[SOURCE_MAXENTITIES + 1] = { false, ... };
static int g_iBarricadeParticle[2][MAXPLAYERS + 1];

int g_iAmmobox_Ammo[SOURCE_MAXENTITIES + 1] = { 0, ... };
char g_strAmmobox_Options[AMMO_BOX_ALL_WEAPON][2][64] = {
	{ "weapon_glock", "20" }
	, { "weapon_usp_silencer", "12" }
	, { "weapon_hkp2000", "13" }
	, { "weapon_p250", "13" }
	, { "weapon_deagle", "7" }
	, { "weapon_elite", "30" }
	, { "weapon_revolver", "8" }
	, { "weapon_cz75a", "12" }
	, { "weapon_mp9", "30" }
	, { "weapon_mp7", "30" }
	, { "weapon_mac10", "30" }
	, { "weapon_mp5sd", "30" }
	, { "weapon_ump45", "25" }
	, { "weapon_p90", "50" }
	, { "weapon_bizon", "64" }
	, { "weapon_famas", "25" }
	, { "weapon_galilar", "35" }
	, { "weapon_m4a1", "30" }
	, { "weapon_m4a1_silencer", "20" }
	, { "weapon_ak47", "30" }
	, { "weapon_sg553", "30" }
	, { "weapon_sg556", "30" }
	, { "weapon_aug", "30" }
	, { "weapon_g3sg1", "20" }
	, { "weapon_scar20", "20" }
	, { "weapon_ssg08", "10" }
	, { "weapon_awp", "10" }
	, { "weapon_nova", "8" }
	, { "weapon_xm1014", "7" }
	, { "weapon_mag7", "5" }
	, { "weapon_m249", "100" }
	, { "weapon_tec9", "18" }
	, { "weapon_fiveseven", "20" }
};

bool g_bUseAmmobox[MAXPLAYERS + 1] = { false, ... };

float g_fClientArmorPiercingBulletDamage[MAXPLAYERS + 1] = { 0.0, ... };

int g_iSelectPropNum[MAXPLAYERS + 1] = { 0, ... };
int g_iTagrenadeSelectNum[SOURCE_MAXENTITIES + 1] = { 0, ... };
char g_strPropList[4][2][128] = {
	{ "models/props/cs_office/vending_machine.mdl", "자판기" }
	, { "models/props_c17/oildrum001.mdl", "드럼통" }
	, { "models/props/cs_office/sofa.mdl", "소파" }
	, { "models/props/de_inferno/hay_bails.mdl", "짚더미" }
};

static int g_iSupportCheckDamage[MAXPLAYERS + 1] = { 0, ... };
static int g_iSupportIgniteDamage[MAXPLAYERS + 1][MAXPLAYERS + 1];

static int g_iSupportArmorPiercingBullet[MAXPLAYERS + 1];
static char g_strArmorPiercingBulletWeapon[MAXPLAYERS + 1][32];

static bool g_bIgniteDamaged[MAXPLAYERS + 1] = { false, ... };

#define SUPPORTER_BARRICADE_CLASSNAME	"prop_dynamic_override"

#define SUPPORTER_BARRICADE_OWNER_BREAK_DISTANCE	70.0	/**< 바리케이드 제거 거리 */

#define SUPPORTER_AMMOBOX_MODEL	"models/props_fairgrounds/giraffe.mdl"
#define SUPPORTER_AMMOBOX_MASTER_MODEL "models/props_fairgrounds/elephant.mdl"

void clientClass_Supporter_PrecacheSounds () {
	PrecacheSound ( SOUND_BARRICADE_SPAWN, true );
	PrecacheSound ( SOUND_BARRICADE_KILL, true );
	PrecacheSound ( SOUND_BARRICADE_BREAK, true );
	PrecacheSound ( SOUND_BARRICADE_ACTIVE, true );
	PrecacheSound ( SOUND_BARRICADE_EXPLODE, true );
}

/**
 * SMG 숙련
 */
stock float clientClass_Supporter_SMGUpgrade ( int client, float dmg, int weapon ) {
	if ( g_iClientClass[client] == ClientClass_Supporter ) {
		int clientClass = view_as<int> ( g_iClientClass[client] );
		int skill_lv = g_iSkill[clientClass][SKILL_ID_SMG_UPGRADE][client];
		//	SMG 숙련을 찍었을 경우
		if ( skill_lv > 0 ) {
			char weapon_name[32];
			if ( weapon != -1 )
				GetEdictClassname ( weapon, weapon_name, sizeof ( weapon_name ) );
				
			for ( int i = 0; i < sizeof ( g_strClassWeapon_Supporter ); i ++ ) {
				//	공격 무기가 SMG일 경우?
				if ( StrEqual ( weapon_name, g_strClassWeapon_Supporter[i], false ) ) {
					return dmg = dmg * ( 1.0 + ( skill_lv * g_fSkillAction[clientClass][SKILL_ID_SMG_UPGRADE] ) );
				}
			}
		}
	}
	
	return dmg;
}


/**
 * 보급병 데이터 초기화
 */
void clientClass_Supporter_ClearSupporterData ( int client ) {
	g_iSelectPropNum[client] = 0;
	g_iSupportCheckDamage[client] = 0;
	
	g_iSupportArmorPiercingBullet[client] = 0;
	g_fClientArmorPiercingBulletDamage[client] = 0.0;
	strcopy ( g_strArmorPiercingBulletWeapon[client], sizeof ( g_strArmorPiercingBulletWeapon[] ), "" );
	
	g_bIgniteDamaged[client] = false;
}

/**
 * Ammobox 삭제
 */
public void clientClass_Supporter_RemoveAmmoboxAndBarricadeEdict () {
	CreateTimer ( 0.2, timerRemoveAmmoboxPost );
}

public Action timerRemoveAmmoboxPost ( Handle timer ) {
	for ( int i = 0; i < ( SOURCE_MAXENTITIES + 1 ); i ++ ) {
		g_bAmmobox[i] = false;
		g_iAmmoboxOwner[i] = -1;
		g_bArmorPiercingAmmobox[i] = false;
		g_iAmmobox_Ammo[i] = 0;
		
		g_iBarricadeOwner[i] = -1;
		g_bExplosiveBarricade[i] = false;
	}
	
	return Plugin_Stop;
}

/**
 * 바리케이드 선택
 * @note 사용 키 누를 시
 */
public void clientClass_Supporter_ChangeBarricadeGrenade ( int client ) {
	if ( g_iClientClass[client] == ClientClass_Supporter ) {
		if ( loringlib_HasWeaponName ( client, "weapon_tagrenade" ) ) {
			PrecacheSound ( SOUND_BARRICADE_SELECT );
			EmitSoundToClient ( client, SOUND_BARRICADE_SELECT );
			g_iSelectPropNum[client] ++;
			if ( g_iSelectPropNum[client] > ( sizeof ( g_strPropList ) - 1 ) )
				g_iSelectPropNum[client] = 0;
		}
	}
}

/** 
 * 바리케이드 수류탄 들고 있을 때 바리케이드 이름 표시
 * @note PostThink에서 표시함.
 */
public void clientClass_Supporter_DisplayBarricadeName ( int client ) {
	if ( g_iClientClass[client] == ClientClass_Supporter ) {
		if ( loringlib_HasWeaponName ( client, "weapon_tagrenade" ) ) {
			SetHudTextParams ( -1.0, 0.687, 0.05, 255, 255, 255, 255, 0, 0.0, 0.0, 0.0 );
			
			int selectnum = g_iSelectPropNum[client];
			char progress[256];
			if ( selectnum >= ( sizeof ( g_strPropList ) - 1 ) )
				Format ( progress, sizeof ( progress ), ">>  %s  <<\n< %s | %s >", g_strPropList[selectnum][1], g_strPropList[selectnum - 1][1], g_strPropList[0][1] );
			else if ( selectnum == 0 )
				Format ( progress, sizeof ( progress ), ">>  %s  <<\n< %s | %s >", g_strPropList[selectnum][1], g_strPropList[sizeof ( g_strPropList ) - 1][1], g_strPropList[selectnum + 1][1] );
			else
				Format ( progress, sizeof ( progress ), ">>  %s  <<\n< %s | %s >", g_strPropList[selectnum][1], g_strPropList[selectnum - 1][1] , g_strPropList[selectnum + 1][1] );
			
		//	ShowSyncHudText ( client, g_hHudSyncArray[HUDSYNC_SUPPORTER_DISPLAY_PROP_NAME], "%s\nPress 'RELOAD' Key to Change barricade...", progress );
			ShowSyncHudText ( client, g_hHudSyncArray[HUDSYNC_SUPPORTER_DISPLAY_PROP_NAME], "%s\n'재장전' 키를 눌러 바리케이드를 변경하세요.", progress );
		}
	}
}

/**
 * SMG 스폰
 */
public void clientClass_Supporter_SMGSpawn ( int client, int skill_Id ) {
	if ( ZR_IsClientZombie ( client ) )
		return;
		
	int clientClass = view_as <int> ( ClientClass_Supporter );
	if ( g_bSkillUse[clientClass][skill_Id][client] ) {
		CLIENT_SKILL_NOT_AVAILABLE_NOW_USING_MSG ( client )
		return;
	}
	
	if ( g_bSkillCooldown[clientClass][skill_Id][client] ) {
		CLIENT_SKILL_NOT_AVAILABLE_COOLDOWN_MSG ( client )
		return;
	}
	
	int consumeAP = g_iSkillConsumeAP[clientClass][skill_Id];
	if ( consumeAP > loringlib_GetEntityArmor ( client ) ) {
		CLIENT_SKILL_NOT_AVAILABLE_NEED_AP_MSG ( client )
		return;
	}
	
	g_bSkillUse[clientClass][skill_Id][client] = true;
	g_bSkillCooldown[clientClass][skill_Id][client] = true;
	loringlib_SetEntityArmor ( client, loringlib_GetEntityArmor ( client ) - consumeAP );
	CLIENT_SKILL_USE_MSG ( client, g_strSkillName[clientClass][skill_Id] )
	
	createWeaponSMG ( client, skill_Id );
}

public void createWeaponSMG ( int client, int skill_Id ) {
	int clientClass = view_as<int> ( ClientClass_Supporter );
	g_bSkillUse[clientClass][skill_Id][client] = false;
	g_iSkillCooldownTicks[clientClass][skill_Id][client] = g_iSkillCooldown[clientClass][skill_Id];
	
	char smgarr[7][2][32] = {
		{ "mac10", "MAC-10" }
		, { "mp7", "MP7" }
		, { "mp9", "MP9" }
		, { "mp5sd", "MP5SD" }
		, { "ump45", "UMP45" }
		, { "bizon", "PP-Bizon" }
		, { "p90", "P90" }
	};
	char weapon[32];
	int randam = GetRandomInt ( 0, sizeof ( smgarr ) - 1 );
	Format ( weapon, sizeof ( weapon ), "weapon_%s", smgarr[randam][0] );
	PrintToChat ( client, " \x03???\x01: 이번에 당첨된 무기는... \x06%s\x01입니다!", MESSAGE_PREFIX, smgarr[randam][1] );
	loringlib_RemoveWeaponSlot ( client, CS_SLOT_PRIMARY );
	GivePlayerItem ( client, weapon );
}

/**
 * 탄알 상자 설치
 */
public void clientClass_Supporter_DispatchAmmobox ( int client, int skill_Id ) {
	if ( ZR_IsClientZombie ( client ) )
		return;
	
	int clientClass = view_as <int> ( ClientClass_Supporter );
	if ( g_bSkillUse[clientClass][skill_Id][client] ) {
		CLIENT_SKILL_NOT_AVAILABLE_NOW_USING_MSG ( client )
		return;
	}
	
	if ( g_bSkillCooldown[clientClass][skill_Id][client] ) {
		CLIENT_SKILL_NOT_AVAILABLE_COOLDOWN_MSG ( client )
		return;
	}
	
	int consumeAP = g_iSkillConsumeAP[clientClass][skill_Id];
	if ( consumeAP > loringlib_GetEntityArmor ( client ) ) {
		CLIENT_SKILL_NOT_AVAILABLE_NEED_AP_MSG ( client )
		return;
	}
	
	float pos[3], ang[3];
	//	에임 위치에 소환
/*	if ( !loringlib_GetClientAimEntityDistance ( client, AMMO_BOX_SPAWN_DISTANCE, pos, ang ) ) {
		PrintToChat ( client, "%s \x01거리가 멀어요.", MESSAGE_PREFIX );
		return;
	}	*/

	//	자기 위치에 소환
	GetClientAbsOrigin ( client, pos );
	GetClientAbsAngles ( client, ang );
	
	g_bSkillUse[clientClass][skill_Id][client] = true;
	g_bSkillCooldown[clientClass][skill_Id][client] = true;
	loringlib_SetEntityArmor ( client, loringlib_GetEntityArmor ( client ) - consumeAP );
	CLIENT_SKILL_USE_MSG ( client, g_strSkillName[clientClass][skill_Id] )
	
	int ammo_value = g_iSkill[clientClass][skill_Id][client] * RoundFloat ( g_fSkillAction[clientClass][skill_Id] );
	int ammobox_masterly = g_iSkill[clientClass][SKILL_ID_AMMO_BOX_MASTERY][client];
	if ( ammobox_masterly > 0 ) {	//	보급 상자 마스터리
		int ammobox_masterly_action = ammobox_masterly * RoundFloat ( g_fSkillAction[clientClass][SKILL_ID_AMMO_BOX_MASTERY] );
		ammo_value += ammobox_masterly_action;
	}
	
	createAmmobox ( client, skill_Id, ammo_value, pos, ang );
}

/**
 * 보급 상자 엔티티 스폰
 *
 * @param client
 * @param ammo
 * @param position
 * @param angles
 */
public void createAmmobox ( int client, int skill_Id, int ammo, float position[3], float angles[3] ) {
//	int entity = CreateEntityByName ( "prop_physics_override" );
	int entity = CreateEntityByName ( SUPPORTER_BARRICADE_CLASSNAME );
	if ( !IsValidEdict ( entity ) ) {
		PrintToChatAll ( "Error: Invalid Edict: %d (Target: %d)", entity, client );
		return;
	}
	
	int clientClass = view_as<int> ( ClientClass_Supporter );
	
	g_bSkillUse[clientClass][skill_Id][client] = false;
	g_iSkillCooldownTicks[clientClass][skill_Id][client] = g_iSkillCooldown[clientClass][skill_Id];
	
	g_bAmmobox[entity] = true;
	g_iAmmobox_Ammo[entity] = ammo;
	g_iAmmoboxOwner[entity] = client;
	
	int armor_piercing_ammobox_lv = g_iSkill[clientClass][SKILL_ID_ARMOR_PIERCING_BULLET][client];
	//	철갑탄 상자를 배웠을 경우
	if ( armor_piercing_ammobox_lv > 0 ) {
		g_bArmorPiercingAmmobox[entity] = true;
	//	SDKHook ( entity, SDKHook_SpawnPost, setEntityAmmoboxModel );
		PrecacheModel ( MODEL_ARMOR_PIERCING_AMMO_BOX );
		DispatchKeyValue ( entity, "model", MODEL_ARMOR_PIERCING_AMMO_BOX );
	}
	
	//	안배웠을 경우
	else {
	//	SDKHook ( entity, SDKHook_SpawnPost, setEntityAmmoboxModel );
		PrecacheModel ( MODEL_AMMO_BOX );
		DispatchKeyValue ( entity, "model", MODEL_AMMO_BOX );
	}
	
	DispatchKeyValue ( entity, "disablereceiveshadows", "1" );
	DispatchKeyValue ( entity, "disableshadows", "1" );
	DispatchKeyValue ( entity, "Solid", "6" );
	
//	SetEntProp ( entity, Prop_Send, "m_usSolidFlags", 12 );
//	SetEntProp ( entity, Prop_Data, "m_nSolidType", 6 );
//	SetEntProp ( entity, Prop_Send, "m_CollisionGroup", 1 );
	
	loringlib_SetEntityNoblock ( entity, true );
	
	static char name[64];
	Format ( name, sizeof ( name ), "%N님의 %s", client, g_bArmorPiercingAmmobox[entity] ? "철갑탄 상자" : "보급 상자" );
	SetEntPropString ( entity, Prop_Data, "m_iName", name );
//	SetEntPropEnt ( entity, Prop_Data, "m_hOwnerEntity", client );
	
//	SetEntProp ( entity, Prop_Data, "m_takedamage", 2, 1 );
//	SetEntProp ( entity, Prop_Data, "m_iHealth", ammo );
	
	DispatchSpawn ( entity );
	ActivateEntity ( entity );
	
//	SDKHook ( entity, SDKHook_SpawnPost, setEntityAmmoboxModel );
	
	TeleportEntity ( entity, position, angles, NULL_VECTOR );
	loringlib_LookAtEntityToTarget__NoY ( entity, client );
	
	SetEntityMoveType ( entity, MOVETYPE_NONE );
}

public void setEntityAmmoboxModel ( int entity ) {
	if ( IsValidEdict ( entity ) ) {
		if ( g_bAmmobox[entity] ) {
			if ( g_bArmorPiercingAmmobox[entity] ) {
				DispatchKeyValue ( entity, "model", MODEL_ARMOR_PIERCING_AMMO_BOX );
				SetEntityModel ( entity, MODEL_ARMOR_PIERCING_AMMO_BOX );
			}
			
			else {
				DispatchKeyValue ( entity, "model", MODEL_ARMOR_PIERCING_AMMO_BOX );
				SetEntityModel ( entity, MODEL_AMMO_BOX );
			}
		}
	}
}

/**
 * 보급 상자 사용
 */
public void clientClass_Supporter_UseAmmobox ( int client, int key ) {
	if ( !loringlib_IsValidClient ( client ) )
		return;
	
/*	int debugWeapon = loringlib_GetActiveWeapon ( client );
	if ( debugWeapon != -1 ) {
		char debugWeaponName[32];
		loringlib_GetActiveWeaponName ( client, debugWeaponName, sizeof ( debugWeaponName ) );
		PrintCenterText ( client, "Max Ammo %d (%s)", LOLAmmoFix_GetWeaponMaxAmmo ( debugWeapon ), debugWeaponName );
	}	*/
	
	int edict = loringlib_GetClientAimEntity ( client );
	if ( !IsValidEdict ( edict ) )
		return;
	
	if ( !g_bAmmobox[edict] )
		return;
	
	if ( g_iAmmobox_Ammo[edict] <= 0 ) {
	//	SetHudTextParamsEx ( -1.0, 0.575, 0.05, { 255, 0, 0, 255 }, { 255, 0, 0, 255 } );
	//	ShowSyncHudText ( client, g_hHudSyncArray[HUDSYNC_AMMOBOX_INFO], "이거... 비어있는데요..." );
		PrintCenterText ( client, "이거... 비어있는데요..." );
		return;
	}
	
	static char name[64];
	GetEntPropString ( edict, Prop_Data, "m_iName", name, sizeof ( name ) );
//	SetHudTextParamsEx ( -1.0, 0.575, 0.01, { 178, 230, 240, 255 }, { 178, 230, 240, 255 } );
//	ShowSyncHudText ( client, g_hHudSyncArray[HUDSYNC_AMMOBOX_INFO], "%s\n탄창: %d개", name, g_iAmmobox_Ammo[edict] );
	PrintCenterText ( client, "<font color='#86E57F'>%s</font>\n탄창: %d개", name, g_iAmmobox_Ammo[edict] );
	
	float origin[3], ammobox_origin[3];
	GetClientAbsOrigin ( client, origin );
	GetEntPropVector ( edict, Prop_Send, "m_vecOrigin", ammobox_origin );
	if ( GetVectorDistance ( origin, ammobox_origin ) > AMMO_BOX_USE_DISTANCE )
		return;
	
//	if ( !loringlib_GetNearTrue ( client, edict, origin, AMMO_BOX_USE_DISTANCE ) )
//		return;
	
	if ( ZR_IsClientZombie ( client ) )
		return;
		
	if ( key & IN_USE ) {
		if ( !g_bUseAmmobox[client] ) {
		//	if ( loringlib_HasWeaponShotgun ( client ) )
		//		return;
		
			static char weapon[32];
			//int ammo;
			int active;
			int owner;
			int now_ammo = -1;
			GetClientWeapon ( client, weapon, sizeof ( weapon ) );
			if ( StrEqual ( weapon, "weapon_healthshot", false ) ||
				StrContains ( weapon, "grenade", false ) != -1 ||
				StrEqual ( weapon, "weapon_c4", false ) ||
				StrEqual  ( weapon, "weapon_flashbang", false ) ||
				StrEqual  ( weapon, "weapon_decoy", false ) ||
				StrEqual  ( weapon, "weapon_snowball", false ) ||
				StrEqual  ( weapon, "weapon_molotov", false ) )
				return;
				
			
			for ( int i = 0; i < AMMO_BOX_ALL_WEAPON; i ++ ) {
				if ( StrContains ( weapon, g_strAmmobox_Options[i][0], false ) != -1 ) {
					now_ammo = loringlib_GetWeaponAmmo ( client );
					if ( now_ammo == -1 ) {
						PrintToChat ( client, "%s ERROR: Invalid Weapon Type: %d", MESSAGE_PREFIX, active );
						return;
					}
					
				//	PrintToConsole ( client, "Ammo Max: %d", LOLAmmoFix_GetWeaponMaxAmmo ( active ) );
				//	PrintToConsole ( client, "Ammo: %d", loringlib_GetWeaponAmmo ( client ) );
					
				//	ammo = StringToInt ( g_strAmmobox_Options[i][1] );
					active = loringlib_GetActiveWeapon ( client );
					
					if ( active != -1 ) {
						g_bUseAmmobox[client] = true;	//	사용 채크
						CreateTimer ( AMMO_BOX_USE_COOLDOWN, timerAmmoboxCooldown, client );	//	쿨다운 계산
						
						owner = g_iAmmoboxOwner[edict];
						//	탄알 상자의 주인이 있을 경우
					/*	if ( loringlib_IsValidClient ( owner ) ) {
							//	탄알 상자를 사용하는 클라이언트가 주인이 아닐 경우
							if ( client != owner ) {
								if ( g_iClientClass[owner] == ClientClass_Supporter ) {
									CLIENT_SUPPORTER_CLASS_USE_AMMOBOX ( owner, client )
								}
							}
						}	*/
						
						//	들고있는 무기의 탄창이 70퍼센트 이상일 경우 사용할 수 없다.
						if ( loringlib_GetPercentage ( now_ammo, LOLAmmoFix_GetWeaponMaxAmmo ( active ) ) > 70.0 ) {
							CLIENT_SUPPORTER_CLASS_AMMO_BOX_USE_IS_EXIST_AMMO_MSG ( client )
							return;
						}
						
						//	철갑탄 상자일 경우?
						if ( g_bArmorPiercingAmmobox[edict] ) {
							g_iSupportArmorPiercingBullet[client] = StringToInt ( g_strAmmobox_Options[i][1] ) + 1;	//	Post ammos
							g_fClientArmorPiercingBulletDamage[client] = g_fSkillAction[view_as<int>( ClientClass_Supporter )][SKILL_ID_ARMOR_PIERCING_BULLET];
							strcopy ( g_strArmorPiercingBulletWeapon[client], sizeof ( g_strArmorPiercingBulletWeapon[] ), weapon );
							
							loringlib_SetClip1 ( active, StringToInt ( g_strAmmobox_Options[i][1] ) );
						}
						
						//	탄창 추가
						int ammo_give = ( LOLAmmoFix_GetWeaponMaxAmmo ( active ) - now_ammo ) / 2;
						
						if ( loringlib_IsValidClient ( owner ) && ZR_IsClientHuman ( owner ) &&
							owner != client )
							g_iSurvivePoint[owner] += SURVIVE_POINT_SUPPORT_AMMOBOX;	//	행동 점수
						
						PrecacheSound ( SOUND_AMMO_BOX_EMPTY );
						EmitAmbientSound ( SOUND_AMMO_BOX_EMPTY, ammobox_origin, edict );
						
						g_iAmmobox_Ammo[edict] --;
					//	loringlib_SetClip1 ( active, max_ammo );
						loringlib_SetPlayerWeaponAmmoEx ( client, active, now_ammo + ammo_give, now_ammo + ammo_give );
						
						//	사용 후 탄창이 0개 되면 엔티티를 삭제한다.
						if ( g_iAmmobox_Ammo[edict] <= 0 ) {
							PrecacheSound ( SOUND_AMMO_BOX_EMPTY );
							EmitAmbientSound ( SOUND_AMMO_BOX_EMPTY, ammobox_origin, edict );
							g_bAmmobox[edict] = false;
							AcceptEntityInput ( edict, "Kill" );
						}
						
						/*
						if ( loringlib_GetClip1 ( active ) < max_ammo ) {
							//	탄알 상자에 있는 탄알 수가 ammo 만큼 있거나 ammo보다 적을 경우
							if ( ammo >= g_iAmmobox_Ammo[edict] ) {
								PrecacheSound ( SOUND_AMMO_BOX_EMPTY );
								EmitSoundToAll ( SOUND_AMMO_BOX_EMPTY, edict, _, SNDLEVEL_NORMAL, _, _, _, _, ammobox_origin );
								
								loringlib_SetClip1 ( active, loringlib_GetClip1 ( active ) + g_iAmmobox_Ammo[edict] );
								g_iAmmobox_Ammo[edict] = 0;
								if ( loringlib_GetClip1 ( active ) > max_ammo ) {
									g_iAmmobox_Ammo[edict] += loringlib_GetClip1 ( active ) - max_ammo;
									loringlib_SetClip1 ( active, max_ammo );
									if ( g_iAmmobox_Ammo[edict] <= 0 ) {
										g_bAmmobox[edict] = false;
										AcceptEntityInput ( edict, "Kill" );
									}
									return;
								}
								g_bAmmobox[edict] = false;
								AcceptEntityInput ( edict, "Kill" );
							//	ELSE
							} else {
								PrecacheSound ( SOUND_AMMO_BOX_USE );
								EmitSoundToAll ( SOUND_AMMO_BOX_USE, edict, _, SNDLEVEL_NORMAL, _, _, _, _, ammobox_origin );
								
								loringlib_SetClip1 ( active, loringlib_GetClip1 ( active ) + ammo );
								g_iAmmobox_Ammo[edict] -= ammo;
								if ( loringlib_GetClip1 ( active ) > max_ammo ) {
									g_iAmmobox_Ammo[edict] += loringlib_GetClip1 ( active ) - max_ammo;
									loringlib_SetClip1 ( active, max_ammo );
									return;
								}
								g_iAmmobox_Ammo[edict] -= ammo;
							}
						}
						
						else {
							CLIENT_SUPPORTER_CLASS_AMMO_BOX_USE_IS_FULL_MSG ( client )
						}
						
						*/
					}
				}
			}
		}
	}
}

/**
 * 탄알 상자 쿨다운 계산
 */
public Action timerAmmoboxCooldown ( Handle timer, int client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Stop;
		
	g_bUseAmmobox[client] = false;
	return Plugin_Continue;
}

/**
 * 바리케이드 수류탄 지급
 */
void clientClass_Support_GiveBarricadeGrenade ( int client, int skill_Id ) {
	if ( ZR_IsClientZombie ( client ) )
		return;
	
	int clientClass = view_as <int> ( ClientClass_Supporter );
	if ( g_bSkillUse[clientClass][skill_Id][client] ) {
		CLIENT_SKILL_NOT_AVAILABLE_NOW_USING_MSG ( client )
		return;
	}
	
	if ( g_bSkillCooldown[clientClass][skill_Id][client] ) {
		CLIENT_SKILL_NOT_AVAILABLE_COOLDOWN_MSG ( client )
		return;
	}
	
	int consumeAP = g_iSkillConsumeAP[clientClass][skill_Id];
	if ( consumeAP > loringlib_GetEntityArmor ( client ) ) {
		CLIENT_SKILL_NOT_AVAILABLE_NEED_AP_MSG ( client )
		return;
	}
	
	g_bSkillUse[clientClass][skill_Id][client] = true;
	loringlib_SetEntityArmor ( client, loringlib_GetEntityArmor ( client ) - consumeAP );
	CLIENT_SKILL_USE_MSG ( client, g_strSkillName[clientClass][skill_Id] )
	
	CreateTimer ( 0.0, timerGiveBarricadeGrenadePost, client, TIMER_FLAG_NO_MAPCHANGE );
}

public Action timerGiveBarricadeGrenadePost ( Handle timer, any client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Stop;
	
	if ( g_iClientClass[client] != ClientClass_Supporter )
		return Plugin_Stop;
	
	int clientClass = view_as<int> ( ClientClass_Supporter );
	int skill_lv = g_iSkill[clientClass][SKILL_ID_BARRICADE_GRENADE][client];
	if ( skill_lv > 0 ) {
		if ( ZR_IsClientHuman ( client ) ) {
			//	위치 선정
			float spawnPos[3];
			float spawnEye[3];
			GetClientAbsAngles ( client, spawnEye );
			if ( !Supporter_IsClientAbsDist ( client, SUPPORTER_BARRICADE_SPAWN_DISTANCE, spawnPos ) )
				GetClientAbsOrigin ( client, spawnPos );
				
			//	체력 및 바리케이드 남아 있는 시간 설정
			int health = SUPPORTER_BARRICADE_HEALTH;
			float lifetime = g_fSkillAction[clientClass][SKILL_ID_BARRICADE_GRENADE];
			
			//	에너지 쉴드 마스터리를 배웠을 경우 값을 더 올려 준다.
			if ( g_iSkill[clientClass][SKILL_ID_BARRICADE_GRENADE_MASTERY][client] > 0 ) {
				health += g_iSkill[clientClass][SKILL_ID_BARRICADE_GRENADE_MASTERY][client] * SUPPORTER_BARRICADE_MASTERY_HEALTH;
				lifetime += float ( g_iSkill[clientClass][SKILL_ID_BARRICADE_GRENADE_MASTERY][client] ) * g_fSkillAction[clientClass][SKILL_ID_BARRICADE_GRENADE_MASTERY];
			}
		
			DataPack data;
			CreateDataTimer ( 0.5, timerBarricadeEntSpawn, data, TIMER_FLAG_NO_MAPCHANGE );
			data.WriteCell ( GetClientUserId ( client ) );
			data.WriteFloat ( spawnPos[0] );
			data.WriteFloat ( spawnPos[1] );
			data.WriteFloat ( spawnPos[2] );
			data.WriteFloat ( spawnEye[0] );
			data.WriteFloat ( spawnEye[1] );
			data.WriteFloat ( spawnEye[2] );
			data.WriteCell ( health );
			data.WriteFloat ( lifetime );
			
			spawnPos[2] -= SUPPORTER_BARRICADE_PARTICLE_Y_FIX;
			
			g_iBarricadeParticle[0][client] = loringlib_CreateParticleEx ( client, 0, spawnPos, spawnEye, g_iSkill[clientClass][SKILL_ID_BARRICADE_EXPLOSION][client] > 0 ? "support_exguard_start" : "support_guard_start", false, lifetime + 0.5 );
		}
	}
	
	return Plugin_Stop;
}

public Action timerBarricadeEntSpawn ( Handle timer, DataPack data ) {
	data.Reset ();
	static int client;
	client = GetClientOfUserId ( data.ReadCell () );
	if ( !loringlib_IsValidClient ( client ) || g_iClientClass[client] != ClientClass_Supporter )
		return Plugin_Stop;
	
	float spawnPos[3];
	float spawnEye[3];
	spawnPos[0] = data.ReadFloat ();
	spawnPos[1] = data.ReadFloat ();
	spawnPos[2] = data.ReadFloat ();
	spawnEye[0] = data.ReadFloat ();
	spawnEye[1] = data.ReadFloat ();
	spawnEye[2] = data.ReadFloat ();
	
	static int health;
	static float lifetime;
	health = data.ReadCell ();
	lifetime = data.ReadFloat ();
	
	static char name[64];
	Format ( name, sizeof ( name ), "%N님의 에너지 쉴드", client );
	
	int clientClass = view_as <int> ( ClientClass_Supporter );
	
/*	int entity = CreateEntityByName ( "prop_physics_override" );
	if ( entity != -1 ) {
		PrecacheModel ( SUPPORTER_BARRICADE_MODEL );
		SetEntityModel ( entity, SUPPORTER_BARRICADE_MODEL );
	//	DispatchKeyValue ( entity, "model", SUPPORTER_BARRICADE_MODEL );
		DispatchKeyValue( entity, "disablereceiveshadows", "1" );
		DispatchKeyValue( entity, "disableshadows", "1" );
		DispatchKeyValue( entity, "Solid", "6" );
		
		SetEntityMoveType ( entity, MOVETYPE_NONE );
		loringlib_SetEntityRenderColor ( entity, 0, 0, 0, 0 );
		loringlib_SetEntityOwner2 ( entity, client );
		
		SetEntProp ( entity, Prop_Data, "m_takedamage", 2, 1 );
		SetEntProp ( entity, Prop_Data, "m_iHealth", health );
		SetEntPropString ( entity, Prop_Data, "m_iName", name );
		
		TeleportEntity ( entity, spawnPos, spawnEye, NULL_VECTOR );
		
		SDKHook ( entity, SDKHook_OnTakeDamage, onPropDamage );
		
		//	에너지 쉴드 마스터 기술
		if ( g_iSkill[clientClass][SKILL_ID_BARRICADE_EXPLOSION][client] > 0 )
			g_bExplosiveBarricade[entity] = true;
		else
			g_bExplosiveBarricade[entity] = false;
		
		EmitAmbientSound ( SOUND_BARRICADE_SPAWN, spawnPos, entity );
		EmitSoundToAll ( SOUND_BARRICADE_ACTIVE, SOUND_FROM_PLAYER, _, _, _, _, _, _, spawnPos, _, _, lifetime );
		
		spawnPos[2] -= SUPPORTER_BARRICADE_PARTICLE_Y_FIX;
		g_iBarricadeParticle[1][client] = loringlib_CreateParticleEx ( entity, 0, spawnPos, spawnEye, g_iSkill[clientClass][SKILL_ID_BARRICADE_EXPLOSION][client] > 0 ? "support_exguard_mid" : "support_guard_mid", true, lifetime );
		
		CreateTimer ( lifetime, timerBarricadeEntKill, entity, TIMER_FLAG_NO_MAPCHANGE );
	}	*/
	
	int entity = CreateEntityByName ( "prop_physics_override" );
	if ( entity != -1 ) {
		PrecacheModel ( SUPPORTER_BARRICADE_MODEL );
		SetEntityModel ( entity, SUPPORTER_BARRICADE_MODEL );
		
		DispatchKeyValue( entity, "disablereceiveshadows", "1" );
		DispatchKeyValue( entity, "disableshadows", "1" );
		DispatchKeyValue( entity, "Solid", "6" );
		
		DispatchSpawn ( entity );
		ActivateEntity ( entity );
		
		SetEntityMoveType ( entity, MOVETYPE_NONE );
		SetEntProp ( entity, Prop_Send, "m_usSolidFlags", 12 );
		SetEntProp ( entity, Prop_Data, "m_nSolidType", 6 );
		SetEntProp ( entity, Prop_Send, "m_nSolidType", 2 );
		loringlib_SetEntityOwner2 ( entity, client );
		loringlib_SetEntityRenderColor ( entity, 255, 255, 255, 0 );
		SetEntProp ( entity, Prop_Data, "m_takedamage", 2, 1 );
		SetEntProp ( entity, Prop_Data, "m_iHealth", health );
		SetEntProp ( entity, Prop_Data, "m_iMaxHealth", health );
		SetEntPropString ( entity, Prop_Data, "m_iName", name );
		
		TeleportEntity ( entity, spawnPos, spawnEye, NULL_VECTOR );
		
		SDKHook ( entity, SDKHook_OnTakeDamage, onPropDamage );
		SDKHook ( entity, SDKHook_StartTouch, Supporter_ShieldStartTouch );
		
		//	에너지 쉴드 마스터 기술
		if ( g_iSkill[clientClass][SKILL_ID_BARRICADE_EXPLOSION][client] > 0 )
			g_bExplosiveBarricade[entity] = true;
		else
			g_bExplosiveBarricade[entity] = false;
		
		EmitAmbientSound ( SOUND_BARRICADE_SPAWN, spawnPos, entity );
		EmitSoundToAll ( SOUND_BARRICADE_ACTIVE, SOUND_FROM_PLAYER, _, _, _, _, _, _, spawnPos, _, _, lifetime );
		
		spawnPos[2] -= SUPPORTER_BARRICADE_PARTICLE_Y_FIX;
		g_iBarricadeParticle[1][client] = loringlib_CreateParticleEx ( entity, 0, spawnPos, spawnEye, g_iSkill[clientClass][SKILL_ID_BARRICADE_EXPLOSION][client] > 0 ? "support_exguard_mid" : "support_guard_mid", true, lifetime );
		
		CreateTimer ( lifetime, timerBarricadeEntKill, entity, TIMER_FLAG_NO_MAPCHANGE );
	}
	return Plugin_Stop;
}

public void Supporter_ShieldStartTouch ( int caller, int activator ) {
	if ( activator < 1 || activator > MaxClients || !IsClientInGame ( activator ) || !IsPlayerAlive ( activator ) || ZR_IsClientHuman ( activator ) )
		return;
	
	if ( !IsValidEdict ( caller ) )
		return;
	
	static float entityPos[3], activatorEye[3], vec[3];
	GetEntPropVector ( caller, Prop_Send, "m_vecOrigin", entityPos );
	GetClientEyePosition ( activator, activatorEye );
	
	entityPos[2] = activatorEye[2];
	MakeVectorFromPoints ( entityPos, activatorEye, vec );
	NormalizeVector ( vec, vec );
	ScaleVector ( vec, SUPPORTER_SHIELD_KNOCKBACK_SCALE );
	TeleportEntity ( activator, NULL_VECTOR, NULL_VECTOR, vec );
	
	if ( GetEntProp ( caller, Prop_Data, "m_takedamage" ) == 2 )
		loringlib_MakeDamage ( activator, caller, GetRandomInt ( SUPPORTER_SHIELD_KNOCKBACK_DMG_MIN, SUPPORTER_SHIELD_KNOCKBACK_DMG_MAX ), "weapon_knife", DMG_SLASH );
	
	PrintToServer ( "[Supporter Shield] %d on Push entity %d, Vectors: %.2f", caller, activator, vec );
}

public Action timerBarricadeEntKill ( Handle timer, any entity ) {
	if ( IsValidEdict ( entity ) ) {

		float pos[3];
		loringlib_GetEntityOriginEx ( entity, pos );
		EmitAmbientSound ( SOUND_BARRICADE_KILL, pos, entity );
		
		int owner = loringlib_GetEntityOwner2 ( entity );
		if ( loringlib_IsValidClient ( owner ) ) {
			int clientClass = view_as<int> ( ClientClass_Supporter );

			g_bSkillUse[clientClass][SKILL_ID_BARRICADE_GRENADE][owner] = false;
			g_iSkillCooldownTicks[clientClass][SKILL_ID_BARRICADE_GRENADE][owner] = g_iSkillCooldown[clientClass][SKILL_ID_BARRICADE_GRENADE];
			g_bSkillCooldown[clientClass][SKILL_ID_BARRICADE_GRENADE][owner] = true;
			
			pos[2] -= SUPPORTER_BARRICADE_PARTICLE_Y_FIX;
			//	폭발 파티클
			if ( g_iSkill[clientClass][SKILL_ID_BARRICADE_EXPLOSION][owner] > 0 )
				loringlib_CreateParticle ( entity, 0, pos, "support_exguard_break" );
			
			Supporter_OnBarricadeExplosion ( entity, owner, pos );
		/*	int idx = INVALID_ENT_REFERENCE;
			for ( int i = 0; i < sizeof ( g_iBarricadeParticle ); i ++ ) {
				if ( g_iBarricadeParticle[i][owner] > 0 ) {
					idx = EntIndexToEntRef ( g_iBarricadeParticle[i][owner] );
					if ( idx != INVALID_ENT_REFERENCE ) {
						AcceptEntityInput ( idx, "Kill" );
						g_iBarricadeParticle[i][owner] = -1;
					}
				}
			}	*/
		}
	}
	
	return Plugin_Stop;
}

void Supporter_OnBarricadeExplosion ( int entity, int owner, float pos[3] ) {
	if ( IsValidEdict ( entity ) ) {
		//	주인이 있는 바리케이드일 경우
		if ( loringlib_IsValidClient ( owner ) ) {
			int idx = INVALID_ENT_REFERENCE;
			for ( int i = 0; i < 2; i ++ ) {
				if ( g_iBarricadeParticle[i][owner] > 0 ) {
					idx = EntIndexToEntRef ( g_iBarricadeParticle[i][owner] );
					if ( idx != INVALID_ENT_REFERENCE ) {
						AcceptEntityInput ( idx, "Kill" );
						g_iBarricadeParticle[i][owner] = -1;
					}
					
				//	PrintToChatAll ( "삭제: %d %d | %N", g_iBarricadeParticle[i][owner], idx, owner );
				}
			}
			
			//	바리케이드 주인의 병과가 서포터일 경우
			if ( g_iClientClass[owner] == ClientClass_Supporter ) {
				int clientClass = view_as<int> ( ClientClass_Supporter );
				int barricade_explosion_lv = g_iSkill[clientClass][SKILL_ID_BARRICADE_EXPLOSION][owner];
				//	바리케이드 익스플로션을 배웠고, 폭발하는 바리케이드일 경우
				if ( barricade_explosion_lv > 0 ) {
					if ( g_bExplosiveBarricade[entity] ) {
						barricadeExplosion ( owner, pos, "weapon_hegrenade", 300.0, RoundFloat ( g_fSkillAction[clientClass][SKILL_ID_BARRICADE_EXPLOSION] ), CS_TEAM_CT );
					//	loringlib_CreateParticle ( entity, 0, pos, "explosion_hegrenade_dirt" );
						PrecacheSound ( SOUND_BARRICADE_EXPLODE );
						EmitAmbientSound ( SOUND_BARRICADE_EXPLODE, pos, entity );
					}
				}
			}
		}
		
		//	주인이 없을 경우
		else {
			//	폭발 바리케이드일 경우
			if ( g_bExplosiveBarricade[entity] ) {
				barricadeExplosion ( 0, pos, "weapon_hegrenade", 300.0, 1000, CS_TEAM_CT );
			//	loringlib_CreateParticle ( entity, 0, pos, "explosion_hegrenade_dirt" );
				PrecacheSound ( SOUND_BARRICADE_EXPLODE );
				EmitAmbientSound ( SOUND_BARRICADE_EXPLODE, pos, entity );
			}
		}
		
		AcceptEntityInput ( entity, "Kill" );
	}
}

public Action onPropDamage ( int entity, int &attacker, int &inflictor, float &damage, int &dmgType ) {
	if ( !IsValidEdict ( entity ) )
		return Plugin_Continue;
		
	if ( !loringlib_IsValidClient ( attacker ) )
		return Plugin_Continue;
		
	if ( ZR_IsClientHuman ( attacker ) ) {
		damage = 0.0;
		return Plugin_Changed;
	}

	int roll = GetRandomInt ( 0, 2 );
	float pos[3];
	GetEntPropVector ( entity, Prop_Send, "m_vecOrigin", pos );
	PrecacheSound ( g_strBreakSounds[roll] );
	EmitAmbientSound ( g_strBreakSounds[roll], pos, entity );
	
	int owner = loringlib_GetEntityOwner2 ( entity );
	if ( loringlib_IsValidClient ( owner ) && ZR_IsClientHuman ( owner ) )
		PrintHintText ( owner, "에너지 쉴드 체력: <font color='#ff0000'>%d</font> / <font color='#00ff00'>%d</font> HP", loringlib_GetEntityHealth ( entity ) - RoundFloat ( damage ), loringlib_GetEntityMaxHealth ( entity ) );
	
	//	바리케이드의 체력이 데미지와 같거나 많을 경우
	if ( damage >= loringlib_GetEntityHealth ( entity ) ) {
		Supporter_OnBarricadeExplosion ( entity, owner, pos );
		EmitAmbientSound ( SOUND_BARRICADE_BREAK, pos, entity );
		pos[2] -= SUPPORTER_BARRICADE_PARTICLE_Y_FIX;
		
		if ( loringlib_IsValidClient ( owner ) ) {
			int clientClass = view_as<int> ( ClientClass_Supporter );

			g_bSkillUse[clientClass][SKILL_ID_BARRICADE_GRENADE][owner] = false;
			g_iSkillCooldownTicks[clientClass][SKILL_ID_BARRICADE_GRENADE][owner] = g_iSkillCooldown[clientClass][SKILL_ID_BARRICADE_GRENADE];
			g_bSkillCooldown[clientClass][SKILL_ID_BARRICADE_GRENADE][owner] = true;
	
			loringlib_CreateParticle ( entity, 0, pos, g_iSkill[clientClass][SKILL_ID_BARRICADE_EXPLOSION][owner] > 0 ? "support_exguard_break" : "support_guard_break" );
			if ( ZR_IsClientHuman ( owner ) )
				PrintHintText ( owner, "에너지 쉴드 체력: <font color='#ff0000'>!!!</font> HP" );
		}
		return Plugin_Handled;
	}

	return Plugin_Continue;
}

void barricadeExplosion ( int client, float pos[3], char[] weapon, float distance, int damage, int team ) {
	if ( loringlib_IsValidClient ( client ) || client == 0 ) {
		for ( int target = 1; target <= MaxClients; target ++ ) {
			if ( loringlib_IsValidClient__PlayGame ( target ) ) {
				if ( GetClientTeam ( target ) != team ) {
					//	거리안에 든 타겟
					if ( loringlib_GetNearTrue ( client, target, pos, distance ) ) {
						loringlib_MakeDamage ( client, target, damage, weapon, DMG_BLAST );
					}
				}
			}
		}
		
	//	loringlib_CreateParticle ( client, 0, pos, "explosion_basic" );
	//	loringlib_CreateParticle ( client, 0, pos, "explosion_hegrenade_dirt" );
	}
}

/**
 * 빙결탄 및 발화탄
 * @note player_hurt에서
 */
void clientclass_Supporter_SpecialBullet ( int victim, int attacker, int dmg, char[] weapon ) {
	if ( !loringlib_IsValidClient ( victim ) ||
		!loringlib_IsValidClient ( attacker ) )
		return;
	
	if ( g_iClientClass[attacker] == ClientClass_Supporter ) {
		int clientClass = view_as<int> ( g_iClientClass[attacker] );
		bool success = false;
		char weapon_name[32];
		//	무기 채크
		for ( int i = 0; i < sizeof ( g_strClassWeapon_Supporter ); i ++ ) {
			strcopy ( weapon_name, sizeof ( weapon_name ), g_strClassWeapon_Supporter[i] );
			ReplaceString ( weapon_name, sizeof ( weapon_name ), "weapon_", "" );
			if ( StrContains ( weapon, weapon_name, false ) != -1 )
				success = true;
		}
		
		//	공격한 무기가 서포터 무기가 맞을 경우
		if ( success ) {
			g_iSupportCheckDamage[attacker] += dmg;
			

			float pos[3];
			GetClientAbsOrigin ( attacker, pos );
			pos[2] += 10.0;
			
			//	빙결탄을 배웠을 경우
			int special_bullet_lv = g_iSkill[clientClass][SKILL_ID_FREEZE_BULLET][attacker];
			if ( special_bullet_lv > 0 ) {
				//	빙결탄
				if ( g_iSupportCheckDamage[attacker] >= SUPPORTER_FREEZE_BULLET_DAMAGE ) {
					g_iSupportCheckDamage[attacker] = 0;
					
					float freeze_bullet_action = special_bullet_lv * g_fSkillAction[clientClass][SKILL_ID_FREEZE_BULLET];
					loringlib_FreezeEntity ( victim, freeze_bullet_action );
					CreateTimer ( freeze_bullet_action + 0.1, freezeBulletFreezeVelmodif, victim, TIMER_FLAG_NO_MAPCHANGE );
					
					float eye[3];
					GetClientEyePosition ( victim, eye );
					loringlib_CreateParticle ( victim, 0, eye, "support_iceshot", true, 3.0 );
					
					g_iSurvivePoint[attacker] += SURVIVE_POINT_SUPPORT_FREEZE_BULLET;
					
					PrecacheSound ( SOUND_SUPPORT_FREEZE_BULLET );
					EmitAmbientSound ( SOUND_SUPPORT_FREEZE_BULLET, pos, attacker );
				}
			}
			
				//	발화탄을 배웠을 경우
			special_bullet_lv = g_iSkill[clientClass][SKILL_ID_IGNITE_BULLET][attacker];
			if ( special_bullet_lv > 0 ) {
				//	발화탄
				if ( g_iSupportCheckDamage[attacker] >= SUPPORTER_IGNITE_BULLET_DAMAGE ) {
					g_iSupportCheckDamage[attacker] = 0;
					//	이미 불타고 있다?
					if ( g_bIgniteDamaged[victim] )
						return;
						
					int ignite_bullet_action = special_bullet_lv * RoundFloat ( g_fSkillAction[clientClass][SKILL_ID_IGNITE_BULLET] );
					IgniteEntity ( victim, 3.0 );
					g_iSupportIgniteDamage[attacker][victim] = 5;
					g_bIgniteDamaged[victim] = true;
					
					PrecacheSound ( SOUND_SUPPORT_IGNITE_BULLET );
					EmitAmbientSound ( SOUND_SUPPORT_IGNITE_BULLET, pos, attacker );
					
				//	loringlib_IgniteEntityEx ( victim, attacker, ignite_bullet_action, 3.0, 0.5, "weapon_taser" );
					
					DataPack dp;
					CreateDataTimer ( 0.5, timerIgniteBulletIgnite, dp, TIMER_REPEAT );
					dp.WriteCell ( victim );
					dp.WriteCell ( attacker );
					dp.WriteCell ( ignite_bullet_action );
				}
			}
		}
	}
}

/**
 * 빙결탄 경직
 */
public Action freezeBulletFreezeVelmodif ( Handle timer, any client ) {
	if ( loringlib_IsValidClient__PlayGame ( client ) )
		SetEntPropFloat ( client, Prop_Send, "m_flVelocityModifier", 0.0 );
}

/**
 * 발화탄 데미지 타이머
 */
public Action timerIgniteBulletIgnite ( Handle timer, DataPack data ) {
	data.Reset ();
	int victim = data.ReadCell ();
	int attacker = data.ReadCell ();
	int damage = data.ReadCell ();

	if ( loringlib_IsValidClient ( victim ) ) {
		if ( !loringlib_IsValidClient__PlayGame ( attacker ) ) {
			g_iSupportIgniteDamage[attacker][victim] = 0;
			return Plugin_Stop;
		}
		
		if ( g_iSupportIgniteDamage[attacker][victim] > 0 ) {
			float eye[3];
			GetClientEyePosition ( victim, eye );
			loringlib_CreateParticle ( victim, 0, eye, "support_fireshot", true, 3.0 );
			
			loringlib_MakeDamage ( attacker, victim, damage, "weapon_taser", DMG_BURN );
			g_iSupportIgniteDamage[attacker][victim] --;
		}
		
		if ( g_iSupportIgniteDamage[attacker][victim] <= 0 ) {
			g_iSupportIgniteDamage[attacker][victim] = 0;
			g_bIgniteDamaged[victim] = false;
			return Plugin_Stop;
		}
	}
	
	return Plugin_Continue;
}

/**
 * 철갑탄이 있는 상태에서 무기 발사 시 철갑탄을 1 깎는다.
 */
void clientClass_Supporter_WeaponFireOnArmorPiercingAmmo ( int client, char[] weapon ) {
	if ( ZR_IsClientHuman ( client ) ) {
		if ( StrEqual ( weapon, "weapon_healthshot", false ) ||
			StrContains ( weapon, "grenade", false ) != -1 ||
			StrEqual ( weapon, "weapon_c4", false ) ||
			StrEqual  ( weapon, "weapon_flashbang", false ) ||
			StrEqual  ( weapon, "weapon_decoy", false ) ||
			StrEqual  ( weapon, "weapon_snowball", false ) ||
			StrEqual  ( weapon, "weapon_molotov", false ) )
			return;
			
		if ( StrContains ( g_strArmorPiercingBulletWeapon[client], weapon ) == -1 )
			return;
		
		if ( g_iSupportArmorPiercingBullet[client] > 0 )
			g_iSupportArmorPiercingBullet[client] --;
	}
}

/**
 * 철갑탄이 있는 상태에서 데미지를 입힐 시 추가 데미지를 준다.
 */
stock float clientClass_Supporter_TakeDamageToArmorPiercingAmmo ( int client, float dmg, int weapon, float dmgpos[3] ) {
	if ( loringlib_IsValidClient ( client ) ) {
		if ( ZR_IsClientHuman ( client ) ) {
			char weapon_name[32];
			if ( weapon != -1 )
				GetEdictClassname ( weapon, weapon_name, sizeof ( weapon_name ) );
			
			char att_weapon[32];
			if ( StrEqual ( g_strArmorPiercingBulletWeapon[client], "weapon_mp5sd", false ) )	strcopy ( att_weapon, sizeof ( att_weapon ), "weapon_mp7" );
			else if ( StrEqual ( g_strArmorPiercingBulletWeapon[client], "weapon_cz75a", false ) )	strcopy ( att_weapon, sizeof ( att_weapon ), "weapon_p250" );
			else if ( StrEqual ( g_strArmorPiercingBulletWeapon[client], "weapon_revolver", false ) )	strcopy ( att_weapon, sizeof ( att_weapon ), "weapon_deagle" );
			else	strcopy ( att_weapon, sizeof ( att_weapon ), g_strArmorPiercingBulletWeapon[client] );
			
			//	칼도 아니고 철갑탄을 장전한 무기가 맞을 경우
			if ( StrContains ( weapon_name, "knife", false ) == -1 &&
				StrContains ( att_weapon, weapon_name, false ) != -1 ) {
				//	철갑탄이 한발 이상 있을 경우
				if ( g_iSupportArmorPiercingBullet[client] >= 1 ) {
					loringlib_CreateParticle ( client, 0, dmgpos, "support_armorshot", false, 1.0 );
				
					return dmg * ( 1.0 + g_fClientArmorPiercingBulletDamage[client] );
				}
			}
		}
	}
	
	return dmg;
}

/**
 * 자신의 바리케이드를 바라보고 공격 시 바리케이드를 없앤다.
 * @note OnPlayerRunCmd에서
 */
void clientClass_Supporter_RunCmdOnBarricadeBreak ( int client ) {
	if ( !ZR_IsClientHuman ( client ) )
		return;

	int target = loringlib_GetClientAimEntity ( client );
	if ( IsValidEdict ( target ) ) {
		char classname[32];
		GetEdictClassname ( target, classname, sizeof ( classname ) );
		char weapon[32];
		loringlib_GetActiveWeaponName ( client, weapon, sizeof ( weapon ) );
		if ( StrContains ( classname, "prop_", false ) != -1 && StrContains ( weapon, "knife", false ) != -1 ) {
			if ( g_iBarricadeOwner[target] > 0 && g_iBarricadeOwner[target] == client ) {
				float targetOrigin[3];
				float clientOrigin[3];
				GetClientAbsOrigin ( client, clientOrigin );
				loringlib_GetEntityOriginEx ( target, targetOrigin );
				
				if ( GetVectorDistance ( clientOrigin, targetOrigin ) <= SUPPORTER_BARRICADE_OWNER_BREAK_DISTANCE ) {
					char sound[128];
					Format ( sound, sizeof ( sound ), "survival/container_damage_0%d.wav", GetRandomInt ( 1, 5 ) );
					PrecacheSound ( sound );
					EmitAmbientSound ( sound, targetOrigin, target );
					AcceptEntityInput ( target, "Kill" );
				}
			}
		}
	}
}

stock bool Supporter_IsClientAbsDist ( int client, float maxTraceDistance, float resultVectors[3] ) {
	if ( loringlib_IsValidClient ( client ) ) {
		float cleyepos[3], cleyeang[3], eyeangvec[3];
		GetClientEyePosition ( client, cleyepos );
		GetClientEyeAngles ( client, cleyeang );
		
		Handle traceRay = null;
		traceRay = TR_TraceRayFilterEx ( cleyepos, cleyeang, MASK_SOLID, RayType_Infinite, loringlib_TraceRayFilter_tarPlayer, client );
		if ( TR_DidHit ( traceRay ) ) {
			float endpos[3];
			TR_GetEndPosition ( endpos, traceRay );
			//	거리가 일정 이하일 경우
			if ( GetVectorDistance ( cleyepos, endpos ) <= maxTraceDistance ) {
				resultVectors[0] = endpos[0];
				resultVectors[1] = endpos[1];
				resultVectors[2] = endpos[2];
				
				delete traceRay;
				return true;
			}
			
			else {
				GetAngleVectors ( cleyeang, eyeangvec, NULL_VECTOR, NULL_VECTOR );
				NormalizeVector ( eyeangvec, eyeangvec );
				ScaleVector ( eyeangvec, maxTraceDistance );
				AddVectors ( cleyepos, eyeangvec, resultVectors );
				
				delete traceRay;
				return true;
			}
		}
		
		delete traceRay;
	}
	
	return false;
}

public bool Supporter_TraceRayDontHitSelf ( int entity, int mask, any data ) {
	if ( entity == data )	return false;
	return true;
}