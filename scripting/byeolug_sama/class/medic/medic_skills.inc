#define CLIENT_MEDIC_MEDISHOT_USE_IS_TARGET_FULL_HP_MSG(%1,%2)	PrintToChat ( %1, "%s \x01%N님의 체력은 가득 차 있습니다.", MESSAGE_PREFIX, %2 );
#define CLIENT_MEDIC_MEDISHOT_SUCCESS_TARGET_HEAL_MSG(%1,%2,%3)	PrintToChat ( %1, "%s \x01%N님을 치료 했습니다. \x06+%d EXP", MESSAGE_PREFIX, %2, %3 );
#define CLIENT_MEDIC_CLASS_FAIL_TARGET_HEAL(%1)		PrintToChat ( %1, " \x04[Medic] \x01팀원을 치료하려면 (의료주사) 기술을 배워야 합니다." );
#define CLIENT_MEDIC_POISON_HEAL_MSG(%1,%2)			PrintToChat ( %1, "%s \x01%N님의 감염상태를 회복했습니다.", MESSAGE_PREFIX, %2 );
#define CLIENT_MEDIC_POISON_HEAL_TARGET_MSG(%1,%2)	PrintToChat ( %1, "%s \x01%N님이 귀하의 감염상태를 회복했습니다.", MESSAGE_PREFIX, %2 );
#define CLIENT_MEDIC_MEDIC_BOOST_ENABLE_MSG(%1,%2)	PrintToChat ( %1, "%s \x01%N님에게 (나노강화치유탄)을 사용했습니다.", MESSAGE_PREFIX, %2 );
#define CLIENT_MEDIC_MEDIC_BOOST_ENABLE_TARGET_MSG(%1,%2)	PrintToChat ( %1, "%s \x01%N님이 귀하에게 (나노강화치유탄)을 맞췄습니다! 귀하는 %.0f초간 연사력이 미친듯이 올라갑니다!!!", MESSAGE_PREFIX, %2, MEDIC_NANO_UPGRADE_BULLET_BOOST_TIME );
#define CLIENT_MEDIC_TARGET_IS_NOW_BOOST_MSG(%1,%2)	PrintToChat ( %1, "%s \x01%N님은 이미 (나노강화치유탄) 효과를 받고 있습니다.", MESSAGE_PREFIX, %2 );

#define MEDIC_POISON_HEAL_REWARD_MIN	10
#define MEDIC_POISON_HEAL_REWARD_MAX	15

#define MEDIC_HP_UPGRADE_DEFAULT_HEALTH		10	/**< 단련된 피부 기본 체력 증가 값 */

#define MEDIC_MEDISHOT_HEAL_DISTANCE	90.0

#define MEDIC_TEAMMATE_HEAL_COOLDOWN	5.0	/**< 아군 치료 쿨타임 */

#define MEDIC_MEDIC_GAS_TIME			5.0		/**< 치유가스 시간 */
#define MEDIC_MEDIC_GAS_DISTANCE		250.0	/**< 치유가스 거리 */
#define MEDIC_MEDIC_GAS_ZOMBIE_DAMAGE	150		/**< 치유가스에 들어간 좀비에게 입히는 기본 피해량 */

#define MEDIC_MEDISHOT_GIVEN_DAMAGE		2000	/**< 의료주사를 받는 기본 피해 량 */
#define MEDIC_POISONSHOT_GIVEN_DAMAGE	1000	/**< 독성주사를 받는 기본 피해 량 */

#define MEDIC_POISON_GRENADE_GIVE_DMGS	2000	/**< 마비 수류탄 일정 피해량 후 지급 */
#define MEDIC_POISON_GRENADE_RADIUS		200.0	/**< 마비 수류탄 거리 */
#define MEDIC_POISON_GRENADE_DURATION	5.0		/**< 마비 수류탄 지속 시간 */

#define MEDIC_NANO_UPGRADE_BULLET_BOOST_TIME	5.0		/**< 나노강화치유탄 지속 시간 */
#define MEDIC_MEDIC_BOOST_RELOAD_SPEED		0.55		/**< 나노강화치유탄 장전 속도 기본: 0.4 */
#define MEDIC_MEDIC_BOOST_SURVIVE_POINT_DAMAGE_SCALE	1000	/**< 나노강화치유탄 맞은 타겟이 이 값 만큼 데미지를 입힐 때 마다 행동점수 득점 */

#define SOUND_MEDIC_MEDIC_BULLET_RELOAD		"qufnr/medic/medic_bullet_reload.mp3"
#define SOUND_MEDIC_NANO_MACHINE_GAS_SPAWN	"qufnr/medic/medic_nano_medic_gas_spawn.mp3"
#define SOUND_MEDIC_MEDIC_GAS_SPAWN			"qufnr/medic/medic_gas_spawn.mp3"

int g_iMedicBullet[MAXPLAYERS + 1] = { 0, ... };
int g_bUseMedishot[MAXPLAYERS + 1] = { false, ... };
int g_iMedicCheckDamage[MAXPLAYERS + 1] = { 0, ... };

static bool g_bMedicGasHealTrue[MAXPLAYERS + 1][MAXPLAYERS + 1];
static int g_iMedicPoison[MAXPLAYERS + 1][MAXPLAYERS + 1];
static int g_iMedicMedicGasTime[MAXPLAYERS + 1] = { 0, ... };
static bool g_bMedicBoost[MAXPLAYERS + 1] = { false, ... };
static float g_fMedicBoostOnDamage[MAXPLAYERS + 1] = { 0.0, ... };
static int g_iMedicBoostOwner[MAXPLAYERS + 1] = { -1, ... };

/**
 * 의료주사기 사용 쿨다운 강제 제거 
 * @note onPlayerSpawned ()에서 호출됨
 */
public void forceCooldownEndMedishot ( int client ) {
	if ( loringlib_IsValidClient ( client ) ) {
		g_bUseMedishot[client] = false;
	}
}

/**
 * 스폰 시 메딕 데이터 초기화
 */
public void clientClass_Medic_ClearMedicData ( int client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return;
	
	forceCooldownEndMedishot ( client );
	g_bMedicBoost[client] = false;
	g_iMedicBoostOwner[client] = -1;
	g_fMedicBoostOnDamage[client] = 0.0;
	g_iMedicBullet[client] = 0;
	g_iMedicCheckDamage[client] = 0;
	g_iMedicMedicGasTime[client] = 0;
	for ( int i = 0; i < sizeof ( g_bMedicGasHealTrue[] ); i ++ )
		g_bMedicGasHealTrue[client][i] = false;
}

/**
 * 치유탄 장전
 */
void clientClass_Medic_ReloadMedicBullet ( int client, int skill_id ) {
	if ( ZR_IsClientZombie ( client ) )
		return;
	
	if ( g_iClientClass[client] != ClientClass_Medic )
		return;
	
	int clientClass = view_as <int> ( g_iClientClass[client] );
	if ( g_bSkillUse[clientClass][skill_id][client] ) {
		CLIENT_SKILL_NOT_AVAILABLE_NOW_USING_MSG ( client )
		return;
	}
	
	if ( g_bSkillCooldown[clientClass][skill_id][client] ) {
		CLIENT_SKILL_NOT_AVAILABLE_COOLDOWN_MSG ( client )
		return;
	}
	
	int consume_ap = g_iSkillConsumeAP[clientClass][skill_id];
	if ( consume_ap > loringlib_GetEntityArmor ( client ) ) {
		CLIENT_SKILL_NOT_AVAILABLE_NEED_AP_MSG ( client )
		return;
	}
	
	g_bSkillUse[clientClass][skill_id][client] = true;
	loringlib_SetEntityArmor ( client, loringlib_GetEntityArmor ( client ) - consume_ap );
	CLIENT_SKILL_USE_MSG ( client, g_strSkillName[clientClass][skill_id] )

	
	PrecacheSound ( SOUND_MEDIC_MEDIC_BULLET_RELOAD );
	float pos[3];
	GetClientAbsOrigin ( client, pos );
	pos[2] += 5.0;
	EmitAmbientSound ( SOUND_MEDIC_MEDIC_BULLET_RELOAD, pos, client );
	
	g_iMedicBullet[client] ++;
}

