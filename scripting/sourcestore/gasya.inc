#define GASYA_DROPTYPE_NORMAL					1	/**< 일반 가챠 */
#define GASYA_DROPTYPE_PREMIUM					2	/**< 플레티넘 가챠 */
#define GASYA_DROPTYPE_PREMIUM_LOSING_TICKET	3	/**< 플레티넘 가챠 (실패 확률) */
#define GASYA_DROPTYPE_KAKUTEI_SR				4	/**< 확정 SR 가챠 */
#define GASYA_DROPTYPE_KAKUTEI_SSR				5	/**< 확정 SSR 가챠 */

/**
 * @section 가챠 가격 (티켓)
 */
#define GASYA_DROP_NORMAL_PRICE		1	/**< 일반 가챠 뽑을 때 가격 */
#define GASYA_DROP_PREMIUM_PRICE	1	/**< 플레티넘 가챠 뽑을 때 가격 */
#define GASYA_TICKET_PREMIUM_PRICE	5	/**< 플레티넘 가챠 보석 가격 */
#define GASYA_KAKUTEI_SR_PRICE	20	/**< SR 확정 가챠 보석 가격 */
#define GASYA_KAKUTEI_SSR_PRICE	30	/**< SSR 확정 가챠 보석 가격 */

/**
 * @section 아이템 등장 확률 (노말 가샤)
 */
#define GASYA_N_ITEM_CHANCE_OTHER	50
#define GASYA_N_ITEM_CHANCE_NORMAL	35
#define GASYA_N_ITEM_CHANCE_RARE	15

/**
 * @section 아이템 등장 확률
 */
#define GASYA_ITEM_CHANCE_OTHER		45
#define GASYA_ITEM_CHANCE_RARE		35
#define GASYA_ITEM_CHANCE_SR		15
#define GASYA_ITEM_CHANCE_SSR		5

int g_iDropItemAmounts[ITEMRARE_SSR + 1] = { 0, ... };
int g_iDropItems[ITEMRARE_SSR + 1][MAXITEMS + 1];

static int g_iGasyaStartTicktime[MAXPLAYERS + 1] = { 0, ... };
static bool g_bIsGasyaStart[MAXPLAYERS + 1] = { false, ... };


char g_strRarityColors[6][16] = {
	""	//	none
	, "\x08"	//	normal
	, "\x05"	//	rare
	, "\x10"		//	sr
	, "\x0E"		//	ssr
	, "\x03"		//	event
};

char g_strRaritySounds[6][128] = {
/*	"ui/panorama/case_awarded_0_common_01.wav"
	, "ui/panorama/case_awarded_1_uncommon_01.wav"
	, "ui/panorama/case_awarded_2_rare_01.wav"
	, "ui/panorama/case_awarded_3_mythical_01.wav"
	, "ui/panorama/case_awarded_4_legendary_01.wav"
	, "ui/panorama/case_awarded_5_ancient_01.wav"	*/
	"ui/item_drop1_common.wav"
	, "ui/item_drop2_uncommon.wav"
	, "ui/item_drop3_rare.wav"
	, "ui/item_drop4_mythical.wav"
//	, "ui/item_drop5_legendary.wav"
	, "ui/item_drop6_ancient.wav"
	, "ui/item_drop6_ancient.wav"
};

#define GASYA_FAILED_SOUND	"survival/turret_death_01.wav"
#define GASYA_START_SOUND	"survival/buy_item_01.wav"

/**
 * 접속 때 뽑기 변수를 초기화합니다.
 *
 * @note OnClientPutInServer
 */
void GASYA_ConnectOnGasyaDataClear ( int client ) {
	if ( client >= 0 && client <= MaxClients ) {
		g_iGasyaStartTicktime[client] = 0;
		g_bIsGasyaStart[client] = false;
	}
}

/**
 * 아이템 데이터베이스를 불러온 후 드랍 아이템을 가져옵니다.
 */
