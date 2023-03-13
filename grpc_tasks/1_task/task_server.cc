#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>

#include "task.grpc.pb.h"

#define WORD_SIZE 128
#define BYTE(n) ((n) % 256)
#define TRY_TO_WAIT(n) ((n) % 100)
#define WAIT 0

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
/*
using task::Triger;
using task::TaskOne;
*/
class TaskOneServer final : public TaskOne::Service {
public:
	TaskOneServer(): Service() {
		srand(time(NULL));	
	}

	Status Triger(ServerContext* context, 
				  const Request* request,
				  Answer* ans) {
		std::string word;
		for(int i = 0; i < WORD_SIZE; ++i) {
			word.push_back(BYTE(rand()));
		}
		if(TRY_TO_WAIT(rand()) == WAIT) {
			usleep(1000000);
		}
		ans -> set_word(word);
		return Status::OK;
	}
};

void RunServer() {
	std::string server_address("0.0.0.0:50051");
	TaskOneServer service;

	grpc::EnableDefaultHealthCheckService(true);
	grpc::reflection::InitProtoReflectionServerBuilderPlugin();
	
	ServerBuilder builder;
	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);

	std::unique_ptr<Server> server(builder.BuildAndStart());
	std::cout << "Server listening on " << server_address << std::endl;
	server->Wait();
};

int main() {
	RunServer();
	return 0;
}
