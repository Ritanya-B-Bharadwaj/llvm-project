#ifndef BERT_TOKENIZER_H
#define BERT_TOKENIZER_H

#include <string>
#include <vector>
#include <unordered_map>

class BertTokenizer {
public:
    explicit BertTokenizer(const std::string& vocabPath);
    
    std::vector<int64_t> tokenize(const std::string& text);
    
private:
    std::unordered_map<std::string, int64_t> vocab;
    std::vector<std::string> id_to_token;
    
    // Special tokens
    int64_t cls_token_id = 101;
    int64_t sep_token_id = 102;
    int64_t pad_token_id = 0;
    int64_t unk_token_id = 100;
    
    void loadVocabulary(const std::string& vocabPath);
    std::vector<std::string> basicTokenize(const std::string& text);
    std::vector<std::string> wordpieceTokenize(const std::string& token);
    std::string toLowerCase(const std::string& text);
    std::string cleanText(const std::string& text);
};

#endif // BERT_TOKENIZER_H
