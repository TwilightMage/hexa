#include "Regex.h"

#include <regex>

Regex::Regex(const String& expression)
    : expression_(_strdup(expression.c()))
{
}

bool Regex::check(const String& subject) const
{
    return std::regex_search(subject.c(), std::regex(expression_));
}
