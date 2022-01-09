#!/bin/bash

./configure
bazel build --workspace_status_command="bash native_client/bazel_workspace_status_cmd.sh" --config=monolithic -c opt --copt=-O3 --copt="-D_GLIBCXX_USE_CXX11_ABI=0" --copt=-fvisibility=hidden //native_client:libdeepspeech.so
mv bazel-bin/native_client/libdeepspeech.so /src/third-party/.

cd /src/third-party
wget -nv https://github.com/mozilla/DeepSpeech/releases/download/v0.9.3/deepspeech-0.9.3-models.pbmm
wget -nv https://github.com/mozilla/DeepSpeech/releases/download/v0.9.3/deepspeech-0.9.3-models.scorer
cd -
