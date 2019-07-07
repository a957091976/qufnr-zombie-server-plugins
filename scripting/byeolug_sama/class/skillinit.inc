#include "byeolug_sama/class/use_skill_function.inc"

/**
 * 기술 쿨다운 채크 값 초기화.
 */
public void clearSkillCooldownData ( int client ) {
	for ( int i = 0; i < MAX_CLIENT_CLASS; i ++ ) {
		for ( int x = 0; x < g_iDefineClassSkills[i]; x ++ ) {
			g_bSkillCooldown[i][x][client] = false;
			g_iSkillCooldownTicks[i][x][client] = 0;
			g_bSkillUse[i][x][client] = false;
		}
	}
}

/**
 * 기술 쿨다운 계산.
 */
public void runningCooldownTime () {
	for ( int id = 1; id <= MaxClients; id ++ ) {
		if ( !loringlib_IsValidClient ( id ) )
			continue;
			
		for ( int i = 0; i < MAX_CLIENT_CLASS; i ++ ) {
			for ( int x = 0; x < g_iDefineClassSkills[i]; x ++ ) {
				if ( g_bSkillCooldown[i][x][id] ) {
					if ( g_iSkillCooldownTicks[i][x][id] > 0 )
						g_iSkillCooldownTicks[i][x][id] --;
						
					if ( g_iSkillCooldownTicks[i][x][id] < 1 ) {
						g_bSkillCooldown[i][x][id] = false;
						CLIENT_SKILL_COOLDOWN_END_MSG ( id, g_strSkillName[i][x] )
					}
				}
			}
		}
	}
}

/**
 * 쿨다운 중인 기술 표시
 * @note OnGameFrame 에서 프레임마다 호출됨.
 */
public void displayCooldownSkills ( int client ) {
	static char list[MAX_CLIENT_CLASS + 1][MAXSKILLS + 1][128];
	static char buffer[256];
	bool display = false;
	for ( int i = 0; i < MAX_CLIENT_CLASS; i ++ ) {
		if ( i == view_as <int> ( g_iClientClass[client] ) ||
			i == view_as <int> ( ClientClass_Normal ) ) {
			for ( int x = 0; x < g_iDefineClassSkills[i]; x ++ ) {
				if ( g_iSkillType[i][x] == SKILL_TYPE_ACTIVE ) {
					if ( g_iSkill[i][x][client] < 1 )
						continue;
					
					display = true;
					if ( g_bSkillCooldown[i][x][client] )
						Format ( list[i][x], sizeof ( list[][] ), "● %s (%d초 남음)", g_strSkillName[i][x], g_iSkillCooldownTicks[i][x][client] );
					else {
						if ( g_bSkillUse[i][x][client] )
							Format ( list[i][x], sizeof ( list[][] ), "● %s (사용중...)", g_strSkillName[i][x] );
				//			Format ( list[i][x], sizeof ( list[][] ), "%s <font color='#F15F5F'>(발동 상태)</font>", g_strSkillName[i][x] );
						else
							Format ( list[i][x], sizeof ( list[][] ), "● %s (사용 가능)", g_strSkillName[i][x] );
				//			Format ( list[i][x], sizeof ( list[][] ), "%s <font color='#86E57F'>(사용 가능)</font>", g_strSkillName[i][x] );
					}
					
					Format ( buffer, sizeof ( buffer ), "%s\n", list[i][x] );
				}
			}
		}
	}
	
	if ( ZR_IsClientZombie ( client ) || ZR_IsClientNemesis ( client ) )
		display = false;
	
	static char partyInfo[512];
	static char partyMembers[256];
	strcopy ( partyMembers, sizeof ( partyMembers ), "" );
	if ( isInParty ( client ) ) {
		if ( loringlib_IsValidClient ( g_iPartyManager[client] ) ) {
			static int memberIndex;
			memberIndex = -1;
			for ( int i = 0; i < PARTY_MAX_MEMBERS; i ++ ) {
				if ( !StrEqual ( g_strPartyMembers[g_iPartyManager[client]][i][PARTY_MEMBER_INDEX], "-1" ) ) {
					memberIndex = StringToInt ( g_strPartyMembers[g_iPartyManager[client]][i][PARTY_MEMBER_INDEX] );
					Format ( partyMembers, sizeof ( partyMembers ), "%s○ %s%s\n", partyMembers, g_strPartyMembers[g_iPartyManager[client]][i][PARTY_MEMBER_NICKNAME], ZR_IsClientHuman ( memberIndex ) ? " (H)" : " (Z)" );
				}
			}

			Format ( partyInfo, sizeof ( partyInfo ), "￣PARTY MEMBERS\n⊙ %s\n%s", g_strPartyName[g_iPartyManager[client]], partyMembers );
		}
	}
	
	else {
		strcopy ( partyInfo, sizeof ( partyInfo ), "" );
		strcopy ( partyMembers, sizeof ( partyMembers ), "" );
	}
	
	SetHudTextParams ( 0.015, 0.105, 0.2, 255, 255, 255, 255, 0, 0.0, 0.0, 0.0 );
	
	if ( !display ) {
		if ( isInParty ( client ) )
			if ( loringlib_IsValidClient ( g_iPartyManager[client] ) )
				ShowSyncHudText ( client, g_hHudSyncArray[HUDSYNC_SKILL_COOLDOWN], "%s", partyInfo );
		
		return;
	}
	
	ShowSyncHudText ( client, g_hHudSyncArray[HUDSYNC_SKILL_COOLDOWN], "￣ACTIVE SKILLS\n%s\n　\n%s", buffer, partyInfo );
//	PrintHintText ( client, "ACTIVE SKILLS (걷기 키로 사용)\n-----------------------\n%s", buffer );
}

