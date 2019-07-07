/**
 * 처치 시 받는 보상
 */
public void getRewardToAttacker ( int vic, int att, int assi ) {
	if ( loringlib_GetClients () <= 2 )
		return;

	if ( !loringlib_IsValidClient ( vic ) ||
		!loringlib_IsValidClient ( att ) )
		return;
		
	if ( ZR_IsClientHuman ( vic ) )
		return;
	
	if ( vic == att )
		return;
	
	if ( g_iClientClass[att] == ClientClass_Normal ) {
		if ( g_iLevel[att] >= LEVELUP_BLOCK_LEVEL ) {
			CLIENT_LEVELUP_BLOCKED_MSG ( att, LEVELUP_BLOCK_LEVEL )
			return;
		}
	}
	
	int randam = RoundFloat ( GetRandomInt ( REWARD_EXP_MIN, REWARD_EXP_MAX ) * EXP_BOOST_SCALE );
	if ( ZR_IsClientNemesis ( vic ) )
		randam = RoundFloat ( GetRandomInt ( REWARD_NEMESIS_KILL_EXP_MIN, REWARD_NEMESIS_KILL_EXP_MAX ) * EXP_BOOST_SCALE );
	
	if ( g_iTotalLevels[att] < LEVEL_NEWBIE )
		randam = RoundFloat ( randam * EXP_NEWBIE_BOOST_SCALE );
	
	g_iExp[att] += randam; CLIENT_GET_EXP_MSG ( att, randam )
	if ( loringlib_IsValidClient ( assi ) ) {	//	어시스터도 있을 경우
		if ( GetClientTeam ( att ) != GetClientTeam ( assi ) )
			return;
		
		if ( g_iClientClass[assi] == ClientClass_Normal ) {
			if ( g_iLevel[assi] >= LEVELUP_BLOCK_LEVEL ) {
				CLIENT_LEVELUP_BLOCKED_MSG ( assi, LEVELUP_BLOCK_LEVEL )
				return;
			}
		}
		
		randam = RoundFloat ( GetRandomInt ( REWARD_ASSIST_EXP_MIN, REWARD_ASSIST_EXP_MAX ) * EXP_BOOST_SCALE );
		if ( ZR_IsClientNemesis ( vic ) )
			randam = RoundFloat ( GetRandomInt ( REWARD_NEMESIS_ASSIST_EXP_MIN, REWARD_NEMESIS_ASSIST_EXP_MAX ) * EXP_BOOST_SCALE );
		
		if ( g_iTotalLevels[assi] < LEVEL_NEWBIE )
			randam = RoundFloat ( randam * EXP_NEWBIE_BOOST_SCALE );
		
		g_iExp[assi] += randam; CLIENT_ASSIST_GET_EXP_MSG ( assi, randam )
	}
}

/**
 * 좀비가 처치 시 받는 보상
 */
public void getRewardToZombie ( int victim, int attacker ) {
	if ( loringlib_GetClients () <= 2 )
		return;

	if ( !loringlib_IsValidClient ( victim ) ||
		!loringlib_IsValidClient ( attacker ) )
		return;
		
	if ( ZR_IsClientZombie ( victim ) )
		return;
	
	if ( victim == attacker )
		return;
	
	if ( g_iClientClass[attacker] == ClientClass_Normal ) {
		if ( g_iLevel[attacker] >= LEVELUP_BLOCK_LEVEL ) {
			CLIENT_LEVELUP_BLOCKED_MSG ( attacker, LEVELUP_BLOCK_LEVEL )
			return;
		}
	}
	
	int random = RoundFloat ( GetRandomInt ( REWARD_EXP_ZOMBIE_MIN, REWARD_EXP_ZOMBIE_MAX ) * EXP_BOOST_SCALE );
	g_iExp[attacker] += random;	CLIENT_GET_EXP_MSG ( attacker, random )
}

/**
 * 접속 시 피해량 초기화
 */
public void REWARD_ConnectOnResetRewardValues ( int client ) {
	g_fDamages[client] = 0.0;
	g_iSurvivePoint[client] = 0;
}
 
/**
 * 라운드 시작 시 피해량 초기화
 */
