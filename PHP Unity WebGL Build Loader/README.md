# PHP Unity WebGL Build Loader
This is an example of some work I had done on a php server as part of the process for:
* Showing a loading screen
* Downloading and starting the Unity WebGL build from an _AWS_ repository
* Writing data onto the webpage for the game instance to use during initialization 

This loader was used for both loading the WebGL build as part of the _Facebook Games_ platform as well as the Jackpot Party self hosted website.

## Explanation
The backend runs this process when the "_load game_" endpoint was hit. Either by direct __URI__ or via a __Facebook redirect__.

As part of this process, the backend has made some SQL queries to gather information. If this is Facebook, the backend also writes some player login information to the webpage that runs the game.
`$data` is an array that was filled out with this data and sent in to this PHP/JS body to help determine how we want the game instance to be setup, and where the build executable files are located.

`onWebGLResize()` and `initWebGL()` are functions that are defined in the client Unity project and built into the executable via the WebGL plugins folder in Unity.
