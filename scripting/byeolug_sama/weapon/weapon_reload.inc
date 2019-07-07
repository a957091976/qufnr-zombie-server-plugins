//float g_fTotalReloadSpeed[MAXPLAYERS + 1] = { 0.0, ... };

public void createWeaponEdictReloadHook ( int edict, const char[] classname ) {
	if ( HasEntProp ( edict, Prop_Send, "m_reloadState" ) )	//	Shotgun hook
		SDKHook ( edict, SDKHook_ReloadPost, onWeaponReloadPost );
}

public void onWeaponReloadPost ( int weapon, bool successful ) {
	int owner = loringlib_GetWeaponOwner ( weapon );
	if ( owner < 1 )
		return;
	
	if ( GetEntProp ( weapon, Prop_Send, "m_reloadState" ) != 2 )
		return;
		
	if ( IsFakeClient ( owner ) )
		return;
	
	int classNormal = view_as <int> ( ClientClass_Normal );
	//	전술 재장전
	float tactics_reload_action = float ( g_iSkill[classNormal][SKILL_ID_TATICS_RELOAD][owner] ) * g_fSkillAction[classNormal][SKILL_ID_TATICS_RELOAD];
		//	리더쉽 재장전 속도
	float assault_leadership_reloadspd = Assault_GetLeadershipReloadSpeed ( owner );
	//	히어로 재장전 속도
	float assault_hero_reloadspd = clientClass_Assault_ActiveSkillHero ( owner, 1 );
	//	딱딱이 마스터리 재장전 속도
	float sniper_autosniper_reloadspd = clientClass_Sniper_AutosniperMastery ( owner );
	//	나노강화치유탄 재장전 속도
	float medic_medic_boost_reloadspd = Medic_MedicBoostReloadSpeed ( owner );
	//	불릿 타임 패시브 재장전 속도
	float gunslinger_bullettime_reloadspd = clientClass_Gunslinger_BulletTimePassiveReloadSpd ( owner );
	
	float equipmentOptionReloadspd = 0.0;
	//	유물 옵션 효과
	for ( int i = 0; i < EQUIPITEM_MAX_SLOTS; i ++ )
		if ( g_fEquipBonusStat[i][EQUIPMENT_BONUS_STAT_RELOADSPD][owner] > 0.0 )
			equipmentOptionReloadspd += g_fEquipBonusStat[i][EQUIPMENT_BONUS_STAT_RELOADSPD][owner];
	
	float reloadIncrease = 1.0 / ( 1.0 + ( tactics_reload_action + assault_leadership_reloadspd + assault_hero_reloadspd + sniper_autosniper_reloadspd + medic_medic_boost_reloadspd + gunslinger_bullettime_reloadspd + equipmentOptionReloadspd ) );
	
	float oldIdleTime = GetEntPropFloat ( weapon, Prop_Send, "m_flTimeWeaponIdle" );
	float gameTime = GetGameTime ();
	float newIdleTime = ( oldIdleTime - gameTime ) * reloadIncrease + gameTime;
	
	SetEntPropFloat ( weapon, Prop_Send, "m_flTimeWeaponIdle", newIdleTime );
	
//	PrintToChat ( owner, "Reloaded, Idle Time: %.5f", newIdleTime );
}

