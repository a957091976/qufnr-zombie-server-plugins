#define PARTY_ALIVE_REWARD_MIN_PLAYERS		1		/**< 파티 생존 경험치 서버 최소 인원 default: 10 */
#define PARTY_ALIVE_REWARD_MIN_PARTYMEMBERS	1		/**< 파티 생존 경험치 파티 맴버 최소 인원 default: 3 */
#define PARTY_ALIVE_REWARD_MULTIPLE_3MEM	6.0		/**< 파티원 3명일 경우 파티 생존 경험치 곱 (좀비 수에 비례) */
#define PARTY_ALIVE_REWARD_MULTIPLE			10.0	/**< 파티원 3명 이상일 경우 파티 생존 경험치 곱 (좀비 수에 비례) */

#define PARTY_REWARD_KILL_BY_PARTY_MANAGER	2
#define PARTY_REWARD_ASSIST_BY_PARTY_MANAGER	2

#define	PARTY_MESSAGE_PREFIX	" \x04[Party]"
#define CLIENT_PARTY_IS_FULL(%1,%2)	PrintToChat ( %1, "%s \x01%s 님의 파티가 가득 차 있습니다.", PARTY_MESSAGE_PREFIX, %2 );
#define CLIENT_PARTY_JOIN_INVITE_MSG_TO_CLIENT(%1,%2)	PrintToChat ( %1, "%s \x01%N 님에게 파티 가입 요청을 보냈습니다.", PARTY_MESSAGE_PREFIX, %2 );
#define CLIENT_PARTY_JOIN_INVITE_MSG_TO_TARGET(%1,%2)	PrintToChat ( %1, "%s \x01%N 님이 귀하의 파티에 가입하고자 합니다.", PARTY_MESSAGE_PREFIX, %2 );
#define CLIENT_PARTY_TARGET_IS_INVALID(%1)				PrintToChat ( %1, "%s \x01유효하지 않은 클라이언트 입니다.", PARTY_MESSAGE_PREFIX );
#define CLIENT_PARTY_TARGET_IN_PARTY(%1,%2)				PrintToChat ( %1, "%s \x01%N 님은 이미 파티에 가입되어 있습니다.", PARTY_MESSAGE_PREFIX, %2 );
#define CLIENT_PARTY_JOIN_ACCEPT(%1,%2)	printToChatParty ( %1, "%s \x01%N 님이 파티에 참가했습니다.", PARTY_MESSAGE_PREFIX, %2 );
#define CLIENT_PARTY_JOIN_IGNORE(%1,%2)	PrintToChat ( %1, "%s \x01%N 님이 파티 가입을 거절했습니다.", PARTY_MESSAGE_PREFIX, %2 );
#define CLIENT_PARTY_DISCONN_PARTY_MANAGER(%1,%2)	PrintToChat ( %1, "%s \x01%s 파티의 파티장이 파티를 떠났습니다.", PARTY_MESSAGE_PREFIX, %2 );
#define CLIENT_PARTY_REWARD_EXP_GAIN(%1,%2)			PrintToChat ( %1, "%s \x01파티 경험치를 획득했습니다. \x05+%d EXP %s", PARTY_MESSAGE_PREFIX, %2, EXP_BOOST_SCALE > 1.0 ? "\x0EEvent↑" : "" );
#define CLIENT_PARTY_NO_PARTY(%1)		PrintToChat ( %1, "%s \x01귀하는 가입된 파티가 없습니다.", PARTY_MESSAGE_PREFIX );
#define CLIENT_PARTY_CHANGE_TITLE_FAIL(%1,%2)	PrintToChat ( %1, "%s \x01파티장만 사용할 수 있습니다. 귀하의 파티의 주인은 %N 님 입니다.", PARTY_MESSAGE_PREFIX, %2 );
#define CLIENT_PARTY_COMMAND_INVITE_FULL(%1)	PrintToChat ( %1, "%s \x01더 이상 초대할 수 없습니다.", PARTY_MESSAGE_PREFIX );

/**
 * 명령어
 */
void registerCmdsOnRegPartyCmds () {
	RegConsoleCmd ( "sm_partytitle", onCommandSetPartyTitle );
	RegConsoleCmd ( "sm_partykick", onCommandPartyKick );
	RegConsoleCmd ( "sm_partyinvite", onCommandPartyInvite );
}

/**
 * 파티원 모두에게 글로우
 * @note player_spawn
 */
