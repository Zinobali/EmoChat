@echo off
set PROTOC_PATH=D:\CppSoftware\vcpkg\installed\x64-windows\tools\protobuf\protoc.exe
set GRPC_PLUGIN_PATH=D:\CppSoftware\vcpkg\installed\x64-windows\tools\grpc\grpc_cpp_plugin.exe
set PROTO_FILE=message.proto

%PROTOC_PATH% -I="." --cpp_out=. --grpc_out=. --plugin=protoc-gen-grpc="%GRPC_PLUGIN_PATH%" "%PROTO_FILE%"

echo Done.
