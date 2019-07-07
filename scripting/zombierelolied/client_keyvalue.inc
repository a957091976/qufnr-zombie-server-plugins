public void loadClientData ( int client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return;
	
	
	clearZmarketValues ( client );
	if ( StrEqual ( g_strPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS], "" ) )
		g_strPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS] = ZR_ZMARKET_DEFAULT_WEAPON_NAME;
	if ( StrEqual ( g_strSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS], "" ) )
		g_strSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS] = ZR_ZMARKET_DEFAULT_WEAPON_NAME;
	g_iClientZombieClass[client] = 0;
	
	static char dbpath[PLATFORM_MAX_PATH];
	static char steam[32];
	static char steam_temp[32];
	GetClientAuthId ( client, AuthId_SteamID64, steam, sizeof ( steam ) );
	if ( IsFakeClient ( client ) )
		strcopy ( steam, sizeof ( steam ), "bot" );
		
	BuildPath ( Path_SM, dbpath, PLATFORM_MAX_PATH, "data/zombierelolied/client_db/%s.txt", steam );
	
	KeyValues kv = new KeyValues ( "data" );
	if ( !kv.ImportFromFile ( dbpath ) ) {
		delete kv;
		return;
	}
	
	if ( !kv.GotoFirstSubKey () ) {
		delete kv;
		return;
	}
	
	do {
		kv.GetSectionName ( steam_temp, sizeof ( steam_temp ) );
		if ( StrEqual ( steam_temp, steam ) ) {
			g_iClientZombieClass[client] = kv.GetNum ( "class_id" );
			kv.GetString ( "primary_weapon_name", g_strPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_NAME], ZR_ZMARKET_WEAPON_DATA_LENGTH );
			kv.GetString ( "primary_weapon_alias", g_strPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS], ZR_ZMARKET_WEAPON_DATA_LENGTH );
			g_iPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_PRICE] = kv.GetNum ( "primary_weapon_price" );
			g_iPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_TEAMIDX] = kv.GetNum ( "primary_weapon_teamindex" );
			kv.GetString ( "secondary_weapon_name", g_strSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_NAME], ZR_ZMARKET_WEAPON_DATA_LENGTH );
			kv.GetString ( "secondary_weapon_alias", g_strSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS], ZR_ZMARKET_WEAPON_DATA_LENGTH );
			g_iSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_PRICE] = kv.GetNum ( "secondary_weapon_price" );
			g_iSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_TEAMIDX] = kv.GetNum ( "secondary_weapon_teamindex" );
			if ( g_iAutobuyMode[client] == -1 )
				g_iAutobuyMode[client] = 0;
			g_iAutobuyMode[client] = kv.GetNum ( "autobuy_toggle" );
			
			delete kv;
			return;
		}
	} while ( kv.GotoNextKey () );
	
	delete kv;
	/*
	//	Primary weapon data.
	kv.JumpToKey ( steam, false );
	kv.GetString ( "primary_weapon_name", g_strPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_NAME], ZR_ZMARKET_WEAPON_DATA_LENGTH );
	
	kv.JumpToKey ( steam, false );
	kv.GetString ( "primary_weapon_alias", g_strPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS], ZR_ZMARKET_WEAPON_DATA_LENGTH );
	
	kv.JumpToKey ( steam, false );
	g_iPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_PRICE] = kv.GetNum ( "primary_weapon_price" );
	
	kv.JumpToKey ( steam, false );
	g_iPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_TEAMIDX] = kv.GetNum ( "primary_weapon_teamindex" );
	
	//	Secondary weapon data.
	kv.JumpToKey ( steam, false );
	kv.GetString ( "secondary_weapon_name", g_strSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_NAME], ZR_ZMARKET_WEAPON_DATA_LENGTH );
	
	kv.JumpToKey ( steam, false );
	kv.GetString ( "secondary_weapon_alias", g_strSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS], ZR_ZMARKET_WEAPON_DATA_LENGTH );
	
	kv.JumpToKey ( steam, false );
	g_iSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_PRICE] = kv.GetNum ( "secondary_weapon_price" );
	
	kv.JumpToKey ( steam, false );
	g_iSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_TEAMIDX] = kv.GetNum ( "secondary_weapon_teamindex" );
	
	if ( g_iAutobuyMode[client] == -1 )
		g_iAutobuyMode[client] = 0;
	kv.JumpToKey ( steam, false );
	g_iAutobuyMode[client] = kv.GetNum ( "autobuy_toggle" );
		
	if ( StrEqual ( g_strPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS], "" ) )
		g_strPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS] = ZR_ZMARKET_DEFAULT_WEAPON_NAME;
	if ( StrEqual ( g_strSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS], "" ) )
		g_strSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS] = ZR_ZMARKET_DEFAULT_WEAPON_NAME;
	
	delete kv;
	*/
}

