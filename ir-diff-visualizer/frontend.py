import os
import sys
import streamlit as st
import tempfile
import base64
import time
from pathlib import Path

# Add the current directory to sys.path to import from backend
sys.path.insert(0, os.path.abspath('.'))

try:
    from main import run_ir_diff
except ImportError as e:
    st.error(f"Import error: {e}")
    st.error("Please ensure all dependencies are installed and files are in the correct location.")
    st.stop()

# 🎨 Page Configuration
st.set_page_config(
    page_title="LLVM IR Diff Analyzer Pro", 
    page_icon="🚀", 
    layout="wide",
    initial_sidebar_state="collapsed"  # Hide sidebar
)

# 🎨 Custom CSS Styling with Custom Background
def load_custom_css():
    st.markdown("""
    <style>
    /* Import Google Fonts */
    @import url('https://fonts.googleapis.com/css2?family=Inter:wght@300;400;500;600;700&display=swap');
    
    /* Global Styles with Custom Background */
    .main {
        font-family: 'Inter', sans-serif;
        background: linear-gradient(135deg, #1e3c72 0%, #2a5298 100%);
        min-height: 100vh;
    }
    
    .stApp {
        background: linear-gradient(135deg, #1e3c72 0%, #2a5298 100%);
    }
    
    /* Main container styling */
    .block-container {
        background: rgba(255, 255, 255, 0.95);
        border-radius: 20px;
        padding: 2rem;
        margin: 1rem;
        box-shadow: 0 20px 60px rgba(0,0,0,0.3);
        backdrop-filter: blur(10px);
    }
    
    /* Custom Header */
    .hero-header {
        background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
        padding: 3rem 2rem;
        border-radius: 20px;
        margin-bottom: 3rem;
        text-align: center;
        color: white;
        box-shadow: 0 15px 40px rgba(0,0,0,0.3);
    }
    
    .hero-title {
        font-size: 4rem;
        font-weight: 700;
        margin-bottom: 1rem;
        text-shadow: 2px 2px 8px rgba(0,0,0,0.3);
        background: linear-gradient(45deg, #fff, #f0f0f0);
        -webkit-background-clip: text;
        -webkit-text-fill-color: transparent;
        background-clip: text;
    }
    
    .hero-subtitle {
        font-size: 1.4rem;
        font-weight: 300;
        opacity: 0.9;
        margin-bottom: 1rem;
    }
    
    .hero-description {
        font-size: 1.1rem;
        opacity: 0.8;
        max-width: 600px;
        margin: 0 auto;
        line-height: 1.6;
    }
    
    /* Feature Cards */
    .feature-card {
        background: linear-gradient(135deg, #ffffff 0%, #f8f9fa 100%);
        padding: 2rem;
        border-radius: 15px;
        box-shadow: 0 8px 30px rgba(0,0,0,0.15);
        border: 1px solid rgba(102, 126, 234, 0.2);
        margin-bottom: 1.5rem;
        transition: all 0.3s ease;
        height: 100%;
    }
    
    .feature-card:hover {
        transform: translateY(-8px);
        box-shadow: 0 15px 40px rgba(0,0,0,0.2);
        border-color: #667eea;
    }
    
    .feature-icon {
        font-size: 3rem;
        margin-bottom: 1rem;
        text-align: center;
    }
    
    .feature-title {
        font-size: 1.3rem;
        font-weight: 600;
        color: #333;
        margin-bottom: 1rem;
        text-align: center;
    }
    
    .feature-description {
        color: #666;
        font-size: 1rem;
        line-height: 1.6;
        text-align: center;
    }
    
    /* Code Editor Styling */
    .code-container {
        background: linear-gradient(135deg, #f8f9fa 0%, #e9ecef 100%);
        border: 2px solid #667eea;
        border-radius: 15px;
        padding: 1.5rem;
        margin: 1.5rem 0;
        transition: all 0.3s ease;
    }
    
    .code-container:hover {
        border-color: #764ba2;
        box-shadow: 0 5px 20px rgba(102, 126, 234, 0.2);
    }
    
    /* Section Headers */
    .section-header {
        background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
        color: white;
        padding: 1.5rem;
        border-radius: 15px;
        margin: 2rem 0 1rem 0;
        text-align: center;
        font-size: 1.5rem;
        font-weight: 600;
        box-shadow: 0 5px 20px rgba(102, 126, 234, 0.3);
    }
    
    /* Pass Selection Cards */
    .pass-selection {
        background: white;
        padding: 2rem;
        border-radius: 15px;
        box-shadow: 0 5px 20px rgba(0,0,0,0.1);
        margin: 1rem 0;
    }
    
    /* Progress Styling */
    .progress-container {
        background: linear-gradient(135deg, #ffffff 0%, #f8f9fa 100%);
        padding: 2.5rem;
        border-radius: 20px;
        box-shadow: 0 10px 30px rgba(0,0,0,0.15);
        margin: 2rem 0;
        border: 1px solid rgba(102, 126, 234, 0.2);
    }
    
    .progress-step {
        display: flex;
        align-items: center;
        margin: 1.5rem 0;
        font-weight: 500;
        font-size: 1.1rem;
    }
    
    .progress-icon {
        width: 40px;
        height: 40px;
        border-radius: 50%;
        background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
        color: white;
        display: flex;
        align-items: center;
        justify-content: center;
        margin-right: 1rem;
        font-size: 1rem;
        box-shadow: 0 3px 10px rgba(102, 126, 234, 0.3);
    }
    
    .progress-icon.completed {
        background: linear-gradient(135deg, #28a745 0%, #20c997 100%);
    }
    
    .progress-icon.current {
        background: linear-gradient(135deg, #ffc107 0%, #fd7e14 100%);
        animation: pulse 2s infinite;
    }
    
    @keyframes pulse {
        0% { transform: scale(1); box-shadow: 0 3px 10px rgba(255, 193, 7, 0.3); }
        50% { transform: scale(1.1); box-shadow: 0 5px 20px rgba(255, 193, 7, 0.5); }
        100% { transform: scale(1); box-shadow: 0 3px 10px rgba(255, 193, 7, 0.3); }
    }
    
    /* Success/Error Styling */
    .success-container {
        background: linear-gradient(135deg, #28a745 0%, #20c997 100%);
        color: white;
        padding: 3rem;
        border-radius: 20px;
        text-align: center;
        margin: 2rem 0;
        box-shadow: 0 15px 40px rgba(40, 167, 69, 0.3);
    }
    
    .error-container {
        background: linear-gradient(135deg, #dc3545 0%, #fd7e14 100%);
        color: white;
        padding: 3rem;
        border-radius: 20px;
        text-align: center;
        margin: 2rem 0;
        box-shadow: 0 15px 40px rgba(220, 53, 69, 0.3);
    }
    
    /* Button Styling */
    .stButton > button {
        background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
        color: white;
        border: none;
        border-radius: 30px;
        padding: 1rem 3rem;
        font-weight: 600;
        font-size: 1.2rem;
        transition: all 0.3s ease;
        box-shadow: 0 8px 25px rgba(102, 126, 234, 0.4);
        width: 100%;
    }
    
    .stButton > button:hover {
        transform: translateY(-3px);
        box-shadow: 0 12px 35px rgba(102, 126, 234, 0.6);
    }
    
    /* Download Button */
    .download-button {
        display: inline-block;
        background: linear-gradient(135deg, #28a745 0%, #20c997 100%);
        color: white;
        padding: 1.5rem 3rem;
        text-decoration: none;
        border-radius: 30px;
        font-weight: 600;
        font-size: 1.2rem;
        box-shadow: 0 8px 25px rgba(40, 167, 69, 0.4);
        transition: all 0.3s ease;
    }
    
    .download-button:hover {
        transform: translateY(-3px);
        box-shadow: 0 12px 35px rgba(40, 167, 69, 0.6);
        text-decoration: none;
        color: white;
    }
    
    /* Hide Streamlit elements */
    #MainMenu {visibility: hidden;}
    footer {visibility: hidden;}
    header {visibility: hidden;}
    .stDeployButton {visibility: hidden;}
    
    /* Hide sidebar completely */
    .css-1d391kg {display: none;}
    .css-1y4p8pa {display: none;}
    section[data-testid="stSidebar"] {display: none;}
    
    /* Tab styling */
    .stTabs [data-baseweb="tab-list"] {
        gap: 8px;
    }
    
    .stTabs [data-baseweb="tab"] {
        background: linear-gradient(135deg, #f8f9fa 0%, #e9ecef 100%);
        border-radius: 10px;
        padding: 10px 20px;
        border: 1px solid #dee2e6;
    }
    
    .stTabs [aria-selected="true"] {
        background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
        color: white;
    }
    
    /* Info boxes */
    .stInfo {
        background: linear-gradient(135deg, #e3f2fd 0%, #bbdefb 100%);
        border: 1px solid #2196f3;
        border-radius: 10px;
    }
    
    /* Metrics */
    .metric-container {
        background: white;
        padding: 1.5rem;
        border-radius: 15px;
        box-shadow: 0 5px 15px rgba(0,0,0,0.1);
        text-align: center;
        border: 1px solid rgba(102, 126, 234, 0.2);
    }
    
    /* Footer */
    .footer {
        background: linear-gradient(135deg, #343a40 0%, #495057 100%);
        color: white;
        text-align: center;
        padding: 2rem;
        border-radius: 15px;
        margin-top: 3rem;
    }
    
    /* Responsive Design */
    @media (max-width: 768px) {
        .hero-title {
            font-size: 2.5rem;
        }
        
        .hero-subtitle {
            font-size: 1.1rem;
        }
        
        .feature-card {
            margin-bottom: 1rem;
        }
    }
    </style>
    """, unsafe_allow_html=True)