void PARTY_SpawnOnSetupGlow ( int client ) {
	if ( loringlib_IsValidClient__PlayGame ( client ) &&
		ZR_IsClientHuman ( client ) &&
		( isInParty ( client ) && g_iPartyMembers[g_iPartyManager[client]] > 1 ) )
		CreateTimer ( 0.5, timerSetupPartyGlowPost, GetClientSerial ( client ), TIMER_FLAG_NO_MAPCHANGE );
}

public Action timerSetupPartyGlowPost ( Handle timer, any data ) {
	static int client;
	client = GetClientFromSerial ( data );
	
	if ( loringlib_IsValidClient__PlayGame ( client ) &&
		ZR_IsClientHuman ( client ) &&
		( isInParty ( client ) && g_iPartyMembers[g_iPartyManager[client]] > 1 ) ) {
		static char model[PLATFORM_MAX_PATH];
		GetClientModel ( client, model, sizeof ( model ) );
		
		static int skin;
		skin = CPS_SetSkin ( client, model, CPS_RENDER );
		if ( skin == -1 )
			return Plugin_Stop;
		
		if ( SDKHookEx ( skin, SDKHook_SetTransmit, onTransmitPartyGlow ) ) {
			loringlib_SetEntityOwner2 ( skin, client );
			
			//	Setup glow
			static int glowOffs;
			if ( ( glowOffs = GetEntSendPropOffs ( skin, "m_clrGlow" ) ) == -1 ) {
				SDKUnhook ( skin, SDKHook_SetTransmit, onTransmitPartyGlow );
				return Plugin_Stop;
			}
			
			SetEntProp ( skin, Prop_Send, "m_bShouldGlow", true, true );
			SetEntProp ( skin, Prop_Send, "m_nGlowStyle", 1 );
			SetEntPropFloat ( skin, Prop_Send, "m_flGlowMaxDist", 10000000.0 );
			
			static int colors[4];
			colors = { 151, 211, 211, 255 };
			SetEntData ( skin, glowOffs, colors[0], _, true );
			SetEntData ( skin, glowOffs + 1, colors[1], _, true );
			SetEntData ( skin, glowOffs + 2, colors[2], _, true );
			SetEntData ( skin, glowOffs + 3, colors[3], _, true );
		}
	}
	
	return Plugin_Stop;
}

public Action onTransmitPartyGlow ( int skin, int client ) {
	if ( !IsValidEdict ( skin ) || !loringlib_IsValidClient ( client ) )
		return Plugin_Handled;
	
	static int skinOwner;
	skinOwner = loringlib_GetEntityOwner2 ( skin );
	if ( loringlib_IsValidClient ( skinOwner ) ) {
		if ( g_iPartyManager[client] == g_iPartyManager[skinOwner] &&
			( isInParty ( client ) && isInParty ( skinOwner ) ) &&
			( ZR_IsClientHuman ( client ) && ZR_IsClientHuman ( skinOwner ) ) )
			return Plugin_Continue;
	}
	
	else {
		SDKUnhook ( skin, SDKHook_SetTransmit, onTransmitPartyGlow );
		return Plugin_Stop;
	}
	
	return Plugin_Handled;
}

/**
 * 파티장에게 텔레포트
 */
void PARTY_RoundStartOnTeleportToPartyManager () {
	CreateTimer ( 0.1, timerTeleportToPartyManagerPost, _, TIMER_FLAG_NO_MAPCHANGE );

}

public Action timerTeleportToPartyManagerPost ( Handle timer ) {
	float partyManagerOrigin[3];
	for ( int i = 1; i <= MaxClients; i ++ ) {
		if ( loringlib_IsValidClient__PlayGame ( i ) ) {
			if ( isInParty ( i ) ) {
				if ( i != g_iPartyManager[i] ) {
					if ( loringlib_IsValidClient__PlayGame ( g_iPartyManager[i] ) ) {
						GetClientAbsOrigin ( g_iPartyManager[i], partyManagerOrigin );
						TeleportEntity ( i, partyManagerOrigin, NULL_VECTOR, NULL_VECTOR );
						PrintToChat ( i, "%s \x01파티장 %N님 곁으로 이동합니다...", PARTY_MESSAGE_PREFIX, g_iPartyManager[i] );
					}
				}
			}
		}
	}
}

