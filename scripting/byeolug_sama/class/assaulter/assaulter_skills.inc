#define SOUND_ASSAULT_BREAKLIMIT_USE		"qufnr/assault/assault_breaklimit.mp3"	/**< 브레이크 리미트 사용 시 소리 */
#define SOUND_ASSAULT_ENCOURAGE				"qufnr/assault/assault_encourage.mp3"		/**< 격려 받을 때 소리 */
#define SOUND_ASSAULT_ENCOURAGE_EFFECT		"qufnr/assault/assault_encourage_effect.mp3"	/**< 격려 사용 시 소리 */
#define SOUND_ASSAULT_HERO_ENCOURAGE		"qufnr/assault/assault_hero_encourage.mp3"	/**< 히어로 받을 때 소리 */

#define ASSAULT_ENCOURAGE_TIME				5.0			/**< 격려 시간 */
#define ASSAULT_ENCOURAGE_DISTANCE			250.0		/**< 격려 거리 */

#define ASSAULT_LEADERSHIP_DISTANCE			200.0		/**< 리더쉽 거리 */
#define ASSAULT_LEADERSHIP_NORMAL_SCALE		0.1			/**< 리더쉽 기본 값 */

#define ASSAULT_AMMO_CHARGE_NORMAL_CHANCE	10.0		/**< 애모차지 기본 확률(찬스) */
#define ASSAULT_OVERHIT_TIME				0.5			/**< 오버히트 반복 시간 간격 */

static float g_fAssaultOverhitDamage[MAXPLAYERS + 1] = { 0.0, ... };

static int g_iAssaultEncourageTime[MAXPLAYERS + 1] = { 0, ... };

static bool g_bInEncourageEffect[MAXPLAYERS + 1][MAXPLAYERS + 1];
static float g_fEncourageDamage[MAXPLAYERS + 1][MAXPLAYERS + 1];

static float g_fLeadershipReloadSpeed[MAXPLAYERS + 1][MAXPLAYERS + 1];

static float g_fHeroReloadSpeed[MAXPLAYERS + 1][MAXPLAYERS + 1];
static float g_fHeroFirerateSpeed[MAXPLAYERS + 1][MAXPLAYERS + 1];

/**
 * 돌격병 변수 초기화
 * @note player_spawn에서
 */
void clientClass_Assault_ClearAssaultData ( int client ) {
	g_fAssaultOverhitDamage[client] = 0.0;
	g_iAssaultEncourageTime[client] = 0;
	for ( int i = 0; i < MAXPLAYERS; i ++ ) {
		g_fEncourageDamage[client][i] = 0.0;
		g_bInEncourageEffect[client][i] = false;
		g_fLeadershipReloadSpeed[client][i] = 0.0;
		
		g_fHeroReloadSpeed[client][i] = 0.0;
		g_fHeroFirerateSpeed[client][i] = 0.0;
	}
}

/**
 * AR 숙련 (전 어썰트 마스터리)
 */
stock float clientClass_Assault_AssaultMasterly ( int client, float dmg, int weapon ) {
	if ( g_iClientClass[client] != ClientClass_Assault )
		return dmg;
	
	int clientClassInt = view_as <int> ( ClientClass_Assault );
	if ( g_iSkill[clientClassInt][SKILL_ID_ASSAULT_MASTERY][client] < 1 )
		return dmg;

	if ( weapon != -1 ) {
		static char edictname[32];
		GetEdictClassname ( weapon, edictname, sizeof ( edictname ) );
		for ( int i = 0; i < sizeof ( g_strClassWeapon_Assault ); i ++ )
			if ( StrContains ( g_strClassWeapon_Assault[i], edictname, false ) != -1 )
				return dmg * ( ROUND_DMG_PERCENTAGE + ( g_iSkill[clientClassInt][SKILL_ID_ASSAULT_MASTERY][client] * g_fSkillAction[clientClassInt][SKILL_ID_ASSAULT_MASTERY] ) );
	}
	
	return dmg;
}

/**
 * 리더쉽 발동
 * @note globalTimer에서
 */