void GASYA_ItemLoadOnDefineDropItems () {
	for ( int i = 0; i < sizeof ( g_iDropItemAmounts ); i ++ )
		g_iDropItemAmounts[i] = 0;
	
	PrintToServer ( "[Source Store] Define to Random Box Drop items:" );
	for ( int rarity = 1; rarity <= 4; rarity ++ ) {
		PrintToServer ( "[Source Store] -- Rarity: %d", rarity );
		for ( int item = 0; item < g_iDefineStoreItem; item ++ ) {
			if ( g_iItemRarity[item] == rarity &&
				( g_iItemRandomDropType[item] == GASYA_DROPTYPE_NORMAL ||
				g_iItemRandomDropType[item] == GASYA_DROPTYPE_PREMIUM ) ) {
				g_iDropItems[rarity][g_iDropItemAmounts[rarity]] = item;
				PrintToServer ( "[Source Store] Item Data: Index %d | Drop Type %d | Item %d | Item Name %s", g_iDropItemAmounts[rarity], g_iItemRandomDropType[item], g_iDropItems[rarity][g_iDropItemAmounts[rarity]], g_strItemName[item] );
				g_iDropItemAmounts[rarity] ++;
			}
		}
	}
}
 
/**
 * Gasya Menu
 */
public void storeGasyaMenu ( int client ) {
	Menu menu = new Menu ( storeGasyaMenuHandle );
	menu.SetTitle ( "%s\n\tPasta Ga$ya (Beta)\n　", g_strConfMenuTitle );
	
	menu.AddItem ( "#VIEW_CHANCE", "확률 보기\n　" );
	
	menu.AddItem ( "#BUY_TICKETS", "뽑기 티켓 구입" );
	
	char jewelshop[128];
	Format ( jewelshop, sizeof ( jewelshop ), "보석 상점 [보석: %d 개]\n　", g_iGasyaPoint[client] );
	menu.AddItem ( "#BUY_ITEMS", jewelshop );
	
	char buffer[256];
	Format ( buffer, sizeof ( buffer ), "일반 뽑기 [티켓: %d 장]", g_iTicket[TICKET_NORMAL][client] );
	menu.AddItem ( "1", buffer, g_iTicket[TICKET_NORMAL][client] >= GASYA_DROP_NORMAL_PRICE ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED );
	Format ( buffer, sizeof ( buffer ), "플레티넘 뽑기 [티켓: %d 장]", g_iTicket[TICKET_PREMIUM][client] );
	menu.AddItem ( "2", buffer, g_iTicket[TICKET_PREMIUM][client] >= GASYA_DROP_PREMIUM_PRICE ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED );
	
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int storeGasyaMenuHandle ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			if ( !loringlib_IsValidClient ( param1 ) )
				return;
			
			char item[32];
			menu.GetItem ( param2, item, sizeof ( item ) );
			if ( StrEqual ( item, "#VIEW_CHANCE" ) ) {
				viewGasyaChance ( param1 );
				return;
			}
			
			if ( StrEqual ( item, "#BUY_TICKETS" ) ) {
				buyGasyaTickets ( param1 );
				return;
			}
			
			if ( StrEqual ( item, "#BUY_ITEMS" ) ) {
				buyGasyaItems ( param1 );
				return;
			}
			
			int droptype = StringToInt ( item );
			
			switch ( droptype ) {
				case GASYA_DROPTYPE_NORMAL: {
					if ( g_bIsGasyaStart[param1] ) {
						PrintToChat ( param1, "%s 나중에 다시 시도해 주세요.", g_strConfChatPrefix );
						return;
					}
					
					if ( g_iTicket[TICKET_NORMAL][param1] < GASYA_DROP_NORMAL_PRICE ) {
						PrintToChat ( param1, "%s \x01자금이 부족합니다.", g_strConfChatPrefix );
						return;
					}
					
					g_iTicket[TICKET_NORMAL][param1] -= GASYA_DROP_NORMAL_PRICE;
				}
				
				case GASYA_DROPTYPE_PREMIUM: {
					if ( g_bIsGasyaStart[param1] ) {
						PrintToChat ( param1, "%s 나중에 다시 시도해 주세요.", g_strConfChatPrefix );
						return;
					}
					
					if ( g_iTicket[TICKET_PREMIUM][param1] < GASYA_DROP_PREMIUM_PRICE ) {
						PrintToChat ( param1, "%s \x01자금이 부족합니다.", g_strConfChatPrefix );
						return;
					}
					
					g_iTicket[TICKET_PREMIUM][param1] -= GASYA_DROP_PREMIUM_PRICE;
				}
			}
			
			g_bIsGasyaStart[param1] = true;
			g_iGasyaStartTicktime[param1] = 0;
			
			DataPack data;
			CreateDataTimer ( 0.0, timerGasyaStart, data, TIMER_FLAG_NO_MAPCHANGE );
			data.WriteCell ( param1 );
			data.WriteCell ( droptype );
			DataPack data2;
			CreateDataTimer ( 1.0, timerGasyaStart, data2, TIMER_FLAG_NO_MAPCHANGE | TIMER_REPEAT );	
			data2.WriteCell ( param1 );
			data2.WriteCell ( droptype );
		}
		
		case MenuAction_Cancel: {
			if ( param2 == MenuCancel_ExitBack )
				onClientCommandStore ( param1, 0 );
		}
	}
}

