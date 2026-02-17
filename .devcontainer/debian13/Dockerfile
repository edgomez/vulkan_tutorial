FROM debian:trixie-slim

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        # ── Build toolchain ──────────────────────────────────────────
        build-essential \
        clang gdb gcc g++ \
        cmake ninja-build \
        git dos2unix clang-format \
        # ── Vulkan SDK / validation ──────────────────────────────────
        libvulkan-dev vulkan-validationlayers \
        # ── SDL3 dev ─────────────────────────────────────────────────
        libsdl3-dev \
        # ── SDL3 build-time dependencies (X11, Wayland, input, …) ───
        libasound2-dev libpipewire-0.3-dev libx11-dev libxext-dev \
        libxrandr-dev libxcursor-dev libxfixes-dev libxi-dev libxss-dev libxtst-dev \
        libxkbcommon-dev libdrm-dev libgbm-dev libgl1-mesa-dev libgles2-mesa-dev \
        libegl1-mesa-dev libdbus-1-dev libibus-1.0-dev libudev-dev libwayland-dev libdecor-0-dev \
        # ── GPU / display runtime (needed to actually render) ────────
        mesa-vulkan-drivers \
        libgl1-mesa-dri \
        libegl-mesa0 \
        libglx-mesa0 \
        # ── Wayland client runtime ───────────────────────────────────
        libwayland-client0 \
        libwayland-cursor0 \
        libwayland-egl1 \
        libdecor-0-plugin-1-gtk \
        # ── X11 client runtime (XWayland / native X11) ───────────────
        libx11-xcb1 \
        libxcb1 \
        # ── Misc ─────────────────────────────────────────────────────
        sudo \
        locales \
    && rm -rf /var/lib/apt/lists/*

# Set up a non-root user for VS Code
ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=${USER_UID}
RUN groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME \
    && echo "$USERNAME ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/$USERNAME

# Let the container user own XDG_RUNTIME_DIR (created at container start)
RUN mkdir -p /run/user/${USER_UID} && chown ${USER_UID}:${USER_GID} /run/user/${USER_UID}

USER $USERNAME
WORKDIR /workspace
