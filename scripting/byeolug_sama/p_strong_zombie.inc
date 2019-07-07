#define		STRONGZOMBIE_MAX_STRONG_GAUGE	100.0	/**< Strong Gauge 최대 값 */
#define		STRONGZOMBIE_GAUGE_CHARGE_DAMAGE_SCALE	1250	/**< Strong Gauge 충전되는 데미지 량 */
#define		STRONGZOMBIE_GAUGE_CHARGE_SCALE		1.0			/**< Strong Gauge 충전되는 량 */
#define		STRONGZOMBIE_GAUGE_CHARGE_SCALE_BY_DEATH	5.0	/**< 죽었을 때 충전되는 량 */
#define		STRONGZOMBIE_ACTIVITY_SOUND			"qufnr/zombie_department/strong_zombie_activity.mp3"

#define		CLIENT_STRONGZOMBIE_UNLOCK_SKILLS_MSG(%1)		PrintToChat ( %1, " \x04[Mother Zombie] \x01숙주좀비의 기술이 더욱 더 강해졌습니다!" );
#define		CLIENT_STRONGZOMBIE_ACTIVITY_MSG				PrintToChatAll ( " \x04[Mother Zombie] \x01모든 숙주좀비가 \x09강화된 숙주좀비\x01로 변했습니다!" );
#define		CLIENT_STRONGZOMBIE_YOU_ARE_ACTIVITY_MSG(%1)	PrintToChat ( %1, " \x04[Mother Zombie] \x01귀하는 이제 \x09강화된 숙주좀비\x01입니다!" );

#define		STRONGZOMBIE_MAX_HEALTH		25000	/**< Max health */
#define		STRONGZOMBIE_KNOCKBACK_SCALE	1.3	/**< Knockback scale. */

bool g_bAliveStrongZombie = false;
bool g_bIsStrongMotherZombie[MAXPLAYERS + 1] = { false, ... };
Handle g_hStrongZombieTimer = null;

char g_strStrongZombieModels[3][256] = {
	"models/player/custom_player/kodua/wb1/wb1.mdl"
	, "models/player/custom_player/kodua/wb2/wb2.mdl"
	, "models/player/custom_player/kodua/wb3/wb3.mdl"
};

/**
 * 라운드 시작 시 강화 좀비 데이터 초기화
 * @note RoundStart에서
 */
void STRONGZOMBIE_RoundStartOnResetStrongZombieData () {
	g_bAliveStrongZombie = false;
	for ( int i = 0; i < sizeof ( g_bIsStrongMotherZombie ); i ++ )
		g_bIsStrongMotherZombie[i] = false;
		
	if ( g_hStrongZombieTimer != null ) {
		KillTimer ( g_hStrongZombieTimer );
		g_hStrongZombieTimer = null;
	}
}

/**
 * 라운드 종료 시 타이머 삭제
 */
void STRONGZOMBIE_RoundEndOnMotherZombieCheckTimerKill () {
	if ( g_hStrongZombieTimer != null ) {
		KillTimer ( g_hStrongZombieTimer );
		g_hStrongZombieTimer = null;
	}
}

/**
 * 라운드 시작 시 강숙 등장 타이머 생성
 * @note freeze_end
 */
void STRONGZOMBIE_RoundStartOnStrongZombieCheckTimer () {
//	float time = 30.0;
	float time = FindConVar ( "mp_roundtime" ).FloatValue;
	time *= 60.0;
	time = time - ( time / 4.0 );
	
	time --;
	
	if ( time < 58.0 )
		return;
	
	g_hStrongZombieTimer = CreateTimer ( time, timerStrongZombieTimer, _, TIMER_FLAG_NO_MAPCHANGE );
}

public Action timerStrongZombieTimer ( Handle timer ) {
	g_hStrongZombieTimer = null;
	
	if ( ZR_IsGameEnd () || !ZR_IsGameStart () )
		return Plugin_Stop;
				
	//	좀비 수가 인간 수와 같거나 더 많을 경우 타이머 삭제
	if ( loringlib_GetTeamCount ( CS_TEAM_CT ) <= loringlib_GetTeamCount ( CS_TEAM_T ) )
		return Plugin_Stop;
	
	if ( !g_bAliveStrongZombie )
		StrongZombie_FindStrongZombie ();
	
	return Plugin_Stop;
}

/**
 * 강화 숙주 좀비로 바꾼다.
 */
