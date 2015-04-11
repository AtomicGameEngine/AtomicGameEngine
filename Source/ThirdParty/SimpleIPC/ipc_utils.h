// Copyright (c) 2010 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SIMPLE_IPC_UTLIS_H_
#define SIMPLE_IPC_UTLIS_H_

#include <string.h>
#include <wchar.h>
#include <new>

#if !defined(countof)
  template <typename T, size_t N>
  char ( &_ArraySizeHelperX( T (&arr)[N] ))[N];
  #define countof(arr) (sizeof(_ArraySizeHelperX(arr)))
#endif

#if !defined(swap)
  template <typename T>
  void swap(T& a, T& b) { T t(a); a = b; b = t; }
#endif

namespace memdet {

template <typename T> T* new_impl(size_t n) {
  return new T[n];
}

template <typename T> void delete_impl(T* o) {
  delete[] o;
}


}  // namespace memdet

namespace ipc {

// The following classes are necessary because in some enviroments where this
// library is used STL is forbidden. In particular HolderString is designed
// to have the same functional interface as std::basic_string for the methods
// that the ipc library uses.

// This container class is an array-like which contains enough functionality
// to replace vector<WireType> in the channel template. This is the only class
// here that supports T to be non-pod and also T does not need a default ctor.
template <typename T, size_t N>
class FixedArray {
public:
  FixedArray() : index_(0) {}

  ~FixedArray() {
    clear();
  }

  bool push_back(const T& ob) {
    if (index_ == N)
      return false;
    new(as_obj(index_)) T(ob);
    ++index_;
    return true;
  }

  T& operator[](size_t ix) {
    return *as_obj(ix);
  }

  size_t max_size() const { return N; }

  size_t size() const { return index_; }

  void clear() {
    for (size_t ix = 0; ix != index_; ++ix) {
      as_obj(ix)->~T();
    }
    index_ = 0;
  }

private:
  T* as_obj(size_t ix) { 
    return reinterpret_cast<T*>(&v_[ix * sizeof(T)]);
  }

  size_t index_;
  char v_[N * sizeof(T)];

  FixedArray(const FixedArray&);
  FixedArray& operator=(const FixedArray&);
};


// We don't support generic iterators but we define this magic two
// types to support insertion to the end and erasure at the beggining.
class IteratorEnd {};
class IteratorBegin {};

template <typename T>
T operator+(const IteratorBegin&, T d) {
  return d;
}

// This container is the backing store of HoldeString and a generic
// vector of plain-old-data. Caveat: Don't use this if your PoD does
// not have an aceptable default value of 0 as in all bytes equal to
// zero.
template <typename T>
class PodVector {
public:
  typedef T value_type;

  PodVector() : capa_(0), size_(0), buf_(0) {}

  ~PodVector() {
    clear();
  }

  const IteratorBegin begin() const { return IteratorBegin(); }

  const IteratorEnd end() const { return IteratorEnd(); }

  T* get() const { return buf_; }

  size_t size() const { return size_; }

  size_t capacity() const { return capa_; }

  T& operator[](size_t ix) {
    return buf_[ix];
  }

  const T& operator[](size_t ix) const {
    return buf_[ix];
  }

  void clear() {
    memdet::delete_impl(buf_);
    size_ = 0;
    capa_ = 0;
    buf_ = 0;
  }

  void resize(size_t n) {
    if (n < size_) {
      size_ = n;
      return;
    }
    Add(0, n - size_);
  }

  void reserve(size_t n) {
    if (n <= capa_)
      return;
    size_t old_s = size_;
    Add(0, n - capa_);
    size_ = old_s;
  }

  void push_back(const T& v) {
    Add(&v, 1);
  }

  // We only support insertions at the end so here we are
  void insert(const IteratorEnd&, const T* begin, const T* end) {
    size_t d = end - begin;
    if (d > 0)
      Add(begin, end - begin);
  }

  void erase(const IteratorBegin&, size_t n) {
    RemoveFront(n);
  }

