#!/bin/sh

# System memory monitoring utility
# Uses /proc/meminfo and sysconf for authoritative memory metrics

# Default configuration
THRESHOLD=20          # Available memory alert threshold (%)
INTERVAL=5            # Monitoring interval in seconds
LOG_FILE="./mem_monitor.log"  # Log storage path
ALERT_COOLDOWN=300    # Alert cooldown period in seconds (5 minutes)

# Print usage information
usage() {
    echo "Usage: $0 [OPTIONS]"
    echo "Options:"
    echo "  -t THRESHOLD   Set alert threshold (1-99%, default: $THRESHOLD)"
    echo "  -i INTERVAL    Set check interval (seconds >0, default: $INTERVAL)"
    echo "  -l LOG_FILE    Set log file path (default: $LOG_FILE)"
    echo "  -h             Display this help message"
    exit 1
}

# Validate numeric input
validate_numeric() {
    if ! printf '%s\n' "$1" | grep -q '^[0-9]\+$'; then
        echo "Error: '$1' is not a valid integer" >&2
        exit 1
    fi
}

# Parse command line options
while getopts "t:i:l:h" opt; do
    case "$opt" in
        t) THRESHOLD="$OPTARG" ;;
        i) INTERVAL="$OPTARG" ;;
        l) LOG_FILE="$OPTARG" ;;
        h) usage ;;
        *) usage ;;
    esac
done

# Validate configuration parameters
validate_numeric "$THRESHOLD"
validate_numeric "$INTERVAL"

if [ "$THRESHOLD" -lt 1 ] || [ "$THRESHOLD" -gt 99 ]; then
    echo "Error: Threshold must be between 1 and 99" >&2
    exit 1
fi

if [ "$INTERVAL" -lt 1 ]; then
    echo "Error: Interval must be greater than 0" >&2
    exit 1
fi

# Initialize log file
touch "$LOG_FILE" || { echo "Error: Unable to create log file $LOG_FILE" >&2; exit 1; }

# Logging function
log_entry() {
    local level="$1"
    local message="$2"
    local timestamp=$(date -u "+%Y-%m-%dT%H:%M:%SZ")  # ISO 8601 UTC timestamp
    printf "%s [%s] %s\n" "$timestamp" "$level" "$message" | tee -a "$LOG_FILE" >&2
}

# Get authoritative memory metrics from /proc/meminfo
get_memory_stats() {
    # Read critical memory metrics (in kB)
    local mem_total=$(awk '/^MemTotal:/ {print $2}' /proc/meminfo)
    local mem_free=$(awk '/^MemFree:/ {print $2}' /proc/meminfo)
    local mem_available=$(awk '/^MemAvailable:/ {print $2}' /proc/meminfo)
    local buffers=$(awk '/^Buffers:/ {print $2}' /proc/meminfo)
    local cached=$(awk '/^Cached:/ {print $2}' /proc/meminfo)
    local slab=$(awk '/^Slab:/ {print $2}' /proc/meminfo)

    # Calculate derived metrics
    local used_phys=$((mem_total - mem_free - buffers - cached - slab))
    local available_pct=$((mem_available * 100 / mem_total))

    # Output metrics in machine-readable format
    echo "$mem_total $mem_free $mem_available $buffers $cached $slab $used_phys $available_pct"
}

# Convert kB to human-readable format
human_readable() {
    local size_kb="$1"
    awk -v kb="$size_kb" 'BEGIN {
        if (kb >= 1048576) printf "%.2f GiB", kb / 1048576;
        else if (kb >= 1024) printf "%.2f MiB", kb / 1024;
        else printf "%d KiB", kb;
    }'
}

# Main monitoring loop
log_entry "INFO" "Monitoring started - Threshold: ${THRESHOLD}%, Interval: ${INTERVAL}s"
last_alert=0

while true; do
    # Clear screen for readability
    clear

    # Get current memory statistics
    stats=$(get_memory_stats)
    set -- $stats  # Split into variables
    mem_total=$1
    mem_free=$2
    mem_available=$3
    buffers=$4
    cached=$5
    slab=$6
    used_phys=$7
    available_pct=$8

    # Display memory information
    echo "===================== SYSTEM MEMORY USAGE ====================="
    echo "Total:        $(human_readable $mem_total)"
    echo "Free:         $(human_readable $mem_free)"
    echo "Available:    $(human_readable $mem_available) ($available_pct%)"
    echo "Used (phys):  $(human_readable $used_phys)"
    echo "Buffers:      $(human_readable $buffers)"
    echo "Cached:       $(human_readable $cached)"
    echo "Slab:         $(human_readable $slab)"
    echo "--------------------------------------------------------------"
    echo "Alert Threshold: ${THRESHOLD}% | Check Interval: ${INTERVAL}s"
    echo "Log File: $LOG_FILE"
    echo "=============================================================="

    # Check threshold and trigger alerts
    current_time=$(date +%s)
    if [ "$available_pct" -lt "$THRESHOLD" ]; then
        if [ $((current_time - last_alert)) -ge "$ALERT_COOLDOWN" ] || [ "$last_alert" -eq 0 ]; then
            alert_msg="Available memory below threshold (${available_pct}% < ${THRESHOLD}%)"
            log_entry "WARNING" "$alert_msg"
            echo "ALERT: $alert_msg"
            last_alert=$current_time
        else
            echo "Status: Below threshold (alert suppressed - cooldown active)"
        fi
    else
        echo "Status: Normal (above threshold)"
    fi

    # Wait for next interval
    sleep "$INTERVAL"
done