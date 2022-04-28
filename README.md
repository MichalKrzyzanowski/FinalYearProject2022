# FinalYearProject2022
Repository holding my final year project for game development.

# Running the game
Download the latest release build and run the SDLBox2DStuff exe. This will play the main game.
If you run the Difficulty Estimation exe, it will run the difficulty estimation program with the most recent level data.

# Building
If, for some reason, you want to create a build of the project, here is what you need:
- SDL2-2.0.22
- SDL2_ttf-2.0.15
- SDL2_image-2.0.5

Make sure to setup the enviroment variables correctly & use the x64 lib files.
Once you have the libraries setup, you can open up the SDLBox2DStuff sln file using vs2019 or later and build the solution.
Once you have the build, copy over the following directories from any of the project directories or create them into your build directory:
- levels
- results
- assets (needs LemonMilk.otf TTF)

Once that's finished, your build should be good to go. Also, a Debug build works best right now!

# Controls
**Mouse:** Navigation

**Left Mouse Button:** Selecting & placing shapes. interacting with UI elements such as level selection list

**SpaceBar:** Toggle Edit/Gameplay Mode

**H:** Toggle help text

**G:** Run difficulty estimation program

**S:** Save level

**L:** Load level

**C:** Clear level

# Links
**Google Drive:** https://drive.google.com/drive/folders/1fEblTN2H-J8T8Oyl6l6q4IRp2Ga9qbB3
