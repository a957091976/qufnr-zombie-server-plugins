/**
 * Load players data.
 */
public void outputPlayerData ( int client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return;
	
	if ( IsFakeClient ( client ) )
		return;
		
	g_bUseTag[client] = false;
	g_strLaserBulletData[client][LASERBULLET_EQUIP_CHECK] = "0";
	g_strLaserBulletData[client][LASERBULLET_MODELPATH] = DEFAULT_LASERBULLET_MODEL;
	resetClientKeyData ( client );
	
	static char steam[32];
	static char steam_temp[32];
	GetClientAuthId ( client, AuthId_SteamID64, steam, sizeof ( steam ) );
	
	static char db[PLATFORM_MAX_PATH];
	BuildPath ( Path_SM, db, PLATFORM_MAX_PATH, "data/sourcestore/playerdb/%s.txt", steam );
	KeyValues kv = new KeyValues ( "data" );
	if ( !kv.ImportFromFile ( db ) ) {
		resetClientKeyData ( client );
		
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
			g_iBalance[client] = kv.GetNum ( "player_balance" );
			g_iTicket[TICKET_NORMAL][client] = kv.GetNum ( "player_normal_tickets" );
			g_iTicket[TICKET_PREMIUM][client] = kv.GetNum ( "player_premium_tickets" );
			g_iGasyaPoint[client] = kv.GetNum ( "player_gasya_point" );
			kv.GetString ( "player_character", g_strEquipItemName[client][ITEMTYPE_CHARACTER], 63 );
			kv.GetString ( "player_trail", g_strEquipItemName[client][ITEMTYPE_TRAIL], 63 );
			kv.GetString ( "player_laserbullet", g_strEquipItemName[client][ITEMTYPE_LASERBULLET], 63 );
			kv.GetString ( "player_tagtitle", g_strEquipItemName[client][ITEMTYPE_TAGTITLE], 63 );
			kv.GetString ( "player_aura", g_strEquipItemName[client][ITEMTYPE_AURA], 63 );
			
			g_bIsMvp[client] = loringlib_IntToBoolean ( kv.GetNum ( "player_mvp" ) );
			g_bIsMvpPlus[client] = loringlib_IntToBoolean ( kv.GetNum ( "player_mvp_plus" ) );
			
			if ( g_bIsMvpPlus[client] ) {
				g_bToggleRainbow[client] = loringlib_IntToBoolean ( kv.GetNum ( "player_toggle_rainbow_body" ) );
				g_fRainbowTickTimes[client] = kv.GetFloat ( "player_rainbow_body_tickspeed" );
			}
			
			//	Items
			static char itemKey[64];
			for ( int i = 0; i < g_iDefineStoreItem; i ++ ) {
				Format ( itemKey, sizeof ( itemKey ), "player_item_%d", i );
				g_iItem[client][i] = kv.GetNum ( itemKey );
			}
			
			//	Checking invalid equipment items.
			for ( int i = 1; i < MAXITEMTYPE; i ++ ) {
				if ( i == ITEMTYPE_TAGTITLE )
					continue;
				
				if ( !IsValidEquipItemName ( i, g_strEquipItemName[client][i] ) ) {
					PrintToChat ( client, "%s \x07%s\x01 아이템은 Source Store 테이터베이스에서 삭제된 아이템 입니다.", g_strConfChatPrefix, g_strEquipItemName[client][i] );
					LogMessage ( "Client %N`s %d equip items are removed. (Item Name: %s)", client, i, g_strEquipItemName[client][i] );
					strcopy ( g_strEquipItemName[client][i], sizeof ( g_strEquipItemName[][] ), "" );
				}
			}
			
			delete kv;
			return;
		}
	} while ( kv.GotoNextKey () );
	
	delete kv;
	
	/*
	//	Balance
	kv.JumpToKey ( steam, false );
	g_iBalance[client] = kv.GetNum ( "player_balance" );
	kv.Rewind ();
	
	//	Equip of character
	kv.JumpToKey ( steam, false );
	kv.GetString ( "player_character", g_strEquipItemName[client][ITEMTYPE_CHARACTER], 63 );
	kv.Rewind ();
	
	//	Equip of trail
	kv.JumpToKey ( steam, false );
	kv.GetString ( "player_trail", g_strEquipItemName[client][ITEMTYPE_TRAIL], 63 );
	kv.Rewind ();
	
	//	Equip of laser bullet
	kv.JumpToKey ( steam, false );
	kv.GetString ( "player_laserbullet", g_strEquipItemName[client][ITEMTYPE_LASERBULLET], 63 );
	kv.Rewind ();
	
	//	Equip of tag title
	kv.JumpToKey ( steam, false );
	kv.GetString ( "player_tagtitle", g_strEquipItemName[client][ITEMTYPE_TAGTITLE], 63 );
	kv.Rewind ();
	
	//	MVP
	kv.JumpToKey ( steam, false );
	g_bIsMvp[client] = loringlib_IntToBoolean ( kv.GetNum ( "player_mvp" ) );
	kv.Rewind ();
	
	//	MVP+
	kv.JumpToKey ( steam, false );
	g_bIsMvpPlus[client] = loringlib_IntToBoolean ( kv.GetNum ( "player_mvp_plus" ) );
	kv.Rewind ();
	
	//	Items
	static char itemKey[64];
	for ( int i = 0; i < g_iDefineStoreItem; i ++ ) {
		Format ( itemKey, sizeof ( itemKey ), "player_item_%d", i );
		kv.JumpToKey ( steam, false );
		g_iItem[client][i] = kv.GetNum ( itemKey );
		kv.Rewind ();
	}
	
	delete kv;	*/
}

/**
 * Save players data.
 */
