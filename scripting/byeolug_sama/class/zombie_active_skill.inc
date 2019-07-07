#define ZOMBIE_POISON_DAMAGE		1	/**< 독 데미지 량 */
#define ZOMBIE_POISON_TICKTIME		1.0	/**< 독 데미지 시간 */

#define ZOMBIE_SPAWN_SKILL_COOLDOWN		10.0	/**< 좀비로 바뀔 때 기술 쿨 다운 */

#define ZOMBIE_ZOMBIE_BOMB_MODEL			"models/crow.mdl"
#define ZOMBIE_ZOMBIE_BOMB_EXPLODE_SOUND	"qufnr/zombie_department/zombie_bomb_explode.mp3"
#define ZOMBIE_ZOMBIE_BOMB_THROW_SOUND		"qufnr/zombie_department/zombie_bomb_throw.mp3"
#define ZOMBIE_ZOMBIE_BOMB_IDLE_SOUND		"qufnr/zombie_department/zombie_bomb_idle%d.mp3"
#define ZOMBIE_ZOMBIE_BOMB_IDLE_MIN		1
#define ZOMBIE_ZOMBIE_BOMB_IDLE_MAX		3
#define ZOMBIE_ZOMBIE_BOMB_THROW_SPEED		650.0	/**< 좀비 폭탄 날라가는 속도 */
#define ZOMBIE_ZOMBIE_BOMB_LIFETIME			2.5		/**< 좀비 폭탄 폭발 시간 */
#define ZOMBIE_ZOMBIE_BOMB_EXPLODE_RADIUS	200.0	/**< 좀비 폭탄 피해 거리 */
#define ZOMBIE_ZOMBIE_BOMB_KNOCKBACK_SCALE			300.0	/**< 좀비 폭탄 넉백 크기 (일반 좀비) */
#define ZOMBIE_ZOMBIE_BOMB_KNOCKBACK_SCALE_DUCK		250.0	/**< 좀비 폭탄 넉백 크기 (일반 좀비) */
#define ZOMBIE_ZOMBIE_BOMB_KNOCKBACK_SCALE_EX		400.0	/**< 좀비 폭탄 넉백 크기 (숙주 좀비) */
#define ZOMBIE_ZOMBIE_BOMB_KNOCKBACK_SCALE_DUCK_EX	300.0	/**< 좀비 폭탄 넉백 크기 (숙주 좀비) */
#define ZOMBIE_ZOMBIE_BOMB_PROP_DAMAGE_MIN			100
#define ZOMBIE_ZOMBIE_BOMB_PROP_DAMAGE_MAX			200
#define ZOMBIE_ZOMBIE_BOMB_PROP_DAMAGE_MIN_EX		200
#define ZOMBIE_ZOMBIE_BOMB_PROP_DAMAGE_MAX_EX		400

#define ZOMBIE_FASTSPEED_DURATION		5.0		/**< 질주 지속 시간 */
#define ZOMBIE_FASTSPEED_DURATION_EX	8.0		/**< 질주 지속 시간 */
#define ZOMBIE_FASTSPEED_SCALE			0.25		/**< 질주 시 증가하는 속도 량 */
#define ZOMBIE_FASTSPEED_SCALE_EX		0.32		/**< 질주 시 증가하는 속도 량 (숙주) */
#define ZOMBIE_FASTSPEED_GRAVITY_SCALE	0.2			/**< 질주 시 증가하는 중력 량 */
#define ZOMBIE_FASTSPEED_GRAVITY_SCALE_EX		0.24	/**< 질주 시 증가하는 중력 량 (숙주) */
#define ZOMBIE_FASTSPEED_DIFF_SCALE				0.06	/**< 질주 속도 증가 값 (난이도) */
#define ZOMBIE_FASTSPEED_DIFF_GRAVITY_SCALE		0.04	/**< 질주 중력 증가 값 (난이도) */

#define ZOMBIE_FASTJUMP_SOUND			"qufnr/zombie_department/zombie_fastjump.mp3"
#define ZOMBIE_FASTJUMP_SCALE			500.0		/**< 좀비 도약 (일반 좀비) */
#define ZOMBIE_FASTJUMP_SCALE_EX		700.0		/**< 좀비 도약 (숙주 좀비) */
#define ZOMBIE_FASTJUMP_DIFF_SCALE		40.0		/**< 도약 증가 값 (난이도) */

#define ZOMBIE_HEAVY_IMMUNITY_DURATION		5.0		/**< 좀비 면역 (일반 좀비) */
#define ZOMBIE_HEAVY_IMMUNITY_DURATION_EX	5.0		/**< 좀비 면역 (숙주 좀비) */
#define ZOMBIE_HEAVY_IMMUNITY_KNOCKBACK_SCALE		1.3		/**< 좀비 면역 넉백 (일반 좀비) */
#define ZOMBIE_HEAVY_IMMUNITY_KNOCKBACK_SCALE_EX	1.8		/**< 좀비 면역 넉백 (숙주 좀비) */
#define ZOMBIE_HEAVY_IMMUNITY_HEALTH_SCALE			3000	/**< 좀비 면역 체력 (일반 좀비) */
#define ZOMBIE_HEAVY_IMMUNITY_HEALTH_SCALE_EX		5000	/**< 좀비 면역 체력 (숙주 좀비) */

/**
 * @section 쿨다운
 */
