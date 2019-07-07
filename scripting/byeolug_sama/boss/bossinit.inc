
/**
 * @section 보스 맵 목록
 */
static char g_strBossLevel[1][128] = {
	"flat"
};

int g_iBossDefineTime = 0;			/**< 보스 등장 시간 변수 */
Handle g_hBossDefineTimer = null;		/**< 보스 등장 타이머 */

/**
 * 보스 맵인지 채크한다.
 */
public void BOSS_MapStartOnCheckBossLevel () {
	static char levelName[128];
	GetCurrentMap ( levelName, sizeof ( levelName ) );
	
	g_bBossLevel = false;
	for ( int i = 0; i < sizeof ( g_strBossLevel ); i ++ ) {
		if ( StrEqual ( levelName, g_strBossLevel[i] ) ) {
			PrintToServer ( "[ZD BOSS] !!! \"%s\" is boss level !!!", levelName );
			g_bBossLevel = true;
		}
	}
}

public void BOSS_RoundStartOnBossSpawnWait () {
	if ( g_bBossLevel ) {
		if ( g_hBossDefineTimer != null ) {
			g_hBossDefineTimer = null;
			KillTimer ( g_hBossDefineTimer );
		}
		
		g_iBossDefineTime = BOSS_DEFINE_TIME;
		g_hBossDefineTimer = CreateTimer ( 1.0, Boss_TimerOnBossDefineStart, _, TIMER_REPEAT );
	}
}

public Action Boss_TimerOnBossDefineStart ( Handle timer ) {
	if ( g_iBossDefineTime >= 0 ) {
		SetHudTextParams ( -1.0, 0.35, 1.1, 200, 0, 0, 255, 0, 0.0, 0.0, 0.0 );
		loringlib_ShowSyncHudTextToAll ( g_hHudSyncArray[HUDSYNC_PLAYER_STATE_MSG2], "%d", g_iBossDefineTime );
	}
	
	g_iBossDefineTime --;
	
	if ( g_iBossDefineTime <= 0 ) {
		//	To do
		Boss_DefineEndOnSpawnBoss ();
		
		g_hBossDefineTimer = null;
		return Plugin_Stop;
	}
	
	return Plugin_Handled;
}

public void Boss_DefineEndOnSpawnBoss () {
	int entity = CreateEntityByName ( "prop_dynamic" );
	if ( entity == -1 ) {
		PrintToChatAll ( " \x07[Error] \x01ERROR: Undefined entity classname." );
		return;
	}
	
	DispatchKeyValue ( entity, "model", "models/monster/morbus/morbus_pestis.mdl" );
	DispatchKeyValue ( entity, "spawnflags", "256" );
	DispatchKeyValue ( entity, "Solid", "6" );
	DispatchKeyValue ( entity, "DefaultAnim", "idle" );
	
	DispatchSpawn ( entity );
	
	static float tempSpawnOrigin[3];
	tempSpawnOrigin[0] = 155.0;
	tempSpawnOrigin[1] = 46.0;
	tempSpawnOrigin[2] = 75.0;
	TeleportEntity ( entity, tempSpawnOrigin, NULL_VECTOR, NULL_VECTOR );
	
	SetEntProp ( entity, Prop_Data, "m_takedamage", 2, 1 );
	SetEntProp ( entity, Prop_Data, "m_iHealth", 1000 );
	SDKHook ( entity, SDKHook_OnTakeDamage, BOSS_OnTakeDamage );
	
	tempSpawnOrigin[2] += 20.0;
	static float dir[3];
	TE_SetupSparks ( tempSpawnOrigin, dir, 100, 100 );
	TE_SendToAll ();
	
	CreateTimer ( 10.0, timerBossFirstCombo, entity, TIMER_FLAG_NO_MAPCHANGE | TIMER_REPEAT );
}

public Action BOSS_OnTakeDamage ( int entity, int& attacker, int& inflictor, float& damage, int& damagetype, int& weapon, float damageForce[3], float damagePosition[3], int damagecustom ) {
	if ( IsValidEdict ( entity ) && loringlib_IsValidClient ( attacker ) ) {
		PrintHintText ( attacker, "<font color='#ff0000'>BOSS HEALTH</font>: %d%%", loringlib_GetEntityHealth ( entity ) - RoundFloat ( damage ) );
		
		//	Boss died
		if ( loringlib_GetEntityHealth ( entity ) - RoundFloat ( damage ) <= 0 ) {
			int ragdoll = CreateEntityByName ( "prop_physics_multiplayer" );
			
			DispatchKeyValue ( ragdoll, "model", "models/monster/morbus/morbus_pestis.mdl" );
			DispatchKeyValue ( ragdoll, "physicsmode", "3" );
			DispatchKeyValue ( ragdoll, "spawnflags", "2" );
			
			static float ragdollOrigin[3];
			loringlib_GetEntityOriginEx ( entity, ragdollOrigin );
			TeleportEntity ( ragdoll, ragdollOrigin, NULL_VECTOR, NULL_VECTOR );
		}
	}
	
	return Plugin_Continue;
}

public Action timerBossFirstCombo ( Handle timer, any entity ) {
	if ( IsValidEdict ( entity ) ) {
		SetVariantString ( "morbus_pestis_attack_hand_shot" );
		AcceptEntityInput ( entity, "SetAnimation" );
		
		CreateTimer ( 1.8, timerBossFirstComboAttack, entity );
	}
	
	return Plugin_Continue;
}

public Action timerBossFirstComboAttack ( Handle timer, any entity ) {
	if ( IsValidEdict ( entity ) ) {
		static float origin[2][3];
		loringlib_GetEntityOriginEx ( entity, origin[0] );
		for ( int i = 1; i <= MaxClients; i ++ ) {
			if ( loringlib_IsValidClient__PlayGame ( i ) ) {
				GetClientAbsOrigin ( i, origin[1] );
				if ( GetVectorDistance ( origin[0], origin[1] ) <= 300 ) {
					loringlib_MakeDamage ( entity, i, GetRandomInt ( 10, 20 ), "world", DMG_SLASH );
					loringlib_ShowFadeUserMessageEx ( i, 200, 1000, FFADE_AUTO, { 255, 0, 0, 190 } );
					loringlib_ShowShakeMessage ( i, 50.0, 2.0 );
				}
				
				else if ( GetVectorDistance ( origin[0], origin[1] ) <= 600 ) {
					loringlib_MakeDamage ( entity, i, GetRandomInt ( 2, 5 ), "world", DMG_SLASH );
					loringlib_ShowFadeUserMessageEx ( i, 100, 800, FFADE_AUTO, { 255, 0, 0, 128 } );
					loringlib_ShowShakeMessage ( i, 30.0, 2.5 );
				}
				
				else {
					loringlib_ShowShakeMessage ( i, 20.0, 1.0 );
				}
			}
		}
	}
	
	return Plugin_Stop;
}