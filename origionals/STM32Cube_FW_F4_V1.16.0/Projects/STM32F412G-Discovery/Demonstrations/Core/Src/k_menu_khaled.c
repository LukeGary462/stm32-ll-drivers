/**
  ******************************************************************************
  * @file    k_menu.c
  * @author  MCD Application Team
  * @version V1.0.2
  * @date    17-February-2017 
  * @brief   This file provides the kernel menu functions 
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright © 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup CORE
  * @{
  */

/** @defgroup KERNEL_MENU
  * @brief Kernel menu routines
  * @{
  */

/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
#define ID_ICONVIEW_MENU               (GUI_ID_USER + 0x00)

typedef struct {
  int xSize, ySize;
  int xPart, yPart;
  int xPos, yPos;
  int Dir;
  int Size;
  int ObjectSize;
} ANIM_DATA;

ICONVIEW_Handle hIcon ;
 WM_HWIN hWin ;
uint8_t Direction = 0;

/**
  * @brief  Animation callback
  * @param  State
  * @param  pVoid:   Application defined void pointer
  * @retval None
  */
static void _SliceInfo(int State, void * pVoid) {
  GUI_USE_PARA(pVoid);
  switch (State) {
  case GUI_ANIM_START:
    GUI_MULTIBUF_Begin();
    break;
  case GUI_ANIM_END:
    GUI_MULTIBUF_End();
    break;
  }
}

/**
  * @brief  Animation information passed by STemWin to the application
  * @param  pMsg: pointer to a data structure of type WM_MESSAGE
  * @param  pVoid:   Application defined void pointer
  * @param  ppData:  Data (pointer) pointer to application data
  * @param  xPosOld: Position to be used for clearing the display
  * @param  Index:   Index of animation
  * @retval None
  */
static void _PrepareDrawing(GUI_ANIM_INFO * pInfo, void * pVoid, ANIM_DATA ** ppData, int xPosOld, int Index) 
{
  ANIM_DATA * pData;

  pData = *ppData = (ANIM_DATA *)pVoid;

  if(Direction == 0)
  {
    pData->xPos = -((pData->xPart) + ((pData->xSize - pData->xPart * 2) * pInfo->Pos) / GUI_ANIM_RANGE);
  }
  else
  {
    pData->xPos = -(pData->xSize - pData->xPart /*- 1*/ - ((pData->xSize - pData->xPart * 2) * pInfo->Pos) / GUI_ANIM_RANGE);
  }
  
  pData->yPos = pData->yPart * (2 * Index + 1);
  
  if (xPosOld) {
    GUI_ClearRect(xPosOld - pData->yPart, pData->yPos - pData->yPart, xPosOld + pData->yPart, pData->yPos + pData->yPart - 1);
  }
  
  switch (pInfo->State) {
  case GUI_ANIM_END:
    break;
  case GUI_ANIM_RUNNING:
    break;
  }
}

/**
  * @brief  Menu Animation
  * @param  pInfo: pointer to a animation informations
  * @param  pVoid:   Application defined void pointer
  * @retval None
  */
static void _AnimDrawMenu(GUI_ANIM_INFO * pInfo, void * pVoid) {
  ANIM_DATA * pData;
  static int xPosOld;
 
  /* Calculate new position */
  _PrepareDrawing(pInfo, pVoid, &pData, xPosOld, 0);
    
  /* Sets window position */
  WM_SetWindowPos(hIcon, pData->xPos, 0, (LCD_GetXSize() + 120), LCD_GetYSize());
  WM_Exec();  
  
  /* Update old position */
  xPosOld = pData->xPos;
}

/**
  * @brief  Menu Animation
  * @param  pData:Custom *void pointer
  * @retval None
  */
static void _AnimDemo(ANIM_DATA * pData) {
  GUI_ANIM_HANDLE hAnim;
  
  /* Create animation object
    
       Remark: The min time/frame here is 100 to be able to notice
               the colors. In a real application this value should
               be significantly smaller to ensure a smooth motion.
    
       Slice callback routine --------------+
       Custom *void pointer ---------+      |
       Minimum time per frame --+    |      |
       Duration ----------+     |    |      |
                          |     |    |      |                                 */
  hAnim = GUI_ANIM_Create(100, 30, pData, _SliceInfo);

   /* Add animation items
  
          Animation routine to be called ------------------------+
          Custom *void pointer ---------------------------+      |
          Method of position calculation +                |      |
          End on timeline ---------+     |                |      |
          Start on timeline -+     |     |                |      |
                             |     |     |                |      |               */
  GUI_ANIM_AddItem(hAnim,    0, 100, ANIM_LINEAR,      pData, _AnimDrawMenu);

  /* Start animation */
  GUI_ANIM_Start(hAnim);
  
  /* Execute animation until end is reached */
  while (GUI_ANIM_Exec(hAnim) == 0) {

    /* Idle time for other tasks */
    //GUI_X_Delay(5);
  }

    GUI_SetBkColor(GUI_TRANSPARENT);
    GUI_SetPenSize(5);
    GUI_DrawRect(0, 0, 235, 235);
  
  /*Delete animation object */
  GUI_ANIM_Delete(hAnim);  
}

