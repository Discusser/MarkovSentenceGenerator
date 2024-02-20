#include <fstream>
#include <ios>
#include <iostream>
#include <regex>
#include <string>

#include "./markovchain.hpp"

WordPair::WordPair(std::string from, std::string to) {
  this->from = from;
  this->to = to;
}

MarkovChain MarkovChain::from_text(std::string filename) {
  MarkovChain instance;

  std::regex regex("\\b[^\\s.,]+\\b");

  std::map<std::string, int> word_counts;
  std::ifstream file;
  std::string line;
  file.open(filename);

  while (std::getline(file, line)) {
    if (line.empty())
      continue;

    std::sregex_iterator i = std::sregex_iterator(line.begin(), line.end(), regex);
    std::string previous = "";
    while (i != std::sregex_iterator()) {
      std::smatch match = *i;
      std::string current = match.str();

      word_counts.emplace(previous, 0.0).first->second++;

      if (!previous.empty()) {
        WordPair pair = WordPair(previous, current);
        // If pair already exists, increase count for it by 1
        // else, assign 1;
        instance.model.emplace(pair, 0.0).first->second++;
      }

      previous = current;
      i++;
    }
  }

  for (auto &kv : instance.model) {
    kv.second /= word_counts[kv.first.from];
    instance.model[kv.first] = kv.second;
  }

  file.close();

  return instance;
}

MarkovChain MarkovChain::from_json(std::string filename) {
  MarkovChain instance;

  std::ifstream file;
  std::string line;
  file.open(filename);

  while (std::getline(file, line)) {
    if (line.size() == 1)
      continue;
    if (line[line.size() - 1] == ',')
      line.pop_back(); // Remove comma if it exists

    WordPair pair;
    std::string probability;

    line = line.substr(1, line.size() - 2);
    std::stringstream linestream(line);

    std::getline(linestream, pair.from, ',');
    pair.from.pop_back();
    pair.from.erase(0, 1);
    std::getline(linestream, pair.to, ',');
    pair.to.pop_back();
    pair.to.erase(0, 1);
    std::getline(linestream, probability, ',');

    instance.model[pair] = std::stod(probability);
  }

  file.close();

  return instance;
}

void MarkovChain::export_to_json(std::string filename) {
  std::ofstream file;
  file.open(filename, std::ios_base::trunc);

  unsigned long model_length = this->model.size();
  unsigned long i = 0;

  // Start JSON Array
  file << "[\n";
  for (const auto &kv : this->model) {
    // Use JSON Arrays instead of objects for more compactness, very ugly but it reduces size a lot
    file << "[";
    // from
    file << "\"" << kv.first.from << "\",";
    // to
    file << "\"" << kv.first.to << "\",";
    // probability
    file << kv.second << "";
    file << "]";

    if (i < model_length - 1) {
      file << ",";
    }
    file << "\n";

    i++;
  }
  // End JSON Array
  file << "]";

  file.close();
}