public void Assault_GlobalTimerOnLeadershipPassive ( int client ) {
	if ( loringlib_IsValidClient ( client ) ) {
		if ( IsPlayerAlive ( client ) && ZR_IsClientHuman ( client ) ) {
			//	돌격병일 경우
			if ( g_iClientClass[client] == ClientClass_Assault ) {
				int clientClass = view_as<int> ( g_iClientClass[client] );
				int leadership_lv = g_iSkill[clientClass][SKILL_ID_LEADERSHIP][client];
				//	리더쉽을 배웠을 경우
				if ( leadership_lv > 0 ) {
					float eye[3];
					GetClientEyePosition ( client, eye );
					for ( int target = 1; target <= MaxClients; target ++ ) {
						if ( loringlib_IsValidClient__PlayGame ( target ) ) {
							//	타겟이 인간일 경우
							if ( ZR_IsClientHuman ( target ) ) {
								//	타겟이 거리안에 있거나, 타겟이 주인일 경우
								if ( loringlib_GetNearTrue ( client, target, eye, ASSAULT_LEADERSHIP_DISTANCE ) ||
									target == client ) {
									g_fLeadershipReloadSpeed[target][client] = ASSAULT_LEADERSHIP_NORMAL_SCALE + float ( g_iSkill[clientClass][SKILL_ID_LEADERSHIP][client] ) * g_fSkillAction[clientClass][SKILL_ID_LEADERSHIP];
								}
								
								//	거리 밖일 경우
								else {
									g_fLeadershipReloadSpeed[target][client] = 0.0;
								}
							}
							
							//	좀비일 경우
							else {
								g_fLeadershipReloadSpeed[target][client] = 0.0;
							}
							
						//	PrintCenterText ( target, "Leadership Reload SPD: %.4f", g_fLeadershipReloadSpeed[target][client] );
						}
					}
					
					return;
				}
			}
			
			//	리더쉽을 안배웠을 경우 장전속도 초기화
			for ( int i = 1; i <= MaxClients; i ++ )
				if ( loringlib_IsValidClient ( i ) )
					g_fLeadershipReloadSpeed[i][client] = 0.0;
		}
	}
}

/**
 * 리더쉽 장전속도 구하기
 */
stock float Assault_GetLeadershipReloadSpeed ( int client ) {
	int result[MAXPLAYERS + 1] = { 1, ... };
	
	for ( int i = 0; i < sizeof ( result ); i ++ )
		for ( int j = 0; j < sizeof ( result ); j ++ )
			if ( g_fLeadershipReloadSpeed[client][i] < g_fLeadershipReloadSpeed[client][j] )
				++result[i];
	
	for ( int i = 0; i < sizeof ( result ); i ++ ) {
		if ( result[i] == 1 ) {	//	버프 받은것들 중 가장 큰 값일 경우
			if ( g_fLeadershipReloadSpeed[client][i] <= 0.0 )	//	가장 큰 값이 0이다?
				return 0.0;
			
			return g_fLeadershipReloadSpeed[client][i];
		}
	}
	
	return 0.0;
}

/**
 * 격려
 */
void clientClass_Assault_Encourage ( int client, int skill ) {
	//	보급병이 아닐 경우
	if ( g_iClientClass[client] != ClientClass_Assault )
		return;
	
	//	좀비일 경우
	if ( ZR_IsClientZombie ( client ) )
		return;
	
	int clientClass = view_as <int> ( g_iClientClass[client] );
	if ( g_bSkillUse[clientClass][skill][client] ) {
		CLIENT_SKILL_NOT_AVAILABLE_NOW_USING_MSG ( client )
		return;
	}
	
	if ( g_bSkillCooldown[clientClass][skill][client] ) {
		CLIENT_SKILL_NOT_AVAILABLE_COOLDOWN_MSG ( client )
		return;
	}
	
	int consume_ap = g_iSkillConsumeAP[clientClass][skill];
	if ( consume_ap > loringlib_GetEntityArmor ( client ) ) {
		CLIENT_SKILL_NOT_AVAILABLE_NEED_AP_MSG ( client )
		return;
	}
	
	g_bSkillUse[clientClass][skill][client] = true;
	loringlib_SetEntityArmor ( client, loringlib_GetEntityArmor ( client ) - consume_ap );
	CLIENT_SKILL_USE_MSG ( client, g_strSkillName[clientClass][skill] )
	
	float pos[3];
	GetClientAbsOrigin ( client, pos );
	pos[2] += 10.0;
	PrecacheSound ( SOUND_ASSAULT_ENCOURAGE );
	EmitAmbientSound ( SOUND_ASSAULT_ENCOURAGE, pos, client );
	
	GetClientEyePosition ( client, pos );
	pos[2] -= 20.0;
	
	//	히어로 이펙트
	if ( g_iSkill[clientClass][SKILL_ID_HERO][client] > 0 )
		loringlib_CreateParticle ( client, 0, pos, "assault_encourage2", true, 5.0 );
	else	//	격려 이펙트
		loringlib_CreateParticle ( client, 0, pos, "assault_encourage", true, 5.0 );
	
	CreateTimer ( 0.0, timerEncourageTimer, client, TIMER_FLAG_NO_MAPCHANGE );
	CreateTimer ( 1.0, timerEncourageTimer, client, TIMER_REPEAT | TIMER_FLAG_NO_MAPCHANGE );
}