public Action onCommandSetPartyTitle ( int client, int args ) {
	if ( loringlib_IsValidClient ( client ) ) {
		if ( !isInParty ( client ) ) {
			CLIENT_PARTY_NO_PARTY ( client )
			return Plugin_Handled;
		}
		
		//	파티장이 아닐 경우
		if ( g_iPartyManager[client] != client ) {
			CLIENT_PARTY_CHANGE_TITLE_FAIL ( client, g_iPartyManager[client] )
			return Plugin_Handled;
		}
		
		if ( args < 1 ) {
			ReplyToCommand ( client, "%s \x01사용법: sm_partytitle <title>", PARTY_MESSAGE_PREFIX );
			return Plugin_Handled;
		}
		
		static char contents[32];
		GetCmdArg ( 1, contents, sizeof ( contents ) );
		
		if ( StrEqual ( contents, PARTY_NO_PARTY_NAME, false ) ) {
			ReplyToCommand ( client, "%s \x01잘못된 파티 명 입니다.", PARTY_MESSAGE_PREFIX );
			return Plugin_Handled;
		}
		
		ReplaceString ( contents, sizeof ( contents ), "\\", "" );
		ReplaceString ( contents, sizeof ( contents ), "%%", "" );
		
		Format ( g_strPartyName[client], sizeof ( g_strPartyName[] ), "%s", contents );
		ReplyToCommand ( client, "%s \x01파티 명을 %s (으)로 변경했습니다.", PARTY_MESSAGE_PREFIX, contents );
	}
	
	return Plugin_Handled;
}

public Action onCommandPartyKick ( int client, int args ) {
	if ( loringlib_IsValidClient ( client ) ) {
		if ( !isInParty ( client ) ) {
			CLIENT_PARTY_NO_PARTY ( client )
			return Plugin_Handled;
		}
		
		//	파티장이 아닐 경우
		if ( g_iPartyManager[client] != client ) {
			CLIENT_PARTY_CHANGE_TITLE_FAIL ( client, g_iPartyManager[client] )
			return Plugin_Handled;
		}
		
		static int members;
		static char buffer[256];
		Menu menu = new Menu ( commandPartyKickHandle );
		menu.SetTitle ( "%s\n\t파티 맴버 강퇴:\n　", MENU_TITLE );
		for ( int i = 0; i < g_iPartyMembers[g_iPartyManager[client]]; i ++ ) {
			members = StringToInt ( g_strPartyMembers[g_iPartyManager[client]][i][PARTY_MEMBER_INDEX] );
			Format ( buffer, sizeof ( buffer ), "%N %s", members, client == members ? "(YOU)" : "" );
			menu.AddItem ( "", buffer, client == members ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT );
		}
		
		menu.ExitBackButton = true;
		menu.Display ( client, MENU_TIME_FOREVER );
	}
	
	return Plugin_Handled;
}

public Action onCommandPartyInvite ( int client, int args ) {
	if ( loringlib_IsValidClient ( client ) ) {
		if ( !isInParty ( client ) ) {
			CLIENT_PARTY_NO_PARTY ( client )
			return Plugin_Handled;
		}
		
		//	파티장이 아닐 경우
		if ( g_iPartyManager[client] != client ) {
			CLIENT_PARTY_CHANGE_TITLE_FAIL ( client, g_iPartyManager[client] )
			return Plugin_Handled;
		}
		
		//	파티가 가득 찼을 경우
		if ( g_iPartyMembers[g_iPartyManager[client]] >= PARTY_MAX_MEMBERS ) {
			CLIENT_PARTY_COMMAND_INVITE_FULL ( client )
			return Plugin_Handled;
		}
		
		Menu menu = new Menu ( commandPartyInviteHandle );
		menu.SetTitle ( "%s\n\t파티 초대:\n　", MENU_TITLE );
		static char info[64], id[16];
		for ( int i = 1; i <= MaxClients; i ++ ) {
			if ( loringlib_IsValidClient ( i ) && !IsFakeClient ( i ) && i != client ) {
				if ( !isClientCharacterSelected ( i ) )
					continue;
				
				Format ( info, sizeof ( info ), "%N%s", i, isInParty ( i ) ? " - PARTY" : "" );
				Format ( id, sizeof ( id ), "%d", GetClientUserId ( i ) );
				menu.AddItem ( id, info, isInParty ( i ) ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT );
			}
		}
		menu.Display ( client, MENU_TIME_FOREVER );
	}
	
	return Plugin_Handled;
}

#define CLIENT_PARTY_TARGET_SEND_INVITE(%1,%2)		PrintToChat ( %1, "%s \x01%N님에게 초대를 보냈습니다.", PARTY_MESSAGE_PREFIX, %2 );

