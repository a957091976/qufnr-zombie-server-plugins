#include "lol_ammofix/lol_ammofix.inc"

/**
 * 이 include 파일은 zmarket에 대해 다루지만, 무기 관련도 이곳에서 다룹니다.
 * 무기 관련 오류가 있다면 이곳에서 수정해주세요.
 */

/**
 * Remove the buy zone.
 */
public void removeBuyzone () {
	static char class[64];
	for ( int i = MaxClients; i <= GetMaxEntities (); i ++ ) {
		if ( !IsValidEdict ( i ) || !IsValidEntity ( i ) )
			continue;
			
		GetEdictClassname ( i, class, sizeof ( class ) );
		if ( StrEqual ( class, "func_buyzone" ) )
			RemoveEdict ( i );
	}
}

/**
 * Infinity weapon ammo
 */
public void infinityWeaponAmmo ( int morikubo ) {
	if ( g_iConfGameAmmoType != 2 )
		return;
		
	if ( !loringlib_IsValidClient__PlayGame ( morikubo ) )
		return;
		
	if ( g_bZombie[morikubo] )
		return;
		
	static char ssr[32];
	int nono = loringlib_GetActiveWeapon ( morikubo );
	if ( nono == -1 )
		return;
		
	GetClientWeapon ( morikubo, ssr, sizeof ( ssr ) );
	if ( StrContains ( ssr, "knife" ) != -1
		|| StrEqual ( ssr, "weapon_hegrenade" )
		|| StrEqual ( ssr, "weapon_smokegrenade" )
		|| StrEqual ( ssr, "weapon_flashbang" )
		|| StrEqual ( ssr, "weapon_tagrenade" )
		|| StrEqual ( ssr, "weapon_healthshot" )
		|| StrEqual ( ssr, "weapon_molotov" )
		|| StrEqual ( ssr, "weapon_incgrenade" )
		|| StrEqual ( ssr, "weapon_decoy" )
		|| StrEqual ( ssr, "weapon_c4" ) )
		return;
		
	loringlib_SetPlayerWeaponAmmoEx ( morikubo, nono, 827, 827 );
}

/**
 * Infinity weapon clips.
 */
public void infinityWeaponClips ( int client ) {
	if ( g_iConfGameAmmoType != 1 )
		return;
		
	int kawaiibokuto = loringlib_GetActiveWeapon ( client );
	if ( kawaiibokuto == -1 )
		return;
		
	static char weapon[32];
	GetClientWeapon ( client, weapon, sizeof ( weapon ) );
	
	if ( StrContains ( weapon, "knife" ) != -1
		|| StrEqual ( weapon, "weapon_hegrenade" )
		|| StrEqual ( weapon, "weapon_smokegrenade" )
		|| StrEqual ( weapon, "weapon_flashbang" )
		|| StrEqual ( weapon, "weapon_tagrenade" )
		|| StrEqual ( weapon, "weapon_healthshot" )
		|| StrEqual ( weapon, "weapon_molotov" )
		|| StrEqual ( weapon, "weapon_incgrande" )
		|| StrEqual ( weapon, "weapon_decoy" )
		|| StrEqual ( weapon, "weapon_c4" ) )
		return;
	loringlib_SetClip1 ( kawaiibokuto, 142 + 1 );
}

/**
 * All weapon remove.
 */
public void removeWeaponItems () {
	static char classname[32];
	for ( int i = MaxClients; i < GetMaxEntities (); i ++ ) {
		if ( !IsValidEdict ( i ) )
			continue;
			
		if ( !GetEdictClassname ( i, classname, sizeof ( classname ) ) )
			continue;
			
		if ( StrContains ( classname, "weapon_" ) == -1 )
			continue;
		
		AcceptEntityInput ( i, "kill" );
	}
}

/**
 * Game start to given items.
 */
public void giveGamestartItems () {
	for ( int client = 1; client <= MaxClients; client ++ ) if ( loringlib_IsValidClient__PlayGame ( client ) ) {
		loringlib_RemoveWeaponSlot ( client, CS_SLOT_GRENADE );
		CreateTimer ( 0.1, timerGiveGamestartItemsPost, client );
	}
}

public Action timerGiveGamestartItemsPost ( Handle timer, int client ) {
	if ( !loringlib_IsValidClient__PlayGame ( client ) )
		return Plugin_Stop;
	
	//	zmarket이 비활성화 되어있을 경우?
	if ( !g_iConfZmarketEnable ) {
		GivePlayerItem ( client, "weapon_hegrenade" );
		GivePlayerItem ( client, "weapon_smokegrenade" );
		GivePlayerItem ( client, "weapon_flashbang" );
	}
	GivePlayerItem ( client, "weapon_knife" );
	return Plugin_Stop;
}

/**
 * Save zmarket values.
 */
public int zmarketInputValues ( int client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return;
	
	if ( IsFakeClient ( client ) )
		return;
	
	//	zmarket이 비활성화 되어있을 경우?
	if ( !g_iConfZmarketEnable )
		return;
	
	if ( StrEqual ( g_strPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS], ZR_ZMARKET_DEFAULT_WEAPON_NAME ) &&
		StrEqual ( g_strSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS], ZR_ZMARKET_DEFAULT_WEAPON_NAME ) )
		return;
	
	static char dbpath[PLATFORM_MAX_PATH];
	static char steam[32];
	GetClientAuthId ( client, AuthId_SteamID64, steam, sizeof ( steam ) );
	BuildPath ( Path_SM, dbpath, PLATFORM_MAX_PATH, "data/zombierelolied/zmarket/loadout_db/%s.txt", steam );
	
	KeyValues kv = new KeyValues ( "zmarket_client_db" );
/*	if ( !kv.ImportFromFile ( dbpath ) ) {
		delete kv;
		return;
	}	*/
	
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
	
	delete kv;
	
	clearZmarketValues ( client );
	
	return;
}

public void clearZmarketValues ( int client ) {
	strcopy ( g_strPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_NAME], ZR_ZMARKET_WEAPON_DATA_LENGTH, "" );
	strcopy ( g_strPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS], ZR_ZMARKET_WEAPON_DATA_LENGTH, "" );
	
	g_iPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_PRICE] = 0;
	g_iPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_TEAMIDX] = 0;
	
	strcopy ( g_strSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_NAME], ZR_ZMARKET_WEAPON_DATA_LENGTH, "" );
	strcopy ( g_strSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS], ZR_ZMARKET_WEAPON_DATA_LENGTH, "" );
	
	g_iSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_PRICE] = 0;
	g_iSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_TEAMIDX] = 0;
	g_iAutobuyMode[client] = 0;
}

