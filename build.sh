#!/bin/bash

CONTAINER_NAME=deepspeechbuilder

docker kill ${CONTAINER_NAME}-container
docker rm ${CONTAINER_NAME}-container
docker build -t ${CONTAINER_NAME}-image .
docker run --name ${CONTAINER_NAME}-container -v $PWD:/src -ti ${CONTAINER_NAME}-image