public void saveClientData ( int client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return;
	/*
	if ( IsFakeClient ( client ) )
		return;
	
	if ( StrEqual ( g_strPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS], ZR_ZMARKET_DEFAULT_WEAPON_NAME ) &&
		StrEqual ( g_strSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS], ZR_ZMARKET_DEFAULT_WEAPON_NAME ) )
		return;
	*/
	static char dbpath[PLATFORM_MAX_PATH];
	static char steam[32];
	GetClientAuthId ( client, AuthId_SteamID64, steam, sizeof ( steam ) );
	if ( IsFakeClient ( client ) )
		strcopy ( steam, sizeof ( steam ), "bot" );
	BuildPath ( Path_SM, dbpath, PLATFORM_MAX_PATH, "data/zombierelolied/client_db/%s.txt", steam );
	
	KeyValues kv = new KeyValues ( "zmarket_client_db" );
/*	if ( !kv.ImportFromFile ( dbpath ) ) {
		delete kv;
		return;
	}	*/
	
	if ( kv.JumpToKey ( steam, true ) ) {
		kv.SetNum ( "class_id", g_iClientZombieClass[client] );
		kv.SetString ( "primary_weapon_name", g_strPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_NAME] );
		kv.SetString ( "primary_weapon_alias", g_strPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS] );
		kv.SetNum ( "primary_weapon_price", g_iPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_PRICE] );
		kv.SetNum ( "primary_weapon_teamindex", g_iPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_TEAMIDX] );
		kv.SetString ( "secondary_weapon_name", g_strSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_NAME] );
		kv.SetString ( "secondary_weapon_alias", g_strSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS] );
		kv.SetNum ( "secondary_weapon_price", g_iSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_PRICE] );
		kv.SetNum ( "secondary_weapon_teamindex", g_iSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_TEAMIDX] );
		kv.SetNum ( "autobuy_toggle", g_iAutobuyMode[client] );
		kv.Rewind ();
		kv.ExportToFile ( dbpath );
	}
	
	clearZmarketValues ( client );
	g_iClientZombieClass[client] = 0;
	delete kv;
	
	/*
	//	Primary weapon data.
	kv.JumpToKey ( steam, true );
	kv.SetString ( "primary_weapon_name", g_strPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_NAME] );
	kv.Rewind ();
	
	kv.JumpToKey ( steam, true );
	kv.SetString ( "primary_weapon_alias", g_strPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS] );
	kv.Rewind ();
	
	kv.JumpToKey ( steam, true );
	kv.SetNum ( "primary_weapon_price", g_iPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_PRICE] );
	kv.Rewind ();
	
	kv.JumpToKey ( steam, true );
	kv.SetNum ( "primary_weapon_teamindex", g_iPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_TEAMIDX] );
	kv.Rewind ();
	
	//	Secondary weapon data.
	kv.JumpToKey ( steam, true );
	kv.SetString ( "secondary_weapon_name", g_strSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_NAME] );
	kv.Rewind ();
	
	kv.JumpToKey ( steam, true );
	kv.SetString ( "secondary_weapon_alias", g_strSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS] );
	kv.Rewind ();
	
	kv.JumpToKey ( steam, true );
	kv.SetNum ( "secondary_weapon_price", g_iSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_PRICE] );
	kv.Rewind ();
	
	kv.JumpToKey ( steam, true );
	kv.SetNum ( "secondary_weapon_teamindex", g_iSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_TEAMIDX] );
	kv.Rewind ();
	
	//	Auto buy toggle value.
	kv.JumpToKey ( steam, true );
	kv.SetNum ( "autobuy_toggle", g_iAutobuyMode[client] );
	kv.Rewind ();
	
	kv.ExportToFile ( dbpath );
	
	delete kv;	*/
	
	return;
}