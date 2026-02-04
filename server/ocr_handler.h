#pragma once

#include "pipeline/ocr_pipeline.h"
#include "file_handler.h"
#include "json_response.h"
#include "pdf_handler.h"
#include <nlohmann/json.hpp>
#include <opencv2/opencv.hpp>
#include <memory>
#include <string>
#include <map>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <optional>
#include <chrono>

using json = nlohmann::json;

namespace ocr_server {

/**
 * @brief OCR请求参数结构
 */
struct OCRRequest {
    std::string file;                       // Base64编码或URL
    int fileType = 1;                       // 1: 图像, 0: PDF
    bool useDocOrientationClassify = false; // 文档方向矫正
    bool useDocUnwarping = false;           // 图片扭曲矫正
    bool useTextlineOrientation = false;    // 文本行方向矫正
    int textDetLimitSideLen = 64;           // 图像边长限制（接收但不实际使用）
    std::string textDetLimitType = "min";   // 边长限制类型: "min" 或 "max"（接收但不实际使用）
    double textDetThresh = 0.3;             // 检测像素阈值
    double textDetBoxThresh = 0.6;          // 检测框阈值
    double textDetUnclipRatio = 1.5;        // 检测扩张系数
    double textRecScoreThresh = 0.0;        // 识别置信度阈值
    bool visualize = false;                 // 是否开启可视化
    
    // 请求大小限制
    static constexpr size_t MAX_BASE64_SIZE = 50 * 1024 * 1024;     // 50MB Base64
    static constexpr size_t MAX_URL_LENGTH = 2048;                  // URL 长度限制
    
    // PDF 参数配置
    int pdfDpi = 150;                       // PDF 渲染 DPI (默认 150)
    int pdfMaxPages = 10;                   // 最大处理页数 (默认 10)
    
    /**
     * @brief 从JSON解析请求参数
     */
    static OCRRequest FromJson(const json& j);
    
    /**
     * @brief 验证请求参数
     */
    bool Validate(std::string& error_msg) const;
};

/**
 * @brief OCR请求处理器
 * 
 * 采用异步模式架构：
 * - POST /ocr/submit: 提交任务立即返回 task_id
 * - GET /ocr/result/{id}: 轮询获取结果
 */
class OCRHandler {
public:
    /**
     * @brief 任务状态枚举
     */
    enum class TaskStatus {
        PENDING,      // 已接收，等待进入 Pipeline
        PROCESSING,   // 正在处理
        COMPLETED,    // 处理完成
        FAILED,       // 处理失败
        NOT_FOUND     // 任务不存在
    };

    /**
     * @brief 构造函数
     * @param pipeline_config OCR Pipeline配置
     * @param vis_output_dir 可视化图片输出目录
     * @param vis_url_prefix 可视化图片URL前缀
     */
    OCRHandler(
        const ocr::OCRPipelineConfig& pipeline_config,
        const std::string& vis_output_dir = "output/vis",
        const std::string& vis_url_prefix = "/static/vis"
    );
    
    /**
     * @brief 析构函数
     */
    ~OCRHandler();
    
    // ==================== 异步模式接口 ====================
    
    /**
     * @brief 异步提交图像 OCR 任务（非阻塞）
     * @param request OCR请求参数
     * @param error_msg 错误信息（如果失败）
     * @return task_id (>0) 成功，-1 失败
     */
    int64_t SubmitImageTask(const OCRRequest& request, std::string& error_msg);
    
    /**
     * @brief 异步提交 PDF OCR 任务（非阻塞）
     * @param request OCR请求参数  
     * @param error_msg 错误信息（如果失败）
     * @return task_id (>0) 成功，-1 失败
     */
    int64_t SubmitPDFTask(const OCRRequest& request, std::string& error_msg);
    
    /**
     * @brief 非阻塞查询图像任务结果
     * @param task_id 任务 ID
     * @param response_json 输出的 JSON 响应
     * @return true 结果已就绪，false 仍在处理中
     */
    bool TryGetImageResult(int64_t task_id, json& response_json);
    
