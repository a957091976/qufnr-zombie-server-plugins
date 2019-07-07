#include <loringlib>
#include <zombierelolied2018>

#define ZR_GAME_HUMAN_WIN_ENABLE_TICKTIME	180		/**< 좀비 섬멸 시 게임이 끝나는 틱 값 default: 90 */

float g_fGlobalTickTime = 0.0;

bool g_bCheckStart = false;
int g_iCheckTimeTicks = 0;

public void OnGameFrame () {
	if ( g_bCheckStart ) {
		if ( GetGameTime () >= g_fGlobalTickTime ) {
			g_fGlobalTickTime = GetGameTime () + 1.0;
			g_iCheckTimeTicks ++;
			if ( ZR_GAME_HUMAN_WIN_ENABLE_TICKTIME == g_iCheckTimeTicks )
				PrintToChatAll ( " \x07[ZR Round Manager] \x01지금 부터 좀비 진영이 모두 섬멸되면 인간 진영이 승리하게 됩니다!" );
		}
	}
}

public void OnPluginStart () {
	HookEvent ( "round_end", onRoundEnd );
}

/**
 * 좀비가 뽑히기 시작하면 시간을 계산한다.
 */
public void ZR_OnZombieInfectStarted () {
	if ( ZR_IsNemesisRound () ) {
		g_bCheckStart = false;
		g_iCheckTimeTicks = ZR_GAME_HUMAN_WIN_ENABLE_TICKTIME + 1;
		return;
	}
	
	g_bCheckStart = true;
	g_iCheckTimeTicks = 0;
	g_fGlobalTickTime = GetGameTime ();
}

public void onRoundEnd ( Event ev, const char[] name, bool dontBroadcast ) {
	g_bCheckStart = false;
}

public Action CS_OnTerminateRound ( float& delay, CSRoundEndReason& reason ) {
	if ( ZR_IsNemesisRound () )
		return Plugin_Continue;
	
	//	인간 승리일 경우
	if ( reason & CSRoundEnd_CTWin ) {
		//	설정된 틱수 보다 낮을 경우 라운드 종료를 무시한다.
		if ( g_iCheckTimeTicks < ZR_GAME_HUMAN_WIN_ENABLE_TICKTIME )
			return Plugin_Handled;
	}
	
	return Plugin_Continue;
}