/**
 * Load zmarket values.
 */
public void zmarketOutputValues ( int client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return;
	
	if ( StrEqual ( g_strPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS], "" ) )
		g_strPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS] = ZR_ZMARKET_DEFAULT_WEAPON_NAME;
	if ( StrEqual ( g_strSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS], "" ) )
		g_strSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS] = ZR_ZMARKET_DEFAULT_WEAPON_NAME;
		
	if ( IsFakeClient ( client ) )
		return;
	
	clearZmarketValues ( client );
	
	//	zmarket이 비활성화 되어있을 경우?
	if ( !g_iConfZmarketEnable )
		return;
	
	static char dbpath[PLATFORM_MAX_PATH];
	static char steam[32];
	GetClientAuthId ( client, AuthId_SteamID64, steam, sizeof ( steam ) );
	BuildPath ( Path_SM, dbpath, PLATFORM_MAX_PATH, "data/zombierelolied/zmarket/loadout_db/%s.txt", steam );
	
	KeyValues kv = new KeyValues ( "zmarket_client_db" );
	if ( !kv.ImportFromFile ( dbpath ) ) {
		clearZmarketValues ( client );
		delete kv;
		return;
	}
	
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
}

/**
 * Load weapon datas.
 */
public void onLoadWeaponDB () {
	//	zmarket이 비활성화 되어있을 경우?
	if ( !g_iConfZmarketEnable )
		return;
	
	hg ();	sg ();	smg ();
	ar ();	sr ();	mg ();
}

/**
 * 데이터베이스를 불러올 파일을 구합니다.
 *
 * @param type			Weapon database name.
 * @return				불러오는데 성공하면 true, 아니면 false를 반환합니다.
 */
stock bool getWeaponDBPath ( char[] type ) {
	static char buff[128];
	if ( GetEngineVersion () == Engine_CSGO ) {
		Format ( buff, sizeof ( buff ), "data/zombierelolied/zmarket/weapon/csgo/%s.txt", type );
		BuildPath ( Path_SM, g_strWeaponDatabase, PLATFORM_MAX_PATH, buff );
		
		return true;
	}
	
	else if ( GetEngineVersion () == Engine_CSS ) {
		Format ( buff, sizeof ( buff ), "data/zombierelolied/zmarket/weapon/cstrike/%s.txt", type );
		BuildPath ( Path_SM, g_strWeaponDatabase, PLATFORM_MAX_PATH, buff );
		
		return true;
	}
	
	else {
		LogMessage ( "This engine version is unsupported." );
		
		return false;
	}
}

/**
 * Loading HGs.
 */
public void hg () {
	g_iTotalWeapons[ZR_WEAPON_HG] = 0;
	
	getWeaponDBPath ( "hg" );
	KeyValues kv = new KeyValues ( "hg" );
	if ( kv.ImportFromFile ( g_strWeaponDatabase ) ) {
		kv.GotoFirstSubKey ();
		do {
			kv.GetSectionName ( g_strWeaponData[ZR_WEAPON_HG][g_iTotalWeapons[ZR_WEAPON_HG]][ZR_DATA_WEAPON_ALIAS], 63 );
			kv.GetString ( "weapon_name", g_strWeaponData[ZR_WEAPON_HG][g_iTotalWeapons[ZR_WEAPON_HG]][ZR_DATA_WEAPON_NAME], 63 );
			kv.GetString ( "weapon_price", g_strWeaponData[ZR_WEAPON_HG][g_iTotalWeapons[ZR_WEAPON_HG]][ZR_DATA_WEAPON_PRICE], 63 );
			kv.GetString ( "weapon_team", g_strWeaponData[ZR_WEAPON_HG][g_iTotalWeapons[ZR_WEAPON_HG]][ZR_DATA_WEAPON_TEAM], 63 );

			g_iTotalWeapons[ZR_WEAPON_HG] ++;
		}
		
		while ( kv.GotoNextKey () );
		
		delete kv;
	}
	
	else
		delete kv;
}

/**
 * Loading SGs.
 */
public void sg () {
	g_iTotalWeapons[ZR_WEAPON_SG] = 0;
	
	getWeaponDBPath ( "sg" );
	KeyValues kv = new KeyValues ( "sg" );
	if ( kv.ImportFromFile ( g_strWeaponDatabase ) ) {
		kv.GotoFirstSubKey ();
		do {
			kv.GetSectionName ( g_strWeaponData[ZR_WEAPON_SG][g_iTotalWeapons[ZR_WEAPON_SG]][ZR_DATA_WEAPON_ALIAS], 63 );
			kv.GetString ( "weapon_name", g_strWeaponData[ZR_WEAPON_SG][g_iTotalWeapons[ZR_WEAPON_SG]][ZR_DATA_WEAPON_NAME], 63 );
			kv.GetString ( "weapon_price", g_strWeaponData[ZR_WEAPON_SG][g_iTotalWeapons[ZR_WEAPON_SG]][ZR_DATA_WEAPON_PRICE], 63 );
			kv.GetString ( "weapon_team", g_strWeaponData[ZR_WEAPON_SG][g_iTotalWeapons[ZR_WEAPON_SG]][ZR_DATA_WEAPON_TEAM], 63 );

			g_iTotalWeapons[ZR_WEAPON_SG] ++;
		}
		
		while ( kv.GotoNextKey () );
		
		delete kv;
	}
	
	else
		delete kv;
}

/**
 * Loading SMGs.
 */
public void smg () {
	g_iTotalWeapons[ZR_WEAPON_SMG] = 0;
	
	getWeaponDBPath ( "smg" );
	KeyValues kv = new KeyValues ( "smg" );
	if ( kv.ImportFromFile ( g_strWeaponDatabase ) ) {
		kv.GotoFirstSubKey ();
		do {
			kv.GetSectionName ( g_strWeaponData[ZR_WEAPON_SMG][g_iTotalWeapons[ZR_WEAPON_SMG]][ZR_DATA_WEAPON_ALIAS], 63 );
			kv.GetString ( "weapon_name", g_strWeaponData[ZR_WEAPON_SMG][g_iTotalWeapons[ZR_WEAPON_SMG]][ZR_DATA_WEAPON_NAME], 63 );
			kv.GetString ( "weapon_price", g_strWeaponData[ZR_WEAPON_SMG][g_iTotalWeapons[ZR_WEAPON_SMG]][ZR_DATA_WEAPON_PRICE], 63 );
			kv.GetString ( "weapon_team", g_strWeaponData[ZR_WEAPON_SMG][g_iTotalWeapons[ZR_WEAPON_SMG]][ZR_DATA_WEAPON_TEAM], 63 );

			g_iTotalWeapons[ZR_WEAPON_SMG] ++;
		}
		
		while ( kv.GotoNextKey () );
		
		delete kv;
	}
	
	else
		delete kv;
}

