#!/bin/bash

# for logging purposes
RED="\e[31m"
GREEN="\e[32m"
YELLOW="\e[33m"
NONE="\e[0m"

# work directory
WORK_DIR=$(dirname $(pwd))

# current directory
CRT_DIR=$(pwd)

log() {
	printf "$@""\n"
}

log_err() {
	local msg="$1"

	log "$RED$msg$NONE"
}

run_command() {
	local cmd="$@"
	local log_file="_log.out"

	$cmd &> "$log_file" || {\
		log_err "failed to execute command: $cmd";
		log_err "dumping command output:"
		cat "$log_file" && rm -rf "$log_file";
		return 1;
	}

	rm -rf "$log_file"

	return 0
}

install_packages() {
	for pkg in "$@"; do
		log "installing $YELLOW$pkg$NONE..."

		if ! run_command "sudo apt-get install -y $pkg"; then
			log_err "failed to install $pkg"
			return 1
		fi
	done

	return 0
}

download_uuu() {
	# where to download uuu from
	local url="https://github.com/nxp-imx/mfgtools/releases/latest/download"
	# name of the binary to download
	local bin_name="$1"
	# where to stored the content of the downloaded binary
	local location="$2"

	if [ ! -f "$location" ]; then
		log "downloading uuu at $location..."

		run_command "curl -f -L --output $location $url/$bin_name" || {\
			log_fail "failed to download $bin_name from $url";
			return 1;
		}

		# downloaded binary will not have the x bit set so do it now
		chmod +x "$location"
	
		local version=$($location | grep -o 'libuuu_[^ ]*')
		log "installed uuu has version: $YELLOW$version$NONE"
	fi

	return 0
}