/*********************************************************************
*
*       _SetupAnimationData
*/
static void _SetupAnimationData(ANIM_DATA * pData) {

  /* Get display size */
  pData->xSize = LCD_GetXSize() - 120;
  pData->ySize = LCD_GetYSize() - 210;
  pData->xPart = 0;
  pData->yPart = 0;
  pData->xPos  = 0;
  pData->Size  = (pData->yPart * 4) / 5;
  pData->Dir   = -1;
}

/*********************************************************************
*
*       _cbBk
*
*  Function description:
*    Callback routine of desktop window
*/
static void cbWin(WM_MESSAGE * pMsg) {

  ANIM_DATA Data;
  static GUI_PID_STATE TS_State  = {0, 0, 0, 0};
  static GUI_PID_STATE TS_State2 = {0, 0, 0, 0};

  /* Animation */
  uint32_t NCode, Id;
  static uint8_t sel = 0;
  
  switch (pMsg->MsgId) 
  {      
  case WM_PAINT:
    if(hIcon)
    {
      WM_BringToBottom(hIcon);
    }
    break;
    
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);     
    NCode = pMsg->Data.v;  
    
    switch (NCode) 
    {  
    case WM_NOTIFICATION_CLICKED:   
      
      GUI_TOUCH_GetState(&TS_State);  

      if (Id == ID_ICONVIEW_MENU)
      { 
        sel = ICONVIEW_GetSel(pMsg->hWinSrc);
                
        if(sel < k_ModuleGetNumber())
        {    
          if(sel < 8)
          {
            ICONVIEW_SetSel(pMsg->hWinSrc, -1);
          }
          else
          {
            ICONVIEW_SetSel(pMsg->hWinSrc, k_ModuleGetNumber() + 1); 
          }
        }
      }    

      break;
      
    case WM_NOTIFICATION_RELEASED:
      GUI_TOUCH_GetState(&TS_State2);
      
      if(TS_State2.x > TS_State.x + 20)
      {
        Direction = 1;
        _SetupAnimationData(&Data);
        
        _AnimDemo(&Data);         
      }   
      else if(TS_State2.x < TS_State.x - 20)
      {
        Direction  = 0;
        _SetupAnimationData(&Data);
        
        _AnimDemo(&Data);         
      }
      else
      {
        if (Id == ID_ICONVIEW_MENU)
        {
          if(sel < k_ModuleGetNumber())
          { 
            module_prop[sel].in_use = 1;
            //WM_SetWindowPos(pMsg->hWin, 0, 0, LCD_GetXSize(), LCD_GetYSize());
            module_prop[sel].module->startup(pMsg->hWin, 0, 0);
            sel = 0;
          }
        }
      }
      break;
    
    default:
      break;
    }
    break;
  default:
    WM_DefaultProc(pMsg);
  }
}

/**
  * @brief  set the GUI profile
  * @param  None.
  * @retval None.
  */
