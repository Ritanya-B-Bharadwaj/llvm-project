#include "BertTokenizer.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <regex>
#include <sstream>

BertTokenizer::BertTokenizer(const std::string& vocabPath) {
    loadVocabulary(vocabPath);
}

void BertTokenizer::loadVocabulary(const std::string& vocabPath) {
    std::ifstream file(vocabPath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open vocabulary file: " + vocabPath);
    }
    
    std::string token;
    int64_t id = 0;
    
    while (std::getline(file, token)) {
        vocab[token] = id;
        id_to_token.push_back(token);
        id++;
    }
    
    std::cout << "Loaded vocabulary with " << vocab.size() << " tokens" << std::endl;
    file.close();
}

std::string BertTokenizer::toLowerCase(const std::string& text) {
    std::string result = text;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string BertTokenizer::cleanText(const std::string& text) {
    // Remove control characters and normalize whitespace
    std::string cleaned = std::regex_replace(text, std::regex("[\\x00-\\x1F\\x7F-\\x9F]"), " ");
    cleaned = std::regex_replace(cleaned, std::regex("\\s+"), " ");
    
    // Trim
    cleaned = std::regex_replace(cleaned, std::regex("^\\s+|\\s+$"), "");
    
    return cleaned;
}

std::vector<std::string> BertTokenizer::basicTokenize(const std::string& text) {
    std::vector<std::string> tokens;
    std::string cleaned = cleanText(toLowerCase(text));
    
    // Split on whitespace and punctuation
    std::regex token_regex(R"(\w+|[^\w\s])");
    std::sregex_iterator iter(cleaned.begin(), cleaned.end(), token_regex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string token = iter->str();
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    
    return tokens;
}

std::vector<std::string> BertTokenizer::wordpieceTokenize(const std::string& token) {
    std::vector<std::string> output_tokens;
    
    if (token.length() > 200) {
        output_tokens.push_back("[UNK]");
        return output_tokens;
    }
    
    bool is_bad = false;
    int start = 0;
    std::vector<std::string> sub_tokens;
    
    while (start < static_cast<int>(token.length())) {
        int end = token.length();
        std::string cur_substr;
        bool found = false;
        
        while (start < end) {
            std::string substr = token.substr(start, end - start);
            if (start > 0) {
                substr = "##" + substr;
            }
            
            if (vocab.find(substr) != vocab.end()) {
                cur_substr = substr;
                found = true;
                break;
            }
            end--;
        }
        
        if (!found) {
            is_bad = true;
            break;
        }
        
        sub_tokens.push_back(cur_substr);
        start = end;
    }
    
    if (is_bad) {
        output_tokens.push_back("[UNK]");
    } else {
        output_tokens = sub_tokens;
    }
    
    return output_tokens;
}

std::vector<int64_t> BertTokenizer::tokenize(const std::string& text) {
    std::vector<int64_t> token_ids;
    
    if (text.empty()) {
        return token_ids;
    }
    
    // Add [CLS] token
    token_ids.push_back(cls_token_id);
    
    // Basic tokenization
    auto basic_tokens = basicTokenize(text);
    
    // WordPiece tokenization
    for (const auto& token : basic_tokens) {
        auto wordpiece_tokens = wordpieceTokenize(token);
        
        for (const auto& wp_token : wordpiece_tokens) {
            auto it = vocab.find(wp_token);
            if (it != vocab.end()) {
                token_ids.push_back(it->second);
            } else {
                token_ids.push_back(unk_token_id);
            }
        }
    }
    
    // Add [SEP] token
    token_ids.push_back(sep_token_id);
    
    return token_ids;
}
