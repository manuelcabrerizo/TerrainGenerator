#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

#define internal static 
#define local_persist static 
#define global_variable static

typedef struct
{
    bool keyDown;
    bool keyUp;
} keyState_t;

typedef struct
{
    double x;
    double y;
} vec2_t;

typedef struct
{
    keyState_t keyStates[256];
    vec2_t mousePos;
} inputHandler_t;

void SetScanCodeUp(uint32_t  KeyScanCode);
void SetScanCodeDown(uint32_t  KeyScanCode);
bool KeyDown(uint32_t  KeyScanCode);
bool KeyUp(uint32_t  KeyScanCode);
void SetMouse(uint32_t  lParamX, uint32_t  lParamY);


#endif