/**
 * Loading Assault rifles.
 */
public void ar () {
	g_iTotalWeapons[ZR_WEAPON_AR] = 0;
	
	getWeaponDBPath ( "ar" );
	KeyValues kv = new KeyValues ( "ar" );
	if ( kv.ImportFromFile ( g_strWeaponDatabase ) ) {
		kv.GotoFirstSubKey ();
		do {
			kv.GetSectionName ( g_strWeaponData[ZR_WEAPON_AR][g_iTotalWeapons[ZR_WEAPON_AR]][ZR_DATA_WEAPON_ALIAS], 63 );
			kv.GetString ( "weapon_name", g_strWeaponData[ZR_WEAPON_AR][g_iTotalWeapons[ZR_WEAPON_AR]][ZR_DATA_WEAPON_NAME], 63 );
			kv.GetString ( "weapon_price", g_strWeaponData[ZR_WEAPON_AR][g_iTotalWeapons[ZR_WEAPON_AR]][ZR_DATA_WEAPON_PRICE], 63 );
			kv.GetString ( "weapon_team", g_strWeaponData[ZR_WEAPON_AR][g_iTotalWeapons[ZR_WEAPON_AR]][ZR_DATA_WEAPON_TEAM], 63 );

			g_iTotalWeapons[ZR_WEAPON_AR] ++;
		}
		
		while ( kv.GotoNextKey () );
		
		delete kv;
	}
	
	else
		delete kv;
}

/**
 * Loading Sniper rifles.
 */
public void sr () {
	g_iTotalWeapons[ZR_WEAPON_SR] = 0;
	
	getWeaponDBPath ( "sr" );
	KeyValues kv = new KeyValues ( "sr" );
	if ( kv.ImportFromFile ( g_strWeaponDatabase ) ) {
		kv.GotoFirstSubKey ();
		do {
			kv.GetSectionName ( g_strWeaponData[ZR_WEAPON_SR][g_iTotalWeapons[ZR_WEAPON_SR]][ZR_DATA_WEAPON_ALIAS], 63 );
			kv.GetString ( "weapon_name", g_strWeaponData[ZR_WEAPON_SR][g_iTotalWeapons[ZR_WEAPON_SR]][ZR_DATA_WEAPON_NAME], 63 );
			kv.GetString ( "weapon_price", g_strWeaponData[ZR_WEAPON_SR][g_iTotalWeapons[ZR_WEAPON_SR]][ZR_DATA_WEAPON_PRICE], 63 );
			kv.GetString ( "weapon_team", g_strWeaponData[ZR_WEAPON_SR][g_iTotalWeapons[ZR_WEAPON_SR]][ZR_DATA_WEAPON_TEAM], 63 );

			g_iTotalWeapons[ZR_WEAPON_SR] ++;
		}
		
		while ( kv.GotoNextKey () );
		
		delete kv;
	}
	
	else
		delete kv;
}

/**
 * Loading Machine guns.
 */
public void mg () {
	g_iTotalWeapons[ZR_WEAPON_MG] = 0;
	
	getWeaponDBPath ( "mg" );
	KeyValues kv = new KeyValues ( "mg" );
	if ( kv.ImportFromFile ( g_strWeaponDatabase ) ) {
		kv.GotoFirstSubKey ();
		do {
			kv.GetSectionName ( g_strWeaponData[ZR_WEAPON_MG][g_iTotalWeapons[ZR_WEAPON_MG]][ZR_DATA_WEAPON_ALIAS], 63 );
			kv.GetString ( "weapon_name", g_strWeaponData[ZR_WEAPON_MG][g_iTotalWeapons[ZR_WEAPON_MG]][ZR_DATA_WEAPON_NAME], 63 );
			kv.GetString ( "weapon_price", g_strWeaponData[ZR_WEAPON_MG][g_iTotalWeapons[ZR_WEAPON_MG]][ZR_DATA_WEAPON_PRICE], 63 );
			kv.GetString ( "weapon_team", g_strWeaponData[ZR_WEAPON_MG][g_iTotalWeapons[ZR_WEAPON_MG]][ZR_DATA_WEAPON_TEAM], 63 );

			g_iTotalWeapons[ZR_WEAPON_MG] ++;
		}
		
		while ( kv.GotoNextKey () );
		
		delete kv;
	}
	
	else
		delete kv;
}

/**
 * Reset zmarket buy menu variables.
 */
public void zmarketResetClientValues ( int client ) {
	for ( int i = 0; i < 2; i ++ )
		g_bBuyWeapon[client][i] = false;
	g_bSaveMode[client] = false;
}

/**
 * Auto buy load-outs
 */
public void zmarketAutobuyWeapons ( int client ) {
	CreateTimer ( 0.0, timerAutobuyWeaponPost, client );
}

