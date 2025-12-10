#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <string>

class GameException : public std::runtime_error {
public:
  explicit GameException(const std::string &message)
      : std::runtime_error("Game Error: " + message) {}
};

class FileLoadException : public GameException {
public:
  explicit FileLoadException(const std::string &filename)
      : GameException("Failed to load file: " + filename) {}
};

class InvalidGridException : public GameException {
public:
  explicit InvalidGridException(const std::string &details)
      : GameException("Invalid grid operation: " + details) {}
};

class AssetLoadException : public GameException {
public:
  explicit AssetLoadException(const std::string &filename,
                              const std::string &type = "Asset")
      : GameException("Failed to load " + type + " from file: " + filename) {}
};

class GameStateException : public GameException {
public:
  explicit GameStateException(const std::string &details)
      : GameException("Invalid game state: " + details) {}
};

class OutOfBoundsException : public GameException {
public:
  explicit OutOfBoundsException(const std::string &details)
      : GameException("Out of bounds access: " + details) {}
};

#endif // EXCEPTIONS_H