public Action timerEncourageTimer ( Handle timer, any client ) {
	if ( client > 0 ) {
		
		//	돌격병일 경우
		if ( g_iClientClass[client] == ClientClass_Assault ) {
			int clientClass = view_as<int> ( g_iClientClass[client] );
			int encourage_lv = g_iSkill[clientClass][SKILL_ID_ENCOURAGE][client];
			//	격려를 배웠을 경우
			if ( encourage_lv > 0 ) {
			
				if ( g_iAssaultEncourageTime[client] < 0 )
					g_iAssaultEncourageTime[client] = 0;
				
				g_iAssaultEncourageTime[client] ++;	//	횟수 증가
				//	횟수가 초과하거나, 주인이 좀비가 됬을 경우 타이머 제거
				if ( g_iAssaultEncourageTime[client] > RoundFloat ( ASSAULT_ENCOURAGE_TIME ) ||
					ZR_IsClientZombie ( client ) ) {
					for ( int clients = 1; clients <= MaxClients; clients ++ ) {
						//	시간이 끝났을 경우 모든 타겟에 대한 변수 초기화
						if ( loringlib_IsValidClient ( clients ) ) {
							g_fEncourageDamage[clients][client] = 0.0;
							g_fHeroReloadSpeed[clients][client] = 0.0;
							g_fHeroFirerateSpeed[clients][client] = 0.0;
						//	g_bInEncourageEffect[clients][client] = false;
						}
					}
					
					g_iSkillCooldownTicks[clientClass][SKILL_ID_ENCOURAGE][client] = g_iSkillCooldown[clientClass][SKILL_ID_ENCOURAGE];
					g_bSkillCooldown[clientClass][SKILL_ID_ENCOURAGE][client] = true;
					
					g_bSkillUse[clientClass][SKILL_ID_ENCOURAGE][client] = false;
					
					g_iAssaultEncourageTime[client] = 0;
					return Plugin_Stop;
				}
			
				loringlib_DisplayNearTrueTriggerByEntity ( client, ASSAULT_ENCOURAGE_DISTANCE, 0.5, 8.0, 5.5, { 60, 180, 200, 255 }, true );
				
				for ( int target = 1; target <= MaxClients; target ++ ) {
					if ( loringlib_IsValidClient__PlayGame ( target ) ) {
						//	인간일 경우
						if ( ZR_IsClientHuman ( target ) ) {
							//	거리안에 있거나, 타겟이 자신일 경우?
							if ( loringlib_IsTargetInSightRange ( client, target, 360.0, ASSAULT_ENCOURAGE_DISTANCE ) ||
								target == client ) {
								
								if ( IsTargetPlayerActivityGamePlay ( client, target ) )
									g_iSurvivePoint[client] += 1;	//	행동 점수
								
							//	g_bInEncourageEffect[target][client] = true;
								g_fEncourageDamage[target][client] = g_fSkillAction[clientClass][SKILL_ID_ENCOURAGE];
								//	격려 마스터리를 배웠을 경우 데미지 값을 올린다.
								if ( g_iSkill[clientClass][SKILL_ID_ENCOURAGE_MASTERY][client] > 0 )
									g_fEncourageDamage[target][client] = g_fSkillAction[clientClass][SKILL_ID_ENCOURAGE] + ( float ( g_iSkill[clientClass][SKILL_ID_ENCOURAGE_MASTERY][client] ) * g_fSkillAction[clientClass][SKILL_ID_ENCOURAGE_MASTERY] );
								
								static float origin[3];
								GetClientAbsOrigin ( target, origin );
								origin[2] += 5.0;
								
								//	히어로를 배웠을 경우
								if ( g_iSkill[clientClass][SKILL_ID_HERO][client] > 0 ) {
									g_fEncourageDamage[target][client] = g_fSkillAction[clientClass][SKILL_ID_ENCOURAGE] + ( float ( g_iSkill[clientClass][SKILL_ID_ENCOURAGE_MASTERY][client] ) * g_fSkillAction[clientClass][SKILL_ID_ENCOURAGE_MASTERY] ) + ( g_fSkillAction[clientClass][SKILL_ID_HERO] );
									g_fHeroReloadSpeed[target][client] = g_fSkillAction[clientClass][SKILL_ID_HERO];
									g_fHeroFirerateSpeed[target][client] = g_fSkillAction[clientClass][SKILL_ID_HERO];
									
									//	히어로 이펙트
									loringlib_CreateParticle ( target, 0, origin, "assault_encourage2_recieve", true, 0.5 );
								}
								
								else	//	격려 이펙트
									loringlib_CreateParticle ( target, 0, origin, "assault_encourage_recieve", true, 0.5 );
									
								
							//	PrintToChat ( target, "encourage damage: %f", g_fEncourageDamage[target][client] );
							}
							
							//	거리 밖에 있을 경우
							else {
							//	g_bInEncourageEffect[target][client] = false;
								g_fEncourageDamage[target][client] = 0.0;
								g_fHeroReloadSpeed[target][client] = 0.0;
								g_fHeroFirerateSpeed[target][client] = 0.0;
							}
						}
						
						//	좀비일 경우(인간이 아닐 경우)
						else {
							g_fEncourageDamage[target][client] = 0.0;
							g_fHeroReloadSpeed[target][client] = 0.0;
							g_fHeroFirerateSpeed[target][client] = 0.0;
						}
					}
				}
				
				return Plugin_Continue;
			}
		}
		
		//	돌격병이 아니거나, 격려를 배우지 않았을 경우
		if ( g_iClientClass[client] != ClientClass_Assault ||
			g_iSkill[view_as<int> ( ClientClass_Assault )][SKILL_ID_ENCOURAGE][client] ) {
			//	타겟의 모든 버프 변수를 초기화한다.
			for ( int i = 1; i <= MaxClients; i ++ ) {
				if ( loringlib_IsValidClient ( i ) ) {
					g_fEncourageDamage[i][client] = 0.0;
					g_fHeroReloadSpeed[i][client] = 0.0;
					g_fHeroFirerateSpeed[i][client] = 0.0;
				}
			}
		}
	}
	
	return Plugin_Stop;
}

