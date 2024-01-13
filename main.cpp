#include <iostream>
#include <SFML/Graphics.hpp>
#include "json.hpp"
#include<fstream>
#include<set>
#include<cmath>

#define NOGUI false
#define screenScale 300

int get_sign(double x) {
    if (x > 0)
        return 1;
    if (x < 0)
        return -1;
    return 1;
}

struct Robot {
    double x = 0.1, y = 0.1, ax = 0, ay = 0, angle = 0;
    sf::Color color = sf::Color::White;

    Robot() {};

    void rotate(double a) {
        angle += a;
        if (angle >= 2 * M_PI)
            angle = angle - 2 * M_PI;
        else if (angle <= 0) {
            angle = M_PI * 2 + angle;
        }
        //std::cout << angle << std::endl;
    }

    void move(double X, double Y, double maxX, double maxY) {
        x += X * cos(angle) + Y * sin(angle);
        y += -X * sin(angle) + Y * cos(angle);
        if(x < 0)
            x = 0;
        if(y < 0)
            y = 0;
        if(x > maxX + 1.2)
            x = maxX + 1.2;
        if(y > maxY + 1.2)
            y = maxY + 1.2;
    }
};

struct Line {
    double x1, y1, x2, y2;

    Line(double X1, double Y1, double X2, double Y2) {
        x1 = std::min(X1, X2);
        x2 = std::max(X1, X2);
        y1 = std::min(Y1, Y2);
        y2 = std::max(Y1, Y2);
    }

    bool operator<(const Line l) const {
        int sx1 = (x1 + x2) / 2;
        int sx2 = (l.x1 + l.x2) / 2;
        int sy1 = (y1 + y2) / 2;
        int sy2 = (l.y1 + l.y2) / 2;
        if (sx1 == sx2)
            return sy1 < sy2;
        return sx1 < sx2;
    }

    bool operator>(const Line l) const {
        int sx1 = (x1 + x2) / 2;
        int sx2 = (l.x1 + l.x2) / 2;
        int sy1 = (y1 + y2) / 2;
        int sy2 = (l.y1 + l.y2) / 2;
        if (sx1 == sx2)
            return sy1 > sy2;
        return sx1 > sx2;
    }

    bool operator==(const Line l) const {
        int sx1 = (x1 + x2) / 2;
        int sx2 = (l.x1 + l.x2) / 2;
        int sy1 = (y1 + y2) / 2;
        int sy2 = (l.y1 + l.y2) / 2;
        return sx1 == sx2 && sy1 == sy2;
    }

    bool operator!=(const Line l) const {
        int sx1 = (x1 + x2) / 2;
        int sx2 = (l.x1 + l.x2) / 2;
        int sy1 = (y1 + y2) / 2;
        int sy2 = (l.y1 + l.y2) / 2;
        return !(sx1 == sx2 && sy1 == sy2);
    }

    friend std::ostream &operator<<(std::ostream &o, Line &l) {
        o << l.x1 << " " << l.y1 << "|" << l.x2 << " " << l.y2 << std::endl;
        return o;
    }
};

struct Field {
    nlohmann::json json;
    int xSize;
    int ySize;
    int scale;
    int indent = 60;

    Field(std::string fileName, int s = 500) {
        std::ifstream fin(fileName);
        std::string text;
        std::string c;
        scale = s;
        if (!fin.is_open()) {
            std::cout << "JSON file name is invalid.\n";
            abort();
        }
        while (!fin.eof()) {
            getline(fin, c);
            text += c;
        }
        json = nlohmann::json::parse(text);
        fin.close();
        std::cout << json << std::endl;
        xSize = (int) (((float) json["FIELD_LENGTH"] - 0.6) * (float) scale);
        ySize = (int) (((float) json["FIELD_WIDTH"] - 0.6) * (float) scale);
    }

    [[nodiscard]] sf::VideoMode getWindowSize() const {
        return sf::VideoMode(xSize + indent * 2, ySize + indent * 2);
    }

