# C# Unity Popup Manager
This is a redacted sample of a system that I had written which is intended to handle blocking gameplay and overlaying a popup in front of the player to be interacted with. The popup itself can have varying degrees of complexity. The system itself also supports layering popups, meaning you can have a popup open up its own sub popup.
Due to how much this system makes use of other systems written within the project, this will be non functional, and is more intended to give an idea of writing APIs and how they are handled, and what is exposed via interface.
Some important things to note here:
* This has a View/Controller design pattern, intended to keep all the visuals and more "Unity" specific code separate from the more complex code.
* The codebase as the concept of a _State Machine_, which the Popup Manager itself uses, this helps to set the state on the visual side.
* The codebase also has an _Injection Container_, this is used to inject dependencies into various controllers when they are created. This helps reduce direct passing of references via multiple layers of controllers. The _Popup Manager_ is accessed through the injection container by various features and systems.

<img src="/Images/PopupDemo.gif" width="400">

## Explanation
The Popup Manager works as being the main controller deciding what is shown over the screen (if anything). Exterior systems and controllers will get a reference to the manager and request to show a popup on screen. The manager will:
* Take this request
* Prioritize it among all the other open requests to show something
* Download any assets needed to show this popup
* Load the assets and show the popup on screen

Because of the complexity of the game there is a need to show nested popups. Features can be purely exposed and engaged through the popup system to the player. These features might then want to show a rule popup (example) overtop of the main feature popup itself, but still maintain the visuals and the running controllers without having to close anything. Due to that need, the Popup Manager implements a layering system that will allow a popup that is already on screen to load and immediately show a new popup without having to close itself or enqueue the popup to be shown after others already in the queue. Each layer is a transform in the Unity hierarchy, stacked over top of each other, all with one popup controller that has been set as the child of that Unity transform.

<img src="/Images/NestedPopupDemo.gif" width="400">