# GameEngine

This project is made in C++ and is still in development.

The project has 2 major parts. One is the launcher where you have to connect to a server. This is where you need to login and the server confirms if you are registered in the database. If successful you are able to create a room or join a room of another player. After two people are in a room and click ready, the second part starts

Its a game created in a selfmade engine from scratch. As a graphics API I use Vulkan and other then that I only use some std includes. Math libary is selfmade. The game is a 1v1 space invader type game. Right now I am developing the gameplay and collisions. The protocol used for the peer to peer connection is UDP. For the launcher I use TCP and the RDBMS is MySQL.
The models and texture maps are created by Felix von Oppeln-Bronikowski

