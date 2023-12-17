/*
версия 1.1
добавлено:
    размер поля в статистике у лучшего и худшего значения времени и кол-ва ходов
удалено:
    подключение библиотек:
        iostream
        algorithm (теперь максимальное и минимальное я ищу сам)
*/
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>
#include <fstream>
#include <string>
#include <vector>
#include <random>
#include "BasicsDateTime.h"
#define width 800
#define height 600

using namespace sf;
using namespace std;

/*Структура одной карты дял хранения передней и задней части карты и bool переменной для отслеживания, видна ли карта в данный момент*/
struct Card {
public:
    //код карты - номер иконки из файла для проверки, открыты ли две одинаковые карты
    Card(Texture& texture, IntRect cardR, IntRect backR, int code) : _code(code) {
        _spriteCard.setTexture(texture);
        _spriteCard.setTextureRect(cardR);
        _spriteBack.setTexture(texture);
        _spriteBack.setTextureRect(backR);
    } 
    void dravSprite(RenderWindow& window) {
        window.draw((_isVisible) ? (_spriteCard) : (_spriteBack));
    }
    void setPos(int x, int y) {
        _spriteBack.setPosition(x, y);
        _spriteCard.setPosition(x, y);
    }
    void makeOpenClose() {//меняет открытое положение на закрытое и наоборот
        _isVisible = !_isVisible;
    }
    bool isVisible() {
        return _isVisible;
    }
    int getCode() {
        return _code;
    }
    void makeVisible() {
        _isVisible = true;
    }
    void makeInvisible() {
        _isVisible = false;
    }
private:
    Sprite _spriteCard;
    Sprite _spriteBack;
    const int _code;//код картинки
    bool _isVisible = false;
};

/*Создаёт массив размера size с рандомными эл-ами (от min до max), при том, что каждый эл-т встречается ровно 2 раза*/
vector<int> randomVector(int size, int min, int max) {
    // Проверка на корректные аргументы
    if (size % 2 != 0 || max < min) {
        std::cerr << "Некорректные аргументы" << std::endl;
        return std::vector<int>();
    }

    std::vector<int> result;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(min, max);

    // Заполняем вектор уникальными случайными числами
    while (result.size() < size) {
        int randomNum = distribution(gen);
        // Проверяем, не было ли такого числа ранее в векторе
        if (std::find(result.begin(), result.end(), randomNum) == result.end()) {
            result.push_back(randomNum);
            result.push_back(randomNum); // Добавляем число дважды
        }
    }

    // Перемешиваем элементы вектора
    std::shuffle(result.begin(), result.end(), gen);
    return result;
}

/*генерирует переменную текста дабы сократить код*/
Text makeText(Font& font, size_t size, Color color, int x, int y, sf::String text = "") {
    Text txt;
    txt.setFont(font);
    txt.setCharacterSize(size);
    txt.setFillColor(color);
    txt.setPosition(x, y);
    txt.setString(text);
    return txt;
}

/*првоеряет, находится ли эл-т ch в массиве*/
bool have(int ch, vector<int> arr) {
    for (auto i : arr) {
        if (ch == i) {
            return true;
        }
    }
    return false;
}

/*класс для работы окна статистики*/
class Statistic {
public:
    //загружает текстуру и ссылку на окно
    Statistic(RenderWindow& window) : _window(window) {
        _texture.loadFromFile("Statistic.png");
        _sprite.setTexture(_texture);
        initialiedArrays();
        Run();
    }
    //запуск окна статистики
    void Run() {
        Font font;
        font.loadFromFile("constan.ttf");
        Text txt1 = makeText(font, 40, Color::White, 0, 0, L"Лучшее время - \nХудшее время - \nМин кол-во ходов - \nМакс кол-во ходов - ");
        String bestT, loseT, bestH, loseH;
        bestLose(bestT, loseT, bestH, loseH);
        String txt = bestT + loseT + bestH + loseH;
        Text txt2 = makeText(font, 40, Color::White, 400, 0, txt);
        Event event;

        bool wasMousePressed = false;
        Vector2i prPos;
        while (_window.isOpen()) {
            while (_window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    _window.close();
                    return;
                }
            }

            bool isMousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);

            if (isMousePressed && !wasMousePressed) {/*кнопка зажата*/ }
            else if (!isMousePressed && wasMousePressed) {//кнока на мыши отжата
                prPos = Mouse::getPosition(_window);
                if (isPreviousBut(prPos)) {
                    return;//выходи из окна
                }
                else if (isClearBut(prPos)) {//очтистка файла статистики
                    ofstream outputFile("statistic.txt");
                    outputFile.close();
                    txt2.setString(L"00:00:00\n00:00:00\n0\n0");
                }
            }

