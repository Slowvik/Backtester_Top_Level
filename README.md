An order management system written in C++ with the following features:

1. It works only with top-level market data, which is available from a historical market data file
2. Supports strategy testing, user can submit orders and get fills (Project includes a simple strategy)


USAGE REQUIREMENTS: 
1. Market Data files used for this are available at https://www.dropbox.com/scl/fo/tyyezv4ihffu953faaxmh/h?dl=0&rlkey=rqk19wb4413lemjjw2od1jecv. They need to be downloaded and put in the folder "Input Files"
2. A C++ compiler (I used g++)

For Windows: 
1. In cmd, type: git clone https://github.com/Slowvik/Backtester_Top_Level.git
2. Navigate to the folder in cmd and compile using: g++ *.cpp -o Backtester.exe (assuming g++ compiler is installed, can change to clang or whatever else is installed. Can change the name of the executable and add more compilation options as desired)
3. Alternatively, after downloading, open the folder Backtester_Top_Level in VSCode. Configure tasks.json and modify ${file} in args to ${workspaceFolder}/*.cpp. Then debug and run.
4. CMake file for Windows coming soon

For Linux:
1. CMake file for Linux Coming soon

TODO:

1. Upgrade the entire code to meet C++17 standards
2. Optimise the code to meet low-latency standards
3. Write comprehensive unit tests for all modules
4. Compile and run on WSL (and create a CMake for Linux)
5. Create a CMake for Windows
6. Deploy a more realistic strategy

