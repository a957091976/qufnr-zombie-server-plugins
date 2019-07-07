//#define	KNOCKBACK_SCALE		3.5

#define		GRENADE_KNOCKBACK_VECTOR_SCALE	1200.0

public Action onZombieKnockback ( int victim, int &attacker, int &inf, float &dmg, int &dmgType ) {
	if ( !loringlib_IsValidClient ( victim ) ||
		!loringlib_IsValidClient ( attacker ) )
		return Plugin_Continue;
		
	if ( GetClientTeam ( attacker ) == GetClientTeam ( victim ) )
		return Plugin_Continue;
		
	if ( g_bZombie[victim] || g_bNemesis[victim] )
		if ( GetEntPropFloat ( victim, Prop_Send, "m_flVelocityModifier" ) < 0.6 )
			SetEntPropFloat ( victim, Prop_Send, "m_flVelocityModifier", 0.6 );
		
	if ( !g_bZombie[attacker] &&
		GetClientTeam ( attacker ) == CS_TEAM_CT &&
		!( dmgType & 2056 ) ) {
		SetEntPropFloat ( victim, Prop_Send, "m_flStamina", 0.0 );
		
		float eye[3], attEye[3], vec[3];
		GetClientEyePosition ( victim, eye );
		GetClientEyePosition ( attacker, attEye );
		eye[2] = 0.0;
		attEye[2] = 0.0;
		
		MakeVectorFromPoints ( attEye, eye, vec );
		NormalizeVector ( vec, vec );
		
		static char weapon[32];
		GetClientWeapon ( attacker, weapon, sizeof ( weapon ) );
		int flag = GetEntityFlags ( victim );
		float distance;
		
		GetClientEyePosition ( victim, eye );
		GetClientEyePosition ( attacker, attEye );
		
		distance = GetVectorDistance ( eye, attEye );
		
		//	NEMESIS KNOCKBACK
		if ( g_bNemesis[victim] ) {
			if ( distance <= 400.00 ) {
				if ( ( flag & FL_DUCKING ) )
					ScaleVector ( vec, dmg * 0.535 / g_fNemesisKnockbackScale );
				else
					ScaleVector ( vec, dmg * 1.247 / g_fNemesisKnockbackScale );
			}
			
			else if ( distance <= 800.00 ) {
				if ( ( flag & FL_DUCKING ) )
					ScaleVector ( vec, dmg * 0.476 / g_fNemesisKnockbackScale );
					
				else
					ScaleVector ( vec, dmg * 1.045 / g_fNemesisKnockbackScale );
			}
			
			else if ( distance > 800.00 ) {
				if ( ( flag & FL_DUCKING ) )
					ScaleVector ( vec, dmg * 0.336 / g_fNemesisKnockbackScale );
				
				else
					ScaleVector ( vec, dmg * 0.735 / g_fNemesisKnockbackScale );
			}
		}
		
		//	ZOMBIE KNOCKBACK
		else {
			//	Sniper knockback.
			if ( StrEqual ( weapon, "weapon_awp" ) ||
				StrEqual ( weapon, "weapon_ssg08" ) ||
				StrEqual ( weapon, "weapon_scout" ) ) {
				
				//	맞는사람과 때리는사람 거리가 400유닛 이하 일 경우
				if ( distance <= 400.00 ) {
					//	맞는사람이 앉아 있을 경우
					if ( ( flag & FL_DUCKING ) )
						//	여기서 2.456 부분을 	높이면 넉백 증가
						ScaleVector ( vec, dmg * 1.8 / g_fZombieKnockbackScale[g_iZombieType[victim]][victim] );
					
					//	일어서있을 경우
					else
						ScaleVector ( vec, dmg * 4.4 / g_fZombieKnockbackScale[g_iZombieType[victim]][victim] );
				}
				
				//	거리가 800유닛 이하 일 경우
				else if ( distance <= 800.00 ) {
					if ( ( flag & FL_DUCKING ) )
						ScaleVector ( vec, dmg * 1.0 / g_fZombieKnockbackScale[g_iZombieType[victim]][victim] );
						
					else
						ScaleVector ( vec, dmg * 3.4 / g_fZombieKnockbackScale[g_iZombieType[victim]][victim] );
				}
				
				//	800유닛 이상일 경우
				else if ( distance > 800.00 ) {
					if ( ( flag & FL_DUCKING ) )
						ScaleVector ( vec, dmg * 0.8 / g_fZombieKnockbackScale[g_iZombieType[victim]][victim] );
					
					else
						ScaleVector ( vec, dmg * 2.3 / g_fZombieKnockbackScale[g_iZombieType[victim]][victim] );
				}
				
				//	다른 무기도 동일합니다.
				//	if문 내려갈 수록 거리가 멀어짐.
				
				
	//			if ( distance >= 2000.0 )
	//				distance = 1999.9;
				
	//			dmg *= 2.0;
				
				/**
				 * 넉백 계산 순서:
				 * (피해량 × 넉백 스케일) - (거리 ÷ 10.0) ÷ 좀비 넉백 스케일
				 */
				
				// 앉은 상태에서는 기본 피해량 나누기 1.5를 합니다.
	/*			if ( ( flag & FL_DUCKING ) &&
					( flag & FL_ONGROUND ) )
					ScaleVector ( vec, ( ( ( dmg / 1.9 ) * KNOCKBACK_SCALE ) - ( distance / 10.0 ) ) / g_fZombieKnockbackScale[g_iZombieType[victim]][victim] );
					
				else
					ScaleVector ( vec, ( ( dmg * KNOCKBACK_SCALE ) - ( distance / 10.0 ) ) / g_fZombieKnockbackScale[g_iZombieType[victim]][victim] );
	*/
			}
			
			//	Shotguns
			else if ( StrEqual ( weapon, "weapon_nova" ) ||
				StrEqual ( weapon, "weapon_xm1014" ) ||
				StrEqual ( weapon, "weapon_mag7" ) ||
				StrEqual ( weapon, "weapon_m3" ) ) {
				if ( distance <= 400.00 ) {
					if ( ( flag & FL_DUCKING ) )
						ScaleVector ( vec, dmg * 0.8 / g_fZombieKnockbackScale[g_iZombieType[victim]][victim] );
						
					else
						ScaleVector ( vec, dmg * 1.8 / g_fZombieKnockbackScale[g_iZombieType[victim]][victim] );
				}
				
				else if ( distance <= 800.00 ) {
					if ( ( flag & FL_DUCKING ) )
						ScaleVector ( vec, dmg * 0.6 / g_fZombieKnockbackScale[g_iZombieType[victim]][victim] );
						
					else
						ScaleVector ( vec, dmg * 1.4 / g_fZombieKnockbackScale[g_iZombieType[victim]][victim] );
				}
				
				else if ( distance > 800.00 ) {
					if ( ( flag & FL_DUCKING ) )
						ScaleVector ( vec, dmg * 0.4 / g_fZombieKnockbackScale[g_iZombieType[victim]][victim] );
					
					else
						ScaleVector ( vec, dmg * 1.0 / g_fZombieKnockbackScale[g_iZombieType[victim]][victim] );
				}
			}
			
			//	another
			else {
				
				if ( distance <= 400.00 ) {
					if ( ( flag & FL_DUCKING ) )
						ScaleVector ( vec, dmg * 1.6 / g_fZombieKnockbackScale[g_iZombieType[victim]][victim] );
						
					else
						ScaleVector ( vec, dmg * 2.8 / g_fZombieKnockbackScale[g_iZombieType[victim]][victim] );
				}
				
				else if ( distance <= 800.00 ) {
					if ( ( flag & FL_DUCKING ) )
						ScaleVector ( vec, dmg * 1.3 / g_fZombieKnockbackScale[g_iZombieType[victim]][victim] );
						
					else
						ScaleVector ( vec, dmg * 2.5 / g_fZombieKnockbackScale[g_iZombieType[victim]][victim] );
				}
				
				else if ( distance > 800.00 ) {
					if ( ( flag & FL_DUCKING ) )
						ScaleVector ( vec, dmg * 0.8 / g_fZombieKnockbackScale[g_iZombieType[victim]][victim] );
					
					else
						ScaleVector ( vec, dmg * 2.0 / g_fZombieKnockbackScale[g_iZombieType[victim]][victim] );
				}
				
	//			if ( distance >= 900.0 )
	//				distance = 899.9;
				
				//	위와 동일
	/*			if ( ( flag & FL_DUCKING ) &&
					( flag & FL_ONGROUND ) )
					ScaleVector ( vec, ( ( ( dmg / 1.3 ) * KNOCKBACK_SCALE ) - ( distance / 10.0 ) ) / g_fZombieKnockbackScale[g_iZombieType[victim]][victim] );
					
				else
					ScaleVector ( vec, ( ( dmg * KNOCKBACK_SCALE ) - ( distance / 10.0 ) ) / g_fZombieKnockbackScale[g_iZombieType[victim]][victim] );
	*/		}
		}
		
//		PrintToChatAll ( "Distance: %.3f", distance );
//		PrintToChatAll ( "Distance Filter: %.3f", distance / 10.0 );
		
		float vel[3], realKnockback[3];
		GetEntPropVector ( victim, Prop_Data, "m_vecVelocity", vel );
		AddVectors ( vel, vec, realKnockback );
		
		TeleportEntity ( victim, NULL_VECTOR, NULL_VECTOR, realKnockback );
		
//		PrintToChatAll ( "Vectors: %.3f %.3f %.3f", vec[0], vec[1], vec[2] );
//		PrintToChatAll ( "Knockback Scale: %.3f %.3f %.3f", realKnockback[0], realKnockback[1], realKnockback[2] );
		
		return Plugin_Continue;
	}
	
	return Plugin_Continue;
}

