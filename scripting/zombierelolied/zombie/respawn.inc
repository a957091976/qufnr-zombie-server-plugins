/**
 * Zombie re-spawn function.
 */
public void zombieRespawn ( int client, int attacker ) {
	if ( !loringlib_IsValidClient ( client ) )
		return;
	
	if ( IsPlayerAlive ( client ) )
		return;
	
	if ( !g_bZombie[client] )
		return;
	
	//	Suicide
	if ( !loringlib_IsValidClient ( attacker ) ) {
		if ( g_bMotherZombie[client] ) {
			//	숙주 좀비 자살
			PrintToChat ( client, "[ZR] 귀하는 숙주 좀비 자살로 인해, 다음 라운드에도 숙주 좀비로 선택될 수 있습니다." );
			g_bSafeHuman[client] = false;
		}
	}
		
	//	시간 설정
	g_fZombieRespawnTime[client] = g_fConfZombieRespawnTime;
	
	//	타이머가 이미 움직이고 있다면 삭제함.
	if ( g_hZombieRespawnCooldown[client] != null ) {
		KillTimer ( g_hZombieRespawnCooldown[client] );
		g_hZombieRespawnCooldown[client] = null;
	}
	
	//	리스폰 타이머 생성
	if ( g_hZombieRespawnCooldown[client] == null )
		g_hZombieRespawnCooldown[client] = CreateTimer ( 0.1, timerRespawnCooldown, client, TIMER_REPEAT );
}

/**
 * Respawn check repeat timer.
 */
public Action timerRespawnCooldown ( Handle timer, int client ) {
	//	클라이언트가 유효하지 않거나, 좀비가 아니라면 타이머를 삭제함.
	if ( !loringlib_IsValidClient ( client ) ||
		!g_bZombie[client] ) {
		g_hZombieRespawnCooldown[client] = null;
		
		return Plugin_Stop;
	}
	
	//	게임이 끝났거나, 게임이 시작되지 않았을 때
	if ( g_bGameEnd || !g_bGameStart ) {
		g_hZombieRespawnCooldown[client] = null;
		
		return Plugin_Stop;
	}
	
	//	리스폰 쿨다운 채크
	if ( g_fZombieRespawnTime[client] > 0.0 ) {
		g_fZombieRespawnTime[client] -= 0.1;
		SetHudTextParams ( -1.0, 0.8, 1.1, 255, 255, 255, 255, 0, 0.1, 0.1, 0.1 );
		ShowHudText ( client, ZR_ZTELE_INFO_CHANNEL, "%.1f초 후 리스폰 됩니다.", g_fZombieRespawnTime[client] );
	}
	
	//	리스폰
	if ( g_fZombieRespawnTime[client] <= 0.1 ) {
		CS_RespawnPlayer ( client );	//	리스폰
		
		SetHudTextParams ( -1.0, 0.8, 1.1, 255, 255, 255, 255, 0, 0.1, 0.1, 0.1 );
		ShowHudText ( client, ZR_ZTELE_INFO_CHANNEL, "스폰 됨." );
		
		/*
		//	클라이언트 좀비 역할에 따라 스폰 설정
		if ( g_bMotherZombie[client] )	infectHumanToMotherZombie ( client );
		else	infectHumanToZombie ( client );
		*/
		
		g_hZombieRespawnCooldown[client] = null;
		return Plugin_Stop;
	}
	
	return Plugin_Handled;
}

