// Code generated by protoc-gen-go-grpc. DO NOT EDIT.

package task

import (
	context "context"
	grpc "google.golang.org/grpc"
	codes "google.golang.org/grpc/codes"
	status "google.golang.org/grpc/status"
)

// This is a compile-time assertion to ensure that this generated file
// is compatible with the grpc package it is being compiled against.
const _ = grpc.SupportPackageIsVersion7

// TaskOneClient is the client API for TaskOne service.
//
// For semantics around ctx use and closing/ending streaming RPCs, please refer to https://pkg.go.dev/google.golang.org/grpc/?tab=doc#ClientConn.NewStream.
type TaskOneClient interface {
	Triger(ctx context.Context, in *Request, opts ...grpc.CallOption) (*Answer, error)
}

type taskOneClient struct {
	cc grpc.ClientConnInterface
}

func NewTaskOneClient(cc grpc.ClientConnInterface) TaskOneClient {
	return &taskOneClient{cc}
}

func (c *taskOneClient) Triger(ctx context.Context, in *Request, opts ...grpc.CallOption) (*Answer, error) {
	out := new(Answer)
	err := c.cc.Invoke(ctx, "/TaskOne/Triger", in, out, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

// TaskOneServer is the server API for TaskOne service.
// All implementations must embed UnimplementedTaskOneServer
// for forward compatibility
type TaskOneServer interface {
	Triger(context.Context, *Request) (*Answer, error)
	mustEmbedUnimplementedTaskOneServer()
}

// UnimplementedTaskOneServer must be embedded to have forward compatible implementations.
type UnimplementedTaskOneServer struct {
}

func (UnimplementedTaskOneServer) Triger(context.Context, *Request) (*Answer, error) {
	return nil, status.Errorf(codes.Unimplemented, "method Triger not implemented")
}
func (UnimplementedTaskOneServer) mustEmbedUnimplementedTaskOneServer() {}

// UnsafeTaskOneServer may be embedded to opt out of forward compatibility for this service.
// Use of this interface is not recommended, as added methods to TaskOneServer will
// result in compilation errors.
type UnsafeTaskOneServer interface {
	mustEmbedUnimplementedTaskOneServer()
}

func RegisterTaskOneServer(s grpc.ServiceRegistrar, srv TaskOneServer) {
	s.RegisterService(&_TaskOne_serviceDesc, srv)
}

func _TaskOne_Triger_Handler(srv interface{}, ctx context.Context, dec func(interface{}) error, interceptor grpc.UnaryServerInterceptor) (interface{}, error) {
	in := new(Request)
	if err := dec(in); err != nil {
		return nil, err
	}
	if interceptor == nil {
		return srv.(TaskOneServer).Triger(ctx, in)
	}
	info := &grpc.UnaryServerInfo{
		Server:     srv,
		FullMethod: "/TaskOne/Triger",
	}
	handler := func(ctx context.Context, req interface{}) (interface{}, error) {
		return srv.(TaskOneServer).Triger(ctx, req.(*Request))
	}
	return interceptor(ctx, in, info, handler)
}

var _TaskOne_serviceDesc = grpc.ServiceDesc{
	ServiceName: "TaskOne",
	HandlerType: (*TaskOneServer)(nil),
	Methods: []grpc.MethodDesc{
		{
			MethodName: "Triger",
			Handler:    _TaskOne_Triger_Handler,
		},
	},
	Streams:  []grpc.StreamDesc{},
	Metadata: "task.proto",
}
