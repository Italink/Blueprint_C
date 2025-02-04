# Blueprint_C
**Blueprint_C** 是一个虚幻引擎插件，使用它可以生成蓝图结构的C++代码代理，以便C++能轻易地读写蓝图属性和调用蓝图方法：
![image-20250204112614686](Resources/image-20250204112614686.png)

## 使用说明

在蓝图编辑器标题栏的 `Window` 菜单下可以打开 `Blueprint_C` 的面板：

![image-20250204112923728](Resources/image-20250204112923728.png)

面板中可调参数的作用如下：

- **Super Class**：导出代码所使用的父类
- **Property**：是否导出属性
- **Function**：是否导出函数
- **Macro**：是否生成UHT宏
- **Meta Data**：是否在UHT宏中显示元数据
- **Split Header**：是否将代码分割为头文件和源文件

在蓝图编辑器中执行编译会自动刷新生成代码。

生成的类定义在内存结构上与蓝图类保持一致，将生成的代码拷贝到工程代码中，就可以在C++中这样去操作蓝图对象：

``` c++
// Cast
//UClass* Class = LoadObject<UClass>(nullptr, TEXT("/Game/NewBlueprint.NewBlueprint_C"));
//UObject* Object = NewObject<UObject>(GetTransientPackage(), Class);
//UNewBlueprint_C* NewBlueprintObj = Cast<UNewBlueprint_C>(Object);

// New Object
UNewBlueprint_C* NewBlueprintObj = NewObject<UNewBlueprint_C>();

// Read Write Property
NewBlueprintObj->NewVar_0;
NewBlueprintObj->NewVar_1 = 2.0f;

// Call Function
FVector OutParam1;
FString OutParam2;
NewBlueprintObj->NewFunction(true, 0, OutParam1, OutParam2);
```

## Todo

- 模块代码自动生成（自动响应蓝图变更，生成头文件依赖，刷新工程文件）
- UMG MVC框架
