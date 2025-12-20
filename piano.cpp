#include <graphics.h>
#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include <math.h>

#define OCTAVE_COUNT 4
#define KEYS_PER_OCTAVE 12
#define KEY_COUNT (OCTAVE_COUNT * KEYS_PER_OCTAVE)

#define WHITE_KEY_WIDTH 40
#define WHITE_KEY_HEIGHT 200
#define BLACK_KEY_WIDTH 25
#define BLACK_KEY_HEIGHT 120

struct PianoKey {
    int x, y;
    int width, height;
    COLORREF color;
    bool isBlack;
    int noteId;
    bool isPressed;
};//定义键盘 

PianoKey keys[KEY_COUNT];

void DrawKey(PianoKey& key);//绘制单个钢琴琴键
void DrawPiano(PianoKey keys[], int keyCount);//绘制总的琴键 
void InitPianoKeys(PianoKey keys[]);//初始化黑白键 
int GetClickedKey(PianoKey keys[], int keyCount, int mouseX, int mouseY);//获取鼠标按键情况 
void HandleMouseEvent(PianoKey keys[], int keyCount);//跳转点击的对象 
void PlayNote(int noteId);//发音控制 
void StopNote(int noteId);//目前无意义 

void InitGraphics() {//绘制大的图表 
    int width = OCTAVE_COUNT * 7 * WHITE_KEY_WIDTH;
    initgraph(width, 600);
    setbkcolor(RGB(240, 240, 245));
    cleardevice();
    settextcolor(BLACK);
    settextstyle(20, 0, "");
}



void DrawUI(bool drawPiano = true) {
    cleardevice();
    
    settextcolor(BLACK);
    settextstyle(20, 0, "");
    outtextxy(300, 30, "钢琴模拟系统");
    
    // 根据参数决定是否绘制钢琴
    if (drawPiano) {
        DrawPiano(keys, KEY_COUNT);
    }
    
    // 绘制说明文字
    settextstyle(16, 0, "");
    outtextxy(50, 350, "使用说明");
    outtextxy(50, 380, "1. 点击琴键演奏");
    outtextxy(50, 410, "2. ESC键退出");
}



int main() {
    InitGraphics();
    InitPianoKeys(keys);
    // 初始绘制完整界面

    
    // 启用双缓冲 对于EasyX：BeginBatchDraw();
    BeginBatchDraw();
    
    while (true) {
		cleardevice();
		DrawUI(true);
        // 处理鼠标事件
        HandleMouseEvent(keys, KEY_COUNT);
        
        // 检查键盘输入
        if (_kbhit()) {
            char ch = _getch();
            if (ch == 27) break; // ESC键退出
        }
        
        // 小幅延迟减少CPU占用
        Sleep(10);
        
        // 统一刷新到屏幕
        FlushBatchDraw();
    }
    
    
    // 结束批量绘制
    EndBatchDraw();
    closegraph();
    return 0;
}

void DrawKey(PianoKey& key) {
    COLORREF fillColor = key.isPressed ? (key.isBlack ? DARKGRAY : LIGHTGRAY) : key.color;
    //呈现视觉反馈 
    setfillcolor(fillColor);
    setlinecolor(BLACK);
    fillrectangle(key.x, key.y, 
                  key.x + key.width, 
                  key.y + key.height);
    
    if (!key.isBlack) {
        setlinecolor(WHITE);
        line(key.x, key.y, key.x + key.width, key.y);
        line(key.x, key.y, key.x, key.y + key.height);
    }
}

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

void InitPianoKeys(PianoKey keys[]) {
    int keyIndex = 0;
    
    for (int octave = 0; octave < OCTAVE_COUNT; octave++) {
        int startX = octave * 7 * WHITE_KEY_WIDTH;
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
                keys[keyIndex].color = WHITE;
                keys[keyIndex].isBlack = false;
                keys[keyIndex].noteId = baseMidiNote + i;
                keys[keyIndex].isPressed = false;
                whiteKeyIndex++;
                keyIndex++;
            }
        }
        
        // 再创建黑键
        for (int i = 0; i < 12; i++) {
            if (keyTypes[i] == 1) { // 黑键
                // 黑键位于两个白键之间
                // 找到对应的白键位置
                int whiteKeyPos = 0;
                for (int j = 0; j < i; j++) {
                    if (keyTypes[j] == 0) whiteKeyPos++;
                }
                
                keys[keyIndex].x = startX + whiteKeyPos * WHITE_KEY_WIDTH - BLACK_KEY_WIDTH / 2;
                keys[keyIndex].y = 100;
                keys[keyIndex].width = BLACK_KEY_WIDTH;
                keys[keyIndex].height = BLACK_KEY_HEIGHT;
                keys[keyIndex].color = BLACK;
                keys[keyIndex].isBlack = true;
                keys[keyIndex].noteId = baseMidiNote + i;
                keys[keyIndex].isPressed = false;
                keyIndex++;
            }
        }
    }
}

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

void HandleMouseEvent(PianoKey keys[], int keyCount) {
    MOUSEMSG msg;
    if (MouseHit()) {
        msg = GetMouseMsg();
        
        if (msg.uMsg == WM_LBUTTONDOWN || msg.uMsg == WM_LBUTTONUP) {
            int keyIndex = GetClickedKey(keys, keyCount, msg.x, msg.y);
            
            if (keyIndex != -1) {
                
                // 设置新状态
                keys[keyIndex].isPressed = (msg.uMsg == WM_LBUTTONDOWN);
                
                // 只有状态改变时才播放声音和重绘
                if (keyIndex != -1) {
                	keys[keyIndex].isPressed = (msg.uMsg == WM_LBUTTONDOWN);
                
                	if (msg.uMsg == WM_LBUTTONDOWN) {
                    	PlayNote(keys[keyIndex].noteId);
                	} else {
                    	StopNote(keys[keyIndex].noteId);
                	}
                	// 不在这里绘制，由主循环统一绘制
            	}
            }
        }
    }
}

void PlayNote(int noteId) {
    double frequency = 440.0 * pow(2.0, (noteId - 69) / 12.0);
    Beep((int)frequency, 500);
}

void StopNote(int noteId) {
    NULL;
}
