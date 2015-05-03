#include "fat_cli.h"
#include "fat_sl.h"
#include "FreeRTOS_CLI.h"

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

void FatCli()
{
	FreeRTOS_CLIRegisterCommand(&xMkDir);
	FreeRTOS_CLIRegisterCommand(&xChDir);
	FreeRTOS_CLIRegisterCommand(&xRmDir);
	FreeRTOS_CLIRegisterCommand(&xPrintDir);
}
