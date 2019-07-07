#define MAX_RANK_LEVEL		12

#define RANK_UP_SOUND	"qufnr/rank/rank_up.mp3"
#define RANK_UP_UPGRADE_TWO_SOUND	"qufnr/rank/rank_up_01.mp3"
#define RANK_UP_UPGRADE_THREE_SOUND	"qufnr/rank/rank_up_02.mp3"

/**
 * 랭크 표시
 */
char g_strRankName[MAX_RANK_LEVEL + 1][16] = {
	""			//	Unranked
	, "\x08D"
	, "\x08C"
	, "\x08B"
	, "\x08A"
	
	, "\x06D"
	, "\x06C"
	, "\x06B"
	, "\x06A"
	
	, "\x09D"
	, "\x09C"
	, "\x09B"
	, "\x09A"
};

/**
 * 랭크 목록
 */
static int g_iRankUpStats[MAX_RANK_LEVEL][2] = {
	{ 20, 500 }
	, { 40, 1000 }
	, { 65, 1600 }
	, { 100, 2500 }
	
	, { 150, 5000 }
	, { 190, 7000 }
	, { 240, 9000 }
	, { 300, 12000 }
	
	, { 500, 20000 }
	, { 580, 30000 }
	, { 700, 40000 }
	, { 1000, 80000 }
};

public void RANK_StartOnPrecacheAll () {
	PrecacheSound ( RANK_UP_SOUND );
	PrecacheSound ( RANK_UP_UPGRADE_TWO_SOUND );
	PrecacheSound ( RANK_UP_UPGRADE_THREE_SOUND );
}

/**
 * 자동 랭크 업 채크
 * @note PostThink에서
 */
public void RANK_FrameOnCheckRankUp ( int client ) {
	if ( loringlib_IsValidClient ( client ) ) {
		if ( g_iRankLevel[client] >= 0 && g_iRankLevel[client] < MAX_RANK_LEVEL ) {
			if ( isClientCharacterSelected ( client ) ) {
				if ( g_iSurviveCounts[client] >= g_iRankUpStats[g_iRankLevel[client]][0] && g_iTotalActPoints[client] >= g_iRankUpStats[g_iRankLevel[client]][1] ) {
					g_iRankLevel[client] ++;
					
					for ( int i = 1; i <= MaxClients; i ++ )
						if ( loringlib_IsValidClient ( i ) )
							loringlib_SayText2 ( client, i, true, " \x04[Rank] \x03%N\x01님이 %s\x01 랭크를 달성했습니다!", client, g_strRankName[g_iRankLevel[client]] );
				
					//	Rank up sounds
					if ( g_iRankLevel[client] == 5 )
						EmitSoundToAll ( RANK_UP_UPGRADE_TWO_SOUND );
					else if ( g_iRankLevel[client] == 9 )
						EmitSoundToAll ( RANK_UP_UPGRADE_THREE_SOUND );
					else
						EmitSoundToAll ( RANK_UP_SOUND );
				}
			}
		}
	}
}

public int ranklevelMenuHandle ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_Select: {
			switch ( param2 ) {
				case 1: {
					static char number[2][16];
					loringlib_NumberFormat ( g_iSurviveCounts[param1], number[0], sizeof ( number[] ) );
					loringlib_NumberFormat ( g_iTotalActPoints[param1], number[1], sizeof ( number[] ) );
					
					for ( int i = 1; i <= MaxClients; i ++ )
						if ( loringlib_IsValidClient ( i ) )
							loringlib_SayText2 ( param1, i, true, " \x04[Rank] \x03%N\x01님의 랭크 정보: \x07누적 생존\x01: %s 회, \x05누적 Act Points\x01: %s, Rank: %s", param1, number[0], number[1], g_strRankName[g_iRankLevel[param1]][0] == EOS ? "Unranked" : g_strRankName[g_iRankLevel[param1]] );
				}
				
				case 3: {
					g_iRankLevel[param1] ++;
					if ( g_iRankLevel[param1] > MAX_RANK_LEVEL )
						g_iRankLevel[param1] = MAX_RANK_LEVEL;
					
					clientCommandRankLevel ( param1, 0 );
				}
				case 4: {
					g_iRankLevel[param1] --;
					if ( g_iRankLevel[param1] < 0 )
						g_iRankLevel[param1] = 0;
						
					clientCommandRankLevel ( param1, 0 );
				}
			}
		}
	}
}

public Action clientCommandRankLevel ( int client, int args ) {
	if ( loringlib_IsValidClient ( client ) ) {
		Panel menu = new Panel ();
		
		static char title[256];
		Format ( title, sizeof ( title ), "%s\n\tRank:", MENU_TITLE );
		menu.SetTitle ( title );
		
		menu.DrawText ( " " );
		menu.DrawText ( "\t\t* 달성 목표는 누적으로 계산됩니다." );
		menu.DrawText ( "\t\t생존 횟수 | Act Points" );
		static char info[256];
		static char number[2][32];
		for ( int i = 0; i < MAX_RANK_LEVEL; i ++ ) {
			loringlib_NumberFormat ( g_iRankUpStats[i][0], number[0], sizeof ( number[] ) );
			loringlib_NumberFormat ( g_iRankUpStats[i][1], number[1], sizeof ( number[] ) );
			Format ( info, sizeof ( info ), "\t\t　%s　|　%s%s", number[0], number[1], i < g_iRankLevel[client] ? " - GET!" : "" );
			menu.DrawText ( info );
		}
		
		menu.DrawText ( " " );
		menu.DrawItem ( "Rank Info" );
		menu.DrawItem ( "Exit" );
		menu.DrawText ( " " );
		menu.DrawItem ( "↑", loringlib_IsClientAdmin ( client ) ? ITEMDRAW_DEFAULT : ITEMDRAW_SPACER );
		menu.DrawItem ( "↓", loringlib_IsClientAdmin ( client ) ? ITEMDRAW_DEFAULT : ITEMDRAW_SPACER );
		
		menu.Send ( client, ranklevelMenuHandle, MENU_TIME_FOREVER );
		
		delete menu;
	}
	
	return Plugin_Handled;
}