  // when |inp| is null then we don't copy, we just set
  // the new memory to zeros.
  void Add(const T* inp, size_t n) {
    if (!n)
      return;
    
    T* newb = NewAlloc(n);
    if (newb) {
      memcpy(newb, buf_, size_ * sizeof(T));
      memdet::delete_impl(buf_);
      buf_ = newb;
    }
    if (inp) {
      memcpy(&buf_[size_], inp, n * sizeof(T));
    } else {
      memset(&buf_[size_], 0, n * sizeof(T));
    }
    size_ += n;
  }

  void RemoveFront(size_t n) {
    if ((0 == size_) || (n > size_))
      return;
    size_t newsz = size_ - n;
    memmove(buf_, &buf_[n], newsz);
    size_ = newsz;
  }

  void Set(const T* inp, size_t n) {
    size_ = 0;
    Add(inp, n);
  }

  void Set(const PodVector<T>& other) {
    Set(other.buf_, other.size_);
  }

  void Swap(PodVector<T>& other) {
    swap(buf_, other.buf_);
    swap(size_, other.size_);
    swap(capa_, other.capa_);
  }

  void DecSize() {
    --size_;
  }
  
private:
  T* NewAlloc(size_t n) {
    size_t rem = capa_ - size_;
    if (n < rem) {
      return 0;
    }
    size_t new_a = n + (size_ * 2) + 1;
    capa_ = (new_a < 16)? 16 : new_a;
    return memdet::new_impl<T>(capa_);
  }

  PodVector(const PodVector&);
  PodVector& operator=(const PodVector&);

  size_t capa_;
  size_t size_;
  T* buf_;
};

// Groups common functionality to the specializations of HolderString below.
// One trick one should be aware on this class is that leverages the fact that
// PodVector overallocates always. So it is safe to write to str_[size_] for
// example to null terminate.
template <typename Ct, typename Derived>
class StringBase {
public:
  typedef Ct value_type;

  StringBase(const StringBase& rhs) {
    assign(rhs.str_.get(), rhs.str_.size());
  }

  StringBase<Ct, Derived>& operator=(const StringBase<Ct, Derived>& rhs) {
    assign(rhs.str_.get(), rhs.str_.size());
    return *this;
  }

  void assign(const Ct* str, size_t size) {
    str_.Set(str, size);
    if (size)
      str_[size] = Ct(0);
  }

  size_t size() const {
    return str_.size();
  }

  void swap(StringBase<Ct, Derived>& other) {
    str_.Swap(other.str_);
  }

  bool operator==(const Ct* rhs) const {
    return (0 == static_cast<const Derived*>(this)->Compare(rhs));
  }

  bool operator!=(const Ct* rhs) const {
    return (0 != static_cast<const Derived*>(this)->Compare(rhs));
  }

  Ct& operator[](size_t ix) {
    return str_[ix];
  }

  const Ct& operator[](size_t ix) const {
    return str_[ix];
  }

  size_t capacity() const { return str_.capacity(); }

protected:
  StringBase() {}

  PodVector<Ct> str_;
};

template <typename Ct>
class HolderString;

// Two specializations of HolderString for char and wchar_t that
// have enough functionality to replace (if desired) the use of the
// standard basic_string.
template <>
class HolderString<char> : public StringBase<char, HolderString<char> > {
public:
  HolderString() {}

  HolderString(const char* str) {
    operator=(str);
  }

  void operator=(const char* str) {
    assign(str, strlen(str));
  }

  void append(const char* str) {
    str_.Add(str, strlen(str) + 1);
    str_.DecSize();
  }

  const char* c_str() const {
    if (!str_.get())
      return "";
    return str_.get(); 
  }

  int Compare(const char* str) const {
    return strcmp(c_str(), str);
  }
  
};

template <>
class HolderString<wchar_t> : public StringBase<wchar_t, HolderString<wchar_t> > {
public:
  HolderString() {}

  HolderString(const wchar_t* str) {
    operator=(str);
  }

  void operator=(const wchar_t* str) {
    assign(str, wcslen(str));
  }

  void append(const wchar_t* str) {
    str_.Add(str, wcslen(str) + 1);
    str_.DecSize();
  }

  const wchar_t* c_str() const {
    if (!str_.get())
      return L"";
    return str_.get(); 
  }

  int Compare(const wchar_t* str) const {
    return wcscmp(c_str(), str);
  }
};

}  // namespace ipc.

#endif // SIMPLE_IPC_UTLIS_H_