#define ZOMBIE_ZOMBIE_BOMB_COOLDOWN			20.0	/**< 좀비 폭탄 쿨다운 */
#define ZOMBIE_ZOMBIE_BOMB_COOLDOWN_EX		15.0
#define ZOMBIE_FASTSPEED_COOLDOWN			25.0	/**< 좀비 질주 쿨다운 */
#define ZOMBIE_FASTSPEED_COOLDOWN_EX		20.0
#define ZOMBIE_FASTJUMP_COOLDOWN			25.0	/**< 좀비 도약 쿨다운 */
#define ZOMBIE_FASTJUMP_COOLDOWN_EX			20.0
#define ZOMBIE_HEAVY_IMMUNITY_COOLDOWN		20.0	/**< 좀비 이뮤니티 쿨다운 */
#define ZOMBIE_HEAVY_IMMUNITY_COOLDOWN_EX	15.0
#define ZOMBIE_MOTHERZOMBIE_BLOODYFIELD_COOLDOWN	30.0	/**< 하울링 쿨다운 */

#define		ZBUY_ZOMBIE_UPGRADE_PRICE		2000		/**< 좀비 강화 가격 */

#define		ZBUY_ZOMBIE_UPGRADE_NORMALZOMBIE_HP			2000
#define		ZBUY_ZOMBIE_UPGRADE_NORMALZOMBIE_KNOCKBACK	0.08
#define		ZBUY_ZOMBIE_UPGRADE_FASTZOMBIE_HP			1000
#define		ZBUY_ZOMBIE_UPGRADE_FASTZOMBIE_KNOCKBACK	0.04
#define		ZBUY_ZOMBIE_UPGRADE_HEAVYZOMBIE_HP			3500
#define		ZBUY_ZOMBIE_UPGRADE_HEAVYZOMBIE_KNOCKBACK	0.12
#define		ZBUY_ZOMBIE_UPGRADE_NEMESIS_HP				5000
#define		ZBUY_ZOMBIE_UPGRADE_NEMESIS_KNOCKBACK		0.15

static bool g_bUseZombieSkill[MAXPLAYERS + 1] = { false, ... };
static int g_iZombieSkillCooldown[MAXPLAYERS + 1] = { 0, ... };

Handle g_hReceiveDollarTimer = null;
int g_iReceiveDollarTime = 0;

/**
 * 좀비 기술 관련 모델 및 소리 프리캐싱
 * @note OnMapStart
 */
void ZOMBIESKILL_ZombieSkillPrecacheAll () {
	PrecacheModel ( ZOMBIE_ZOMBIE_BOMB_MODEL, true );
	
	PrecacheSound ( ZOMBIE_ZOMBIE_BOMB_EXPLODE_SOUND, true );
	PrecacheSound ( ZOMBIE_ZOMBIE_BOMB_THROW_SOUND, true );
	
	PrecacheSound ( ZOMBIE_FASTJUMP_SOUND, true );
	
	static char format[256];
	for ( int i = ZOMBIE_ZOMBIE_BOMB_IDLE_MIN; i <= ZOMBIE_ZOMBIE_BOMB_IDLE_MAX; i ++ ) {
		Format ( format, sizeof ( format ), ZOMBIE_ZOMBIE_BOMB_IDLE_SOUND, i );
		PrecacheSound ( format, true );
	}
}


/**
 * 스폰 시 독 효과 삭제
 */
void ZOMBIEPOISON_SpawnOnClearZombiePoison ( int client ) {
	if ( loringlib_IsValidClient ( client ) ) {
		g_bZombiePoison[client] = false;
		g_iZombiePoisonAttacker[client] = -1;
		g_fZombiePoisonTickTime[client] = 0.0;
	}
}

/**
 * 라운드 시작 시 좀비 기술 변수 초기화
 */
void ZOMBIESKILL_ZombieSkillVarClear () {
	for ( int i = 0; i < ( MAXPLAYERS + 1 ); i ++ ) {
		g_iZombieUpgrade[i] = 0;
		g_bUseZombieSkill[i] = false;
		g_iZombieSkillCooldown[i] = 0;
	}
}

/**
 * 접속 시 좀비 기술 사용 변수를 초기화한다.
 */
void ZOMBIESKILL_ConnectOnClearCooldown ( int client ) {
	g_bUseZombieSkill[client] = false;
	g_iZombieSkillCooldown[client] = 0;
}

/**
 * R 누를 시 좀비 기술 사용
 *
 * @note OnPlayerRunCmd
 */
void ZOMBIESKILL_RunCmdOnUseZombieSkill ( int client ) {
	if ( IsPlayerAlive ( client ) && !ZR_IsClientHuman ( client ) && !ZR_IsClientNemesis ( client ) ) {
		//	기술을 사용하고 있지 않을 경우
		
		//	난이도가 가장 쉬울때 일 경우
		if ( g_iDifficulty == 0 )
			return;
		
		if ( !g_bUseZombieSkill[client] ) {
			g_bUseZombieSkill[client] = true;
			
			if ( g_bIsStrongMotherZombie[client] || ZR_IsClientNemesis ( client ) ) {
				ZOMBIESKILL_BloodyField ( client );
				return;
			}
			
			switch ( ZR_GetClientZombieClass ( client ) ) {
			//	case ZR_ZOMBIE_CLASS_NORMAL: ZombieSkill_ThrowZombieBomb ( client );
				case ZR_ZOMBIE_CLASS_NORMAL: ZombieSkill_FastSpeed ( client );
				case ZR_ZOMBIE_CLASS_FAST: ZombieSkill_FastJump ( client );
				case ZR_ZOMBIE_CLASS_HEAVY: ZombieSkill_HeavyImmunity ( client );
				default: return;
			}
		}
	}
}

/**
 * 좀비 기술 쿨다운 표시
 */
