# DeepX OCR - High-Performance C++ OCR Inference Engine

<p align="center">
  <a href="README_CN.md">中文</a> •
  <img src="https://img.shields.io/badge/C++-17-blue.svg" alt="C++">
  <img src="https://img.shields.io/badge/Platform-Linux-green.svg" alt="Platform">
  <img src="https://img.shields.io/badge/Build-Passing-brightgreen.svg" alt="Build Status">
</p>

**DeepX OCR** is a high-performance, multi-threaded asynchronous OCR inference engine based on **PP-OCRv5**, optimized for **DeepX NPU** acceleration.

---

## 📖 Documentation

- **[System Architecture](docs/architecture.md)** - Detailed architecture diagrams, data flow, and model configuration.

---

## ✨ Features

- **🚀 High Performance**: Asynchronous pipeline optimized for DeepX NPU.
- **🔄 Multi-threading**: Efficient thread pool management for concurrent processing.
- **🛠️ Modular Design**: Decoupled Detection, Classification, and Recognition modules.
- **🌍 Multi-language Support**: Built-in `freetype` support for rendering multi-language text.
- **📊 Comprehensive Benchmarking**: Integrated tools for performance analysis.

---

## ⚡ Quick Start

### 1. Clone & Initialize
```bash
# Clone the repository and initialize submodules
git clone --recursive git@github.com:Chris-godz/DEEPX-OCR.git
cd DEEPX-OCR
```

### 2. Install Dependencies
```bash
# Install freetype dependencies (for multi-language text rendering)
sudo apt-get install libfreetype6-dev libharfbuzz-dev libfmt-dev
```

### 3. Build & Setup
```bash
# Build the project
./build.sh

# Download/Setup models
./setup.sh

# Set DXRT environment variables (Example)
source ./set_env.sh 1 2 1 3 2 4
```

### 4. Run Tests
```bash
# Run the interactive test menu
./run.sh
```

---

## 🛠️ Build Configuration

This project uses **Git Submodules** to manage dependencies (`nlohmann/json`, `Clipper2`, `spdlog`, `OpenCV`, `opencv_contrib`).

### Option 1: Build OpenCV from Source (Recommended)
*Includes `opencv_contrib` for better text rendering support.*

```bash
# Update submodules
git submodule update --init 3rd-party/opencv
git submodule update --init 3rd-party/opencv_contrib

# Build
./build.sh
```

### Option 2: Use System OpenCV
*Faster build if you already have OpenCV installed.*

```bash
# Set environment variable
export BUILD_OPENCV_FROM_SOURCE=OFF

# Build
./build.sh
```

---

## 📁 Project Structure

```
OCR/
├── 📂 src/                    # Source Code
│   ├── 📂 common/             # Common Utilities (geometry, visualizer, logger)
│   ├── 📂 preprocessing/      # Preprocessing (uvdoc, image_ops)
│   ├── 📂 detection/          # Text Detection Module
│   ├── 📂 classification/     # Orientation Classification
│   ├── 📂 recognition/        # Text Recognition Module
│   └── 📂 pipeline/           # Main OCR Pipeline
├── 📂 3rd-party/              # Dependencies (Git Submodules)
│   ├── 📦 json                # nlohmann/json
│   ├── 📦 clipper2            # Polygon Clipping
│   ├── 📦 spdlog              # Logging
│   ├── 📦 opencv              # Computer Vision
│   ├── 📦 opencv_contrib      # Extra Modules (freetype)
│   ├── 📦 crow                # HTTP Framework
│   ├── 📦 poppler             # PDF Rendering
│   ├── 📦 cpp-base64          # Base64 Encoding
│   └── 📦 googletest          # Unit Testing Framework
├── 📂 engine/model_files      # Model Weights
│   ├── 📂 server/             # High-Accuracy Models
│   └── 📂 mobile/             # Lightweight Models
├── 📂 server/                 # HTTP Server
│   ├── 📂 benchmark/          # API Benchmark
│   ├── 📂 tests/              # Server Tests
│   └── 📂 webui/              # Web Interface
├── 📂 benchmark/              # Performance Benchmarking
├── 📂 test/                   # Unit & Integration Tests
├── 📂 docs/                   # Documentation
├── 📜 build.sh                # Build Script
├── 📜 run.sh                  # Interactive Runner
├── 📜 setup.sh                # Model Setup Script
└── 📜 set_env.sh              # Environment Setup
```

