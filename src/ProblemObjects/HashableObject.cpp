//
// Created by Максим Процик on 15.04.2023.
//
#include "HashableObject.h"

size_t HashableObject::GetHash() const
{
    if (!m_hashCalculated)
    {
        m_hash = CalculateHash();
        m_hashCalculated = true;
    }
    return m_hash;
}

void HashableObject::InvalidateHash() const
{
    m_hashCalculated = false;
}
