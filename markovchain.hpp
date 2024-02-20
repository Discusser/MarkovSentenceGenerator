#pragma once

#include <map>
#include <string>

class WordPair {
 public:
  WordPair() : from(), to() {}
  WordPair(std::string from, std::string to);
  std::string from;
  std::string to;
};

class WordPairComparator {
 public:
  bool operator()(const WordPair &lhs, const WordPair &rhs) const {
    return lhs.from > rhs.from ? true : lhs.from == rhs.from ? lhs.to > rhs.to : false;
  }
};

class MarkovChain {
 public:
  MarkovChain() : model() {}
  static MarkovChain from_text(std::string filename);
  static MarkovChain from_json(std::string filename);
  void export_to_json(std::string filename);
  std::map<WordPair, double, WordPairComparator> model;
};