            wasMousePressed = isMousePressed;

            _window.clear();
            _window.draw(_sprite);
            _window.draw(txt1);
            _window.draw(txt2);
            _window.display();
        }
    }
    /*нахождение лучшего и худшего*/
    void bestLose(sf::String& bestTime, sf::String& loseTime, sf::String& bestHod, sf::String& loseHod) {
        int posBT = 0;
        int posLT = 0;
        int posBH = 0;
        int posLH = 0;
        //находим минимальное и максимальное значение по позиции
        for (int i = 0; i < _times.size(); i++) {
            if (_times[i] < _times[posBT]) {
                posBT = i;
            }
            if (_times[i] > _times[posLT]) {
                posLT = i;
            }
            if (_times[i] < _countHod[posBT]) {
                posBH = i;
            }
            if (_times[i] > _countHod[posLT]) {
                posLH = i;
            }
        }
        //инициализация строк
        bestTime = string(_times[posBT]).erase(8) + L" поле = " + to_string(_posX[posBT]) + L"x" + to_string(_posY[posBT]) + '\n';
        loseTime = string(_times[posLT]).erase(8) + L" поле = " + to_string(_posX[posLT]) + L"x" + to_string(_posY[posLT]) + '\n';
        bestHod = to_string(_countHod[posBH]) + L" поле = " + to_string(_posX[posBH]) + L"x" + to_string(_posY[posBH]) + '\n';
        loseHod = to_string(_countHod[posLH]) + L" поле = " + to_string(_posX[posLH]) + L"x" + to_string(_posY[posLH]) + '\n';
    }
    /*инициализация массива статистики*/
    void initialiedArrays() {
        ifstream reader("statistic.txt");
        reader.seekg(0, std::ios::end);
        if (reader.tellg() == 0) {//проверка, является ли файл пустым
            _times.push_back(0);
            _countHod.push_back(0);
            return;
        }
        reader.seekg(0, std::ios::beg);

        int pri1;
        int pri2;
        int pri3;
        int pri4;
        while (reader >> pri1 >> pri2 >> pri3 >> pri4) {
            _times.push_back(pri1);
            _countHod.push_back(pri2);
            _posX.push_back(pri3);
            _posY.push_back(pri4);
        }
        reader.close();
    }
    bool isPreviousBut(Vector2i pos) {
        return (pos.x > 0 && pos.x < 320 && pos.y > 460 && pos.y < height);
    }
    bool isClearBut(Vector2i pos) {
        return (pos.x > 480 && pos.x < width && pos.y > 460 && pos.y < height);
    }
private:
    RenderWindow& _window;
    Texture _texture;
    Sprite _sprite;
    vector<sib::Time> _times;
    vector<int> _countHod;
    vector<int> _posX;
    vector<int> _posY;
};

