#include "directive_descriptions.h"

#include <map>
#include <string>

class DirectiveDescriptions {
public:
    static const std::map<std::string, std::string>& getDescriptions() {
        static const std::map<std::string, std::string> descriptions = {
            {"omp parallel", "Creates a parallel region."},
            {"omp for", "Distributes loop iterations among threads."},
            {"omp sections", "Defines a structured block of code to be executed by multiple threads."},
            {"omp single", "Specifies that a block of code should be executed by a single thread."},
            {"omp task", "Defines a task that can be executed asynchronously."},
            {"omp target", "Offloads code to a target device."},
            {"omp taskloop", "Creates tasks for loop iterations."},
            // Add more OpenMP directives and their descriptions as needed
        };
        return descriptions;
    }

    static const std::map<std::string, std::string>& getIRTransformations() {
        static const std::map<std::string, std::string> irTransformations = {
            {"omp parallel", "call @__kmpc_fork_call"},
            {"omp for", "call @__kmpc_for_static_f"},
            {"omp sections", "call @__kmpc_sections"},
            {"omp single", "call @__kmpc_single"},
            {"omp task", "call @__kmpc_task"},
            {"omp target", "call @__kmpc_target"},
            {"omp taskloop", "call @__kmpc_taskloop"},
            // Add more OpenMP directives and their corresponding IR transformations as needed
        };
        return irTransformations;
    }
};