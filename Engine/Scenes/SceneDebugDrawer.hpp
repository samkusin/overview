//
//  SceneDebugDrawer.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/6/15.
//
//

#ifndef Overview_SceneDebugDrawer_hpp
#define Overview_SceneDebugDrawer_hpp

#include "CKGfx/NodeRenderer.hpp"

#include <bullet/LinearMath/btIDebugDraw.h>

#include <vector>

namespace cinek {
    namespace ove {
    
class SceneDebugDrawer : public btIDebugDraw
{
    DefaultColors _ourColors;
    int _debugMode;
    
    std::vector<std::pair<btVector3, btVector3>> _lineBuffer;
    btVector3 _currentLineColor;
    
    const cinek::gfx::NodeRenderer::ProgramMap* _programs;
    const cinek::gfx::NodeRenderer::UniformMap* _uniforms;
    const cinek::gfx::Camera* _camera;
    
    static constexpr int kLineBufferSize = 256;
    
public:
    SceneDebugDrawer();
    
    virtual ~SceneDebugDrawer();
    
    virtual DefaultColors getDefaultColors() const {
		return _ourColors;
	}
	
    virtual void setDefaultColors(const DefaultColors& colors) {
		_ourColors = colors;
	}


	virtual void drawLine
    (
        const btVector3& from1,
        const btVector3& to1,
        const btVector3& color1
    );

	virtual void drawContactPoint
    (
        const btVector3& PointOnB,
        const btVector3& normalOnB,
        btScalar distance,
        int lifeTime,
        const btVector3& color
    );
     

	virtual void reportErrorWarning(const char* warningString);

	virtual void draw3dText(const btVector3& location,const char* textString);

	virtual void setDebugMode(int debugMode) {
        _debugMode = debugMode;
    }

	virtual int	getDebugMode() const {
		return _debugMode;
	}

    virtual void flushLines();
    
    void setup
    (
        const cinek::gfx::NodeRenderer::ProgramMap& programs,
        const cinek::gfx::NodeRenderer::UniformMap& uniforms,
        const cinek::gfx::Camera& camera
    );
};
    
    } /* namespace ove */
} /* namespace cinek */


#endif /* SceneDebugDrawer_hpp */
