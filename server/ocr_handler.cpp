#include "ocr_handler.h"
#include "common/logger.hpp"
#include "common/visualizer.h"
#include <regex>

namespace ocr_server {

// ==================== OCRRequest ====================

OCRRequest OCRRequest::FromJson(const json& j) {
    OCRRequest req;
    
    // 必填字段
    if (j.contains("file") && j["file"].is_string()) {
        req.file = j["file"].get<std::string>();
    }
    
    // 可选字段（使用默认值）
    if (j.contains("fileType")) req.fileType = j["fileType"].get<int>();
    if (j.contains("useDocOrientationClassify")) req.useDocOrientationClassify = j["useDocOrientationClassify"].get<bool>();
    if (j.contains("useDocUnwarping")) req.useDocUnwarping = j["useDocUnwarping"].get<bool>();
    if (j.contains("useTextlineOrientation")) req.useTextlineOrientation = j["useTextlineOrientation"].get<bool>();
    if (j.contains("textDetLimitSideLen")) req.textDetLimitSideLen = j["textDetLimitSideLen"].get<int>();
    if (j.contains("textDetLimitType")) req.textDetLimitType = j["textDetLimitType"].get<std::string>();
    if (j.contains("textDetThresh")) req.textDetThresh = j["textDetThresh"].get<double>();
    if (j.contains("textDetBoxThresh")) req.textDetBoxThresh = j["textDetBoxThresh"].get<double>();
    if (j.contains("textDetUnclipRatio")) req.textDetUnclipRatio = j["textDetUnclipRatio"].get<double>();
    if (j.contains("textRecScoreThresh")) req.textRecScoreThresh = j["textRecScoreThresh"].get<double>();
    if (j.contains("visualize")) req.visualize = j["visualize"].get<bool>();
    
    // PDF 专用参数
    if (j.contains("pdfDpi")) req.pdfDpi = j["pdfDpi"].get<int>();
    if (j.contains("pdfMaxPages")) req.pdfMaxPages = j["pdfMaxPages"].get<int>();
    
    return req;
}

bool OCRRequest::Validate(std::string& error_msg) const {
    // 检查必填字段
    if (file.empty()) {
        error_msg = "Missing required parameter: 'file'";
        return false;
    }
    
    // 检查文件大小限制
    bool is_url = (file.find("http://") == 0 || file.find("https://") == 0);
    
    if (is_url) {
        // URL 长度限制
        if (file.size() > MAX_URL_LENGTH) {
            error_msg = fmt::format("URL too long (max {} characters)", MAX_URL_LENGTH);
            return false;
        }
    } else {
        // Base64 大小限制
        if (file.size() > MAX_BASE64_SIZE) {
            error_msg = fmt::format("File too large (max {} MB)", 
                                    MAX_BASE64_SIZE / (1024 * 1024));
            return false;
        }
    }

    // 检查fileType
    if (fileType != 0 && fileType != 1) {
        error_msg = "fileType must be 0 (PDF) or 1 (Image)";
        return false;
    }
    
    // PDF 参数验证
    if (fileType == 0) {
        // DPI 限制
        if (pdfDpi < PDFConstants::MIN_DPI || pdfDpi > PDFConstants::MAX_DPI) {
            error_msg = fmt::format("pdfDpi must be in range [{}, {}]",
                                    PDFConstants::MIN_DPI, PDFConstants::MAX_DPI);
            return false;
        }
        
        // 页数限制
        if (pdfMaxPages < PDFConstants::MIN_PAGES || pdfMaxPages > PDFConstants::MAX_PAGES) {
            error_msg = fmt::format("pdfMaxPages must be in range [{}, {}]",
                                    PDFConstants::MIN_PAGES, PDFConstants::MAX_PAGES);
            return false;
        }
        
        // 内存预估警告（A4 @ 150 DPI ~= 8.7MB/页）
        if (pdfMaxPages > PDFConstants::HIGH_MEMORY_PAGE_THRESHOLD && 
            pdfDpi > PDFConstants::HIGH_MEMORY_DPI_THRESHOLD) {
            LOG_WARN("High memory usage expected: {} pages at {} DPI", 
                     pdfMaxPages, pdfDpi);
        }
    }
    
    // textDetLimitSideLen 和 textDetLimitType: 接收但不实际使用，只做基本验证
    // 不会因为这两个参数的值而拒绝请求
    if (textDetLimitSideLen < 1) {
        LOG_WARN("textDetLimitSideLen={} is too small, will use default model selection", textDetLimitSideLen);
    }
    if (textDetLimitType != "min" && textDetLimitType != "max") {
        LOG_WARN("textDetLimitType='{}' is invalid (should be 'min' or 'max'), ignored", textDetLimitType);
    }
    
    // 检查实际使用的参数范围
    if (textDetThresh < 0.0 || textDetThresh > 1.0) {
        error_msg = "textDetThresh must be in range [0.0, 1.0]";
        return false;
    }
    
    if (textDetBoxThresh < 0.0 || textDetBoxThresh > 1.0) {
        error_msg = "textDetBoxThresh must be in range [0.0, 1.0]";
        return false;
    }
    
    if (textDetUnclipRatio < 1.0 || textDetUnclipRatio > 3.0) {
        error_msg = "textDetUnclipRatio must be in range [1.0, 3.0]";
        return false;
    }
    
    if (textRecScoreThresh < 0.0 || textRecScoreThresh > 1.0) {
        error_msg = "textRecScoreThresh must be in range [0.0, 1.0]";
        return false;
    }
    
    return true;
}

// ==================== OCRHandler ====================

OCRHandler::OCRHandler(
    const ocr::OCRPipelineConfig& pipeline_config,
    const std::string& vis_output_dir,
    const std::string& vis_url_prefix)
    : base_config_(pipeline_config)
    , vis_output_dir_(vis_output_dir)
    , vis_url_prefix_(vis_url_prefix) {
    
    // 创建基础Pipeline实例（会被每次请求的配置覆盖）
    base_pipeline_ = std::make_shared<ocr::OCRPipeline>(base_config_);
    LOG_INFO("OCRHandler initialized");
}

OCRHandler::~OCRHandler() {
    // 停止清理线程
    if (cleanup_running_) {
        cleanup_running_ = false;
        if (cleanup_thread_.joinable()) {
            cleanup_thread_.join();
        }
    }
    // 停止结果收集线程
    StopResultCollector();
    LOG_INFO("OCRHandler destroyed");
}

void OCRHandler::CleanupExpiredTasks() {
    while (cleanup_running_) {
        std::this_thread::sleep_for(std::chrono::seconds(30));  // 每 30 秒清理一次
        
        auto now = std::chrono::steady_clock::now();
        int cleaned = 0;
        
        {
            std::lock_guard<std::mutex> lock(task_meta_mutex_);
            for (auto it = task_meta_.begin(); it != task_meta_.end(); ) {
                auto age = std::chrono::duration_cast<std::chrono::seconds>(
                    now - it->second.createTime).count();
                
                if (age > TASK_EXPIRE_SECONDS) {
                    int64_t task_id = it->first;
                    it = task_meta_.erase(it);
                    
                    // 同时清理 result_store_
                    {
                        std::lock_guard<std::mutex> result_lock(result_mutex_);
                        result_store_.erase(task_id);
                    }
                    cleaned++;
                } else {
                    ++it;
                }
            }
        }
        
        if (cleaned > 0) {
            LOG_INFO("[CLEANUP] Removed {} expired tasks", cleaned);
        }
    }
}

void OCRHandler::StartResultCollector() {
    if (collector_running_) return;
    collector_running_ = true;
    result_collector_thread_ = std::thread(&OCRHandler::ResultCollectorLoop, this);
    LOG_INFO("Result collector thread started");
    
    // 同时启动清理线程
    if (!cleanup_running_) {
        cleanup_running_ = true;
        cleanup_thread_ = std::thread(&OCRHandler::CleanupExpiredTasks, this);
        LOG_INFO("Task cleanup thread started");
    }
}

void OCRHandler::StopResultCollector() {
    if (!collector_running_) return;
    collector_running_ = false;
    if (result_collector_thread_.joinable()) {
        result_collector_thread_.join();
    }
    LOG_INFO("Result collector thread stopped");
}

void OCRHandler::ResultCollectorLoop() {
    while (collector_running_) {
        std::vector<ocr::PipelineOCRResult> results;
        int64_t result_id;
        cv::Mat processed_image;
        bool success = true;
        
        if (base_pipeline_->getResult(results, result_id, &processed_image, &success)) {
            if (!success) {
                LOG_WARN("[COLLECTOR] Task failed for task_id={}", result_id);
            } else {
                LOG_DEBUG("[COLLECTOR] Got result for task_id={}, storing in map", result_id);
            }
            
            {
                std::lock_guard<std::mutex> lock(result_mutex_);
                result_store_[result_id] = TaskResult{std::move(results), std::move(processed_image), success};
            }
            result_cv_.notify_all();  // 通知所有等待的请求
        }
    }
}

bool OCRHandler::WaitForResult(int64_t task_id, std::vector<ocr::PipelineOCRResult>& results, 
                                cv::Mat& processedImage, bool& success, int timeout_ms) {
    std::unique_lock<std::mutex> lock(result_mutex_);
    
    auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms);
    
