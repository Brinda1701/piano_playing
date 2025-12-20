#include <graphics.h>
#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include <math.h>
#include <string>

#define OCTAVE_COUNT 4
#define KEYS_PER_OCTAVE 12
#define KEY_COUNT (OCTAVE_COUNT * KEYS_PER_OCTAVE)

#define WHITE_KEY_WIDTH 40
#define WHITE_KEY_HEIGHT 200
#define BLACK_KEY_WIDTH 22
#define BLACK_KEY_HEIGHT 130

struct PianoKey {
    int x, y;
    int width, height;
    COLORREF color;
    bool isBlack;
    int noteId;
    bool isPressed;
    char noteName[4]; // 音符名称
};

PianoKey keys[KEY_COUNT];

// 函数声明
void DrawKey(PianoKey& key);
void DrawPiano(PianoKey keys[], int keyCount);
void InitPianoKeys(PianoKey keys[]);
int GetClickedKey(PianoKey keys[], int keyCount, int mouseX, int mouseY);
void HandleMouseEvent(PianoKey keys[], int keyCount);
void PlayNote(int noteId);
void StopNote(int noteId);
void DrawPianoDecoration();

// 初始化图形界面
void InitGraphics() {
    int width = OCTAVE_COUNT * 7 * WHITE_KEY_WIDTH + 100; // 增加一些边距
    initgraph(width, 700); // 增加高度
    setbkcolor(RGB(50, 50, 60)); // 深灰色背景
    cleardevice();
    settextcolor(RGB(220, 220, 220));
    settextstyle(20, 0, "");
}

// 绘制UI
void DrawUI() {
    cleardevice();
    
    // 绘制钢琴装饰
    DrawPianoDecoration();
    
    // 绘制钢琴键盘
    DrawPiano(keys, KEY_COUNT);
    
    // 绘制标题
    settextcolor(RGB(240, 240, 240));
    settextstyle(24, 0, _T("宋体"));
    outtextxy((OCTAVE_COUNT * 7 * WHITE_KEY_WIDTH + 100)/2 - 100, 30, "虚拟钢琴模拟器");
    
    // 绘制说明文字
    settextstyle(16, 0, _T("宋体"));
    settextcolor(RGB(200, 200, 200));
    outtextxy(50, 450, "使用说明");
    outtextxy(50, 480, "1. 鼠标点击琴键演奏");
    outtextxy(50, 510, "2. ESC键退出程序");
    
    // 绘制作者信息
    settextstyle(12, 0, _T("楷体"));
    settextcolor(RGB(180, 180, 180));
    outtextxy(50, 620, "小组作者：balabala");
    
    // 绘制音符名称
    settextstyle(14, 0, _T("Arial"));
    settextcolor(RGB(180, 180, 180));
    for (int i = 0; i < KEY_COUNT; i++) {
        if (!keys[i].isBlack && keys[i].y == 100) {
            int textX = keys[i].x + keys[i].width/2 - 10;
            int textY = keys[i].y + keys[i].height + 10;
            outtextxy(textX, textY, keys[i].noteName);
        }
    }
}

int main() {
    InitGraphics();
    InitPianoKeys(keys);
    
    // 启用双缓冲
    BeginBatchDraw();
    
    // 初始绘制
    DrawUI();
    
    while (true) {
        // 处理鼠标事件
        HandleMouseEvent(keys, KEY_COUNT);
        
        // 检查退出键
        if (_kbhit()) {
            char ch = _getch();
            if (ch == 27) break; // ESC键退出
        }
        
        // 统一刷新到屏幕
        FlushBatchDraw();
        Sleep(10);
    }
    
    // 结束批量绘制
    EndBatchDraw();
    closegraph();
    return 0;
}

