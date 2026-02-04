# OCR API Benchmark Report: default

**Generated at:** 2026-02-04 19:20:27

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
| **QPS** | **7.64** |
| **Success QPS** | 7.64 |
| **Total Duration (ms)** | 52372.98 |

## ⏱️ Latency Statistics

| Percentile | Latency (ms) |
|------------|--------------|
| Min | 522.82 |
| P50 (Median) | 2618.61 |
| P90 | 3034.82 |
| P95 | 3193.79 |
| P99 | 3498.46 |
| P99.9 | 3646.66 |
| Max | 3655.53 |
| Mean | 2594.17 |
| Std Dev | 421.29 |

## 📝 OCR Statistics

| Metric | Value |
|--------|-------|
| Total Characters | 13140 |
| Total Pages (pdf)| 0 |
| CPS (Chars/sec) | 236.88 |
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
| Average | 13.03% |
| Max | 35.00% |
| Min | 0.00% |

### Memory
| Metric | Value |
|--------|-------|
| Average | 10.04% |
| Max | 10.60% |
| Min | 6.90% |

## 📊 Per-Sample Results

| Sample Name | Runs | Success | Avg Latency (ms) | Chars | Pages | Accuracy (%) |
|-------------|------|---------|------------------|-------|-------|--------------|
| `image_1.jpg` | 20 | 20 | 2670.16 | 6 | 0 | 100.00 |
| `image_10.jpg` | 20 | 20 | 2398.55 | 69 | 0 | 100.00 |
| `image_11.jpg` | 20 | 20 | 2568.62 | 8 | 0 | 100.00 |
| `image_12.jpg` | 20 | 20 | 2618.32 | 17 | 0 | 100.00 |
| `image_13.jpg` | 20 | 20 | 2608.58 | 36 | 0 | 100.00 |
| `image_14.jpg` | 20 | 20 | 2601.06 | 21 | 0 | 100.00 |
| `image_15.jpg` | 20 | 20 | 2595.88 | 10 | 0 | 100.00 |
| `image_16.jpg` | 20 | 20 | 2645.14 | 24 | 0 | 100.00 |
| `image_17.jpg` | 20 | 20 | 2460.40 | 12 | 0 | 100.00 |
| `image_18.jpg` | 20 | 20 | 2773.89 | 49 | 0 | 100.00 |
| `image_19.jpg` | 20 | 20 | 2701.66 | 8 | 0 | 100.00 |
| `image_2.jpg` | 20 | 20 | 2367.23 | 49 | 0 | 100.00 |
| `image_20.jpg` | 20 | 20 | 2764.55 | 34 | 0 | 100.00 |
| `image_3.jpg` | 20 | 20 | 2407.42 | 52 | 0 | 100.00 |
| `image_4.jpg` | 20 | 20 | 2609.93 | 72 | 0 | 73.91 |
| `image_5.jpg` | 20 | 20 | 2605.53 | 72 | 0 | 72.46 |
| `image_6.jpg` | 20 | 20 | 2761.48 | 53 | 0 | 97.87 |
| `image_7.jpg` | 20 | 20 | 2579.03 | 11 | 0 | 100.00 |
| `image_8.jpg` | 20 | 20 | 2463.60 | 36 | 0 | 100.00 |
| `image_9.jpg` | 20 | 20 | 2682.36 | 18 | 0 | 94.44 |

---