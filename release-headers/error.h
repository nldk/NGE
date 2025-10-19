#ifndef NGE_ERROR_H
#define NGE_ERROR_H
#include <cstdlib>
#include <string>
class Error {
public:
    bool error;
    std::string message;
    Error(bool error,std::string message) {
        this->error = error;
        this->message = message;
    };
    ~Error() = default;
};


#endif