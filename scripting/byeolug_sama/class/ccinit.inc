/**
 * 클라이언트 클래스를 KeyValues로 부터 찾아옴.
 */
public void defineClassKeyValue () {
	static char temp[PLATFORM_MAX_PATH];
	static char file[32];
	Format ( file, sizeof ( file ), "class_list.txt" );
	BuildPath ( Path_SM, temp, PLATFORM_MAX_PATH, "data/zombierelolied/addons/%s", file );
	KeyValues kv = new KeyValues ( file );
	if ( kv.ImportFromFile ( temp ) ) {
		g_iDefineClass = 0;
		kv.GotoFirstSubKey ();
		do {
			kv.GetSectionName ( g_strClientClassName[g_iDefineClass], CLASS_NAME_MAX_LENGTH );
			kv.GetString ( "description", g_strClientClassDescription[g_iDefineClass], CLASS_DESCRIPTION_MAX_LENGTH );
			g_iClientClassNeedlvl[g_iDefineClass] = kv.GetNum ( "needlvl" );
			g_iClientClassOnlyAdmin[g_iDefineClass] = kv.GetNum ( "only_admin" );
			g_iDefineClass ++;
		}
		
		while ( kv.GotoNextKey () );
		delete kv;
	} else	delete kv;
}