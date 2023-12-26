#include "Window.h"
#include "Application.h"
#include "GLFW/glfw3.h"
#include "Smedja/Events/ApplicationEvent.h"
#include "Smedja/Events/KeyEvent.h"
#include "Smedja/Events/MouseEvent.h"
#include "pch.h"

namespace Smedja {

static bool s_GLFWInitialized = false;

Window::Window() {
    init(WindowProps());
}

Window::Window(const WindowProps &props) {
    init(props);
}

Window::~Window() {
    shutdown();
}

// Initializes a GLFW window as per GLFW documentation
// Should support multiple window instances
void Window::init(const WindowProps &props) {
    m_Data.title = props.title;
    m_Data.width = props.width;
    m_Data.height = props.height;

    SD_CORE_INFO("Creating window {0} ({1}, {2})", props.title, props.width,
                 props.height);

    if (!s_GLFWInitialized) {
        int success = glfwInit();
        CORE_ASSERT(success, "Could not initialize GLFW!");

        // Set GLFW error callback which will report GLFW errors using spdlog
        glfwSetErrorCallback([](int error, const char *description) {
            SD_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
        });
        s_GLFWInitialized = true;
    }

    m_Window = glfwCreateWindow((int)props.width, (int)props.height,
                                m_Data.title.c_str(), nullptr, nullptr);

    if (m_Window == nullptr) {
        SD_CORE_ERROR("Failed to create GLFW window!");
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_Window);
    glfwSetWindowUserPointer(m_Window, &m_Data);
    setVSync(true);

    // Set GLFW callbacks
    glfwSetWindowSizeCallback(
        m_Window, [](GLFWwindow *window, int width, int height) {
            // Update window data
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
            data.width = width;
            data.height = height;

            WindowResizeEvent e(width, height);
            data.eventCallback(e);
        });

    glfwSetWindowCloseCallback(m_Window, [](GLFWwindow *window) {
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

            WindowCloseEvent e;
            data.eventCallback(e);
        });

    glfwSetKeyCallback(m_Window, [](GLFWwindow *window, int key, int scancode,
                                    int action, int mods) {
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

            if (action == GLFW_PRESS) {
                KeyPressedEvent e(key);
                data.eventCallback(e);
            } else if (action == GLFW_RELEASE) {
                KeyReleasedEvent e(key);
                data.eventCallback(e);
            }
        });

    glfwSetCursorPosCallback(
        m_Window, [](GLFWwindow *window, double xpos, double ypos) {
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

            MouseMovedEvent e(xpos, ypos);
            data.eventCallback(e);
        });

    glfwSetMouseButtonCallback(
        m_Window, [](GLFWwindow *window, int button, int action, int mods) {
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

            if (action == GLFW_PRESS) {
                MouseButtonPressedEvent e(button);
                data.eventCallback(e);
            } else { // Should only be GLFW_PRESS or GLFW_RELEASE
                MouseButtonReleasedEvent e(button);
                data.eventCallback(e);
            }
        });

    glfwSetScrollCallback(
        m_Window, [](GLFWwindow *window, double xoffset, double yoffset) {
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

            MouseScrolledEvent e(xoffset, yoffset);
            data.eventCallback(e);
        });
}

// Shutdown does not call GLFW terminate as it may be used by other windows
void Window::shutdown() {
    glfwDestroyWindow(m_Window);
}

void Window::onUpdate() {
    glfwPollEvents();
    glfwSwapBuffers(m_Window);
}

void Window::setVSync(bool enabled) {
    if (enabled) {
        glfwSwapInterval(1);
    } else {
        glfwSwapInterval(0);
    }
    m_Data.vSync = enabled;
}

bool Window::isVSync() const {
    return m_Data.vSync;
}

} // namespace Smedja