public void REWARD_RoundStartOnResetRewardValues () {
	for ( int client = 1; client <= MaxClients; client ++ ) {
		if ( loringlib_IsValidClient ( client ) ) {
			g_fDamages[client] = 0.0;
			g_iSurvivePoint[client] = 0;
		}
	}
}

/**
 * 생존 경험치 보상
 */
void roundEndOnSurviveReward ( int winner ) {
	if ( g_bBossLevel )	//	보스맵일 경우
		return;
	
	//	이긴팀이 아무도 아닐 경우
	if ( winner != CS_TEAM_CT && winner != CS_TEAM_T )
		return;
	
	if ( loringlib_GetClients () > SURVIVE_REWARD_ALLOW_PLAYERS_MIN ) {
		roundEndOnNemesisReward ();
		
		//	좀비가 2명이거나 이하일 경우 보상을 받지 않는다.
//		if ( GetTeamClientCount ( CS_TEAM_T ) <= 2 )
//			return;
		
		SURVIVE_POINT_RoundEndOnSurvivePointReward ();
		
		if ( !ZR_IsNemesisRound () ) {
			int totalExps;
			int survivetimes;
			int survive;
			int tempActPointExps;
			char info[256];
			for ( int i = 1; i <= MaxClients; i ++ ) {
				if ( loringlib_IsValidClient__PlayGame ( i ) && !IsFakeClient ( i ) ) {
					//	레벨이 블록 레벨 값이거나, 높을 경우 무시한다.
					if ( g_iClientClass[i] == ClientClass_Normal && g_iLevel[i] >= LEVELUP_BLOCK_LEVEL ) {
						CLIENT_LEVELUP_BLOCKED_MSG ( i, LEVELUP_BLOCK_LEVEL )
						continue;
					}
					
					strcopy ( info, sizeof ( info ), "" );
					totalExps = 0;	//	전체 경험치 값 초기화
					
					//	생존 시간 만큼 계산한다.
					survivetimes = RoundFloat ( g_iSurviveTime[i] / SURVIVE_TIME_EXP_DIVISION );
					survivetimes = RoundFloat ( survivetimes * EXP_BOOST_SCALE );
					tempActPointExps = 0;
					survive = 0;
					//	생존할 시
					if ( ZR_IsClientHuman ( i ) && GetClientTeam ( i ) == CS_TEAM_CT && winner == CS_TEAM_CT ) {
						survive = RoundFloat ( GetRandomInt ( REWARD_ROUND_END_SURVIVE_MIN, REWARD_ROUND_END_SURVIVE_MAX ) * EXP_BOOST_SCALE );
						g_iSurviveCounts[i] ++;
						//	newbie bonus
					//	if ( g_iTotalLevels[i] < LEVEL_NEWBIE )
					//		survive = RoundFloat ( survive * EXP_NEWBIE_BOOST_SCALE );
						
						
					//	g_iExp[i] += survive;
					//	CLIENT_ROUNDEND_SURVIVE_REWARD_MSG ( i, survive )
					}
					
					totalExps = survivetimes + survive;
					//	newbie bonus
					if ( g_iTotalLevels[i] < LEVEL_NEWBIE )
						totalExps = RoundFloat ( totalExps * EXP_NEWBIE_BOOST_SCALE );
					
					if ( g_iSurviveTime[i] > 0 )
						Format ( info, sizeof ( info ), "\x09생존 시간\x01 + " );
					if ( g_iSurvivePoint[i] > SURVIVE_POINT_CUT_LINE ) {
						Format ( info, sizeof ( info ), "%s\x06행동 점수\x01 + ", info );
						tempActPointExps = RoundFloat ( ( g_iSurvivePoint[i] * SURVIVE_POINT_EXCHANGE_EXP_SCALE ) * EXP_BOOST_SCALE );
						g_iTotalActPoints[i] += g_iSurvivePoint[i];
					} if ( survive > 0 )
						Format ( info, sizeof ( info ), "%s\x0A생존 보너스\x01", info );
					else
						Format ( info, sizeof ( info ), "%s\x08생존 못한 보너스\x01", info );
					
					PrintToChat ( i, " \x04[Reward] \x01경험치 보상을 받았습니다: %s \x05전체 %d EXP %s", info, totalExps + tempActPointExps, EXP_BOOST_SCALE > 1.0 ? "\x0EEvent↑" : "" );
					g_iExp[i] += totalExps;
				}
			}
		}
	}
	
	else {
		for ( int i = 1; i <= MaxClients; i ++ )
			if ( loringlib_IsValidClient__PlayGame ( i ) )
				if ( ZR_IsClientHuman ( i ) && GetClientTeam ( i ) == CS_TEAM_CT ) 
					CLIENT_ROUNDEND_SURVIVE_NOT_ALLOW_PLAYERS_MIN_MSG ( i, SURVIVE_REWARD_ALLOW_PLAYERS_MIN )
		return;
	}
}

