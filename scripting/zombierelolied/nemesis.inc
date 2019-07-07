float g_fNemesisPreviousVel[MAXPLAYERS + 1][3];
float g_fNemesisJumpCooldownTicks;
bool g_bNemesisJumpCooldown;
bool g_bNemesisJumpCheck[MAXPLAYERS + 1];

/**
 * Kill Nemesis dynamic light.
 * @note RoundEnd & PlayerDeath
 */
public void killNemesisDynamicLight ( int target ) {
	if ( target == -1 ) {
		if ( IsValidEdict ( g_iNemesisEffect ) && IsValidEntity ( g_iNemesisEffect ) )
			AcceptEntityInput ( g_iNemesisEffect, "Kill" );
	}
	else {
		if ( loringlib_IsValidClient ( target ) ) {
			if ( g_bNemesis[target] ) {
				if ( IsValidEdict ( g_iNemesisEffect ) && IsValidEntity ( g_iNemesisEffect ) )
					AcceptEntityInput ( g_iNemesisEffect, "Kill" );
			}
		}
	}
}

/**
 * Infect to nemesis.
 */
public void infectHumanToNemesis ( int client ) {
	if ( !loringlib_IsValidClient__PlayGame ( client ) )
		return;
	
	g_bZombie[client] = true;
	g_bNemesis[client] = true;
	g_bNemesisJumpCheck[client] = false;
	g_bNemesisJumpCooldown = true;
	g_fNemesisJumpCooldownTicks = g_fNemesisLJCooldown;
	CreateTimer ( 0.1, timerNemesisJumpCooldown, client, TIMER_REPEAT | TIMER_FLAG_NO_MAPCHANGE );
	
	if ( g_iConfDebug ) {
		PrintToServer ( "Called: infectHumanToNemesis ();" );
		PrintToServer ( "Target(Parameter): %N(%d)", client, client );
		PrintToServer ( "Model: \"%s\"", g_strNemesisModel );
		PrintToServer ( "Model2: \"%s\"", g_strNemesisArmsModel );
	}
	
	CS_SwitchTeam ( client, CS_TEAM_T );
	loringlib_ShowShakeMessage ( client, 30.0, 3.0 );
	
	int health_math = g_iNemesisHealthMath * loringlib_GetTeamClients ();
	
	loringlib_SetEntityMaxHealth ( client, g_iNemesisHealth + health_math );
	SetEntityHealth ( client, loringlib_GetEntityMaxHealth ( client ) );
	
	PrecacheModel ( g_strNemesisModel );
	SetEntityModel ( client, g_strNemesisModel );
	if ( GetEngineVersion () == Engine_CSGO )
	//	FPVMI_AddViewModelToClient ( client, "weapon_knife", PrecacheModel ( g_strNemesisArmsModel ) );
		loringlib_SetEntityArmsModel ( client, g_strNemesisArmsModel );
	
	zombieSoundsOnClientInfected ( client );
	nemesisWeaponSlotClear ( client );
	
	int entity;
	float pos[3];
	GetClientAbsOrigin ( client, pos );
	loringlib_CreateParticle ( client, entity, pos, "zombie_infection2", true );
	if ( IsValidEdict ( g_iNemesisEffect ) && IsValidEntity ( g_iNemesisEffect ) )
		AcceptEntityInput ( g_iNemesisEffect, "Kill" );
	g_iNemesisEffect = loringlib_EffectLightDynamic ( pos, "255 0 0 255", 450.0, 300.0, 0.0, true, client );
	
	Call_StartForward ( g_hForwardHandlers[ZR_FORWARD_ON_CHANGE_NEMESIS] );
	Call_PushCell ( client );
	Call_Finish ();
}

public void nemesisWeaponSlotClear ( int client ) {
	if ( SDKHookEx ( client, SDKHook_WeaponCanUse, onWeaponPickup ) )
		SDKUnhook ( client, SDKHook_WeaponCanUse, onWeaponPickup );
	
	if ( GetEngineVersion () == Engine_CSGO )
	//	FPVMI_AddViewModelToClient ( client, "weapon_knife", PrecacheModel ( g_strNemesisArmsModel ) );
		loringlib_SetEntityArmsModel ( client, g_strNemesisArmsModel );
	
	loringlib_KillWeaponSlot ( client );
	GivePlayerItem ( client, "weapon_knife" );
	
	loringlib_SetEntityAccount ( client, g_iConfGameZombieStartMoney );
	CreateTimer ( 0.1, timerNemesisRehooking, client );
}

