#!/bin/bash
set -euo pipefail

PREFIX="SLSPDSDK-HP-VZ"
SRC_DIR="../"
BUILD_DIR="${SRC_DIR}/release_build"
CMAKE_ARGS=("-DCMAKE_BUILD_TYPE=Release")
LIB_PATH="$BUILD_DIR/src/libai_sl.so"
MODELS_PATH="${SRC_DIR}/models/yvu420sp_0910"
PARENT_DIR="../"
OUTPUT_DIR="./package"
DATE_FORMAT="%Y%m%d%H%M%S"
OPENCV_LIB="${SRC_DIR}third_party/glibc/opencv/lib/lib412"

PACKAGE_DIR="$OUTPUT_DIR/package_tmp"

parse_args() {
    while [[ $# -gt 0 ]]; do
        case "$1" in
            --prefix)
                PREFIX="$2"
                shift 2
                ;;
            --src-dir)
                SRC_DIR="$2"
                shift 2
                ;;
            --build-dir)
                BUILD_DIR="$2"
                shift 2
                ;;
            --cmake-args)
                shift
                while [[ $# -gt 0 && ! "$1" =~ ^-- ]]; do
                    CMAKE_ARGS+=("$1")
                    shift
                done
                ;;
            --lib-path)
                LIB_PATH="$2"
                shift 2
                ;;
            --parent-dir)
                PARENT_DIR="$2"
                shift 2
                ;;
            --output-dir)
                OUTPUT_DIR="$2"
                shift 2
                ;;
            --date-format)
                DATE_FORMAT="$2"
                shift 2
                ;;
            -h|--help)
                show_help
                exit 0
                ;;
            *)
                echo "错误：未知参数 $1"
                show_help
                exit 1
                ;;
        esac
    done
}

# 显示帮助信息（保持不变）
show_help() {
    cat << EOF
用法: $0 [选项]

功能: 编译动态库并打包为自解压.run文件，包含config、models目录及动态库

选项:
  --prefix <名称>      打包文件前缀(默认: app)
  --src-dir <路径>     源码目录(默认: ./)
  --build-dir <路径>   编译输出目录(默认: <src-dir>/build)
  --cmake-args <参数>  传递给cmake的参数(可多个，如: -DCMAKE_BUILD_TYPE=Release)
  --lib-path <路径>    动态库文件路径(必填)
  --parent-dir <路径>  包含config和models的上层目录(默认: ../)
  --output-dir <路径>  打包文件输出目录(默认: ./output)
  --date-format <格式> 日期格式(默认: %Y%m%d，参考date命令格式)
  -h, --help           显示帮助信息
EOF
}

check_dependencies() {
    local required_commands=("cmake" "make" "tar" "date" "cp" "mkdir" "mktemp")
    for cmd in "${required_commands[@]}"; do
        if ! command -v "${cmd}" &> /dev/null; then
            echo "错误：缺少必要命令 ${cmd}，请先安装"
            exit 1
        fi
    done
}

build_library() {
    echo "=== 开始编译动态库 ==="
    rm -rf "${BUILD_DIR}"
    mkdir -p "${BUILD_DIR}"
    cd "${BUILD_DIR}" || { echo "错误：无法进入构建目录 ${BUILD_DIR}"; exit 1; }

    echo "运行cmake: cmake ${SRC_DIR} ${CMAKE_ARGS[*]}"
    cmake "${SRC_DIR}" "${CMAKE_ARGS[@]}"

    echo "开始编译: make -j$(nproc)"
    make -j$(nproc)

    cd - > /dev/null
    echo "=== 动态库编译完成 ==="
}

