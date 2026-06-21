// FBPhysicalTouch.h
// 物理触控参数生成器 — 模拟真实人手触控特征
// 集成到 WebDriverAgentLib/Utilities/

#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>

NS_ASSUME_NONNULL_BEGIN

/**
 * 一次触控的完整物理参数。
 * 对应 XCPointerEvent 可设置的所有生物特征字段。
 */
typedef struct {
    CGFloat force;          // 压力：0.15-0.70，log-normal
    CGFloat majorRadius;    // 触点长轴半径(px)：8-18，正态
    CGFloat minorRadius;    // 触点短轴半径(px)：majorRadius × 0.62-0.78
    CGFloat angle;          // 椭圆旋转角(rad)：0-π，均匀
    CGFloat offsetX;        // 坐标X偏移(px)：高斯 σ=3.5
    CGFloat offsetY;        // 坐标Y偏移(px)：高斯 σ=4.0
    NSTimeInterval tapDuration;  // tap持续时长(s)：0.08-0.18，log-normal
    NSTimeInterval rampUpDuration;   // force斜坡上升时长(s)：0.05-0.08
    NSTimeInterval rampDownDuration; // force斜坡下降时长(s)：0.04-0.07
} FBTouchPhysics;

@interface FBPhysicalTouch : NSObject

/**
 * 生成一组随机物理触控参数。
 * 每次 tap/touchDown 前调用，保证每次触控特征唯一。
 */
+ (FBTouchPhysics)randomParams;

/**
 * 将偏移叠加到目标坐标上，返回实际落点。
 * 保证落点不超出屏幕边界（传入屏幕尺寸）。
 */
+ (CGPoint)applyOffset:(CGPoint)target
               physics:(FBTouchPhysics)physics
            screenSize:(CGSize)screenSize;

/**
 * force 斜坡值：touchDown 后经过 elapsed 秒时的当前 force。
 * 上升段：0 → peakForce，线性，持续 rampUp 秒。
 * 平台段：peakForce。
 * 下降段（接近 touchUp 前 rampDown 秒）：peakForce → 0，线性。
 * totalDuration = tapDuration。
 */
+ (CGFloat)forceAtElapsed:(NSTimeInterval)elapsed
                  physics:(FBTouchPhysics)physics;

@end

NS_ASSUME_NONNULL_END
