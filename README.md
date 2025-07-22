# MotionStacker

MotionStacker is a simple and lightweight tool for visualizing and testing stack sprite animations. It allows you to load a spritesheet, configure its frame layout, and preview the animation with various effects. It's built with C++ and the raylib library.

## Features

*   **Drag and Drop:** Easily load your sprite sheets by dragging them into the application window.
*   **Spritesheet Configuration:** Set the number of horizontal and vertical frames in your spritesheet.
*   **Animation Preview:** Play and stop the animation.
*   **Frame Stacking:** Renders all horizontal frames stacked vertically, which is useful for motion effects.
*   **Adjustable Speed:** Control the duration of each frame.
*   **Rotation:** Apply a continuous rotation to the stacked sprites.
*   **Pixelizer Effect:** A simple shader to pixelate the output.
*   **Customizable UI:** Change background color and hide the UI for an unobstructed view.

## Technologies Used

*   **C++:** The core application logic is written in C++.
*   **raylib:** For creating the window, handling graphics, and user input.
*   **raygui:** For the immediate mode graphical user interface.
*   **CMake:** For building the project.

## Project Structure

```
.
├── CMakeLists.txt
├── assets
│   └── Ubuntu-Regular.ttf
├── libs
│   ├── raygui
│   └── raylib
└── src
    ├── font_data.h
    ├── main.cpp
    └── pixel_shader.h
```

## Getting Started

### Prerequisites

*   A C++ compiler (GCC, Clang, MSVC)
*   CMake (version 3.25 or higher)
*   Git

### Building

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/your-username/MotionStacker.git
    cd MotionStacker
    ```

2.  **Initialize submodules:**
    The `raylib` and `raygui` libraries are included as submodules.
    ```bash
    git submodule update --init --recursive
    ```

3.  **Configure and build with CMake:**
    ```bash
    cmake -B build
    cmake --build build
    ```

4.  **Run the application:**
    The executable will be located in the `build` directory.
    ```bash
    ./build/MotionStaker
    ```
    On Windows, it will be:
    ```powershell
    .\build\Release\MotionStaker.exe
    ```

## How to Use

1.  Launch the application.
2.  Drag and drop your sprite sheet file (e.g., `.png`) into the window.
3.  The configuration panel will appear. Set the number of horizontal (`H-Frames`) and vertical (`V-Frames`) frames your sprite sheet contains.
4.  Click "Confirm".
5.  Use the preview panel to play/stop the animation, adjust frame duration, and toggle effects like rotation and pixelization.
