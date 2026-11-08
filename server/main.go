package main

import (
	"bufio"
	"encoding/binary"
	"fmt"
	"io"
	"log"
	"net"
	"os"
	"path/filepath"
	"runtime"
	"time"
)

var exPath string

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
	if string(h.Magic[:]) != "GSP1" || h.Version != 1 || h.Length > 256<<20 {
		return h, nil, fmt.Errorf("invalid header: %q v%d len=%d", h.Magic, h.Version, h.Length)
	}

	payload := make([]byte, h.Length)
	_, err := io.ReadFull(r, payload)
	return h, payload, err
}

func main() {
	_, filename, _, _ := runtime.Caller(0)
	exPath = filepath.Dir(filename) + "/data"
	log.Print(exPath)

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
	writer := bufio.NewWriter(conn)

	for {
		h, payload, err := readMsg(reader)
		if err != nil {
			if err != io.EOF {
				log.Println("read error: ", err)
				log.Print("acknowledgement: Error")
				writer.Write([]byte{1})
				writer.Flush()
			}
			return
		}

		v, _ := time.Now().UTC().MarshalText()
		f, err := os.Create(exPath + "/" + string(v))
		if err != nil {
			panic(err)
		}
		defer f.Close()
		// n, err := f.Write([]byte(fmt.Sprintf("%x", payload)))
		n, err := f.Write(payload)
		if err != nil {
			log.Println("write error: ", err)
		} else if n != int(h.Length) {
			log.Print("Could not write whole payload")
		}

		log.Printf("type=%d len=%d payload=%x", h.Version, h.Length, payload)
		log.Print("acknowledgement: Success")
		writer.Write([]byte{0})
		writer.Flush()
	}
}