public void inputPlayerData ( int client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return;
	
	g_bUseTag[client] = false;
	
	static char steam[32];
	GetClientAuthId ( client, AuthId_SteamID64, steam, sizeof ( steam ) );
	if ( IsFakeClient ( client ) )
		strcopy ( steam, sizeof ( steam ), "bot" );
	
	static char db[PLATFORM_MAX_PATH];
	BuildPath ( Path_SM, db, PLATFORM_MAX_PATH, "data/sourcestore/playerdb/%s.txt", steam );
	KeyValues kv = new KeyValues ( "data" );
//	if ( !kv.ImportFromFile ( db ) )
//		delete kv;
	
	if ( kv.JumpToKey ( steam, true ) ) {
		kv.SetNum ( "player_balance", g_iBalance[client] );
		kv.SetNum ( "player_normal_tickets", g_iTicket[TICKET_NORMAL][client] );
		kv.SetNum ( "player_premium_tickets", g_iTicket[TICKET_PREMIUM][client] );
		kv.SetNum ( "player_gasya_point", g_iGasyaPoint[client] );
		kv.SetString ( "player_character", g_strEquipItemName[client][ITEMTYPE_CHARACTER] );
		kv.SetString ( "player_trail", g_strEquipItemName[client][ITEMTYPE_TRAIL] );
		kv.SetString ( "player_laserbullet", g_strEquipItemName[client][ITEMTYPE_LASERBULLET] );
		kv.SetString ( "player_tagtitle", g_strEquipItemName[client][ITEMTYPE_TAGTITLE] );
		kv.SetString ( "player_aura", g_strEquipItemName[client][ITEMTYPE_AURA] );
		kv.SetNum ( "player_mvp", loringlib_BooleanToInt ( g_bIsMvp[client] ) );
		kv.SetNum ( "player_mvp_plus", loringlib_BooleanToInt ( g_bIsMvpPlus[client] ) );
		
		if ( g_bIsMvpPlus[client] ) {
			kv.SetNum ( "player_toggle_rainbow_body", loringlib_BooleanToInt ( g_bToggleRainbow[client] ) );
			kv.SetFloat ( "player_rainbow_body_tickspeed", g_fRainbowTickTimes[client] );
		}
		
		//	Items
		static char itemKey[64];
		for ( int i = 0; i < g_iDefineStoreItem; i ++ ) {
			Format ( itemKey, sizeof ( itemKey ), "player_item_%d", i );
			if ( g_iItem[client][i] > 0 )
				kv.SetNum ( itemKey, g_iItem[client][i] );
			else 
				kv.DeleteKey ( itemKey );
		}
	
		kv.Rewind ();
		kv.ExportToFile ( db );
	}
	
	resetClientKeyData ( client );
	delete kv;
	
	/*
	//	Balance
	kv.JumpToKey ( steam, true );
	kv.SetNum ( "player_balance", g_iBalance[client] );

	//	Equip of character
	kv.JumpToKey ( steam, true );
	kv.SetString ( "player_character", g_strEquipItemName[client][ITEMTYPE_CHARACTER] );

	//	Equip of trail
	kv.JumpToKey ( steam, true );
	kv.SetString ( "player_trail", g_strEquipItemName[client][ITEMTYPE_TRAIL] );

	//	Equip of laser bullet
	kv.JumpToKey ( steam, true );
	kv.SetString ( "player_laserbullet", g_strEquipItemName[client][ITEMTYPE_LASERBULLET] );

	//	Equip of tag title
	kv.JumpToKey ( steam, true );
	kv.SetString ( "player_tagtitle", g_strEquipItemName[client][ITEMTYPE_TAGTITLE] );

	//	MVP
	kv.JumpToKey ( steam, true );
	kv.SetNum ( "player_mvp", loringlib_BooleanToInt ( g_bIsMvp[client] ) );

	//	MVP+
	kv.JumpToKey ( steam, true );
	kv.SetNum ( "player_mvp_plus", loringlib_BooleanToInt ( g_bIsMvpPlus[client] ) );

	//	Items
	static char itemKey[64];
	for ( int i = 0; i < g_iDefineStoreItem; i ++ ) {
		Format ( itemKey, sizeof ( itemKey ), "player_item_%d", i );
		if ( g_iItem[client][i] > 0 ) {
			kv.JumpToKey ( steam, true );
			kv.SetNum ( itemKey, g_iItem[client][i] );
		}
		else {
			kv.JumpToKey ( steam, true );
			kv.DeleteKey ( itemKey );
		}
	}
	
	kv.Rewind ();
	kv.ExportToFile ( db );
	delete kv;
	
	resetClientKeyData ( client );
	*/
}

void resetClientKeyData ( int client ) {
	g_iBalance[client] = 0;
	g_iGasyaPoint[client] = 0;
	for ( int i = 0; i < sizeof ( g_iTicket ); i ++ )
		g_iTicket[i][client] = 0;
	strcopy ( g_strEquipItemName[client][ITEMTYPE_CHARACTER], 64, "" );
	strcopy ( g_strEquipItemName[client][ITEMTYPE_TRAIL], 64, "" );
	strcopy ( g_strEquipItemName[client][ITEMTYPE_LASERBULLET], 64, "" );
	strcopy ( g_strEquipItemName[client][ITEMTYPE_TAGTITLE], 64, "" );
	strcopy ( g_strEquipItemName[client][ITEMTYPE_AURA], 64, "" );
	
	g_bIsMvp[client] = false;
	g_bIsMvpPlus[client] = false;
	g_bToggleRainbow[client] = false;
	g_fRainbowTickTimes[client] = 1.0;
	
	for ( int i = 0; i < g_iDefineStoreItem; i ++ )
		g_iItem[client][i] = 0;
}