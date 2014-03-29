/*
    Copyright (C) 2013 Genist
*/

#ifndef _LED_H_
#define _LED_H_


typedef enum {
    LOAD_OK,    //!< Load between lower and upper limits
    LOAD_UNDER, //!< Load under weight
    LOAD_OVER,  //!< Load over weight
} INDICATOR;


typedef struct {
    INDICATOR     val;
    uint32_t    color;
} IndToColor;

typedef enum {
    MULT_100 =1,    //!< Multiply by 100
    MULT_1000,    //!< Multiply by 1000
    MULT_10000,    //!< Multiply by 10000
    MULT_100000    //!< Multiply by 100000
} MULTIPLIER;


typedef struct {
    MULTIPLIER     val;
    uint32_t    color;
} MulToColor;

typedef enum {
    VAL_2=1,    //!< Load is 2 times the multiplier
    VAL_4,  //!< Load is 4 times the multiplier
    VAL_6,    //!< Load is 6 times the multiplier
    VAL_8,    //!< Load is 8 times the multiplier
    VAL_10
} VALUE;


typedef struct {
    VALUE     val;
    uint32_t    color;
} ValToColor;


typedef struct {
   uint32_t        indicator;
   uint32_t     value;
   uint32_t     multiplier ;
} Colorcode;

typedef struct {
   int    timeof;
   int  timeon;
   int     repeat;
} Timecode;



#define LED_RED_MASK    0x00ff0000
#define LED_GREEN_MASK  0x0000ff00
#define LED_BLUE_MASK   0x000000ff
#define LED_RED_SHIFT   16
#define LED_GREEN_SHIFT 8
#define LED_BLUE_SHIFT  0
#define LED_BLACK       0x00

/* Colors redefinition for V5
*/
#define BARE_LED

#ifdef BARE_LED
#define LED_GREEN       (0x000000ff)
#define LED_RED         (0x00ff0000)
#define LED_BLUE        (0x0000ff00)
#define LED_WHITE       (0x00ffffff)
#define LED_ORANGE      (0x00ff003f)
#define LED_YELLOW      (0x00ff007f)
#define LED_PURPLE      (0x00ffff00)
#define LED_CYAN        (0x0000ffff)
#else //blue and green swap
#define LED_BLUE        (0x000000ff)
#define LED_RED         (0x00ff0000)
#define LED_GREEN       (0x0000ff00)
#define LED_WHITE       (0x00ffffff)
#define LED_ORANGE      (0x00ff3f00)
#define LED_YELLOW      (0x00ff7f00)
#define LED_PURPLE      (0x00ff00ff)
#define LED_CYAN        (0x0000ffff)
#endif 
/*
#define LED_RED         (0x00ff00ff)
#define LED_GREEN       (0x00ff0808)
#define LED_BLUE        (0x0022ff22)
#define LED_WHITE       (0x00808080)
#define LED_YELLOW      (0x00200010)
#define LED_PURPLE      (0x000090f0)
#define LED_CYAN        (0x000090f0)
#endif
*/

#define LED_ON            palSetPad(LED_STBY_PAD, LED_STBY_PIN)
#define LED_OFF         palClearPad(LED_STBY_PAD, LED_STBY_PIN)
/* Scaling factors needed to make 0x00ffffff = white */
#define RED_MAX         1000
#define GREEN_MAX       667 
#define BLUE_MAX        667 
/*
#define LED_RED         LED_RED_MASK
#define LED_GREEN       LED_GREEN_MASK
#define LED_BLUE        LED_BLUE_MASK
#define LED_WHITE       0x00ffffff
#define LED_YELLOW      (LED_RED_MASK | LED_GREEN_MASK)
#define LED_MAGENTA     (LED_RED_MASK | LED_BLUE_MASK)
#define LED_CYAN        (LED_GREEN_MASK | LED_BLUE_MASK)
*/

#define LED_BLINK_OFF   (0xFFFF0000)
/* Definitions for blinking pattern while stationary mode */
#define LED_BLINK_ON_S    (0x02000400)
#define LED_BLINK_DIV_S    (3)
#define LED_BLINK_REP_S    (6)

/* Definitions for blinking pattern while mobile */
#define LED_BLINK_ON_M    (0x0000400)
#define LED_BLINK_DIV_M    (1)
#define LED_BLINK_REP_M    (1)
extern void setLEDColor(uint32_t color);
extern uint8_t setBlinkCode(int load);
extern void setMotionFlag(bool motion);

#endif /* _LED_H_ */
