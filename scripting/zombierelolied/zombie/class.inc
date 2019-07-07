#define MAX_ZOMBIE_CLASS		255 + 1
#define MAX_ZOMBIE_MODEL		2
#define MAX_ZOMBIE_ARMS_MODEL	9
int g_iTotalZombieClass = 0;

int g_iZombieClassHealth[2][MAX_ZOMBIE_CLASS];
int g_iZombieClassHealthMath[2][MAX_ZOMBIE_CLASS];
int g_iZombieClassRechargeHealthScale[2][MAX_ZOMBIE_CLASS];
float g_fZombieClassSpeed[2][MAX_ZOMBIE_CLASS];
float g_fZombieClassGravity[2][MAX_ZOMBIE_CLASS];
float g_fZombieClassKnockbackScale[2][MAX_ZOMBIE_CLASS];
float g_fZombieClassRechargeHealthTime[2][MAX_ZOMBIE_CLASS];
char g_strZombieClassname[MAX_ZOMBIE_CLASS][32];
char g_strZombieClassDescription[MAX_ZOMBIE_CLASS][256];
char g_strZombieClassMotherZombieModels[MAX_ZOMBIE_CLASS][MAX_ZOMBIE_MODEL][256];
char g_strZombieClassZombieModels[MAX_ZOMBIE_CLASS][MAX_ZOMBIE_MODEL][256];
char g_strZombieClassZombieArmsModel[MAX_ZOMBIE_CLASS][MAX_ZOMBIE_ARMS_MODEL][256];

bool g_bViewMode[MAXPLAYERS + 1] = { false, ... };

int g_iNemesisHealth;
int g_iNemesisHealthMath;
int g_iNemesisRechargeHealthScale;
float g_fNemesisSpeed;
float g_fNemesisGravity;
float g_fNemesisKnockbackScale;
float g_fNemesisRechargeHealthTime;
float g_fNemesisLJScale;
float g_fNemesisLJCooldown;
char g_strNemesisModel[256];
char g_strNemesisArmsModel[256];

public void onLoadNemesisDB () {
	static char path[PLATFORM_MAX_PATH];
	BuildPath ( Path_SM, path, PLATFORM_MAX_PATH, "data/zombierelolied/zombie/nemesis.txt" );
	KeyValues kv = new KeyValues ( "zombie_nemesis" );
	if ( kv.ImportFromFile ( path ) ) {
		kv.GotoFirstSubKey ();
		static char section[32];
		do {
			kv.GetSectionName ( section, sizeof ( section ) );
			if ( StrEqual ( section, "nemesis" ) ) {
				g_iNemesisHealth = kv.GetNum ( "nemesis_health" );
				g_iNemesisHealthMath = kv.GetNum ( "nemesis_health_math" );
				g_iNemesisRechargeHealthScale = kv.GetNum ( "nemesis_recharge_health_scale" );
				
				g_fNemesisLJScale = kv.GetFloat ( "nemesis_longjump_scale" );
				g_fNemesisLJCooldown = kv.GetFloat ( "nemesis_longjump_cooldown" );
				g_fNemesisSpeed = kv.GetFloat ( "nemesis_speed" );
				g_fNemesisGravity = kv.GetFloat ( "nemesis_gravity" );
				g_fNemesisKnockbackScale = kv.GetFloat ( "nemesis_knockback" );
				g_fNemesisRechargeHealthTime = kv.GetFloat ( "nemesis_recharge_health_time" );
				
				kv.GetString ( "nemesis_model", g_strNemesisModel, sizeof ( g_strNemesisModel ) );
				kv.GetString ( "nemesis_arms_model", g_strNemesisArmsModel, sizeof ( g_strNemesisArmsModel ) );
			}
		} while ( kv.GotoNextKey () );
		delete kv;
	} else
		delete kv;
}