def create_hero_section():
    """Create an attractive hero section"""
    st.markdown("""
    <div class="hero-header">
        <div class="hero-title">🚀 LLVM IR Diff Analyzer Pro</div>
        <div class="hero-subtitle">Professional Compiler Optimization Analysis Platform</div>
        <div class="hero-description">
            Transform your C++ code analysis with AI-powered insights and beautiful visualizations. 
            Analyze LLVM optimization passes with professional-grade reporting and technical insights.
        </div>
    </div>
    """, unsafe_allow_html=True)

def create_feature_cards():
    """Create feature highlight cards"""
    col1, col2, col3 = st.columns(3)
    
    with col1:
        st.markdown("""
        <div class="feature-card">
            <div class="feature-icon">🤖</div>
            <div class="feature-title">AI-Powered Analysis</div>
            <div class="feature-description">
                Advanced AI generates comprehensive reports with technical insights, 
                optimization explanations, and performance impact analysis.
            </div>
        </div>
        """, unsafe_allow_html=True)
    
    with col2:
        st.markdown("""
        <div class="feature-card">
            <div class="feature-icon">⚡</div>
            <div class="feature-title">Lightning Fast Processing</div>
            <div class="feature-description">
                Optimized LLVM integration provides rapid compilation and analysis 
                with real-time feedback and professional reporting.
            </div>
        </div>
        """, unsafe_allow_html=True)
    
    with col3:
        st.markdown("""
        <div class="feature-card">
            <div class="feature-icon">📊</div>
            <div class="feature-title">Professional Reports</div>
            <div class="feature-description">
                Beautiful HTML reports with interactive diffs, detailed metrics, 
                and executive summaries ready for technical presentations.
            </div>
        </div>
        """, unsafe_allow_html=True)

