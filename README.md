# C2ImplantSrc

## Overview

**C2ImplantSrc** is a repository containing the implant code for a Command and Control (C2) system. This code facilitates communication between the C2 server (SimpleC2) and the implanted devices. It is designed to enable efficient command execution and data transmission between the server and the clients.

## Features

- **HTTP Communication**: Utilizes CURL for seamless HTTP communication with the server.
- **System Information Retrieval**: Uses WINAPI to gather system and IP information from the implanted devices.
- **Asynchronous Task Execution**: Supports the asynchronous execution of tasks for improved performance and responsiveness.

## Installation

To install the implant code, follow these steps:

1. **Clone the repository**:
    ```sh
    git clone https://github.com/yourusername/C2ImplantSrc.git
    ```

2. **Navigate to the project directory**:
    ```sh
    cd C2ImplantSrc
    ```

3. **Open the solution file**: 
   - Launch **Visual Studio** and open the solution file `C2ImplantSrc.sln`.

4. **Modify Configuration**:
   - Open `variables.h` and update the following variables:
     - **Server URL**: Change the URL to match your C2 server.
     - **Implant ID**: Update the ID to match your implant.

5. **Build the project**: 
   - Compile the project using Visual Studio to create the implant executable.

## Usage

Once the implant is built, you can deploy it on the target devices. Ensure that the implanted devices have access to the C2 server to facilitate communication.

### Example Commands

- To send a command from the server, the implant will listen for incoming HTTP requests and respond accordingly based on the command received.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Contact

For any inquiries or issues, please contact [k499wang@uwaterloo.ca](mailto:k499wang@uwaterloo.ca).

## Further Reading

- [CURL Documentation](https://curl.se/docs/)
- [WINAPI Documentation](https://learn.microsoft.com/en-us/windows/win32/api/)
- [Visual Studio Documentation](https://docs.microsoft.com/en-us/visualstudio/)

