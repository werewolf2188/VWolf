#pragma once

static bool CheckForGLErrors(const char* file, const char* code, int line) {
    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
        
        switch (err) {
            case GL_INVALID_ENUM:
                //An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag.
                VWOLF_CORE_ERROR("OpenGL INVALID ENUM Error: File %s. Line: %d. Code: %s", file, line, code);
                VWOLF_CORE_ASSERT(false);
                break;
            case GL_INVALID_VALUE:
                //A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag.
                VWOLF_CORE_ERROR("OpenGL INVALID VALUE Error: File %s. Line: %d. Code: %s", file, line, code);
                VWOLF_CORE_ASSERT(false);
                break;
            case GL_INVALID_OPERATION:
                //The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag.
                VWOLF_CORE_ERROR("OpenGL INVALID OPERATION Error: File %s. Line: %d. Code: %s", file, line, code);
#ifndef VWOLF_PLATFORM_WINDOWS
                VWOLF_CORE_ASSERT(false); // TODO: For now I'm going to not allow this in windows, since I'm not paying attention on cleaning the memory correctly.
#endif
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                //The framebuffer object is not complete. The offending command is ignored and has no other side effect than to set the error flag.
                VWOLF_CORE_ERROR("OpenGL INVALID FRAME BUFFER OPERATION Error: File %s. Line: %d. Code: %s", file, line, code);
                VWOLF_CORE_ASSERT(false);
                break;
            case GL_OUT_OF_MEMORY:
                //There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded.
                VWOLF_CORE_ERROR("OpenGL OUT OF MEMORY Error: File %s. Line: %d. Code: %s", file, line, code);
                VWOLF_CORE_ASSERT(false);
                break;
            case GL_STACK_UNDERFLOW:
                //An attempt has been made to perform an operation that would cause an internal stack to underflow.
                VWOLF_CORE_ERROR("OpenGL STACK UNDERFLOW Error: File %s. Line: %d. Code: %s", file, line, code);
                VWOLF_CORE_ASSERT(false);
                break;
            case GL_STACK_OVERFLOW:
                //An attempt has been made to perform an operation that would cause an internal stack to overflow.
                VWOLF_CORE_ERROR("OpenGL STACK OVERFLOW Error: File %s. Line: %d. Code: %s", file, line, code);
                VWOLF_CORE_ASSERT(false);
                break;
        }
        return true;
    }
    return false;
}

static void GLFWErrorCallback(int error, const char* description)
{
    VWOLF_CORE_ERROR("GLFW Error (%d): %s", error, description);
}

static void OpenGLMessageCallback(
    unsigned source,
    unsigned type,
    unsigned id,
    unsigned severity,
    int length,
    const char* message,
    const void* userParam)
{
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:         VWOLF_CORE_FATAL(message); return;
    case GL_DEBUG_SEVERITY_MEDIUM:       VWOLF_CORE_ERROR(message); return;
    case GL_DEBUG_SEVERITY_LOW:          VWOLF_CORE_WARNING(message); return;
    case GL_DEBUG_SEVERITY_NOTIFICATION: VWOLF_CORE_TRACE(message); return;
    }

    VWOLF_CORE_ASSERT(false, "Unknown severity level!");
}

#ifndef GLThrowIfFailed
#define GLThrowIfFailedNoAction(x) \
CheckForGLErrors(__FILE__, x, __LINE__);
#endif

#ifndef GLThrowIfFailed
#define GLThrowIfFailed(x) \
(x); \
CheckForGLErrors(__FILE__, #x, __LINE__);
#endif
