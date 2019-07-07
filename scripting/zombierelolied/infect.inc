Handle g_hAlertMessageHandler = null;

/**
 * Create Hud sync handler.
 */
public void createAlertHudMsgSyncHandler ( bool create ) {
	if ( create )
		g_hAlertMessageHandler = CreateHudSynchronizer ();
	else
		for ( int i = 1; i <= MaxClients; i ++ )
			if ( loringlib_IsValidClient ( i ) )
				ClearSyncHud ( i, g_hAlertMessageHandler );
}

/**
 * Changing connect players.
 * @note Not use.
 */
public void changeConnectPlayerInfo () {
	for ( int client = 1; client <= MaxClients; client ++ ) {
		if ( !loringlib_IsValidClient__PlayGame ( client ) )
			continue;
			
		if ( !g_bGameStart ) {
			if ( !g_bClientGamePlayer[client] )	changeDefaultToHuman ( client );
			
			if ( g_bZombie[client] )		g_bZombie[client] = false;
			if ( g_bMotherZombie[client] )	g_bMotherZombie[client] = false;
		}
	}
}

/**
 * Round start to Infect starting.
 */
public void infectStart () {
	g_bGameEnd = false;
	g_iInfectCountdown = g_iConfInfectCountdown;
	g_bNemesisRound = false;
	
	//	Nemesis Mode
	if ( g_iConfNemesisEnable == 1 ) {
		if ( loringlib_GetClients () >= g_iConfNemesisPlayerCount ) {
			if ( loringlib_IsFloatProbability ( g_fConfNemesisRoundChance ) ) {
				g_iInfectCountdown = g_iConfNemesisInfectCountdown;
				g_bNemesisRound = true;
			}
			
			else {
				g_iInfectCountdown = g_iConfInfectCountdown;
				g_bNemesisRound = false;
			}
		}
	}
	
//	changeConnectPlayerInfo ();
	//	Reset zombie variables.
	for ( int client = 1; client <= MaxClients; client ++ ) {
		if ( !loringlib_IsValidClient ( client ) )
			continue;
			
		g_bZombie[client] = false;
		g_bMotherZombie[client] = false;
	}
	
	//	mp_freezetime이 적용된 서버에서는 그 시간 이 끝나면 적용됨.
	if ( FindConVar ( "mp_freezetime" ).IntValue > 0 ) {
		CreateTimer ( float ( FindConVar ( "mp_freezetime" ).IntValue ), timerInfectStartPost );
		return;
	}
	
	CreateTimer ( 0.0, timerInfectStartPost );
	return;
}

/**
 * Infect start check to post.
 */
public Action timerInfectStartPost ( Handle timer ) {
	//	타이머가 겹치지 않게 움직이고 있다면 삭제함.
	if ( g_hInfectStartCooldown != null ) {
		KillTimer ( g_hInfectStartCooldown );
		g_hInfectStartCooldown = null;
	}
	
	//	그래도 삭제되지 않았다면 디버그 메세지를 띄우고 라운드 다시 시작.
	if ( g_hInfectStartCooldown != null ) {
		PrintToChatAll ( "[ZR] Error: Infect start handler is NOT NULL." );
		ServerCommand ( "mp_restartgame 1" );
		return Plugin_Stop;
	}
	
	//	화이트리스트 맵일 경우?
	if ( g_bIsWhitelistLevel ) {
		g_bGameStart = true;
		g_bGameWaitPlayer = false;
		
		return Plugin_Stop;
	}
	
	g_hInfectStartCooldown = CreateTimer ( 1.0, timerInfectStart, _, TIMER_REPEAT );
	
	return Plugin_Handled;
}

/**
 * Infect start.
 */
