#include <libndls.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <cctype>
#include "ai_brain.h"

extern "C" void _fini(void) {}

#define SCREEN_W 320
#define SCREEN_H 240
#define MAX_HISTORY 20
#define VISIBLE_MSG_COUNT 5

// Dark Aesthetic Palette
#define COLOR_BG          0x0821  // Charcoal
#define COLOR_HEADER_BG   0x10A2  // Dark Slate Header
#define COLOR_CYAN        0x07FF  // Neon Cyan
#define COLOR_GREEN       0x07E0  // Online Green
#define COLOR_GOLD        0xFFE0  // User Text Gold
#define COLOR_WHITE       0xFFFF  // Pure White
#define COLOR_GRAY        0x7BEF  // Text Gray
#define COLOR_DARK_GRAY   0x31E6  // Muted Border
#define COLOR_INPUT_BG    0x1885  // Input Box Dark Blue
#define COLOR_MENU_BG     0x0842  // Menu Overlay Box

static uint16_t framebuffer[SCREEN_W * SCREEN_H];

static const uint8_t font8x8[59][8] = {
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, {0x18,0x3C,0x3C,0x18,0x18,0x00,0x18,0x00},
    {0x66,0x66,0x24,0x00,0x00,0x00,0x00,0x00}, {0x6C,0x6C,0xFE,0x6C,0xFE,0x6C,0x6C,0x00},
    {0x18,0x3E,0x60,0x3C,0x06,0x7C,0x18,0x00}, {0x00,0x66,0x6C,0x18,0x30,0x66,0x46,0x00},
    {0x38,0x6C,0x38,0x76,0xDC,0xCC,0x76,0x00}, {0x18,0x18,0x30,0x00,0x00,0x00,0x00,0x00},
    {0x0C,0x18,0x30,0x30,0x30,0x18,0x0C,0x00}, {0x30,0x18,0x0C,0x0C,0x0C,0x18,0x30,0x00},
    {0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00}, {0x00,0x18,0x18,0x7E,0x18,0x18,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x30}, {0x00,0x00,0x00,0x7E,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00}, {0x06,0x0C,0x18,0x30,0x60,0xC0,0x80,0x00},
    {0x3C,0x66,0x6E,0x76,0x66,0x66,0x3C,0x00}, {0x18,0x38,0x18,0x18,0x18,0x18,0x7E,0x00},
    {0x3C,0x66,0x0C,0x18,0x30,0x60,0x7E,0x00}, {0x3C,0x66,0x06,0x1C,0x06,0x66,0x3C,0x00},
    {0x0C,0x1C,0x3C,0x6C,0x7E,0x0C,0x0C,0x00}, {0x7E,0x60,0x7C,0x06,0x06,0x66,0x3C,0x00},
    {0x3C,0x66,0x60,0x7C,0x66,0x66,0x3C,0x00}, {0x7E,0x06,0x0C,0x18,0x30,0x30,0x30,0x00},
    {0x3C,0x66,0x66,0x3C,0x66,0x66,0x3C,0x00}, {0x3C,0x66,0x66,0x3E,0x06,0x66,0x3C,0x00},
    {0x00,0x18,0x18,0x00,0x18,0x18,0x00,0x00}, {0x00,0x18,0x18,0x00,0x18,0x18,0x30,0x00},
    {0x0C,0x18,0x30,0x60,0x30,0x18,0x0C,0x00}, {0x00,0x00,0x7E,0x00,0x7E,0x00,0x00,0x00},
    {0x30,0x18,0x0C,0x06,0x0C,0x18,0x30,0x00}, {0x3C,0x66,0x0C,0x18,0x18,0x00,0x18,0x00},
    {0x3C,0x66,0x6E,0x6E,0x60,0x62,0x3C,0x00}, {0x18,0x3C,0x66,0x66,0x7E,0x66,0x66,0x00},
    {0x7C,0x66,0x66,0x7C,0x66,0x66,0x7C,0x00}, {0x3C,0x66,0x60,0x60,0x60,0x66,0x3C,0x00},
    {0x78,0x6C,0x66,0x66,0x66,0x6C,0x78,0x00}, {0x7E,0x60,0x60,0x7C,0x60,0x60,0x7E,0x00},
    {0x7E,0x60,0x60,0x7C,0x60,0x60,0x60,0x00}, {0x3C,0x66,0x60,0x6E,0x66,0x66,0x3C,0x00},
    {0x66,0x66,0x66,0x7E,0x66,0x66,0x66,0x00}, {0x3C,0x18,0x18,0x18,0x18,0x18,0x3C,0x00},
    {0x1E,0x0C,0x0C,0x0C,0x0C,0x6C,0x38,0x00}, {0x66,0x6C,0x78,0x70,0x78,0x6C,0x66,0x00},
    {0x60,0x60,0x60,0x60,0x60,0x60,0x7E,0x00}, {0x63,0x77,0x7F,0x6B,0x63,0x63,0x63,0x00},
    {0x66,0x76,0x7E,0x7E,0x6E,0x66,0x66,0x00}, {0x3C,0x66,0x66,0x66,0x66,0x66,0x3C,0x00},
    {0x7C,0x66,0x66,0x7C,0x60,0x60,0x60,0x00}, {0x3C,0x66,0x66,0x66,0x6A,0x6C,0x36,0x00},
    {0x7C,0x66,0x66,0x7C,0x6C,0x66,0x66,0x00}, {0x3C,0x66,0x60,0x3C,0x06,0x66,0x3C,0x00},
    {0x7E,0x18,0x18,0x18,0x18,0x18,0x18,0x00}, {0x66,0x66,0x66,0x66,0x66,0x66,0x3C,0x00},
    {0x66,0x66,0x66,0x66,0x66,0x3C,0x18,0x00}, {0x63,0x63,0x63,0x6B,0x7F,0x77,0x63,0x00},
    {0x66,0x66,0x3C,0x18,0x3C,0x66,0x66,0x00}, {0x66,0x66,0x66,0x3C,0x18,0x18,0x18,0x00},
    {0x7E,0x06,0x0C,0x18,0x30,0x60,0x7E,0x00}
};