---

## 🧪 Testing & Benchmarking

### Interactive Mode
```bash
./run.sh
```

### Manual Execution
```bash
# Pipeline Test
./build_Release/bin/test_pipeline_async

# Module Tests
./build_Release/test_detector                 # Detection
./build_Release/test_recognizer               # Recognition (Server)
./build_Release/test_recognizer_mobile        # Recognition (Mobile)
```

### Benchmarking
```bash
# Run Python benchmark wrapper
python3 benchmark/run_benchmark.py --model server
python3 benchmark/run_benchmark.py --model mobile
```

### 📊 Benchmark Reports (Summary)

#### x86 Platform

**Test Configuration** (from `docs/results/local/x86/` reports):
- Model: PP-OCR v5 (DEEPX NPU acceleration)
- Dataset Size: 20 images
- Success Rate: 100% (20/20)

**Performance Summary (Server)**:
| Setup | Avg Inference Time (ms) | Avg FPS | Avg CPS (chars/s) | Avg Character Accuracy |
|---|---:|---:|---:|---:|
| Single Card | 135.06 | 7.40 | 243.22 | 96.93% |
| Dual Cards | 67.89 | 14.73 | 483.88 | 96.93% |
| Three Cards | 45.55 | 21.96 | 721.23 | 96.93% |

**Performance Summary (Mobile)**:
| Setup | Avg Inference Time (ms) | Avg FPS | Avg CPS (chars/s) | Avg Character Accuracy |
|---|---:|---:|---:|---:|
| Single Card | 82.93 | 12.06 | 378.63 | 89.60% |
| Dual Cards | 44.24 | 22.61 | 709.83 | 89.60% |
| Three Cards | 33.00 | 30.30 | 951.57 | 89.60% |

**Detailed Reports**:
| Setup | Server | Mobile |
|---|---|---|
| Single Card | [Report](docs/results/local/x86/server/DXNN-OCR_benchmark_report_singlecard.md) | [Report](docs/results/local/x86/mobile/DXNN-OCR_benchmark_report_singlecard.md) |
| Dual Cards | [Report](docs/results/local/x86/server/DXNN-OCR_benchmark_report_dualcards.md) | [Report](docs/results/local/x86/mobile/DXNN-OCR_benchmark_report_dualcards.md) |
| Three Cards | [Report](docs/results/local/x86/server/DXNN-OCR_benchmark_report_threecards.md) | [Report](docs/results/local/x86/mobile/DXNN-OCR_benchmark_report_threecards.md) |

---

#### ARM Platform (Rockchip aarch64)

**Test Configuration** (from `docs/results/local/arm/` reports):
- Model: PP-OCR v5 (DEEPX NPU acceleration)
- Dataset Size: 20 images
- Success Rate: 100% (20/20)

**Performance Summary**:
| Model | Avg Inference Time (ms) | Avg FPS | Avg CPS (chars/s) | Avg Character Accuracy |
|---|---:|---:|---:|---:|
| Server | 133.88 | 7.47 | 245.74 | 96.82% |
| Mobile | 60.00 | 16.67 | 524.96 | 89.37% |

**Detailed Reports**:
| Model | Report |
|---|---|
| Server | [Report](docs/results/local/arm/DXNN-OCR_benchmark_report_server.md) |
| Mobile | [Report](docs/results/local/arm/DXNN-OCR_benchmark_report_mobile.md) |

<details>
<summary><b>🔄 Reproduce Benchmark Results</b></summary>

To reproduce the benchmark results above, run the following commands:

```bash
# 1. Build the project
./build.sh

# 2. Download/setup models
./setup.sh

# 3. Set DXRT environment variables (example)
source ./set_env.sh 1 2 1 3 2 4

# 4. Run benchmark (server model, 60 runs per image)
python3 benchmark/run_benchmark.py --model server --runs 60 \
    --images_dir test/twocode_images

# 5. Run benchmark (mobile model, 60 runs per image)
python3 benchmark/run_benchmark.py --model mobile --runs 60 \
    --images_dir test/twocode_images
```