/**
 * 병과 기술을 KeyValues로 부터 찾아옴.
 */
public void defineSkillKeyValue () {
	static char temp[PLATFORM_MAX_PATH];
	static char file[32];
	for ( int i = 0; i <= MAX_CLIENT_CLASS; i ++ ) {
		getClientClassFilename ( view_as <ClientClass> ( i ), file, sizeof ( file ) );
		BuildPath ( Path_SM, temp, PLATFORM_MAX_PATH, "data/zombierelolied/addons/class_skills/%s", file );
		KeyValues kv = new KeyValues ( file );
		if ( kv.ImportFromFile ( temp ) ) {
			g_iDefineClassSkills[i] = 0;
			kv.GotoFirstSubKey ();
			do {
				kv.GetSectionName ( g_strSkillName[i][g_iDefineClassSkills[i]], SKILL_NAME_MAX_LENGTH );
				kv.GetString ( "description", g_strSkillDescription[i][g_iDefineClassSkills[i]], SKILL_DESCRIPTION_MAX_LENGTH );
				g_iSkillMaxlvl[i][g_iDefineClassSkills[i]] = kv.GetNum ( "maxlvl" );
				g_iSkillType[i][g_iDefineClassSkills[i]] = kv.GetNum ( "type" );
				if ( g_iSkillType[i][g_iDefineClassSkills[i]] == SKILL_TYPE_ACTIVE ) {
					g_iSkillConsumeAP[i][g_iDefineClassSkills[i]] = kv.GetNum ( "consume_armorpoint" );
					g_iSkillCooldown[i][g_iDefineClassSkills[i]] = kv.GetNum ( "cooldown" );
				}
				g_iSkillNeedPrice[i][g_iDefineClassSkills[i]] = kv.GetNum ( "needplayerpt" );
				g_fSkillAction[i][g_iDefineClassSkills[i]] = kv.GetFloat ( "actionvalue" );
				g_iSkillBlockSkill_Id[i][g_iDefineClassSkills[i]] = kv.GetNum ( "block_skill_id" );
				g_iSkillActivitySkill_Id[i][g_iDefineClassSkills[i]] = kv.GetNum ( "activity_skill_id" );
				g_iSkillActivitySkillMaxlvl_Id[i][g_iDefineClassSkills[i]] = kv.GetNum ( "activity_skill_maxlvl_id" );
				
				g_iDefineClassSkills[i] ++;
			}
			
			while ( kv.GotoNextKey () );
			delete kv;
		}
		
		else	delete kv;
	}
}