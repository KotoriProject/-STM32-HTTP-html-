#ifndef _GUI_
#define _GUI_

#include "oled.h"
#include "ws2811.h"

/*GUI return Status*/
typedef enum
{
    GUI_OK,
    GUI_Error,
} GUI_StatusTypeDef;
/*GUI ipnut key*/
typedef enum
{
    KEY_NONE,
    KEY_UP,
    KEY_DOWN,
    KEY_BACK,
    KEY_OK
} GUI_InputKey;

/*item type*/
typedef enum
{
    item_type_none,
    item_type_page,
    item_type_func,
} item_type;
/*选项结构体*/
typedef struct
{
    uint8_t *text;         // 显示的文本内容
    item_type item_type;   // 类型
    void *value;           // 值（函数地址或页地址）
    uint8_t value_text[3]; // 值-文本显示
} item;
/*页结构体*/
typedef struct PAGE
{
    uint8_t *page_title;    // 页面标题
    item items[7];          // 选项组
    uint8_t choose;         // 当前选择项
    struct PAGE *last_page; // 上一界面
} page;

extern page pages[10];
extern volatile uint16_t GUI_REFRESH_BUSY;

GUI_StatusTypeDef GUI_Refresh(GUI_InputKey key);

#define GUI_REFRESH GUI_Refresh(KEY_NONE)
//#define GUI_KEY_UP GUI_Refresh(KEY_UP)
//#define GUI_KEY_DOWN GUI_Refresh(KEY_DOWN)
//#define GUI_KEY_OK GUI_Refresh(KEY_OK)
//#define GUI_KEY_BACK GUI_Refresh(KEY_BACK)

#endif
