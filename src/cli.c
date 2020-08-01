/////////////////////////////
//	cli.c
/////////////////////////////
#define CLI_GLOBALS
#include "includes.h"
/////////////////////////////



#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>

//#include "cli_main.h"
//#include "cli_cmd.h"

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

static char cmdBuf[MAX_USER_CMD_BUF];
static char systemBuf[MAX_USER_CMD_BUF];

extern char *optarg;
extern int optind, opterr, optopt;

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

static void
_hex_dump(int index, unsigned char *buf, int size)
{
	int i;
	int o;
	char str[32];

	if (index < 0)
		index = 0;
	o = 0;
	memset(str, 0, 32);

	for (i=0; i <size; i++)
	{
		str[o] = 0;
		if ((i % 16) == 0)
		{
			if (i == 0)
				printf("%04x: ", index+i);
			else
				printf("    %s\n%04x: ", str, index+i);
			o = 0;
			memset(str, 0, 32);
		}
		else if ((i % 4) == 0) 
			printf(" ");

		printf("%02x ", buf[i]);

		if (isprint(buf[i]))
			str[o] = buf[i];
		else
			str[o] = '.';
		o++;
	}

	i %= 16;
	if (i > 0)
	{
		for (; i < 16; i++)
		{
			if (i == 4 || i == 8 || i == 12)
				printf("    ");
			else
				printf("   ");
		}
	}

	if (o > 0)
		printf("    %s\n", str);
	else
		printf("\n");
	fflush(stdout);
}

void
hex_dump(unsigned char *buf, int size)
{
	if (size > 128)
	{
		_hex_dump(0, &buf[0], 64);
		printf("....:\n");
		_hex_dump(size-64, &buf[size - 64], 64);
	}
	else if (size > 0)
		_hex_dump(0, buf, size);
}

////////////////////////////////////////////////////////////////////////

static void
_local_get_string(char *str, int max_chars)
{
	char charData;
	int count;

	count = 0;
	str[max_chars - 1] = '\0';

	while ((charData = getchar()) != '\n')
	{
		if (count >= max_chars)
			break;

		str[count++] = charData;
	}

	str[count] = '\0';
}

static char *
_skip_white_space(char *str, int max_chars)
{
	int i;

	*(str + max_chars - 1) = '\0';

	for (i=0; i<max_chars; i++)
	{
		if (*str == ' ' || *str == '\t')
			str++;
		else
			break;
	}

	return str;
}

static int
_make_argv_string(char *str, char **avpp)
{
	int argc;
	int after_alpha;

	after_alpha = 0;

	argc = 0;
	avpp[argc++] = str;

	while (*str != '\0')
	{
		if (*str >= '!' && *str <= '~')
		{
			if (after_alpha)
			{
				after_alpha = 0;
				avpp[argc++] = str;
			}
			str++;
		}
		else if (*str == ' ' || *str == '\t')
		{
			*str++ = '\0';
			after_alpha++;
		}
		else
			str++;
	}

	return argc;
}

static int
_get_cmd_code(char *str)
{
	int  i;

	for(i=0; usr_cmd_tbl[i].cmd != NULL; i++)
		if (strncmp(str, usr_cmd_tbl[i].cmd, MAX_USER_CMD_BUF) == 0)
			return i;

	return -1;
}

void 
_set_tty_for_erase_key()
{
	int dev_fd;
	struct termios tio;

	dev_fd = open("/dev/tty", O_RDWR|O_NOCTTY);
	if (dev_fd > 0)
	{
		tcgetattr(dev_fd, &tio);

		tio.c_cc[VERASE] = 0x08;			// '^H'

		tcsetattr(dev_fd, TCSANOW, &tio);

		close(dev_fd);
	}
}



int CLI_Process( void )
{
	char *cmd_av[MAX_ARGV_CNT];
	int cmd_ac;
	char *str;
	int i;
	int cmd;
	int result;

	_set_tty_for_erase_key();
	/*
	//printf("\n%s Command Line Interface V%s ....\n", PROGRAM_NAME, PROGRAM_VERSION);
	//printf("         Compiled at %s, %s\n\n", __TIME__, __DATE__);
	*/
	printf("-------------------------------------------------------\n");
	printf("\n < Register Command Line Interface >\n\n");
	printf("-------------------------------------------------------\n");
	

	result = 0;

	while (result >= 0)
	{
		optarg = NULL;
		optind = 0;
		opterr = 0;
		optopt = 0;

		for (i=0; i<MAX_ARGV_CNT; i++)
			cmd_av[i] = NULL;

		printf("%s ", PROMPT);

		_local_get_string(cmdBuf, MAX_USER_CMD_BUF);

		str = _skip_white_space(cmdBuf, MAX_USER_CMD_BUF);

		strncpy(systemBuf, str, MAX_USER_CMD_BUF - (str - cmdBuf));

		if (*str == '\n' || *str == '\0')
			continue;

		cmd_ac = _make_argv_string(str, &cmd_av[0]);

		if ((cmd = _get_cmd_code(cmd_av[0])) < 0)
		{
			system(systemBuf);
		}
		else
		{
			printf("\n");
			result = usr_cmd_tbl[cmd].func(cmd_ac, cmd_av);
			printf("\n");
		}
	}

	return 0;
}






///////////////////////////////////////////////////////////////////////////////////////
//
// Functions Definitions
//
///////////////////////////////////////////////////////////////////////////////////////


/* EOF */