/**
 * 피해량에 대한 경험치 보상
 */
/*
public void roundEndOnExpReward () {
	if ( loringlib_GetClients () > 2 ) {
		roundEndOnNemesisReward ();
		int result;
		for ( int i = 1; i <= MaxClients; i ++ ) {
			if ( loringlib_IsValidClient ( i ) ) {
				if ( g_iClientClass[i] == ClientClass_Normal ) {
					if ( g_iLevel[i] >= LEVELUP_BLOCK_LEVEL ) {
						CLIENT_LEVELUP_BLOCKED_MSG ( i, LEVELUP_BLOCK_LEVEL )
						g_fDamages[i] = 0.0;
						continue;
					}
				}
			
				result = RoundFloat ( ( g_fDamages[i] / DAMAGE_REWARD_CALCULATION ) * EXP_BOOST_SCALE );
				if ( result <= 0 ) {
					CLIENT_ROUNDEND_REWARD_ZERO_MSG ( i )
					g_fDamages[i] = 0.0;
					continue;
				}
				
				CLIENT_ROUNDEND_REWARD_MSG ( i, result )
				g_iExp[i] += result;
				g_fDamages[i] = 0.0;
			}
		}
	}
}	*/

/**
 * 네메시스 라운드 후 끝났을 때 보상
 */
public void roundEndOnNemesisReward () {
	int exps;
	int zdollars;
	char balance_name[32];
	SourceStore_GetMoneyName ( balance_name, sizeof ( balance_name ) );
	if ( ZR_IsNemesisRound () ) {
		for ( int i = 1; i <= MaxClients; i ++ ) {
			if ( loringlib_IsValidClient__PlayGame ( i ) ) {
				if ( !ZR_IsClientNemesis ( i ) ) {
					if ( g_iClientClass[i] == ClientClass_Normal && g_iLevel[i] >= LEVELUP_BLOCK_LEVEL ) {
						CLIENT_LEVELUP_BLOCKED_MSG ( i, LEVELUP_BLOCK_LEVEL )
						continue;
					}
					
					exps = RoundFloat ( GetRandomInt ( REWARD_NEMESIS_ENDROUND_ALIVE_MIN, REWARD_NEMESIS_ENDROUND_ALIVE_MAX ) * EXP_BOOST_SCALE );
					zdollars = GetRandomInt ( REWARD_NEMESIS_ENDROUND_ZDOLLAR_ALIVE_MIN, REWARD_NEMESIS_ENDROUND_ZDOLLAR_ALIVE_MAX );
					if ( g_iTotalLevels[i] < LEVEL_NEWBIE )
						exps = RoundFloat ( exps * EXP_NEWBIE_BOOST_SCALE );
					
					CLIENT_NEMESIS_ROUND_ALIVE_REWARD_MSG ( i, exps, zdollars, balance_name )
					g_iExp[i] += exps;
				//	SourceStore_SetClientBalance ( i, SourceStore_GetClientBalance ( i ) + zdollars );
					SourceStore_AddClientBalance ( i, zdollars );
				}
			}
		}
	}
}

/**
 * 행동 점수 보상
 */
