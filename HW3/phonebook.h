#pragma once

#include <string>

class Phonebook
{
	std::string firstName;
	std::string secondName;
	std::string patronymic;
	size_t phone;

  public:
	Phonebook();

	Phonebook(std::string name, std::string second_name, std::string patron, size_t ph);

	bool operator<(const Phonebook &arg);

	bool operator>(const Phonebook &arg);

	friend std::istream &operator>>(std::istream &in, Phonebook &arg);

	friend std::ostream &operator<<(std::ostream &out, const Phonebook &arg);
};
