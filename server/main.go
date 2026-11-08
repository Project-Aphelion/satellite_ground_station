package main

import (
	"bufio"
	"crypto/rand"
	"encoding/binary"
	"fmt"
	"io"
	"log"
	"net"
	"os"
	"path/filepath"
	"runtime"
)

var exPath string

type Header struct {
	Magic   [4]byte // "GSP1"
	Version uint8   // 0x01
	Length  uint64  // 8 bytes, lenght
	// payload
}

// func readMsg(r io.Reader) (Header, []byte, error) {
// 	var h Header
// 	if err := binary.Read(r, binary.BigEndian, &h); err != nil {
// 		return h, nil, err
// 	}
// 	if string(h.Magic[:]) != "GSP1" || h.Version != 1 || h.Length > 256<<20 {
// 		return h, nil, fmt.Errorf("invalid header: %q v%d len=%d", h.Magic, h.Version, h.Length)
// 	}

// 	payload := make([]byte, h.Length)
// 	_, err := io.ReadFull(r, payload)
// 	return h, payload, err
// }

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

	if err := receive(bufio.NewReader(conn)); err != nil {
		log.Println("error: ", err)
		conn.Write([]byte{1}) // Error
		return
	}
	conn.Write([]byte{0}) // Success
}

func receive(r io.Reader) error {
	var h Header
	if err := binary.Read(r, binary.BigEndian, &h); err != nil {
		return err
	}
	if string(h.Magic[:]) != "GSP1" || h.Version != 1 || h.Length > 256<<20 {
		return fmt.Errorf("invalid header: %q v%d len=%d", h.Magic, h.Version, h.Length)
	}
	path := exPath + "/" + rand.Text() + ".iq"
	if err := writePayload(r, h.Length, path); err != nil {
		os.Remove(path) // remove incomplete recording
		return err
	}
	log.Printf("saved %s (%d bytes)", path, h.Length)
	return nil
}

func writePayload(r io.Reader, lenght uint64, path string) error {
	f, err := os.Create(path)
	if err != nil {
		return err
	}
	defer f.Close()

	n, err := io.CopyN(f, r, int64(lenght))
	if err != nil {
		return err
	}

	if n != int64(lenght) {
		return fmt.Errorf("incomplete payload")
	}

	return f.Sync()
}
