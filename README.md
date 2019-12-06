# `I Lang`

## How to set up workspace to develop:

* #### Clone this repository
    Using ssh
    `git clone git@gitlab.com:teamplate-teamname-t/i-lang.git`  
    or via https
    `git clone https://gitlab.com/teamplate-teamname-t/i-lang.git`
    
* #### Download CLion 
    It will help us with its built-in and flexible code quality management.  
    [Jetbrains download link](https://www.jetbrains.com/clion/download)

* #### Create project
    Select `File` → `New CMake project from sources`.  
    Import this repository as a project directory.

* #### Set CMake build profiles
    Go to `Settings` → `Build, Execution and Deployment` → `CMake`.  
    Available profiles:
    * Debug - for building and running with debug-friendly flags
    * Release - for normal, optimized configuration
    * Test - for unit tests 

* #### Set up code style
    Go to `Settings` → `Editor` → `Code Style`.  
    Select Code Style and turn on `Enable ClangFormat with clangd server`.
    
* #### Set up code analysis
    Go to `Settings` → `Editor` → `Inspections`.  
    Unfold `C/C++` → `General` and make sure that: 
    * Clang-Tidy is enabled
    * `Use IDE settings` is unmarked 
    
 * #### You are ready now!
