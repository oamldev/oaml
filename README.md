[![Build Status](https://travis-ci.org/marcelofg55/oaml.svg)](https://travis-ci.org/marcelofg55/oaml.svg?branch=master)
[![License](http://img.shields.io/:license-mit-blue.svg)](http://doge.mit-license.org)

### Open Adaptive Music Library 

OAML is a library the makes it easy to implement adaptive music in games.


### First, what is adaptive music on video games?

Adaptive music, also known as interactive music or dynamic music, is music that reacts to what's happening on the video game. For example, if there is a battle going on, music will get more tense to stimulate the player's reaction to the battle. There is a nice paper on the subject written by David Vink that can be found on gamecareerguide ([link](http://www.gamecareerguide.com/features/768/student_thesis_adaptive_music_for_.php?print=1)).


### How does it works?

OAML uses short music loops to make music less linear and more interactive with the player.

In OAML there are three basic types of short loops:

- Intro loops, played when the track starts.
- Main loops, this is what OAML will keep playing when there is no condition triggered (actually these main loops can have one condition that is OAML_CONDID_MAIN_LOOP, check the msnake_oaml demo for more info on how it works).
- Conditional loops, these loops are defined by a condition, based on the data provided for the condition (id, type and values) OAML will play them when these conditions meet, for example, when enemies are engaged a condition can be triggered and music will play loops that get more intense.

Note that a track is only required to have one main loop at least, all the other loops are optional.


### Supported audio formats

Supports the following audio formats:
- ogg
- wav
- aif


### Supported game engines
- Unity3d
- Godot: [oamlGodotModule](https://github.com/oamldev/oamlGodotModule)


### Demos

Demos page: [https://oamldev.github.io/demos/](https://oamldev.github.io/demos/)


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

To create the oaml.defs file check [oamlStudio](https://github.com/oamldev/oamlStudio).


### Exporting music for OAML

When exporting music from your DAW to use with OAML the key to make the loops work seamlessly is to **enable the tail on export**.
For now since resampling is not implemented in OAML you need to export the music in the same sample rate that your project is going to use, for example if your project uses 44100hz export all your music in 44100hz as well.


### Notes

- If you're using OAML on your project or you're interested to do so and need some help contact me.
- Got any question? Write me an email to <marcelo@marcelofernandezmusic.com>.


### TODO

- Make the possibility that tension will not simply change to a condition loop but instead that both loops (main loop and conditional loop) will play together based on the tension percent, need to test it first.
- Resampling music?
- Add a function for playing SFX's with a 3d position.
- Add a loudness effect, and a reverb effect as well.
- Implement OAML in more game engines, love2d, etc.


### Games using OAML

- [https://github.com/Andrettin/Wyrmgus](https://github.com/Andrettin/Wyrmgus)


### About

Copyright (c) 2015-2016 Marcelo Fernandez <marcelo@marcelofernandezmusic.com>
