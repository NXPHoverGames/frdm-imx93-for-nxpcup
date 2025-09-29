# Linux bash script that handles the installation of required
# packages and the setup of the development environment. Running
# this script is more or less the equivalent of manually performing
# the steps presented in [1] (Linux)
#
# [1]: https://docs.zephyrproject.org/latest/develop/getting_started/index.html

#!/bin/bash

# immediately exit on error
set -e

# import utility functions and variables
source "./scripts/util.sh"

# don't install packages if set to 1
NO_PACKAGE_INSTALL=0

# don't download uuu if set to 1
NO_UUU_DOWNLOAD=0

# name of the uuu binary to download
UUU_NAME="uuu"

# list of packages to install
PACKAGES=(
	git cmake ninja-build
	gperf ccache dfu-util
	device-tree-compiler wget python3-dev
	python3-venv python3-tk xz-utils file
	make gcc gcc-multilib g++-multilib
	libsdl2-dev libmagic1 curl minicom
)

setup_west() {
	# set up the virtual environment
	if [ ! -d "$WORK_DIR/.venv" ]; then
		log "setting up virtual environment at $WORK_DIR/.venv..."

		if ! run_command "python3 -m venv $WORK_DIR/.venv"; then
			log_err "failed to set up virtual environment"
			exit 1
		fi
	fi

	# activate the virtual environment
	log "activating virtual environment at $YELLOW$WORK_DIR/.venv$NONE..."
	source "$WORK_DIR"/.venv/bin/activate

	# install west if need be
	log "installing west..."
	if ! pip3 show &> /dev/null; then
		if ! run_command "pip3 install west"; then
			log_err "failed to install west"
			exit 1
		fi
	fi

	# initialize west if need be
	if [ ! -d "$WORK_DIR/.west" ]; then
		log "initializing west at $WORK_DIR..."

		if ! (cd $WORK_DIR && run_command "west init -l $CRT_DIR"); then
			log_err "failed to initialize west"
			exit 1
		fi
	fi

	# update the repositories
	log "updating repositories..."
	run_command "west update" || { log_err "failed to update repositories"; exit 1; }

	# export the CMAKE package
	log "exporting cmake package..."
	run_command "west zephyr-export" || { log_err "failed to export cmake package"; exit 1; }

	# install python package used by west
	log "installing west packages..."
	run_command "west packages pip --install" || {\
		log_err "failed to install west packages";
		exit 1;
	}

	# install the ARM64 toolchain
	log "installing the ARM64 toolchain..."
	run_command "west sdk install --install-base $WORK_DIR -t aarch64-zephyr-elf" || {\
		log_err "failed to install the toolchain";
		exit 1;
	}

	log "$GREEN""west environment ready""$NONE"
	log "activate virtual environment by sourcing $YELLOW$WORK_DIR/.venv/bin/activate$NONE"
}

main() {
	if [ "$NO_PACKAGE_INSTALL" = "0" ]; then
		install_packages "${PACKAGES[@]}"
	fi

	if [ "$NO_UUU_DOWNLOAD" = "0" ]; then
		download_uuu "$UUU_NAME" "$CRT_DIR/boot/$UUU_NAME"
	fi

	setup_west
}

print_usage() {
	echo "Usage: $0 [-h] [-p] [-d] [-u UUU_NAME]"
	echo "Options:"
	echo " -h                print help information"
	echo " -p                skip package installation"
	echo " -d                skip uuu download"
	echo " -u                download specified uuu binary"
}

# parse arguments
while getopts "hpdu:" opt; do
	case "$opt" in
	h) print_usage; exit 0 ;;
	p) NO_PACKAGE_INSTALL=1 ;;
	d) NO_UUU_DOWNLOAD=1 ;;
	u) UUU_NAME="${OPTARG}" ;;
	*) print_usage; exit 1 ;;
	esac
done

main