public void onLoadZombieClassDB () {
	onLoadNemesisDB ();

	g_iTotalZombieClass = 0;
	
	static char path[PLATFORM_MAX_PATH];
	BuildPath ( Path_SM, path, PLATFORM_MAX_PATH, "data/zombierelolied/zombie/class.txt" );
	KeyValues kv = new KeyValues ( "zombie_class" );
	if ( kv.ImportFromFile ( path ) ) {
		kv.GotoFirstSubKey ();
		do {
			kv.GetSectionName ( g_strZombieClassname[g_iTotalZombieClass], 31 );
			kv.GetString ( "zombie_description", g_strZombieClassDescription[g_iTotalZombieClass], 255 );
			kv.GetString ( "zombie_mother_zombie_model_1", g_strZombieClassMotherZombieModels[g_iTotalZombieClass][0], 255 );
			kv.GetString ( "zombie_mother_zombie_model_2", g_strZombieClassMotherZombieModels[g_iTotalZombieClass][1], 255 );
			kv.GetString ( "zombie_zombie_model_1", g_strZombieClassZombieModels[g_iTotalZombieClass][0], 255 );
			kv.GetString ( "zombie_zombie_model_2", g_strZombieClassZombieModels[g_iTotalZombieClass][1], 255 );
			static char key[64];
			for ( int i = 0; i < MAX_ZOMBIE_ARMS_MODEL; i ++ ) {
				Format ( key, sizeof ( key ), "zombie_zombie_arms_model_%d", i + 1 );
				kv.GetString ( key, g_strZombieClassZombieArmsModel[g_iTotalZombieClass][i], 255 );
			}
			
			g_iZombieClassHealth[ZR_NORMAL_ZOMBIE_CLASS][g_iTotalZombieClass] = kv.GetNum ( "zombie_health" );
			g_iZombieClassHealth[ZR_MOTHER_ZOMBIE_CLASS][g_iTotalZombieClass] = kv.GetNum ( "zombie_health_ex" );
			g_iZombieClassHealthMath[ZR_NORMAL_ZOMBIE_CLASS][g_iTotalZombieClass] = kv.GetNum ( "zombie_health_math" );
			g_iZombieClassHealthMath[ZR_MOTHER_ZOMBIE_CLASS][g_iTotalZombieClass] = kv.GetNum ( "zombie_health_math_ex" );
			g_iZombieClassRechargeHealthScale[ZR_NORMAL_ZOMBIE_CLASS][g_iTotalZombieClass] = kv.GetNum ( "zombie_health_recharge_scale" );
			g_iZombieClassRechargeHealthScale[ZR_MOTHER_ZOMBIE_CLASS][g_iTotalZombieClass] = kv.GetNum ( "zombie_health_recharge_scale_ex" );
			g_fZombieClassSpeed[ZR_NORMAL_ZOMBIE_CLASS][g_iTotalZombieClass] = kv.GetFloat ( "zombie_speed" );
			g_fZombieClassSpeed[ZR_MOTHER_ZOMBIE_CLASS][g_iTotalZombieClass] = kv.GetFloat ( "zombie_speed_ex" );
			g_fZombieClassGravity[ZR_NORMAL_ZOMBIE_CLASS][g_iTotalZombieClass] = kv.GetFloat ( "zombie_gravity" );
			g_fZombieClassGravity[ZR_MOTHER_ZOMBIE_CLASS][g_iTotalZombieClass] = kv.GetFloat ( "zombie_gravity_ex" );
			g_fZombieClassKnockbackScale[ZR_NORMAL_ZOMBIE_CLASS][g_iTotalZombieClass] = kv.GetFloat ( "zombie_knockback_scale" );
			g_fZombieClassKnockbackScale[ZR_MOTHER_ZOMBIE_CLASS][g_iTotalZombieClass] = kv.GetFloat ( "zombie_knockback_scale_ex" );
			g_fZombieClassRechargeHealthTime[ZR_NORMAL_ZOMBIE_CLASS][g_iTotalZombieClass] = kv.GetFloat ( "zombie_health_recharge_time" );
			g_fZombieClassRechargeHealthTime[ZR_MOTHER_ZOMBIE_CLASS][g_iTotalZombieClass] = kv.GetFloat ( "zombie_health_recharge_time_ex" );
			g_iTotalZombieClass ++;
			
			//	overflow?
			if ( g_iTotalZombieClass > MAX_ZOMBIE_CLASS ) {
				SetFailState ( "[ZR] Overflowed." );
				delete kv;
			}
		}
		
		while ( kv.GotoNextKey () );
		delete kv;
	} else
		delete kv;
}