/**
 * 치유가스 소환
 */
void clientClass_Medic_DispatchMedicGas ( int client, int skill_id ) {
	if ( ZR_IsClientZombie ( client ) )
		return;
	
	if ( g_iClientClass[client] != ClientClass_Medic )
		return;
	
	int clientClass = view_as <int> ( g_iClientClass[client] );
	if ( g_bSkillUse[clientClass][skill_id][client] ) {
		CLIENT_SKILL_NOT_AVAILABLE_NOW_USING_MSG ( client )
		return;
	}
	
	if ( g_bSkillCooldown[clientClass][skill_id][client] ) {
		CLIENT_SKILL_NOT_AVAILABLE_COOLDOWN_MSG ( client )
		return;
	}
	
	int consume_ap = g_iSkillConsumeAP[clientClass][skill_id];
	if ( consume_ap > loringlib_GetEntityArmor ( client ) ) {
		CLIENT_SKILL_NOT_AVAILABLE_NEED_AP_MSG ( client )
		return;
	}
	
	g_bSkillUse[clientClass][skill_id][client] = true;
	loringlib_SetEntityArmor ( client, loringlib_GetEntityArmor ( client ) - consume_ap );
	CLIENT_SKILL_USE_MSG ( client, g_strSkillName[clientClass][skill_id] )
	
	float pos[3];
	loringlib_GetEntityOriginEx ( client, pos );

	pos[2] += 5.0;
	
	//	나노머신가스를 배웠을 경우?
	if ( g_iSkill[clientClass][SKILL_ID_NANO_MACHINE_GAS][client] > 0 ) {
		//	나노머신가스 이펙트
	//	loringlib_CreateParticle ( client, 0, pos, "heal_nanopulse", true, 1.0 );
		PrecacheSound ( SOUND_MEDIC_NANO_MACHINE_GAS_SPAWN );
		EmitAmbientSound ( SOUND_MEDIC_NANO_MACHINE_GAS_SPAWN, pos, client );
	} else {
		//	치유가스 이펙트
	//	loringlib_CreateParticle ( client, 0, pos, "heal_pulse", true, 1.0 );
		PrecacheSound ( SOUND_MEDIC_MEDIC_GAS_SPAWN );
		EmitAmbientSound ( SOUND_MEDIC_MEDIC_GAS_SPAWN, pos, client );
	}
	
	g_iMedicMedicGasTime[client] = 0;	//	가스 유효 시간 셋팅
	CreateTimer ( 0.0, timerMedicGasTimer, client, TIMER_FLAG_NO_MAPCHANGE );	//	시전 시 바로 한번 힐 해준다.
	CreateTimer ( 1.0, timerMedicGasTimer, client, TIMER_REPEAT | TIMER_FLAG_NO_MAPCHANGE );
}

/**
 * 치유가스 효과
 */
public Action timerMedicGasTimer ( Handle timer, any owner ) {
	if ( owner != -1 ) {
		//	주인이 유효하지 않을 경우 타이머 제거
		if ( !loringlib_IsValidClient ( owner ) ) {
			g_iMedicMedicGasTime[owner] = 0;
			return Plugin_Stop;
		}
		
		//	주인이 좀비일 경우 타이머 제거
		if ( ZR_IsClientZombie ( owner ) ) {
			g_iMedicMedicGasTime[owner] = 0;
			return Plugin_Stop;
		}
		
		//	주인이 메딕이 아닐 경우 타이머 제거
		if ( g_iClientClass[owner] != ClientClass_Medic ) {
			g_iMedicMedicGasTime[owner] = 0;
			return Plugin_Stop;
		}
		
		int clientClass = view_as<int> ( g_iClientClass[owner] );
		int skill_lv = g_iSkill[clientClass][SKILL_ID_MEDIC_GAS][owner];
		//	치유가스를 비우지 않았을 경우 타이머 제거
		if ( skill_lv <= 0 ) {
			g_iMedicMedicGasTime[owner] = 0;
			return Plugin_Stop;
		}
		
		if ( g_iMedicMedicGasTime[owner] < 0 )
			g_iMedicMedicGasTime[owner] = 0;
		
		g_iMedicMedicGasTime[owner] ++;	//	횟수 증가
		//	횟수가 초과할 경우 타이머 제거
		if ( g_iMedicMedicGasTime[owner] > RoundFloat ( MEDIC_MEDIC_GAS_TIME ) ) {
			g_bSkillUse[clientClass][SKILL_ID_MEDIC_GAS][owner] = false;
			g_iSkillCooldownTicks[clientClass][SKILL_ID_MEDIC_GAS][owner] = g_iSkillCooldown[clientClass][SKILL_ID_MEDIC_GAS];
			g_bSkillCooldown[clientClass][SKILL_ID_MEDIC_GAS][owner] = true;
			g_iMedicMedicGasTime[owner] = 0;
			for ( int i = 0; i < sizeof ( g_bMedicGasHealTrue[] ); i ++ )
				g_bMedicGasHealTrue[owner][i] = false;
			
			return Plugin_Stop;
		}
		
		int heal_scale = skill_lv * RoundFloat ( g_fSkillAction[clientClass][SKILL_ID_MEDIC_GAS] );	//	힐 량
		
		//	치유가스 마스터리를 배웠을 경우?
		if ( g_iSkill[clientClass][SKILL_ID_MEDIC_GAS_MASTERY][owner] > 0 )
			//	기본 힐 량에서 치유가스 마스터리 량 만큼 더한다.
			heal_scale += g_iSkill[clientClass][SKILL_ID_MEDIC_GAS_MASTERY][owner] * RoundFloat ( g_fSkillAction[clientClass][SKILL_ID_MEDIC_GAS_MASTERY] );
		
		bool nano_machine_gas = false;
		//	나노머신가스를 배웠을 경우?
		if ( g_iSkill[clientClass][SKILL_ID_NANO_MACHINE_GAS][owner] > 0 ) {
			nano_machine_gas = true;
			heal_scale += g_iSkill[clientClass][SKILL_ID_NANO_MACHINE_GAS][owner] * RoundFloat ( g_fSkillAction[clientClass][SKILL_ID_NANO_MACHINE_GAS] );
		}
		
		float eye[3];
		GetClientEyePosition ( owner, eye );
		float pos[3];
		GetEntPropVector ( owner, Prop_Send, "m_vecOrigin", pos );
		pos[2] += 10.0;
		
		int color[4];
		if ( nano_machine_gas ) {
			//	나노머신가스 이펙트
			color = { 95, 0, 255, 255 };
			loringlib_CreateParticle ( owner, 0, pos, "heal_nanopulse", true, 1.0 );
		} else {
			color = { 0, 255, 0, 255 };
			//	치유가스 이펙트
			loringlib_CreateParticle ( owner, 0, pos, "heal_pulse", true, 1.0 );
		}
		
		//	거리 표시 트리거 함수
	//	loringlib_DisplayNearTrueTriggerByEntity ( entity, MEDIC_MEDIC_GAS_DISTANCE, 1.1, 5.0, 2.5, color );
		loringlib_DisplayNearTrueTriggerByEntity ( owner, MEDIC_MEDIC_GAS_DISTANCE, 1.1, 2.0, 0.0, color, true );
		
	//	int exp = GetRandomInt ( MEDIC_POISON_HEAL_REWARD_MIN, MEDIC_POISON_HEAL_REWARD_MAX );
		float target_pos[3];	//	치유받는 사람 위치 변수
		for ( int target = 0; target <= MaxClients; target ++ ) {
			if ( loringlib_IsValidClient__PlayGame ( target ) ) {
				//	가스 범위 안에 있는 타겟을 채크한다.
				if ( loringlib_GetNearTrue ( owner, target, eye, MEDIC_MEDIC_GAS_DISTANCE ) ||
					target == owner ) {
					//	가스의 주인과 타겟 둘다 인간일 경우
					if ( ZR_IsClientHuman ( target ) && ZR_IsClientHuman ( owner ) ) {
						//	나노머신가스일 경우?
						if ( nano_machine_gas ) {
							//	타겟이 감염상태일 경우 치료한다.
							if ( g_bZombiePoison[target] ) {
							
								g_bZombiePoison[target] = false;
								g_iZombiePoisonAttacker[target] = -1;
								g_fZombiePoisonTickTime[target] = 0.0;
								
								//	감염상태 치료 대상이 주인이 아닐 경우 메세지 출력
								if ( target != owner ) {
									g_bMedicGasHealTrue[owner][target] = true;
									g_iSurvivePoint[owner] += SURVIVE_POINT_MEDIC_NANO_MACHINE_GAS_POISON_HEAL;
								//	g_iExp[owner] += RoundFloat ( exp * EXP_BOOST_SCALE );
									CLIENT_MEDIC_POISON_HEAL_MSG ( owner, target )
								}
							}
						}
					
						//	힐이 가능 하면 힐을 한다.
						if ( Medic_OnPlayerHealInit ( owner, target, heal_scale, false, false ) ) {
							//	힐 대상이 내가 아닐 경우
							if ( owner != target ) {
								//	힐을 처음 받는 타겟일 경우 점수 지급
								if ( !g_bMedicGasHealTrue[owner][target] ) {
									g_iSurvivePoint[owner] += SURVIVE_POINT_MEDIC_HEALGAS_HEAL;
									g_bMedicGasHealTrue[owner][target] = true;
								}
							}
							GetClientAbsOrigin ( target, target_pos );
							PrecacheSound ( SOUND_MEDIC_MEDISHOT_SUCCESS );
							EmitAmbientSound ( SOUND_MEDIC_MEDISHOT_SUCCESS, pos, target );
						}
					}
					
					//	가스의 주인은 인간, 타겟은 좀비일 경우
					else if ( ZR_IsClientZombie ( target ) && ZR_IsClientHuman ( owner ) ) {
						//	독 데미지를 준다.
						loringlib_MakeDamage ( owner, target, MEDIC_MEDIC_GAS_ZOMBIE_DAMAGE + heal_scale, "weapon_taser", DMG_POISON );
					}
				}
			}
		}
	}
	
	return Plugin_Continue;
}

