/**
 * 라운드 상황을 봅니다.
 */
public Action clientCommandRound ( int client, int args ) {
	if ( client == 0 ) {
		PrintToServer ( "[Round Level] Current Round Level: %d RL", g_iRoundLevel );
		PrintToServer ( "[Round Level] Difficulty: %s (Debug: %d)", g_strRoundLevelNameArray[g_iDifficulty][1], g_iDifficulty );
	}
	
	else if ( loringlib_IsValidClient ( client ) ) {
		PrintToChatAll ( " \x03[Round Level] \x01현재 라운드 레벨: %d RL", g_iRoundLevel );
		PrintToChatAll ( " \x03[Round Level] \x01난이도: %s", g_strRoundLevelNameArray[g_iDifficulty][1] );
	}
}

/**
 * 맵 로드 및 라운드 시작 시 라운드 레벨을 계산한다.
 */
public void ROUNDLEVEL_StartOnCalcRoundLevel () {
	if ( g_iRoundLevel <= 0 )	//	First plugin running
		g_iRoundLevel = ROUNDLEVEL_DEFAULT_RLEVEL;

	for ( int i = 0; i < sizeof ( g_strRoundLevelNameArray ); i ++ ) {
		if ( ( i == sizeof ( g_strRoundLevelNameArray ) - 1 ) ? g_iRoundLevel >= StringToInt ( g_strRoundLevelNameArray[i][0] ) : g_iRoundLevel >= StringToInt ( g_strRoundLevelNameArray[i][0] ) && g_iRoundLevel < StringToInt ( g_strRoundLevelNameArray[i + 1][0] ) ) {
			g_iDifficulty = i;
			return;
		}
	}
}

/**
 * 라운드 시작 시 라운드 레벨 계산을 적용시킨다.
 */
public void ROUNDLEVEL_RoundStartOnSetRoundLevel () {
	ROUNDLEVEL_StartOnCalcRoundLevel ();
	
	SetHudTextParamsEx ( -1.0, 0.385, 3.0, { 222, 80, 80, 255 }, { 196, 180, 60, 255 }, 2, 0.25, 0.03, 0.1 );
	loringlib_ShowSyncHudTextToAll ( g_hHudSyncArray[HUDSYNC_ROUNDLEVEL_ALERT], "ROUND LEVEL: %d RL\nDIFFICULTY: %s", g_iRoundLevel, g_strRoundLevelNameArray[g_iDifficulty][1] );
}

/**
 * 라운드 종료 시 라운드 레벨 계산
 * @note round_end
 */
public void ROUNDLEVEL_RoundEndOnCalcRoundLevel ( int winnerTeam ) {
	//	무승부일 경우?
	if ( winnerTeam != ZR_HUMAN_TEAM && winnerTeam != ZR_ZOMBIE_TEAM )
		return;
		
	//	인간이 승리할 경우?
	if ( winnerTeam == ZR_HUMAN_TEAM ) {
		g_iRoundLevel ++;
		PrintToChatAll ( " \x03[Round Level] \x01라운드 레벨이 상승 했습니다. \x06%d → %d", g_iRoundLevel - 1, g_iRoundLevel );
		
		if ( g_iRoundLevel % ROUNDLEVEL_CHANGE_LEVELNAME_RLEVEL == 0 ) {
			for ( int i = 1; i < sizeof ( g_strRoundLevelNameArray ); i ++ ) {
				if ( g_iRoundLevel == StringToInt ( g_strRoundLevelNameArray[i][0] ) ) {
					g_iDifficulty = i;
					PrintToChatAll ( " \x03[Round Level] \x01난이도가 %s\x01(으)로 상승 했습니다.", g_strRoundLevelNameArray[g_iDifficulty][2], g_strRoundLevelNameArray[g_iDifficulty][1] );
					return;
				}
			}
		}
	}
	
	//	좀비가 승리할 경우?
	if ( winnerTeam == ZR_ZOMBIE_TEAM ) {
		if ( g_iRoundLevel <= 0 ) {
			g_iRoundLevel = 0;
			return;
		}
		
		g_iRoundLevel --;
		PrintToChatAll ( " \x03[Round Level] \x01라운드 레벨이 하락 했습니다. \x06%d → %d", g_iRoundLevel + 1, g_iRoundLevel );
		
		if ( g_iRoundLevel % ROUNDLEVEL_CHANGE_LEVELNAME_RLEVEL == 0 ) {
			for ( int i = 1; i < sizeof ( g_strRoundLevelNameArray ); i ++ ) {
				if ( g_iRoundLevel + 1 == StringToInt ( g_strRoundLevelNameArray[i][0] ) ) {
					g_iDifficulty --;
					PrintToChatAll ( " \x03[Round Level] \x01난이도가 %s%s\x01(으)로 하락 했습니다.", g_strRoundLevelNameArray[g_iDifficulty][2], g_strRoundLevelNameArray[g_iDifficulty][1] );
					return;
				}
			}
		}
	}
}