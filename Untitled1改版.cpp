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
};

PianoKey keys[KEY_COUNT];

void DrawKey(PianoKey& key);
void DrawPiano(PianoKey keys[], int keyCount);
void InitPianoKeys(PianoKey keys[]);
int GetClickedKey(PianoKey keys[], int keyCount, int mouseX, int mouseY);
void HandleMouseEvent(PianoKey keys[], int keyCount);
void PlayNote(int noteId);
void StopNote(int noteId);

void InitGraphics() {
    int width = OCTAVE_COUNT * 7 * WHITE_KEY_WIDTH;
    initgraph(width, 600);
    setbkcolor(RGB(240, 240, 245));
    cleardevice();
    settextcolor(BLACK);
    settextstyle(20, 0, "");
}

void DrawUI() {
    outtextxy(300, 30, "钢琴模拟系统");
    DrawPiano(keys, KEY_COUNT);
    
    settextstyle(16, 0, "");
    outtextxy(50, 350, "使用说明");
    outtextxy(50, 380, "1. 点击琴键演奏");
    outtextxy(50, 410, "2. ESC键退出");
}

int main() {
    InitGraphics();
    InitPianoKeys(keys);
    
    bool needRedraw = true;
    
    while (true) {
        if (needRedraw) {
            DrawUI();
            needRedraw = false;
        }//减少了刷新屏幕的次数 

        HandleMouseEvent(keys, KEY_COUNT);
        
        if (_kbhit()) {
            char ch = _getch();
            if (ch == 27) break;
        }//通过esc退出 
        
        Sleep(10);
    }
    
    closegraph();
    return 0;
}
void DrawKey(PianoKey& key) {
    COLORREF fillColor = key.isPressed ? (key.isBlack ? DARKGRAY : LIGHTGRAY) : key.color;
    
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
    for (int i = 0; i < keyCount; i++) {
        DrawKey(keys[i]);
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
                keys[keyIndex].isPressed = (msg.uMsg == WM_LBUTTONDOWN);
                
                if (msg.uMsg == WM_LBUTTONDOWN) {
                    PlayNote(keys[keyIndex].noteId);
                } else {
                    StopNote(keys[keyIndex].noteId);
                }
                
                cleardevice();
                DrawPiano(keys, keyCount);
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
