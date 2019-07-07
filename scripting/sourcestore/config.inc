/**
 * Config variables
 */
#define MAXITEMRARITY			6

#define MAXMENUTYPE				5
#define MENUTYPE_CHARACTER		1
#define MENUTYPE_TRAIL			2
#define MENUTYPE_TAGTITLE		3
#define MENUTYPE_LASERBULLET	4
#define MENUTYPE_AURA			5

int g_iConfZombieMod = 0;		/**< Zombie Mod */
int g_iConfMinimumPlayers = 0;	/**< 이 값에 대입 된 값의 인원 수 부터 보상을 받습니다. */
int g_iConfMinimumBalance = 0;	/**< Mimimum balance by killer */
int g_iConfMaximumBalance = 0;	/**< Maximum balance by killer */
int g_iConfMinimumBalanceByAssister = 0;	/**< Mimimum balance reward by assister */
int g_iConfMaximumBalanceByAssister = 0;	/**< Maximum balance reward by assister */
int g_iConfMinimumBalanceByZombie = 0;		/**< Mimimum balance reward by zombie */
int g_iConfMaximumBalanceByZombie = 0;		/**< Maximum balance reward by zombie */
int g_iConfMinimumBalanceByAlive = 0;		/**< Minimum balance reward by alive players. (Zombie Mod Only) */
int g_iConfMaximumBalanceByAlive = 0;		/**< Maximum balance reward by alive players. (Zombie Mod Only) */
int g_iConfMaxInventoryItems = 0;		/**< Max currently items. */
int g_iConfAssisterCanGetBalance = 0;	/**< Assister can get balance? */
int g_iConfMenuEnable[MAXMENUTYPE + 1] = { 0, ... };	/**< Menu display */
int g_iConfRewardMinPlayers = 0;		/**< 최소한 이 값보다 높아야 플레이어 보상을 받습니다. */
float g_fConfPlayRewardTime = 0.0;		/**< Reward time. */
int g_iConfPlayReward = 0;				/**< Client can get reward balance amount */
int g_iConfPlayRewardBonus = 0;			/**< Client can get bonus reward balance amount */
int g_iConfGasyaTicketPrice[2] = { 0, ... };	/**< 가챠 티켓 가격 */
char g_strConfRewardBonusGroupName[16];	/**< Reward bonus group name. */
char g_strConfMoneyName[16];
char g_strConfMenuTitle[32];
char g_strConfBuyItemTitle[32];
char g_strConfVMITitle[32];
char g_strConfVMETitle[32];
char g_strConfItemGasyaTitle[32];
char g_strConfGasyaName[32];
char g_strConfPremiumGasyaName[32];
char g_strConfFreeTagName[64];
char g_strConfChatPrefix[64];
char g_strConfSoundReward[128];
char g_strConfItemRarity[MAXITEMRARITY][64];		/**< 아이템 등급 변수 */
char g_strConfClassLevel[MAXSTORELEVEL][64];	/**< 유저 등급 변수 */
int g_iConfUseMvp;
int g_iConfUseRandombox;
float g_fConfSoundVolumeReward = 0.0;
float g_fConfRainbowBulletFrequency = 1.0;
float g_fConfPreviewDisplayTime = 0.0;

