#define SHOTGUNNER_PULSESHOT_DISTANCE		300.0	/**< 펄스샷 맞는 거리 */
#define SHOTGUNNER_PULSESHOT_ANGLE_SCALE	179.0	/**< 펄스샷 맞는 앵글 각도 */
#define SHOTGUNNER_PULSESHOT_KNOCKBACK_SCALE	1200.0	/**< 펄스샷 넉백 */
#define SHOTGUNNER_PULSESHOT_MASTERY_KNOCKBACK_SCALE	80.0	/**< 펄스샷 마스터리 넉백 */

#define SHOTGUNNER_LANDMINE_EXPLODE_DISTANCE	350.0
#define SHOTGUNNER_LANDMINE_WORLD_DAMAGE		300.0		/**< 지뢰 기본 폭발 피해 */
#define SHOTGUNNER_LANDMINE_MODEL		"models/weapons/eminem/laser_mine/w_laser_mine_dropped3.mdl"			/**< 지뢰 모델 */
#define SHOTGUNNER_LANDMINE_LAND_SOUND	"survival/breach_land_01.wav"	/**< 지로 설치 시 소리 */
#define SHOTGUNNER_LANDMINE_EXPLOSION	"survival/turret_death_01.wav"	/**< 지뢰 폭발 시 소리 */
#define SHOTGUNNER_LANDMINE_TARGET_VELMODIF_DURATION	1.0			/**< 지뢰 밟은 타겟에게 경직 주는 시간 (초) */

#define SHOTGUNNER_BLOCKING_POWER_DEFAULT_SCALE		0.5		/**< 기본 경직 값 */

#define SHOTGUNNER_EXPLODE_BULLET_DEFAULT_DMG_SCALE	10.0	/**< 폭발탄 기본 데미지 값 */
#define SHOTGUNNER_EXPLODE_BULLET_RADIUS			130.0	/**< 폭발탄 폭발 범위 */

#define SHOTGUNNER_LANDMINE_BLACKHOLE_DURATION		0.4			/**< 지뢰 블랙홀 지속 시간 */
#define SHOTGUNNER_LANDMINE_BLACKHOLE_DISTANCE		180.0		/**< 지뢰 블랙홀 당기는 거리 */
#define SHOTGUNNER_LANDMINE_BLACKHOLE_DAMAGE_DISTANCE	350.0	/**< 지뢰 블랙홀 폭발 데미지 거리 */
#define SHOTGUNNER_LANDMINE_BLACKHOLE_FULLFORCE		-500.0		/**< 지뢰 블랙홀 당기는 힘 */

#define SOUND_SHOTGUNNER_LANDMINE_DETECTED_EXPLODE	"qufnr/shotgunner/shotgunner_landmine_blackhole.mp3"
#define SOUND_SHOTGUNNER_LANDMINE_DISPATCH			"qufnr/shotgunner/shotgunner_landmine_dispatch.mp3"
#define SOUND_SHOTGUNNER_LANDMINE_EXPLODE			"qufnr/shotgunner/shotgunner_landmine_detected.mp3"
#define SOUND_SHOTGUNNER_PULSESHOT					"qufnr/shotgunner/shotgunner_pulseshot.mp3"
#define SOUND_SHOTGUNNER_PULSESHOT_EX				"qufnr/shotgunner/shotgunner_pulseshot_ex.mp3"

static int g_iShotgunnerDamage[MAXPLAYERS + 1] = { 0, ... };	/**< 폭발탄 데미지 */
static bool g_bIsShotgunFired[MAXPLAYERS + 1];
static bool g_bIsZetPulseUsed[MAXPLAYERS + 1] = { false, ... };
static int g_iShotgunnerZetPulseCounts[MAXPLAYERS + 1] = { 0, ... };

void ClientClass_Shotgunner_PrecacheSounds () {
	PrecacheSound ( SOUND_SHOTGUNNER_LANDMINE_DETECTED_EXPLODE, true );
	PrecacheSound ( SOUND_SHOTGUNNER_LANDMINE_DISPATCH, true );
	PrecacheSound ( SOUND_SHOTGUNNER_LANDMINE_EXPLODE, true );
	PrecacheSound ( SOUND_SHOTGUNNER_PULSESHOT, true );
	PrecacheSound ( SOUND_SHOTGUNNER_PULSESHOT_EX, true );
}

/**
 * 전문병 데이터 변수 초기화
 */
/*
void ClientClass_Shotgunner_ClearShotgunnerData ( int client ) {
	
}
*/

/**
 * SG 숙련
 */
stock float ClientClass_Shotgunner_ShotgunMastery ( int client, float dmg, int weapon ) {
	if ( g_iClientClass[client] != ClientClass_Shotgunner )
		return dmg;
	
	int clientClass = view_as <int> ( ClientClass_Shotgunner );
	int shotgunMastery = g_iSkill[clientClass][SKILL_ID_SG_UPGRADE][client];
	if ( shotgunMastery > 0 ) {
		if ( weapon != -1 ) {
			char weaponname[32];
			GetEdictClassname ( weapon, weaponname, sizeof ( weaponname ) );
			bool shotgun = false;
			for ( int i = 0; i < sizeof ( g_strClassWeapon_Shotgunner ); i ++ )
				if ( StrContains ( weaponname, g_strClassWeapon_Shotgunner[i], false ) != -1 )
					shotgun = true;
			
			
			//	샷건일 경우?
			if ( shotgun ) {
				return dmg * ( ( ROUND_DMG_PERCENTAGE + ( g_iSkill[clientClass][SKILL_ID_SG_UPGRADE][client] * g_fSkillAction[clientClass][SKILL_ID_SG_UPGRADE] ) ) );
			}
		}
	}
	
	return dmg;
}

