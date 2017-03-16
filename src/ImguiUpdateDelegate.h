//
//  ImguiUpdateDelegate.h
//  Shepherd
//
//  Created by Tyler Dahl on 3/4/17.
//
//

#ifndef ImguiUpdateDelegate_h
#define ImguiUpdateDelegate_h

class World;

class ImguiUpdateDelegate {
public:
    virtual void ImguiUpdate(World *world, bool drawGUI) = 0;
};

#endif /* ImguiUpdateDelegate_h */
