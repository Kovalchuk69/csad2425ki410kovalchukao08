#include "pch.h"
#include "../lab3_apks/Rock_paper_scissors_client\Rock_paper_scissors_client.h"

TEST(CreateJSONTest, NonEmptyArray) {
    vector<int> array = { 1, 2, 3, 4, 5 };
    string myjson = createJSON(array);

    EXPECT_EQ(myjson, "{\"array\":[1,2,3,4,5]}");
}
TEST(SaveGameStateTest, SavesCorrectJSON) {
    std::string filename = "test_config.json";
    vector<int> gameState = { 1, 2, 3 };
    std::string resultText = "Player 1 wins!";
    std::string gameInfo = "Round 1";

    // Викликаємо функцію
    saveGameState(gameState, resultText, gameInfo, filename);

    // Відкриваємо файл для перевірки
    std::ifstream file(filename);
    ASSERT_TRUE(file.is_open());

    json j;
    file >> j;
    file.close();

    // Перевіряємо вміст JSON
    ASSERT_EQ(j["gameState"], gameState);
    ASSERT_EQ(j["resultText"], resultText);
    ASSERT_EQ(j["gameInfo"], gameInfo);

    // Видаляємо тестовий файл
    std::remove(filename.c_str());
}
TEST(LoadGameStateTest, LoadsCorrectData) {
    std::string filename = "test_config.json";

    // Створюємо тестовий JSON-файл
    json j;
    j["gameState"] = { 1, 2, 3 };
    j["resultText"] = "Player 1 wins!";
    j["gameInfo"] = "Round 1";

    std::ofstream file(filename);
    file << j.dump(4);
    file.close();

    // Викликаємо функцію
    vector<int> gameState;
    std::string resultText;
    std::string gameInfo;

    bool success = loadGameState(gameState, resultText, gameInfo, filename);

    // Перевіряємо успішність завантаження
    ASSERT_TRUE(success);
    ASSERT_EQ(gameState, std::vector<int>({ 1, 2, 3 }));
    ASSERT_EQ(resultText, "Player 1 wins!");
    ASSERT_EQ(gameInfo, "Round 1");

    // Видаляємо тестовий файл
    std::remove(filename.c_str());
}
TEST(ParseJSONTest, ParsesCorrectData) {
    std::string jsonString = "{\"array\":[7,8,9],\"result\":\"Player 2 wins!\",\"info\":\"Round 2\"}";
    vector<int> array;
    std::string resultText;
    std::string gameInfo;

    // Викликаємо функцію
    bool success = parseJSON(jsonString, array, resultText, gameInfo);

    // Перевіряємо успішність парсингу
    ASSERT_TRUE(success);
    ASSERT_EQ(array, std::vector<int>({ 7, 8, 9 }));
    ASSERT_EQ(resultText, "Player 2 wins!");
    ASSERT_EQ(gameInfo, "Round 2");
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}