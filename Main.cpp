/*
������ 1.1
���������:
    ������ ���� � ���������� � ������� � ������� �������� ������� � ���-�� �����
�������:
    ����������� ���������:
        iostream
        algorithm (������ ������������ � ����������� � ��� ���)
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

/*��������� ����� ����� ��� �������� �������� � ������ ����� ����� � bool ���������� ��� ������������, ����� �� ����� � ������ ������*/
struct Card {
public:
    //��� ����� - ����� ������ �� ����� ��� ��������, ������� �� ��� ���������� �����
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
    void makeOpenClose() {//������ �������� ��������� �� �������� � ��������
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
    const int _code;//��� ��������
    bool _isVisible = false;
};

/*������ ������ ������� size � ���������� ��-��� (�� min �� max), ��� ���, ��� ������ ��-� ����������� ����� 2 ����*/
vector<int> randomVector(int size, int min, int max) {
    // �������� �� ���������� ���������
    if (size % 2 != 0 || max < min) {
        std::cerr << "������������ ���������" << std::endl;
        return std::vector<int>();
    }

    std::vector<int> result;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(min, max);

    // ��������� ������ ����������� ���������� �������
    while (result.size() < size) {
        int randomNum = distribution(gen);
        // ���������, �� ���� �� ������ ����� ����� � �������
        if (std::find(result.begin(), result.end(), randomNum) == result.end()) {
            result.push_back(randomNum);
            result.push_back(randomNum); // ��������� ����� ������
        }
    }

    // ������������ �������� �������
    std::shuffle(result.begin(), result.end(), gen);
    return result;
}

/*���������� ���������� ������ ���� ��������� ���*/
Text makeText(Font& font, size_t size, Color color, int x, int y, sf::String text = "") {
    Text txt;
    txt.setFont(font);
    txt.setCharacterSize(size);
    txt.setFillColor(color);
    txt.setPosition(x, y);
    txt.setString(text);
    return txt;
}

/*���������, ��������� �� ��-� ch � �������*/
bool have(int ch, vector<int> arr) {
    for (auto i : arr) {
        if (ch == i) {
            return true;
        }
    }
    return false;
}

/*����� ��� ������ ���� ����������*/
class Statistic {
public:
    //��������� �������� � ������ �� ����
    Statistic(RenderWindow& window) : _window(window) {
        _texture.loadFromFile("Statistic.png");
        _sprite.setTexture(_texture);
        initialiedArrays();
        Run();
    }
    //������ ���� ����������
    void Run() {
        Font font;
        font.loadFromFile("constan.ttf");
        Text txt1 = makeText(font, 40, Color::White, 0, 0, L"������ ����� - \n������ ����� - \n��� ���-�� ����� - \n���� ���-�� ����� - ");
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

            if (isMousePressed && !wasMousePressed) {/*������ ������*/ }
            else if (!isMousePressed && wasMousePressed) {//����� �� ���� ������
                prPos = Mouse::getPosition(_window);
                if (isPreviousBut(prPos)) {
                    return;//������ �� ����
                }
                else if (isClearBut(prPos)) {//�������� ����� ����������
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
    /*���������� ������� � �������*/
    void bestLose(sf::String& bestTime, sf::String& loseTime, sf::String& bestHod, sf::String& loseHod) {
        int posBT = 0;
        int posLT = 0;
        int posBH = 0;
        int posLH = 0;
        //������� ����������� � ������������ �������� �� �������
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
        //������������� �����
        bestTime = string(_times[posBT]).erase(8) + L" ���� = " + to_string(_posX[posBT]) + L"x" + to_string(_posY[posBT]) + '\n';
        loseTime = string(_times[posLT]).erase(8) + L" ���� = " + to_string(_posX[posLT]) + L"x" + to_string(_posY[posLT]) + '\n';
        bestHod = to_string(_countHod[posBH]) + L" ���� = " + to_string(_posX[posBH]) + L"x" + to_string(_posY[posBH]) + '\n';
        loseHod = to_string(_countHod[posLH]) + L" ���� = " + to_string(_posX[posLH]) + L"x" + to_string(_posY[posLH]) + '\n';
    }
    /*������������� ������� ����������*/
    void initialiedArrays() {
        ifstream reader("statistic.txt");
        reader.seekg(0, std::ios::end);
        if (reader.tellg() == 0) {//��������, �������� �� ���� ������
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

/*����� ��� ������ ����*/
class Play {
public:
    /*������������� ������ �� ����, ��������� ������� ���� � ������ ����, ����� ���-�� ��������*/
    Play(RenderWindow& window, int x, int y) : _window(window), _x(x), _y(y) {
        nWidth = x * 130;
        nHeight = y * 130;
        //���������� ��� ���������� ������� ���� ���� � ���� �� ����� ������ �����, ���-�� ����� � ���-�� ���
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
        //����� ��� ������ �������
        Text timeT = makeText(font, 50, Color::White, 0, nHeight);
        //����� ��� ������ ���-�� ���
        Text pairsCount = makeText(font, 50, Color::White, 256, nHeight, (L"���: " + to_string(_codeArr.size()) + "/" + to_string(_x * _y / 2)));
        int countHod = 0;
        //����� ��� ������ ���-�� �����
        Text hodCount = makeText(font, 50, Color::White, 512, nHeight, (L"�����: " + to_string(countHod)));
        while (_window.isOpen()) {
            if(_codeArr.size() < (_x*_y /2))//����� ������� ����, ���� �� ��� �������� ��� �������
                timeT.setString(string(sib::Time::Now() - timer).erase(8));
            while (_window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    _window.close();
                    return;
                }
            }
            bool isMousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);
            if (isMousePressed && !wasMousePressed) {/*���� ������*/ }
            else if (!isMousePressed && wasMousePressed) {//���� ��������
                pos = Mouse::getPosition(_window);
                pos.x /= 128;
                pos.y /= 128;
                //�������� �� �������� ��������� ������� ����
                if (!(pos.x >= _field.size() || pos.y >= _field[0].size() || pos.x < 0 || pos.y < 0)) {
                    //���� �������� �������
                    if (!_field[pos.x][pos.y].isVisible()) {
                        //���� ��� ��� ����� �������
                        if (count == 2) {
                            count = 0;
                            //���� ����� ������� ���������
                            if (mas[0] == mas[1]) {
                                //���������� ���� �������� ���� � ������ �������������
                                _codeArr.push_back(mas[0]);
                            }
                            closeNoUse();
                        }
                        mas[count++] = _field[pos.x][pos.y].getCode();//��������� ���� ������� ��������
                        countHod++;
                        hodCount.setString(L"�����: " + to_string(countHod));//���������� ���-�� �����
                        _field[pos.x][pos.y].makeVisible();
                        if (count == 2 && (mas[0] == mas[1])) {//���� ����� ������� ����� ������� ��� �������� � �������������, �� ���������� ���� ����� � ������ ������
                            count = 0;
                            _codeArr.push_back(mas[0]);
                        }
                    }//���� �������� �������
                    else {
                        if (count == 2) {//������������ ��������, ��������� ����
                            count = 0;
                            if (mas[0] == mas[1]) {
                                _codeArr.push_back(mas[0]);
                            }
                            closeNoUse();
                        }
                        if (_codeArr.size() == (_x * _y / 2)) {
                            break;
                        }
                    }//���������� ���-�� ���
                    pairsCount.setString(L"���: " + to_string(_codeArr.size()) + "/" + to_string(_x * _y / 2));
                }
            }
            wasMousePressed = isMousePressed;
            _window.clear();
            draw();
            _window.draw(timeT);
            _window.draw(hodCount);
            _window.draw(pairsCount);
            _window.display();
        }//������ ����������� � ����
        ofstream writer("statistic.txt", ios::app);
        writer << sib::Time(string(timeT.getString())).AllMilliseconds() << ' ' << countHod << ' ' << _x << ' ' << _y << endl;
        writer.close();
    }
    void initializedArrays() {
        _texture.loadFromFile("logos.png");
        const int max = _x * _y;//������ ��������
        int count = 0;
        vector<int> numbers = randomVector(max, 1, 16);//��������� ������� �������� �� ����� � ����������
        vector<Card> pr;//������������� ������
        for (int i = 0; i < _x && count < max; i++) {
            pr.clear();
            for (int j = 0; j < _y && count < max; j++, count++) {//���������� � ������ ����� ��������
                pr.push_back(Card(_texture, IntRect(128 * numbers[count], 0, 128, 128), IntRect(0, 0, 128, 128), numbers[count]));
                pr[j].setPos(130 * i, 130 * j);
            }
            _field.push_back(pr);
        }
    }
    void draw() {//��������� ���� �������� �� �����
        for (int i = 0; i < _x; i++) {
            for (int j = 0; j < _y; j++) {
                _field[i][j].dravSprite(_window);
            }
        }
    }
    void closeNoUse() {//�������� ���� ��������, ������� ��� � ������� ������
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

/*����� ��� �������� �������� ����*/
class Game {
public:
    //�������� ���� ���� � �������� ������
    Game() : _window(VideoMode(width, height), L"���� \'������\'", sf::Style::Titlebar | sf::Style::Close) {
        Image icon;
        icon.loadFromFile("Icon.png");
        const sf::Uint8* iconPixels = icon.getPixelsPtr();
        _window.setIcon(icon.getSize().x, icon.getSize().y, iconPixels);
    }
    void Run() {
        Vector2i pr;
        View view;//�������� ������� ����
        _window.setSize(Vector2u(width, height));
        view.setSize(width, height);
        view.setCenter(width / 2, height / 2);
        while (_window.isOpen()) {//������ ��������� ����, ������� �� ���������� ���������� ������ ������� �������� (Vector2i)
            _window.setSize(Vector2u(width, height));
            _window.setView(view);
            pr = Hello();//������� �������� ����
            if (pr.x == 0 && pr.y == 0) {
                return;//���� ������������ ������ ����, �� ��-�� ��� � ����� ����� 0, ������ ������������ ������ ���������
            }//����� ��������� ������� ������� - ������ ����
            Play game(_window, max(pr.x, pr.y), min(pr.x, pr.y));
        }

    }
private://������
    Vector2i Hello() {
        Texture texture;
        texture.loadFromFile("Hello.png");
        Sprite sprite;
        sprite.setTexture(texture);
        sf::Font font;
        font.loadFromFile("constan.ttf");
        Text num1 = makeText(font, 200, Color::White, 100 ,200), num2 = makeText(font, 200, Color::White, 470, 200);//����� ��� ����������� ������� ����, ������� ������ �����������
        int Xr = 2, Yr = 2;
        sf::Event event;
        bool wasMousePressed = false;//���� �� �� ����� ������ �����
        Vector2i prPos;
        while (_window.isOpen()){//����������� ������� ����, ������� ������ ������������
            num1.setString(to_string(Xr));
            num2.setString(to_string(Yr));
            
            while (_window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    _window.close();//�������� �� �������� ����. ���� ��� �������, �� ���������� ������� ����������
                    return Vector2i(0, 0);
                }
            }//������ �� ����� ������
            bool isMousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);
            //���� ����� ������ � �� ����� �� ���� ������ - ������ ��� ������ ������
            if (isMousePressed && !wasMousePressed) {/*mous is presed*/ }
            else if (!isMousePressed && wasMousePressed){//����� ���� ����� �� ������, �� ��� ���� ������, �� ��� ��������� ����
                prPos = Mouse::getPosition(_window);//��������� ������� ����� � ��������, ����� �� ������������ �����-���� ������ �� ������
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
                }//��������, ����� ������� ������ ���� ������� �������������
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
            //���� ������ ������ - �� ��� ���� ������, � ���� ��� �� ������ �� ��� � �� ���� ������
            wasMousePressed = isMousePressed;
            _window.clear();
            _window.draw(sprite);
            _window.draw(num1);
            _window.draw(num2);
            _window.display();
        }
        return Vector2i(Xr, Yr);
    }
    //������� ��� ��������, ��� ��������� ������ ������� �����, ����� ���������, ������ �� ��������������� ������ �� ������
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
private://���������
    RenderWindow _window;//����, ������� ���������� �� ���� ���������, ��� ���������� ��������� ���� � ����������� �� �� ���� ������ ��-��.
};

/*������������� � ������ ����*/
int main() {
    Game memo;
    memo.Run();
    return 0;
}