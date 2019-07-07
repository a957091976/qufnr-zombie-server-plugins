#define		SNIPER_DOTAN_RELOAD_COUNT				1	/**< 도탄 장전 시 올라가는 총알 */
#define		SNIPER_PENETRATE_BULLET_RELOAD_COUNT	1	/**< 관통탄 장전 시 올라가는 총알 */
#define		SNIPER_PENETRATE_BULLET_SNIPER_DMG		0.5
#define		SNIPER_HITMARK_BULLET_TIME				5.0	/**< 적중탄 시간 */
//#define		SNIPER_VECTOR_BULLET_DISTANCE				110.0	/**< 백터 불릿 거리 */

#define		SOUND_SNIPER_PENETRATE_FIRE			"qufnr/sniper/sniper_penetrate_bullet_fire.mp3"
#define		SOUND_SNIPER_PENETRATE_RELOAD		"qufnr/sniper/sniper_penetrate_bullet_reload.mp3"


#define		SNIPER_LAST_BULLET_MISSILE_SPEED		500.0	/**< 라스트불릿 미사일 속도 */

int g_iSniperAmmo[MAXPLAYERS + 1] = { 0, ... };

static float g_fHitmarkBulletTicktime[MAXPLAYERS + 1] = { 0.0, ... };
static bool g_bUseHitmarkBullet[MAXPLAYERS + 1] = { false, ... };
static int g_iHitmarkBulletTarget[MAXPLAYERS + 1] = { -1, ... };

static float g_fHitmarkBulletDamage[MAXPLAYERS + 1][MAXPLAYERS + 1];
static int g_iHitmarkBulletHitCount[MAXPLAYERS + 1][MAXPLAYERS + 1];

static bool g_bUseLastBullet[MAXPLAYERS + 1] = { false, ... };

//static float g_fSmokeOrigin[3] = { -30.0, 0.0, 0.0 };
//static float g_fSmokeAngle[3] = { 0.0, -180.0, 0.0 };

/**
 * 저격병 데이터 초기화
 */
void clientClass_Sniper_ClearSniperData ( int client ) {
	if ( loringlib_IsValidClient ( client ) ) {
		g_bUseLastBullet[client] = false;
		g_bUseHitmarkBullet[client] = false;
		g_iSniperAmmo[client] = 0;
		g_iHitmarkBulletTarget[client] = -1;
		for ( int i = 0; i < MAXPLAYERS + 1; i ++ ) {
			g_fHitmarkBulletDamage[client][i] = 0.0;
			g_iHitmarkBulletHitCount[client][i] = 0;
		}
	}
}

/**
 * 저격병 총알 표시
 * @note PostThink에서
 */
public void clientClass_Sniper_DisplaySniperBullet ( int client ) {
	if ( loringlib_IsValidClient__PlayGame ( client ) ) {
		if ( g_iClientClass[client] != ClientClass_Sniper )
			return;
			
		if ( ZR_IsClientZombie ( client ) )
			return;
		
		int clientClass = view_as<int> ( g_iClientClass[client] );
		
		char name[32];
		//	사냥의 표식을 배웠을 경우
		if ( g_iSkill[clientClass][SKILL_ID_VECTOR_BULLET][client] > 0 )
			strcopy ( name, sizeof ( name ), "표식탄" );
		//	관통탄을 배웠을 경우
		else if ( g_iSkill[clientClass][SKILL_ID_PENETRATE_BULLET][client] > 0 )
			strcopy ( name, sizeof ( name ), "관통탄" );
		else
			return;
		
		SetHudTextParamsEx ( 0.61, 0.96, 0.05, { 255, 0, 0, 255 }, { 255, 0, 0, 255 }, 0, 0.01, 0.01, 0.05 );
		ShowSyncHudText ( client, g_hHudSyncArray[HUDSYNC_MEDIC_BULLET], "%s: %d발", name, g_iSniperAmmo[client] );

	}
}

/**
 * 관통탄 배울 시 저격 추가 피해
 * @note TakeDamage에서
 */
