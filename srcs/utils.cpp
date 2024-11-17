#include "general.hpp"

/* Function has all the accepted error codes in HTTP 1.1 proctocol for this webserv project */
bool isValidError(int errorCode)
{
    int array[] = {200, 201, 202, 203, 204, 205, 206, 207, 208, 226, 300, 301, 302, 303, 304, 305, 306, 307, 308, 500, 501, 502, 503, 504, 505, 506, 507, 508, 510, 511, 400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411, 412, 413, 414, 415, 416, 417 ,418, 420, 421, 422, 423, 424, 425, 426, 428, 429, 431, 451};
    std::vector<int> valid;
    valid.assign(array, array + 60);

    for (size_t i = 0; i < valid.size(); i++)
    {
        if (valid[i] == errorCode)
            return true;
    }
    return false;
}

bool isNumeric(const std::string &str)
{
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
    {
        if (!std::isdigit(*it)) {
            return false;
        }
    }
    return true;
}