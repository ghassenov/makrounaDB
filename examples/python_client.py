#!/usr/bin/env python3
import socket


def encode_command(args):
    output = f"*{len(args)}\r\n"
    for item in args:
        output += f"${len(item)}\r\n{item}\r\n"
    return output.encode()


def read_line(sock):
    data = bytearray()
    while True:
        chunk = sock.recv(1)
        if not chunk:
            raise RuntimeError("connection closed")
        data.extend(chunk)
        if len(data) >= 2 and data[-2:] == b"\r\n":
            return data.decode()


def read_reply(sock):
    header = read_line(sock)
    prefix = header[0]
    if prefix in ["+", "-", ":"]:
        return header.strip()
    if prefix == "$":
        length = int(header[1:-2])
        if length < 0:
            return "(nil)"
        payload = b""
        while len(payload) < length + 2:
            payload += sock.recv(length + 2 - len(payload))
        return payload[:-2].decode()
    return f"unsupported: {header.strip()}"


def main():
    with socket.create_connection(("127.0.0.1", 6379), timeout=2.0) as sock:
        commands = [
            ["PING"],
            ["SET", "name", "makrounaDB"],
            ["GET", "name"],
            ["INCR", "counter"],
            ["EXPIRE", "name", "30"],
            ["TTL", "name"],
        ]

        for cmd in commands:
            sock.sendall(encode_command(cmd))
            print("->", " ".join(cmd))
            print("<-", read_reply(sock))


if __name__ == "__main__":
    main()
