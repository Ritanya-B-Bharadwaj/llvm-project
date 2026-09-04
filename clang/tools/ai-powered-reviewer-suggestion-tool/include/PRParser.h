#ifndef PRPARSER_H
#define PRPARSER_H

#include <string>
#include <vector>
#include <memory>
#include <onnxruntime_cxx_api.h>
#include "BertTokenizer.h"

class PRParser {
public:
    PRParser(const std::string& modelPath, const std::string& vocabPath);
    ~PRParser();
    
    std::vector<float> generateEmbedding(const std::string& text);
    std::vector<std::string> extractFiles(const std::string& prContent);
    
private:
    std::unique_ptr<Ort::Env> env;
    std::unique_ptr<Ort::Session> session;
    std::unique_ptr<BertTokenizer> tokenizer;
    Ort::MemoryInfo memory_info;
    Ort::AllocatorWithDefaultOptions allocator;
    
    std::vector<std::string> input_names;
    std::vector<std::string> output_names;
    std::vector<std::vector<int64_t>> input_shapes;
    std::vector<std::vector<int64_t>> output_shapes;
    
    std::vector<float> processOutput(Ort::Value& output_tensor);
    std::string cleanText(const std::string& text);
    void initializeModelInfo();
    std::vector<int64_t> padSequence(const std::vector<int64_t>& sequence, size_t max_length);
};

#endif // PRPARSER_H
