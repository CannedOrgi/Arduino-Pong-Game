# Arduino-Pong-Game
An Arduino-based Pong game that has two modes to play- Breakout and Pong.

In this project, I have created a dual-game system featuring the classic games Pong and Breakout, utilizing
 two microcontrollers from the Arduino series: the Arduino Mega and the Arduino UNO. This approach
 allows us to leverage the distinct capabilities of each microcontroller to optimize both the game functionality
 and the system’s overall performance.

The Arduino Mega serves as the primary controller, responsible for executing the main game logic and
 managing the display outputs on a TFT screen. The choice of the Mega is due to its superior processing
 power and enhanced memory capacity, which are ideal for handling the complex computations and the dynamic
 graphical requirements of the games.
 
 Conversely, the Arduino UNO manages the user interface elements, including the button inputs and audio
 feedback via a buzzer. This division of tasks simplifies the coding and circuit design on the UNO, allowing it
 to efficiently process the inputs and send them to the Mega through serial communication. Using the UNO for
 these auxiliary tasks helps in maintaining the system’s responsiveness and stability by offloading direct input
 handling from the Mega.
 
 The two Arduinos communicate using the standard TX (Transmit) and RX (Receive) serial pins. This setup
 exemplifies a practical application of serial communication between different microcontrollers, illustrating an
 essential technique in embedded systems design where multiple processors work in tandem to accomplish more
 complex tasks.
