#define	CONVERSION_FEET_TO_UNITS		0.0625
#define NO_TELEPORT		0

#define ZTELE_MSG_X		-1.0
#define ZTELE_MSG_Y		0.444

float g_fTempClientPosition[MAXPLAYERS + 1][3];

public void spawnSetupZtele ( int client ) {
	g_iTeleport[client] = NO_TELEPORT;
	
	//	Ztele 갯수
	g_iZteleCounts[client][CS_TEAM_CT] = g_iConfHumanZteleCount;
	g_iZteleCounts[client][CS_TEAM_T] = g_iConfZombieZteleCount;
	
	//	Ztele 쿨 다운
	g_fZteleCooldown[client][CS_TEAM_CT] = g_fConfHumanZteleCooldown;
	g_fZteleCooldown[client][CS_TEAM_T] = g_fConfZombieZteleCooldown;
	
	g_hZteleCooldown[client] = null;
/*
	if ( g_hZteleCooldown[client] != null ) {
		KillTimer ( g_hZteleCooldown[client] );
		g_hZteleCooldown[client] = null;
	}	*/
}

public Action onClientCommandZtele ( int client, int args ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Stop;
		
	if ( !IsPlayerAlive ( client ) ) {
		PrintToChat ( client, "[ZR] 이 명령어는 살아있는 상태에서 사용할 수 있습니다." );
		return Plugin_Stop;
	}
	
	//	좀비 상태에서 사용했을 경우
	if ( g_bZombie[client] ) {
		//	Config에서 사용할 수 없게 해놨을 경우
		if ( g_iConfZteleCanUseZombie != 1 ) {
			PrintToChat ( client, "[ZR] 관리자에 의해 사용할 수 없게 되어 있습니다." );
			return Plugin_Stop;
		}
	}
	
	//	인간 상태에서 사용했을 경우
	if ( !g_bZombie[client] ) {
		//	Config에서 사용할 수 없게 해놨을 경우
		if ( g_iConfZteleCanUseHuman != 1 ) {
			PrintToChat ( client, "[ZR] 관리자에 의해 사용할 수 없게 되어 있습니다." );
			return Plugin_Stop;
		}
	}
	
	//	게임이 끝났을 경우
	if ( g_bGameEnd ) {
		PrintToChat ( client, "[ZR] 게임이 시작된 상태에서 사용할 수 있습니다." );
		return Plugin_Stop;
	}
	
	//	Ztele을 모두 사용했을 경우
	if ( g_iZteleCounts[client][GetClientTeam ( client )] <= 0 ) {
		PrintToChat ( client, "[ZR] Ztele은 최대 %d번 사용할 수 있습니다.", GetClientTeam ( client ) == CS_TEAM_CT ? g_iConfHumanZteleCount : GetClientTeam ( client ) == CS_TEAM_T ? g_iConfZombieZteleCount : -1 );
		return Plugin_Stop;
	}
	
	//	텔레포트 중일 경우
	if ( g_iTeleport[client] != NO_TELEPORT ) {
		PrintToChat ( client, "[ZR] 텔레포트 중 입니다... 잠시만 기다려주세요." );
		return Plugin_Stop;
	}
	
	g_iTeleport[client] = GetClientTeam ( client );	//	텔레포트 중으로 바꿉니다.
	GetClientAbsOrigin ( client, g_fTempClientPosition[client] );	//	커멘드를 사용한 위치를 구하고,
	//	쿨 다운 변수를 설정합니다.
	g_fZteleCooldown[client][CS_TEAM_CT] = g_fConfHumanZteleCooldown;
	g_fZteleCooldown[client][CS_TEAM_T] = g_fConfZombieZteleCooldown;
//	g_fZteleCooldown[client][GetClientTeam ( client )] = ( GetClientTeam ( client ) == CS_TEAM_CT ? g_fConfHumanZteleCooldown : GetClientTeam ( client ) == CS_TEAM_T ? g_fConfZombieZteleCooldown : -1 );
	
	//	타이머가 돌아간다면 죽입니다.
	if ( g_hZteleCooldown[client] != null ) {
		KillTimer ( g_hZteleCooldown[client] );
		g_hZteleCooldown[client] = null;
	}
	
	//	죽인 후 null이 되면 타이머를 돌립니다.
	if ( g_hZteleCooldown[client] == null ) {
		g_hZteleCooldown[client] = CreateTimer ( 0.1, timerZteleRepeatChecker, client, TIMER_REPEAT );
		
		Call_StartForward ( g_hForwardHandlers[ZR_FORWARD_ON_CLIENT_ZTELE_STARTED] );
		Call_PushCell ( client );
		Call_Finish ();
	}
	
	return Plugin_Continue;
}