/**
 * 의료주사 치료
 * @note OnPlayerRunCmd ()에서 IN_ATTACK2를 눌렀을 때 호출됨
 */
public void onUseMedishotTarget ( int client ) {
	static char itemname[32];
	int active = loringlib_GetActiveWeaponName ( client, itemname, sizeof ( itemname ) );
	if ( StrContains ( itemname, "healthshot" ) == -1 )
		return;
	
	//	주인이 메딕이 아닐 경우
	if ( g_iClientClass[client] != ClientClass_Medic )
		return;
	
	//	아직 쿨타임이 있을 경우
	if ( g_bUseMedishot[client] ) {
		CLIENT_MEDIC_MEDISHOT_IS_NOT_END_COOLDOWN_MSG ( client )
		return;
	}
	
	int target = loringlib_GetClientAimPlayer ( client );
	//	타겟이 유효하지 않을 경우
	if ( !loringlib_IsValidClient ( target ) )
		return;
	
	//	타겟이 좀비일 경우
	if ( ZR_IsClientZombie ( target ) )
		return;
	
	float pos[3], targetpos[3];
	GetClientAbsOrigin ( client, pos );
	GetClientAbsOrigin ( target, targetpos );
	//	치유 거리 체크
	if ( GetVectorDistance ( pos, targetpos ) > MEDIC_MEDISHOT_HEAL_DISTANCE )
		return;
	
	//	체력이 만땅일 경우 메세지로 알린다.
/*	if ( loringlib_GetEntityHealth ( target ) >= loringlib_GetEntityMaxHealth ( target ) ) {
		CLIENT_MEDIC_MEDISHOT_USE_IS_TARGET_FULL_HP_MSG ( client, target )
		return;
	}	*/
	
	int clientClass = view_as <int> ( g_iClientClass[client] );
	int medishot_lv = g_iSkill[clientClass][SKILL_ID_MEDISHOT][client];
	int heal_scale = MEDISHIOT_NORMAL_HEAL_SCALE + ( ( g_iSkill[clientClass][SKILL_ID_MEDIC_GAS_MASTERY][client] * RoundFloat ( g_fSkillAction[clientClass][SKILL_ID_MEDIC_GAS_MASTERY] ) ) * 2 );
	//	의료주사 기술 레벨이 0 이상일 경우?
	if ( medishot_lv > 0 ) {
		//	타겟이 감염상태일 경우?
		if ( g_bZombiePoison[target] ) {
			CreateTimer ( MEDIC_TEAMMATE_HEAL_COOLDOWN, timerCooldownMedishot, client );
			g_bUseMedishot[client] = true;	//	쿨타임 체크
			
			g_bZombiePoison[target] = false;	//	감염상태 제거
			g_iZombiePoisonAttacker[target] = -1;
			g_fZombiePoisonTickTime[target] = 0.0;
			Medic_OnPlayerHealInit ( client, target, heal_scale, _, true );	//	치료 함수
			
			targetpos[2] += 10.0;
			PrecacheSound ( SOUND_MEDIC_MEDISHOT_SUCCESS );
			EmitAmbientSound ( SOUND_MEDIC_MEDISHOT_SUCCESS, targetpos, target );
			loringlib_CreateParticle ( target, 0, targetpos, "heal_up", true, 0.5 );
			
		//	int exp = RoundFloat ( GetRandomInt ( MEDIC_POISON_HEAL_REWARD_MIN, MEDIC_POISON_HEAL_REWARD_MAX ) * EXP_BOOST_SCALE );
		//	g_iExp[client] += exp;
			CLIENT_MEDIC_POISON_HEAL_MSG ( client, target )
			CLIENT_MEDIC_POISON_HEAL_TARGET_MSG ( target, client )
			
			if ( loringlib_GetPrimaryAmmo ( client, active ) <= 1 ) {
				loringlib_RemoveWeaponSlot ( client, CS_SLOT_MEDISHOT );
				FakeClientCommand ( client, "use weapon_knife" );
			}
			else
				loringlib_SetPrimaryAmmo ( client, active, loringlib_GetPrimaryAmmo ( client, active ) - 1 );
				
			return;
		}
	
		//	타겟에게 힐을 할 수 있을 경우?
		if ( Medic_OnPlayerHealInit ( client, target, heal_scale, _, true ) ) {
			//	쿨타임 체크 타이머
			CreateTimer ( MEDIC_TEAMMATE_HEAL_COOLDOWN, timerCooldownMedishot, client );
			g_bUseMedishot[client] = true;	//	쿨타임 체크
			
			targetpos[2] += 10.0;
			PrecacheSound ( SOUND_MEDIC_MEDISHOT_SUCCESS );
			EmitAmbientSound ( SOUND_MEDIC_MEDISHOT_SUCCESS, targetpos, target );
			
			//	의료주사기 한개 삭제 로직
			if ( loringlib_GetPrimaryAmmo ( client, active ) <= 1 ) {
				loringlib_RemoveWeaponSlot ( client, CS_SLOT_MEDISHOT );
				FakeClientCommand ( client, "use weapon_knife" );
			}
			else
				loringlib_SetPrimaryAmmo ( client, active, loringlib_GetPrimaryAmmo ( client, active ) - 1 );
				
			return;
		}
		
	} else {
		CLIENT_MEDIC_CLASS_FAIL_TARGET_HEAL ( client )
		return;
	}
}

