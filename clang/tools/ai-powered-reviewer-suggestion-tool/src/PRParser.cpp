#include "PRParser.h"
#include <iostream>
#include <algorithm>
#include <regex>
#include <sstream>

PRParser::PRParser(const std::string& modelPath, const std::string& vocabPath)
    : memory_info(Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault)) {
    
    try {
        // Initialize ONNX Runtime environment with logging disabled
        env = std::make_unique<Ort::Env>(ORT_LOGGING_LEVEL_ERROR, "PRParser");
        
        // Create session options
        Ort::SessionOptions session_options;
        session_options.SetIntraOpNumThreads(1);
        session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_BASIC);
        
        // Load the model
        session = std::make_unique<Ort::Session>(*env, modelPath.c_str(), session_options);
        
        // Initialize tokenizer
        tokenizer = std::make_unique<BertTokenizer>(vocabPath);
        
        // Get model input/output information
        initializeModelInfo();
        
        std::cout << "PRParser initialized with model: " << modelPath << std::endl;
        std::cout << "Model loaded successfully with " << input_names.size()
                  << " inputs and " << output_names.size() << " outputs" << std::endl;
        
        // Print input details for debugging
        for (size_t i = 0; i < input_names.size(); i++) {
            std::cout << "Input " << i << ": " << input_names[i] << " Shape: [";
            for (size_t j = 0; j < input_shapes[i].size(); j++) {
                std::cout << input_shapes[i][j];
                if (j < input_shapes[i].size() - 1) std::cout << ", ";
            }
            std::cout << "]" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error initializing PRParser: " << e.what() << std::endl;
        throw;
    }
}

PRParser::~PRParser() = default;

void PRParser::initializeModelInfo() {
    // Get input information
    size_t num_inputs = session->GetInputCount();
    input_names.clear();
    input_shapes.clear();
    
    for (size_t i = 0; i < num_inputs; i++) {
        // Get input name
        auto input_name_ptr = session->GetInputNameAllocated(i, allocator);
        input_names.push_back(std::string(input_name_ptr.get()));
        
        // Get input shape
        auto input_type_info = session->GetInputTypeInfo(i);
        auto input_tensor_info = input_type_info.GetTensorTypeAndShapeInfo();
        auto input_shape = input_tensor_info.GetShape();
        input_shapes.push_back(input_shape);
    }
    
    // Get output information
    size_t num_outputs = session->GetOutputCount();
    output_names.clear();
    output_shapes.clear();
    
    for (size_t i = 0; i < num_outputs; i++) {
        // Get output name
        auto output_name_ptr = session->GetOutputNameAllocated(i, allocator);
        output_names.push_back(std::string(output_name_ptr.get()));
        
        // Get output shape
        auto output_type_info = session->GetOutputTypeInfo(i);
        auto output_tensor_info = output_type_info.GetTensorTypeAndShapeInfo();
        auto output_shape = output_tensor_info.GetShape();
        output_shapes.push_back(output_shape);
    }
}

std::string PRParser::cleanText(const std::string& text) {
    if (text.empty()) return "";
    
    // Remove excessive whitespace and newlines
    std::string cleaned = std::regex_replace(text, std::regex("\\s+"), " ");
    
    // Trim leading and trailing whitespace
    cleaned = std::regex_replace(cleaned, std::regex("^\\s+|\\s+$"), "");
    
    // Limit length to prevent memory issues
    if (cleaned.length() > 2048) {
        cleaned = cleaned.substr(0, 2048);
    }
    
    return cleaned;
}

std::vector<int64_t> PRParser::padSequence(const std::vector<int64_t>& sequence, size_t max_length) {
    std::vector<int64_t> padded(max_length, 0); // PAD token ID is typically 0
    
    size_t copy_length = std::min(sequence.size(), max_length);
    std::copy(sequence.begin(), sequence.begin() + copy_length, padded.begin());
    
    return padded;
}