public int commandPartyInviteHandle ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			static char id[16];
			menu.GetItem ( param2, id, sizeof ( id ) );
			static int client;
			client = StringToInt ( id );
			client = GetClientOfUserId ( client );
			
			if ( !loringlib_IsValidClient ( client ) ) {
				CLIENT_PARTY_TARGET_IS_INVALID ( param1 )
				return;
			}
			
			if ( isInParty ( client ) ) {
				CLIENT_PARTY_TARGET_IN_PARTY ( param1, client )
				return;
			}
			
			sendInviteMenu ( client, param1 );
			CLIENT_PARTY_TARGET_SEND_INVITE ( param1, client )
		}
	}
}

void sendInviteMenu ( int client, int partyOwner ) {
	if ( !loringlib_IsValidClient ( partyOwner ) || !loringlib_IsValidClient ( client ) )
		return;
	
	if ( isInParty ( partyOwner ) ) {
		Menu menu = new Menu ( partyInviteMenuHandle );
		menu.SetTitle ( "%s\n%N님이 귀하를 파티에 초대했습니다.\n　", MENU_TITLE, partyOwner );
		
		static char ownerId[16];
		IntToString ( GetClientUserId ( partyOwner ), ownerId, sizeof ( ownerId ) );
		menu.AddItem ( ownerId, "수락" );
		menu.AddItem ( "", "거절" );
		menu.Display ( client, MENU_TIME_FOREVER );
	}
	
	else {
		PrintToChat ( client, "%s \x01만료됨.", PARTY_MESSAGE_PREFIX );
		return;
	}
}

//#define CLIENT_PARTY_INVITE_OWNER_ARE_NOT_IN_PARTY(%1,%2)		PrintToChat ( %1, "%s \x01%N님은 파티에 있지 않습니다.", PARTY_MESSAGE_PREFIX, %2 );
#define CLIENT_PARTY_INVITE_TARGET_ARE_IN_ANOTHER_PARTY(%1,%2)	PrintToChat ( %1, "%s \x01%N님이 파티 초대에 수락했지만, 이미 다른 파티에 들어가 있습니다.", PARTY_MESSAGE_PREFIX, %2 );
#define CLIENT_PARTY_INVITE_TARGET_FULL(%1)	PrintToChat ( %1, "%s \x01파티가 가득 찼습니다.", PARTY_MESSAGE_PREFIX );

public int partyInviteMenuHandle ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			switch ( param2 ) {
				case 0: {
					static char ownerId[16];
					menu.GetItem ( param2, ownerId, sizeof ( ownerId ) );
					static int partyOwner;
					partyOwner = StringToInt ( ownerId );
					partyOwner = GetClientOfUserId ( partyOwner );
					
					if ( !loringlib_IsValidClient ( partyOwner ) ) {
						PrintToChat ( param1, "%s \x01만료됨.", PARTY_MESSAGE_PREFIX );
						return;
					}
					
					//	초대한 사람이 파티에 없거나, 초대한 사람이 파티장이 아닐 경우
					if ( !isInParty ( partyOwner ) || g_iPartyManager[partyOwner] != partyOwner ) {
						PrintToChat ( param1, "%s \x01만료됨.", PARTY_MESSAGE_PREFIX );
						return;
					}
					
					//	초대 받은 사람이 파티에 이미 있을 경우
					if ( isInParty ( param1 ) ) {
						CLIENT_PARTY_INVITE_TARGET_ARE_IN_ANOTHER_PARTY ( partyOwner, param1 )
						return;
					}
					
					//	초대한 파티가 가득찼을 경우
					if ( g_iPartyMembers[partyOwner] >= PARTY_MAX_MEMBERS ) {
						CLIENT_PARTY_INVITE_TARGET_FULL ( param1 )
						return;
					}
					
					static char member_data[2][32];
					IntToString ( param1, member_data[PARTY_MEMBER_INDEX], sizeof ( member_data[] ) );
					GetClientName ( param1, member_data[PARTY_MEMBER_NICKNAME], sizeof ( member_data[] ) );
					
					for ( int i = 0; i < PARTY_MAX_MEMBERS; i ++ ) {
						if ( StrEqual ( g_strPartyMembers[partyOwner][i][PARTY_MEMBER_INDEX], "-1" ) ) {
							g_iPartyMembers[partyOwner] ++;
							g_strPartyMembers[partyOwner][i][PARTY_MEMBER_INDEX] = member_data[PARTY_MEMBER_INDEX];
							g_strPartyMembers[partyOwner][i][PARTY_MEMBER_NICKNAME] = member_data[PARTY_MEMBER_NICKNAME];
							
							g_bPartyJoined[param1] = true;
							g_iPartyManager[param1] = partyOwner;
							
							CLIENT_PARTY_JOIN_ACCEPT ( partyOwner, param1 )
							return;
						}
					}
					
					PrintToChat ( param1, "%s \x01실패", PARTY_MESSAGE_PREFIX );
				}
			}
		}
	}
}

