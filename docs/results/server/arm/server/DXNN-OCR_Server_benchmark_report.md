# OCR API Benchmark Report: default

**Generated at:** 2026-02-04 18:05:07

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
| **QPS** | **7.45** |
| **Success QPS** | 7.45 |
| **Total Duration (ms)** | 53670.25 |

## ⏱️ Latency Statistics

| Percentile | Latency (ms) |
|------------|--------------|
| Min | 636.52 |
| P50 (Median) | 2646.28 |
| P90 | 3043.16 |
| P95 | 3422.70 |
| P99 | 4270.81 |
| P99.9 | 4697.79 |
| Max | 4712.97 |
| Mean | 2635.66 |
| Std Dev | 484.11 |

## 📝 OCR Statistics

| Metric | Value |
|--------|-------|
| Total Characters | 13160 |
| Total Pages (pdf)| 0 |
| CPS (Chars/sec) | 225.62 |
| Accuracy | 96.82% |

## ⚠️ Error Statistics

| Error Type | Count |
|------------|-------|

**Error Rate:** 0.00%
**Timeout Rate:** 0.00%

## 💻 Resource Monitoring

### CPU
| Metric | Value |
|--------|-------|
| Average | 18.47% |
| Max | 62.80% |
| Min | 1.30% |

### Memory
| Metric | Value |
|--------|-------|
| Average | 31.62% |
| Max | 34.90% |
| Min | 18.20% |

## 📊 Per-Sample Results

| Sample Name | Runs | Success | Avg Latency (ms) | Chars | Pages | Accuracy (%) |
|-------------|------|---------|------------------|-------|-------|--------------|
| `image_1.jpg` | 20 | 20 | 2764.02 | 6 | 0 | 100.00 |
| `image_10.jpg` | 20 | 20 | 2376.02 | 69 | 0 | 100.00 |
| `image_11.jpg` | 20 | 20 | 2659.89 | 8 | 0 | 100.00 |
| `image_12.jpg` | 20 | 20 | 2723.75 | 17 | 0 | 100.00 |
| `image_13.jpg` | 20 | 20 | 2851.10 | 36 | 0 | 100.00 |
| `image_14.jpg` | 20 | 20 | 2747.46 | 21 | 0 | 100.00 |
| `image_15.jpg` | 20 | 20 | 2538.95 | 10 | 0 | 100.00 |
| `image_16.jpg` | 20 | 20 | 2601.85 | 24 | 0 | 100.00 |
| `image_17.jpg` | 20 | 20 | 2416.33 | 12 | 0 | 100.00 |
| `image_18.jpg` | 20 | 20 | 2845.77 | 50 | 0 | 97.78 |
| `image_19.jpg` | 20 | 20 | 2711.08 | 8 | 0 | 100.00 |
| `image_2.jpg` | 20 | 20 | 2412.36 | 49 | 0 | 100.00 |
| `image_20.jpg` | 20 | 20 | 2766.53 | 34 | 0 | 100.00 |
| `image_3.jpg` | 20 | 20 | 2475.18 | 52 | 0 | 100.00 |
| `image_4.jpg` | 20 | 20 | 2621.66 | 72 | 0 | 73.91 |
| `image_5.jpg` | 20 | 20 | 2699.79 | 72 | 0 | 72.46 |
| `image_6.jpg` | 20 | 20 | 2841.57 | 53 | 0 | 97.87 |
| `image_7.jpg` | 20 | 20 | 2561.10 | 11 | 0 | 100.00 |
| `image_8.jpg` | 20 | 20 | 2398.04 | 36 | 0 | 100.00 |
| `image_9.jpg` | 20 | 20 | 2700.79 | 18 | 0 | 94.44 |

---