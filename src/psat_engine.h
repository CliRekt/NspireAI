#ifndef PSAT_ENGINE_H
#ifndef PSAT_ENGINE_H
#define PSAT_ENGINE_H

#include <cmath>
#include <stdio.h>

struct PSATResult {
    char title[32];
    char line1[64];
    char line2[64];
    char line3[64];
};

class PSATEngine {
public:
    // 1. Percentages & Growth
    static PSATResult solvePercent(float old_val, float new_val) {
        PSATResult res;
        snprintf(res.title, sizeof(res.title), "PSAT: PERCENT CHANGE");
        float pct_change = ((new_val - old_val) / old_val) * 100.0f;
        
        snprintf(res.line1, sizeof(res.line1), "OLD: %.2F  NEW: %.2F", old_val, new_val);
        snprintf(res.line2, sizeof(res.line2), "CHANGE: %+.2F%%", pct_change);
        snprintf(res.line3, sizeof(res.line3), "MULTIPLICATION FACTOR: %.4F", new_val / old_val);
        return res;
    }

    // 2. Geometry: Circle Sectors
    static PSATResult solveSector(float radius, float angle_deg) {
        PSATResult res;
        snprintf(res.title, sizeof(res.title), "PSAT: SECTOR & ARC");
        float rad_fraction = angle_deg / 360.0f;
        float arc_length = rad_fraction * 2.0f * 3.14159265f * radius;
        float sector_area = rad_fraction * 3.14159265f * radius * radius;

        snprintf(res.line1, sizeof(res.line1), "RADIUS: %.2F  ANGLE: %.1F DEG", radius, angle_deg);
        snprintf(res.line2, sizeof(res.line2), "ARC LENGTH  = %.3F", arc_length);
        snprintf(res.line3, sizeof(res.line3), "SECTOR AREA = %.3F", sector_area);
        return res;
    }

    // 3. Right Triangle Trig
    static PSATResult solveTrig(float opp, float adj) {
        PSATResult res;
        snprintf(res.title, sizeof(res.title), "PSAT: RIGHT TRIANGLE");
        float hyp = sqrtf(opp * opp + adj * adj);
        float angle_deg = atan2f(opp, adj) * (180.0f / 3.14159265f);

        snprintf(res.line1, sizeof(res.line1), "HYPOTENUSE = %.3F", hyp);
        snprintf(res.line2, sizeof(res.line2), "ANGLE THETA= %.2F DEG", angle_deg);
        snprintf(res.line3, sizeof(res.line3), "SIN=%.2F COS=%.2F TAN=%.2F", opp/hyp, adj/hyp, opp/adj);
        return res;
    }
};

#endif
