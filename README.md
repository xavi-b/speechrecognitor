```
mkdir models
cd models
MODELS_VERSION=0.7.0
wget https://github.com/mozilla/DeepSpeech/releases/download/v$MODELS_VERSION/deepspeech-$MODELS_VERSION-models.pbmm -O models.pbmm
wget https://github.com/mozilla/DeepSpeech/releases/download/v$MODELS_VERSION/deepspeech-$MODELS_VERSION-models.tflite -O models.tflite
wget https://github.com/mozilla/DeepSpeech/releases/download/v$MODELS_VERSION/deepspeech-$MODELS_VERSION-models.scorer -O models.scorer
```

```
BAZEL_VERSION=0.24.1
wget https://github.com/bazelbuild/bazel/releases/download/$BAZEL_VERSION/bazel-$BAZEL_VERSION-installer-linux-x86_64.sh
chmod +x bazel-$BAZEL_VERSION-installer-linux-x86_64.sh
sudo ./bazel-$BAZEL_VERSION-installer-linux-x86_64.sh
```

```
cd tensorflow
git checkout origin/r1.15
# choose python3 !
./configure
```

```
cd tensorflow
ln -s ../DeepSpeech/native_client ./
bazel build \
    --workspace_status_command="bash native_client/bazel_workspace_status_cmd.sh" \
    --config=monolithic -c opt --copt=-O3 --copt="-D_GLIBCXX_USE_CXX11_ABI=0" \
    --copt=-fvisibility=hidden //native_client:libdeepspeech.so
```

Output in `tensorflow/bazel-bin/`

# native_client
```
TFDIR=~/tensorflow
cd ../DeepSpeech/native_client
make deepspeech
```

https://github.com/mozilla/DeepSpeech/blob/v0.7.0/native_client/README.rst

# TTS

```
sudo apt install qtspeech5-speechd-plugin
sudo apt install speech-dispatcher-ibmtts
sudo apt install mbrola mbrola-en1 mbrola-us1 
sudo apt install libttspico-data libttspico-utils libttspico0
sudo apt install espeak espeak-ng
sudo apt install freetts
sudo apt install festival festvox-us1 festvox-us2 festvox-us3
```
