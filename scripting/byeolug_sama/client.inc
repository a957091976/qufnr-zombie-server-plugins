/**
 * Create hud sync handlers.
 */
public void createHudSyncHandler ( bool create ) {
	if ( create ) {
		for ( int i = 0; i < IN_PLUGIN_TOTAL_HUDSYNCS; i ++ )
			g_hHudSyncArray[i] = CreateHudSynchronizer ();
	}
	
	else {
		for ( int i = 1; i <= MaxClients; i ++ )
			if ( loringlib_IsValidClient ( i ) )
				for ( int x = 0; x < IN_PLUGIN_TOTAL_HUDSYNCS; x ++ )
					ClearSyncHud ( i, g_hHudSyncArray[x] );
	}
}

/**
 * Connect Clients
 */
public void OnClientPostAdminCheck ( int client ) {
	onClientSDKHooks ( client );
	CLIENT_InitOnCreateClientsideTimer ( client, true );
	REWARD_ConnectOnResetRewardValues ( client );
//	defineClientKeyValues ( client );
	JOINSELECT_ConnectOnDisplayLogin ( client );
	
	STAT_StatOverflowCheck ( client );
	clearSkillCooldownData ( client );
	connectPartyClearOptions ( client );
	clientStatHooks ( client, true );
	
	ZOMBIESKILL_ConnectOnClearCooldown ( client );
}

/**
 * Disconnect Clients
 */
public void OnClientDisconnect ( int client ) {
	onClientSDKUnhooks ( client );
	CLIENT_InitOnCreateClientsideTimer ( client, false );
	
	inputClientKeyValues ( client, g_iSelectSlotIndex[client] );
	inputClientGlobalKeyValues ( client );
	
	onWeaponFirerateClearModifyNextAtt ( client );
	partyQuit ( client );
	clientStatHooks ( client, false );
}

public void onClientSDKHooks ( int client ) {
	SDKHook ( client, SDKHook_OnTakeDamage, onTakeDamage );
	SDKHook ( client, SDKHook_PostThink, onPostThink );
	SDKHook ( client, SDKHook_WeaponDrop, onWeaponDrop );
	onWeaponFirerateFunctionSDKHooks ( client, true );
}

public void onClientSDKUnhooks ( int client ) {
	SDKUnhook ( client, SDKHook_OnTakeDamage, onTakeDamage );
	SDKUnhook ( client, SDKHook_PostThink, onPostThink );
	SDKUnhook ( client, SDKHook_WeaponDrop, onWeaponDrop );
	onWeaponFirerateFunctionSDKHooks ( client, false );
}

/**
 * SDKHook_PostThink
 *
 * @param client
 * @noreturn
 */
public void onPostThink ( int client ) {
//	displayPlayerState ( client );
	clientScoreboardInfo ( client );
	
//	clientClass_Sniper_DisplaySniperBullet ( client );
	clientClass_Supporter_DisplayBarricadeName ( client );
	clientClass_Gunslinger_SetFullautoPistol ( client );
	
	ZOMBIEPOISON_PostThinkOnPoisonTickDamage ( client );
	CLIENT_GameFrameOnSetHumanHelmet ( client );
	
	RANK_FrameOnCheckRankUp ( client );
	
	loringlib_HideHUD ( client, HIDEHUD_HEALTH );
}

/**
 * SDKHook_WeaponDrop
 *
 * @param client
 * @return Actions
 */
public Action onWeaponDrop ( int client, int weapon ) {
	return Plugin_Continue;
}

/**
 * SDKHook_OnTakeDamage
 *
 * @param victim
 * @param attacker
 * @param inf
 * @param dmg
 * @param dmgType
 * @return Event Action
 */
