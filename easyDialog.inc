
#if defined _inc_easyDialog
	#undef _inc_easyDialog
#endif

#if defined _easyDialog_included
	#endinput
#endif

#if defined _INC_EASYDIALOG_INCLUDED
	#endinput
#endif

#define _INC_EASYDIALOG_INCLUDED
#define _easyDialog_included

// Forward functions
forward OnDialogPerformed(playerid, string:dialog[], response, success);

// Fallback for the isnull commands
#if !defined IsNull
	#define IsNull(%1) \
		((!(%1[0])) || (((%1[0]) == '\1') && (!(%1[1]))))
#endif

// Make things better to distinguish between string and array
#define string:

// Backwards compatible
#define Dialog:%1(%2) forward dialog@%1(%2); public dialog@%1(%2)

// Add new dialog stuff
#define DialogResponse%1(%2) Dialog:%1(%2)
#define dialog@%8\32;%9 dialog@%8

// Overloading a bit (with backwards compatible)
#define Dialog_Show(%1,%2, Dialog_ShowPlayer(%1,#%2,
#define Dialog_Open Dialog_ShowPlayer

// Replace old dialog define with enum
#if !defined DIALOG_STYLE
	#if defined DIALOG_STYLE_MSGBOX
		#undef DIALOG_STYLE_MSGBOX
	#endif
	#if defined DIALOG_STYLE_INPUT
		#undef DIALOG_STYLE_INPUT
	#endif
	#if defined DIALOG_STYLE_PASSWORD
		#undef DIALOG_STYLE_PASSWORD
	#endif
	#if defined DIALOG_STYLE_LIST
		#undef DIALOG_STYLE_LIST
	#endif
	#if defined DIALOG_STYLE_TABLIST
		#undef DIALOG_STYLE_TABLIST
	#endif
	#if defined DIALOG_STYLE_TABLIST_HEADERS
		#undef DIALOG_STYLE_TABLIST_HEADERS
	#endif

	enum DIALOG_STYLE {
		DIALOG_STYLE_MSGBOX,
		DIALOG_STYLE_INPUT,
		DIALOG_STYLE_LIST,
		DIALOG_STYLE_PASSWORD,
		DIALOG_STYLE_TABLIST,
		DIALOG_STYLE_TABLIST_HEADERS,
	};
#endif

static
	string:Dialog_g_sCurrentName[MAX_PLAYERS][32 char],
	bool:Dialog_g_sPlayerOpenedDialog[MAX_PLAYERS] = {bool:-1, ...};

native easyDialog_Show(playerid, dialogid, DIALOG_STYLE:style, const string:caption[], const string:info[], const string:button1[], const string:button2[]) = ShowPlayerDialog;
stock bool:Dialog_ShowPlayer(playerid, const string:dialogName[], DIALOG_STYLE:style, const string:caption[], const string:info[], const string:button1[], const string:button2[] = "", {Float, _}:...)
{
	new bool:ret = bool:-1;
	// Just return false if player isn't connected
	if (!IsPlayerConnected(playerid))
	{
		return false;
	}

	static
		string:strFmt[4097],
		funcArgs
	;

	if (!strlen(info))
	{
		return false;
	}
	if ((funcArgs = numargs()) > 7)
	{
		while (--funcArgs >= 7)
		{
			#emit LCTRL 5
			#emit LOAD.alt funcArgs
			#emit SHL.C.alt 2
			#emit ADD.C 12
			#emit ADD
			#emit LOAD.I
			#emit PUSH.pri
		}
		#emit PUSH.S info
		#emit PUSH.C 4096
		#emit PUSH.C strFmt

		#emit LOAD.S.pri 8
		#emit CONST.alt 16
		#emit SUB
		#emit PUSH.pri

		#emit SYSREQ.C format
		#emit LCTRL 5
		#emit SCTRL 4

		ret = !!easyDialog_Show(playerid, 32700, style, caption, strFmt, button1, button2);
	}
	else
	{
		ret = !!easyDialog_Show(playerid, 32700, style, caption, info, button1, button2);
	}

	Dialog_g_sPlayerOpenedDialog[playerid] = true;
	strpack(Dialog_g_sCurrentName[playerid], dialogName, 32 char);
	return ret;
}

stock bool:Dialog_IsOpened(playerid)
{
	return (IsPlayerConnected(playerid) && Dialog_g_sPlayerOpenedDialog[playerid]);
}

stock bool:Dialog_Close(playerid)
{
	Dialog_g_sCurrentName[playerid]{0} = EOS;
	Dialog_g_sPlayerOpenedDialog[playerid] = false;

	return !!easyDialog_Show(playerid, -1, DIALOG_STYLE_MSGBOX, " ", " ", " ", "");
}

public OnDialogResponse(playerid, dialogid, response, listitem, string:inputtext[])
{
	static funcODF = cellmax;

	if (funcODF == cellmax)
	{
		funcODF = funcidx("OnDialogPerformed");
	}

	// Sanitize inputs.
	for (new idx = 0, len = strlen(inputtext); idx < len; idx ++)
	{
		if (inputtext[idx] == '%')
		{
			inputtext[idx] = '#';
		}
	}

	if (dialogid == 32700 && strlen(Dialog_g_sCurrentName[playerid]) > 0)
	{
		new
			funcNameFmt[40];

		strcat(funcNameFmt, "dialog@");
		strcat(funcNameFmt, Dialog_g_sCurrentName[playerid]);

		Dialog_Close(playerid);

		if ((funcODF == -1) || (CallLocalFunction("OnDialogPerformed", "dsdd", playerid, funcNameFmt[7], response, funcidx(funcNameFmt) != -1)))
		{
	 		CallLocalFunction(funcNameFmt, "ddds", playerid, response, listitem, (!inputtext[0]) ? ("\1") : (inputtext));
		}
	}

	#if defined easyDialog_OnDialogResponse
		return easyDialog_OnDialogResponse(playerid, dialogid, response, listitem, inputtext);
	#else
		return 0;
	#endif
}
#if defined _ALS_OnDialogResponse
	#undef OnDialogResponse
#else
	#define _ALS_OnDialogResponse
#endif

#define OnDialogResponse easyDialog_OnDialogResponse
#if defined easyDialog_OnDialogResponse
	forward easyDialog_OnDialogResponse(playerid, dialogid, response, listitem, string:inputtext[]);
#endif
