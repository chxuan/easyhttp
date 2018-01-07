#!/bin/bash

curl -d "hello world" "localhost:6666/add?a=1&b=2"
curl -o "beyond.mp3" "localhost:6666/download?file_name=beyond.mp3"

