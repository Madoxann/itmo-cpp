#include "phonebook.h"
#include <iostream>

Phonebook::Phonebook() : Phonebook("", "", "", 0){};

Phonebook::Phonebook(std::string name, std::string second_name, std::string patron, size_t ph)
{
	firstName = name;
	secondName = second_name;
	patronymic = patron;
	phone = ph;
}

bool Phonebook::operator<(const Phonebook &arg)
{
	if (this->firstName == arg.firstName)
	{
		if (this->secondName == arg.secondName)
		{
			if (this->patronymic == arg.patronymic)
				return this->phone < arg.phone;
			return this->patronymic < arg.patronymic;
		}
		return this->secondName < arg.secondName;
	}
	return this->firstName < arg.firstName;
}

bool Phonebook::operator>(const Phonebook &arg)
{
	if (this->firstName == arg.firstName && this->secondName == arg.secondName && this->patronymic == arg.patronymic &&
		this->phone == arg.phone)
		return false;
	return !(*this < arg);
}

std::istream &operator>>(std::istream &in, Phonebook &arg)
{
	return in >> arg.firstName >> arg.secondName >> arg.patronymic >> arg.phone;
}

std::ostream &operator<<(std::ostream &out, const Phonebook &arg)
{
	return out << arg.firstName << " " << arg.secondName << " " << arg.patronymic << " " << arg.phone;
}
