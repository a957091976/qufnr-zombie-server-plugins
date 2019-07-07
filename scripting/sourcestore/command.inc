#define SS_SETTINGTYPE_ADD		1
#define SS_SETTINGTYPE_MINUS	2
#define SS_SETTINGTYPE_SET		3
int g_iSettingType[MAXPLAYERS + 1] = { 0, ... };
int g_iSettingTarget[MAXPLAYERS + 1] = { -1, ... };
bool g_bSettingMode[MAXPLAYERS + 1] = { false, ... };

public void onRegisterCommands () {
	RegConsoleCmd ( "say", onClientCommandSay );
	RegConsoleCmd ( "say_team", onClientCommandSay );
	
	RegConsoleCmd ( "say", onClientCommandSayAdminSetting );
	RegConsoleCmd ( "say_team", onClientCommandSayAdminSetting );
	
	RegConsoleCmd ( "sm_sinfo", onClientCommandStoreInfo );
	RegConsoleCmd ( "sm_storeitemlist", onClientCommandStoreItemList );
	RegConsoleCmd ( "sm_store", onClientCommandStore );
	RegConsoleCmd ( "autobuy", onClientCommandStoreBind );
	if ( GetEngineVersion () == Engine_CSS )
		RegConsoleCmd ( "cheer", onClientCommandStoreBind );
	RegConsoleCmd ( "sm_pstore", onClientCommandPremiumStore );
	
	RegAdminCmd ( "sm_ssadmin", onClientCommandSSAdmin, ADMFLAG_ROOT );
	RegAdminCmd ( "sm_givestoreitem", onClientCommandGiveStoreItem, ADMFLAG_ROOT );
	RegAdminCmd ( "sm_ssadmincmd", onClientCommandStoreAdminCmd, ADMFLAG_ROOT );
	RegAdminCmd ( "@adminskins", onClientCommandAdminSkins, ADMFLAG_BAN );
	
	RegConsoleCmd ( "sm_mirror", onClientCommandMirror );
	RegConsoleCmd ( "sm_tp", onClientCommandThirdPerson );
	
	RegConsoleCmd ( "sm_rainbow", onClientCommandRainbow );
}

public void findCvars () {
	g_hCvarAllowThridperson = FindConVar ( "sv_allow_thirdperson" );
	if ( g_hCvarAllowThridperson == null )
		LogError ( "Convar \"sv_allow_thirdperson\" not found!" );
	SetConVarInt ( g_hCvarAllowThridperson, 1 );
	HookConVarChange ( g_hCvarAllowThridperson, changeCvar_AllowThridperson );
}

public void changeCvar_AllowThridperson ( ConVar cvar, const char[] oldVal, const char[] newVal ) {
	if ( cvar == g_hCvarAllowThridperson )
		if ( StringToInt ( newVal ) != -1 )
			SetConVarInt ( g_hCvarAllowThridperson, 1 );
}

public Action onClientCommandStoreBind ( int client, int args ) {
	onClientCommandStore ( client, 0 );
	return Plugin_Handled;
}

/**
 * View info.
 */
public Action onClientCommandStoreInfo ( int client, int args ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Stop;
	
	if ( args < 1 ) {
		PrintToChat ( client, "%s \x01Usage: sm_sinfo <user>", g_strConfChatPrefix );
		return Plugin_Stop;
	}
	
	static char arg1[32];
	GetCmdArg ( 1, arg1, sizeof ( arg1 ) );
//	int target = loringlib_GetIndexByClientName ( arg1, false, false );
	static int target;
	target = loringlib_FindTarget ( arg1 );
	
	if ( !loringlib_IsValidClient ( target ) ) {
		PrintToChat ( client, "%s \x01알 수 없는 클라이언트 입니다.", g_strConfChatPrefix );
		return Plugin_Stop;
	}
	
	displayPlayerInfo ( client, target );
	return Plugin_Stop;
}

