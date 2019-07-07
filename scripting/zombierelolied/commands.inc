public void registerCommands () {
	RegConsoleCmd ( "sm_ztele", onClientCommandZtele, "Teleport to re-spawn area." );
	RegConsoleCmd ( "sm_zmarket", onClientCommandZmarket, "Open the zmakret menu." );
	RegConsoleCmd ( "sm_zclass", onClientCommandZclass, "Open the zclass menu." );
	RegConsoleCmd ( "sm_changeviewmodel", onClientCommandChangeViewmodel, "" );
	RegConsoleCmd ( "sm_zrtest", zrtest );
}