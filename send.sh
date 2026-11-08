#!/bin/sh
# Send a file to the server as one GSP1 packet and print the ack.
# Usage: ./send.sh <file> [host] [port]
set -e
f=$1; host=${2:-localhost}; port=${3:-8080}
[ -f "$f" ] || { echo "usage: $0 <file> [host] [port]"; exit 1; }
len=$(wc -c < "$f" | tr -d ' ')

ack=$( { printf 'GSP1\001'; printf '%016x' "$len" | xxd -r -p; cat "$f"; } \
	| nc -w 2 "$host" "$port" | xxd -p)

case $ack in
	00) echo "OK: sent $len bytes" ;;
	01) echo "server error"; exit 1 ;;
	*)  echo "no/invalid ack: '$ack'"; exit 1 ;;
esac