/**
 * 클라이언트가 인간으로 스폰 시 좀비 클래스 Numeric 변수를 설정합니다.
 */
public void spawnToSetupZombieStats ( int client ) {
	if ( g_bZombie[client] )
		return;
	
	//	Normal zombie.
	g_iZombieHealth[ZR_NORMAL_ZOMBIE_CLASS][client] = ( g_iZombieClassHealth[ZR_NORMAL_ZOMBIE_CLASS][g_iClientZombieClass[client]] + g_iZombieClassHealthMath[ZR_NORMAL_ZOMBIE_CLASS][g_iClientZombieClass[client]] * loringlib_GetPlayGameClients () );
	g_fZombieSpeed[ZR_NORMAL_ZOMBIE_CLASS][client] = g_fZombieClassSpeed[ZR_NORMAL_ZOMBIE_CLASS][g_iClientZombieClass[client]];
	g_fZombieGravity[ZR_NORMAL_ZOMBIE_CLASS][client] = g_fZombieClassGravity[ZR_NORMAL_ZOMBIE_CLASS][g_iClientZombieClass[client]];
	g_fZombieKnockbackScale[ZR_NORMAL_ZOMBIE_CLASS][client] = g_fZombieClassKnockbackScale[ZR_NORMAL_ZOMBIE_CLASS][g_iClientZombieClass[client]];
	
	//	Mother zombie.
	g_iZombieHealth[ZR_MOTHER_ZOMBIE_CLASS][client] = ( g_iZombieClassHealth[ZR_MOTHER_ZOMBIE_CLASS][g_iClientZombieClass[client]] + g_iZombieClassHealthMath[ZR_NORMAL_ZOMBIE_CLASS][g_iClientZombieClass[client]] * loringlib_GetPlayGameClients () );
	g_fZombieSpeed[ZR_MOTHER_ZOMBIE_CLASS][client] = g_fZombieClassSpeed[ZR_MOTHER_ZOMBIE_CLASS][g_iClientZombieClass[client]];
	g_fZombieGravity[ZR_MOTHER_ZOMBIE_CLASS][client] = g_fZombieClassGravity[ZR_MOTHER_ZOMBIE_CLASS][g_iClientZombieClass[client]];
	g_fZombieKnockbackScale[ZR_MOTHER_ZOMBIE_CLASS][client] = g_fZombieClassKnockbackScale[ZR_MOTHER_ZOMBIE_CLASS][g_iClientZombieClass[client]];
}
 
/**
 * 클라이언트가 접속 시 좀비 클래스에 맞게 변수를 설정합니다.
 */
public void connectToSetupZombieData ( int client ) {
	CreateTimer ( 0.0, timerSetupNormalZombieData, client );
	CreateTimer ( 0.0, timerSetupMotherZombieData, client );
}

