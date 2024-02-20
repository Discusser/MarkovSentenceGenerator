#include <algorithm>
#include <cstdlib>
#include <glib.h>
#include <iostream>
#include <ostream>
#include <vector>

#include "./markovchain.hpp"

char *data_file;
char *output_model;
char *input_model;

GOptionEntry command_line_arguments[] = {
    {"file", 'f', G_OPTION_FLAG_NONE, G_OPTION_ARG_FILENAME, &data_file,
     "The path to the file containing the words to be used as a model", ""},
    {"output_model", 'o', G_OPTION_FLAG_NONE, G_OPTION_ARG_FILENAME, &output_model,
     "The path for the JSON model that will be generated", ""},
    {"input_model", 'm', G_OPTION_FLAG_NONE, G_OPTION_ARG_FILENAME, &input_model,
     "The path for the JSON model that will be used. Cannot be use with -o or -f", ""},
    {nullptr}
};

int main(int argc, char *argv[]) {
  GError *error = nullptr;
  GOptionContext *context = g_option_context_new(nullptr);
  g_option_context_add_main_entries(context, command_line_arguments, nullptr);
  g_option_context_set_help_enabled(context, true);

  if (!g_option_context_parse(context, &argc, &argv, &error)) {
    g_error("An error occured while parsing command line arguments: %s\n", error->message);
    g_error_free(error);
  }

  g_option_context_free(context);

  if (data_file && output_model) {
    MarkovChain model = MarkovChain::from_text(data_file);
    model.export_to_json(output_model);
  } else if (input_model) {
    MarkovChain markov = MarkovChain::from_json(input_model);
    std::map<std::string, std::map<double, std::vector<std::string>>> optimized_map;

    for (const auto &kv : markov.model) {
      optimized_map.emplace(kv.first.from, std::map<double, std::vector<std::string>>())
          .first->second.emplace(kv.second, std::vector<std::string>())
          .first->second.push_back(kv.first.to);
    }

    std::string word;
    while (true) {
      std::cout << "Starting word: ";
      std::getline(std::cin, word);
      unsigned int seed = time(nullptr);
      while (true) {
        if (optimized_map.count(word) == 0)
          break;

        auto probability_word_multimap = optimized_map[word];

        double random = static_cast<double>(rand_r(&seed)) / RAND_MAX;
        for (const auto &kv : probability_word_multimap) {
          if (random <= kv.first) {
            word = kv.second[rand_r(&seed) % kv.second.size()];
            std::cout << word << " ";
            std::flush(std::cout); // Ensure the word prints
            break;
          }
        }
      }
      std::cout << std::endl;
    }
  } else {
    g_error("Missing options, try --help");
  }

  return 0;
}