public void displayPlayerInfo ( int client, int target ) {
	if ( !loringlib_IsValidClient ( client ) )
		return;
	
	if ( !loringlib_IsValidClient ( target ) ) {
		PrintToChat ( client, "%s ERROR: \x02Invalid Client Index: %d", g_strConfChatPrefix, target );
		return;
	}
	
	static char idstr[32];
	GetClientAuthId ( target, AuthId_SteamID64, idstr, sizeof ( idstr ) );
	static char balance_format[16];
	loringlib_NumberFormat ( g_iBalance[target], balance_format, sizeof ( balance_format ) );
	static char info_format[3][256];
	Format ( info_format[0], sizeof ( info_format[] ), "\t\t지갑: %s %s", balance_format, g_strConfMoneyName );
	Format ( info_format[1], sizeof ( info_format[] ), "\t\t프리미엄: %s", g_bIsMvpPlus[target] ? "MVP+" : ( g_bIsMvp[target] ? "MVP" : "NONE" ) );
	Format ( info_format[2], sizeof ( info_format[] ), "\t\tCommunity ID: %s", idstr );
	
	Menu menu = new Menu ( playerInfoMenuHandle );
	menu.SetTitle ( "%N님의 스토어 정보:\n　%s\n%s\n%s", target, info_format[0], info_format[1], info_format[2] );
	
	static char item[5][64];
	getEquipItemName ( client, ITEMTYPE_CHARACTER, item[0], sizeof ( item[] ) );
	getEquipItemName ( client, ITEMTYPE_TRAIL, item[1], sizeof ( item[] ) );
	getEquipItemName ( client, ITEMTYPE_TAGTITLE, item[2], sizeof ( item[] ) );
	getEquipItemName ( client, ITEMTYPE_LASERBULLET, item[3], sizeof ( item[] ) );
	getEquipItemName ( client, ITEMTYPE_AURA, item[4], sizeof ( item[] ) );
	static char list[5][128];
	Format ( list[0], sizeof ( list[] ), "캐릭터: %s", item[0] );
	Format ( list[1], sizeof ( list[] ), "트레일: %s", item[1] );
	Format ( list[2], sizeof ( list[] ), "칭호: %s", item[2] );
	Format ( list[3], sizeof ( list[] ), "레이저 총알: %s", item[3] );
	Format ( list[4], sizeof ( list[] ), "아우라: %s", item[4] );
	for ( int i = 0; i < 5; i ++ ) {
		if ( i == ( ITEMTYPE_TAGTITLE - 1 ) ) {
			menu.AddItem ( "#TAGTITLE_ITEM", list[i], g_iConfMenuEnable[MENUTYPE_TAGTITLE] == 1 ? ITEMDRAW_DISABLED : ITEMDRAW_SPACER );
			continue;
		}
		
		menu.AddItem ( item[i], list[i], g_iConfMenuEnable[i + 1] == 1 ? ITEMDRAW_DISABLED : ITEMDRAW_SPACER );
	}
	
	static char targetidx[16];
	IntToString ( target, targetidx, sizeof ( targetidx ) );
	menu.AddItem ( targetidx, "인벤토리" );
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int playerInfoMenuHandle ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			static char sel[16];
			menu.GetItem ( param2, sel, sizeof ( sel ) );
			int index = StringToInt ( sel );
			displayPlayerInventory ( param1, index );
		}
		case MenuAction_Cancel: {
			switch ( param2 ) {
				case MenuCancel_ExitBack: {
					onClientCommandStore ( param1, 0 );
				}
			}
		}
	}
}

/**
 * 아이템 목록 보기
 */
public Action onClientCommandStoreItemList ( int client, int args ) {
	if ( client == CONSOLE ) {
		PrintToServer ( "INDEX | ITEM NAME" );
		for ( int i = 0; i < g_iDefineStoreItem; i ++ ) {
			PrintToServer ( "%d | %s", i, g_strItemName[i] );
		}
		
		return Plugin_Handled;
	}
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Handled;
	
	if ( !loringlib_IsClientAdmin ( client ) )
		return Plugin_Handled;
		
	Menu menu = new Menu ( menuStoreItemListHandle );
	menu.SetTitle ( "Store Item List:\n　" );
	static char format[256];
	for ( int i = 0; i < g_iDefineStoreItem; i ++ ) {
		Format ( format, sizeof ( format ), "%s [%d]", g_strItemName[i], i );
		menu.AddItem ( "", format, ITEMDRAW_DISABLED );
	}
	menu.Display ( client, MENU_TIME_FOREVER );
	return Plugin_Handled;
}

public int menuStoreItemListHandle ( Menu menu, MenuAction action, int param1, int param2 ) {
	if ( action == MenuAction_End )
		delete menu;
}

/**
 * 타 유저 인벤토리 보기
 */