**Parameters**:
| Parameter | Description | Default |
|---|---|---|
| `--model` | Model type (`server` / `mobile`) | `server` |
| `--runs` | Number of runs per image | `3` |
| `--images_dir` | Test images directory | `images` |
| `--no-acc` | Skip accuracy calculation | - |
| `--no-cpp` | Skip C++ benchmark (use existing results) | - |

</details>

---

### 📡 API Server Benchmark

**Test configuration** (same across all reports):
- Mode: throughput
- Concurrency: 20
- Runs per sample: 20

#### x86 Platform

**Server Model:**
| Setup | QPS | Success Rate | CPS (chars/s) | Accuracy | Avg Latency (ms) | P50 (ms) | P99 (ms) |
|---|--:|---:|---:|---:|---:|---:|---:|
| Single Card | 7.64 | 100% | 236.88 | 96.93% | 2594.17 | 2618.61 | 3498.46 |
| Dual Cards | 13.62 | 100% | 401.24 | 89.60% | 1423.65 | 1438.99 | 1786.95 |
| Three Cards | 21.50 | 100% | 605.96 | 96.93% | 900.14 | 907.47 | 1517.51 |

**Mobile Model:**
| Setup | QPS | Success Rate | CPS (chars/s) | Accuracy | Avg Latency (ms) | P50 (ms) | P99 (ms) |
|---|--:|---:|---:|---:|---:|---:|---:|
| Single Card | 13.62 | 100% | 401.24 | 89.60% | 1423.65 | 1438.99 | 1786.95 |
| Dual Cards | 23.97 | 100% | 692.24 | 89.60% | 788.05 | 763.87 | 1586.34 |
| Three Cards | 28.00 | 100% | 801.66 | 89.60% | 635.59 | 564.74 | 1299.82 |

**Detailed reports**:
| Setup | Server | Mobile |
|---|---|---|
| Single Card | [Report](docs/results/server/x86/server/DXNN-OCR_Server_benchmark_report_singlecard.md) | [Report](docs/results/server/x86/mobile/DXNN-OCR_Server_benchmark_report_singlecard.md) |
| Dual Cards | [Report](docs/results/server/x86/server/DXNN-OCR_Server_benchmark_report_dualcards.md) | [Report](docs/results/server/x86/mobile/DXNN-OCR_Server_benchmark_report_dualcards.md) |
| Three Cards | [Report](docs/results/server/x86/server/DXNN-OCR_Server_benchmark_report_threecards.md) | [Report](docs/results/server/x86/mobile/DXNN-OCR_Server_benchmark_report_threecards.md) |

#### ARM Platform (Rockchip aarch64)

| Model | QPS | Success Rate | CPS (chars/s) | Accuracy | Avg Latency (ms) | P50 (ms) | P99 (ms) |
|---|--:|---:|---:|---:|---:|---:|---:|
| Server | 7.45 | 100% | 225.62 | 96.82% | 2635.66 | 2646.28 | 4270.81 |
| Mobile | 16.11 | 100% | 469.57 | 89.37% | 1192.55 | 1200.13 | 1673.76 |

**Detailed reports**:
| Model | Report |
|---|---|
| Server | [Report](docs/results/server/arm/server/DXNN-OCR_Server_benchmark_report.md) |
| Mobile | [Report](docs/results/server/arm/mobile/DXNN-OCR_Server_benchmark_report.md) |

<details>
<summary><b>🔄 Reproduce API Server Benchmark Results</b></summary>

1. **Start the OCR server**:

```bash
cd server
./run_server.sh
```

2. **Install benchmark dependencies**:

```bash
cd server/benchmark
pip install -r requirements.txt
```

3. **Run throughput test**:

```bash
./quick_start.sh

# Select option 2 to run the throughput test
```

</details>

---

## 🖥️ WebUI Demo

1. **Start the OCR server** (required for the WebUI backend):

```bash
cd server
./run_server.sh
```

2. **Start the WebUI**:

```bash
cd server/webui
python3 -m venv venv && source venv/bin/activate
pip install -r requirements.txt
python app.py
```

![WebUI main interface](docs/images/image_web.png)

**Access**: http://localhost:7860