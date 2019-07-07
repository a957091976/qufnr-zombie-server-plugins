/**
 * 접속 이벤트 같은거 할 때 쓰임.
 */
void JOIN_MODE_EVENT_OnDataLoadToJoinModeEventCheck ( int client ) {
	CreateTimer ( 1.0, timerJoinModeEventCheck, client );
}

public Action timerJoinModeEventCheck ( Handle timer, int client ) {
	if ( loringlib_IsValidClient ( client ) )
		JOIN_MODE_EVENT_OnConnectToJoinModeEvent ( client, JOIN_MODE_EVENT_SET_INDEX, JOIN_MODE_EVENT_ENABLE );
	
	return Plugin_Stop;
}

void JOIN_MODE_EVENT_OnConnectToJoinModeEvent ( int client, int index, int event ) {
	//	접속 이벤트가 비활성화되어 있을 경우
	if ( index == JOIN_MODE_EVENT_NONE || event == 0 ) {
		g_iGameJoinEventIndex[client] = 0;
		return;
	}
	
	//	활성화되어 있을 경우
	if ( event == 1 ) {
		if ( g_iGameJoinEventIndex[client] == 1 )
			return;
		
		g_iGameJoinEventIndex[client] = 1;
		switch ( index ) {
			//	스텟 초기화
			case JOIN_MODE_EVENT_RESETSTATS: {
				resetClientStatKeys ( client, false );
				PrintToChat ( client, "%s \x01스텟이 초기화되었습니다. !menu 에서 스텟을 다시 배분해주세요.", MESSAGE_PREFIX );
				return;
			}
			
			//	기술 초기화
			case JOIN_MODE_EVENT_RESETSKILLS: {
				resetClientClassKeys ( client, true, false );
				g_iPlayerPoint[client] = ( g_iLevel[client] - 1 );
				PrintToChat ( client, "%s \x01기술이 초기화되었습니다. !menu 에서 기술을 다시 배분해주세요.", MESSAGE_PREFIX );
				return;
			}
			
			//	캐릭터 슬롯 추가 이벤트
			case JOIN_MODE_EVENT_2019_CHARACTER_SLOT_EVENT: {
				g_iItem[22][client] += 10;	//	HP 물약 III
				g_iItem[17][client] += 10;	//	AP 물약 III
				PrintToChat ( client, "%s \x01물약 세트를 받았습니다.", MESSAGE_PREFIX );
				return;
			}
			
			//	2019 리코드 이벤트
			case JOIN_MODE_EVENT_2019_REMAKE_EVENT: {
				g_iItem[41][client] ++;
				PrintToChat ( client, "%s \x012019 리코드 보상을 받았습니다. \x10!menu\x01 후 인벤토리에서 확인하세요.", MESSAGE_PREFIX );
				
				//	VIP거나, SVIP일 경우
				if ( SourceStore_IsClientStoreMVP ( client ) || SourceStore_IsClientStoreMVPPlus ( client ) ) {
					g_iItem[42][client] ++;
				}
				return;
			}
			
			default: {
				g_iGameJoinEventIndex[client] = 0;
				return;
			}
		}
	}
	
}