def create_code_input_section():
    """Create an enhanced code input section"""
    st.markdown('<div class="section-header">💻 Code Input & Configuration</div>', unsafe_allow_html=True)
    
    # Enhanced tabs
    tab1, tab2 = st.tabs(["✍️ Code Editor", "📁 File Upload"])
    
    tmp_file_path = None
    
    with tab1:
        st.markdown("### Write or Paste Your C++ Code")
        
        # Quick examples
        col1, col2, col3 = st.columns(3)
        
        with col1:
            if st.button("🔄 Memory Example", use_container_width=True):
                st.session_state.example_code = """
int memory_example(int n) {
    int* ptr = new int(n);
    int result = *ptr + 10;
    delete ptr;
    return result;
}

int main() {
    return memory_example(42);
}"""
        
        with col2:
            if st.button("🔄 Loop Example", use_container_width=True):
                st.session_state.example_code = """
int loop_example(int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += i * 2 + 1;
    }
    return sum;
}

int main() {
    return loop_example(10);
}"""
        
        with col3:
            if st.button("🔄 Optimization Example", use_container_width=True):
                st.session_state.example_code = """
int optimization_example(int x) {
    int temp1 = x + 0;      // Identity
    int temp2 = temp1 * 1;  // Identity  
    int temp3 = temp2 + 0;  // Identity
    if (temp3 > 0) {
        return temp3 * 2;
    } else {
        return temp3 + 5;
    }
}

int main() {
    return optimization_example(15);
}"""
        
        # Get code from session state or default
        default_code = getattr(st.session_state, 'example_code', """
// Example: Function with optimization opportunities
int conditional_example(int n) {
    int x = n + 0;          // Identity operation (will be optimized)
    int y = x * 1;          // Another identity (will be optimized)
    
    if (y > 10) {
        return y * 2;
    } else {
        return y + 5;
    }
}

int main() {
    return conditional_example(15);
}""")
        
        code = st.text_area(
            "C++ Source Code", 
            value=default_code,
            height=350,
            help="Enter valid C++ code that will be compiled to LLVM IR and optimized",
            placeholder="Enter your C++ code here..."
        )
        
        if code:
            with tempfile.NamedTemporaryFile(suffix='.cpp', delete=False, mode='w') as tmp_file:
                tmp_file.write(code)
                tmp_file_path = tmp_file.name
                
            # Show code stats
            lines = len(code.split('\n'))
            chars = len(code)
            st.info(f"📊 Code Stats: {lines} lines, {chars} characters")
    
    with tab2:
        st.markdown("### Upload C++ Source File")
        
        uploaded_file = st.file_uploader(
            "Choose a C++ file", 
            type=['cpp', 'cc', 'cxx', 'c++', 'c'],
            help="Upload a C++ source file for optimization analysis"
        )
        
        if uploaded_file:
            with tempfile.NamedTemporaryFile(suffix='.cpp', delete=False, mode='w') as tmp_file:
                content = uploaded_file.getvalue().decode('utf-8')
                tmp_file.write(content)
                tmp_file_path = tmp_file.name
            
            st.success(f"📁 Uploaded: {uploaded_file.name}")
            
            # Show file preview
            with st.expander("👀 File Preview"):
                st.code(content, language='cpp')
    
    return tmp_file_path

