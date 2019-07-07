static char g_strWeaponLowAmmoDatas[32][2][32] = {
	{ "weapon_glock", "5" }
	, { "weapon_hkp2000", "3" }
	, { "weapon_usp_silencer", "3" }
	, { "weapon_p250", "3" }
	, { "weapon_fiveseven", "5" }
	, { "weapon_cz75a", "4" }
	, { "weapon_tec9", "5" }
	, { "weapon_deagle", "2" }
	, { "weapon_revolver", "2" }
	, { "weapon_mp7", "7" }
	, { "weapon_mp9", "7" }
	, { "weapon_mac10", "7" }
	, { "weapon_ump45", "6" }
	, { "weapon_p90", "10" }
	, { "weapon_bizon", "12" }
	, { "weapon_famas", "7" }
	, { "weapon_galilar", "8" }
	, { "weapon_m4a1", "7" }
	, { "weapon_ak47", "7" }
	, { "weapon_sg553", "5" }
	, { "weapon_aug", "5" }
	, { "weapon_ssg08", "2" }
	, { "weapon_awp", "2" }
	, { "weapon_scar20", "4" }
	, { "weapon_g3sg1", "4" }
	, { "weapon_nova", "3" }
	, { "weapon_xm1014", "3" }
	, { "weapon_mag7", "2" }
	, { "weapon_sawedoff", "3" }
	, { "weapon_m249", "10" }
	, { "weapon_negev", "10" }
	, { "weapon_mp5sd", "7" }
};

void WeaponLowAmmo_TimerOnAlertLowAmmoMessage ( int client ) {
	if ( loringlib_IsValidClient ( client ) && ZR_IsClientHuman ( client ) ) {
		char weapon[32];
		int active = loringlib_GetActiveWeaponName ( client, weapon, sizeof ( weapon ) );
		
		if ( active != -1 ) {
			for ( int i = 0; i < sizeof ( g_strWeaponLowAmmoDatas ); i ++ ) {
				if ( StrEqual ( weapon, g_strWeaponLowAmmoDatas[i][0], false ) ) {
					if ( loringlib_GetClip1 ( active ) > 0 && loringlib_GetClip1 ( active ) <= StringToInt ( g_strWeaponLowAmmoDatas[i][1] ) ) {
						SetHudTextParams ( -1.0, 0.540, 0.12, 242, 203, 97, 255, 0, 0.0, 0.0, 0.0 );
						ShowSyncHudText ( client, g_hHudSyncArray[HUDSYNC_LOWAMMO_ALERT], "탄알 적음 (%d 발)", loringlib_GetClip1 ( active ) );
					}
					
					else if ( loringlib_GetClip1 ( active ) <= 0 ) {
						SetHudTextParams ( -1.0, 0.540, 0.12, 241, 95, 95, 255, 0, 0.0, 0.0, 0.0 );
						ShowSyncHudText ( client, g_hHudSyncArray[HUDSYNC_LOWAMMO_ALERT], "탄알 없음" );
					}
				}
			}
		}
	}
}

void WeaponLowAmmo_FireOnAlertLowAmmoSound ( int client, char[] weapon ) {
	if ( loringlib_IsValidClient ( client ) && ZR_IsClientHuman ( client ) ) {
		for ( int i = 0; i < sizeof ( g_strWeaponLowAmmoDatas ); i ++ ) {
			if ( StrContains ( g_strWeaponLowAmmoDatas[i][0], weapon, false ) != -1 ) {
				int active = loringlib_GetActiveWeapon ( client );
				if ( active != -1 ) {
					if ( loringlib_GetClip1 ( active ) == StringToInt ( g_strWeaponLowAmmoDatas[i][1] ) + 1 ) {
						PrecacheSound ( "qufnr/low_ammo.mp3" );
						EmitSoundToClient ( client, "qufnr/low_ammo.mp3" );
					}
				}
			}
		}
	}
}