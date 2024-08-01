#include "Input.h"

namespace Input {
    //Code taken from Hell2024 engine
    bool keyPressed[372];
    bool keyDown[372];
    bool keyDownLastFrame[372];

    double mouseX = 0;
    double mouseY = 0;
    double mouseOffsetX = 0;
    double mouseOffsetY = 0;

    bool leftMouseDown = false;
    bool rightMouseDown = false;
    bool leftMousePressed = false;
    bool rightMousePressed = false;
    bool leftMouseDownLastFrame = false;
    bool rightMouseDownLastFrame = false;

    GLFWwindow* window;

    bool Input::KeyPressed(char c) {
        return keyPressed[std::toupper(c)];
    }
    
    bool Input::KeyDown(char c) {
        return keyDown[std::toupper(c)];
    }
    
    bool Input::KeyDownLastFrame(char c) {
        return keyDownLastFrame[std::toupper(c)];
    }
    
    int Input::GetMouseX() {
        return (int)mouseX;
    }
    
    int Input::GetMouseY() {
        return (int)mouseY;
    }

    void Input::DisableCursor() {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    void Input::HideCursor() {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }

    void Input::ShowCursor() {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    void Input::CenterMouse() {
        glfwSetCursorPos(window, SCREENWIDTH / 2, SCREENHEIGHT / 2);
    }

    bool Input::LeftMouseDown() {
        return leftMouseDown;
    }
    
    bool Input::RightMouseDown() {
        return rightMouseDown;
    }
    
    bool LeftMousePressed() {
        return leftMousePressed;
    }
    
    bool RightMousePressed() {
        return rightMousePressed;
    }

    void Input::Init() {
        double x, y;
        window = Backend::GetWindowPointer();
        glfwGetCursorPos(window, &x, &y);
        mouseOffsetX = x;
        mouseOffsetY = y;
        mouseX = x;
        mouseY = y;
    }
    
    void Input::Update() {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            Backend::CloseWindow();
        }

        for (int i = 32; i < 349; i++) {
            // down
            if (glfwGetKey(window, i) == GLFW_PRESS)
            {
                keyDown[i] = true;
            }
                
            else
                keyDown[i] = false;

            // press
            if (keyDown[i] && !keyDownLastFrame[i])
                keyPressed[i] = true;
            else
                keyPressed[i] = false;
            keyDownLastFrame[i] = keyDown[i];
        }

        double x, y;
        glfwGetCursorPos(window, &x, &y);
        mouseOffsetX = x - mouseX;
        mouseOffsetY = y - mouseY;
        mouseX = x;
        mouseY = y;

        // Left mouse down/pressed
        leftMouseDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        if (leftMouseDown == GLFW_PRESS && !leftMouseDownLastFrame)
            leftMousePressed = true;
        else
            leftMousePressed = false;
        leftMouseDownLastFrame = leftMouseDown;

        // Right mouse down/pressed
        rightMouseDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
        if (rightMouseDown == GLFW_PRESS && !rightMouseDownLastFrame)
            rightMousePressed = true;
        else
            rightMousePressed = false;
        rightMouseDownLastFrame = rightMouseDown;
    }
}
