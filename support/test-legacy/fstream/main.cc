#include <fstream>
#include <iostream>
#include <string_view>
using namespace std;

// construct `fstream` with `filesystem::path::value_type *`
bool test(const wchar_t *filename) {
  static string_view content = "test 测试\r\n";
  {
    ofstream ofs(filename, ios::binary);
    if (!ofs.is_open()) {
      cerr << "[fstream] cannot open wide file name for write: " << filename
           << endl;
      return false;
    }
    ofs.write(content.data(), content.size());
  }
  {
    ifstream ifs(filename, ios::binary);
    if (!ifs.is_open()) {
      cerr << "[fstream] cannot open wide file name for read: " << filename
           << endl;
      return false;
    }
    ifs.seekg(0, ios::end);
    size_t len = ifs.tellg();
    if (len != content.size()) {
      cerr << "[fstream] read length mismatch: " << len
           << " != " << content.size() << endl;
      return false;
    }
    ifs.seekg(0, ios::beg);
    char buf[1024];
    ifs.read(buf, len);
    if (content != string_view(buf, len)) {
      cerr << "[fstream] read content mismatch" << endl;
      return false;
    }
  }
  return true;
}

int main() {
  if (!test(L"test.txt"))
    return 1;
  if (!test(L"测试.txt"))
    return 1;
  return 0;
}
