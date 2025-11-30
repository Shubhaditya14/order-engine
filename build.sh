#!/bin/bash

# Order Matching Engine - Build Script
# Usage: ./build.sh [clean|run|test]

set -e  # Exit on error

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Clean build
clean() {
    log_info "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
    log_info "Clean complete"
}

# Build C++ backend
build_backend() {
    log_info "Building C++ backend..."
    
    # Create build directory
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    # Configure with CMake
    log_info "Running CMake..."
    cmake .. -DCMAKE_BUILD_TYPE=Release
    
    # Build
    log_info "Compiling..."
    make -j$(sysctl -n hw.ncpu)
    
    log_info "Backend build complete: $BUILD_DIR/ome"
}

# Build React frontend
build_frontend() {
    log_info "Building React frontend..."
    
    cd "$PROJECT_ROOT/gui"
    
    # Install dependencies if needed
    if [ ! -d "node_modules" ]; then
        log_info "Installing npm dependencies..."
        npm install
    fi
    
    log_info "Frontend ready"
}

# Run the backend
run_backend() {
    if [ ! -f "$BUILD_DIR/ome" ]; then
        log_error "Backend not built. Run './build.sh' first."
        exit 1
    fi
    
    log_info "Starting Order Matching Engine..."
    log_info "WebSocket server: ws://localhost:8080"
    cd "$BUILD_DIR"
    ./ome
}

# Run the frontend
run_frontend() {
    log_info "Starting React GUI..."
    log_info "GUI: http://localhost:5173"
    cd "$PROJECT_ROOT/gui"
    npm run dev
}

# Run both backend and frontend
run_all() {
    log_info "Starting full system..."
    log_info "Backend: ws://localhost:8080"
    log_info "Frontend: http://localhost:5173"
    log_warn "Press Ctrl+C to stop"
    
    # Start backend in background
    cd "$BUILD_DIR"
    ./ome &
    BACKEND_PID=$!
    
    # Wait a bit for backend to start
    sleep 2
    
    # Start frontend
    cd "$PROJECT_ROOT/gui"
    npm run dev
    
    # Cleanup on exit
    trap "kill $BACKEND_PID 2>/dev/null" EXIT
}

# Main
case "${1:-build}" in
    clean)
        clean
        ;;
    build)
        build_backend
        build_frontend
        log_info "✓ Build complete!"
        log_info "Run './build.sh run' to start the system"
        ;;
    backend)
        run_backend
        ;;
    frontend)
        run_frontend
        ;;
    run)
        run_all
        ;;
    test)
        log_warn "Tests not implemented yet"
        ;;
    *)
        echo "Usage: $0 {build|clean|run|backend|frontend|test}"
        echo ""
        echo "Commands:"
        echo "  build     - Build C++ backend and prepare frontend"
        echo "  clean     - Remove build artifacts"
        echo "  run       - Run both backend and frontend"
        echo "  backend   - Run only backend"
        echo "  frontend  - Run only frontend"
        echo "  test      - Run tests (not implemented)"
        exit 1
        ;;
esac