public void displayPlayerInventory ( int client, int target ) {
	if ( !loringlib_IsValidClient ( client ) )
		return;
	
	if ( !loringlib_IsValidClient ( target ) ) {
		PrintToChat ( client, "%s \x02Invalid Client Index: %d", g_strConfChatPrefix, target );
		return;
	}
	
	Menu menu = new Menu ( displayPlayerInvHandle );
	menu.SetTitle ( "%N님의 스토어 정보:\n\t인벤토리\n　", target );
	bool validItem = false;
	
	static char targetidx[16];
	IntToString ( target, targetidx, sizeof ( targetidx ) );
	menu.AddItem ( targetidx, "뒤로\n　\n　" );
	
	static char itemInfo[3][128];
	for ( int i = 0; i < g_iDefineStoreItem; i ++ ) {
		if ( g_iItem[target][i] > 0 ) {
			validItem = true;
			getItemRareName ( i, itemInfo[0], 127 );
			Format ( itemInfo[1], 127, "%s | %s [%d개]", g_strItemName[i], itemInfo[0], g_iItem[target][i] );
			Format ( itemInfo[2], 127, "%d", i );
			menu.AddItem ( itemInfo[2], itemInfo[1], ITEMDRAW_DISABLED );
		}
	}
	if ( !validItem )
		menu.AddItem ( "", "없음", ITEMDRAW_DISABLED );
	menu.ExitButton = false;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int displayPlayerInvHandle ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			static char sel[16];
			menu.GetItem ( param2, sel, sizeof ( sel ) );
			int index = StringToInt ( sel );
			displayPlayerInfo ( param1, index );
		}
	}
}

/** 
 * Give Item Cmd.
 */
public Action onClientCommandGiveStoreItem ( int client, int args ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Stop;
	
	if ( !loringlib_IsClientAdmin ( client ) ) {
		PrintToChat ( client, "\x04[Store Admin]\x01 \x01귀하는 권한이 없습니다." );
		return Plugin_Stop;
	}
	
	if ( args < 1 ) {
		PrintToChat ( client, "\x04[Store Admin]\x01 \x01Usage: sm_givestoreitem <username> <item>" );
		return Plugin_Stop;
	}
	
	static char arg1[32];
	GetCmdArg ( 1, arg1, sizeof ( arg1 ) );
	static char targetname[32];
	int target = -1;
	for ( int i = 1; i <= MaxClients; i ++ ) {
		if ( loringlib_IsValidClient ( i ) ) {
			GetClientName ( i, targetname, sizeof ( targetname ) );
			if ( StrContains ( arg1, targetname, false ) != -1 )
				target = i;
		}
	}
	
	if ( !loringlib_IsValidClient ( target ) ) {
		PrintToChat ( client, " \x04[Store Admin]\x01 Invalid client index: %d", target );
		return Plugin_Stop;
	}
	
	static char arg2[16];
	GetCmdArg ( 2, arg2, sizeof ( arg2 ) );
	if ( !loringlib_IsStringNumeric ( arg2, sizeof ( arg2 ) ) ) {
		PrintToChat ( client, " \x04[Store Admin]\x01 %s은(는) 아이템 값이 아닙니다.", arg2 );
		return Plugin_Stop;
	}
	
	int itemId = StringToInt ( arg2 );
	if ( !isValidItem ( itemId ) ) {
		PrintToChat ( client, " \x04[Store Admin]\x01 %d는 유효하지 않는 아이템 값입니다.", itemId );
		return Plugin_Stop;
	}
	
	g_iItem[target][itemId] ++;
	PrintToChat ( client, " \x04[Store Admin]\x01 %N님에게 %s 아이템을 지급했습니다.", target, g_strItemName[itemId] );
	PrintToChat ( target, " \x04[Store Admin]\x01 관리자가 귀하에게 %s 아이템을 지급했습니다.", g_strItemName[itemId] );
	return Plugin_Stop;
}

/**
 * Admin commands.
 */