public void changeWeaponReloadSpeed ( int client ) {
	int classNormal = view_as <int> ( ClientClass_Normal );
	//	전술 재장전
	float tactics_reload_action = float ( g_iSkill[classNormal][SKILL_ID_TATICS_RELOAD][client] ) * g_fSkillAction[classNormal][SKILL_ID_TATICS_RELOAD];
	//	리더쉽 재장전 속도
	float assault_leadership_reloadspd = Assault_GetLeadershipReloadSpeed ( client );
	//	히어로 재장전 속도
	float assault_hero_reloadspd = clientClass_Assault_ActiveSkillHero ( client, 1 );
	//	딱딱이 마스터리 재장전 속도
	float sniper_autosniper_reloadspd = clientClass_Sniper_AutosniperMastery ( client );
	//	나노강화치유탄 재장전 속도
	float medic_medic_boost_reloadspd = Medic_MedicBoostReloadSpeed ( client );
	//	불릿 타임 패시브 재장전 속도
	float gunslinger_bullettime_reloadspd = clientClass_Gunslinger_BulletTimePassiveReloadSpd ( client );
	
	float equipmentOptionReloadspd = 0.0;
	//	유물 옵션 효과
	for ( int i = 0; i < EQUIPITEM_MAX_SLOTS; i ++ )
		if ( g_fEquipBonusStat[i][EQUIPMENT_BONUS_STAT_RELOADSPD][client] > 0.0 )
			equipmentOptionReloadspd += g_fEquipBonusStat[i][EQUIPMENT_BONUS_STAT_RELOADSPD][client];
	
	
	char weapon[32];
	int active = loringlib_GetActiveWeaponName ( client, weapon, sizeof ( weapon ) );
	if ( active == INVALID_ENT_REFERENCE )
		return;
	
	//	샷건 채크
	bool shotgun = HasEntProp ( active, Prop_Send, "m_reloadState" );
	if ( shotgun ) {
		int reloadState = GetEntProp ( active, Prop_Send, "m_reloadState" );
		//	총알이 꽉 찼거나, 총알이 없을 경우
		if ( reloadState == 0 )
			return;
	}
	
	float oldNextAtt = GetEntPropFloat ( active, Prop_Send, "m_flNextPrimaryAttack" );
	float gameTime = GetGameTime ();
	
	float reloadIncrease = 1.0 / ( 1.0 + ( tactics_reload_action + assault_leadership_reloadspd + assault_hero_reloadspd + sniper_autosniper_reloadspd + medic_medic_boost_reloadspd + gunslinger_bullettime_reloadspd + equipmentOptionReloadspd ) );
	SetEntPropFloat ( active, Prop_Send, "m_flPlaybackRate", 1.0 / reloadIncrease );
	
	int viewModel = GetEntPropEnt ( client, Prop_Send, "m_hViewModel" );
	if ( viewModel != INVALID_ENT_REFERENCE )
		SetEntPropFloat ( viewModel, Prop_Send, "m_flPlaybackRate", 1.0 / reloadIncrease );
	
	float newNextAtt = ( oldNextAtt - gameTime ) * reloadIncrease;
	
	DataPack data;
	if ( shotgun ) {
		CreateDataTimer ( 0.01, timerCheckOfShotgunEnd, data, TIMER_FLAG_NO_MAPCHANGE | TIMER_REPEAT );
		data.WriteCell ( EntIndexToEntRef ( active ) );
		data.WriteCell ( GetClientUserId ( client ) );
	}
	
	else {
		CreateDataTimer ( newNextAtt, timerResetPlaybackRate, data, TIMER_FLAG_NO_MAPCHANGE );
		data.WriteCell ( EntIndexToEntRef ( active ) );
		data.WriteCell ( GetClientUserId ( client ) );
	}
	
	newNextAtt += gameTime;
	
	SetEntPropFloat ( active, Prop_Send, "m_flTimeWeaponIdle", newNextAtt );
	SetEntPropFloat ( active, Prop_Send, "m_flNextPrimaryAttack", newNextAtt );
	SetEntPropFloat ( client, Prop_Send, "m_flNextAttack", newNextAtt );
}

/**
 * 플레이백 레이트 초기화
 */
public Action timerResetPlaybackRate ( Handle timer, DataPack data ) {
	data.Reset ();
	
	int active = EntRefToEntIndex ( data.ReadCell () );
	int owner = GetClientOfUserId ( data.ReadCell () );
	
	if ( active != INVALID_ENT_REFERENCE )
		SetEntPropFloat ( active, Prop_Send, "m_flPlaybackRate", 1.0 );
	
	if ( owner > 0 )
		resetClientViewModel ( owner );
	
	return Plugin_Stop;
}

/**
 * 샷건 장전 마무리
 */
public Action timerCheckOfShotgunEnd ( Handle timer, DataPack data ) {
	data.Reset ();
	
	int active = EntRefToEntIndex ( data.ReadCell () );
	int owner = GetClientOfUserId ( data.ReadCell () );
	
	//	없어졌을 경우
	if ( active != INVALID_ENT_REFERENCE ) {
		if ( loringlib_IsValidClient ( owner ) ) {
			resetClientViewModel ( owner );
			return Plugin_Stop;
		}
	}
	
	int weaponOwner = loringlib_GetWeaponOwner ( active );
	//	무기가 드랍된 경우
	if ( weaponOwner <= 0 ) {
		//	이전 사용한 무기 주인 뷰모델을 초기화
		if ( owner > 0 )
			resetClientViewModel ( owner );
			
		//	무기 플레이백 초기화
		SetEntPropFloat ( active, Prop_Send, "m_flPlaybackRate", 1.0 );
		return Plugin_Stop;
	}
	
	int reloadState = GetEntProp ( active, Prop_Send, "m_reloadState" );
	
	//	재장전 중일 경우
	if ( reloadState > 0 )
		return Plugin_Continue;
	
	//	재장전이 끝난 경우
	SetEntPropFloat ( active, Prop_Send, "m_flPlaybackRate", 1.0 );
	
	if ( owner > 0 )
		resetClientViewModel ( owner );
		
	return Plugin_Stop;
}

/**
 * 클라이언트 뷰모델 초기화
 */
stock void resetClientViewModel ( int client ) {
	int viewmodel = GetEntPropEnt ( client, Prop_Send, "m_hViewModel" );
	if ( viewmodel != INVALID_ENT_REFERENCE )
		SetEntPropFloat ( viewmodel, Prop_Send, "m_flPlaybackRate", 1.0 );
}