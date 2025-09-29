# Linux bash script that handles the installation of required
# packages and the setup of the docker environment.

#!/bin/bash

# immediately exit on error
set -e

# import utility functions and variables
source "./scripts/util.sh"

# name of the uuu binary to download
UUU_NAME="uuu"

# path to the compose.yaml file
COMPOSE_FILE="./docker/compose.yaml"

main() {
	# download uuu if need be
	download_uuu "$UUU_NAME" "$CRT_DIR/boot/$UUU_NAME"

	# create image and start container
	log "creating image and starting container..."
	run_command "docker compose -f $COMPOSE_FILE up --detach" || {\
		log_fail "failed to create image and start the container"
		return 1
	}

	# all good, now shut down the container
	log "shutting down container..."
	run_command "docker compose -f $COMPOSE_FILE down" || {\
		log_fail "failed to shut down the container"
		return 1
	}

	log "$GREEN""docker environment ready!""$NONE"

	return 0
}

print_usage() {
	echo "Usage: $0 [-h] [-u UUU_NAME]"
	echo "Options:"
	echo " -h                print help information"
	echo " -u                download specified uuu binary"
}

# parse arguments
while getopts "hu:" opt; do
	case "$opt" in
	h) print_usage; exit 0 ;;
	u) UUU_NAME="${OPTARG}" ;;
	*) print_usage; exit 1 ;;
	esac
done

main
