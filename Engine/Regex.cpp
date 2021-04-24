#include "Regex.h"

#include <regex>

Regex::Regex(const String& expression)
    : expression(_strdup(expression.c()))
{
}

bool Regex::Check(const String& subject) const
{
    return std::regex_search(subject.c(), std::regex(expression));
}