public int commandPartyKickHandle ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			int memberClient = StringToInt ( g_strPartyMembers[g_iPartyManager[param1]][param2][PARTY_MEMBER_INDEX] );
			if ( loringlib_IsValidClient ( memberClient ) ) {
				PrintToChat ( param1, "%s \x01%N 님을 귀하의 파티에서 강퇴 했습니다.", PARTY_MESSAGE_PREFIX, memberClient );
				PrintToChat ( memberClient, "%s \x01귀하는 %N 님의 %s 파티에서 강퇴 당했습니다.", PARTY_MESSAGE_PREFIX, g_iPartyManager[memberClient], g_strPartyName[g_iPartyManager[memberClient]] );
				partyQuit ( memberClient );
			}
			
			else
				CLIENT_PARTY_TARGET_IS_INVALID ( param1 )
		}
		case MenuAction_Cancel: {
			if ( param2 == MenuCancel_ExitBack )
				clientCommandPartyMenu ( param1, 0 );
		}
	}
}

/**
 * 접속 시 파티 초기화
 */
public void connectPartyClearOptions ( int client ) {
	g_strPartyName[client] = PARTY_NO_PARTY_NAME;
	g_bPartyJoined[client] = false;
	for ( int i = 0; i < PARTY_MAX_MEMBERS; i ++ ) {
		g_strPartyMembers[client][i][PARTY_MEMBER_INDEX] = "-1";
		g_strPartyMembers[client][i][PARTY_MEMBER_NICKNAME] = PARTY_NO_MEMBER_NICKNAME;
	}
}

/**
 * 파티 초기화
 */
public void partyQuit ( int client ) {
	if ( !g_bPartyJoined[client] )
		return;
	
	//	파티 메니저일 경우
	int member = -1;
	if ( g_iPartyManager[client] == client ) {
		for ( int i = 0; i < PARTY_MAX_MEMBERS; i ++ ) {
			member = StringToInt ( g_strPartyMembers[g_iPartyManager[client]][i][PARTY_MEMBER_INDEX] );
			if ( member == -1 )
				continue;
			
			if ( !loringlib_IsValidClient ( member ) )
				continue;
			
			if ( member == client )
				continue;
			
			g_bPartyJoined[member] = false;
			g_iPartyManager[member] = -1;
			
			CLIENT_PARTY_DISCONN_PARTY_MANAGER ( member, g_strPartyName[client] )
		}
		
		g_bPartyJoined[client] = false;
		g_iPartyMembers[client] = 0;
		g_iPartyManager[client] = 0;
		g_strPartyName[client] = PARTY_NO_PARTY_NAME;
	}
	
	//	파티원일 경우
	else {
		for ( int i = 0; i < PARTY_MAX_MEMBERS; i ++ ) {
			member = StringToInt ( g_strPartyMembers[g_iPartyManager[client]][i][PARTY_MEMBER_INDEX] );
			if ( member == -1 )
				continue;
				
			if ( !loringlib_IsValidClient ( member ) )
				continue;
			
			if ( member == client ) {
				g_strPartyMembers[g_iPartyManager[member]][i][PARTY_MEMBER_INDEX] = "-1";
				g_strPartyMembers[g_iPartyManager[member]][i][PARTY_MEMBER_NICKNAME] = PARTY_NO_MEMBER_NICKNAME;
				g_iPartyMembers[g_iPartyManager[member]] --;
				
				g_bPartyJoined[member] = false;
				g_iPartyManager[member] = -1;
			}
		}
	}
	
	for ( int i = 0; i < PARTY_MAX_MEMBERS; i ++ ) {
		g_strPartyMembers[client][i][PARTY_MEMBER_INDEX] = "-1";
		g_strPartyMembers[client][i][PARTY_MEMBER_NICKNAME] = PARTY_NO_MEMBER_NICKNAME;
	}
}
 