def create_optimization_selector():
    """Create an enhanced optimization pass selector"""
    st.markdown('<div class="section-header">⚙️ Optimization Pass Selection</div>', unsafe_allow_html=True)
    
    # Core passes with descriptions
    col1, col2 = st.columns(2)
    
    with col1:
        st.markdown("### 🎯 Core Optimization Passes")
        
        core_passes = {
            "🧠 Memory to Register (mem2reg)": {
                "value": "mem2reg",
                "description": "Promotes memory allocations to SSA form, eliminating unnecessary loads/stores",
                "use_case": "Best for functions with many local variables"
            },
            "🗑️ Dead Code Elimination (dce)": {
                "value": "dce", 
                "description": "Removes unused instructions and variables that don't affect program output",
                "use_case": "Ideal for cleaning up development artifacts"
            },
            "🔧 Instruction Combining (instcombine)": {
                "value": "instcombine",
                "description": "Combines redundant instructions and simplifies expressions",
                "use_case": "Perfect for arithmetic optimizations"
            },
            "🌊 Control Flow Simplification (simplifycfg)": {
                "value": "simplifycfg",
                "description": "Simplifies control flow graphs and removes redundant branches",
                "use_case": "Great for complex conditional logic"
            }
        }
        
        selected_core = st.radio(
            "Select core optimization:",
            options=list(core_passes.keys()),
            index=0
        )
        
        # Show description
        pass_info = core_passes[selected_core]
        st.info(f"**Description:** {pass_info['description']}\n\n**Use Case:** {pass_info['use_case']}")
        
        pass_string = pass_info['value']
    
    with col2:
        st.markdown("### 🚀 Advanced Optimization Passes")
        
        advanced_passes = {
            "🔄 Loop Unrolling (loop-unroll)": {
                "value": "loop-unroll",
                "description": "Unrolls loops to reduce branch overhead and enable further optimizations",
                "use_case": "Best for small, fixed-iteration loops"
            },
            "📐 Scalar Replacement (sroa)": {
                "value": "sroa",
                "description": "Breaks down aggregates into individual scalar values",
                "use_case": "Excellent for struct/array optimizations"
            },
            "🔀 Global Value Numbering (gvn)": {
                "value": "gvn",
                "description": "Eliminates redundant computations across basic blocks",
                "use_case": "Powerful for mathematical expressions"
            }
        }
        
        use_advanced = st.checkbox("🎯 Enable Advanced Pass", help="Use more sophisticated optimization techniques")
        
        if use_advanced:
            advanced_selected = st.selectbox(
                "Choose advanced pass:",
                options=list(advanced_passes.keys()),
                help="Advanced passes provide deeper optimizations"
            )
            
            pass_info = advanced_passes[advanced_selected]
            st.info(f"**Description:** {pass_info['description']}\n\n**Use Case:** {pass_info['use_case']}")
            pass_string = pass_info['value']
    
    return pass_string

