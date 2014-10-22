#include "cli.h"

#include "FreeRTOS.h"
#include "os_task.h"
#include "os_queue.h"
#include "os_event_groups.h"
#include "os_semphr.h"

#include "FreeRTOS_CLI.h"
#include "sci.h"

#include <string.h>
#include <stdio.h>

#define CMD_BUFFER_SIZE 150

static char cli_input[CMD_BUFFER_SIZE], cli_output[CMD_BUFFER_SIZE];

static BaseType_t prvSciSendCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	sciBASE_t *sci;
	BaseType_t len;
	int param_len;
	int sci_n;
	char *param_p;
	param_p = (char *)FreeRTOS_CLIGetParameter(pcCommandString, 1, &len);
	sscanf(param_p, "%d", &sci_n);
	param_p = (char *)FreeRTOS_CLIGetParameter(pcCommandString, 2, &len);
	sscanf(param_p, "%d", &param_len);
	param_p = (char *)FreeRTOS_CLIGetParameter(pcCommandString, 3, &len);

	sci = sci_n ? sciREG : scilinREG;

	if(sciSendOS(sci, param_len, (uint8*)param_p) == pdPASS)
	{
		strncpy(pcWriteBuffer, "Sending complete\n\0", 18);
	}
	else
	{
		strncpy(pcWriteBuffer, "Sending failed\n\0", 16);
	}
	return pdFALSE;
}
CLI_Command_Definition_t xSciSendCommand =
{
	"sciSend",
	"\r\nsciSend sci len msg:\r\n Send data through sci\r\n\r\n",
	prvSciSendCommand,
	3
};

void cliInit(void)
{
	FreeRTOS_CLIRegisterCommand(&xSciSendCommand);
}

void cliTask(void *pvParameters)
{
	uint8 data;
	int recv_len = 0;
	int send_len;
	BaseType_t xMoreDataToFollow;

	while(1)
	{
		if(sciReceiveByteOS(scilinREG, &data) == pdPASS)
		{
			cli_input[recv_len] = data;
			recv_len++;
			if(data == '\n')
			{
				cli_input[recv_len-1] = '\0';
				do{
					xMoreDataToFollow = FreeRTOS_CLIProcessCommand(cli_input, cli_output, CMD_BUFFER_SIZE);
					send_len = strlen(cli_output);
					sciSendOS(scilinREG, send_len, (uint8*)cli_output);
				}while(xMoreDataToFollow != pdFALSE);
					recv_len = 0;
			}
			else if(recv_len == CMD_BUFFER_SIZE)
			{
				recv_len = 0;
				sciSendOS(scilinREG, 14, (uint8*)"cmd too long\r\n");
			}
		}
	}
}
