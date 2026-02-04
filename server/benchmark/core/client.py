"""
异步 HTTP 客户端模块
提供高性能的异步请求能力
"""

import json
import time
import asyncio
import aiohttp
from typing import Optional, Dict, Any, Tuple
from dataclasses import dataclass
from .metrics import RequestStatus, ErrorCategory, RequestMetrics


@dataclass
class RequestResult:
    """请求结果"""
    success: bool
    status_code: int
    latency_ms: float
    error_msg: str = ""
    error_category: ErrorCategory = ErrorCategory.NONE
    
    # OCR 结果
    text: str = ""
    char_count: int = 0
    page_count: int = 0
    ocr_results: list = None
    
    # 原始响应
    response_json: Optional[Dict] = None


class AsyncHTTPClient:
    """异步 HTTP 客户端"""
    
    def __init__(self, base_url: str, token: str, timeout: int = 60,
                 max_connections: int = 100, verify_ssl: bool = False):
        """
        初始化客户端
        
        Args:
            base_url: 服务器 URL
            token: 认证 token
            timeout: 请求超时时间（秒）
            max_connections: 最大连接数
            verify_ssl: 是否验证 SSL 证书
        """
        self.base_url = base_url
        self.token = token
        self.timeout = timeout
        self.verify_ssl = verify_ssl
        
        # 连接器配置
        self.connector = aiohttp.TCPConnector(
            limit=max_connections,
            limit_per_host=max_connections,
            keepalive_timeout=30,
            force_close=False,
            ssl=verify_ssl
        )
        
        self.session: Optional[aiohttp.ClientSession] = None
    
    async def __aenter__(self):
        """异步上下文管理器入口"""
        self.session = aiohttp.ClientSession(
            connector=self.connector,
            timeout=aiohttp.ClientTimeout(total=self.timeout)
        )
        return self
    
    async def __aexit__(self, exc_type, exc_val, exc_tb):
        """异步上下文管理器退出"""
        if self.session:
            await self.session.close()
    
    async def _parse_ocr_response(self, response: aiohttp.ClientResponse,
                                   latency_ms: float) -> RequestResult:
        """解析 OCR 响应体并组装 RequestResult"""
        try:
            response_json = await response.json()
        except Exception as e:
            return RequestResult(
                success=False,
                status_code=response.status,
                latency_ms=latency_ms,
                error_msg=f"JSON decode error: {e}",
                error_category=ErrorCategory.DECODE
            )

        if response.status != 200:
            error_cat = ErrorCategory.HTTP_4XX if 400 <= response.status < 500 else ErrorCategory.HTTP_5XX
            return RequestResult(
                success=False,
                status_code=response.status,
                latency_ms=latency_ms,
                error_msg=f"HTTP {response.status}",
                error_category=error_cat,
                response_json=response_json
            )
        error_code = response_json.get("errorCode", -1)

        if error_code != 0:
            return RequestResult(
                success=False,
                status_code=response.status,
                latency_ms=latency_ms,
                error_msg=response_json.get("errorMsg", "Unknown error"),
                error_category=ErrorCategory.VALIDATION,
                response_json=response_json
            )
        result = response_json.get("result", {})
        
        if "pages" in result:
            pages = result.get("pages", [])
            total_chars = 0
            all_texts = []
            for page in pages:
                ocr_results = page.get("ocrResults", [])
                for ocr in ocr_results:
                    text = ocr.get("prunedResult", "")
                    all_texts.append(text)
                    total_chars += len(text)
            return RequestResult(
                success=True,
                status_code=response.status,
                latency_ms=latency_ms,
                text="".join(all_texts),
                char_count=total_chars,
                page_count=result.get("renderedPages", 0),
                ocr_results=pages,
                response_json=response_json
            )
        ocr_results = result.get("ocrResults", [])
        texts = [r.get("prunedResult", "") for r in ocr_results]
        text = "".join(texts)
        return RequestResult(
            success=True,
            status_code=response.status,
            latency_ms=latency_ms,
            text=text,
            char_count=len(text),
            page_count=0,
            ocr_results=ocr_results,
            response_json=response_json
        )
    
    async def send_ocr_request(self, body: bytes, request_id: int = 0,
                                poll_timeout: float = 60.0,
                                poll_interval: float = 0.005) -> RequestResult:
        """
        发送 OCR 请求
        
        Args:
            body: UTF-8 编码的 JSON 字符串（已序列化的 payload）
            request_id: 请求 ID（用于日志）
            poll_timeout: 轮询超时时间（秒）
            poll_interval: 轮询间隔（秒）
        
        Returns:
            RequestResult 对象
        """
        start_time = time.time()
        
        # 1. 提交任务
        success, task_id, error_msg = await self.submit_ocr_task(body)
        
        if not success:
            return RequestResult(
                success=False,
                status_code=0,
                latency_ms=(time.time() - start_time) * 1000,
                error_msg=f"Submit failed: {error_msg}",
                error_category=ErrorCategory.VALIDATION
            )
        
        # 2. 轮询结果
        submit_time = time.time() - start_time
        remaining_timeout = max(poll_timeout - submit_time, 1.0)
        
        return await self.poll_ocr_result(task_id, timeout=remaining_timeout, poll_interval=poll_interval)
    
    async def warmup(self, file_base64: str, ocr_params: Dict[str, Any],
                     warmup_count: int = 5, concurrency: int = 1,
                     poll_interval: float = 0.005) -> tuple[int, int]:
        """
        预热阶段
        
        Args:
            file_base64: 预热用的文件
            ocr_params: OCR 参数
            warmup_count: 预热请求数量
            concurrency: 并发数
            poll_interval: 轮询间隔（秒）
        
        Returns:
            (成功数, 总数)
        """
        body = json.dumps({"file": file_base64, **ocr_params}, ensure_ascii=False).encode("utf-8")
        print(f"\n[Warmup] Starting async warmup with {warmup_count} requests (concurrency={concurrency})...")
        
        semaphore = asyncio.Semaphore(concurrency)
        
        async def _one_warmup(i: int):
            async with semaphore:
                return await self.send_ocr_request(body, request_id=-i-1, poll_interval=poll_interval)
        
        results = await asyncio.gather(*[_one_warmup(i) for i in range(warmup_count)])
        success_count = sum(1 for r in results if r.success)
        
        for i, r in enumerate(results):
            status = "✓" if r.success else "✗"
            msg = f"{r.latency_ms:.2f}ms" if r.success else r.error_msg
            print(f"  Warmup {i+1}/{warmup_count}: {msg} {status}")
        
        success_rate = success_count / warmup_count * 100
        print(f"[Warmup] Completed: {success_count}/{warmup_count} ({success_rate:.0f}% success)\n")

        return success_count, warmup_count
    
    async def health_check(self, health_url: Optional[str] = None) -> bool:
        """
        健康检查
        
        Args:
            health_url: 健康检查 URL（默认使用 base_url 的 /health）
        
        Returns:
            是否健康
        """
        if health_url is None:
            # 从 base_url 推导 health_url
            from urllib.parse import urlparse, urlunparse
            parsed = urlparse(self.base_url)
            health_url = urlunparse((parsed.scheme, parsed.netloc, '/health', '', '', ''))
        
        try:
            async with self.session.get(health_url, timeout=aiohttp.ClientTimeout(total=5)) as response:
                if response.status == 200:
                    data = await response.json()
                    print(f"[Health Check] Server is healthy: {data}")
                    return True
                else:
                    print(f"[Health Check] Server returned {response.status}")
                    return False
        except Exception as e:
            print(f"[Health Check] Failed: {e}")
            return False

    # ==================== 异步模式接口（Submit + Poll） ====================
    
    async def submit_ocr_task(self, body: bytes) -> Tuple[bool, int, str]:
        """
        异步提交 OCR 任务（非阻塞）
        
        Args:
            body: UTF-8 编码的 JSON 请求体
        
        Returns:
            (success, task_id, error_msg)
        """
        headers = {
            "Content-Type": "application/json",
            "Authorization": f"token {self.token}"
        }
        
        # 构造 submit URL
        from urllib.parse import urlparse, urlunparse
        parsed = urlparse(self.base_url)
        submit_url = urlunparse((parsed.scheme, parsed.netloc, '/ocr/submit', '', '', ''))
        
        try:
            async with self.session.post(submit_url, headers=headers, data=body) as response:
                result = await response.json()
                
                if response.status == 202:
                    task_id = result.get("taskId", -1)
                    return True, task_id, ""
                else:
                    error_msg = result.get("errorMsg", f"HTTP {response.status}")
                    return False, -1, error_msg
                    
        except asyncio.TimeoutError:
            return False, -1, "Submit request timeout"
        except asyncio.CancelledError:
            return False, -1, "Submit request cancelled"
        except aiohttp.ClientError as e:
            return False, -1, f"Connection error: {e}"
        except Exception as e:
            return False, -1, f"Unexpected error: {e}"
    
    async def poll_ocr_result(self, task_id: int, timeout: float = 60.0,
                               poll_interval: float = 0.005) -> RequestResult:
        """
        轮询获取 OCR 结果
        
        Args:
            task_id: 任务 ID
            timeout: 总超时时间（秒）
            poll_interval: 轮询间隔（秒），默认 5ms
        
        Returns:
            RequestResult 对象
        """
        headers = {
            "Content-Type": "application/json",
            "Authorization": f"token {self.token}"
        }
        
        # 构造 result URL
        from urllib.parse import urlparse, urlunparse
        parsed = urlparse(self.base_url)
        result_url = urlunparse((parsed.scheme, parsed.netloc, f'/ocr/result/{task_id}', '', '', ''))
        
        start_time = time.time()
        poll_count = 0
        
        while (time.time() - start_time) < timeout:
            poll_count += 1
            try:
                async with self.session.get(result_url, headers=headers) as response:
                    latency_ms = (time.time() - start_time) * 1000
                    
                    if response.status == 200:
                        # 结果就绪
                        return await self._parse_ocr_response(response, latency_ms)
                    
                    elif response.status == 202:
                        # 仍在处理，等待后继续轮询
                        await asyncio.sleep(poll_interval)
                    
                    elif response.status == 404:
                        # 任务不存在
                        return RequestResult(
                            success=False,
                            status_code=404,
                            latency_ms=latency_ms,
                            error_msg="Task not found",
                            error_category=ErrorCategory.VALIDATION
                        )
                    
                    else:
                        # 其他错误
                        error_cat = ErrorCategory.HTTP_4XX if 400 <= response.status < 500 else ErrorCategory.HTTP_5XX
                        return RequestResult(
                            success=False,
                            status_code=response.status,
                            latency_ms=latency_ms,
                            error_msg=f"HTTP {response.status}",
                            error_category=error_cat
                        )
                        
            except asyncio.CancelledError:
                return RequestResult(
                    success=False,
                    status_code=0,
                    latency_ms=(time.time() - start_time) * 1000,
                    error_msg="Poll request cancelled",
                    error_category=ErrorCategory.TIMEOUT
                )
            except aiohttp.ClientError as e:
                return RequestResult(
                    success=False,
                    status_code=0,
                    latency_ms=(time.time() - start_time) * 1000,
                    error_msg=f"Connection error: {e}",
                    error_category=ErrorCategory.CONNECTION
                )
            except Exception as e:
                return RequestResult(
                    success=False,
                    status_code=0,
                    latency_ms=(time.time() - start_time) * 1000,
                    error_msg=f"Unexpected error: {e}",
                    error_category=ErrorCategory.UNKNOWN
                )
        
        # 超时
        return RequestResult(
            success=False,
            status_code=0,
            latency_ms=timeout * 1000,
            error_msg=f"Polling timeout after {poll_count} attempts",
            error_category=ErrorCategory.TIMEOUT
        )
