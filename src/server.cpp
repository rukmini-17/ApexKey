#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <fstream>
#include <mutex>

#include <grpcpp/grpcpp.h>
#include "kv_store.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using apexkey::KVStore;
using apexkey::PutRequest;
using apexkey::PutResponse;
using apexkey::GetRequest;
using apexkey::GetResponse;
using apexkey::UpdateRequest;
using apexkey::UpdateResponse;
using apexkey::DeleteRequest;
using apexkey::DeleteResponse;

class KVStoreServiceImpl final : public KVStore::Service {
private:
    std::unordered_map<std::string, std::string> database;
    const std::string db_file = "apex_data.txt";
    std::mutex db_mutex;

    // Helper function to sync memory to disk 
    void sync_to_disk() {
        std::ofstream outfile(db_file, std::ios::trunc);
        for (const auto& [k, v] : database) {
            outfile << k << " " << v << "\n";
        }
    }

public:
    KVStoreServiceImpl() {
        std::ifstream infile(db_file);
        std::string k, v;
        while (infile >> k >> v) {
            database[k] = v;
        }
    }

    Status Put(ServerContext* context, const PutRequest* request, PutResponse* response) override {
        std::lock_guard<std::mutex> lock(db_mutex);
        database[request->key()] = request->value();
        
        std::ofstream outfile(db_file, std::ios_base::app);
        outfile << request->key() << " " << request->value() << "\n";
        
        response->set_success(true);
        return Status::OK;
    }

    Status Get(ServerContext* context, const GetRequest* request, GetResponse* response) override {
        std::lock_guard<std::mutex> lock(db_mutex);
        auto it = database.find(request->key());
        if (it != database.end()) {
            response->set_value(it->second);
            response->set_found(true);
        } else {
            response->set_found(false);
        }
        return Status::OK;
    }

    Status Update(ServerContext* context, const UpdateRequest* request, UpdateResponse* response) override {
        std::lock_guard<std::mutex> lock(db_mutex);
        if (database.find(request->key()) != database.end()) {
            database[request->key()] = request->value();
            sync_to_disk();
            response->set_success(true);
        } else {
            response->set_success(false);
            response->set_message("Key not found.");
        }
        return Status::OK;
    }

    Status Delete(ServerContext* context, const DeleteRequest* request, DeleteResponse* response) override {
        std::lock_guard<std::mutex> lock(db_mutex);
        if (database.erase(request->key())) {
            sync_to_disk();
            response->set_success(true);
        } else {
            response->set_success(false);
        }
        return Status::OK;
    }
};

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    KVStoreServiceImpl service;
    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "ApexKey Server listening on " << server_address << std::endl;
    server->Wait();
}

int main() {
    RunServer();
    return 0;
}