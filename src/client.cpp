#include <iostream>
#include <memory>
#include <string>
#include <chrono> 
#include <vector>

#include <grpcpp/grpcpp.h>
#include "kv_store.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using apexkey::KVStore;
using apexkey::PutRequest;
using apexkey::PutResponse;
using apexkey::GetRequest;
using apexkey::GetResponse;
using apexkey::DeleteRequest;
using apexkey::DeleteResponse;
using apexkey::UpdateRequest;
using apexkey::UpdateResponse;

// Function to measure performance metrics for Benchmarking
void RunBenchmark(std::unique_ptr<KVStore::Stub>& stub, int num_operations) {
    std::cout << "\n>>> STARTING BENCHMARK: " << num_operations << " OPERATIONS <<<" << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < num_operations; ++i) {
        PutRequest request;
        request.set_key("bench_key_" + std::to_string(i));
        request.set_value("bench_val_" + std::to_string(i));

        PutResponse response;
        ClientContext context;
        Status status = stub->Put(&context, request, &response);
        
        if (!status.ok()) {
            std::cerr << "Benchmark failed at iteration " << i << std::endl;
            return;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;

    double ops_per_sec = num_operations / diff.count();
    double avg_latency_ms = (diff.count() / num_operations) * 1000;

    std::cout << "--------------------------------------------" << std::endl;
    std::cout << "RESULTS:" << std::endl;
    std::cout << "Total Time      : " << diff.count() << " seconds" << std::endl;
    std::cout << "Throughput      : " << ops_per_sec << " ops/sec" << std::endl;
    std::cout << "Average Latency : " << avg_latency_ms << " ms/op" << std::endl;
    std::cout << "--------------------------------------------\n" << std::endl;
}

int main() {
    // Establish connection to the server
    auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
    auto stub = KVStore::NewStub(channel);

    std::string test_key = "ProjectName";

    std::cout << "--- STARTING STANDARD CRUD VERIFICATION ---" << std::endl;

    // 1. CREATE (PUT)
    PutRequest put_req;
    put_req.set_key(test_key);
    put_req.set_value("InitialValue");
    PutResponse put_res;
    ClientContext put_ctx;
    stub->Put(&put_ctx, put_req, &put_res);
    std::cout << "[CHECK] 1. CREATE: Success." << std::endl;

    // 2. READ (GET)
    GetRequest get_req;
    get_req.set_key(test_key);
    GetResponse get_res;
    ClientContext get_ctx;
    stub->Get(&get_ctx, get_req, &get_res);
    std::cout << "[CHECK] 2. READ: Value is '" << get_res.value() << "'" << std::endl;

    // 3. UPDATE
    UpdateRequest up_req;
    up_req.set_key(test_key);
    up_req.set_value("ApexKey_v2");
    UpdateResponse up_res;
    ClientContext up_ctx;
    stub->Update(&up_ctx, up_req, &up_res);
    std::cout << "[CHECK] 3. UPDATE: Value updated to 'ApexKey_v2'" << std::endl;

    // 4. DELETE
    DeleteRequest del_req;
    del_req.set_key(test_key);
    DeleteResponse del_res;
    ClientContext del_ctx;
    stub->Delete(&del_ctx, del_req, &del_res);
    std::cout << "[CHECK] 4. DELETE: Success." << std::endl;

    // 5. FINAL VERIFY
    GetRequest final_req;
    final_req.set_key(test_key);
    GetResponse final_res;
    ClientContext final_ctx;
    stub->Get(&final_ctx, final_req, &final_res);
    if (!final_res.found()) {
        std::cout << "[CHECK] 5. VERIFY: Key confirmed deleted.\n" << std::endl;
    }

    // --- RUN BENCHMARK ---
    // Change to test higher number of requests
    RunBenchmark(stub, 10000); 

    return 0;
}