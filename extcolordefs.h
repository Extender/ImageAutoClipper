#ifndef EXTCOLORDEFS_H
#define EXTCOLORDEFS_H

// Extended version of colordefs.h, taken from the ImageResizer project
// Version: 20170128

#include <stdint.h>
#include <math.h>

#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif
#ifndef M_PI_F
# define M_PI_F ((float)3.1415927)
#endif
#ifndef def_roundf
#define def_roundf(x) (round((float)(x)))
// Use the following line instead if "round" is not defined. Also works with negative numbers.
// #define def_roundf(x) ((float)(((x)-floor((float)(x)))>=0.5?ceil((float)(x)):floor((float)(x))))
#endif
#ifndef getByte
#define getByte(value,byte) (((value)>>((byte)*8))&0xFF)
#endif
#ifndef getAlpha
#define getAlpha(x) (((x)>>24)&0xff)
#endif
#ifndef getRed
#define getRed(x) (((x)>>16)&0xff)
#endif
#ifndef getGreen
#define getGreen(x) (((x)>>8)&0xff)
#endif
#ifndef getBlue
#define getBlue(x) ((x)&0xff)
#endif
#ifndef getColor
#define getColor(a,r,g,b) ((uint32_t)((((uint32_t)(a))<<24)|(((uint32_t)(r))<<16)|(((uint32_t)(g))<<8)|((uint32_t)(b))))
#endif
#ifndef colorComponentToF
#define colorComponentToF(x) ((float)((float)(x))/255.0f)
#endif
#ifndef colorFToComponent
#define colorFToComponent(x) ((uint8_t)def_roundf((x)*255.0f))
#endif
#ifndef getFAlpha
#define getFAlpha(x) ((float)((float)(((x)>>24)&0xff))/255.0f)
#endif
#ifndef getFRed
#define getFRed(x) ((float)((float)(((x)>>16)&0xff))/255.0f)
#endif
#ifndef getFGreen
#define getFGreen(x) ((float)((float)(((x)>>8)&0xff))/255.0f)
#endif
#ifndef getFBlue
#define getFBlue(x) ((float)((float)((x)&0xff))/255.0f)
#endif
#ifndef getFBWComponent
#define getFBWComponent(r,g,b) ((float)(0.2126f*((float)(r))+0.7152f*((float)(g))+0.0722f*((float)(b))))
#endif
#ifndef getFBWComponentFromColor
#define getFBWComponentFromColor(color) ((float)(0.2126f*getFRed(color)+0.7152f*getFGreen(color)+0.0722f*getFBlue(color)))
#endif
#ifndef getBWComponent
#define getBWComponent(color) ((uint8_t)def_roundf(getFBWComponent(getFRed(color),getFGreen(color),getFBlue(color))*255.0f))
#endif
// No "getBWColor"; use "getColor" with a variable and "getBWComponent" (to avoid performing the same calculations multiple times in macros).
#ifndef doubleDiv
#define doubleDiv(a,b) ((double)((double)(a))/((double)(b)))
#endif
#ifndef floatDiv
#define floatDiv(a,b) ((float)(((float)(a))/((float)(b))))
#endif
#ifndef pow2
#define pow2(a) ((a)*(a))
#endif
#ifndef pow2f
#define pow2f(a) ((float)(((float)(a))*((float)(a))))
#endif
#ifndef getColorError
#define getColorError(a,b) floatDiv(sqrt((float)pow2f((float)getRed(a)-(float)getRed(b))+pow2f((float)getGreen(a)-(float)getGreen(b))+pow2f((float)getBlue(a)-(float)getBlue(b))),sqrt(195075.0f))
#endif
#ifndef getLinearColorError
#define getLinearColorError(a,b) floatDiv((floatDiv(abs((float)getRed(a)-(float)getRed(b)),255.0f)+floatDiv(abs((float)getGreen(a)-(float)getGreen(b)),255.0f)+floatDiv(abs((float)getBlue(a)-(float)getBlue(b))),255.0f),3.0f)
#endif
#ifndef ifGTZero
#define ifGTZero(x) ((x)>0?(x):0)
#endif
#ifndef ifGTZeroArg
#define ifGTZeroArg(x,y) ((x)>0?(y):0)
#endif
#ifndef __max
#define __max(x,y) ((x)>(y)?(x):(y))
#endif
#ifndef __min
#define __min(x,y) ((x)<(y)?(x):(y))
#endif
#ifndef radians
#define radians(x) ((((float)x)/180.0f)*M_PI)
#endif

#endif // EXTCOLORDEFS_H