public void ZOMBIESKILL_DisplayCooldown ( int client ) {
	if ( loringlib_IsValidClient__PlayGame ( client ) ) {
		if ( ZR_IsClientZombie ( client ) || ZR_IsClientNemesis ( client ) ) {
			if ( g_iZombieSkillCooldown[client] > 0 && g_bUseZombieSkill[client] ) {
				SetHudTextParams ( 0.255, 0.987, 0.2, 255, 0, 0, 255, 0, 0.0, 0.0, 0.0 );
				ShowSyncHudText ( client, g_hHudSyncArray[HUDSYNC_PLAYER_STATE_MSG2], "좀비 기술 대기... %d sec", g_iZombieSkillCooldown[client] );
			}
		}
	}
}

/**
 * 좀비 기술 쿨다운 종료
 *
 * @param timer
 * @param data			클라이언트 시리얼 값
 * @return action
 */
public Action timerZombieSkillCooldownEnd ( Handle timer, any data ) {
	static int client;
	client = GetClientFromSerial ( data );
	
	if ( loringlib_IsValidClient ( client ) ) {
		if ( ZR_IsClientHuman ( client ) ) {
			g_bUseZombieSkill[client] = false;
			g_iZombieSkillCooldown[client] = 0;
			return Plugin_Stop;
		}
		
		g_iZombieSkillCooldown[client] --;
		
		if ( g_iZombieSkillCooldown[client] <= 0 ) {
			if ( g_bUseZombieSkill[client] ) {
				g_bUseZombieSkill[client] = false;
				
				if ( g_iZombieUpgrade[client] < ZBUY_ZOMBIE_UPGRADE_MAX ) {
					if ( !g_bIsStrongMotherZombie[client] && ZR_IsClientMotherZombie ( client ) ) {
						static int color[3];
						color = { 255, 255, 255 };
						loringlib_PrintInstructorMessage ( client, 5.0, 300.0, 1.0, HINT_STATIC_ON_HUD, true, ICON_USE_BINDING, "", "reload", true, color, "키를 눌러 좀비 기술을 사용하세요." );
						PrintToChat ( client, " \x04[Zombie Skill] \x01좀비 기술이 준비되었습니다." );
					}
					
					return Plugin_Stop;
				}
				
				static int color[3];
				color = { 255, 255, 255 };
				loringlib_PrintInstructorMessage ( client, 5.0, 300.0, 1.0, HINT_STATIC_ON_HUD, true, ICON_USE_BINDING, "", "reload", true, color, "키를 눌러 좀비 기술을 사용하세요." );
				PrintToChat ( client, " \x04[Zombie Skill] \x01좀비 기술이 준비되었습니다." );
				
				return Plugin_Stop;
			}
		}
	}
	
	return Plugin_Continue;
}

/**
 * 좀비 질주
 */
void ZombieSkill_FastSpeed ( int client ) {
	static float fastSpeedScale;
	static float fastSpeedGravityScale;
	static float duration;
	
	DataPack data;
	if ( ZR_IsClientMotherZombie ( client ) ) {
		fastSpeedScale = ZOMBIE_FASTSPEED_SCALE_EX + ( float ( g_iDifficulty ) * ZOMBIE_FASTSPEED_DIFF_SCALE );
		fastSpeedGravityScale = ZOMBIE_FASTSPEED_GRAVITY_SCALE_EX + ( float ( g_iDifficulty ) * ZOMBIE_FASTSPEED_DIFF_GRAVITY_SCALE );
		
		CreateDataTimer ( 0.1, ZombieSkill_TimerFastSpeedStart, data, TIMER_REPEAT | TIMER_FLAG_NO_MAPCHANGE );
		data.WriteCell ( GetClientSerial ( client ) );
		data.WriteFloat ( GetGameTime () + ZOMBIE_FASTSPEED_DURATION_EX );
		data.WriteFloat ( loringlib_GetEntityLaggedmovement ( client ) );
		data.WriteFloat ( GetEntityGravity ( client ) );
		
		duration = ZOMBIE_FASTSPEED_DURATION_EX;
	} else {
		fastSpeedScale = ZOMBIE_FASTSPEED_SCALE + ( float ( g_iDifficulty ) * ZOMBIE_FASTSPEED_DIFF_SCALE );
		fastSpeedGravityScale = ZOMBIE_FASTSPEED_GRAVITY_SCALE + ( float ( g_iDifficulty ) * ZOMBIE_FASTSPEED_DIFF_GRAVITY_SCALE );
		
		CreateDataTimer ( 0.1, ZombieSkill_TimerFastSpeedStart, data, TIMER_REPEAT | TIMER_FLAG_NO_MAPCHANGE );
		data.WriteCell ( GetClientSerial ( client ) );
		data.WriteFloat ( GetGameTime () + ZOMBIE_FASTSPEED_DURATION );
		data.WriteFloat ( loringlib_GetEntityLaggedmovement ( client ) );
		data.WriteFloat ( GetEntityGravity ( client ) );
		
		duration = ZOMBIE_FASTSPEED_DURATION;
	}
	
	float origin[3], angle[3];
	GetClientAbsOrigin ( client, origin );
	origin[2] += 32.0;
	GetClientAbsAngles ( client, angle );
	loringlib_CreateParticleEx ( client, 0, origin, angle, "zombie_speedboost", true, duration, false );
	
	loringlib_CreateParticle ( client, 0, origin, "zombie_speedboost_screen", false, duration, true );
	
	loringlib_SetEntityFov ( client, 110 );
	loringlib_SetEntityLaggedmovement ( client, loringlib_GetEntityLaggedmovement ( client ) + fastSpeedScale );
	SetEntityGravity ( client, GetEntityGravity ( client ) - fastSpeedGravityScale );
}

