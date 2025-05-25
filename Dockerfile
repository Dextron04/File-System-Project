# Use an official GCC image for building C code
FROM gcc:latest

# Install make just in case it's missing (usually included)
RUN apt-get update && apt-get install -y make

# Set the working directory inside the container
WORKDIR /usr/src/filesystem

# Copy all local files into the container
COPY . .

# Run make to build your file system
RUN make

# Set default command; you can replace 'main' with your binary name
CMD ["./filesystem"]

