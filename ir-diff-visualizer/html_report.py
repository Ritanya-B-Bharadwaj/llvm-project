# html_report.py - Fixed version with independent scrolling and better visual diff
import os
from datetime import datetime
from typing import List, Dict, Any

def generate_enhanced_html_report(before_lines: List[str], after_lines: List[str], 
                                diff_result: Dict, ai_summary: str, 
                                source_file: str = None, passes: str = None) -> str:
    """
    Generate a comprehensive HTML report with enhanced difflib analysis and AI summary.
    """
    # Create output filename
    if passes and source_file:
        passes_clean = passes.replace(',', '_').replace(' ', '_')
        source_basename = os.path.splitext(os.path.basename(source_file))[0]
        timestamp = datetime.now().strftime("%H%M%S")
        output_path = os.path.join("output", f"{passes_clean}_{source_basename}_{timestamp}_report.html")
    else:
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        output_path = os.path.join("output", f"ir_diff_report_{timestamp}.html")
    
    # Ensure output directory exists
    os.makedirs("output", exist_ok=True)
    
    # Get metrics from diff result
    metrics = diff_result['metrics']
    
    # Format the AI summary properly
    formatted_ai_summary = ai_summary.replace('\n', '<br>').replace('##', '<h4>').replace('### ', '<h5>')
    
    # Format source file and passes for display
    source_display = os.path.basename(source_file) if source_file else 'IR Analysis'
    passes_display = passes if passes else 'Optimization'
    timestamp_display = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    
    # Join lines for code display
    before_code = '\n'.join(before_lines)
    after_code = '\n'.join(after_lines)
    
    # Generate the comprehensive HTML report
    html_content = f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>LLVM IR Optimization Analysis{' - ' + passes if passes else ''}</title>
    <style>
        /* Professional Styling */
        * {{
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }}
        
        body {{
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #f5f7fa 0%, #c3cfe2 100%);
            min-height: 100vh;
            padding: 20px;
        }}
        
        .container {{
            max-width: 100%;
            margin: 0 auto;
            background: white;
            border-radius: 15px;
            box-shadow: 0 10px 30px rgba(0,0,0,0.1);
            overflow: hidden;
        }}
        
        .header {{
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            padding: 30px;
            text-align: center;
        }}
        
        .header h1 {{
            font-size: 2.5em;
            margin-bottom: 10px;
            font-weight: 300;
        }}
        
        .header .subtitle {{
            font-size: 1.2em;
            opacity: 0.9;
        }}
        
        .content {{
            padding: 30px;
        }}
        
        .section {{
            margin-bottom: 40px;
        }}
        
        .section h2 {{
            color: #2c3e50;
            border-bottom: 3px solid #3498db;
            padding-bottom: 10px;
            margin-bottom: 20px;
            font-size: 1.8em;
        }}
        
        .metrics-grid {{
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
            gap: 20px;
            margin-bottom: 30px;
        }}
        
        .metric-card {{
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            padding: 25px;
            border-radius: 12px;
            text-align: center;
            box-shadow: 0 5px 15px rgba(0,0,0,0.1);
        }}
        
        .metric-value {{
            font-size: 2.5em;
            font-weight: bold;
            margin-bottom: 5px;
        }}
        
        .metric-label {{
            font-size: 1em;
            opacity: 0.9;
        }}
        
        .ai-summary {{
            background: linear-gradient(135deg, #74b9ff 0%, #0984e3 100%);
            color: white;
            padding: 25px;
            border-radius: 12px;
            margin-bottom: 30px;
        }}
        
        .ai-summary h3 {{
            margin-bottom: 15px;
            font-size: 1.5em;
        }}
        
        .performance-impact {{
            padding: 20px;
            border-radius: 8px;
            margin: 20px 0;
            font-weight: bold;
            text-align: center;
        }}
        
        .positive-impact {{
            background: linear-gradient(135deg, #00b894 0%, #00cec9 100%);
            color: white;
        }}
        
        .neutral-impact {{
            background: linear-gradient(135deg, #636e72 0%, #b2bec3 100%);
            color: white;
        }}
        
        .optimization-details {{
            background: #f8f9fa;
            padding: 25px;
            border-radius: 12px;
            border-left: 5px solid #3498db;
        }}
        
        /* ENHANCED VISUAL DIFF ANALYZER STYLING */
        .visual-diff-container {{
            width: 100%;
            background: white;
            border-radius: 12px;
            box-shadow: 0 4px 15px rgba(0,0,0,0.1);
            overflow: hidden;
            border: 1px solid #e1e5e9;
        }}
        
        .diff-controls {{
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            padding: 15px 20px;
            display: flex;
            justify-content: space-between;
            align-items: center;
        }}
        
        .diff-controls h3 {{
            margin: 0;
            font-size: 1.2em;
        }}
        
        .diff-stats {{
            font-size: 0.9em;
            opacity: 0.9;
        }}
        
        .diff-content {{
            width: 100%;
            overflow-x: auto;
            overflow-y: auto;
            max-height: 600px;
            background: #fafbfc;
        }}
        
        /* Enhanced diff table styling */
        table.diff {{
            width: 100%;
            border-collapse: collapse;
            font-family: 'Fira Code', 'Monaco', 'Consolas', monospace;
            font-size: 14px;
            line-height: 1.4;
            background: white;
            min-width: 800px;
        }}
        
        .diff th {{
            background: #f6f8fa;
            color: #24292e;
            padding: 12px 8px;
            text-align: left;
            font-weight: 600;
            border-bottom: 1px solid #e1e4e8;
            position: sticky;
            top: 0;
            z-index: 10;
        }}
        
        .diff td {{
            padding: 2px 8px;
            border-bottom: 1px solid #f0f0f0;
            vertical-align: top;
            white-space: pre;
            font-family: 'Fira Code', 'Monaco', 'Consolas', monospace;
        }}
        
        /* Line number styling */
        .diff_next {{
            background: #f6f8fa !important;
            color: #586069 !important;
            text-align: center;
            font-weight: bold;
            width: 40px;
            user-select: none;
            border-right: 1px solid #e1e4e8;
        }}
        
        /* Diff highlighting */
        .diff_add {{
            background: #e6ffed !important;
            border-left: 3px solid #28a745;
        }}
        
        .diff_sub {{
            background: #ffeef0 !important;
            border-left: 3px solid #d73a49;
        }}
        
        .diff_chg {{
            background: #fff5b4 !important;
            border-left: 3px solid #ffd33d;
        }}
        
        /* LLVM-specific syntax highlighting */
        .llvm-keyword {{
            color: #d73a49;
            font-weight: bold;
        }}
        
        .llvm-type {{
            color: #005cc5;
        }}
        
        .llvm-register {{
            color: #6f42c1;
        }}
        
        .llvm-number {{
            color: #032f62;
        }}
        
        /* CODE COMPARISON SECTION - INDEPENDENT SCROLLING */
        .code-comparison {{
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 20px;
            margin-top: 20px;
        }}
        
        .code-panel {{
            background: #f8f9fa;
            border: 1px solid #dee2e6;
            border-radius: 12px;
            overflow: hidden;
            box-shadow: 0 4px 15px rgba(0,0,0,0.1);
        }}
        
        .code-panel-header {{
            background: linear-gradient(135deg, #343a40 0%, #495057 100%);
            color: white;
            padding: 15px 20px;
            font-weight: bold;
            display: flex;
            justify-content: space-between;
            align-items: center;
            position: sticky;
            top: 0;
            z-index: 5;
        }}
        
        .code-panel-title {{
            font-size: 1.1em;
        }}
        
        .code-line-count {{
            background: rgba(255,255,255,0.2);
            padding: 4px 8px;
            border-radius: 4px;
            font-size: 0.85em;
        }}
        
        .code-panel-content {{
            position: relative;
            overflow: auto;
            max-height: 500px;
            background: #2d3748;
        }}
        
        .code-panel pre {{
            margin: 0;
            padding: 20px;
            color: #e2e8f0;
            font-family: 'Fira Code', 'Monaco', 'Consolas', monospace;
            font-size: 14px;
            line-height: 1.5;
            white-space: pre;
            overflow-x: auto;
            min-width: max-content;
        }}
        
        /* Independent scroll indicators */
        .scroll-indicator {{
            position: absolute;
            top: 50%;
            right: 15px;
            transform: translateY(-50%);
            background: rgba(0,0,0,0.7);
            color: white;
            padding: 6px 12px;
            border-radius: 20px;
            font-size: 11px;
            pointer-events: none;
            opacity: 0.8;
            transition: opacity 0.3s ease;
        }}
        
        /* Custom scrollbars */
        .diff-content::-webkit-scrollbar,
        .code-panel-content::-webkit-scrollbar {{
            height: 10px;
            width: 10px;
        }}
        
        .diff-content::-webkit-scrollbar-track,
        .code-panel-content::-webkit-scrollbar-track {{
            background: #f1f3f4;
            border-radius: 5px;
        }}
        
        .diff-content::-webkit-scrollbar-thumb,
        .code-panel-content::-webkit-scrollbar-thumb {{
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            border-radius: 5px;
        }}
        
        .diff-content::-webkit-scrollbar-thumb:hover,
        .code-panel-content::-webkit-scrollbar-thumb:hover {{
            background: linear-gradient(135deg, #5a6fd8 0%, #6a4190 100%);
        }}
        
        .footer {{
            background: #34495e;
            color: white;
            text-align: center;
            padding: 20px;
            margin-top: 40px;
        }}
        
        /* Responsive design */
        @media (max-width: 1200px) {{
            .code-comparison {{
                grid-template-columns: 1fr;
                gap: 15px;
            }}
            
            .metrics-grid {{
                grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            }}
        }}
        
        @media (max-width: 768px) {{
            .header h1 {{
                font-size: 2em;
            }}
            
            .content {{
                padding: 20px;
            }}
            
            .diff-controls {{
                flex-direction: column;
                gap: 10px;
                text-align: center;
            }}
            
            .code-panel pre {{
                font-size: 12px;
                padding: 15px;
            }}
        }}
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🔧 LLVM IR Optimization Analysis</h1>
            <div class="subtitle">
                Source: {source_display} | 
                Pass: {passes_display} | 
                Generated: {timestamp_display}
            </div>
        </div>
        
        <div class="content">
            <!-- Performance Metrics Section -->
            <div class="section">
                <h2>📊 Performance Metrics</h2>
                <div class="metrics-grid">
                    <div class="metric-card">
                        <div class="metric-value">{metrics.instructions_eliminated:+d}</div>
                        <div class="metric-label">Instructions Changed</div>
                    </div>
                    <div class="metric-card">
                        <div class="metric-value">{metrics.reduction_percentage:.1f}%</div>
                        <div class="metric-label">Code Reduction</div>
                    </div>
                    <div class="metric-card">
                        <div class="metric-value">{metrics.memory_ops_eliminated}</div>
                        <div class="metric-label">Memory Ops Eliminated</div>
                    </div>
                    <div class="metric-card">
                        <div class="metric-value">{metrics.basic_blocks_before - metrics.basic_blocks_after:+d}</div>
                        <div class="metric-label">Basic Blocks Changed</div>
                    </div>
                </div>
                
                <div class="performance-impact {'positive-impact' if 'Positive' in metrics.performance_impact else 'neutral-impact'}">
                    💡 {metrics.performance_impact}
                </div>
            </div>
            
            <!-- AI Summary Section -->
            <div class="section">
                <div class="ai-summary">
                    <h3>🤖 AI Analysis Summary</h3>
                    <div>{formatted_ai_summary}</div>
                </div>
            </div>
            
            <!-- Optimization Details -->
            <div class="section">
                <h2>🔍 Optimization Details</h2>
                <div class="optimization-details">
                    <h3>Transformation: {metrics.optimization_type.upper().replace('-', ' ')}</h3>
                    <p><strong>Before:</strong> {metrics.instructions_before} instructions, {metrics.memory_operations_before} memory operations, {metrics.basic_blocks_before} basic blocks</p>
                    <p><strong>After:</strong> {metrics.instructions_after} instructions, {metrics.memory_operations_after} memory operations, {metrics.basic_blocks_after} basic blocks</p>
                    <p><strong>Summary:</strong> {diff_result['summary']}</p>
                </div>
            </div>
            
            <!-- ENHANCED VISUAL DIFF ANALYZER -->
            <div class="section">
                <h2>🎨 Visual Diff Analysis</h2>
                <div class="visual-diff-container">
                    <div class="diff-controls">
                        <h3>📋 Line-by-Line Comparison</h3>
                        <div class="diff-stats">
                            Lines: {len(before_lines)} → {len(after_lines)} | 
                            Changes: {abs(len(before_lines) - len(after_lines))}
                        </div>
                    </div>
                    <div class="diff-content">
                        {diff_result['html_diff']}
                    </div>
                </div>
            </div>
            
            <!-- INDEPENDENT SCROLLING CODE COMPARISON -->
            <div class="section">
                <h2>📝 Code Comparison</h2>
                <div class="code-comparison">
                    <div class="code-panel" id="before-panel">
                        <div class="code-panel-header">
                            <div class="code-panel-title">📄 Before Optimization</div>
                            <div class="code-line-count">{len(before_lines)} lines</div>
                        </div>
                        <div class="code-panel-content" id="before-content">
                            <div class="scroll-indicator">↔ Scroll independently</div>
                            <pre>{before_code}</pre>
                        </div>
                    </div>
                    
                    <div class="code-panel" id="after-panel">
                        <div class="code-panel-header">
                            <div class="code-panel-title">✨ After Optimization</div>
                            <div class="code-line-count">{len(after_lines)} lines</div>
                        </div>
                        <div class="code-panel-content" id="after-content">
                            <div class="scroll-indicator">↔ Scroll independently</div>
                            <pre>{after_code}</pre>
                        </div>
                    </div>
                </div>
            </div>
        </div>
        
        <div class="footer">
            <p>Generated by LLVM IR Diff Analyzer | Enhanced with Professional Analysis & AI Insights</p>
        </div>
    </div>

    <script>
        document.addEventListener('DOMContentLoaded', function() {{
            // INDEPENDENT SCROLLING - NO SYNCHRONIZATION
            const beforeContent = document.getElementById('before-content');
            const afterContent = document.getElementById('after-content');
            
            // Hide scroll indicators after user interacts
            function hideScrollIndicator(element) {{
                const indicator = element.querySelector('.scroll-indicator');
                if (indicator) {{
                    let hideTimeout;
                    element.addEventListener('scroll', function() {{
                        indicator.style.opacity = '0.3';
                        clearTimeout(hideTimeout);
                        hideTimeout = setTimeout(() => {{
                            indicator.style.opacity = '0';
                        }}, 2000);
                    }}, {{ once: false }});
                }}
            }}
            
            // Apply independent scrolling to both panels
            if (beforeContent) hideScrollIndicator(beforeContent);
            if (afterContent) hideScrollIndicator(afterContent);
            
            // Enhance diff table with LLVM syntax highlighting
            const diffTables = document.querySelectorAll('table.diff');
            diffTables.forEach(table => {{
                const cells = table.querySelectorAll('td');
                cells.forEach(cell => {{
                    let content = cell.textContent;
                    if (content) {{
                        // Highlight LLVM keywords
                        content = content.replace(/\\b(define|declare|alloca|load|store|ret|br|label|phi|icmp|add|sub|mul|div|call)\\b/g, 
                            '<span class="llvm-keyword">$1</span>');
                        
                        // Highlight types
                        content = content.replace(/\\b(i32|i64|i8|ptr|void)\\b/g, 
                            '<span class="llvm-type">$1</span>');
                        
                        // Highlight registers
                        content = content.replace(/%[a-zA-Z0-9_]+/g, 
                            '<span class="llvm-register">$&</span>');
                        
                        // Highlight numbers
                        content = content.replace(/\\b\\d+\\b/g, 
                            '<span class="llvm-number">$&</span>');
                        
                        cell.innerHTML = content;
                    }}
                }});
            }});
            
            // Add copy functionality
            const codeBlocks = document.querySelectorAll('pre');
            codeBlocks.forEach(block => {{
                block.addEventListener('dblclick', function() {{
                    const selection = window.getSelection();
                    const range = document.createRange();
                    range.selectNodeContents(this);
                    selection.removeAllRanges();
                    selection.addRange(range);
                    
                    try {{
                        document.execCommand('copy');
                        
                        // Show feedback
                        const originalBg = this.style.background;
                        this.style.background = '#d4edda';
                        setTimeout(() => {{
                            this.style.background = originalBg;
                        }}, 500);
                    }} catch (err) {{
                        console.log('Copy failed');
                    }}
                }});
            }});
        }});
    </script>
</body>
</html>"""
    
    # Write the HTML report
    with open(output_path, 'w', encoding='utf-8') as f:
        f.write(html_content)
    
    return output_path

# Backward compatibility function
def generate_html_report(before_lines, after_lines, diff_text, summary, source_file=None, passes=None):
    """Backward compatibility wrapper"""
    # Create a basic diff_result structure
    diff_result = {
        'html_diff': f'<pre>{diff_text}</pre>',
        'text_diff': diff_text,
        'summary': summary,
        'metrics': type('obj', (object,), {
            'instructions_eliminated': 0,
            'reduction_percentage': 0.0,
            'memory_ops_eliminated': 0,
            'basic_blocks_before': 0,
            'basic_blocks_after': 0,
            'optimization_type': 'general',
            'performance_impact': 'Analysis not available',
            'instructions_before': len(before_lines),
            'instructions_after': len(after_lines),
            'memory_operations_before': 0,
            'memory_operations_after': 0
        })()
    }
    
    return generate_enhanced_html_report(before_lines, after_lines, diff_result, summary, source_file, passes)