def create_download_link(file_path):
    """Generate an enhanced download link"""
    try:
        with open(file_path, "rb") as f:
            file_data = f.read()
        b64_data = base64.b64encode(file_data).decode()
        
        file_size = len(file_data) / 1024  # KB
        
        return f'''
        <div style="text-align: center; margin: 3rem 0;">
            <a href="data:text/html;base64,{b64_data}" 
               download="llvm_ir_analysis_report.html" 
               class="download-button">
                📥 Download Complete Report ({file_size:.1f} KB)
            </a>
        </div>
        '''
    except Exception as e:
        return f"<p>Error creating download link: {e}</p>"

def create_results_display(report_path, pass_name):
    """Create enhanced results display"""
    st.markdown("""
    <div class="success-container">
        <h2>🎉 Analysis Complete!</h2>
        <p>Your LLVM IR optimization analysis has been generated successfully</p>
    </div>
    """, unsafe_allow_html=True)
    
    # Results summary
    col1, col2, col3 = st.columns(3)
    
    with col1:
        st.markdown("""
        <div class="metric-container">
            <h3>📊 Optimization Pass</h3>
            <h2 style="color: #667eea;">{}</h2>
        </div>
        """.format(pass_name.upper()), unsafe_allow_html=True)
    
    with col2:
        if os.path.exists(report_path):
            file_size = os.path.getsize(report_path) / 1024
            st.markdown("""
            <div class="metric-container">
                <h3>📄 Report Size</h3>
                <h2 style="color: #28a745;">{:.1f} KB</h2>
            </div>
            """.format(file_size), unsafe_allow_html=True)
        else:
            st.markdown("""
            <div class="metric-container">
                <h3>📄 Report</h3>
                <h2 style="color: #dc3545;">Not Found</h2>
            </div>
            """, unsafe_allow_html=True)
    
    with col3:
        st.markdown("""
        <div class="metric-container">
            <h3>⏱️ Status</h3>
            <h2 style="color: #28a745;">Ready ✅</h2>
        </div>
        """, unsafe_allow_html=True)
    
    # Download section
    if os.path.exists(report_path):
        download_link = create_download_link(report_path)
        st.markdown(download_link, unsafe_allow_html=True)
        
        # Report preview
        with st.expander("👀 Report Preview & Features"):
            st.info("**Your professional report includes:**")
            st.markdown("""
            - 🔍 **Side-by-side IR comparison** with syntax highlighting
            - 🤖 **AI-generated analysis** explaining optimizations in detail
            - 📊 **Performance metrics** and comprehensive statistics  
            - 📋 **Executive summary** perfect for technical presentations
            - 🎨 **Professional formatting** with interactive navigation
            - 📈 **Optimization impact** analysis and recommendations
            """)
    else:
        st.error("❌ Report file not found")

