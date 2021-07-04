#pragma once

#ifndef Audio_H
#define Audio_H

#include <map>
#include <SDL.h>
#include <SDL_mixer.h>
#include <stdlib.h>
#include <string>
#include <tuple>
#include <vector>
#include "data.h"

class Music
{
	private:
		Mix_Music* _music;
	public:
		Music(const char*);
		~Music();
		void fadeIn();
		void play();
};

class Sound
{
	private:
		int _channel;
		Mix_Chunk* _chunk;
	public:
		Sound(const char*, int);
		~Sound();
		void play();
};

class Audio
{
	private:
		static std::map<std::string, Music*> _music;
		static std::map<std::string, Sound*> _sounds;
	public:
		static void load();
		static void playMusic(std::string);
		static void playSound(std::string);
		static void switchMusic(std::string);
};

#endif