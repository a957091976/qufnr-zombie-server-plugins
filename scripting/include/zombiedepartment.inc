native int ZD_GetClientHumanClassNum ( int client );

native int ZD_GetClientHumanClassName ( int client, char[] buffer, int size, bool isKeyvalueName, bool isKorean );

native int ZD_GetClientLevel ( int client );

native int ZD_GetClientExp ( int client );

native int ZD_GetClientMaxExp ( int client );

native int ZD_GetClientRank ( int client );

native void ZD_GetClientRankName ( int client, char[] buffer, int maxlen );

forward void ZD_OnChangeLevel ( int client, int oldLevel, int newLevel, int newLevelTotalExps );

forward void ZD_OnBuyItem ( int client, char[] itemName );