/*класс для работы игры*/
class Play {
public:
    /*инициализация ссылки на окно, изменение размера окна с учётом того, какое кол-во карточек*/
    Play(RenderWindow& window, int x, int y) : _window(window), _x(x), _y(y) {
        nWidth = x * 130;
        nHeight = y * 130;
        //переменная для добавления размера окна если в окно не может влезть время, кол-во ходов и кол-во пар
        int plus = (nWidth < 774) ? (774 - nWidth) : (0);
        _window.setSize(Vector2u((nWidth + plus), (nHeight + 70)));
        _view.setSize((nWidth + plus), (nHeight + 70));
        _view.setCenter((nWidth + plus) / 2, (nHeight  + 70)/ 2);
        _window.setView(_view);
        initializedArrays();
        timer = sib::Time::Now();
        Run();
    }
private:
    void Run() {
        Event event;
        bool wasMousePressed = false;
        Vector2i pos;
        int mas[2] = {};
        int count = 0;
        Font font;
        font.loadFromFile("constan.ttf");
        //текст для вывода времени
        Text timeT = makeText(font, 50, Color::White, 0, nHeight);
        //текст для вывода кол-ва пар
        Text pairsCount = makeText(font, 50, Color::White, 256, nHeight, (L"Пар: " + to_string(_codeArr.size()) + "/" + to_string(_x * _y / 2)));
        int countHod = 0;
        //текст для вывода кол-ва ходов
        Text hodCount = makeText(font, 50, Color::White, 512, nHeight, (L"Ходов: " + to_string(countHod)));
        while (_window.isOpen()) {
            if(_codeArr.size() < (_x*_y /2))//вывод времени игры, если не все карточки ещё собраны
                timeT.setString(string(sib::Time::Now() - timer).erase(8));
            while (_window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    _window.close();
                    return;
                }
            }
            bool isMousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);
            if (isMousePressed && !wasMousePressed) {/*мышь зажата*/ }
            else if (!isMousePressed && wasMousePressed) {//мышь отпущена
                pos = Mouse::getPosition(_window);
                pos.x /= 128;
                pos.y /= 128;
                //проверка на верность координат нажатой мыши
                if (!(pos.x >= _field.size() || pos.y >= _field[0].size() || pos.x < 0 || pos.y < 0)) {
                    //если карточка закрыта
                    if (!_field[pos.x][pos.y].isVisible()) {
                        //если уже две карты открыты
                        if (count == 2) {
                            count = 0;
                            //если карты открыты одинаковы
                            if (mas[0] == mas[1]) {
                                //добавления кода открытых карт в массив игрорирования
                                _codeArr.push_back(mas[0]);
                            }
                            closeNoUse();
                        }
                        mas[count++] = _field[pos.x][pos.y].getCode();//получения кода нажатой карточки
                        countHod++;
                        hodCount.setString(L"Ходов: " + to_string(countHod));//обновления кол-ва ходов
                        _field[pos.x][pos.y].makeVisible();
                        if (count == 2 && (mas[0] == mas[1])) {//если после нажатия стали открыты две карточки и ониодинаковые, то добавления кода карты в массив игнора
                            count = 0;
                            _codeArr.push_back(mas[0]);
                        }
                    }//если карточка открыта
                    else {
                        if (count == 2) {//анналогичная ситуация, описанная выше
                            count = 0;
                            if (mas[0] == mas[1]) {
                                _codeArr.push_back(mas[0]);
                            }
                            closeNoUse();
                        }
                        if (_codeArr.size() == (_x * _y / 2)) {
                            break;
                        }
                    }//обновления кол-ва пар
                    pairsCount.setString(L"Пар: " + to_string(_codeArr.size()) + "/" + to_string(_x * _y / 2));
                }
            }
            wasMousePressed = isMousePressed;
            _window.clear();
            draw();
            _window.draw(timeT);
            _window.draw(hodCount);
            _window.draw(pairsCount);
            _window.display();
        }//запись результатов в файл
        ofstream writer("statistic.txt", ios::app);
        writer << sib::Time(string(timeT.getString())).AllMilliseconds() << ' ' << countHod << ' ' << _x << ' ' << _y << endl;
        writer.close();
    }
    void initializedArrays() {
        _texture.loadFromFile("logos.png");
        const int max = _x * _y;//размер карточек
        int count = 0;
        vector<int> numbers = randomVector(max, 1, 16);//генерация номеров карточки из файла с текстурами
        vector<Card> pr;//промежуточный массив
        for (int i = 0; i < _x && count < max; i++) {
            pr.clear();
            for (int j = 0; j < _y && count < max; j++, count++) {//добавления в массив новой карточки
                pr.push_back(Card(_texture, IntRect(128 * numbers[count], 0, 128, 128), IntRect(0, 0, 128, 128), numbers[count]));
                pr[j].setPos(130 * i, 130 * j);
            }
            _field.push_back(pr);
        }
    }
    void draw() {//отрисовка всех карточек на экран
        for (int i = 0; i < _x; i++) {
            for (int j = 0; j < _y; j++) {
                _field[i][j].dravSprite(_window);
            }
        }
    }
    void closeNoUse() {//закрытие всех карточек, которых нет в массиве игнора
        for (int i = 0; i < _x; i++) {
            for (int j = 0; j < _y; j++) {
                if (!have(_field[i][j].getCode(), _codeArr)) {
                    _field[i][j].makeInvisible();
                }
            }
        }
    }
private:
    sib::Time timer;
    Texture _texture;
    RenderWindow& _window;
    View _view;
    vector<vector<Card>> _field;
    vector<int> _codeArr;
    int _x;
    int _y;
    int nWidth;
    int nHeight;
};