def main():
    """Main application function with enhanced UI"""
    # Load custom CSS
    load_custom_css()
    
    # Hero section
    create_hero_section()
    
    # Feature cards
    create_feature_cards()
    
    # Code input section
    tmp_file_path = create_code_input_section()
    
    # Optimization selector
    pass_string = create_optimization_selector()
    
    # Analysis button
    st.markdown('<div class="section-header">🚀 Run Analysis</div>', unsafe_allow_html=True)
    
    col1, col2, col3 = st.columns([1, 2, 1])
    with col2:
        if st.button("🚀 Start Professional Analysis", type="primary", use_container_width=True):
            if not tmp_file_path:
                st.markdown("""
                <div class="error-container">
                    <h2>❌ Input Required</h2>
                    <p>Please provide C++ code before starting analysis</p>
                </div>
                """, unsafe_allow_html=True)
                return
            
            # Progress tracking
            progress_steps = [
                "🔄 Initializing analysis pipeline...",
                "📝 Compiling C++ to LLVM IR...", 
                "⚙️ Applying optimization passes...",
                "🔍 Analyzing IR transformations...",
                "🤖 Generating AI insights...",
                "📊 Creating visual reports...",
                "✅ Finalizing analysis..."
            ]
            
            progress_bar = st.progress(0)
            
            # Progress container
            st.markdown("""
            <div class="progress-container">
                <h3 style="text-align: center; margin-bottom: 2rem;">🔄 Analysis Progress</h3>
            </div>
            """, unsafe_allow_html=True)
            
            status_container = st.empty()
            
            try:
                for i, step in enumerate(progress_steps):
                    with status_container.container():
                        st.markdown(f"""
                        <div class="progress-step">
                            <div class="progress-icon current">⏳</div>
                            <span>{step}</span>
                        </div>
                        """, unsafe_allow_html=True)
                    
                    progress_bar.progress((i + 1) / len(progress_steps))
                    time.sleep(0.8)  # Simulate processing time
                
                # Run the actual analysis
                with st.spinner("🔄 Running LLVM optimization analysis..."):
                    report_path = run_ir_diff(tmp_file_path, pass_string)
                
                progress_bar.progress(1.0)
                status_container.empty()
                
                # Display results (no balloons)
                create_results_display(report_path, pass_string)
                
            except Exception as e:
                st.markdown("""
                <div class="error-container">
                    <h2>❌ Analysis Failed</h2>
                    <p>There was an error during the optimization analysis</p>
                </div>
                """, unsafe_allow_html=True)
                
                st.error(f"Error: {str(e)}")
                
                with st.expander("🔧 Debug Information"):
                    st.text(f"Error: {e}")
                    st.text(f"Pass: {pass_string}")
                    st.text(f"File: {tmp_file_path}")
                    
                    # System info
                    st.text("System Information:")
                    st.text(f"Python: {sys.version}")
                    st.text(f"Platform: {sys.platform}")
                    
                    if tmp_file_path and os.path.exists(tmp_file_path):
                        st.text(f"Input file size: {os.path.getsize(tmp_file_path)} bytes")
                        
            finally:
                # Cleanup
                if tmp_file_path and os.path.exists(tmp_file_path):
                    try:
                        os.unlink(tmp_file_path)
                    except:
                        pass
    
    # Footer
    st.markdown("""
    <div class="footer">
        <h3>🚀 LLVM IR Diff Analyzer Pro</h3>
        <p>Built with ❤️ using Streamlit & LLVM | Professional Compiler Analysis Tool</p>
        <p><small>© 2024 LLVM IR Diff Analyzer Pro - Advanced Optimization Analysis Platform</small></p>
    </div>
    """, unsafe_allow_html=True)

if __name__ == "__main__":
    main()