#ifndef PathRenderer_h
#define PathRenderer_h

#include "Component.h"
#include "../Path.h"

class PathRenderer : public Component {
public:
    PathRenderer() { name = "PathRenderer"; };
    PathRenderer(Path *p) :PathRenderer() { path = p; }
    virtual ~PathRenderer() {};
    
    Path *path = nullptr;
};

#endif /* PathRenderer_h */