public Action timerSetupNormalZombieData ( Handle timer, int client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Handled;
	
	//	Set client zombie values.
	int randomModel = GetRandomInt ( 0, ( MAX_ZOMBIE_MODEL - 1 ) );
	int randomArmsModel = GetRandomInt ( 0, (MAX_ZOMBIE_ARMS_MODEL - 1 ) );
	
	//	Normal zombie.
	g_iZombieHealth[ZR_NORMAL_ZOMBIE_CLASS][client] = ( g_iZombieClassHealth[ZR_NORMAL_ZOMBIE_CLASS][g_iClientZombieClass[client]] + g_iZombieClassHealthMath[ZR_NORMAL_ZOMBIE_CLASS][g_iClientZombieClass[client]] * loringlib_GetPlayGameClients () );
	g_fZombieSpeed[ZR_NORMAL_ZOMBIE_CLASS][client] = g_fZombieClassSpeed[ZR_NORMAL_ZOMBIE_CLASS][g_iClientZombieClass[client]];
	g_fZombieGravity[ZR_NORMAL_ZOMBIE_CLASS][client] = g_fZombieClassGravity[ZR_NORMAL_ZOMBIE_CLASS][g_iClientZombieClass[client]];
	g_fZombieKnockbackScale[ZR_NORMAL_ZOMBIE_CLASS][client] = g_fZombieClassKnockbackScale[ZR_NORMAL_ZOMBIE_CLASS][g_iClientZombieClass[client]];
	
	g_strZombieModel[ZR_NORMAL_ZOMBIE_CLASS][client] = g_strZombieClassZombieModels[g_iClientZombieClass[client]][randomModel];
	g_strZombieArmsModel[client] = g_strZombieClassZombieArmsModel[g_iClientZombieClass[client]][randomArmsModel];
	
	if ( StrEqual ( g_strZombieModel[ZR_NORMAL_ZOMBIE_CLASS][client], "" ) ) {
		if ( g_iConfDebug ) {
			PrintToServer ( "[ZR] Zombie Class(ZR_NORMAL_ZOMBIE_CLASS) input failure on %N. Try again...", client );
			LogError ( "[ZR] Zombie Class(ZR_NORMAL_ZOMBIE_CLASS) input failure on %N. Try again...", client );
		}
		g_iClientZombieClass[client] = 0;
		CreateTimer ( 0.1, timerSetupNormalZombieData, client );
		return Plugin_Handled;
	}
	
	if ( g_iConfDebug )
		PrintToServer ( "[ZR] Zombie Class(ZR_NORMAL_ZOMBIE_CLASS) input successful on %N.", client );
	
	return Plugin_Handled;
}

public Action timerSetupMotherZombieData ( Handle timer, int client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Handled;
		
	//	Set client zombie values.
	int randomModel = GetRandomInt ( 0, ( MAX_ZOMBIE_MODEL - 1 ) );
	
	//	Mother zombie.
	g_iZombieHealth[ZR_MOTHER_ZOMBIE_CLASS][client] = ( g_iZombieClassHealth[ZR_MOTHER_ZOMBIE_CLASS][g_iClientZombieClass[client]] + g_iZombieClassHealthMath[ZR_NORMAL_ZOMBIE_CLASS][g_iClientZombieClass[client]] * loringlib_GetPlayGameClients () );
	g_fZombieSpeed[ZR_MOTHER_ZOMBIE_CLASS][client] = g_fZombieClassSpeed[ZR_MOTHER_ZOMBIE_CLASS][g_iClientZombieClass[client]];
	g_fZombieGravity[ZR_MOTHER_ZOMBIE_CLASS][client] = g_fZombieClassGravity[ZR_MOTHER_ZOMBIE_CLASS][g_iClientZombieClass[client]];
	g_fZombieKnockbackScale[ZR_MOTHER_ZOMBIE_CLASS][client] = g_fZombieClassKnockbackScale[ZR_MOTHER_ZOMBIE_CLASS][g_iClientZombieClass[client]];
	
	g_strZombieModel[ZR_MOTHER_ZOMBIE_CLASS][client] = g_strZombieClassMotherZombieModels[g_iClientZombieClass[client]][randomModel];
	
	if ( StrEqual ( g_strZombieModel[ZR_MOTHER_ZOMBIE_CLASS][client], "" ) ) {
		if ( g_iConfDebug ) {
			PrintToServer ( "[ZR] Zombie Class(ZR_MOTHER_ZOMBIE_CLASS) input failure on %N. Try again...", client );
			LogError ( "[ZR] Zombie Class(ZR_MOTHER_ZOMBIE_CLASS) input failure on %N. Try again...", client );
		}
		g_iClientZombieClass[client] = 0;
		CreateTimer ( 0.1, timerSetupMotherZombieData, client );
		return Plugin_Handled;
	}
	
	if ( g_iConfDebug )
		PrintToServer ( "[ZR] Zombie Class(ZR_MOTHER_ZOMBIE_CLASS) input successful on %N.", client );
	
	return Plugin_Handled;
}

/**
 * On open zclass menu.
 */
