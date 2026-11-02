package main

import (
	"bufio"
	"encoding/binary"
	"io"
	"log"
	"net"
)

type Header struct {
	Magic   [4]byte // "GSP1"
	Version uint8   // 0x01
	Length  uint64  // 8 bytes, lenght
	// payload
}

func readMsg(r io.Reader) (Header, []byte, error) {
	var h Header
	if err := binary.Read(r, binary.BigEndian, &h); err != nil {
		return h, nil, err
	}

	payload := make([]byte, h.Length)
	_, err := io.ReadFull(r, payload)
	return h, payload, err
}

func main() {
	listener, err := net.Listen("tcp", ":8080")
	if err != nil {
		log.Fatal("Error listening: ", err)
	}

	defer listener.Close()

	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Println("Error conn: ", err)
			continue
		}
		go HandleConnection(conn)
	}
}

func HandleConnection(conn net.Conn) {
	defer conn.Close()

	reader := bufio.NewReader(conn)

	for {
		h, payload, err := readMsg(reader)
		if err != nil {
			if err != io.EOF {
				log.Println("read error: ", err)
			}
			return
		}

		log.Printf("type=%d len=%d payload=%x", h.Version, h.Length, payload)
	}
}