public Action timerCooldownMedishot ( Handle timer, int client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Stop;
		
	g_bUseMedishot[client] = false;
	CLIENT_MEDIC_MEDISHOT_IS_COOLDOWN_ENDED_MSG ( client )
	return Plugin_Stop;
}

/**
 * 의료주사 특정 피해마다 주기
 * @note TakeDamage
 */
void clientClass_Medic_PlayerHurtToGiveMedishot ( int victim, int attacker, float damage, int dmgType, int active ) {
	if ( !loringlib_IsValidClient ( victim ) ||
		!loringlib_IsValidClient ( attacker ) )
		return;
	
	if ( active == -1 || !( dmgType & DMG_BULLET ) )
		return;
	
	//	메딕이 아닐 경우
	if ( g_iClientClass[attacker] != ClientClass_Medic )
		return;
	
	//	때리는 플레이어가 인간이고, 맞는 플레이어가 좀비일 경우
	if ( ZR_IsClientHuman ( attacker ) && ZR_IsClientZombie ( victim ) ) {
		int clientClass = view_as<int> ( g_iClientClass[attacker] );
		int medishot_lv = g_iSkill[clientClass][SKILL_ID_MEDISHOT][attacker];
		//	의료주사를 배웠을 경우
		if ( medishot_lv > 0 ) {
			bool is_medic_weapon = false;
			
			char weapon[32];
			GetEdictClassname ( active, weapon, sizeof ( weapon ) );
			int index = GetEntProp ( active, Prop_Send, "m_iItemDefinitionIndex" );
			switch ( index ) {
				case 23:	strcopy ( weapon, sizeof ( weapon ), "weapon_mp5sd" );
				case 60:	strcopy ( weapon, sizeof ( weapon ), "weapon_m4a1_silencer" );
				case 61:	strcopy ( weapon, sizeof ( weapon ), "weapon_usp_silencer" );
				case 63:	strcopy ( weapon, sizeof ( weapon ), "weapon_cz75a" );
			}
			
			//	무기 이름을 구해 메딕 무기인지 채크한다.
			for ( int i = 0; i < sizeof ( g_strClassWeapon_Medic ); i ++ )
				if ( StrContains ( g_strClassWeapon_Medic[i], weapon, false ) != -1 )
					is_medic_weapon = true;
			
			//	메딕 무기가 아닐 경우 return.
			if ( !is_medic_weapon )
				return;
			
			//	메딕 무기로 공격해 나온 피해량을 변수에 넣는다.
			g_iMedicCheckDamage[attacker] += RoundFloat ( damage );
			
			//	일정 피해량에 도달 했을 경우?
			if ( g_iMedicCheckDamage[attacker] >= MEDIC_POISON_GRENADE_GIVE_DMGS ) {
				g_iMedicCheckDamage[attacker] = 0;	//	초기화
				
			//	의료주사기를 준다.
			//	EquipPlayerWeapon ( attacker, GivePlayerItem ( attacker, "weapon_healthshot" ) );
				GivePlayerItem ( attacker, "weapon_decoy" );
			}
		}
	}
}

/**
 * 마비 수류탄 투척
 */
void clientClass_Medic_CreatePoisonGrenade ( int edict, const char[] classname ) {
	if ( StrContains ( classname, "decoy_projectile", false ) != -1 )
		SDKHook ( edict, SDKHook_SpawnPost, onPoisonGrenadeSpawned );
}

/**
 * 마비 수류탄이 날라갈 때
 */
public void onPoisonGrenadeSpawned ( int entity ) {
	if ( IsValidEdict ( entity ) ) {
		int owner = loringlib_GetEntityOwner2 ( entity );
		if ( loringlib_IsValidClient ( owner ) && ZR_IsClientHuman ( owner ) ) {
			if ( g_iClientClass[owner] == ClientClass_Medic )
				//	엔티티 주인이 인간일 경우
				if ( ZR_IsClientHuman ( owner ) )
					SDKHook ( entity, SDKHook_GroundEntChangedPost, onPoisonGrenadeBounce );
		}
	}
}

/**
 * 마비 수류탄이 땅에 닿았을 경우 터지게 한다.
 */
public void onPoisonGrenadeBounce ( int entity ) {
	if ( IsValidEdict ( entity ) ) {
		int owner = loringlib_GetEntityOwner2 ( entity );
		if ( loringlib_IsValidClient ( owner ) ) {
			//	엔티티 주인이 인간일 경우
			if ( ZR_IsClientHuman ( owner ) ) {
				//	엔티티가 땅에 닿았을 경우
				float pos[3];
				loringlib_GetEntityOriginEx ( entity, pos );
				spawnPoisonEntity ( owner, pos );
				
				AcceptEntityInput ( entity, "Kill" );
				return;
			}
			
			else {
				AcceptEntityInput ( entity, "Kill" );
				return;
			}
		}
			
		else {
			AcceptEntityInput ( entity, "Kill" );
			return;
		}
	}
}

void spawnPoisonEntity ( int client, float pos[3] ) {
	if ( !loringlib_IsValidClient ( client ) || g_iClientClass[client] != ClientClass_Medic )
		return;
	
	PrecacheSound ( "weapons/smokegrenade/sg_explode.wav" );
	EmitAmbientSound ( "weapons/smokegrenade/sg_explode.wav", pos );
	
	static float durations;
	durations = MEDIC_POISON_GRENADE_DURATION + float ( g_iSkill[view_as<int> ( ClientClass_Medic )][SKILL_ID_MEDISHOT][client] ) * g_fSkillAction[view_as<int> ( ClientClass_Medic )][SKILL_ID_MEDISHOT];
	
	loringlib_CreateParticleEx ( 0, 0, pos, NULL_VECTOR, "medic_pgrenade", false, durations );
	
	DataPack data;
	CreateDataTimer ( 0.1, timerPoisonGrenadeInit, data, TIMER_REPEAT | TIMER_FLAG_NO_MAPCHANGE );
	data.WriteFloat ( GetGameTime () + durations );
	data.WriteFloat ( pos[0] );
	data.WriteFloat ( pos[1] );
	data.WriteFloat ( pos[2] );
	
	DataPack data2;
	CreateDataTimer ( 0.5, timerPoisonGrenadeDamage, data2, TIMER_REPEAT | TIMER_FLAG_NO_MAPCHANGE );
	data2.WriteFloat ( GetGameTime () + durations );
	data2.WriteCell ( client );
	data2.WriteFloat ( pos[0] );
	data2.WriteFloat ( pos[1] );
	data2.WriteFloat ( pos[2] );
}