/*класс для главного игрового окна*/
class Game {
public:
    //создания окна игры и загрузка иконки
    Game() : _window(VideoMode(width, height), L"Игра \'Память\'", sf::Style::Titlebar | sf::Style::Close) {
        Image icon;
        icon.loadFromFile("Icon.png");
        const sf::Uint8* iconPixels = icon.getPixelsPtr();
        _window.setIcon(icon.getSize().x, icon.getSize().y, iconPixels);
    }
    void Run() {
        Vector2i pr;
        View view;//загрузка размера окна
        _window.setSize(Vector2u(width, height));
        view.setSize(width, height);
        view.setCenter(width / 2, height / 2);
        while (_window.isOpen()) {//запуск начальног окна, которое по завешрении возвращает размер матрицы карточек (Vector2i)
            _window.setSize(Vector2u(width, height));
            _window.setView(view);
            pr = Hello();//откртие главного окна
            if (pr.x == 0 && pr.y == 0) {
                return;//если пользователь закрыл окно, то эл-ты икс и игрек будут 0, значит пользователь закрыл программу
            }//после получения размера матрицы - запуск игры
            Play game(_window, max(pr.x, pr.y), min(pr.x, pr.y));
        }

    }
private://методы
    Vector2i Hello() {
        Texture texture;
        texture.loadFromFile("Hello.png");
        Sprite sprite;
        sprite.setTexture(texture);
        sf::Font font;
        font.loadFromFile("constan.ttf");
        Text num1 = makeText(font, 200, Color::White, 100 ,200), num2 = makeText(font, 200, Color::White, 470, 200);//текст для отображения размера окна, которое вводит пользоватль
        int Xr = 2, Yr = 2;
        sf::Event event;
        bool wasMousePressed = false;//была ли до этого нажата мышка
        Vector2i prPos;
        while (_window.isOpen()){//отображение размера окна, которое вводит пользователь
            num1.setString(to_string(Xr));
            num2.setString(to_string(Yr));
            
            while (_window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    _window.close();//проверка на закрытие окна. Если оно закрыто, то возвращает нулевые координаты
                    return Vector2i(0, 0);
                }
            }//нажата ли мышка сейчас
            bool isMousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);
            //если мышка нажата и до этого не была нажата - значит она сейчас зажата
            if (isMousePressed && !wasMousePressed) {/*mous is presed*/ }
            else if (!isMousePressed && wasMousePressed){//иначе если мышка не зажата, но она была зажата, то был единичный клик
                prPos = Mouse::getPosition(_window);//получения позиции мышки и проверка, нажал ли пользователь какую-либо кнопку на экране
                if (isHelloUpX(prPos)) {
                    Xr++;
                }
                else if (isHelloDownX(prPos)) {
                    Xr--;
                }
                else if (isHelloUpY(prPos)) {
                    Yr++;
                }
                else if (isHelloDownY(prPos)) {
                    Yr--;
                }
                else if (isHelloButtonPres(prPos)) {
                    if (isFeidly(Xr, Yr)) {
                        return Vector2i(Xr, Yr);
                    }
                }
                else if (isClose(prPos)) {
                    _window.close();
                    return Vector2i(0, 0);
                }
                else if (isHelloStatistic(prPos)) {
                    Statistic stat(_window);
                }//провекра, какие размеры кнопок были введены пользователем
                if (Xr < 1) {
                    Xr = 15;
                }
                if (Yr < 1) {
                    Yr = 15;
                }
                if (Xr > 15) {
                    Xr = 1;
                }
                if (Yr > 15) {
                    Yr = 1;
                }
            }
            //если кнопка нажата - то она была нажата, а если она не нажата то она и не была нажата
            wasMousePressed = isMousePressed;
            _window.clear();
            _window.draw(sprite);
            _window.draw(num1);
            _window.draw(num2);
            _window.display();
        }
        return Vector2i(Xr, Yr);
    }
    //функции для проверки, где находится курсор нажатой мышки, чтобы проверить, нажата ли соответствующая кнопка на экране
    bool isHelloUpX(Vector2i pos) {
        return (pos.x > 300 && pos.y > 270 && pos.x < 370 && pos.y < 335);
    }
    bool isHelloDownX(Vector2i pos) {
        return (pos.x > 300 && pos.y > 330 && pos.x < 370 && pos.y < 395);
    }
    bool isHelloUpY(Vector2i pos) {
        return (pos.x > 670 && pos.y > 270 && pos.x < 740 && pos.y < 335);
    }
    bool isHelloDownY(Vector2i pos) {
        return (pos.x > 670 && pos.y > 330 && pos.x < 740 && pos.y < 395);
    }
    bool isHelloButtonPres(Vector2i pos) {
        return (pos.x > 150 && pos.y > 450 && pos.x < 560 && pos.y < 660);
    }
    bool isFeidly(int x, int y) {
        if ((x * y) > 30 || (x * y % 2) == 1 || x == 0 || y == 0) {
            return false;
        }
        return true;
    }
    bool isHelloStatistic(Vector2i pos){
        return (pos.x > 0 && pos.y > 0 && pos.x < 240 && pos.y < 50);
    }
    bool isClose(Vector2i pos) {
        return (pos.x > 660 && pos.x < width && pos.y > 0 && pos.y < 50);
    }
private://параметры
    RenderWindow _window;//окно, которое передаёться по всей программе, для сохранения открытого окна и отображения на нём всех нужных эл-ов.
};

/*инициализация и запуск игры*/
int main() {
    Game memo;
    memo.Run();
    return 0;
}