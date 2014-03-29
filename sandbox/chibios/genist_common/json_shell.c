/*
    (c) Genist 2013
*/

/**
 * @file    json_shell.c
 * @brief   JSON shell code.
 *
 * @{
 */

#include <string.h>

#include "ch.h"
#include "hal.h"
#include "json_shell.h"
#include "chprintf.h"
#include "genist_debug.h"
#include "cJSON.h"
#include "db.h"
#include "chconf.h"


#define JSON_MAX_LINE_LENGTH    1024

#if 1
#define CODE_STX        0x02
#define CODE_ETX        0x03
#else
#define CODE_STX        '@'
#define CODE_ETX        '\r'
#endif
#define SEND_STX(chp)   chSequentialStreamPut(chp, CODE_STX);
#define SEND_ETX(chp)   chSequentialStreamPut(chp, CODE_ETX);
#define JSON_SHELL_WA_SIZE   THD_WA_SIZE(2048)

Thread *shelltp = NULL;
extern Thread *startConsole(void);

/**
 * Parse for tokens
 * @param str
 * @param delim
 * @param saveptr
 * @return
 */
static char *_strtok(char *str, const char *delim, char **saveptr)
{
    char *token;
    if (str)
        *saveptr = str;
    token = *saveptr;

    if (!token)
        return NULL;

    token += strspn(token, delim);
    *saveptr = strpbrk(token, delim);
    if (*saveptr)
        *(*saveptr)++ = '\0';

    return *token ? token : NULL;
}

/**
 * JSON CONSOLE command
 * Go into ChibiOS shell mode. Type exit to go back to JSON console.
 * @param chp
 * @param jstring
 */
void cmd_console(BaseSequentialStream *chp, char *jstring)
{
    (void) jstring;
    (void) chp;
    if (!shelltp)
    {
        /* Spawns a new shell.*/
        shelltp = startConsole();
    }

    /* Wait for the thread to exit*/
    chThdWait(shelltp);
    shelltp = NULL;
    dbgprint_enable = FALSE;
    SEND_STX(chp);
    chprintf(chp, "OK");
    SEND_ETX(chp);
}

/**
 * JSON GET command. Retrieves the database as a JSON formatted
 * string.
 * @param chp
 * @param jstring
 */
void cmd_get(BaseSequentialStream *chp, char *jstring)
{
    cJSON *dbObject = dbGetJSON(jstring);
    char *dbString = cJSON_PrintUnformatted(dbObject);

    SEND_STX(chp);
    if (dbString == NULL)
    {
        chprintf(chp, "ERROR");
    }
    else
    {
        chprintf(chp, "OK %s", dbString);
    }
    SEND_ETX(chp);
    chHeapFree(dbString);
    cJSON_Delete(dbObject);
}

/**
 * JSON SET command. Sets the value of one or more database items.
 * The input is a JSON formatted string. If the string contains
 * items that are not recognized, no database items are modified.
 *
 * @param chp
 * @param jstring
 */
void cmd_set(BaseSequentialStream *chp, char *jstring)
{
    SEND_STX(chp);
    if (!dbSetJSON(jstring))
    {
        chprintf(chp, "OK");
    }
    else
    {
        chprintf(chp, "ERROR");
    }
    SEND_ETX(chp);
}

/**
 * JSON SAVE database command. Saves the database to Flash.
 *
 * @param chp
 * @param jstring
 */
void cmd_save(BaseSequentialStream *chp, char *jstring)
{
    (void) jstring;
    SEND_STX(chp);
    if (!dbSave())
    {
        chprintf(chp, "OK");
    }
    else
    {
        chprintf(chp, "ERROR");
    }
    SEND_ETX(chp);
}
static JSONCommand commands[] =
{
        { "GET", cmd_get },
        { "SET", cmd_set },
        { "SAVE", cmd_save },
        { "CONSOLE", cmd_console },
        {NULL, NULL}
};

/**
 * Execute command if found
 * @param jscp
 * @param chp
 * @param name
 * @param jstring
 * @return TRUE if command is not found
 */