/**
 * 파티 메뉴
 */
public Action clientCommandPartyMenu ( int client, int args ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Continue;
	
	if ( !loringlib_IsClientAdmin ( client ) )
		return Plugin_Handled;
	
	if ( !isClientCharacterSelected ( client ) ) {
		CLIENT_YOU_NO_SELECTE_CHARACTER_MSG ( client )
		return Plugin_Handled;
	}
	
	Menu menu = new Menu ( commandPartyMenuHandle );
	if ( g_bPartyJoined[client] ) {
		static char members[256];
		members = "파티원:\n";
		for ( int i = 0; i < PARTY_MAX_MEMBERS; i ++ )
			if ( !StrEqual ( g_strPartyMembers[g_iPartyManager[client]][i][PARTY_MEMBER_INDEX], "-1" ) )
				Format ( members, sizeof ( members ), "%s\t\t%s\n", members, g_strPartyMembers[g_iPartyManager[client]][i][PARTY_MEMBER_NICKNAME] );
		menu.SetTitle ( "%s\n\t파티 정보:\n\t파티 명: %s\n\t%s\n　", MENU_TITLE, g_strPartyName[g_iPartyManager[client]], members );
	} else {
		menu.SetTitle ( "%s\n\t가입된 파티가 없습니다.\n　", MENU_TITLE );
	}
	
	menu.AddItem ( "", "파티 만들기", g_bPartyJoined[client] ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT );
	menu.AddItem ( "", "파티 찾기" );
	menu.AddItem ( "", "파티 탈퇴\n-----------------", g_bPartyJoined[client] ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED );
	
	menu.AddItem ( "", "파티 맴버 강퇴", g_bPartyJoined[client] && g_iPartyManager[client] == client ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED );
	menu.AddItem ( "", "파티 이름 변경:\n\t\tsm_partytitle <title>", ITEMDRAW_DISABLED );
	
	menu.Display ( client, MENU_TIME_FOREVER );
	return Plugin_Handled;
}

public int commandPartyMenuHandle ( Menu menu, MenuAction action, int param1, int param2 ) {
	if ( !loringlib_IsValidClient ( param1 ) )
		delete menu;
		
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			switch ( param2 ) {
				case 0: partyMenuPartyCreate ( param1 );
				case 1: partyMenuPartyList ( param1 );
				case 2: partyQuit ( param1 );
				
				case 3: onCommandPartyKick ( param1, 0 );
				case 4: return;
			}
		}
	}
}

/**
 * 파티 만들기
 */
public void partyMenuPartyCreate ( int client ) {
	if ( g_bPartyJoined[client] )
		return;
		
	g_bPartyJoined[client] = true;
	g_iPartyManager[client] = client;
	g_iPartyMembers[client] = 1;
	
	Format ( g_strPartyName[client], sizeof ( g_strPartyName[] ), "%N의 파티", client );
	
	for ( int i = 0; i < PARTY_MAX_MEMBERS; i ++ ) {
		g_strPartyMembers[client][i][PARTY_MEMBER_INDEX] = "-1";
		g_strPartyMembers[client][i][PARTY_MEMBER_NICKNAME] = PARTY_NO_MEMBER_NICKNAME;
	}
	
	IntToString ( client, g_strPartyMembers[client][0][PARTY_MEMBER_INDEX], sizeof ( g_strPartyMembers[][][] ) );
	GetClientName ( client, g_strPartyMembers[client][0][PARTY_MEMBER_NICKNAME], sizeof ( g_strPartyMembers[][][] ) );
	
	clientCommandPartyMenu ( client, 0 );
}

/**
 * 만들어진 파티 목록
 */