// 绘制钢琴装饰
void DrawPianoDecoration() {
    int width = OCTAVE_COUNT * 7 * WHITE_KEY_WIDTH + 100;
    
    // 绘制钢琴主体（深色木纹效果）
    setfillcolor(RGB(30, 20, 10));
    fillrectangle(30, 50, width - 30, 350);
    
    // 钢琴边框（金属质感）
    setlinecolor(RGB(180, 180, 200));
    setlinestyle(PS_SOLID, 3);
    rectangle(25, 45, width - 25, 355);
    
    // 钢琴键盘底座
    setfillcolor(RGB(40, 30, 20));
    fillrectangle(40, 320, width - 40, 340);
    
    // 钢琴支架
    setfillcolor(RGB(80, 60, 40));
    fillrectangle(100, 340, 150, 420);
    fillrectangle(width - 150, 340, width - 100, 420);
    
    // 踏板区域
    setfillcolor(RGB(50, 40, 30));
    fillrectangle(width/2 - 100, 420, width/2 + 100, 440);
    
    // 踏板
    setfillcolor(RGB(60, 60, 60));
    fillrectangle(width/2 - 80, 440, width/2 - 40, 460);
    fillrectangle(width/2 - 20, 440, width/2 + 20, 460);
    fillrectangle(width/2 + 40, 440, width/2 + 80, 460);
    
    // 恢复默认线型
    setlinestyle(PS_SOLID, 1);
}

// 绘制单个琴键
void DrawKey(PianoKey& key) {
    // 按下状态的颜色
    COLORREF fillColor;
    if (key.isPressed) {
        if (key.isBlack) {
            fillColor = RGB(80, 80, 80); // 按下的黑键
        } else {
            fillColor = RGB(230, 230, 240); // 按下的白键
        }
    } else {
        fillColor = key.color;
    }
    
    // 绘制琴键主体
    setfillcolor(fillColor);
    setlinecolor(BLACK);
    fillrectangle(key.x, key.y, 
                  key.x + key.width, 
                  key.y + key.height);
    
    // 白键特殊处理
    if (!key.isBlack) {
        // 白键底部阴影效果
        setlinecolor(RGB(100, 100, 100));
        line(key.x, key.y + key.height - 1, key.x + key.width, key.y + key.height - 1);
        
        // 白键右侧分割线（更粗更明显）
        setlinecolor(RGB(40, 40, 40));
        setlinestyle(PS_SOLID, 2);
        line(key.x + key.width - 1, key.y, key.x + key.width - 1, key.y + key.height);
        setlinestyle(PS_SOLID, 1);
        
        // 白键左上角高光
        setlinecolor(WHITE);
        line(key.x + 1, key.y + 1, key.x + key.width - 2, key.y + 1);
        line(key.x + 1, key.y + 1, key.x + 1, key.y + 10);
    } else {
        // 黑键光泽效果
        setlinecolor(RGB(60, 60, 60));
        rectangle(key.x + 1, key.y + 1, key.x + key.width - 1, key.y + key.height - 1);
        
        // 黑键顶部高光
        setlinecolor(RGB(100, 100, 100));
        line(key.x + 3, key.y + 3, key.x + key.width - 3, key.y + 3);
    }
}

// 绘制所有琴键
void DrawPiano(PianoKey keys[], int keyCount) {
    // 先绘制所有白键
    for (int i = 0; i < keyCount; i++) {
        if (!keys[i].isBlack) {
            DrawKey(keys[i]);
        }
    }
    
    // 再绘制所有黑键（覆盖在白键上面）
    for (int i = 0; i < keyCount; i++) {
        if (keys[i].isBlack) {
            DrawKey(keys[i]);
        }
    }
}