/**
 * 독 스폰 
 */
public Action timerPoisonGrenadeInit ( Handle timer, DataPack data ) {
	data.Reset ();
	static float durations;
	static float pos[3];
	durations = data.ReadFloat ();
	pos[0] = data.ReadFloat ();
	pos[1] = data.ReadFloat ();
	pos[2] = data.ReadFloat ();

	if ( GetGameTime () >= durations )
		return Plugin_Stop;
	
	float targetpos[3];
	for ( int target = 1; target <= MaxClients; target ++ ) {
		if ( loringlib_IsValidClient__PlayGame ( target ) ) {
			if ( !ZR_IsClientHuman ( target ) ) {
				GetClientAbsOrigin ( target, targetpos );
				if ( GetVectorDistance ( pos, targetpos ) <= MEDIC_POISON_GRENADE_RADIUS ) {
					SetEntPropFloat ( target, Prop_Send, "m_flVelocityModifier", 0.15 );
				}
			}
		}
	}
	
	return Plugin_Continue;
}

/**
 * 독 스폰 
 */
public Action timerPoisonGrenadeDamage ( Handle timer, DataPack data ) {
	data.Reset ();
	static float durations;
	static int client;
	static float pos[3];
	durations = data.ReadFloat ();
	client = data.ReadCell ();
	pos[0] = data.ReadFloat ();
	pos[1] = data.ReadFloat ();
	pos[2] = data.ReadFloat ();

	if ( GetGameTime () >= durations )
		return Plugin_Stop;
	
	if ( !loringlib_IsValidClient ( client ) )
		client = 0;
	
	float targetpos[3];
	for ( int target = 1; target <= MaxClients; target ++ ) {
		if ( loringlib_IsValidClient__PlayGame ( target ) ) {
			if ( !ZR_IsClientHuman ( target ) ) {
				GetClientAbsOrigin ( target, targetpos );
				if ( GetVectorDistance ( pos, targetpos ) <= MEDIC_POISON_GRENADE_RADIUS ) {
				//	SetEntPropFloat ( target, Prop_Send, "m_flVelocityModifier", 0.15 );
					loringlib_MakeDamage ( client, target, 5, "weapon_c4", DMG_POISON );
				}
			}
		}
	}
	
	return Plugin_Continue;
}

/**
 * 독성주사 특정 피해마다 쏘기
 * @note player_hurt에서
 */
void clientClass_Medic_PlayerhurtToFireMedicPoison ( int victim, int attacker, float damage, int dmgType, int active, float dmgPos[3] ) {
	if ( !loringlib_IsValidClient ( victim ) ||
		!loringlib_IsValidClient ( attacker ) )
		return;
	
	//	무기가 유효하지 않거나, 피해 타입이 불릿이 아닐 경우
	if ( active == -1 || !( dmgType & DMG_BULLET ) )
		return;
	
	//	메딕이 아닐 경우
	if ( g_iClientClass[attacker] != ClientClass_Medic )
		return;
	
	//	때리는 플레이어는 인간이고, 맞는 플레이어는 인간이 아닐 경우
	if ( ZR_IsClientHuman ( attacker ) && !ZR_IsClientHuman ( victim ) ) {
		int clientClass = view_as<int> ( g_iClientClass[attacker] );
		int poisonshot_lv = g_iSkill[clientClass][SKILL_ID_POSIONSHOT][attacker];
		//	독성주사를 배웠을 경우
		if ( poisonshot_lv > 0 ) {
			int poisonshot_action = ( poisonshot_lv * RoundFloat ( g_fSkillAction[clientClass][SKILL_ID_POSIONSHOT] ) );
			bool is_medic_weapon = false;
			
			char weapon[32];
			GetEdictClassname ( active, weapon, sizeof ( weapon ) );
			int index = GetEntProp ( active, Prop_Send, "m_iItemDefinitionIndex" );
			switch ( index ) {
				case 23:	strcopy ( weapon, sizeof ( weapon ), "weapon_mp5sd" );
				case 60:	strcopy ( weapon, sizeof ( weapon ), "weapon_m4a1_silencer" );
				case 61:	strcopy ( weapon, sizeof ( weapon ), "weapon_usp_silencer" );
				case 63:	strcopy ( weapon, sizeof ( weapon ), "weapon_cz75a" );
			}
			
			//	무기 이름을 구해 메딕 무기인지 채크한다.
			for ( int i = 0; i < sizeof ( g_strClassWeapon_Medic ); i ++ )
				if ( StrContains ( g_strClassWeapon_Medic[i], weapon, false ) != -1 )
					is_medic_weapon = true;
			
			//	메딕 무기가 아닐 경우 return.
			if ( !is_medic_weapon )
				return;
				
			//	메딕 무기로 공격해 나온 피해량을 변수에 넣는다.
			g_iMedicCheckDamage[attacker] += RoundFloat ( damage );
			
			//	일정 피해량에 도달 했을 경우?
			if ( g_iMedicCheckDamage[attacker] >= MEDIC_POISONSHOT_GIVEN_DAMAGE ) {
				g_iMedicCheckDamage[attacker] = 0;	//	초기화
				
				//	레이저 Temperate Effects
				float start[3], ang[3], end[3];
				GetClientEyePosition ( attacker, start );
				GetClientEyeAngles ( attacker, ang );
				TR_TraceRayFilter ( start, ang, MASK_SOLID, RayType_Infinite, hitSelf, attacker );
				if ( TR_DidHit ( null ) )
					TR_GetEndPosition ( end );
				
				TE_SetupBeamPoints ( start, dmgPos, PrecacheModel ( "materials/sprites/laser.vmt" ), 0, 1, 1, 0.8, 1.1, 1.1, 1, 0.38, { 165, 102, 255, 255 }, 1 );
				TE_SendToAll ();

			//	loringlib_IgniteEntityEx ( victim, attacker, poisonshot_action, 10.0, 0.5, "weapon_taser" );
				
				g_iMedicPoison[attacker][victim] += 5;
				if ( g_iMedicPoison[attacker][victim] > 5 )
					g_iMedicPoison[attacker][victim] = 5;
				
				DataPack dp;
				CreateDataTimer ( 1.0, timerMedicPosionDamage, dp, TIMER_REPEAT );
				dp.WriteCell ( victim );
				dp.WriteCell ( attacker );
				dp.WriteCell ( poisonshot_action );
			}
		}
	}
}

/**
 * 치유탄 표시
 * @note OnGameFrame ()에서 호출됨
 */