public void partyMenuPartyList ( int client ) {
	Menu menu = new Menu ( partyListMenuHandle );
	menu.SetTitle ( "%s\n\t파티 찾기:\n　", MENU_TITLE );
	static char item[64];
	static char target[4];
	bool empty = true;
	for ( int i = 1; i <= MaxClients; i ++ ) {
		if ( !loringlib_IsValidClient ( i ) )
			continue;
		
		if ( StrEqual ( g_strPartyName[i], PARTY_NO_PARTY_NAME ) )
			continue;
		
		empty = false;
		
		Format ( item, sizeof ( item ), "%s [%d/%d]", g_strPartyName[i], g_iPartyMembers[i], PARTY_MAX_MEMBERS );
		Format ( target, sizeof ( target ), "%d", i );
		
		if ( g_bPartyJoined[client] )
			menu.AddItem ( target, item, ITEMDRAW_DISABLED );
		else
			menu.AddItem ( target, item, g_iPartyMembers[i] >= PARTY_MAX_MEMBERS ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT );
	}
	
	if ( empty )
		menu.AddItem ( "", "파티가 없습니다.", ITEMDRAW_DISABLED );
	
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int partyListMenuHandle ( Menu menu, MenuAction action, int param1, int param2 ) {
	if ( !loringlib_IsValidClient ( param1 ) )
		delete menu;
		
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			static char item[4];
			menu.GetItem ( param2, item, sizeof ( item ) );
			int target = StringToInt ( item );
			if ( !loringlib_IsValidClient ( target ) )
				return;
			
			if ( !isInParty ( target ) )
				return;
			
			if ( g_iPartyMembers[target] >= PARTY_MAX_MEMBERS ) {
				CLIENT_PARTY_IS_FULL ( param1, target )
				return;
			}
			
			CLIENT_PARTY_JOIN_INVITE_MSG_TO_CLIENT ( param1, target )
			CLIENT_PARTY_JOIN_INVITE_MSG_TO_TARGET ( target, param1 )
			partyJoinPartyAlert ( target, param1 );
		}
		
		case MenuAction_Cancel: {
			if ( param2 == MenuCancel_ExitBack )
				clientCommandPartyMenu ( param1, 0 );
		}
	}
}

/**
 * 파티 가입 요청 메뉴
 */
public void partyJoinPartyAlert ( int client, int target ) {
	static char clientIdx[16];
	Menu menu = new Menu ( joinPartyAlertMenuHandle );
	menu.SetTitle ( "%s\n\t파티 가입 요청:\n\t%N 님이 귀하의 파티에 가입하고자 합니다.", MENU_TITLE, target );
	
	static int userid;
	userid = GetClientUserId ( target );
	Format ( clientIdx, sizeof ( clientIdx ), "%d", userid );
	menu.AddItem ( clientIdx, "수락" );
	menu.AddItem ( clientIdx, "거절" );
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int joinPartyAlertMenuHandle ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			static char item[16];
			menu.GetItem ( param2, item, sizeof ( item ) );
			static int target;
			target = StringToInt ( item, sizeof ( item ) );
			target = GetClientOfUserId ( target );
			
			if ( !loringlib_IsValidClient ( target ) ) {
				CLIENT_PARTY_TARGET_IS_INVALID ( param1 )
				return;
			}
			
			//	이미 파티에 참가되어 있을 경우
			if ( isInParty ( target ) ) {
				CLIENT_PARTY_TARGET_IN_PARTY ( param1, target )
				return;
			}
			
			//	파티가 가득 찼을 경우
			if ( g_iPartyMembers[param1] <= PARTY_MAX_MEMBERS ) {
				PrintToChat ( param1, "%s \x01더 이상 파티에 인원을 초대할 수 없습니다.", PARTY_MESSAGE_PREFIX );
				PrintToChat ( target, "%s \x01파티가 가득 찼습니다.", PARTY_MESSAGE_PREFIX );
				return;
			}
			
			switch ( param2 ) {
				//	수락
				case 0: {
					static char member_data[2][32];
					IntToString ( target, member_data[PARTY_MEMBER_INDEX], sizeof ( member_data[] ) );
					GetClientName ( target, member_data[PARTY_MEMBER_NICKNAME], sizeof ( member_data[] ) );
					
					for ( int i = 0; i < PARTY_MAX_MEMBERS; i ++ ) {
						if ( StrEqual ( g_strPartyMembers[param1][i][PARTY_MEMBER_INDEX], "-1" ) ) {
							g_iPartyMembers[param1] ++;
							g_strPartyMembers[param1][g_iPartyMembers[param1] - 1][PARTY_MEMBER_INDEX] = member_data[PARTY_MEMBER_INDEX];
							g_strPartyMembers[param1][g_iPartyMembers[param1] - 1][PARTY_MEMBER_NICKNAME] = member_data[PARTY_MEMBER_NICKNAME];
							
							g_bPartyJoined[target] = true;
							g_iPartyManager[target] = param1;
							
							CLIENT_PARTY_JOIN_ACCEPT ( param1, target )
							return;
						}
					}
					
					PrintToChat ( param1, "%s \x01실패", PARTY_MESSAGE_PREFIX );
				}
				
				//	거절
				case 1: {
					CLIENT_PARTY_JOIN_IGNORE ( target, param1 )
				}
			}
		}
	}
}

