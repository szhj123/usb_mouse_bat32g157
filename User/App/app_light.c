/********************************************************
* @file       app_light.c
* @author     szhj13
* @version    V1.0
* @date       2022-05-18
* @brief      the start address of program
**********************************************************
* @Modify

**********************************************************/

/* Includes ---------------------------------------------*/
#include "app_light.h"
/* Private typedef --------------------------------------*/
typedef void (*light_show_callback_t)(void );
/* Private define ---------------------------------------*/
/* Private macro ----------------------------------------*/
/* Private function -------------------------------------*/
static void App_Light_Handler(void *arg );
/* Private variables ------------------------------------*/
static light_show_callback_t light_show_callback = NULL;
static uint16_t        lightDelayCnt;
static rainbow_para_t  rainbowPara;
static solid_on_para_t solidOnPara;
static breath_para_t   breathPara;

light_t lightBuf[] = 
{
    { 
        .port = PORTA,
        .pin  = PIN0,
    },
    { 
        .port = PORTA,
        .pin  = PIN1,
    },
    { 
        .port = PORTA,
        .pin  = PIN3,
    },

    { 
        .port = PORTA,
        .pin  = PIN12,
    },
    { 
        .port = PORTA,
        .pin  = PIN13,
    },
    { 
        .port = PORTA,
        .pin  = PIN14,
    },
};

void App_Light_Init(void )
{
    Drv_Light_Init();

    Drv_Task_Regist_Period(0, 1, App_Light_Handler, NULL);
}

void App_Ligth_Delay_Count(void )
{
    if(lightDelayCnt < 0xffff)
    {
        lightDelayCnt++;
    }
}

void App_Light_Clr_Delay_Count(void )
{
    lightDelayCnt = 0;
}

uint16_t App_Light_Get_Delay_Count(void )
{
    return lightDelayCnt;
}

void App_Light_Rainbow_Callback(light_show_callback_t callback )
{
    light_show_callback = callback;
}

static void App_Light_Handler(void *arg )
{
    App_Ligth_Delay_Count();
    
    if(light_show_callback != NULL)
    {
        light_show_callback();
    }
}

static void App_Light_Set_Rainbow_Color(light_t *light )
{
    switch(light->lightState)
    {
        case LIGHT_STATE_RED:
        {
            if(light->rVal == 255 && light->gVal == 0 && light->bVal == 0)
            {
                light->lightState = LIGHT_STATE_BLUE;
            }
            else
            {
                if(light->rVal < 255)
                {
                    light->rVal++;
                }
                if(light->gVal > 0)
                {
                    light->gVal--;
                }
                if(light->bVal > 0)
                {
                    light->bVal--;
                }
            }
            break;
        }
        case LIGHT_STATE_BLUE:
        {
            if(light->rVal == 0 && light->gVal == 0 && light->bVal == 255)
            {
                light->lightState = LIGHT_STATE_GREEN;
            }
            else
            {
                if(light->rVal > 0)
                {
                    light->rVal--;
                }
                if(light->gVal > 0)
                {
                    light->gVal--;
                }
                if(light->bVal < 255)
                {
                    light->bVal++;
                }
            }
            break;
        }
        case LIGHT_STATE_GREEN:
        {
            if(light->rVal == 0 && light->gVal == 255 && light->bVal == 0)
            {
                light->lightState = LIGHT_STATE_RED;
            }
            else
            {
                if(light->rVal > 0)
                {
                    light->rVal--;
                }
                if(light->gVal < 255)
                {
                    light->gVal++;
                }
                if(light->bVal > 0)
                {
                    light->bVal--;
                }
            }
            break;
        }
        default: break;
    }
}

static void App_Light_Rainbow_Callback(void )
{
    uint8_t i;

    Drv_Light_Set_All_Off();

    if(App_Light_Get_Delay_Count() > rainbowPara.delayTime)
    {
        App_Light_Clr_Delay_Count();
        
        for(i=0;i<(sizeof(lightBuf)/sizeof(light_t));i++)
        {
            App_Light_Set_Rainbow_Color(&lightBuf[i]);
        }
    }

    Drv_Light_Set_On(lightBuf[rainbowPara.lightIndex]);

    if(rainbowPara.lightIndex < (sizeof(lightBuf)/sizeof(light_t)))
    {
        rainbowPara.lightIndex++;
    }
    else
    {
        rainbowPara.lightIndex = 0;
    }
}

