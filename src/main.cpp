#include <libndls.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <cctype>
#include "psat_engine.h"

extern "C" void _fini(void) {}

#define SCREEN_W 320
#define SCREEN_H 240
#define PI 3.14159265358979323846f
#define GRAVITY 9.80665f

static uint16_t framebuffer[SCREEN_W * SCREEN_H];

// Minimal 8x8 font rendering table (characters 32 to 90)
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

enum Mode {
    MODE_PHYSICS_2D,
    MODE_PSAT_PERCENT,
    MODE_PSAT_SECTOR,
    MODE_PSAT_TRIG,
    MODE_PSAT_QUADRATIC,
    MODE_COUNT
};

struct InputField {
    char label[16];
    char text[16];
    float getValue() const { return atof(text); }
};

class NspireAI {
private:
    Mode current_mode = MODE_PHYSICS_2D;
    int selected_field = 0;

    // Field storage per mode
    InputField proj_fields[3] = { {"V0 (M/S)", "25.0"}, {"ANG (DEG)", "35.0"}, {"Y0 (M)", "2.0"} };
    InputField pct_fields[2]  = { {"OLD VAL", "50.0"}, {"NEW VAL", "75.0"} };
    InputField sec_fields[2]  = { {"RADIUS", "10.0"}, {"ANGLE", "60.0"} };
    InputField trig_fields[2] = { {"OPPOSITE", "3.0"}, {"ADJACENT", "4.0"} };
    InputField quad_fields[3] = { {"A COEFF", "1.0"}, {"B COEFF", "-5.0"}, {"C COEFF", "6.0"} };