public Action onClientCommandStoreAdminCmd ( int client, int args ) {
	if ( !loringlib_IsValidClient ( client ) && client != 0 )
		return Plugin_Handled;
	
	if ( args < 2 ) {
		ReplyToCommand ( client, " \x04[Admin Command] \x01Usage: sm_ssadmincmd <#userid|name> <type> <amount>" );
		ReplyToCommand ( client, " \x04[Admin Command] \x01List of types: 1 = Balance, 2 = Premium Rank" );
		return Plugin_Handled;
	}
	
	char targetname[32];
	GetCmdArg ( 1, targetname, sizeof ( targetname ) );
	
	int target = -1;
	target = FindTarget ( client, targetname, true, false );
	
	if ( !loringlib_IsValidClient ( target ) ) {
		ReplyToCommand ( client, " \x04[Admin Command] \x01유효하지 않는 클라이언트 입니다." );
		return Plugin_Handled;
	}
	
	if ( target == client ) {
		ReplyToCommand ( client, " \x04[Admin Command] \x01You can not give it myself." );
		return Plugin_Handled;
	}
	
	char type[8];
	GetCmdArg ( 2, type, sizeof ( type ) );
	if ( !loringlib_IsStringNumeric ( type, sizeof ( type ) ) ) {
		ReplyToCommand ( client, " \x04[Admin Command] \x01List of types: 1 = Balance, 2 = Store Rank, 3 = Premium Rank" );
		return Plugin_Handled;
	}
	
	int type_num = StringToInt ( type );
	if ( type_num > 4 ) {
		ReplyToCommand ( client, " \x04[Admin Command] \x01Type %d was invalid type number.");
		return Plugin_Handled;
	}
	
	char amounts[16];
	GetCmdArg ( 3, amounts, sizeof ( amounts ) );
	int amount_num = StringToInt ( amounts );
	if ( amount_num < 1 || amount_num > 2 ) {
		ReplyToCommand ( client, " \x04[Admin Command] \x01The value must be greater than 0." );
		return Plugin_Handled;
	}
	
	switch ( type_num ) {
		case 1: {
			PrintToChat ( target, " \x04[Admin Command] \x01You got %d %s(s) from Administrator.", amount_num, g_strConfMoneyName );
			ReplyToCommand ( client, " \x04[Admin Command] \x01Added %d %s(s) on %N client.", amount_num, g_strConfMoneyName, target );
			g_iBalance[target] += amount_num;
			return Plugin_Handled;
		}
		
		case 2: {
			switch ( amount_num ) {
				case 1: {
					ReplyToCommand ( client, " \x04[Admin Command] \x01Set VIP on %N client.", target );
					g_bIsMvp[target] = true;
					g_bIsMvpPlus[target] = false;
				}
				case 2: {
					ReplyToCommand ( client, " \x04[Admin Command] \x01Set SVIP on %N client.", target );
					g_bIsMvp[target] = true;
					g_bIsMvpPlus[target] = true;
				}
				case 3: {
					ReplyToCommand ( client, " \x04[Admin Command] \x01Remove premium rank on %N client.", target );
					g_bIsMvp[target] = false;
					g_bIsMvpPlus[target] = false;
				}
			}
			
			return Plugin_Handled;
		}
		
		default: {
			ReplyToCommand ( client, " \x04[Admin Command] \x01ERROR: Invalid type number. Please try again." );
			return Plugin_Handled;
		}
	}
	
	return Plugin_Handled;
}

/**
 * Admin tools.
 */
public Action onClientCommandSSAdmin ( int client, int args ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Stop;
	
	if ( !loringlib_IsClientAdmin ( client ) ) {
		PrintToChat ( client, "%s \x01귀하는 권한이 없습니다.", g_strConfChatPrefix );
		return Plugin_Stop;
	}
	
	Menu menu = new Menu ( ssadminMenuHandler );
	menu.SetTitle ( "SSADMIN:\n　" );
	menu.AddItem ( "", "클라이언트 자금 설정" );
	menu.AddItem ( "", "클라이언트 프리미엄 등급 설정" );
	menu.AddItem ( "", "RELOAD SOURCE STORE" );
	menu.Display ( client, MENU_TIME_FOREVER );
	
	return Plugin_Continue;
}

public int ssadminMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_Select: {
			switch ( param2 ) {
				case 0: setClientBalanceMenu ( param1 );
				case 1: setClientStorePremium ( param1 );
				case 2: reloadSSKeyValues ( param1 );
			}
		}
	}
}

/**
 * Reload SourceStore Key values.
 */
public void reloadSSKeyValues ( int client ) {
	if ( !loringlib_IsClientAdmin ( client ) ) {
		PrintToChat ( client, "%s \x01귀하는 권한이 없습니다.", g_strConfChatPrefix );
		return;
	}
	
	Menu menu = new Menu ( reloadMenuHandler );
	menu.SetTitle ( "SSADMIN:\n\tRELOAD SOURCE STORE\n　" );
	menu.AddItem ( "", "Reload Configurations", ITEMDRAW_DISABLED );
	menu.AddItem ( "", "Reload Item Database" );
	
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int reloadMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_Select: {
			switch ( param2 ) {
			//	case 0:	reloadConfigurations ();
				case 1: reloadItemDatabase ();
			}
		}
		
		case MenuAction_Cancel: {
			switch ( param2 ) {
				case MenuCancel_ExitBack:{
					onClientCommandSSAdmin ( param1, 0 );
					return;
				}
			}
		}
	}
}

