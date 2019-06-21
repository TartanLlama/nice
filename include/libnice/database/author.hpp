#pragma once

#include <string>
#include <ctime>
#include <fmt/format.h>
#include <fmt/time.h>

namespace noice::database
{
/**
 *  Represents the author of a commit
 *  Stores their name, email, and time of commit
 */
class author
{
  public:
    author(std::string name, std::string email, std::time_t time)
        : name_(std::move(name)), email_(std::move(email)), time_(time)
    {
    }

    /**
     * Authors format to:
     *  <name> \<<email>\> <unix timestamp> <UTC offset>
     */
    friend std::string to_string(author const &author)
    {
        return fmt::format("{} <{}> {} {:%z}", author.name_, author.email_, author.time_, *std::localtime(&author.time_));
    }

  private:
    std::string name_;
    std::string email_;
    std::time_t time_;
};
} // namespace noice::database