# C2ImplantSrc


## Overview

C2ImplantSrc is developed to enable secure and reliable communication between the **C2 server** and **implanted devices**. This project supports the core functionalities required for devices to connect to and receive commands from the **SimpleC2** server.

## Features

- **HTTP Communication**: Uses `cURL` for reliable HTTP requests to communicate with the C2 server.
- **System Information Retrieval**: Leverages `WinAPI` to collect and transmit system and IP information.
- **Asynchronous Task Execution**: Executes tasks asynchronously to ensure smooth operation and responsiveness.

## Installation

1. **Clone the repository**:
    ```sh
    git clone https://github.com/yourusername/C2ImplantSrc.git
    cd C2ImplantSrc
    ```

2. **Open and Build the Project**:
   - Open the solution file `C2ImplantSrc.sln` in Visual Studio.
   - Build the project to create the implant executable.

3. **Modify Configuration**:
   - Update variables in `variables.h` to match your server’s configuration:
     - Set the URLs to point to your SimpleC2 server.
     - Adjust the Implant ID to match your setup.
