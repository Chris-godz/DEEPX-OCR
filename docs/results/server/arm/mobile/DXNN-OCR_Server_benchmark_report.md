# OCR API Benchmark Report: default

**Generated at:** 2026-02-04 18:08:13

## 📋 Test Configuration

| Parameter | Value |
|-----------|-------|
| Test Name | default |
| Test Mode | throughput |
| Concurrency | 20 |
| Runs per Sample | 20 |
| Server URL | http://localhost:8080/ocr |
| Timeout | 60s |

## 🚀 Performance Summary

| Metric | Value |
|--------|-------|
| **Total Requests** | 400 |
| **Success Rate** | 100.00% |
| **QPS** | **16.11** |
| **Success QPS** | 16.11 |
| **Total Duration (ms)** | 24830.82 |

## ⏱️ Latency Statistics

| Percentile | Latency (ms) |
|------------|--------------|
| Min | 295.14 |
| P50 (Median) | 1200.13 |
| P90 | 1366.68 |
| P95 | 1458.05 |
| P99 | 1673.76 |
| P99.9 | 2007.77 |
| Max | 2022.30 |
| Mean | 1192.55 |
| Std Dev | 180.39 |

## 📝 OCR Statistics

| Metric | Value |
|--------|-------|
| Total Characters | 12480 |
| Total Pages (pdf)| 0 |
| CPS (Chars/sec) | 469.57 |
| Accuracy | 89.37% |

## ⚠️ Error Statistics

| Error Type | Count |
|------------|-------|

**Error Rate:** 0.00%
**Timeout Rate:** 0.00%

## 💻 Resource Monitoring

### CPU
| Metric | Value |
|--------|-------|
| Average | 40.02% |
| Max | 74.70% |
| Min | 10.10% |

### Memory
| Metric | Value |
|--------|-------|
| Average | 31.48% |
| Max | 34.10% |
| Min | 19.60% |

## 📊 Per-Sample Results

| Sample Name | Runs | Success | Avg Latency (ms) | Chars | Pages | Accuracy (%) |
|-------------|------|---------|------------------|-------|-------|--------------|
| `image_1.jpg` | 20 | 20 | 1233.90 | 6 | 0 | 100.00 |
| `image_10.jpg` | 20 | 20 | 1140.39 | 70 | 0 | 100.00 |
| `image_11.jpg` | 20 | 20 | 1179.07 | 7 | 0 | 100.00 |
| `image_12.jpg` | 20 | 20 | 1214.49 | 17 | 0 | 100.00 |
| `image_13.jpg` | 20 | 20 | 1230.39 | 36 | 0 | 100.00 |
| `image_14.jpg` | 20 | 20 | 1274.29 | 15 | 0 | 66.67 |
| `image_15.jpg` | 20 | 20 | 1210.17 | 10 | 0 | 100.00 |
| `image_16.jpg` | 20 | 20 | 1205.83 | 24 | 0 | 100.00 |
| `image_17.jpg` | 20 | 20 | 1059.41 | 12 | 0 | 100.00 |
| `image_18.jpg` | 20 | 20 | 1203.12 | 51 | 0 | 97.78 |
| `image_19.jpg` | 20 | 20 | 1208.79 | 8 | 0 | 100.00 |
| `image_2.jpg` | 20 | 20 | 1147.51 | 48 | 0 | 82.98 |
| `image_20.jpg` | 20 | 20 | 1225.61 | 32 | 0 | 90.32 |
| `image_3.jpg` | 20 | 20 | 1148.87 | 54 | 0 | 97.87 |
| `image_4.jpg` | 20 | 20 | 1191.17 | 55 | 0 | 59.42 |
| `image_5.jpg` | 20 | 20 | 1214.43 | 72 | 0 | 71.01 |
| `image_6.jpg` | 20 | 20 | 1268.09 | 52 | 0 | 93.62 |
| `image_7.jpg` | 20 | 20 | 1182.47 | 9 | 0 | 60.00 |
| `image_8.jpg` | 20 | 20 | 1084.96 | 27 | 0 | 78.79 |
| `image_9.jpg` | 20 | 20 | 1227.93 | 19 | 0 | 88.89 |

---