/**
 * Reload item database.
 */
public void reloadItemDatabase () {
	clearStoreItem ();
	DataPack data = new DataPack ();
	data.WriteCell ( true );
	CreateDataTimer ( 0.1, timerDefineStoreItemPost, data );
}

/**
 * Set client store premium.
 */
public void setClientStorePremium ( int client ) {
	g_iSettingTarget[client] = -1;
	Menu menu = new Menu ( clientStorePremiumMenuHandler );
	menu.SetTitle ( "SSADMIN:\n\t클라이언트 프리미엄 등급 설정\n　" );
	menu.AddItem ( "", "VIP" );
	menu.AddItem ( "", "SVIP" );
	menu.AddItem ( "", "Delete" );
	
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int clientStorePremiumMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_Select: {
			g_iSettingTarget[param1] = param2;
			setStorePremiumPlayerList ( param1 );
		}
		
		case MenuAction_Cancel: {
			switch ( param2 ) {
				case MenuCancel_ExitBack:{
					onClientCommandSSAdmin ( param1, 0 );
					return;
				}
			}
		}
	}
}

public void setStorePremiumPlayerList ( int client ) {
	Menu menu = new Menu ( setStorePremiumPlayerListHandler );
	menu.SetTitle ( "SSADMIN:\n\t클라이언트 프리미엄 등급 설정\n　" );
	static char list[64];
	static char clients[4];
	for ( int i = 1; i <= MaxClients; i ++ ) {
		if ( !loringlib_IsValidClient ( i ) )
			continue;
			
		if ( IsFakeClient ( i ) )
			continue;
			
		Format ( clients, sizeof ( clients ), "%d", i );
		Format ( list, sizeof ( list ), "%N%s", i, loringlib_IsClientAdmin ( i ) ? "[ADM]" : "" );
		menu.AddItem ( clients, list );
	}
	
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int setStorePremiumPlayerListHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_Select: {
			static char item[4];
			menu.GetItem ( param2, item, sizeof ( item ) );
			int id = StringToInt ( item );
			if ( !loringlib_IsValidClient ( id ) ) {
				PrintToChat ( param1, "%s \x01ERROR: 유효하지 않는 클라이언트 값입니다.", g_strConfChatPrefix );
				return;
			}
			
			char type[16];
			switch ( g_iSettingTarget[param1] ) {
				case 0: {
					strcopy ( type, sizeof ( type ), "VIP" );
					g_bIsMvp[id] = true;
					g_bIsMvpPlus[id] = false;
				}
				
				case 1: {
					strcopy ( type, sizeof ( type ), "SVIP" );
					g_bIsMvp[id] = true;
					g_bIsMvpPlus[id] = true;
				}
				
				case 2: {
					g_bIsMvp[id] = false;
					g_bIsMvpPlus[id] = false;
					PrintToChat ( param1, "%s \x01%N님의 프리미엄 등급을 제거 했습니다.", g_strConfChatPrefix, id );
					return;
				}
			}
			
			PrintToChat ( param1, "%s \x01%N님의 프리미엄 등급을 %s (으)로 설정했습니다.", g_strConfChatPrefix, id, type );

			CreateTimer ( 0.5, timerClearSettingTarget, param2 );
		}
	}
}

/**
 * Set client balance.
 */
public void setClientBalanceMenu ( int client ) {
	g_bSettingMode[client] = false;
	g_iSettingType[client] = 0;
	g_iSettingTarget[client] = -1;
	
	Menu menu = new Menu ( clientBalanceMenuHandler );
	menu.SetTitle ( "SSADMIN:\n\t클라이언트 자금 설정\n　" );
	menu.AddItem ( "1", "Add Balance" );
	menu.AddItem ( "2", "Cutback Balance" );
	menu.AddItem ( "3", "Set Balance" );
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int clientBalanceMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_Select: {
			static char item[4];
			menu.GetItem ( param2, item, sizeof ( item ) );
			g_iSettingType[param1] = StringToInt ( item );
			setBalancePlayerList ( param1 );
		}
		
		case MenuAction_Cancel: {
			switch ( param2 ) {
				case MenuCancel_ExitBack:{
					onClientCommandSSAdmin ( param1, 0 );
					return;
				}
			}
		}
	}
}

/**
 * Set Balance Menu player list.
 */
