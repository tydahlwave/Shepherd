//
//  WindowCallbackDelegate.h
//  SimpleGame
//
//  Created by Tyler Dahl on 1/23/17.
//
//

#ifndef WindowCallbackDelegate_h
#define WindowCallbackDelegate_h

class WindowCallbackDelegate {
public:
    virtual void KeyPressed(World *world, int windowWidth, int windowHeight, int key, int action) = 0;
    virtual void MouseMoved(World *world, int windowWidth, int windowHeight, double mouseX, double mouseY) = 0;
};

#endif /* WindowCallbackDelegate_h */
