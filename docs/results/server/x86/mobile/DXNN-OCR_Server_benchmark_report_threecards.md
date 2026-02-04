# OCR API Benchmark Report: default

**Generated at:** 2026-02-04 19:10:55

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
| **QPS** | **28.00** |
| **Success QPS** | 28.00 |
| **Total Duration (ms)** | 14284.70 |

## ⏱️ Latency Statistics

| Percentile | Latency (ms) |
|------------|--------------|
| Min | 3.17 |
| P50 (Median) | 564.74 |
| P90 | 1068.25 |
| P95 | 1253.24 |
| P99 | 1299.82 |
| P99.9 | 1381.64 |
| Max | 1413.37 |
| Mean | 635.59 |
| Std Dev | 283.14 |

## 📝 OCR Statistics

| Metric | Value |
|--------|-------|
| Total Characters | 12480 |
| Total Pages (pdf)| 0 |
| CPS (Chars/sec) | 801.66 |
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
| Average | 54.42% |
| Max | 100.00% |
| Min | 0.00% |

### Memory
| Metric | Value |
|--------|-------|
| Average | 12.38% |
| Max | 13.40% |
| Min | 7.70% |

## 📊 Per-Sample Results

| Sample Name | Runs | Success | Avg Latency (ms) | Chars | Pages | Accuracy (%) |
|-------------|------|---------|------------------|-------|-------|--------------|
| `image_1.jpg` | 20 | 20 | 663.44 | 6 | 0 | 100.00 |
| `image_10.jpg` | 20 | 20 | 489.04 | 70 | 0 | 100.00 |
| `image_11.jpg` | 20 | 20 | 631.41 | 7 | 0 | 100.00 |
| `image_12.jpg` | 20 | 20 | 619.06 | 17 | 0 | 100.00 |
| `image_13.jpg` | 20 | 20 | 697.71 | 36 | 0 | 100.00 |
| `image_14.jpg` | 20 | 20 | 724.45 | 15 | 0 | 66.67 |
| `image_15.jpg` | 20 | 20 | 584.35 | 10 | 0 | 100.00 |
| `image_16.jpg` | 20 | 20 | 684.82 | 24 | 0 | 100.00 |
| `image_17.jpg` | 20 | 20 | 442.92 | 12 | 0 | 100.00 |
| `image_18.jpg` | 20 | 20 | 699.57 | 51 | 0 | 97.78 |
| `image_19.jpg` | 20 | 20 | 685.28 | 8 | 0 | 100.00 |
| `image_2.jpg` | 20 | 20 | 499.03 | 47 | 0 | 82.98 |
| `image_20.jpg` | 20 | 20 | 693.17 | 33 | 0 | 93.55 |
| `image_3.jpg` | 20 | 20 | 570.63 | 54 | 0 | 97.87 |
| `image_4.jpg` | 20 | 20 | 704.90 | 55 | 0 | 60.87 |
| `image_5.jpg` | 20 | 20 | 754.02 | 72 | 0 | 71.01 |
| `image_6.jpg` | 20 | 20 | 723.42 | 52 | 0 | 93.62 |
| `image_7.jpg` | 20 | 20 | 706.64 | 9 | 0 | 60.00 |
| `image_8.jpg` | 20 | 20 | 475.49 | 27 | 0 | 78.79 |
| `image_9.jpg` | 20 | 20 | 662.44 | 19 | 0 | 88.89 |

---