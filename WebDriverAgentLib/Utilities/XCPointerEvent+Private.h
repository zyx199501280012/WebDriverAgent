// XCPointerEvent+Private.h
// XCTest 私有属性声明，允许设置触控物理参数
// 在 FBPhysicalTouch.m 和 patch 集成点 #import 此文件

// XCPointerEvent / XCSynthesizedEventRecord 是 XCTest 私有类，
// 不在公开头文件中，使用前向声明即可（运行时通过 KVC 访问）
@class XCPointerEvent, XCSynthesizedEventRecord;

// XCPointerEvent 私有扩展（iOS 15 / Xcode 13.x 验证可用）
@interface XCPointerEvent (Private)
@property (nonatomic, readwrite) CGFloat pressure;       // 压力 0.0-1.0
@property (nonatomic, readwrite) CGFloat majorRadius;    // 触点长轴半径 px
@property (nonatomic, readwrite) CGFloat minorRadius;    // 触点短轴半径 px
// azimuth 在部分版本存在，编译报错时注释掉
// @property (nonatomic) CGFloat azimuth;
@end

// XCSynthesizedEventRecord 私有构造（WDA 已在用，此处仅供参考）
@interface XCSynthesizedEventRecord (Private)
- (instancetype)initWithName:(NSString *)name
        interfaceOrientation:(NSInteger)orientation;
- (void)addPointerEvent:(XCPointerEvent *)event;
@end
