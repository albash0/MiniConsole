#pragma once
#include <cstdint>
#include <cstring>
#include <map>
#include <string>
#include <vector>

// Flash survives object destruction/recreation, allowing reboot tests.
struct Preferences {
  inline static std::map<std::string, std::vector<uint8_t>> flash;
  inline static bool failNextWrite = false;
  inline static uint32_t blobWrites = 0;
  std::string name;
  bool begin(const char *ns, bool) { name = ns; return true; }
  void end() { name.clear(); }
  std::string key(const char *k) const { return name + "/" + k; }
  bool remove(const char *k) { return flash.erase(key(k)) != 0; }
  size_t getBytesLength(const char *k) { auto it = flash.find(key(k)); return it == flash.end() ? 0 : it->second.size(); }
  size_t getBytes(const char *k, void *data, size_t capacity) {
    auto it = flash.find(key(k));
    if (it == flash.end() || capacity < it->second.size()) return 0;
    memcpy(data, it->second.data(), it->second.size()); return it->second.size();
  }
  size_t putBytes(const char *k, const void *data, size_t size) {
    if (failNextWrite) { failNextWrite = false; return 0; }
    auto start = static_cast<const uint8_t *>(data);
    flash[key(k)] = std::vector<uint8_t>(start, start + size); ++blobWrites; return size;
  }
  template<class T> T get(const char *k, T d) { T n; return getBytes(k, &n, sizeof(n)) == sizeof(n) ? n : d; }
  template<class T> size_t put(const char *k, T v) { return putBytes(k, &v, sizeof(v)); }
  uint8_t getUChar(const char *k, uint8_t d) { return get(k, d); }
  bool getBool(const char *k, bool d) { return get(k, d); }
  uint16_t getUShort(const char *k, uint16_t d) { return get(k, d); }
  uint32_t getUInt(const char *k, uint32_t d) { return get(k, d); }
  size_t putUChar(const char *k, uint8_t v) { return put(k, v); }
  size_t putBool(const char *k, bool v) { return put(k, v); }
  size_t putUShort(const char *k, uint16_t v) { return put(k, v); }
  size_t putUInt(const char *k, uint32_t v) { return put(k, v); }
};