public void clientClass_Medic_DisplayMedicBullet () {
	for ( int id = 1; id <= MaxClients; id ++ ) {
		if ( !loringlib_IsValidClient__PlayGame ( id ) )
			continue;
	
		if ( g_iClientClass[id] != ClientClass_Medic )
			continue;
			
		if ( ZR_IsClientZombie ( id ) )
			continue;
		
		if ( g_iSkill[g_iClientClass[id]][SKILL_ID_MEDIC_BULLET][id] <= 0 )
			continue;
		
//		char weapon[32];
//		int active = loringlib_GetActiveWeaponName ( id, weapon, sizeof ( weapon ) );
//		if ( active != INVALID_ENT_REFERENCE ) {
//			if ( StrEqual ( weapon, "weapon_ssg08" ) ) {
		SetHudTextParamsEx ( 0.61, 0.96, 0.05, { 255, 0, 0, 255 }, { 255, 0, 0, 255 }, 0, 0.01, 0.01, 0.05 );
		ShowSyncHudText ( id, g_hHudSyncArray[HUDSYNC_MEDIC_BULLET], "치유탄: %d발", g_iMedicBullet[id] );
//			}
//		}
	}
}

/**
 * HP 증가
 * @note stat.inc의 SpawnPost에서
 */
public void clientClass_Medic_HPUpgrade ( int client ) {
	if ( ZR_IsClientZombie ( client ) )
		return;
		
	if ( g_iClientClass[client] != ClientClass_Medic )
		return;
		
	int clientClass = view_as <int> ( g_iClientClass[client] );
	int hp_upgrade_lv = g_iSkill[clientClass][SKILL_ID_HP_UPGRADE][client];
	int hp_upgrade_action = MEDIC_HP_UPGRADE_DEFAULT_HEALTH + ( hp_upgrade_lv * RoundFloat ( g_fSkillAction[clientClass][SKILL_ID_HP_UPGRADE] ) );
	if ( hp_upgrade_lv <= 0 )
		return;
		
	loringlib_SetEntityHealth ( client, loringlib_GetEntityHealth ( client ) + hp_upgrade_action );
	loringlib_SetEntityMaxHealth ( client, loringlib_GetEntityHealth ( client ) );
}

/**
 * 치유탄 발사
 *
 * @param victim		맞은 플레이어
 * @param client		쏘는 플레이어
 * @param weapon		무기
 * @noreturn
 */
