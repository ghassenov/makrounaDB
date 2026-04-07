# Benchmarks

## Quick run

1. Start server:
   - `./build/makrounaDB --port 6379 --aof /tmp/makrouna-bench.aof`
2. Run benchmark:
   - `python3 benchmarks/simple_benchmark.py --host 127.0.0.1 --port 6379 --requests 10000`

The benchmark writes a markdown report to `benchmarks/report.md`.
