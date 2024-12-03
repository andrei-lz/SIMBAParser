# PCAP Parser

This is a high-performance PCAP (Packet Capture) parser designed to efficiently process network packet data with chunked input reading and buffered JSON output. It has been tested against SIMBA protocol dumps (version 4.3) from the [Moscow Exchange FTP server](https://ftp.moex.com/pub/SIMBA/Spectra/prod).

---

## Features

### 1. **Chunked Input Reading**
- Uses memory-mapped files for efficient I/O, minimizing system overhead for large PCAP files.
- Supports reading in chunks, avoiding the need to load entire files into memory.
- Designed with performance in mind, leveraging platform-specific optimizations (e.g., `mmap` on Linux, `MapViewOfFile` on Windows).

### 2. **Buffered JSON Output**
- Outputs packet data in JSON format for easier analysis and integration with modern tools.
- Buffered approach to minimize disk I/O, flushing output in large chunks every 50000 packets rather than per packet.

### 3. **Protocol Support**
- Decodes Ethernet, IPv4, UDP, and TCP headers.
- Parses SIMBA protocol packets, focusing on key message types like:
  - **Order Updates**
  - **Order Executions**
  - **Order Book Snapshots**
  - **Security Definition**
  - **Security Status**
  - **Security Definition Update Report**
  - **Sequence Reset**
  - **Trading Session Status**

### 4. **Cross-Platform Support**
- Fully functional on Linux and Windows, with platform-specific optimizations for memory mapping and file handling.

### 5. **Performance**
- Processes ~50,000 packets in under 500ms on Debian x64 with optimized build settings.

---

## Building the Project

### Prerequisites
- A modern C++ compiler supporting C++20 (e.g., GCC 10+, Clang 12+, MSVC 2019+).
- [CMake](https://cmake.org/) and [Ninja](https://ninja-build.org/) for the build process.

### Build Instructions
1. Clone the repository:
   ```bash
   git clone <repository_url>
   cd PCAPParser
   ```
2. Create a build directory and configure the project:
    ```bash
    mkdir build
    cd build
    cmake .. -GNinja
    ninja
    ```

### Running The Parser
    ``bash
    ./PCAPParser -p <pcap_file> -o <output_file>
    ```

### Sample Output
    ```json
    [{"MsgSeqNum":6084478,"MsgSize":64,"MsgFlags":9,"SendingTime":1696916700000578783}, {"TransactTime":0,"ExchangeTradingSessionID":4294967295}, {"blockLength":28,"templateId":10,"schemaId":19780,"version":4}, [{ "SecurityID": 4177141, "SecurityIDSource": "8", "Volatility": {"mantissa":4798531,"exponent":-5}, "TheorPrice": {"mantissa":978500,"exponent":-5}, "TheorPriceLimit": {"mantissa":978500,"exponent":-5} }]]
    ```
### Improvements I Would Like To Make

If more time were available, the following features and optimizations could be implemented:

### 1. **SIMBA Protocol Enhancements**
- Add support for decoding SIMBA version 5.

### 2. **Streaming Mode**
- Introduce a true streaming mode to process packets as they arrive, reducing memory usage for live capture scenarios.

### 3. **Multithreading**
- Parallelize packet processing for further performance improvements on multi-core systems.

### 4. **Compression**
- Support compressed PCAP files (e.g., `.pcap.gz`) to handle archived data directly.

### 5. **Expanded Protocol Support**
- Decode IPv6 headers and other advanced protocols (e.g., ESP, SCTP).

### 6. **Enhanced Error Handling**
- Improve recovery from malformed packets or incomplete PCAP files.

### 7. **Faster Output**
- When profiling, around 13% of the CPU usage is taken by all the streams that I use for `operator<<` overloading of different types and outputting them into a file. I would implement a system similar to the mapping where I would paste in entire strings instead with statically alocated data structures

### 8. **Test Coverage**
- Add comprehensive unit and integration tests to ensure reliability across edge cases.