public Action onClientCommandZclass ( int client, int args ) {
	Menu menu = new Menu ( zclassMenuHandler );
	static char buff[128];
	menu.SetTitle ( "ZClass\n\t좀비: %s", g_strZombieClassname[g_iClientZombieClass[client]] );
	for ( int i = 0; i < g_iTotalZombieClass; i ++ ) {
		Format ( buff, sizeof ( buff ), "%s", g_strZombieClassname[i] );
		menu.AddItem ( "", buff );
	}
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int zclassMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	if ( !loringlib_IsValidClient ( param1 ) )
		return;
		
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select:	zclassViewer ( param1, param2 );
	}
}

/**
 * ZClass viewer.
 */
public void zclassViewer ( int client, int class ) {
	static char classnum[4];
	IntToString ( class, classnum, sizeof ( classnum ) );
	static char stats[6][64];
	static char changeViewmode[256];
	Menu menu = new Menu ( zclassViewerMenuHandler );
	menu.SetTitle ( "ZClass\n\t%s에 대해서...\n　\n%s", g_strZombieClassname[class], g_strZombieClassDescription[class] );
	if ( !g_bViewMode[client] ) {
		Format ( stats[0], 63, "\t\t체력: %d HP", g_iZombieClassHealth[ZR_NORMAL_ZOMBIE_CLASS][class] );
		Format ( stats[1], 63, "\t\t체력 추가 량: +%d HP", g_iZombieClassHealthMath[ZR_NORMAL_ZOMBIE_CLASS][class] );
		Format ( stats[2], 63, "\t\t체력 회복 량: (%.1f초 당) +%d HP", g_fZombieClassRechargeHealthTime[ZR_NORMAL_ZOMBIE_CLASS][class], g_iZombieClassRechargeHealthScale[ZR_NORMAL_ZOMBIE_CLASS][class] );
		Format ( stats[3], 63, "\t\t이동속도: %.2f", g_fZombieClassSpeed[ZR_NORMAL_ZOMBIE_CLASS][class] );
		Format ( stats[4], 63, "\t\t점프력: %.2f", g_fZombieClassGravity[ZR_NORMAL_ZOMBIE_CLASS][class] );
		Format ( stats[5], 63, "\t\t넉백: %.4f", g_fZombieClassKnockbackScale[ZR_NORMAL_ZOMBIE_CLASS][class] );
	}
	
	else {
		Format ( stats[0], 63, "\t\t체력: %d HP (EX)", g_iZombieClassHealth[ZR_MOTHER_ZOMBIE_CLASS][class] );
		Format ( stats[1], 63, "\t\t체력 추가 량: +%d HP (EX)", g_iZombieClassHealthMath[ZR_MOTHER_ZOMBIE_CLASS][class] );
		Format ( stats[2], 63, "\t\t체력 회복 량: (%.1f초 당) +%d HP (EX)", g_fZombieClassRechargeHealthTime[ZR_MOTHER_ZOMBIE_CLASS][class], g_iZombieClassRechargeHealthScale[ZR_MOTHER_ZOMBIE_CLASS][class] );
		Format ( stats[3], 63, "\t\t이동속도: %.2f (EX)", g_fZombieClassSpeed[ZR_MOTHER_ZOMBIE_CLASS][class] );
		Format ( stats[4], 63, "\t\t점프력: %.2f (EX)", g_fZombieClassGravity[ZR_MOTHER_ZOMBIE_CLASS][class] );
		Format ( stats[5], 63, "\t\t넉백: %.4f (EX)", g_fZombieClassKnockbackScale[ZR_MOTHER_ZOMBIE_CLASS][class] );
	}
	
	//	debug
	/*
	PrintToChat ( client, "\x04 " );
	PrintToChat ( client, "Section Name: %s", g_strZombieClassname[class] );
	PrintToChat ( client, "\"zombie_mother_zombie_model_1\" \"%s\"", g_strZombieClassMotherZombieModels[class][0] );
	PrintToChat ( client, "\"zombie_mother_zombie_model_2\" \"%s\"", g_strZombieClassMotherZombieModels[class][1] );
	PrintToChat ( client, "\"zombie_zombie_model_1\" \"%s\"", g_strZombieClassZombieModels[class][0] );
	PrintToChat ( client, "\"zombie_zombie_model_2\" \"%s\"", g_strZombieClassZombieModels[class][1] );
	PrintToChat ( client, "\x04 " );
	for ( int i = 0; i < 9; i ++ )
		PrintToChat ( client, "\"zombie_zombie_arms_model_%d\" \"%s\"", i + 1, g_strZombieClassZombieArmsModel[class][i] );
	PrintToChat ( client, "\x04 " );
	*/
	
	Format ( changeViewmode, sizeof ( changeViewmode ), "%s\n　\n%s\n%s\n%s\n%s\n%s\n%s", !g_bViewMode[client] ? "숙주 상태 보기" : "일반 상태 보기", stats[0], stats[1], stats[2], stats[3], stats[4], stats[5] );
	menu.AddItem ( classnum, "선택" );
	menu.AddItem ( classnum, changeViewmode );
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int zclassViewerMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_Select: {
			static char item[4];
			menu.GetItem ( param2, item, sizeof ( item ) );
			int class = StringToInt ( item );
			
			switch ( param2 ) {
				case 1: {				
					g_bViewMode[param1] = !g_bViewMode[param1];
					zclassViewer ( param1, class );
				}
				
				case 0:	changeClientZombieClass ( param1, class );
			}
		}
		
		case MenuAction_Cancel: {
			switch ( param2 ) {
				case MenuCancel_ExitBack: {
					onClientCommandZclass ( param1, 0 );
					return;
				}
				
				case MenuCancel_Exit: {
					delete menu;
					return;
				}
			}
		}
	}
}