stock float clientClass_Sniper_PenetrateBulletSniperDamage ( int client, float dmg, int weaponIndex ) {
	if ( g_iClientClass[client] == ClientClass_Sniper ) {
		int clientClass = view_as <int> ( g_iClientClass[client] );
		int penetrate_bullet_lv = g_iSkill[clientClass][SKILL_ID_PENETRATE_BULLET][client];
		if ( penetrate_bullet_lv > 0 ) {
			if ( weaponIndex != -1 ) {
				char weapon[32];
				GetEdictClassname ( weaponIndex, weapon, sizeof ( weapon ) );
				if ( StrEqual ( weapon, "weapon_awp", false ) || StrEqual ( weapon, "weapon_ssg08", false ) )
					return dmg * ( 1.0 + SNIPER_PENETRATE_BULLET_SNIPER_DMG );
			}
		}
	}
	
	return dmg;
}

/**
 * 줌샷
 */
stock float clientClass_Sniper_ZoomShot ( int client, float dmg ) {
	if ( g_iClientClass[client] == ClientClass_Sniper ) {
		int clientClass = view_as <int> ( g_iClientClass[client] );
		int zoomshot_lv = g_iSkill[clientClass][SKILL_ID_ZOOMSHOT][client];
		//	줌샷을 배웠을 경우
		if ( zoomshot_lv > 0 ) {
			//	줌하고 있을 경우
			if ( GetEntProp ( client, Prop_Send, "m_bIsScoped" ) ) {
				return dmg * ( 1.0 + float ( zoomshot_lv ) * g_fSkillAction[clientClass][SKILL_ID_ZOOMSHOT] );
			}
		}
	}
	
	return dmg;
}

/**
 * 특수탄 장전
 */
void clientClass_Sniper_ReloadSniperSpecialBullet ( int client, int skill ) {
	//	저격병이 아닐 경우
	if ( g_iClientClass[client] != ClientClass_Sniper )
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
	
	g_iSkillCooldownTicks[clientClass][skill][client] = g_iSkillCooldown[clientClass][skill];
	
	float pos[3];
	GetClientAbsOrigin ( client, pos );
	//	표식탄
	if ( skill == SKILL_ID_VECTOR_BULLET ) {
	//	g_iSniperAmmo[client] += SNIPER_DOTAN_RELOAD_COUNT;
		g_bUseHitmarkBullet[client] = true;
		g_fHitmarkBulletTicktime[client] = SNIPER_HITMARK_BULLET_TIME;
		if ( g_iSkill[clientClass][SKILL_ID_VECTOR_BULLET_MASTERY][client] > 0 )	//	마스터리를 배웠을 경우 시간을 추가한다.
			g_fHitmarkBulletTicktime[client] += float ( g_iSkill[clientClass][SKILL_ID_VECTOR_BULLET_MASTERY][client] ) * g_fSkillAction[clientClass][SKILL_ID_VECTOR_BULLET_MASTERY];
			
		CreateTimer ( 0.1, timerHitmarkBullet, client, TIMER_REPEAT | TIMER_FLAG_NO_MAPCHANGE );
	}
	
	//	관통탄
	else if ( skill == SKILL_ID_PENETRATE_BULLET ) {
		PrecacheSound ( SOUND_SNIPER_PENETRATE_RELOAD );
		EmitAmbientSound ( SOUND_SNIPER_PENETRATE_RELOAD, pos, client );
		g_iSniperAmmo[client] += SNIPER_PENETRATE_BULLET_RELOAD_COUNT;
	}
	
	else	PrintToChat ( client, "%s \x01ERROR: Invalid skill id. %d", MESSAGE_PREFIX, skill );
}