public Action ZombieSkill_TimerFastSpeedStart ( Handle timer, DataPack data ) {
	data.Reset ();
	
	static int client;
	client = GetClientFromSerial ( data.ReadCell () );
	
	if ( !loringlib_IsValidClient__PlayGame ( client ) )
		return Plugin_Stop;
	
	if ( ZR_IsClientHuman ( client ) ) {
		loringlib_SetEntityFov ( client, 90 );
		loringlib_SetEntityLaggedmovement ( client, 1.0 );
		SetEntityGravity ( client, 1.0 );
		return Plugin_Stop;
	}
	
	static float duration;
	duration = data.ReadFloat ();
	
	static float options[2];
	options[0] = data.ReadFloat ();
	options[1] = data.ReadFloat ();
	
	if ( duration <= GetGameTime () ) {
		loringlib_SetEntityFov ( client, 90 );
		loringlib_SetEntityLaggedmovement ( client, options[0] );
		SetEntityGravity ( client, options[1] );
		
		static float cooldown;
		if ( ZR_IsClientMotherZombie ( client ) )
			cooldown = ZOMBIE_FASTSPEED_COOLDOWN_EX;
		else
			cooldown = ZOMBIE_FASTSPEED_COOLDOWN;
		g_iZombieSkillCooldown[client] = RoundFloat ( cooldown );
		CreateTimer ( 1.0, timerZombieSkillCooldownEnd, GetClientSerial ( client ), TIMER_REPEAT | TIMER_FLAG_NO_MAPCHANGE );
		return Plugin_Stop;
	}
	
	if ( !IsPlayerAlive ( client ) ) {
		if ( ZR_IsClientHuman ( client ) ) {
			loringlib_SetEntityFov ( client, 90 );
			loringlib_SetEntityLaggedmovement ( client, 1.0 );
			SetEntityGravity ( client, 1.0 );
		} else {
			loringlib_SetEntityFov ( client, 90 );
			loringlib_SetEntityLaggedmovement ( client, options[0] );
			SetEntityGravity ( client, options[1] );
		}
		
		return Plugin_Stop;
	}
	
	return Plugin_Continue;
}

/**
 * 좀비 수류탄 던지기
 * @param NOT USED
 */
void ZombieSkill_ThrowZombieBomb ( int client ) {
	static int entity;
	entity = CreateEntityByName ( "decoy_projectile" );
	
	if ( entity != -1 ) {
		SDKHook ( entity, SDKHook_SpawnPost, onSpawnZombieBomb );
		
		loringlib_SetEntityOwner2 ( entity, client );
		SetEntProp ( entity, Prop_Data, "m_takedamage", 0 );
		
		DispatchSpawn ( entity );
		ActivateEntity ( entity );
		
		static float ang[3], pos[3], vec[3], result[3];
		GetClientEyeAngles ( client, ang );
		GetClientEyePosition ( client, pos );
		GetAngleVectors ( ang, vec, NULL_VECTOR, NULL_VECTOR );
		NormalizeVector ( vec, vec );
		AddVectors ( pos, vec, result );
		NormalizeVector ( vec, vec );
		
		ScaleVector ( vec, ZOMBIE_ZOMBIE_BOMB_THROW_SPEED );
		SetEntityMoveType ( entity, MOVETYPE_FLYGRAVITY );
		
		TeleportEntity ( entity, result, ang, vec );
		
		static float entOrigin[3];
		loringlib_GetEntityOriginEx ( entity, entOrigin );
		loringlib_CreateParticle ( entity, 0, entOrigin, "zombie_bomb_trail", true, ZOMBIE_ZOMBIE_BOMB_LIFETIME );
		EmitAmbientSound ( ZOMBIE_ZOMBIE_BOMB_THROW_SOUND, entOrigin );
	
		CreateTimer ( ZOMBIE_ZOMBIE_BOMB_LIFETIME, timerZombieBombExplode, entity, TIMER_FLAG_NO_MAPCHANGE );
		
		SDKHookEx ( entity, SDKHook_StartTouch, onStartTouchZombieBomb );
		
		static float cooldown;
		if ( g_iZombieUpgrade[client] >= ZBUY_ZOMBIE_UPGRADE_MAX )
			cooldown = ZOMBIE_ZOMBIE_BOMB_COOLDOWN_EX;
		else
			cooldown = ZOMBIE_ZOMBIE_BOMB_COOLDOWN;
		
		g_iZombieSkillCooldown[client] = RoundFloat ( cooldown );
		CreateTimer ( 1.0, timerZombieSkillCooldownEnd, GetClientSerial ( client ), TIMER_REPEAT | TIMER_FLAG_NO_MAPCHANGE );
	}
}

/**
 * 좀비 폭탄이 스폰될 때 모델을 설정한다.
 */
public void onSpawnZombieBomb ( int entity ) {
	if ( IsValidEdict ( entity ) ) {
		SetEntityModel ( entity, ZOMBIE_ZOMBIE_BOMB_MODEL );
	}
}

/**
 * 좀비 폭탄이 어딘가에 터치 되었을 때 벨록시티를 죽인다.
 */
public void onStartTouchZombieBomb ( int entity, int other ) {
	if ( IsValidEdict ( entity ) ) {
		static float angles[3], origin[3];
		GetEntPropVector ( entity, Prop_Data, "m_angRotation", angles );
		loringlib_GetEntityOriginEx ( entity, origin );
		
		TeleportEntity ( entity, origin, angles, NULL_VECTOR_FLY_BLOCK );
		SetEntityMoveType ( entity, MOVETYPE_NONE );
	}
}

/**
 * 좀비 폭탄 폭발 init
 */