struct Message {
    char sender[8];
    char text[128];
};

struct KeyMap {
    t_key key;
    char ch;
};

static const KeyMap key_mappings[] = {
    {KEY_NSPIRE_A, 'A'}, {KEY_NSPIRE_B, 'B'}, {KEY_NSPIRE_C, 'C'}, {KEY_NSPIRE_D, 'D'},
    {KEY_NSPIRE_E, 'E'}, {KEY_NSPIRE_F, 'F'}, {KEY_NSPIRE_G, 'G'}, {KEY_NSPIRE_H, 'H'},
    {KEY_NSPIRE_I, 'I'}, {KEY_NSPIRE_J, 'J'}, {KEY_NSPIRE_K, 'K'}, {KEY_NSPIRE_L, 'L'},
    {KEY_NSPIRE_M, 'M'}, {KEY_NSPIRE_N, 'N'}, {KEY_NSPIRE_O, 'O'}, {KEY_NSPIRE_P, 'P'},
    {KEY_NSPIRE_Q, 'Q'}, {KEY_NSPIRE_R, 'R'}, {KEY_NSPIRE_S, 'S'}, {KEY_NSPIRE_T, 'T'},
    {KEY_NSPIRE_U, 'U'}, {KEY_NSPIRE_V, 'V'}, {KEY_NSPIRE_W, 'W'}, {KEY_NSPIRE_X, 'X'},
    {KEY_NSPIRE_Y, 'Y'}, {KEY_NSPIRE_Z, 'Z'},
    {KEY_NSPIRE_0, '0'}, {KEY_NSPIRE_1, '1'}, {KEY_NSPIRE_2, '2'}, {KEY_NSPIRE_3, '3'},
    {KEY_NSPIRE_4, '4'}, {KEY_NSPIRE_5, '5'}, {KEY_NSPIRE_6, '6'}, {KEY_NSPIRE_7, '7'},
    {KEY_NSPIRE_8, '8'}, {KEY_NSPIRE_9, '9'},
    {KEY_NSPIRE_SPACE, ' '}, {KEY_NSPIRE_PERIOD, '.'}, {KEY_NSPIRE_MINUS, '-'}
};

