#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build"
NATIVE_BUILD_DIR="${BUILD_DIR}"
WINDOWS_BUILD_DIR="${BUILD_DIR}-windows"
OUT_DIR="${ROOT_DIR}/releases"
TOOLCHAIN_FILE="${ROOT_DIR}/scripts/release/toolchains/mingw-w64-x86_64.cmake"
BUILD_TYPE="Release"
FORMAT="all"
RUN_TESTS="false"
CREATE_GITHUB_RELEASE="false"
TAG=""
TITLE=""
NOTES_FILE=""
DRAFT="false"
PRERELEASE="false"
ARTIFACTS=()

usage() {
    cat <<'EOF'
Usage:
  bash scripts/release/build_release.sh [options]

Options:
  --format <all|deb|rpm|tgz|windows>   Package format to build (default: all)
  --build-dir <path>           CMake build directory (default: ./build)
  --out-dir <path>             Export directory for artifacts (default: ./releases)
  --build-type <type>          CMake build type (default: Release)
  --run-tests                  Run ctest before packaging

  --github-release             Create/update GitHub release and upload artifacts
  --tag <tag>                  Git tag to release (default: current git tag, else git describe, else v0.1.0)
  --title <title>              Release title (default: tag)
  --notes-file <file>          Path to release notes markdown/text file
  --draft                      Create release as draft
  --prerelease                 Mark release as prerelease

  -h, --help                   Show this help message

Examples:
  bash scripts/release/build_release.sh --format all --run-tests
  bash scripts/release/build_release.sh --format rpm
  bash scripts/release/build_release.sh --format windows
  bash scripts/release/build_release.sh --format deb --github-release --tag v0.1.0
EOF
}

err() {
    echo "error: $*" >&2
    exit 1
}

require_cmd() {
    local cmd="$1"
    command -v "$cmd" >/dev/null 2>&1 || err "required command not found: $cmd"
}

