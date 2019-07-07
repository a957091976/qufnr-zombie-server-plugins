Handle g_hClientZombieRechargeHealthHandler[MAXPLAYERS + 1] = { null, ... };

float g_fNewGenenration[MAXPLAYERS + 1] = { 0.0, ... };

public void createHealthRechargeTimer ( int client ) {
	g_fNewGenenration[client] = GetGameTime ();
}

/**
 * Health Recharger
 */
public void zombieRegargeHealthLooping ( int client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return;
	
	if ( !g_bZombie[client] )
		return;
	
	if ( !IsPlayerAlive ( client ) )
		return;
	
	float now = GetGameTime ();
	if ( g_fNewGenenration[client] <= now ) {
		//	nemesis recharge health.
		if ( g_bNemesis[client] ) {
			g_fNewGenenration[client] = now + g_fNemesisRechargeHealthTime;
			loringlib_SetEntityHealth ( client, loringlib_GetEntityHealth ( client ) + g_iNemesisRechargeHealthScale );
			if ( loringlib_GetEntityHealth ( client ) > loringlib_GetEntityMaxHealth ( client ) )
				loringlib_SetEntityHealth ( client, loringlib_GetEntityMaxHealth ( client ) );
		}
		
		//	another zombies.
		else {
			g_fNewGenenration[client] = now + g_fZombieClassRechargeHealthTime[g_bMotherZombie[client] ? ZR_MOTHER_ZOMBIE_CLASS : ZR_NORMAL_ZOMBIE_CLASS][g_iClientZombieClass[client]];
		
			int zombie = g_iClientZombieClass[client];
			loringlib_SetEntityHealth ( client, loringlib_GetEntityHealth ( client ) + g_iZombieClassRechargeHealthScale[g_iZombieType[client]][zombie] );
			if ( loringlib_GetEntityHealth ( client ) > loringlib_GetEntityMaxHealth ( client ) )
				loringlib_SetEntityHealth ( client, loringlib_GetEntityMaxHealth ( client ) );
		}
	}
}