public Action timerAutobuyWeaponPost ( Handle timer, int client ) {
	if ( !loringlib_IsValidClient__PlayGame ( client ) )
		return Plugin_Stop;
	
	loringlib_SetEntityAccount ( client, g_iConfGameStartMoney );	//	Set game start money value from config.
	
	//	zmarket이 비활성화 되어있을 경우?
	if ( !g_iConfZmarketEnable )
		return Plugin_Stop;
	
	//	자동 구매 모드가 꺼져있으면 메뉴를 엷니다.
	if ( g_iAutobuyMode[client] != 1 ) {
		PrintToChat ( client, "[ZR] '/zmarket' 명령으로 무기를 구입하세요." );
		onClientCommandZmarket ( client, 0 );
	}
	
	
	//	주 무기 또는 보조 무기 중 하나를 구입했다면 Plugin_Stop을 반환합니다.
	if ( g_bBuyWeapon[client][ZR_ZMARKET_PRIMARY_WEAPON] ||
		g_bBuyWeapon[client][ZR_ZMARKET_SECONDARY_WEAPON] )
		return Plugin_Stop;
	
	//	로드아웃이 모두 비어있다면 Plugin_Stop을 반환합니다.
	if ( StrEqual ( g_strPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS], ZR_ZMARKET_DEFAULT_WEAPON_NAME ) &&
		StrEqual ( g_strSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS], ZR_ZMARKET_DEFAULT_WEAPON_NAME ) )
		return Plugin_Stop;
		
	//	자동 구입 모드가 켜져있고, 게임이 진행중이지 않을 경우
	if ( g_iAutobuyMode[client] == 1 &&
		( !g_bGameStart && !g_bGameEnd ) )
		//	수동 로드아웃 구입과 같이 로드아웃을 구입합니다.
		zmarketSelfBuyLoadouts ( client );
	
	/*	
	//	저장 된 주 무기 또는 보조 무기가 비어있다면 Plugin_Stop을 반환합니다.
	if ( StrEqual ( g_strPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS], ZR_ZMARKET_DEFAULT_WEAPON_NAME ) ||
		StrEqual ( g_strSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS], ZR_ZMARKET_DEFAULT_WEAPON_NAME ) )
		return Plugin_Stop;
	
	//	자동 구입 모드가 켜져있고, 게임이 진행중이지 않을 경우
	if ( g_iAutobuyMode[client] == 1 && 
		( !g_bGameStart && !g_bGameEnd ) ) {
		int totalPrice = g_iPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_PRICE] + g_iSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_PRICE];
		
		//	달러를 충분히 소지하고 있다면, 로드아웃을 구입합니다.
		if ( loringlib_GetEntityAccount ( client ) >= totalPrice ) {
			int team = GetClientTeam ( client );
			loringlib_SubEntityAccount ( client, totalPrice );	//	로드아웃 가격만큼 뺄셈
			
			PrintToChat ( client, "[ZR] ZMarket에서 설정한 무기를 자동으로 구입했습니다. (총 가격: $%d)", totalPrice );
			
			//	구입 처리
			g_bBuyWeapon[client][ZR_ZMARKET_PRIMARY_WEAPON] = true;
			g_bBuyWeapon[client][ZR_ZMARKET_SECONDARY_WEAPON] = true;
			
			//	새로 구입한 무기를 땅에 떨어뜨리지 않게 미리 주 무기와 보조 무기를 업앱니다.
			loringlib_RemoveWeaponSlot ( client, CS_SLOT_PRIMARY );
			loringlib_RemoveWeaponSlot ( client, CS_SLOT_SECONDARY );
			
			//	클라이언트의 무기 도색이 사라지지 않게 클라이언트의 팀 인덱스 값을
			//	무기의 고유 팀 인덱스 값으로 바꾼상태로 무기를 생성합니다.
			loringlib_SetEntityTeamNumber ( client, g_iPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_TEAMIDX] );
			GivePlayerItem ( client, g_strPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_NAME] );
			loringlib_SetEntityTeamNumber ( client, g_iSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_TEAMIDX] );
			GivePlayerItem ( client, g_strSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_NAME] );
			
			//	생성 후 다시 클라이언트는 원 상태 팀으로 돌립니다.
			loringlib_SetEntityTeamNumber ( client, team );
		}
		
		//	자금 부족일 경우
		else
			PrintToChat ( client, "[ZR] 자금이 부족합니다." );
	}
	*/
	
	return Plugin_Stop;
}

/**
 * Open the zmarket menu.
 */
public Action onClientCommandZmarket ( int client, int args ) {
	//	zmarket이 비활성화 되어있을 경우?
	if ( !g_iConfZmarketEnable ) {
		PrintToChat ( client, "[ZR] 관리자에 의해 zmarket이 비활성화 되어 있습니다." );
		return;
	}

	Menu menu = new Menu ( zmarketMainMenuHandler );
	menu.SetTitle ( "ZMarket\n　" );
	if ( !IsPlayerAlive ( client ) ) {
		menu.AddItem ( "", "-1", ITEMDRAW_DISABLED );
		menu.Display ( client, 10 );
		
		return;
	}
	
	menu.AddItem ( "", "로드아웃 및 설정" );
	menu.AddItem ( "", "무기 구입" );
	if ( g_iConfZMarketAmmoBuyEnable == 1 )
		if ( g_iConfGameAmmoType == 0 )
			if ( !g_bZombie[client] )
				menu.AddItem ( "", "탄창 구입" );
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int zmarketMainMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	if ( !loringlib_IsValidClient__PlayGame ( param1 ) )
		return;
	
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			switch ( param2 ) {
				case 0: zmarketViewMyLoadouts ( param1 );
				case 1: zmarketBuyWeapons ( param1 );
				case 2: zmarketBuyAmmo ( param1 );
			//	case 2: zmarketSetMyCookies ( param1 );
			}
		}
	}
}

/**
 * Buy ammo
 */
public void zmarketBuyAmmo ( int client ) {
	Menu menu = new Menu ( zmarketBuyAmmoHandler );
	menu.SetTitle ( "ZMarket\n\t탄창 구입\n　\n\t\t들고 있는 무기의 탄창을 구입\n　" );
	static char size[3][32] = { "[Small]", "[Medium]", "[Large]" }
	static char info[256];
	static char price[16];
	if ( g_bZombie[client] ) {
		menu.AddItem ( "", "?", ITEMDRAW_DISABLED );
		menu.ExitBackButton = true;
		menu.Display ( client, MENU_TIME_FOREVER );
		return;
	}
	for ( int i = 0; i < 3; i ++ ) {
		IntToString ( g_iConfZmarketAmmoPrice[i], price, sizeof ( price ) );
		Format ( info, sizeof ( info ), "Ammo %s [$%s]", size[i] );
		menu.AddItem ( price, info, loringlib_GetEntityAccount ( client ) < g_iConfZmarketAmmoPrice[i] ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT );
	}
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int zmarketBuyAmmoHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			if ( g_bZombie[param1] )
				return;
			
			static char item[16];
			menu.GetItem ( param2, item, sizeof ( item ) );
			
			if ( loringlib_HasWeaponGrenade ( param1 ) || loringlib_HasWeaponName ( param1, "weapon_knife" ) ) {
				PrintToChat ( param1, "[ZR] 주 무기 및 보조 무기만 사용할 수 있습니다." );
				return;
			}
		
			int ammo = loringlib_GetWeaponAmmo ( param1 );
			if ( ammo < 0 ) {
				PrintToChat ( param1, "[ZR] ERROR: INVALID WEAPON AMMO AMOUNT INDEX: %d", ammo );
				return;
			}
			
			int price = StringToInt ( item );
			if ( loringlib_GetEntityAccount ( param1 ) < price || price < 0 ) {
				zmarketBuyAmmo ( param1 );
				return;
			}
			
			int charge_ammo = LOLAmmoFix_GetWeaponMaxAmmo ( param1 ) / g_iConfZmarketAmmoAmount[param2];
			loringlib_SubEntityAccount ( param1, price );
			
			PrintToChat ( param1, "[ZR] 탄창을 구입했습니다. \x05탄창 +%d", charge_ammo );
			loringlib_SetWeaponAmmo ( param1, ammo + charge_ammo );
			return;
		}
		case MenuAction_Cancel: {
			if ( param2 == MenuCancel_ExitBack )
				onClientCommandZmarket ( param1, 0 );
		}
	}
}