public Action timerInfectStart ( Handle timer ) {
	
	//	플레이어가 한명이거나, 아무도 없을 경우
	if ( loringlib_GetPlayGameClients () <= 1 ) {
		//	죽은 사람이 한명 이상 있을 경우
		if ( loringlib_GetDeadClients () >= 1 ) {
			g_bGameWaitPlayer = false;
			
			//	게임 시작
			ServerCommand ( "mp_ignore_round_win_conditions 0" );
			CS_TerminateRound ( 5.0, CSRoundEnd_Draw, true );
			
			g_hInfectStartCooldown = null;
			return Plugin_Stop;
		}
		
		ServerCommand ( "mp_ignore_round_win_conditions 1" );
		g_bGameWaitPlayer = true;
		
		SetHudTextParams ( -1.0, 0.35, 1.1, 255, 255, 255, 255, 0, 0.0, 0.0, 0.0 );
		loringlib_ShowSyncHudTextToAll ( g_hAlertMessageHandler, "다른 플레이어를 기다리고 있습니다..." );
//		PrintCenterTextAll ( "<font class='fontSize-xl'>다른 플레이어를 기다리고 있습니다...</font>" );
		
		return Plugin_Handled;
	}
	
	//	플레이어를 기다리고 있을 경우
	if ( g_bGameWaitPlayer ) {
		//	팀에 플레이어가 들어왔을 경우
		if ( loringlib_GetTeamCount ( CS_TEAM_CT, false ) > 1 ||
			loringlib_GetTeamCount ( CS_TEAM_T, false ) > 0 ) {
			ServerCommand ( "mp_ignore_round_win_conditions 0" );
			
			g_bGameWaitPlayer = false;
			CS_TerminateRound ( 3.0, CSRoundEnd_Draw, true );
			
			SetHudTextParams ( -1.0, 0.35, 3.0, 255, 255, 255, 255, 0, 0.0, 0.0, 0.0 );
			loringlib_ShowSyncHudTextToAll ( g_hAlertMessageHandler, "게임이 시작됩니다." );
//			PrintCenterTextAll ( "<font class='fontSize-xl'>게임이 시작됩니다.</font>" );

			g_hInfectStartCooldown = null;
			return Plugin_Stop;
		}
	}
	
	//	카운트 다운
	if ( g_iInfectCountdown >= 0 ) {
		if ( g_bNemesisRound ) {
			SetHudTextParams ( -1.0, 0.35, 1.1, 255, 0, 0, 255, 0, 0.0, 0.0, 0.0 );
			loringlib_ShowSyncHudTextToAll ( g_hAlertMessageHandler, "%d초 후 네메시스가 출현합니다.", g_iInfectCountdown );
		}
		
		else {
			SetHudTextParams ( -1.0, 0.35, 1.1, 255, 255, 255, 255, 0, 0.0, 0.0, 0.0 );
			loringlib_ShowSyncHudTextToAll ( g_hAlertMessageHandler, "%d초 후 좀비가 출현합니다.", g_iInfectCountdown );
		}
//		PrintCenterTextAll ( "<font class='fontSize-xl'>%d초 후 좀비가 출현합니다.</font>", g_iInfectCountdown );
		
		Call_StartForward ( g_hForwardHandlers[ZR_FORWARD_ON_ZOMBIE_DEFINE_TICKS] );
		Call_PushCell ( g_iInfectCountdown );
		Call_Finish ();
		
		g_iInfectCountdown --;
	}
	
	//	10초 전 카운트 다운 알림
/*	switch ( g_iInfectCountdown ) {
		case 9: loringlib_EmitSoundToAll ( "music/zombiequfnr/10sec.mp3", SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL );
		case 4: loringlib_EmitSoundToAll ( "music/zombiequfnr/5sec.mp3", SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL );
		case 3: loringlib_EmitSoundToAll ( "music/zombiequfnr/4sec.mp3", SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL );
		case 2: loringlib_EmitSoundToAll ( "music/zombiequfnr/3sec.mp3", SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL );
		case 1: loringlib_EmitSoundToAll ( "music/zombiequfnr/2sec.mp3", SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL );
		case 0: loringlib_EmitSoundToAll ( "music/zombiequfnr/1sec.mp3", SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL );
	}	*/
	
	if ( g_iInfectCountdown <= 0 ) {
		ServerCommand ( "mp_ignore_round_win_conditions 0" );
		CreateTimer ( 1.5, timerFindInfectTarget );
		g_hInfectStartCooldown = null;
		
		return Plugin_Stop;
	}
	
	return Plugin_Handled;
}