    while (true) {
        auto it = result_store_.find(task_id);
        if (it != result_store_.end()) {
            // 找到结果
            results = std::move(it->second.results);
            processedImage = std::move(it->second.processedImage);
            success = it->second.success;
            result_store_.erase(it);
            LOG_DEBUG("[WAIT] Found result for task_id={}, success={}", task_id, success);
            return true;
        }
        
        // 等待通知或超时
        if (result_cv_.wait_until(lock, deadline) == std::cv_status::timeout) {
            LOG_WARN("[WAIT] Timeout waiting for task_id={}", task_id);
            success = false;  // 超时也视为失败
            return false;
        }
    }
}

int64_t OCRHandler::GenerateTaskId() {
    static std::atomic<int64_t> task_counter{0};
    return ++task_counter;
}

std::string OCRHandler::SaveVisualization(const cv::Mat& image, 
                                           const std::vector<ocr::PipelineOCRResult>& results,
                                           int pageIndex) {
    if (image.empty()) return "";
    
    // 将 PipelineOCRResult 转换为 TextBox 以便使用 Visualizer
    std::vector<ocr::TextBox> text_boxes;
    for (const auto& result : results) {
        ocr::TextBox box;
        for (size_t i = 0; i < 4 && i < result.box.size(); ++i) {
            box.points[i] = result.box[i];
        }
        box.text = result.text;
        box.confidence = result.confidence;
        box.rotated = false;
        text_boxes.push_back(box);
    }
    
    // 使用可视化器生成带框的图像
    cv::Mat vis_image = ocr::Visualizer::drawOCRResults(image, text_boxes, true, true);
    
    // 生成文件名（支持页码后缀）
    std::string vis_filename;
    if (pageIndex >= 0) {
        // PDF 多页模式：添加页码后缀
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).count();
        vis_filename = fmt::format("ocr_vis_{}_page{}.jpg", timestamp, pageIndex);
        
        std::string full_path = vis_output_dir_ + "/" + vis_filename;
        if (cv::imwrite(full_path, vis_image)) {
            LOG_INFO("Visualization image saved: {}", full_path);
        } else {
            LOG_ERROR("Failed to save visualization image: {}", full_path);
            return "";
        }
    } else {
        // 单图模式：使用 FileHandler
        vis_filename = FileHandler::SaveVisualizationImage(vis_image, vis_output_dir_);
    }
    
    if (!vis_filename.empty()) {
        return vis_url_prefix_ + "/" + vis_filename;
    }
    return "";
}