public Action timerZombieBombExplode ( Handle timer, any entity ) {
	if ( !IsValidEdict ( entity ) )
		return Plugin_Stop;
	
	static float touchOrigin[3];
	loringlib_GetEntityOriginEx ( entity, touchOrigin );
	touchOrigin[2] += 3.0;
	
	static float targetOrigin[3];
	static int owner;
	owner = loringlib_GetEntityOwner2 ( entity );
	
	static float vectors[3], vectorVelocity[3];
	static float targetEyePos[3];
	
	static float punchAngles[3];
	punchAngles[0] = GetRandomFloat ( -89.0, 89.0 );
	punchAngles[1] = GetRandomFloat ( -89.0, 89.0 );
	punchAngles[2] = 0.0;
	
	static float punchVelocity[3];
	punchVelocity[0] = 180.0;
	punchVelocity[1] = 180.0;
	punchVelocity[2] = 180.0;
	
	static float knockbackScale;
	
	for ( int target = 1; target <= MaxClients; target ++ ) {
		if ( loringlib_IsValidClient__PlayGame ( target ) ) {
			loringlib_GetEntityOriginEx ( target, targetOrigin );
			
			//	거리 내에 있을 경우
			if ( GetVectorDistance ( touchOrigin, targetOrigin ) <= ZOMBIE_ZOMBIE_BOMB_EXPLODE_RADIUS ) {
				loringlib_ShowAimPunch ( target, punchAngles );
				loringlib_SetAimPunchVel ( target, punchVelocity );
				
				GetClientEyePosition ( target, targetEyePos );
				MakeVectorFromPoints ( touchOrigin, targetEyePos, vectors );
				NormalizeVector ( vectors, vectors );
				
				//	넉백 크기 설정
				if ( GetEntityFlags ( target ) & FL_DUCKING ) {
					if ( loringlib_IsValidClient ( owner ) ) {
						if ( g_iZombieUpgrade[owner] >= ZBUY_ZOMBIE_UPGRADE_MAX )
							knockbackScale = ZOMBIE_ZOMBIE_BOMB_KNOCKBACK_SCALE_DUCK_EX;
						else
							knockbackScale = ZOMBIE_ZOMBIE_BOMB_KNOCKBACK_SCALE_DUCK;
					}else
						knockbackScale = ZOMBIE_ZOMBIE_BOMB_KNOCKBACK_SCALE_DUCK;
				}
				
				else {
					if ( loringlib_IsValidClient ( owner ) ) {
						if ( g_iZombieUpgrade[owner] >= ZBUY_ZOMBIE_UPGRADE_MAX )
							knockbackScale = ZOMBIE_ZOMBIE_BOMB_KNOCKBACK_SCALE_EX;
						else
							knockbackScale = ZOMBIE_ZOMBIE_BOMB_KNOCKBACK_SCALE;
					}else
						knockbackScale = ZOMBIE_ZOMBIE_BOMB_KNOCKBACK_SCALE;
				}
				
				ScaleVector ( vectors, knockbackScale );
				GetEntPropVector ( target, Prop_Data, "m_vecVelocity", vectorVelocity );
				vectors[2] += GetRandomFloat ( 20.0, 40.0 );
				TeleportEntity ( target, NULL_VECTOR, NULL_VECTOR, vectors );
				
				if ( loringlib_IsValidClient ( owner ) )
					if ( ZR_IsClientHuman ( target ) )
						loringlib_MakeDamage ( owner, target, GetRandomInt ( 1, 3 ), "weapon_fists", DMG_SHOCK );
			}
		}
	}
	
	//	프롭에 피해를 준다.
	static float edictOrigin[3];
	static char edictName[64];
	static int damageScale;
	for ( int props = MaxClients + 1; props <= SOURCE_MAXENTITIES; props ++ ) {
		if ( IsValidEdict ( props ) && IsValidEntity ( props ) ) {
			GetEdictClassname ( props, edictName, sizeof ( edictName ) );
			if ( StrContains ( edictName, "prop_", false ) != -1 ) {
				if ( GetEntProp ( props, Prop_Data, "m_takedamage" ) == 2 ) {
					loringlib_GetEntityOriginEx ( props, edictOrigin );
					if ( GetVectorDistance ( touchOrigin, edictOrigin ) <= ZOMBIE_ZOMBIE_BOMB_EXPLODE_RADIUS ) {
						if ( loringlib_IsValidClient ( owner ) ) {
							if ( g_iZombieUpgrade[owner] >= ZBUY_ZOMBIE_UPGRADE_MAX )
								damageScale = GetRandomInt ( ZOMBIE_ZOMBIE_BOMB_PROP_DAMAGE_MIN_EX, ZOMBIE_ZOMBIE_BOMB_PROP_DAMAGE_MAX_EX );
							else
								damageScale = GetRandomInt ( ZOMBIE_ZOMBIE_BOMB_PROP_DAMAGE_MIN, ZOMBIE_ZOMBIE_BOMB_PROP_DAMAGE_MAX );
						}
						else
							damageScale = GetRandomInt ( ZOMBIE_ZOMBIE_BOMB_PROP_DAMAGE_MIN, ZOMBIE_ZOMBIE_BOMB_PROP_DAMAGE_MAX );
						
						loringlib_MakeDamage ( loringlib_IsValidClient ( owner ) ? owner : 0, props, damageScale, "weapon_knife", DMG_SLASH );
					}
				}
			}
		}
	}
	
	EmitAmbientSound ( ZOMBIE_ZOMBIE_BOMB_EXPLODE_SOUND, touchOrigin );
	loringlib_CreateParticle ( entity, 0, touchOrigin, "zombie_bomb_explode" );
	
	AcceptEntityInput ( entity, "kill" );
	return Plugin_Stop;
}

/**
 * 도약
 */