parse_args() {
    while [[ $# -gt 0 ]]; do
        case "$1" in
            --format)
                [[ $# -ge 2 ]] || err "--format expects a value"
                FORMAT="$2"
                shift 2
                ;;
            --build-dir)
                [[ $# -ge 2 ]] || err "--build-dir expects a value"
                BUILD_DIR="$2"
                shift 2
                ;;
            --out-dir)
                [[ $# -ge 2 ]] || err "--out-dir expects a value"
                OUT_DIR="$2"
                shift 2
                ;;
            --build-type)
                [[ $# -ge 2 ]] || err "--build-type expects a value"
                BUILD_TYPE="$2"
                shift 2
                ;;
            --run-tests)
                RUN_TESTS="true"
                shift
                ;;
            --github-release)
                CREATE_GITHUB_RELEASE="true"
                shift
                ;;
            --tag)
                [[ $# -ge 2 ]] || err "--tag expects a value"
                TAG="$2"
                shift 2
                ;;
            --title)
                [[ $# -ge 2 ]] || err "--title expects a value"
                TITLE="$2"
                shift 2
                ;;
            --notes-file)
                [[ $# -ge 2 ]] || err "--notes-file expects a value"
                NOTES_FILE="$2"
                shift 2
                ;;
            --draft)
                DRAFT="true"
                shift
                ;;
            --prerelease)
                PRERELEASE="true"
                shift
                ;;
            -h|--help)
                usage
                exit 0
                ;;
            *)
                err "unknown option: $1"
                ;;
        esac
    done

    case "$FORMAT" in
        all|deb|rpm|tgz|windows)
            ;;
        *)
            err "invalid --format: $FORMAT (expected all|deb|rpm|tgz|windows)"
            ;;
    esac

    NATIVE_BUILD_DIR="$BUILD_DIR"
    WINDOWS_BUILD_DIR="${BUILD_DIR}-windows"
}

resolve_tag() {
    if [[ -n "$TAG" ]]; then
        return
    fi

    if git -C "$ROOT_DIR" describe --tags --exact-match >/dev/null 2>&1; then
        TAG="$(git -C "$ROOT_DIR" describe --tags --exact-match)"
        return
    fi

    if git -C "$ROOT_DIR" describe --tags --abbrev=0 >/dev/null 2>&1; then
        TAG="$(git -C "$ROOT_DIR" describe --tags --abbrev=0)"
        return
    fi

    TAG="v0.1.0"
}

configure_and_build_native() {
    mkdir -p "$NATIVE_BUILD_DIR"
    cmake -S "$ROOT_DIR" -B "$NATIVE_BUILD_DIR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
    cmake --build "$NATIVE_BUILD_DIR"
}

configure_and_build_windows() {
    [[ -f "$TOOLCHAIN_FILE" ]] || err "toolchain file not found: $TOOLCHAIN_FILE"
    require_cmd x86_64-w64-mingw32-gcc
    require_cmd x86_64-w64-mingw32-g++

    mkdir -p "$WINDOWS_BUILD_DIR"
    cmake \
        -S "$ROOT_DIR" \
        -B "$WINDOWS_BUILD_DIR" \
        -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
        -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN_FILE"
    cmake --build "$WINDOWS_BUILD_DIR"
}

run_tests() {
    if [[ "$RUN_TESTS" == "true" ]]; then
        if [[ "$FORMAT" == "windows" ]]; then
            echo "==> Skipping tests for windows cross-build"
            return
        fi
        ctest --test-dir "$NATIVE_BUILD_DIR" --output-on-failure
    fi
}

build_package() {
    local generator="$1"
    local config_build_dir="$2"
    echo "==> Building package with CPack generator: ${generator}"
    cpack --config "$config_build_dir/CPackConfig.cmake" -G "$generator" -B "$OUT_DIR"
}

collect_file() {
    local pattern="$1"
    mapfile -t matched < <(find "$OUT_DIR" -maxdepth 1 -type f -name "$pattern" | sort)
    if [[ ${#matched[@]} -eq 0 ]]; then
        err "no artifacts found for pattern: $pattern"
    fi
    ARTIFACTS+=("${matched[@]}")
}

build_artifacts() {
    mkdir -p "$OUT_DIR"
    ARTIFACTS=()

    case "$FORMAT" in
        all)
            build_package "DEB" "$NATIVE_BUILD_DIR"
            build_package "RPM" "$NATIVE_BUILD_DIR"
            build_package "ZIP" "$WINDOWS_BUILD_DIR"
            collect_file "*.deb"
            collect_file "*.rpm"
            collect_file "*.zip"
            ;;
        deb)
            build_package "DEB" "$NATIVE_BUILD_DIR"
            collect_file "*.deb"
            ;;
        rpm)
            build_package "RPM" "$NATIVE_BUILD_DIR"
            collect_file "*.rpm"
            ;;
        tgz)
            build_package "TGZ" "$NATIVE_BUILD_DIR"
            collect_file "*.tar.gz"
            ;;
        windows)
            build_package "ZIP" "$WINDOWS_BUILD_DIR"
            collect_file "*.zip"
            ;;
    esac

    echo "==> Exported artifacts:"
    for artifact in "${ARTIFACTS[@]}"; do
        echo "  - $artifact"
    done
}

create_or_update_github_release() {
    [[ "$CREATE_GITHUB_RELEASE" == "true" ]] || return

    require_cmd gh
    resolve_tag

    local release_title="$TITLE"
    [[ -n "$release_title" ]] || release_title="$TAG"

    local -a create_args=(release create "$TAG" --title "$release_title")

    if [[ "$DRAFT" == "true" ]]; then
        create_args+=(--draft)
    fi

    if [[ "$PRERELEASE" == "true" ]]; then
        create_args+=(--prerelease)
    fi

    if [[ -n "$NOTES_FILE" ]]; then
        [[ -f "$NOTES_FILE" ]] || err "notes file not found: $NOTES_FILE"
        create_args+=(--notes-file "$NOTES_FILE")
    else
        create_args+=(--generate-notes)
    fi

    local release_exists="false"
    pushd "$ROOT_DIR" >/dev/null
    if gh release view "$TAG" >/dev/null 2>&1; then
        release_exists="true"
    fi
    if [[ "$release_exists" == "false" ]]; then
        gh "${create_args[@]}" "${ARTIFACTS[@]}"
    else
        echo "==> Release $TAG already exists. Uploading artifacts..."
        gh release upload "$TAG" "${ARTIFACTS[@]}" --clobber
    fi
    popd >/dev/null
}

main() {
    require_cmd cmake
    require_cmd cpack
    require_cmd git

    parse_args "$@"

    case "$FORMAT" in
        windows)
            configure_and_build_windows
            ;;
        all)
            configure_and_build_native
            configure_and_build_windows
            ;;
        *)
            configure_and_build_native
            ;;
    esac

    run_tests
    build_artifacts
    create_or_update_github_release

    echo "Done."
}

main "$@"