ocr::OCRPipelineConfig OCRHandler::CreatePipelineConfig(const OCRRequest& request) const {
    ocr::OCRPipelineConfig config = base_config_;
    
    // 文档预处理配置
    config.useDocPreprocessing = request.useDocOrientationClassify || request.useDocUnwarping;
    config.docPreprocessingConfig.useOrientation = request.useDocOrientationClassify;
    config.docPreprocessingConfig.useUnwarping = request.useDocUnwarping;
    
    // 文本行方向分类
    config.useClassification = request.useTextlineOrientation;
    
    // 检测参数
    config.detectorConfig.sizeThreshold = request.textDetLimitSideLen;
    config.detectorConfig.thresh = static_cast<float>(request.textDetThresh);
    config.detectorConfig.boxThresh = static_cast<float>(request.textDetBoxThresh);
    config.detectorConfig.unclipRatio = static_cast<float>(request.textDetUnclipRatio);
    
    // 识别参数（需要在RecognizerConfig中添加scoreThresh字段）
    // config.recognizerConfig.scoreThresh = static_cast<float>(request.textRecScoreThresh);
    
    // 可视化
    config.enableVisualization = request.visualize;
    
    return config;
}

bool OCRHandler::LoadInputImage(const OCRRequest& request, cv::Mat& image, std::string& error_msg) {
    // 判断是Base64还是URL
    bool is_url = false;
    if (request.file.find("http://") == 0 || request.file.find("https://") == 0) {
        is_url = true;
    }
    
    if (is_url) {
        // 从URL下载
        LOG_INFO("Downloading image from URL...");
        if (!FileHandler::DownloadImageFromURL(request.file, image)) {
            error_msg = "Failed to download image from URL";
            return false;
        }
    } else {
        // Base64解码
        LOG_INFO("Decoding Base64 image...");
        if (!FileHandler::DecodeBase64Image(request.file, image)) {
            error_msg = "Failed to decode Base64 image";
            return false;
        }
    }
    
    return true;
}

