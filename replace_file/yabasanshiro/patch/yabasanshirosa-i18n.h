#ifndef I18N_H
#define I18N_H

#include <string>
#include <cstring>

enum Language {
    LANG_EN = 0,
    LANG_ZH = 1
};

// Language detection from LANG/LC_ALL environment variables
static inline Language detect_language() {
    const char* lc_all = getenv("LC_ALL");
    const char* lang = getenv("LANG");
    
    // Check LC_ALL first, then LANG
    if (lc_all && strstr(lc_all, "zh_CN")) {
        return LANG_ZH;
    }
    if (lang && strstr(lang, "zh_CN")) {
        return LANG_ZH;
    }
    return LANG_EN;
}

// Current language (auto-detected, can be overridden)
extern Language g_current_lang;

// Translation strings
struct I18NStrings {
    const char* menu_title;
    const char* player1;
    const char* player2;
    const char* player1_settings;
    const char* player2_settings;
    const char* config;
    const char* exit;
    const char* reset;
    const char* save_state;
    const char* load_state;
    const char* open_cd_tray;
    const char* close_cd_tray;
    const char* show_fps;
    const char* enable_frame_skip;
    const char* about;
    const char* close;
    
    // Config items
    const char* resolution;
    const char* aspect_rate;
    const char* rotate_resolution;
    const char* rotate_screen;
    const char* compute_shader;
    const char* full_screen;
    
    // Resolution options
    const char* native;
    const char* x2;
    const char* x4;
    const char* p720;
    
    // Aspect ratio options
    const char* original;
    const char* ratio_4_3;
    const char* ratio_16_9;
    
    // Button names
    const char* up;
    const char* down;
    const char* left;
    const char* right;
    const char* start;
    const char* select;
    const char* cancel;
    const char* analog_mode;
    
    // File browser
    const char* select_file;
    const char* load_slot;
    const char* save_slot;
    
    // Messages
    const char* push_key_for;
    const char* finish;
    const char* checking;
};

// English strings
static const I18NStrings lang_en = {
    "Yaba Sanshiro Menu",        // menu_title
    "Player1",                   // player1
    "Player2",                   // player2
    "Player1 Input Settings",    // player1_settings
    "Player2 Input Settings",    // player2_settings
    "Config",                    // config
    "Exit",                      // exit
    "Reset",                     // reset
    "Save State",                // save_state
    "Load State",                // load_state
    "Open CD Tray",              // open_cd_tray
    "Close CD Tray",             // close_cd_tray
    "Show/Hide FPS",             // show_fps
    "Enable/Disable Frame Skip", // enable_frame_skip
    "About",                     // about
    "Close",                     // close
    
    "Resolution",                // resolution
    "Aspect rate",               // aspect_rate
    "Rotate screen resolution",  // rotate_resolution
    "Rotate screen",             // rotate_screen
    "Use compute shader",        // compute_shader
    "Full Screen",               // full_screen
    
    "Native",                    // native
    "2x",                        // x2
    "4x",                        // x4
    "720p",                      // p720
    
    "Original",                  // original
    "4:3",                       // ratio_4_3
    "16:9",                      // ratio_16_9
    
    "UP",                        // up
    "DOWN",                      // down
    "LEFT",                      // left
    "RIGHT",                     // right
    "START",                     // start
    "SELECT",                    // select
    "Cancel",                    // cancel
    "Analog mode",               // analog_mode
    
    "Select File",               // select_file
    "Load Slot ",                // load_slot
    "Save Slot ",                // save_slot
    
    "Push key for ",             // push_key_for
    "Finish",                    // finish
    "Checking "                  // checking
};

