#include "database.hh"

namespace ccqlite
{
database::database(std::string const& filePath)
    : pDatabase(nullptr)
{}

database::~database()
{
    int const ret = sqlite3_close(pDatabase);
    pDatabase = nullptr;
}
}