// ==================== 异步接口实现 ====================

int64_t OCRHandler::SubmitImageTask(const OCRRequest& request, std::string& error_msg) {
    // 1. 验证请求参数
    if (!request.Validate(error_msg)) {
        LOG_WARN("[SUBMIT] Invalid request: {}", error_msg);
        return -1;
    }
    
    // 2. 确保 pipeline 已初始化
    static std::once_flag init_flag;
    std::call_once(init_flag, [this]() {
        if (!base_pipeline_->initialize()) {
            LOG_ERROR("Failed to initialize base pipeline");
            throw std::runtime_error("Failed to initialize OCR pipeline");
        }
        base_pipeline_->start();
        LOG_INFO("Base pipeline initialized and started");
        StartResultCollector();
    });
    
    // 3. 加载图像
    cv::Mat image;
    if (!LoadInputImage(request, image, error_msg)) {
        LOG_ERROR("[SUBMIT] Failed to load image: {}", error_msg);
        return -1;
    }
    LOG_INFO("[SUBMIT] Image loaded: {}x{}", image.cols, image.rows);
    
    // 4. 构建任务配置
    ocr::OCRTaskConfig taskConfig;
    taskConfig.useDocOrientationClassify = request.useDocOrientationClassify;
    taskConfig.useDocUnwarping = request.useDocUnwarping;
    taskConfig.useTextlineOrientation = request.useTextlineOrientation;
    taskConfig.textDetThresh = static_cast<float>(request.textDetThresh);
    taskConfig.textDetBoxThresh = static_cast<float>(request.textDetBoxThresh);
    taskConfig.textDetUnclipRatio = static_cast<float>(request.textDetUnclipRatio);
    taskConfig.textRecScoreThresh = static_cast<float>(request.textRecScoreThresh);
    
    // 5. 生成任务 ID 并初始化状态
    int64_t task_id = GenerateTaskId();
    
    {
        std::lock_guard<std::mutex> lock(task_meta_mutex_);
        TaskMeta meta;
        meta.status = TaskStatus::PENDING;
        meta.taskType = "image";
        meta.createTime = std::chrono::steady_clock::now();
        meta.request = request;
        task_meta_[task_id] = meta;
    }
    
    // 6. 提交到 Pipeline
    if (!base_pipeline_->pushTask(image, task_id, taskConfig)) {
        // 队列满，清理状态
        {
            std::lock_guard<std::mutex> lock(task_meta_mutex_);
            task_meta_.erase(task_id);
        }
        error_msg = "Pipeline queue is full, please retry later";
        LOG_WARN("[SUBMIT] Pipeline queue full for task_id={}", task_id);
        return -1;
    }
    
    // 7. 更新状态为处理中
    {
        std::lock_guard<std::mutex> lock(task_meta_mutex_);
        if (task_meta_.count(task_id)) {
            task_meta_[task_id].status = TaskStatus::PROCESSING;
        }
    }
    
    LOG_INFO("[SUBMIT] Image task {} submitted successfully", task_id);
    return task_id;
}

