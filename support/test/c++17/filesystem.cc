#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <string_view>

using namespace std;
namespace fs = filesystem;

const fs::path dir = fs::u8path("test__filesystem__‘dir’");
const fs::path dir_copy = fs::u8path("test__filesystem__‘dir_copy’");

const fs::path file = dir / fs::u8path("‘file’.txt");
const fs::path file_copy = dir / fs::u8path("‘file_copy’.txt");
const fs::path file_new_name = dir / fs::u8path("‘file_new’.txt");

string_view content[2] = {"test\n", "测试\n"};

bool test_create_directory() {
  error_code ec;
  fs::create_directory(dir, ec);
  if (ec) {
    cerr << "[test_create_directory] ";
    cerr << "fs::create_directory failed: " << ec.message() << '\n';
    return false;
  }
  return true;
}

bool do_write() {
  fstream ofs(file, ios::out | ios::binary);
  if (!ofs.is_open()) {
    cerr << "[do_write] ";
    cerr << "fstream::fstream failed" << '\n';
    return false;
  }
  for (const auto &line : content)
    ofs.write(line.data(), line.size());
  return true;
}

bool test_copy_file() {
  error_code ec;
  fs::copy_file(file, file_copy, fs::copy_options::overwrite_existing, ec);
  if (ec) {
    cerr << "[test_copy_file] ";
    cerr << "fs::copy_file failed: " << ec.message() << '\n';
    return false;
  }
  return true;
}

bool test_resize_file() {
  error_code ec;
  fs::resize_file(file, content[0].size(), ec);
  if (ec) {
    cerr << "[test_copy_file] ";
    cerr << "fs::resize_file failed: " << ec.message() << '\n';
    return false;
  }
  fstream ifs(file, ios::in | ios::binary);
  if (!ifs.is_open()) {
    cerr << "[test_copy_file] ";
    cerr << "fopen failed" << '\n';
    return false;
  }
  ifs.seekg(0, ios::end);
  size_t len = ifs.tellg();
  if (len != content[0].size()) {
    cerr << "[test_copy_file] ";
    cerr << "length mismatch: " << len << " != " << content[0].size() << '\n';
    return false;
  }
  ifs.seekg(0, ios::beg);
  char buf[1024];
  ifs.read(buf, len);
  if (!ifs) {
    cerr << "[test_copy_file] ";
    cerr << "fstream::read failed" << '\n';
    return false;
  }
  if (content[0] != string_view(buf, len)) {
    cerr << "[test_copy_file] ";
    cerr << "read content mismatch" << '\n';
    return false;
  }
  return true;
}

bool test_copy() {
  error_code ec;
  fs::copy(dir, dir_copy,
           fs::copy_options::overwrite_existing | fs::copy_options::recursive,
           ec);
  if (ec) {
    cerr << "[test_copy] ";
    cerr << "fs::copy failed: " << ec.message() << '\n';
    return false;
  }
  return true;
}

bool test_directory_iterator() {
  error_code ec;
  auto cwd = fs::current_path(ec);
  if (ec) {
    cerr << "[test_directory_iterator] ";
    cerr << "fs::current_path failed: " << ec.message() << '\n';
    return false;
  }

  auto dir_iter = fs::recursive_directory_iterator(".", ec);
  if (ec) {
    cerr << "[test_directory_iterator] ";
    cerr << "fs::recursive_directory_iterator failed: " << ec.message() << '\n';
    return false;
  }

  set<decltype(dir.u8string())> files;
  for (const auto &entry : dir_iter) {
    auto rel = fs::relative(entry.path(), cwd, ec);
    if (ec) {
      cerr << "[test_directory_iterator] ";
      cerr << "fs::relative failed: " << ec.message() << '\n';
      return false;
    }
    auto rel_str = rel.generic_u8string();
    if (rel_str.find("test__filesystem__") == 0)
      files.insert(rel_str);
  }
  for (const auto &file : files)
    cout << file << '\n';
  return true;
}

