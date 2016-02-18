### Open Adaptive Music Library

OAML is a library the makes it easy to implement adaptive music in games.


### How does it works?

OAML uses short music loops to make music less linear and more interactive with the player.
It's usual in games that music is only used as a background and that it doesn't react to what is happening in the game, with OAML this changes and music can react based on what's going on in the game.

In OAML there are four basic types of short loops:
- Intro loops, played when the track starts.
- Main loops, this is what OAML will keep playing when there is no condition triggered.
- Conditional loops, these loops are defined by a condition, based on the data provided for the condition (id, type and values) OAML will play them when these conditions meet, for example, when enemies are engaged a condition can be triggered and music will play loops that get more intense.
- Ending loops, played when the track ends.

Note that a track is only required to have one main loop at least, all the other loops are optional.


### Features

Supports the following audio formats: **ogg, wav, aif**.


### Demos

Folder 'demos' contains so far:

- msnake:
Based on Mogria's Snake [https://github.com/mogria/msnake.git](https://github.com/mogria/msnake.git).
This is a ncurses based snake game that I've adapted to use OAML, music in this demo gets faster as the snake grows. The music is a simple 8bit theme that starts with 80bpm and goes up to 140bpm.


### How to compile

On Linux and OSX:
```
	mkdir build
	cd build
	cmake ..
	make
	sudo make install
```

On Windows with Visual Studio check the folder 'vs'.


### Usage

Example of basic usage in C++:

```C++
{
	// Defined somewhere in your code
	oamlApi *oaml;

	// Initialization, oaml.defs is the definitions saved/export with oamlStudio
	oaml = new oamlApi();
	oaml->Init("oaml.defs");

	// Play a track
	oaml->PlayTrack("track1");

	// Pause music
	oaml->Pause();

	// Resume music
	oaml->Resume();

	// Stop music
	oaml->StopPlaying();

	// Shutdown OAML
	oaml->Shutdown();
	delete oaml;
}
```

To create the oaml.defs file check [oamlStudio](https://github.com/marcelofg55/oamlStudio).


### Exporting music for OAML

When exporting music from your DAW to use with OAML the key to make the loops work seeamlessly is to **enable the tail on export**.


### About

Copyright (c) 2015-2016 Marcelo Fernandez
