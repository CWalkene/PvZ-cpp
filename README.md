# 植物大战僵尸 C++ 实现

使用 **C++20 + EasyX** 复刻《植物大战僵尸》，实现一个可游玩的白天草坪关卡。

## 已实现功能

- **草坪与种植**：5 行 9 列网格，支持选卡、鼠标跟随和半透明放置预览。
- **三种植物**：豌豆射手攻击同一行前方的僵尸，向日葵生产阳光，坚果阻挡僵尸。
- **阳光系统**：天空掉落与向日葵生产，点击收集后播放回收动画并增加阳光。
- **战斗系统**：僵尸随机出现在不同的行，支持行走、啃食、受击闪白和死亡动画。
- **铲子工具**：移除已种植的植物。
- **音频与结算**：循环背景音乐、交互和战斗音效，以及胜利、失败提示。

## 构建与运行

### 环境准备

- Windows
- Visual Studio，安装“使用 C++ 的桌面开发”工作负载及 Windows SDK
- 支持 C++20 的 MSVC 编译器；当前工程配置使用 **`v145` 平台工具集**
- 为所用 Visual Studio 环境安装并配置 **EasyX 图形库**，确保可以找到 `easyx.h` 及对应架构的库

EasyX 不包含在本仓库中，需要单独安装。

### 使用命令行

在 Visual Studio 的开发者 PowerShell 中，进入项目根目录执行：

```powershell
msbuild .\PvZ-cpp.vcxproj /p:Configuration=Release /p:Platform=x64
```

构建完成后，从输出目录启动：

```powershell
Set-Location .\build\bin\x64\Release
.\PvZ-cpp.exe
```

### 资源路径

程序通过相对路径 `assets/...` 加载图片和音频，**运行时的工作目录必须包含 `assets` 文件夹**。

工程会在构建后自动将根目录的 `assets/` 复制到程序输出目录。默认输出位置为：

```text
build/bin/<平台>/<配置>/PvZ-cpp.exe
build/bin/<平台>/<配置>/assets/
```

## 项目结构

```text
PvZ-cpp/
├── src/
│   ├── main.cpp              # 程序入口
│   ├── Game.h / Game.cpp     # 游戏循环、输入、场景管理和胜负判断
│   ├── Plant.h / Plant.cpp   # 植物基类与公共动画逻辑
│   ├── Peashooter.*          # 豌豆射手
│   ├── Sunflower.*           # 向日葵
│   ├── Wallnut.h             # 坚果
│   ├── Zombie.*              # 僵尸状态、移动与攻击
│   ├── Bullet.*              # 豌豆子弹
│   ├── Sun.*                 # 阳光生成、动画与收集
│   ├── Audio.*               # 背景音乐与音效
│   └── GraphicsUtils.*       # 图片透明混合与受击绘制
├── assets/                   # 图片、动画帧、音乐与音效
├── PvZ-cpp.slnx              # Visual Studio 解决方案
├── PvZ-cpp.vcxproj           # C++ 项目与构建配置
├── PvZ-cpp.vcxproj.filters   # Visual Studio 文件分组
├── .gitignore               # 本机缓存与构建产物忽略规则
└── LICENSE
```

### 许可证与素材

项目许可证见 [LICENSE](LICENSE)，内容为 GNU GPL v3。

本项目为《植物大战僵尸》的非官方学习实现。游戏名称、图像、音乐等第三方素材的权利归各自权利人所有，不因本仓库中的代码许可证而改变。
