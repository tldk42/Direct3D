#pragma once

enum class ELayerType : uint32_t
{
	GameObject = 0,
	UI,
	GUI,
	Background_2D,
	Foreground_2D,
	Player_2D,
	Particle_2D,
	End
};

class IRenderable
{
public:
	virtual void PreRender() = 0;
	virtual void Render() = 0;
	virtual void PostRender() = 0;
	
	virtual ELayerType GetLayerType() = 0;
};