int64_t OCRHandler::SubmitPDFTask(const OCRRequest& request, std::string& error_msg) {
    // 1. 验证请求参数
    if (!request.Validate(error_msg)) {
        LOG_WARN("[SUBMIT] Invalid PDF request: {}", error_msg);
        return -1;
    }
    
    // 2. 确保 pipeline 已初始化
    static std::once_flag init_flag;
    std::call_once(init_flag, [this]() {
        if (!base_pipeline_->initialize()) {
            LOG_ERROR("Failed to initialize base pipeline");
            throw std::runtime_error("Failed to initialize OCR pipeline");
        }
        base_pipeline_->start();
        LOG_INFO("Base pipeline initialized and started");
        StartResultCollector();
    });
    
    // 3. 渲染 PDF
    PDFRenderConfig pdfConfig;
    pdfConfig.dpi = request.pdfDpi;
    pdfConfig.maxPages = request.pdfMaxPages;
    pdfConfig.maxDpi = 300;
    
    PDFRenderResult renderResult;
    bool isURL = (request.file.find("http://") == 0 || request.file.find("https://") == 0);
    
    if (isURL) {
        renderResult = pdf_handler_.RenderFromURL(request.file, pdfConfig);
    } else {
        renderResult = pdf_handler_.RenderFromBase64(request.file, pdfConfig);
    }
    
    if (!renderResult.success && renderResult.pages.empty()) {
        error_msg = renderResult.errorMsg;
        LOG_ERROR("[SUBMIT] PDF rendering failed: {}", error_msg);
        return -1;
    }
    
    LOG_INFO("[SUBMIT] PDF rendered: {} pages", renderResult.renderedPages);
    
    // 4. 生成主任务 ID
    int64_t main_task_id = GenerateTaskId();
    
    // 5. 构建任务配置
    ocr::OCRTaskConfig taskConfig;
    taskConfig.useDocOrientationClassify = request.useDocOrientationClassify;
    taskConfig.useDocUnwarping = request.useDocUnwarping;
    taskConfig.useTextlineOrientation = request.useTextlineOrientation;
    taskConfig.textDetThresh = static_cast<float>(request.textDetThresh);
    taskConfig.textDetBoxThresh = static_cast<float>(request.textDetBoxThresh);
    taskConfig.textDetUnclipRatio = static_cast<float>(request.textDetUnclipRatio);
    taskConfig.textRecScoreThresh = static_cast<float>(request.textRecScoreThresh);
    
    // 6. 初始化主任务状态
    std::vector<int64_t> pageTaskIds;
    
    {
        std::lock_guard<std::mutex> lock(task_meta_mutex_);
        TaskMeta meta;
        meta.status = TaskStatus::PROCESSING;
        meta.taskType = "pdf";
        meta.createTime = std::chrono::steady_clock::now();
        meta.request = request;
        meta.totalPages = renderResult.totalPages;
        meta.renderedPages = renderResult.renderedPages;
        task_meta_[main_task_id] = meta;
    }
    
    // 7. 提交所有页面到 Pipeline
    for (const auto& page : renderResult.pages) {
        if (!page.success) {
            LOG_WARN("[SUBMIT] Skipping failed page {}", page.pageIndex);
            pageTaskIds.push_back(-1);  // 标记失败页
            continue;
        }
        
        int64_t pageTaskId = GenerateTaskId();
        
        if (base_pipeline_->pushTask(page.image, pageTaskId, taskConfig)) {
            pageTaskIds.push_back(pageTaskId);
            LOG_DEBUG("[SUBMIT] Page {} submitted as task_id={}", page.pageIndex, pageTaskId);
        } else {
            LOG_ERROR("[SUBMIT] Failed to submit page {} (queue full)", page.pageIndex);
            pageTaskIds.push_back(-1);
        }
    }
    
    // 8. 更新主任务的页面任务 ID 列表
    {
        std::lock_guard<std::mutex> lock(task_meta_mutex_);
        if (task_meta_.count(main_task_id)) {
            task_meta_[main_task_id].pageTaskIds = pageTaskIds;
        }
    }
    
    LOG_INFO("[SUBMIT] PDF task {} submitted with {} page tasks", main_task_id, pageTaskIds.size());
    return main_task_id;
}

