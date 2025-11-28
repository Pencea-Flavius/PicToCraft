#ifndef OOP_GAMEMODE_H
#define OOP_GAMEMODE_H

#include <iostream>
#include <memory>

enum class GameModeType { Score, Mistakes, Time, Torch, Spiders };

struct GameConfig {
    GameModeType baseMode = GameModeType::Score;
    bool timeMode = false;
    bool torchMode = false;
    bool spidersMode = false;
};

namespace sf {
    class RenderWindow;
    class Event;
} // namespace sf

class GameMode {
protected:
    int score;
    int mistakes;
    static int totalGameModesCreated;
    static int activeGameModes;

public:
    GameMode();

    virtual ~GameMode();

    virtual void onBlockToggled(bool isCorrect, bool isCompleted,
                                bool wasCompleted);

    [[nodiscard]] virtual bool isLost() const = 0;

    [[nodiscard]] virtual int getMaxMistakes() const { return 0; }

    [[nodiscard]] virtual bool shouldDisplayScore() const = 0;

    virtual void update(float deltaTime) {
    }

    virtual void draw(class sf::RenderWindow &window) const {
    }

    virtual bool handleInput(const sf::Event &event,
                             const sf::RenderWindow &window) {
        return false;
    }

    [[nodiscard]] virtual int getScore() const;

    [[nodiscard]] virtual int getMistakes() const;

    [[nodiscard]] virtual std::unique_ptr<GameMode> clone() const = 0;

    [[nodiscard]] virtual std::string getName() const = 0;

    virtual void print(std::ostream &os) const {
        os << "GameMode (Score: " << score << ", Mistakes: " << mistakes << ")";
    }

    friend std::ostream &operator<<(std::ostream &os, const GameMode &gm) {
        gm.print(os);
        return os;
    }

    virtual void setGrid(class Grid *g) {
    }

    virtual void setRenderer(const class GridRenderer *r) {
    }

    void reset();
};

class GameModeDecorator : public GameMode {
protected:
    std::unique_ptr<GameMode> wrappedMode;

public:
    explicit GameModeDecorator(std::unique_ptr<GameMode> mode)
        : wrappedMode(std::move(mode)) {
    }

    ~GameModeDecorator() override = default;

    [[nodiscard]] std::unique_ptr<GameMode> clone() const override = 0;

    void setGrid(Grid *g) override {
        if (wrappedMode)
            wrappedMode->setGrid(g);
    }

    void setRenderer(const GridRenderer *r) override {
        if (wrappedMode)
            wrappedMode->setRenderer(r);
    }

    void onBlockToggled(bool isCorrect, bool isCompleted,
                        bool wasCompleted) override {
        if (wrappedMode) {
            wrappedMode->onBlockToggled(isCorrect, isCompleted, wasCompleted);
        }
    }

    [[nodiscard]] bool isLost() const override {
        return wrappedMode ? wrappedMode->isLost() : false;
    }

    [[nodiscard]] int getMaxMistakes() const override {
        return wrappedMode ? wrappedMode->getMaxMistakes() : 0;
    }

    [[nodiscard]] bool shouldDisplayScore() const override {
        return wrappedMode ? wrappedMode->shouldDisplayScore() : false;
    }

    void update(float deltaTime) override {
        if (wrappedMode) {
            wrappedMode->update(deltaTime);
        }
    }

    void draw(sf::RenderWindow &window) const override {
        if (wrappedMode) {
            wrappedMode->draw(window);
        }
    }

    bool handleInput(const sf::Event &event,
                     const sf::RenderWindow &window) override {
        if (wrappedMode) {
            return wrappedMode->handleInput(event, window);
        }
        return false;
    }

    [[nodiscard]] int getScore() const override {
        return wrappedMode ? wrappedMode->getScore() : 0;
    }

    [[nodiscard]] int getMistakes() const override {
        return wrappedMode ? wrappedMode->getMistakes() : 0;
    }

    void print(std::ostream &os) const override {
        if (wrappedMode) {
            wrappedMode->print(os);
        }
    }

    [[nodiscard]] std::string getName() const override {
        return wrappedMode ? wrappedMode->getName() : "Decorator";
    }

    [[nodiscard]] GameMode *getWrappedMode() const { return wrappedMode.get(); }
};

#endif // OOP_GAMEMODE_H