/**
 * 펄스샷 장전
 */
void clientClass_Shotgunner_ReloadPulseshot ( int client, int skill ) {
	//	전문병이 아닐 경우
	if ( g_iClientClass[client] != ClientClass_Shotgunner )
		return;
	
	//	좀비일 경우
	if ( ZR_IsClientZombie ( client ) )
		return;
	
	int clientClass = view_as <int> ( g_iClientClass[client] );
	//	기술을 사용중일 경우
	if ( g_bSkillUse[clientClass][skill][client] ) {
		CLIENT_SKILL_NOT_AVAILABLE_NOW_USING_MSG ( client )
		return;
	}
	
	//	기술 쿨이 있을 경우
	if ( g_bSkillCooldown[clientClass][skill][client] ) {
		CLIENT_SKILL_NOT_AVAILABLE_COOLDOWN_MSG ( client )
		return;
	}
	
	int consume_ap = g_iSkillConsumeAP[clientClass][skill];
	//	AP가 부족할 경우
	if ( consume_ap > loringlib_GetEntityArmor ( client ) ) {
		CLIENT_SKILL_NOT_AVAILABLE_NEED_AP_MSG ( client )
		return;
	}
	
	g_bSkillUse[clientClass][skill][client] = true;
	loringlib_SetEntityArmor ( client, loringlib_GetEntityArmor ( client ) - consume_ap );
	CLIENT_SKILL_USE_MSG ( client, g_strSkillName[clientClass][skill] )
	if ( g_iSkill[clientClass][SKILL_ID_PULSESHOT_EX][client] > 0 )
		g_bIsZetPulseUsed[client] = false;
}

/**
 * 펄스샷 발사
 * @note weapon_fire에서
 */
void ClientClass_Shotgunner_FireOnPulseshot ( int client, char[] weapon ) {
	if ( loringlib_IsValidClient__PlayGame ( client ) ) {
		if ( g_iClientClass[client] != ClientClass_Shotgunner )
			return;
		
		int clientClass = view_as <int> ( ClientClass_Shotgunner );
		//	펄스샷 기술을 사용중일 경우?
		if ( g_bSkillUse[clientClass][SKILL_ID_PULSESHOT][client] ) {
			bool shotgun = false;
			for ( int i = 0; i < sizeof ( g_strClassWeapon_Shotgunner ); i ++ )
				if ( StrContains ( g_strClassWeapon_Shotgunner[i], weapon, false ) != -1 )
					shotgun = true;
			
			
			//	샷건일 경우?
			if ( shotgun ) {
				//	쿨 다운을 만든다.
				g_bSkillUse[clientClass][SKILL_ID_PULSESHOT][client] = false;
				g_iSkillCooldownTicks[clientClass][SKILL_ID_PULSESHOT][client] = g_iSkillCooldown[clientClass][SKILL_ID_PULSESHOT];
				g_bSkillCooldown[clientClass][SKILL_ID_PULSESHOT][client] = true;
				
				int damage = RoundFloat ( g_fSkillAction[clientClass][SKILL_ID_PULSESHOT] );
				float knockbackScale = SHOTGUNNER_PULSESHOT_KNOCKBACK_SCALE;
				//	펄스샷 마스터리를 배웠을 경우
				if ( g_iSkill[clientClass][SKILL_ID_PULSESHOT_MASTERY][client] > 0 ) {
					knockbackScale += ( g_iSkill[clientClass][SKILL_ID_PULSESHOT_MASTERY][client] * SHOTGUNNER_PULSESHOT_MASTERY_KNOCKBACK_SCALE );
					damage += RoundFloat ( g_fSkillAction[clientClass][SKILL_ID_PULSESHOT_MASTERY] * g_iSkill[clientClass][SKILL_ID_PULSESHOT_MASTERY][client] );
				}
				
				//	발사
				float attEye[3], vec[3];
				char activename[32];
				loringlib_GetActiveWeaponName ( client, activename, sizeof ( activename ) );
				GetClientEyePosition ( client, attEye );
				float attEyeAng[3];
				GetClientEyeAngles ( client, attEyeAng );
				EmitAmbientSound ( SOUND_SHOTGUNNER_PULSESHOT, attEye );
				//	제트기류
				if ( g_iSkill[clientClass][SKILL_ID_PULSESHOT_EX][client] > 0 ) {
				//	loringlib_CreateParticleEx ( client, 0, attEye, attEyeAng, "special_pulse_child1", true, 3.0 );
					loringlib_CreateParticleEx ( client, 0, attEye, attEyeAng, "special_zetpulse", true, 3.0 );
					DataPack data;
					CreateDataTimer ( 0.0, timerZetPulseshotRepeat, data, TIMER_FLAG_NO_MAPCHANGE );
					data.WriteCell ( GetClientUserId ( client ) );
					data.WriteString ( activename );
					data.WriteFloat ( knockbackScale );
					data.WriteCell ( damage );
					DataPack repeatTimerData;
					CreateDataTimer ( g_fSkillAction[clientClass][SKILL_ID_PULSESHOT_EX], timerZetPulseshotRepeat, repeatTimerData, TIMER_REPEAT | TIMER_FLAG_NO_MAPCHANGE );
					repeatTimerData.WriteCell ( GetClientUserId ( client ) );
					repeatTimerData.WriteString ( activename );
					repeatTimerData.WriteFloat ( knockbackScale );
					repeatTimerData.WriteCell ( damage );
				}
				
				//	일반 펄스샷
				else {
				//	loringlib_CreateParticleEx ( client, 0, attEye, attEyeAng, "special_pulse_child1", true, 3.0 );
					float tarEye[3];
					float vel[3], resultVel[3];
					loringlib_CreateParticleEx ( client, 0, attEye, attEyeAng, "special_pulse", true, 5.0 );
					for ( int target = 1; target <= MaxClients; target ++ ) {
						if ( loringlib_IsValidClient__PlayGame ( target ) ) {
							if ( !ZR_IsClientHuman ( target ) ) {
								//	거리와 각도 안에 있을 경우를 채크한다.
								if ( loringlib_IsTargetInSightRange ( client, target, SHOTGUNNER_PULSESHOT_ANGLE_SCALE, SHOTGUNNER_PULSESHOT_DISTANCE ) ) {
									GetClientEyePosition ( target, tarEye );
									tarEye[2] = 0.0;
									attEye[2] = 0.0;
									
									MakeVectorFromPoints ( attEye, tarEye, vec );
									NormalizeVector ( vec, vec );
									
									ScaleVector ( vec, knockbackScale );
									GetEntPropVector ( target, Prop_Data, "m_vecVelocity", vel );
									AddVectors ( vel, vec, resultVel );
									
									TeleportEntity ( target, NULL_VECTOR, NULL_VECTOR, resultVel );
									loringlib_MakeDamage ( client, target, damage, activename, DMG_POISON );
									g_iSurvivePoint[client] += SURVIVE_POINT_SHOTGUNNER_PULSESHOT_ATTACK;
								}
							}
						}
					}
				}
			}
		}
	}
}

