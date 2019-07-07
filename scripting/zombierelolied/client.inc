/**
 * Connect to change client variable values.
 */
public void connectToChangeClientValue ( int client ) {
	g_bClientGamePlayer[client] = false;	//	클라이언트 게임 참가 여부
	g_bZombie[client] = false;				//	클라이언트 좀비 여부
	g_bMotherZombie[client] = false;		//	클라이언트 숙주 좀비 여부
	g_bNemesis[client] = false;
	
	g_iClientActions[CLIENT_ACTION_KILL][client] = 0;
	g_iClientActions[CLIENT_ACTION_DEATH][client] = 0;
	g_iClientActions[CLIENT_ACTION_SCORE][client] = 0;
	
	loringlib_ClearVectors ( g_fSpawnPosition[client] );	//	클라이언트 스폰 백터 위치 삭제
	
	SDKHook ( client, SDKHook_OnTakeDamageAlive, onInfectHumans );	//	감염 hook
	SDKHook ( client, SDKHook_OnTakeDamage, onTakeDamage );		//	데미지 hook
//	SDKHook ( client, SDKHook_OnTakeDamage, onZombieKnockback );	//	넉백 hook
	SDKHook ( client, SDKHook_PostThinkPost, onPostThinkPost );	//	포스트 팅크
}

/**
 * Disconnect to clear client variable values.
 */
public void disconnectToClearClientValue ( int client ) {
	SDKUnhook ( client, SDKHook_OnTakeDamageAlive, onInfectHumans );	//	감염 unhook
	SDKUnhook ( client, SDKHook_OnTakeDamage, onTakeDamage );		//	데미지 unhook
//	SDKUnhook ( client, SDKHook_OnTakeDamage, onZombieKnockback );	//	넉백 unhook
	SDKUnhook ( client, SDKHook_PostThinkPost, onPostThinkPost );	//	포스트 팅크
}

/**
 * Post think post.
 */
public void onPostThinkPost ( int client ) {
	zombieRegargeHealthLooping ( client );
	
	loringlib_SetCSKillPoints ( client, g_iClientActions[CLIENT_ACTION_KILL][client] );
	loringlib_SetCSDeathPoints ( client, g_iClientActions[CLIENT_ACTION_DEATH][client] );
	CS_SetClientContributionScore ( client, g_iClientActions[CLIENT_ACTION_SCORE][client] );
}

/**
 * Spawn to set client position vectors.
 */
public void setSpawnPosition ( int client ) {
	if ( !IsPlayerAlive ( client ) )
		return;
		
	GetClientAbsOrigin ( client, g_fSpawnPosition[client] );
}

/**
 * On select client team.
 *//*
public Action onSelectClientTeam ( int client, int args ) {
	if ( IsPlayerAlive ( client ) )
		return Plugin_Handled;
	
	if ( g_bZombie[client] )
		return Plugin_Handled;
	
	CS_SwitchTeam ( client, CS_TEAM_CT );
	ServerCommand ( "bot_quota 0" );
	
	return Plugin_Handled;
}
*/

/**
 * 클라이언트가 나가고 서버에 아무도 없을 경우 게임을 재시작 합니다.
 */
public void disconnectToGameRestart ( int client ) {
	if ( !IsClientInGame ( client ) )
		return;
		
	if ( GetClientTeam ( client ) == CS_TEAM_NONE ||
		GetClientTeam ( client ) == CS_TEAM_SPECTATOR )
		return;
		
	if ( loringlib_GetTeamCount ( GetClientTeam ( client ) ) <= 1 ) {
		if ( !g_bGameStart )
			return;
		
		CS_TerminateRound ( 3.0, CSRoundEnd_Draw, true );
	}
}

/**
 * 인간 좀비킬 처리
 */
public void killTheZombie ( int victim, int attacker ) {
	if ( !loringlib_IsValidClient ( victim ) ||
		!loringlib_IsValidClient ( attacker ) )
		return;
		
	if ( victim == attacker )
		return;
		
	if ( GetClientTeam ( victim ) == GetClientTeam ( attacker ) )
		return;
	
	if ( g_bZombie[attacker] )
		return;
		
	g_iClientActions[CLIENT_ACTION_KILL][attacker] ++;
	g_iClientActions[CLIENT_ACTION_SCORE][attacker] += 2;
	g_iClientActions[CLIENT_ACTION_DEATH][victim] ++;
}