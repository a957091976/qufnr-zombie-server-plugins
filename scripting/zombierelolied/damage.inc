public Action onTakeDamage ( int victim, int &attacker, int &inflictor, float &dmg, int &dmgType ) {
	zombieSoundsOnTakeDamages ( victim );
	onZombieGrenadeKnockback ( victim, attacker, dmgType );
	
	if ( !loringlib_IsValidClient ( victim ) ||
		!loringlib_IsValidClient ( attacker ) )
		return Plugin_Continue;
	
	if ( inflictor != -1 ) {
		char infName[32];
		GetEdictClassname ( inflictor, infName, sizeof ( infName ) );
//		PrintToChatAll ( "inf name: %s (%i)", infName, inflictor );
		
		//	인간 피해 무효
		if ( !g_bZombie[victim] ) {
			if ( StrContains ( infName, "projectile", false ) != -1 ||
				StrContains ( infName, "inferno", false ) != -1 )
				return Plugin_Handled;
		}
	}

/*	if ( !g_bZombie[victim] ) {
		//	폭발, 화염병 피해, 투사체 피해 받을 시 무효
		if ( dmgType & 64 ||
			( ( dmgType & 8 ) && ( dmg <= 8.0 ) ) ||
			dmgType & 128 )
			return Plugin_Handled;
	}	*/
	
	//	라운드가 끝났을 경우 무효
	if ( g_bGameEnd )
		return Plugin_Handled;
	
	return Plugin_Continue;
}