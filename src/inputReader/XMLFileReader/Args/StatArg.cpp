#include "StatArg.h"

#include <utility>

StatArg::StatArg(int frequency, double rdfDeltaR, double rdfStart, double rdfEnd, std::string path)
    : frequency{frequency}, rdfDeltaR{rdfDeltaR}, rdfStart{rdfStart}, rdfEnd{rdfEnd}, path{std::move(path)} {}

int StatArg::getFrequency() const { return frequency; }

double StatArg::getRdfDeltaR() const { return rdfDeltaR; }

double StatArg::getRdfStart() const { return rdfStart; }

double StatArg::getRdfEnd() const { return rdfEnd; }

const std::string& StatArg::getPath() const { return path; }
