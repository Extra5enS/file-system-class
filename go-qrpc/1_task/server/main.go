package main

import (
    "context"
    "log"
    "net"
	"math/rand"
	"time"

    "google.golang.org/grpc"
    pb "../task"
)

const (
    port = ":50051"
)

type server struct {
    pb.UnimplementedTaskOneServer
}

func (c *taskOneClient) Triger(ctx context.Context, in *Request, opts ...grpc.CallOption) (*Answer, error) {
	rand.Seed(time.Now().Unix())
	word := []byte{}
	for i = 0; i < 128; ++i {
		word = append(word, rand.Intn(256))
	}
	return &pb.Answer{Word: word}, nil
}


func main() {
    lis, err := net.Listen("tcp", port)
    if err != nil {
        log.Fatalf("failed to listen: %v", err)
    }
    s := grpc.NewServer()
    pb.RegisterTaskOneServer(s, &server{})
    if err := s.Serve(lis); err != nil {
        log.Fatalf("failed to serve: %v", err)
    }
}

