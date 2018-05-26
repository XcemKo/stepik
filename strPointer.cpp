class StringPointer {
private:
	char* ptr;
	std::string* string_ptr;
	int lenght;
public:
	std::string *operator->() {
		return string_ptr;
	}

	operator std::string*() {
		return string_ptr;
	}

	StringPointer(std::string *Pointer) {
		if (Pointer != NULL)
			string_ptr = Pointer;
		else
			string_ptr = new std::string();
	}
	~StringPointer() {}
};