public Action timerHitmarkBullet ( Handle timer, any client ) {
	if ( loringlib_IsValidClient ( client ) ) {
		if ( !ZR_IsGameEnd () ) {
			if ( g_iClientClass[client] == ClientClass_Sniper ) {
				if ( g_bUseHitmarkBullet[client] ) {
					int clientClass = view_as<int> ( ClientClass_Sniper );
					g_fHitmarkBulletTicktime[client] -= 0.1;
					
					char targetname[64];
					if ( loringlib_IsValidClient ( g_iHitmarkBulletTarget[client] ) ) {
						if ( IsPlayerAlive ( g_iHitmarkBulletTarget[client] ) )
							Format ( targetname, sizeof ( targetname ), "%N", g_iHitmarkBulletTarget[client] );
						else
							Format ( targetname, sizeof ( targetname ), "%N [DEAD]", g_iHitmarkBulletTarget[client] );
					}
					
					else
						Format ( targetname, sizeof ( targetname ), "NO TARGET" );
					
					SetHudTextParams ( -1.0, 0.788, 0.15, 90, 200, 230, 255, 0, 0.0, 0.0, 0.0 );
					ShowSyncHudText ( client, g_hHudSyncArray[HUDSYNC_SNIPER_HITMARK_BULLET_INFO], "LAST TARGET: %s\nTIME: %.1f second%s", targetname, g_fHitmarkBulletTicktime[client], g_fHitmarkBulletTicktime[client] >= 2.0 ? "s" : "" );
					
					//	시간이 끝나면 삭제하고, 데이터를 모두 초기화 시킨다. 그리고 쿨타임을 만든다.
					if ( g_fHitmarkBulletTicktime[client] <= 0.0 ) {
						g_bSkillCooldown[clientClass][SKILL_ID_VECTOR_BULLET][client] = true;
						g_bSkillUse[clientClass][SKILL_ID_VECTOR_BULLET][client] = false;
						g_iSkillCooldownTicks[clientClass][SKILL_ID_VECTOR_BULLET][client] = g_iSkillCooldown[clientClass][SKILL_ID_VECTOR_BULLET];
						g_bUseHitmarkBullet[client] = false;
						g_iHitmarkBulletTarget[client] = -1;
						for ( int i = 0; i < MAXPLAYERS + 1; i ++ ) {
							g_fHitmarkBulletDamage[client][i] = 0.0;
							g_iHitmarkBulletHitCount[client][i] = 0;
						}
						
						SetHudTextParams ( -1.0, 0.788, 1.5, 90, 200, 230, 255, 0, 0.0, 0.0, 0.0 );
						ShowSyncHudText ( client, g_hHudSyncArray[HUDSYNC_SNIPER_HITMARK_BULLET_INFO], "^" );
					
						return Plugin_Stop;
					}
					
					return Plugin_Continue;
				}
			}
		}
	}
	
	return Plugin_Stop;
}

/**
 * 적중탄 데미지
 */
