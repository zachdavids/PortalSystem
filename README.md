# PortalSystem
****

<p align="center">
<img src="PortalSystem.gif">
</p>

A portal system made for Unreal Engine 4 using C++ and based off that of Valve's Portal series.

### Features
  * Scene Capture Portals 
  * Teleportation
  * Portal Gun
  * Placement System
 
### In Development
  * Alternative Portal Gun Firing Mode (Tunneling, Wormhole)
  * Seamlessness
  * Recursive Scene Capturing

### Known Bugs
  * Teleportation does not reorient player
  * Render targets not cleared when target portal deleted (Change Portal Texture to Placeholder)
  * Player not visible in scene captures (Ensure Mesh Set to Visible in Rendering)
  * Portal will not spawn under player
  * Portal does not recognize collision if spawned under player (Check Overlapping Actors on BeginPlay to Solve)
