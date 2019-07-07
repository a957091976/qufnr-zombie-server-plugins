public void defineNeedStatpoints () {
	g_iNeedStatPoint[STAT_STR] = STAT_NEED_SP_STR;
	g_iNeedStatPoint[STAT_DEF] = STAT_NEED_SP_DEF;
	g_iNeedStatPoint[STAT_AGL] = STAT_NEED_SP_AGL;
}

public void defineStatMaxlvl () {
	g_iStatMaxlvl[STAT_STR] = STAT_MAXLVL_STR;
	g_iStatMaxlvl[STAT_DEF] = STAT_MAXLVL_DEF;
	g_iStatMaxlvl[STAT_AGL] = STAT_MAXLVL_AGL;
}

public void clientStatHooks ( int client, bool hook ) {
	if ( hook ) {
		SDKHook ( client, SDKHook_SpawnPost, onSpawnPost_Stats );
		return;
	}
	
	SDKUnhook ( client, SDKHook_SpawnPost, onSpawnPost_Stats );
}

public Action clientCommandStat ( int client, int args ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Handled;
		
	Menu menu = new Menu ( statMenuHandler );
	menu.SetTitle ( "%s\n\t%s\n\t\tStat Points: %d SP\n　", MENU_TITLE, MENU_STAT_TITLE, g_iStatPoint[client] );
	static char info[256];
	static char statName[32];
	static int totalStatOption;
	for ( int i = 0; i < MAXSTATS; i ++ ) {
		getStatName ( i, statName, sizeof ( statName ) );
//		PrintToServer ( "%s MAXLVL %d", statName[i], g_iStatMaxlvl[i] );

		totalStatOption = 0;
		for ( int x = 0; x < EQUIPITEM_MAX_SLOTS; x ++ )
			if ( g_fEquipBonusStat[x][i][client] > 0.0 )
				totalStatOption += RoundFloat ( g_fEquipBonusStat[x][i][client] );

		if ( totalStatOption > 0 ) {
			Format ( info, sizeof ( info ), "%s: %d (%d+%d)%s", statName, g_iStats[i][client] + totalStatOption, g_iStats[i][client], totalStatOption, g_iStatMaxlvl[i] <= g_iStats[i][client] ? " [MAX]" : "" );
		} else
			Format ( info, sizeof ( info ), "%s: %d%s", statName, g_iStats[i][client], g_iStatMaxlvl[i] <= g_iStats[i][client] ? " [MAX]" : "" );
		menu.AddItem ( "", info, g_iStatPoint[client] >= g_iNeedStatPoint[i] ? ( g_iStatMaxlvl[i] <= g_iStats[i][client] ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT ) : ITEMDRAW_DISABLED );
	}
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
	return Plugin_Handled;
}

public int statMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	if ( !loringlib_IsValidClient ( param1 ) )
		delete menu;
		
	switch ( action ) {
		case MenuAction_Select: {
			if ( g_iStatPoint[param1] < g_iNeedStatPoint[param2] ) {
				CLIENT_NEED_MORE_STATPOINTS ( param1 )
				return;
			}
			
			g_iStatPoint[param1] --;
			g_iStats[param2][param1] ++;
			clientCommandStat ( param1, 0 );
		}
		case MenuAction_Cancel: {
			switch ( param2 ) {
				case MenuCancel_ExitBack: {
					clientClassStatMenu ( param1 );
					return;
				}
			}
		}
	}
}

/**
 * 방어력 및 민첩성 연산
 */
public void onSpawnPost_Stats ( int client ) {
	CreateTimer ( 0.3, timerOnSpawnPostStats, client, TIMER_FLAG_NO_MAPCHANGE );
}