stock float clientClass_Sniper_TakeDamageOnHitmarkBulletDamage ( int victim, int client, float dmg, int idx, float dmgpos[3] ) {
	if ( g_iClientClass[client] == ClientClass_Sniper ) {
		if ( ZR_IsClientHuman ( client ) && !ZR_IsClientHuman ( victim ) ) {
			if ( idx == -1 )
				return dmg;
			
			char weapon[32];
			GetEdictClassname ( idx, weapon, sizeof ( weapon ) );
			if ( StrEqual ( weapon, "weapon_g3sg1", false ) ||
				StrEqual ( weapon, "weapon_scar20", false ) ) {
				int clientClass = view_as<int> ( ClientClass_Sniper );
				int hitmark_bullet_lv = g_iSkill[clientClass][SKILL_ID_VECTOR_BULLET][client];
				if ( hitmark_bullet_lv > 0 ) {
					//	적중탄을 사용하고있는지 여부를 채크한다.
					//	적중탄을 사용하고 있고, 적중탄 타겟이 victim과 일치할 경우?
					if ( g_bUseHitmarkBullet[client] ) {
						g_iHitmarkBulletTarget[client] = victim;
					
						g_iHitmarkBulletHitCount[client][victim] ++;
						
						//	3의 배수일 경우
						if ( g_iHitmarkBulletHitCount[client][victim] % 3 == 0 ) {
						//	SetHudTextParams ( -1.0, 0.655, 1.0, 90, 200, 230, 255, 0, 0.01, 0.01, 0.5 );
						//	ShowSyncHudText ( client, g_hHudSyncArray[HUDSYNC_SNIPER_HITMARK_BULLET_INFO], "TARGET: %N\nHIT COUNT: %d\nDAMAGE: %.1f%%", victim, g_iHitmarkBulletHitCount[client], 100.0 * ( float ( g_iHitmarkBulletHitCount[client] ) * g_fHitmarkBulletDamage[client] ) );
							
						//	loringlib_CreateParticle ( victim, 0, dmgpos, "blood_impact_headshot_01c", true, 0.5 );
							
							//	데미지 설정
							g_fHitmarkBulletDamage[client][victim] += float ( hitmark_bullet_lv ) * g_fSkillAction[clientClass][SKILL_ID_VECTOR_BULLET];
							//	적중탄 마스터리를 배웠을 경우 데미지 추가
						//	if ( g_iSkill[clientClass][SKILL_ID_VECTOR_BULLET_MASTERY][client] > 0 )
						//		g_fHitmarkBulletDamage[client][victim] += float ( g_iSkill[clientClass][SKILL_ID_VECTOR_BULLET_MASTERY][client] ) * g_fSkillAction[clientClass][SKILL_ID_VECTOR_BULLET_MASTERY];
							
							float damageResult = dmg * ( 1.0 + ( float ( g_iHitmarkBulletHitCount[client][victim] ) * g_fHitmarkBulletDamage[client][victim] ) );
							
							EmitSoundToClient ( client, SOUND_SNIPER_VECTOR_BULLET_HIT );
							
							//	마무리 일격
							if ( g_iSkill[clientClass][SKILL_ID_LAST_BLOW][client] > 0 ) {
								//	loringlib_MakeExplosion ( client, _, dmgpos, "weapon_c4", 350, 0, g_fSkillAction[clientClass][SKILL_ID_LAST_BLOW], CS_TEAM_CT, 6146 );
								lastblowExplosion ( client, dmgpos, "weapon_c4", 250.0, RoundFloat ( g_fSkillAction[clientClass][SKILL_ID_LAST_BLOW] ), CS_TEAM_CT );
							}
							
							loringlib_CreateParticle ( victim, 0, dmgpos, "sniper_huntermark", true, 1.5 );
							loringlib_CreateParticle ( victim, 0, dmgpos, "sniper_huntermark_target", true, 0.5 );
							
							return damageResult;
						}
					}
				}
			}
		}
	}
	
	return dmg;
}

/**
 * 마무리 일격 폭발
 */
void lastblowExplosion ( int client, float pos[3], char[] weapon, float distance, int damage, int team ) {
	if ( loringlib_IsValidClient ( client ) ) {
		float nearestDist = 0.0;
		for ( int target = 1; target <= MaxClients; target ++ ) {
			if ( loringlib_IsValidClient__PlayGame ( target ) ) {
				if ( GetClientTeam ( target ) != team ) {
					//	거리안에 있다?
					if ( loringlib_GetNearTrue ( client, target, pos, distance, nearestDist ) ) {
						loringlib_MakeDamage ( client, target, damage - RoundFloat ( nearestDist ), weapon, DMG_BURN );
					}
				}
			}
		}
		
		loringlib_CreateParticle ( client, 0, pos, "sniper_huntermarkshot_explode", false, 3.0 );
		EmitAmbientSound ( SOUND_SNIPER_LAST_BLOW_EXPLODE, pos, client );
	}
}

/**
 * 딱딱이 마스터리
 * @note 재장전 채크 콜백에서
 */
stock float clientClass_Sniper_AutosniperMastery ( int client ) {
	if ( loringlib_IsValidClient ( client ) ) {
		if ( g_iClientClass[client] == ClientClass_Sniper ) {
			int clientClass = view_as<int> ( ClientClass_Sniper );
			if ( g_iSkill[clientClass][SKILL_ID_AUTOSNIPER_MASTERY][client] > 0 ) {
				char weapon[32];
				loringlib_GetActiveWeaponName ( client, weapon, sizeof ( weapon ) );
				if ( StrEqual ( weapon, "weapon_scar20", false ) ||
					StrEqual ( weapon, "weapon_g3sg1", false ) ) {
				//	PrintHintText ( client, "%f", float ( g_iSkill[clientClass][SKILL_ID_AUTOSNIPER_MASTERY][client] ) * g_fSkillAction[clientClass][SKILL_ID_AUTOSNIPER_MASTERY] );
					return float ( g_iSkill[clientClass][SKILL_ID_AUTOSNIPER_MASTERY][client] ) * g_fSkillAction[clientClass][SKILL_ID_AUTOSNIPER_MASTERY];
				}
			}
		}
	}
	
	return 0.0;
}

