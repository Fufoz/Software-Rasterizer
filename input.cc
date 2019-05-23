#include "input.h"
#include <SDL2/SDL.h>
#include <bitset>

//actual key states
static std::bitset<BTN_COUNT> keysState;
static std::bitset<MBTN_COUNT> mouseState;

static int mouseWheelVSign = 0;
static int mouseWheelHSign = 0;


void flushInputStates()
{
    keysState.reset();
    mouseState.reset();
}

void pollEvents()
{
    mouseWheelHSign = 0;
    mouseWheelVSign = 0;

    SDL_Event event;
    int iter = 0;
	while(SDL_PollEvent(&event)) {

        switch(event.type) {
            //keys message pump
            case SDL_KEYDOWN :
            case SDL_KEYUP :
                //updateKeyCodes(event);
                switch(event.key.keysym.sym)
                {
                    case SDLK_w : keysState[BTN_W] = event.key.state; break;
                    case SDLK_a : keysState[BTN_A] = event.key.state; break;
                    case SDLK_s : keysState[BTN_S] = event.key.state; break;
                    case SDLK_d : keysState[BTN_D] = event.key.state; break;
                    case SDLK_q : keysState[BTN_Q] = event.key.state; break;
                    case SDLK_e : keysState[BTN_E] = event.key.state; break;
                    case SDLK_r : keysState[BTN_R] = event.key.state; break;
                    case SDLK_t : keysState[BTN_T] = event.key.state; break;
                    case SDLK_y : keysState[BTN_Y] = event.key.state; break;
                    case SDLK_u : keysState[BTN_U] = event.key.state; break;
                    case SDLK_i : keysState[BTN_I] = event.key.state; break;
                    case SDLK_o : keysState[BTN_O] = event.key.state; break;
                    case SDLK_p : keysState[BTN_P] = event.key.state; break;
                    case SDLK_LEFTBRACKET : keysState[BTN_LEFT_BRACKET] = event.key.state; break;
                    case SDLK_RIGHTBRACKET : keysState[BTN_RIGHT_BRACKET] = event.key.state; break;
                    case SDLK_BACKSLASH : keysState[BTN_BACKSLASH] = event.key.state; break;
                    case SDLK_f : keysState[BTN_F] = event.key.state; break;
                    case SDLK_g : keysState[BTN_G] = event.key.state; break;
                    case SDLK_h : keysState[BTN_H] = event.key.state; break;
                    case SDLK_j : keysState[BTN_J] = event.key.state; break;
                    case SDLK_k : keysState[BTN_K] = event.key.state; break;
                    case SDLK_l : keysState[BTN_L] = event.key.state; break;
                    case SDLK_SEMICOLON : keysState[BTN_SEMICOLON] = event.key.state; break;
                    case SDLK_QUOTE : keysState[BTN_QUOTE] = event.key.state; break;
                    case SDLK_z : keysState[BTN_Z] = event.key.state; break;
                    case SDLK_x : keysState[BTN_X] = event.key.state; break;
                    case SDLK_c : keysState[BTN_C] = event.key.state; break;
                    case SDLK_v : keysState[BTN_V] = event.key.state; break;
                    case SDLK_b : keysState[BTN_B] = event.key.state; break;
                    case SDLK_n : keysState[BTN_N] = event.key.state; break;
                    case SDLK_m : keysState[BTN_M] = event.key.state; break;
                    case SDLK_COMMA : keysState[BTN_COMMA] = event.key.state; break;
                    case SDLK_PERIOD : keysState[BTN_DOT] = event.key.state; break;
                    case SDLK_SLASH : keysState[BTN_4WARDSLASH] = event.key.state; break;
                    case SDLK_RETURN : keysState[BTN_ENTER] = event.key.state; break;
                    case SDLK_RSHIFT : keysState[BTN_RSHIFT] = event.key.state; break;
                    case SDLK_LSHIFT : keysState[BTN_LSHIFT] = event.key.state; break;
                    case SDLK_LCTRL : keysState[BTN_CTRL] = event.key.state; break;
                    case SDLK_RCTRL : keysState[BTN_RCTRL] = event.key.state; break;
                    case SDLK_LALT : keysState[BTN_LALT] = event.key.state; break;
                    case SDLK_RALT : keysState[BTN_RALT] = event.key.state; break;
                    case SDLK_HOME : keysState[BTN_HOME] = event.key.state; break;
                    case SDLK_SPACE : keysState[BTN_SPACE] = event.key.state; break;
                    case SDLK_BACKSPACE : keysState[BTN_BACK] = event.key.state; break;
                    case SDLK_CAPSLOCK : keysState[BTN_CAPS] = event.key.state; break;
                    case SDLK_TAB : keysState[BTN_TAB] = event.key.state; break;
                    case SDLK_BACKQUOTE : keysState[BTN_CONSOLE] = event.key.state; break;
                    case SDLK_1 : keysState[BTN_1] = event.key.state; break;
                    case SDLK_2 : keysState[BTN_2] = event.key.state; break;
                    case SDLK_3 : keysState[BTN_3] = event.key.state; break;
                    case SDLK_4 : keysState[BTN_4] = event.key.state; break;
                    case SDLK_5 : keysState[BTN_5] = event.key.state; break;
                    case SDLK_6 : keysState[BTN_6] = event.key.state; break;
                    case SDLK_7 : keysState[BTN_7] = event.key.state; break;
                    case SDLK_8 : keysState[BTN_8] = event.key.state; break;
                    case SDLK_9 : keysState[BTN_9] = event.key.state; break;
                    case SDLK_0 : keysState[BTN_0] = event.key.state; break;
                    case SDLK_MINUS : keysState[BTN_MINUS] = event.key.state; break;
                    case SDLK_EQUALS : keysState[BTN_EQUAL] = event.key.state; break;
                    case SDLK_F1 : keysState[BTN_F1] = event.key.state; break;
                    case SDLK_F2 : keysState[BTN_F2] = event.key.state; break;
                    case SDLK_F3 : keysState[BTN_F3] = event.key.state; break;
                    case SDLK_F4 : keysState[BTN_F4] = event.key.state; break;
                    case SDLK_F5 : keysState[BTN_F5] = event.key.state; break;
                    case SDLK_F6 : keysState[BTN_F6] = event.key.state; break;
                    case SDLK_F7 : keysState[BTN_F7] = event.key.state; break;
                    case SDLK_F8 : keysState[BTN_F8] = event.key.state; break;
                    case SDLK_F9 : keysState[BTN_F9] = event.key.state; break;
                    case SDLK_F10 : keysState[BTN_F10] = event.key.state; break;
                    case SDLK_F11 : keysState[BTN_F11] = event.key.state; break;
                    case SDLK_F12 : keysState[BTN_F12] = event.key.state; break;
                    case SDLK_UP : keysState[BTN_UP] = event.key.state; break;
                    case SDLK_LEFT : keysState[BTN_LEFT] = event.key.state; break;
                    case SDLK_DOWN : keysState[BTN_DOWN] = event.key.state; break;
                    case SDLK_RIGHT : keysState[BTN_RIGHT] = event.key.state; break;
                    case SDLK_ESCAPE : keysState[BTN_ESCAPE] = event.key.state; break;
                    default : break;
                }
                break;
            //mouse button message pump
            case SDL_MOUSEBUTTONDOWN :
            case SDL_MOUSEBUTTONUP :
                switch(event.button.button) {
                    case SDL_BUTTON_LEFT : mouseState[MBTN_LEFT] = event.button.state; break;
                    case SDL_BUTTON_RIGHT : mouseState[MBTN_RIGHT] = event.button.state; break;
                    case SDL_BUTTON_MIDDLE : mouseState[MBTN_MIDDLE] = event.button.state; break;
                    case SDL_BUTTON_X1 : mouseState[MBTN_EXTRA_X1] = event.button.state; break;
                    case SDL_BUTTON_X2 : mouseState[MBTN_EXTRA_X2] = event.button.state; break;
                    default : break;
                }
                //updateMouseCodes(event);
                break;
            //mouse wheel message pump
            case SDL_MOUSEWHEEL :
                mouseWheelHSign = event.wheel.x;
                mouseWheelVSign = event.wheel.y;
                break;
            default :
                break;
        }
    }
}

bool isKeyPressed(KeyCode key)
{
    return keysState[key];
}

bool isMouseWheelScrolledUp()
{
    return mouseWheelVSign > 0;
}

bool isMouseWheelScrolledDown()
{
    return mouseWheelVSign < 0;
}

bool isMouseBtnPressed(MouseCode code)
{
    return mouseState[code];
}

Vec2 getDeltaMousePosition()
{
    Vec2 out;
    int x;
    int y;
    SDL_GetRelativeMouseState(&x, &y);
    out.x = (float)x;
    out.y = (float)y;
    return out;
}

Vec2 getMousePosition()
{
    int x, y;
    Vec2 position = {};

    SDL_GetMouseState(&x, &y);
    position.x = (float)x;
    position.y = (float)y;
    
    return position;
}
