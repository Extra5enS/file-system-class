#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <thread>

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
using grpc::Channel;
using grpc::ClientContext;
/*
using task::Triger;
using task::TaskOne;
*/

// part fron 1_task/
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

void RunSubServer(std::string server_address) {
	TaskOneServer service;

	grpc::EnableDefaultHealthCheckService(true);
	grpc::reflection::InitProtoReflectionServerBuilderPlugin();
	
	ServerBuilder builder;
	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);

	std::unique_ptr<Server> server(builder.BuildAndStart());
	std::cout << "SubServer listening on " << server_address << std::endl;
	server->Wait();
};

class TaskOneClient {
public:
	TaskOneClient(std::shared_ptr<Channel> channel)
   	: stub_(TaskOne::NewStub(channel))	{}
	
	std::string Triger(const std::string& user) {
		Request request;
		request.set_name(user);

		Answer ans;
		ClientContext context;

		auto status = stub_ -> Triger(&context, request, &ans);
		
		
		if(!status.ok()) {
			std::cout << status.error_code() << ": " 
				<< status.error_message() << std::endl;
			return {};
		}
		return ans.word();
	}	

private:
	std::unique_ptr<TaskOne::Stub> stub_;

};
////////

class TaskTwoServer final : public TaskTwo::Service {
public:
	TaskTwoServer(std::vector<std::string> addr): Service() {
		for(int i = 0; i < addr.size(); ++i) {
			// Start subservers
			threads.push_back(std::thread(RunSubServer, addr[i]));
			// Connect to it
			task_one_array.push_back(
					grpc::CreateChannel(addr[i], 
						grpc::InsecureChannelCredentials())	
			);
		}
	}

	Status Gen(ServerContext* context,
			   const Request* request,
			   Answer* ans) {
		std::string big_word;
		for(int i = 0; i < threads.size(); ++i) {
			std::string word = task_one_array[i].Triger("main_server");
			for(auto byte : word) {
				big_word.push_back(byte);
			}
		}
		ans -> set_word(big_word);
		return Status::OK;
	}

private:
	std::vector<TaskOneClient> task_one_array;
	std::vector<std::thread> threads;
};

void RunServer() {
	std::string server_address("0.0.0.0:50051");
	TaskTwoServer service({
			"0.0.0.0:50052",		
			"0.0.0.0:50053",		
			"0.0.0.0:50054",		
			"0.0.0.0:50055",
			"0.0.0.0:50056",
			"0.0.0.0:50057",
			"0.0.0.0:50058",
			"0.0.0.0:50059",
			"0.0.0.0:50062",		
			"0.0.0.0:50063",		
			"0.0.0.0:50064",		
			"0.0.0.0:50065",
			"0.0.0.0:50066",
			"0.0.0.0:50067",
			"0.0.0.0:50068",
			"0.0.0.0:50069"
			});
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