void STRONGZOMBIE_ChangeStrongZombie ( int client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return;
	
	g_bIsStrongMotherZombie[client] = true;
	g_iZombieUpgrade[client] = ZBUY_ZOMBIE_UPGRADE_MAX;
	
	if ( !IsPlayerAlive ( client ) )
		return;
	
	switch ( ZR_GetClientZombieClass ( client ) ) {
		case 0, 1, 2:	{
			PrecacheModel ( g_strStrongZombieModels[ZR_GetClientZombieClass ( client )] );
			SetEntityModel ( client, g_strStrongZombieModels[ZR_GetClientZombieClass ( client )] );
		}
		
		default: {
			int randomModel = GetRandomInt ( 1, sizeof ( g_strStrongZombieModels ) ) - 1;
			PrecacheModel ( g_strStrongZombieModels[randomModel] );
			SetEntityModel ( client, g_strStrongZombieModels[randomModel] );
		}
	}
	setClientSkinGlow ( client, 1 );
	loringlib_EffectLightDynamic ( _, "255 0 0 255", 300.0, 350.0, 999.0, true, client );
	
	int color[4] = { 255, 0, 0, 25 };
	loringlib_ShowFadeUserMessage ( client, 5, 1, color );
	
	loringlib_SetEntityMaxHealth ( client, STRONGZOMBIE_MAX_HEALTH );
	loringlib_SetEntityHealth ( client, STRONGZOMBIE_MAX_HEALTH );
	
	ZR_SetClientKnockbackScale ( client, ZR_GetClientZombieType ( client ), ZR_GetClientKnockbackScale ( client, ZR_GetClientZombieType ( client ) ) + STRONGZOMBIE_KNOCKBACK_SCALE );

	float eye[3];
	GetClientAbsOrigin ( client, eye );
	eye[2] -= 10.0;
	EmitAmbientSound ( SOUND_STRONGZOMBIE_SPAWN_IDLE, eye, client );
}

/**
 * 강화 숙주 스폰 시
 */
void STRONGZOMBIE_SpawnOnChangeStrongZombie ( int client ) {
	if ( loringlib_IsValidClient__PlayGame ( client ) )
		if ( ZR_IsClientMotherZombie ( client ) )
			CreateTimer ( 0.2, timerChangeStrongZombiePost, client, TIMER_FLAG_NO_MAPCHANGE );
}

public Action timerChangeStrongZombiePost ( Handle timer, any client ) {
	if ( !loringlib_IsValidClient__PlayGame ( client ) )
		return Plugin_Stop;
	
	if ( ZR_IsGameEnd () || !ZR_IsGameStart () )
		return Plugin_Stop;
	
	if ( g_bIsStrongMotherZombie[client] )
		STRONGZOMBIE_ChangeStrongZombie ( client );
	return Plugin_Stop;
}

/**
 * 숙주 좀비를 강화 숙주 좀비로 바꾼다.
 */
stock void StrongZombie_FindStrongZombie () {
	if ( StrongZombie_GetMotherZombieCounts () <= 0 )
		return;
	
	if ( !ZR_IsGameStart () || ZR_IsGameEnd () )
		return;
	
	bool find = false;
	for ( int target = 1; target <= MaxClients; target ++ ) {
		if ( loringlib_IsValidClient ( target ) ) {
			if ( ZR_IsClientMotherZombie ( target ) ) {
				if ( !g_bIsStrongMotherZombie[target] ) {
					CLIENT_STRONGZOMBIE_YOU_ARE_ACTIVITY_MSG ( target )
					
					find = true;
					
					if ( IsPlayerAlive ( target ) )
						STRONGZOMBIE_ChangeStrongZombie ( target );
					else {
						g_bIsStrongMotherZombie[target] = true;
					}
				}
			}
		}
	}
	
	if ( find )	{
		g_bAliveStrongZombie = true;
		CLIENT_STRONGZOMBIE_ACTIVITY_MSG
	}
}

stock int StrongZombie_GetMotherZombieCounts () {
	int result = 0;
	for ( int i = 1; i <= MaxClients; i ++ )
		if ( loringlib_IsValidClient ( i ) )
			if ( ZR_IsClientMotherZombie ( i ) )
				result ++;
	return result;
}

stock int StrongZombie_GetStrongZombie () {
	for ( int i = 1; i <= MaxClients; i ++ )
		if ( loringlib_IsValidClient ( i ) )
			if ( g_bIsStrongMotherZombie[i] )
				return i;
	
	return 0;
}