void ZombieSkill_FastJump ( int client ) {
	if ( !( GetEntityFlags ( client ) & FL_ONGROUND ) ) {
		g_bUseZombieSkill[client] = false;
		PrintToChat ( client, " \x04[Fast Zombie] \x01공중에서는 사용할 수 없습니다." );
		return;
	}
	
	static float fastJumpScale, cooldown;
	if ( g_iZombieUpgrade[client] >= ZBUY_ZOMBIE_UPGRADE_MAX ) {
		fastJumpScale = ZOMBIE_FASTJUMP_SCALE_EX;
		cooldown = ZOMBIE_FASTJUMP_COOLDOWN_EX;
	}
	else {
		fastJumpScale = ZOMBIE_FASTJUMP_SCALE;
		cooldown = ZOMBIE_FASTJUMP_COOLDOWN;
	}

	fastJumpScale += ( ZOMBIE_FASTJUMP_DIFF_SCALE * g_iDifficulty );	//	난이도에 따라 증가
	
	static float anglevector[3], clienteyeangle[3], clientpos[3];
	GetClientEyeAngles ( client, clienteyeangle );
	GetClientEyePosition ( client, clientpos );
	GetAngleVectors ( clienteyeangle, anglevector, NULL_VECTOR, NULL_VECTOR );
	NormalizeVector ( anglevector, anglevector );
	
	ScaleVector ( anglevector, fastJumpScale );
	
	clientpos[2] += 5.0;
	EmitAmbientSound ( ZOMBIE_FASTJUMP_SOUND, clientpos );
	TeleportEntity ( client, NULL_VECTOR, NULL_VECTOR, anglevector );
	clientpos[2] -= 45.0;
	loringlib_CreateParticleEx ( client, 0, clientpos, NULL_VECTOR, "zombie_jump", false, 5.0 );
	loringlib_ShowShakeMessage ( client, 10.5, 1.5 );
	
	g_iZombieSkillCooldown[client] = RoundFloat ( cooldown );
	CreateTimer ( 1.0, timerZombieSkillCooldownEnd, GetClientSerial ( client ), TIMER_REPEAT | TIMER_FLAG_NO_MAPCHANGE );
}

/**
 * 이뮤니티
 */
void ZombieSkill_HeavyImmunity ( int client ) {
	static float cooldown, duration, knockbackImmunityScale;
	static int healthScale;
	if ( g_iZombieUpgrade[client] >= ZBUY_ZOMBIE_UPGRADE_MAX ) {
		cooldown = ZOMBIE_HEAVY_IMMUNITY_COOLDOWN_EX;
		duration = ZOMBIE_HEAVY_IMMUNITY_DURATION_EX;
		knockbackImmunityScale = ZOMBIE_HEAVY_IMMUNITY_KNOCKBACK_SCALE_EX;
		healthScale = ZOMBIE_HEAVY_IMMUNITY_HEALTH_SCALE_EX;
	}
	else {
		cooldown = ZOMBIE_HEAVY_IMMUNITY_COOLDOWN;
		duration = ZOMBIE_HEAVY_IMMUNITY_DURATION;
		knockbackImmunityScale = ZOMBIE_HEAVY_IMMUNITY_KNOCKBACK_SCALE;
		healthScale = ZOMBIE_HEAVY_IMMUNITY_HEALTH_SCALE;
	}
	
	static int skinId;
	skinId = setClientSkinGlow ( client, 1, { 250, 237, 125, 255 } );
	
	DataPack data;
	CreateDataTimer ( duration, timerHeavyImmunityDurationEnd, data, TIMER_FLAG_NO_MAPCHANGE );
	data.WriteCell ( GetClientSerial ( client ) );
	data.WriteFloat ( knockbackImmunityScale );
	data.WriteCell ( skinId );
	
	//	넉백 설정
	ZR_SetClientKnockbackScale ( client, ZR_GetClientZombieType ( client ), ZR_GetClientKnockbackScale ( client, ZR_GetClientZombieType ( client ) ) + knockbackImmunityScale );
	
	//	체력 설정
	loringlib_SetEntityHealth ( client, loringlib_GetEntityHealth ( client ) + healthScale );
	//	최대 체력으로 넘어갈 경우 최대 체력 값을 체력값으로 수정한다.
	if ( loringlib_GetEntityHealth ( client ) > loringlib_GetEntityMaxHealth ( client ) ) {
		loringlib_SetEntityMaxHealth ( client, loringlib_GetEntityHealth ( client ) );
		loringlib_SetEntityHealth ( client, loringlib_GetEntityMaxHealth ( client ) );
	}
	
	loringlib_ShowFadeUserMessageEx ( client, RoundFloat ( duration ) * 100, RoundFloat ( duration ) * 30, FFADE_AUTO, { 150, 120, 0, 100 } );
	loringlib_ShowShakeMessage ( client, 10.0, 1.0 );
	
	g_iZombieSkillCooldown[client] = RoundFloat ( cooldown );
	CreateTimer ( 1.0, timerZombieSkillCooldownEnd, GetClientSerial ( client ), TIMER_REPEAT | TIMER_FLAG_NO_MAPCHANGE );
}

/**
 * 이뮤니티 지속 종료
 */