/**
 * View my load-outs
 */
public void zmarketViewMyLoadouts ( int client ) {
	Menu menu = new Menu ( zmarketViewMyLoadoutHandler );
	menu.SetTitle ( "ZMarket\n\t로드아웃 보기\n　" );
	static char lore[5][128];
	Format ( lore[0], 127, "주 무기: %s", g_strPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS] );
	Format ( lore[1], 127, "보조 무기: %s", g_strSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS] );
	Format ( lore[2], 127, "로드아웃 구입\n\t\t로드아웃 가격 합: $%d\n　", g_iPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_PRICE] + g_iSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_PRICE] );
	Format ( lore[3], 127, "자동 구입: %s", g_iAutobuyMode[client] == 1 ? "활성화" : "비활성화" );
	Format ( lore[4], 127, "저장 모드: %s", g_bSaveMode[client] ? "활성화" : "비활성화" );
	
	menu.AddItem ( "0", lore[0], StrEqual ( g_strPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS], ZR_ZMARKET_DEFAULT_WEAPON_NAME ) ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT );
	menu.AddItem ( "1", lore[1], StrEqual ( g_strSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS], ZR_ZMARKET_DEFAULT_WEAPON_NAME ) ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT );
	menu.AddItem ( "2", "투척 무기: [ANY]", ITEMDRAW_DISABLED );
	if ( IsPlayerAlive ( client ) )
		menu.AddItem ( "3", lore[2], g_bBuyWeapon[client][ZR_ZMARKET_PRIMARY_WEAPON] || g_bBuyWeapon[client][ZR_ZMARKET_SECONDARY_WEAPON] ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT );
	else
		menu.AddItem ( "3", lore[2], ITEMDRAW_DISABLED );
	menu.AddItem ( "4", lore[3] );
	menu.AddItem ( "5", lore[4] );
	
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int zmarketViewMyLoadoutHandler ( Menu menu, MenuAction action, int param1, int param2 ) {	
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			switch ( param2 ) {
				case 0: {
					//	if weapon name is [EMPTY]?
					if ( StrEqual ( g_strPrimaryWeaponData[param1][ZR_ZMARKET_WEAPON_ALIAS], ZR_ZMARKET_DEFAULT_WEAPON_NAME ) )
						return;
					
					zmarketClearLoadouts ( param1 );
				}
				case 1: {
					if ( StrEqual ( g_strSecondaryWeaponData[param1][ZR_ZMARKET_WEAPON_ALIAS], ZR_ZMARKET_DEFAULT_WEAPON_NAME ) )
						return;
					
					zmarketClearLoadouts ( param1, true );
				}
				case 2: return;
				case 3:	zmarketSelfBuyLoadouts ( param1 );
				//	Toggle auto buy mode
				case 4: {
					g_iAutobuyMode[param1] = !g_iAutobuyMode[param1];
					zmarketViewMyLoadouts ( param1 );
				}
				//	Toggle save mode
				case 5: {
					g_bSaveMode[param1] = !g_bSaveMode[param1];
					if ( g_bSaveMode[param1] )
						zmarketBuyWeapons ( param1 );
					else
						zmarketViewMyLoadouts ( param1 );
				}
			}
		}
		
		case MenuAction_Cancel: {
			switch ( param2 ) {
				case MenuCancel_ExitBack: {
					onClientCommandZmarket ( param1, 0 );
					return;
				}
				
				case MenuCancel_Exit: {
					delete menu;
					return;
				}
			}
		}
	}
}

/**
 * Self buy load-outs
 * @note 저장된 로드아웃이 없을 경우 구입하지 않습니다, 두 무기 중 하나만 지정했다면 지정한 무기만 구입됩니다.
 */
public void zmarketSelfBuyLoadouts ( int client ) {
	if ( !IsPlayerAlive ( client ) )
		return;
		
	if ( g_bZombie[client] )
		return;
		
	//	저장된 무기가 없을 경우
	if ( StrEqual ( g_strPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS], ZR_ZMARKET_DEFAULT_WEAPON_NAME ) &&
		StrEqual ( g_strSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS], ZR_ZMARKET_DEFAULT_WEAPON_NAME ) ) {
		PrintToChat ( client, "[ZR] 저장된 로드아웃이 없습니다." );
		zmarketViewMyLoadouts ( client );
		return;
	}
	
	//	무기를 이미 샀을 경우
	if ( g_bBuyWeapon[client][ZR_ZMARKET_PRIMARY_WEAPON] ||
		g_bBuyWeapon[client][ZR_ZMARKET_SECONDARY_WEAPON] ) {
		PrintToChat ( client, "[ZR] 귀하는 이미 무기를 구입했습니다." );
		return;
	}
	
	int price;
	int team = GetClientTeam ( client );
	//	주 무기를 사지 않았을 경우
	if ( !g_bBuyWeapon[client][ZR_ZMARKET_PRIMARY_WEAPON] ) {
		//	저장된 주 무기 로드아웃이 있을 경우
		if ( !StrEqual ( g_strPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS], ZR_ZMARKET_DEFAULT_WEAPON_NAME ) ) {
			price = g_iPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_PRICE];
			//	자금이 충분할 경우
			if ( loringlib_GetEntityAccount ( client ) >= price ) {
				loringlib_SubEntityAccount ( client, price );
				g_bBuyWeapon[client][ZR_ZMARKET_PRIMARY_WEAPON] = true;	//	구입 채크
				
				loringlib_RemoveWeaponSlot ( client, CS_SLOT_PRIMARY );	//	들고 있는 주 무기가 있을 경우 삭제 합니다.
				
				loringlib_SetEntityTeamNumber ( client, g_iPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_TEAMIDX] );
				GivePlayerItem ( client, g_strPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_NAME] );
			} else {
				PrintToChat ( client, "[ZR] \x05%s \x01무기를 구입할 자금이 충분하지 않습니다.", g_strPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS] );
			}
		}
	}
	
	//	보조 무기를 사지 않았을 경우
	if ( !g_bBuyWeapon[client][ZR_ZMARKET_SECONDARY_WEAPON] ) {
		//	저장된 보조 무기 로드아웃이 있을 경우
		if ( !StrEqual ( g_strSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS], ZR_ZMARKET_DEFAULT_WEAPON_NAME ) ) {
			price = g_iSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_PRICE];
			//	자금이 충분할 경우
			if ( loringlib_GetEntityAccount ( client ) >= price ) {
				loringlib_SubEntityAccount ( client, price );
				g_bBuyWeapon[client][ZR_ZMARKET_SECONDARY_WEAPON] = true;
				
				loringlib_RemoveWeaponSlot ( client, CS_SLOT_SECONDARY );
				
				loringlib_SetEntityTeamNumber ( client, g_iSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_TEAMIDX] );
				GivePlayerItem ( client, g_strSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_NAME] );
			} else {
				PrintToChat ( client, "[ZR] \x05%s \x01무기를 구입할 자금이 충분하지 않습니다.", g_strSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS] );
			}
		}
	}
	
	loringlib_SetEntityTeamNumber ( client, team );
}

