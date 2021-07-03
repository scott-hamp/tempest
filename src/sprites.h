#pragma once

#ifndef Sprite_Included
#define Sprite_Included

#include <exception>
#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <vector>
#include "geometry.h"

class Sprite
{
	private:
		double _angle;
		SDL_RendererFlip _flip;
		SDL_Point _origin;
		SDL_Rect _rect;
		Scale2D _scale;
		SDL_Texture* _texture;
	public:
		Sprite(SDL_Renderer*, const char*);
		~Sprite();
		SDL_Rect getRectScaled(double);
		SDL_Rect getRectScaled(Scale2D);
		SDL_Rect rect();
		SDL_Rect rectScaled();
		void render(SDL_Renderer*, SDL_Point);
		void render(SDL_Renderer*, SDL_Point, Scale2D);
		void render(SDL_Renderer*, SDL_Point, Scale2D, double);
		void render(SDL_Renderer*, SDL_Point, double);
		void render(SDL_Renderer*, SDL_Point, SDL_RendererFlip);
		Scale2D scale();
		void setAngle(double);
		void setColor(SDL_Color);
		void setFlip(SDL_RendererFlip);
		void setOpacity(double);
		void setOrigin(SDL_Point);
		void setOrigin(double, double);
		void setScale(double);
		void setScale(Scale2D);
		void setScaleBy(double);
		void setScaleBy(double, double);
};

class SpriteAnimation
{
	private:
		double _angle;
		double _delay;
		double _delayTimer;
		SDL_RendererFlip _flip;
		SDL_Point _origin;
		SDL_Rect _rect;
		Scale2D _scale;
		int _spriteIndex;
		std::vector<Sprite*> _sprites;
	public:
		SpriteAnimation(SDL_Renderer*, const char*, double);
		~SpriteAnimation();
		SDL_Rect rect(int);
		SDL_Rect rectScaled(int);
		void render(SDL_Renderer*, SDL_Point);
		void setScale(double);
		void setScale(Scale2D);
		void update(double);
};

#endif