public Action onTakeDamage ( int victim, int &attacker, int &inf, float &dmg, int &dmgType, int &weapon, float damageForce[3], float damagePosition[3] ) {
	if ( !loringlib_IsValidClient ( victim ) ||
		!loringlib_IsValidClient ( attacker ) )
		return Plugin_Continue;
	
	zombie_TakeDamageOnInfectDamage ( victim, attacker, weapon, dmg );
	
	clientClass_Medic_MedicBulletFired ( victim, attacker, weapon );
	
	if ( GetClientTeam ( victim ) == GetClientTeam ( attacker ) )
		return Plugin_Continue;
	
	//	공격자가 인간일 경우
	if ( ZR_IsClientHuman ( attacker ) ) {
		//	Old stat str
	/*	int totalStr = g_iStats[STAT_STR][attacker] + g_iBonusStats[STAT_STR][attacker];
		if ( totalStr > 0 )
			dmg = dmg * ( 1.0 + ( ( totalStr ) * STAT_STR_VALUE ) );	*/
		
		//	New stat str
		dmg = STAT_GetStatSTRResult ( attacker, dmg );	//	On stat.inc function.
		
		dmg = clientClass_Assault_AssaultMasterly ( attacker, dmg, weapon );	//	AR 숙련 (전 어썰트 마스터리)
		dmg = clientClass_Assault_TakeDamageOnOverhit ( attacker, dmg, weapon, damagePosition );	//	오버히트
	//	dmg = clientClass_Assault_SprayControlMastery ( attacker, dmg );	//	스프레이 컨트롤 마스터리
		dmg = clientClass_Assault_TakeDamageOnEncourage ( attacker, dmg );	//	격려 효과
		
		dmg = clientClass_Sniper_ZoomShot ( attacker, dmg );		//	줌샷
		dmg = clientClass_Sniper_TakeDamageOnHitmarkBulletDamage ( victim, attacker, dmg, weapon, damagePosition );	//	적중탄
	//	dmg = clientClass_Sniper_SniperMasterly ( attacker, dmg );		//	스나이퍼 마스터리
	//	dmg = clientClass_Sniper_AutoSniperMasterly ( attacker, dmg );	//	딱딱이 마스터리
		
		dmg = clientClass_Supporter_TakeDamageToArmorPiercingAmmo ( attacker, dmg, weapon, damagePosition );
		dmg = clientClass_Supporter_SMGUpgrade ( attacker, dmg, weapon );
		
		dmg = clientClass_Sniper_PenetrateBulletSniperDamage ( attacker, dmg, weapon );	//	관통탄 기본 피해
		dmg = clientClass_Sniper_TakeDamageOnPenetrateBullet ( attacker, dmg, weapon, damagePosition );	//	관통탄 발사
		dmg = clientClass_Sniper_TakeDamageOnHawkeyeDamage ( victim, attacker, dmg, weapon );
		
		dmg = ClientClass_Shotgunner_ShotgunMastery ( attacker, dmg, weapon );
		
		dmg = clientClass_Gunslinger_PistolUpgrade ( attacker, dmg, weapon );
		dmg = clientClass_Gunslinger_BulletTimePassiveDamage ( attacker, dmg, weapon );
		dmg = clientClass_Gunslinger_DesperadoPassiveDamage ( attacker, dmg, weapon );
		dmg = clientClass_Gunslinger_DesperadoTargettingToAttack ( victim, attacker, dmg, dmgType );
		
		clientClass_Medic_PlayerhurtToFireMedicPoison ( victim, attacker, dmg, dmgType, weapon, damagePosition );
		clientClass_Medic_PlayerHurtToGiveMedishot ( victim, attacker, dmg, dmgType, weapon );
		Medic_TakeDamageOnMedicBoostOwnerSurvivePoint ( attacker, dmg );
		clientClass_Gunslinger_BulletDanceHitEff ( attacker, dmgType, damagePosition );
		SURVIVE_POINT_TakeDamageOnCheckSurvivePoint ( victim, attacker, dmg, dmgType, weapon );
	//	takeDamageOnExpReward ( victim, attacker, dmg );
		
		return Plugin_Changed;
	}
	
	return Plugin_Continue;
}

/**
 * 생존 시간 채크
 * @note timerGlobalTimer의 Clientside 에서
 */
void SURVIVE_TIME_OnTimeCheckSurvive ( int client ) {
	if ( ZR_IsClientHuman ( client ) && IsPlayerAlive ( client ) && ( ZR_IsGameStart () && !ZR_IsGameEnd () ) && !g_bBossLevel )
		g_iSurviveTime[client] ++;
}

/**
 * 데이터 불러올 시 생존 시간 초기화
 */
void SURVIVE_TIME_DataLoadOnResetSurviveTime ( int client ) {
	g_iSurviveTime[client] = 0;
}

/**
 * 라운드 시작 시 생존 시간 초기화
 */
void SURVIVE_TIME_RoundStartOnResetSurviveTime () {
	for ( int i = 1; i <= MaxClients; i ++ )
		if ( loringlib_IsValidClient ( i ) )
			g_iSurviveTime[i] = 0;
}

/**
 * 행동 점수 채크
 * @note TakeDamage에서
 */