/**
 * 히어로 발동
 *
 * @param client
 * @param type			1 = 장전속도, 2 = 연사속도
 */
stock float clientClass_Assault_ActiveSkillHero ( int client, int type ) {
	if ( loringlib_IsValidClient ( client ) ) {
		switch ( type ) {
			//	reload speed.
			case 1: {
				int result[MAXPLAYERS + 1] = { 1, ... };
				for ( int i = 0; i < sizeof ( result ); i ++ )
					for ( int j = 0; j < sizeof ( result ); j ++ )
						if ( g_fHeroReloadSpeed[client][i] < g_fHeroReloadSpeed[client][j] )
							++result[i];
				
				for ( int i = 0; i < sizeof ( result ); i ++ ) {
					if ( result[i] == 1 ) {
						if ( g_fHeroReloadSpeed[client][i] <= 0.0 )	//	is zero?
							return 0.0;
							
						if ( g_iSkill[view_as<int> ( ClientClass_Assault )][SKILL_ID_HERO][i] <= 0 )
							return 0.0;
							
						return g_fHeroReloadSpeed[client][i];
					}
				}
			}
			
			//	firerate speed.
			case 2: {
				int result[MAXPLAYERS + 1] = { 1, ... };
				for ( int i = 0; i < sizeof ( result ); i ++ )
					for ( int j = 0; j < sizeof ( result ); j ++ )
						++result[i];
				
				for ( int i = 0; i < sizeof ( result ); i ++ ) {
					if ( result[i] == 1 ) {
						if ( g_fHeroFirerateSpeed[client][i] <= 0.0 )	//	is zero?
							return 0.0;
							
						if ( g_iSkill[view_as<int> ( ClientClass_Assault )][SKILL_ID_HERO][i] <= 0 )
							return 0.0;
							
						return g_fHeroFirerateSpeed[client][i];
					}
				}
			}
			
			default: return 0.0;
		}
	}
	
	return 0.0;
}