public Action timerFindInfectTarget ( Handle timer ) {
	g_bGameStart = true;	//	GAME START!!!
	
	//	Find of zombies...
	//	아무도 없을 때
	if ( loringlib_GetPlayGameClients () < 1 ) {
		g_bGameStart = false;
		CS_TerminateRound ( 5.0, CSRoundEnd_Draw, true );
		
		return Plugin_Stop;
	}
	
	if ( g_bNemesisRound ) {
		turnIntoInfectNemesis ();
		
		Call_StartForward ( g_hForwardHandlers[ZR_FORWARD_ON_ZOMBIE_INFECT_STARTED] );
		Call_Finish ();
		
		uncheckSafeHuman ();
		
		return Plugin_Stop;
	}
	
	//	10명 이하
	if ( loringlib_GetPlayGameClients () < 10 )
		turnIntoInfectZombie ( 1 );
		
	//	10명 이상
	else
		turnIntoInfectZombie ( RoundToZero ( loringlib_GetPlayGameClients () / 5.5 ) );
	
	Call_StartForward ( g_hForwardHandlers[ZR_FORWARD_ON_ZOMBIE_INFECT_STARTED] );
	Call_Finish ();
	
	uncheckSafeHuman ();
	
	return Plugin_Stop;
}

/**
 * Change human to mother zombie.
 */
public void infectHumanToMotherZombie ( int client ) {
	if ( !loringlib_IsValidClient__PlayGame ( client ) )
		return;
	
	//	Respawn to zombie escape mod.
	if ( g_iConfModeType == ZR_GAMEMODE_ZOMBIE_ESCAPE )
		TeleportEntity ( client, g_fSpawnPosition[client], NULL_VECTOR, NULL_VECTOR );
	
	g_iZombieType[client] = ZR_MOTHER_ZOMBIE_CLASS;
	g_bZombie[client] = true;
	g_bMotherZombie[client] = true;

	//	Invalid zombie class data?
/*	if ( StrEqual ( g_strZombieModel[g_iZombieType[client]][client], "" ) ) {
		if ( g_iConfDebug ) {
			PrintToServer ( "[ZR] Zombie Class(ZR_MOTHER_ZOMBIE_CLASS) infect choose failure on %N. Try again...", client );
			LogError ( "[ZR] Zombie Class(ZR_MOTHER_ZOMBIE_CLASS) infect choose failure on %N. Try again...", client );
		}
		g_iClientZombieClass[client] = 0;
		CreateTimer ( 0.0, timerSetupMotherZombieData, client );
		CreateTimer ( 0.1, timerInfectHumanToMotherZombie, client );
		return;
	}*/

	if ( g_iConfDebug ) {
		PrintToServer ( "Called: infectHumanToMotherZombie ();" );
		PrintToServer ( "Target(Parameter): %N(%d)", client, client );
		PrintToServer ( "Model: \"%s\"", g_strZombieModel[g_iZombieType[client]][client] );
		PrintToServer ( "Model2: \"%s\"", g_strZombieArmsModel[client] );
	}
	
	//	Broadcast zombie type.
	if ( g_iConfGameCanUseZombieType == 1 &&
		g_iConfBroadcastMotherZombieType == 1 )
			PrintToChatAll ( "[ZR] %N님은 \x0F%s\x01 입니다!", client, g_strZombieClassname[g_iClientZombieClass[client]] );

	//	Change of team.
	CS_SwitchTeam ( client, CS_TEAM_T );
	loringlib_ShowShakeMessage ( client, 65.0, 1.5 );	//	Screen shake

	SetEntProp ( client, Prop_Send, "m_bHasHelmet", 0 );
	//	좀비모드
	if ( g_iConfModeType == ZR_GAMEMODE_ZOMBIE_MOD ) {
		SetEntityHealth ( client, g_iZombieHealth[ZR_MOTHER_ZOMBIE_CLASS][client] );
		loringlib_SetEntityMaxHealth ( client, GetClientHealth ( client ) );
	}
	
	//	좀비탈출
	if ( g_iConfModeType == ZR_GAMEMODE_ZOMBIE_ESCAPE ) {
		SetEntityHealth ( client, g_iZombieClassHealth[ZR_MOTHER_ZOMBIE_CLASS][g_iClientZombieClass[client]] );
		loringlib_SetEntityMaxHealth ( client, GetClientHealth ( client ) );
	}
	
//	loringlib_SetEntityLaggedmovement ( client, g_fZombieSpeed[ZR_MOTHER_ZOMBIE_CLASS][client] );
//	SetEntityGravity ( client, g_fZombieGravity[ZR_MOTHER_ZOMBIE_CLASS][client] );
		
	PrecacheModel ( g_strZombieModel[ZR_MOTHER_ZOMBIE_CLASS][client] );
	SetEntityModel ( client, g_strZombieModel[ZR_MOTHER_ZOMBIE_CLASS][client] );
	if ( GetEngineVersion () == Engine_CSGO )
		FPVMI_AddViewModelToClient ( client, "weapon_knife", PrecacheModel ( g_strZombieArmsModel[client] ) );

	zombieSoundsOnClientInfected ( client );
	zombieWeaponSlotClear ( client );
	int entity;
	float pos[3], eye[3];
	GetClientAbsOrigin ( client, pos );
	GetClientEyePosition ( client, eye );
	eye[2] -= 10.0;
	loringlib_CreateParticle ( client, entity, eye, "zombie_infection", false, 5.0 );
	loringlib_CreateParticle ( client, 0, pos, "zombie_infectedfull", false, 999.0, true );
	
	//	ZR_OnChangeZombie () Callback.
	Call_StartForward ( g_hForwardHandlers[ZR_FORWARD_ON_CHANGE_ZOMBIE] );
	Call_PushCell ( client );
	Call_PushCell ( g_iZombieType[client] );
	Call_PushCell ( g_bMotherZombie[client] );
	Call_Finish ();
	
	//	ZR_OnChangeMotherZombie () Callback.
	Call_StartForward ( g_hForwardHandlers[ZR_FORWARD_ON_CHANGE_MOTHER_ZOMBIE] );
	Call_PushCell ( client );
	Call_Finish ();
}