    void drawField(sf::RenderWindow &window) {

        sf::RectangleShape backgroundColor;
        backgroundColor.setSize(sf::Vector2f(xSize + indent * 2, ySize + indent * 2));
        backgroundColor.setPosition(0, 0);
        backgroundColor.setFillColor(sf::Color::Green);
        window.draw(backgroundColor);

        sf::RectangleShape baseRect;
        baseRect.setSize(sf::Vector2f(xSize, ySize));
        baseRect.setPosition(indent, indent);
        baseRect.setFillColor(sf::Color::Green);
        baseRect.setOutlineColor(sf::Color::White);
        baseRect.setOutlineThickness(10);
        window.draw(baseRect);

        sf::CircleShape middleCircle;
        middleCircle.setRadius(50);
        middleCircle.setPosition(xSize / 2 + indent - 50, ySize / 2 + indent - 50);
        middleCircle.setFillColor(sf::Color::Green);
        middleCircle.setOutlineColor(sf::Color::White);
        middleCircle.setOutlineThickness(10);
        window.draw(middleCircle);

        sf::RectangleShape middleLine;
        middleLine.setSize(sf::Vector2f(10, ySize));
        middleLine.setPosition(xSize / 2 + indent - 5, indent);
        middleLine.setFillColor(sf::Color::White);
        middleLine.setOutlineColor(sf::Color::White);
        middleLine.setOutlineThickness(0);
        window.draw(middleLine);

        sf::CircleShape leftRedGoalCircle;
        leftRedGoalCircle.setRadius(10);
        leftRedGoalCircle.setPosition(xSize + indent - 10, ySize / 2 + indent - 200 - 10);
        leftRedGoalCircle.setFillColor(sf::Color::Red);
        leftRedGoalCircle.setOutlineColor(sf::Color::White);
        leftRedGoalCircle.setOutlineThickness(0);
        window.draw(leftRedGoalCircle);

        sf::CircleShape rightRedGoalCircle;
        rightRedGoalCircle.setRadius(10);
        rightRedGoalCircle.setPosition(xSize + indent - 10, ySize / 2 + indent + 200 - 10);
        rightRedGoalCircle.setFillColor(sf::Color::Red);
        rightRedGoalCircle.setOutlineColor(sf::Color::White);
        rightRedGoalCircle.setOutlineThickness(0);
        window.draw(rightRedGoalCircle);

        sf::CircleShape leftBlueGoalCircle;
        leftBlueGoalCircle.setRadius(10);
        leftBlueGoalCircle.setPosition(indent - 10, ySize / 2 + indent - 200 - 10);
        leftBlueGoalCircle.setFillColor(sf::Color::Blue);
        leftBlueGoalCircle.setOutlineColor(sf::Color::White);
        leftBlueGoalCircle.setOutlineThickness(0);
        window.draw(leftBlueGoalCircle);

        sf::CircleShape rightBlueGoalCircle;
        rightBlueGoalCircle.setRadius(10);
        rightBlueGoalCircle.setPosition(indent - 10, ySize / 2 + indent + 200 - 10);
        rightBlueGoalCircle.setFillColor(sf::Color::Blue);
        rightBlueGoalCircle.setOutlineColor(sf::Color::White);
        rightBlueGoalCircle.setOutlineThickness(0);
        window.draw(rightBlueGoalCircle);

    }

    void drawRobot(Robot &robot, sf::RenderWindow &window) {
        sf::CircleShape R;
        R.setRadius(10);
        R.setPosition(robot.x * scale - 10, robot.y * scale - 10);
        R.setFillColor(robot.color);
        window.draw(R);
        sf::VertexArray line(sf::Lines, 2);
        line[0] = sf::Vector2f(robot.x * scale, robot.y * scale);
        line[1] = sf::Vector2f(robot.x * scale + 50 * sin(robot.angle), robot.y * scale + 50 * cos(robot.angle));
        line[0].color = robot.color;
        line[1].color = sf::Color::Red;
        window.draw(line);

    }
};


int main() {
    Field field("../file.json");
    sf::RenderWindow window(field.getWindowSize(), "Robofootbal ZV");
    Robot robot;
    robot.color = sf::Color::Red;
    Robot robot1;
    robot1.color = sf::Color::Blue;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::S) {
                    robot.move(0, -0.01, field.xSize / field.scale, field.ySize / field.scale);
                }
                if (event.key.code == sf::Keyboard::W) {
                    robot.move(0, 0.01, field.xSize / field.scale, field.ySize / field.scale);
                }
                if (event.key.code == sf::Keyboard::D) {
                    robot.move(-0.01, 0, field.xSize / field.scale, field.ySize / field.scale);
                }
                if (event.key.code == sf::Keyboard::A){
                    robot.move(0.01, 0, field.xSize / field.scale, field.ySize / field.scale);
                }
                if (event.key.code == sf::Keyboard::Right) {
                    robot.rotate(-0.05);
                }
                if (event.key.code == sf::Keyboard::Left) {
                    robot.rotate(0.05);
                }

            }
        }

        field.drawField(window);
        field.drawRobot(robot, window);
        field.drawRobot(robot1, window);
        window.display();

    }
}
