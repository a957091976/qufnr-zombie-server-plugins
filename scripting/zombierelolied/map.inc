/**
 * 맵을 불러올 때 이 맵이 화이트리스트 맵인지 채크
 */
public void Map_MapStartOnLoadWhitelistMaps () {
	static char levelName[128];
	GetCurrentMap ( levelName, sizeof ( levelName ) );
	ReplaceString ( levelName, sizeof ( levelName ), "/", "#" );
	
	static char temp[256];
	BuildPath ( Path_SM, temp, sizeof ( temp ), "data/zombierelolied/whitelist_levels/%s.txt", levelName );
	
	KeyValues kv = new KeyValues ( "whitelist_levels" );
	if ( kv.ImportFromFile ( temp ) )
		g_bIsWhitelistLevel = true;
	else
		g_bIsWhitelistLevel = false;
	
	delete kv;
}

/**
 * 밤으로 만들기
 */
public void onChangeMapStyle () {
	if ( g_iConfGameMapNightMode == 1 ) {
		loringlib_SetMapStyle ( "b", "sky_csgo_night02" );	//	Change night map style.
		loringlib_SetSunEntity ( false );
	}
}

/**
 * 맵 엔티티 삭제
 */
public void onRemoveMapEntity () {
	static char class[64];
	for ( int i = MaxClients; i <= GetMaxEntities (); i ++ ) {
		if ( !IsValidEdict ( i ) || !IsValidEntity ( i ) )
			continue;
			
		GetEdictClassname ( i, class, sizeof ( class ) );
		if ( StrEqual ( class, "hostage_entity" ) ||
			StrEqual ( class, "func_hostage_rescue" ) ||
			StrEqual ( class, "func_bomb_target" ) )
			RemoveEdict ( i );
	}
}