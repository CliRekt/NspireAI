#ifndef AI_BRAIN_H
#define AI_BRAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <cctype>

#define PI 3.14159265358979323846f
#define GRAVITY 9.80665f

class AIBrain {
private:
    static bool contains(const char* haystack, const char* needle) {
        return strstr(haystack, needle) != NULL;
    }

    static int extractNumbers(const char* str, float* out_nums, int max_nums) {
        int count = 0;
        const char* p = str;
        while (*p && count < max_nums) {
            if (isdigit(*p) || (*p == '-' && isdigit(*(p + 1)))) {
                out_nums[count++] = static_cast<float>(atof(p));
                if (*p == '-') p++;
                while (*p && (isdigit(*p) || *p == '.')) p++;
            } else {
                p++;
            }
        }
        return count;
    }

    static bool tryUnitConversion(const char* lower, float val, char* out_reply, size_t max_len) {
        if (contains(lower, "mph to m/s") || contains(lower, "mph to ms")) {
            snprintf(out_reply, max_len, "CONVERSION: %.2F MPH = %.2F M/S", val, val * 0.44704f);
            return true;
        }
        if (contains(lower, "m/s to mph") || contains(lower, "ms to mph")) {
            snprintf(out_reply, max_len, "CONVERSION: %.2F M/S = %.2F MPH", val, val * 2.23694f);
            return true;
        }
        if (contains(lower, "kmh to m/s") || contains(lower, "km/h to m/s")) {
            snprintf(out_reply, max_len, "CONVERSION: %.2F KM/H = %.2F M/S", val, val / 3.6f);
            return true;
        }
        if (contains(lower, "m to ft") || contains(lower, "meters to feet")) {
            snprintf(out_reply, max_len, "CONVERSION: %.2F M = %.2F FT", val, val * 3.28084f);
            return true;
        }
        if (contains(lower, "ft to m") || contains(lower, "feet to meters")) {
            snprintf(out_reply, max_len, "CONVERSION: %.2F FT = %.2F M", val, val / 3.28084f);
            return true;
        }
        if (contains(lower, "deg to rad") || contains(lower, "degrees to radians")) {
            snprintf(out_reply, max_len, "CONVERSION: %.2F DEG = %.4F RAD", val, val * (PI / 180.0f));
            return true;
        }
        if (contains(lower, "rad to deg") || contains(lower, "radians to degrees")) {
            snprintf(out_reply, max_len, "CONVERSION: %.4F RAD = %.2F DEG", val, val * (180.0f / PI));
            return true;
        }
        if (contains(lower, "kg to lbs") || contains(lower, "kg to lb")) {
            snprintf(out_reply, max_len, "CONVERSION: %.2F KG = %.2F LBS", val, val * 2.20462f);
            return true;
        }
        if (contains(lower, "lbs to kg") || contains(lower, "lb to kg")) {
            snprintf(out_reply, max_len, "CONVERSION: %.2F LBS = %.2F KG", val, val / 2.20462f);
            return true;
        }
        if (contains(lower, "c to f")) {
            snprintf(out_reply, max_len, "CONVERSION: %.1F C = %.1F F", val, (val * 9.0f / 5.0f) + 32.0f);
            return true;
        }
        if (contains(lower, "f to c")) {
            snprintf(out_reply, max_len, "CONVERSION: %.1F F = %.1F C", val, (val - 32.0f) * 5.0f / 9.0f);
            return true;
        }
        return false;
    }

public:
    static void ask(const char* prompt, char* out_reply, size_t max_len) {
        char lower[128];
        int i = 0;
        for (; prompt[i] && i < 127; i++) {
            lower[i] = static_cast<char>(tolower(prompt[i]));
        }
        lower[i] = '\0';

        float nums[10];
        int num_count = extractNumbers(lower, nums, 10);

        if (num_count >= 1 && tryUnitConversion(lower, nums[0], out_reply, max_len)) {
            return;
        }

        // Physics: 2D Motion
        if (contains(lower, "project") || contains(lower, "launch") || contains(lower, "angle") || contains(lower, "v0")) {
            float v0 = (num_count >= 1) ? nums[0] : 25.0f;
            float angle = (num_count >= 2) ? nums[1] : 45.0f;
            float y0 = (num_count >= 3) ? nums[2] : 0.0f;

            float rad = angle * (PI / 180.0f);
            float vx = v0 * cosf(rad);
            float vy0 = v0 * sinf(rad);
            float disc = vy0 * vy0 + 2.0f * GRAVITY * y0;
            float t_flight = (vy0 + sqrtf(disc)) / GRAVITY;
            float range = vx * t_flight;
            float h_max = y0 + (vy0 * vy0) / (2.0f * GRAVITY);

            snprintf(out_reply, max_len, "2D MOTION: TIME=%.2FS | RANGE=%.2FM | MAX H=%.2FM (VX=%.1F, VY=%.1F)", t_flight, range, h_max, vx, vy0);
            return;
        }

        // Physics: Force (F = m * a)
        if (contains(lower, "force") || contains(lower, "f=ma") || contains(lower, "accel")) {
            float m = (num_count >= 1) ? nums[0] : 10.0f;
            float a = (num_count >= 2) ? nums[1] : 9.8f;
            snprintf(out_reply, max_len, "FORCE (F=MA): F = %.2F N (MASS=%.1FKG, A=%.2FM/S^2)", m * a, m, a);
            return;
        }

        // Algebra II: Quadratics
        if (contains(lower, "quad") || contains(lower, "vertex") || contains(lower, "root") || contains(lower, "x^2")) {
            float a = (num_count >= 1) ? nums[0] : 1.0f;
            float b = (num_count >= 2) ? nums[1] : -5.0f;
            float c = (num_count >= 3) ? nums[2] : 6.0f;

            float disc = b * b - 4.0f * a * c;
            float h = -b / (2.0f * a);
            float k = a * h * h + b * h + c;

            if (disc >= 0) {
                float x1 = (-b + sqrtf(disc)) / (2.0f * a);
                float x2 = (-b - sqrtf(disc)) / (2.0f * a);
                snprintf(out_reply, max_len, "QUAD: ROOTS X1=%.2F, X2=%.2F | VERTEX (H,K)=(%.2F, %.2F)", x1, x2, h, k);
            } else {
                snprintf(out_reply, max_len, "QUAD: DISC=%.2F (NO REAL ROOTS) | VERTEX (H,K)=(%.2F, %.2F)", disc, h, k);
            }
            return;
        }

        // Algebra II / Trig: Right Triangles
        if (contains(lower, "trig") || contains(lower, "triangle") || contains(lower, "hypotenuse") || contains(lower, "sin")) {
            float opp = (num_count >= 1) ? nums[0] : 3.0f;
            float adj = (num_count >= 2) ? nums[1] : 4.0f;

            float hyp = sqrtf(opp * opp + adj * adj);
            float theta = atan2f(opp, adj) * (180.0f / PI);

            snprintf(out_reply, max_len, "TRIG: HYP=%.2F | THETA=%.1F DEG | SIN=%.2F, COS=%.2F, TAN=%.2F", hyp, theta, opp/hyp, adj/hyp, opp/adj);
            return;
        }

        // Algebra II: Logarithms
        if (contains(lower, "log") || contains(lower, "ln")) {
            float base = (num_count >= 2) ? nums[0] : 10.0f;
            float val = (num_count >= 2) ? nums[1] : ((num_count == 1) ? nums[0] : 100.0f);
            if (num_count == 1) base = 10.0f;

            float res = logf(val) / logf(base);
            snprintf(out_reply, max_len, "LOG: LOG_%.1F(%.1F) = %.4F", base, val, res);
            return;
        }

        snprintf(out_reply, max_len, "TRY: 'LAUNCH 25 35' | 'QUAD 1 -5 6' | 'LOG 2 8' | '60 MPH TO M/S'");
    }
};

#endif