/**
 * 보석 상점
 */
void buyGasyaItems ( int client ) {
	Menu menu = new Menu ( buyGasyaItemHandle );
	menu.SetTitle ( "%s\n\t보석 상점\n\t\t보석: %d 개\n　", g_strConfMenuTitle, g_iGasyaPoint[client] );
	char items[256];
	Format ( items, sizeof ( items ), "플레티넘 뽑기 티켓 1장 [%d]", GASYA_TICKET_PREMIUM_PRICE );
	menu.AddItem ( "#TICKET_PREMIUM", items, g_iGasyaPoint[client] >= GASYA_TICKET_PREMIUM_PRICE ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED );
	Format ( items, sizeof ( items ), "SR 확정 뽑기 1회 [%d]", GASYA_KAKUTEI_SR_PRICE );
	menu.AddItem ( "#KAKUTEI_SR", items, g_iGasyaPoint[client] >= GASYA_KAKUTEI_SR_PRICE ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED );
	Format ( items, sizeof ( items ), "SSR 확정 뽑기 1회 [%d]", GASYA_KAKUTEI_SSR_PRICE );
	menu.AddItem ( "#KAKUTEI_SSR", items, g_iGasyaPoint[client] >= GASYA_KAKUTEI_SSR_PRICE ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED );
	
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int buyGasyaItemHandle ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			char options[64];
			int price;
			menu.GetItem ( param2, options, sizeof ( options ) );
			if ( StrEqual ( options, "#TICKET_PREMIUM" ) ) {
				price = GASYA_TICKET_PREMIUM_PRICE;
				if ( g_iGasyaPoint[param1] < price ) {
					PrintToChat ( param1, "%s 보석이 부족합니다.", g_strConfChatPrefix );
					return;
				}
				
				g_iGasyaPoint[param1] -= price;
				g_iTicket[TICKET_PREMIUM][param1] ++;
				PrintToChat ( param1, "%s 플레티넘 뽑기 티켓을 구입했습니다.", g_strConfChatPrefix );
			}
			
			else if ( StrEqual ( options, "#KAKUTEI_SR" ) ) {
				if ( g_bIsGasyaStart[param1] ) {
					PrintToChat ( param1, "%s 나중에 다시 시도해 주세요.", g_strConfChatPrefix );
					return;
				}
				
				price = GASYA_KAKUTEI_SR_PRICE;
				
				if ( g_iGasyaPoint[param1] < price ) {
					PrintToChat ( param1, "%s 보석이 부족합니다.", g_strConfChatPrefix );
					return;
				}
				
				g_iGasyaPoint[param1] -= price;
				
				g_bIsGasyaStart[param1] = true;
				g_iGasyaStartTicktime[param1] = 0;
				
				int droptype = GASYA_DROPTYPE_KAKUTEI_SR;
				
				DataPack data;
				CreateDataTimer ( 0.0, timerGasyaStart, data, TIMER_FLAG_NO_MAPCHANGE );
				data.WriteCell ( param1 );
				data.WriteCell ( droptype );
				DataPack data2;
				CreateDataTimer ( 1.0, timerGasyaStart, data2, TIMER_FLAG_NO_MAPCHANGE | TIMER_REPEAT );	
				data2.WriteCell ( param1 );
				data2.WriteCell ( droptype );
			}
			
			else if ( StrEqual ( options, "#KAKUTEI_SSR" ) ) {
				if ( g_bIsGasyaStart[param1] ) {
					PrintToChat ( param1, "%s 나중에 다시 시도해 주세요.", g_strConfChatPrefix );
					return;
				}
				
				price = GASYA_KAKUTEI_SR_PRICE;
				
				if ( g_iGasyaPoint[param1] < price ) {
					PrintToChat ( param1, "%s 보석이 부족합니다.", g_strConfChatPrefix );
					return;
				}
				
				g_iGasyaPoint[param1] -= price;
				
				g_bIsGasyaStart[param1] = true;
				g_iGasyaStartTicktime[param1] = 0;
				
				int droptype = GASYA_DROPTYPE_KAKUTEI_SSR;
				
				DataPack data;
				CreateDataTimer ( 0.0, timerGasyaStart, data, TIMER_FLAG_NO_MAPCHANGE );
				data.WriteCell ( param1 );
				data.WriteCell ( droptype );
				DataPack data2;
				CreateDataTimer ( 1.0, timerGasyaStart, data2, TIMER_FLAG_NO_MAPCHANGE | TIMER_REPEAT );	
				data2.WriteCell ( param1 );
				data2.WriteCell ( droptype );
			}
		}
		case MenuAction_Cancel: {
			if ( param2 == MenuCancel_ExitBack )
				storeGasyaMenu ( param1 );
		}
	}
}