/**
 * Change human to zombie.
 */
public void infectHumanToZombie ( int client ) {
	if ( !loringlib_IsValidClient__PlayGame ( client ) )
		return;
	
	g_iZombieType[client] = ZR_NORMAL_ZOMBIE_CLASS;
	g_bZombie[client] = true;
	g_bMotherZombie[client] = false;
	
	//	Invalid zombie class data?
/*	if ( StrEqual ( g_strZombieModel[g_iZombieType[client]][client], "" ) ) {
		if ( g_iConfDebug ) {
			PrintToServer ( "[ZR] Zombie Class(ZR_NORMAL_ZOMBIE_CLASS) infect choose failure on %N. Try again...", client );
			LogError ( "[ZR] Zombie Class(ZR_NORMAL_ZOMBIE_CLASS) infect choose failure on %N. Try again...", client );
		}
		g_iClientZombieClass[client] = 0;
		CreateTimer ( 0.0, timerSetupNormalZombieData, client );
		CreateTimer ( 0.1, timerInfectHumanToZombie, client );
		return;
	}	*/

	if ( g_iConfDebug ) {
		PrintToServer ( "Called: infectHumanToZombie ();" );
		PrintToServer ( "Target(Parameter): %N(%d)", client, client );
		PrintToServer ( "Model: \"%s\"", g_strZombieModel[g_iZombieType[client]][client] );
		PrintToServer ( "Model2: \"%s\"", g_strZombieArmsModel[client] );
	}
	
	//	Change of team.
	CS_SwitchTeam ( client, CS_TEAM_T );
	loringlib_ShowShakeMessage ( client, 65.0, 1.5 );	//	Screen shake
	
	SetEntProp ( client, Prop_Send, "m_bHasHelmet", 0 );
	//	좀비모드
	if ( g_iConfModeType == ZR_GAMEMODE_ZOMBIE_MOD ) {
		SetEntityHealth ( client, g_iZombieHealth[ZR_NORMAL_ZOMBIE_CLASS][client] );
		loringlib_SetEntityMaxHealth ( client, GetClientHealth ( client ) );
	}
	
	//	좀비탈출
	if ( g_iConfModeType == ZR_GAMEMODE_ZOMBIE_ESCAPE ) {
		SetEntityHealth ( client, g_iZombieClassHealth[ZR_NORMAL_ZOMBIE_CLASS][g_iClientZombieClass[client]] );
		loringlib_SetEntityMaxHealth ( client, GetClientHealth ( client ) );
	}
	
	PrecacheModel ( g_strZombieModel[ZR_NORMAL_ZOMBIE_CLASS][client] );
	SetEntityModel ( client, g_strZombieModel[ZR_NORMAL_ZOMBIE_CLASS][client] );

	zombieSoundsOnClientInfected ( client );
	zombieWeaponSlotClear ( client );
	int entity;
	float pos[3];
	GetClientAbsOrigin ( client, pos );
	loringlib_CreateParticle ( client, entity, pos, "explosion_basic", true, 5.0 );
	loringlib_CreateParticle ( client, 0, pos, "zombie_infectedfull", false, 999.0, true );
	
	//	ZR_OnChangeZombie () Callback.
	Call_StartForward ( g_hForwardHandlers[ZR_FORWARD_ON_CHANGE_ZOMBIE] );
	Call_PushCell ( client );
	Call_PushCell ( g_iZombieType[client] );
	Call_PushCell ( g_bMotherZombie[client] );
	Call_Finish ();
}