public void setBalancePlayerList ( int client ) {
	Menu menu = new Menu ( setBalancePlayerListHandler );
	menu.SetTitle ( "SSADMIN:\n\t클라이언트 자금 설정\n　" );
	static char list[64];
	static char clients[4];
	for ( int i = 1; i <= MaxClients; i ++ ) {
		if ( !loringlib_IsValidClient ( i ) )
			continue;
			
		if ( IsFakeClient ( i ) )
			continue;
		
		IntToString ( i, clients, sizeof ( clients ) );
		Format ( list, sizeof ( list ), "%N%s", i, loringlib_IsClientAdmin ( i ) ? "[ADM]" : "" );
		menu.AddItem ( clients, list );
	}
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int setBalancePlayerListHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_Select: {
			static char item[4];
			menu.GetItem ( param2, item, sizeof ( item ) );
			g_bSettingMode[param1] = true;
			g_iSettingTarget[param1] = StringToInt ( item );
			PrintToChat ( param1, "%s \x01채팅창에 값을 입력하세요:", g_strConfChatPrefix );
		}
		
		case MenuAction_Cancel: {
			switch ( param2 ) {
				case MenuCancel_ExitBack:{
					setClientBalanceMenu ( param1 );
					return;
				}
			}
		}
	}
}

public Action onClientCommandSayAdminSetting ( int client, int args ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Continue;
	
	if ( !loringlib_IsClientAdmin ( client ) )
		return Plugin_Continue;
		
	if ( !g_bSettingMode[client] )
		return Plugin_Continue;
	
	if ( g_iSettingType[client] == 0 )
		return Plugin_Continue;
	
	if ( !loringlib_IsValidClient ( g_iSettingTarget[client] ) )
		return Plugin_Continue;
		
	static char contents[16];
	GetCmdArgString ( contents, sizeof ( contents ) );
	StripQuotes ( contents );
	
	if ( StrEqual ( contents, "cancel", false ) ) {
		PrintToChat ( client, "%s \x01취소되었습니다.", g_strConfChatPrefix );
		g_bSettingMode[client] = false;
		g_iSettingType[client] = 0;
		g_iSettingTarget[client] = -1;
		onClientCommandSSAdmin ( client, 0 );
		
		return Plugin_Handled;
	}
	
	if ( !loringlib_IsStringNumeric ( contents, sizeof ( contents ) ) ) {
		PrintToChat ( client, "%s \x01숫자로 입력하세요.", g_strConfChatPrefix );
		
		return Plugin_Handled;
	}
	
	switch ( g_iSettingType[client] ) {
		case SS_SETTINGTYPE_ADD: {
			PrintToChat ( client, "%s \x01%N님에게 %s %s 만큼 추가했습니다.", g_strConfChatPrefix, g_iSettingTarget[client], contents, g_strConfMoneyName );
			PrintToChat ( g_iSettingTarget[client], "%s \x01관리자에 의해 %s %s 이(가) 추가되었습니다.", g_strConfChatPrefix, contents, g_strConfMoneyName );
			g_iBalance[g_iSettingTarget[client]] += StringToInt ( contents );
		}
		
		case SS_SETTINGTYPE_MINUS: {
			PrintToChat ( client, "%s \x01%N님에게 %s %s 만큼 삭감했습니다.", g_strConfChatPrefix, g_iSettingTarget[client], contents, g_strConfMoneyName );
			PrintToChat ( g_iSettingTarget[client], "%s \x01관리자에 의해 %s %s 이(가) 삭감되었습니다.", g_strConfChatPrefix, contents, g_strConfMoneyName );
			g_iBalance[g_iSettingTarget[client]] -= StringToInt ( contents );
		}
		
		case SS_SETTINGTYPE_SET: {
			PrintToChat ( client, "%s \x01%N님의 데이터를 %s %s (으)로 설정했습니다.", g_strConfChatPrefix, g_iSettingTarget[client], contents, g_strConfMoneyName );
			PrintToChat ( g_iSettingTarget[client], "%s \x01관리자에 의해 %s %s (으)로 설정되었습니다.", g_strConfChatPrefix, contents, g_strConfMoneyName );
			g_iBalance[g_iSettingTarget[client]] = StringToInt ( contents );
		}
	}
	
	g_bSettingMode[client] = false;
	g_iSettingType[client] = 0;
	g_iSettingTarget[client] = -1;
	return Plugin_Handled;
}

/**
 * 미러 모드
 */
