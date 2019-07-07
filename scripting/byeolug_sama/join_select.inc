void JOINSELECT_registerCommands () {
	AddCommandListener ( onPlayerJoinTeam, "jointeam" );
	RegConsoleCmd ( "sm_select", onSelectCharacterMenu );
}

public Action onPlayerJoinTeam ( int client, const char[] cmd, int args ) {
	return Plugin_Handled;
}

public Action onSelectCharacterMenu ( int client, int args ) {
	if ( !isClientCharacterSelected ( client ) )
		openLoginMenu ( client );
	else {
		//	라운드 시작이 아닐 경우
		if ( !ZR_IsGameStart () ) {
			if ( !g_bSelectSlotCooldown[client] ) {
				g_bSelectSlotCooldown[client] = true;
			
				static int tempSlot;
				tempSlot = g_iSelectSlotIndex[client];
			
				clearSkillCooldownData ( client );
				partyQuit ( client );
				
				static bool saveSuccess;
				saveSuccess = inputClientKeyValues ( client, g_iSelectSlotIndex[client] );
				
				CreateTimer ( SLOT_SELECT_TIME, timerSelectSlotCooldown, GetClientSerial ( client ), TIMER_FLAG_NO_MAPCHANGE );
				
				DataPack data;
				CreateDataTimer ( 0.1, timerLogoutPost, data, TIMER_FLAG_NO_MAPCHANGE );
				data.WriteCell ( GetClientSerial ( client ) );
				data.WriteCell ( saveSuccess );
				data.WriteCell ( tempSlot );
			}
			else
				PrintToChat ( client, "%s \x01지금은 사용할 수 없습니다. 잠시 후 다시 시도해주세요.", MESSAGE_PREFIX );
		}
		else
			PrintToChat ( client, "%s \x01좀비가 선택되었거나, 게임이 진행중일 경우에는 사용할 수 없습니다.", MESSAGE_PREFIX );
	}
	
	return Plugin_Handled;
}

public Action timerSelectSlotCooldown ( Handle timer, any serial ) {
	static int client;
	client = GetClientFromSerial ( serial );
	
	if ( loringlib_IsValidClient ( client ) )
		g_bSelectSlotCooldown[client] = false;
	
	return Plugin_Handled;
}

public Action timerLogoutPost ( Handle timer, DataPack data ) {
	data.Reset ();
	
	static int client;
	static bool saveSuccess;
	static int tempSlot;
	client = GetClientFromSerial ( data.ReadCell () );
	saveSuccess = data.ReadCell ();
	tempSlot = data.ReadCell ();
	
	if ( loringlib_IsValidClient ( client ) ) {
		if ( saveSuccess )
			PrintToChat ( client, "%s \x01귀하의 데이터를 저장하는데 성공 했습니다. \x08(저장된 슬롯: %d Slot)", MESSAGE_PREFIX, tempSlot );
		else
			PrintToChat ( client, "%s \x01ERROR: \x07데이터를 저장하는데 문제가 생겼습니다. 관리자에게 문의하세요.", MESSAGE_PREFIX );
		
		PrintToServer ( "Client %N Logout (Status: %s)", client, saveSuccess ? "SUCCESS" : "FAILED" );
		
		g_iSelectSlotIndex[client] = SLOT_NO_SELECT;
		ChangeClientTeam ( client, CS_TEAM_SPECTATOR );
		openLoginMenu ( client );
	}
	
	return Plugin_Stop;
}

void JOINSELECT_ConnectOnDisplayLogin ( int client ) {
	if ( loringlib_IsValidClient ( client ) && !IsFakeClient ( client ) ) {
		g_iSelectSlotIndex[client] = SLOT_NO_SELECT;
		resetClientKeyData ( client );
		
		defineClientGlobalKeyValues ( client );
		g_iTotalLevels[client] = getClientAccountsTotalLevel ( client );
		
		if ( g_iSelectSlotIndex[client] == SLOT_NO_SELECT ) {
			ChangeClientTeam ( client, CS_TEAM_SPECTATOR );
			
			openLoginMenu ( client );
		}
	}
}