public Action timerNemesisRehooking ( Handle timer, int client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Stop;
		
	loringlib_RemoveWeaponSlot ( client, CS_SLOT_PRIMARY );
	loringlib_RemoveWeaponSlot ( client, CS_SLOT_SECONDARY );
	loringlib_RemoveWeaponSlot ( client, CS_SLOT_GRENADE );
	loringlib_RemoveWeaponSlot ( client, CS_SLOT_C4 );
	
	SDKHook ( client, SDKHook_WeaponCanUse, onWeaponPickup );
	
	loringlib_SetEntityLaggedmovement ( client, g_fNemesisSpeed );
	SetEntityGravity ( client, g_fNemesisGravity );
	
	return Plugin_Stop;
}

void nemesisJumped ( int client ) {
	if ( loringlib_IsValidClient ( client ) ) {
		if ( g_bNemesis[client] ) {
			GetEntPropVector ( client, Prop_Data, "m_vecVelocity", g_fNemesisPreviousVel[client] );
			g_bNemesisJumpCheck[client] = true;
		}
	}
}

void nemesisFootstep ( int client ) {
	if ( loringlib_IsValidClient ( client ) )
		if ( g_bNemesis[client] )
			g_bNemesisJumpCheck[client] = false;
}

void nemesisLongJump ( int client ) {
	if ( g_bNemesis[client] ) {
		for ( int i = 1; i <= MaxClients; i ++ ) {
			if ( loringlib_IsValidClient ( i ) ) {
				if ( i != client ) {
					char hp[16];
					loringlib_NumberFormat ( GetClientHealth ( client ), hp, sizeof ( hp ) );
					if ( IsPlayerAlive ( client ) )
						PrintHintText ( i, "<font class='fontSize-l'>NEMESIS HEALTH: <font color='#ff0000' class='fontSize-xl'>%s</font> HP</font>", hp );
					else
						PrintHintText ( i, "<font class='fontSize-l'>NEMESIS HEALTH: <font class='fontSize-xl' color='#993800'><b>！</b></font></font>" );
				}
			}
		}
	
		if ( IsPlayerAlive ( client ) ) {
			if ( g_bNemesisJumpCooldown )
				PrintHintText ( client, "Nemesis Long Jump: <font color='#ff0000'>COOLDOWN</font>(%.1fs)", g_fNemesisJumpCooldownTicks );
			else
				PrintHintText ( client, "Nemesis Long Jump: <font color='#00ff00'>READY!!!</font>" );
		}
		
		if ( !g_bNemesisJumpCooldown ) {
			if ( g_bNemesisJumpCheck[client] ) {
				float vel[3];
				GetEntPropVector ( client, Prop_Data, "m_vecVelocity", vel );
				if ( vel[2] > g_fNemesisPreviousVel[client][2] ) {
					float increase = g_fNemesisLJScale + 1.0;
					vel[0] *= increase;
					vel[1] *= increase;
					
					TeleportEntity ( client, NULL_VECTOR, NULL_VECTOR, vel );
					g_bNemesisJumpCheck[client] = false;
					g_bNemesisJumpCooldown = true;
					g_fNemesisJumpCooldownTicks = g_fNemesisLJCooldown;
					CreateTimer ( 0.1, timerNemesisJumpCooldown, client, TIMER_REPEAT | TIMER_FLAG_NO_MAPCHANGE );
				}
			}
		}
	}
}

public Action timerNemesisJumpCooldown ( Handle timer, any client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Stop;
	
	if ( !IsPlayerAlive ( client ) )
		return Plugin_Stop;
		
	if ( g_fNemesisJumpCooldownTicks > 0.0 )
		g_fNemesisJumpCooldownTicks -= 0.1;
	
	if ( g_fNemesisJumpCooldownTicks <= 0.0 ) {
		g_bNemesisJumpCooldown = false;
		return Plugin_Stop;
	}
	
	return Plugin_Continue;
}