void SURVIVE_POINT_RoundEndOnSurvivePointReward () {
	//	보스 맵일 경우 무시한다.
	if ( g_bBossLevel )
		return;

	int tempExp;
	
	int top[3];
	g_iSurvivePoint[0] = 0;
	for ( int i = 1; i <= MaxClients; i ++ ) {
		if ( g_iSurvivePoint[i] >= g_iSurvivePoint[top[0]] ) {
			top[2] = top[1];
			top[1] = top[0];
			top[0] = i;
		}
		
		else if ( g_iSurvivePoint[i] >= g_iSurvivePoint[top[1]] ) {
			top[2] = top[1];
			top[1] = i;
		}
		
		else if ( g_iSurvivePoint[i] >= g_iSurvivePoint[top[2]] ) {
			top[2] = i;
		}
	}
	
	char top_name[3][32];
	for ( int i = 0; i < 3; i ++ ) {
		if ( loringlib_IsValidClient ( top[i] ) )
			GetClientName ( top[i], top_name[i], sizeof ( top_name[] ) );
		else {
			strcopy ( top_name[i], sizeof ( top_name[] ), "---" );
			top[i] = 0;
		}
	}
	
//	char temp[128];
	SetHudTextParams ( -1.0, 0.3, FindConVar ( "mp_round_restart_delay" ).FloatValue, 255, 255, 255, 255 );
	for ( int i = 1; i <= MaxClients; i ++ ) {
		if ( loringlib_IsValidClient ( i ) ) {
			if ( !IsFakeClient ( i ) ) {
				if ( g_iClientClass[i] == ClientClass_Normal && g_iLevel[i] >= LEVELUP_BLOCK_LEVEL ) {
					ShowSyncHudText ( i, g_hHudSyncArray[HUDSYNC_ROUND_END_SCOREBOARD], "%d 레벨 부터는 병과가 없는 상태에서 경험치를 모을 수 없습니다.\n!hclass 명령어로 병과를 선택하세요.", LEVELUP_BLOCK_LEVEL );
					continue;
				}
				tempExp = RoundFloat ( ( g_iSurvivePoint[i] * SURVIVE_POINT_EXCHANGE_EXP_SCALE ) * EXP_BOOST_SCALE );
				
				if ( g_iTotalLevels[i] < LEVEL_NEWBIE )
					tempExp = RoundFloat ( tempExp * EXP_NEWBIE_BOOST_SCALE );
				
				if ( g_iSurvivePoint[i] > SURVIVE_POINT_CUT_LINE ) {
					if ( tempExp > SURVIVE_POINT_MAX_REWARD_EXPS )
						tempExp = SURVIVE_POINT_MAX_REWARD_EXPS;
					
				//	Format ( temp, sizeof ( temp ), "행동 점수에 대한 경험치를 획득했습니다: +%d EXP", tempExp );
				//	PrintToChat ( i, " \x04[Act Points] \x01행동 점수에 대한 경험치를 획득했습니다. \x05+ %d EXP %s", tempExp, EXP_BOOST_SCALE > 1.0 ? "\x0EEvent↑" : "" );
				
					g_iExp[i] += tempExp;
				}
				
			/*	else {
					switch ( GetRandomInt ( 0, 3 ) ) {
						case 0: Format ( temp, sizeof ( temp ), "괜찮아요, 다음에 열심히 하면되니깐요." );
						case 1: Format ( temp, sizeof ( temp ), "다음 라운드에 열심히 해봅시다!" );
						case 2: Format ( temp, sizeof ( temp ), "%N! 간바리마스!", i );
						case 3: Format ( temp, sizeof ( temp ), "아 아쉽다... 조금만 더 열심히 했다면..." );
					}
				}	*/
				
				ShowSyncHudText ( i, g_hHudSyncArray[HUDSYNC_ROUND_END_SCOREBOARD], "행동 점수 TOP3:\n1) %d - %s\n2) %d - %s\n3) %d - %s\n\n귀하는 이번 라운드에서 %d만큼의 행동 점수를 획득했습니다.",
					g_iSurvivePoint[top[0]], top_name[0],
					g_iSurvivePoint[top[1]], top_name[1],
					g_iSurvivePoint[top[2]], top_name[2],
					g_iSurvivePoint[i] );
			}
		}
	}
}

/*
public void takeDamageOnExpReward ( int victim, int attacker, float dmg ) {
	if ( loringlib_GetClients () <= 2 )
		return;

	if ( ZR_IsClientZombie ( attacker ) )
		return;
		
	if ( GetClientTeam ( victim ) == GetClientTeam ( attacker ) )
		return;
	
	float result = g_fDamages[attacker] / DAMAGE_REWARD_CALCULATION;
	if ( result >= 1.0 ) {
		g_iExp[attacker] += RoundFloat ( result );
		g_fDamages[attacker] = 0.0;
	}
}	*/