/**
 * Buy weapons.
 */
public void zmarketBuyWeapons ( int client ) {
	Menu menu = new Menu ( zmarketBuyWeaponHandler );
	menu.SetTitle ( "ZMarket\n\t무기 구입%s", g_bSaveMode[client] ? "\n\t[저장 모드]\n　" : "\n　" );
	if ( !IsPlayerAlive ( client ) ) {
		menu.AddItem ( "", "-1", ITEMDRAW_DISABLED );
		menu.Display ( client, 10 );
		return;
	}
	
	if ( g_bSaveMode[client] ) {
		menu.AddItem ( "0", "Pistols" );
		menu.AddItem ( "1", "Shotguns" );
		menu.AddItem ( "2", "SMGs" );
		menu.AddItem ( "3", "Assault Rifles" );
		menu.AddItem ( "4", "Sniper Rifles" );
		menu.AddItem ( "5", "Machine Guns" );
	} else {
		menu.AddItem ( "0", "Pistols", g_bBuyWeapon[client][ZR_ZMARKET_SECONDARY_WEAPON] ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT );
		menu.AddItem ( "1", "Shotguns", g_bBuyWeapon[client][ZR_ZMARKET_PRIMARY_WEAPON] ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT );
		menu.AddItem ( "2", "SMGs", g_bBuyWeapon[client][ZR_ZMARKET_PRIMARY_WEAPON] ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT );
		menu.AddItem ( "3", "Assault Rifles", g_bBuyWeapon[client][ZR_ZMARKET_PRIMARY_WEAPON] ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT );
		menu.AddItem ( "4", "Sniper Rifles", g_bBuyWeapon[client][ZR_ZMARKET_PRIMARY_WEAPON] ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT );
		menu.AddItem ( "5", "Machine Guns", g_bBuyWeapon[client][ZR_ZMARKET_PRIMARY_WEAPON] ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT );
	}
	menu.ExitBackButton = true;
	
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int zmarketBuyWeaponHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	if ( !loringlib_IsValidClient__PlayGame ( param1 ) ||
		g_bZombie[param1] )
		return;
	
	switch ( action ) {
		case MenuAction_Select: {
			switch ( param2 ) {
				case 0:	zmarketWeaponMenu ( param1, "Pistols", ZR_WEAPON_HG );
				case 1: zmarketWeaponMenu ( param1, "Shotguns", ZR_WEAPON_SG );
				case 2: zmarketWeaponMenu ( param1, "SMGs", ZR_WEAPON_SMG );
				case 3: zmarketWeaponMenu ( param1, "Assault Rifles", ZR_WEAPON_AR );
				case 4: zmarketWeaponMenu ( param1, "Sniper Rifles", ZR_WEAPON_SR );
				case 5: zmarketWeaponMenu ( param1, "Machine Guns", ZR_WEAPON_MG );
			}
		}
		
		case MenuAction_Cancel: {
			switch ( param2 ) {
				case MenuCancel_ExitBack: {
					onClientCommandZmarket ( param1, 0 );
					return;
				}
				
				case MenuCancel_Exit: {
					delete menu;
					return;
				}
			}
		}
	}
}

/**
 * Primary weapon menu.
 */