/**
 * Change default human to human.
 */
public void changeDefaultToHuman ( int client ) {
	CreateTimer ( 0.1, timerDefaultToHumanPost, client, TIMER_FLAG_NO_MAPCHANGE );
}

public Action timerDefaultToHumanPost ( Handle timer, any client ) {
	if ( !loringlib_IsValidClient__PlayGame ( client ) )
		return;
	
	zombieUnhook ( client );	//	Zombie unhooking.
	
	//	화이트리스트 맵이 아닐 경우
	if ( !g_bIsWhitelistLevel ) {
		//	게임이 시작된 상태일 때
		if ( g_bGameStart && !g_bGameEnd ) {
			if ( !g_bNemesisRound ) {
				//	일반 좀비
				if ( g_bZombie[client] && !g_bMotherZombie[client] ) {
					CreateTimer ( 0.1, timerInfectHumanToZombie, client );
				//	PrintToChatAll ( "%d setting Zombie.", client );
					return;
				}
				
				//	숙주 좀비
				if ( g_bMotherZombie[client] ) {
					CreateTimer ( 0.1, timerInfectHumanToMotherZombie, client );
				//	PrintToChatAll ( "%d setting Mother Zombie.", client );
					return;
				}
				
				//	좀비가 아닌 상태
				if ( !g_bZombie[client] ) {
					//	게임이 시작된 상태에서 스폰 시 좀비로 바꾼다.
					CreateTimer ( 0.1, timerInfectHumanToZombie, client );
				//	PrintToChatAll ( "%d setting Not Zombie to Zombie.", client );
					return;
				}
			}
		}
	}
	
	if ( GetClientTeam ( client ) == CS_TEAM_T )
		CS_SwitchTeam ( client, CS_TEAM_CT );
	
	g_bZombie[client] = false;
	g_bMotherZombie[client] = false;
	g_bNemesis[client] = false;
	
	zmarketAutobuyWeapons ( client );
	
	CS_UpdateClientModel ( client );	//	Update client character model.
	FPVMI_RemoveViewModelToClient ( client, "weapon_knife" );		//	Remove the knife view model. (feat. FPVMI)
	//	인간 지구력 설정
	SetEntityHealth ( client, g_iConfHumanHealth );
	loringlib_SetEntityMaxHealth ( client, g_iConfHumanHealth );
	loringlib_SetEntityLaggedmovement ( client, g_fConfHumanSpeed );
	loringlib_SetEntityGravity ( client, g_fConfHumanGravity );
	
	CreateTimer ( 0.1, timerGiveGamestartItemsPost, client );
	
	Call_StartForward ( g_hForwardHandlers[ZR_FORWARD_ON_CHANGE_HUMAN] );
	Call_PushCell ( client );
	Call_Finish ();
}