/**
 * 격려 발동
 */
stock float clientClass_Assault_TakeDamageOnEncourage ( int client, float dmg ) {
	int result[MAXPLAYERS + 1] = { 1, ... };
	
	for ( int i = 0; i < sizeof ( result ); i ++ )
		for ( int j = 0; j < sizeof ( result ); j ++ )
			if ( g_fEncourageDamage[client][i] < g_fEncourageDamage[client][j] )
				++result[i];
	
	for ( int i = 0; i < sizeof ( result ); i ++ ) {
		if ( result[i] == 1 ) {	//	버프 받은것들 중 가장 큰 값일 경우
			if ( g_fEncourageDamage[client][i] <= 0.0 )	//	가장 큰 값이 0이다?
				return dmg;
			
			//	만약 가장 큰값을 가진 주인이 격려를 배우지 않았을 경우
			if ( g_iSkill[view_as<int> ( ClientClass_Assault )][SKILL_ID_ENCOURAGE][i] <= 0 )
				return dmg;
			
		//	PrintToChatAll ( "가장 딜 버프가 높은 사람은 %N 입니다.", i );
			return dmg * ( 1.0 + g_fEncourageDamage[client][i] );
		}
	}
	
	return dmg;
}
 
/**
 * 브레이크 리미트
 */
void clientClass_Assault_BreakLimit ( int client, int skill ) {
	//	보급병이 아닐 경우
	if ( g_iClientClass[client] != ClientClass_Assault )
		return;
	
	if ( ZR_IsClientZombie ( client ) )
		return;
	
	int clientClass = view_as <int> ( g_iClientClass[client] );
	if ( g_bSkillUse[clientClass][skill][client] ) {
		CLIENT_SKILL_NOT_AVAILABLE_NOW_USING_MSG ( client )
		return;
	}
	
	if ( g_bSkillCooldown[clientClass][skill][client] ) {
		CLIENT_SKILL_NOT_AVAILABLE_COOLDOWN_MSG ( client )
		return;
	}
	
	int consume_ap = g_iSkillConsumeAP[clientClass][skill];
	if ( consume_ap > loringlib_GetEntityArmor ( client ) ) {
		CLIENT_SKILL_NOT_AVAILABLE_NEED_AP_MSG ( client )
		return;
	}
	
	g_bSkillUse[clientClass][skill][client] = true;
	loringlib_SetEntityArmor ( client, loringlib_GetEntityArmor ( client ) - consume_ap );
	CLIENT_SKILL_USE_MSG ( client, g_strSkillName[clientClass][skill] )
	
	float skillTime = g_fSkillAction[clientClass][skill];

	//	브레이크 리미트 마스터리를 배웠을 경우?
	if ( g_iSkill[clientClass][SKILL_ID_BREAK_LIMIT_MASTERY][client] > 0 )
		//	지속 시간을 더한다.
		skillTime += float ( g_iSkill[clientClass][SKILL_ID_BREAK_LIMIT_MASTERY][client] ) * g_fSkillAction[clientClass][SKILL_ID_BREAK_LIMIT_MASTERY];
	
	qufnrTools_DisplayProgressBar ( client, skillTime );
	CreateTimer ( skillTime, timerBreakLimitTimer, GetClientSerial ( client ) );
	
	g_iSurvivePoint[client] += SURVIVE_POINT_USE_SKILLS;	//	행동 점수
	
	float pos[3];
	loringlib_GetEntityOriginEx ( client, pos );
	
	PrecacheSound ( SOUND_ASSAULT_BREAKLIMIT_USE );
	EmitAmbientSound ( SOUND_ASSAULT_BREAKLIMIT_USE, pos, client );
	
	GetClientEyePosition ( client, pos );
	pos[2] -= 20.0;
	
	//	오버히트를 배웠을 경우?
	if ( g_iSkill[clientClass][SKILL_ID_OVERHIT][client] > 0 ) {
		g_fAssaultOverhitDamage[client] = 0.0;
		CreateTimer ( 0.0, timerOverhitTimer, GetClientSerial ( client ), TIMER_FLAG_NO_MAPCHANGE );
		CreateTimer ( ASSAULT_OVERHIT_TIME, timerOverhitTimer, GetClientSerial ( client ), TIMER_REPEAT | TIMER_FLAG_NO_MAPCHANGE );
		loringlib_CreateParticle ( client, 0, pos, "assault_breaklimit2", true, skillTime );
	}
	
	else //	브레이크 리미트 이펙트
		loringlib_CreateParticle ( client, 0, pos, "assault_breaklimit", true, skillTime );
}

