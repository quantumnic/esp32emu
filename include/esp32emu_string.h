#pragma once
#include <string>
#include <cstring>
#include <cstdlib>

class String {
public:
    String() {}
    String(const char* s) : buf_(s ? s : "") {}
    String(const std::string& s) : buf_(s) {}
    String(char c) : buf_(1, c) {}
    String(int n)  : buf_(std::to_string(n)) {}
    String(unsigned int n) : buf_(std::to_string(n)) {}
    String(long n) : buf_(std::to_string(n)) {}
    String(unsigned long n) : buf_(std::to_string(n)) {}
    String(float n, unsigned char dec = 2) { char b[32]; snprintf(b,sizeof(b),"%.*f",dec,n); buf_=b; }
    String(double n, unsigned char dec = 2) { char b[32]; snprintf(b,sizeof(b),"%.*f",dec,n); buf_=b; }

    const char* c_str() const { return buf_.c_str(); }
    unsigned int length() const { return (unsigned int)buf_.size(); }
    bool isEmpty() const { return buf_.empty(); }

    String& operator=(const char* s) { buf_ = s ? s : ""; return *this; }
    String& operator=(const String& s) { buf_ = s.buf_; return *this; }

    String operator+(const String& o) const { return String(buf_ + o.buf_); }
    String operator+(const char* s) const { return String(buf_ + (s?s:"")); }
    String operator+(char c) const { return String(buf_ + std::string(1,c)); }
    friend String operator+(const char* l, const String& r) { return String(std::string(l?l:"") + r.buf_); }

    String& operator+=(const String& o) { buf_ += o.buf_; return *this; }
    String& operator+=(const char* s) { if(s) buf_ += s; return *this; }
    String& operator+=(char c) { buf_ += c; return *this; }

    bool operator==(const String& o) const { return buf_ == o.buf_; }
    bool operator==(const char* s) const { return buf_ == (s?s:""); }
    bool operator!=(const String& o) const { return buf_ != o.buf_; }
    bool operator<(const String& o) const { return buf_ < o.buf_; }

    char charAt(unsigned int i) const { return buf_[i]; }
    void setCharAt(unsigned int i, char c) { buf_[i] = c; }
    char operator[](unsigned int i) const { return buf_[i]; }
    char& operator[](unsigned int i) { return buf_[i]; }

    int indexOf(char c, unsigned int from = 0) const {
        auto p = buf_.find(c, from);
        return p == std::string::npos ? -1 : (int)p;
    }
    int indexOf(const String& s, unsigned int from = 0) const {
        auto p = buf_.find(s.buf_, from);
        return p == std::string::npos ? -1 : (int)p;
    }
    int lastIndexOf(char c) const {
        auto p = buf_.rfind(c);
        return p == std::string::npos ? -1 : (int)p;
    }

    String substring(unsigned int from) const { return String(buf_.substr(from)); }
    String substring(unsigned int from, unsigned int to) const { return String(buf_.substr(from, to - from)); }

    bool startsWith(const String& s) const { return buf_.rfind(s.buf_, 0) == 0; }
    bool endsWith(const String& s) const {
        if (s.buf_.size() > buf_.size()) return false;
        return buf_.compare(buf_.size() - s.buf_.size(), s.buf_.size(), s.buf_) == 0;
    }

    void trim() {
        auto a = buf_.find_first_not_of(" \t\r\n");
        auto b = buf_.find_last_not_of(" \t\r\n");
        if (a == std::string::npos) buf_.clear();
        else buf_ = buf_.substr(a, b - a + 1);
    }

    void toLowerCase() { for (auto& c : buf_) c = tolower(c); }
    void toUpperCase() { for (auto& c : buf_) c = toupper(c); }

    long toInt() const { return std::atol(buf_.c_str()); }
    float toFloat() const { return std::atof(buf_.c_str()); }
    double toDouble() const { return std::atof(buf_.c_str()); }

    void replace(const String& from, const String& to) {
        size_t pos = 0;
        while ((pos = buf_.find(from.buf_, pos)) != std::string::npos) {
            buf_.replace(pos, from.buf_.size(), to.buf_);
            pos += to.buf_.size();
        }
    }

    void remove(unsigned int idx, unsigned int cnt = 1) { buf_.erase(idx, cnt); }
    void reserve(unsigned int sz) { buf_.reserve(sz); }

    bool equalsIgnoreCase(const String& o) const {
        if (buf_.size() != o.buf_.size()) return false;
        for (size_t i = 0; i < buf_.size(); i++)
            if (tolower(buf_[i]) != tolower(o.buf_[i])) return false;
        return true;
    }

    // Conversion to std::string
    operator std::string() const { return buf_; }

private:
    std::string buf_;
};
