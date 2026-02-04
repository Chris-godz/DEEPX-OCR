# DeepX OCR - 高性能 C++ OCR 推理引擎

<p align="center">
  <a href="README.md">English</a> •
  <img src="https://img.shields.io/badge/C++-17-blue.svg" alt="C++">
  <img src="https://img.shields.io/badge/Platform-Linux-green.svg" alt="Platform">
  <img src="https://img.shields.io/badge/Build-Passing-brightgreen.svg" alt="Build Status">
</p>

**DeepX OCR** 是一个基于 **PP-OCRv5** 的高性能、多线程异步 OCR 推理引擎，专为 **DeepX NPU** 加速优化。

---

## 📖 文档

- **[系统架构文档](docs/architecture.md)** - 详细的架构图、数据流和模型配置。

---

## ✨ 特性

- **🚀 高性能**：针对 DeepX NPU 优化的异步流水线。
- **🔄 多线程**：高效的线程池管理，支持并发处理。
- **🛠️ 模块化设计**：解耦的检测、分类和识别模块。
- **🌍 多语言支持**：内置 `freetype` 支持，完美渲染多语言文本。
- **📊 全面的基准测试**：集成了性能分析工具。

---

## ⚡ 快速开始

### 1. 克隆与初始化
```bash
# 克隆项目并初始化子模块
git clone --recursive git@github.com:Chris-godz/DEEPX-OCR.git
cd DEEPX-OCR
```

### 2. 安装依赖
```bash
# 安装 freetype 依赖（用于多语言文字渲染）
sudo apt-get install libfreetype6-dev libharfbuzz-dev libfmt-dev
```

### 3. 编译与设置
```bash
# 编译项目
./build.sh

# 下载/设置模型
./setup.sh

# 设置 DXRT 环境变量（示例）
source ./set_env.sh 1 2 1 3 2 4
```

### 4. 运行测试
```bash
# 运行交互式测试菜单
./run.sh
```

---

## 🛠️ 构建配置

本项目使用 **Git Submodules** 管理依赖（`nlohmann/json`, `Clipper2`, `spdlog`, `OpenCV`, `opencv_contrib`）。

### 选项 1：从源码编译 OpenCV（推荐）
*包含 `opencv_contrib` 以获得更好的文本渲染支持。*

```bash
# 更新子模块
git submodule update --init 3rd-party/opencv
git submodule update --init 3rd-party/opencv_contrib

# 编译
./build.sh
```

### 选项 2：使用系统 OpenCV
*如果已安装 OpenCV，构建速度更快。*

```bash
# 设置环境变量
export BUILD_OPENCV_FROM_SOURCE=OFF

# 编译
./build.sh
```

---

## 📁 项目结构

```
OCR/
├── 📂 src/                    # 源代码
│   ├── 📂 common/             # 公共工具 (geometry, visualizer, logger)
│   ├── 📂 preprocessing/      # 预处理 (uvdoc, image_ops)
│   ├── 📂 detection/          # 文本检测模块
│   ├── 📂 classification/     # 方向分类模块
│   ├── 📂 recognition/        # 文本识别模块
│   └── 📂 pipeline/           # OCR 主流水线
├── 📂 3rd-party/              # 依赖库 (Git Submodules)
│   ├── 📦 json                # nlohmann/json
│   ├── 📦 clipper2            # 多边形裁剪
│   ├── 📦 spdlog              # 日志库
│   ├── 📦 opencv              # 计算机视觉
│   ├── 📦 opencv_contrib      # 扩展模块 (freetype)
│   ├── 📦 crow                # HTTP 框架
│   ├── 📦 poppler             # PDF 渲染
│   ├── 📦 cpp-base64          # Base64 编码
│   └── 📦 googletest          # 单元测试框架
├── 📂 engine/model_files      # 模型权重
│   ├── 📂 server/         # 高精度模型
│   └── 📂 mobile/         # 轻量级模型
├── 📂 server/                 # HTTP 服务器
│   ├── 📂 benchmark/          # API 基准测试
│   ├── 📂 tests/              # 服务器测试
│   └── 📂 webui/              # Web 界面
├── 📂 benchmark/              # 性能基准测试
├── 📂 test/                   # 单元与集成测试
├── 📂 docs/                   # 文档
├── 📜 build.sh                # 编译脚本
├── 📜 run.sh                  # 交互式运行脚本
├── 📜 setup.sh                # 模型设置脚本
└── 📜 set_env.sh              # 环境设置脚本
```

