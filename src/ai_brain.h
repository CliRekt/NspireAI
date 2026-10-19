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

    static void simplifyRadical(int n, int &out_outside, int &out_inside) {
        if (n < 0) n = -n;
        out_outside = 1;
        out_inside = n;
        for (int i = static_cast<int>(sqrt(n)); i >= 2; i--) {
            if (n % (i * i) == 0) {
                out_outside = i;
                out_inside = n / (i * i);
                break;
            }
        }
    }

    static int gcd(int a, int b) {
        a = abs(a); b = abs(b);
        while (b) { int t = b; b = a % b; a = t; }
        return a;
    }

    static bool tryUnitConversion(const char* lower, float val, char* out_reply, size_t max_len) {
        if (contains(lower, "mph to m/s") || contains(lower, "mph to ms")) {
            snprintf(out_reply, max_len, "CONV: %.2F MPH = %.2F M/S", val, val * 0.44704f);
            return true;
        }
        if (contains(lower, "m/s to mph") || contains(lower, "ms to mph")) {
            snprintf(out_reply, max_len, "CONV: %.2F M/S = %.2F MPH", val, val * 2.23694f);
            return true;
        }
        if (contains(lower, "kmh to m/s") || contains(lower, "km/h to m/s")) {
            snprintf(out_reply, max_len, "CONV: %.2F KM/H = %.2F M/S", val, val / 3.6f);
            return true;
        }
        if (contains(lower, "m to ft") || contains(lower, "meters to feet")) {
            snprintf(out_reply, max_len, "CONV: %.2F M = %.2F FT", val, val * 3.28084f);
            return true;
        }
        if (contains(lower, "ft to m") || contains(lower, "feet to meters")) {
            snprintf(out_reply, max_len, "CONV: %.2F FT = %.2F M", val, val / 3.28084f);
            return true;
        }
        if (contains(lower, "deg to rad") || contains(lower, "degrees to radians")) {
            snprintf(out_reply, max_len, "CONV: %.2F DEG = %.4F RAD", val, val * (PI / 180.0f));
            return true;
        }
        if (contains(lower, "rad to deg") || contains(lower, "radians to degrees")) {
            snprintf(out_reply, max_len, "CONV: %.4F RAD = %.2F DEG", val, val * (180.0f / PI));
            return true;
        }
        if (contains(lower, "kg to lbs") || contains(lower, "kg to lb")) {
            snprintf(out_reply, max_len, "CONV: %.2F KG = %.2F LBS", val, val * 2.20462f);
            return true;
        }
        if (contains(lower, "lbs to kg") || contains(lower, "lb to kg")) {
            snprintf(out_reply, max_len, "CONV: %.2F LBS = %.2F KG", val, val / 2.20462f);
            return true;
        }
        if (contains(lower, "c to f")) {
            snprintf(out_reply, max_len, "CONV: %.1F C = %.1F F", val, (val * 9.0f / 5.0f) + 32.0f);
            return true;
        }
        if (contains(lower, "f to c")) {
            snprintf(out_reply, max_len, "CONV: %.1F F = %.1F C", val, (val - 32.0f) * 5.0f / 9.0f);
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

        // 1. UNIT CONVERSIONS
        if (num_count >= 1 && tryUnitConversion(lower, nums[0], out_reply, max_len)) {
            return;
        }

        // 2. DELTAMATH: SIMPLIFYING RADICALS
        if (contains(lower, "rad") || contains(lower, "sqrt") || contains(lower, "simplify")) {
            int rad_val = (num_count >= 1) ? static_cast<int>(nums[0]) : 72;
            int out_a = 1, out_b = rad_val;
            simplifyRadical(rad_val, out_a, out_b);

            if (out_b == 1) {
                snprintf(out_reply, max_len, "RADICAL: SQRT(%d) = %d", rad_val, out_a);
            } else if (out_a == 1) {
                snprintf(out_reply, max_len, "RADICAL: SQRT(%d) (CANNOT BE SIMPLIFIED)", rad_val);
            } else {
                snprintf(out_reply, max_len, "RADICAL: SQRT(%d) = %d*SQRT(%d)", rad_val, out_a, out_b);
            }
            return;
        }

        // 3. DELTAMATH: COMPLETING THE SQUARE
        if (contains(lower, "cts") || contains(lower, "complete") || contains(lower, "square")) {
            float a = (num_count >= 1) ? nums[0] : 1.0f;
            float b = (num_count >= 2) ? nums[1] : 6.0f;
            float c = (num_count >= 3) ? nums[2] : -7.0f;

            float h = -b / (2.0f * a);
            float add_val = (b / (2.0f * a)) * (b / (2.0f * a));
            float rhs = add_val - (c / a);

            snprintf(out_reply, max_len, "CTS: (X - %.2F)^2 = %.2F | ADD TO BOTH SIDES = %.2F", -h, rhs, add_val);
            return;
        }

        // 4. DELTAMATH: FACTOR TRINOMIALS
        if (contains(lower, "factor")) {
            int a = (num_count >= 1) ? static_cast<int>(nums[0]) : 2;
            int b = (num_count >= 2) ? static_cast<int>(nums[1]) : 5;
            int c = (num_count >= 3) ? static_cast<int>(nums[2]) : -3;

            int disc = b * b - 4 * a * c;
            int sq = static_cast<int>(sqrt(disc));

            if (disc >= 0 && sq * sq == disc) {
                int num1 = -b + sq;
                int den1 = 2 * a;
                int g1 = gcd(num1, den1);
                int p1 = den1 / g1, q1 = num1 / g1;

                int num2 = -b - sq;
                int den2 = 2 * a;
                int g2 = gcd(num2, den2);
                int p2 = den2 / g2, q2 = num2 / g2;

                snprintf(out_reply, max_len, "FACTOR: (%dX %s %d)(%dX %s %d) | ROOTS: X = %.2F, %.2F",
                         p1, (q1 >= 0) ? "-" : "+", abs(q1),
                         p2, (q2 >= 0) ? "-" : "+", abs(q2),
                         static_cast<float>(q1)/p1, static_cast<float>(q2)/p2);
            } else {
                snprintf(out_reply, max_len, "FACTOR: NOT FACTORABLE OVER INTEGERS");
            }
            return;
        }

        // 5. DELTAMATH: DISCRIMINANT & NATURE OF ROOTS
        if (contains(lower, "nature") || contains(lower, "discrim")) {
            float a = (num_count >= 1) ? nums[0] : 1.0f;
            float b = (num_count >= 2) ? nums[1] : 2.0f;
            float c = (num_count >= 3) ? nums[2] : 5.0f;

            float disc = b * b - 4.0f * a * c;
            int sq = static_cast<int>(sqrt(fabsf(disc)));

            if (disc > 0) {
                if (sq * sq == static_cast<int>(disc)) {
                    snprintf(out_reply, max_len, "NATURE: DISC = %.0F > 0 | 2 REAL RATIONAL ROOTS", disc);
                } else {
                    snprintf(out_reply, max_len, "NATURE: DISC = %.0F > 0 | 2 REAL IRRATIONAL ROOTS", disc);
                }
            } else if (disc == 0) {
                snprintf(out_reply, max_len, "NATURE: DISC = 0 | 1 REAL DOUBLE ROOT");
            } else {
                snprintf(out_reply, max_len, "NATURE: DISC = %.0F < 0 | 2 COMPLEX ROOTS", disc);
            }
            return;
        }

        // 6. DELTAMATH: QUADRATIC WORD PROBLEMS (PROFIT / GRAVITY / HEIGHT)
        if (contains(lower, "profit") || contains(lower, "gravity") || contains(lower, "height")) {
            float a = (num_count >= 1) ? nums[0] : -16.0f;
            float b = (num_count >= 2) ? nums[1] : 64.0f;
            float c = (num_count >= 3) ? nums[2] : 80.0f;

            float t_peak = -b / (2.0f * a);
            float h_max = a * t_peak * t_peak + b * t_peak + c;

            float disc = b * b - 4.0f * a * c;
            float t_land = (disc >= 0) ? (-b - sqrtf(disc)) / (2.0f * a) : 0.0f;

            snprintf(out_reply, max_len, "WORD PROB: MAX=%.2F AT T=%.2FS | GROUND/ZERO AT T=%.2FS", h_max, t_peak, t_land);
            return;
        }

        // 7. GENERAL QUADRATIC SOLVER (EXACT RADICALS & DECIMALS)
        if (contains(lower, "quad") || contains(lower, "vertex") || contains(lower, "root") || contains(lower, "x^2") || contains(lower, "solve")) {
            float a = (num_count >= 1) ? nums[0] : 1.0f;
            float b = (num_count >= 2) ? nums[1] : -6.0f;
            float c = (num_count >= 3) ? nums[2] : 4.0f;

            float disc = b * b - 4.0f * a * c;
            float h = -b / (2.0f * a);
            float k = a * h * h + b * h + c;

            if (disc >= 0) {
                int out_a = 1, out_b = static_cast<int>(disc);
                simplifyRadical(static_cast<int>(disc), out_a, out_b);

                float x1 = (-b + sqrtf(disc)) / (2.0f * a);
                float x2 = (-b - sqrtf(disc)) / (2.0f * a);

                if (out_b == 1 || out_b == 0) {
                    snprintf(out_reply, max_len, "QUAD: X = %.2F, %.2F | VERTEX: (%.2F, %.2F)", x1, x2, h, k);
                } else {
                    snprintf(out_reply, max_len, "EXACT: (%.0F +/- %d*SQRT(%d))/%.0F | DEC: %.2F, %.2F", -b, out_a, out_b, 2.0f*a, x1, x2);
                }
            } else {
                int out_a = 1, out_b = static_cast<int>(-disc);
                simplifyRadical(static_cast<int>(-disc), out_a, out_b);
                float realPart = -b / (2.0f * a);
                float imagPart = sqrtf(-disc) / (2.0f * a);

                snprintf(out_reply, max_len, "COMPLEX: X = %.2F +/- %.2FI | EXACT: (%.0F +/- %d*SQRT(%d)I)/%.0F", realPart, imagPart, -b, out_a, out_b, 2.0f*a);
            }
            return;
        }

        // 8. PHYSICS & TRIG
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

            snprintf(out_reply, max_len, "PHYSICS 2D: TIME=%.2FS | RANGE=%.2FM | MAX H=%.2FM", t_flight, range, h_max);
            return;
        }

        if (contains(lower, "trig") || contains(lower, "triangle") || contains(lower, "hypotenuse") || contains(lower, "sin")) {
            float opp = (num_count >= 1) ? nums[0] : 3.0f;
            float adj = (num_count >= 2) ? nums[1] : 4.0f;

            float hyp = sqrtf(opp * opp + adj * adj);
            float theta = atan2f(opp, adj) * (180.0f / PI);

            snprintf(out_reply, max_len, "TRIG: HYP=%.2F | THETA=%.1F DEG | SIN=%.2F, COS=%.2F, TAN=%.2F", hyp, theta, opp/hyp, adj/hyp, opp/adj);
            return;
        }

        // Fallback
        snprintf(out_reply, max_len, "TRY: 'FACTOR 2 5 -3' | 'CTS 1 6 -7' | 'RAD 72' | 'QUAD 1 -6 4'");
    }
};

#endif