/**
 * 뽑기 티켓 메뉴
 */
void buyGasyaTickets ( int client ) {
	Menu menu = new Menu ( buyGasyaTicketHandle );
	menu.SetTitle ( "%s\n\t뽑기 티켓 구입\n\t\t선택 시 바로 구입됩니다.\n　", g_strConfMenuTitle );
	char format[256];
	Format ( format, sizeof ( format ), "일반 뽑기 티켓 [%d]", g_iConfGasyaTicketPrice[TICKET_NORMAL] );
	menu.AddItem ( "0", format, g_iConfGasyaTicketPrice[TICKET_NORMAL] <= g_iBalance[client] ? ITEMDRAW_DEFAULT: ITEMDRAW_DISABLED );
	Format ( format, sizeof ( format ), "플레티넘 뽑기 티켓 [%d]", g_iConfGasyaTicketPrice[TICKET_PREMIUM] );
	menu.AddItem ( "1", format, g_iConfGasyaTicketPrice[TICKET_PREMIUM] <= g_iBalance[client] ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED );
	
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int buyGasyaTicketHandle ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			char item[16];
			menu.GetItem ( param2, item, sizeof ( item ) );
			int ticketType = StringToInt ( item );
			
			if ( ticketType < 0 || ticketType > 2 )
				return;
			
			if ( g_iConfGasyaTicketPrice[ticketType] > g_iBalance[param1] ) {
				PrintToChat ( param1, "%s 자금이 부족합니다.", g_strConfChatPrefix );
				return;
			}
			
			if ( ticketType == TICKET_NORMAL )
				PrintToChat ( param1, "%s 일반 뽑기 티켓을 구입했습니다.", g_strConfChatPrefix );
			else if ( ticketType == TICKET_PREMIUM )
				PrintToChat ( param1, "%s 플레티넘 뽑기 티켓을 구입했습니다.", g_strConfChatPrefix );
			PrecacheSound ( "qufnr/zombie_department/buy_item.mp3" );
			EmitSoundToClient ( param1, "qufnr/zombie_department/buy_item.mp3" );
			
			g_iBalance[param1] -= g_iConfGasyaTicketPrice[ticketType];
			g_iTicket[ticketType][param1] ++;
			
			buyGasyaTickets ( param1 );
		}
		case MenuAction_Cancel: {
			if ( param2 == MenuCancel_ExitBack )
				storeGasyaMenu ( param1 );
		}
	}
}

/**
 * 뽑기 시작 전 대기
 *
 */
public Action timerGasyaStart ( Handle timer, DataPack data ) {
	data.Reset ();
	int client = data.ReadCell ();
	int droptype = data.ReadCell ();

	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Stop;
	
	Panel panel = new Panel ();
	char panelTitle[256];
	Format ( panelTitle, sizeof ( panelTitle ), "%s\n\tPasta Ga$ya (Beta)\n　", g_strConfMenuTitle );
	panel.SetTitle ( panelTitle );
	switch ( g_iGasyaStartTicktime[client] ) {
		case 0:	{
			panel.DrawText ( "뽑는 중." );
			PrecacheSound ( GASYA_START_SOUND );
			EmitSoundToClient ( client, GASYA_START_SOUND );
		}
		case 1: panel.DrawText ( "뽑는 중.." );
		case 2: panel.DrawText ( "뽑는 중..." );
		case 3: panel.DrawText ( "뽑는 중...." );
		case 4: panel.DrawText ( "뽑는 중....." );
		default: panel.DrawText ( "뽑는 중......" );
	}
	
	g_iGasyaStartTicktime[client] ++;
	
	panel.Send ( client, noSelector, 1 );
	
	if ( g_iGasyaStartTicktime[client] >= 4 ) {
		startGasya ( client, droptype );
		g_iGasyaStartTicktime[client] = 0;
		return Plugin_Stop;
	}
	
	return Plugin_Continue;
}

