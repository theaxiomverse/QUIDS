# Use an official Ubuntu as the base image
FROM ubuntu:24.04

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive

# Install required packages
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libopenmpi-dev \
    libgtest-dev \
    libssl-dev \
    libfmt-dev \
    zlib1g-dev \
    libzstd-dev \
    libsnappy-dev \
    libbz2-dev \
    liblz4-dev \
    libyaml-dev \
    libhdf5-dev \
    libnetcdf-dev \
    libcurl4-openssl-dev \
    libeigen3-dev \
    curl
   
RUN rm -rf /var/lib/apt/lists/*

# Install Rust and Cargo
RUN curl https://sh.rustup.rs -sSf | sh -s -- -y
ENV PATH=/root/.cargo/bin:$PATH

# Install Intel oneAPI DPC++ compiler
RUN wget -qO- https://apt.repos.intel.com/oneapi/setup.sh | bash -s -- -a -s \
    && apt-get update 


# Install BLAKE3 from itzmeanjan/blake3
 RUN git clone https://github.com/BLAKE3-team/BLAKE3.git \
     && cd BLAKE3 \
     && cd c \
     && mkdir build \
     && cd build \
     && cmake .. \
     && make -j \
     && make install


   

# Install RocksDB
RUN git clone https://github.com/facebook/rocksdb.git \
     && cd rocksdb \
     && git checkout master \
     && mkdir build \
     && cd build \
     && cmake .. \
     && make -j static_lib \
     && make install \
     && cd ..


# Set the working directory
WORKDIR /app

# Copy the project files
COPY . .

# Create the build directory
RUN rm -rf build

RUN mkdir -p build

# Configure and build the project
#RUN cd build \
#    && cmake .. -DCMAKE_BUILD_TYPE=Release \
#    && make -j$(nproc)

# Set the entrypoint to run the built executable
ENTRYPOINT ["/bin/bash"]