void k_SetGuiProfile(void) 
{
  BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
  DROPDOWN_SetDefaultSkin(DROPDOWN_SKIN_FLEX);
  
  FRAMEWIN_SetDefaultTextColor(0, GUI_WHITE);
  FRAMEWIN_SetDefaultTextColor(1, GUI_WHITE);      
  
  FRAMEWIN_SetDefaultBarColor(0, GUI_STCOLOR_LIGHTBLUE);
  FRAMEWIN_SetDefaultBarColor(1, GUI_STCOLOR_LIGHTBLUE);
  
  FRAMEWIN_SetDefaultFont(&GUI_FontAvantGarde32);
  
  FRAMEWIN_SetDefaultClientColor(GUI_WHITE);  
  FRAMEWIN_SetDefaultTitleHeight(35);
  
  
  WINDOW_SetDefaultBkColor(GUI_WHITE);
  
//  LISTVIEW_SetDefaultGridColor(GUI_WHITE);
//  LISTVIEW_SetDefaultBkColor(LISTVIEW_CI_SEL, GUI_STCOLOR_LIGHTBLUE);
//  DROPDOWN_SetDefaultColor(DROPDOWN_CI_SEL, GUI_STCOLOR_LIGHTBLUE);
//  LISTVIEW_SetDefaultBkColor(LISTVIEW_CI_SELFOCUS, GUI_STCOLOR_DARKBLUE);
//  LISTVIEW_SetDefaultTextColor(LISTVIEW_CI_UNSEL, GUI_STCOLOR_LIGHTBLUE);
//  DROPDOWN_SetDefaultColor(DROPDOWN_CI_SELFOCUS, GUI_STCOLOR_LIGHTBLUE);
//  SCROLLBAR_SetDefaultWidth(24);
//  SCROLLBAR_SetDefaultSTSkin();
//  
//  HEADER_SetDefaultBkColor(GUI_STCOLOR_LIGHTBLUE);
//  HEADER_SetDefaultTextColor(GUI_WHITE);
//  HEADER_SetDefaultFont(&GUI_FontAvantGarde24);
//  DROPDOWN_SetDefaultFont(&GUI_FontAvantGarde24);
//  LISTVIEW_SetDefaultFont(&GUI_FontAvantGarde20);
//  HEADER_SetDefaultSTSkin();
//  SCROLLBAR_SetDefaultColor(GUI_STCOLOR_LIGHTBLUE, SCROLLBAR_CI_THUMB);  
//  SCROLLBAR_SetDefaultColor(GUI_STCOLOR_LIGHTBLUE, SCROLLBAR_CI_SHAFT);  
//  SCROLLBAR_SetDefaultColor(GUI_STCOLOR_LIGHTBLUE, SCROLLBAR_CI_ARROW);    
//  ST_CHOOSEFILE_SetDelim('/');
//  ST_CHOOSEFILE_SetButtonSize(60, 40);
  
//  ST_CHOOSEFILE_SetButtonBitmaps (2, 
//                                  &bmup, 
//                                  NULL);
//                                 
//  ST_CHOOSEFILE_SetButtonBitmaps (1, 
//                                  &bmok, 
//                                  &bmOK_disabled);
//                                 
//  ST_CHOOSEFILE_SetButtonBitmaps (0, 
//                                  &bm_fccancel, 
//                                   NULL);
  
  GUI_SetDefaultFont(&GUI_FontLubalGraph20);
  
  TEXT_SetDefaultTextColor(GUI_STCOLOR_LIGHTBLUE);
  TEXT_SetDefaultFont(&GUI_FontLubalGraph20);
}

/**
  * @brief  Callback routine of desktop window
  * @param  pMsg:
  * @retval None
  */
static void _cbBk(WM_MESSAGE * pMsg) {
  GUI_RECT r;

  switch (pMsg->MsgId) 
  {      
  case WM_PAINT:
    WM_GetInsideRect(&r);
    GUI_ClearRectEx(&r);    
    break;
 
  default:
    WM_DefaultProc(pMsg);
  }
}

/**
  * @brief  Initializes the menu.
  * @param  None.
  * @retval None
  */
void k_InitMenu(void) 
{
  uint8_t i = 0;
  
  WM_SetCallback(WM_HBKWIN, _cbBk);
  
  hWin = WM_CreateWindowAsChild(0, 0,  3 * (LCD_GetXSize() /2), LCD_GetYSize(), WM_HBKWIN,
           WM_CF_SHOW | WM_CF_MOTION_X, cbWin, 0);  
  
  hIcon = ICONVIEW_CreateEx(0, 
                            0, 
                            LCD_GetXSize() + 120, 
                            LCD_GetYSize(), 
                            hWin, 
                            WM_CF_SHOW/* | WM_CF_HASTRANS*/,
                            NULL,
                            ID_ICONVIEW_MENU, 
                            110, 
                            110);
  
  ICONVIEW_SetSpace(hIcon, GUI_COORD_Y, 10);
  ICONVIEW_SetFrame(hIcon, GUI_COORD_Y, 5);
  ICONVIEW_SetSpace(hIcon, GUI_COORD_X, 10);
  ICONVIEW_SetFrame(hIcon, GUI_COORD_X, 5);  
  
  ICONVIEW_SetBkColor(hIcon, ICONVIEW_CI_BK, GUI_BLACK);
    
  for (i = 0; i < k_ModuleGetNumber(); i++)
  {
    ICONVIEW_AddBitmapItem(hIcon,module_prop[i].module->icon, (char *)module_prop[i].module->name);
  }
}  

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
