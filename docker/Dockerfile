FROM ubuntu:22.04

# skip user interaction for apt-get
ENV DEBIAN_FRONTEND=noninteractive

SHELL ["/bin/bash", "-c"]

# install required packages
RUN <<EOF
	apt-get -y update

	apt-get -y upgrade

	apt-get install -y --no-install-recommends \
		git cmake ninja-build \
		gperf ccache dfu-util \
		device-tree-compiler wget python3-dev \
		python3-venv python3-tk xz-utils file \
		make gcc gcc-multilib g++-multilib \
		libsdl2-dev libmagic1 curl
EOF

# create and switch to the working directory
WORKDIR /home/root/nxpcup_root/nxpcup

# copy required files
COPY ../scripts/setup.sh ./scripts/setup.sh
COPY ../scripts/util.sh ./scripts/util.sh
COPY ../west.yml .

# prepare the environment
RUN ./scripts/setup.sh -p -d
