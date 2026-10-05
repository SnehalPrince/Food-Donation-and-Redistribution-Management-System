#include "Donor.h"
#include <iostream>

// [INHERITANCE]
Donor::Donor(const std::string& id, const std::string& name, const std::string& contact, const std::string& source)
    : Person(id, name, contact), m_source(source) {}

// [POLYMORPHISM]
void Donor::display() const {
    std::cout << "Donor ID: " << getId()
              << " | Name: " << getName()
              << " | Contact: " << getContact()
              << " | Source: " << m_source << "\n";
}