public void changeClientZombieClass ( int client, int class ) {
	if ( !loringlib_IsValidClient ( client ) )
		return;
		
	g_iClientZombieClass[client] = class;
	
	changeClientZombieClass_Post ( client, class );
	
	PrintToChat ( client, "[ZR] %s을(를) 선택했습니다.", g_strZombieClassname[class] );
}

public void changeClientZombieClass_Post ( int client, int class ) {
	int randomModel = GetRandomInt ( 0, ( MAX_ZOMBIE_MODEL - 1 ) );
	int randomArmsModel = GetRandomInt ( 0, (MAX_ZOMBIE_ARMS_MODEL - 1 ) );
	
	//	Normal zombie.
	g_iZombieHealth[ZR_NORMAL_ZOMBIE_CLASS][client] = ( g_iZombieClassHealth[ZR_NORMAL_ZOMBIE_CLASS][g_iClientZombieClass[client]] + g_iZombieClassHealthMath[ZR_NORMAL_ZOMBIE_CLASS][g_iClientZombieClass[client]] * loringlib_GetPlayGameClients () );
	g_fZombieSpeed[ZR_NORMAL_ZOMBIE_CLASS][client] = g_fZombieClassSpeed[ZR_NORMAL_ZOMBIE_CLASS][g_iClientZombieClass[client]];
	g_fZombieGravity[ZR_NORMAL_ZOMBIE_CLASS][client] = g_fZombieClassGravity[ZR_NORMAL_ZOMBIE_CLASS][g_iClientZombieClass[client]];
	g_fZombieKnockbackScale[ZR_NORMAL_ZOMBIE_CLASS][client] = g_fZombieClassKnockbackScale[ZR_NORMAL_ZOMBIE_CLASS][g_iClientZombieClass[client]];
	
//	g_strZombieModel[ZR_NORMAL_ZOMBIE_CLASS][client] = g_strZombieClassZombieModels[g_iClientZombieClass[client]][randomModel];
//	g_strZombieArmsModel[client] = g_strZombieClassZombieArmsModel[g_iClientZombieClass[client]][randomArmsModel];
	strcopy ( g_strZombieModel[ZR_NORMAL_ZOMBIE_CLASS][client], sizeof ( g_strZombieModel[][] ), g_strZombieClassZombieModels[g_iClientZombieClass[client]][randomModel] );
	strcopy ( g_strZombieArmsModel[client], sizeof ( g_strZombieArmsModel[] ), g_strZombieClassZombieArmsModel[g_iClientZombieClass[client]][randomArmsModel] );
	
	//	Mother zombie.
	g_iZombieHealth[ZR_MOTHER_ZOMBIE_CLASS][client] = ( g_iZombieClassHealth[ZR_MOTHER_ZOMBIE_CLASS][g_iClientZombieClass[client]] + g_iZombieClassHealthMath[ZR_NORMAL_ZOMBIE_CLASS][g_iClientZombieClass[client]] * loringlib_GetPlayGameClients () );
	g_fZombieSpeed[ZR_MOTHER_ZOMBIE_CLASS][client] = g_fZombieClassSpeed[ZR_MOTHER_ZOMBIE_CLASS][g_iClientZombieClass[client]];
	g_fZombieGravity[ZR_MOTHER_ZOMBIE_CLASS][client] = g_fZombieClassGravity[ZR_MOTHER_ZOMBIE_CLASS][g_iClientZombieClass[client]];
	g_fZombieKnockbackScale[ZR_MOTHER_ZOMBIE_CLASS][client] = g_fZombieClassKnockbackScale[ZR_MOTHER_ZOMBIE_CLASS][g_iClientZombieClass[client]];
	
//	g_strZombieModel[ZR_MOTHER_ZOMBIE_CLASS][client] = g_strZombieClassMotherZombieModels[g_iClientZombieClass[client]][randomModel];
//	g_strZombieArmsModel[client] = g_strZombieClassZombieArmsModel[g_iClientZombieClass[client]][randomArmsModel];
	strcopy ( g_strZombieModel[ZR_MOTHER_ZOMBIE_CLASS][client], sizeof ( g_strZombieModel[][] ), g_strZombieClassMotherZombieModels[g_iClientZombieClass[client]][randomModel] );
	strcopy ( g_strZombieArmsModel[client], sizeof ( g_strZombieArmsModel[] ), g_strZombieClassZombieArmsModel[g_iClientZombieClass[client]][randomArmsModel] );
}

