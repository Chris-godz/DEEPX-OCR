#!/bin/bash
# DeepX OCR Server 一键启动脚本
# Usage: ./start_server.sh [options]

set -e

# ============================================
# 颜色定义
# ============================================
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color
BOLD='\033[1m'

# ============================================
# 项目路径
# ============================================
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# 默认配置文件路径
CONFIG_FILE="${SCRIPT_DIR}/config.yaml"

# ============================================
# 帮助信息
# ============================================
show_help() {
    echo -e "${CYAN}========================================${NC}"
    echo -e "${BOLD}DeepX OCR Server 启动脚本${NC}"
    echo -e "${CYAN}========================================${NC}"
    echo ""
    echo -e "${GREEN}Usage:${NC} $0 [options]"
    echo ""
    echo -e "${YELLOW}Options:${NC}"
    echo "  -c, --config <file>     配置文件路径 (默认: ${SCRIPT_DIR}/config.yaml)"
    echo "  -h, --help              显示帮助信息"
    echo ""
    echo -e "${YELLOW}Examples:${NC}"
    echo "  $0                           # 使用默认配置文件启动"
    echo "  $0 -c /path/to/config.yaml   # 指定配置文件"
    echo ""
    echo -e "${YELLOW}Configuration File (config.yaml):${NC}"
    echo "  配置文件使用 YAML 格式，包含以下配置项："
    echo "  - server.port:           服务端口"
    echo "  - server.threads:        HTTP 线程数"
    echo "  - model.type:            模型类型 (server/mobile)"
    echo "  - directories.vis_dir:   可视化输出目录"
    echo ""
    exit 0
}

# ============================================
# YAML 解析函数
# ============================================
# 从 YAML 文件中读取指定键的值
# 支持简单的嵌套结构，如 server.port
parse_yaml_value() {
    local file="$1"
    local key="$2"
    local default="$3"
    
    if [ ! -f "$file" ]; then
        echo "$default"
        return
    fi
    
    # 将点分隔的键转换为层级
    local key_parts=(${key//./ })
    local current_indent=0
    local found_parent=false
    local result=""
    
    if [ ${#key_parts[@]} -eq 1 ]; then
        # 单级键
        result=$(grep -E "^${key}:" "$file" 2>/dev/null | head -1 | sed 's/^[^:]*:[[:space:]]*//' | sed 's/[[:space:]]*#.*//' | sed 's/^["'\'']\(.*\)["'\'']$/\1/')
    else
        # 多级键 (如 server.port)
        local parent="${key_parts[0]}"
        local child="${key_parts[1]}"
        
        # 使用 awk 来解析嵌套的 YAML
        result=$(awk -v parent="$parent" -v child="$child" '
            BEGIN { in_parent = 0 }
            /^[a-zA-Z_]/ { in_parent = 0 }
            $0 ~ "^" parent ":" { in_parent = 1; next }
            in_parent && $0 ~ "^[[:space:]]+" child ":" {
                gsub(/^[[:space:]]*[^:]+:[[:space:]]*/, "")
                gsub(/[[:space:]]*#.*/, "")
                gsub(/^["'\'']|["'\'']$/, "")
                print
                exit
            }
        ' "$file")
    fi
    
    # 如果结果为空，返回默认值
    if [ -z "$result" ]; then
        echo "$default"
    else
        echo "$result"
    fi
}

# ============================================
# 参数解析
# ============================================
while [[ $# -gt 0 ]]; do
    case $1 in
        -c|--config)
            CONFIG_FILE="$2"
            shift 2
            ;;
        -h|--help)
            show_help
            ;;
        *)
            echo -e "${RED}Error: Unknown option $1${NC}"
            show_help
            ;;
    esac
done

# ============================================
# 检查配置文件
# ============================================
if [ ! -f "$CONFIG_FILE" ]; then
    echo -e "${RED}Error: Configuration file not found: $CONFIG_FILE${NC}"
    echo -e "${YELLOW}Please create config.yaml or specify a config file with -c option${NC}"
    exit 1
fi

echo -e "${BLUE}Loading configuration from: $CONFIG_FILE${NC}"

# ============================================
# 从配置文件读取配置
# ============================================
PORT=$(parse_yaml_value "$CONFIG_FILE" "server.port" "8080")
THREADS=$(parse_yaml_value "$CONFIG_FILE" "server.threads" "32")
MODEL=$(parse_yaml_value "$CONFIG_FILE" "model.type" "server")
VIS_DIR=$(parse_yaml_value "$CONFIG_FILE" "directories.vis_dir" "output/vis")

# 固定目录配置
LOG_DIR="logs"
BUILD_DIR="${PROJECT_ROOT}/build_Release"

# ============================================
# 验证模型类型
# ============================================
if [[ "$MODEL" != "server" && "$MODEL" != "mobile" ]]; then
    echo -e "${RED}Error: Invalid model type '$MODEL'. Use 'server' or 'mobile'.${NC}"
    exit 1
fi

# ============================================
# 打印启动信息
# ============================================
echo -e "${CYAN}========================================${NC}"
echo -e "${BOLD}🚀 DeepX OCR Server${NC}"
echo -e "${CYAN}========================================${NC}"
echo ""
echo -e "${GREEN}Configuration (from $CONFIG_FILE):${NC}"
echo -e "  Port:        ${YELLOW}$PORT${NC}"
echo -e "  Model:       ${YELLOW}$MODEL${NC}"
echo -e "  Threads:     ${YELLOW}$THREADS${NC}"
echo -e "  Vis Dir:     ${YELLOW}$VIS_DIR${NC}"
echo -e "  Project:     ${YELLOW}$PROJECT_ROOT${NC}"
echo ""

# ============================================
# 检查编译目录
# ============================================
if [ ! -d "$BUILD_DIR" ]; then
    echo -e "${RED}Error: Build directory not found: $BUILD_DIR${NC}"
    echo -e "${YELLOW}Please run: cd $PROJECT_ROOT && bash build.sh${NC}"
    exit 1
fi

if [ ! -f "$BUILD_DIR/bin/ocr_server" ]; then
    echo -e "${RED}Error: ocr_server executable not found: $BUILD_DIR/bin/ocr_server${NC}"
    echo -e "${YELLOW}Please run: cd $PROJECT_ROOT && bash build.sh${NC}"
    exit 1
fi

# ============================================
# 设置 DXRT 环境变量 (自动检测和配置)
# ============================================
echo -e "${BLUE}Checking DXRT environment variables...${NC}"

# 自动检测环境变量，未配置则自动设置
if [ -z "$CUSTOM_INTER_OP_THREADS_COUNT" ]; then
    source "$PROJECT_ROOT/set_env.sh" 1 2 1 3 2 4
    echo -e "${GREEN}✓ Environment variables auto-configured${NC}"
else
    echo -e "${GREEN}✓ Environment variables already set${NC}"
fi

# ============================================
# 启动服务
# ============================================
echo ""
echo -e "${CYAN}----------------------------------------${NC}"
echo -e "${GREEN}Starting OCR Server...${NC}"
echo -e "${CYAN}----------------------------------------${NC}"
echo ""

# 切换到项目根目录
cd "$PROJECT_ROOT"

# 构建命令
CMD="$BUILD_DIR/bin/ocr_server --port $PORT --model $MODEL --threads $THREADS --vis-dir $VIS_DIR --log-dir $LOG_DIR"
echo -e "${BLUE}Command: $CMD${NC}"
echo ""

# 执行
exec $CMD
