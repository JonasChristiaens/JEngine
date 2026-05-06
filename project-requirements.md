# Project requirements

This file contains the project requirements, and the game specific requirements for what I want to make/add to the version of the game I am making.



For the exam project you implement one out of ten games, for which you'll need to build a game engine. This page describes the requirements for your engine and game regardless of which game you implement.



### Common project requirements

#### Game Requirements

Regardless of which game you were assigned, these are common features that your game must have:



* At least three levels (or stages, rounds), feel free to add more. Check the rules of your game to see which features these three levels need (if applicable).
* A player has 3 extra lives, so 4 in total (the one he started with + 3 more chances
* There must be a single player, a co-op and a versus mode
* Music \& sound effects must play. F2 is a button to mute the sound (and, against all expectations, it also unmutes the sound, but only if it was muted)
* F1 is a button to skip levels
* The single player mode must be completely playable with both keyboard and gamepad.

  * This means that I can choose to play the game with the keyboard and everything works, even start menu, high score view, etc.
  * This also means that I can choose to play the game with the gamepad and everything works, even start menu, high score view, etc.
* The two player modes are always playable with one player on the keyboard and the other with a gamepad. And it is always possible to play with two gamepads as well.
* When the game is over display the player's score and a highscore list. This highscore list must be permanent, meaning the next time we play the game we see the highscores we made before. This means you'll have to save these somewhere. Remember: player's find it important to see who achieved a certain highscore.

  * Hint: games in arcade halls do not have keyboards, so how do you enter a highscore in these games?



#### Engine requirements

* Use the minigin project Links to an external site. as a start point for your game engine. This is a github project that you can use as a template for your own. We recommend using that since it will automatically have github actions setup that verify your build.
* There are at least two projects in your visual studio solution: the game engine and the game. The engine is a static and/or dynamic library and the game is an executable.
* Level layout/setup is read from a file, you decide which format. 
* Make sure the project builds for all build targets at warning level 4 and warnings flagged as errors.
* Apply all best practices we’ve seen during the course.
* Some functionality in your game or engine must make use of threading. 
* Apply software design patterns and game programming patterns as you see fit, but we expect to see at least Game Loop, Update method, Command, Observer and/or EventQueue, Component and State.
* You must use a public git repo. Ideally this is a repo that used the minigin repo as a template. Make sure you use it from the start, so we are able to see all your commits and progress. Only pushing your code in one go at the end of the deadline is regarded as "didn't use git".
* Use the latest available stable version of visual studio.



#### Recommendations

* Have simple visuals - we don't care about the looks of your game, it's only the code that counts. It must however function properly of course.
* Consider using a text format for your levels (like json or xml) while debugging. For "release" a binary format could be a better option.
* Play fair – don’t copy code from your colleagues or the internet without credits. Submissions will be run through plagiarism detection software.



#### Deliverables

Submit a zip file named 2DAEXX\_lastname\_firstname\_exam.zip containing



* The source code of your project including one visual studio solution and project files, or a properly configured cmake project.
* All available build targets must compile and deliver a working executable.
* An x64 Release build
* A readme.md containing

  * Some specifics about your engine and the design choices you made
  * A link to the source control depot you used for your project





### Bomberman Specific information

#### Game requirements

* The player can walk around in a labyrinth as Bomberman. You need to find the exit that is hidden behind walls/barrels/crates/…



##### Enemies

There are eight different enemies in the underground plant (but we'll only implement 4), each one a little different from the others. They move at a variety of speeds, with the fast monsters capable of overtaking Bomberman at full speed. They have varying degrees of intelligence. Some wander aimlessly about the stage, while others hone in on you and stay on your trail when you try to get away.



Balloom, Oneal, Doll, Minvo



* As a player you must accomplish two things: You must destroy every enemy, and you must reveal the exit which can only be used once every enemy is defeated.
* In the level there can also be several power-ups hidden behind the same obstructions.
* Bomberman begins the game with ability to produce one bomb at a time with an explosion range of one square. Bombs detonate on their own a couple of seconds after being dropped.
* The touch of an enemy, and being caught in an explosion are both lethal to Bomberman.
* Bomberman's bombs can also detonate one another in chain reactions.
* There must be three types of pickups:

  * Extra bomb: Bomberman can now place an extra bomb at the same time.
  * Detonator: The player can detonate the oldest bomb by pressing a button
  * Flames: Increases explosion range from the bomb in four directions by 1 square



There are three different modes!

* Single player, as you would expect it.
* Co-op - two players can play.
* Versus - One player plays as Bomberman, another player can play as a Balloom.