public Action timerHeavyImmunityDurationEnd ( Handle timer, DataPack data ) {
	data.Reset ();
	
	static int client;
	static float knockbackImmunityScale;
	client = GetClientFromSerial ( data.ReadCell () );
	
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Stop;
	
	if ( IsPlayerAlive ( client ) ) {
		ZR_SetClientKnockbackScale ( client, ZR_GetClientZombieType ( client ), ZR_GetClientKnockbackScale ( client, ZR_GetClientZombieType ( client ) ) - knockbackImmunityScale );
		
		knockbackImmunityScale = data.ReadFloat ();
		static int glow;
		glow = data.ReadCell ();
		
		if ( glow != INVALID_ENT_REFERENCE ) {
			SetEntProp ( glow, Prop_Send, "m_bShouldGlow", false, true );
			AcceptEntityInput ( glow, "Kill" );
		}
	//	loringlib_ShowFadeUserMessageEx ( client, 1, 1, FFADE_IN | FFADE_PURGE, { 0, 0, 0, 0 } );
	}
	
	return Plugin_Stop;
}

/**
 * 블러디 필드
 */
void ZOMBIESKILL_BloodyField ( int client ) {
	if ( ( ZR_IsClientMotherZombie ( client ) && g_bIsStrongMotherZombie[client] ) ||
		ZR_IsClientNemesis ( client ) ) {
		float pos[3];
		float punch[3];
		punch[0] = GetRandomFloat ( -49.0, 59.9 );
		punch[1] = GetRandomFloat ( -49.0, 59.9 );
		punch[2] = 0.0;
		float punch_vel[3] = { 130.0, 130.0, 150.0 };
		float target_pos[3];
		float distance;

		loringlib_ShowShakeMessage ( client, 10.0, 3.0 );
		EmitSoundToAll ( ZOMBIESKILL_USE_KILL_SOUNDEFF, _, _, SNDLEVEL_NONE );

		//	일반 블러디필드
		GetEntPropVector ( client, Prop_Send, "m_vecOrigin", pos );
		pos[2] += 10.0;
		loringlib_CreateParticle ( client, 0, pos, "zombie_howl", false );

		float skillDistance = ZOMBIESKILL_BLOODYFIELD_RADIUS;
	//	loringlib_DisplayNearTrueTrigger ( client, skillDistance, 1.5, 20.0, 10.0, { 255, 0, 0, 255 } );
		
		for ( int victim = 1; victim <= MaxClients; victim ++ ) {
			if ( !loringlib_IsValidClient__PlayGame ( victim ) )
				continue;
			
			if ( ZR_IsClientHuman ( victim ) ) {
				GetEntPropVector ( victim, Prop_Send, "m_vecOrigin", target_pos );
				if ( GetVectorDistance ( target_pos, pos ) <= skillDistance ) {
					distance = GetVectorDistance ( pos, target_pos );
					
					if ( distance <= 300.0 ) {
					//	loringlib_ShowShakeMessage ( victim, 100.0, 5.0 );
						loringlib_MakeDamage ( client, victim, GetRandomInt ( 20, 25 ), "weapon_fists", DMG_SHOCK );
					} else {
					//	loringlib_ShowShakeMessage ( victim, 80.0, 5.0 );
						loringlib_MakeDamage ( client, victim, GetRandomInt ( 15, 20 ), "weapon_fists", DMG_SHOCK );
					}
					
					loringlib_ShowAimPunch ( victim, punch );
					loringlib_SetAimPunchVel ( victim, punch_vel );
					
					CreateTimer ( 0.5, timerRepeatPunchView, victim, TIMER_FLAG_NO_MAPCHANGE );
					CreateTimer ( 1.0, timerRepeatPunchView, victim, TIMER_FLAG_NO_MAPCHANGE );
					CreateTimer ( 1.5, timerRepeatPunchView, victim, TIMER_FLAG_NO_MAPCHANGE );
				//	CreateTimer ( 2.0, timerRepeatPunchView, victim, TIMER_FLAG_NO_MAPCHANGE );
					
					target_pos[2] += 20.0;
					loringlib_CreateParticle ( victim, 0, target_pos, "blood_impact_headshot", false );
				}
			}
		}
		
		g_iZombieSkillCooldown[client] = RoundFloat ( ZOMBIE_MOTHERZOMBIE_BLOODYFIELD_COOLDOWN );
		CreateTimer ( 1.0, timerZombieSkillCooldownEnd, GetClientSerial ( client ), TIMER_REPEAT | TIMER_FLAG_NO_MAPCHANGE );
	}
}

public Action timerRepeatPunchView ( Handle timer, any client ) {
	if ( loringlib_IsValidClient__PlayGame ( client ) ) {
		if ( !ZR_IsClientZombie ( client ) ) {
			float punch[3];
			punch[0] = GetRandomFloat ( -49.0, 59.9 );
			punch[1] = GetRandomFloat ( -49.0, 59.9 );
			punch[2] = 0.0;
			float punch_vel[3] = { 130.0, 130.0, 150.0 };
			
			loringlib_ShowAimPunch ( client, punch );
			loringlib_SetAimPunchVel ( client, punch_vel );
		}
	}
	
	return Plugin_Stop;
}

/**
 * 기가 드릴 브레이커
 */
public void ZOMBIESKILL_GigaDrillBreaker ( int client ) {
	if ( !ZR_IsClientMotherZombie ( client ) && !ZR_IsClientNemesis ( client ) )
		return;
	
	PrecacheSound ( "training/firewerks_burst_02.wav" );
	EmitSoundToAll ( "training/firewerks_burst_02.wav", _, _, SNDLEVEL_NONE );
	
	g_bUseZombieSkill[client] = true;
	
	if ( g_bIsStrongMotherZombie[client] || ZR_IsClientNemesis ( client ) ) {
		for ( int i = 1; i <= MaxClients; i ++ ) {
			if ( loringlib_IsValidClient__PlayGame ( i ) ) {
				if ( !ZR_IsClientHuman ( i ) ) {
					SDKHook ( i, SDKHook_PostThinkPost, ZOMBIESKILL_OnGigaDrillBreaker );
					CreateTimer ( 6.0, timerGigaDrillBreaker, i, TIMER_FLAG_NO_MAPCHANGE );
				}
			}
		}
		
		return;
	}
	
	SDKHook ( client, SDKHook_PostThinkPost, ZOMBIESKILL_OnGigaDrillBreaker );
	CreateTimer ( 3.5, timerGigaDrillBreaker, client, TIMER_FLAG_NO_MAPCHANGE );
}

