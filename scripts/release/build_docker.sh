#!/usr/bin/env bash
set -euo pipefail

IMAGE=""
TAG="latest"
PUSH="false"

usage() {
    cat <<'EOF'
Usage:
  bash scripts/release/build_docker.sh --image <dockerhub_user/makrounadb> [options]

Options:
  --image <name>     Docker image name (required), e.g. louay/makrounadb
  --tag <tag>        Docker tag (default: latest)
  --push             Push image to Docker Hub after build
  -h, --help         Show help

Examples:
  bash scripts/release/build_docker.sh --image louay/makrounadb --tag v0.1.0
  bash scripts/release/build_docker.sh --image louay/makrounadb --tag latest --push
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

while [[ $# -gt 0 ]]; do
    case "$1" in
        --image)
            [[ $# -ge 2 ]] || err "--image expects a value"
            IMAGE="$2"
            shift 2
            ;;
        --tag)
            [[ $# -ge 2 ]] || err "--tag expects a value"
            TAG="$2"
            shift 2
            ;;
        --push)
            PUSH="true"
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

[[ -n "$IMAGE" ]] || err "--image is required"
require_cmd docker

FULL_IMAGE="${IMAGE}:${TAG}"

echo "==> Building image ${FULL_IMAGE}"
docker build -t "$FULL_IMAGE" .

if [[ "$PUSH" == "true" ]]; then
    echo "==> Pushing ${FULL_IMAGE}"
    docker push "$FULL_IMAGE"
fi

echo "Done."
