# C2ImplantSrc


## Overview

C2ImplantSrc is a repository containing the implant code for a Command and Control (C2) system. This code is designed to facilitate communication between the C2 server and the implanted devices. It is used for the implant code to communicate with SimpleC2.

## Features

- HTTP communication with CURL.
- WINAPI use to get system information and Ip information.
- Asynchronously executes tasks.

## Installation

To install the implant code, open the solution file `C2ImplantSrc.sln` in Visual Studio and build the project. Make sure to modify the variables in variables.h. Modify the URLs to match your server URL and modify the ID to match your Implant ID.

```sh
git clone https://github.com/yourusername/C2ImplantSrc.git
cd C2ImplantSrc
```

## License

This project is licensed under the MIT License. See the `LICENSE` file for more details.

## Contact

For any questions or issues, please contanct k499wang@uwaterloo.ca
