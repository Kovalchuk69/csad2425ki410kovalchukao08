void setup() {
  pinMode(13, OUTPUT); // Ініціалізація піну для лампочки
  digitalWrite(13, HIGH);

  Serial.begin(9600);  // Налаштування серійного зв'язку зі швидкістю 9600 біт/с
  while (!Serial);     // Очікуємо поки порт стане доступним (тільки для Leonardo і схожих плат)
  digitalWrite(13, LOW);
}

void loop() {
  if (Serial.available() > 0) {
    String receivedMessage = Serial.readString();  // Читаємо дані з серійного порту
    digitalWrite(13, HIGH);

    // Парсимо JSON повідомлення
    String parsedMessage = parseJSON(receivedMessage);

    // Поділ повідомлення на перше і друге слово
    int spaceIndex = parsedMessage.indexOf(' ');
    String firstWord, secondWord;

    if (spaceIndex != -1) {
      firstWord = parsedMessage.substring(0, spaceIndex);
      secondWord = parsedMessage.substring(spaceIndex + 1);
    } else {
      firstWord = parsedMessage;  // Якщо повідомлення містить тільки одне слово
      secondWord = "";
    }

    // Перетворення регістру слів
    firstWord.toLowerCase();
    secondWord.toUpperCase();

    // Формуємо змінене повідомлення
    String modifiedMessage = firstWord + " " + secondWord;

    // Створюємо JSON для відправки
    String jsonToSend = createJSON(modifiedMessage);

    digitalWrite(13, LOW);
    // Відправляємо змінене повідомлення назад
    Serial.print(jsonToSend);
  }
}

// Функція створення JSON
String createJSON(String message) {
  return "{\"message\":\"" + message + "\"}";
}

// Функція парсингу JSON
String parseJSON(String json) {
  int start = json.indexOf("\"message\":\"") + 11; // 11 - довжина ключа "message" з лапками
  int end = json.indexOf("\"", start);

  if (start != -1 && end != -1) {
    return json.substring(start, end);
  }

  return ""; // Якщо JSON некоректний
}