public Action timerOnSpawnPostStats ( Handle timer, int client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Stop;
	
	clientClass_Normal_APUpgrade ( client );
	clientClass_Medic_HPUpgrade ( client );
	
	int totalStatOption[2] = { 0, ... };
	for ( int x = 0; x < EQUIPITEM_MAX_SLOTS; x ++ ) {
		if ( g_fEquipBonusStat[x][EQUIPMENT_BONUS_STAT_DEF][client] > 0.0 )
			totalStatOption[0] += RoundFloat ( g_fEquipBonusStat[x][EQUIPMENT_BONUS_STAT_DEF][client] );
		if ( g_fEquipBonusStat[x][EQUIPMENT_BONUS_STAT_AGL][client] > 0.0 )
			totalStatOption[1] += RoundFloat ( g_fEquipBonusStat[x][EQUIPMENT_BONUS_STAT_AGL][client] );
	}
	
	int statDef = g_iStats[STAT_DEF][client] + totalStatOption[0];
	int statAgl = g_iStats[STAT_AGL][client] + totalStatOption[1];
	if ( statDef > 0 ) {
		if ( !ZR_IsClientHuman ( client ) ) {
			loringlib_SetEntityArmor ( client, loringlib_GetEntityArmor ( client ) + ( STAT_DEF_VALUE_BY_ZOMBIE * statDef ) );
		}
		
		else {
			loringlib_SetEntityArmor ( client, loringlib_GetEntityArmor ( client ) + ( STAT_DEF_VALUE * statDef ) );
		}
	}
	
	if ( statAgl > 0 ) {
		if ( ZR_IsClientHuman ( client ) ) {
			loringlib_SetEntityLaggedmovement ( client, 1.0 + ( ( STAT_AGL_VALUE * statAgl ) / 2.3 ) );
			loringlib_SetEntityGravity ( client, 1.0 - ( ( STAT_AGL_VALUE * statAgl ) / 2.5 ) );
		}
	}
	
//	PrintToChatAll ( "%N AGL: %d", client, statAgl );
//	PrintToChatAll ( "%N DEF: %d", client, statDef );
	
	return Plugin_Stop;
}

/**
 * 좀비 방어력 연산
 */
void STAT_OnZombieArmorChange ( int client ) {
	int totalStatOption = 0;
	for ( int x = 0; x < EQUIPITEM_MAX_SLOTS; x ++ ) {
		if ( g_fEquipBonusStat[x][EQUIPMENT_BONUS_STAT_DEF][client] > 0.0 )
			totalStatOption += RoundFloat ( g_fEquipBonusStat[x][EQUIPMENT_BONUS_STAT_DEF][client] );
	}

	int statDef = g_iStats[STAT_DEF][client] + totalStatOption;
	if ( statDef > 0 )
		if ( ZR_IsClientZombie ( client ) )
			loringlib_SetEntityArmor ( client, loringlib_GetEntityArmor ( client ) + ( STAT_DEF_VALUE_BY_ZOMBIE * statDef ) );
}

/**
 * 스텟이 최대치에 넘어 버렸을 경우, 스텟을 최대치로 설정하고 나머지 스탯은 포인트로 돌려준다.
 */
void STAT_StatOverflowCheck ( int client ) {
	CreateTimer ( 0.5, timerStatOverflowCheck, client );
}

public Action timerStatOverflowCheck ( Handle timer, any client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Stop;
	
	for ( int i = 0; i < MAXSTATS; i ++ ) {
		if ( g_iStats[i][client] > g_iStatMaxlvl[i] ) {
			PrintToChat ( client, "%s \x01귀하의 스텟이 잘못 연산되어 Stat Points로 바뀌었습니다. [%d 번째 스텟 | 변경된 값: %d | Stat Points: %d -> %d SP]", MESSAGE_PREFIX, i, g_iStatMaxlvl[i], g_iStatPoint[client], g_iStatPoint[client] + ( ( g_iNeedStatPoint[i] * g_iStats[i][client] ) - g_iStatMaxlvl[i] ) );
			g_iStatPoint[client] += ( ( g_iNeedStatPoint[i] * g_iStats[i][client] ) - g_iStatMaxlvl[i] );
			g_iStats[i][client] = g_iStatMaxlvl[i];
		}
	}
	
	return Plugin_Stop;
}

/**
 * 공격 스텟 값을 구한다.
 *
 * @param client
 * @param damage		TakeDamage Hook에서 피해량 값
 */
stock float STAT_GetStatSTRResult ( int client, float damage ) {
	int totalStatOption = 0;
	for ( int x = 0; x < EQUIPITEM_MAX_SLOTS; x ++ ) {
		if ( g_fEquipBonusStat[x][EQUIPMENT_BONUS_STAT_STR][client] > 0.0 )
			totalStatOption += RoundFloat ( g_fEquipBonusStat[x][EQUIPMENT_BONUS_STAT_STR][client] );
	}
	
	int strStat = g_iStats[STAT_STR][client] + totalStatOption;
	if ( strStat > 0 )
		return damage * ( 1.0 + ( ( strStat ) * STAT_STR_VALUE ) );
	
	return damage;
}