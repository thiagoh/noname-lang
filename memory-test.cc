#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <memory>

class Error {
 private:
  char* _what;
  char* _whatcopied;
  std::string _whatstr;

 public:
  explicit Error() : _what(0), _whatstr("") {}
  virtual ~Error() = default;
  const char* what() const { return _what; }
  const char* whatcopied() const { return _whatcopied; }
  const std::string whatstr() const { return _whatstr; }
  void what(char* what) {
    this->_what = what;
    this->_whatcopied = (char*)calloc(strlen(_what), sizeof(char));
    strcpy(this->_whatcopied, what);
    this->_whatstr = (std::string(what));
  }
};

void create(Error& error) {
  char msg[256];
  sprintf(msg, "My error message: '%s'", "Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
  error.what(msg);
  std::cout << error.what() << "\n";
}

void doit() {
  Error error;
  create(error);
  std::cout << error.what() << "\n";
  std::cout << error.whatstr() << "\n";
  std::cout << error.whatcopied() << "\n";
}

int main(int args, char** argv) {
  doit();
  return 0;
}
