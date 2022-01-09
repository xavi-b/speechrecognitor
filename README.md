https://deepspeech.readthedocs.io/en/r0.9/BUILDING.html
https://deepspeech.readthedocs.io/en/v0.8.2/BUILDING.html

https://github.com/bazelbuild/bazel/releases/tag/3.1.0

https://aws.amazon.com/fr/blogs/developer/real-time-streaming-transcription-with-the-aws-c-sdk/

# AWS SDK C++
```
mkdir build
cd build
cmake .. -DBUILD_ONLY="core;transcribestreaming" -DCMAKE_INSTALL_PREFIX=$PWD/../../
# -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTING=OFF
make
make install
```
