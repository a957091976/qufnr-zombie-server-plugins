/**
 * Infect to human.
 */
public Action onInfectHumans ( int victim, int &attacker, int &inflictor, float &dmg, int &dmgType, int &weaponidx, float damageForce[3], float damagePosition[3] ) {
	if ( !loringlib_IsValidClient ( victim ) || !loringlib_IsValidClient ( attacker ) )
		return Plugin_Continue;
		
	//	클라이언트 생존 여부 체크
	if ( !IsPlayerAlive ( attacker ) || !IsPlayerAlive ( victim ) )
		return Plugin_Continue;
	
	//	맞는 사람 인간 체크
	if ( GetClientTeam ( victim ) != CS_TEAM_CT || g_bZombie[victim] )
		return Plugin_Continue;
		
	//	때리는 사람 인간 체크
	if ( !g_bZombie[attacker] && !g_bNemesis[attacker] )
		return Plugin_Continue;
	
	//	같은 팀일 경우 체크
	if ( GetClientTeam ( attacker ) == GetClientTeam ( victim ) )
		return Plugin_Continue;
	
	//	라운드 상황 체크
	if ( !g_bGameStart || g_bGameEnd )
		return Plugin_Handled;
		
	//	데미지 타입 체크
	if ( dmgType & DMG_BLAST ||
		dmgType & DMG_BURN ||
		dmgType & DMG_BULLET ||
		dmgType & 128 )
		return Plugin_Handled;
	
	//	밑에서 부터 감염 관련 함수들 입니다.

	//	네메시스 감염
	if ( g_bNemesis[attacker] ) {
		if ( weaponidx != -1 ) {
			char weapon[32];
			GetEdictClassname ( weaponidx, weapon, sizeof ( weapon ) );
			if ( StrContains ( weapon, "knife", false ) != -1 ) {
				dmg *= 5.0;
				if ( loringlib_GetTeamCount ( CS_TEAM_CT, true ) == 1 ) {
					//	Health over.
					if ( dmg >= loringlib_GetEntityHealth ( victim ) ) {
						CS_TerminateRound ( 5.0, CSRoundEnd_TerroristWin, true );	//	Nemesis win
						g_iZombieWinCount ++;
					}
				}
				
				g_iClientActions[CLIENT_ACTION_KILL][attacker] ++;
				g_iClientActions[CLIENT_ACTION_SCORE][attacker] += 2;
				g_iClientActions[CLIENT_ACTION_DEATH][victim] ++;
				
				return Plugin_Changed;
			}
		}
	}
	
	//	좀비 감염
	else {
		//	마지막 남은 생존자를 죽였을 때
		if ( loringlib_GetTeamCount ( CS_TEAM_CT, true ) == 1 ) {
			//	체력이 다 닳았을 때
			if ( dmg >= loringlib_GetEntityHealth ( victim ) ) {
				loringlib_CreateFakeCallDeathEvent ( victim, attacker, "prop_exploding_barrel", false );
				g_iClientActions[CLIENT_ACTION_KILL][attacker] ++;
				g_iClientActions[CLIENT_ACTION_SCORE][attacker] += 2;
				g_iClientActions[CLIENT_ACTION_DEATH][victim] ++;
				
				infectHumanToZombie ( victim );
				
				CS_TerminateRound ( 5.0, CSRoundEnd_TerroristWin, true );	//	좀비 승리 처리
				g_iZombieWinCount ++;
				
				return Plugin_Handled;
			}
		}
		
		else {
			//	숙주 좀비가 생존자를 죽였을 때
			if ( g_bMotherZombie[attacker] ) {
				//	데미지 타입이 쇼크일 경우
				if ( dmgType & DMG_SHOCK ) {
					//	체력이 모두 소진 되야 사망합니다.
					if ( dmg >= loringlib_GetEntityHealth ( victim ) ) {
						loringlib_CreateFakeCallDeathEvent ( victim, attacker, "prop_exploding_barrel", false );
						g_iClientActions[CLIENT_ACTION_KILL][attacker] ++;
						g_iClientActions[CLIENT_ACTION_SCORE][attacker] += 2;
						g_iClientActions[CLIENT_ACTION_DEATH][victim] ++;
						
						infectHumanToZombie ( victim );
						
						return Plugin_Handled;
					} else return Plugin_Continue;
				}
				
				dmg = 0.0;
				loringlib_CreateFakeCallDeathEvent ( victim, attacker, "prop_exploding_barrel", false );
				g_iClientActions[CLIENT_ACTION_KILL][attacker] ++;
				g_iClientActions[CLIENT_ACTION_SCORE][attacker] += 2;
				g_iClientActions[CLIENT_ACTION_DEATH][victim] ++;
				
				infectHumanToZombie ( victim );
				
				return Plugin_Changed;
			}
			
			//	일반 좀비가 생존자를 죽였을 때
			if ( g_bZombie[attacker] && !g_bMotherZombie[attacker] ) {
				if ( dmg >= loringlib_GetEntityHealth ( victim ) ) {
					dmg = 0.0;
					
					loringlib_CreateFakeCallDeathEvent ( victim, attacker, "prop_exploding_barrel", false );
					g_iClientActions[CLIENT_ACTION_KILL][attacker] ++;
					g_iClientActions[CLIENT_ACTION_SCORE][attacker] += 2;
					g_iClientActions[CLIENT_ACTION_DEATH][victim] ++;
				
					infectHumanToZombie ( victim );
					
					return Plugin_Changed;
				}
			}
		}
	}
	
	return Plugin_Continue;
}