public Action timerBreakLimitTimer ( Handle timer, any data ) {
	int id = GetClientFromSerial ( data );
	if ( id < 1 )	//	클라이언트가 없을 경우
		return Plugin_Stop;
	
	
	int clientClass = view_as <int> ( ClientClass_Assault );
	g_iSkillCooldownTicks[clientClass][SKILL_ID_BREAK_LIMIT][id] = g_iSkillCooldown[clientClass][SKILL_ID_BREAK_LIMIT];
	g_bSkillCooldown[clientClass][SKILL_ID_BREAK_LIMIT][id] = true;
	
	g_bSkillUse[clientClass][SKILL_ID_BREAK_LIMIT][id] = false;
	
	return Plugin_Stop;
}

/**
 * 브레이크 리미트 발동
 * @note weapon_fire에서
 */
void clientClass_Assault_WeaponFireOnBreakLimit ( int client, char[] weapon ) {
	if ( g_iClientClass[client] != ClientClass_Assault )
		return;
		
	int clientClass = view_as <int> ( g_iClientClass[client] );
	//	브레이크 리미트가 발동되었을 경우
	if ( g_bSkillUse[clientClass][SKILL_ID_BREAK_LIMIT][client] ) {
		bool is_ar = false;
		for ( int i = 0; i < sizeof ( g_strClassWeapon_Assault ); i ++ )
			if ( StrContains ( g_strClassWeapon_Assault[i], weapon, false ) != -1 )
				is_ar = true;
		
		//	병과 무기일 경우
		if ( is_ar ) {
			int active = GetEntPropEnt ( client, Prop_Send, "m_hActiveWeapon" );
			//	들고있는 무기가 유효할 경우
			if ( active != INVALID_ENT_REFERENCE )
				loringlib_SetClip1 ( active, loringlib_GetClip1 ( active ) + 1 );
		}
	}
}

/**
 * 오버히트 발동
 */
public Action timerOverhitTimer ( Handle timer, any data ) {
	int client = GetClientFromSerial ( data );
	if ( client < 1 )
		return Plugin_Stop;
	
	//	돌격병일 경우?
	if ( g_iClientClass[client] == ClientClass_Assault ) {
		//	도중에 좀비가 되었을 경우
		if ( ZR_IsClientZombie ( client ) ) {
			g_fAssaultOverhitDamage[client] = 0.0;
			return Plugin_Stop;
		}
	
		int clientClass = view_as<int> ( g_iClientClass[client] );
		int overhit_lv = g_iSkill[clientClass][SKILL_ID_OVERHIT][client];
		//	오버히트를 배웠을 경우
		if ( overhit_lv > 0 ) {
			//	브레이크 리미트가 끝났을 경우 타이머를 삭제한다.
			if ( !g_bSkillUse[clientClass][SKILL_ID_BREAK_LIMIT][client] ) {
				g_fAssaultOverhitDamage[client] = 0.0;
				return Plugin_Stop;
			}
			
			//	데미지를 타이머 마다 오버히트 엑션 값만큼 올린다.
			g_fAssaultOverhitDamage[client] += g_fSkillAction[clientClass][SKILL_ID_OVERHIT];
			
		//	PrintToChatAll ( "assault overhit damage: %f", g_fAssaultOverhitDamage[client] );
			
			return Plugin_Continue;
		}
	}
	
	return Plugin_Stop;
}

/**
 * 오버히트 데미지 적용
 */
