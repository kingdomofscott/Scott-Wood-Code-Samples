# C++ Unreal Magic System
This is a sample of a system I had made for a project I was working on in my free time. The project was a 3d 3rd person hack and slash action game.
I'm *especially* proud of this one as I had completely free reign over the design and the needs it had to fill.

<img src="/Images/MagicDemo.gif" width="400">

## Explanation
The purpose of the system was to encapsulate __Magic Attacks__ that a player can cast in a variety of ways. These magic attacks can deal damage to enemies, be used in the overworld for exploration, or whatever else is needed.

A basic rundown of how this all works together:
* A magic attack exists, this extends from ABaseMagic
    * This encapsulates everything about the magic attack, the name, type, attributes, visuals, behavior, etc.
* Magic attacks have attributes 
    * Attributes help define the behavior of the attack. If it deals damage, if it travels, if it affects physics.
* Magic attacks have state machines
    * This is to control the lifetime of the attack.

The best things to look at would be `BaseMagic.h/cpp`, `WhirlwindMagic.h/cpp`, and `MagicDealDamageOverTimeAttribute.h.cpp`

### Thoughts
I'm very happy with how this system turned out, I think it's incredibly modular and incredibly explandable. The key goal here was to reduce how much code had to be rewritten assuming for 100+ magic attacks.
That said, this was also written before Unreal released their __Gameplay Ability System__ which would have been a very similar concept and this would be closer to reinventing the wheel.