    /**
     * @brief 非阻塞查询 PDF 任务结果
     * @param task_id 任务 ID
     * @param response_json 输出的 JSON 响应
     * @return true 结果已就绪，false 仍在处理中
     */
    bool TryGetPDFResult(int64_t task_id, json& response_json);
    
    /**
     * @brief 查询任务状态
     * @param task_id 任务 ID
     * @return 任务状态
     */
    TaskStatus GetTaskStatus(int64_t task_id);
    
    /**
     * @brief 查询任务类型
     * @param task_id 任务 ID
     * @return "image", "pdf", 或 "" (未找到)
     */
    std::string GetTaskType(int64_t task_id);
    
private:
    /**
     * @brief 从请求参数创建OCR Pipeline配置
     */
    ocr::OCRPipelineConfig CreatePipelineConfig(const OCRRequest& request) const;
    
    /**
     * @brief 加载输入图像（Base64或URL）
     */
    bool LoadInputImage(const OCRRequest& request, cv::Mat& image, std::string& error_msg);
    
    std::shared_ptr<ocr::OCRPipeline> base_pipeline_;  // 基础Pipeline实例
    ocr::OCRPipelineConfig base_config_;               // 基础配置
    std::string vis_output_dir_;                       // 可视化输出目录
    std::string vis_url_prefix_;                       // 可视化URL前缀
    
    // ==================== 结果存储（同步 + 异步共用） ====================
    struct TaskResult {
        std::vector<ocr::PipelineOCRResult> results;
        cv::Mat processedImage;
        bool success = true;  // 任务是否成功（false 表示检测/识别引擎异常）
    };
    std::map<int64_t, TaskResult> result_store_;       // task_id -> 结果
    std::mutex result_mutex_;                           // 保护 result_store_
    std::condition_variable result_cv_;                 // 通知等待的请求
    std::thread result_collector_thread_;               // 后台结果收集线程
    std::atomic<bool> collector_running_{false};        // 收集线程运行标志
    
    // ==================== 异步任务状态管理 ====================
    struct TaskMeta {
        TaskStatus status = TaskStatus::PENDING;
        std::string taskType;  // "image" or "pdf"
        std::chrono::steady_clock::time_point createTime;
        OCRRequest request;    // 保存原始请求（用于可视化等）
        // PDF 任务特有
        int totalPages = 0;
        int renderedPages = 0;
        std::vector<int64_t> pageTaskIds;  // 各页的子任务 ID
    };
    std::map<int64_t, TaskMeta> task_meta_;            // task_id -> 任务元信息
    std::mutex task_meta_mutex_;                        // 保护 task_meta_
    
    // 定期清理过期任务（防止内存泄漏）
    std::thread cleanup_thread_;
    std::atomic<bool> cleanup_running_{false};
    void CleanupExpiredTasks();
    static constexpr int TASK_EXPIRE_SECONDS = 300;    // 任务过期时间：5 分钟
    
    void StartResultCollector();                        // 启动结果收集线程
    void StopResultCollector();                         // 停止结果收集线程
    void ResultCollectorLoop();                         // 结果收集循环
    bool WaitForResult(int64_t task_id, std::vector<ocr::PipelineOCRResult>& results, 
                       cv::Mat& processedImage, bool& success, int timeout_ms = 10000);
    
    // ==================== PDF 处理相关 ====================
    
    PDFHandler pdf_handler_;                            // PDF 处理器
    
    /**
     * @brief 生成唯一任务 ID
     */
    static int64_t GenerateTaskId();
    
    /**
     * @brief 保存可视化图片并返回 URL
     * @param image 处理后的图像
     * @param results OCR 结果
     * @param pageIndex 页码 (-1 表示非 PDF)
     */
    std::string SaveVisualization(const cv::Mat& image, 
                                   const std::vector<ocr::PipelineOCRResult>& results,
                                   int pageIndex = -1);
};

} // namespace ocr_server