// 初始化钢琴键
void InitPianoKeys(PianoKey keys[]) {
    int keyIndex = 0;
    
    // 音符名称
    const char* noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    
    for (int octave = 0; octave < OCTAVE_COUNT; octave++) {
        int startX = 50 + octave * 7 * WHITE_KEY_WIDTH;
        int baseMidiNote = 36 + octave * 12; // C2开始
        
        // 定义每个键的类型：0=白键, 1=黑键
        int keyTypes[] = {0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0};
        
        // 白键在八度内的位置索引
        int whiteKeyPositions[] = {0, 1, 2, 3, 4, 5, 6};
        int whiteKeyIndex = 0;
        
        // 先创建白键
        for (int i = 0; i < 12; i++) {
            if (keyTypes[i] == 0) { // 白键
                keys[keyIndex].x = startX + whiteKeyPositions[whiteKeyIndex] * WHITE_KEY_WIDTH;
                keys[keyIndex].y = 100;
                keys[keyIndex].width = WHITE_KEY_WIDTH;
                keys[keyIndex].height = WHITE_KEY_HEIGHT;
                keys[keyIndex].color = RGB(250, 250, 255); // 稍微偏蓝的白键
                keys[keyIndex].isBlack = false;
                keys[keyIndex].noteId = baseMidiNote + i;
                keys[keyIndex].isPressed = false;
                
                // 设置音符名称
                sprintf(keys[keyIndex].noteName, "%s%d", noteNames[i], octave + 2);
                
                whiteKeyIndex++;
                keyIndex++;
            }
        }
        
        // 再创建黑键
        for (int i = 0; i < 12; i++) {
            if (keyTypes[i] == 1) { // 黑键
                // 黑键位于两个白键之间
                int whiteKeyPos = 0;
                for (int j = 0; j < i; j++) {
                    if (keyTypes[j] == 0) whiteKeyPos++;
                }
                
                keys[keyIndex].x = startX + whiteKeyPos * WHITE_KEY_WIDTH - BLACK_KEY_WIDTH / 2;
                keys[keyIndex].y = 100;
                keys[keyIndex].width = BLACK_KEY_WIDTH;
                keys[keyIndex].height = BLACK_KEY_HEIGHT;
                keys[keyIndex].color = RGB(30, 30, 30); // 深灰色黑键
                keys[keyIndex].isBlack = true;
                keys[keyIndex].noteId = baseMidiNote + i;
                keys[keyIndex].isPressed = false;
                
                // 设置音符名称
                sprintf(keys[keyIndex].noteName, "%s%d", noteNames[i], octave + 2);
                
                keyIndex++;
            }
        }
    }
}

// 获取点击的琴键
int GetClickedKey(PianoKey keys[], int keyCount, int mouseX, int mouseY) {
    // 先检查黑键（它们在白键上面）
    for (int i = 0; i < keyCount; i++) {
        if (keys[i].isBlack) {
            if (mouseX >= keys[i].x && mouseX <= keys[i].x + keys[i].width &&
                mouseY >= keys[i].y && mouseY <= keys[i].y + keys[i].height) {
                return i;
            }
        }
    }
    
    // 再检查白键
    for (int i = 0; i < keyCount; i++) {
        if (!keys[i].isBlack) {
            if (mouseX >= keys[i].x && mouseX <= keys[i].x + keys[i].width &&
                mouseY >= keys[i].y && mouseY <= keys[i].y + keys[i].height) {
                return i;
            }
        }
    }
    
    return -1;
}

// 处理鼠标事件
void HandleMouseEvent(PianoKey keys[], int keyCount) {
    MOUSEMSG msg;
    if (MouseHit()) {
        msg = GetMouseMsg();
        
        if (msg.uMsg == WM_LBUTTONDOWN || msg.uMsg == WM_LBUTTONUP) {
            int keyIndex = GetClickedKey(keys, keyCount, msg.x, msg.y);
            
            if (keyIndex != -1) {
                keys[keyIndex].isPressed = (msg.uMsg == WM_LBUTTONDOWN);
                
                if (msg.uMsg == WM_LBUTTONDOWN) {
                    PlayNote(keys[keyIndex].noteId);
                } else {
                    StopNote(keys[keyIndex].noteId);
                }
                
                // 重绘整个界面
                DrawUI();
            }
        }
    }
}

// 播放音符
void PlayNote(int noteId) {
    double frequency = 440.0 * pow(2.0, (noteId - 69) / 12.0);
    Beep((int)frequency, 300); // 缩短声音持续时间，使键盘响应更灵敏
}

// 停止音符
void StopNote(int noteId) {
    // 这里可以添加停止声音的逻辑
    // 由于Beep是阻塞的，目前不需要额外处理
}
