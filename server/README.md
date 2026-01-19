# DeepX OCR Server

基于 Crow 框架的高性能 OCR HTTP 服务，支持并发请求处理，支持图像和 PDF 文件输入。

## 编译

```bash
cd /home/deepx/Desktop/ocr_demo
bash build.sh
```

## 启动

```bash
# 设置环境变量
source ./set_env.sh 1 2 1 3 2 4

# 启动服务
cd build_Release
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(pwd)/../3rd-party/pdfium/lib ./bin/ocr_server [选项]
```

### 命令行参数

| 参数 | 说明 | 默认值 |
|------|------|--------|
| `-p, --port` | 服务端口 | 8080 |
| `-t, --threads` | HTTP 线程数 | 4 |
| `-v, --vis-dir` | 可视化输出目录 | output/vis |
| `-h, --help` | 帮助 | - |

## API

### POST /ocr

OCR 识别接口，支持 Base64 编码图像/PDF 和 URL 两种输入方式。

**请求头**

```
Content-Type: application/json
Authorization: token <任意字符串>
```

**请求体**

| 参数 | 类型 | 必填 | 默认值 | 说明 |
|------|------|------|--------|------|
| file | string | 是 | - | Base64 编码的图像/PDF 或文件 URL |
| fileType | int | 否 | 1 | 文件类型：1=图像，0=PDF |
| useDocOrientationClassify | bool | 否 | false | 启用文档方向分类 |
| useDocUnwarping | bool | 否 | false | 启用文档扭曲矫正 |
| useTextlineOrientation | bool | 否 | false | 启用文本行方向矫正 |
| textDetThresh | float | 否 | 0.3 | 检测像素阈值 [0.0-1.0] |
| textDetBoxThresh | float | 否 | 0.6 | 检测框阈值 [0.0-1.0] |
| textDetUnclipRatio | float | 否 | 1.5 | 检测框扩张系数 [1.0-3.0] |
| textRecScoreThresh | float | 否 | 0.0 | 识别置信度阈值 [0.0-1.0] |
| visualize | bool | 否 | false | 生成可视化结果图像 |
| pdfDpi | int | 否 | 150 | PDF 渲染 DPI（仅 fileType=0 时有效，范围 72-300） |
| pdfMaxPages | int | 否 | 10 | PDF 最大处理页数（仅 fileType=0 时有效，范围 1-100） |

**图像 OCR 响应示例 (fileType=1)**

```json
{
    "logId": "uuid-string",
    "errorCode": 0,
    "errorMsg": "Success",
    "result": {
        "ocrResults": [
            {
                "prunedResult": "识别的文字",
                "score": 0.98,
                "points": [
                    {"x": 100, "y": 50},
                    {"x": 300, "y": 50},
                    {"x": 300, "y": 80},
                    {"x": 100, "y": 80}
                ]
            }
        ],
        "ocrImage": "/static/vis/ocr_vis_xxx.jpg"
    }
}
```

**PDF OCR 响应示例 (fileType=0)**

```json
{
    "logId": "uuid-string",
    "errorCode": 0,
    "errorMsg": "Success",
    "result": {
        "totalPages": 4,
        "renderedPages": 2,
        "warning": "Only first 2 of 4 pages were processed due to page limit",
        "pages": [
            {
                "pageIndex": 0,
                "ocrResults": [
                    {
                        "prunedResult": "第一页的文字",
                        "score": 0.95,
                        "points": [...]
                    }
                ]
            },
            {
                "pageIndex": 1,
                "ocrResults": [
                    {
                        "prunedResult": "第二页的文字",
                        "score": 0.92,
                        "points": [...]
                    }
                ]
            }
        ]
    }
}
```

**错误码**

| errorCode | HTTP 状态码 | 说明 |
|-----------|-------------|------|
| 0 | 200 | 成功 |
| 1001 | 400 | 参数错误 |
| 1002 | 400 | PDF 文件无法打开 |
| 1003 | 400 | PDF 格式无效或文件损坏 |
| 1004 | 401 | PDF 需要密码 |
| 1005 | 403 | PDF 安全策略不支持 |
| 1006 | 400 | PDF 页面不存在 |
| 1007 | 400 | PDF 页面尺寸异常 |
| 1008 | 400 | PDF 页数超出限制 |
| 1009 | 400 | PDF DPI 超出限制 |
| 2001 | 500 | 服务内部错误 |
| 2002 | 503 | 内存分配失败 |
| 2003 | 504 | PDF 渲染超时 |
| 3001 | 401 | 认证失败 |

### PDF 处理说明

- **内存控制**：PDF 渲染会消耗较多内存，建议使用默认参数（DPI=150，最大 10 页）
- **内存估算**：A4 页面 @ 150 DPI 约 8.7MB/页，10 页约 87MB
- **并行处理**：多页 PDF 采用并行渲染和并行 OCR 处理
- **页数限制**：超出 `pdfMaxPages` 的页面不会被处理，响应中会包含 `warning` 字段

### GET /health

健康检查接口。

```json
{
    "status": "healthy",
    "service": "DeepX OCR Server",
    "version": "1.0.0"
}
```

### GET /static/vis/\<filename\>

访问可视化结果图像。

## 基准测试

### 图像 API 基准测试

使用 `benchmark/run.sh` 进行性能测试：

```bash
cd server/benchmark

# 串行模式测试（默认）
./run.sh

# 并发模式测试（4 并发）
./run.sh -c 4

# 指定运行次数
./run.sh -r 3

# 查看帮助
./run.sh -h
```

### PDF OCR 自动化测试

使用 `benchmark/pdf_ocr_test.py` 进行 PDF OCR 功能测试：

```bash
cd server/benchmark

# 测试 pdf_file/ 目录中的所有 PDF
python3 pdf_ocr_test.py

# 指定 DPI 和最大页数
python3 pdf_ocr_test.py --dpi 150 --max-pages 100

# 只测试指定 PDF 文件
python3 pdf_ocr_test.py --pdf book-rev7.pdf

# 详细输出模式
python3 pdf_ocr_test.py -v

# 查看帮助
python3 pdf_ocr_test.py --help
```

**PDF 测试目录结构**

```
benchmark/
├── pdf_file/           # 放置待测试的 PDF 文件
└── result/             # OCR 结果输出目录
    └── xxx_OCR_result.json
```

**测试结果**

- `result/<pdf名>_OCR_result.json` - 每个 PDF 的 OCR 结果
- `result/test_report_<时间戳>.json` - 测试汇总报告

## 单元测试

```bash
cd build_Release
ctest --output-on-failure
```

## 目录结构

```
server/
├── server_main.cpp      # 服务入口
├── ocr_handler.cpp/h    # OCR 请求处理器
├── pdf_handler.cpp/h    # PDF 渲染处理器
├── file_handler.cpp/h   # 文件处理（Base64/URL）
├── json_response.cpp/h  # JSON 响应构建器
├── benchmark/           # 基准测试工具
│   ├── run.sh           # 图像 API 测试脚本
│   ├── pdf_ocr_test.py  # PDF OCR 测试脚本
│   ├── pdf_file/        # 测试 PDF 文件
│   ├── result/          # PDF 测试结果
│   └── images/          # 测试图片
└── tests/               # 单元测试
```

## 输出

- 可视化图像: `output/vis/`
- 日志: `logs/deepx_ocr.log`
