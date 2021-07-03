#include "sprites.h"

Sprite::Sprite(SDL_Renderer* renderer, const char* assetPath)
{
	SDL_Surface* surface = IMG_Load(assetPath);
	if (!surface)
	{
		auto em = "IMG_Load: " + (std::string)SDL_GetError();

		SDL_Log(assetPath);

		throw std::exception(em.c_str());
	}

	_texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	int w, h;
	SDL_QueryTexture(_texture, NULL, NULL, &w, &h);

	_rect.x = 0;
	_rect.y = 0;
	_rect.w = w;
	_rect.h = h;

	_angle = 0;
	_flip = SDL_FLIP_NONE;
	_origin = { _rect.w / 2, _rect.h / 2 };
	_scale = { 1.0, 1.0 };
}

Sprite::~Sprite()
{
	SDL_DestroyTexture(_texture);
}

SDL_Rect Sprite::getRectScaled(double s)
{
	SDL_Rect rs;
	rs.x = 0;
	rs.y = 0;
	rs.w = _rect.w * s;
	rs.h = _rect.h * s;

	return rs;
}

SDL_Rect Sprite::getRectScaled(Scale2D s)
{
	SDL_Rect rs;
	rs.x = 0;
	rs.y = 0;
	rs.w = _rect.w * s.X;
	rs.h = _rect.h * s.Y;

	return rs;
}

SDL_Rect Sprite::rectScaled()
{
	SDL_Rect rs;
	rs.x = 0;
	rs.y = 0;
	rs.w = _rect.w * _scale.X;
	rs.h = _rect.h * _scale.Y;

	return rs;
}

SDL_Rect Sprite::rect()
{
	return _rect;
}

void Sprite::render(SDL_Renderer* renderer, SDL_Point point)
{
	SDL_Rect drect;
	drect.x = point.x - (_origin.x * _scale.X);
	drect.y = point.y - (_origin.y * _scale.Y);
	drect.w = _rect.w * _scale.X;
	drect.h = _rect.h * _scale.Y;
	const SDL_Point org = { _origin.x * _scale.X, _origin.y * _scale.Y };

	SDL_RenderCopyEx(renderer, _texture, &_rect, &drect, _angle, &org, _flip);
}

void Sprite::render(SDL_Renderer* renderer, SDL_Point point, Scale2D scale)
{
	Scale2D os = { _scale.X, _scale.Y };

	setScale(scale);
	render(renderer, point);
	setScale(os);
}

void Sprite::render(SDL_Renderer* renderer, SDL_Point point, Scale2D scale, double angle)
{
	Scale2D os = { _scale.X, _scale.Y };
	double oa = _angle;

	setScale(scale);
	setAngle(angle);
	render(renderer, point);
	setScale(os);
	setAngle(oa);
}

void Sprite::render(SDL_Renderer* renderer, SDL_Point point, double angle)
{
	double oa = _angle;

	setAngle(angle);
	render(renderer, point);
	setAngle(oa);
}

void Sprite::render(SDL_Renderer* renderer, SDL_Point point, SDL_RendererFlip flip)
{
	SDL_RendererFlip of = _flip;

	setFlip(flip);
	render(renderer, point);
	setFlip(of);
}

Scale2D Sprite::scale()
{
	return _scale;
}

void Sprite::setAngle(double a)
{
	_angle = std::min(std::max(a, 0.0), 359.0);
}

void Sprite::setColor(SDL_Color c)
{
	SDL_SetTextureColorMod(_texture, (Uint8)c.r, (Uint8)c.g, (Uint8)c.b);
}

void Sprite::setFlip(SDL_RendererFlip f)
{
	_flip = f;
}

void Sprite::setOpacity(double o)
{
	SDL_SetTextureAlphaMod(_texture, (Uint8)(o * 255.0));
}

void Sprite::setOrigin(SDL_Point o)
{
	_origin = o;
}

void Sprite::setOrigin(double xper, double yper)
{
	_origin = { (int)(_rect.w * xper), (int)(_rect.h * yper) };
}

void Sprite::setScale(double s)
{
	_scale = { s, s };
}

void Sprite::setScale(Scale2D s)
{
	_scale = s;
}

void Sprite::setScaleBy(double longest)
{
	while(rectScaled().w > longest || rectScaled().h > longest)
		setScale({ _scale.X * 0.99, _scale.Y * 0.99 });
}

void Sprite::setScaleBy(double shortest, double longest)
{
	if (shortest > longest) throw "Invalid argument.";

	_scale = { 1.0, 1.0 };

	if (_rect.w >= longest || _rect.h >= longest)
	{
		while (rectScaled().w > longest || rectScaled().h > longest)
			setScale({ _scale.X * 0.99, _scale.Y * 0.99 });
	}
	else
	{
		while (rectScaled().w < shortest || rectScaled().h < shortest)
			setScale({ _scale.X * 1.01, _scale.Y * 1.01 });
	}
}

SpriteAnimation::SpriteAnimation(SDL_Renderer* renderer, const char* path, double delay)
{
	std::string pathstr = path;
	int ind = -1;

	for (int i = 0; i < pathstr.length(); i++)
	{
		if (path[i] != '*') continue;

		ind = i;
		break;
	}

	if (ind == -1) throw "Path is invalid.";

	std::string ls = pathstr.substr(0, ind);
	std::string rs = pathstr.substr(size_t(ind + 1));

	int frame = 1;
	while (true)
	{
		auto ps = ls + std::to_string(frame) + rs;

		try
		{
			_sprites.push_back(new Sprite(renderer, ps.c_str()));

			auto lm = "  -- Frame #" + std::to_string(frame);
			SDL_Log(lm.c_str());

			frame++;
		}
		catch (std::exception e) { break; }
	}

	_delay = delay;
	_delayTimer = _delay;
}

SpriteAnimation::~SpriteAnimation()
{

}

SDL_Rect SpriteAnimation::rect(int index)
{
	return _sprites[index]->rect();
}

SDL_Rect SpriteAnimation::rectScaled(int index)
{
	return _sprites[index]->rectScaled();
}

void SpriteAnimation::render(SDL_Renderer* renderer, SDL_Point p)
{
	_sprites[_spriteIndex]->render(renderer, p);
}

void SpriteAnimation::setScale(double s)
{
	for (auto spr : _sprites) spr->setScale(s);
}

void SpriteAnimation::setScale(Scale2D s)
{
	for (auto spr : _sprites) spr->setScale(s);
}

void SpriteAnimation::update(double delta)
{
	_delayTimer -= delta;
	if (_delayTimer > 0.0) return;

	_delayTimer = _delay;
	_spriteIndex++;

	if (_spriteIndex >= _sprites.size())
		_spriteIndex = 0;
}