public void zmarketWeaponMenu ( int client, char[] title, int weapon ) {
	Menu menu = new Menu ( zmarketPrimaryWeaponMenuHandler );
	static char item[64];
	static char weaponId[4];
	int price;
	int acc = loringlib_GetEntityAccount ( client );
	if ( g_bSaveMode[client] )
		acc = g_iConfGameMaxMoney;
	IntToString ( weapon, weaponId, sizeof ( weaponId ) );	//	Change int to string.
	
	menu.SetTitle ( "ZMarket\n%s", title );
	//	무기 값에 따라 아이템 정렬 합니다.
	switch ( weapon ) {
		case ZR_WEAPON_HG: {	//	권총
			for ( int i = 0; i < g_iTotalWeapons[ZR_WEAPON_HG]; i ++ ) {
				price = StringToInt ( g_strWeaponData[ZR_WEAPON_HG][i][ZR_DATA_WEAPON_PRICE] );
				Format ( item, sizeof ( item ), "%s [$%d]", g_strWeaponData[ZR_WEAPON_HG][i][ZR_DATA_WEAPON_ALIAS], price );
				menu.AddItem ( weaponId, item, acc >= price ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED );
			//	PrintToChat ( client, item );
			}
			
			menu.ExitBackButton = true;
			menu.Display ( client, MENU_TIME_FOREVER );
			
			return;
		}
	
		case ZR_WEAPON_SG: {		//	샷건
			for ( int i = 0; i < g_iTotalWeapons[ZR_WEAPON_SG]; i ++ ) {
				price = StringToInt ( g_strWeaponData[ZR_WEAPON_SG][i][ZR_DATA_WEAPON_PRICE] );
				Format ( item, sizeof ( item ), "%s [$%d]", g_strWeaponData[ZR_WEAPON_SG][i][ZR_DATA_WEAPON_ALIAS], price );
				menu.AddItem ( weaponId, item, acc >= price ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED );
			//	PrintToChat ( client, item );
			}
			
			menu.ExitBackButton = true;
			menu.Display ( client, MENU_TIME_FOREVER );
			
			return;
		}
		
		case ZR_WEAPON_SMG: {	//	기관단총
			for ( int i = 0; i < g_iTotalWeapons[ZR_WEAPON_SMG]; i ++ ) {
				price = StringToInt ( g_strWeaponData[ZR_WEAPON_SMG][i][ZR_DATA_WEAPON_PRICE] );
				Format ( item, sizeof ( item ), "%s [$%d]", g_strWeaponData[ZR_WEAPON_SMG][i][ZR_DATA_WEAPON_ALIAS], price );
				menu.AddItem ( weaponId, item, acc >= price ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED );
			//	PrintToChat ( client, item );
			}
			
			menu.ExitBackButton = true;
			menu.Display ( client, MENU_TIME_FOREVER );
			
			return;
		}
		
		case ZR_WEAPON_AR: {	//	돌격소총
			for ( int i = 0; i < g_iTotalWeapons[ZR_WEAPON_AR]; i ++ ) {
				price = StringToInt ( g_strWeaponData[ZR_WEAPON_AR][i][ZR_DATA_WEAPON_PRICE] );
				Format ( item, sizeof ( item ), "%s [$%d]", g_strWeaponData[ZR_WEAPON_AR][i][ZR_DATA_WEAPON_ALIAS], price );
				menu.AddItem ( weaponId, item, acc >= price ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED );
			//	PrintToChat ( client, item );
			}
			
			menu.ExitBackButton = true;
			menu.Display ( client, MENU_TIME_FOREVER );
			
			return;
		}
		
		case ZR_WEAPON_SR: {	//	저격소총
			for ( int i = 0; i < g_iTotalWeapons[ZR_WEAPON_SR]; i ++ ) {
				price = StringToInt ( g_strWeaponData[ZR_WEAPON_SR][i][ZR_DATA_WEAPON_PRICE] );
				Format ( item, sizeof ( item ), "%s [$%d]", g_strWeaponData[ZR_WEAPON_SR][i][ZR_DATA_WEAPON_ALIAS], price );
				menu.AddItem ( weaponId, item, acc >= price ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED );
			//	PrintToChat ( client, item );
			}
			
			menu.ExitBackButton = true;
			menu.Display ( client, MENU_TIME_FOREVER );
			
			return;
		}
		
		case ZR_WEAPON_MG: {	//	기관총
			for ( int i = 0; i < g_iTotalWeapons[ZR_WEAPON_MG]; i ++ ) {
				price = StringToInt ( g_strWeaponData[ZR_WEAPON_MG][i][ZR_DATA_WEAPON_PRICE] );
				Format ( item, sizeof ( item ), "%s [$%d]", g_strWeaponData[ZR_WEAPON_MG][i][ZR_DATA_WEAPON_ALIAS], price );
				menu.AddItem ( weaponId, item, acc >= price ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED );
			//	PrintToChat ( client, item );
			}
			
			menu.ExitBackButton = true;
			menu.Display ( client, MENU_TIME_FOREVER );
			
			return;
		}
	}
}

public int zmarketPrimaryWeaponMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	if ( !loringlib_IsValidClient__PlayGame ( param1 ) )
		return;
	
	if ( !g_bSaveMode[param1] )
		if ( g_bZombie[param1] ||
			( g_bBuyWeapon[param1][ZR_ZMARKET_PRIMARY_WEAPON] && g_bBuyWeapon[param1][ZR_ZMARKET_SECONDARY_WEAPON] ) )
			return;
	
	switch ( action ) {
		case MenuAction_Select: {
			static char menuItem[4];
			int weapon = -1;
			menu.GetItem ( param2, menuItem, sizeof ( menuItem ) );
			weapon = StringToInt ( menuItem );
			int acc = loringlib_GetEntityAccount ( param1 );
			if ( g_bSaveMode[param1] )
				acc = g_iConfGameMaxMoney;
				
			int price = StringToInt ( g_strWeaponData[weapon][param2][ZR_DATA_WEAPON_PRICE] );
			if ( acc < price ) {
				PrintToChat ( param1, "[ZR] 자금이 부족합니다." );
				return;
			}
			
			//	Is pistol?
			if ( weapon == ZR_WEAPON_HG ) {
				//	Save the this weapon.
				if ( zmarketSaveWeaponToClient ( 
					param1, 
					StringToInt ( g_strWeaponData[weapon][param2][ZR_DATA_WEAPON_PRICE] ), 
					StringToInt ( g_strWeaponData[weapon][param2][ZR_DATA_WEAPON_TEAM] ), 
					g_strWeaponData[weapon][param2][ZR_DATA_WEAPON_NAME], 
					g_strWeaponData[weapon][param2][ZR_DATA_WEAPON_ALIAS],
					true
				) )	return;
					
				//	Buy the this weapon.
				zmarketBuyWeaponToClient ( 
					param1,
					StringToInt ( g_strWeaponData[weapon][param2][ZR_DATA_WEAPON_PRICE] ), 
					StringToInt ( g_strWeaponData[weapon][param2][ZR_DATA_WEAPON_TEAM] ), 
					g_strWeaponData[weapon][param2][ZR_DATA_WEAPON_NAME], 
					g_strWeaponData[weapon][param2][ZR_DATA_WEAPON_ALIAS],
					true
				);
				
				if ( !( g_bBuyWeapon[param1][ZR_ZMARKET_PRIMARY_WEAPON] && g_bBuyWeapon[param1][ZR_ZMARKET_SECONDARY_WEAPON] ) )
					zmarketBuyWeapons ( param1 );
				
				return;
			}
			
			//	else
			
			//	Save the this weapon.
			if ( zmarketSaveWeaponToClient ( 
				param1, 
				StringToInt ( g_strWeaponData[weapon][param2][ZR_DATA_WEAPON_PRICE] ), 
				StringToInt ( g_strWeaponData[weapon][param2][ZR_DATA_WEAPON_TEAM] ), 
				g_strWeaponData[weapon][param2][ZR_DATA_WEAPON_NAME], 
				g_strWeaponData[weapon][param2][ZR_DATA_WEAPON_ALIAS]
			) )	return;
				
			//	Buy the this weapon.
			zmarketBuyWeaponToClient ( 
				param1,
				StringToInt ( g_strWeaponData[weapon][param2][ZR_DATA_WEAPON_PRICE] ), 
				StringToInt ( g_strWeaponData[weapon][param2][ZR_DATA_WEAPON_TEAM] ), 
				g_strWeaponData[weapon][param2][ZR_DATA_WEAPON_NAME], 
				g_strWeaponData[weapon][param2][ZR_DATA_WEAPON_ALIAS]
			);
			
			if ( !( g_bBuyWeapon[param1][ZR_ZMARKET_PRIMARY_WEAPON] && g_bBuyWeapon[param1][ZR_ZMARKET_SECONDARY_WEAPON] ) )
				zmarketBuyWeapons ( param1 );
		}

		case MenuAction_Cancel: {
			switch ( param2 ) {
				case MenuCancel_ExitBack: {
					zmarketBuyWeapons ( param1 );
					
					return;
				}
				
				case MenuCancel_Exit: {
					delete menu;
					
					return;
				}
			}
		}
	}
}