/**
 * 제트기류
 */
public Action timerZetPulseshotRepeat ( Handle timer, DataPack dp ) {
	dp.Reset ();
	static int client;
	static float attEye[3];
	static float attEyeAng[3];
	static char weapon[32];
	static float knockbackScale;
	static int damage;
	client = GetClientOfUserId ( dp.ReadCell () );
	
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Stop;
	
	if ( g_iClientClass[client] != ClientClass_Shotgunner ) {
		g_iShotgunnerZetPulseCounts[client] = 0;
		return Plugin_Stop;
	}
	
	if ( !ZR_IsClientHuman ( client ) ) {
		g_iShotgunnerZetPulseCounts[client] = 0;
		return Plugin_Stop;
	}
	
	g_iShotgunnerZetPulseCounts[client] ++;
	if ( g_iShotgunnerZetPulseCounts[client] > 2 ) {
		//	쿨 다운을 만든다.
		int clientClass = view_as <int> ( ClientClass_Shotgunner );
		g_bSkillUse[clientClass][SKILL_ID_PULSESHOT][client] = false;
		g_iSkillCooldownTicks[clientClass][SKILL_ID_PULSESHOT][client] = g_iSkillCooldown[clientClass][SKILL_ID_PULSESHOT];
		g_bSkillCooldown[clientClass][SKILL_ID_PULSESHOT][client] = true;
		g_iShotgunnerZetPulseCounts[client] = 0;
		return Plugin_Stop;
	}
	
	GetClientEyePosition ( client, attEye );
	GetClientEyeAngles ( client, attEyeAng );
	dp.ReadString ( weapon, sizeof ( weapon ) );
	knockbackScale = dp.ReadFloat ();
	damage = dp.ReadCell ();
	
	static float tarEye[3];
	static float vel[3], resultVel[3], vec[3];
	
	
	EmitAmbientSound ( SOUND_SHOTGUNNER_PULSESHOT_EX, attEye );
	loringlib_CreateParticleEx ( client, 0, attEye, attEyeAng, "special_zetpulse_replay", true, 5.0 );
	
	for ( int target = 1; target <= MaxClients; target ++ ) {
		if ( loringlib_IsValidClient__PlayGame ( target ) ) {
			if ( !ZR_IsClientHuman ( target ) ) {
				//	거리와 각도 안에 있을 경우를 채크한다.
				if ( loringlib_IsTargetInSightRange ( client, target, SHOTGUNNER_PULSESHOT_ANGLE_SCALE, SHOTGUNNER_PULSESHOT_DISTANCE ) ) {
					GetClientEyePosition ( target, tarEye );
					tarEye[2] = 0.0;
					attEye[2] = 0.0;
					
					MakeVectorFromPoints ( attEye, tarEye, vec );
					NormalizeVector ( vec, vec );
					
					ScaleVector ( vec, knockbackScale );
					GetEntPropVector ( target, Prop_Data, "m_vecVelocity", vel );
					AddVectors ( vel, vec, resultVel );
					
					TeleportEntity ( target, NULL_VECTOR, NULL_VECTOR, resultVel );
					loringlib_MakeDamage ( client, target, damage, weapon, DMG_POISON );
					g_iSurvivePoint[client] += SURVIVE_POINT_SHOTGUNNER_PULSESHOT_ATTACK;
				}
			}
		}
	}
	
	return Plugin_Continue;
}

