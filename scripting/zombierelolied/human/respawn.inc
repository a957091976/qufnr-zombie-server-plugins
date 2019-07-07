/**
 * Human re-spawn functions.
 */
public void humanRespawn ( int client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return;
	
	if ( IsPlayerAlive ( client ) )
		return;
	
	if ( g_bZombie[client] )
		return;
		
	//	Now nemesis time.
	if ( g_bNemesisRound )
		return;
	
	//	좀비 선택 도중 사망 시 게임을 다시 시작함.
	if ( !g_bGameStart && !g_bGameEnd ) {
		if ( loringlib_GetTeamCount ( CS_TEAM_CT, true ) <= 1 ) {
			if ( g_hInfectStartCooldown != null ) {
				KillTimer ( g_hInfectStartCooldown );
				g_hInfectStartCooldown = null;
			}
			
			CS_TerminateRound ( 5.0, CSRoundEnd_Draw, true );
			return;
		}
	}
	
	//	시간 설정
	g_fHumanRespawnTime[client] = g_fConfHumanRespawnTime;
	
	//	좀비 스폰 타이머가 움직이고 있다면 삭제함.
	if ( g_hZombieRespawnCooldown[client] != null ) {
		KillTimer ( g_hZombieRespawnCooldown[client] );
		g_hZombieRespawnCooldown[client] = null;
	}
	
	//	타이머 생성
	CreateTimer ( g_fHumanRespawnTime[client], timerHumanRespawnCooldown, client );
}

public Action timerHumanRespawnCooldown ( Handle timer, int client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Stop;
	
	if ( g_bGameEnd )
		return Plugin_Stop;
	
	CS_RespawnPlayer ( client );
	
	/*
	//	게임이 시작되지 않은 상태
	if ( !g_bGameStart ) {
		//	인간으로 스폰
		CS_RespawnPlayer ( client );
		changeDefaultToHuman ( client );
	}
	
	//	게임이 시작된 상태
	else {
		//	스폰
		CS_RespawnPlayer ( client );
		infectHumanToZombie ( client );
	}
	*/
	
	return Plugin_Stop;
}