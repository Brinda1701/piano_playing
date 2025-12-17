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
    
    while (true) {
        DrawUI();
        HandleMouseEvent(keys, KEY_COUNT);
        
        if (_kbhit()) {
            char ch = _getch();
            if (ch == 27) break;
        }
        
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
        
        // 白键 (每个八度7个白键)
        for (int i = 0; i < 7; i++) {
            keys[keyIndex].x = startX + i * WHITE_KEY_WIDTH;
            keys[keyIndex].y = 100;
            keys[keyIndex].width = WHITE_KEY_WIDTH;
            keys[keyIndex].height = WHITE_KEY_HEIGHT;
            keys[keyIndex].color = WHITE;
            keys[keyIndex].isBlack = false;
            keys[keyIndex].noteId = keyIndex;
            keys[keyIndex].isPressed = false;
            keyIndex++;
        }
        
        // 黑键 (每个八度5个黑键)
        int blackPositions[] = {1, 2, 4, 5, 6};
        for (int i = 0; i < 5; i++) {
            int whiteIndex = blackPositions[i];
            keys[keyIndex].x = startX + whiteIndex * WHITE_KEY_WIDTH - BLACK_KEY_WIDTH / 2;
            keys[keyIndex].y = 100;
            keys[keyIndex].width = BLACK_KEY_WIDTH;
            keys[keyIndex].height = BLACK_KEY_HEIGHT;
            keys[keyIndex].color = BLACK;
            keys[keyIndex].isBlack = true;
            keys[keyIndex].noteId = keyIndex;
            keys[keyIndex].isPressed = false;
            keyIndex++;
        }
    }
}

int GetClickedKey(PianoKey keys[], int keyCount, int mouseX, int mouseY) {
    for (int i = 0; i < keyCount; i++) {
        if (keys[i].isBlack) {
            if (mouseX >= keys[i].x && mouseX <= keys[i].x + keys[i].width &&
                mouseY >= keys[i].y && mouseY <= keys[i].y + keys[i].height) {
                return i;
            }
        }
    }
    
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
    // C2 (MIDI note 36) 到 B5 (MIDI note 83)
    int midiNote = 36 + noteId;
    int frequency = 440 * pow(2.0, (midiNote - 69) / 12.0);
    Beep(frequency, 500);
}

void StopNote(int noteId) {
    // EasyX的Beep函数不支持实时停止，这里保持为空
    NULL;
}
