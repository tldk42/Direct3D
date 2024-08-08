#pragma once

enum class ERenderingLayer : uint8_t
{
	GameObject,
	UI,
	GUI,
	Background_2D,
	Foreground_2D,
	Player_2D,
	Particle_2D,
};

class IRenderable
{
public:
	virtual void PreRender() = 0;
	virtual void Render() = 0;
	virtual void PostRender() = 0;

	virtual ERenderingLayer GetLayerType() = 0;
};
