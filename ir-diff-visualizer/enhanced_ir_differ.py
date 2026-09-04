# enhanced_ir_differ.py
import difflib
import re
from typing import List, Dict, Tuple, Optional
from dataclasses import dataclass
from datetime import datetime

@dataclass
class OptimizationMetrics:
    """Metrics for optimization analysis"""
    instructions_before: int
    instructions_after: int
    instructions_eliminated: int
    reduction_percentage: float
    memory_operations_before: int
    memory_operations_after: int
    memory_ops_eliminated: int
    basic_blocks_before: int
    basic_blocks_after: int
    optimization_type: str
    performance_impact: str

class EnhancedIRDiffer:
    """Enhanced difflib with LLVM IR awareness and professional features"""
    
    def __init__(self):
        self.optimization_signatures = {
            'mem2reg': {
                'removed_patterns': [r'alloca', r'store.*ptr', r'load.*ptr'],
                'added_patterns': [r'phi.*\[', r'icmp'],
                'description': 'Memory-to-Register Promotion',
                'benefit': 'Eliminated stack operations, promoted variables to virtual registers'
            },
            'dce': {
                'removed_patterns': [r'add.*0', r'mul.*1', r'unused\d+', r'dead\d+', r'waste\d+'],
                'added_patterns': [],
                'description': 'Dead Code Elimination',
                'benefit': 'Removed unused computations and variables'
            },
            'instcombine': {
                'removed_patterns': [r'add.*0', r'mul.*1', r'temp\d+.*=.*add.*10', r'identity\d+'],
                'added_patterns': [],
                'description': 'Instruction Combining',
                'benefit': 'Simplified and combined redundant operations'
            },
            'simplifycfg': {
                'removed_patterns': [r'br label %\d+', r'^\d+:\s*$', r'br label %\d+\s*$'],
                'added_patterns': [],
                'description': 'Control Flow Graph Simplification',
                'benefit': 'Removed unnecessary basic blocks and branches'
            },
            'loop-unroll': {
                'removed_patterns': [r'phi.*\[.*loop'],
                'added_patterns': [r'add.*nsw.*i32'],
                'description': 'Loop Unrolling',
                'benefit': 'Reduced loop overhead by replicating loop body'
            }
        }
    
    def generate_enhanced_diff(self, before_lines: List[str], after_lines: List[str], 
                             pass_name: str = "unknown") -> Dict:
        """Generate enhanced diff with LLVM-specific analysis"""
        
        # Calculate comprehensive metrics
        metrics = self._calculate_optimization_metrics(before_lines, after_lines, pass_name)
        
        # Generate HTML diff with custom styling
        html_diff = self._generate_professional_html_diff(before_lines, after_lines, metrics)
        
        # Create text diff for compatibility
        text_diff = self._generate_text_diff(before_lines, after_lines)
        
        # Analyze specific changes
        change_analysis = self._analyze_specific_changes(before_lines, after_lines, pass_name)
        
        return {
            'html_diff': html_diff,
            'text_diff': text_diff,
            'metrics': metrics,
            'change_analysis': change_analysis,
            'summary': self._generate_executive_summary(metrics, change_analysis)
        }
    
    def _calculate_optimization_metrics(self, before_lines: List[str], 
                                      after_lines: List[str], pass_name: str) -> OptimizationMetrics:
        """Calculate detailed optimization metrics"""
        
        # Count instructions (excluding comments, labels, metadata)
        before_instructions = self._count_instructions(before_lines)
        after_instructions = self._count_instructions(after_lines)
        
        # Count memory operations
        before_memory = self._count_memory_operations(before_lines)
        after_memory = self._count_memory_operations(after_lines)
        
        # Count basic blocks
        before_blocks = self._count_basic_blocks(before_lines)
        after_blocks = self._count_basic_blocks(after_lines)
        
        # Calculate metrics
        eliminated = before_instructions - after_instructions
        reduction_pct = (eliminated / before_instructions * 100) if before_instructions > 0 else 0
        memory_eliminated = before_memory - after_memory
        
        # Determine optimization type and performance impact
        opt_type = self._detect_optimization_type(before_lines, after_lines, pass_name)
        performance_impact = self._assess_performance_impact(eliminated, memory_eliminated, 
                                                           before_blocks, after_blocks)
        
        return OptimizationMetrics(
            instructions_before=before_instructions,
            instructions_after=after_instructions,
            instructions_eliminated=eliminated,
            reduction_percentage=reduction_pct,
            memory_operations_before=before_memory,
            memory_operations_after=after_memory,
            memory_ops_eliminated=memory_eliminated,
            basic_blocks_before=before_blocks,
            basic_blocks_after=after_blocks,
            optimization_type=opt_type,
            performance_impact=performance_impact
        )
    
    def _count_instructions(self, lines: List[str]) -> int:
        """Count actual LLVM IR instructions"""
        count = 0
        for line in lines:
            line = line.strip()
            # Skip comments, metadata, empty lines, target info
            if (line and 
                not line.startswith(';') and 
                not line.startswith('!') and
                not line.startswith('target') and
                not line.startswith('source_filename') and
                not line.startswith('attributes') and
                not re.match(r'^\d+:\s*$', line) and  # Empty labels
                ('=' in line or line.startswith('br ') or line.startswith('ret '))):
                count += 1
        return count
    
    def _count_memory_operations(self, lines: List[str]) -> int:
        """Count memory-related operations"""
        count = 0
        for line in lines:
            if re.search(r'\b(alloca|load|store)\b', line):
                count += 1
        return count
    
    def _count_basic_blocks(self, lines: List[str]) -> int:
        """Count basic blocks"""
        count = 0
        for line in lines:
            # Basic block labels: "number:" or "%name:"
            if re.match(r'^(\d+|%\w+):\s*$', line.strip()):
                count += 1
        return count
    
    def _detect_optimization_type(self, before_lines: List[str], 
                                 after_lines: List[str], pass_name: str) -> str:
        """Detect the type of optimization that occurred"""
        
        if pass_name in self.optimization_signatures:
            return pass_name
        
        before_text = '\n'.join(before_lines)
        after_text = '\n'.join(after_lines)
        
        # Check for mem2reg patterns
        if ('alloca' in before_text and 'alloca' not in after_text and 
            'phi' in after_text and 'phi' not in before_text):
            return 'mem2reg'
        
        # Check for dead code elimination
        elif (len(before_lines) > len(after_lines) and 
              any(re.search(r'(unused|dead|waste)\d+', line) for line in before_lines)):
            return 'dce'
        
        # Check for instruction combining
        elif any(re.search(r'add.*0|mul.*1', line) for line in before_lines):
            return 'instcombine'
        
        # Check for CFG simplification
        elif before_text.count('br label') > after_text.count('br label'):
            return 'simplifycfg'
        
        # Check for loop unrolling (more instructions, fewer branches)
        elif (len(after_lines) > len(before_lines) and 
              before_text.count('br i1') > after_text.count('br i1')):
            return 'loop-unroll'
        
        return 'general'
    
    def _assess_performance_impact(self, eliminated_instructions: int, 
                                 eliminated_memory_ops: int,
                                 before_blocks: int, after_blocks: int) -> str:
        """Assess the performance impact of optimizations"""
        
        if eliminated_memory_ops > 0:
            return "High Positive Impact - Memory operations eliminated"
        elif eliminated_instructions > 5:
            return "Medium Positive Impact - Significant instruction reduction"
        elif eliminated_instructions > 0:
            return "Low Positive Impact - Some instructions eliminated"
        elif eliminated_instructions < 0 and before_blocks > after_blocks:
            return "Code Size Increase - Potential performance gain from unrolling"
        elif eliminated_instructions == 0:
            return "Neutral Impact - Structural improvements"
        else:
            return "Mixed Impact - Analysis required"
    
    def _generate_professional_html_diff(self, before_lines: List[str], 
                                       after_lines: List[str], 
                                       metrics: OptimizationMetrics) -> str:
        """Generate professional HTML diff with custom styling and metrics"""
        
        # Create HTML differ with custom settings
        html_differ = difflib.HtmlDiff(wrapcolumn=100, tabsize=2)
        
        # Generate base HTML diff
        html_diff = html_differ.make_file(
            before_lines, 
            after_lines,
            fromdesc=f'Before {metrics.optimization_type.upper()} Optimization',
            todesc=f'After {metrics.optimization_type.upper()} Optimization',
            context=True,
            numlines=3
        )
        
        # Add our enhanced styling and metrics
        enhanced_html = self._add_professional_styling(html_diff, metrics)
        
        return enhanced_html
    
    def _add_professional_styling(self, html_diff: str, metrics: OptimizationMetrics) -> str:
        """Add professional CSS styling and metrics dashboard"""
        
        # Professional CSS styling
        professional_css = """
        <style>
        /* Professional Styling for LLVM IR Diff */
        body { 
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; 
            background: #f8f9fa; 
            margin: 0; 
            padding: 20px; 
        }
        
        .metrics-dashboard {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            padding: 25px;
            border-radius: 12px;
            margin-bottom: 25px;
            box-shadow: 0 4px 15px rgba(0,0,0,0.1);
        }
        
        .metrics-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 20px;
            margin-top: 20px;
        }
        
        .metric-card {
            background: rgba(255,255,255,0.1);
            backdrop-filter: blur(10px);
            padding: 15px;
            border-radius: 8px;
            text-align: center;
        }
        
        .metric-value {
            font-size: 2em;
            font-weight: bold;
            margin-bottom: 5px;
        }
        
        .metric-label {
            font-size: 0.9em;
            opacity: 0.9;
        }
        
        .optimization-info {
            background: white;
            padding: 20px;
            border-radius: 8px;
            margin-bottom: 20px;
            border-left: 5px solid #007bff;
            box-shadow: 0 2px 10px rgba(0,0,0,0.05);
        }
        
        /* Enhanced diff styling */
        table.diff {
            font-family: 'Fira Code', 'Monaco', 'Consolas', monospace;
            border: none;
            border-radius: 8px;
            overflow: hidden;
            box-shadow: 0 4px 15px rgba(0,0,0,0.1);
            background: white;
        }
        
        .diff_header {
            background: #343a40 !important;
            color: white !important;
            font-weight: bold;
            padding: 12px !important;
        }
        
        .diff_next {
            background: #6c757d !important;
            color: white !important;
            text-align: center;
            font-weight: bold;
        }
        
        .diff_add {
            background: #d4edda !important;
            border-left: 4px solid #28a745 !important;
        }
        
        .diff_chg {
            background: #fff3cd !important;
            border-left: 4px solid #ffc107 !important;
        }
        
        .diff_sub {
            background: #f8d7da !important;
            border-left: 4px solid #dc3545 !important;
        }
        
        /* LLVM-specific highlighting */
        td:contains("alloca") { font-weight: bold; }
        td:contains("phi") { font-weight: bold; color: #007bff; }
        td:contains("load") { background-color: rgba(255, 193, 7, 0.1); }
        td:contains("store") { background-color: rgba(255, 193, 7, 0.1); }
        
        .performance-impact {
            margin-top: 15px;
            padding: 15px;
            border-radius: 8px;
            font-weight: bold;
        }
        
        .positive-impact { background: #d4edda; color: #155724; border-left: 4px solid #28a745; }
        .neutral-impact { background: #e2e3e5; color: #383d41; border-left: 4px solid #6c757d; }
        .negative-impact { background: #f8d7da; color: #721c24; border-left: 4px solid #dc3545; }
        </style>
        """
        
        # Metrics dashboard HTML
        metrics_dashboard = f"""
        <div class="metrics-dashboard">
            <h1>🔧 {metrics.optimization_type.upper().replace('-', ' ')} Optimization Analysis</h1>
            <p>{self.optimization_signatures.get(metrics.optimization_type, {}).get('description', 'Code Optimization')}</p>
            
            <div class="metrics-grid">
                <div class="metric-card">
                    <div class="metric-value">{metrics.instructions_eliminated:+d}</div>
                    <div class="metric-label">Instructions Changed</div>
                </div>
                <div class="metric-card">
                    <div class="metric-value">{metrics.reduction_percentage:.1f}%</div>
                    <div class="metric-label">Reduction Percentage</div>
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
        </div>
        
        <div class="optimization-info">
            <h3>📊 Optimization Details</h3>
            <p><strong>Before:</strong> {metrics.instructions_before} instructions, {metrics.memory_operations_before} memory operations</p>
            <p><strong>After:</strong> {metrics.instructions_after} instructions, {metrics.memory_operations_after} memory operations</p>
            <p><strong>Benefit:</strong> {self.optimization_signatures.get(metrics.optimization_type, {}).get('benefit', 'Code improvement achieved')}</p>
            
            <div class="performance-impact {self._get_impact_class(metrics.performance_impact)}">
                💡 {metrics.performance_impact}
            </div>
        </div>
        """
        
        # Insert our enhancements
        enhanced_html = html_diff.replace('<style', professional_css + '<style')
        enhanced_html = enhanced_html.replace('<body>', f'<body>{metrics_dashboard}')
        
        return enhanced_html
    
    def _get_impact_class(self, impact: str) -> str:
        """Get CSS class based on performance impact"""
        if 'Positive' in impact:
            return 'positive-impact'
        elif 'Neutral' in impact or 'Mixed' in impact:
            return 'neutral-impact'
        else:
            return 'negative-impact'
    
    def _generate_text_diff(self, before_lines: List[str], after_lines: List[str]) -> str:
        """Generate simple text diff for compatibility"""
        diff = difflib.unified_diff(
            before_lines, 
            after_lines,
            fromfile='before.ll',
            tofile='after.ll',
            lineterm=''
        )
        return '\n'.join(diff)
    
    def _analyze_specific_changes(self, before_lines: List[str], 
                                after_lines: List[str], pass_name: str) -> Dict:
        """Analyze specific types of changes that occurred"""
        
        changes = {
            'eliminated_patterns': [],
            'added_patterns': [],
            'transformed_constructs': [],
            'key_improvements': []
        }
        
        before_text = '\n'.join(before_lines)
        after_text = '\n'.join(after_lines)
        
        if pass_name in self.optimization_signatures:
            sig = self.optimization_signatures[pass_name]
            
            # Check for eliminated patterns
            for pattern in sig['removed_patterns']:
                if re.search(pattern, before_text) and not re.search(pattern, after_text):
                    changes['eliminated_patterns'].append(pattern)
            
            # Check for added patterns
            for pattern in sig['added_patterns']:
                if re.search(pattern, after_text) and not re.search(pattern, before_text):
                    changes['added_patterns'].append(pattern)
        
        # Detect specific transformations
        if 'alloca' in before_text and 'phi' in after_text:
            changes['transformed_constructs'].append('Stack variables → Virtual registers')
        
        if re.search(r'add.*0|mul.*1', before_text):
            changes['key_improvements'].append('Identity operations eliminated')
        
        if before_text.count('br label') > after_text.count('br label'):
            changes['key_improvements'].append('Unnecessary branches removed')
        
        return changes
    
    def _generate_executive_summary(self, metrics: OptimizationMetrics, 
                                  changes: Dict) -> str:
        """Generate executive summary for management/presentation"""
        
        summary = f"""
🎯 OPTIMIZATION SUMMARY: {metrics.optimization_type.upper()}

📈 PERFORMANCE METRICS:
   • Instructions: {metrics.instructions_before} → {metrics.instructions_after} ({metrics.instructions_eliminated:+d})
   • Memory Operations: {metrics.memory_operations_before} → {metrics.memory_operations_after} ({metrics.memory_ops_eliminated:+d})
   • Code Reduction: {metrics.reduction_percentage:.1f}%

🔧 KEY IMPROVEMENTS:
   • {metrics.performance_impact}
   • {self.optimization_signatures.get(metrics.optimization_type, {}).get('benefit', 'Code optimization achieved')}

💡 IMPACT: {metrics.performance_impact}
        """.strip()
        
        return summary