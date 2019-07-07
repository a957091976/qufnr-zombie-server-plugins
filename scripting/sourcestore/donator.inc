void DONATOR_ConnectOnRainbowBodyHook ( int client ) {
	if ( client > 0 && client <= MaxClients )
		CreateTimer ( 0.5, timerRainbowBodyHook, client, TIMER_FLAG_NO_MAPCHANGE );
}

void DONATOR_DisconnectOnUnhook ( int client ) {
	SDKUnhook ( client, SDKHook_PostThinkPost, DONATOR_ThinkOnBodyColorChange );
}

public Action timerRainbowBodyHook ( Handle timer, any client ) {
	if ( loringlib_IsValidClient ( client ) ) {
	/*	if ( IsFakeClient ( client ) ) {
			g_bIsMvpPlus[client] = true;
			g_bToggleRainbow[client] = true;
			g_fRainbowTickTimes[client] = 1.0;
		}	*/
		
		if ( g_bIsMvpPlus[client] ) {
			SDKHook ( client, SDKHook_PostThinkPost, DONATOR_ThinkOnBodyColorChange );
		}
	}
	
	return Plugin_Handled;
}

public void DONATOR_ThinkOnBodyColorChange ( int client ) {
	if ( loringlib_IsValidClient ( client ) ) {
		if ( g_bIsMvpPlus[client] ) {
			if ( IsPlayerAlive ( client ) ) {
				if ( g_bToggleRainbow[client] ) {
					int rgba[4];
					rgba[0] = RoundFloat ( Sine ( ( GetEngineTime () / g_fRainbowTickTimes[client] ) * 4.0 ) ) * 127 + 128;
					rgba[1] = RoundFloat ( Sine ( ( GetEngineTime () / g_fRainbowTickTimes[client] ) * 4.0 + 2.0 ) ) * 127 + 128;
					rgba[2] = RoundFloat ( Sine ( ( GetEngineTime () / g_fRainbowTickTimes[client] ) * 4.0 + 4.0 ) ) * 127 + 128;
					rgba[3] = 255;
					
					loringlib_SetEntityRenderColorEx ( client, rgba );
				}
			}
		}
		
		else {
			SDKUnhook ( client, SDKHook_PostThinkPost, DONATOR_ThinkOnBodyColorChange );
			return;
		}
	}
}