public void clientClass_Medic_MedicBulletFired ( int victim, int client, int weapon ) {
	//	병과가 Medic이 아닐 경우
	if ( g_iClientClass[client] != ClientClass_Medic )
		return;
	
	//	죽어있을 경우
	if ( !IsPlayerAlive ( client ) || !IsPlayerAlive ( victim ) )
		return;
		
	//	좀비일 경우
	if ( ZR_IsClientZombie ( client ) )
		return;
	
	if ( weapon == INVALID_ENT_REFERENCE || weapon < 0 )
		return;
	
	static char weaponname[32];
	GetEdictClassname ( weapon, weaponname, sizeof ( weaponname ) );
	int index = GetEntProp ( weapon, Prop_Send, "m_iItemDefinitionIndex" );
	switch ( index ) {
		case 23:	strcopy ( weaponname, sizeof ( weaponname ), "weapon_mp5sd" );
		case 60:	strcopy ( weaponname, sizeof ( weaponname ), "weapon_m4a1_silencer" );
		case 61:	strcopy ( weaponname, sizeof ( weaponname ), "weapon_usp_silencer" );
		case 63:	strcopy ( weaponname, sizeof ( weaponname ), "weapon_cz75a" );
	}
	
	//	메딕의 주무기가 아닐 경우
	if ( !StrEqual ( weaponname, "weapon_ssg08", false ) &&
		!StrEqual ( weaponname, "weapon_mp5sd", false ) )
		return;
	
	int clientClass = view_as <int> ( g_iClientClass[client] );
	int medic_bullet_lv = g_iSkill[clientClass][SKILL_ID_MEDIC_BULLET][client];
	//	기술을 안배웠을 경우
	if ( medic_bullet_lv <= 0 )
		return;
		
	//	기술 사용중이 아닐 경우
	if ( !g_bSkillUse[clientClass][SKILL_ID_MEDIC_BULLET][client] )
		return;
		
	//	치료탄 갯수가 없을 경우
	if ( g_iMedicBullet[client] <= 0 )
		return;
	
	//	레이저 Temperate Effects
	float start[3], ang[3], end[3];
	GetClientEyePosition ( client, start );
	GetClientEyeAngles ( client, ang );
	TR_TraceRayFilter ( start, ang, MASK_SOLID, RayType_Infinite, hitSelf, client );
	if ( TR_DidHit ( null ) )
		TR_GetEndPosition ( end );
	
	//	치유탄 엑션 값
	int medic_bullet_aciton = medic_bullet_lv * RoundFloat ( g_fSkillAction[clientClass][SKILL_ID_MEDIC_BULLET] );
	
	//	치유탄 마스터리를 배웠을 경우
	if ( g_iSkill[clientClass][SKILL_ID_MEDIC_BULLET_MASTERY][client] > 0 )
		//	치유탄 마스터리 엑션 값 추가
		medic_bullet_aciton += g_iSkill[clientClass][SKILL_ID_MEDIC_BULLET_MASTERY][client] * RoundFloat ( g_fSkillAction[clientClass][SKILL_ID_MEDIC_BULLET_MASTERY] );
	
	//	같은 팀일 경우
	//	치료탄 발사 후 아군 치료
	if ( ZR_IsClientHuman ( client ) && ZR_IsClientHuman ( victim ) ) {
		float targetpos[3];
		GetClientAbsOrigin ( victim, targetpos );
		//	타겟이 감염상태일 경우?
		if ( g_bZombiePoison[victim] ) {
		//	CreateTimer ( MEDIC_TEAMMATE_HEAL_COOLDOWN, timerCooldownMedishot, client );
		//	g_bUseMedishot[client] = true;	//	쿨타임 체크
			
			g_bZombiePoison[victim] = false;	//	감염상태 제거
			g_iZombiePoisonAttacker[victim] = -1;
			g_fZombiePoisonTickTime[victim] = 0.0;
			if ( victim != client )	//	행동 점수
				g_iSurvivePoint[client] += SURVIVE_POINT_MEDIC_MEDIC_BULLET_HEAL;
			Medic_OnPlayerHealInit ( client, victim, medic_bullet_aciton, _, false );	//	치료 함수
			
			targetpos[2] += 10.0;
			PrecacheSound ( SOUND_MEDIC_MEDISHOT_SUCCESS );
			EmitAmbientSound ( SOUND_MEDIC_MEDISHOT_SUCCESS, targetpos, victim );
			loringlib_CreateParticle ( victim, 0, targetpos, "heal_up", true, 0.5 );
			
			//	감염상태치료 시 주는 경험치량 설정
		//	int exp = RoundFloat ( GetRandomInt ( MEDIC_POISON_HEAL_REWARD_MIN, MEDIC_POISON_HEAL_REWARD_MAX ) * EXP_BOOST_SCALE );
		//	g_iExp[client] += exp;
			CLIENT_MEDIC_POISON_HEAL_MSG ( client, victim )
			CLIENT_MEDIC_POISON_HEAL_TARGET_MSG ( victim, client )
			
			g_iMedicBullet[client] --;
			g_iSkillCooldownTicks[clientClass][SKILL_ID_MEDIC_BULLET][client] = g_iSkillCooldown[clientClass][SKILL_ID_MEDIC_BULLET];
			g_bSkillCooldown[clientClass][SKILL_ID_MEDIC_BULLET][client] = true;
			g_bSkillUse[clientClass][SKILL_ID_MEDIC_BULLET][client] = false;

			TE_SetupBeamPoints ( start, end, PrecacheModel ( "materials/sprites/laser.vmt" ), 0, 1, 1, 0.8, 1.1, 1.1, 1, 0.38, { 0, 255, 0, 255 }, 1 );
			TE_SendToAll ();
			
			//	나노강화치유탄
			if ( g_iSkill[clientClass][SKILL_ID_NANO_UPGRADE_MEDIC_BULLET][client] > 0 ) {
				if ( !g_bMedicBoost[victim] ) {
					float pos[3];
					GetClientEyePosition ( victim, pos );
					loringlib_CreateParticle ( victim, 0, pos, "heal_nanoboost", false, MEDIC_NANO_UPGRADE_BULLET_BOOST_TIME, true );	//	화면 파티클
				
					g_bMedicBoost[victim] = true;
					SDKHook ( victim, SDKHook_PostThinkPost, Medic_PostThinkToMedicBoost );
					CreateTimer ( MEDIC_NANO_UPGRADE_BULLET_BOOST_TIME, timerMedicBoostCooldown, victim );
					
					CLIENT_MEDIC_MEDIC_BOOST_ENABLE_MSG ( client, victim )
					CLIENT_MEDIC_MEDIC_BOOST_ENABLE_TARGET_MSG ( victim, client )
					
					TE_SetupBeamPoints ( start, end, PrecacheModel ( "materials/sprites/laser.vmt" ), 0, 1, 1, 0.8, 1.1, 1.1, 1, 0.38, { 90, 200, 230, 255 }, 1 );
					TE_SendToAll ();
				}
				
				else
					CLIENT_MEDIC_TARGET_IS_NOW_BOOST_MSG ( client, victim )
			}
			
			return;
		}
	
		//	타겟에게 힐을 할 수 있을 경우?
		if ( Medic_OnPlayerHealInit ( client, victim, medic_bullet_aciton, _, true ) ) {
			Medic_OnTeammateHealToSelfHeal ( client, medic_bullet_aciton );
			
			//	쿨타임 체크 타이머
		//	CreateTimer ( MEDIC_TEAMMATE_HEAL_COOLDOWN, timerCooldownMedishot, client );
		//	g_bUseMedishot[client] = true;	//	쿨타임 체크
			
			targetpos[2] += 10.0;
			PrecacheSound ( SOUND_MEDIC_MEDISHOT_SUCCESS );
			EmitAmbientSound ( SOUND_MEDIC_MEDISHOT_SUCCESS, targetpos, victim );

			g_iMedicBullet[client] --;
			g_iSkillCooldownTicks[clientClass][SKILL_ID_MEDIC_BULLET][client] = g_iSkillCooldown[clientClass][SKILL_ID_MEDIC_BULLET];
			g_bSkillCooldown[clientClass][SKILL_ID_MEDIC_BULLET][client] = true;
			g_bSkillUse[clientClass][SKILL_ID_MEDIC_BULLET][client] = false;

			TE_SetupBeamPoints ( start, end, PrecacheModel ( "materials/sprites/laser.vmt" ), 0, 1, 1, 0.8, 1.1, 1.1, 1, 0.38, { 0, 255, 0, 255 }, 1 );
			TE_SendToAll ();
			
			//	나노강화치유탄
			if ( g_iSkill[clientClass][SKILL_ID_NANO_UPGRADE_MEDIC_BULLET][client] > 0 ) {
				if ( !g_bMedicBoost[victim] ) {
					g_bMedicBoost[victim] = true;
					SDKHook ( victim, SDKHook_PostThinkPost, Medic_PostThinkToMedicBoost );
					CreateTimer ( MEDIC_NANO_UPGRADE_BULLET_BOOST_TIME, timerMedicBoostCooldown, victim );
					
					CLIENT_MEDIC_MEDIC_BOOST_ENABLE_MSG ( client, victim )
					CLIENT_MEDIC_MEDIC_BOOST_ENABLE_TARGET_MSG ( victim, client )
					
					TE_SetupBeamPoints ( start, end, PrecacheModel ( "materials/sprites/laser.vmt" ), 0, 1, 1, 0.8, 1.1, 1.1, 1, 0.38, { 90, 200, 230, 255 }, 1 );
					TE_SendToAll ();
				}
				
				else
					CLIENT_MEDIC_TARGET_IS_NOW_BOOST_MSG ( client, victim )
			}
			
			return;
		}
		
		//	이미 타겟이 체력이 최대치일 경우
		else {
			//	나노강화치유탄을 배웠을 경우 사용하게 한다.
			if ( g_iSkill[clientClass][SKILL_ID_NANO_UPGRADE_MEDIC_BULLET][client] > 0 ) {
				if ( !g_bMedicBoost[victim] ) {
					Medic_OnTeammateHealToSelfHeal ( client, medic_bullet_aciton );
					g_iMedicBullet[client] --;	//	치유탄 -1
					g_iSkillCooldownTicks[clientClass][SKILL_ID_MEDIC_BULLET][client] = g_iSkillCooldown[clientClass][SKILL_ID_MEDIC_BULLET];
					g_bSkillCooldown[clientClass][SKILL_ID_MEDIC_BULLET][client] = true;
					g_bSkillUse[clientClass][SKILL_ID_MEDIC_BULLET][client] = false;
				
					g_bMedicBoost[victim] = true;
					g_iMedicBoostOwner[victim] = client;
					g_fMedicBoostOnDamage[victim] = 0.0;
					SDKHook ( victim, SDKHook_PostThinkPost, Medic_PostThinkToMedicBoost );
					CreateTimer ( MEDIC_NANO_UPGRADE_BULLET_BOOST_TIME, timerMedicBoostCooldown, victim );
					
					CLIENT_MEDIC_MEDIC_BOOST_ENABLE_MSG ( client, victim )
					CLIENT_MEDIC_MEDIC_BOOST_ENABLE_TARGET_MSG ( victim, client )
					
					TE_SetupBeamPoints ( start, end, PrecacheModel ( "materials/sprites/laser.vmt" ), 0, 1, 1, 0.8, 1.1, 1.1, 1, 0.38, { 90, 200, 230, 255 }, 1 );
					TE_SendToAll ();
				}
				
				else
					CLIENT_MEDIC_TARGET_IS_NOW_BOOST_MSG ( client, victim )
			}
		}
	}
}

public bool hitSelf ( int entity, int contentsMask, any data ) {
	if ( entity == data )	return false;
	return true;
}

/**
 * 나노 강회 치유탄 재장전 속도
 */
stock float Medic_MedicBoostReloadSpeed ( int client ) {
	if ( g_bMedicBoost[client] ) {
		return MEDIC_MEDIC_BOOST_RELOAD_SPEED;
	}
	
	return 0.0;
}

/**
 * 나노 강화 치유탄 공격속도
 */