/**
 * Clear client variables.
 */
public void clearClientVariables ( int client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return;

//	g_bZombie[client] = false;	g_bMotherZombie[client] = false;
	g_bClientGamePlayer[client] = false;

	//	리스폰 중 이라면 타이머를 삭제함.
	if ( g_hZombieRespawnCooldown[client] != null ) {
		KillTimer ( g_hZombieRespawnCooldown[client] );
		g_hZombieRespawnCooldown[client] = null;
	}
}

/**
 * Uncheck safe human.
 */
public void uncheckSafeHuman () {
	for ( int client = 1; client <= MaxClients; client ++ ) {
		if ( !loringlib_IsValidClient ( client ) )
			continue;
			
		//	숙주 좀비가 아닌 클라이언트는 safe human 변수를 false로 바꾼다.
		if ( !g_bMotherZombie[client] )
			g_bSafeHuman[client] = false;
	}
}

/**
 * End from game round.
 */
public void infectRoundEnd () {
	g_bGameStart = false;
	g_bGameEnd = true;
	g_bNemesisRound = false;
	
	//	라운드 종료 시 클라이언트 변수 초기화
	for ( int client = 1; client <= MaxClients; client ++ ) {
		if ( !loringlib_IsValidClient ( client ) )
			continue;
		
		clearClientVariables ( client );
		changeClientZombieClass_Post ( client, g_iClientZombieClass[client] );
	}
}

/**
 * 좀비로 감염 시 무기 슬롯 삭제
 */
public void zombieWeaponSlotClear ( int client ) {
	if ( SDKHookEx ( client, SDKHook_WeaponCanUse, onWeaponPickup ) )
		zombieUnhook ( client );
	
	if ( GetEngineVersion () == Engine_CSGO )
		FPVMI_AddViewModelToClient ( client, "weapon_knife", PrecacheModel ( g_strZombieArmsModel[client] ) );
	
	loringlib_KillWeaponSlot ( client );
	GivePlayerItem ( client, "weapon_knife" );
	
	loringlib_SetEntityAccount ( client, g_iConfGameZombieStartMoney );
	
	CreateTimer ( 0.1, timerZombieRehook, client );
}

public Action timerZombieRehook ( Handle timer, int client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Stop;
		
	loringlib_RemoveWeaponSlot ( client, CS_SLOT_PRIMARY );
	loringlib_RemoveWeaponSlot ( client, CS_SLOT_SECONDARY );
	loringlib_RemoveWeaponSlot ( client, CS_SLOT_GRENADE );
	loringlib_RemoveWeaponSlot ( client, CS_SLOT_C4 );
	loringlib_RemoveWeaponSlot ( client, CS_SLOT_MEDISHOT );
	loringlib_RemoveWeaponSlot ( client, CS_SLOT_MEDISHOT + 1 );	//	test
	
	SDKHook ( client, SDKHook_WeaponCanUse, onWeaponPickup );
	
	if ( g_bZombie[client] ) {
		if ( g_bMotherZombie[client] ) {
			loringlib_SetEntityLaggedmovement ( client, g_fZombieSpeed[ZR_MOTHER_ZOMBIE_CLASS][client] );
			SetEntityGravity ( client, g_fZombieGravity[ZR_MOTHER_ZOMBIE_CLASS][client] );
		}
		else {
			loringlib_SetEntityLaggedmovement ( client, g_fZombieSpeed[ZR_NORMAL_ZOMBIE_CLASS][client] );
			SetEntityGravity ( client, g_fZombieGravity[ZR_NORMAL_ZOMBIE_CLASS][client] );
		}
	}
	
	return Plugin_Stop;
}

/**
 * Zombie unhooking.
 */
