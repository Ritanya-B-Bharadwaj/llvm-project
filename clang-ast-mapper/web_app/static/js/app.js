// Main JavaScript for AST Line Mapper web application

document.addEventListener('DOMContentLoaded', function() {
    // Initialize tooltips
    var tooltipTriggerList = [].slice.call(document.querySelectorAll('[data-bs-toggle="tooltip"]'));
    var tooltipList = tooltipTriggerList.map(function(tooltipTriggerEl) {
        return new bootstrap.Tooltip(tooltipTriggerEl);
    });

    // Initialize popovers
    var popoverTriggerList = [].slice.call(document.querySelectorAll('[data-bs-toggle="popover"]'));
    var popoverList = popoverTriggerList.map(function(popoverTriggerEl) {
        return new bootstrap.Popover(popoverTriggerEl);
    });

    // Example loading functions
    function loadExampleCode(exampleName) {
        // Example predefined code snippets
        const examples = {
            'simple': `#include <iostream>\n\nint main() {\n    int x = 42;\n    std::cout << "Hello, world! x = " << x << std::endl;\n    return 0;\n}`,
            
            'variable_declarations': `#include <string>\n\nint main() {\n    // Basic types\n    int a = 5;\n    float b = 3.14f;\n    double c = 2.71828;\n    bool d = true;\n    char e = 'A';\n    \n    // Reference and pointer\n    int& ref = a;\n    int* ptr = &a;\n    \n    // Auto type deduction\n    auto f = 100;\n    auto g = 1.5;\n    \n    // String\n    std::string h = "Hello, AST!";\n    \n    return 0;\n}`,
            
            'control_flow': `#include <iostream>\n\nint main() {\n    // If-else statement\n    int x = 10;\n    \n    if (x > 5) {\n        std::cout << "x is greater than 5" << std::endl;\n    } else if (x == 5) {\n        std::cout << "x equals 5" << std::endl;\n    } else {\n        std::cout << "x is less than 5" << std::endl;\n    }\n    \n    // For loop\n    for (int i = 0; i < 3; i++) {\n        std::cout << "Loop iteration " << i << std::endl;\n    }\n    \n    // While loop\n    int j = 0;\n    while (j < 3) {\n        std::cout << "While iteration " << j << std::endl;\n        j++;\n    }\n    \n    // Switch statement\n    char grade = 'B';\n    switch (grade) {\n        case 'A':\n            std::cout << "Excellent!" << std::endl;\n            break;\n        case 'B':\n            std::cout << "Good job!" << std::endl;\n            break;\n        default:\n            std::cout << "Invalid grade" << std::endl;\n    }\n    \n    return 0;\n}`,
            
            'functions': `#include <iostream>\n\n// Function declaration\nint add(int a, int b);\n\n// Function with default parameter\nvoid greet(const std::string& name = "World");\n\n// Function template\ntemplate <typename T>\nT maximum(T a, T b) {\n    return (a > b) ? a : b;\n}\n\nint main() {\n    // Function calls\n    int sum = add(5, 3);\n    std::cout << "Sum: " << sum << std::endl;\n    \n    greet();\n    greet("C++ Developer");\n    \n    // Template function calls\n    std::cout << "Max (int): " << maximum<int>(10, 15) << std::endl;\n    std::cout << "Max (double): " << maximum<double>(3.14, 2.71) << std::endl;\n    \n    // Lambda function\n    auto multiply = [](int x, int y) -> int { return x * y; };\n    std::cout << "Product: " << multiply(4, 7) << std::endl;\n    \n    return 0;\n}\n\n// Function definition\nint add(int a, int b) {\n    return a + b;\n}\n\nvoid greet(const std::string& name) {\n    std::cout << "Hello, " << name << "!" << std::endl;\n}`
        };
        
        // Get the CodeMirror instance
        const cmInstance = document.querySelector('.CodeMirror').CodeMirror;
        
        // Set the example code
        if (examples[exampleName]) {
            cmInstance.setValue(examples[exampleName]);
        }
    }

    // Add event listeners to example buttons if they exist
    document.querySelectorAll('.example-btn').forEach(button => {
        button.addEventListener('click', function() {
            loadExampleCode(this.dataset.example);
        });
    });

    // Copy to clipboard functionality
    document.querySelectorAll('.copy-btn').forEach(button => {
        button.addEventListener('click', function() {
            const targetId = this.dataset.target;
            const textToCopy = document.getElementById(targetId).textContent;
            
            navigator.clipboard.writeText(textToCopy).then(() => {
                // Change button text temporarily
                const originalText = this.textContent;
                this.textContent = 'Copied!';
                setTimeout(() => {
                    this.textContent = originalText;
                }, 2000);
            }).catch(err => {
                console.error('Failed to copy text: ', err);
            });
        });
    });

    // Tab switching
    document.querySelectorAll('.nav-tabs .nav-link').forEach(tab => {
        tab.addEventListener('click', function(event) {
            event.preventDefault();
            
            // Remove active class from all tabs and tab panes
            document.querySelectorAll('.nav-tabs .nav-link').forEach(t => {
                t.classList.remove('active');
            });
            
            document.querySelectorAll('.tab-pane').forEach(p => {
                p.classList.remove('show', 'active');
            });
            
            // Add active class to clicked tab
            this.classList.add('active');
            
            // Show corresponding tab content
            const target = document.querySelector(this.dataset.bsTarget);
            if (target) {
                target.classList.add('show', 'active');
            }
        });
    });
});