/**
 * Save zclass values.
 */
public int zclassInputValues ( int client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return;
		
	if ( IsFakeClient ( client ) )
		return;
	
	static char dbpath[PLATFORM_MAX_PATH];
	static char steam[32];
	GetClientAuthId ( client, AuthId_SteamID64, steam, sizeof ( steam ) );
	BuildPath ( Path_SM, dbpath, PLATFORM_MAX_PATH, "data/zombierelolied/zombie/class_db_%s.txt", steam );
	
	KeyValues kv = new KeyValues ( "zclass_client_db" );
	
	kv.JumpToKey ( steam, true );
	kv.SetNum ( "class_id", g_iClientZombieClass[client] );
	kv.Rewind ();
	
	kv.ExportToFile ( dbpath );
	
	delete kv;
	
	g_iClientZombieClass[client] = 0;
}

/**
 * Load zclass values.
 */
public void zclassOutputValues ( int client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return;
	
	if ( IsFakeClient ( client ) ) {
		g_iClientZombieClass[client] = GetRandomInt ( 0, g_iTotalZombieClass );
		return;
	}
	
	g_iClientZombieClass[client] = 0;
	
	static char dbpath[PLATFORM_MAX_PATH];
	static char steam[32];
	GetClientAuthId ( client, AuthId_SteamID64, steam, sizeof ( steam ) );
	BuildPath ( Path_SM, dbpath, PLATFORM_MAX_PATH, "data/zombierelolied/zombie/class_db_%s.txt", steam );
	
	KeyValues kv = new KeyValues ( "zclass_client_db" );
	if ( !kv.ImportFromFile ( dbpath ) ) {
		g_iClientZombieClass[client] = 0;
		delete kv;
		return;
	}
	
	kv.JumpToKey ( steam, false );
	g_iClientZombieClass[client] = kv.GetNum ( "class_id" );
	kv.Rewind ();
	
	delete kv;
}

/**
 * @section Debug infect invalid client.
 */
/**
 * Change normal zombie post
 */
public Action timerInfectHumanToZombie ( Handle timer, int client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Stop;
		
	infectHumanToZombie ( client );
	return Plugin_Stop;
}

/**
 * Change mother zombie post
 */
public Action timerInfectHumanToMotherZombie ( Handle timer, int client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Stop;
		
	infectHumanToMotherZombie ( client );
	return Plugin_Stop;
}