void onZombieGrenadeKnockback ( int victim, int attacker, int dmgtype ) {
	if ( loringlib_IsValidClient__PlayGame ( victim ) && loringlib_IsValidClient__PlayGame ( attacker ) ) {
		if ( ( g_bZombie[victim] || g_bNemesis[victim] ) && !g_bZombie[attacker] ) {
			if ( dmgtype == DMG_BLAST ) {
				float eye[3], attEye[3], vec[3];
				GetClientEyePosition ( victim, eye );
				GetClientEyePosition ( attacker, attEye );
				eye[2] = 0.0;
				attEye[2] = 0.0;
				
				MakeVectorFromPoints ( attEye, eye, vec );
				NormalizeVector ( vec, vec );
				ScaleVector ( vec, GRENADE_KNOCKBACK_VECTOR_SCALE );
				
				float vel[3];
				GetEntPropVector ( victim, Prop_Data, "m_vecVelocity", vel );
				AddVectors ( vel, vec, vec );
				
				TeleportEntity ( victim, NULL_VECTOR, NULL_VECTOR, vec );
				IgniteEntity ( victim, 3.0 );
				return;
			}
		}
	}
}

void onZombieKnockbackNewVector ( int victim, int attacker, int damage, int hitgroup, char[] weapon ) {
	if ( g_bGameEnd || !g_bGameStart )
		return;
	
	if ( !loringlib_IsValidClient ( victim ) ||
		!loringlib_IsValidClient ( attacker ) )
		return;
		
	if ( GetClientTeam ( attacker ) == GetClientTeam ( victim ) )
		return;
		
//	PrintToChatAll ( "m_flVelocityModifier %.2f", GetEntPropFloat ( victim, Prop_Send, "m_flVelocityModifier" ) );
		
	if ( g_bZombie[victim] || g_bNemesis[victim] &&
		GetClientTeam ( attacker ) == CS_TEAM_CT ) {
	//	SetEntPropFloat ( victim, Prop_Send, "m_flVelocityModifier", 1.0 );
		SetEntPropFloat ( victim, Prop_Send, "m_flStamina", 0.0 );
		
		float velModif;
		float eye[3], attEye[3], vec[3];
		GetClientEyePosition ( victim, eye );
		GetClientEyePosition ( attacker, attEye );
		eye[2] = 0.0;
		attEye[2] = 0.0;
		
		MakeVectorFromPoints ( attEye, eye, vec );
		NormalizeVector ( vec, vec );
		
		int flag = GetEntityFlags ( victim );

		GetClientEyePosition ( victim, eye );
		GetClientEyePosition ( attacker, attEye );
		
		float distance = GetVectorDistance ( eye, attEye );
		float dmg = float ( damage );
		float knockbackScale = g_bNemesis[victim] ? g_fNemesisKnockbackScale : g_fZombieKnockbackScale[g_iZombieType[victim]][victim];
		float scale_vector;
		float add_vector;
		
		//	ADDITIONAL VECTOR SCALES
		if ( StrEqual ( weapon, "awp" ) )			add_vector = 2.5;	//	awp
		else if ( StrEqual ( weapon, "ssg08" ) )	add_vector = 2.2;	//	ssg08
		else if ( StrEqual ( weapon, "scar20" ) ||
			StrEqual ( weapon, "g3sg1" ) )			add_vector = -0.5;	//	ttak ttak ii
/*		else if ( StrEqual ( weapon, "nova" ) ||
			StrEqual ( weapon, "m3" ) ||
			StrEqual ( weapon, "xm1014" ) ||
			StrEqual ( weapon, "mag7" ) ||
			StrEqual ( weapon, "sawedoff" ) )		add_vector = -1.0;	*///	shotgun
		else if ( StrContains ( weapon, "knife" ) != -1 )	return;		//	knife
		else										add_vector = 0.0;	//	default no vector.
		
		if ( g_bZombie[victim] || g_bNemesis[victim] ) {
			if ( dmg >= 700.0 )	//	데미지가 700이 넘을 경우
				dmg = 700.0;
		
			//	경직 설정
			if ( distance <= 400.0 ) {	//	거리가 400유닛 이하일 경우
				//	ADDITIONAL VelocityModifier SCALES
				if ( loringlib_IsWeaponPistol ( weapon ) )			velModif = 0.2;
				else if ( loringlib_IsWeaponSMGs ( weapon ) )		velModif = 0.3;
				else if ( loringlib_IsWeaponShotgun ( weapon ) )	velModif = 0.5;
				else if ( loringlib_IsWeaponSniper ( weapon ) )		velModif = 0.1;
				else if ( loringlib_IsWeaponAutoSniper ( weapon ) )	velModif = 0.15;
				else if ( loringlib_IsWeaponARs ( weapon ) )		velModif = 0.2;
				else												velModif = 1.0;
			}
			else {
				if ( loringlib_IsWeaponPistol ( weapon ) )			velModif = 0.5;
				else if ( loringlib_IsWeaponSMGs ( weapon ) )		velModif = 0.6;
				else if ( loringlib_IsWeaponShotgun ( weapon ) )	velModif = 0.6;
				else if ( loringlib_IsWeaponSniper ( weapon ) )		velModif = 0.3;
				else if ( loringlib_IsWeaponAutoSniper ( weapon ) )	velModif = 0.4;
				else if ( loringlib_IsWeaponARs ( weapon ) )		velModif = 0.4;
				else												velModif = 1.0;
			}
			
			SetEntPropFloat ( victim, Prop_Send, "m_flVelocityModifier", velModif );
		
			//	넉백 설정
			if ( distance <= 800.0 ) {	//	거리가 800유닛 이하일 경우
				//	샷건일 경우
				if ( loringlib_IsWeaponShotgun ( weapon ) ) {
					switch ( hitgroup ) {
						case HITGROUP_HEAD:		scale_vector = dmg * ( 3.0 ) / knockbackScale;
						case HITGROUP_CHEST:	scale_vector = dmg * ( 5.0 ) / knockbackScale;
						case HITGROUP_STOMACH:	scale_vector = dmg * ( 5.0 ) / knockbackScale;
						case HITGROUP_LEFTARM:	scale_vector = dmg * ( 4.2 ) / knockbackScale;
						case HITGROUP_RIGHTARM:	scale_vector = dmg * ( 4.2 ) / knockbackScale;
						case HITGROUP_LEFTLEG:	scale_vector = dmg * ( 3.5 ) / knockbackScale;
						case HITGROUP_RIGHTLEG:	scale_vector = dmg * ( 3.5 ) / knockbackScale;
					//	default: 				scale_vector = dmg * ( add_vector ) / knockbackScale;
						default: return;
					}
				}
				
				else {
					switch ( hitgroup ) {
						case HITGROUP_HEAD:		scale_vector = dmg * ( 2.8 + add_vector ) / knockbackScale;
						case HITGROUP_CHEST:	scale_vector = dmg * ( 5.5 + add_vector ) / knockbackScale;
						case HITGROUP_STOMACH:	scale_vector = dmg * ( 5.5 + add_vector ) / knockbackScale;
						case HITGROUP_LEFTARM:	scale_vector = dmg * ( 5.0 + add_vector ) / knockbackScale;
						case HITGROUP_RIGHTARM:	scale_vector = dmg * ( 5.0 + add_vector ) / knockbackScale;
						case HITGROUP_LEFTLEG:	scale_vector = dmg * ( 4.5 + add_vector ) / knockbackScale;
						case HITGROUP_RIGHTLEG:	scale_vector = dmg * ( 4.5 + add_vector ) / knockbackScale;
					//	default: 				scale_vector = dmg * ( add_vector ) / knockbackScale;
						default: return;
					}
				}
			}
			
			else {
				switch ( hitgroup ) {
					case HITGROUP_HEAD:		scale_vector = dmg * ( 1.8 ) / knockbackScale;
					case HITGROUP_CHEST:	scale_vector = dmg * ( 3.8 ) / knockbackScale;
					case HITGROUP_STOMACH:	scale_vector = dmg * ( 3.8 ) / knockbackScale;
					case HITGROUP_LEFTARM:	scale_vector = dmg * ( 1.9 ) / knockbackScale;
					case HITGROUP_RIGHTARM:	scale_vector = dmg * ( 1.9 ) / knockbackScale;
					case HITGROUP_LEFTLEG:	scale_vector = dmg * ( 1.4 ) / knockbackScale;
					case HITGROUP_RIGHTLEG:	scale_vector = dmg * ( 1.4 ) / knockbackScale;
				//	default: 				scale_vector = dmg / knockbackScale;
					default: return;
				}
			}
			

			//	Is zombie now ducking.
			if ( flag & FL_DUCKING )
				if ( scale_vector > 0.0 )
					scale_vector = scale_vector / 1.75;
		}
		
		ScaleVector ( vec, scale_vector );
		
		float vel[3];
		GetEntPropVector ( victim, Prop_Data, "m_vecVelocity", vel );
		AddVectors ( vel, vec, vec );
		
		TeleportEntity ( victim, NULL_VECTOR, NULL_VECTOR, vec );
		
	//	loringlib_SetEntityLaggedmovement ( victim, g_fZombieSpeed[g_iZombieType[victim]][victim] / 2.0 );
	//	CreateTimer ( 0.1, timerVictimSlowdown, victim, TIMER_FLAG_NO_MAPCHANGE	);
	}
}

public Action timerVictimSlowdown ( Handle timer, any victim ) {
	if ( !loringlib_IsValidClient ( victim ) )
		return Plugin_Stop;
	
	if ( g_bGameEnd && !g_bGameStart ) {
		loringlib_SetEntityLaggedmovement ( victim, g_fZombieSpeed[g_iZombieType[victim]][victim] );
		return Plugin_Stop;
	}
	
	if ( !g_bGameStart ) {
		loringlib_SetEntityLaggedmovement ( victim, 1.0 );
		return Plugin_Stop;
	}
	
	loringlib_SetEntityLaggedmovement ( victim, g_fZombieSpeed[g_iZombieType[victim]][victim] );
	return Plugin_Stop;
}