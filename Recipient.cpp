#include "Recipient.h"
#include <iostream>

// [INHERITANCE]
RecipientOrganization::RecipientOrganization(const std::string& id, const std::string& name, const std::string& contact, const std::string& location)
    : Person(id, name, contact), m_location(location) {}

// [POLYMORPHISM]
void RecipientOrganization::display() const {
    std::cout << "Recipient ID: " << getId()
              << " | Org Name: " << getName()
              << " | Contact: " << getContact()
              << " | Location: " << m_location << "\n";
}
