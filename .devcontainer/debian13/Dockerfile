FROM debian:trixie-slim

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        build-essential \
        clang gdb gcc g++ \
        cmake ninja-build \
        git dos2unix clang-format \
        libsdl3-dev libvulkan-dev vulkan-validationlayers \
        sudo \
        libasound2-dev libpipewire-0.3-dev libx11-dev libxext-dev \
        libxrandr-dev libxcursor-dev libxfixes-dev libxi-dev libxss-dev libxtst-dev \
        libxkbcommon-dev libdrm-dev libgbm-dev libgl1-mesa-dev libgles2-mesa-dev \
        libegl1-mesa-dev libdbus-1-dev libibus-1.0-dev libudev-dev libwayland-dev \
    && rm -rf /var/lib/apt/lists/*

# Set up a non-root user for VS Code
ARG USERNAME=vscode
RUN useradd -m $USERNAME && echo "$USERNAME ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/$USERNAME
USER $USERNAME
WORKDIR /workspace
