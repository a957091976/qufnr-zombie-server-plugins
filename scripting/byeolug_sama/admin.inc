/**
 * 아이템 초기화 메뉴
 */
public Action clientCommandResetItemMenu ( int client, int args ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Handled;
	
	if ( !loringlib_IsClientAdmin ( client ) ) {
		CLIENT_PERMISSION_MSG ( client )
		return Plugin_Handled;
	}
	
	Menu menu = new Menu ( resetItemMenuHandler );
	menu.SetTitle ( "%s\n　", MENU_TITLE );
	static char clientnum[4];
	static char name[32];
	for ( int i = 1; i <= MaxClients; i ++ ) {
		if ( loringlib_IsValidClient ( i ) ) {
			IntToString ( i, clientnum, sizeof ( clientnum ) );
			GetClientName ( i, name, sizeof ( name ) );
			menu.AddItem ( clientnum, name );
		}
	}
	
	menu.Display ( client, MENU_TIME_FOREVER );
	return Plugin_Handled;
}

public int resetItemMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	if ( !loringlib_IsValidClient ( param1 ) )
		delete menu;
		
	switch ( action ) {
		case MenuAction_Select: {
			static char item[4];
			menu.GetItem ( param2, item, sizeof ( item ) );
			int target = StringToInt ( item );
			if ( !loringlib_IsValidClient ( target ) ) {
				CLIENT_THIS_PLAYER_IS_INVALID_MSG ( param1 )
				return;
			}
				
			resetClientItemKeys ( target );
			CLIENT_RESET_DATA_BY_TARGET_MSG ( target )
			CLIENT_RESET_DATA_BY_ADMIN_MSG ( param1, target )
		}
	}
}

/**
 * 어드민 메뉴
 */
public Action clientCommandAdminCmd ( int client, int args ) {
	if ( !loringlib_IsValidClient ( client ) && client != 0 )
		return Plugin_Handled;
	
	if ( args < 2 ) {
		ReplyToCommand ( client, " \x04[Admin Command] \x01sm_zdadmin <#userid|name> <type> <amount>" );
		ReplyToCommand ( client, " \x04[Admin Command] \x01List of types: 1 = EXP, 2 = LEVEL, 3 = STAT POINTS, 4 = PLAYERPT" );
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
	
	if ( !isClientCharacterSelected ( target ) ) {
		ReplyToCommand ( client, " \x04[Admin Command] \x01타겟이 병과를 선택하지 않았습니다." );
		return Plugin_Handled;
	}
	
/*	if ( target == client ) {
		ReplyToCommand ( client, " \x04[Admin Command] \x01You can not give it myself." );
		return Plugin_Handled;
	}	*/
	
	char type[8];
	GetCmdArg ( 2, type, sizeof ( type ) );
	if ( !loringlib_IsStringNumeric ( type, sizeof ( type ) ) ) {
		ReplyToCommand ( client, " \x04[Admin Command] \x01List of types: 1 = EXP, 2 = LEVEL, 3 = STAT POINTS, 4 = PLAYERPT" );
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
	if ( amount_num < 1 ) {
		ReplyToCommand ( client, " \x04[Admin Command] \x01The value must be greater than 0." );
		return Plugin_Handled;
	}
	
	switch ( type_num ) {
		case 1: {
			PrintToChat ( target, " \x04[Admin Command] \x01You got %d EXPs from Administrator.", amount_num );
			ReplyToCommand ( client, " \x04[Admin Command] \x01Added %d EXPs on %N client.", amount_num, target );
			g_iExp[target] += amount_num;
			return Plugin_Handled;
		}
		
		case 2: {
			PrintToChat ( target, " \x04[Admin Command] \x01You got %d Levels from Administrator.", amount_num );
			ReplyToCommand ( client, " \x04[Admin Command] \x01Added %d Levels on %N client.", amount_num, target );
			g_iLevel[target] += amount_num;
			g_iStatPoint[target] += amount_num * REWARD_LEVELUP_SP;
			g_iPlayerPoint[target] += amount_num * REWARD_LEVELUP_PLAYERPT;
			return Plugin_Handled;
		}
		
		case 3: {
			PrintToChat ( target, " \x04[Admin Command] \x01You got %d Stat Points from Administrator.", amount_num );
			ReplyToCommand ( client, " \x04[Admin Command] \x01Added %d Stat Points on %N client.", amount_num, target );
			g_iStatPoint[target] += amount_num;
			return Plugin_Handled;
		}
		
		case 4: {
			PrintToChat ( target, " \x04[Admin Command] \x01You got %d 플레이어pt from Administrator.", amount_num );
			ReplyToCommand ( client, " \x04[Admin Command] \x01Added %d Playerpt on %N client.", amount_num, target );
			g_iPlayerPoint[target] += amount_num;
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
 * 아이템 주기
 */
public Action clientCommandGiveItem ( int client, int args ) {
	if ( !loringlib_IsValidClient ( client ) && client != 0 )
		return Plugin_Continue;
	
	if ( args < 2 ) {
		ReplyToCommand ( client, " \x04[Admin Command] \x01sm_zdgiveitem <#userid|name> <item> <amount>" );
		return Plugin_Handled;
	}
	
	static char playername[32];
	int target = -1;
	GetCmdArg ( 1, playername, sizeof ( playername ) );
	target = FindTarget ( client, playername, true, false );
	
	if ( !loringlib_IsValidClient ( target ) ) {
		ReplyToCommand ( client, " \x04[Admin Command] \x01Invalid client %d.", target );
		return Plugin_Handled;
	}
	
	if ( target == client ) {
		ReplyToCommand ( client, " \x04[Admin Command] \x01You can not give it myself." );
		return Plugin_Handled;
	}
	
	static char itemcode[16];
	GetCmdArg ( 2, itemcode, sizeof ( itemcode ) );
	if ( !loringlib_IsStringNumeric ( itemcode, sizeof ( itemcode ) ) ) {
		ReplyToCommand ( client, " \x04[Admin Command] \x01Only input a numeric." );
		return Plugin_Handled;
	}
	
	int code = StringToInt ( itemcode );
	if ( !isValidItem ( code ) ) {
		ReplyToCommand ( client, " \x04[Admin Command] \x01Invalid Item Code: %d", code );
		return Plugin_Handled;
	}
	
	static char amount[16];
	GetCmdArg ( 3, amount, sizeof ( amount ) );
	int amount_num = StringToInt ( amount );
	if ( amount_num < 1 ) {
		ReplyToCommand ( client, " \x04[Admin Command] \x01The value must be greater than 0." );
		return Plugin_Handled;
	}
	
	ReplyToCommand ( client, " \x04[Admin Command] \x01%N님에게 %s 아이템을 %d개 지급했습니다.", target, g_strItemName[code], amount_num );
	PrintToChat ( target, " \x04[Admin Command] \x01관리자로 부터 %s 아이템을 %d개 받았습니다.", g_strItemName[code], amount_num );
	
	g_iItem[code][target] += amount_num;
	return Plugin_Handled;
}