class NspireAI {
private:
    char prompt_buffer[40] = "";
    Message chat_history[MAX_HISTORY];
    int msg_count = 0;
    int scroll_offset = 0;
    bool show_menu = false;
    unsigned int frame_ticks = 0;

    void drawChar(int x, int y, char c, uint16_t color) {
        c = static_cast<char>(std::toupper(c));
        if (c < 32 || c > 90) c = '?';
        int idx = c - 32;
        for (int r = 0; r < 8; r++) {
            uint8_t bits = font8x8[idx][r];
            for (int col = 0; col < 8; col++) {
                if (bits & (0x80 >> col)) {
                    int px = x + col;
                    int py = y + r;
                    if (px >= 0 && px < SCREEN_W && py >= 0 && py < SCREEN_H) {
                        framebuffer[py * SCREEN_W + px] = color;
                    }
                }
            }
        }
    }

    void drawText(int x, int y, const char* str, uint16_t color) {
        while (*str) {
            drawChar(x, y, *str, color);
            x += 8;
            str++;
        }
    }

    void fillRect(int x, int y, int w, int h, uint16_t color) {
        for (int j = y; j < y + h; j++) {
            for (int i = x; i < x + w; i++) {
                if (i >= 0 && i < SCREEN_W && j >= 0 && j < SCREEN_H) {
                    framebuffer[j * SCREEN_W + i] = color;
                }
            }
        }
    }

    void drawRect(int x, int y, int w, int h, uint16_t color) {
        for (int j = y; j < y + h; j++) {
            for (int i = x; i < x + w; i++) {
                if (i >= 0 && i < SCREEN_W && j >= 0 && j < SCREEN_H) {
                    if (i == x || i == x + w - 1 || j == y || j == y + h - 1) {
                        framebuffer[j * SCREEN_W + i] = color;
                    }
                }
            }
        }
    }

    void drawHLine(int x, int y, int w, uint16_t color) {
        for (int i = x; i < x + w; i++) {
            if (i >= 0 && i < SCREEN_W && y >= 0 && y < SCREEN_H) {
                framebuffer[y * SCREEN_W + i] = color;
            }
        }
    }

    void addMessage(const char* sender, const char* text) {
        if (msg_count < MAX_HISTORY) {
            strncpy(chat_history[msg_count].sender, sender, 7);
            strncpy(chat_history[msg_count].text, text, 127);
            msg_count++;
        } else {
            for (int i = 0; i < MAX_HISTORY - 1; i++) {
                chat_history[i] = chat_history[i + 1];
            }
            strncpy(chat_history[MAX_HISTORY - 1].sender, sender, 7);
            strncpy(chat_history[MAX_HISTORY - 1].text, text, 127);
        }
        scroll_offset = 0; // Auto scroll to bottom
    }

    void handleKeyboard() {
        int len = strlen(prompt_buffer);

        // Toggle Quick Menu
        if (isKeyPressed(KEY_NSPIRE_MENU)) {
            show_menu = !show_menu;
            msleep(180);
            return;
        }

        // Scroll Chat History (Up/Down Arrows)
        if (isKeyPressed(KEY_NSPIRE_UP)) {
            if (scroll_offset < msg_count - VISIBLE_MSG_COUNT) scroll_offset++;
            msleep(120);
            return;
        }
        if (isKeyPressed(KEY_NSPIRE_DOWN)) {
            if (scroll_offset > 0) scroll_offset--;
            msleep(120);
            return;
        }

        size_t num_keys = sizeof(key_mappings) / sizeof(key_mappings[0]);
        for (size_t i = 0; i < num_keys; i++) {
            if (isKeyPressed(key_mappings[i].key)) {
                if (len < 32) {
                    prompt_buffer[len] = key_mappings[i].ch;
                    prompt_buffer[len + 1] = '\0';
                    msleep(110);
                    return;
                }
            }
        }

        if (isKeyPressed(KEY_NSPIRE_DEL) && len > 0) {
            prompt_buffer[len - 1] = '\0';
            msleep(110);
            return;
        }

        if (isKeyPressed(KEY_NSPIRE_ENTER) && len > 0) {
            addMessage("YOU", prompt_buffer);

            char ai_reply[128];
            AIBrain::ask(prompt_buffer, ai_reply, sizeof(ai_reply));
            addMessage("AI", ai_reply);

            prompt_buffer[0] = '\0';
            msleep(180);
            return;
        }
    }

public:
    NspireAI() {
        addMessage("AI", "NSPIRE AI ASSISTANT ONLINE.");
        addMessage("AI", "TYPE YOUR MATH PROBLEM OR QUERY!");
        addMessage("AI", "PRESS [MENU] FOR QUICK HELP.");
    }

