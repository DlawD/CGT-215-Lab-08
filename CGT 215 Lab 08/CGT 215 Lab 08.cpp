#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include <vector>

using namespace std;
using namespace sf;
using namespace sfp;

void LoadTex(Texture& tex, string filename) {
    if (!tex.loadFromFile(filename)) {
        cout << "Could not load " << filename << endl;
    }
}

int main()
{

    RenderWindow window(VideoMode(800, 600), "Duck Hunter");
    World world(Vector2f(0, 0));
    int score(0);
    int arrows(5);

    PhysicsSprite& crossBow = *new PhysicsSprite();
    Texture cbowTex;
    LoadTex(cbowTex, "images/crossbow.png");
    crossBow.setTexture(cbowTex);
    Vector2f sz = crossBow.getSize();
    crossBow.setCenter( Vector2f (400, 600 - (sz.y / 2) ) );

    PhysicsSprite arrow;
    Texture arrowTex;
    LoadTex(arrowTex, "images/arrow.png");
    arrow.setTexture(arrowTex);
    bool drawingArrow(false);

    PhysicsRectangle top;
    top.setSize(Vector2f(800, 10));
    top.setCenter(Vector2f(400, 5));
    top.setStatic(true);
    world.AddPhysicsBody(top);

    PhysicsRectangle left;
    left.setSize(Vector2f(10, 600));
    left.setCenter(Vector2f(5, 300));
    left.setStatic(true);
    world.AddPhysicsBody(left);

    PhysicsRectangle right;
    right.setSize(Vector2f(10, 600));
    right.setCenter(Vector2f(795, 300));
    right.setStatic(true);
    world.AddPhysicsBody(right);

    Texture yellowTex;
    LoadTex(yellowTex, "images/yellow_duck.png");
    PhysicsShapeList<PhysicsSprite> Ducks;

    top.onCollision = [&drawingArrow, &world, &arrow]
    (PhysicsBodyCollisionResult result) {
        drawingArrow = false;
        world.RemovePhysicsBody(arrow);
    };

    Font font;
    if (!font.loadFromFile("arial.ttf")) {
        cout << "Couldn't load font arial.ttf" << endl;
        exit(1);
    }
    Text scoreText;
    scoreText.setFont(font);
    Text arrowCountText;
    arrowCountText.setFont(font);
    
    Clock clock;
    Time lastTime(clock.getElapsedTime());
    Time currentTime(lastTime);
    Time duckflyTime(lastTime);

    while ( (arrows > 0) || drawingArrow) {

        currentTime = clock.getElapsedTime();
        Time deltaTime = currentTime - lastTime;
        Time deltafly = currentTime - duckflyTime;
        long deltaMS = deltaTime.asMilliseconds();
        long deltaflyMS = deltafly.asMilliseconds();

        if (deltaMS > 9) {

            if (deltaflyMS > 500) {

                duckflyTime = currentTime;
                PhysicsSprite& Duck = Ducks.Create();
                Duck.setTexture(yellowTex);
                Vector2f sz = Duck.getSize();
                Duck.setCenter(Vector2f(10, 20 + (sz.y / 2)));
                Duck.setVelocity(Vector2f(0.5, 0));
                world.AddPhysicsBody(Duck);
                Duck.onCollision =
                    [&drawingArrow, &world, &arrow, &Duck, &Ducks, &score, &right]
                (PhysicsBodyCollisionResult result) {
                    if (result.object2 == arrow) {
                        drawingArrow = false;
                        world.RemovePhysicsBody(arrow);
                        world.RemovePhysicsBody(Duck);
                        Ducks.QueueRemove(Duck);
                        score += 10;
                    }
                    if (result.object2 == right) {
                        world.RemovePhysicsBody(Duck);
                        Ducks.QueueRemove(Duck);
                    }
                };

            }

            lastTime = currentTime;
            world.UpdatePhysics(deltaMS);

            //MoveCrossbow(crossBow, deltaMS);
            if (Keyboard::isKeyPressed(Keyboard::Space) &&
                !drawingArrow) {
                drawingArrow = true;
                arrow.setCenter(crossBow.getCenter());
                arrow.setVelocity(Vector2f(0, -1));
                world.AddPhysicsBody(arrow);
                arrows -= 1;
            }

            window.clear();
            if (drawingArrow) {
                window.draw(arrow);
            }
            for (PhysicsShape& Duck : Ducks) {
                window.draw((PhysicsSprite&)Duck);
            }
            window.draw(crossBow);
            scoreText.setString(to_string(score));
            FloatRect textBounds = scoreText.getGlobalBounds();
            scoreText.setPosition(
                Vector2f(790 - textBounds.width, 590 - textBounds.height));
            window.draw(scoreText);
            arrowCountText.setString(to_string(arrows));
            textBounds = arrowCountText.getGlobalBounds();
            arrowCountText.setPosition(
                Vector2f(10, 590 - textBounds.height));
            window.draw(arrowCountText);
            //world.VisualizeAllBounds(window);

            window.display();
            Ducks.DoRemovals();

        }

    }

    Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setString("GAME OVER");
    FloatRect textBounds = gameOverText.getGlobalBounds();
    gameOverText.setPosition(Vector2f(
        400 - (textBounds.width / 2),
        300 - (textBounds.height / 2)
    ));
    window.draw(gameOverText);
    window.display();
    while (true) {
        if (Keyboard::isKeyPressed(Keyboard::Space) && arrows == 0) {
            break;
        }
    }
    
}