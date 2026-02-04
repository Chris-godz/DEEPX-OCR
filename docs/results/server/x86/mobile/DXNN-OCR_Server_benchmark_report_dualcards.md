# OCR API Benchmark Report: default

**Generated at:** 2026-02-04 19:14:49

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
| **QPS** | **23.97** |
| **Success QPS** | 23.97 |
| **Total Duration (ms)** | 16689.23 |

## ⏱️ Latency Statistics

| Percentile | Latency (ms) |
|------------|--------------|
| Min | 2.73 |
| P50 (Median) | 763.87 |
| P90 | 1140.84 |
| P95 | 1290.13 |
| P99 | 1586.34 |
| P99.9 | 1737.66 |
| Max | 1758.20 |
| Mean | 788.05 |
| Std Dev | 287.47 |

## 📝 OCR Statistics

| Metric | Value |
|--------|-------|
| Total Characters | 12480 |
| Total Pages (pdf)| 0 |
| CPS (Chars/sec) | 692.24 |
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
| Average | 45.25% |
| Max | 77.20% |
| Min | 2.50% |

### Memory
| Metric | Value |
|--------|-------|
| Average | 10.35% |
| Max | 11.20% |
| Min | 6.70% |

## 📊 Per-Sample Results

| Sample Name | Runs | Success | Avg Latency (ms) | Chars | Pages | Accuracy (%) |
|-------------|------|---------|------------------|-------|-------|--------------|
| `image_1.jpg` | 20 | 20 | 810.90 | 6 | 0 | 100.00 |
| `image_10.jpg` | 20 | 20 | 651.30 | 70 | 0 | 100.00 |
| `image_11.jpg` | 20 | 20 | 683.41 | 7 | 0 | 100.00 |
| `image_12.jpg` | 20 | 20 | 805.70 | 17 | 0 | 100.00 |
| `image_13.jpg` | 20 | 20 | 953.77 | 36 | 0 | 100.00 |
| `image_14.jpg` | 20 | 20 | 851.02 | 15 | 0 | 66.67 |
| `image_15.jpg` | 20 | 20 | 807.36 | 10 | 0 | 100.00 |
| `image_16.jpg` | 20 | 20 | 791.71 | 24 | 0 | 100.00 |
| `image_17.jpg` | 20 | 20 | 615.28 | 12 | 0 | 100.00 |
| `image_18.jpg` | 20 | 20 | 830.90 | 51 | 0 | 97.78 |
| `image_19.jpg` | 20 | 20 | 851.90 | 8 | 0 | 100.00 |
| `image_2.jpg` | 20 | 20 | 683.85 | 47 | 0 | 82.98 |
| `image_20.jpg` | 20 | 20 | 854.86 | 33 | 0 | 93.55 |
| `image_3.jpg` | 20 | 20 | 661.97 | 54 | 0 | 97.87 |
| `image_4.jpg` | 20 | 20 | 885.44 | 55 | 0 | 60.87 |
| `image_5.jpg` | 20 | 20 | 938.13 | 72 | 0 | 71.01 |
| `image_6.jpg` | 20 | 20 | 896.66 | 52 | 0 | 93.62 |
| `image_7.jpg` | 20 | 20 | 772.83 | 9 | 0 | 60.00 |
| `image_8.jpg` | 20 | 20 | 622.15 | 27 | 0 | 78.79 |
| `image_9.jpg` | 20 | 20 | 791.87 | 19 | 0 | 88.89 |

---