void openLoginMenu ( int client ) {
	static char temp[PLATFORM_MAX_PATH];
	static char steam[32];
	
	GetClientAuthId ( client, AuthId_SteamID64, steam, sizeof ( steam ) );
	BuildPath ( Path_SM, temp, PLATFORM_MAX_PATH, KEYVALUE_PATH, steam );
	
	KeyValues kv = new KeyValues ( "data" );
	
	Menu menu = new Menu ( loginMenuHandle );
	menu.SetTitle ( "%s\n\t캐릭터 선택:\n\t\tTotal Level: %d LV\n\t\tSlots: %d / %d\n　", MENU_TITLE, g_iTotalLevels[client], g_iUnlockSlots[client], MAX_SELECT_SLOT );
	
	//	데이터가 없을 경우
	if ( !kv.ImportFromFile ( temp ) ) {
		menu.AddItem ( "#NEW_DATA", "새로운 캐릭터 생성" );
		menu.Display ( client, MENU_TIME_FOREVER );
		
		delete kv;
		return;
	}
	
	//	데이터가 있을 경우
	static char menuItem[256], keyBuffer[64], menuData[16], humanClassName[32];
	for ( int i = 1; i <= MAX_SELECT_SLOT; i ++ ) {
		if ( i == 1 )
			Format ( keyBuffer, sizeof ( keyBuffer ), "%s", steam );
		else
			Format ( keyBuffer, sizeof ( keyBuffer ), "%s_slot%d", steam, i );
		
		if ( kv.JumpToKey ( keyBuffer ) ) {
			getClientClassName ( view_as<ClientClass> ( kv.GetNum ( "client_class_id" ) ), humanClassName, sizeof ( humanClassName ), false, true );
			if ( humanClassName[0] == EOS )
				Format ( humanClassName, sizeof ( humanClassName ), "병과 없음" );
			
			Format ( menuItem, sizeof ( menuItem ), "%s [Level.%d] %s", humanClassName, kv.GetNum ( "client_level" ), g_iUnlockSlots[client] < i ? " (잠긴 슬롯)" : "" );
			Format ( menuData, sizeof ( menuData ), "%d", i );
			menu.AddItem ( menuData, menuItem, g_iSelectSlotIndex[client] != SLOT_NO_SELECT ? ITEMDRAW_DISABLED : g_iUnlockSlots[client] >= i ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED );
			
			kv.Rewind ();
			
			PrintToServer ( "Client %N load Key name: %s", client, keyBuffer );
		}
		
		else {
			Format ( menuData, sizeof ( menuData ), "%d", i );
			menu.AddItem ( menuData, g_iUnlockSlots[client] < i ? "(잠긴 슬롯)" : "(없음)", g_iUnlockSlots[client] < i ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT );
		}
		
	}
	
	menu.ExitButton = false;
	menu.Display ( client, MENU_TIME_FOREVER );
	delete kv;
}

public int loginMenuHandle ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			static char menuItem[16];
			menu.GetItem ( param2, menuItem, sizeof ( menuItem ) );
			if ( StrEqual ( menuItem, "#NEW_DATA" ) ) {
				PrintToChat ( param1, "%s \x01새로운 데이터를 생성했습니다.", MESSAGE_PREFIX );
				g_iSelectSlotIndex[param1] = 1;
				defineClientKeyValues ( param1, g_iSelectSlotIndex[param1] );
				
				selectOnSpawnInit ( param1 );
				return;
			}
			
			static int slotIndex;
			slotIndex = StringToInt ( menuItem );
			//	슬롯 값이 0이거나 최대 슬롯 값 밖일 경우
			if ( slotIndex == SLOT_NO_SELECT || slotIndex > MAX_SELECT_SLOT ) {
				PrintToChat ( param1, "%s \x01ERROR: 잘못 된 값이 전달되었습니다.", MESSAGE_PREFIX );
				g_iSelectSlotIndex[param1] = SLOT_NO_SELECT;
				JOINSELECT_ConnectOnDisplayLogin ( param1 );
				
				return;
			}
			
			g_iSelectSlotIndex[param1] = slotIndex;
			defineClientKeyValues ( param1, g_iSelectSlotIndex[param1] );
			
		
			selectOnSpawnInit ( param1 );
		}
	}
}

void selectOnSpawnInit ( int client ) {
	PrintToServer ( "%N 클라이언트의 데이터를 불러오고 팀에 합류했습니다.", client );
	ChangeClientTeam ( client, CS_TEAM_CT );
	CreateTimer ( 0.1, timerCheckClientJoinTeamToDead, client );
}

public Action timerCheckClientJoinTeamToDead ( Handle timer, int client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Handled;
	
	if ( IsPlayerAlive ( client ) )
		return Plugin_Handled;
	
	if ( ZR_IsNemesisRound () ) {
		PrintToChat ( client, "[ZR] 네메시스 모드 상태에서는 스폰할 수 없습니다." );
		return Plugin_Handled;
	}
	
	CS_RespawnPlayer ( client );
	return Plugin_Handled;
}