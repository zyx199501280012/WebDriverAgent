// FBPhysicalTouch.m

#import "FBPhysicalTouch.h"
#import <math.h>

// ── 基础随机分布 ──────────────────────────────────────────────────

/** Box-Muller 变换：生成标准正态随机数 N(0,1) */
static double fb_gaussian(void) {
    static BOOL hasSpare = NO;
    static double spare;
    if (hasSpare) {
        hasSpare = NO;
        return spare;
    }
    double u, v, s;
    do {
        u = (double)arc4random() / UINT32_MAX * 2.0 - 1.0;
        v = (double)arc4random() / UINT32_MAX * 2.0 - 1.0;
        s = u * u + v * v;
    } while (s >= 1.0 || s == 0.0);
    double mul = sqrt(-2.0 * log(s) / s);
    spare = v * mul;
    hasSpare = YES;
    return u * mul;
}

/** 均匀分布 [lo, hi] */
static double fb_uniform(double lo, double hi) {
    return lo + (double)arc4random() / UINT32_MAX * (hi - lo);
}

/** Log-normal：median=exp(mu), sigma 为对数标准差 */
static double fb_lognormal(double mu, double sigma) {
    return exp(mu + sigma * fb_gaussian());
}

/** 截断到 [lo, hi] */
static double fb_clamp(double v, double lo, double hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

// ── FBPhysicalTouch ──────────────────────────────────────────────

@implementation FBPhysicalTouch

+ (FBTouchPhysics)randomParams {
    FBTouchPhysics p;

    // Force：log-normal(μ=log(0.4), σ=0.3)，截断到 [0.15, 0.70]
    p.force = (CGFloat)fb_clamp(fb_lognormal(log(0.4), 0.3), 0.15, 0.70);

    // MajorRadius：N(13, 2.5²)，截断到 [8, 18]
    p.majorRadius = (CGFloat)fb_clamp(13.0 + 2.5 * fb_gaussian(), 8.0, 18.0);

    // MinorRadius：majorRadius × U(0.62, 0.78)
    p.minorRadius = (CGFloat)(p.majorRadius * fb_uniform(0.62, 0.78));

    // Angle：U(0, π)
    p.angle = (CGFloat)fb_uniform(0.0, M_PI);

    // 坐标偏移：X~N(0, 3.5²)，Y~N(0, 4.0²)
    p.offsetX = (CGFloat)(fb_gaussian() * 3.5);
    p.offsetY = (CGFloat)(fb_gaussian() * 4.0);

    // Tap 持续时长：log-normal(μ=4.5, σ=0.3) ms，截断到 [80, 180] ms
    double tapMs = fb_clamp(fb_lognormal(4.5, 0.3), 80.0, 180.0);
    p.tapDuration = tapMs / 1000.0;

    // Force 斜坡：上升 50-80ms，下降 40-70ms
    p.rampUpDuration   = fb_uniform(0.050, 0.080);
    p.rampDownDuration = fb_uniform(0.040, 0.070);

    // 保证斜坡时长 < tapDuration
    double headroom = p.tapDuration * 0.85;
    if (p.rampUpDuration + p.rampDownDuration > headroom) {
        double scale = headroom / (p.rampUpDuration + p.rampDownDuration);
        p.rampUpDuration   *= scale;
        p.rampDownDuration *= scale;
    }

    return p;
}

+ (CGPoint)applyOffset:(CGPoint)target
               physics:(FBTouchPhysics)physics
            screenSize:(CGSize)screenSize {
    CGFloat x = target.x + physics.offsetX;
    CGFloat y = target.y + physics.offsetY;
    // 保证落点在屏幕内（留 5px 边距避免边缘检测）
    x = (CGFloat)fb_clamp(x, 5.0, screenSize.width  - 5.0);
    y = (CGFloat)fb_clamp(y, 5.0, screenSize.height - 5.0);
    return CGPointMake(x, y);
}

+ (CGFloat)forceAtElapsed:(NSTimeInterval)elapsed
                  physics:(FBTouchPhysics)physics {
    CGFloat peak = physics.force;
    NSTimeInterval total = physics.tapDuration;
    NSTimeInterval up    = physics.rampUpDuration;
    NSTimeInterval down  = physics.rampDownDuration;
    NSTimeInterval downStart = total - down;

    if (elapsed <= 0)              return 0.0f;
    if (elapsed < up)              return peak * (CGFloat)(elapsed / up);
    if (elapsed < downStart)       return peak;
    if (elapsed < total)           return peak * (CGFloat)(1.0 - (elapsed - downStart) / down);
    return 0.0f;
}

@end
