FROM debian:stable

RUN apt-get update
RUN apt-get install -y wget
RUN apt-get install -y python3
RUN apt-get install -y libsox-dev
RUN apt-get install -y bazel-bootstrap
RUN apt-get install -y git
RUN apt-get install -y python3-distutils

# python
RUN ln -s /usr/bin/python3 /usr/bin/python

# bazel
RUN wget -nv https://github.com/bazelbuild/bazel/releases/download/3.1.0/bazel-3.1.0-linux-x86_64
RUN chmod +x bazel-3.1.0-linux-x86_64
RUN mv bazel-3.1.0-linux-x86_64 /usr/bin/.

WORKDIR /src/third-party/DeepSpeech/tensorflow
CMD /src/internal_build.sh