/**
 * 호크아이 데미지 연산
 * @note TakeDamage에서
 */
stock float clientClass_Sniper_TakeDamageOnHawkeyeDamage ( int victim, int client, float dmg, int idx ) {
	if ( g_iClientClass[client] == ClientClass_Sniper ) {
		if ( !ZR_IsClientHuman ( victim ) ) {
			if ( idx == -1 )
				return dmg;
				
			char weapon[32];
			GetEdictClassname ( idx, weapon, sizeof ( weapon ) );
			if ( StrEqual ( weapon, "weapon_awp", false ) ||
				StrEqual ( weapon, "weapon_ssg08", false ) ) {
				int clientClass = view_as<int> ( ClientClass_Sniper );
				int hawkeye_lv = g_iSkill[clientClass][SKILL_ID_HAWKEYE][client];
				if ( hawkeye_lv > 0 ) {
					float cutoff = 0.0;
					if ( StrEqual ( weapon, "weapon_awp", false ) )
						cutoff = 300.0;
					else if ( StrEqual ( weapon, "weapon_ssg08", false ) )
						cutoff = 150.0;
					else
						return dmg;
				
					if ( cutoff != 0.0 && dmg >= cutoff ) {
						//	대가리 파티클
						float eye[3];
						GetClientEyePosition ( victim, eye );
						loringlib_CreateParticle ( victim, 0, eye, "blood_impact_goop_heavy" );
					
						float damage = float ( hawkeye_lv ) * g_fSkillAction[clientClass][SKILL_ID_HAWKEYE];
						return dmg * ( 1.0 + damage );
					}
				}
			}
		}
	}
	
	return dmg;
}

/**
 * 관통탄 발사 init
 * @note TakeDamage에서
 */
stock float clientClass_Sniper_TakeDamageOnPenetrateBullet ( int client, float dmg, int index, float dmgpos[3] ) {
	if ( g_iClientClass[client] == ClientClass_Sniper ) {
		int clientClass = view_as<int> ( g_iClientClass[client] );
		int penetrate_bullet_lv = g_iSkill[clientClass][SKILL_ID_PENETRATE_BULLET][client];
		//	관통탄을 배웠을 경우
		if ( penetrate_bullet_lv > 0 ) {
			//	특수탄이 없고, 라스트불릿이 아닐 경우
			//	특수탄이 있거나, 라스트불릿일 경우 넘어간다.
			if ( g_iSniperAmmo[client] <= 0 && !g_bUseLastBullet[client] )
				return dmg;
			
			if ( index == -1 )
				return dmg;
			
			char weapon[32];
			GetEdictClassname ( index, weapon, sizeof ( weapon ) );
			//	저격만 해당된다.
			if ( StrContains ( weapon, "awp", false ) == -1 &&
				StrContains ( weapon, "ssg08", false ) == -1 )
				return dmg;
		
			float damage = g_fSkillAction[clientClass][SKILL_ID_PENETRATE_BULLET];
			//	관통탄 마스터리를 배웠을 경우
			if ( g_iSkill[clientClass][SKILL_ID_PENETRATE_BULLET_MASTERY][client] > 0 )
				damage += float ( g_iSkill[clientClass][SKILL_ID_PENETRATE_BULLET_MASTERY][client] ) * g_fSkillAction[clientClass][SKILL_ID_PENETRATE_BULLET_MASTERY];
			
			float pos[3];
			GetClientAbsOrigin ( client, pos );
			PrecacheSound ( SOUND_SNIPER_PENETRATE_FIRE );
			EmitAmbientSound ( SOUND_SNIPER_PENETRATE_FIRE, pos, client );
			
		//	loringlib_MakeDamage ( client, other, RoundFloat ( damage ), "weapon_awp", DMG_BURN );
			
			loringlib_CreateParticle ( client, 0, dmgpos, "sniper_penetrationshot", true, 1.0 );
			
			return dmg + damage;
		}
	}
	
	return dmg;
}

/**
 * 관통탄 발사 전 레이저 이펙트 및 라스트불릿 init
 * @note bullet_impact에서
 */