public Action onClientCommandMirror ( int client, int args ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Continue;
		
	if ( !IsPlayerAlive ( client ) ) {
		ReplyToCommand ( client, "%s \x01You may not use this command while dead.", g_strConfChatPrefix );
		return Plugin_Handled;
	}
	
	if ( g_iClientSettingToggle[SETTINGTYPE_MIRROR][client] ) {
		g_iClientSettingToggle[SETTINGTYPE_MIRROR][client] = !g_iClientSettingToggle[SETTINGTYPE_MIRROR][client];
		toggleMirror ( client, g_iClientSettingToggle[SETTINGTYPE_MIRROR][client] );
		return Plugin_Handled;
	}
	
	if ( !g_iClientSettingToggle[SETTINGTYPE_MIRROR][client] ) {
		if ( g_iClientSettingToggle[SETTINGTYPE_THIRDPERSON][client] ) {
			ReplyToCommand ( client, "%s \x013인칭 상태에서는 사용할 수 없습니다.", g_strConfChatPrefix );
			return Plugin_Handled;
		}
	}
	
	if ( !( GetEntityFlags ( client ) & FL_ONGROUND ) ) {
		ReplyToCommand ( client, "%s \x01공중에서는 사용할 수 없습니다.", g_strConfChatPrefix );
		return Plugin_Handled;
	}
	
	if ( loringlib_GetEntitySpeed ( client ) > 0.0 ) {
		ReplyToCommand ( client, "%s \x01움직이는 상태에서는 사용할 수 없습니다.", g_strConfChatPrefix );
		return Plugin_Handled;
	}
	
	g_iClientSettingToggle[SETTINGTYPE_MIRROR][client] = !g_iClientSettingToggle[SETTINGTYPE_MIRROR][client];
	toggleMirror ( client, g_iClientSettingToggle[SETTINGTYPE_MIRROR][client] );
	return Plugin_Handled;
}

/**
 * 미러 모드 설명 메세지
 */
public void displayMirrorModeMsg ( int client ) {
	if ( !loringlib_IsValidClient__PlayGame ( client ) )
		return;
	
	if ( !g_iClientSettingToggle[SETTINGTYPE_MIRROR][client] )
		return;
	
	PrintCenterText ( client, "화면을 돌려가며 캐릭터의 모습을 확인하세요.<br/>미러 모드 상태에서는 움직일 수 없습니다." );
}
 
/**
 * Toggle the mirror mode.
 *
 * @client
 * @toggle	1 = Enable, 0 = Disable
 */
stock void toggleMirror ( int client, int toggle ) {
	if ( !loringlib_IsValidClient ( client ) )
		return;
	
	if ( IsFakeClient ( client ) )
		return;
	
	if ( toggle == 1 ) {
		if ( !IsPlayerAlive ( client ) )
			return;
		
		SetEntPropEnt ( client, Prop_Send, "m_hObserverTarget", 0 );
		SetEntProp ( client, Prop_Send, "m_iObserverMode", 1 );
		SetEntProp ( client, Prop_Send, "m_bDrawViewmodel", 0 );
		SetEntProp ( client, Prop_Send, "m_iFOV", 120 );
		SendConVarValue ( client, FindConVar ( "mp_forcecamera" ), "1" );
		
		SetEntityMoveType ( client, MOVETYPE_NONE );
	}
	
	else {
		SetEntPropEnt ( client, Prop_Send, "m_hObserverTarget", -1 );
		SetEntProp ( client, Prop_Send, "m_iObserverMode", 0 );
		SetEntProp ( client, Prop_Send, "m_bDrawViewmodel", 1 );
		SetEntProp ( client, Prop_Send, "m_iFOV", 90 );
		static char val[6];
		GetConVarString ( FindConVar ( "mp_forcecamera" ), val, 6 );
		SendConVarValue ( client, FindConVar ( "mp_forcecamera" ), val );
		
		SetEntityMoveType ( client, MOVETYPE_WALK );
	}
}

/**
 * 3인칭, 1인칭
 */
public Action onClientCommandThirdPerson ( int client, int args ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Continue;
	
	if ( !g_iClientSettingToggle[SETTINGTYPE_THIRDPERSON][client] ) {
		if ( g_iClientSettingToggle[SETTINGTYPE_MIRROR][client] ) {
			ReplyToCommand ( client, "%s \x01거울 모드 상태에서는 사용할 수 없습니다.", g_strConfChatPrefix );
			return Plugin_Handled;
		}
	}
	
	g_iClientSettingToggle[SETTINGTYPE_THIRDPERSON][client] = !g_iClientSettingToggle[SETTINGTYPE_THIRDPERSON][client];
	if ( g_iClientSettingToggle[SETTINGTYPE_THIRDPERSON][client] == 1 )
		ClientCommand ( client, "thirdperson" );
	else
		ClientCommand ( client, "firstperson" );
	
	return Plugin_Handled;
}

