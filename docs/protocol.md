# GSP1 — Ground Station Protocol

GSP1 is our custom binary protocol for transferring recordings from the C ground station to the Go backend over TCP.

## Packet format

Each TCP connection transfers one recording.

| Field | Size | Description |
|---|---|---|
| Magic | 4 bytes | ASCII `GSP1` |
| Version | 1 byte | `0x01` |
| Payload length | 8 bytes | Unsigned 64-bit integer, big-endian |
| Payload | N bytes | Raw binary recording data |

**Total header size: 13 bytes.**

Example: Sending a 4-byte payload (`DE AD BE EF`):

```text
47 53 50 31  01  00 00 00 00 00 00 00 04  DE AD BE EF
|-- GSP1 --|  |   |------ length -------|  |-- payload --|
           v1
```

## Communication flow

1. C client establishes a TCP connection to the Go server.
2. Client sends the 13-byte header.
3. Client sends exactly `payload_length` bytes.
4. Server validates the header and receives the payload.
5. Server saves the recording to a file.
6. Server sends a 1-byte acknowledgement:
   - `0x00` — Success
   - `0x01` — Error
7. Connection closes.

## Rules

- All multibyte integers are big-endian (network byte order).
- Maximum payload size: 256 MiB (268,435,456 bytes).
- Server rejects invalid magic, unsupported versions, or oversized payloads.
- TCP is a byte stream; reads and writes may be partial.
- Incomplete transfers must not be saved as successful recordings.
- Client only reports success after receiving `0x00`.
- No authentication or encryption in v1. Use only on trusted local networks.

## Implementation

**C client:** `send()`, `connect()`, `read()`, `write()`

**Go server:** `net.Listen()`, `io.ReadFull()`, `io.CopyN()`, `encoding/binary.BigEndian`

The payload is treated as arbitrary binary data for now. Satellite-specific metadata (frequency, sample rate, timestamp, etc.) will be added later.