public void ZOMBIESKILL_OnGigaDrillBreaker ( int client ) {
	if ( loringlib_IsValidClient ( client ) ) {
		if ( IsPlayerAlive ( client ) ) {
			if ( !ZR_IsClientHuman ( client ) ) {
				if ( g_bUseZombieSkill[client] ) {
					char weapon[32];
					int active = loringlib_GetActiveWeaponName ( client, weapon, sizeof ( weapon ) );
					if ( active != -1 ) {
						if ( StrContains ( weapon, "knife", false ) != -1 ) {
							float nextPrimaryAtt = GetEntPropFloat ( active, Prop_Send, "m_flNextPrimaryAttack" );
							
							nextPrimaryAtt -= GetGameTime ();
							nextPrimaryAtt *= 0.35;
							nextPrimaryAtt += GetGameTime ();
							
							SetEntPropFloat ( active, Prop_Send, "m_flNextPrimaryAttack", nextPrimaryAtt );
						
							return;
						}
					}
				}
			}
		}
		
		SDKUnhook ( client, SDKHook_PostThinkPost, ZOMBIESKILL_OnGigaDrillBreaker );
	}
}

public Action timerGigaDrillBreaker ( Handle timer, any client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Stop;
	
	g_bUseZombieSkill[client] = false;
	SDKUnhook ( client, SDKHook_PostThinkPost, ZOMBIESKILL_OnGigaDrillBreaker );
	
	return Plugin_Stop;
}

/**
 * 좀비 감염
 */ 
void zombie_TakeDamageOnInfectDamage ( int victim, int attacker, int idx, float dmg ) {
	if ( ZR_IsClientHuman ( victim ) && !ZR_IsClientHuman ( attacker ) ) {
		if ( !loringlib_IsValidClient ( g_iZombiePoisonAttacker[victim] ) ) {
			if ( idx == -1 )
				return;
			
			char weapon[32];
			GetEdictClassname ( idx, weapon, sizeof ( weapon ) );
			if ( StrContains ( weapon, "knife", false ) == -1 || dmg <= float ( ZOMBIE_POISON_DAMAGE ) )
				return;
			
			//	독 상태가 아닐 경우
			if ( !g_bZombiePoison[victim] ) {
				if ( ZR_IsGameEnd () )
					return;
				
				g_bZombiePoison[victim] = true;	//	독 상태로 만든다.
				g_fZombiePoisonTickTime[victim] = GetGameTime ();
				g_iZombiePoisonAttacker[victim] = attacker;
			}
		}
	}
}

/**
 * 감염상태 데미지
 */
void ZOMBIEPOISON_PostThinkOnPoisonTickDamage ( int client ) {
	if ( loringlib_IsValidClient ( client ) ) {
		if ( g_fZombiePoisonTickTime[client] != 0.0 && g_fZombiePoisonTickTime[client] <= GetGameTime () ) {
			g_fZombiePoisonTickTime[client] = GetGameTime () + ZOMBIE_POISON_TICKTIME;
			//	독 상태일 경우
			if ( g_bZombiePoison[client] ) {
				if ( ZR_IsGameEnd () || !ZR_IsClientHuman ( client ) || !IsPlayerAlive ( client ) ) {
					g_bZombiePoison[client] = false;
					g_iZombiePoisonAttacker[client] = -1;
					g_fZombiePoisonTickTime[client] = 0.0;
					return;
				}
				
				//	독을 전파한 어테커가 유효할 경우
				if ( loringlib_IsValidClient ( g_iZombiePoisonAttacker[client] ) ) {
					loringlib_MakeDamage ( g_iZombiePoisonAttacker[client], client, ZOMBIE_POISON_DAMAGE, "weapon_knife", DMG_POISON );
					float eye[3];
					GetClientEyePosition ( client, eye );
					loringlib_CreateParticle ( client, 0, eye, "zombie_infected", false, ZOMBIE_POISON_TICKTIME, true );
					
					float pos[3];
					GetClientAbsOrigin ( client, pos );
					pos[2] += 15.0;
					loringlib_CreateParticle ( client, 0, pos, "zombie_infecttrail", true, ZOMBIE_POISON_TICKTIME );
					
					float punch[3];
					punch[0] = GetRandomFloat ( -39.0, 39.9 );
					punch[1] = GetRandomFloat ( -39.0, 39.9 );
					punch[2] = 0.0;
					float punch_vel[3] = { 100.0, 100.0, 100.0 };
					
					loringlib_ShowAimPunch ( client, punch );
					loringlib_SetAimPunchVel ( client, punch_vel );
				}
				
				//	유효하지 않을 경우 독을 삭제한다.
				else {
					g_bZombiePoison[client] = false;
					g_iZombiePoisonAttacker[client] = -1;
					g_fZombiePoisonTickTime[client] = 0.0;
					SetHudTextParamsEx ( -1.0, -1.0, 3.0, { 151, 211, 211, 255 }, { 2, 103, 255, 255 }, 2, 0.04, 0.02, 0.5 );
					ShowSyncHudText ( client, g_hHudSyncArray[HUDSYNC_ZOMBIE_POISON_SECRET_CLEAR], "모리쿠보의 마법으로 %N씨의 감염상태를 치료했는데요...", client );
				}
			}
		}
	}
}