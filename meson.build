project('MarkovSentenceGenerator', 'cpp', version: '0.1', default_options: ['cpp_std=c++17'])

glib = dependency('glib-2.0')
json = dependency('nlohmann_json')
dependencies = [glib, json]

sources = ['sentencegenerator.cpp', 'markovchain.cpp']

executable('MarkovSentenceGenerator', sources, dependencies: dependencies)