    void drawChar(int x, int y, char c, uint16_t color) {
        c = std::toupper(c);
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

    void processKeypad(InputField* fields, int num_fields) {
        if (isKeyPressed(KEY_NSPIRE_UP)) {
            selected_field = (selected_field - 1 + num_fields) % num_fields;
            msleep(150);
            return;
        }
        if (isKeyPressed(KEY_NSPIRE_DOWN)) {
            selected_field = (selected_field + 1) % num_fields;
            msleep(150);
            return;
        }

        InputField& active = fields[selected_field];
        int len = strlen(active.text);

        // Character entry
        char charToAppend = 0;
        if (isKeyPressed(KEY_NSPIRE_0)) charToAppend = '0';
        else if (isKeyPressed(KEY_NSPIRE_1)) charToAppend = '1';
        else if (isKeyPressed(KEY_NSPIRE_2)) charToAppend = '2';
        else if (isKeyPressed(KEY_NSPIRE_3)) charToAppend = '3';
        else if (isKeyPressed(KEY_NSPIRE_4)) charToAppend = '4';
        else if (isKeyPressed(KEY_NSPIRE_5)) charToAppend = '5';
        else if (isKeyPressed(KEY_NSPIRE_6)) charToAppend = '6';
        else if (isKeyPressed(KEY_NSPIRE_7)) charToAppend = '7';
        else if (isKeyPressed(KEY_NSPIRE_8)) charToAppend = '8';
        else if (isKeyPressed(KEY_NSPIRE_9)) charToAppend = '9';
        else if (isKeyPressed(KEY_NSPIRE_PERIOD)) charToAppend = '.';
        else if (isKeyPressed(KEY_NSPIRE_MINUS)) charToAppend = '-';

        if (charToAppend != 0 && len < 12) {
            active.text[len] = charToAppend;
            active.text[len + 1] = '\0';
            msleep(130);
        }

        // Backspace / Delete
        if (isKeyPressed(KEY_NSPIRE_DEL) && len > 0) {
            active.text[len - 1] = '\0';
            msleep(130);
        }
    }

    void renderFields(InputField* fields, int num_fields, int start_y) {
        for (int i = 0; i < num_fields; i++) {
            int y = start_y + (i * 22);
            uint16_t boxColor = (i == selected_field) ? 0xFFE0 : 0x39E7;
            uint16_t textColor = (i == selected_field) ? 0xFFFF : 0x9CD6;

            char buf[32];
            snprintf(buf, sizeof(buf), "%-9s: %s", fields[i].label, fields[i].text);
            drawText(15, y + 3, buf, textColor);
            drawRect(10, y, 200, 18, boxColor);
        }
    }

    void solve2DProjectile() {
        drawText(10, 8, "=== NSPIRE AI: 2D PROJECTILE ===", 0x07E0);
        renderFields(proj_fields, 3, 30);

        float v0 = proj_fields[0].getValue();
        float angle_deg = proj_fields[1].getValue();
        float y0 = proj_fields[2].getValue();

        float rad = angle_deg * (PI / 180.0f);
        float vx = v0 * cosf(rad);
        float vy0 = v0 * sinf(rad);

        float A = -0.5f * GRAVITY;
        float B = vy0;
        float C = y0;
        float disc = B * B - 4.0f * A * C;
        float t_flight = (disc >= 0) ? (-B - sqrtf(disc)) / (2.0f * A) : 0.0f;

        float range = vx * t_flight;
        float h_max = y0 + (vy0 * vy0) / (2.0f * GRAVITY);

        char buf[64];
        drawText(10, 105, "RESULTS:", 0x07FF);
        snprintf(buf, sizeof(buf), "VX=%.2F M/S  VY0=%.2F M/S", vx, vy0);
        drawText(10, 125, buf, 0xFFFF);
        snprintf(buf, sizeof(buf), "FLIGHT TIME = %.2F S", t_flight);
        drawText(10, 145, buf, 0xFFE0);
        snprintf(buf, sizeof(buf), "MAX HEIGHT  = %.2F M", h_max);
        drawText(10, 165, buf, 0xFFE0);
        snprintf(buf, sizeof(buf), "MAX RANGE   = %.2F M", range);
        drawText(10, 185, buf, 0xFFE0);
    }

    void solvePSATDisplay() {
        PSATResult res;
        InputField* fields = nullptr;
        int num_fields = 0;

        if (current_mode == MODE_PSAT_PERCENT) {
            fields = pct_fields; num_fields = 2;
            res = PSATEngine::solvePercent(pct_fields[0].getValue(), pct_fields[1].getValue());
        } else if (current_mode == MODE_PSAT_SECTOR) {
            fields = sec_fields; num_fields = 2;
            res = PSATEngine::solveSector(sec_fields[0].getValue(), sec_fields[1].getValue());
        } else if (current_mode == MODE_PSAT_TRIG) {
            fields = trig_fields; num_fields = 2;
            res = PSATEngine::solveTrig(trig_fields[0].getValue(), trig_fields[1].getValue());
        } else if (current_mode == MODE_PSAT_QUADRATIC) {
            fields = quad_fields; num_fields = 3;
            res = PSATEngine::solveQuadratic(quad_fields[0].getValue(), quad_fields[1].getValue(), quad_fields[2].getValue());
        }

        drawText(10, 8, res.title, 0x07E0);
        renderFields(fields, num_fields, 30);

        int res_y = 30 + (num_fields * 22) + 10;
        drawText(10, res_y, "RESULTS:", 0x07FF);
        drawText(10, res_y + 20, res.line1, 0xFFFF);
        drawText(10, res_y + 40, res.line2, 0xFFE0);
        drawText(10, res_y + 60, res.line3, 0x07FF);
    }

public:
    void run() {
        while (true) {
            if (isKeyPressed(KEY_NSPIRE_ESC)) break;

            // Mode switching via Left / Right
            if (isKeyPressed(KEY_NSPIRE_LEFT)) {
                current_mode = static_cast<Mode>((current_mode - 1 + MODE_COUNT) % MODE_COUNT);
                selected_field = 0;
                msleep(180);
            } else if (isKeyPressed(KEY_NSPIRE_RIGHT)) {
                current_mode = static_cast<Mode>((current_mode + 1) % MODE_COUNT);
                selected_field = 0;
                msleep(180);
            }

            // Keypad digit entry
            if (current_mode == MODE_PHYSICS_2D) processKeypad(proj_fields, 3);
            else if (current_mode == MODE_PSAT_PERCENT) processKeypad(pct_fields, 2);
            else if (current_mode == MODE_PSAT_SECTOR) processKeypad(sec_fields, 2);
            else if (current_mode == MODE_PSAT_TRIG) processKeypad(trig_fields, 2);
            else if (current_mode == MODE_PSAT_QUADRATIC) processKeypad(quad_fields, 3);

            memset(framebuffer, 0, sizeof(framebuffer));

            if (current_mode == MODE_PHYSICS_2D) {
                solve2DProjectile();
            } else {
                solvePSATDisplay();
            }

            drawText(5, 212, "L/R: MODE | UP/DN: FIELD | 0-9: EDIT", 0x7BEF);
            drawText(5, 226, "PRESS [ESC] TO EXIT", 0xF800);

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