stock float clientClass_Assault_TakeDamageOnOverhit ( int client, float dmg, int weapon, float damagePosition[3] ) {
	//	돌격병일 경우?
	if ( g_iClientClass[client] == ClientClass_Assault ) {
		if ( !ZR_IsClientZombie ( client ) ) {
			int clientClass = view_as<int> ( g_iClientClass[client] );
			int overhit_lv = g_iSkill[clientClass][SKILL_ID_OVERHIT][client];
			//	오버히트를 배웠을 경우
			if ( overhit_lv > 0 ) {
				//	브레이크 리미트가 끝났을 경우
				if ( !g_bSkillUse[clientClass][SKILL_ID_BREAK_LIMIT][client] )
					return dmg;
				
				char hand_weapon[32];
				if ( weapon != -1 ) {
					GetEdictClassname ( weapon, hand_weapon, sizeof ( hand_weapon ) );
					for ( int i = 0; i < sizeof ( g_strClassWeapon_Assault ); i ++ ) {
						if ( StrEqual ( g_strClassWeapon_Assault[i], hand_weapon, false ) ) {
							loringlib_CreateParticle ( client, 0, damagePosition, "assault_overhitshot" );
							return dmg * ( 1.0 + g_fAssaultOverhitDamage[client] );
						}
					}
				}
			}
		}
	}
	
	return dmg;
}

/**
 * 애모 차지
 * @note 돌격소총 발사 시 확률로 총알 +1 발 충전
 */
public void clientClass_Assault_AmmoCharge ( int client, char[] weapon ) {
	if ( !loringlib_IsValidClient__PlayGame ( client ) )
		return;
	
	if ( g_iClientClass[client] != ClientClass_Assault )
		return;
		
	int clientClass = view_as <int> ( g_iClientClass[client] );
	int ammo_charge_lv = g_iSkill[clientClass][SKILL_ID_AMMO_CHARGE][client];
	if ( ammo_charge_lv > 0 ) {
		float ammo_charge_action = float ( ammo_charge_lv ) * g_fSkillAction[clientClass][SKILL_ID_AMMO_CHARGE];

		int active = loringlib_GetActiveWeapon ( client );
		if ( active == INVALID_ENT_REFERENCE )
			return;
		
		bool is_ar = false;
		//	병과 무기인지 채크한다.
		for ( int i = 0; i < sizeof ( g_strClassWeapon_Assault ); i ++ )
			if ( StrContains ( g_strClassWeapon_Assault[i], weapon, false ) != -1 )
				is_ar = true;	//	병과 무기일 경우 채크해준다.
		
		//	병과 무기일 경우
		if ( is_ar ) {
			//	탄창이 없을 경우
			if ( loringlib_GetClip1 ( active ) <= 0 )
				return;
			
			//	찬스를 정한다.	(기본 찬스 + 스킬 찬스)
			if ( loringlib_IsFloatProbability ( ASSAULT_AMMO_CHARGE_NORMAL_CHANCE + ammo_charge_action ) ) {
				//	스프레이 컨트롤 시 찬스를 무시한다.
				if ( g_bSkillUse[clientClass][SKILL_ID_BREAK_LIMIT][client] )
					return;
				
				loringlib_SetClip1 ( active, loringlib_GetClip1 ( active ) + 1 );
			}
		}
	}
}

stock bool IsTargetPlayerActivityGamePlay ( int client, int target ) {
	if ( loringlib_IsValidClient__PlayGame ( client ) && loringlib_IsValidClient__PlayGame ( target ) ) {
		if ( client != target ) {
			if ( GetClientTeam ( client ) == GetClientTeam ( target ) ) {
				int target_aim_target = loringlib_GetClientAimPlayer ( target );
				if ( ( loringlib_IsValidClient__PlayGame ( target_aim_target ) && GetClientTeam ( target_aim_target ) != GetClientTeam ( target ) ) ||
					( GetClientButtons ( target ) & IN_ATTACK ) ||
					( GetClientButtons ( target ) & IN_ATTACK2 ) ||
					( GetEntProp ( client, Prop_Send, "m_bIsScoped" ) && ( GetClientButtons ( target ) & IN_ATTACK ) ) )
					return true;
			}
		}
	}
	
	return false;
}