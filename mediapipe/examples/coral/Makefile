SHELL := /bin/bash

MAKEFILE_DIR := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
MEDIAPIPE_DIR := $(MAKEFILE_DIR)/../../..

BAZEL_COMPILATION_MODE ?= opt
BAZEL_TARGET ?= mediapipe/examples/coral:face_detection_tpu
BAZEL_CPU ?= aarch64

OUT_DIR := $(MEDIAPIPE_DIR)/out/$(BAZEL_CPU)

PLATFORM ?= amd64
DOCKER_FILE ?= $(MAKEFILE_DIR)/Dockerfile.$(PLATFORM)
DOCKER_COMMAND ?=

bazel_output = $(MEDIAPIPE_DIR)/bazel-bin/$(subst :,/,$(1))

define run_command
chmod a+w /; \
groupadd --gid $(shell id -g) $(shell id -g -n); \
useradd -m -e '' -s /bin/bash --gid $(shell id -g) --uid $(shell id -u) $(shell id -u -n); \
echo '$(shell id -u -n) ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers; \
su $(shell id -u -n) $(if $(1),-c '$(1)',)
endef

.PHONY: help
help:
	@echo "make help   - Print help"
	@echo "make docker - Run Docker environment"
	@echo "make build  - Run Bazel build, use BAZEL_TARGET to choose which target to build"

ifeq (,$(wildcard /.dockerenv))
.PHONY: docker
docker:
	docker run --rm -i --tty \
	    -v $(MEDIAPIPE_DIR):/mediapipe \
	    --workdir /mediapipe/ \
	    $(shell docker build -q - < $(DOCKER_FILE)) \
	    /bin/bash -c "$(call run_command,$(DOCKER_COMMAND))"
endif

.PHONY: build
build:
	(cd $(MEDIAPIPE_DIR) && \
	 bazel build \
	     --config=gcc_config \
		 --define MEDIAPIPE_DISABLE_GPU=1 \
	     $(BAZEL_TARGET) && \
	 mkdir -p $(OUT_DIR) && \
	 cp -f $(call bazel_output,$(BAZEL_TARGET)) $(OUT_DIR))
	 cp -rvf $(OUT_DIR)/* /home/manu/nfs/tmp/mediapipe
