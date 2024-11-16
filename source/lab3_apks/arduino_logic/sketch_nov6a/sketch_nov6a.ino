/**
@file sketch_nov6a.ino
*/

#include <ArduinoJson.h>

/// @brief The target score needed to win the game.
const int targetScore = 3;

void setup() {
  /**
   * @brief Initializes the Serial communication and the random number generator.
   *
   * Sets the baud rate to 9600 for Serial communication and seeds the random
   * number generator using an analog read on pin 0 to ensure randomness.
   */
  Serial.begin(9600);
  while (!Serial) { }
  randomSeed(analogRead(0));
}

void loop() {
  /// @brief Game mode labels for display in the game info.
  String modes[] = {"Player vs Computer ", "Player vs Player ", "Computer vs Computer "};

  /// @brief Labels for Player 1 based on the selected game mode.
  String player1[] = {" Player ", " Player 1 ", " Computer1 "};

  /// @brief Labels for Player 2 based on the selected game mode.
  String player2[] = {"Computer ", " Player 2 ", " Computer2 "};

  /// @brief Choices available for the game: Rock, Paper, Scissors.
  String choices[] = {"Rock ", " Paper ", " Scissors"};

  // Check if there is incoming data on the Serial.
  if (Serial.available() > 0) {
    // Read the incoming JSON string until a newline character.
    String input = Serial.readStringUntil('\n');

    /**
     * @brief JSON document to parse the input data.
     * 
     * A static JSON document of size 200 is used for parsing the input data
     * to prevent dynamic memory allocation on the microcontroller.
     */
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, input);

    // Check for JSON parsing errors.
    if (error) {
      Serial.println("{\"error\":\"Invalid JSON\"}");
      return;
    }

    // Retrieve the array from the JSON document.
    JsonArray array = doc["array"];
    if (!array.isNull() && array.size() == 5) {
      int mode = array[0]; ///< @brief Game mode selected by the player.

      // Generate random choices for the computer based on the game mode.
      if (mode == 1) {
        array[2] = random(1, 4);  // Player vs Computer mode
      } else if (mode == 3) {
        array[1] = random(1, 4);  // Computer vs Computer mode, both players are AI
        array[2] = random(1, 4);
      }

      int player1Choice = array[1]; ///< @brief Player 1's choice.
      int player2Choice = array[2]; ///< @brief Player 2's choice.
      int player1Wins = array[3];   ///< @brief Current win count for Player 1.
      int player2Wins = array[4];   ///< @brief Current win count for Player 2.

      // Determine the result of the round.
      String resultText;  ///< @brief Text describing the outcome of the round.
      String gameInfo;    ///< @brief Additional information about the current game state.

      // Logic for determining the winner of the round.
      if (player1Choice == player2Choice) {
        resultText = "Draw";
      } else if ((player1Choice == 1 && player2Choice == 3) || 
                 (player1Choice == 2 && player2Choice == 1) || 
                 (player1Choice == 3 && player2Choice == 2)) {
        player1Wins++;
        resultText = player1[mode - 1] + " wins this round";
      } else {
        player2Wins++;
        resultText = player2[mode - 1] + " wins this round";
      }

      // Update the array with the current scores.
      array[3] = player1Wins;
      array[4] = player2Wins;

      // Check if either player has reached the target score.
      if (player1Wins == targetScore) {
        resultText = player1[mode - 1] + " wins the game";
      } else if (player2Wins == targetScore) {
        resultText = player2[mode - 1] + " wins the game";
      }

      /**
       * @brief JSON document to store the response data.
       * 
       * This document is used to create the JSON response, including the updated
       * game state array, the result text, and additional game information.
       */
      StaticJsonDocument<200> responseDoc;
      JsonArray responseArray = responseDoc.createNestedArray("array");
      for (int i = 0; i < 5; i++) {
        responseArray.add(array[i]);
      }

      // Add the result and game information to the JSON response.
      responseDoc["result"] = resultText;
      responseDoc["info"] = modes[mode - 1] + player1[mode - 1] + " choice: " + choices[player1Choice - 1] + ";  " +
                            player2[mode - 1] + " choice: " + choices[player2Choice - 1];

      // Serialize the JSON response and send it over Serial.
      serializeJson(responseDoc, Serial);
      Serial.println();  // End the message with a newline character.
    }
  }

  delay(100);  // Delay to avoid overloading the Serial communication.
}
