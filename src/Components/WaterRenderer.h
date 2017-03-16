//
//  WaterRenderer.h
//  Shepherd
//
//  Created by Sharon Che on 2/12/17.
//
//

#ifndef WaterRenderer_h
#define WaterRenderer_h

#include "Component.h"
#include "../Material.h"
#include "../Shader.h"
#include "../Texture.h"
#include "../WaterTile.h"
#include "../WaterFrameBuffers.h"

class WaterRenderer : public Component {
public:
	WaterRenderer() { name = "WaterRenderer"; };
	WaterRenderer(WaterTile *wt, Model *m, Shader *s, Texture *t, Texture *n, Texture *d, Material *mat, WaterFrameBuffers *b) :
		WaterRenderer() { waterTile = wt; model = m; shader = s; texture = t; normal = n; dudv = d; material = mat; buffers = b; }
	virtual ~WaterRenderer() {};

	WaterTile *waterTile = nullptr;
	Model *model = nullptr;
	Shader *shader = nullptr;
	Texture *texture = nullptr;
	Texture *normal = nullptr;
	Texture *dudv = nullptr;
	Material *material = nullptr;
	WaterFrameBuffers *buffers = nullptr;
};

#endif
