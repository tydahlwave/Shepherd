//
//  Logger.h
//  Shepherd
//
//  Created by Tyler Dahl on 3/10/17.
//
//

#ifndef Logger_h
#define Logger_h

#include <stdio.h>
#include <string>

enum LogLevel {
    INFO, ERROR
};

static void Log(LogLevel level, std::string message) {
    std::fprintf(stderr, "%s\n", message.c_str());
}

static void Log(LogLevel level, std::string message, std::string file, int line) {
    std::string revisedMessage = message + " File: %s. Line %d.\n";
    std::fprintf(stderr, revisedMessage.c_str(), file.c_str(), line);
}

static void Log(LogLevel level, char *message, std::string file, int line) {
    Log(level, std::string(message), file, line);
}

static void LogGLError(std::string file, int line) {
    GLenum error = glGetError();
    
    if (error != GL_NO_ERROR) {
        Log(ERROR, (char *)glewGetErrorString(error), file, line);
    }
}

#endif /* Logger_h */