public void Medic_PostThinkToMedicBoost ( int client ) {
	if ( loringlib_IsValidClient ( client ) ) {
		//	나노강화치유탄 효과 도중에 좀비로 변했을 경우
		if ( ZR_IsClientZombie ( client ) ) {
			g_bMedicBoost[client] = false;
			SDKUnhook ( client, SDKHook_PostThinkPost, Medic_PostThinkToMedicBoost );
			return;
		}
		
		//	활성화
		if ( g_bMedicBoost[client] ) {
			char weapon[32];
			int active = loringlib_GetActiveWeaponName ( client, weapon, sizeof ( weapon ) );
			if ( active != -1 ) {
				
				if ( StrContains ( weapon, "grenade", false ) != -1 ||
					StrEqual ( weapon, "weapon_flashbang", false ) ||
					StrEqual ( weapon, "weapon_snowball", false ) ||
					StrEqual ( weapon, "weapon_decoy", false ) ||
					StrEqual ( weapon, "weapon_molotov", false ) ||
					StrEqual ( weapon, "weapon_incgrenade", false ) ||
					StrEqual ( weapon, "weapon_healthshot", false ) ||
					StrEqual ( weapon, "weapon_c4", false ) ||
					StrEqual ( weapon, "weapon_tagrenade", false ) ||
					StrContains ( weapon, "knife", false ) != -1 )
					return;
				
				float nextPrimaryAtt = GetEntPropFloat ( active, Prop_Send, "m_flNextPrimaryAttack" );
				
				nextPrimaryAtt -= GetGameTime ();
				nextPrimaryAtt *= 0.77777;
				nextPrimaryAtt += GetGameTime ();
				
				SetEntPropFloat ( active, Prop_Send, "m_flNextPrimaryAttack", nextPrimaryAtt );

				SetEntPropFloat ( active, Prop_Send, "m_fAccuracyPenalty", 0.0 );
				SetEntPropVector ( client, Prop_Send, "m_aimPunchAngle", NULL_VECTOR );
				SetEntPropVector ( client, Prop_Send, "m_aimPunchAngleVel", NULL_VECTOR );
				SetEntPropVector ( client, Prop_Send, "m_viewPunchAngle", NULL_VECTOR );
				return;
			}
		}
		
		g_bMedicBoost[client] = false;
		SDKUnhook ( client, SDKHook_PostThinkPost, Medic_PostThinkToMedicBoost );
	}
}

/**
 * 나노강화치유탄 데미지량 마다 행동점수 주기
 * @note TakeDamage에서
 */
stock void Medic_TakeDamageOnMedicBoostOwnerSurvivePoint ( int client, float dmg ) {
	if ( g_bMedicBoost[client] ) {
		int owner = g_iMedicBoostOwner[client];
		if ( loringlib_IsValidClient__PlayGame ( owner ) && ZR_IsClientHuman ( owner ) ) {
			g_fMedicBoostOnDamage[client] += dmg;
			if ( g_fMedicBoostOnDamage[client] >= MEDIC_MEDIC_BOOST_SURVIVE_POINT_DAMAGE_SCALE ) {
				g_fMedicBoostOnDamage[client] = 0.0;
				g_iSurvivePoint[owner] += SURVIVE_POINT_MEDIC_BOOST_TEAMMATE_DAMAGE;
			}
		}
	}
}

public Action timerMedicBoostCooldown ( Handle timer, any client ) {
	if ( loringlib_IsValidClient ( client ) )
		g_bMedicBoost[client] = false;
		
	return Plugin_Stop;
}

public Action timerMedicPosionDamage ( Handle timer, DataPack pack ) {
	pack.Reset ();
	int victim = pack.ReadCell ();
	int attacker = pack.ReadCell ();
	int damage = pack.ReadCell ();
	
	if ( loringlib_IsValidClient ( victim ) ) {
		if ( !loringlib_IsValidClient__PlayGame ( attacker ) ) {
			g_iMedicPoison[attacker][victim] = 0;
			return Plugin_Stop;
		}
		
		if ( g_iMedicPoison[attacker][victim] > 0 ) {
			float pos[3];
			GetClientEyePosition ( victim, pos );
			pos[2] -= 20.0;
			loringlib_CreateParticle ( victim, 0, pos, "heal_poisonshot", true, 1.0 );
			loringlib_MakeDamage ( attacker, victim, damage, "weapon_taser", DMG_POISON );
			g_iMedicPoison[attacker][victim] --;
		}
		
		if ( g_iMedicPoison[attacker][victim] <= 0 ) {
			g_iMedicPoison[attacker][victim] = 0;
			return Plugin_Stop;
		}
	}
	
	return Plugin_Continue;
}

/** 
 * 메딕이 타겟을 힐할 때 사용하는 함수
 *
 * @param client			메딕 클라이언트
 * @param target			타겟
 * @param heal_scale		총 힐량
 * @param message			메세지 출력 여부
 */
stock bool Medic_OnPlayerHealInit ( int client, int target, int heal_scale, bool message = true, bool given_exps = false ) {
	if ( g_iClientClass[client] == ClientClass_Medic ) {
		//	치료해주는 대상과 채료해주는 사람이 인간이여야만 한다.
		if ( ZR_IsClientHuman ( client ) && ZR_IsClientHuman ( target ) ) {
			//	타겟이 최대 체력일 경우?
			if ( loringlib_GetEntityHealth ( target ) >= loringlib_GetEntityMaxHealth ( target ) ) {
				if ( message )
					CLIENT_MEDIC_MEDISHOT_USE_IS_TARGET_FULL_HP_MSG ( client, target )
				return false;
			}
			
			//	치료할 체력의 여유가 있다면?
			loringlib_SetEntityHealth ( target, loringlib_GetEntityHealth ( target ) + heal_scale );	//	치료 해준다.
			
			if ( client != target )	//	자신에게 치료하는게 아닐 경우 행동 점수를 준다.
				g_iSurvivePoint[client] += SURVIVE_POINT_MEDIC_HEALTH_HEAL;
			
			float pos[3];
			GetClientEyePosition ( target, pos );
			loringlib_CreateParticle ( target, 0, pos, "heal_healscreen", false, 1.0, true );	//	화면 파티클
			
			if ( message ) {
				CLIENT_MEDIC_CLASS_MEDIC_HEAL ( client, target, heal_scale )
				CLIENT_MEDIC_CLASS_MEDIC_HEAL_TARGET ( target, client, heal_scale )
			}
			
			int survivePoints = RoundFloat ( heal_scale / 10.0 );
			
			//	치료 후 체력이 최대 체력 보다 많을 경우?
			if ( loringlib_GetEntityHealth ( target ) > loringlib_GetEntityMaxHealth ( target ) ) {
				//	치유된 량 만큼만 점수를 준다.
				survivePoints = RoundFloat ( ( heal_scale - ( loringlib_GetEntityHealth ( target ) - loringlib_GetEntityMaxHealth ( target ) ) ) / 10.0 );
				loringlib_SetEntityHealth ( target, loringlib_GetEntityMaxHealth ( target ) );	//	최대 체력으로 바꾼다.
			}
			
			if ( given_exps )
				g_iSurvivePoint[client] += survivePoints;
			
			return true;
		}
	}
	
	return false;
}

/**
 * 자기 치료
 *
 * @param client
 * @param heal_scale_original
 */
stock void Medic_OnTeammateHealToSelfHeal ( int client, int heal_scale_original ) {
	if ( loringlib_IsValidClient__PlayGame ( client ) ) {
		int heal = RoundFloat ( heal_scale_original / 1.5 );
		if ( heal < 10 )
			heal = 10;
		
		if ( loringlib_GetEntityHealth ( client ) < loringlib_GetEntityMaxHealth ( client ) )
			loringlib_SetEntityHealth ( client, loringlib_GetEntityHealth ( client ) + heal );
		
		if ( loringlib_GetEntityHealth ( client ) > loringlib_GetEntityMaxHealth ( client ) )
			loringlib_SetEntityHealth ( client, loringlib_GetEntityMaxHealth ( client ) );
	}
}