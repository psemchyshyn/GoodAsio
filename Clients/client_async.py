import socket
import time
import sys

#
# IP_ADDRESS = sys.argv[1]
# PORT = int(sys.argv[2])

import asyncio

async def tcp_echo_client():
    reader, writer = await asyncio.open_connection(
        '127.0.0.1', 1024)
    writer.write(b"Client's request")
    await writer.drain()

    data = await reader.read(20)
    writer.close()
    await writer.wait_closed()

async def main():
    while True:
        result = await asyncio.gather(*[tcp_echo_client() for i in range(1000)])

asyncio.run(main())