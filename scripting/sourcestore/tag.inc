public Action onClientCommandSay ( int client, int args ) {
	if ( !g_bUseTag[client] )
		return Plugin_Continue;
		
	char chat[192], tagTemp[16];
	GetCmdArgString ( chat, 191 );
	StripQuotes ( chat );
	
	Format ( tagTemp, sizeof ( tagTemp ), chat );
	for ( int i = 0; i < g_iDefineStoreItem; i ++ ) {
		if ( g_iItemType[i] != ITEMTYPE_TAGTITLE )
			continue;
		
		if ( StrEqual ( g_strItemName[i], tagTemp, false ) ) {
			PrintToChat ( client, "%s \x01%s 은(는) 사용할 수 없는 칭호 명 입니다.", g_strConfChatPrefix, chat );
			return Plugin_Handled;
		}
	}
	
	Menu menu = new Menu ( useTagAcceptHandler );
	menu.SetTitle ( "%s\n\t\"%s\"(으)로 태그를 설정할까요?\n\t한글은 최대 5자, 영어는 최대 16자 까지 가능합니다.\n　", g_strConfMenuTitle, tagTemp );
	menu.AddItem ( tagTemp, "Yes" );
	menu.AddItem ( "", "Retry" );
	menu.AddItem ( "", "Cancel" );
	
	menu.ExitButton = false;
	menu.Display ( client, MENU_TIME_FOREVER );
	
	return Plugin_Handled;
}

public int useTagAcceptHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_Select: {
			char tagName[16];
			menu.GetItem ( param2, tagName, sizeof ( tagName ) );
			switch ( param2 ) {
				case 0: {
					if ( g_iItem[param1][g_iTempTagItem[param1]] < 1 ) {
						PrintToChat ( param1, "%s \x01아이템 데이터에 문제가 있습니다. 관리자에게 문의하세요.", g_strConfChatPrefix );
						g_bUseTag[param1] = false;
						
						return;
					}
					
					g_iItem[param1][g_iTempTagItem[param1]] --;
					Format ( g_strEquipItemName[param1][ITEMTYPE_TAGTITLE], 63, tagName );
					PrintToChat ( param1, "%s \x01태그를 \"%s\"(으)로 설정했습니다.", g_strConfChatPrefix,  g_strEquipItemName[param1][ITEMTYPE_TAGTITLE] );
					g_bUseTag[param1] = false;
				}
				
				case 1: {
					PrintToChat ( param1, "%s \x01다시 입력하세요.", g_strConfChatPrefix );
					g_bUseTag[param1] = true;
				}
				
				case 2: {
					PrintToChat ( param1, "%s \x01취소했습니다.", g_strConfChatPrefix );
					g_bUseTag[param1] = false;
				}
			}
		}
	}
}