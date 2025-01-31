# megacloneman
Mega Man 2 proof of concept in C++ with SDL. Quick Man stage.

# Brief
I would like to explain here how certain things work just for prosperity.

# Movement
It's a simple tile-based movement, with only one type of tiles, namely 16 x 16 blocks that collide with Mega Man from all around. `TileScan` is the `struct` allowing to reason about multiple tiles that could align with one of Mega Man's sides, seeing that he is wide and tall enough to be potentially touching as many as 2 x 2 tiles. It's not revolutionary but eases up the clipping process a little bit. `TileClipping` is responsible for adjusting Mega Man's position and velocity based on the tile map around him. Simply put, his movement is divided in two states, grounded and air. By jumping or being grounded without tiles underneath his hitbox, he enters the air state, in which he is subject to gravity. Otherwise, he is not. This explicit distinction would eventually allow adding slopes, seeing that in a grounded state, all that needs to be done would be to adjust Mega Man's vertical position to match, for example, the y-height of the sloped tile in his middle below himself, OR the highest tile he is touching left or right.

Of course clipping is not a hassle with mere square blocks. The new position is predicted for Mega Man and using `TileScan` along with a check in which direction he is moving, it can be determined whether he needs to be pushed out of the right, left, top or bottom part of a tile. Naturally, he would only be pushed to the left out of a tile if he moved to the right and his right side touched a group of solid tiles. This holds for any direction. This clipper also needs to notify Mega Man should he bump into a bottom tile, as this is exactly the condition under which he would transition from an air into a grounded state.

Setting it up like this has good benefits. The most obvious is that the boss Quick Man and the enemy Sniper Joe can easily make use of what is effectively Mega Man's movement code, allowing them to very easily interact with the tiles around them, become subject to gravity and allow checking whether they bumped into anything.

# Key Binds
The backend is SDL, which provides `SDL_GetKeyboardState` indexed by `SDL_Scancode` types to see which keys are currently being pressed. An `std::unordered_map` is used to map from `std::string` to `SDL_Scancode` to effectively rename these SDL-dependent numbers to backend-agnostic identifiers. This is also what easily allows keybinds.

# Settings File (mega_settings.txt)
You may edit this file to set your key binds, starting room, invincibility and certain features including whether you can use charge shots or sliding. The latter 2 are options because Mega Man 2 didn't have sliding or charge shots. The settings file works by combining the `fstream::find` function with its `>>` operator in a template function to expect a certain input. A shortcoming of course is that expecting an `int` where the user wrote an `std::string` requires error handling, which can be greatly improved here.

# Actors
Naturally, all actors `Tick` once per frame, which is 60 times a second. When all objects finished ticking in order, they will all be drawn. Actors include Quick Man related enemies, as well as the boss Quick Man himself. Logic is implemented to prevent enemies from firing at you when they are not currently visible. Projectiles can also be aimed at Mega Man to target him precisely under the influence of gravity. Actually, seeing that actor logic ticks 60 times per second, physics are discrete, and as such, it is notable that the equation of motion more or less equates to `s(t) = s0 + t * v + t * (t + t) a / 2` as opposed to the non-discrete and more widely recognised `s(t) = s0 + t * v + t^2 a / 2`. The discrete variant can be derived by using the regular Gaussian sum `1 + 2 + ... + n = n * (n + 1) / 2` on acceleration.

This project uses a struct of multiple arrays of properties. It was an experiment to see how feasible this style of work is in the event that a `struct of array` approach would be preferable over `array of struct`. Needless to say, in retrospect, the `array of struct` approach would've been superior seeing how few objects are generated by each room in the first place.

# Integer scaling
The game screen is centred and scaled to the nearest possible inner size that would fit the window. This does two things. First, it keeps the actual game screen "Mega Man proportioned", so having a wider monitor does not allow the player to see ahead. Second, it prevents pixel distortion effects at non-integer multiples. Seeing that this is a sprite based game, individual pixels doubling would become quickly noticable and jarring to see, and while a bilinear image filter would smoothen this effect, it would also make the game look blurry and inferior due to its already low resolution not being helped any. This is, however, also optional, and can be turned off in `mega_settings.txt`.

# Level setup
A level is a big sprite containing all rooms Mega Man can move in. The truth is that between level transitions, sections of this large sprite are drawn as the background. Collision maps are provided separately and overlaid, allowing Mega Man to move and collide with tiles. I stuck with a primitive approach. Although the image method allows using graphics programs to pretty much draw the entire level, collision has to be coded in by hand. An update that would make use of Tiled could be preferable, which allows importing such a large sprite as well as drawing the necessary collision tile layer, instead of having to code it in painstakingly.

# Notice
The graphics obviously aren't mine. They are just to fit the on the nose Mega Man reference. Capcom isn't very unfriendly with fangames of this sort, so I suppose it is all right in fair use, since I don't pretend to make money off of it.