public Action timerClearSettingTarget ( Handle timer, int client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Stop;
	
	g_iSettingTarget[client] = -1;
	return Plugin_Continue;
}

public Action onClientCommandRainbow ( int client, int args ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Handled;
	
	if ( !g_bIsMvpPlus[client] ) {
		ReplyToCommand ( client, "%s 귀하는 권한이 없습니다.", g_strConfChatPrefix );
		return Plugin_Handled;
	}
	
	Menu menu = new Menu ( donatorRainbowMenuHandler );
	menu.SetTitle ( "%s\n\tYou're the best donator.\n　\n\t\tRainbow Body:\n　", g_strConfMenuTitle );
	
	char format[256];
	Format ( format, sizeof ( format ), "Rainbow Body: %s", g_bToggleRainbow[client] ? "활성화" : "비활성화" );
	menu.AddItem ( "", format, g_bIsMvpPlus[client] ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED );
	menu.AddItem ( "", "속도 설정", g_bIsMvpPlus[client] ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED );
	
	menu.Display ( client, MENU_TIME_FOREVER );
	return Plugin_Handled;
}

public int donatorRainbowMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			switch ( param2 ) {
				case 0:	{
					g_bToggleRainbow[param1] = !g_bToggleRainbow[param1];
					if ( !g_bToggleRainbow[param1] )
						loringlib_SetEntityRenderColorEx ( param1, { 255, 255, 255, 255 } );
					onClientCommandRainbow ( param1, 0 );
				}
				
				case 1: {
					donatorRainbowSetTicktime ( param1 );
				}
			}
		}
	}
}

void donatorRainbowSetTicktime ( int client ) {
	if ( loringlib_IsValidClient ( client ) ) {
		if ( !g_bIsMvpPlus[client] ) {
			PrintToChat ( client, "%s 귀하는 권한이 없습니다.", g_strConfChatPrefix );
			return;
		}
		
		Menu menu = new Menu ( donatorRainbowSetTicktimeMenuHandler );
		menu.SetTitle ( "%s\n\tYou're the best donator.\n　\n\t\t속도 설정:\n　", g_strConfMenuTitle );
		menu.AddItem ( "0.1", "빠르게", g_fRainbowTickTimes[client] == 0.1 ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT );
		menu.AddItem ( "0.5", "보통", g_fRainbowTickTimes[client] == 0.5 ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT );
		menu.AddItem ( "1.0", "느리게", g_fRainbowTickTimes[client] == 1.0 ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT );
		
		menu.Display ( client, MENU_TIME_FOREVER );
	}
}

public int donatorRainbowSetTicktimeMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			char item[16];
			menu.GetItem ( param2, item, sizeof ( item ) );
			float scale = StringToFloat ( item );
			
			if ( scale <= 0.0 )
				return;
			
			g_fRainbowTickTimes[param1] = scale;
			PrintToChat ( param1, "%s Changing rainbow body tick times to %.1f Ticks.", g_strConfChatPrefix, scale );
			
		}
	}
}

public Action onClientCommandAdminSkins ( int client, int args ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Handled;
	
	Menu menu = new Menu ( adminskinMenuHandle );
	menu.SetTitle ( "%s\n\tAdmin Skins:\n　", g_strConfMenuTitle );
	
	char steam[32];
	GetClientAuthId ( client, AuthId_Steam3, steam, sizeof ( steam ) );
	
	char format[128];
	char itemId[16];
	for ( int i = 0; i < g_iDefineStoreItem; i ++ ) {
		if ( g_iItemRarity[i] == ITEMRARE_ADMIN ) {
			IntToString ( i, itemId, sizeof ( itemId ) );
			Format ( format, sizeof( format ), "%s %s", g_strItemName[i], StrContains ( steam, g_strItemAdmSkinSteamID[i], false ) != -1 ? "[ONLY YOU]" : "[PRIVATE]" );
			menu.AddItem ( itemId, format, StrContains ( steam, g_strItemAdmSkinSteamID[i], false ) != -1 ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED );
		}
	}
	menu.Display ( client, MENU_TIME_FOREVER );
	
	return Plugin_Handled;
}

public int adminskinMenuHandle ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			char item[16];
			menu.GetItem ( param2, item, sizeof ( item ) );
			int itemId = StringToInt ( item );
			if ( itemId != -1 )
				buyItem ( param1, itemId );
		}
	}
}