void clientClass_Sniper_PenetrateBulletImpact ( int client, float pos[3] ) {
	if ( loringlib_IsValidClient ( client ) ) {
		if ( g_iClientClass[client] == ClientClass_Sniper ) {
			int clientClass = view_as<int> ( g_iClientClass[client] );
			int penetrate_bullet_lv = g_iSkill[clientClass][SKILL_ID_PENETRATE_BULLET][client];
			//	관통탄을 배웠을 경우
			if ( penetrate_bullet_lv > 0 ) {
				
				char weapon[32];
				int active = loringlib_GetActiveWeaponName ( client, weapon, sizeof ( weapon ) );
				if ( !StrEqual ( weapon, "weapon_awp", false ) &&
					!StrEqual ( weapon, "weapon_ssg08", false ) )
					return;
							
				//	라스트불릿을 배웠을 경우
				if ( g_iSkill[clientClass][SKILL_ID_LAST_BULLET][client] > 0 ) {
					//	주 무기의 탄창이 1발이거나 이하일 경우
					if ( loringlib_GetClip1 ( active ) <= 1 ) {
						float start[3];
						GetClientEyePosition ( client, start );
							
						TE_SetupBeamPoints ( start, pos, PrecacheModel ( "materials/qufnr/smoke_beam.vmt" ), 0, 1, 1, 0.7, 0.5, 0.5, 1, 0.5, { 255, 0, 0, 255 }, 0 );
						TE_SendToAll ();
						
						g_bUseLastBullet[client] = true;
						CreateTimer ( 0.2, timerUseLastBulletPost, client, TIMER_FLAG_NO_MAPCHANGE );
					
						
					//	CreateTimer ( 0.1, timerUseSniperAmmoPost, client, TIMER_FLAG_NO_MAPCHANGE );
						return;
					}
				}
				
				//	특수탄이 있을 경우
				if ( g_iSniperAmmo[client] > 0 ) {
					float start[3];
					GetClientEyePosition ( client, start );
						
					TE_SetupBeamPoints ( start, pos, PrecacheModel ( "materials/qufnr/smoke_beam.vmt" ), 0, 1, 1, 0.7, 0.5, 0.5, 1, 0.5, { 255, 0, 0, 255 }, 0 );
					TE_SendToAll ();
				}
			}
		}
	}
}

/**
 * 관통탄 발사 후 특수탄 갯수 변수값 깍기
 * @note weapon_fire에서
 */
public void Sniper_WeaponFireOnPenetrateBullet ( int client, char[] weapon ) {
	if ( loringlib_IsValidClient ( client ) ) {
		if ( g_iSkill[view_as<int> ( ClientClass_Sniper )][SKILL_ID_PENETRATE_BULLET][client] > 0 ) {
			if ( !g_bUseLastBullet[client] ) {	//	라스트 불릿일 경우 제외함.
				//	저격무기 둘다 아닐 경우
				if ( StrContains ( weapon, "ssg08", false ) == -1 &&
					StrContains ( weapon, "awp", false ) == -1 )
					return;
				
				if ( g_iSniperAmmo[client] > 0 )
					CreateTimer ( 0.1, timerUseSniperAmmoPost, client, TIMER_FLAG_NO_MAPCHANGE );
			}
		}
	}
}

public Action timerUseSniperAmmoPost ( Handle timer, any client ) {
	if ( loringlib_IsValidClient ( client ) ) {
		if ( g_iSniperAmmo[client] <= 0 ) {
			g_iSniperAmmo[client] = 0;
			return Plugin_Stop;
		}
		
		g_iSniperAmmo[client] --;
		
		g_bSkillCooldown[view_as<int> ( ClientClass_Sniper )][SKILL_ID_PENETRATE_BULLET][client] = true;
		g_bSkillUse[view_as<int> ( ClientClass_Sniper )][SKILL_ID_PENETRATE_BULLET][client] = false;
		
	}
	
	return Plugin_Stop;
}

public Action timerUseLastBulletPost ( Handle timer, any client ) {
	if ( loringlib_IsValidClient ( client ) ) {
		g_bUseLastBullet[client] = false;
	}
}