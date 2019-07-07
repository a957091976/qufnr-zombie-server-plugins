public void createRewardTimer ( int client ) {
	if ( loringlib_IsValidClient ( client ) ) {
		if ( IsFakeClient ( client ) )
			return;
		
	//	killRewardTimer ( client );
		if ( g_fConfPlayRewardTime <= 0.0 )
			return;
		
		if ( g_hRewardTimer[client] == null )
			g_hRewardTimer[client] = CreateTimer ( g_fConfPlayRewardTime, timerBalanceReward, client, TIMER_REPEAT );
	}
}

public void killRewardTimer ( int client ) {
	if ( g_hRewardTimer[client] != null ) {
		KillTimer ( g_hRewardTimer[client] );
		g_hRewardTimer[client] = null;
	}
}

/**
 * Timer Handler
 */
public Action timerBalanceReward ( Handle timer, int client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Stop;
	
	if ( GetClientTeam ( client ) == CS_TEAM_SPECTATOR )
		return Plugin_Continue;

	if ( loringlib_GetTeamClients () < g_iConfRewardMinPlayers )
		return Plugin_Continue;
		
	SetHudTextParamsEx ( 0.075, 0.359, 5.0, { 204, 166, 61, 255 }, { 204, 166, 61, 255 }, 2, 0.08, 0.05, 0.05 );
	static char tag[16];
	static char buffer[128];
	CS_GetClientClanTag ( client, tag, sizeof ( tag ) );
	if ( StrEqual ( tag, g_strConfRewardBonusGroupName ) ) {
		Format ( buffer, sizeof ( buffer ), "Play Rewards: %d %s\nBonus Rewards: +%d %s", g_iConfPlayReward, g_strConfMoneyName, g_iConfPlayRewardBonus, g_strConfMoneyName );
		g_iBalance[client] += ( g_iConfPlayReward + g_iConfPlayRewardBonus );
	}
	
	else {
		Format ( buffer, sizeof ( buffer ), "Play Rewards: %d %s\nYou can get a bonus rewards when use the tag.", g_iConfPlayReward, g_strConfMoneyName );
		g_iBalance[client] += g_iConfPlayReward;
	}
	
	ShowSyncHudText ( client, g_hRewardMessageHud, buffer );

	PrecacheSound ( g_strConfSoundReward );
	EmitSoundToClient ( client, g_strConfSoundReward, _, _, _, _, g_fConfSoundVolumeReward );
	
	return Plugin_Continue;
}

/**
 * Reward kill players.
 */
public void rewardOfKills ( int victim, int attacker, int assister ) {
	if ( !loringlib_IsValidClient ( victim ) || !loringlib_IsValidClient ( attacker ) )
		return;
	
	if ( GetClientTeam ( victim ) == GetClientTeam ( attacker ) )
		return;
	
	if ( loringlib_GetTeamClients () < g_iConfMinimumPlayers )
		return;
	
	//	Original
	if ( g_iConfZombieMod != 1 ) {
		g_iBalance[attacker] += GetRandomInt ( g_iConfMinimumBalance, g_iConfMaximumBalance );
		if ( GetEngineVersion () == Engine_CSGO )
			if ( loringlib_IsValidClient ( assister ) )
				if ( g_iConfAssisterCanGetBalance == 1 )
					if ( GetClientTeam ( assister ) == GetClientTeam ( attacker ) )
						g_iBalance[assister] += GetRandomInt ( g_iConfMinimumBalanceByAssister, g_iConfMaximumBalanceByAssister );
	}
	
	//	Zombie Mod
	else {
		//	Kill enemy in Human Team.
		if ( GetClientTeam ( attacker ) == CS_TEAM_CT ) {
			g_iBalance[attacker] += GetRandomInt ( g_iConfMinimumBalance, g_iConfMaximumBalance );
			if ( GetEngineVersion () == Engine_CSGO )
				if ( loringlib_IsValidClient ( assister ) )
					if ( g_iConfAssisterCanGetBalance == 1 )
						if ( GetClientTeam ( assister ) == GetClientTeam ( attacker ) )
							g_iBalance[assister] += GetRandomInt ( g_iConfMinimumBalanceByAssister, g_iConfMaximumBalanceByAssister );
		}
		
		//	Kill enemy in Zombie Team.
		else if ( GetClientTeam ( attacker ) == CS_TEAM_T ) {
			g_iBalance[attacker] += GetRandomInt ( g_iConfMinimumBalanceByZombie, g_iConfMaximumBalanceByZombie );
		}
	}
}

/**
 * Reward round end to alive humans.
 */
public void rewardOfAliveHumans ( int winner ) {
	if ( loringlib_GetTeamClients () < g_iConfMinimumPlayers )
		return;
		
	int random;
	for ( int id = 1; id <= MaxClients; id ++ ) {
		if ( loringlib_IsValidClient__PlayGame ( id ) ) {
			if ( GetClientTeam ( id ) == winner && GetClientTeam ( id ) == CS_TEAM_CT ) {
				random = GetRandomInt ( g_iConfMinimumBalanceByAlive, g_iConfMaximumBalanceByAlive );
				PrintToChat ( id, "%s \x01생존 보상 \x10+%d %s", g_strConfChatPrefix, random, g_strConfMoneyName );
				g_iBalance[id] += random;
			}
		}
	}
}