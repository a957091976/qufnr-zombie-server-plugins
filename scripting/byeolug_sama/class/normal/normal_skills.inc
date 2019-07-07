/**
 * AP 증가
 * @note 'stat.inc'의 onSpawnPost_Stat에서
 */
public void clientClass_Normal_APUpgrade ( int client ) {
	if ( ZR_IsClientZombie ( client ) )
		return;
	
	int clientClass = view_as <int> ( ClientClass_Normal );
	int lvl = g_iSkill[clientClass][SKILL_ID_AP_UPGRADE][client];
	if ( lvl < 1 )
		return;
		
	float actionvalue = g_fSkillAction[clientClass][SKILL_ID_AP_UPGRADE];
	loringlib_SetEntityArmor ( client, loringlib_GetEntityArmor ( client ) + ( lvl * RoundToZero ( actionvalue ) ) );
}
 
/**
 * 스프레이 컨트롤
 */
/*
public void clientClass_Normal_SprayControl ( int client, int skill_Id ) {
	if ( ZR_IsClientZombie ( client ) )
		return;
	
	int clientClass = view_as <int> ( ClientClass_Normal );
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
	
	float skillTime = g_fSkillAction[clientClass][skill_Id] * float ( g_iSkill[clientClass][skill_Id][client] );
	
//	loringlib_ShowProgressBar ( client, RoundToZero ( g_fSkillAction[clientClass][skill_Id] * float ( g_iSkill[clientClass][skill_Id][client] ) ) );
//	PrintToChat ( client, "skillAction %.1f", g_fSkillAction[clientClass][skill_Id] );
//	PrintToChat ( client, "clientSkillLvl %d", g_iSkill[clientClass][skill_Id][client] );
//	PrintToChat ( client, "Tot %.1f", g_fSkillAction[clientClass][skill_Id] * float ( g_iSkill[clientClass][skill_Id][client] ) );
	qufnrTools_DisplayProgressBar ( client, skillTime );
	CreateTimer ( skillTime, timerSprayControl, GetClientSerial ( client ) );
}

public Action timerSprayControl ( Handle timer, any data ) {
	int id = GetClientFromSerial ( data );
	if ( id == 0 )
		return Plugin_Stop;
	
//	loringlib_RemoveProgressBar ( id );
	
	int clientClass = view_as <int> ( ClientClass_Normal );
	g_iSkillCooldownTicks[clientClass][SKILL_ID_SPRAY_CONTROL][id] = g_iSkillCooldown[clientClass][SKILL_ID_SPRAY_CONTROL];
	g_bSkillCooldown[clientClass][SKILL_ID_SPRAY_CONTROL][id] = true;
	
	g_bSkillUse[clientClass][SKILL_ID_SPRAY_CONTROL][id] = false;
	
	return Plugin_Stop;
}	*/

/**
 * 수류탄 마스터리
 */
public void grenadeMasterlyDamage ( int edict, const char[] classname ) {
	if ( !StrEqual ( classname, "hegrenade_projectile" ) )
		return;
	
	SDKHook ( edict, SDKHook_SpawnPost, changeGrenadeDamage );
}

public void changeGrenadeDamage ( int edict ) {
	CreateTimer ( 0.1, changeGrenadeDamage_Post, edict );
}

public Action changeGrenadeDamage_Post ( Handle timer, any edict ) {
	if ( !IsValidEntity ( edict ) )
		return Plugin_Handled;
	
	float damage = GetEntPropFloat ( edict, Prop_Send, "m_flDamage" );	//	def 99.0
	float radius = GetEntPropFloat ( edict, Prop_Send, "m_DmgRadius" );	//	def 350.0
	
	int owner = loringlib_GetEntityOwner2 ( edict );
	if ( !loringlib_IsValidClient ( owner ) )
		return Plugin_Handled;
	
	int classint = view_as <int> ( ClientClass_Normal );
	if ( g_iSkill[classint][SKILL_ID_GRENADE_MASTERLY][owner] < 1 )	//	폭발 데미지
		SetEntPropFloat ( edict, Prop_Send, "m_flDamage", ( GRENADE_DAMAGE * damage ) );
	else
		SetEntPropFloat ( edict, Prop_Send, "m_flDamage", ( GRENADE_DAMAGE * damage ) * ( ROUND_DMG_PERCENTAGE + ( g_iSkill[classint][SKILL_ID_GRENADE_MASTERLY][owner] * g_fSkillAction[classint][SKILL_ID_GRENADE_MASTERLY] ) ) );
	
	if ( g_iSkill[classint][SKILL_ID_GRENADE_EXPLODE][owner] < 1 )	//	폭발 범위
		SetEntPropFloat ( edict, Prop_Send, "m_DmgRadius", radius );
	else
		SetEntPropFloat ( edict, Prop_Send, "m_DmgRadius", radius * ( ROUND_DMG_PERCENTAGE + ( g_iSkill[classint][SKILL_ID_GRENADE_EXPLODE][owner] * g_fSkillAction[classint][SKILL_ID_GRENADE_EXPLODE] ) ) );
	
//	PrintToChat ( owner, "Damages: %.3f", GetEntPropFloat ( edict, Prop_Send, "m_flDamage" ) );
//	PrintToChat ( owner, "Radius: %.3f", GetEntPropFloat ( edict, Prop_Send, "m_DmgRadius" ) );
	
	return Plugin_Handled;
}

/**
 * 피닉스 파이어레이트
 */
stock float clientClass_Normal_PhoenixFirerate ( int client ) {
	float result = 0.0;
	
	int clientClass = view_as <int> ( ClientClass_Normal );
	int phoenix_firerate_lv = g_iSkill[clientClass][SKILL_ID_PHOENIX_FIRERATE][client];
	float phoenix_firerate_action = g_fSkillAction[clientClass][SKILL_ID_PHOENIX_FIRERATE];
	if ( phoenix_firerate_lv <= 0 )
		return result;
		
	result = float ( phoenix_firerate_lv ) * phoenix_firerate_action;
	
	return result;
}