/**
 * 지뢰 수류탄 지급
 */
void clientClass_Shotgunner_GiveLandmineGrenade ( int client, int skill ) {
	//	전문병이 아닐 경우
	if ( g_iClientClass[client] != ClientClass_Shotgunner )
		return;
	
	//	좀비일 경우
	if ( ZR_IsClientZombie ( client ) )
		return;
	
	int clientClass = view_as <int> ( g_iClientClass[client] );
	//	기술을 사용중일 경우
	if ( g_bSkillUse[clientClass][skill][client] ) {
		CLIENT_SKILL_NOT_AVAILABLE_NOW_USING_MSG ( client )
		return;
	}
	
	//	기술 쿨이 있을 경우
	if ( g_bSkillCooldown[clientClass][skill][client] ) {
		CLIENT_SKILL_NOT_AVAILABLE_COOLDOWN_MSG ( client )
		return;
	}
	
	int consume_ap = g_iSkillConsumeAP[clientClass][skill];
	//	AP가 부족할 경우
	if ( consume_ap > loringlib_GetEntityArmor ( client ) ) {
		CLIENT_SKILL_NOT_AVAILABLE_NEED_AP_MSG ( client )
		return;
	}
	
	g_bSkillUse[clientClass][skill][client] = true;
	loringlib_SetEntityArmor ( client, loringlib_GetEntityArmor ( client ) - consume_ap );
	CLIENT_SKILL_USE_MSG ( client, g_strSkillName[clientClass][skill] )
	
	CreateTimer ( 0.0, timerGiveLandmineGrenade, client );
}

public Action timerGiveLandmineGrenade ( Handle timer, any client ) {
	if ( loringlib_IsValidClient__PlayGame ( client ) ) {
		GivePlayerItem ( client, "weapon_decoy" );
	}
	
	return Plugin_Stop;
}

/**
 * 지뢰 수류탄 발사 시 스폰 훅
 */
void clientClass_Shotgunner_CreateLandmineGrenade ( int edict, const char[] classname ) {
	if ( StrContains ( classname, "decoy_projectile", false ) != -1 )
		SDKHook ( edict, SDKHook_SpawnPost, onLandmineGrenadeSpawned );
}

/**
 * 지뢰 수류탄 발사 시 스폰 되고난 후 설정
 */
public void onLandmineGrenadeSpawned ( int entity ) {
	if ( IsValidEdict ( entity ) ) {
		int owner = loringlib_GetEntityOwner2 ( entity );
		if ( loringlib_IsValidClient ( owner ) && ZR_IsClientHuman ( owner ) ) {
			if ( g_iClientClass[owner] == ClientClass_Shotgunner )
				//	엔티티 주인이 인간일 경우
				if ( ZR_IsClientHuman ( owner ) )
					SDKHook ( entity, SDKHook_GroundEntChangedPost, onLandmineGrenadeBounce );
		}
	}
}

/**
 * 지뢰 수류탄이 땅에 닿을 때 지뢰를 설치 한다.
 */
