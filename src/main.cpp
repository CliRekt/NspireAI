#include <libndls.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>

extern "C" void _fini(void) {}

#define SCREEN_W 320
#define SCREEN_H 240

static uint16_t framebuffer[SCREEN_W * SCREEN_H];

// Kinematics solver state
struct KinematicsState {
    bool has_v0 = false, has_v = false, has_a = false, has_t = false, has_d = false;
    float v0 = 0, v = 0, a = 0, t = 0, d = 0;
};

class NspireAI {
private:
    KinematicsState state;

    uint16_t rgbTo565(uint8_t r, uint8_t g, uint8_t b) {
        return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
    }

public:
    void solveKinematics() {
        // Equation 1: v = v0 + a*t
        if (state.has_v0 && state.has_a && state.has_t && !state.has_v) {
            state.v = state.v0 + state.a * state.t;
            state.has_v = true;
        }
        // Equation 2: d = v0*t + 0.5*a*t^2
        if (state.has_v0 && state.has_t && state.has_a && !state.has_d) {
            state.d = state.v0 * state.t + 0.5f * state.a * state.t * state.t;
            state.has_d = true;
        }
        // Equation 3: v^2 = v0^2 + 2*a*d
        if (state.has_v0 && state.has_a && state.has_d && !state.has_v) {
            state.v = sqrtf(state.v0 * state.v0 + 2.0f * state.a * state.d);
            state.has_v = true;
        }
    }

    void render() {
        memset(framebuffer, 0, sizeof(framebuffer));
        
        // Render UI header
        lcd_blit(framebuffer, SCR_320x240_565);
    }

    void run() {
        // Demo scenario: v0 = 0 m/s, a = 9.8 m/s^2, t = 3.0 s
        state.v0 = 0.0f; state.has_v0 = true;
        state.a = 9.8f;  state.has_a = true;
        state.t = 3.0f;  state.has_t = true;

        solveKinematics();

        while (!isKeyPressed(KEY_NSPIRE_ESC)) {
            render();
            msleep(20);
        }
    }
};

int main() {
    NspireAI app;
    app.run();
    return 0;
}