---

## 🧪 测试与基准测试

### 交互模式
```bash
./run.sh
```

### 手动执行
```bash
# Pipeline 测试
./build_Release/bin/test_pipeline_async

# 模块测试
./build_Release/test_detector                 # 检测
./build_Release/test_recognizer               # 识别 (Server)
./build_Release/test_recognizer_mobile        # 识别 (Mobile)
```

### 基准测试
```bash
# 运行 Python 基准测试包装器
python3 benchmark/run_benchmark.py --model server
python3 benchmark/run_benchmark.py --model mobile
```

### 📊 基准测试报告（汇总）

#### x86 平台

**测试配置**（来源：`docs/results/local/x86/` 报告）：
- 模型：PP-OCR v5（DEEPX NPU 加速）
- 数据集规模：20 张图片
- 成功率：100%（20/20）

**性能汇总（Server）**：
| 配置 | 平均推理耗时 (ms) | 平均 FPS | 平均 CPS（字符/秒） | 平均字符准确率 |
|---|---:|---:|---:|---:|
| 单卡 | 135.06 | 7.40 | 243.22 | 96.93% |
| 双卡 | 67.89 | 14.73 | 483.88 | 96.93% |
| 三卡 | 45.55 | 21.96 | 721.23 | 96.93% |

**性能汇总（Mobile）**：
| 配置 | 平均推理耗时 (ms) | 平均 FPS | 平均 CPS（字符/秒） | 平均字符准确率 |
|---|---:|---:|---:|---:|
| 单卡 | 82.93 | 12.06 | 378.63 | 89.60% |
| 双卡 | 44.24 | 22.61 | 709.83 | 89.60% |
| 三卡 | 33.00 | 30.30 | 951.57 | 89.60% |

**详细报告**：
| 配置 | Server | Mobile |
|---|---|---|
| 单卡 | [报告](docs/results/local/x86/server/DXNN-OCR_benchmark_report_singlecard.md) | [报告](docs/results/local/x86/mobile/DXNN-OCR_benchmark_report_singlecard.md) |
| 双卡 | [报告](docs/results/local/x86/server/DXNN-OCR_benchmark_report_dualcards.md) | [报告](docs/results/local/x86/mobile/DXNN-OCR_benchmark_report_dualcards.md) |
| 三卡 | [报告](docs/results/local/x86/server/DXNN-OCR_benchmark_report_threecards.md) | [报告](docs/results/local/x86/mobile/DXNN-OCR_benchmark_report_threecards.md) |

---

#### ARM 平台（Rockchip aarch64）

**测试配置**（来源：`docs/results/local/arm/` 报告）：
- 模型：PP-OCR v5（DEEPX NPU 加速）
- 数据集规模：20 张图片
- 成功率：100%（20/20）

**性能汇总**：
| 模型 | 平均推理耗时 (ms) | 平均 FPS | 平均 CPS（字符/秒） | 平均字符准确率 |
|---|---:|---:|---:|---:|
| Server | 133.88 | 7.47 | 245.74 | 96.82% |
| Mobile | 60.00 | 16.67 | 524.96 | 89.37% |

**详细报告**：
| 模型 | 报告 |
|---|---|
| Server | [报告](docs/results/local/arm/server/DXNN-OCR_benchmark_report.md) |
| Mobile | [报告](docs/results/local/arm/mobile/DXNN-OCR_benchmark_report.md) |

<details>
<summary><b>🔄 复现基准测试结果</b></summary>

运行以下命令复现上述基准测试结果：

```bash
# 1. 编译项目
./build.sh

# 2. 下载/设置模型
./setup.sh

# 3. 设置 DeepX NPU 环境变量
source ./set_env.sh 3 2 1 3 2 4

# 4. 运行基准测试（Server 模型，每张图片运行 60 次）
python3 benchmark/run_benchmark.py --model server --runs 60 \
    --images_dir test/twocode_images

# 5. 运行基准测试（Mobile 模型，每张图片运行 60 次）
python3 benchmark/run_benchmark.py --model mobile --runs 60 \
    --images_dir test/twocode_images
```