    void run() {
        while (true) {
            if (isKeyPressed(KEY_NSPIRE_ESC)) break;

            frame_ticks++;
            handleKeyboard();

            fillRect(0, 0, SCREEN_W, SCREEN_H, COLOR_BG);

            // 1. Header Bar
            fillRect(0, 0, SCREEN_W, 22, COLOR_HEADER_BG);
            drawText(8, 7, "NSPIRE AI", COLOR_CYAN);
            drawText(225, 7, "[ONLINE]", COLOR_GREEN);
            drawHLine(0, 22, SCREEN_W, COLOR_CYAN);

            // 2. Chat Log Rendering with Up/Down Scroll
            int y_off = 30;
            int start_idx = msg_count - VISIBLE_MSG_COUNT - scroll_offset;
            if (start_idx < 0) start_idx = 0;
            int end_idx = start_idx + VISIBLE_MSG_COUNT;
            if (end_idx > msg_count) end_idx = msg_count;

            for (int i = start_idx; i < end_idx; i++) {
                bool is_user = (strcmp(chat_history[i].sender, "YOU") == 0);
                uint16_t color = is_user ? COLOR_GOLD : COLOR_CYAN;

                char line[140];
                snprintf(line, sizeof(line), "[%s] %s", chat_history[i].sender, chat_history[i].text);
                drawText(8, y_off, line, color);
                y_off += 28;
            }

            // 3. Input Box with Blinking Cursor
            fillRect(6, 178, 308, 26, COLOR_INPUT_BG);
            drawRect(6, 178, 308, 26, COLOR_CYAN);

            bool cursor_blink = (frame_ticks / 10) % 2 == 0;
            if (strlen(prompt_buffer) == 0) {
                drawText(14, 187, cursor_blink ? "> _" : "> ", COLOR_DARK_GRAY);
            } else {
                char display_p[48];
                snprintf(display_p, sizeof(display_p), "> %s%s", prompt_buffer, cursor_blink ? "_" : " ");
                drawText(14, 187, display_p, COLOR_WHITE);
            }

            // 4. Footer Guide Bar
            drawHLine(0, 214, SCREEN_W, COLOR_DARK_GRAY);
            drawText(8, 222, "[MENU] HELP  [UP/DN] SCROLL  [ESC] EXIT", COLOR_GRAY);

            // 5. Quick Help Menu Overlay (Triggered by [MENU] key)
            if (show_menu) {
                fillRect(20, 30, 280, 140, COLOR_MENU_BG);
                drawRect(20, 30, 280, 140, COLOR_CYAN);
                drawText(30, 38, "=== QUICK HELP & COMMANDS ===", COLOR_GOLD);
                drawText(30, 58, "* FACTOR: 'FACTOR 2 5 -3'", COLOR_WHITE);
                drawText(30, 74, "* COMPLETE SQ: 'CTS 1 6 -7'", COLOR_WHITE);
                drawText(30, 90, "* RADICALS: 'RAD 72'", COLOR_WHITE);
                drawText(30, 106, "* QUAD SOLVE: 'QUAD 1 -6 4'", COLOR_WHITE);
                drawText(30, 122, "* UNITS: '60 MPH TO M/S'", COLOR_WHITE);
                drawText(30, 145, "PRESS [MENU] TO CLOSE", COLOR_CYAN);
            }

            lcd_blit(framebuffer, SCR_320x240_565);
            msleep(20);
        }
    }
};

int main() {
    NspireAI app;
    app.run();
    return 0;
}