public int noSelector ( Menu menu, MenuAction action, int param1, int param2 ) {
	return 0;
}

/**
 * 가챠 확률 보기
 */
void viewGasyaChance ( int client ) {
	Panel panel = new Panel ();
	char title[256];
	Format ( title, sizeof ( title ), "%s\n\t뽑기 확률???\n　", g_strConfMenuTitle );
	panel.SetTitle ( title );
	
	panel.DrawText ( "\t\t※ 일반 뽑기 에서는 꽝이 나올 확률이 있습니다!" );
	char format[256];
	
	panel.DrawText ( "\t일반 뽑기:" );
	Format ( format, sizeof ( format ), "\t\tMISC: %d%%", GASYA_N_ITEM_CHANCE_OTHER );
	panel.DrawText ( format );
	Format ( format, sizeof ( format ), "\t\tNORMAL: %d%%", GASYA_N_ITEM_CHANCE_NORMAL );
	panel.DrawText ( format );
	Format ( format, sizeof ( format ), "\t\tRARE: %d%%", GASYA_N_ITEM_CHANCE_RARE );
	panel.DrawText ( format );
	
	panel.DrawText ( "　" );
	panel.DrawText ( "\t플레티넘 뽑기:" );
	Format ( format, sizeof ( format ), "\t\tMISC: %d%%", GASYA_ITEM_CHANCE_OTHER );
	panel.DrawText ( format );
	Format ( format, sizeof ( format ), "\t\tRARE: %d%%", GASYA_ITEM_CHANCE_RARE );
	panel.DrawText ( format );
	Format ( format, sizeof ( format ), "\t\tSR: %d%%", GASYA_ITEM_CHANCE_SR );
	panel.DrawText ( format );
	Format ( format, sizeof ( format ), "\t\tSSR: %d%%", GASYA_ITEM_CHANCE_SSR );
	panel.DrawText ( format );
	
	panel.DrawItem ( "Back" );
	panel.DrawItem ( "Exit" );
	
	panel.Send ( client, viewGasyaChanceHandle, MENU_TIME_FOREVER );
}

public int viewGasyaChanceHandle ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			switch ( param2 ) {
				case 1:	storeGasyaMenu ( param1 );
			}
		}
	}
}

/**
 * 뽑기 시작
 *
 * @param client
 * @param droptype
 */
