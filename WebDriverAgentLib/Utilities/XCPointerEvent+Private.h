// XCPointerEvent+Private.h
// XCTest 私有属性声明，允许设置触控物理参数
// 在 FBPhysicalTouch.m 和 patch 集成点 #import 此文件

#import <XCTest/XCTest.h>

// XCPointerEvent 私有扩展（iOS 15 / Xcode 13.x 验证可用）
@interface XCPointerEvent (Private)
@property (nonatomic) CGFloat pressure;       // 压力 0.0-1.0
@property (nonatomic) CGFloat majorRadius;    // 触点长轴半径 px
@property (nonatomic) CGFloat minorRadius;    // 触点短轴半径 px
// azimuth 在部分版本存在，编译报错时注释掉
// @property (nonatomic) CGFloat azimuth;
@end

// XCSynthesizedEventRecord 私有构造（WDA 已在用，此处仅供参考）
@interface XCSynthesizedEventRecord (Private)
- (instancetype)initWithName:(NSString *)name
        interfaceOrientation:(NSInteger)orientation;
- (void)addPointerEvent:(XCPointerEvent *)event;
@end
