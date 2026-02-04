# OCR API Benchmark Report: default

**Generated at:** 2026-02-04 19:09:42

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
| **QPS** | **21.50** |
| **Success QPS** | 21.50 |
| **Total Duration (ms)** | 18607.03 |

## ⏱️ Latency Statistics

| Percentile | Latency (ms) |
|------------|--------------|
| Min | 343.51 |
| P50 (Median) | 907.47 |
| P90 | 1278.85 |
| P95 | 1384.72 |
| P99 | 1517.51 |
| P99.9 | 1562.47 |
| Max | 1576.08 |
| Mean | 900.14 |
| Std Dev | 268.77 |

## 📝 OCR Statistics

| Metric | Value |
|--------|-------|
| Total Characters | 13140 |
| Total Pages (pdf)| 0 |
| CPS (Chars/sec) | 605.96 |
| Accuracy | 96.93% |

## ⚠️ Error Statistics

| Error Type | Count |
|------------|-------|

**Error Rate:** 0.00%
**Timeout Rate:** 0.00%

## 💻 Resource Monitoring

### CPU
| Metric | Value |
|--------|-------|
| Average | 34.70% |
| Max | 68.40% |
| Min | 1.20% |

### Memory
| Metric | Value |
|--------|-------|
| Average | 11.33% |
| Max | 12.30% |
| Min | 7.80% |

## 📊 Per-Sample Results

| Sample Name | Runs | Success | Avg Latency (ms) | Chars | Pages | Accuracy (%) |
|-------------|------|---------|------------------|-------|-------|--------------|
| `image_1.jpg` | 20 | 20 | 888.59 | 6 | 0 | 100.00 |
| `image_10.jpg` | 20 | 20 | 653.22 | 69 | 0 | 100.00 |
| `image_11.jpg` | 20 | 20 | 888.64 | 8 | 0 | 100.00 |
| `image_12.jpg` | 20 | 20 | 933.89 | 17 | 0 | 100.00 |
| `image_13.jpg` | 20 | 20 | 977.01 | 36 | 0 | 100.00 |
| `image_14.jpg` | 20 | 20 | 962.80 | 21 | 0 | 100.00 |
| `image_15.jpg` | 20 | 20 | 899.11 | 10 | 0 | 100.00 |
| `image_16.jpg` | 20 | 20 | 937.78 | 24 | 0 | 100.00 |
| `image_17.jpg` | 20 | 20 | 618.64 | 12 | 0 | 100.00 |
| `image_18.jpg` | 20 | 20 | 1010.53 | 49 | 0 | 100.00 |
| `image_19.jpg` | 20 | 20 | 991.29 | 8 | 0 | 100.00 |
| `image_2.jpg` | 20 | 20 | 804.34 | 49 | 0 | 100.00 |
| `image_20.jpg` | 20 | 20 | 1055.99 | 34 | 0 | 100.00 |
| `image_3.jpg` | 20 | 20 | 707.90 | 52 | 0 | 100.00 |
| `image_4.jpg` | 20 | 20 | 1094.01 | 72 | 0 | 73.91 |
| `image_5.jpg` | 20 | 20 | 1023.99 | 72 | 0 | 72.46 |
| `image_6.jpg` | 20 | 20 | 1076.92 | 53 | 0 | 97.87 |
| `image_7.jpg` | 20 | 20 | 909.98 | 11 | 0 | 100.00 |
| `image_8.jpg` | 20 | 20 | 623.14 | 36 | 0 | 100.00 |
| `image_9.jpg` | 20 | 20 | 944.97 | 18 | 0 | 94.44 |

---