prepare_package() {
    echo "=== 准备打包文件 ==="
    local tmp_dir=$PACKAGE_DIR
    rm -rf "${tmp_dir}"
    mkdir -p "${tmp_dir}"

    echo "使用临时打包目录: ${tmp_dir}"

    # 拷贝config目录
    local config_src="${PARENT_DIR}/config"
    if [[ -d "${config_src}" ]]; then
        cp -r "${config_src}" "${tmp_dir}/"
        echo "已拷贝: ${config_src}"
    else
        echo "错误：${config_src} 目录不存在，缺少必要配置文件，退出打包"
        rm -rf "${tmp_dir}"
        exit 1
    fi

    # 拷贝models目录
    if [[ -d "${MODELS_PATH}" ]]; then
        cp -r "${MODELS_PATH}" "${tmp_dir}/"
        mv "${tmp_dir}/${MODELS_PATH##*/}" "${tmp_dir}/models"
        echo "已拷贝: ${MODELS_PATH}"
    else
        echo "错误：${MODELS_PATH} 目录不存在，退出打包"
        rm -rf "${tmp_dir}"
        exit 1
    fi

    # 拷贝动态库
    if [[ -f "${LIB_PATH}" ]]; then
        cp "${LIB_PATH}" "${tmp_dir}/"
        echo "已拷贝动态库: ${LIB_PATH}"
    else
        echo "错误：动态库文件 ${LIB_PATH} 不存在，退出打包"
        rm -rf "${tmp_dir}"
        exit 1
    fi

    # 拷贝opencv库
    if [[ -d "${OPENCV_LIB}" ]]; then
        cp -rf ${OPENCV_LIB}/* "${tmp_dir}/"
        echo "已拷贝: ${OPENCV_LIB}"
    else
        echo "错误：${OPENCV_LIB} 目录不存在，退出打包"
        rm -rf "${tmp_dir}"
        exit 1
    fi

    # 拷贝version
    cp "${SRC_DIR}/version_control/version.txt" "${tmp_dir}/"
    echo "已拷贝: version.txt"

    echo "=== 打包文件准备完成 ==="
    echo "${tmp_dir}"
}

generate_runfile() {
    local tmp_dir=$PACKAGE_DIR
    local package_date=$(date +"${DATE_FORMAT}")
    local package_name="${PREFIX}-${package_date}-DEV.run"
    local package_path="${OUTPUT_DIR}/${package_name}"
    local target_dir="/media/feature/lib"

    echo "=== 生成打包文件 ==="
    mkdir -p "${OUTPUT_DIR}"

    echo "extract version info from version.txt"
    version=$(grep '^version' "${tmp_dir}/version.txt" | cut -d':' -f2)
    echo "version: ${version}"
    build_time=$(grep 'build-time' "${tmp_dir}/version.txt" | sed -n 's/.*build-time:\([^,，]*\).*/\1/p')
    echo "build-time: ${build_time}"
    ownership=$(grep 'ownership' "${tmp_dir}/version.txt" | sed -n 's/.*ownership:\([^,]*\).*/\1/p')
    echo "ownership: ${ownership}"
#    cat > "${package_path}" << EOF
##!/bin/sh
#set -e
#
#TARGET_DIR="${target_dir}"
#echo "package info"
#echo "version: ${version}"
#echo "build-time: ${build_time}"
#echo "ownership: ${ownership}"
#echo "check install dir : \${TARGET_DIR}"
#mkdir -p "\${TARGET_DIR}"
#
#echo "clean target install directory..."
#if [ -d "\${TARGET_DIR}" ]; then
#    find "\${TARGET_DIR}" -mindepth 1 -maxdepth 1 ! -name "log" -exec rm -rf {} +
#    echo "clean directory done."
#else
#    echo "if \${TARGET_DIR} not exist, create it."
#fi
#
#ARCHIVE_START=\$(awk '/^__ARCHIVE_BEGIN__/ {print NR + 1; exit 0}' "\$0")
#
#echo "start extract package to \${TARGET_DIR}..."
#tail -n+\${ARCHIVE_START} "\$0" | tar xz -C "\${TARGET_DIR}"
#
#echo "install package done."
#exit 0
#__ARCHIVE_BEGIN__
#EOF

    # 追加打包内容
#    tar -czf - -C "${tmp_dir}" . >> "${package_path}"
#    chmod +x "${package_path}"

    # 清理临时目录
#    rm -rf "${tmp_dir}"

    echo "=== 目录构建完成 ==="
    echo "生成目录: ${PACKAGE_DIR}"
}

# 主流程（保持不变）
main() {
    parse_args "$@"
    check_dependencies
    build_library
    rm -rf "$PACKAGE_DIR"
    prepare_package
    generate_runfile
}

# 启动主流程
main "$@"