void App_Light_Rainbow(void )
{
    rainbowPara.speed = App_Mouse_Get_Rainbow_Speed();
    rainbowPara.direction = App_Mouse_Get_Rainbow_Direction();
    rainbowPara.lightIndex = 0;
    
    App_Light_Clr_Delay_Count();

    switch(rainbowPara.speed)
    {
        case 1: rainbowPara.delayTime = 4; break;
        case 3: rainbowPara.delayTime = 3; break;
        case 2: rainbowPara.delayTime = 2; break;
        case 4: rainbowPara.delayTime = 1; break;
        default: break;
    }
    
    uint8_t i;;

    for(i=0;i<(sizeof(lightBuf)/sizeof(light_t));i++)
    {
        lightBuf[i].rVal = lightBuf[i].gVal = lightBuf[i].bVal = 0;
    }

    if(rainbowPara.direction)
    {
        lightBuf[0].lightState = LIGHT_STATE_RED;
        lightBuf[1].lightState = LIGHT_STATE_BLUE;
        lightBuf[2].lightState = LIGHT_STATE_GREEN;
        lightBuf[3].lightState = LIGHT_STATE_RED;
        lightBuf[4].lightState = LIGHT_STATE_BLUE;
        lightBuf[5].lightState = LIGHT_STATE_GREEN;
    }
    else
    {
        lightBuf[0].lightState = LIGHT_STATE_GREEN;
        lightBuf[1].lightState = LIGHT_STATE_BLUE;
        lightBuf[2].lightState = LIGHT_STATE_RED;
        lightBuf[3].lightState = LIGHT_STATE_GREEN;
        lightBuf[4].lightState = LIGHT_STATE_BLUE;
        lightBuf[5].lightState = LIGHT_STATE_RED;
    }

    App_Light_Set_Show_Callback(App_Light_Rainbow_Callback);
}

void App_Light_Solid_On(void )
{
    solidOnPara.brightness = (brightness_t)App_Mouse_Get_Solid_On_Brightness();
    solidOnPara.lightColor = App_Mouse_Get_Solid_On_Color();    

    light_show_callback = NULL;

    switch(solidOnPara.brightness)
    {
        case BRIGHTNESS_25:
        {
            solidOnPara.lightColor.rVal = solidOnPara.lightColor.rVal / 4;
            solidOnPara.lightColor.gVal = solidOnPara.lightColor.gVal / 4;
            solidOnPara.lightColor.bVal = solidOnPara.lightColor.bVal / 4;
            break;
        }
        case BRIGHTNESS_50:
        {
            solidOnPara.lightColor.rVal = solidOnPara.lightColor.rVal / 2;
            solidOnPara.lightColor.gVal = solidOnPara.lightColor.gVal / 2;
            solidOnPara.lightColor.bVal = solidOnPara.lightColor.bVal / 2;
            break;
        }
        case BRIGHTNESS_75:
        {
            solidOnPara.lightColor.rVal = solidOnPara.lightColor.rVal * 3 / 4;
            solidOnPara.lightColor.gVal = solidOnPara.lightColor.gVal * 3 / 4;
            solidOnPara.lightColor.bVal = solidOnPara.lightColor.bVal * 3 / 4;
            break;
        }
        case BRIGHTNESS_100:
        default: break;
    }

    Drv_Light_Set_All_On(solidOnPara.lightColor.rVal, solidOnPara.lightColor.gVal, solidOnPara.lightColor.bVal);
}

void App_Light_Breath(void )
{
    breathPara.speed = App_Mouse_Get_Breah_Speed();

    App_Mouse_Get_Breath_Color(breathPara.lightColorBuf, sizeof(breathPara.lightColorBuf)/sizeof(light_color_t));
    
}

void App_Light_Switch(uint8_t lightMode )
{
    switch((light_mode_t )lightMode)
    {
        case LIGHT_OFF:
        {
            break;
        }
        case LIGHT_RAINBOW:
        {
            App_Light_Rainbow();
            break;
        }
        case LIGHT_SOLID_ON:
        {
            App_Light_Solid_On();
            break;
        }
        case LIGHT_BREATH:
        {
            App_Light_Breath();
            break;
        }
        case LIGHT_NEON:
        {
            break;
        }
        case LIGHT_FLASH:
        {            break;
        }
        case LIGHT_MONOCHROMATIC_DRAG:
        {            break;
        }
        case LIGHT_REACTION:
        {
            break;
        }
        case LIGHT_CHROMATIC_DRAG:
        {
            break;
        }
        default: break;
    }
}

