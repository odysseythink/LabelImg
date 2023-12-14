#!/bin/sh

protoc --go_out=. --plugin=protoc-gen-go=D:\go_work\gopath\bin\protoc-gen-go.exe *.proto

protoc -I ./ --cpp_out=D:\qt_work\knowledge_utils *.proto