#ifndef CONFIG_H
#define CONFIG_H

#include <string>

namespace openmp_test_gen {
namespace config {

// Default configuration values
constexpr int DEFAULT_NUM_TESTS = 1;
constexpr int MAX_NUM_TESTS = 4;
constexpr int MIN_NUM_TESTS = 1;

constexpr const char* DEFAULT_STAGE = "sema";
constexpr const char* DEFAULT_DB_PATH = "openmp_patterns.db";
constexpr const char* DEFAULT_REPO = "llvm/llvm-project";
constexpr const char* OUTPUT_DIR = "outputs";

// API configuration
constexpr const char* GROQ_API_URL = "https://api.groq.com/openai/v1/chat/completions";
constexpr const char* GROQ_MODEL = "llama3-70b-8192";
constexpr int GROQ_MAX_TOKENS = 1024;
constexpr double GROQ_TEMPERATURE = 0.3;
constexpr long GROQ_TIMEOUT = 60L;

// GitHub API configuration
constexpr const char* GITHUB_API_BASE = "https://api.github.com/repos";
constexpr long GITHUB_TIMEOUT = 30L;

// File naming patterns
constexpr const char* OUTPUT_FILENAME_PATTERN = "pr_%d_%s_test_%d.cpp";

// Valid stages
const std::string VALID_STAGES[] = {"parse", "sema", "codegen"};

// Environment variable names
constexpr const char* ENV_GROQ_API_KEY = "GROQ_API_KEY";
constexpr const char* ENV_GITHUB_TOKEN = "GITHUB_TOKEN";

} // namespace config
} // namespace openmp_test_gen

#endif // CONFIG_H
