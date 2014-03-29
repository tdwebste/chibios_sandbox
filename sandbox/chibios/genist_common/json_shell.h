/*
    ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/**
 * @file    json_shell.h
 * @brief   Simple JSON shell header.
 *
 * @addtogroup SHELL
 * @{
 */

#ifndef _JSON_SHELL_H_
#define _JSON_SHELL_H_

#define JSON_SHELL_MAX_LINE_LENGTH  1024

/**
 * @brief   Command handler function type.
 */
typedef void (*jsoncmd_t)(BaseSequentialStream *chp, char *jstring);

/**
 * @brief   Custom command entry type.
 */
typedef struct {
  const char            *jsc_name;          /**< @brief Command name.       */
  jsoncmd_t             jsc_function;       /**< @brief Command function.   */
} JSONCommand;

#ifdef __cplusplus
extern "C" {
#endif
  Thread *jsonShellCreate(const BaseSequentialStream *chp, size_t size, tprio_t prio);
  Thread *jsonShellCreateStatic(const BaseSequentialStream *chp, void *wsp,
                            size_t size, tprio_t prio);
  bool_t jsonShellGetLine(BaseSequentialStream *chp, char *line, unsigned size);
  Thread *startJSON(void);

#ifdef __cplusplus
}
#endif

#endif /* _JSON_SHELL_H_ */

/** @} */
