#include "audio.h"

Music::Music(const char* assetPath)
{
	_music = Mix_LoadMUS(assetPath);

	if (!_music)
		throw std::exception(Mix_GetError());
}

Music::~Music()
{
	Mix_FreeMusic(_music);
}

void Music::fadeIn()
{
	Mix_VolumeMusic(20);
	if(Mix_FadeInMusic(_music, -1, 3000) == -1)
		throw std::exception("Mix_FadeInMusic: The file could not played.");
}

void Music::play()
{
	Mix_VolumeMusic(20);
	if (Mix_PlayMusic(_music, -1) == -1)
		throw std::exception("Mix_PlayMusic: The file could not played.");
}

Sound::Sound(const char* assetPath, int channel)
{
	_chunk = Mix_LoadWAV(assetPath);
	_channel = channel;

	if (!_chunk)
		throw std::exception("Mix_LoadWAV: The .wav file could not be loaded.");
}

Sound::~Sound()
{
	Mix_FreeChunk(_chunk);
}

void Sound::play()
{
	Mix_Volume(_channel, 30);
	if(Mix_PlayChannel(_channel, _chunk, 0) == -1)
		throw std::exception("Mix_PlayChannel: The .wav file could not played.");
}

std::map<std::string, Music*> Audio::_music;
std::map<std::string, Sound*> Audio::_sounds;

void Audio::load()
{
	for (auto const& l : Data::getAudio())
	{
		auto parts = Strings::split(l, ':');
		auto audioName = parts[0];
		parts = Strings::split(parts[1], ',');

		if (parts[0].compare("music") == 0)
			_music[audioName] = new Music(parts[1].c_str());
		else
			_sounds[audioName] = new Sound(parts[1].c_str(), 1);

		printf(("   + '" + audioName + "' (" + parts[1] + ")\n").c_str());
	}
}

void Audio::playMusic(std::string musicName)
{
	_music[musicName]->play();
}

void Audio::playSound(std::string soundName)
{
	_sounds[soundName]->play();
}

void Audio::switchMusic(std::string newMusicName)
{
	while (!Mix_FadeOutMusic(3000) && Mix_PlayingMusic())
		SDL_Delay(100);

	_music[newMusicName]->fadeIn();
}