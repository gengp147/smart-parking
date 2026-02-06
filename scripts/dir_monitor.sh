#!/bin/sh

# Display script usage
usage() {
    echo "Usage: $0 <target_directory> [options]"
    echo "Options:"
    echo "  -t <threshold_percent>   Warn when available space % is below this value (default: 10)"
    echo "Examples:"
    echo "  $0 /home               # Monitor filesystem for /home"
    echo "  $0 /data -t 5          # Monitor /data, warn below 5% available"
    exit 1
}

# Convert size string (e.g., 1.9G, 348.8M) to MB (floating-point)
convert_to_mb() {
    local size_str="$1"
    local num=$(echo "$size_str" | sed -E 's/^([0-9.]+)[A-Za-z]$/\1/')  # Extract number
    local unit=$(echo "$size_str" | sed -E 's/^[0-9.]+([A-Za-z])$/\1/')  # Extract unit

    case "$unit" in
        K|k) echo "$num / 1024" | bc -l ;;  # KB to MB
        M|m) echo "$num" | bc -l ;;         # MB remains
        G|g) echo "$num * 1024" | bc -l ;;  # GB to MB
        T|t) echo "$num * 1024 * 1024" | bc -l ;;  # TB to MB
        *) echo "0" ;;  # Unknown unit
    esac
}

# Check if target directory is provided
if [ $# -lt 1 ]; then
    echo "Error: Please specify target directory!"
    usage
fi

# Parse parameters
TARGET_DIR="$1"
THRESHOLD=10  # Default warning threshold

# Process options (-t)
shift  # Skip target directory
while getopts "t:" opt; do
    case $opt in
        t)
            # Validate threshold is integer 0-100
            case "$OPTARG" in
                ''|*[!0-9]*)
                    echo "Error: Threshold must be an integer between 0-100"
                    usage
                    ;;
                *)
                    if [ "$OPTARG" -lt 0 ] || [ "$OPTARG" -gt 100 ]; then
                        echo "Error: Threshold must be between 0-100"
                        usage
                    fi
                    THRESHOLD="$OPTARG"
                    ;;
            esac
            ;;
        \?)
            echo "Error: Invalid option -$OPTARG"
            usage
            ;;
        :)
            echo "Error: Option -$OPTARG requires a threshold value"
            usage
            ;;
    esac
done

# Check if target directory exists
if [ ! -d "$TARGET_DIR" ]; then
    echo "Error: Directory '$TARGET_DIR' does not exist or is invalid"
    exit 1
fi

# Get absolute path (POSIX-compliant)
ABS_DIR=$(cd "$TARGET_DIR" 2>/dev/null && pwd)
if [ -z "$ABS_DIR" ]; then
    echo "Error: Failed to resolve absolute path for '$TARGET_DIR'"
    exit 1
fi

# Get filesystem info using df -h (filter header line)
DISK_INFO=$(df -h "$ABS_DIR" | tail -n 1)
if [ -z "$DISK_INFO" ]; then
    echo "Error: Failed to retrieve filesystem information"
    exit 1
fi

# Extract filesystem details
FILESYSTEM=$(echo "$DISK_INFO" | awk '{print $1}')
SIZE=$(echo "$DISK_INFO" | awk '{print $2}')
USED=$(echo "$DISK_INFO" | awk '{print $3}')
AVAIL=$(echo "$DISK_INFO" | awk '{print $4}')
USE_PERCENT=$(echo "$DISK_INFO" | awk '{print $5}')
MOUNT_POINT=$(echo "$DISK_INFO" | awk '{print $6}')

# Convert total and available size to MB (for consistent calculation)
SIZE_MB=$(convert_to_mb "$SIZE")
AVAIL_MB=$(convert_to_mb "$AVAIL")

# Calculate available percentage (using bc for floating-point)
if [ $(echo "$SIZE_MB > 0" | bc -l) -eq 1 ]; then
    AVAIL_PERCENT=$(echo "scale=2; ($AVAIL_MB / $SIZE_MB) * 100" | bc -l)
    # Round to integer
    AVAIL_PERCENT=$(printf "%.0f" "$AVAIL_PERCENT")
else
    AVAIL_PERCENT=0
fi

# Print monitoring results
echo "============================================="
echo "Monitored directory: $ABS_DIR"
echo "Filesystem: $FILESYSTEM (Mount point: $MOUNT_POINT)"
echo "---------------------------------------------"
echo "Total capacity: $SIZE"
echo "Used capacity: $USED (Usage rate: $USE_PERCENT)"
echo "Available capacity: $AVAIL (Available percentage: $AVAIL_PERCENT%)"
echo "============================================="

# Threshold check with proper formatting
if [ "$(echo "$AVAIL_PERCENT < $THRESHOLD" | bc -l)" -eq 1 ]; then
    printf "\033[31mWarning: Available space ($AVAIL_PERCENT%) below threshold ($THRESHOLD%)! Clean up space.\033[0m\n"
    exit 2
else
    printf "\033[32mStatus normal: Available space ($AVAIL_PERCENT%) above threshold ($THRESHOLD%).\033[0m\n"
    exit 0
fi