bool test_equivalent() {
  error_code ec;
  bool eq;

  eq = fs::equivalent(file, file, ec);
  if (ec) {
    cerr << "[test_equivalent] ";
    cerr << "fs::equivalent failed: " << ec.message() << '\n';
    return false;
  }
  if (!eq) {
    cerr << "[test_equivalent] ";
    cerr << "fs::equivalent returned false" << '\n';
    return false;
  }

  eq = fs::equivalent(file, file_copy, ec);
  if (ec) {
    cerr << "[test_equivalent] ";
    cerr << "fs::equivalent failed: " << ec.message() << '\n';
    return false;
  }
  if (eq) {
    cerr << "[test_equivalent] ";
    cerr << "fs::equivalent returned true" << '\n';
    return false;
  }
  return true;
}

bool test_rename() {
  error_code ec;
  fs::rename(file, file_new_name, ec);
  if (ec) {
    cerr << "[test_rename] ";
    cerr << "fs::rename failed: " << ec.message() << '\n';
    return false;
  }
  bool exist = fs::exists(file, ec);
  if (ec) {
    cerr << "[test_rename] ";
    cerr << "fs::exists failed: " << ec.message() << '\n';
    return false;
  }
  if (exist) {
    cerr << "[test_rename] ";
    cerr << "rename failed" << '\n';
    return false;
  }

  // check size
  size_t size = fs::file_size(file_new_name, ec);
  if (ec) {
    cerr << "[test_rename] ";
    cerr << "fs::file_size failed: " << ec.message() << '\n';
    return false;
  }
  if (size == size_t(-1)) {
    cerr << "[test_rename] ";
    cerr << "fs::file_size returned -1" << '\n';
    return false;
  }
  if (size != content[0].size()) {
    cerr << "[test_rename] ";
    cerr << "size mismatch: " << size << " != " << content[0].size() << '\n';
  }

  // check content
  fstream ifs(file_new_name, ios::in | ios::binary);
  if (!ifs.is_open()) {
    cerr << "[test_rename] ";
    cerr << "fstream::fstream failed" << '\n';
    return false;
  }
  char buf[1024];
  ifs.read(buf, size);
  if (!ifs) {
    cerr << "[test_rename] ";
    cerr << "fstream::read failed" << '\n';
    return false;
  }
  if (content[0] != string_view(buf, size)) {
    cerr << "[test_rename] ";
    cerr << "read content mismatch" << '\n';
    return false;
  }
  return true;
}

bool test_remove() {
  error_code ec;
  fs::remove(file_copy, ec);
  if (ec) {
    cerr << "[test_remove] ";
    cerr << "fs::remove failed: " << ec.message() << '\n';
    return false;
  }
  bool exist = fs::exists(file_copy, ec);
  if (ec) {
    cerr << "[test_remove] ";
    cerr << "fs::exists failed: " << ec.message() << '\n';
    return false;
  }
  if (exist) {
    cerr << "[test_remove] ";
    cerr << "failed" << '\n';
    return false;
  }

  fs::remove_all(dir, ec);
  if (ec) {
    cerr << "[test_remove] ";
    cerr << "fs::remove_all failed: " << ec.message() << '\n';
    return false;
  }
  exist = fs::exists(dir, ec);
  if (ec) {
    cerr << "[test_remove] ";
    cerr << "fs::exists failed: " << ec.message() << '\n';
    return false;
  }

  return true;
}

int main() {
  if (!test_create_directory())
    return 1;
  if (!do_write())
    return 1;
  if (!test_copy_file())
    return 1;
  if (!test_resize_file())
    return 1;
  if (!test_copy())
    return 1;
  if (!test_directory_iterator())
    return 1;
  if (!test_equivalent())
    return 1;
  if (!test_rename())
    return 1;
  if (!test_remove())
    return 1;
  return 0;
}
