#ifndef INPUT_H
#define INPUT_H
#include "maths.h"

enum KeyCode
{
    BTN_W = 0,
    BTN_A,
    BTN_S,
    BTN_D,
    BTN_Q,
    BTN_E,
    BTN_R,
    BTN_T,
    BTN_Y,
    BTN_U,
    BTN_I,
    BTN_O,
    BTN_P,
    BTN_LEFT_BRACKET,
    BTN_RIGHT_BRACKET,
    BTN_BACKSLASH,
    BTN_F,
    BTN_G,
    BTN_H,
    BTN_J,
    BTN_K,
    BTN_L,
    BTN_SEMICOLON,
    BTN_QUOTE,
    BTN_Z,
    BTN_X,
    BTN_C,
    BTN_V,
    BTN_B,
    BTN_N,
    BTN_M,
    BTN_COMMA,
    BTN_DOT,
    BTN_4WARDSLASH,
    BTN_ENTER,
    BTN_RSHIFT,
    BTN_LSHIFT,
    BTN_CTRL,
    BTN_RCTRL,
    BTN_LALT,
    BTN_RALT,
    BTN_HOME,//pusk
    BTN_SPACE,
    BTN_BACK,
    BTN_CAPS,
    BTN_TAB,
    BTN_CONSOLE,
    BTN_1,
    BTN_2,
    BTN_3,
    BTN_4,
    BTN_5,
    BTN_6,
    BTN_7,
    BTN_8,
    BTN_9,
    BTN_0,
    BTN_MINUS,
    BTN_EQUAL,
    BTN_F1,
    BTN_F2,
    BTN_F3,
    BTN_F4,
    BTN_F5,
    BTN_F6,
    BTN_F7,
    BTN_F8,
    BTN_F9,
    BTN_F10,
    BTN_F11,
    BTN_F12,
    BTN_UP,
    BTN_LEFT,
    BTN_DOWN,
    BTN_RIGHT,
    BTN_ESCAPE,
    BTN_COUNT
};

enum MouseCode
{
    MBTN_LEFT = 0,
    MBTN_RIGHT,
    MBTN_MIDDLE,
    MBTN_EXTRA_X1,
    MBTN_EXTRA_X2,
    MBTN_COUNT
};

//resets inputs to invalid state
void flushInputStates();

//pumps events from OS queue
void pollEvents();

bool isKeyPressed(KeyCode key);
bool isMouseBtnPressed(MouseCode code);
bool isMouseWheelScrolledUp();
bool isMouseWheelScrolledDown();

Vec2 getMousePosition();
Vec2 getDeltaMousePosition();

#endif