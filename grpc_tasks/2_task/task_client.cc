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

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using grpc::Channel;
using grpc::ClientContext;
/*
using task::Triger;
using task::TaskTwo;
*/
class TaskTwoClient {
public:
	TaskTwoClient(std::shared_ptr<Channel> channel)
   	: stub_(TaskTwo::NewStub(channel))	{}
	
	std::vector<uint8_t> Gen(const std::string& user) {
		Request request;
		request.set_name(user);

		Answer ans;
		ClientContext context;

		auto status = stub_ -> Gen(&context, request, &ans);
		
		std::vector<uint8_t> res;
		for(auto byte : ans.word()) {
			res.push_back(uint8_t(byte));
		}
		
		if(!status.ok()) {
			std::cout << status.error_code() << ": " 
				<< status.error_message() << std::endl;
			return {};
		}
		return res;
	}	

private:
	std::unique_ptr<TaskTwo::Stub> stub_;

};

void print_byte_word(std::vector<uint8_t> word) {
	for(const auto& byte : word) {
		printf("%u ", byte);
	}
}

int main(int argc, char** argv) {
	TaskTwoClient task_two_client(
		grpc::CreateChannel("0.0.0.0:50051", grpc::InsecureChannelCredentials())
	);
	std::string user("qwert");
	std::vector<uint8_t> word = task_two_client.Gen(user);
	print_byte_word(word);
	std::cout << std::endl;
	return 0;
};
