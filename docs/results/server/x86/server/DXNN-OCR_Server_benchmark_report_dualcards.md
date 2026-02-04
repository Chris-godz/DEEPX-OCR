# OCR API Benchmark Report: default

**Generated at:** 2026-02-04 19:21:34

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
| **QPS** | **13.62** |
| **Success QPS** | 13.62 |
| **Total Duration (ms)** | 29363.48 |

## ⏱️ Latency Statistics

| Percentile | Latency (ms) |
|------------|--------------|
| Min | 168.51 |
| P50 (Median) | 1438.99 |
| P90 | 1593.99 |
| P95 | 1645.97 |
| P99 | 1786.95 |
| P99.9 | 1854.59 |
| Max | 1858.92 |
| Mean | 1423.65 |
| Std Dev | 194.00 |

## 📝 OCR Statistics

| Metric | Value |
|--------|-------|
| Total Characters | 12480 |
| Total Pages (pdf)| 0 |
| CPS (Chars/sec) | 401.24 |
| Accuracy | 89.60% |

## ⚠️ Error Statistics

| Error Type | Count |
|------------|-------|

**Error Rate:** 0.00%
**Timeout Rate:** 0.00%

## 💻 Resource Monitoring

### CPU
| Metric | Value |
|--------|-------|
| Average | 31.60% |
| Max | 52.50% |
| Min | 0.00% |

### Memory
| Metric | Value |
|--------|-------|
| Average | 9.16% |
| Max | 9.80% |
| Min | 6.40% |

## 📊 Per-Sample Results

| Sample Name | Runs | Success | Avg Latency (ms) | Chars | Pages | Accuracy (%) |
|-------------|------|---------|------------------|-------|-------|--------------|
| `image_1.jpg` | 20 | 20 | 1423.54 | 6 | 0 | 100.00 |
| `image_10.jpg` | 20 | 20 | 1321.57 | 70 | 0 | 100.00 |
| `image_11.jpg` | 20 | 20 | 1411.27 | 7 | 0 | 100.00 |
| `image_12.jpg` | 20 | 20 | 1424.68 | 17 | 0 | 100.00 |
| `image_13.jpg` | 20 | 20 | 1426.50 | 36 | 0 | 100.00 |
| `image_14.jpg` | 20 | 20 | 1451.88 | 15 | 0 | 66.67 |
| `image_15.jpg` | 20 | 20 | 1418.31 | 10 | 0 | 100.00 |
| `image_16.jpg` | 20 | 20 | 1506.50 | 24 | 0 | 100.00 |
| `image_17.jpg` | 20 | 20 | 1235.45 | 12 | 0 | 100.00 |
| `image_18.jpg` | 20 | 20 | 1472.42 | 51 | 0 | 97.78 |
| `image_19.jpg` | 20 | 20 | 1517.45 | 8 | 0 | 100.00 |
| `image_2.jpg` | 20 | 20 | 1330.85 | 47 | 0 | 82.98 |
| `image_20.jpg` | 20 | 20 | 1477.35 | 33 | 0 | 93.55 |
| `image_3.jpg` | 20 | 20 | 1346.80 | 54 | 0 | 97.87 |
| `image_4.jpg` | 20 | 20 | 1464.59 | 55 | 0 | 60.87 |
| `image_5.jpg` | 20 | 20 | 1454.18 | 72 | 0 | 71.01 |
| `image_6.jpg` | 20 | 20 | 1511.20 | 52 | 0 | 93.62 |
| `image_7.jpg` | 20 | 20 | 1520.54 | 9 | 0 | 60.00 |
| `image_8.jpg` | 20 | 20 | 1306.26 | 27 | 0 | 78.79 |
| `image_9.jpg` | 20 | 20 | 1451.70 | 19 | 0 | 88.89 |

---