bool OCRHandler::TryGetImageResult(int64_t task_id, json& response_json) {
    // 1. 检查任务状态
    TaskMeta meta;
    {
        std::lock_guard<std::mutex> lock(task_meta_mutex_);
        auto it = task_meta_.find(task_id);
        if (it == task_meta_.end()) {
            response_json = JsonResponseBuilder::BuildErrorResponse(
                ErrorCode::INVALID_PARAMETER, "Task not found");
            return true;  // 返回 true 表示有响应（错误响应）
        }
        meta = it->second;
    }
    
    // 2. 尝试获取结果
    TaskResult result;
    bool found = false;
    {
        std::lock_guard<std::mutex> lock(result_mutex_);
        auto it = result_store_.find(task_id);
        if (it != result_store_.end()) {
            result = std::move(it->second);
            result_store_.erase(it);
            found = true;
        }
    }
    
    if (!found) {
        // 结果尚未就绪
        return false;
    }
    
    // 3. 更新任务状态
    {
        std::lock_guard<std::mutex> lock(task_meta_mutex_);
        if (task_meta_.count(task_id)) {
            task_meta_[task_id].status = result.success ? TaskStatus::COMPLETED : TaskStatus::FAILED;
        }
    }
    
    // 4. 检查处理是否成功
    if (!result.success) {
        LOG_ERROR("[RESULT] Task {} failed (engine error)", task_id);
        response_json = JsonResponseBuilder::BuildErrorResponse(
            ErrorCode::INTERNAL_ERROR, "OCR processing failed (detection engine error)");
        return true;
    }
    
    LOG_INFO("[RESULT] Task {} completed: {} text boxes", task_id, result.results.size());
    
    // 5. 保存可视化（如果启用）
    std::string vis_url;
    if (meta.request.visualize && !result.processedImage.empty()) {
        vis_url = SaveVisualization(result.processedImage, result.results);
        if (!vis_url.empty()) {
            LOG_INFO("[RESULT] Visualization saved: {}", vis_url);
        }
    }
    
    // 6. 计算处理时间
    auto now = std::chrono::steady_clock::now();
    int64_t processing_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - meta.createTime).count();
    LOG_INFO("[RESULT] Image task {} processing time: {} ms", task_id, processing_time_ms);
    
    // 7. 构建成功响应
    response_json = JsonResponseBuilder::BuildSuccessResponse(result.results, vis_url, processing_time_ms);
    return true;
}