static bool_t jcmdexec(const JSONCommand *jscp, BaseSequentialStream *chp,
        char *name, char *jstring)
{

    while (jscp->jsc_name != NULL)
    {
        if (strcmp(jscp->jsc_name, name) == 0)
        {
            jscp->jsc_function(chp, jstring);
            return FALSE;
        }
        jscp++;
    }
    return TRUE;
}

/**
 * @brief   JSON Shell thread function.
 *
 * @param[in] p         pointer to a @p BaseSequentialStream object
 * @return              Termination reason.
 * @retval RDY_OK       terminated by command.
 * @retval RDY_RESET    terminated by reset condition on the I/O channel.
 */
static msg_t json_thread(void *p)
{
    BaseSequentialStream *chp = (BaseSequentialStream *) p;
    char *cmd, *jstring, *tokp, line[JSON_MAX_LINE_LENGTH];

    chRegSetThreadName("json");
    while (TRUE)
    {
        if (jsonShellGetLine(chp, line, sizeof(line)))
        {
            break;
        }
        /* Format is <command> <json-string> */
        cmd = _strtok(line, " \t", &tokp);
        jstring = _strtok(NULL, " \t", &tokp);
        if (jcmdexec(commands, chp, cmd, jstring))
        {
            SEND_STX(chp);
            chprintf(chp, "ERROR");
            SEND_ETX(chp);
        }
    }
    return 0; /* Never executed.*/
}

/**
 * @brief   Spawns a new JSON shell.
 * @pre     @p CH_USE_MALLOC_HEAP and @p CH_USE_DYNAMIC must be enabled.
 *
 * @param[in] chp       pointer to a @p BaseSequentialStream object
 * @param[in] size      size of the shell working area to be allocated
 * @param[in] prio      priority level for the new shell
 * @return              A pointer to the shell thread.
 * @retval NULL         thread creation failed because memory allocation.
 */
#if CH_USE_HEAP && CH_USE_DYNAMIC
Thread *jsonShellCreate(const BaseSequentialStream *chp, size_t size, tprio_t prio)
{
    return chThdCreateFromHeap(NULL, size, prio, json_thread, (void *)chp);
}
#endif

/**
 * @brief   Create statically allocated JSON shell thread.
 *
 * @param[in] scp       pointer to a @p BaseSequentialStream object
 * @param[in] wsp       pointer to a working area dedicated to the shell thread stack
 * @param[in] size      size of the shell working area
 * @param[in] prio      priority level for the new shell
 * @return              A pointer to the shell thread.
 */
Thread *jsonShellCreateStatic(const BaseSequentialStream *chp, void *wsp,
        size_t size, tprio_t prio)
{

    return chThdCreateStatic(wsp, size, prio, json_thread, (void *)chp);
}

/**
 * @brief   Reads a whole line from the input channel. A line is enclosed by
 * STX/ETX characters.
 *
 * @param[in] chp       pointer to a @p BaseSequentialStream object
 * @param[in] line      pointer to the line buffer
 * @param[in] size      buffer maximum length
 * @return              The operation status.
 * @retval TRUE         the channel was reset.
 * @retval FALSE        operation successful.
 */
bool_t jsonShellGetLine(BaseSequentialStream *chp, char *line, unsigned size)
{
    char *p = line;

    while (TRUE)
    {
        char c;

        /* Read a character */
        if (chSequentialStreamRead(chp, (uint8_t *)&c, 1) == 0)
        {
            /* No character found (EOF?) */
            return TRUE;
        }
        switch (c)
        {
        case CODE_STX:
            /* STX encountered. Reset the line */
            p = line;
            break;
        case CODE_ETX:
            /* ETX encountered. Line is complete */
            *(p++) = 0;
            return FALSE;
            break;
        default:
            /* Normal character */
            *(p++) = c;
            break;
        }
        if ((unsigned)(p - line) >= size)
        {
            return TRUE;
        }
    }
    return TRUE;
}

/**
 * Application entry point.
 */
Thread *startJSON(void)
{
    return jsonShellCreate((BaseSequentialStream *)&CONSOLE, JSON_SHELL_WA_SIZE, NORMALPRIO);
}
/** @} */