// Chinese strings
static const I18NStrings lang_zh = {
    "Yaba Sanshiro 菜单",        // menu_title
    "玩家1",                     // player1
    "玩家2",                     // player2
    "玩家1 输入设置",            // player1_settings
    "玩家2 输入设置",            // player2_settings
    "配置",                      // config
    "退出",                      // exit
    "重置",                      // reset
    "保存存档",                  // save_state
    "读取存档",                  // load_state
    "打开光驱",                  // open_cd_tray
    "关闭光驱",                  // close_cd_tray
    "显示/隐藏 FPS",             // show_fps
    "启用/禁用 跳帧",            // enable_frame_skip
    "关于",                      // about
    "关闭",                      // close
    
    "分辨率",                    // resolution
    "宽高比",                    // aspect_rate
    "旋转分辨率",                // rotate_resolution
    "旋转屏幕",                  // rotate_screen
    "计算着色器",                // compute_shader
    "全屏",                      // full_screen
    
    "原始",                      // native
    "2倍",                       // x2
    "4倍",                       // x4
    "720p",                      // p720
    
    "原始",                      // original
    "4:3",                       // ratio_4_3
    "16:9",                      // ratio_16_9
    
    "上",                        // up
    "下",                        // down
    "左",                        // left
    "右",                        // right
    "开始",                      // start
    "选择",                      // select
    "取消",                      // cancel
    "摇杆模式",                  // analog_mode
    
    "选择文件",                  // select_file
    "读取存档 ",                 // load_slot
    "保存存档 ",                 // save_slot
    
    "请按键: ",                  // push_key_for
    "完成",                      // finish
    "检查中: "                   // checking
};

// Get current language strings
static inline const I18NStrings* get_strings() {
    return (g_current_lang == LANG_ZH) ? &lang_zh : &lang_en;
}

// Convenience macros
#define I18N_MENU_TITLE          (get_strings()->menu_title)
#define I18N_PLAYER1             (get_strings()->player1)
#define I18N_PLAYER2             (get_strings()->player2)
#define I18N_PLAYER1_SETTINGS    (get_strings()->player1_settings)
#define I18N_PLAYER2_SETTINGS    (get_strings()->player2_settings)
#define I18N_CONFIG              (get_strings()->config)
#define I18N_EXIT                (get_strings()->exit)
#define I18N_RESET               (get_strings()->reset)
#define I18N_SAVE_STATE          (get_strings()->save_state)
#define I18N_LOAD_STATE          (get_strings()->load_state)
#define I18N_OPEN_CD_TRAY        (get_strings()->open_cd_tray)
#define I18N_CLOSE_CD_TRAY       (get_strings()->close_cd_tray)
#define I18N_SHOW_FPS            (get_strings()->show_fps)
#define I18N_ENABLE_FRAME_SKIP   (get_strings()->enable_frame_skip)
#define I18N_ABOUT               (get_strings()->about)
#define I18N_CLOSE               (get_strings()->close)
#define I18N_RESOLUTION          (get_strings()->resolution)
#define I18N_ASPECT_RATE         (get_strings()->aspect_rate)
#define I18N_ROTATE_RESOLUTION   (get_strings()->rotate_resolution)
#define I18N_ROTATE_SCREEN       (get_strings()->rotate_screen)
#define I18N_COMPUTE_SHADER      (get_strings()->compute_shader)
#define I18N_FULL_SCREEN         (get_strings()->full_screen)
#define I18N_NATIVE              (get_strings()->native)
#define I18N_2X                  (get_strings()->x2)
#define I18N_4X                  (get_strings()->x4)
#define I18N_720P                (get_strings()->p720)
#define I18N_ORIGINAL            (get_strings()->original)
#define I18N_4_3                 (get_strings()->ratio_4_3)
#define I18N_16_9                (get_strings()->ratio_16_9)
#define I18N_UP                  (get_strings()->up)
#define I18N_DOWN                (get_strings()->down)
#define I18N_LEFT                (get_strings()->left)
#define I18N_RIGHT               (get_strings()->right)
#define I18N_START               (get_strings()->start)
#define I18N_SELECT              (get_strings()->select)
#define I18N_CANCEL              (get_strings()->cancel)
#define I18N_ANALOG_MODE         (get_strings()->analog_mode)
#define I18N_SELECT_FILE         (get_strings()->select_file)
#define I18N_LOAD_SLOT           (get_strings()->load_slot)
#define I18N_SAVE_SLOT           (get_strings()->save_slot)
#define I18N_PUSH_KEY_FOR        (get_strings()->push_key_for)
#define I18N_FINISH              (get_strings()->finish)
#define I18N_CHECKING            (get_strings()->checking)

#endif // I18N_H
