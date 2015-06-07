#include "fat_cli.h"
#include "fat_sl.h"
#include "FreeRTOS_CLI.h"
#include "api_mdriver_sdcard.h"

static BaseType_t prvMkDir( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	BaseType_t len;
	char *param_p;
	param_p = (char *)FreeRTOS_CLIGetParameter(pcCommandString, 1, &len);

	if(fr_mkdir(param_p) == 0)
	{
		pcWriteBuffer[0] = '\n';
		pcWriteBuffer[1] = '\0';
	}
	else
	{
		strncpy(pcWriteBuffer, "Error\n\0", 7);
	}

	return pdFALSE;
}


CLI_Command_Definition_t xMkDir =
{
	"mkdir",
	"\r\nmkdir dir_name:\r\n Makes new directory dir_name\r\n\r\n",
	prvMkDir,
	1
};

static BaseType_t prvChDir( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	BaseType_t len;
	char *param_p;
	param_p = (char *)FreeRTOS_CLIGetParameter(pcCommandString, 1, &len);

	if(fr_chdir(param_p) == 0)
	{
		pcWriteBuffer[0] = '\n';
		pcWriteBuffer[1] = '\0';
	}
	else
	{
		strncpy(pcWriteBuffer, "Error\n\0", 7);
	}

	return pdFALSE;
}


CLI_Command_Definition_t xChDir =
{
	"cd",
	"\r\ncd dir_name:\r\n Change directory to dir_name\r\n\r\n",
	prvChDir,
	1
};

static BaseType_t prvRmDir( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	BaseType_t len;
	char *param_p;
	param_p = (char *)FreeRTOS_CLIGetParameter(pcCommandString, 1, &len);

	if(fr_rmdir(param_p) == 0)
	{
		pcWriteBuffer[0] = '\n';
		pcWriteBuffer[1] = '\0';
	}
	else
	{
		strncpy(pcWriteBuffer, "Error\n\0", 7);
	}

	pcWriteBuffer[0] = '\n';

	return pdFALSE;
}


CLI_Command_Definition_t xRmDir =
{
	"rmdir",
	"\r\nrmdir dir_name:\r\n Removes directory dir_name\r\n\r\n",
	prvRmDir,
	1
};

static BaseType_t prvPrintDir( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	BaseType_t len;
	char *param_p;
	param_p = (char *)FreeRTOS_CLIGetParameter(pcCommandString, 1, &len);

	fn_getcwd(pcWriteBuffer, xWriteBufferLen, 1);

	return pdFALSE;
}


CLI_Command_Definition_t xPrintDir =
{
	"pwd",
	"\r\npwd:\r\n Prints working directory\r\n\r\n",
	prvPrintDir,
	0
};

static BaseType_t prvInitVolume( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	BaseType_t len;
	char *param_p;
	char ret;
	param_p = (char *)FreeRTOS_CLIGetParameter(pcCommandString, 1, &len);

	ret = f_initvolume(sdcard_initfunc);

	if(ret == 0)
	{
		strncpy(pcWriteBuffer, "Volume initialized\n\0", 20);
	}
	else
	{
		strncpy(pcWriteBuffer, "Error\n\0", 7);
	}

	return pdFALSE;
}

CLI_Command_Definition_t xInitVolume =
{
	"initVolume",
	"\r\npwd:\r\n Initialize given volume, 1 or 2\r\n\r\n",
	prvInitVolume,
	0
};

static BaseType_t prvFormatVolume( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	BaseType_t len;
	char *param_p;
	char ret;
	param_p = (char *)FreeRTOS_CLIGetParameter(pcCommandString, 1, &len);

	if(param_p[0] == '1')
	{
		ret = f_format( F_FAT12_MEDIA );
	}
	else if(param_p[0] == '2')
	{
		ret = f_format( F_FAT16_MEDIA );
	}
	else if(param_p[0] == '3')
	{
		ret = f_format( F_FAT32_MEDIA );
	}
	else
	{
		strncpy(pcWriteBuffer, "Error, wrong FS type\n\0", 28);
		return pdFALSE;
	}

	if(ret == 0)
	{
		strncpy(pcWriteBuffer, "Volumes formated\n\0", 18);
	}
	else
	{
		strncpy(pcWriteBuffer, "Error, not formated\n\0", 21);
		//sprintf(pcWriteBuffer, "Error:%i\n\0", ret);
	}

	return pdFALSE;
}

CLI_Command_Definition_t xFormatVolume =
{
	"formatVolume",
	"\r\npwd:\r\n Formats sdcards, 1 - FAT12, 2 - FAT16, 3 - FAT32\r\n\r\n",
	prvFormatVolume,
	1
};

static BaseType_t prvInitFS( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	BaseType_t len;
	char *param_p;
	char ret;
	param_p = (char *)FreeRTOS_CLIGetParameter(pcCommandString, 1, &len);

	ret = fs_init();

	if(ret == 0)
	{
		strncpy(pcWriteBuffer, "File system initialized\n\0", 25);
	}
	else
	{
		strncpy(pcWriteBuffer, "Error\n\0", 7);
	}

	return pdFALSE;
}

CLI_Command_Definition_t xInitFS =
{
	"initFS",
	"\r\npwd:\r\n Initialize file system\r\n\r\n",
	prvInitFS,
	0
};

static BaseType_t prvSetActiveVolume( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	BaseType_t len;
	char *param_p;
	char ret;
	param_p = (char *)FreeRTOS_CLIGetParameter(pcCommandString, 1, &len);

	if(param_p[0] == '1')
	{
		set_active_card(0);
		strncpy(pcWriteBuffer, "Volume 1 activated\n\0", 20);
	}
	else if(param_p[0] == '2')
	{
		set_active_card(1);
		strncpy(pcWriteBuffer, "Volume 2 activated\n\0", 20);
	}
	else
	{
		strncpy(pcWriteBuffer, "Wrong volume number\n\0", 21);
	}

	return pdFALSE;
}

CLI_Command_Definition_t xSetActiveVolume =
{
	"setActiveVolume",
	"\r\npwd:\r\n Sets the active volume\r\n\r\n",
	prvSetActiveVolume,
	1
};


void FatCli()
{
	FreeRTOS_CLIRegisterCommand(&xMkDir);
	FreeRTOS_CLIRegisterCommand(&xChDir);
	FreeRTOS_CLIRegisterCommand(&xRmDir);
	FreeRTOS_CLIRegisterCommand(&xPrintDir);
	FreeRTOS_CLIRegisterCommand(&xInitVolume);
	FreeRTOS_CLIRegisterCommand(&xFormatVolume);
	FreeRTOS_CLIRegisterCommand(&xInitFS);
	FreeRTOS_CLIRegisterCommand(&xSetActiveVolume);
}