**参数说明**：
| 参数 | 说明 | 默认值 |
|---|---|---|
| `--model` | 模型类型（`server` / `mobile`） | `server` |
| `--runs` | 每张图片运行次数 | `3` |
| `--images_dir` | 测试图片目录 | `images` |
| `--no-acc` | 跳过准确率计算 | - |
| `--no-cpp` | 跳过 C++ 基准测试（使用已有结果） | - |

</details>

---

### 📡 API 服务器基准测试

**测试配置**（所有报告一致）：
- 模式：吞吐量（throughput）
- 并发数：20
- 每样本运行次数：20

#### x86 平台

**Server 模型：**
| 配置 | QPS | 成功率 | CPS（字符/秒） | 准确率 | 平均延迟 (ms) | P50 (ms) | P99 (ms) |
|---|--:|---:|---:|---:|---:|---:|---:|
| 单卡 | 7.64 | 100% | 236.88 | 96.93% | 2594.17 | 2618.61 | 3498.46 |
| 双卡 | 13.62 | 100% | 401.24 | 89.60% | 1423.65 | 1438.99 | 1786.95 |
| 三卡 | 21.50 | 100% | 605.96 | 96.93% | 900.14 | 907.47 | 1517.51 |

**Mobile 模型：**
| 配置 | QPS | 成功率 | CPS（字符/秒） | 准确率 | 平均延迟 (ms) | P50 (ms) | P99 (ms) |
|---|--:|---:|---:|---:|---:|---:|---:|
| 单卡 | 13.62 | 100% | 401.24 | 89.60% | 1423.65 | 1438.99 | 1786.95 |
| 双卡 | 23.97 | 100% | 692.24 | 89.60% | 788.05 | 763.87 | 1586.34 |
| 三卡 | 28.00 | 100% | 801.66 | 89.60% | 635.59 | 564.74 | 1299.82 |

**详细报告**：
| 配置 | Server | Mobile |
|---|---|---|
| 单卡 | [报告](docs/results/server/x86/server/DXNN-OCR_Server_benchmark_report_singlecard.md) | [报告](docs/results/server/x86/mobile/DXNN-OCR_Server_benchmark_report_singlecard.md) |
| 双卡 | [报告](docs/results/server/x86/server/DXNN-OCR_Server_benchmark_report_dualcards.md) | [报告](docs/results/server/x86/mobile/DXNN-OCR_Server_benchmark_report_dualcards.md) |
| 三卡 | [报告](docs/results/server/x86/server/DXNN-OCR_Server_benchmark_report_threecards.md) | [报告](docs/results/server/x86/mobile/DXNN-OCR_Server_benchmark_report_threecards.md) |

#### ARM 平台（Rockchip aarch64）

| 模型 | QPS | 成功率 | CPS（字符/秒） | 准确率 | 平均延迟 (ms) | P50 (ms) | P99 (ms) |
|---|--:|---:|---:|---:|---:|---:|---:|
| Server | 7.45 | 100% | 225.62 | 96.82% | 2635.66 | 2646.28 | 4270.81 |
| Mobile | 16.11 | 100% | 469.57 | 89.37% | 1192.55 | 1200.13 | 1673.76 |

**详细报告**：
| 模型 | 报告 |
|---|---|
| Server | [报告](docs/results/server/arm/server/DXNN-OCR_Server_benchmark_report.md) |
| Mobile | [报告](docs/results/server/arm/mobile/DXNN-OCR_Server_benchmark_report.md) |

<details>
<summary><b>🔄 复现 API 服务器基准测试结果</b></summary>

1. **启动 OCR 服务**：

```bash
cd server
./run_server.sh
```

2. **安装基准测试依赖**：

```bash
cd server/benchmark
pip install -r requirements.txt
```

3. **运行吞吐量测试**：

```bash
./quick_start.sh

# 选择选项 2 运行吞吐量测试
```

</details>

---

## 🖥️ WebUI 演示

1. **启动 OCR 服务**（WebUI 后端依赖）：

```bash
cd server
./run_server.sh
```

2. **启动 WebUI**：

```bash
cd server/webui
python3 -m venv venv && source venv/bin/activate
pip install -r requirements.txt
python app.py
```

![WebUI 主界面](docs/images/image_web.png)

**访问地址**：http://localhost:7860