void startGasya ( int client, int droptype, int loop = 1 ) {
	g_bIsGasyaStart[client] = false;
	
	if ( droptype != GASYA_DROPTYPE_NORMAL &&
		droptype != GASYA_DROPTYPE_PREMIUM &&
		droptype != GASYA_DROPTYPE_PREMIUM_LOSING_TICKET &&
		droptype != GASYA_DROPTYPE_KAKUTEI_SR &&
		droptype != GASYA_DROPTYPE_KAKUTEI_SSR )
		return;
	
	char gasyaName[32];
	char noColorGasyaName[32];
	switch ( droptype ) {
		case GASYA_DROPTYPE_NORMAL:			strcopy ( gasyaName, sizeof ( gasyaName ), "\x0B일반 뽑기\x01" );
		case GASYA_DROPTYPE_PREMIUM, GASYA_DROPTYPE_PREMIUM_LOSING_TICKET:	strcopy ( gasyaName, sizeof ( gasyaName ), "\x03플레티넘 뽑기\x01" );
		case GASYA_DROPTYPE_KAKUTEI_SR:		strcopy ( gasyaName, sizeof ( gasyaName ), "\x10SR 확정 뽑기\x01" );
		case GASYA_DROPTYPE_KAKUTEI_SSR:	strcopy ( gasyaName, sizeof ( gasyaName ), "\x10SSR 확정 뽑기\x01" );
		default:							strcopy ( gasyaName, sizeof ( gasyaName ), "Unknown" );
	}
	
	switch ( droptype ) {
		case GASYA_DROPTYPE_NORMAL:			strcopy ( noColorGasyaName, sizeof ( noColorGasyaName ), "일반 뽑기" );
		case GASYA_DROPTYPE_PREMIUM, GASYA_DROPTYPE_PREMIUM_LOSING_TICKET:	strcopy ( noColorGasyaName, sizeof ( noColorGasyaName ), "플레티넘 뽑기" );
		case GASYA_DROPTYPE_KAKUTEI_SR:		strcopy ( noColorGasyaName, sizeof ( noColorGasyaName ), "SR 확정 뽑기" );
		case GASYA_DROPTYPE_KAKUTEI_SSR:	strcopy ( noColorGasyaName, sizeof ( noColorGasyaName ), "SSR 확정 뽑기" );
		default:							strcopy ( noColorGasyaName, sizeof ( noColorGasyaName ), "Unknown" );
	}
	
	int rarity = GASYA_GetDropRarity ( droptype );
	if ( rarity == -1 ) {
		PrintToChat ( client, " %s\x01 에러가 발생 했습니다. 관리자에게 문의하세요.", g_strConfChatPrefix );
		LogMessage ( "%N (#%d)님이 뽑기 도중 올바르지 않은 값이 전달되었습니다.", client, GetClientUserId ( client ) );
		return;
	}
	
//	PrintToChatAll ( "rarity: %d", rarity );
//	PrintToChatAll ( "drop_type: %d", droptype );
	
	if ( loop < 1 )
		loop = 1;
	
	int temp = 0;
	int findValue = 0, item = -1;
	char dropItemType[32];
	char dropItemRarityName[32];
	//	홧수 만큼 반복한다.
	while ( temp < loop ) {
		//	일반 드랍
		if ( rarity == 0 ) {
			GASYA_OnNormalDrop ( client, droptype );
				
			if ( droptype == GASYA_DROPTYPE_PREMIUM )
				g_iGasyaPoint[client] ++;
				
			temp ++;
		}
		
		//	아이템 드랍
		else {
			findValue = GetRandomInt ( 0, g_iDropItemAmounts[rarity] );
			item = g_iDropItems[rarity][findValue];
			
			if ( !loringlib_IsValidClient ( client ) || 
				( item < 0 || item > g_iDefineStoreItem ) ) {
				PrintToChat ( client, " %s 에러가 발생 했습니다. 관리자에게 문의하세요.", g_strConfChatPrefix );
				LogMessage ( "잘못된 아이템 데이터 또는 클라이언트 인덱스: CL %d | ID %d", client, item );
				return;
			}
		
			if ( item >= 0 &&
				g_iItemRarity[item] == rarity &&
				( g_iItemRandomDropType[item] > 0 && g_iItemRandomDropType[item] <= droptype ) ) {
				
				PrecacheSound ( g_strRaritySounds[rarity] );
				EmitSoundToAll ( g_strRaritySounds[rarity] );
				
				getItemEquipTypeName ( item, dropItemType, sizeof ( dropItemType ), true );
				getItemRareName ( item, dropItemRarityName, sizeof ( dropItemRarityName ) );
				PrintToChatAll ( " \x0E[Pasta Gasya]\x01 %N님이 %s 에서 %s%s [%s] \x01(%s) 아이템을 획득했습니다!", client, gasyaName, g_strRarityColors[rarity], g_strItemName[item], dropItemRarityName, dropItemType );
				
				g_iItem[client][item] ++;
				
				if ( droptype == GASYA_DROPTYPE_PREMIUM )
					g_iGasyaPoint[client] ++;
				
				drawGasyaDropItem ( client, item );
				
				Call_StartForward ( g_hForwardHandlerArray[FORWARD_ON_DROP_GASYA_ITEM] );
				Call_PushCell ( client );
				Call_PushCell ( item );
				Call_PushString ( noColorGasyaName );
				Call_PushString ( g_strItemName[item] );
				Call_PushString ( dropItemRarityName );
				Call_PushString ( dropItemType );
				Call_Finish ();
				
				temp ++;
			}
		}
	}
	
//	PrintToChatAll ( "find_value: %d", findValue );
//	PrintToChatAll ( "item_id: %d", item );
}

/**
 * 일반 드랍
 */