/**
 * 계속 저장모드에 머무를지 묻습니다.
 */
public void askMoresave ( int client ) {
	Menu menu = new Menu ( askMoresavePanelHandler );
	menu.SetTitle ( "ZMArket\n\t무기를 모두 저장했습니까?" );
	menu.AddItem ( "", "네" );
	menu.AddItem ( "", "로드아웃 및 설정" );
	menu.AddItem ( "", "아니오" );
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int askMoresavePanelHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			switch ( param2 ) {
				case 0: {	onClientCommandZmarket ( param1, 0 );	g_bSaveMode[param1] = false;	}
				case 1: {	zmarketViewMyLoadouts ( param1 );	}
				case 2: {	zmarketBuyWeapons ( param1 );	}
			}
		}
	}
}

/**
 * 무기를 로드아웃에 저장합니다.
 *
 * @param client		클라이언트 인덱스
 * @param price			무기 가격
 * @param team			팀 값
 * @param index			무기 인덱스 값
 * @param alias			무기 별명
 * @param isPistol		무기 권총 여부
 * @return	noreturn
 */
stock bool zmarketSaveWeaponToClient ( int client, int price, int team, char index[64], char alias[64], bool isPistol = false ) {
	//	세이브 모드가 아니면 리턴
	if ( !g_bSaveMode[client] )
		return false;
		
	if ( isPistol ) {
		g_strSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_NAME] = index;
		g_strSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS] = alias;
		g_iSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_PRICE] = price;
		g_iSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_TEAMIDX] = team;
	}
	
	else {
		g_strPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_NAME] = index;
		g_strPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS] = alias;
		g_iPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_PRICE] = price;
		g_iPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_TEAMIDX] = team;
	}
	
	askMoresave ( client );
	
	PrintToChat ( client, "[ZR] %s 무기를 로드아웃에 추가했습니다. \x03[$%d]", alias, price );
	return true;
}

/**
 * 로드아웃을 초기화합니다.
 */
stock void zmarketClearLoadouts ( int client, bool isPistol = false ) {
	if ( isPistol ) {
		g_strSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_NAME] = ZR_ZMARKET_DEFAULT_WEAPON_NAME;
		g_strSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS] = ZR_ZMARKET_DEFAULT_WEAPON_NAME;
		g_iSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_PRICE] = 0;
		g_iSecondaryWeaponData[client][ZR_ZMARKET_WEAPON_TEAMIDX] = 0;
	}
	
	else {
		g_strPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_NAME] = ZR_ZMARKET_DEFAULT_WEAPON_NAME;
		g_strPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_ALIAS] = ZR_ZMARKET_DEFAULT_WEAPON_NAME;
		g_iPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_PRICE] = 0;
		g_iPrimaryWeaponData[client][ZR_ZMARKET_WEAPON_TEAMIDX] = 0;
	}
	
	zmarketViewMyLoadouts ( client );
}

/**
 * 무기를 구입합니다.
 *
 * @param client		클라이언트 인덱스
 * @param price			무기 가격
 * @param team			팀 값
 * @param index			무기 인덱스 값
 * @param alias			무기 별명
 * @param isPistol		무기 권총 여부
 * @return			구입 성공 시 true, 성공하지 못했다면 false를 반환합니다.
 */
stock bool zmarketBuyWeaponToClient ( int client, int price, int team, char index[64], char alias[64], bool isPistol = false ) {
//	PrintToChat ( client, "test" );
	//	세이브 모드일 경우
	if ( g_bSaveMode[client] )
		return false;
		
	//	클라이언트 자금보다 높을 경우
	if ( price > loringlib_GetEntityAccount ( client ) ) {
		PrintToChat ( client, "[ZR] 자금이 부족합니다." );
		return false;
	}
	
	if ( isPistol ) {
		if ( g_bBuyWeapon[client][ZR_ZMARKET_SECONDARY_WEAPON] ) {
			PrintToChat ( client, "[ZR] 이미 구입했습니다." );
			return false;
		}
		
		loringlib_RemoveWeaponSlot ( client, CS_SLOT_SECONDARY );
		g_bBuyWeapon[client][ZR_ZMARKET_SECONDARY_WEAPON] = true;
	}
	
	else {
		if ( g_bBuyWeapon[client][ZR_ZMARKET_PRIMARY_WEAPON] ) {
			PrintToChat ( client, "[ZR] 이미 구입했습니다." );
			return false;
		}
	
		loringlib_RemoveWeaponSlot ( client, CS_SLOT_PRIMARY );
		g_bBuyWeapon[client][ZR_ZMARKET_PRIMARY_WEAPON] = true;
	}
	
	int teamnum = GetClientTeam ( client );
	if ( team != -1 )
		loringlib_SetEntityTeamNumber ( client, team );
	else {
		PrintToChat ( client, "[ZR] 무기의 데이터에 문제가 있습니다. 관리자에게 문의하세요." );
		return false;
	}
	GivePlayerItem ( client, index );
	loringlib_SetEntityTeamNumber ( client, teamnum );
	
	loringlib_SubEntityAccount ( client, price );
	
	return true;
}