public void SURVIVE_POINT_TakeDamageOnCheckSurvivePoint ( int victim, int client, float dmg, int dmgType, int weaponid ) {
	if ( !loringlib_IsValidClient ( victim ) ||
		!loringlib_IsValidClient ( client ) )
		return;
	
	if ( !ZR_IsClientHuman ( client ) )
		return;
	
	if ( GetClientTeam ( client ) == GetClientTeam ( victim ) )
		return;
	
	if ( !ZR_IsGameStart () )
		return;
	
	static char weapon[32];
	strcopy ( weapon, sizeof ( weapon ), "" );
	if ( weaponid != -1 )
		GetEdictClassname ( weaponid, weapon, sizeof ( weapon ) );
	
	if ( StrContains ( weapon, "knife", false ) == -1 ) {
		g_fDamages[client] += dmg;
		if ( g_fDamages[client] >= SURVIVE_POINT_DAMAGE_SCALE ) {
			g_fDamages[client] = 0.0;
			g_iSurvivePoint[client] += SURVIVE_POINT_GIVEN_DAMAGES;
		}
	}
}

/**
 * 클라이언트 스코어보드 표시
 */
public void clientScoreboardInfo ( int client ) {
	/*
	static char name[32];
	getClientClassName ( g_iClientClass[client], name, sizeof ( name ), false, true );
	if ( ZR_IsClientZombie ( client ) ) {
		if ( ZR_IsClientMotherZombie ( client ) )
			CS_SetClientClanTag ( client, "숙주좀비" );
		else {
			if ( ZR_IsClientNemesis ( client ) )
				CS_SetClientClanTag ( client, "네메시스" );
			else
				CS_SetClientClanTag ( client, "좀비" );
		}
	}
	
	else
		CS_SetClientClanTag ( client, name );
	*/
/*	if ( SourceStore_IsClientStoreMVPPlus ( client ) )
		CS_SetClientClanTag ( client, "【SVIP】" );
	else if ( SourceStore_IsClientStoreMVP ( client ) )
		CS_SetClientClanTag ( client, "【VIP】" );
	else
		CS_SetClientClanTag ( client, "" );	*/
	CS_SetMVPCount ( client, g_iLevel[client] );
}

/**
 * Can pick up hook
 */
public Action onHumanWeaponPickup ( int client, int weapon ) {
	if ( loringlib_IsValidClient ( client ) ) {
		if ( ZR_IsClientHuman ( client ) ) {
			static char name[32];
			GetEdictClassname ( weapon, name, sizeof ( name ) );
			
			int index = GetEntProp ( weapon, Prop_Send, "m_iItemDefinitionIndex" );
			switch ( index ) {
				case 23:	strcopy ( name, sizeof ( name ), "weapon_mp5sd" );
				case 60:	strcopy ( name, sizeof ( name ), "weapon_m4a1_silencer" );
				case 61:	strcopy ( name, sizeof ( name ), "weapon_usp_silencer" );
				case 63:	strcopy ( name, sizeof ( name ), "weapon_cz75a" );
			}
			
			//	기본 무기일 경우
			if ( isDefaultWeapon ( name ) ) {
				return Plugin_Continue;
			
			//	기본 무기가 아닐 경우
			} else {
				switch ( g_iClientClass[client] ) {
					//	병과가 없을 경우
					case ClientClass_Normal: return Plugin_Continue;
					
					//	병과가 있을 경우
					default: {
						//	픽업한 무기 이름이 병과 무기가 맞을 경우
						if ( isClassWeapon ( g_iClientClass[client], name ) )
							return Plugin_Continue;
						else
							return Plugin_Handled;
					}
				}
			}
		}
	}
	
	return Plugin_Continue;
}

void CLIENT_InitOnCreateClientsideTimer ( int client, bool connect ) {
	if ( connect ) {
		if ( g_hClientTimer[client] == null )
			g_hClientTimer[client] = CreateTimer ( 0.1, timerClientsideTimer, client, TIMER_REPEAT );
	}
	
	else {
		if ( g_hClientTimer[client] != null ) {
			KillTimer ( g_hClientTimer[client] );
			g_hClientTimer[client] = null;
		}
	}
}

void CLIENT_GameFrameOnSetHumanHelmet ( int client ) {
	if ( loringlib_IsValidClient__PlayGame ( client ) ) {
		if ( ZR_IsClientHuman ( client ) ) {
			if ( !loringlib_HasEntityHelmet ( client ) )
				loringlib_SetEntityHelmet ( client, 1 );
		}
	}
}