bool OCRHandler::TryGetPDFResult(int64_t task_id, json& response_json) {
    // 1. 检查任务状态
    TaskMeta meta;
    {
        std::lock_guard<std::mutex> lock(task_meta_mutex_);
        auto it = task_meta_.find(task_id);
        if (it == task_meta_.end()) {
            response_json = JsonResponseBuilder::BuildErrorResponse(
                ErrorCode::INVALID_PARAMETER, "Task not found");
            return true;
        }
        meta = it->second;
    }
    
    // 2. 检查所有页面是否都已完成
    std::map<int, TaskResult> pageResults;
    bool allCompleted = true;
    
    for (size_t i = 0; i < meta.pageTaskIds.size(); ++i) {
        int64_t pageTaskId = meta.pageTaskIds[i];
        
        if (pageTaskId < 0) {
            // 渲染失败的页面，创建空结果
            TaskResult emptyResult;
            emptyResult.success = false;
            pageResults[static_cast<int>(i)] = emptyResult;
            continue;
        }
        
        std::lock_guard<std::mutex> lock(result_mutex_);
        auto it = result_store_.find(pageTaskId);
        if (it != result_store_.end()) {
            pageResults[static_cast<int>(i)] = it->second;
        } else {
            allCompleted = false;
        }
    }
    
    if (!allCompleted) {
        // 还有页面未完成
        return false;
    }
    
    // 3. 所有页面完成，构建响应
    LOG_INFO("[RESULT] PDF task {} all pages completed", task_id);
    
    // 清理页面结果
    {
        std::lock_guard<std::mutex> lock(result_mutex_);
        for (int64_t pageTaskId : meta.pageTaskIds) {
            if (pageTaskId > 0) {
                result_store_.erase(pageTaskId);
            }
        }
    }
    
    // 更新任务状态
    {
        std::lock_guard<std::mutex> lock(task_meta_mutex_);
        if (task_meta_.count(task_id)) {
            task_meta_[task_id].status = TaskStatus::COMPLETED;
        }
    }
    
    // 构建页面结果 JSON
    json pagesArray = json::array();
    std::map<int, std::string> pageVisUrls;
    
    for (int i = 0; i < meta.renderedPages; ++i) {
        json pageJson;
        pageJson["pageIndex"] = i;
        
        if (pageResults.count(i) && pageResults[i].success) {
            const auto& result = pageResults[i];
            
            json ocrResultsJson = json::array();
            for (const auto& r : result.results) {
                ocrResultsJson.push_back(JsonResponseBuilder::ConvertOCRResultToJson(r));
            }
            pageJson["ocrResults"] = ocrResultsJson;
            
            // 可视化
            if (meta.request.visualize && !result.processedImage.empty()) {
                std::string visUrl = SaveVisualization(result.processedImage, result.results, i);
                if (!visUrl.empty()) {
                    pageJson["ocrImage"] = visUrl;
                }
            }
        } else {
            pageJson["ocrResults"] = json::array();
            if (pageResults.count(i)) {
                pageJson["error"] = "Page processing failed";
            }
        }
        
        pagesArray.push_back(pageJson);
    }
    
    // 计算处理时间
    auto now = std::chrono::steady_clock::now();
    int64_t processing_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - meta.createTime).count();
    LOG_INFO("[RESULT] PDF task {} processing time: {} ms", task_id, processing_time_ms);
    
    response_json = JsonResponseBuilder::BuildPDFSuccessResponse(
        pagesArray, meta.totalPages, meta.renderedPages, processing_time_ms);
    
    return true;
}

OCRHandler::TaskStatus OCRHandler::GetTaskStatus(int64_t task_id) {
    std::lock_guard<std::mutex> lock(task_meta_mutex_);
    auto it = task_meta_.find(task_id);
    if (it == task_meta_.end()) {
        return TaskStatus::NOT_FOUND;
    }
    return it->second.status;
}

std::string OCRHandler::GetTaskType(int64_t task_id) {
    std::lock_guard<std::mutex> lock(task_meta_mutex_);
    auto it = task_meta_.find(task_id);
    if (it == task_meta_.end()) {
        return "";
    }
    return it->second.taskType;
}

} // namespace ocr_server
