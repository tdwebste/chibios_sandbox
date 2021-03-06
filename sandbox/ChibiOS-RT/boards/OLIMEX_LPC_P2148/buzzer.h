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

#ifndef _BUZZER_H_
#define _BUZZER_H_

#ifdef __cplusplus
extern "C" {
#endif
  void buzzInit(void);
  void buzzPlay(uint32_t freq, systime_t duration);
  void buzzPlayWait(uint32_t freq, systime_t duration);
#ifdef __cplusplus
}
#endif

extern EventSource BuzzerSilentEventSource;

#endif /* _BUZZER_H_ */
