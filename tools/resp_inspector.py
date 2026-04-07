#!/usr/bin/env python3
import argparse


def inspect(payload: str):
    raw = payload.encode("utf-8").decode("unicode_escape")
    print("Raw bytes:")
    print(repr(raw.encode()))
    print("\nVisible format:")
    print(raw.replace("\r", "\\r").replace("\n", "\\n\n"))


def main():
    parser = argparse.ArgumentParser(description="Inspect RESP payload escaping")
    parser.add_argument("payload", help="Payload string, e.g. '*1\\r\\n$4\\r\\nPING\\r\\n'")
    args = parser.parse_args()
    inspect(args.payload)


if __name__ == "__main__":
    main()
