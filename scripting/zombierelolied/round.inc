Handle g_hRoundEndTimer = null;

/**
 * 스코어 보드에 승리 수를 설정합니다.
 */
public void displayRoundWinCountInScoreboard () {
	CS_SetTeamScore ( CS_TEAM_T, g_iZombieWinCount );
	CS_SetTeamScore ( CS_TEAM_CT, g_iHumanWinCount );
}

/**
 * 라운드 승리 수를 초기화 합니다.
 */
public void clearRoundWinCounts () {
	g_iZombieWinCount = 0;
	g_iHumanWinCount = 0;
}
 
/**
 * Round Start pre settings.
 */
public void roundStartOnPreSettings () {
	g_bGameEnd = false;
	g_bGameStart = false;
	g_bGameWaitPlayer = false;
}
 
/**
 * The round is starting.
 */
public void roundEndOnRoundStart () {
	if ( g_hRoundEndTimer != null ) {
		KillTimer ( g_hRoundEndTimer );
		g_hRoundEndTimer = null;
	}
}

/**
 * 라운드 종료
 */
public void roundEndOnRoundEnd () {
	if ( g_hRoundEndTimer != null ) {
		KillTimer ( g_hRoundEndTimer );
		g_hRoundEndTimer = null;
	}
}

/**
 * The freeze time is ending.
 */
public void roundEndOnFreezeEnd () {
	float time = GetConVarFloat ( FindConVar ( "mp_roundtime" ) );
	time *= 60.0;
	
	if ( GetEngineVersion () == Engine_CSGO )
		time --;
	
	g_hRoundEndTimer = CreateTimer ( time, timerRoundEndTimer, _, TIMER_FLAG_NO_MAPCHANGE );
}

/**
 * Timer callback, called when round time reaches 0.
 *
 * @param timer			The timer handler.
 */
public Action timerRoundEndTimer ( Handle timer ) {
	g_hRoundEndTimer = null;
	if ( loringlib_GetTeamCount ( CS_TEAM_CT ) == 0 &&
		loringlib_GetTeamCount ( CS_TEAM_T ) == 0 )
		return Plugin_Stop;
		
	if ( !g_bGameStart || g_bGameEnd )
		return Plugin_Stop;
	
	CS_TerminateRound ( FindConVar ( "mp_round_restart_delay" ).FloatValue, CSRoundEnd_CTWin, true );
	return Plugin_Stop;
}

/**
 * 라운드 비길 시 진영 상황을 통해 승리 진영을 설정합니다.
 */ /*
public Action CS_OnTerminateRound ( float &delay, CSRoundEndReason &reason ) {
	if ( !g_bGameStart )
		return Plugin_Continue;

	//	defause/bomb(de_) 또는 hostage rescue(cs_) 맵에서 승리 진영 처리
	if ( loringlib_IsMapDE_ () || loringlib_IsMapCS_ () ) {
		//	인간 1명 이상 생존, 좀비 1명 이상 있을 경우
		if ( loringlib_GetTeamCount ( CS_TEAM_CT, true ) >= 1 &&
			loringlib_GetTeamCount ( CS_TEAM_T ) >= 1 ) {
			reason = CSRoundEnd_CTWin;	//	대테러부대 승리
			g_iHumanWinCount ++;
			
			return Plugin_Changed;
		}
		
		//	좀비 1명 이상, 인간 0명 이하일 경우
		else if ( loringlib_GetTeamCount ( CS_TEAM_T, true ) >= 1 &&
			loringlib_GetTeamCount ( CS_TEAM_CT, true ) <= 0 ) {
			reason = CSRoundEnd_TerroristWin;	//	테러리스트 승리
			g_iZombieWinCount ++;
			
			return Plugin_Changed;
		}
		
		return Plugin_Handled;
	}
	
	//	라운드 비길 시 인원 수로 승리 진영을 선택 함.
	if ( reason & CSRoundEnd_Draw ) {
		if ( loringlib_GetTeamCount ( CS_TEAM_CT, true ) >= 1 &&
			loringlib_GetTeamCount ( CS_TEAM_T ) >= 1 ) {
			reason = CSRoundEnd_CTWin;
			g_iHumanWinCount ++;
			
			return Plugin_Changed;
		}
		
		else if ( loringlib_GetTeamCount ( CS_TEAM_T, true ) >= 1 &&
			loringlib_GetTeamCount ( CS_TEAM_CT, true ) <= 0 ) {
			reason = CSRoundEnd_TerroristWin;
			g_iZombieWinCount ++;
			
			return Plugin_Changed;
		}
	}
	
	return Plugin_Continue;
}	*/