public void zombieUnhook ( int client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return;
		
	SDKUnhook ( client, SDKHook_WeaponCanUse, onWeaponPickup );
}

/**
 * Change of zombie view model.
 */
public Action onClientCommandChangeViewmodel ( int client, int args ) {
	if ( !loringlib_IsClientAdmin ( client ) )
		return Plugin_Stop;
	
	if ( args < 1 ) {
		PrintToChat ( client, "[ZR] Usage: sm_changeviewmodel <1-%d|remove>", MAX_ZOMBIE_ARMS_MODEL );
		return Plugin_Stop;
	}
	
	static char num[16];
	GetCmdArg ( 1, num, sizeof ( num ) );
	if ( StrEqual ( num, "remove", false ) ) {
		PrintToChat ( client, "[ZR] Removed." );
		FPVMI_RemoveViewModelToClient ( client, "weapon_knife" );
		return Plugin_Stop;
	}
	
	if ( StringToInt ( num ) >= 1 && StringToInt ( num ) <= MAX_ZOMBIE_ARMS_MODEL ) {
		FPVMI_RemoveViewModelToClient ( client, "weapon_knife" );
		static char keyvaluepath[PLATFORM_MAX_PATH];
		BuildPath ( Path_SM, keyvaluepath, PLATFORM_MAX_PATH, "data/zombierelolied/zombie/class.txt" );
		KeyValues kv = new KeyValues ( "zombie_class" );
		if ( !kv.ImportFromFile ( keyvaluepath ) ) {
			delete kv;
			return Plugin_Stop;
		}
		
		static char model[256];
		static char jumpToKey[64];
		jumpToKey = g_strZombieClassname[g_iClientZombieClass[client]];
		static char key[128];
		Format ( key, sizeof ( key ), "zombie_zombie_arms_model_%s", num );
		kv.JumpToKey ( jumpToKey, false );
		kv.GetString ( key, model, sizeof ( model ) );
		int modelIndex = PrecacheModel ( model );
		if ( modelIndex == -1 ) {
			PrintToChat ( client, "[ZR] Error: Invalid arms model \"%s\"", model );
			delete kv;
			return Plugin_Stop;
		}
		FPVMI_AddViewModelToClient ( client, "weapon_knife", modelIndex );
		PrintToChat ( client, "[ZR] Changed." );
		PrintToChat ( client, "[ZR] Model Path: \x0E%s", model );
		
		delete kv;
		return Plugin_Stop;
	} 
	else	PrintToChat ( client, "[ZR] Usage: sm_changeviewmodel <1-%d|remove>" );
	return Plugin_Continue;
}

/**
 * Turn random players into the zombies.
 *
 * @param zombies		The amount of zombies.
 */
stock void turnIntoInfectZombie ( int zombies ) {
	int zombie = -1;
	int loop;
	
	while ( loop < zombies ) {
		zombie = loringlib_GetRandomAlive ( GetRandomInt ( 1, loringlib_GetPlayGameClients () ) );
		if ( !loringlib_IsValidClient__PlayGame ( zombie ) )
			continue;
		
		if ( g_bSafeHuman[zombie] )
			continue;
		
		if ( g_bZombie[zombie] ||
			g_bMotherZombie[zombie] )
			continue;
			
		infectHumanToMotherZombie ( zombie );
		g_bSafeHuman[zombie] = true;	//	Safe human checking.
		loop ++;
	}
}

/**
 * Turn random players into the nemesis.
 */
stock void turnIntoInfectNemesis () {
	int target = -1;
	int loop;
	while ( loop < 1 ) {
		target = loringlib_GetRandomAlive ( GetRandomInt ( 1, loringlib_GetPlayGameClients () ) );
		if ( !loringlib_IsValidClient__PlayGame ( target ) )
			continue;
		
		if ( g_bNemesis[target] )
			return;
		
		infectHumanToNemesis ( target );
		g_bNemesis[target] = true;
		loop ++;
	}

	Call_StartForward ( g_hForwardHandlers[ZR_FORWARD_ON_CHOOSE_NEMESIS] );
	Call_PushCell ( loop );
	Call_Finish ();
}