/**
 * 적 처치 시 파티원에게 경험치 지급
 */
public void getRewardToPartyMembers ( int victim, int attacker, int assister ) {
	if ( !loringlib_IsValidClient ( victim ) || !loringlib_IsValidClient ( attacker ) )
		return;
		
	if ( GetClientTeam ( victim ) == GetClientTeam ( attacker ) )
		return;
		
	if ( !ZR_IsClientHuman ( attacker ) )
		return;
		
	//	파티에 있지 않을 경우
	if ( !g_bPartyJoined[attacker] )
		return;
		
	//	파티 맴버가 1명이거나 이하일 경우
	if ( g_iPartyMembers[g_iPartyManager[attacker]] <= 1 )
		return;
		
	int member = -1;
	int exp_rewards = 0;
	for ( int i = 0; i < g_iPartyMembers[g_iPartyManager[attacker]]; i ++ ) {
		member = StringToInt ( g_strPartyMembers[g_iPartyManager[attacker]][i][PARTY_MEMBER_INDEX] );
		//	유효하지 않은 파티원 일 경우
		if ( !loringlib_IsValidClient ( member ) )
			continue;
			
		//	파티원이 attacker일 경우
		if ( member == attacker )
			continue;
		
		//	파티원이 victim일 경우
		if ( victim == member )
			continue;
		
		//	파티장이 attacker일 경우
		if ( g_iPartyManager[attacker] == attacker )
			exp_rewards = RoundFloat ( ( GetRandomInt ( REWARD_EXP_MIN, REWARD_EXP_MAX ) / PARTY_REWARD_KILL_BY_PARTY_MANAGER ) * EXP_BOOST_SCALE );
		//	파티장이 assister일 경우
		else if ( g_iPartyManager[attacker] == assister )
			exp_rewards = RoundFloat ( ( GetRandomInt ( REWARD_ASSIST_EXP_MIN, REWARD_ASSIST_EXP_MAX ) / PARTY_REWARD_ASSIST_BY_PARTY_MANAGER ) * EXP_BOOST_SCALE );
		
		else {	//	그 외
			exp_rewards = RoundFloat ( GetRandomInt ( REWARD_PARTY_EXP_MIN, REWARD_PARTY_EXP_MAX ) * EXP_BOOST_SCALE );
		}
		
		CLIENT_PARTY_REWARD_EXP_GAIN ( member, exp_rewards )
		g_iExp[member] += exp_rewards;
		//	
	}
}

void roundEndOnPartyAllAliveReward () {
	if ( loringlib_GetClients () > PARTY_ALIVE_REWARD_MIN_PLAYERS ) {
		bool alive = true;
		int members = -1;
		for ( int target = 1; target <= MaxClients; target ++ ) {
			if ( loringlib_IsValidClient ( target ) && isInParty ( target ) ) {
				//	파티 맴버가 설정된 값 이거나, 이하일 경우
				if ( g_iPartyMembers[g_iPartyManager[target]] <= PARTY_ALIVE_REWARD_MIN_PARTYMEMBERS )
					continue;
				
				//	타겟의 파티 인원 전체를 구한다.
				for ( int i = 0; i < g_iPartyMembers[g_iPartyManager[target]]; i ++ ) {
					members = StringToInt ( g_strPartyMembers[g_iPartyManager[target]][i][PARTY_MEMBER_INDEX] );
					//	한명이라도 좀비가 있을 경우 false.
					if ( !ZR_IsClientHuman ( members ) )
						alive = false;
				}
				
				if ( alive ) {
					static float multiple;
					if ( g_iPartyMembers[g_iPartyManager[target]] <= 3 )
						multiple = PARTY_ALIVE_REWARD_MULTIPLE_3MEM;
					else
						multiple = PARTY_ALIVE_REWARD_MULTIPLE;
					static int temp;
					temp = RoundFloat ( ( loringlib_GetTeamCount ( CS_TEAM_T ) * multiple ) * EXP_BOOST_SCALE );
					PrintToChat ( target, "%s \x01파티 생존 보너스를 획득했습니다. \x05+%d EXP %s", PARTY_MESSAGE_PREFIX, temp, EXP_BOOST_SCALE > 1.0 ? "\x0EEvent↑" : "" );
					g_iExp[target] += temp;
				}
			}
		}
	}
}