void GASYA_OnNormalDrop ( int client, int droptype ) {

	Panel panel = new Panel ();
	char title[256];
	Format ( title, sizeof ( title ), "%s\nPasta Ga$ya (Beta)\n　", g_strConfMenuTitle );
	panel.SetTitle ( title );
	
	char dropname[64];
	switch ( droptype ) {
		case GASYA_DROPTYPE_NORMAL: {
			if ( loringlib_IsIntegerChance ( 60 ) ) {
				PrecacheSound ( GASYA_FAILED_SOUND );
				EmitSoundToClient ( client, GASYA_FAILED_SOUND );
			
				PrintToChat ( client, " \x0E[Pasta Gasya]\x01 이런, 아무것도 얻을 수 없었습니다... 왜죠?" );
				
				strcopy ( dropname, sizeof ( dropname ), "#NONE" );
			}
			
			else {
				int random = GetRandomInt ( 300, 800 );
				PrintToChat ( client, " \x0E[Pasta Gasya]\x01 %d %s 을(를) 획득했습니다.", random, g_strConfMoneyName );
				g_iBalance[client] += random;
				
				Format ( dropname, sizeof ( dropname ), "%d %s", random, g_strConfMoneyName );
			}
		}
		
		case GASYA_DROPTYPE_PREMIUM: {
			int random;
			switch ( GetRandomInt ( 1, 3 ) ) {
				case 1: {
					random = GetRandomInt ( 2000, 3100 );
					PrintToChat ( client, " \x0E[Pasta Gasya]\x01 %d %s 을(를) 획득했습니다.", random, g_strConfMoneyName );
					g_iBalance[client] += random;
					
					Format ( dropname, sizeof ( dropname ), "%d %s", random, g_strConfMoneyName );
				}
				
				case 2: {
					random = GetRandomInt ( 1, 3 );
					PrintToChat ( client, " \x0E[Pasta Gasya]\x01 뽑기 보석 %d개 을(를) 획득했습니다.", random, g_strConfMoneyName );
					g_iGasyaPoint[client] += random;
					
					Format ( dropname, sizeof ( dropname ), "뽑기 보석 %d개", random );
				}
				
				case 3: {
					startGasya ( client, GASYA_DROPTYPE_PREMIUM_LOSING_TICKET, 1 );
					delete panel;
					return;
				}
			}
		}
	}
	
	char text[256];
	if ( StrEqual ( dropname, "#NONE" ) )
		Format ( text, sizeof ( text ), "\t\t꽝!!!" );
	else
		Format ( text, sizeof ( text ), "\t\t뽑기에서 %s 을(를) 뽑았습니다!", dropname );
	panel.DrawText ( text );
	panel.DrawItem ( "Back" );
	panel.DrawItem ( "Exit" );
	panel.Send ( client, viewGasyaChanceHandle, MENU_TIME_FOREVER );
}

void drawGasyaDropItem ( int client, int item ) {
	Panel panel = new Panel ();
	char title[256];
	Format ( title, sizeof ( title ), "%s\nPasta Ga$ya (Beta)\n　", g_strConfMenuTitle );
	panel.SetTitle ( title );
	
	char buffer[256];
	Format ( buffer, sizeof ( buffer ), "\t\t뽑기에서 %s 아이템을 뽑았습니다!", g_strItemName[item] );
	char lore[4][128];
	char itemtype[64];
	getItemEquipTypeName ( item, itemtype, sizeof ( itemtype ), true );
	Format ( lore[0], sizeof ( lore[] ), "\t\tID: %d", item );
	Format ( lore[1], sizeof ( lore[] ), "\t\t유형: %s", itemtype );
	Format ( lore[2], sizeof ( lore[] ), "\t\t희귀도: %s", g_strConfItemRarity[g_iItemRarity[item]] );
	if ( g_iItemPrice[item] > 0 )
		Format ( lore[3], sizeof ( lore[] ), "\t\t가격: %d %s", g_iItemPrice[item], g_strConfMoneyName );
	else
		Format ( lore[3], sizeof ( lore[] ), "\t\t가격: [무료 아이템]" );
	
	panel.DrawText ( buffer );
	panel.DrawText ( "　" );
	
	for ( int i = 0; i < sizeof ( lore ); i ++ )
		panel.DrawText ( lore[i] );
	
	panel.DrawItem ( "Back" );
	panel.DrawItem ( "Exit" );
	panel.Send ( client, viewGasyaChanceHandle, MENU_TIME_FOREVER );
}

