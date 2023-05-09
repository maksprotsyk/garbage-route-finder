//
// Created by Максим Процик on 15.04.2023.
//

#ifndef ROUTING_PROBLEM_HASHABLEOBJECT_H
#define ROUTING_PROBLEM_HASHABLEOBJECT_H

#include <cstddef>

#include <boost/functional/hash.hpp>

class HashableObject
{
public:
    size_t GetHash() const;
    void InvalidateHash() const;
protected:
    virtual size_t CalculateHash() const = 0;
private:
    mutable bool m_hashCalculated = false;
    mutable size_t m_hash = 0;
};


#endif //ROUTING_PROBLEM_HASHABLEOBJECT_H
