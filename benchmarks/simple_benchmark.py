#!/usr/bin/env python3
import argparse
import socket
import statistics
import time


def encode_command(args):
    chunks = [f"*{len(args)}\r\n".encode()]
    for item in args:
        payload = item.encode()
        chunks.append(f"${len(payload)}\r\n".encode())
        chunks.append(payload + b"\r\n")
    return b"".join(chunks)


def read_line(sock):
    buffer = bytearray()
    while True:
        b = sock.recv(1)
        if not b:
            raise RuntimeError("connection closed")
        buffer.extend(b)
        if len(buffer) >= 2 and buffer[-2:] == b"\r\n":
            return bytes(buffer)


def read_reply(sock):
    head = read_line(sock)
    if not head:
        raise RuntimeError("empty reply")

    prefix = head[:1]
    if prefix in (b"+", b"-", b":"):
        return head

    if prefix == b"$":
        length = int(head[1:-2])
        if length < 0:
            return head
        payload = b""
        expected = length + 2
        while len(payload) < expected:
            chunk = sock.recv(expected - len(payload))
            if not chunk:
                raise RuntimeError("connection closed")
            payload += chunk
        return head + payload

    raise RuntimeError(f"unsupported RESP prefix: {prefix!r}")


def run_benchmark(host, port, requests):
    samples_ms = []
    with socket.create_connection((host, port), timeout=2.0) as sock:
        for i in range(requests):
            key = f"bench:{i % 1000}"
            start = time.perf_counter()
            sock.sendall(encode_command(["SET", key, str(i)]))
            read_reply(sock)
            samples_ms.append((time.perf_counter() - start) * 1000.0)

    total_seconds = sum(samples_ms) / 1000.0
    ops_per_sec = requests / total_seconds if total_seconds > 0 else 0.0
    p50 = statistics.median(samples_ms)
    p95 = sorted(samples_ms)[int(len(samples_ms) * 0.95) - 1]
    return ops_per_sec, p50, p95


def main():
    parser = argparse.ArgumentParser(description="makrounaDB simple benchmark")
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=6379)
    parser.add_argument("--requests", type=int, default=5000)
    parser.add_argument("--out", default="benchmarks/report.md")
    args = parser.parse_args()

    ops, p50, p95 = run_benchmark(args.host, args.port, args.requests)

    report = (
        "# Benchmark Report\n\n"
        f"- Host: {args.host}:{args.port}\n"
        f"- Requests: {args.requests}\n"
        f"- Throughput (ops/sec): {ops:.2f}\n"
        f"- p50 latency (ms): {p50:.4f}\n"
        f"- p95 latency (ms): {p95:.4f}\n"
    )

    with open(args.out, "w", encoding="utf-8") as handle:
        handle.write(report)

    print(report)


if __name__ == "__main__":
    main()
