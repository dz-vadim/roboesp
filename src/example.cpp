// // Подключаем библиотеки
// #include <WiFi.h>
// #include <WebServer.h>
// #include <ESPmDNS.h>
// #include <SPIFFS.h>

// // Определяем интервал записи в файл
// const unsigned long INTERVAL = 10000;
// unsigned long write_millis = 0;

// // Определяем название и пароль точки доступа
// const char* ssid = "название_WiFi"; // Это поле нужно заполнить необходимыми данными
// const char* password = "пароль_WiFi"; // Это поле нужно заполнить необходимыми данными

// // Определяем имя сервера в сети
// const char* host = "piranha-logger";

// // Создаём объект Веб-сервера
// WebServer server(80);

// // Функция обработки клиента
// void handleClient()
// {
//     // Выводим содержимое корневого каталога как Веб-страницу
//     listRootToHtml();
// }

// // Функция чтения файла
// bool handleFileRead(String path)
// {
//     // Открываем файл для чтения по указанному пути
//     File file = SPIFFS.open(path, "r");

//     // Если файл не удалось открыть
//     if (!file) {
//         Serial.println("Ошибка. Файл не существует.");

//         // Отвечаем клиенту ошибкой
//         server.send(404, "text/plain", "FileNotFound");

//         // Возвращаемся из функции
//         return false;
//     }

//     // Отдаём клиенту содержимое файла
//     server.streamFile(file, "text/html");
//     // Закрываем файл
//     file.close();
//     // Возвращаемся из функции
//     return true;
// }

// // Функция вывода списка файлов в HTML страницу
// void listRootToHtml()
// {
//     // Открываем корневой файл
//     File root = SPIFFS.open("/");

//     // Если не получилось открыть
//     if (!root) {
//         // Выводим ошибку
//         Serial.println("error");
//         // Возвращаемся из функции
//         return;
//     }

//     // Создаём строку для конкатенации
//     String html = "<html><meta charset=\"UTF-8\"><body>";

//     // Если корневой файл - каталог
//     if (root.isDirectory()) {

//         // Открываем следующий файл
//         File file = root.openNextFile();

//         // Входим в цикл, если файл существует
//         /* После прохода данного цикла должна
//          * получиться страница вида:
//          * <html><meta charset="UTF-8"><body>
//          * <p><a href="имя_первого_файла download="имя_первого_файла">
//          * имя_первого_файла</a></p>
//          * <p><a href="имя_второго_файла download="имя_второго_файла">
//          * имя_второго_файла</a></p>
//          * .....
//          * <p><a href="имя_N-фала download="имя_N-файла">
//          * имя_N-файла</a></p>
//          * </body></html>
//          */
//         while (file) {

//             // Записываем имя файла
//             String name = file.name();

//             // Удаляем первый символ ("/");
//             name.remove(0, 1);

//             // Конкатенируем строку
//             html += "<p><a href=\"";
//             html += name;
//             html += (String)"\" download=\"";
//             html += name +"\">";
//             html += name;
//             html += "</a></p>";

//             // Открываем следующий файл
//             file = root.openNextFile();
//         }
//         html += "</body></html>";
//     }

//     // Отравляем страницу клиенту
//     server.send(200, "text/html", html);
// }

// void setup() {

//     // Форматируем внутренную память (нужно выполнить только один раз)
//     // SPIFFS.format();

//     // Инициируем объект файловой системы
//     SPIFFS.begin();

//     Serial.begin(115200);
//     Serial.println();
//     Serial.println("Подключаемся к WiFi");

//     // Инициируем точку доступа WiFi
//     WiFi.begin(ssid, password);
//     while (WiFi.status() != WL_CONNECTED) {
//         Serial.print('.');
//         delay(100);
//     }
//     // Записываем IP-адрес
//     IPAddress myIP = WiFi.localIP();

//     // Инициируем Multicast DNS
//     MDNS.begin(host);

//     // Выводим IP-адрес Веб-сервера
//     Serial.print("IP-адрес: ");
//     Serial.println(myIP);

//     // Функция, выполняемая при подключении к клиента корневому каталогу
//     server.on("/",  HTTP_GET, handleClient);
//     // Функция, которая будет выполнена, если файл не найден
//     server.onNotFound([]() {
//         // Если по указанному пути файл не найден...
//         if (!handleFileRead(server.uri())) {
//             // Отвечаем клиенту ошибкой 404
//             server.send(404, "text/plain", "FILE NOT FOUND");
//         }
//     });

//     // Инициируем сервер
//     server.begin();
//     Serial.println("Сервер запущен.");
// }

// void loop() {
//     // Библиотечная функция обработки клиента
//     server.handleClient();

//     // Если прошёл интервал ожидания
//     if (millis() - write_millis > INTERVAL) {
//         // Записываем текущие millis
//         write_millis = millis();
//         // Создаём файл с абсолютным путём
//         File file = SPIFFS.open("/logger.txt", "w");

//         // Если файл не получилось создать
//         if (!file)
//             // Выводим сообщение
//             Serial.println("error creating file");
//         // Иначе
//         else
//             // Выводим текущие millis в файл
//             file.println(write_millis);
//             file.flush();
//             file.close();
//     }
//     // Даём процессору переключиться на другие задачи
//     delay(2);
// }