# Agar.GT
Revision on the Agar.io game

In our version of the game, a player is associated with a blob, that interacts with other (non
player (AI controlled) or second player) blobs and assorted objects in the game field of play. A
blob can move in any direction with the goal of eating smaller blobs, and its (maximum) speed
decreases as its mass increases. A blob gains mass by colliding with and eating smaller blobs in
the field of play. The field of play is a large rectangular petri dish with at least 512 x 512 pixels.
In this game, the player is not afforded a full view of the field of play. Instead, the 128 x 128
uLCD screen serves as a floating “window” into a larger world. In the base version of the game,
if the player’s blob is eaten by another blob, the game is over. The baseline game may be played
in single player mode in which the other player is controlled by the game, and in two player
mode, where the second player’s mbed setup is remotely controlled over an Ethernet link.

Game Rules

Each player is able to see a view of the game world (displayed on its LCD screen)
which contains their blob. This could be just always centered on their blob, or it
could be something else.

A player controls the direction of their blob will move by tilting their board. The
game determines the direction by reading the accelerometer.

A blob moves at a velocity that is inversely proportional to its mass (area). You are
free to set the baseline velocity to enhance the feel of the game.

If two blobs collide, the blob with greater mass will consume the blob with lesser
mass, increasing its own mass correspondingly. We leave it to you to determine
how dissimilar blobs must be for the smaller to be eaten by the larger.

A blobs mass is represented by its area on the screen. That is all blobs have the
same density.

A player wins if its blob eats up the opponent’s blob.

On startup the world is randomly seeded with an odd number small food blobs.
Food blobs float around the game world without interacting with each other. In the
baseline game, player blobs grow by consuming food blobs.

A blob cannot move off the edge of the screen. If a food blob reaches the edge of
the game world, it bounces completely elastically (reverses direction without losing
speed). Player blobs cannot move outside the bounding rectangle of the game
world.
