#pragma once

#include <string>

#include <sqlite3.h>

#include "ccqliteapi.hh"

namespace ccqlite
{
class CCQLITE_API database
{
  public:
    database() = delete;
    database(std::string const& filePath);
    ~database();

  private:
    sqlite3* pDatabase;
};
} // namespace MyNamespace
