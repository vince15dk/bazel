/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <functional>

#include <grpcpp/generic/generic_stub.h>
#include <grpcpp/impl/rpc_method.h>
#include <grpcpp/support/client_callback.h>

namespace grpc {

namespace {
std::unique_ptr<GenericClientAsyncReaderWriter> CallInternal(
    ChannelInterface* channel, ClientContext* context,
    const grpc::string& method, CompletionQueue* cq, bool start, void* tag) {
  return std::unique_ptr<GenericClientAsyncReaderWriter>(
      internal::ClientAsyncReaderWriterFactory<ByteBuffer, ByteBuffer>::Create(
          channel, cq,
          internal::RpcMethod(method.c_str(),
                              internal::RpcMethod::BIDI_STREAMING),
          context, start, tag));
}

}  // namespace

// begin a call to a named method
std::unique_ptr<GenericClientAsyncReaderWriter> GenericStub::Call(
    ClientContext* context, const grpc::string& method, CompletionQueue* cq,
    void* tag) {
  return CallInternal(channel_.get(), context, method, cq, true, tag);
}

// setup a call to a named method
std::unique_ptr<GenericClientAsyncReaderWriter> GenericStub::PrepareCall(
    ClientContext* context, const grpc::string& method, CompletionQueue* cq) {
  return CallInternal(channel_.get(), context, method, cq, false, nullptr);
}

// setup a unary call to a named method
std::unique_ptr<GenericClientAsyncResponseReader> GenericStub::PrepareUnaryCall(
    ClientContext* context, const grpc::string& method,
    const ByteBuffer& request, CompletionQueue* cq) {
  return std::unique_ptr<GenericClientAsyncResponseReader>(
      internal::ClientAsyncResponseReaderFactory<ByteBuffer>::Create(
          channel_.get(), cq,
          internal::RpcMethod(method.c_str(), internal::RpcMethod::NORMAL_RPC),
          context, request, false));
}

void GenericStub::experimental_type::UnaryCall(
    ClientContext* context, const grpc::string& method,
    const ByteBuffer* request, ByteBuffer* response,
    std::function<void(Status)> on_completion) {
  internal::CallbackUnaryCall(
      stub_->channel_.get(),
      internal::RpcMethod(method.c_str(), internal::RpcMethod::NORMAL_RPC),
      context, request, response, std::move(on_completion));
}

void GenericStub::experimental_type::PrepareBidiStreamingCall(
    ClientContext* context, const grpc::string& method,
    experimental::ClientBidiReactor<ByteBuffer, ByteBuffer>* reactor) {
  internal::ClientCallbackReaderWriterFactory<ByteBuffer, ByteBuffer>::Create(
      stub_->channel_.get(),
      internal::RpcMethod(method.c_str(), internal::RpcMethod::BIDI_STREAMING),
      context, reactor);
}

}  // namespace grpc