/**
 * 드랍되는 희귀도를 구합니다.
 *
 * @param droptype
 * @return 아이템 희귀도
 * @error 유효하지 않는 드롭타입일 경우 0를 반환합니다.
 */
/*
stock int GASYA_GetDropRarity ( int droptype ) {
	switch ( droptype ) {
		case GASYA_DROPTYPE_NORMAL: {
			if ( loringlib_IsIntegerChance ( GASYA_N_ITEM_CHANCE_OTHER ) )
				return 0;
			else if ( loringlib_IsIntegerChance ( GASYA_N_ITEM_CHANCE_NORMAL ) )
				return ITEMRARE_NORMAL;
			else if ( loringlib_IsIntegerChance ( GASYA_N_ITEM_CHANCE_RARE ) )
				return ITEMRARE_RARE;
		}
		
		case GASYA_DROPTYPE_PREMIUM: {
			if ( loringlib_IsIntegerChance ( GASYA_ITEM_CHANCE_OTHER ) )
				return 0;
			else if ( loringlib_IsIntegerChance ( GASYA_ITEM_CHANCE_RARE ) )
				return ITEMRARE_RARE;
			else if ( loringlib_IsIntegerChance ( GASYA_ITEM_CHANCE_SR ) )
				return ITEMRARE_SR;
			else if ( loringlib_IsIntegerChance ( GASYA_ITEM_CHANCE_SSR ) )
				return ITEMRARE_SSR;
		}
		
		default: return 0;
	}
	
	return 0;
}	*/

stock int GASYA_GetDropRarity ( int droptype ) {
	int rarity = -1;
	while ( 1 ) {
		if ( droptype == GASYA_DROPTYPE_NORMAL ) {
			if ( loringlib_IsIntegerChance ( GASYA_N_ITEM_CHANCE_OTHER ) ) {
				rarity = 0;
				break;
			}
			else if ( loringlib_IsIntegerChance ( GASYA_N_ITEM_CHANCE_NORMAL ) ) {
				rarity = ITEMRARE_NORMAL;
				break;
			}
			else if ( loringlib_IsIntegerChance ( GASYA_N_ITEM_CHANCE_RARE ) ) {
				rarity = ITEMRARE_RARE;
				break;
			}
		}
		
		else if ( droptype == GASYA_DROPTYPE_PREMIUM ) {
			if ( loringlib_IsIntegerChance ( GASYA_ITEM_CHANCE_OTHER ) ) {
				rarity = 0;
				break;
			}
			else if ( loringlib_IsIntegerChance ( GASYA_ITEM_CHANCE_RARE ) ) {
				rarity = ITEMRARE_RARE;
				break;
			}
			else if ( loringlib_IsIntegerChance ( GASYA_ITEM_CHANCE_SR ) ) {
				rarity = ITEMRARE_SR;
				break;
			}
			else if ( loringlib_IsIntegerChance ( GASYA_ITEM_CHANCE_SSR ) ) {
				rarity = ITEMRARE_SSR;
				break;
			}
		}
		
		//	플레티넘 가챠 실패 시
		else if ( droptype == GASYA_DROPTYPE_PREMIUM_LOSING_TICKET ) {
			if ( loringlib_IsIntegerChance ( 80 ) )
				rarity = ITEMRARE_NORMAL;
			else
				rarity = ITEMRARE_RARE;
			break;
		}
		
		//	확정 SR
		else if ( droptype == GASYA_DROPTYPE_KAKUTEI_SR ) {
			rarity = ITEMRARE_SR;
			break;
		}
		
		//	확정 SSR
		else if ( droptype == GASYA_DROPTYPE_KAKUTEI_SSR ) {
			rarity = ITEMRARE_SSR;
			break;
		}
		
		else {
			if ( loringlib_IsIntegerChance ( GASYA_N_ITEM_CHANCE_OTHER ) ) {
				rarity = 0;
				break;
			}
			else if ( loringlib_IsIntegerChance ( GASYA_N_ITEM_CHANCE_NORMAL ) ) {
				rarity = ITEMRARE_NORMAL;
				break;
			}
			else if ( loringlib_IsIntegerChance ( GASYA_N_ITEM_CHANCE_RARE ) ) {
				rarity = ITEMRARE_RARE;
				break;
			}
		}
	}
	
	return rarity;
}