public void onLandmineGrenadeBounce ( int entity ) {
	if ( IsValidEdict ( entity ) ) {
		int owner = loringlib_GetEntityOwner2 ( entity );
		if ( loringlib_IsValidClient ( owner ) ) {
			//	엔티티 주인이 인간일 경우
			if ( ZR_IsClientHuman ( owner ) ) {
				//	엔티티가 땅에 닿았을 경우
				float pos[3];
				loringlib_GetEntityOriginEx ( entity, pos );
				spawnLandmineInit ( owner, pos );
				
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

/**
 * 지뢰 설치 init
 */
void spawnLandmineInit ( int client, float pos[3] ) {
	if ( loringlib_IsValidClient ( client ) && ZR_IsClientHuman ( client ) ) {
		if ( g_iClientClass[client] == ClientClass_Shotgunner ) {
			int clientClass = view_as <int> ( ClientClass_Shotgunner );
			//	쿨 다운을 만든다.
			g_bSkillUse[clientClass][SKILL_ID_LANDMINE][client] = false;
			g_iSkillCooldownTicks[clientClass][SKILL_ID_LANDMINE][client] = g_iSkillCooldown[clientClass][SKILL_ID_LANDMINE];
			g_bSkillCooldown[clientClass][SKILL_ID_LANDMINE][client] = true;
		
			int landmine = CreateEntityByName ( "prop_dynamic_override" );
			if ( landmine != -1 ) {
			//	SDKHook ( landmine, SDKHook_SpawnPost, landmineSpawnOnSetModel );
				landmineSpawnOnSetModel ( landmine );
			//	DispatchKeyValue ( landmine, "model", SHOTGUNNER_LANDMINE_MODEL );
			//	DispatchKeyValue( landmine, "disablereceiveshadows", "1" );
			//	DispatchKeyValue( landmine, "disableshadows", "1" );
			//	DispatchKeyValue( landmine, "Solid", "6" );
			
				//	blocking projection. thanks you for bw.
				SetEntProp ( landmine, Prop_Send, "m_usSolidFlags", 12 );
				SetEntProp ( landmine, Prop_Data, "m_nSolidType", 6 );
				SetEntProp ( landmine, Prop_Send, "m_CollisionGroup", 1 );
			
			//	ActivateEntity ( landmine );
				DispatchSpawn ( landmine );
				
				float ang[3] = { -89.0, 0.0, 0.0 };
				pos[2] -= 2.0;
				TeleportEntity ( landmine, pos, ang, NULL_VECTOR );
				
				int glowOffs;
				if ( ( glowOffs = GetEntSendPropOffs ( landmine, "m_clrGlow" ) ) != -1 ) {
					SetEntProp ( landmine, Prop_Send, "m_bShouldGlow", true, true );
					SetEntProp ( landmine, Prop_Send, "m_nGlowStyle", 1 );
					SetEntPropFloat ( landmine, Prop_Send, "m_flGlowMaxDist", 10000000.0 );
					
					if ( g_iSkill[clientClass][SKILL_ID_LANDMINE_EX][client] > 0 ) {
						SetEntData ( landmine, glowOffs, 128, _, true );
						SetEntData ( landmine, glowOffs + 1, 65, _, true );
						SetEntData ( landmine, glowOffs + 2, 210, _, true );
						SetEntData ( landmine, glowOffs + 3, 255, _, true );
					}
					
					else {
						SetEntData ( landmine, glowOffs, 255, _, true );
						SetEntData ( landmine, glowOffs + 1, 0, _, true );
						SetEntData ( landmine, glowOffs + 2, 0, _, true );
						SetEntData ( landmine, glowOffs + 3, 255, _, true );
					}
				}
				
				loringlib_SetEntityOwner2 ( landmine, client );
				SetEntityMoveType ( landmine, MOVETYPE_NONE );
				
				SDKHook ( landmine, SDKHook_Touch, touchOnLandmineExplosion );
				
				pos[2] += 8.0;
				EmitAmbientSound ( SOUND_SHOTGUNNER_LANDMINE_DISPATCH, pos );
			}
		}
	}
}

public void landmineSpawnOnSetModel ( int entity ) {
	if ( IsValidEdict ( entity ) ) {
		PrecacheModel ( SHOTGUNNER_LANDMINE_MODEL );
		SetEntityModel ( entity, SHOTGUNNER_LANDMINE_MODEL );
	}
}

public void touchOnLandmineExplosion ( int entity, int other ) {
	if ( IsValidEdict ( entity ) ) {
		if ( loringlib_IsValidClient ( other ) && !ZR_IsClientHuman ( other ) ) {
			int owner = loringlib_GetEntityOwner2 ( entity );
			float pos[3];
			loringlib_GetEntityOriginEx ( entity, pos );
			
			EmitAmbientSound ( SOUND_SHOTGUNNER_LANDMINE_EXPLODE, pos );
			
			SetEntPropFloat ( other, Prop_Send, "m_flVelocityModifier", 0.0 );
			
			//	특이점 지뢰
			if ( loringlib_IsValidClient ( owner ) ) {
				
				DataPack data;
				CreateDataTimer ( 0.5, timerLandmineExplosionPost, data, TIMER_FLAG_NO_MAPCHANGE );
				data.WriteCell ( other );
				data.WriteCell ( GetClientUserId ( owner ) );
				data.WriteFloat ( pos[0] );
				data.WriteFloat ( pos[1] );
				data.WriteFloat ( pos[2] );
				
				if ( g_iClientClass[owner] == ClientClass_Shotgunner ) {
					int clientClass = view_as<int> ( ClientClass_Shotgunner );
					if ( g_iSkill[clientClass][SKILL_ID_LANDMINE_EX][owner] > 0 ) {
						loringlib_CreateParticleEx ( owner, 0, pos, NULL_VECTOR, "special_light2", false, 1.0 );
						loringlib_CreateParticleEx ( owner, 0, pos, NULL_VECTOR, "special_blackhole", false, SHOTGUNNER_LANDMINE_BLACKHOLE_DURATION );
						
						DataPack data2;
						CreateDataTimer ( 0.1, timerLandmineBlackhole, data2, TIMER_REPEAT | TIMER_FLAG_NO_MAPCHANGE );
						data2.WriteFloat ( GetGameTime () + SHOTGUNNER_LANDMINE_BLACKHOLE_DURATION );
						data2.WriteFloat ( pos[0] );
						data2.WriteFloat ( pos[1] );
						data2.WriteFloat ( pos[2] );
						data2.WriteCell ( GetClientUserId ( owner ) );
					//	data2.WriteFloat ( 100.0 );		//	Damages
					}
					
					else
						loringlib_CreateParticleEx ( owner, 0, pos, NULL_VECTOR, "special_light1", false, 1.0 );
				}
			}
			
			//	주인이 유효하지 않을 경우
			else {
				FakeClientCommand ( other, "say \"%d번째 지뢰(Owner: %d)는 불발 지뢰입니다. 그러므로 삭제 됩니다.\"", entity, owner );
			}
			
			AcceptEntityInput ( entity, "Kill" );
		}
	}
}

public Action timerLandmineExplosionPost ( Handle timer, DataPack data ) {
	data.Reset ();
	static int other;
	static int owner;
	float pos[3];
	
	other = data.ReadCell ();
	owner = GetClientOfUserId ( data.ReadCell () );
	pos[0] = data.ReadFloat ();
	pos[1] = data.ReadFloat ();
	pos[2] = data.ReadFloat ();
	
	int entity = CreateEntityByName ( "prop_dynamic_override" );
	if ( entity == -1 )
		return Plugin_Stop;
	
	PrecacheModel ( "models/props/ar_dizzy/dizzy_cinderblock/dizzy_cinderblock.mdl" );
	SetEntityModel ( entity, "models/props/ar_dizzy/dizzy_cinderblock/dizzy_cinderblock.mdl" );
	loringlib_SetEntityRenderColor ( entity, 0, 0, 0, 0 );
	loringlib_SetEntityNoblock ( entity, true );
	SetEntityMoveType ( entity, MOVETYPE_NONE );
	DispatchSpawn ( entity );
	TeleportEntity ( entity, pos, NULL_VECTOR, NULL_VECTOR );
	CreateTimer ( 0.5, timerExplosionTargetEntKill, entity, TIMER_FLAG_NO_MAPCHANGE );
	
	//	터치된 사람이 좀비일 경우
	if ( loringlib_IsValidClient__PlayGame ( other ) && !ZR_IsClientHuman ( other ) ) {
		//	엔티티 주인이 사람일 경우
		if ( loringlib_IsValidClient ( owner ) ) {
			float damage = 0.0;
			int clientClass = view_as <int> ( ClientClass_Shotgunner );
			if ( g_iSkill[clientClass][SKILL_ID_LANDMINE][owner] > 0 )
				damage = float ( g_iSkill[clientClass][SKILL_ID_LANDMINE][owner] ) * g_fSkillAction[clientClass][SKILL_ID_LANDMINE];
			
			//	전술 지뢰 마스터리
			if ( g_iSkill[clientClass][SKILL_ID_LANDMINE_MASTERY][owner] > 0 )
				damage += float ( g_iSkill[clientClass][SKILL_ID_LANDMINE_MASTERY][owner] ) * g_fSkillAction[clientClass][SKILL_ID_LANDMINE_MASTERY];
			
			//	particles
			pos[2] += 30.0;
			EmitAmbientSound ( SOUND_SHOTGUNNER_LANDMINE_DETECTED_EXPLODE, pos );
			//	특이점 지뢰
			if ( g_iSkill[clientClass][SKILL_ID_LANDMINE_EX][owner] > 0 ) {
				loringlib_CreateParticleEx ( owner, 0, pos, NULL_VECTOR, "special_mine_bexplode" );
			} else
				loringlib_CreateParticleEx ( owner, 0, pos, NULL_VECTOR, "special_mine_explode" );
			
			
		//	loringlib_MakeExplosion ( owner, _, pos, "weapon_hegrenade", RoundFloat ( damage ), 360, 0.0, CS_TEAM_CT, 6146 );
			for ( int target = 1; target <= MaxClients; target ++ ) {
				if ( loringlib_IsValidClient__PlayGame ( target ) ) {
					if ( !ZR_IsClientHuman ( target ) ) {
						if ( loringlib_IsTargetInSightRange ( entity, target, 360.0, SHOTGUNNER_LANDMINE_EXPLODE_DISTANCE ) ) {
							loringlib_MakeDamage ( owner, target, RoundFloat ( damage ), "weapon_hegrenade", DMG_BLAST );
							DataPack modifScaleDataPack;
							CreateDataTimer ( 0.1, timerLandmineTargetVelModifScale, modifScaleDataPack, TIMER_FLAG_NO_MAPCHANGE );
							modifScaleDataPack.WriteFloat ( GetGameTime () + SHOTGUNNER_LANDMINE_TARGET_VELMODIF_DURATION );
							modifScaleDataPack.WriteCell ( target );
						}
					}
				}
			}
		}
		
		//	주인이 사람이 아니거나, 유효하지 않을 경우
		else {
			for ( int target = 1; target <= MaxClients; target ++ ) {
				if ( loringlib_IsValidClient__PlayGame ( target ) ) {
					if ( !ZR_IsClientHuman ( target ) ) {
						if ( loringlib_IsTargetInSightRange ( entity, target, 360.0, SHOTGUNNER_LANDMINE_EXPLODE_DISTANCE ) ) {
							loringlib_MakeDamage ( 0, target, 350, "weapon_hegrenade", DMG_BLAST );
							DataPack modifScaleDataPack;
							CreateDataTimer ( 0.1, timerLandmineTargetVelModifScale, modifScaleDataPack, TIMER_FLAG_NO_MAPCHANGE );
							modifScaleDataPack.WriteFloat ( GetGameTime () + SHOTGUNNER_LANDMINE_TARGET_VELMODIF_DURATION );
							modifScaleDataPack.WriteCell ( target );
						}
					}
				}
			}
		}
	}
	
	return Plugin_Stop;
}

/**
 * 지뢰 맞은 타겟에게 경직을 준다.
 */
public Action timerLandmineTargetVelModifScale ( Handle timer, DataPack data ) {
	data.Reset ();
	
	if ( GetGameTime () >= data.ReadFloat () )
		return Plugin_Stop;
	
	static int client;
	client = data.ReadCell ();
	if ( !loringlib_IsValidClient__PlayGame ( client ) )
		return Plugin_Stop;
	
	if ( ZR_IsClientHuman ( client ) )
		return Plugin_Stop;
	
	if ( IsPlayerAlive ( client ) )
		SetEntPropFloat ( client, Prop_Send, "m_flVelocityModifier", 0.1 );
		
	return Plugin_Continue;
}

public Action timerExplosionTargetEntKill ( Handle timer, any entity ) {
	if ( IsValidEdict ( entity ) )
		AcceptEntityInput ( entity, "Kill" );
	return Plugin_Stop;
}

/**
 * 특이점 지뢰
 */
public Action timerLandmineBlackhole ( Handle timer, DataPack dp ) {
	dp.Reset ();
	
	float ticks = dp.ReadFloat ();
	
	float pos[3];
	pos[0] = dp.ReadFloat ();
	pos[1] = dp.ReadFloat ();
	pos[2] = dp.ReadFloat ();
	
	int owner = GetClientOfUserId ( dp.ReadCell () );
	
//	float damage = dp.ReadFloat ();
	
	float effPos[3];
	effPos[0] = pos[0];
	effPos[1] = pos[1];
	effPos[2] = pos[2] + 5.0;
	
	if ( !loringlib_IsValidClient ( owner ) )
		return Plugin_Stop;
		
	if ( g_iClientClass[owner] != ClientClass_Shotgunner ||
		g_iSkill[g_iClientClass[owner]][SKILL_ID_LANDMINE_EX][owner] < 1 )
		return Plugin_Stop;

	//	폭발
	if ( GetGameTime () >= ticks ) {
	/*	loringlib_CreateParticleEx ( owner, 0, effPos, NULL_VECTOR, "special_mine_bexplode" );
		for ( int i = 1; i <= MaxClients; i ++ ) {
			if ( loringlib_IsValidClient__PlayGame ( i ) ) {
				if ( !ZR_IsClientHuman ( i ) ) {
					float targetPos2[3];
					GetClientAbsOrigin ( i, targetPos2 );
					//	데미지 거리안에 왔을 경우 데미지를 입힌다.
					if ( GetVectorDistance ( pos, targetPos2 ) <= SHOTGUNNER_LANDMINE_EXPLODE_DISTANCE ) {
						loringlib_MakeDamage ( owner, i, RoundFloat ( damage ), "weapon_hegrenade", DMG_BLAST );
					}
				}
			}
		}	*/
		return Plugin_Stop;
	}
	
	for ( int target = 1; target <= MaxClients; target ++ ) {
		if ( !IsClientInGame ( target ) ||
			!IsPlayerAlive ( target ) )
			continue;
		
		if ( ZR_IsClientHuman ( target ) )
			continue;
		
		float targetPos[3];
		GetClientAbsOrigin ( target, targetPos );
		
		float distance = GetVectorDistance ( pos, targetPos );
		
		if ( distance <= SHOTGUNNER_LANDMINE_BLACKHOLE_DISTANCE ) {
			float vel[3];
			MakeVectorFromPoints ( pos, targetPos, vel );
			NormalizeVector ( vel, vel );
			ScaleVector ( vel, SHOTGUNNER_LANDMINE_BLACKHOLE_FULLFORCE );
			TeleportEntity ( target, NULL_VECTOR, NULL_VECTOR, vel );
		}
	}
	
	return Plugin_Continue;
}

/**
 * 저지력
 * @note player_hurt에서
 */
void clientClass_Shotgunner_HurtOnBlockingPower ( int victim, int attacker, char[] weapon ) {
	if ( loringlib_IsValidClient ( victim ) && loringlib_IsValidClient ( attacker ) ) {
		if ( g_iClientClass[attacker] == ClientClass_Shotgunner ) {
			int clientClass = view_as <int> ( ClientClass_Shotgunner );
			//	저지력을 배웠을 경우
			if ( g_iSkill[clientClass][SKILL_ID_BLOCKING_POWER][attacker] > 0 ) {
				//	때리는 클라이언트가 인간이고 맞는 클라이언트가 좀비일 경우
				if ( !ZR_IsClientHuman ( victim ) && ZR_IsClientHuman ( attacker ) ) {
					bool shotgun = false;
					for ( int i = 0; i < sizeof ( g_strClassWeapon_Shotgunner ); i ++ )
						if ( StrContains ( g_strClassWeapon_Shotgunner[i], weapon ) != -1 )
							shotgun = true;
					
					if ( shotgun ) {
						float blockingpower = SHOTGUNNER_BLOCKING_POWER_DEFAULT_SCALE - float ( g_iSkill[clientClass][SKILL_ID_BLOCKING_POWER][attacker] ) * g_fSkillAction[clientClass][SKILL_ID_BLOCKING_POWER];
						if ( blockingpower < 0.1 )
							blockingpower = 0.1;
						
						DataPack data;
						CreateDataTimer ( 0.1, timerHurtOnBlockingPowerPost, data, TIMER_FLAG_NO_MAPCHANGE );
						data.WriteCell ( GetClientUserId ( victim ) );
						data.WriteCell ( GetClientUserId ( attacker ) );
						data.WriteFloat ( blockingpower );
					}
				}
			}
		}
	}
}

public Action timerHurtOnBlockingPowerPost ( Handle timer, DataPack data ) {
	data.Reset ();
	static int victim;
	static int attacker;
	victim = GetClientOfUserId ( data.ReadCell () );
	attacker = GetClientOfUserId ( data.ReadCell () );
	
	if ( loringlib_IsValidClient__PlayGame ( victim ) && loringlib_IsValidClient ( attacker ) )
		SetEntPropFloat ( victim, Prop_Send, "m_flVelocityModifier", data.ReadFloat () );
	return Plugin_Stop;
}

/**
 * 폭발탄 발사 전에 샷건 데미지 구하기
 *
 * @note player_hurt에서
 */
void clientClass_Shotgunner_HurtOnGetExplodeBulletDamage ( int victim, int client, int dmg_health, char[] weapon ) {
	if ( loringlib_IsValidClient__PlayGame ( victim ) &&
		loringlib_IsValidClient__PlayGame ( client ) ) {
		if ( g_iClientClass[client] == ClientClass_Shotgunner ) {
			if ( ZR_IsClientHuman ( client ) && !ZR_IsClientHuman ( victim ) ) {
				int clientClass = view_as <int> ( ClientClass_Shotgunner );
				if ( g_iSkill[clientClass][SKILL_ID_SG_EXPLODE_PELLET][client] > 0 ) {
					bool shotgun = false;
					for ( int i = 0; i < sizeof ( g_strClassWeapon_Shotgunner ); i ++ )
						if ( StrContains ( g_strClassWeapon_Shotgunner[i], weapon, false ) != -1 )
							shotgun = true;
					
					//	샷건일 경우 데미지를 구한다.
					if ( shotgun ) {
						if ( !g_bIsShotgunFired[client] ) {
							DataPack data;
							CreateDataTimer ( 0.1, timerShotgunnerHitCallback, data, TIMER_FLAG_NO_MAPCHANGE );
							data.WriteCell ( GetClientUserId ( client ) );
							data.WriteCell ( GetClientUserId ( victim ) );
							
							g_bIsShotgunFired[client] = true;
							g_iShotgunnerDamage[client] = dmg_health;
						}
						
						else
							g_iShotgunnerDamage[client] += dmg_health;
					
				//		PrintToChatAll ( "%d damages", g_iShotgunnerDamage[client] );
					}
				}
			}
		}
	}
}

public Action timerShotgunnerHitCallback ( Handle timer, DataPack dp ) {
	dp.Reset ();
	static int client;
	static int victim;
	client = GetClientOfUserId ( dp.ReadCell () );
	victim = GetClientOfUserId ( dp.ReadCell () );
	
	if ( client <= 0 )
		return Plugin_Stop;
	
	if ( victim <= 0 ) {
		g_bIsShotgunFired[client] = false;
		g_iShotgunnerDamage[client] = 0;
	
		return Plugin_Stop;
	}

	if ( g_iClientClass[client] == ClientClass_Shotgunner ) {
		int clientClass = view_as <int> ( ClientClass_Shotgunner );
		g_iShotgunnerDamage[client] = RoundFloat ( g_iShotgunnerDamage[client] * SHOTGUNNER_EXPLODE_BULLET_DEFAULT_DMG_SCALE / 100.0 );
		g_iShotgunnerDamage[client] += RoundFloat ( ROUND_DMG_PERCENTAGE + float ( g_iSkill[clientClass][SKILL_ID_SG_EXPLODE_PELLET][client] ) );
	//	PrintToChatAll ( "tot %d damages", g_iShotgunnerDamage[client] );
		
		float pos[3];
		GetClientEyePosition ( victim, pos );
		float targetPos[3];
		for ( int target = 1; target <= MaxClients; target ++ ) {
			if ( loringlib_IsValidClient__PlayGame ( target ) ) {
				if ( !ZR_IsClientHuman ( target ) ) {
					GetClientEyePosition ( target, targetPos );
					if ( GetVectorDistance ( pos, targetPos ) <= SHOTGUNNER_EXPLODE_BULLET_RADIUS ) {
						loringlib_MakeDamage ( client, target, g_iShotgunnerDamage[client], "weapon_hegrenade", DMG_POISON );
						IgniteEntity ( target, 0.5 );
					}
				}
			}
		}
	}
	
	g_bIsShotgunFired[client] = false;
	g_iShotgunnerDamage[client] = 0;
	
	return Plugin_Stop;
}

/**
 * 폭발탄 이펙트
 * @note bullet_impact에서
 */
void clientClass_Shotgunner_ImpactOnExplodeBullet ( int client, float pos[3] ) {
	if ( loringlib_IsValidClient ( client ) ) {
		if ( g_iClientClass[client] == ClientClass_Shotgunner ) {
			if ( ZR_IsClientHuman ( client ) ) {
				int clientClass = view_as <int> ( ClientClass_Shotgunner );
				//	폭발탄을 배웠을 경우
				if ( g_iSkill[clientClass][SKILL_ID_SG_EXPLODE_PELLET][client] > 0 ) {
					//	데미지가 없다면 return 한다.
					if ( g_iShotgunnerDamage[client] <= 0 )
						return;
					
					loringlib_CreateParticleEx ( client, 0, pos, NULL_VECTOR, "special_explosiverounds", false, 0.5 );
					
				/*	float targetPos[3];
					for ( int target = 1; target <= MaxClients; target ++ ) {
						if ( loringlib_IsValidClient__PlayGame ( target ) ) {
							if ( !ZR_IsClientHuman ( target ) ) {
								GetClientAbsOrigin ( target, targetPos );
								if ( GetVectorDistance ( pos, targetPos ) <= SHOTGUNNER_EXPLODE_BULLET_RADIUS ) {
									loringlib_MakeDamage ( client, target, g_iShotgunnerDamage[client], "weapon_hegrenade", DMG_POISON );
									g_iShotgunnerDamage[client] = 0;
								}
							}
						}
					}	*/
				}
			}
		}
	}
}