std::vector<float> PRParser::generateEmbedding(const std::string& text) {
    try {
        // Validate input
        if (text.empty()) {
            std::cerr << "Warning: Empty text provided for embedding" << std::endl;
            return std::vector<float>(384, 0.0f);
        }
        
        // Clean the text
        std::string cleaned_text = cleanText(text);
        if (cleaned_text.empty()) {
            std::cerr << "Warning: Text becomes empty after cleaning" << std::endl;
            return std::vector<float>(384, 0.0f);
        }
        
        // Tokenize the text
        auto tokens = tokenizer->tokenize(cleaned_text);
        if (tokens.empty()) {
            std::cerr << "Warning: No tokens generated for text: " << cleaned_text.substr(0, 50) << "..." << std::endl;
            return std::vector<float>(384, 0.0f);
        }
        
        // Prepare sequences with proper length (max 512 for BERT models)
        const size_t max_length = 512;
        std::vector<int64_t> input_ids = padSequence(tokens, max_length);
        
        // Create attention mask (1 for real tokens, 0 for padding)
        std::vector<int64_t> attention_mask(max_length, 0);
        for (size_t i = 0; i < std::min(tokens.size(), max_length); i++) {
            attention_mask[i] = 1;
        }
        
        // Create token type ids (all 0s for single sentence)
        std::vector<int64_t> token_type_ids(max_length, 0);
        
        // Create input tensors
        std::vector<int64_t> input_shape = {1, static_cast<int64_t>(max_length)};
        
        auto input_ids_tensor = Ort::Value::CreateTensor<int64_t>(
            memory_info, input_ids.data(), input_ids.size(),
            input_shape.data(), input_shape.size());
            
        auto attention_mask_tensor = Ort::Value::CreateTensor<int64_t>(
            memory_info, attention_mask.data(), attention_mask.size(),
            input_shape.data(), input_shape.size());
        
        // Prepare inputs based on model requirements
        std::vector<Ort::Value> input_tensors;
        std::vector<const char*> input_names_cstr;
        
        // Most sentence transformers need input_ids and attention_mask
        if (input_names.size() >= 2) {
            input_names_cstr.push_back(input_names[0].c_str()); // input_ids
            input_names_cstr.push_back(input_names[1].c_str()); // attention_mask
            input_tensors.push_back(std::move(input_ids_tensor));
            input_tensors.push_back(std::move(attention_mask_tensor));
            
            // Add token_type_ids if model expects it
            if (input_names.size() >= 3) {
                auto token_type_ids_tensor = Ort::Value::CreateTensor<int64_t>(
                    memory_info, token_type_ids.data(), token_type_ids.size(),
                    input_shape.data(), input_shape.size());
                input_names_cstr.push_back(input_names[2].c_str());
                input_tensors.push_back(std::move(token_type_ids_tensor));
            }
        } else {
            std::cerr << "Error: Model has insufficient inputs (" << input_names.size() << ")" << std::endl;
            return std::vector<float>(384, 0.0f);
        }
        
        // Convert output names to const char*
        std::vector<const char*> output_names_cstr;
        for (const auto& name : output_names) {
            output_names_cstr.push_back(name.c_str());
        }
        
        // Validate input names are not empty
        for (const auto& name : input_names_cstr) {
            if (!name || strlen(name) == 0) {
                std::cerr << "Error: Empty input name detected" << std::endl;
                return std::vector<float>(384, 0.0f);
            }
        }
        
        // Run inference
        auto output_tensors = session->Run(Ort::RunOptions{nullptr},
                                         input_names_cstr.data(),
                                         input_tensors.data(),
                                         input_names_cstr.size(),
                                         output_names_cstr.data(),
                                         output_names_cstr.size());
        
        if (output_tensors.empty()) {
            std::cerr << "Error: No output tensors returned" << std::endl;
            return std::vector<float>(384, 0.0f);
        }
        
        // Process output (usually the last hidden state)
        return processOutput(output_tensors[0]);
        
    } catch (const Ort::Exception& e) {
        std::cerr << "ONNX Runtime error: " << e.what() << std::endl;
        return std::vector<float>(384, 0.0f);
    } catch (const std::exception& e) {
        std::cerr << "Error generating embedding: " << e.what() << std::endl;
        return std::vector<float>(384, 0.0f);
    }
}

std::vector<float> PRParser::processOutput(Ort::Value& output_tensor) {
    try {
        // Get tensor info
        auto tensor_info = output_tensor.GetTensorTypeAndShapeInfo();
        auto shape = tensor_info.GetShape();
        
        if (shape.size() < 3) {
            std::cerr << "Error: Unexpected output tensor shape" << std::endl;
            return std::vector<float>(384, 0.0f);
        }
        
        // Get tensor data
        float* tensor_data = output_tensor.GetTensorMutableData<float>();
        size_t batch_size = shape[0];
        size_t seq_length = shape[1];
        size_t hidden_size = shape[2];
        
        // Perform mean pooling over sequence length
        std::vector<float> result(hidden_size, 0.0f);
        
        for (size_t i = 0; i < seq_length; i++) {
            for (size_t j = 0; j < hidden_size; j++) {
                result[j] += tensor_data[i * hidden_size + j];
            }
        }
        
        // Average the pooled values
        for (size_t j = 0; j < hidden_size; j++) {
            result[j] /= static_cast<float>(seq_length);
        }
        
        // L2 normalize the embedding
        float norm = 0.0f;
        for (float val : result) {
            norm += val * val;
        }
        norm = std::sqrt(norm);
        
        if (norm > 0.0f) {
            for (float& val : result) {
                val /= norm;
            }
        }
        
        return result;
        
    } catch (const std::exception& e) {
        std::cerr << "Error processing output: " << e.what() << std::endl;
        return std::vector<float>(384, 0.0f);
    }
}

std::vector<std::string> PRParser::extractFiles(const std::string& prContent) {
    std::vector<std::string> files;
    std::istringstream stream(prContent);
    std::string line;
    
    // Look for file patterns in PR content
    std::regex file_pattern(R"(\+\+\+ b/(.+))");
    std::regex diff_pattern(R"(diff --git a/.+ b/(.+))");
    
    while (std::getline(stream, line)) {
        std::smatch match;
        if (std::regex_search(line, match, file_pattern) && match.size() > 1) {
            files.push_back(match[1].str());
        } else if (std::regex_search(line, match, diff_pattern) && match.size() > 1) {
            files.push_back(match[1].str());
        }
    }
    
    // Remove duplicates
    std::sort(files.begin(), files.end());
    files.erase(std::unique(files.begin(), files.end()), files.end());
    
    return files;
}