public Action timerZteleRepeatChecker ( Handle timer, int client ) {
	if ( !loringlib_IsValidClient__PlayGame ( client ) )
		return Plugin_Stop;

	SetHudTextParams ( ZTELE_MSG_X, ZTELE_MSG_Y, 0.1, 255, 255, 255, 255, 0, 0.0, 0.0, 0.0 );
	ShowHudText ( client, ZR_ZTELE_INFO_CHANNEL, "%.1f초 후 텔레포트 됩니다.", g_fZteleCooldown[client][GetClientTeam ( client )] );
	
	if ( g_fZteleCooldown[client][GetClientTeam ( client )] >= 0.0 )
		g_fZteleCooldown[client][GetClientTeam ( client )] -= 0.1;
	
	//	게임이 종료된 경우이거나, 텔레포트 도중 감염되었을 경우
	if ( g_bGameEnd ||
		g_iTeleport[client] != GetClientTeam ( client ) ) {
		SetHudTextParams ( ZTELE_MSG_X, ZTELE_MSG_Y, 0.1, 255, 255, 255, 255, 0, 0.0, 0.0, 0.0 );
		ShowHudText ( client, ZR_ZTELE_INFO_CHANNEL, "텔레포트가 취소되었습니다." );
		
		//	텔레포트를 취소합니다.
		g_iTeleport[client] = NO_TELEPORT;
		g_fZteleCooldown[client][CS_TEAM_CT] = g_fConfHumanZteleCooldown;
		g_fZteleCooldown[client][CS_TEAM_T] = g_fConfZombieZteleCooldown;
//		g_fZteleCooldown[client][GetClientTeam ( client )] = ( GetClientTeam ( client ) == CS_TEAM_CT ? g_fConfHumanZteleCooldown : GetClientTeam ( client ) == CS_TEAM_T ? g_fConfZombieZteleCooldown : -1 );
		
		PrintToChat ( client, "[ZR] 텔레포트가 취소되었습니다." );
		
		g_hZteleCooldown[client] = null;
		return Plugin_Stop;
	}
	
	float vec[3];
	GetClientAbsOrigin ( client, vec );
	
	float distance = GetVectorDistance ( vec, g_fTempClientPosition[client] );
	float cancelDistance = loringlib_ConvertUnits ( g_fConfZteleAutoCancelDistance, CONVERSION_FEET_TO_UNITS );
	
	//	설정된 자동 취소 거리보다 멀리있으면 텔레포트를 취소합니다.
	if ( distance > cancelDistance ) {
		SetHudTextParams ( ZTELE_MSG_X, ZTELE_MSG_Y, 3.0, 255, 255, 255, 255, 0, 0.0, 0.0, 0.0 );
		ShowHudText ( client, ZR_ZTELE_INFO_CHANNEL, "텔레포트가 취소되었습니다. (%.2f ft.)", cancelDistance );
		PrintToChat ( client, "[ZR] 텔레포트가 취소되었습니다. (%.2f ft.)", cancelDistance );
		
		g_iTeleport[client] = NO_TELEPORT;
		g_fZteleCooldown[client][CS_TEAM_CT] = g_fConfHumanZteleCooldown;
		g_fZteleCooldown[client][CS_TEAM_T] = g_fConfZombieZteleCooldown;
//		g_fZteleCooldown[client][GetClientTeam ( client )] = ( GetClientTeam ( client ) == CS_TEAM_CT ? g_fConfHumanZteleCooldown : GetClientTeam ( client ) == CS_TEAM_T ? g_fConfZombieZteleCooldown : -1 );
		
		g_hZteleCooldown[client] = null;
		return Plugin_Stop;
	}
	
	//	Successfully
	if ( g_fZteleCooldown[client][GetClientTeam ( client )] <= 0.0 ) {
		g_iTeleport[client] = NO_TELEPORT;
		g_iZteleCounts[client][GetClientTeam ( client )] --;	//	Ztele 횟수 차감
		g_fZteleCooldown[client][CS_TEAM_CT] = g_fConfHumanZteleCooldown;
		g_fZteleCooldown[client][CS_TEAM_T] = g_fConfZombieZteleCooldown;
//		g_fZteleCooldown[client][GetClientTeam ( client )] = ( GetClientTeam ( client ) == CS_TEAM_CT ? g_fConfHumanZteleCooldown : GetClientTeam ( client ) == CS_TEAM_T ? g_fConfZombieZteleCooldown : -1 );
		

		SetHudTextParams ( ZTELE_MSG_X, ZTELE_MSG_Y, 3.0, 255, 255, 255, 255, 0, 0.0, 0.0, 0.0 );
		ShowHudText ( client, ZR_ZTELE_INFO_CHANNEL, "스폰지역으로 텔레포트했습니다. (남은 횟수: %d)", g_iZteleCounts[client][GetClientTeam ( client )] );
		PrintToChat ( client, "[ZR] 스폰지역으로 텔레포트했습니다. (남은 횟수: %d)", g_iZteleCounts[client][GetClientTeam ( client )] );
		
		TeleportEntity ( client, g_fSpawnPosition[client], NULL_VECTOR, NULL_VECTOR_FLY_BLOCK );
		
		Call_StartForward ( g_hForwardHandlers[ZR_FORWARD_ON_CLIENT_ZTELE_TELEPORTED] );
		Call_PushCell ( client );
		Call_Finish ();
		
		g_hZteleCooldown[client] = null;
		return Plugin_Stop;
	}
	
	return Plugin_Continue;
}