public void onLoadConfig () {
	static char db[PLATFORM_MAX_PATH];
	static char temp[256];
	BuildPath ( Path_SM, db, PLATFORM_MAX_PATH, "configs/qufnrsourcestore.cfg" );
	KeyValues kv = new KeyValues ( "configs" );
	if ( kv.ImportFromFile ( db ) ) {
		kv.JumpToKey ( "qufnrsourcestore", false );
		
		g_iConfZombieMod = kv.GetNum ( "sourcestore_zombie_mod" );
		g_iConfMinimumPlayers = kv.GetNum ( "sourcestore_minimum_players" );
		g_iConfAssisterCanGetBalance = kv.GetNum ( "sourcestore_assister_can_get_balance" );
		
		g_iConfRewardMinPlayers = kv.GetNum ( "sourcestore_reward_minplayers" );
		g_fConfPlayRewardTime = kv.GetFloat ( "sourcestore_reward_time" );
		kv.GetString ( "sourcestore_reward_bonus_group_name", g_strConfRewardBonusGroupName, sizeof ( g_strConfRewardBonusGroupName ) );
		kv.GetString ( "sourcestore_reward_sound", g_strConfSoundReward, sizeof ( g_strConfSoundReward ) );
		g_fConfSoundVolumeReward = kv.GetFloat ( "sourcestore_reward_sound_volume" );
		
		g_iConfUseRandombox = kv.GetNum ( "sourcestore_use_randombox" );
		g_iConfGasyaTicketPrice[TICKET_NORMAL] = kv.GetNum ( "sourcestore_normal_gasya_ticket_price" );
		g_iConfGasyaTicketPrice[TICKET_PREMIUM] = kv.GetNum ( "sourcestore_premium_gasya_ticket_price" );
		
		kv.GetString ( "sourcestore_money_name", g_strConfMoneyName, sizeof ( g_strConfMoneyName ) );
		kv.GetString ( "sourcestore_mainmenu_title", g_strConfMenuTitle, sizeof ( g_strConfMenuTitle ) );
		kv.GetString ( "sourcestore_buy_item_title", g_strConfBuyItemTitle, sizeof ( g_strConfBuyItemTitle ) );
		kv.GetString ( "sourcestore_VMI_title", g_strConfVMITitle, sizeof ( g_strConfVMITitle ) );
		kv.GetString ( "sourcestore_VME_title", g_strConfVMETitle, sizeof ( g_strConfVMETitle ) );
		kv.GetString ( "sourcestore_item_gasya_title", g_strConfItemGasyaTitle, sizeof ( g_strConfItemGasyaTitle ) );
		kv.GetString ( "sourcestore_gasya_name", g_strConfGasyaName, sizeof ( g_strConfGasyaName ) );
		kv.GetString ( "sourcestore_premium_gasya_name", g_strConfPremiumGasyaName, sizeof ( g_strConfPremiumGasyaName ) );
		kv.GetString ( "sourcestore_free_tag_name", g_strConfFreeTagName, sizeof ( g_strConfFreeTagName ) );
		kv.GetString ( "sourcestore_chat_prefix", g_strConfChatPrefix, sizeof ( g_strConfChatPrefix ) );
		if ( !qufnrTools_FormatColor ( g_strConfChatPrefix, sizeof ( g_strConfChatPrefix ) ) )
			LogMessage ( "Chat prefix color was Not Found!" );
		g_iConfMenuEnable[MENUTYPE_CHARACTER] = kv.GetNum ( "sourcestore_menu_enable_character" );
		g_iConfMenuEnable[MENUTYPE_TRAIL] = kv.GetNum ( "sourcestore_menu_enable_trail" );
		g_iConfMenuEnable[MENUTYPE_LASERBULLET] = kv.GetNum ( "sourcestore_menu_enable_laserbullet" );
		g_iConfMenuEnable[MENUTYPE_TAGTITLE] = kv.GetNum ( "sourcestore_menu_enable_tagtitle" );
		g_iConfMenuEnable[MENUTYPE_AURA] = kv.GetNum ( "sourcestore_menu_enable_aura" );
		g_fConfPreviewDisplayTime = kv.GetFloat ( "sourcestore_preview_display_time" );
		
		for ( int i = 0; i < MAXITEMRARITY; i ++ ) {
			Format ( temp, sizeof ( temp ), "sourcestore_item_rarity_%d", i );
			kv.GetString ( temp, g_strConfItemRarity[i], 63 );
		}
		
		for ( int i = 0; i < MAXSTORELEVEL; i ++ ) {
			Format ( temp, sizeof ( temp ), "sourcestore_storelevel_%d", i );
			kv.GetString ( temp, g_strConfClassLevel[i], 63 );
		}
		g_iConfUseMvp = kv.GetNum ( "sourcestore_use_mvp" );
		
		g_iConfMinimumBalance = kv.GetNum ( "sourcestore_minimum_balance" );
		g_iConfMaximumBalance = kv.GetNum ( "sourcestore_maximum_balance" );
		g_iConfMinimumBalanceByAssister = kv.GetNum ( "sourcestore_maximum_balance_by_assister" );
		g_iConfMaximumBalanceByAssister = kv.GetNum ( "sourcestore_maximum_balance_by_assister" );
		g_iConfMinimumBalanceByZombie = kv.GetNum ( "sourcestore_maximum_balance_by_zombie" );
		g_iConfMaximumBalanceByZombie = kv.GetNum ( "sourcestore_maximum_balance_by_zombie" );
		g_iConfMinimumBalanceByAlive = kv.GetNum ( "sourcestore_minimum_balance_by_alivehumans" );
		g_iConfMaximumBalanceByAlive = kv.GetNum ( "sourcestore_maximum_balance_by_alivehumans" );
		g_iConfPlayReward = kv.GetNum ( "sourcestore_play_reward" );
		g_iConfPlayRewardBonus = kv.GetNum ( "sourcestore_play_reward_bonus" );
		g_iConfMaxInventoryItems = kv.GetNum ( "sourcestore_max_inventory_items" );
		
		delete kv;
	}

	else
		delete kv;
}
