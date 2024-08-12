#include <cstdio>
#include <filesystem>
#include <iostream>
#include <set>
using namespace std;
namespace fs = std::filesystem;

string_view content[2] = {"test\r\n", "测试\r\n"};

bool test_create_directory() {
  error_code ec;
  fs::create_directory("test", ec);
  if (ec)
    cerr << "[filesystem:test_create_directory] create_directory failed: "
         << ec.message() << endl;
  return !ec;
}

bool do_write() {
  FILE *fp = fopen("test/test.txt", "wb");
  if (!fp) {
    cerr << "[filesystem:do_write] do_write fopen failed" << endl;
    return false;
  }
  for (const auto &line : content)
    fwrite(line.data(), 1, line.size(), fp);
  fclose(fp);
  return true;
}

bool test_copy_file() {
  error_code ec;
  fs::copy_file("test/test.txt", L"test/测试.txt",
                fs::copy_options::overwrite_existing, ec);
  if (ec)
    cerr << "[filesystem:test_copy_file] copy_file failed: " << ec.message()
         << endl;
  return !ec;
}

bool test_resize_file() {
  error_code ec;
  fs::resize_file("test/test.txt", content[0].size(), ec);
  if (ec) {
    cerr << "[filesystem:test_resize_file] resize_file failed: " << ec.message()
         << endl;
    return false;
  }
  FILE *fp = fopen("test/test.txt", "rb");
  if (!fp) {
    cerr << "[filesystem:test_resize_file] test_resize_file fopen failed"
         << endl;
    return false;
  }
  fseek(fp, 0, SEEK_END);
  size_t len = ftell(fp);
  if (len != content[0].size()) {
    cerr << "[filesystem:test_resize_file] test_resize_file length mismatch: "
         << len << " != " << content[0].size() << endl;
    return false;
  }
  fseek(fp, 0, SEEK_SET);
  char buf[1024];
  fread(buf, 1, len, fp);
  fclose(fp);
  if (content[0] != string_view(buf, len)) {
    cerr << "[filesystem:test_resize_file] test_resize_file read content "
            "mismatch"
         << endl;
    return false;
  }
  return true;
}

bool test_copy() {
  error_code ec;
  fs::copy("test", L"测试",
           fs::copy_options::overwrite_existing | fs::copy_options::recursive,
           ec);
  if (ec)
    cerr << "[filesystem:test_copy] copy failed: " << ec.message() << endl;
  return !ec;
}

bool test_directory_iterator() {
  error_code ec;
  auto cwd = fs::current_path(ec);
  if (ec) {
    cerr << "[filesystem:test_copy] current_path failed: " << ec.message()
         << endl;
    return false;
  }

  auto dir = fs::recursive_directory_iterator(".", ec);
  if (ec) {
    cerr << "[filesystem:test_copy] recursive_directory_iterator failed: "
         << ec.message() << endl;
    return false;
  }

  set<fs::path> files;
  for (const auto &entry : dir) {
    auto rel = fs::relative(entry.path(), cwd, ec);
    if (ec) {
      cerr << "[filesystem:test_copy] relative failed: " << ec.message()
           << endl;
      return false;
    }
    files.insert(rel);
  }
  for (const auto &file : files)
    cout << file << endl;
  return true;
}

bool test_equivalent() {
  error_code ec;
  bool eq;

  eq = fs::equivalent("test/test.txt", "test\\test.txt", ec);
  if (ec) {
    cerr << "[filesystem:test_equivalent] equivalent failed: " << ec.message()
         << endl;
    return false;
  }
  if (!eq) {
    cerr << "[filesystem:test_equivalent] equivalent returned false" << endl;
    return false;
  }

  eq = fs::equivalent("test/test.txt", "test/测试.txt", ec);
  if (ec) {
    cerr << "[filesystem:test_equivalent] equivalent failed: " << ec.message()
         << endl;
    return false;
  }
  if (eq) {
    cerr << "[filesystem:test_equivalent] equivalent returned true" << endl;
    return false;
  }
  return true;
}

bool test_rename() {
  error_code ec;
  fs::rename("test/test.txt", "test/test2.txt", ec);
  if (ec) {
    cerr << "[filesystem:test_rename] rename failed: " << ec.message() << endl;
    return false;
  }
  bool exist = fs::exists("test/test.txt", ec);
  if (ec) {
    cerr << "[filesystem:test_rename] exists failed: " << ec.message() << endl;
    return false;
  }
  if (exist) {
    cerr << "[filesystem:test_rename] test_rename failed" << endl;
    return false;
  }

  // check size
  size_t size = fs::file_size("test/test2.txt", ec);
  if (ec) {
    cerr << "[filesystem:test_rename] file_size failed: " << ec.message()
         << endl;
    return false;
  }
  if (size == size_t(-1)) {
    cerr << "[filesystem:test_rename] file_size returned -1" << endl;
    return false;
  }
  if (size != content[0].size()) {
    cerr << "[filesystem:test_rename] size mismatch: " << size
         << " != " << content[0].size() << endl;
  }

  // check content
  FILE *fp = fopen("test/test2.txt", "rb");
  if (!fp) {
    cerr << "[filesystem:test_rename] fopen failed" << endl;
    return false;
  }
  char buf[1024];
  size_t len = fread(buf, 1, size, fp);
  if (len != size) {
    cerr << "[filesystem:test_rename] fread failed" << endl;
    return false;
  }
  fclose(fp);
  if (content[0] != string_view(buf, len)) {
    cerr << "[filesystem:test_rename] read content mismatch" << endl;
    return false;
  }
  return true;
}

bool test_remove() {
  error_code ec;
  fs::remove(L"test/测试.txt", ec);
  if (ec) {
    cerr << "[filesystem:test_remove] remove failed: " << ec.message() << endl;
    return false;
  }
  bool exist = fs::exists(L"test/测试.txt", ec);
  if (ec) {
    cerr << "[filesystem:test_remove] exists failed: " << ec.message() << endl;
    return false;
  }
  if (exist) {
    cerr << "[filesystem:test_remove] failed" << endl;
    return false;
  }

  fs::remove_all("test", ec);
  if (ec) {
    cerr << "[filesystem:test_remove] remove_all failed: " << ec.message()
         << endl;
    return false;
  }
  exist = fs::exists("test", ec);
  if (ec) {
    cerr << "[filesystem:test_remove] exists failed: " << ec.message() << endl;
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
