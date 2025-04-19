#ifndef TRANSFORMER_HPP
#define TRANSFORMER_HPP

#include "Shared.hpp"

class Transformer
{
private:
    pid_t pid; 
    int fd[2];


public:
    Transformer(pid_t pid, int fd[2]);
    ~Transformer();
};

Transformer::Transformer(pid_t pid, int fd[2])
                : pid(pid), fd{fd[0], fd[1]}
{
}

Transformer::~Transformer()
{
}

#endif // TRANSFORMER_HPP